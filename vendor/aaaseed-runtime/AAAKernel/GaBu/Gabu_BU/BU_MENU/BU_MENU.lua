if CLASS.DECLARE( "BU_MENU", BUI, {
	__time_before_close = 4,
	ref = {},
	} ) then
	BU_MENU:set_class_status_doc(	CLASS.STATUS.GABU,
									"is a BUTTON associated Window encapsulating a SELECTOR" )
end


local release_time = 2

function BU_MENU:do_click_up( x, y )
	oo.getsuper(BU_MENU).do_click_up( self, x,y )
	
	local x_init, y_init = self.x_init, self.y_init
	local x, y = self:get_xy()

	if x ~= x_init or y ~= y_init then
		self.__b_lock = true
	end

end

function BU_MENU:get_button_sxy( bu )
	local x_left, y_bottom	= bu:convert_xy_local_to_top( -.5, -.5 )
	local x_right, y_top	= bu:convert_xy_local_to_top( .5, .5 )

	local sx = math.abs( x_left - x_right )
	local sy = math.abs( y_bottom - y_top )

	--self:print( "toto "..sx.." x "..sy )
	local max_sx = bu.__menu_max_sx
	if max_sx then
		sx = math.min( sx, max_sx )
	end
	return sx * (bu.__menu_fx or 1), sy * (bu.__menu_fy or 1)
end

function BU_MENU:flip_menu_display( b_call_auto )
	--self:print( "flip_menu_display" )
	local bu_window   = self.__bu_window
	local bu_selector = self:get_selector()
	local bu_text     = self.__bu_text

	if not self.b_menu_display then
		self.time_open = 0
		--self.__bu_button:set_value( self:get_value() )
	end

	local b_menu_display = not self.b_menu_display
	self.b_menu_display = b_menu_display

	--maanew
	self:set_active( b_menu_display )
	bu_window:set_active( b_menu_display )
	bu_window:get_bus_down():set_active( b_menu_display )

	bu_selector:update()

	if self.b_menu_display and not b_call_auto then
		bu_window.__b_lock = false

		local bu = self.__bu_button

		local x, y = bu:convert_xy_local_to_top( 0, 0 )

		local sx, sy = self:get_button_sxy( bu )

		local nb_u, nb_v = bu_selector:get_nb_uv()

		local sy_menu_global = sy * (nb_v + 1)
		local sx_menu_global = sx * nb_u

		local sx_menu = 1
		local sy_menu = sy_menu_global / sx_menu_global
		local sx_item = sx_menu / nb_u
		local sy_item = sy_menu / ( nb_v + 1 )

		--now we can set a coor system for the bus
		--		with 0,0 at the bottom left, a width of 1, and a height of the ratio
		--		this way it will be orthonormed (in english ?)
		local bus = bu_selector:get_bus_up()
		bus:set_transfo( sx_menu,sy_menu, sx_menu/2,sy_menu/2 )

		-- and then compute the rest
		-- selector
		local sy_selector = sy_item * nb_v
		local sx_selector = sx_menu
		bu_selector:set_xy( 	sx_selector * .5,		sy_selector * .5	)
		bu_selector:set_sxy( 	sx_selector, 			sy_selector			)

		bu_text:set_xy(			sx_selector * .8 * .5,	sy_selector + sy_item *.5 )
		bu_text:set_sxy(		sx_selector * .8,		sy_item )

		--	close box
		local s_close = math.min( sy_item, sx_menu )
		bu = self.__bu_close
		bu:set_xy( 	sx_menu - s_close * .5,	sy_menu - s_close * .5	)
		s_close = s_close * .8
		bu:set_sxy( 	s_close, 		s_close			)

		-- place the window
		local osx, osy = ga.cam:get_ortho_max_size()
		osx, osy = osx*.5, osy*.5

		if osy < y + sy_menu_global then
			y = y - sy_menu_global*.5 - sy*.5
		else
			y = y + sy_menu_global*.5 + sy*.5
		end

		if x - sx_menu_global * .5 < -osx then
			x = x + sx_menu_global * .5
		elseif osx < x + sx_menu_global * .5 then
			x = x - sx_menu_global * .5
		end

		bu_window:set_xy( x, y )
		bu_window:set_sxy( sx_menu_global, sy_menu_global )
		bu_window.x_init, bu_window.y_init = x, y
	end
end

