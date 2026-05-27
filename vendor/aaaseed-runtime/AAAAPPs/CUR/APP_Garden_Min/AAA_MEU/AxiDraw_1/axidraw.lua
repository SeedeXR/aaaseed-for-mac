 
function meu:define_ui()
	local plot = self.plot
	local ref = plot.ref
	local ui = self.ui
	local bu
	local ix = 1
	local iy = 1
	local SY = 1
	local DY = .5
	local SYM = SY * .8

	ui.bu_link_info			=	self:add_text_info(	{ix,iy,	8,SYM}, "Output" )

	iy = iy + SY * 1.5
	bu = self:add_selector(	{ix,iy,	8,SY*1.5}, "Serial port nb" ):set_target_param( ref.port_nb ):set_nb(16,2)
		for i=1,32,4 do
			bu:set_item_text( i, i )
		end
	iy = iy + SY * 1.5
	bu = self:add_button(	{ix, 		iy,		SY,SY},	"Link",			self, "b_link", false )
		--self.__ba = bu:__get_balue()
		--self.__ba.b_dbg_value = true
		bu:set_text_visible(false)
		bu:set_target_param( plot.ref.open_asked )
		--self.__ba:print( self.__ba:get_value() )	
	bu = self:add_text_info({ix+1,		iy,		3,SYM},	"Closed" ):set_text_color( "problem" )
		ui.bu_link_open_info	=	bu

