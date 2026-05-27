function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_bu_texture_target_unit()

	self:add_mapping_by_side()
	self:add_rgbf(	{1,6.8,	8,1 })

	self:add_nb_uv( {1, 11} )

	self:add_camera()

	self:add_rendering()

	self:add_size_uvf_video( {9,9, 8 ,3})
	self:add_trz()

	self:add_monitor( {9,2.2} )
end

