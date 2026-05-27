function meu:define_ui()
	local ref = self.ref
	local bu

	self:add_bu_texture_target_unit()

	self:add_mapping_by_side_only()
	self:add_rgbf(	{1,8,	8,1 })
	self:add_blending( {1,5.8} )

	self:add_camera()

	self:add_size_uvf_video()

	self:add_monitor( {9,2.5} )
end

function meu:draw()
	self:draw_layers_begin()

		self:draw_layer( 1 )

		local bind = app.logo.bind
		local lsx, lsy = aaa.img.get_size( bind )
		if lsx then

			local f4 = self:get_meu_by_name( "Ref_Fbo_F4" )
			local sx, sy = f4:get_pixel_size()
			--aaa.print_method()

			local t = self.ui.__t_bu_size_uv
			local su = t.su:get_value()
			local sv = t.sv:get_value()
			
			local x_min = sx * .75
			local x_max = sx
			local y_min = 0
			local y_max = sx * .25 / lsx * sy

			local v_max = sv * ( -.5 + y_max / sy)

			gol.set_texture_dim( 2 )
			gol.bind_texture( bind )
			gol.set_quad_uv()
			aaa.draw_rect_uv(su * .25, - sv * .5, su * .5, v_max)
		end

	self:draw_layers_end()
end