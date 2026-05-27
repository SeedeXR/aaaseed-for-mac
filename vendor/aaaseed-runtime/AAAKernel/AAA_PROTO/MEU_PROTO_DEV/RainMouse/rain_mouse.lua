function meu:define_meu_infos( )
	return { 	author = "Mâa ",
				tags = { "art", "draw", "procedural", "3d", "experimental", "unfinished", "input", "tutorial", "point" },
				help =	{	"a box od particle influence by the mouse position.",
							"Done with OpenCL."
						}
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu

	local ix,iy = 1,1
	local SY = 1
	local DY = .2

	local ocl = self:get_opencl()

	self:add_camera()
	self:add_rendering()

	self:add_opencl_base()

	iy = 3
	self:add_oclgl_slider_ocl_param_target(	{ix,iy,	8,SY},	"PullBack Inflence",	ocl:get_param_ref(1),	.1,	0,1 )
--	iy = iy + SY
--	self:add_oclgl_slider_ocl_param_target(	{ix,iy,	8,SY},	"Mouse Distance",		ocl:get_param_ref(3),	1,	0,4. )
	iy = iy + SY 
	self:add_oclgl_slider_ocl_param_target(	{ix,iy,	8,SY},	"Mouse Inflence",		ocl:get_param_ref(2),	.2,	-1,1 )
	iy = iy + SY + DY
	self:add_oclgl_slider_ocl_param_target(	{ix,iy,	8,SY},	"Speed Vertical",		ocl:get_param_ref(7),	0,	-1,1 )
	iy = iy + SY + DY
	-- bu = self:add_trig(	{ix, iy }, "Restart", ref.restart_trig )
	-- iy = iy + 1

	ix,iy = 9,5
	self:add_bu_texture_target_layer(	{ix,iy,	8,6},	"Tex",		1, true,		self:get_layer(1), self:get_layer_mapping(1) )


end

function meu:init()
	local ref = self.ref
	self:ocl_init()
--	ref.bdd = self:get_layer_bdd(1) 
--	ref.restart_trig = param.get_ref( ref.bdd, "restart_trig" )

end
-- function meu:update()
-- 	local ref = self.ref
-- 	local sha = self:get_shading()
-- end
