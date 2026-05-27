function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_bu_texture_target_unit()

--	self:add_mapping_by_side_only()
--	self:add_rgbf(	{1,6.8,	8,1 })

--	self:add_nb_uv( {1, 11} )

	self:add_camera()

	self:add_rendering()
end

function meu:draw()
	--MEU.draw(self)
	self:draw_layers_begin()
		local nb = 3
		local si = 10.5

		self:draw_layer(2)
		for i=1,nb do

			gol.push_matrix()
				gol.translate_y( 3.4 )
				--gol.translate_x( ((i-.5)/nb-.5) * si )
				gol.translate_x( (i-2) * 3.5 )
				gol.rotate_x( -.125 )
				--gol.rotate_y( (i-2) * 0.125)
			self:draw_layer(1)
			gol.pop_matrix()

		end
	self:draw_layers_end()
end

