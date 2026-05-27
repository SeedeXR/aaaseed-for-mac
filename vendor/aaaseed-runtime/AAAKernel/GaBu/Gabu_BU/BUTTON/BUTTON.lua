--todo	deal serious with text justification (in/out)
--toto		click on external text work too
--todo	highlight with cross only on trig
if CLASS.DECLARE( "BUTTON", BUI, {
	__tex_draw			= nil,
	__tex_on 			= nil,
	__tex_off 			= nil,
	__b_text_inside 	= true,
	__b_check_mul		= false,
	__b_check_disk		= false,
	__text_rect_ratio 	= 4,
	__text_align_x 		= "center",
	} ) then
	BUTTON:set_class_status_doc(	CLASS.STATUS.GABU,
									"this is simply a BUTTON, it display text according to a value" )
end

--override the default value to avoid rect too long for small Text
--	it long by default to allow more easy contact
function BUTTON:set_text_rect_ratio( r )	self.__text_rect_ratio = r			return self end

function BUTTON:set_text_inside( b )		self.__b_text_inside = b 			return self end
function BUTTON:is_text_inside()			return self.__b_text_inside			end

function BUTTON:is_inside_xy_local( x,y )	--todo could be faster
	local b_in = oo.getsuper( BUTTON ).is_inside_xy_local( self, x,y )
	if b_in then
		return true
	end

	--for text button outside
	if self:is_text_draw() and not self.__b_inside_main_rect_only then
		--aaa.print_fn()
		if not self:is_text_inside() then
			--todo we compute the rect when we draw
			--	we should centralize in a unique fn
			--	and also cache it
			local l,b, sx,sy = self:__get_text_rect()
			if l then
				local r, t = l + sx, b + sy
				local l_rect, r_rect, b_rect, t_rect

				local lim_l = - .75 - .5 -- .75 is the beginning of the text at the right of button .5 is the size of the button

				-- Left and Right position
				if l < lim_l then	-- Left and Right position is text not button
					l_rect = l
					r_rect = r
				else				-- Left position is button and Right position is text
					l_rect = -.5
					r_rect = r
				end

				-- Bottom position
				if b < -.5 then	b_rect = b
				else			b_rect = -.5
				end

				-- Top position
				if t > .5 then 	t_rect = t
				else			t_rect = .5
				end

				if x < r_rect and l_rect < x and y < t_rect and b_rect < y then
					--self:print( "alt "..x..", "..y.." )" )
					return true
				end
			end
		end
	end
	return false
end

BUTTON.doc.is_multiple = "() return true if BUTTON was set using set_multiple"
function BUTTON:is_multiple()				return self.__b_multiple 			end
BUTTON.doc.set_multiple = "( tab_name_or_selector, name, val ) like set_menu() but with a different UI"
function BUTTON:set_multiple( tab_name_or_selector, name, val )
	--table.print( self:__get_balue(1), "balue at begin", 2 )
	local nb
	local bu_selector
	--todonow test is_class_name()
	--table.print( tab_name_or_selector, "tab_name_or_selector", 1 )
	if CLASS.is_gabu_obj( tab_name_or_selector, SELECTOR ) then
		bu_selector = tab_name_or_selector
		nb = tab_name_or_selector:get_nb()
		--self:box_debug( "selector have "..nb.." item" )
	else
		--table.print( tab_name_or_selector, "tab_name_or_selector", 1 )
		nb = #tab_name_or_selector
		--self:print( " BUTTON:set_multiple nb is "..nb )
		self:set_min_max( 1, nb )	--todo range [0,nb] ?
		bu_selector = self:__create_selector_multiple( tab_name_or_selector, name, val )
	end

	--init
	self.__b_multiple = true
	self:set_min_max_strict( true )
	--self:set_value_type_bool(false)

	--bu_selector:__transfert_target_from( self )	--	of target already set the delegate
	--table.print( self:__get_balue(1), "balue before", 2 )
	self:set_selector( bu_selector )
	self:set_target_bu( bu_selector )
	--table.print( self:__get_balue(1), "balue after", 2 )
	--bu_selector:set_target_bu( self ) 
	self:set_text_selector( true )

	return self
