function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()

	local SY = 1
	local ix,iy = 1,1
	local SX = 8/3

	local sha	= self:get_shading()

	ix, iy = 1, 3
	self:add_trig(	{ix,		iy },		"ReLoad", sha, "reload_all", false )

	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Y", 	sha, "vu_float_01", 1, 0, 4 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Angle", 	sha, "vu_float_02", 0, -1, 1 ):set_meter( false )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Triangle Dist Thres", 	sha, "gu_float_01", 0.1, 0, 1)
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Camera Dist Thres", 	sha, "gu_float_02", 1.0, 1.0, 100.0)

	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Translation X", 	sha, "vu_01_r_x", 1, -8, 8 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Translation Y", 	sha, "vu_01_g_y", 1, -8, 8 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Translation Z", 	sha, "vu_01_b_z", 1, -8, 8 )

	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Rotation X", 	sha, "vu_02_r_x", 1, -8, 8 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Rotation Y", 	sha, "vu_02_g_y", 1, -8, 8 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Rotation Z", 	sha, "vu_02_b_z", 1, -8, 8 )
--[[
	iy = iy + SY
	bu = self:add_selector(	{ix,iy,	4,SY},		"Map" )
		bu:set_nb_min_0( 2, 1 )
		bu:set_target_param( param.get_ref( sha, "fu_int_01" ) )
		bu:set_item_text( 1, "Square" )
--	self:add_slider(	{ix+4,iy,	4,SY},		"Hardness", sha, "fu_float_01", 1, .5, 5. )
	self:add_slider_two(	{ix+4,iy,	4,SY},		"Hardness", sha, "fu_float_01", "fu_float_02", 0, 1, 0, 1. )
--]]
end

function meu:init()
	self:add_shading( 3 )
end


