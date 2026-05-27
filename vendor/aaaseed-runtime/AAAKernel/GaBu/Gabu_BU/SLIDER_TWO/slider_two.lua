if CLASS.DECLARE( "SLIDER_TWO", SLIDER, {
	__b_meter_cross  = false,
	__str_slider_two_draw_step = "range"
	} ) then
	SLIDER_TWO:set_class_status_doc(	CLASS.STATUS.GABU,
										"a SLIDER for two values"  )
end

function SLIDER_TWO:get_value_cano_clamped(id)
	local super = oo.getsuper(SLIDER_TWO)
	if id == 3 then
		return (super.get_value_cano_clamped(self,2) - super.get_value_cano_clamped(self,1)) / 2
	end
	return super.get_value_cano_clamped(self,id)
end
function SLIDER_TWO:get_value_cano(id)
	local super = oo.getsuper(SLIDER_TWO)
	if id == 3 then
		return (super.get_value_cano(self,2) - super.get_value_cano(self,1)) / 2
	end
	return super.get_value_cano(self,id)
end
function SLIDER_TWO:get_value(id)
	local super = oo.getsuper(SLIDER_TWO)
	if id == 3 then
		return (super.get_value(self,2) - super.get_value(self,1)) / 2
	end
	return super.get_value(self,id)
end

function SLIDER_TWO:swap_values()
	self:print_debug( "SLIDER_TWO:swap_values()" )
	local super = oo.getsuper(SLIDER_TWO)
	local v1 = super.get_value_cano( self, 1 )
	local v2 = super.get_value_cano( self, 2 )
	self:print( v1.." <---> "..v2 )
	super.set_value( self, v2, 1 )
	super.set_value( self, v1, 2 )
end
function SLIDER_TWO:swap_values_ui()
	self:swap_values()
	local t = self:make_table_to_call_method( self, "swap_values" )
	--local redo = self:make_table_to_call_method( self, "swap_values" )
	ga:add_undo_redo( t, t )
end

function SLIDER_TWO:set_value_cano( val, id )
	local super = oo.getsuper(SLIDER_TWO)
	if id == 3 then
		local v1,v2 = super.get_value_cano(self,1), super.get_value_cano(self,2)
		local dvh = (v2 - v1) / 2
		super.set_value_cano( self, val - dvh, 1 ) 
		super.set_value_cano( self, val + dvh, 2 )
		return
	else
		super.set_value_cano( self, val, id )
	end
end
function SLIDER_TWO:set_value( val, id )
	local super = oo.getsuper(SLIDER_TWO)
	if id == 3 then
		local v1,v2 = super.get_value(self,1), super.get_value(self,2)
		local dvh = (v2 - v1) / 2
		super.set_value( self, val - dvh, 1 ) 
		super.set_value( self, val + dvh, 2 )
	else
		super.set_value( self, val, id )
	end
end
function SLIDER_TWO:set_value_ui( val, id )
	local super = oo.getsuper(SLIDER_TWO)
	if id == 3 then
		local v1,v2 = super.get_value(self,1), super.get_value(self,2)
		local dvh = (v2 - v1) / 2
		--todo regroup so it is one undo
		super.set_value_ui( self, val - dvh, 1 ) 
		super.set_value_ui( self, val + dvh, 2 )
	else
		super.set_value_ui( self, val, id )
	end
end
function SLIDER_TWO:add_value_ui( inc, id )
	local super = oo.getsuper(SLIDER_TWO)
	if id == 3 then
		local val = self:get_value( id )
		self:set_value_ui( val + inc, id )
	else
		super.add_value_ui( self, inc, id )
	end
end

function SLIDER_TWO:set_value_type_integer( b )
	self:__get_balue(1):set_value_type_integer(b)
	self:__get_balue(2):set_value_type_integer(b)
	return self
end

function SLIDER_TWO:set_min_max( min, max, id )
	--hack
	if id then
		BUI.set_min_max( self, min, max, id )
	else
		BUI.set_min_max( self, min, max, 1 )
		BUI.set_min_max( self, min, max, 2 )
	end
	self:__add_value_def_from_min_max( min, max )
	return self
end

function SLIDER_TWO:set_min_max_value( min, max, value, id )
	--hack
	if id then
		BUI.set_min_max_value( self, min, max, value, id )
		self:__add_value_def_from_min_max( min, max, value )
	else
		self:box_debug( "pb" )
	end
	return self
