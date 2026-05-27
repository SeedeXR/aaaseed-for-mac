
function meu:define_ui()
	local ref = self.ref
	local bu
	local par


	self:add_camera()

	local iy = 1
	local ix = 1
	local sy = 1

	iy = 3
	local bu = self:add_slider(	{ix,iy,	8,sy},		"Coverage", ref.coverage, nil, 0, 0, 30 )
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	8,sy},		"Center u", ref.center_u, nil, 0, 0, 1 )
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	8,sy},		"Center v", ref.center_v, nil, 0, 0, 1 )
	-- local bu = self:add_button( {ix, iy, sy, sy },		"Draw", self:get_layer_ref_table(2).use_bdd )
	-- 	bu:set_min_max( 3, 2 )
	-- iy = iy + sy
	-- bu = self:add_button(	{	ix+1, iy, sy, sy },	"Draw_force", ref.b_draw_force )

	-- iy = iy + sy
	-- bu = self:add_trig(	{		ix, iy,	 sy, sy},		"Restart", ref.restart_trig )

	-- iy = iy + sy
	-- bu = self:add_button(	{	ix, iy, nil, sy },	"Field", ref.field_active )

	-- iy = iy + sy
	-- bu = self:add_button(	{	ix+1, iy, sy, sy },	"Doors", ref.dp_active )

	-- iy = iy + sy
	-- bu = self:add_button(	{	ix+1, iy, sy, sy },	"Floor", ref.floor_active )

end

function meu:init()
	local ref = self.ref

	ref.bdd					=	self:get_layer_bdd( 1 )
	ref.coverage			=	param.get_ref( ref.bdd, "out_coverage" )
	ref.center_u			=	param.get_ref( ref.bdd, "out_center_u" )
	ref.center_v			=	param.get_ref( ref.bdd, "out_center_v" )
	-- ref.field_active		=	param.get_ref( ref.bdd, "field_active" )
	-- ref.repulse_dist		=	param.get_ref( ref.bdd, "repulse_distance" )
	-- ref.b_steering			=	param.get_ref( ref.bdd, "steering_active" )
	-- ref.b_draw_force		=	param.get_ref( ref.bdd, "draw_force" )

	-- local lay = self:get_layer(2)

	-- local dp = aaa.obj.get_down( lay, "Doors" )
	-- ref.def_dp = dp
	-- ref.dp_active = param.get_ref( dp, "active" )

	-- local floor = aaa.obj.get_down( lay, "Floor" )
	-- ref.def_floor = floor
	-- ref.floor_active = param.get_ref( floor, "active" )
end


function meu:update()
	-- local ref = self.ref
	-- local b_doors = param.get_bool( ref.dp_active )
	-- param.set( ref.repulse_dist,  b_doors and .0002 or .042	 )
	-- param.set( ref.b_steering,  not b_doors )
end
--[[
function poid.hook_do_contact( a, b )
	--aaa.print( a.." - "..b )
	return 1.
end
--]]
--poid.hook_do_contact = nil
