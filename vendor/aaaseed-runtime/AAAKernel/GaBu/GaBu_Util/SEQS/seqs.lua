CLASS.DECLARE( "SEQS" )

function SEQS:__init_new( owner )
	SEQS.cur = self

	self.__seqs				= {}
	self.__seq_nb			= 0
	self.__seqs_id_by_title	= {}
	self.__seq_cur_id		= 0
	self.__b_hack_define_ui	= true
	self.__test_time		= 0.
	self.__b_run			= false
	self.__time_factor		= 1.
	self.__owner			= owner
	return self
end

function SEQS:create( name, owner )
	local self = SEQS:create_instance( name )
	self:print_inverse( "SEQS:create( \""..name.."\" )" )
	return self:__init_new( owner )
end
function SEQS:recreate( name, owner )
	if owner.seqs then
		owner.seqs:run(false)
		--self.seqs = nil
	end
	owner.seqs = SEQS:create( name, owner )
	return owner.seqs
end

function SEQS:get_seq( id )		return self.__seqs[id]							end
function SEQS:get_seq_cur_id()	return self.__seq_cur_id						end
function SEQS:get_seq_cur()		return self:get_seq( self:get_seq_cur_id( ) )	end
function SEQS:add_seq( seq )
	local nb = self.__seq_nb + 1
	self.__seq_nb = nb
	self.__seqs[nb] = seq
	self.__seqs_id_by_title[ seq:get_name_lowercase() ] = self.__seq_nb
	return seq
end
function SEQS:create_seq( name, level_id, seq_id, comment, duration )
	local seq = self:add_seq( SEQ:create( name, level_id, seq_id, comment, duration ) )
	seq:set_ease_in_time(1.)
	return seq
end

function SEQS:find_id_by_title( title )
	if type(title) == "string" then
		--self:print( "try to find "..title )
		--table.print( self.__seqs_id_by_title, "__seqs_id_by_title", 1 )

		return self.__seqs_id_by_title[string.lower(title)]
	end
end
function SEQS:get_time_from_id( id )
	local id_cur = self:get_seq_cur_id()
	local seq_cur = self:get_seq_cur()
	if seq_cur then	--todo mona was crashing the tablet
		local t = seq_cur:get_time()
		while id < id_cur do
			t = t + self:get_seq(id):get_duration()
			id = id + 1
		end
		return t
	end
	return 0
end
function SEQS:get_time_left_from_id( id )
--	local id_cur = self:get_seq()
	if self:get_seq( id ) then
		local t = self:get_seq( id ):get_time()
		return self:get_seq(id):get_duration() - t
	end
	return 0.0
end
function SEQS:get_time_from_title( title )
	local id = self:find_id_by_title( title )
	return self:get_time_from_id(id)
end
function SEQS:get_duration_from_title( title_begin, title_end )
	local id		= self:find_id_by_title( title_begin )
	local id_end	= title_end and self:find_id_by_title( title_end ) or self:get_seq_cur_id()
	local t = 0
	while id <= id_end do
		t = t + self:get_seq(id):get_duration()
		id = id + 1
	end
	return t
end

function SEQS:switch_seq( id_old, id_new )
	--aaa.debug.print_traceback( "SEQS:switch_seq()" )
	--	error()
	--	aaa.debug.print_traceback()
	local seq_old = self:get_seq( id_old )
	local seq_new = self:get_seq( id_new )
	--if not seq then return end
	if self.verbose >= 1 then aaa.print_inverse(		"SEQS : Switch to "..id_new.." "..(seq_new and seq_new:get_title() or nil) ) end
	if seq_old then
		if self.verbose >= 1 then aaa.print_inverse(	"            from "..id_old.." "..seq_old:get_title() ) end
		seq_old:switch( false, seq_new )
	end
	if seq_new then
		seq_new:switch( true, seq_old )
		--todo eo left over
		if seq_new.switch_leo then
			seq_new:switch_leo( seq_old )
		end
	end
	self.__seq_cur_id = id_new
	--if seq_new then seq_new:print() end
	return seq_new