end
function SLIDER_TWO:set_slider_two_target(		tab_or_ref, name1_or_ref, name2_or_ref, val1, val2, vmin, vmax )
	self:set_min_max( vmin, vmax )
	if tab_or_ref or name1_or_ref then
		self:set_target( tab_or_ref, name1_or_ref, nil, 1 )
	end
	if tab_or_ref or name2_or_ref then
		self:set_target( tab_or_ref, name2_or_ref, nil, 2 )
	end
	if val1 then
		self:set_value( val1, 1 )
		self:add_values_def( val1 )
		self:set_def( val1, 1 )
	end
	if val2 then
		self:set_value( val2, 2 ) 
		self:add_values_def( val2 )
		self:set_def( val2, 2 )
	end
	return self
end

function SLIDER_TWO:set_meter_cross( b )	self.__b_meter_cross = b	end
function SLIDER_TWO:is_meter_cross()		return self.__b_meter_cross	end
function SLIDER_TWO:set_central( b )		self.__b_central = b 		end



-- UIF
--
function SLIDER_TWO:add_uif_zones_base( zones, dist, x,y )
	zones.Swap		=	{	angle=28.5/32,		dangle=5/32,	dist=1.3,	dy=.05,	}
	oo.getsuper(SLIDER_TWO).add_uif_zones_base( self, zones, dist, x,y )
	return zones
end
function SLIDER_TWO:do_uif_command( uif )
	aaa.print_fn()
	--table.print( uif, "uif" )
	local command = BU:get_uif_name_sel( uif )
	local b_used = true
	-- already done on the fly and on the right value when multiple values (SLIDER_TWO)
	if	command=="swap"	then	self:swap_values_ui()
								uif.b_undo_done = true
	else
		b_used = false 
	end
	return GABU_OBJ.do_uif_command_with_super( self, b_used, uif, SLIDER_TWO )
end

function SLIDER_TWO:do_mouse_move( x,y )
	local id = self:get_id_interacting()
--	if id==3 then
--		self:__do_mouse_move_slider_base( x, y, 3 )
--	else
		self:__do_mouse_move_slider_base( x,y, id )
--	end
end

function SLIDER_TWO:do_click_down( x,y )
	local val1 = self:get_value_cano_clamped(1)
	local val2 = self:get_value_cano_clamped(2)

	local x_l = val1 - .5
	local x_r = val2 - .5

	local id
	if self.__b_central then	--todo revive this
		if		x < x_l	then	id = 1
		elseif	x_r < x	then	id = 2
		else 					id = 3
		end
	else
		if math.abs( x_r - x ) < math.abs( x_l - x ) then	id = 2
		else												id = 1
		end
	end
	self:set_id_interacting( id )
	--local value = id==3 and (val[1]+val[2])/2 or val[id]
	self:__do_click_down_slider_base( x,y, id )	--, id==3 and 1 or id )
end


--function SLIDER_TWO:do_click_up()	oo.getsuper(SLIDER_TWO).do_click_up( self )	end
--function SLIDER_TWO:update()		oo.getsuper(SLIDER_TWO).update(self)		end

function SLIDER_TWO:__get_lr_ref_from_val_cano( val_l, val_r )
	local l, r

	l = -.5 + clamp_01(val_l)
	if val_r == nil then 	r = l
	else 					r = -.5 + clamp_01(val_r)
	end
	--aaa.show( val_l.." "..l, self.."l" )
	--aaa.show( val_r.." "..r, self.."r" )
	return l, r
end

SLIDER_TWO.doc.set_draw_step = { "( str_how ) str_how is a string defining how the slider is drawn,",
									" can be range, linear, linear_inverse, step_two." }
function SLIDER_TWO:set_draw_step( str_how )
	self.__str_slider_two_draw_step = str_how
	return self
end

function SLIDER_TWO:draw()
	-- tghis do not work and should eventually redone
	-- local val
	-- local id = self:get_id_interacting()
	-- if id==3 then	val = ( self:get_value( 1 ) + self:get_value( 2 ) ) * .5
	-- else			val = self:get_value( id )
	-- end
	-- self:__draw_slider( val )
	self:__draw_slider()
end

function SLIDER_TWO:create( name, rect )
	local self = SLIDER_TWO:create_instance( name, rect )
	self:set_meter( true )
	self:set_method_on_click_double( self, "do_click_double" )
	return self
end

