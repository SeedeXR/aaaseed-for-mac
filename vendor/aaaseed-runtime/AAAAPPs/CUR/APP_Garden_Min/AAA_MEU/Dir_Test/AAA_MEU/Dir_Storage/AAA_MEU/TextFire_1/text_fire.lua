function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	--self:add_mapping_by_side()

	self:add_camera()

	bu = self:add_trig_fn(	{1, 1},		"focus",	aaa.obj.set_focus_ui, ref.galaxy )

	local ix = 9
	local iy = 2
	self:add_trig(	{ ix, iy }, 			"Restart",				ref.galaxy, "restart_trig", false )

	self:add_slider(	{ix,iy+1,	8,1},	"Time",					ref.galaxy, "param_01", 1, -4, 4 )
	self:add_slider(	{ix,iy+2,	8,1},	"Speed",				ref.galaxy, "param_02", 1, -4, 4 )
--	self:add_slider(	{ix,iy+2,	8,1},	"cone deformation",		ref.galaxy, "param_03", 1, -4, 4 )
end

function meu:init()
	local ref = self.ref
	ref.galaxy = self:get_obj_down( "__MEU_BDD_OCL" )
end
