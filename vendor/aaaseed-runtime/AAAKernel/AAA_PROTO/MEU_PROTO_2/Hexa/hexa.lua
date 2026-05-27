function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "2d", "3D", "art", "Draw", "Core", "CoreGraphic", "Procedural", "texture", "VJ", "unfinished" },
			help = "Dispplay a Grid with the texture mapped in an hexagonal way"
		}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ix,iy = 1,1
	local DY = .2
	local SY = 1
	self:add_bu_texture_target_unit({ix,iy,	8,6}		)
	iy = iy + 6 + DY
	self:add_size_uvf(				{ix,iy, 	nil,SY}		)
	iy = iy + SY
	self:add_nb_uv(  				{ix,iy, 	nil,SY}		)
	iy = iy + SY + DY
	self:add_mapping_and_blending(	{ix,iy,		nil,SY*3}	)
	iy = iy + SY*3 + DY
	self:add_mapping_hexa( 			{ix,16-3,	8,3} 		)

	self:add_rgb(					{9,8,		8,SY}		)

	self:add_camera()
end

function meu:draw_icon()
	MEU.draw_icon( self )

	self:set_icon_color()
	gol.set_line_width( 2 )
	gol.set_texture_dim( 0 )
	gol.push_translate_scale_2d( -.35, 0, .3, 1.2 )
		aaa.draw_circle_axe_z( 0, 0, 0, 1.2, 6 )
	gol.pop_matrix()
end