end
function BUTTON:__create_selector_multiple( tab_text, name, val )
	--todo "menu choose" is a bad name
	local bu = SELECTOR:create( "menu choose", { 1,1, 2,2 } )
		bu:set_text_draw( false )
		local min, max = self:get_min_max()
		bu:set_nb( 1, max - min + 1 )
		bu:set_item_text( 1, unpack(tab_text) )
		bu:set_value( val or 1 )
	return bu
end

--[[ 2024 Sep removed by Maa done differently in BUI now (intercept the get_balue)
-- function BUTTON:__do_delegate_for_value( method, ... )
-- 	local del, self = self:__get_delegate_and_self_for_value()
-- 	return del[method]( self, ... )
-- end

--todo clean this at BU or BUI level
--should cover more like is/set_value_type_bool
-- function BUTTON:get_value( ... )					return self:__do_delegate_for_value( "get_value", 					... )	end
-- function BUTTON:set_value( ... )					return self:__do_delegate_for_value( "set_value", 					... )	end
-- function BUTTON:get_value_cano_clamped( ... )		return self:__do_delegate_for_value( "get_value_cano_clamped", 		... )	end		--2024 Sep
-- function BUTTON:set_value_cano( ... )				return self:__do_delegate_for_value( "set_value_cano", 		... )			end		--2024 Sep
-- function BUTTON:set_value_ui( ... )					return self:__do_delegate_for_value( "set_value_ui", 				... )	end		--todo check this
-- function BUTTON:add_value_ui( ... )					return self:__do_delegate_for_value( "add_value_ui", 				... )	end
-- function BUTTON:get_value_as_bool( ... )			return self:__do_delegate_for_value( "get_value_as_bool", 			... )	end

-- function BUTTON:get_item_text( ... )				return self:__do_delegate_for_value( "get_item_text", 				... )	end
-- function BUTTON:set_item_text( i, ... )				return self:__do_delegate_for_value( "set_item_text", 	i,			... )	end

-- function BUTTON:get_item_data( ... )				return self:__do_delegate_for_value( "get_item_data", 				... )	end
-- function BUTTON:set_item_data( i, ... )				return self:__do_delegate_for_value( "set_item_data", 	i,			... )	end

-- function BUTTON:get_values( ... )					return self:__do_delegate_for_value( "get_values", 					... )	end
-- function BUTTON:set_values( ... )					return self:__do_delegate_for_value( "set_values", 					... )	end

-- function BUTTON:set_target_param( ... )				return self:__do_delegate_for_value( "set_target_param", 			... )	end
-- function BUTTON:set_target_lua( ... )				return self:__do_delegate_for_value( "set_target_lua", 				... )	end
-- function BUTTON:set_target( ... )					return self:__do_delegate_for_value( "set_target", 					... )	end

-- function BUTTON:set_midi( ... )						return self:__do_delegate_for_value( "set_midi", 					... )	end
-- function BUTTON:set_function_on_value_change( ... )	return self:__do_delegate_for_value( "set_function_on_value_change", 		... )	end
-- function BUTTON:set_method_on_value_change( ... )	return self:__do_delegate_for_value( "set_method_on_value_change", 	... )	end

-- function BUTTON:get_min_max( ... )					return self:__do_delegate_for_value( "get_min_max", 				... )	end
-- --]]

function BUTTON:set_trig()
	self.__b_trig = true
	self:set_value( false )
	self:set_value_load_save( false )
--	self:set_pos_load_save( false )
	self:set_preset_use( false )
--	self:set_text_inside( false )

	if not rawget( self, "__color_back" ) then
		self:set_color_back( "reset" )
	end
	return self
end
function BUTTON:is_trig()			return self.__b_trig			end

function BUTTON:set_check_mul( b )	self.__b_check_mul = b			end
function BUTTON:is_check_mul() 		return self.__b_check_mul		end

function BUTTON:set_check_disk( b )	self.__b_check_disk = b			end
function BUTTON:is_check_disk() 	return self.__b_check_disk		end

function BUTTON:set_frame_plus( b )	self.__b_frame_plus = b			end
function BUTTON:is_frame_plus() 	return self.__b_frame_plus		end

--  tex settings
function BUTTON:set_tex_draw( bind_on, bind_off )
	self.__b_tex_draw = true
	self.__tex_on = bind_on
	self.__tex_off = bind_off
