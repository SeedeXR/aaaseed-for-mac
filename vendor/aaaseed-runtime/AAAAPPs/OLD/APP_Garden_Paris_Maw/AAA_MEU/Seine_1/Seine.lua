function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_mapping_by_side()
	self:add_bu_texture_target_unit()

	self:add_camera()


	local ix = 9
	local iy = 2
	self:add_slider(	{ix,iy,	8,1},	"Time Factor",			ref.seine, "param_01", 1, -1, 1 )
	self:add_slider(	{ix,iy+1,	8,1},	"cone deformation",		ref.seine, "param_02", 1, -4, 4 )
end

function meu:init()
	local ref = self.ref
	ref.seine = self:get_obj_down( "seine" )
end
