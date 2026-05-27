
if CLASS.DECLARE( "PERGOLA_MUR" ) then
	PERGOLA_MUR:set_class_status_doc(	CLASS.STATUS.APP,
										"PERGOLA_MUR is one of the product" )
end

function PERGOLA_MUR:draw_hessian()
	local cybstores = app:get_cybstores_meu()

	cybstores:set_shader( "hessian" )
	cybstores:draw_layer( 13 )	-- Container for store ceiling
	cybstores:set_gl_parameters( "hessian" )

	local store_layer = cybstores:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )
	local center_x = param.get( store_layer, "center_x" )
	local width = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" )
	local depth = ( param.get( store_model, "size_axe" ) + app.store.deportage_mur / 1000 ) * app.store.hessian_opening

	local min_x = center_x - width / 2
	local max_x = center_x + width / 2

	local angle = app.store.hessian_angle / 180 * math.pi
	local cos_angle = math.cos( angle )
	local sin_angle = math.sin( angle )
	local dy = math.sin( angle / 180 * math.pi )

	-- toile = 120cm x 120cm
	gol.normal3( 0, 1, 0 )

	local max_y = height
	local min_y = height - depth * math.sin( angle )
	local min_z = 0
	local max_z = depth * math.cos( angle )

	gol.set_quad_uv( 0, 0, (max_x - min_x) / app.hessian.width, depth / app.hessian.height )
	gol.normal3( 0, cos_angle, sin_angle )
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

end

function PERGOLA_MUR:draw_zip()
	local zip_des = app.zips.length
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
		local depth = ( param.get( store_model, "size_axe" ) + app.store.deportage_mur / 1000 ) * app.store.hessian_opening
	
		local min_x = center_x - width / 2
		local max_x = center_x + width / 2
	
		local angle = app.store.hessian_angle / 180 * math.pi

		local beam_width = 0.09
		local z = depth * math.cos( angle ) + beam_width * .25
	
		local max_y = height - depth * math.sin( angle )
		local min_y = math.max( 0, max_y - zip_des / 1000 )
		local zip_height = max_y - min_y

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

function PERGOLA_MUR:draw_beam()
	local cybstores = app:get_cybstores_meu()

	local tex = app.tex_armatures.pergola_mur_selected

	if tex then
		local layer = cybstores:get_layer( 8 )
		aaa.layer.set_bind_2d( layer, tex.bind )
	end

	local beam_width = 0.09

	local store_layer = cybstores:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )

	local center_x = param.get( store_layer, "center_x" )
	local width  = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" )
	local depth = ( param.get( store_model, "size_axe" ) + app.store.deportage_mur / 1000 ) * app.store.hessian_opening

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
	param.set( beam_bdd, "center_y", y )
	param.set( beam_model, "size_v", beam_width ) -- note: barre de charge = 150x80 et non 150x150
	param.set( beam_model, "size_axe", beam_width )
	param.set( beam_model, "size_u",   width )
	param.set( beam_bdd, "center_x", center_x )
	param.set( beam_bdd, "center_z", z )
	cybstores:draw_layer( 8 )
end

function PERGOLA_MUR:draw_coffre()
	local cybstores = app:get_cybstores_meu()

	local tex = app.tex_armatures.pergola_mur_selected
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

function PERGOLA_MUR:draw_frame()
	local cybstores = app:get_cybstores_meu()

	local tex = app.tex_armatures.pergola_mur_selected
	if tex then
		local layer = cybstores:get_layer( 8 )
		aaa.layer.set_bind_2d( layer, tex.bind )
	end

	local beam_width = 0.09

	local store_layer = cybstores:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )

	local center_x = param.get( store_layer, "center_x" )
	local width  = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" )
	-- local depth = param.get( store_model, "size_axe" ) * app.store.hessian_opening

	
	local total_length = param.get( store_model, "size_axe" )
	local length_deportage = total_length + app.store.deportage_mur / 1000

	local beam_bdd = cybstores:get_layer_bdd( 8 )
	local beam_model = aaa.layer.get_model( cybstores:get_layer( 8 ) )

	local width_min_beams = ( width - beam_width ) * 0.5

	local angle = app.store.hessian_angle / 180 * math.pi
	
	local min_x = center_x - width_min_beams
	local max_x = center_x + width_min_beams

	-- size_u = x
	-- size_v = y
	-- size_axe = z

	param.set( beam_model, "size_v", beam_width ) 
	param.set( beam_model, "size_axe", beam_width )
	param.set( beam_model, "size_u",   length_deportage )

	param.set( beam_bdd, "center_x", 0 )
	param.set( beam_bdd, "center_z", 0 )
	param.set( beam_bdd, "center_y", 0 )

	-- side beams
	gol.push_matrix()
	gol.translate( min_x, height )
	gol.rotate_x( angle / ( 2 * math.pi ) )
	gol.rotate_y( .25 )
	gol.translate( -length_deportage*.5 )
	cybstores:draw_layer( 8 )
	gol.pop_matrix ()

	gol.push_matrix()
	gol.translate( max_x, height )
	gol.rotate_x( angle / ( 2 * math.pi ) )
	gol.rotate_y( .25 )
	gol.translate( -length_deportage*.5 )
	cybstores:draw_layer( 8 )
	gol.pop_matrix ()

	-- front beam
	local angle = app.store.hessian_angle / 180 * math.pi
	local z = total_length * math.cos( angle )
	local y = height - total_length * math.sin( angle )
	param.set( beam_model, "size_u",   width )

	gol.push_matrix()
	gol.translate(		center_x,
						height - beam_width - total_length * .75 * math.sin( angle ),
						z * .68 )
	-- gol.translate( -total_length*.5 )
	cybstores:draw_layer( 8 )
	gol.pop_matrix ()


	-- pillars
	if tex then
		local layer = cybstores:get_layer( 7 )
		aaa.layer.set_bind_2d( layer, tex.bind )
	end
	local pillar_width = 0.09
	local pillar_bdd = cybstores:get_layer_bdd( 7 )
	local pillar_model = aaa.layer.get_model( cybstores:get_layer( 7 ) )

	param.set( pillar_bdd, "center_y", ( y - beam_width + pillar_width ) * 0.5 ) -- make sure it's just below the ceiling
	param.set( pillar_model, "size_v", y - beam_width + pillar_width )
	param.set( pillar_model, "size_u", pillar_width * 1 )
	param.set( pillar_model, "size_axe", pillar_width )
	
	param.set( pillar_bdd, "center_z", z - ( pillar_width - beam_width ) * 0.5 )
	param.set( pillar_bdd, "center_x", center_x - width * .5 + pillar_width * 0.5 )
	cybstores:draw_layer( 7 )
	param.set( pillar_bdd, "center_x", center_x + width * .5 - pillar_width * 0.5 )
	cybstores:draw_layer( 7 )

end

function PERGOLA_MUR:draw()
	local cybstores = app:get_cybstores_meu()
	
	cybstores:draw_layer( 1 )
	self:draw_beam()
	self:draw_frame()
	self:draw_coffre()
	self:draw_hessian()
	self:draw_zip()
	cybstores:draw_layer( 1 )
end