end
-- Check also if tex is allow in the GA
function BUTTON:is_tex_draw() 	return (self.__b_tex_draw and GA:is_tex_draw_custom()) 	end
function BUTTON:get_tex_draw() 	return self.__tex_on, self.__tex_off					end


function BUTTON:set_confirmation( b, text_true, text_false, b_flip_color, b_self_destroy )
	text_true  = text_true  or "Yes"
	text_false = text_false or "No"

	self.__b_is_confirmation = b
	self.__b_disabled_action = true
	self.__b_confirmation_displayed = false
	self.__b_confirmation_self_destroy = b_self_destroy
	
	if b then
		local name_begin = self:get_name().."_"
		self:set_trig()

		local bus = self:get_bus_up()
		local tab_key = bus:get_tab_key()
		bus:set_tab_key()
		local bu1, bu2

	-- add the confirmation button
		local function set_bu( bu, text )
			bu:set_pos_load_save(false)
			--bu:set_text_align_x( "center" ) --todo check
			--bu:set_text_x( -.1 )
			bu:set_text( text )
			bu:set_text_inside( true )
			bu:set_active( false )
		end

		local function set_color( bu, b )
			bu:set_color_back( b and { .1,.5,0, 1 } or { 1,0,0, 1 } )
		end

		local bu_false = bus:add_trig( name_begin.."false" )
			set_bu( bu_false, text_false )
			set_color( bu_false, not b_flip_color )
			bu_false:set_method_on_click( self, "show_confirmation", false )

		-- true button
		local bu_true = bus:add_trig( name_begin.."true",		{0,0, 8/16,8/16} )
		 	set_bu( bu_true, text_true )
		 	set_color( bu_true, b_flip_color )
		 	bu_true:set_method_on_click( self, "confirm", self:get_x(), self:get_y() )

		self.__bu_confirmation = { bu_true=bu_true, bu_false=bu_false }
--		self.bus_confirmation = bus

		bus:set_tab_key( tab_key )
	else
	end

	return self
end
function BUTTON:destroy_self()
	local t_bu = self.__bu_confirmation
	if t_bu then
		t_bu.bu_true :free()
		t_bu.bu_false:free()
	end
	local bus = self:get_bus_up()
	bus:remove_bu( self )
	self:free()
end

function BUTTON:is_confirmation() 	return self.__b_is_confirmation end
function BUTTON:show_confirmation( b )
	local t_bu = self.__bu_confirmation
	t_bu.bu_true :set_active( b )
	t_bu.bu_false:set_active( b )
	self:set_visible( not b )
	self.__b_confirmation_displayed = b
	if b then
		local text_type = self:__get_text_type()
		local x,y = self:get_xy()
		local sx,sy = self:get_sxy()
		sx = sx / 2
	
		local function set_bu( bu, x )
			bu:set_xy_sxy( x,y, sx,sy )
			bu :move_to_front()
			bu:__set_text_type(text_type)
		end

		set_bu( t_bu.bu_false, x-sx*.5 )
		set_bu( t_bu.bu_true,  x+sx*.5 )
	else
		if self.__b_confirmation_self_destroy then
			self:destroy_self()
		end
	end
end

function BUTTON:confirm( x,y )
	self:print( "BUTTON:confirm()" )
	self:show_confirmation( false )
	self.__b_disabled_action = false

	self:do_click_down( x,y )
	self:do_click()
	self:do_click_up( x,y )
end

