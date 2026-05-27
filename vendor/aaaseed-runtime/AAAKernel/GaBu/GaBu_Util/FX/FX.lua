--
--	FX
--
CLASS.DECLARE( "FX" )

function FX:set_bind_2d( bind )	self.meu:set_bind_2d( bind )	end

function FX:create_window()
	local name = self:get_name()
	--	we pass FX here to avoid trouble in MEU:create
	self.meu = MEU:create( self.ref.obj, name, "FX" )	--todo check this way to use MEU and the "1"
	self.__b_no_lua_ok = true	--hack to avoid worse
	self.meu.__b_dont_free_obj = true	--todo what happen here ?

	local bus = BUS:begin_window( name )
		self.bus = bus
		--bus:set_bu_pos_load_save( true )

		local bu
		--	PRESET
		bu = bus:add_preset( "preset", {1,3.6, 2.,.4} )
		self.bu_preset = bu
		bu:set_nb( 8, 2 )

		bus:move_next()
		self:add_ui_window( true )

		--	we do a close button
		bu = bus:add_trig( "close", {3.9,3.9, .8,.8} )
			bu.draw = BUTTON.__draw_close
			bu:set_method_on_click( self, "close_window" )

	bus:end_window()

	--MEU:add_rgb( ix, iy, ref )
	self.bu_preset:set_preset_target( self.meu )
	self.meu:__set_bus( bus, 2 )	--todoaqua true skip preset load avoid provoking error
											--todoaqua solve this (a filename problem)
	return bus
end

function FX:init_inst( b_active )
	if self.verbose >= 1 then aaa.print_method() end

	self.__b_active_at_init = b_active

	local ref = self.ref
	ref.layer_a		=	aaa.layers.get_layer( ref.obj, 1 )
	--self:print( ref.layer_a )
	if ref.layer_a then
		ref.color	=	aaa.obj.get_branch_by_class_no_error( ref.layer_a, "color" )
		if ref.color then
			ref.alpha = param.get_ref( ref.color, "global_alpha" )
		end
	end

	--todo meu set it to false, meu and fx have to merge at some point
	param.set( ref.obj, "lua", true )
	aaa.obj.update_then_draw( ref.obj )

	self:draw()
	if	self.init	then	self:init()		end
end

function FX:create( name, obj, b_active )
	local self = FX:create_instance_with_obj( name, obj )
	self:init_inst( b_active )
	return self
end

function FX:close_window()
	self.__bu_win:set_active( false )
end

function FX:set_focus()
	if self.verbose >= 1 then aaa.print_method() end
	local obj = self.ref.obj
	aaa.obj.set_focus_ui( obj )
	self.__bu_win:set_active( true )
end

function FX:__set_window_state(state)
	if self.__bu_win then
		self.__bu_win:__set_window_state(state)
	else
		self:print_inverse( "No bu_win" )
	end
end

function FX:set_focus_if_on()
	if self.__bu_active:get_value_as_bool() then
		self:set_focus()
	else
		self:close_window()
	end
end

function FX:add_ui_window( b_alpha )
	local bu
	if b_alpha and self.ref.alpha then
		bu = bus_cur:add_slider( "Alpha" )
		bu:set_target_param( self.ref.alpha )
		bu:set_min_max( 0, 1 )
		self.__bu_alpha = bu	--todoaqia check interaction with other
	end
	return bu
end

function FX:add_ui()
	local name = self:get_name()
	local bu = bus_cur:add_button( name )
	bu:set_mobile( true )
	bu:set_pos_load_save( true )
	bu:set_value_load_save( true )
	bu:set_text( name )
	if self.__b_active_at_init then bu:set_value( 1 ) end
	bu:set_method_on_click( self, "set_focus_if_on" )
	self.__bu_active = bu

	self:create_window()

	return bu
end

function FX:add_window( i )
	local bu = app:add_window_auto( self:get_name(), self.bus,		i  )
	bu:set_method_on_click( self, "set_focus" )
	self.__bu_win = bu
end

function FX:load()	end
function FX:init()	end

function FX:is_active()
	return self.__bu_active and self.__bu_active:get_value_as_bool() or false
end

function FX:update()
	if self.__bu_active then
		if not self.__bu_active:get_value_as_bool() then return end
	end
	if GA.b_spy then aaa.spy.push_range( self..":update()", 7 ) end
	if self.update_low then
		--self:print( "update low" )
		FX.cur = self
		self:update_low()
		FX.cur = nil
	end
	if GA.b_spy then aaa.spy.pop_range() end
end

function FX:draw()
	if self.__bu_active then
		if not self.__bu_active:get_value_as_bool() then return end
	end

	if GA.b_spy then aaa.spy.push_range( self..":update()", 8 ) end
	local ref = self.ref
	--if self.bu_alpha then param.set( ref.alpha, self.bu_alpha:get_value() ) end
	FX.cur = self
	if self.camera_to_use then
		--self:print( self.camera_to_use )
		if app and app.draw_camera then	app:draw_camera( self.camera_to_use ) end
	end
	--self:print( "draw "..ref.obj.." "..aaa.obj.get_filename(ref.obj) )
	aaa.obj.update_then_draw( ref.obj )
	if self.draw_low then
		--self:print( "draw_low "..ref.obj )
		self:draw_low()
	end
	FX.cur = nil
	if GA.b_spy then aaa.spy.pop_range() end
end

