function meu:define_meu_infos( )
	return	{ author = "Mâa",
				help= { "First shot at a Step Sequencer" },
				tags = { "core", "output", "generator", "VJ", "Sound", "procedural", "experimental", "unfinished" }
			}
end

function meu:get_preset_nb()
	return 24
end

local function __draw_seq(bu)
	SELECTOR.draw(bu)
	local t = bu.__seq_hack
	local line = t.line
	local m =  t.meu
	--gol.set_default()
	if line.b_active then
		local x =  -.5 + m.phase
		gol.color_green( .8 )
		gol.set_line_width(6)
		aaa.draw_line( x,-.7, x,.7 )
	end

	local id_sel = m.ui_id_sel
	--m:print( id_sel )
	if id_sel == line.ui_id then
		gol.set_line_width(4)
		aaa.draw_rect_line( -.5,-.5,	.5,.5	)
	end
end


local function __set_bu_cur(bu)
	local t =  bu.__seq_hack
	--table.print( t, "seq_hack", 2 )
	local meu = t.meu
	meu.ui.bu_ui:set_value( t.line.ui_id ) 
	--aaa.print_fn()
	bu:get_class().set_bu_cur(bu)
end

local track_group = { "A", "B", "C", "D" }
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local ix,iy = 1,1
	local layer = self:get_layer(1)
	local SY = 1.
	local DY = .2

	bu = self:add_slider(					{ix,iy,		4,SY},	"BPM",			self, "bpm", 			60,	1,256	)
	bu = self:add_midi_button_channel(		{ix+8,iy,	2,SY},	"Channel"		)
		bu:set_target( self, "ch" )
		ui.bu_ch = bu

	 bu = self:add_midi_button_controller(	{ix+10,iy,	1,SY},	"Controller"	)
		bu:set_target( self, "ctl" )
		ui.bu_ctl = bu

	iy = iy + SY + DY
	bu = self:add_button(					{ix,iy,		1,SY},	"ui_id"			)
		:set_text_inside( true )
		bu:set_menu( track_group )
		bu:set_text_selector( true )
		ui.bu_ui = bu

	local SX_PAT = 15
	local SYS = SY*3
	local SXS = SX_PAT/16
	
	local YS = iy
	iy = iy + SYS + DY

	local IX = 1
	for i=1,self.line_nb do
		local line = self.lines[i]
		line.letter = track_group[i]
		line.ui_id = i
		line.values = {}
		
		self:begin_bu_group( line.letter )
		for j=1,16 do
			
			bu = self:add_slider(   {IX+1+1*(j-1)*SXS,YS,	SXS,SYS},  "value_"..j, line.values, j, 1, 0,1 )
				:set_text( line.letter..j )
				--:set_text_visible( false )
		end
		self:end_bu_group()

		bu = self:add_button(  		{IX,iy,		1,SY},  "Active"..i ):set_value(true)
			:set_target_lua( line, "b_active" )
			:set_text( line.letter )
			:set_text_inside( true )
			bu.__seq_hack = { meu=self, line=line }
			bu.set_bu_cur = __set_bu_cur

		bu = self:add_selector(		{IX+1,iy,	SX_PAT,SY},  "Seq"..i )
			:set_nb(16)
			:set_text_visible( false )
			:set_selection_multiple( true )
			bu.__seq_hack = { meu=self, line=line }
			bu.draw = __draw_seq
			bu.set_bu_cur = __set_bu_cur
			line.bu_seq = bu
		iy = iy + SY
	end

	bus_cur:set_method( "draw_after",	self,	"draw_ui_after"	)
end

function meu:init()
	local lines = {}
	self.line_nb = 4
	for i=1,self.line_nb do
		local line = {}
		line.scope = SCOPE:create( i )
		lines[i] = line
	end
	self.lines = lines
	self.phase = 0
end

function meu:draw_curves( l,b, r,t )
	--if true then return end

	local sx = r-l
	local sy = t-b
		
	gol.push_matrix()
		gol.translate( l, b )
		gol.scale( r-l, t-b )

		gol.set_texture_dim( 0 )

	-- GRID 
		gol.color_cyan( .5 )
		gol.set_line_width( 1 )
		for y=0,1,1/4		do	aaa.draw_line( 0,y, 1,y )	end
		local dx = 1/16
		for x=0+dx,1-dx,dx	do	aaa.draw_line( x,0, x,1 )	end

	-- CURVES
		gol.set_line_width( 2 )
		--for x=,4,1 do
		--	aaa.draw_line( x,0, x,1 )
		--end
		--aaa.draw_rect_line( 0,0, 1,1 )

		local lines = self.lines
		gol.color_red( )
		lines[1].scope:draw()
		gol.color_green( )
		lines[2].scope:draw()
		gol.color_blue( )
		lines[3].scope:draw()
		gol.color_white( )
		lines[4].scope:draw()

	gol.pop_matrix()
end

function meu:draw_ui_after()
	self:draw_curves( -.5,-.4, .5,-.2 )
end

function meu:update_ui()
	local ui = self.ui
	ui.bu_ch:set_text( "Ch "..ui.bu_ch:get_value() )

--	
--	local scopes = self.scopes
	-- gol.set_default()
	-- for i=1,self.line_nb do
	-- 	local line = self.lines[i]
	-- 	local r = line.rect
	-- end

	local sel = ui.bu_ui:get_value()
	for i=1,self.line_nb do
		local line = self.lines[i]
		self:set_bu_group_active( line.letter, i == sel )
	end
	self.ui_id_sel = sel
	--self:print( self.ui_id_sel )

end

function meu:update()
	local ui = self.ui

	local t = math.fmod( aaa.time.t / 60 / 4 * self.bpm, 1. )
	self.phase = t
	t = math.floor(t * 16) + 1
	local t_bu = ui.bu_seq
	for i=1,self.line_nb do
		local line = self.lines[i]
		local v = line.b_active and (line.bu_seq:is_item_selected( t ) and line.values[t] or 0) or 0

		aaa.midi.set_control( self.ch, self.ctl+i-1 , v )
		line.scope:add( v )
	end
end


-- function meu:draw()
-- end
