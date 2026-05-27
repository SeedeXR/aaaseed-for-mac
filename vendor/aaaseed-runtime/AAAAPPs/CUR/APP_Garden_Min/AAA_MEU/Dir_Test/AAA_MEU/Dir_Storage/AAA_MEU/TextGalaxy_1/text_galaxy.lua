function meu:define_ui()
	self:define_ui_oclgl_galaxy( self )

	local ref = self.ref
	local par

	local bu, ix, iy
	local ocl_obj = self:get_opencl():get_obj()

	local ix = 9
	local iy = 6
	self:add_slider(	{ix,iy+1,	8,1},	"Time",					ocl_obj, "param_05", 1, -4, 4 )
	self:add_slider(	{ix,iy+2,	8,1},	"dt",					ocl_obj, "param_06", 1, 0, 1 )
--	self:add_slider(	{ix,iy+2,	8,1},	"cone deformation",		ref.galaxy, "param_03", 1, -4, 4 )
end

function meu:init()		self:ocl_init()	end
function meu:update()
	self:update_oclgl_galaxy()
end
