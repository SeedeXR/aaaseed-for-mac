
if CLASS.DECLARE( "BANNE" ) then
	BANNE:set_class_status_doc(	CLASS.STATUS.APP,
								"BANNE is one of the product" )
end


function BANNE:draw_hessian()
	local cybstores = app:get_cybstores_meu()

	cybstores:set_shader( "hessian" )
	cybstores:draw_layer( 13 )	-- Container for store ceiling
	cybstores:set_gl_parameters( "hessian" )
	
	local store_layer = cybstores:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )
	local center_x = param.get( store_layer, "center_x" )
	local width = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" )
	local depth = param.get( store_model, "size_axe" ) * app.store.hessian_opening

	local angle = app.store.hessian_angle / 180 * math.pi
	local cos_angle = math.cos( angle )
	local sin_angle = math.sin( angle )
	local dy = math.sin( angle / 180 * math.pi )

	local min_x = center_x - width / 2
	local max_x = center_x + width / 2
	local max_y = height
	local min_y = height - depth * math.sin( angle )
	local min_z = .2
	local max_z = min_z + depth * math.cos( angle )


	gol.normal3( 0, cos_angle, sin_angle )

	gol.set_quad_uv( 0, 0, (max_x - min_x) / app.hessian.width, depth / app.hessian.height )
	aaa.draw_triangle_strip_4xyz_uv( 	min_x, max_y, min_z,
							min_x, min_y, max_z,
							max_x, max_y, min_z,
							max_x, min_y, max_z)
	gol.normal3( 0, -cos_angle, -sin_angle )
	gol.set_quad_uv( -(max_x - min_x) / app.hessian.width, -depth / app.hessian.height, 0, 0 )
	aaa.draw_triangle_strip_4xyz_uv( 	min_x, min_y, max_z,
							min_x, max_y, min_z,
							max_x, min_y, max_z,
							max_x, max_y, min_z)

	gol.set_texture_dim( 0 )
end

function BANNE:draw_zip()
	local zip_des = app.zips.length * app.zips.opening
	local opening = app.store.hessian_opening
	if zip_des and zip_des > 0 then
		local cybstores = app:get_cybstores_meu()


		cybstores:set_shader( "zip_tex" )
		cybstores:draw_layer( 13 )	-- Container for store ceiling
		cybstores:set_gl_parameters( "zip_tex" )
		

		local box = cybstores:get_layer_bdd( 6 )
		local store_layer = cybstores:get_layer_bdd( 6 )
		local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )
		local center_x = param.get( store_layer, "center_x" )
		local width = param.get( store_model, "size_u" )
		local height = param.get( store_model, "size_v" )
		local depth = param.get( store_model, "size_axe" ) * app.store.hessian_opening
	
		local min_x = center_x - width / 2
		local max_x = center_x + width / 2
	
		local angle = app.store.hessian_angle / 180 * math.pi

		local beam_width = 0.09
		local z = depth * math.cos( angle ) + beam_width + .1

		local max_y = height - depth * math.sin( angle ) - beam_width * 1.3
		local min_y = math.max( 0, max_y - zip_des / 1000 )

		gol.set_quad_uv( 0, 0, (max_x - min_x) / app.zip_tex.width, (max_y - min_y) / app.zip_tex.height )
		gol.normal3( 0, 0, 1 )
		aaa.draw_triangle_strip_4xyz_uv( 	min_x, max_y, z,
								min_x, min_y, z,
								max_x, max_y, z,
								max_x, min_y, z)
		gol.normal3( 0, 0, -1 )
		gol.set_quad_uv( -(max_x - min_x) / app.zip_tex.width, -(max_y - min_y) / app.zip_tex.height, 0, 0 )
		aaa.draw_triangle_strip_4xyz_uv( 	min_x, min_y, z,
								min_x, max_y, z,
								max_x, min_y, z,
								max_x, max_y, z)
	end
end

