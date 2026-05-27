function meu:define_meu_infos( )
	return { 	author = "Mâa ",
				tags = { "device", "input", "output", "unfinished", "tutorial" },
				help =	{	"Example of MEU reading information from an arduino device.",
							"was done to read Christin Amoretti Analog sound analyse"
						}
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local ui = self.ui

	local SY= 1
	local ix, iy = 1,1
--	bu = self:add_slider(	{ix+1,iy,	7,1}, 	"speed",	self, "speed" )
--	self:add_trig_restart(	{ix,iy+1}		)

self:set_tab_key( "Main" )
	ix, iy = 1,1
	ui.bu_line			=	self:add_text_info(	{ix,iy,	8,1}, "Line" )
	iy = iy + 1

	local S = 16./10.
	local SS = S * .8
	local DS = (S - SS) / 2
	bu = self:add_button( {ix+DS,iy+DS, SS,SS },	"Force", self, "b_force", false ):set_text_visible( false )

	local t = {}
	local tf= {}
	for i = 1,9 do
		local x = ix+i*S
		local fn = i==9 and BUTTON.set_check_disk or BUTTON.set_check_mul
		bu = self:add_button( {x,iy, S,S },	"Force ON"..i ):set_text_visible( false )
			fn( bu, true )
			tf[i] = bu
		bu = self:add_button( {x,iy+S, S,S },	"ON"..i ):set_text_visible( false )
			fn( bu, true )
			t[i] = bu
	end
	ui.bu_on = t
	ui.bu_force = tf
	iy = iy + S
	bu = self:add_selector(	{ix,iy,	S,S}, "Direction" )
		bu:set_nb( 1, 3 )
		bu:set_item_text( 1, "Up", "No", "Down" )
		bu:set_text_visible( false )
		bu:set_target( self, "s_dir" )
		ui.bu_direction = bu

self:set_tab_key( "Com" )
	ix, iy = 1,1
	iy = iy + SY * 1.5
	bu = self:add_selector(	{ix,iy,	8,SY*1.5}, "Serial port nb" )
		bu:set_target_param( param.get_ref( ref.bdd, "com_port_nb" ) )
		bu:set_nb(16,2)
		for i=1,32,4 do
			bu:set_item_text( i, i )
		end
	iy = iy + SY * 1.5
	self:add_button( {ix, iy, SY,SY },	"Open", ref.bdd, "open_asked",	false ):set_text_visible(false)
	ui.bu_open_info	=	self:add_text_info(	{ix+1,iy,	3,SY}, "Closed" ):set_text_color( "problem")

	iy = iy + 2
	self:add_button( {ix, iy, SY*2,SY },	"Osc",	self, "b_osc_in",		false )
	ui.bu_osc_info	=	self:add_text_info(	{ix+SY*2,iy,	3,SY}, "Osc_info" )

	bus_cur:set_method( "draw_after",	self,	"draw_ui_after"	)
end

function meu:init()
	local ref = self.ref

	ref.bdd = self:get_layer_bdd(1)
	ref.received			= param.get_ref( ref.bdd, "received" )
	ref.open_state			= param.get_ref( ref.bdd, "open_state" )
	ref.line_received_nb	= param.get_ref( ref.bdd, "line_received_nb" )
	ref.line_received = {}
	for i=1,8 do
		ref.line_received[i] = param.get_ref( ref.bdd, "line_received_"..i )
	end

	self.nb_frame = 128
	self.event_nb = 9
	self.v_last = array.new( self.event_nb, 0 )
	self.bufs = {}
end

function meu:draw_event( i_begin, i_end, b_up, ox, oy, sy )
	local nb = math.min( #self.bufs, self.nb_frame )
	if nb < 0  then return end

	ox = (ox or 0) - .5
	local f = b_up and -1. or .6
	sy = sy or 1
	f = sy *f / nb
	oy = oy or 0

	local by = (b_up and .33 or .22) + oy
	local function draw( i, j1, j2 )
		local x = (i+.5) / 10.  + ox
		--self:print( j1.." -- "..j2 )
		aaa.draw_line( x, by - j1 * f, x, by - j2 * f )
	end

	gol.set_texture_dim( 0 )
--	gol.push_matrix()
		gol.color_white()
		--gol.set_line_width( 16 * ( 1-j/nb) )
		--self:print( #self.bufs )

		for i=i_begin,i_end do
			local t
			local jb=1
			local je
			while true do
				while jb <= nb do
					t = self.bufs[jb]
					if t[i]==1 then break end
					jb = jb + 1
				end
				if jb > nb then
					break
				end
				je = jb + 1
				while je <= nb do
					t = self.bufs[je]
					if t[i]==0 then break end
					je = je + 1
				end
				draw( i, jb, je )
				jb = je
			end
		end
--	gol.pop_matrix()
end

function meu:draw_ui_after()
	aaa.show( "draw", self )
	if self.s_dir~=2 then
		gol.set_line_width( 4 )
		self:draw_event( 1, 9, self.s_dir==1 )
	end
end
function meu:draw_icon()
	gol.set_line_width( 2 )
--	if self.s_dir==1 then
--		self:draw_event( 1, 9, false, 0, .3, -4. )
--	else
		self:draw_event( 1, 9, false, 0, -.75, 4. )
--	end
end

function meu:update_ui()
	local ui = self.ui
	local ref = self.ref

	--self:print( param.get_bool( ref.open_state ) )
	ui.bu_open_info:set_visible( not param.get_bool( ref.open_state ) )

--	ui.bu_line:set_text( param.get( line_ref[i_line] ) )
	self:draw_ui_after()
end

function meu:__add_state( t )
	table.insert( self.bufs, 1, t )
	if #self.bufs > self.nb_frame then
		table.remove( self.bufs )
	end
end

function meu:draw()
	local ref = self.ref
	local ui = self.ui

	MEU.draw( self )

	--self:print( param.get( ref.received ) )

	local t = { }
	if self.b_force then
		local bu_force = ui.bu_force
		for i=1,9 do
			t[i] = bu_force[i]:get_value_as_bool()
		end
	else
		if self.b_osc_in then
			array.set( t, false, self.event_nb )
			--OSC
			repeat
				--self:print( "receive_osc()" )
				local tab = aaa.net.osc_take_by_start( "/aaa/anal/band" )
				if tab then
					--table.print( tab, "tags args", 3 )
					for i, v in ipairs(tab) do
						--table.print( v.args, "args", 3 )
						--app:print_osc_tags_args( v.tags, v.args )
						local args = v.args
						t[args[1]] = args[2]==1 and true or false
						--self:print( args[1].." = "..args[2] )
					end
				else
					break
				end
			until true
			--table.print( t, "t" )
		else
			--ARDUINO
			local nb = param.get( ref.line_received_nb )
			if nb > 0 then
				local line_ref = ref.line_received
				for i_line=1,nb do
					local str = param.get( line_ref[i_line] )
					if string.len( str ) == 17 then
						--self:print( "line "..i_line.." : "..str )
						for i=1,9 do
							local i_sub = 1 + (i-1)* 2
							local c = string.sub( str, i_sub, i_sub )
							t[i] = t[i] or c=='1'
							--self:print( i.." -- "..c.." "..t[i] )
						end
					end
				end
			end
		end
	end

	local bu_on = ui.bu_on
	for i=1,self.event_nb do
		bu_on[i]:set_value( t[i] and 1 or 0 )
		local v = t[i] and 1 or 0
		t[i] = v
--		if v < .9 then
--			v = interpolate( (self.v_last[i] or 0.), v, .2 )
--		end
		self.v_last[i] = v
		aaa.midi.set_control( 4, i, v )
	end
	self:__add_state(t)
end
