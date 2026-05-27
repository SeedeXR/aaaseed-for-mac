
function meu:define_ui()
	local ref = self.ref
	local bu
	local par


	self:add_camera()

	local iy = 1
	local ix = 1
	local sy = 1
	local bu

	bu = self:add_button( {ix, iy, sy, sy },	"Draw", self:get_layer_ref_table(2).use_bdd )
		bu:set_min_max( 3, 2 )
	iy = iy + sy
	bu = self:add_button(	{	ix+1, iy, sy, sy },	"Draw_force", ref.b_draw_force )

	iy = iy + sy
	bu = self:add_trig(	{	ix, iy,	 sy, sy},		"Restart", ref.restart_trig )

	iy = iy + sy
	bu = self:add_button(	{	ix, iy, nil, sy },	"Field", ref.field_active )

	iy = iy + sy
	bu = self:add_button(	{	ix+1, iy, sy, sy },	"Doors", ref.dp_active )

	iy = iy + sy
	bu = self:add_button(	{	ix+1, iy, sy, sy },	"Floor", ref.floor_active )

end

function meu:init()
	local ref = self.ref

	ref.bdd					=	self:get_layer_bdd( 2 )
	ref.restart_trig		=	param.get_ref( ref.bdd, "restart_trig" )
	ref.field_active		=	param.get_ref( ref.bdd, "field_active" )
	ref.repulse_dist		=	param.get_ref( ref.bdd, "repulse_distance" )
	ref.b_steering			=	param.get_ref( ref.bdd, "steering_active" )
	ref.b_draw_force		=	param.get_ref( ref.bdd, "draw_force" )
	ref.repulse_alpha		=	param.get_ref( ref.bdd, "repulse_alpha" )
end

function meu:update()
--	u = interpolate( self.u, u, .1 )
--	self.u = u
--	self:set_time_factor( "A", u )
--	local sp = math.abs( u - self.u ) * 30
--	self:set_pan( "A", 0 )
--	self:set_volume( "A", sp)
--	self.u = u
end

--[[
function poid.hook_do_contact( a, b )
	--aaa.print( a.." - "..b )
	return 1.
end
--]]
--poid.hook_do_contact = nil