function BUTTON:do_click_down( x,y )
	if self:is_confirmation() and self.__b_disabled_action and not aaa.keyboard.is_alt() then
		oo.getsuper(BUTTON).do_click_down( self, x,y )
		self:show_confirmation( not self.__b_confirmation_displayed )
	elseif self:is_multiple() then
		oo.getsuper(BUTTON).do_click_down( self, x,y )
		self:add_value_ui( (x < 0) and -1 or 1 )
	elseif self:is_menu() then
		oo.getsuper(BUTTON).do_click_down( self, x,y )
	else
		--self:print( "BUTTON:do_click_down "..x.." "..y )
		if self.verbose >= 1 then aaa.print_method() end
		--self:__print_value( "after SUPER:do_click_down() "..oo.getsuper(BUTTON) )
		oo.getsuper(BUTTON).do_click_down( self, x,y )
		--self:__print_value( "then SUPER:do_click_down() "..oo.getsuper(BUTTON) )
		--self:print( "BUTTON:do_click_down() min-max is : "..self:get_min().."-"..self:get_max() )
		if self:is_trig() then
			--self:print( "BUTTON:do_click_down() is trig ")
			--self:print( "BUTTON:do_click_down() min-max is : "..self:get_min().."-"..self:get_max() )
			if self:get_min()==self:get_max() then
				self:set_value( self:get_min() )
			else
				--self:print( "BUTTON:do_click_down() set_value_cano(1)" )
				self:set_value_cano( 1 )
			end
		else
			--self:print( "BUTTON:do_click_down() not trig " )
			--self:print( "not trig "..x.." "..y.." value "..self:get_value().." as bool "..self:get_value_as_bool() )
			--self:print( "Was "..self:get_value_cano_clamped() )

			-- this test could be an option (removed 2025 May)
			--if x<.5 then
				--	here we flip on click
				--hackMay
				--if not self:is_mobile() or self:is_click_double() then
				--self:print( "\tis_value_type_bool() 	: "..self:is_value_type_bool()		)
				--self:print( "\tget_value_cano()			: "..self:get_value_cano()			)
				--self:print( "\tget_value_cano_clamped() : "..self:get_value_cano_clamped()	)
				self:flip_value()
			--end
		end
	end
end

function BUTTON:do_click_up( x,y )
	--aaa.print_method()
	oo.getsuper(BUTTON).do_click_up( self, x,y )
	if self:is_multiple() or self:is_menu() then
	else
		if not self.__b_disabled_action then
			--self:print( "testing is_trig()" )
			if self:is_trig() then
				--self:print( "BUTTON:do_click_up() "..self:get_min().." "..self:get_max() )
				if self:get_min()==self:get_max() then
					self:print( "BUTTON:do_click_up() min==max"..self:get_min() )
					self:set_value( self:get_min() )
				else
					self:set_value_cano( 0 )
				end
			end
		end
	end

	-- reinit the confirmation
	if self:is_confirmation() and not self.__b_disabled_action then
		self.__b_disabled_action = true
	end
end
--[[ try to transfert to BUI
function BUTTON:do_key(key)
	self:print_do_key( "BUTTON", key )

	local b_key_used
	local id = self:get_id_interacting()
	if		key==42 or key==43 or key==45 or key==47 or key==46 then -- + - * / .
		if self:is_trig() then
			b_key_used = self:do_fn( "click" )
		end
		if not b_key_used then
			local new_val
			local old_value = self:get_value()
			if type(old_value) == "boolean" then
				if		key==42 or key==43  then	new_val = self:get_max(id)
				elseif	key==45 or key==47  then	new_val = self:get_min(id)
				end
			else
				if		key == "-" then		new_val = old_value - 1
				elseif	key == "+" then		new_val = old_value + 1
				elseif	key == "/" then		new_val = old_value / 2
				elseif	key == "*" then		new_val = old_value * 2
				end
			end
			if new_val then
				self:print( "new value is "..new_val )
				b_key_used = true
				self:set_value_ui( new_val, id )
			end
		end
	end
	return b_key_used or oo.getsuper(BUTTON).do_key( self, key )
end
--]]

function BUTTON:update()
	--aaa.print_method()
	local menu = self:get_menu()
	if menu then
		menu:update()
	end
	oo.getsuper(BUTTON).update(self)
end

--[[
function BUTTON:draw_close()
	local w = self.__draw_text_line_width
	local b = self:get_bu_up():is_contact()
	if b then
		gol.color_green()
	else
		gol.color_cyan()
	end
	gol.set_line_width( w * (b and 2 or 1) )
	self:gol_draw_frame_rect()
end
]]--

function BUTTON:draw_tex()
	gol.color_white()
	--todo deal with menu case
	local tex_on, tex_off = self:get_tex_draw()
	aaa.draw_bind_rect( self:get_value_as_bool() and tex_on or tex_off,  -.5,-.5, .5,.5 )
	--maatex gol.set_texture_dim( 0 )
end

--todoclean with draw_text_on_top
function BUTTON:__draw_text( b_special, b_state )
	--if true then return end
	if GA.b_spy then aaa.spy.push_range( "text", 5 ) end

	local text
	--hack for fbo if self:is_menu() or self:is_multiple() then
