CLASS.DECLARE( "PRESET", SELECTOR, {
	__b_preset_use = false,
	} )

function PRESET:do_mouse_move( x, y )
	--self:print( "do_mouse_move( "..x..", "..y.." )" )
	oo.getsuper(PRESET).do_mouse_move( self, x, y )
	--self:print( "do_click_down "..self:get_value().."/"..self.__nb )
	if self.__preset_target and not aaa.keyboard.is_ctrl() then
		self.__preset_target:recall_preset( self:get_value() )
	end
end

function PRESET:begin_fx()
	oo.getsuper(PRESET).begin_fx( self, .1, .05 )
end

function PRESET:do_click_down( x,y )
	oo.getsuper(PRESET).do_click_down( self, x,y )
	--self:print( "do_click_down "..self:get_value().."/"..self.__nb )
	if self.__preset_target then
		--self:print_debug( aaa.keyboard.is_shift().." "..aaa.keyboard.is_ctrl().." "..aaa.keyboard.is_alt() )
		if aaa.keyboard.is_ctrl() then
			--self:print_debug( "CTRL" )
			if aaa.keyboard.is_alt() then
				--self:print_debug( "alt" )
				if aaa.keyboard.is_shift() then
					--self:print_debug( "shift" )
					self:print_debug( "SAVE_PRESETS" )
					self.__preset_target:save_presets( 1 )	
					self:begin_fx()				--SHIFT CTRL ALT
				else
					self:print_debug( "ERASE ".. self:get_value() )
					self.__preset_target:erase_preset( self:get_value() )	--CTRL ALT
					self:begin_fx()
				end
			else
				self:print_debug( "STORE ".. self:get_value() )
				self.__preset_target:store_preset( self:get_value() )		--ALT
				self:begin_fx()
			end
		end
	end
end

function PRESET:do_click_up()
	--self:print( "value is "..self:get_value() )
	--table.print( self.__balues, "hbjkh", 2 )
	--aaa.print_method()
	oo.getsuper(PRESET).do_click_up(self)
end

--todo would it be better to handle change at the BUI level
function PRESET:do_key( key )
	self:print_do_key( "PRESET", key )

	local val = self:get_value() 
	local b_key_used = oo.getsuper(PRESET).do_key( self, key )
	if b_key_used then
		if self.__preset_target and val ~= self:get_value() then
			self.__preset_target:recall_preset( self:get_value() )
			self:begin_fx()
		end
	end
	return b_key_used 
end
function PRESET:do_key_special( key )
	self:print_do_key_special( "PRESET", key )
	return oo.getsuper(PRESET).do_key_special( self, key )
end

function PRESET:update()
	--aaa.print_method()
	oo.getsuper(PRESET).update(self)
end

function PRESET:create( name, rect )
	local self = PRESET:create_instance( name, rect )
	--todo have to be done bit not the right way
	self:set_nb( 8 )
	return self
end
function PRESET:set_preset_target( tar )
	self.__preset_target = tar
end

function PRESET:get_bu_list()
	local target = self.__preset_target
	--aaa.print_method()
	if target and target.__get_bu_for_preset then
		return target:__get_bu_for_preset(0)
	end
end

function PRESET:draw_items()
	local preset_target = self.__preset_target
	if preset_target then
		local alpha = self:get_alpha_bu_to_draw() * .5
		local x,y
		local dx = self.__nb_u_over
		local dy = self.__nb_v_over
		gol.color_green( alpha )
		for i = 1, self:get_nb() do
			if preset_target:get_preset( i ) then
				x,y = self:get_item_lb(i)
				aaa.draw_rect( x,y, x+dx,y+dy )
			end
		end
		gol.color_white( alpha )
		local i_sel = self:get_value()
		x,y = self:get_item_lb(i_sel)
		--self:print( x.." "..y )
		aaa.draw_rect( x+dx*.2, y+dy*.2, x+dx*.8, y+dy*.8 )
	end
end