function BANNE:draw_arms()
	if app.store.hessian_opening == 0 then
		return
	end
	local cybstores = app:get_cybstores_meu()
	local tex = app.tex_armatures.banne_selected
	if tex then
		local layer = cybstores:get_layer( 8 )
		aaa.layer.set_bind_2d( layer, tex.bind )
	end

	local beam_width = app.dimensions.pergola.lames.poutre * .5 -- note: barre de charge = 150x80 et non 75x75

	local store_layer = cybstores:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )

	local center_x = param.get( store_layer, "center_x" )
	local width  = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" )
	local depth = param.get( store_model, "size_axe" ) * app.store.hessian_opening
	local total_length = param.get( store_model, "size_axe" ) + .2

	local beam_bdd = cybstores:get_layer_bdd( 8 )
	local beam_model = aaa.layer.get_model( cybstores:get_layer( 8 ) )

	local width_min_beams = ( width - beam_width ) * 0.5

	local angle = app.store.hessian_angle / 180 * math.pi
	local z = depth * math.cos( angle ) + .2
	local y = height - depth * math.sin( angle )

	local min_x = center_x - width / 2 * .85
	local max_x = center_x + width / 2 * .85

	-- size_u = x
	-- size_v = y
	-- size_axe = z

	param.set( beam_model, "size_v", beam_width )
	param.set( beam_model, "size_axe", beam_width )
	param.set( beam_model, "size_u",   total_length / 2 )

	param.set( beam_bdd, "center_x", 0 )
	param.set( beam_bdd, "center_z", 0 )
	param.set( beam_bdd, "center_y", 0 )

	local beam_angle = math.asin( app.store.hessian_opening )
	gol.push_matrix()
	gol.translate( min_x, y, z )
	gol.rotate_x( angle / ( 2 * math.pi ) )
	gol.rotate_y( beam_angle / ( 2 * math.pi ) )
	gol.translate( total_length / 4, -beam_width )
	cybstores:draw_layer( 8 )
	gol.pop_matrix ()

	gol.push_matrix()
	gol.translate( max_x, y, z )
	gol.rotate_x( angle / ( 2 * math.pi ) )
	gol.rotate_y( -beam_angle / ( 2 * math.pi ) )
	gol.translate( -total_length / 4, -beam_width )
	cybstores:draw_layer( 8 )
	gol.pop_matrix ()

	gol.push_matrix()
	gol.translate( min_x, height, beam_width )
	gol.rotate_x( angle / ( 2 * math.pi ) )
	gol.rotate_y( -beam_angle / ( 2 * math.pi ) )
	gol.translate( total_length / 4, -beam_width )
	cybstores:draw_layer( 8 )
	gol.pop_matrix ()

	gol.push_matrix()
	gol.translate( max_x, height, beam_width )
	gol.rotate_x( angle / ( 2 * math.pi ) )
	gol.rotate_y( beam_angle / ( 2 * math.pi ) )
	gol.translate( -total_length / 4, -beam_width )
	cybstores:draw_layer( 8 )
	gol.pop_matrix ()

end

function BANNE:draw_beam()
	local cybstores = app:get_cybstores_meu()

	local tex = app.tex_armatures.banne_selected
	if tex then
		local layer = cybstores:get_layer( 8 )
		aaa.layer.set_bind_2d( layer, tex.bind )
	end

	local beam_width = app.dimensions.pergola.lames.poutre * .5 -- note: barre de charge = 150x80 et non 75x75

	local store_layer = cybstores:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )

	local center_x = param.get( store_layer, "center_x" )
	local width  = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" )
	local depth = param.get( store_model, "size_axe" ) * app.store.hessian_opening

	local beam_bdd = cybstores:get_layer_bdd( 8 )
	local beam_model = aaa.layer.get_model( cybstores:get_layer( 8 ) )

	local width_min_beams = ( width - beam_width ) * 0.5

	local angle = app.store.hessian_angle / 180 * math.pi
	local z = depth * math.cos( angle ) + .2
	local y = height - depth * math.sin( angle )

	-- size_u = x
	-- size_v = y
	-- size_axe = z

	-- parallel to wall
	param.set( beam_bdd, "center_y", y - beam_width * .4 )
	param.set( beam_model, "size_v", beam_width ) -- note: barre de charge = 150x80 et non 150x150
	param.set( beam_model, "size_axe", beam_width )
	param.set( beam_model, "size_u",   width )
	param.set( beam_bdd, "center_x", center_x )
	param.set( beam_bdd, "center_z", z )
	cybstores:draw_layer( 8 )
end

function BANNE:draw_coffre()
	local cybstores = app:get_cybstores_meu()
	local tex = app.tex_armatures.banne_selected
	if tex then
		local layer = cybstores:get_layer( 8 )
		aaa.layer.set_bind_2d( layer, tex.bind )
	end

	local beam_width = app.dimensions.pergola.lames.poutre * .5 -- note: barre de charge = 150x80 et non 75x75

	local store_layer = cybstores:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )

	local center_x = param.get( store_layer, "center_x" )
	local width  = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" )
	local depth = param.get( store_model, "size_axe" ) * app.store.hessian_opening

	local beam_bdd = cybstores:get_layer_bdd( 8 )
	local beam_model = aaa.layer.get_model( cybstores:get_layer( 8 ) )

	local width_min_beams = ( width - beam_width ) * 0.5

	local angle = app.store.hessian_angle / 180 * math.pi
	local z = depth * math.cos( angle )
	local y = height - depth * math.sin( angle )

	-- size_u = x
	-- size_v = y
	-- size_axe = z

	-- parallel to wall
	param.set( beam_model, "size_v", .247 )
	param.set( beam_model, "size_axe", 0.197 )
	param.set( beam_model, "size_u",   width )
	param.set( beam_bdd, "center_y", height )
	param.set( beam_bdd, "center_x", center_x )
	param.set( beam_bdd, "center_z", 0.197 * .5 )
	cybstores:draw_layer( 8 )
