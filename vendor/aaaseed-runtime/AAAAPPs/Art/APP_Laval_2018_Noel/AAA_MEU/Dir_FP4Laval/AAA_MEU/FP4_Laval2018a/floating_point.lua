

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