function BU_MENU:define_selector_menu( tab_name_or_selector )

	local bus = BUS:create( "menu" )

	--bus:set_active( false )
	bus:init_begin()

		-- selector
		local bu_sel
		if CLASS.is_gabu_obj( tab_name_or_selector, SELECTOR ) then
			bu_sel = bus:add_bu( tab_name_or_selector )	-- use the passed selector
		else
			bu_sel = bus:add_selector( "menu choose",	{1,1,	2,2}  )	-- create a selector from table
				bu_sel:set_text_draw( false )
				local nb = #tab_name_or_selector
				bu_sel:set_nb_min_0( 1, nb )
				bu_sel:set_item_text( 1, unpack(tab_name_or_selector) )
		end
		self:set_selector( bu_sel )
		self:set_target_bu( bu_sel )

		-- close
		local bu
		bu = bus:add_trig( "close", {2,2, .2,.2} ):set_text_visible(false)
			bu.draw = BUTTON.__draw_close
			bu:set_method_on_click( self, "flip_menu_display", true )
			self.__bu_close = bu

		-- text
		local sy = .2 * .25
		local sx = .8
		bu = bus:add_text( self:get_name(), {0,0, 1,1} )
			bu:set_ui_active(false)
			self.__bu_text = bu

	bus:init_end()

	return bus
end

function BU_MENU:init( bu_button, tab_name_or_selector )
	self.__bu_button = bu_button

	self.b_menu_display = false
	self.time_open = 0

	local bus = BUS:create( self:get_name().."_Menu" )

	bus:init_begin()

		local bu = bus:add_window( "menu", self:define_selector_menu( tab_name_or_selector ),	{0,0, 1,1} )
		bu:set_active( false )

		bu:set_text_draw( false )
		bu:set_method_on_click_double( self, "no_click_double" )
		bu.do_click_up = BU_MENU.do_click_up
		bu.__b_lock = false

	bus:init_end()
	bus:set_active( false )

	ga:add_bus_top( bus )
--	bu:set_dplane(-42)	--don't solve buss order

	self.__bu_window = bu
	self.__bus_menu = bus
end


function BU_MENU:create( name, bu_button, tab_name_or_selector )
	local self = BU_MENU:create_instance( name )
	self:init( bu_button, tab_name_or_selector )
	return self
end

function BU_MENU:set_active( b_on )
	--self:print( "tututu "..b_on )
	--self.__bu_window:set_active( b_on )
	self.__bus_menu:set_active( b_on )
	oo.getsuper(BU_MENU).set_active( self, b_on )
end

function BU_MENU:no_click_double()
	self:print( "no_click_double fn for this window" )
end

function BU_MENU:free()
	ga:remove_bus( self.__bus_menu )
	self.__bus_menu:free()
	oo.getsuper(BU_MENU).free(self)
end


function BU_MENU:set_nb( nb_u, nb_v )
	--aaa.debug.print_traceback( "BU_MENU:set_nb" )
	local bu = self:get_selector()
	local _,v = bu:get_nb_uv()

	if nb_u == nil then
		nb_u = 1
	end

	if nb_v == nil then
		nb_v = math.floor( v/nb_u )
		local mod = v%nb_u
		if mod > 0 then
			nb_v = nb_v + 1
		end
	end

	bu:set_nb( nb_u, nb_v )
	return self
end

function BU_MENU:set_nb_min_0( nb_u, nb_v )
	local bu = self:get_selector()
	local _,v = bu:get_nb_uv()

	if nb_u == nil then
		nb_u = 1
	end

	if nb_v == nil then
		nb_v = math.floor( v/nb_u )
		local mod = v%nb_u
		if mod > 0 then
			nb_v = nb_v + 1
		end
	end

	bu:set_nb_min_0( nb_u, nb_v )
	return self
end
function BU_MENU:set_horizontal( nb_v )	self:set_nb( nil, nb_v )	end
function BU_MENU:set_vertical( nb_u )	self:set_nb( nb_u, nil )	end


function BU_MENU:__update_presence()
	--self:print( "__update_presence()" )

	local bu_window = self.__bu_window
	if not bu_window.__b_lock then
		if self.b_menu_display then
			--self:print( "BU_MENU:update()" )
			if bu_window:get_contact_nb_down() > 0 or aaa.keyboard.is_alt() then
				self.time_open = 0
			else
				-- Get mouse position
				local mouse_x,mouse_y = aaa.mouse.get_xy_render()
				--self:show( mouse_x, "mouse_render_x" )
				-- Convert mouse position to the main coordinate
				mouse_x,mouse_y = EVENT.transform_move_coor( mouse_x,mouse_y )
				--self:show( mouse_x, "mouse_x" )
				if bu_window:is_inside( mouse_x,mouse_y ) then
					self.time_open = 0
				--todo deal in multitouch with over mode when it exist
				else	
					if self.time_open then
						local time_living = self.time_open + aaa.time.dt
						--self:print( self.." "..current_time_living )
						if time_living >= self.__time_before_close then
							self:flip_menu_display( true )
							self.time_open = nil
						else
							self.time_open = time_living
						end
					end
				end
			end
		end
	end
end

function BU_MENU:draw()
	--self:print( "draw() ")
end