--	if self:is_multiple() or self:is_multiple()then

	if self:is_text_selector() then
		--self:show( self:get_value(), "self:get_value" )
		local sel = self:get_selector()
		if sel then

			-- self:print( sel:get_value()	.." "..sel:get_item_text()
			-- 							.." "..sel:get_item_data()
			-- 							.." "..sel:get_item_symbo()
			-- 			)
			--table.print( sel, "selector", 3 )
			--todo delegate too ?
			--todoopt do faster
			text = sel:get_item_symbo() or sel:get_item_text() or sel:get_item_data()
		else
			text = self:get_text()
		end
	else
		text = self:get_text()
	end

	if text then
		--if text == "App"  then
			--self:print( "where" )
			--aaa.debug.print_traceback()
		--end

		-- Attribute
		if b_special then
			--BU:set_text_line_attr( b_state )
			self:draw_text_color_sel( not b_state )
		else
			--BU:set_text_line_attr()
			self:draw_text_color_sel()
		end
		-- Text
		local SY = .4
		--todo this should be commented at least
		--Draw text inside bu
		if self:is_text_inside() then
			local SX = .45
		--[[
			local rect = {}
			rect.l = -SX
			rect.r = SX
			rect.t = SY
			rect.b = -SY
		--]]
			self:draw_text_lrbt( text, -SX,SX, -SY,SY )
			--todo does not seem to function
			--self:draw_text_lrbt( text, -SX,SX, -SY,SY, self:get_text_align_x() )
		--Draw text at right of bu
		else
			--todo this should be commented at least
			-- but probably have to go
			--local su = self:get_text_factor() * math.min( 1, self:get_ratio_y() ) * .8
			local su = math.min( 1, self:get_ratio_y() ) * .8
			self:draw_text_lrbt( text, .75, .75 + su * self.__text_rect_ratio, -SY,SY, "left" )
		end

	end

	if GA.b_spy then aaa.spy.pop_range() end
end

function BUTTON:draw_mul()		aaa.draw_mul_line(		0,0,	.4,.4	)	end
function BUTTON:draw_disk()		aaa.draw_disk_axe_z(	0,0,0,	.8, 24	)	end
function BUTTON:draw_back()
	self:draw_back_bind_only()
end

function BUTTON:__draw_close()
	self:set_text_line_attr()
	gol.color_white()
	self:draw_mul()