end
function SEQS:restart_seq( id )
	id = id or self:get_seq_cur_id()
	--self:print( "SEQS:restart_seq( "..id )
	return self:switch_seq( id, id )
end

function SEQS:set_seq( id, b_error )
	if type(id) == "string" then
		id = string.lower( id )
		local new_id = self:find_id_by_title( id )
		if new_id == nil then
			if b_error then error( " can't change seq with name "..id ) end
			return false
		end
		id = new_id
	end
	if id == nil then
		if b_error then error( " Game can't change seq with nil id" ) end
		return false
	end
	--aaa.print( id.." "..self.seq_nb )
	id = id - 1
	if id <= 0 then id = 0 end
	id = math.fmod( id, self.__seq_nb ) + 1

	--aaa.print( id.." "..self.seq_nb )
	if self.__seq_cur_id ~= id then
		self:switch_seq( self.__seq_cur_id, id )
	end

	return true
end

function SEQS:inc_seq()			return self:set_seq( self:get_seq_cur_id() + 1 )	end
function SEQS:dec_seq()			return self:set_seq( self:get_seq_cur_id() - 1 )	end
function SEQS:restart( seq_id )
	table.apply_fn( self.__seqs, function(elt) elt:disable_mu_used( false ) end )
	return self:set_seq( seq_id or 1  )
end
function SEQS:begin( b_preroll )
	if b_preroll then
		self:ask_preroll()
	end
	self:run(true)
	return self:restart( 1 )
end

function SEQS:run( b_on, sender )
	self:print( "asked run( "..b_on.." )" )
	local b
	if type(b_on) == "boolean" then
		b = b_on
	else
		b = b_on > .5
	end
	if self.__b_run ~= b then
		self.__b_run = b
		self:print( "run is now ".. self.__b_run )
		if sender then
			sender:send( "play", b and 1 or 0 )
		end
	end
end
function SEQS:is_test()			return self.__test_time > 0.		end
function SEQS:is_run()			return self.__b_run and not ( self:is_test() or self:is_preroll() ) end
function SEQS:flip()			self:run( not self.__b_run ) 		end
function SEQS:set_looping( b )	self.__b_looping = b 				end
function SEQS:is_looping()		return self.__b_looping				end

--	but static
function SEQS.ui_inc_seq( on )		if on==1 and SEQS.cur	then return SEQS.cur:inc_seq()	end		end
function SEQS.ui_dec_seq( on )		if on==1 and SEQS.cur	then return SEQS.cur:dec_seq()	end		end
function SEQS.ui_begin( on )		if on==1 and SEQS.cur	then return SEQS.cur:restart()	end		end
function SEQS.ui_run( b_on )		if SEQS.cur				then SEQS.cur:run( b_on )		end		end
function SEQS.ui_flip()				if SEQS.cur				then SEQS.cur:flip()			end		end
function SEQS.ui_set_seq( title )
	if SEQS.cur	then
		return SEQS.cur:set_seq( title )
	end
	return false
end

--	PREROLL
--
function SEQS:is_preroll() return self.__b_preroll end
function SEQS:ask_preroll()
	self.__b_preroll = true
	self:print_inverse( "Preroll asked" )
end
function SEQS:complete_preroll()
	self.__b_preroll = false
	self:print_inverse( "Preroll done" )
end


