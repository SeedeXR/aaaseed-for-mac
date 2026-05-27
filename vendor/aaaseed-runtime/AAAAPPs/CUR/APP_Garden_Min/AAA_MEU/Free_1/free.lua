function meu:define_ui()
	local ref = self.ref
	local bu
	local par


	self:add_bu_texture_target_unit()

	self:add_mapping_by_side_only()
	self:add_rgbf(	{1,6.8,	nil,1 })
	self:add_blending()

	self:add_camera()

	self:add_rendering()

	self:add_size_uvf_video()

	self:add_monitor( {9,2.2} )
end