end

function BANNE:draw_lambrequin_lumineux()
	local zip_length = app.zips.length * .001
	local zip_des = zip_length * app.zips.opening
	local ll = app.lambrequin_lumineux
	if zip_des and ll.count > 0 then
		local cybstores = app:get_cybstores_meu()

		cybstores:set_shader( "lambrequin_lumineux")
		cybstores:draw_layer( 14 )	-- Container for lambrequin_lumineux

		local tex = app.tab3d.tex_lambrequin_lumineux

		if not tex then return end

		local size_ratio = tex.sx / tex.sy

		local lamb_height = math.min(zip_length, ll.height * .001)
		local lamb_width  = ll.ratio * size_ratio * lamb_height
		local pos_x       = ll.position_x
		local pos_y       = ll.position_y


		gol.set_wrap_2d_edge()
		gol.set_texture_dim( 2 )
		gol.bind_texture( tex.bind )

		local box = cybstores:get_layer_bdd( 6 )
		local store_layer = cybstores:get_layer_bdd( 6 )
		local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )
		local center_x = param.get( store_layer, "center_x" )
		local width = param.get( store_model, "size_u" )
		local height = param.get( store_model, "size_v" )
		local depth = param.get( store_model, "size_axe" ) * app.store.hessian_opening
	
		if lamb_width  > width  then
			lamb_width = width
			lamb_height = lamb_width / (ll.ratio * size_ratio)
		end


		-- zip parameters
		local min_x = center_x - (width - lamb_width) * .5
		local max_x = center_x + (width - lamb_width) * .5
		local angle = app.store.hessian_angle / 180 * math.pi

		if ll.count == 2 then
			local param_width = max_x - min_x
			pos_x = .5 - pos_x * .5
			pos_x = (lamb_width + (param_width - lamb_width) * pos_x) / param_width
		end

		local beam_width = 0.09
		local z = depth * math.cos( angle ) + beam_width + .1

		local max_y = height - depth * math.sin( angle ) - beam_width * 1.3
		local min_y = math.max( 0, max_y - zip_des )

		zip_des = max_y - min_y
		zip_length = zip_des * app.zips.opening

		local max_height = max_y
		-- if lamb_height > max_y - min_x then lamb_height = max_y - min_y end

		-- lambrequin lumineux parameters
		local x = min_x + (pos_x * .5 + .5) * (max_x - min_x)
		min_x = x - lamb_width  * .5
		max_x = x + lamb_width  * .5

		-- local y = (min_y + max_y) * .5
		pos_y = 1 - pos_y
		pos_y = (lamb_height * .5 + pos_y * (zip_length - lamb_height)) / zip_length
		local y = max_y - zip_length * pos_y + (zip_length - zip_des)
		min_y = y - lamb_height * .5
		max_y = y + lamb_height * .5

		local uv_y = 1
		if (max_y > max_height) then
		    uv_y = (max_height - min_y) / (max_y - min_y)
			max_y = max_height
		end
		-- self:print("in", min_x, max_x, min_y, max_y)


		-- print(min_x, max_y, z)
		--Maa removing depth is a bad idea to avoid z fighting: we get logo on horizontal bar
		--gol.set_depth( false )	
		gol.set_quad_uv(0, 0, 1, uv_y)
		gol.normal3( 0, 0, 1 )
		z = z + .002	-- to avoid z fighting we offset z a little
		--todo should be made with a loop
		aaa.draw_triangle_strip_4xyz_uv( 	min_x, max_y, z,
								min_x, min_y, z,
								max_x, max_y, z,
								max_x, min_y, z)
								
		if ll.count == 2 then
			min_x = center_x - (width - lamb_width) * .5
			max_x = center_x + (width - lamb_width) * .5
	
			x = min_x + (-pos_x * .5 + .5) * (max_x - min_x)
			min_x = x - lamb_width  * .5
			max_x = x + lamb_width  * .5
			aaa.draw_triangle_strip_4xyz_uv( 	min_x, max_y, z,
									min_x, min_y, z,
									max_x, max_y, z,
									max_x, min_y, z)
		end
		--gol.set_depth( true )
	end
end

function BANNE:draw()
	local cybstores = app:get_cybstores_meu()
	cybstores:draw_layer( 1 )

	self:draw_arms()
	self:draw_beam()
	self:draw_coffre()
	self:draw_hessian()
	self:draw_zip()
	self:draw_lambrequin_lumineux()
end
