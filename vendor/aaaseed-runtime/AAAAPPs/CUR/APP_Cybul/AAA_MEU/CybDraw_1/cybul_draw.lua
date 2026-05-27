
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	self:add_shading_ui({ix,iy,	8} )
	ix = ix 
	iy = iy + SY + DY
	
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY}, "Axe Line Size", self, "line_axe_width",	1,	0,32	)

	iy = 4
	SY = 2.5

	self:add_bu_texture_target_unit( {9,iy, nil,SY}, "Src", 1	)
	iy = iy + SY

end

function meu:init()
	local ref = self.ref
	self.layer_attr = 1
	self.layer_shading = 5
	self.layer_attr_grid = 7
	local sha = self:add_shading( self.layer_shading )
	sha:set_save_frag_float( false, 1,8 )

	self.use_cache = true
end

function meu:set_frag_floats( A, B, C, D )
	local sha = self:get_shading()

	sha:set_frag_float_1_8(		A[1],	A[2],
								B[1],	B[2],
								C[1],	C[2],
								D[1],	D[2]
							)

	gol.update_uniform_fragment_float()
end

function meu:draw_ext_scene()
	local t = app.tab3d
	if t.bind and app.scene.wall_width and t.ext_wall_lt then
		local sx, sy
		local A = t.ext_wall_lt
		local B = t.ext_wall_rt
		local C = t.ext_wall_lh
		local D = t.ext_wall_rh
		local E = t.ext_floor_bl
		local F = t.ext_floor_br

		local a, b, c, d
		local from_x, to_x, from_y, to_y, from_z, to_z

		-- gol.set_wrap_2d_border()

		gol.set_texture_dim( 2 )
		gol.bind_texture( 65 ) -- F2 tex
		
		local scene_center_x = app.scene.scene_center_x
		-- wall
		sx = app.scene.ext_wall_width
		sy = app.scene.ext_wall_height
		-- print( sx, sy )


		self:set_frag_floats( A, B, C, D )
		gol.normal3( 0, 0, 1 )
		gol.set_quad_uv() -- 0-1 will be used to interpolate correct uv location
		aaa.draw_rect_uv( -sx/2 + scene_center_x, 0, sx/2 + scene_center_x, sy )

		self:set_frag_floats( C, D, E, F )
		
		sx = app.scene.ext_wall_width
		sy = app.scene.ext_floor_depth
		-- print( sx, sy )

		gol.normal3( 0, 1, 0 )
		gol.set_quad_uv() -- 0-1 will be used to interpolate correct uv location
		aaa.draw_triangle_strip_4xyz_uv( -sx/2 + scene_center_x, 0, 0,
		-sx/2 + scene_center_x, 0, sy,
		sx/2 + scene_center_x, 0, 0,
		sx/2 + scene_center_x, 0, sy )
	end
end

function meu:draw_wall()
	local t = app.tab3d
	if t.bind and app.scene.wall_width and t.ext_wall_lt then
		local pts = t.points
		local A = pts[1]
		local B = pts[2]
		local C = pts[3]
		local D = pts[4]

		-- gol.set_wrap_2d_border()

		gol.set_texture_dim( 2 )
		gol.bind_texture( 65 ) -- F2 tex
		
		local scene_center_x = app.scene.scene_center_x
		-- wall
		local sx = app.scene.wall_width  * 0.001
		local sy = app.scene.wall_height * 0.001

		self:set_frag_floats( A, B, C, D )
		gol.normal3( 0, 0, 1 )
		gol.set_quad_uv() -- 0-1 will be used to interpolate correct uv location
		-- aaa.draw_rect_uv_at_z( -sx/2, 	0, sx/2, sy, 0 )
		aaa.draw_triangle_strip_4xyz_uv(	-sx/2,  0,  0.1,
								-sx/2,  sy, 0.1,
							     sx/2,  0,  0.1,
							     sx/2,  sy, 0.1 )
		-- aaa.draw_triangle_strip_4xyz_uv(	-sx/2,  0,  -0.1,
		-- 						-sx/2,  sy, -0.1,
		-- 						 sx/2,  0,  -0.1,
		-- 						 sx/2,  sy, -0.1 )	
 
		
		sx = app.scene.ext_wall_width
		sy = app.scene.ext_floor_depth

		-- gol.normal3( 0, 1, 0 )
		-- gol.set_quad_uv() -- 0-1 will be used to interpolate correct uv location
		-- aaa.draw_triangle_strip_4xyz_uv( -sx/2 + scene_center_x, 0, 0,
		-- 			     	  -sx/2 + scene_center_x, 0, sy,
		-- 			     	   sx/2 + scene_center_x, 0, 0,
		-- 			     	   sx/2 + scene_center_x, 0, sy )

	end
end

function meu:draw_house()
	self:draw_layer( self.layer_shading )
	if ga:get_pass_info().name ~= "shadow" then
		self:draw_ext_scene()
	else
		self:draw_wall()
	end
end

function meu:draw_axe()
	-- grid
	if ga:get_pass_info().name ~= "shadow" then

		gol.set_line_smooth( true )

		gol.set_line_width( self.line_axe_width )
		local center_x = app.scene.scene_center_x
		
		local height = app.scene.ext_wall_height
		local depth  = app.scene.ext_floor_depth
		local width  = app.scene.ext_wall_width
		local half_width = width * .5
		local i
		local steps = app.tab3d.grid_steps * 0.001

		app:set_grid_color()

		-- y
		-- gol.color3( 0, 0.5, 0 )
		i = 0
		while i < half_width do
			gol.draw_lines_3d( -i + center_x, 0, .01, -i + center_x, height, .01 )
			gol.draw_lines_3d( i + center_x, 0, .01,  i + center_x, height, .01 )
			i = i + steps
		end

		-- z
		-- gol.color3( 0, 0, 0.5 )
		i = 0
		while i < half_width do
			gol.draw_lines_3d( -i + center_x, .01, 0, -i + center_x, .01, depth )
			gol.draw_lines_3d( i + center_x, .01, 0,  i + center_x, .01, depth )
			i = i + steps
		end

		-- gol.color3( 0.5, 0, 0 )
		-- x wall
		i = 0
		while i < app.scene.ext_wall_height do 
			gol.draw_lines_3d( -half_width + center_x, i, .01, half_width + center_x, i, .01 )
			i = i + steps
		end
		-- x floor
		i = 0
		while i < app.scene.ext_floor_depth do 
			gol.draw_lines_3d( -half_width + center_x, .01, i, half_width + center_x, .01, i )
			i = i + steps
		end
	end
end

function meu:draw()
	gol.reset() -- hack: avoid

	gol.set_wrap_2d_edge()
	gol.set_texture_dim( 0 )

 	self:draw_layers_begin()
 		self:draw_layer( self.layer_attr )

		-- self:draw_layer( 2 )	-- Cube
		-- self:draw_layer( 3 )	-- Cube

		self:draw_house()

		if app.tab3d.show_grid and app.draw_axe_in_stage then
			self:draw_layer( self.layer_attr_grid )
			self:draw_axe()
			self:draw_layer( self.layer_attr )
		end

		if app.tab3d.show_store then
			if app.tab3d.store_type == "Pergola Mur" then
				PERGOLA_MUR:draw()
			elseif app.tab3d.store_type == "Pergola Sol" then
				PERGOLA:draw()
			elseif app.tab3d.store_type == "Banne" then
				BANNE:draw()
			end
		end


	self:draw_layers_end()

	gol.reset() -- hack: avoid
end