function SEQS:updraw( b_update )
	if self.verbose >= 2 then self:print( "updraw( "..b_update.." )" ) end
	--b_update = not b_update
	if b_update then	-- b_update set by stereo
		self.__dt = aaa.time.dt * self.__time_factor
	else
		self.__dt = nil	-- to test bad update strategy was 0
	end

	local seq = self:get_seq_cur()
	if not seq then
		--self:print_error( "No current seq" )
		return
	end
	--self:print( "maa updraw( "..b_update.." )" )
	seq:updraw( self.__b_run, b_update, self.__dt, self.__test_time )
	--todomdh	maa add this
	seq = self:get_seq_cur()

	--self:print( "lopping : "..self:is_looping() )
	if b_update and self:is_preroll() then
		--self:print( "update and preroll")
		self:inc_seq()
		if self:get_seq_cur_id() == 1 then
			self:complete_preroll()
		end
	elseif (not aaa.stereo.is_active() or not b_update) and seq:is_done( self.__b_run, self.__test_time ) then
		--self:print( "inc_seq")
		--todomdh	maa removed this
	--	self:print( "SEQS:updraw() inc_seq looping is "..self:is_looping() )
		if self:is_looping() then
			self:restart_seq()
		else
			self:inc_seq()
		end
	end
end

function SEQS:do_action( str_src, str_action, ...  )	--todo use the ...
	local str = string.lower(str_action)
	local mess = "from "..str_src.." SEQS:do_action( \""..str.."\" )"	--todo add the ..,
	self:print( mess )

	local b_used = self:set_seq( str, false )
	if not b_used then
		b_used = true
		if		str == "begin"			then self:restart(1)

		elseif	str == "prev"			then self:dec_seq()
		elseif	str == "next"			then self:inc_seq()

		elseif	str == "play"			then self:run( true )
		elseif	str == "stop"			then self:run( false )
		elseif	str == "flip"			then self:flip()

		elseif  str == "redefine"		then (self.__owner or app):define_seqs()

		else	self:print( "SEQS don't do "..str )
				b_used = false
		end
	end
	return b_used
end

function SEQS:do_key_special( key )
	self:print_do_key_special( "SEQS", key )

	return false
end

