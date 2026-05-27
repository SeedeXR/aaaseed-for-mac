

--todo for testing
--local	VER_PM			= 5 + nil

function meu:define_ui()
	local ref = self.ref
	local bu
	local par


	self:add_camera()

	local iy = 1
	local ix = 1
	local bu = self:add_button( {ix, iy, nil, nil },	"Draw", self:get_layer_ref_table(2).use_bdd )
		bu:set_min_max( 3, 2 )

	iy= iy + 1
	ix= ix + 1
	bu = self:add_trig(	{ix, iy }, "Restart", ref.restart_trig )

	iy= iy + 1
	bu = self:add_button( {ix, iy, nil, nil },	"Field", ref.field_active )

end

function meu:init()
	local ref = self.ref

	local lay2	=	self:get_layer( 2 )
	aaa.obj.update( lay2 )	-- update to create color object
						--todo make a get_obj_always to avoid the updating
	local col	=	aaa.obj.get_down_by_class( lay2, "color" )
	ref. alpha	=	param.get_ref( col, "global_alpha" )
	ref.bdd					=	self:get_layer_bdd( 2 )
	ref.restart_trig		=	param.get_ref( ref.bdd, "restart_trig" )
	ref.field_active		=	param.get_ref( ref.bdd, "field_active" )

end

---[[
function poid.hook_do_contact( a, b )
	--aaa.print( a.." - "..b )
	return 1.
end
--]]
--poid.hook_do_contact = nil

function meu:draw()
	local ref = self.ref
	param.set( ref.alpha, self:get_alpha() )
	MEU.draw( self )
end