--	self:add_slider_two(	{ix,iy+1,	8,1}, "Temperature", self, "temp_min", "temp_max", 0, 700, 0, 700 ):set_value_type_integer(true)
	 
	local SX = 8/4
	iy = iy + SY*1.5
	ui.bu_queue	=	self:add_text_info(	{ix+2,iy-SY,	6,SY*.7}, "Queue.size" )
	iy = iy + SY*1.3
	self:add_trig_method(	{ix,		iy,		SX,SY},	"Begin",		plot, "set_begin"				)
	self:add_trig_method(	{ix+SX,		iy,		SX,SY},	"Start",		plot, "set_printing", true		):set_color_back("green")
	self:add_trig_method(	{ix+SX*2,	iy,		SX,SY},	"Stop",			plot, "set_printing", false		):set_color_back("red")
	self:add_trig_method(	{ix+SX*3,	iy,		SX,SY},	"Clear",		plot, "clear"					):set_color_back("red")
	iy = iy + SY
	self:add_slider_two(	{ix,		iy,		5,SY},	"Range",		plot, "range_begin", "range_end",	0, 1, 0, 1 )

	iy = iy + SY * 1.5
	SY = .8
	self:add_trig_method(	{ix,		iy,		4,SY},	"QueryMotor",	plot, "query_motor"				)
	self:add_trig_method(	{ix+4,		iy,		4,SY},	"QueryButton",	plot, "query_button"			)

	iy = iy + SY
	local sx = 8 / 3	
	self:add_trig_method(	{ix,		iy,		sx,SY},	"QueryStep",	plot, "query_step"				)
	self:add_trig_method(	{ix+sx,		iy,		sx,SY},	"ClearStep",	plot, "clear_step"				)		
	self:add_trig_method(	{ix+sx*2,	iy,		sx,SY},	"SyncStep",		plot, "sync_step"				)
	self:add_trig_method(	{ix+4,		iy+1,	4,SY},	"ResetPos",		plot, "reset_pos"				)
	self:add_trig_method(	{ix+1,		iy+1,	3,SY},	"Dump",			plot, "dump"					)

	iy = iy + SY*3	
	self:add_trig_method(	{ix,		iy,		sx,SY},	"QueryPower",	plot, "add_axidraw", "QR"		)
	self:add_trig_method(	{ix+sx,		iy,		sx,SY},	"Version",		plot, "add_axidraw", 	"V"		)
	self:add_trig_method(	{ix+sx*2,	iy,		sx,SY},	"Query General",plot, "query_general"			)
	iy = iy + SY*1.5	
	self:add_trig_method(	{ix,		iy,		4,SY},	"Reset",		plot, "add_axidraw", "R"		):set_color_back("red")
	self:add_trig_method(	{ix+4,		iy,		4,SY},	"Reboot",		plot, "add_axidraw", "RB"		):set_color_back("red")
	iy = iy + SY
	self:add_trig_method(	{ix,		iy,		4,SY},	"Motor No",		plot, "stop_motors"				):set_color_back("red")


	ix = 9	
	iy = 1
	SY = 1
	self:add_trig_method(	{ix,		iy,		4,SY},	"Pen.Up",		plot, "send_pen_up", 10			) --:set_text( "Up" )
	self:add_trig_method(	{ix+4,		iy,		4,SY},	"Pen.Toggle",	plot, "add_axidraw", "TP"	) --:set_text( "Toggle" )
	iy = iy + SY	
	self:add_trig_method(	{ix,		iy,		4,SY},	"Pen.Down",		plot, "send_pen_down", 10		) --:set_text( "Down" )
	self:add_trig_method(	{ix+5,		iy,		3,SY},	"Pen.Query",	plot, "query_pen"			) --:set_text( "Query" )

	iy = iy + SY	
	self:add_slider_two(	{ix,		iy,		5,SY},	"Pen.Position", plot, "pen_min", "pen_max"  ):set_text( "Position" )
	self:add_trig_method(	{ix+5,		iy,		3,SY},	"Pen.Set",		plot, "do_set_pen_pos_min_max"	):set_text( "Set" )
	
	local SX = 8 / 4
	iy = iy + SY * 1
	ui.bu_x	=	self:add_text_info(	{ix,iy,		4,SY},	"X"				):set_text_justification( "right" )
	ui.bu_y	=	self:add_text_info(	{ix+4,iy,	4,SY},	"Y"				):set_text_justification( "left" )	
	iy = iy + SY
	self:add_trig_method(	{ix,		iy,		SX,SY},	"LUp",			self, "move_step_ui",	-1,1 ):set_text( "\\" )
	self:add_trig_method(	{ix+SX,		iy,		SX,SY},	"Up",			self, "move_step_ui",	0,1 )
	self:add_trig_method(	{ix+SX*2,	iy,		SX,SY},	"RUp",			self, "move_step_ui",	1,1 ):set_text( "/" )
	self:add_trig_method(	{ix+SX*3,	iy,		SX,SY},	"Test",			self, "move_test"	)
	iy = iy + SY		
	self:add_trig_method(	{ix,		iy,		SX,SY},	"Left",			self, "move_step_ui",	-1,0 )
	self:add_trig_method(	{ix+SX*2,	iy,		SX,SY},	"Right",		self, "move_step_ui",	1,0 )
	iy = iy + SY		
	self:add_trig_method(	{ix,		iy,		SX,SY},	"LDown",		self, "move_step_ui",	-1,-1 ):set_text( "/" )
	self:add_trig_method(	{ix+SX,		iy,		SX,SY},	"Down",			self, "move_step_ui",	0,-1 )
	self:add_trig_method(	{ix+SX*2,	iy,		SX,SY},	"RDown",		self, "move_step_ui",	1,-1 ):set_text( "\\" )
	iy = iy + SY		
	bu = self:add_selector(	{ix,		iy,		4,SY}, "Clipping" )
		:set_target_lua( self, "s_clip" )
		:set_nb(2)
		bu:set_item_text( 1, "Line", "Segment" )
	SX = 2
	self:add_trig_method(	{ix+4,iy+SY*SY*2,	SX,SY},	"Split .6",	plot, "insert_pause", .6	)
	for i=1,3 do
		self:add_trig_method({ix+6,iy+SY*(i-1),	SX,SY},	"Split "..i,	plot, "insert_pause_nb", i	)
	end
	
	iy = iy + SY
	SX = 8 / 3
	self:add_trig_method(	{ix,		iy,		SX,SY},	"Add",			self, "add_to_plot", false	):set_color_back("restart")
	self:add_trig_method(	{ix+SX,		iy,		SX,SY},	"Add Order",	self, "add_to_plot", true	):set_color_back("restart")
	iy = iy + SY

	iy = iy + SY*1.5
	bu = self:add_slider_xy({ix,		iy,		4,3},	"Pos"			)
		ui.bu_pos = bu
	self:add_trig_method(	{ix,		iy-1,	2,SY},	"Go",			self, "do_go_xy_cano"	):set_color_back("restart")
	bu = self:add_slider_xy({ix+5,		iy,		3,3},	"Offset"		)
		ui.bu_offset = bu
	