if not aaa.b_ios then

	local SY = 1
	function SEQS:define_buttons( bus, target, action_method_name )

		local function fn_make_rect( rect )
			local sy = (rect[4] or SY) *.12
			local FSX = .8
			local sx = rect[3] * FSX
			local DY = -.11
			local DX = rect[3]
			return  {rect[1] * FSX -.3 + sx * .5,	rect[2] * DY, 	sx,sy}
		end

		local function add_button( rect, text )
			return bus:add_button( text, fn_make_rect( rect ) )
		end
		local function add_button_trig( rect, text )
			local bu = bus:add_trig( text, fn_make_rect( rect ) )
			bu:set_method_on_click( target, action_method_name, "Bu", text )
			return bu
		end
		local function add_slider( rect,	text, ... )
			local bu = bus:add_slider( text, fn_make_rect( rect ), ... )
			return bu
		end

		local bu
		local iy = 0
		local x,y, sx,sy


		local DY = .2
		local SEP_Y = SY + DY
		local SEP_FOR_2 = .01

		bu = add_button_trig(						{0, 			iy,			.5-SEP_FOR_2},	"redefine" 	)
			bu:set_color_back( { 1, 0, 0, .2 } )

		bu = add_slider(							{.5+SEP_FOR_2,	iy,			.5-SEP_FOR_2},	"test"		)
			bu:set_min_max_value( 0, 10, 0 )
			--bu:set_color_back( { 1, 1, 0, .5 } )
			bu:set_meter_color( 1,0,0, 1 )
			bu:set_meter( true )
			bu:set_show_value( true )
			bu:set_target( self, "__test_time" )


		iy = iy + SY + DY*2
		local bu_flip = add_button_trig(			{0,				iy, 		.7-SEP_FOR_2},	"flip"	 	)
		bu_flip:set_function( "update_before",
							function()
								bu_flip:set_color_back( self.__b_run and { 0, 1, 0, .5 } or { 1, 0, 0, 1. } )
							end
					)
		local bu_loop = add_button(					{.7+SEP_FOR_2,	iy,			.3},			"loop"		)
		bu_loop:set_target_lua( self, "__b_looping", false )

		iy = iy + SY + DY
		bu = add_slider(  							{0,				iy, 		.7-SEP_FOR_2},	"time"		)
			bu:set_meter( true )
			bu:set_show_value( true )
			bu:set_method( "update_before", bu, "update_before" )
			bu.update_before =
				function( bu )
					--bu:print( "uiui" )
					local seq = self:get_seq_cur()
					if seq then
						local dur = seq:get_duration() or 3600.
						bu:set_min_max( 0., dur )
						if not self:is_run() or bu:is_contact() then
							seq:__set_time( bu:get_value() )
						else
							bu:set_value( seq:get_time() )
						end
					end
					--bu:set_text_color_green_info( self:is_run() )
				end

		bu = add_slider( 							{.7+SEP_FOR_2,	iy,			.3-SEP_FOR_2},	"Speed"		)
			bu:set_target_lua( self, "__time_factor" )
			bu:set_min_max_value( .1, 10., 1 )
			bu:set_meter( true )
			bu:set_show_value( true )

	--bu = bus:add_text_info( "info_time", 	{x,y, sx,sy } )
	--		bu:set_method( "update_before", bu, "update_before" )
	--		bu.update_before = function(bu)
	--								local seq = self:get_seq_cur()
	--								bu:set_text( seq and BU:format_time_duration( seq:get_time(), seq:get_duration() ) )
									--bu:set_text_color_green_info( self:is_run() )
	--							end
		iy = iy + SY
		bu = bus:add_text_info( "info", fn_make_rect( {0,iy, 1,SY} ))
			bu:set_method( "update_before", bu, "update_before" )
			bu.update_before =
				function( bu )
					--bu:print( "uiui" )
					local seq = self:get_seq_cur()
					local str = self:get_seq_cur_id().." : "..(seq and (seq:get_seq_id().." "..seq:get_title()))
					local dur = seq and seq:get_duration()
					if dur then str = str.." "..string.format( "%.2f", dur).."s" end
					bu:set_text( str )
					bu:set_text_color_green_info( self:is_run() )
				end

		iy = iy + SY + DY*1
		add_button_trig(							{0, 				iy, 		.5-SEP_FOR_2},	"prev"		)
		add_button_trig(							{0.5+SEP_FOR_2, 	iy,			.5-SEP_FOR_2},	"next"		)


		iy = iy + SY + DY
		add_button_trig(							{0,				iy,			1},	"begin"		)

		iy = iy + DY *2
		local seqs = self.__seqs
		local level_id = -42

		local nb = #seqs
		iy = iy + SY * .5
		iy = iy + DY
		local sy = (17-iy+DY)/nb - DY
		local sx = .8
		iy = iy + sy * .5
		--iy = iy + sy + DY
		for i=1,nb do
			local x
			local seq = seqs[i]
			--	table.print( tab[i], "seqs[i]", 1 )
			local id_new = seq:get_level_id()
			if level_id ~= id_new or id_new == 0 then
				level_id = id_new
				x = 0
			else
				x = 1. - sx
			end
			add_button_trig(						{x,				iy,			sx,sy},		seq:get_title()	)
			iy = iy + sy + DY
		end
	end

	function SEQS:define_bus_seqs()
		local bus = BUS:create( self:get_name().."_Seq" )
		--bus:set_active( false )
		bus:init_begin()

			self:define_buttons( bus, self, "do_action" )

		bus:init_end()
		return bus
	end

	function SEQS:define_window_seqs()
		local bus_seq = self:define_bus_seqs()

		local bu
		local bus = BUS:create( "SEQS" )

		bus:set_active( false )
		bus:init_begin_add_to_ga()

			bu = bus:add_window(	"Seq",	bus_seq,	{-3.5, -0.75,	.8, 3} )
				bus_seq:set_transfo( 	1,4,			0.07,-1.9 )
				bu:set_pos_mini(		{-3.6, 1.7,		.5, .25} )
				bu:set_color_back( {0,0,0,.5} )

		bus:init_end()
		return bus
	end

end