end
function BUTTON:draw()
	--if true then return end
	if self.verbose >= 3 then aaa.print_method() end

	--if (self:is_trig() or self:is_menu()) and self:is_draw_min() then
	-- if self:is_draw_min() then
	-- 	return
	-- end

	if GA.b_spy then aaa.spy.push_range( "BUTTON", 3 ) end

	local b_special
	local b_state
	if self:is_tex_draw() then	--tex mean texture here
		self:draw_tex()
	else
		local b_menu			= self:is_menu()
		local b_multiple		= self:is_multiple()
		local b_trig			= self:is_trig()
		local b_menu_multiple	= b_menu or b_multiple

		local b_check
		local b_check_draw

		if b_multiple then
			local draw_by_value = self.__draw_by_value
			b_special = draw_by_value ~= nil
			if b_special then
				b_state = self:get_value() == draw_by_value.value
			end
		else
			if b_menu then
				local draw_by_value = self.__draw_by_value
				b_special = draw_by_value ~= nil
				if b_special then
					--self:print( self:get_selector():get_value()..","..self:get_value() .." -- ".. draw_by_value.value )
					b_state = self:get_value() == draw_by_value.value
				end
			else
			--todo why we do this test min/max
			--if (not b_menu and not b_multiple and not self:is_confirmation()) and self:get_value_as_bool() then
				if not self:is_confirmation() then
					if not b_trig then
						--gol.color_red()
						--gol.draw_line_strip_2d(	-.25, .25,	0, -.25, 1., .75 )
						b_special = self:is_text_inside()
						b_state = self:get_value_as_bool()
						-- if self:is_name( "shader" ) then
						-- 	self:show( b_state, "b_state" )
						-- end
					end
					--self:print( "Titi "..b_special )
					--b_check_draw = true
					if not b_special or not self:is_text_inside()  then
						b_check = true
						b_check_draw = b_state
					end
				end
			end
		end

		--gol.set_line_width( self:is_contact() and 3 or 2 )
		
		--if true then return end
		--gol.color_white( .5 )
		
		if b_special then
			self:__draw_back_by_value( b_state )
		elseif b_trig then
			self:gol_color_back()
			self:gol_draw_rect_plain()
		elseif b_menu_multiple then
			self:gol_color_back()
			self:gol_draw_rect_plain()
		else
			if self:is_frame_plus() then
				self:gol_color_back()
				aaa.draw_crosshair(	0,0, .5,.1 )
				--aaa.draw_plus_line(    0,   0, .5, .5 )
			else
				if b_check then
					--self:gol_color_back()
					--hack do it cleaner ,ore generic (by name ?)
					gol.color( 0,.4,.4, 1 )
					--gol.set_line_width( 2 )
					self:gol_draw_rect_line()
					--aaa.draw_rect_line_size()
				end
			end
		end

		if b_menu_multiple then
			gol.color_white( self:get_alpha_bu_to_draw() )
			gol.set_line_width(1)
			if b_multiple then
				--todo use contact when one	
				aaa.draw_line_y( .5, .2 )	--todo this is ok for inc/dec only
				local s = .1
				--gol.draw_line_strip_2d( -s,.4,	s,.4 )
				local x = -s+.05
				gol.draw_line_strip_2d( x,.25,	-s,.35, 	x,.45)
				x = -x
				gol.draw_line_strip_2d( x,.25,	s,.35, 	x,.45)
			else
				gol.draw_line_loop_2d( -.1,.5,	-.1,.4,	.1,.4,	.1,.5 )
			end
		else
			if b_check_draw then
				gol.color_white( self:get_alpha_bu_to_draw() )

				--hackMay
				--if b_trig or self:is_mobile() then
				if b_trig then
					b_special = false
					if self:is_check_mul() then
						self:draw_mul()
					elseif self:is_check_disk() then
						self:draw_disk()
					end
				else
					if self:is_check_mul() then
						self:draw_mul()
					elseif self:is_check_disk() then
						self:draw_disk()
					else
						gol.draw_line_strip_2d(	-.25, .25,	0, -.25, 1., .75 )
					end
				end
			end
		end
	end

	if GA.b_spy then aaa.spy.pop_range() end

	self.__b_button_special = b_special
	self.__b_button_state = b_state
end


function BUTTON:draw_fore()
	if self.verbose >= 3 then aaa.print_method() end

	if self:is_text_draw() then
		self:__draw_text( self.__b_button_special, self.__b_button_state )
	end

	BU.draw_fore( self )
end

function BUTTON:create( name, rect )
	local self = BUTTON:create_instance( name, rect )
	if self:get_ratio_x()<=1.000001 then
		self:set_text_inside( false )
	end
	--self:set_value_type_bool(true)

	self:set_color_back_named_no_error( name )
	return self
end

--todo check if these 2 fns are useful ?
--todo propagate list return strategy
function BUTTON.create_midi_vert( x_min,sx, y_min,sy, nb, ch, ctl )
	local tab = {}
	local f   = .8
	local y	  = y_min + sy * .5
	local dx  = sx / ( nb - 1 + f )
	local x	  = x_min + dx * .5 * f
	local bu
	for i=1,nb do
		bu = bus_cur:add_button( "Button MIDI vert "..(i-1), {x,y, dx*f,sy} )
		if ch then
			bu:set_midi( ch, ctl + i - 1 )
			bu:set_text_draw( i==1 or i==5 )
			bu:set_pos_load_save( false )
		end
		tab[i] = bu
		x = x + dx
	end
	return tab
end

function BUTTON.create_midi_hori( x_min, sx, y_min, sy, nb, ch, ctl )
	local tab = {}
	local f   = .8
	local x   = x_min + sx * .5
	local dy  = sy / ( nb - 1 + f )
	local y   = y_min + sy - dy * .5 * f
	local bu
	for i=1,nb do
		bu = bus_cur:add_button( "Button MIDI hori "..(i-1), {x,y, sx,dy*f} )
		if ch then
			bu:set_midi( ch, ctl + i - 1 )
			bu:set_text_draw( i==1 or i==5 )
			bu:set_pos_load_save( false )
		end
		tab[i] = bu
		y = y - dy
	end
	return tab
end

function BUTTON:get_uif_zones()
	local zones = {}

	oo.getsuper(BUTTON).add_uif_zones_base( self, zones, 0 )

	return zones
end	