end
function meu:init()
	local bdd = self:get_layer_bdd(1)
	self.plot = PLOT:create( self:get_name() )
	self.plot:set_border_xy_mm( 15, 10 )	-- 2 cm each side
	self.plot:set_bdd( bdd )
end
function meu:do_go_xy_cano()
	local bu = self.ui.bu_pos
	--todo why get_value() is wrong here
	self.plot:do_go_xy_cano( bu:get_value_cano_clamped(1), bu:get_value_cano_clamped(2) )
end
function meu:move_step_ui( dx, dy )
	local plot = self.plot
	local x,y = plot:get_hard_xy_step()
	plot:do_go_xy_step( x+400*dx, y-400*dy )
end

function meu:move_test()
	local plot = self.plot
	plot:add_move_dxy_step( 300,300 )
	plot:add_move_dxy_step( 300,-300 )
	plot:add_move_dxy_step( -300,-300 )
	plot:add_move_dxy_step( -300, 300 )
end


function meu:update_ui()
--	if true then return end
	
	local function make_time_str( sec )
		sec = math.floor(sec) 
		local min = math.floor( sec / 60 )
		sec = sec - 60 * min
		local hour  = math.floor( min / 60 )
		local str
		if hour > 0 then
			min = min - 60 * hour
			str = hour.."h"
		else
			str  = ""
		end
		return str..min.."m"..sec
	end	

	local plot = self.plot 
	local ui = self.ui
	ui.bu_link_open_info:set_visible( not self.b_link or not param.get_bool( plot.ref.open_state ) )
	
	ui.bu_x:set_text( plot.pen_axidraw_x )
	ui.bu_y:set_text( plot.pen_axidraw_y )

	local id = plot.__cur_id or 1
	local nb = plot.queue:get_nb()
	--self:print( id..nb )
	if id==1 and id<nb then
		self.time_start = aaa.time.t
		plot.__len_inked = 0
	end

	local str = aaa.format.int_to_char_with_space( id )
	str = str.." / "..aaa.format.int_to_char_with_space( nb )
	local ts = self.time_start
	if ts then
		local t
		if id <= nb then
			t = aaa.time.t
			self.time_stop = t
		else
			t = self.time_stop
		end
		t = t - self.time_start
		local len = t/id*nb
		str = str.."\n".. make_time_str(t).." / "..make_time_str(len)
		local dist = plot.__len_inked or 0
		aaa.show( dist, "len")	
		dist = math.floor( dist / plot.step_by_mm ) / 1000
		str = str.."\n"..dist
		aaa.show( dist, "dist")	
	end
	ui.bu_queue:set_text( str )
end


function meu:update()
--	if true then return end

	local plot = self.plot
	--if self.b_link then
		local tmp = param.get( plot.ref.received_last_empty_not )
		--if tmp ~= 0 then
			self.ui.bu_link_info:set_text( tmp )
		--end
	--end
	plot:set_simul( not self.b_link )
	local bu = self.ui.bu_offset
	plot:set_offset(  bu:get_value_cano_clamped(1), bu:get_value_cano_clamped(2) )
	plot:update()
end

function meu:plot_strip_order( strip )
	local indexes = strip.__indexes
	local nb = #indexes
	local pl = self.__pl
	local plot = self.plot

	local x,y

	local function get_xy( i )
		local pt = pl:get_pt( indexes[i] )
		return pt[1], pt[2]
	end

	if strip.__b_invalid then
		if not self.__b_need_down then
			--self.plot:add_pen_up()
			--self.__b_need_down = true
		end
	else
		x,y = get_xy( 1 )
		if plot:is_inside_aaa( x,y ) then
			plot:add_move_xy_aaa( x, y )
			if self.__b_need_down then
				self.plot:add_pen_down()
				self.__b_need_down = false
			end
		end
	end
end

