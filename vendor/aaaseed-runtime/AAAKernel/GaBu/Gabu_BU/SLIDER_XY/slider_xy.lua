if CLASS.DECLARE( "SLIDER_XY", BUI,	{	__b_pos_draw = true	} ) then
	SLIDER_XY:set_class_status_doc(	CLASS.STATUS.GABU,
									"a BUI for XY (2d) values"  )
end

function SLIDER_XY:set_relative( b )	self.__b_relative = b		end
function SLIDER_XY:is_relative()		return self.__b_relative	end

function SLIDER_XY:set_pos_draw( b )	self.__b_pos_draw = b		end
function SLIDER_XY:is_pos_draw()		return self.__b_pos_draw	end

function SLIDER_XY:u_to_x( u )	local d = self.__du		return u*(1-d) + d*.5 - .5		end
function SLIDER_XY:v_to_y( v )	local d = self.__dv		return v*(1-d) + d*.5 - .5		end
function SLIDER_XY:x_to_u( x )	local d = self.__du		return (x - d*.5 + .5) / (1-d)	end
function SLIDER_XY:y_to_v( y )	local d = self.__dv		return (y - d*.5 + .5) / (1-d)	end

function SLIDER_XY:create( text, rect )
	local self = SLIDER_XY:create_instance( text, rect )
	self.__b_undo_redo_using_values = true
	--self = oo.getsuper(SLIDER_XY):create( rect )

	self:set_method_on_click_double( self, "center" )
	self.__du = 1/16
	self.__dv = self.__du * rect[3]/rect[4]
	return self
end

function SLIDER_XY:set_slider_xy_target(	tab_or_ref, name1_or_ref, name2_or_ref, val1,val2, vmin1,vmax1, vmin2,vmax2 )
	if tab_or_ref then
		self:set_target( tab_or_ref, name1_or_ref, nil, 1 )
		self:set_target( tab_or_ref, name2_or_ref, nil, 2 )
	end
	self:set_min_max_value( vmin1, vmax1, val1, 1 )
	self:set_min_max_value( vmin2, vmax2, val2, 2 )
	return self
end

function SLIDER_XY:do_mouse_move( x,y )
	--aaa.print_method()
	oo.getsuper(SLIDER_XY).do_mouse_move( self, x,y )

	self:set_value_cano( self:x_to_u( x ) - self.__u_offset, 1 )
	self:set_value_cano( self:y_to_v( y ) - self.__v_offset, 2 )
end


function SLIDER_XY:do_click_down( x,y )
	--aaa.print_method()
	oo.getsuper(SLIDER_XY).do_click_down( self, x,y )

	local uc = self:get_value_cano_clamped(1)
	--local xc = self:x_to_u(uc)
	local duh = self.__du * .5
	local u = self:x_to_u(x)
	local b_inside_u = inside( u, uc-duh, uc+duh )
	self:print( "button x "..(b_inside_u and "inside" or "outside") )
	

	local vc = self:get_value_cano_clamped(2)
	--local yc = self:x_to_u(vc)
	local dvh = self.__dv * .5
	local v = self:y_to_v(y)
	local b_inside_v = inside( v, vc-dvh, vc+dvh )
	self:print( "button y "..(b_inside_v and "inside" or "outside") )
	

	if (b_inside_u and b_inside_v) or self.__b_relative then
		-- we don't move it
		self.__u_offset = u-uc
		self.__v_offset = v-vc
	else
		-- we center it on contact
		self:set_value_cano( u, 1 )
		self:set_value_cano( v, 2 )
		self.__u_offset = 0
		self.__v_offset = 0
	end

--	self:do_mouse_move( x, y )
end
-- function SLIDER_XY:do_click_up()
-- 	oo.getsuper(SLIDER_XY).do_click_up(self)
-- 	--self:print( "SLIDER_XY:do_click_up()" )
-- end

function SLIDER_XY:update()
	--aaa.print_method()
	oo.getsuper(SLIDER_XY).update(self)
end


--todo add set and get
function SLIDER_XY:center()
	self:set_value_cano( .5, 1 )
	self:set_value_cano( .5, 2 )
end

--todo check this
function SLIDER_XY:draw()
	local u,v = self:get_value_cano_clamped(1), self:get_value_cano_clamped(2)

	--	draw background
	self:gol_draw_rect_back()

	if self:is_pos_draw() then
		gol.color_white( self:get_alpha_bu_to_draw() * .5 )
		--  draw knob
		local x,y = self:u_to_x(u),self:v_to_y(v)
		local du,dv = self.__du*.5, self.__dv*.5
		aaa.draw_rect( x-du,y-dv, x+du,y+dv )
	end

	--TEXT
	if self:is_text_draw() then
		local u,v = self:get_value(1), self:get_value(2)
		local str = self:get_text()
		str = str.." "..self:format_value(u).." "..self:format_value(v)

		self:draw_text_color_sel()
		local SX = .45
		local SY = .4
		self:draw_text_lrbt( str, -SX,SX, -SY,SY )

		gol.set_texture_dim( 0 )
	end
end

function SLIDER_XY:get_uif_zones()
	local zones = {}
	BU.add_uif_zones_base( self, zones )
	return zones
end