function meu:plot_strip( strip )
	local indexes = strip.__indexes
	local nb = #indexes
	local pl = self.__pl
	local plot = self.plot

	local x,y
	
	local function get_xy( i )
		if i > nb then
			self:print( i.." too big #indexes is ".. nb )
		end
		local pt = pl:get_pt( indexes[i] )
		return pt[1], pt[2]
	end
	local function add_move( i )
		local x,y = get_xy( i )
		self.x_last, self.y_last = x,y
		plot:add_move_xy_aaa( x, y )
		--self:print( pt[1].." pt "..pt[2] )
	end
	local function is_to_draw( x,y )
		--local d = x*x + y*y
		--if outside( x*x + y*y, 0.5*0.5, 0*0 ) then	return false	end 
		--return plot:is_inside_aaa( x,y ) and inside( aaa.math.get_turbulence( x,y,0, 1,3 ), .25, .45 )
		--return plot:is_inside_aaa( x,y ) and (math.fmod( x, .25 ) < .125)  and (math.fmod( y, .25 ) < .125)
		if not plot:is_inside_aaa( x,y ) then
			return false
		end
		return true
	end


	local function draw( ib,ie, b_closed )
		--self:print( "draw "..ib.." to "..ie.." "..b_closed )

		local function dist_to_last( i )
			local x,y = get_xy( i )
			x,y = x-self.x_last, y-self.y_last
			return x*x + y*y
		end

		if not b_closed and dist_to_last(ie) < dist_to_last(ib) then
			add_move( ie )
			plot:add_pen_down()
				for i=ie-1,ib,-1	do		add_move( i )	end
			plot:add_pen_up()
		else
			add_move( ib )
			plot:add_pen_down()
				for i=ib+1,ie		do		add_move( i )	end
				if b_closed			then	add_move( ib )	end
			plot:add_pen_up()
		end
	end

	-- validate (all pts inside)
	local i_out = nil
	for i=1,nb do
		local x,y = get_xy( i )
		if not is_to_draw( x,y ) then
			i_out = i
			break
		end
	end

	--strip:print( "begin point_nb "..nb )
	if i_out == nil then
		-- all points valid : we draw all
		draw( 1,nb, strip:is_closed() )
	else
		
		if self.s_clip == 1 then
			--self:print( "strip have a point "..i_out.." outside "..x..", "..y.." : skipping" )
			return
		end
		
		if i_out > 1 then
			--self:print( "we have first point out at index "..i_out.." on "..nb )
			draw( 1,i_out-1, false )
		end

		repeat
			local i_in
			for i=i_out,nb do
				local x,y = get_xy( i )
				if is_to_draw( x,y ) then i_in = i break end
			end	
			if i_in then
				--self:print( "in at index "..i_in )
				i_out = nil
				for i=i_in+1,nb do
					local x,y = get_xy( i )
					if not is_to_draw( x,y ) then
						i_out = i
						break
					end
				end
				if i_out then 
					--self:print( "out at index "..i_out )
					draw( i_in,i_out-1, false )
				else
					--self:print( "rest is in" )
					add_move( i_in )
					plot:add_pen_down()
						for i=i_in+1,nb			do		add_move( i )	end
						if strip:is_closed()	then
							local x,y = get_xy( 1 )
							if is_to_draw( x,y ) then
								add_move( 1 )
							end
						end
					plot:add_pen_up()
					--self:print( "__________________we reach the end")
					break
				end
			else
				--self:print( "___________________No more in")
				break
			end
		until false

	end
end

function meu:add_to_plot( b_order )
	self.x_last = self.x_last or 0
	self.y_last = self.y_last or 0
	local meu_gene = app:get_meu_by_name( "AxiGene_Test" )
	local strips = meu_gene:get_strips()
	if not strips then return end

	--self:print( #strips.__strips )
	strips = table.copy_deep( strips )
	--self:print( #strips.__strips )

	self.__pl = strips:get_point_list()
	self.plot:add_pen_up()
	if b_order then
		self.__b_need_down = true
		strips = strips:plot( self, "plot_strip_order" )
	else
		strips = strips:plot( self, "plot_strip" )
	end
	self.plot:add_pen_up()
end

function meu:draw()
--	if true then return end

	--if self.b_link then
	MEU.draw(self)
	--end
	self.plot:draw()
	--MEU.__plot = self.plot
end