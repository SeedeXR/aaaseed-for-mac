
if CLASS.DECLARE( "PERGOLA" ) then
	PERGOLA:set_class_status_doc(	CLASS.STATUS.APP,
									"PERGOLA is one of the product" )
end


function PERGOLA:draw_pillars()
	local cybstores = app:get_cybstores_meu()

	local tex = app.tex_armatures.pergola_selected
	if tex then
		local layer = cybstores:get_layer( 7 )
		aaa.layer.set_bind_2d( layer, tex.bind )
	end

	local store_layer = cybstores:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )

	local pillar_width = app.dimensions.pergola.lames.poteau
	local beam_width = app.dimensions.pergola.lames.poutre

	local center_x = param.get( store_layer, "center_x" )
	local center_z = param.get( store_layer, "center_z" )
	local width  = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" )
	local depth  = param.get( store_model, "size_axe" )

	local width_min_pillar = ( width - pillar_width ) * 0.5
	local pillar_bdd = cybstores:get_layer_bdd( 7 )
	local pillar_model = aaa.layer.get_model( cybstores:get_layer( 7 ) )
	param.set( pillar_bdd, "center_y", height * 0.5 - beam_width * .5 ) -- make sure it's just below the ceiling
	param.set( pillar_model, "size_v", height - beam_width ) -- make sure it's just below the ceiling
	
	if center_z ~= 0 then
		param.set( pillar_bdd, "center_z", center_z + pillar_width * 0.5 )
		param.set( pillar_bdd, "center_x", center_x - width_min_pillar )
		cybstores:draw_layer( 7 )
		param.set( pillar_bdd, "center_x", center_x + width_min_pillar )
		cybstores:draw_layer( 7 )
	end

	param.set( pillar_bdd, "center_z", center_z + depth - pillar_width * 0.5 )
	param.set( pillar_bdd, "center_x", center_x - width_min_pillar )
	cybstores:draw_layer( 7 )
	param.set( pillar_bdd, "center_x", center_x + width_min_pillar )
	cybstores:draw_layer( 7 )

	if app.store.pillar_count == 6 then
		if width > depth then
			-- middle x-axis
			param.set( pillar_bdd, "center_x", center_x )
			if center_z ~= 0 then
				param.set( pillar_bdd, "center_z", center_z + pillar_width * 0.5 )
				cybstores:draw_layer( 7 )
			end
			param.set( pillar_bdd, "center_z", center_z + depth - pillar_width * 0.5 )
			cybstores:draw_layer( 7 )
		else 
			-- middle z-axis
			param.set( pillar_bdd, "center_z", center_z + depth * .5 )
			param.set( pillar_bdd, "center_x", center_x - width_min_pillar )
			cybstores:draw_layer( 7 )
			param.set( pillar_bdd, "center_x", center_x + width_min_pillar )
			cybstores:draw_layer( 7 )
		end
	end
end

function PERGOLA:draw_beams()
	local cybstores = app:get_cybstores_meu()

	local tex = app.tex_armatures.pergola_selected
	if tex then
		local layer = cybstores:get_layer( 8 )
		aaa.layer.set_bind_2d( layer, tex.bind )
	end

	local beam_width = app.dimensions.pergola.lames.poutre

	local store_layer = cybstores:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )

	local center_x = param.get( store_layer, "center_x" )
	local center_z = param.get( store_layer, "center_z" )
	local width  = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" )
	local depth  = param.get( store_model, "size_axe" ) + app.store.deportage / 1000
	local depth_pillars  = param.get( store_model, "size_axe" )

	local beam_bdd = cybstores:get_layer_bdd( 8 )
	local beam_model = aaa.layer.get_model( cybstores:get_layer( 8 ) )

	local width_min_beams = ( width - beam_width ) * 0.5

	-- size_u = x
	-- size_v = y
	-- size_axe = z

	param.set( beam_bdd, "center_y", height - beam_width * .5 )
	param.set( beam_model, "size_v", beam_width )

	-- perpendicular to wall
	param.set( beam_model, "size_u",   beam_width )
	param.set( beam_model, "size_axe", depth )

	param.set( beam_bdd, "center_z", center_z + depth * 0.5 )
	
	param.set( beam_bdd, "center_x", center_x - width_min_beams )
	cybstores:draw_layer( 8 )
	param.set( beam_bdd, "center_x", center_x + width_min_beams )
	cybstores:draw_layer( 8 )

	if width > depth_pillars and app.store.pillar_count == 6 then
		param.set( beam_bdd, "center_x", center_x )
		cybstores:draw_layer( 8 )
	end

	-- parallel to wall
	param.set( beam_model, "size_u",   width )
	-- param.set( beam_model, "size_v",   beam_width )
	param.set( beam_model, "size_axe", beam_width )

	param.set( beam_bdd, "center_x", center_x )
	
	param.set( beam_bdd, "center_z", center_z + beam_width * .5 )
	cybstores:draw_layer( 8 )
	param.set( beam_bdd, "center_z", center_z + depth - beam_width * .5 )
	cybstores:draw_layer( 8 )

	if width < depth_pillars and app.store.pillar_count == 6 then
		param.set( beam_bdd, "center_z", center_z + ( depth_pillars ) * .5 )
		cybstores:draw_layer( 8 )
	end
end

function PERGOLA:draw_leds()
	local cybstores = app:get_cybstores_meu()

	local tex = app.tex_armatures.pergola_selected
	if tex then
		local layer = cybstores:get_layer( 15 ) -- layer for leds
		aaa.layer.set_bind_2d( layer, tex )
	end
	cybstores:draw_layer( 15 )

	local beam_width = app.dimensions.pergola.lames.poutre
	local pillar_width = app.dimensions.pergola.lames.poteau
	local led_width = beam_width * .3

	local store_layer = cybstores:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )

	local center_x = param.get( store_layer, "center_x" )
	local center_z = param.get( store_layer, "center_z" )
	local width  = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" )
	local depth  = param.get( store_model, "size_axe" ) + app.store.deportage / 1000
	local depth_pillars  = param.get( store_model, "size_axe" )

	local width_min_beams = ( width - beam_width ) * 0.5

	-- size_u = x
	-- size_v = y
	-- size_axe = z

	local min_x, max_x, min_z, max_z
	local y = height - beam_width - .02

	gol.normal3( 0, -1, 0 )
	gol.set_quad_uv()
	gol.set_depth( true )

	min_z = center_z + pillar_width * .5
	max_z = center_z + depth - pillar_width * .5

	min_x = center_x - width_min_beams - led_width * .5
	max_x = center_x - width_min_beams + led_width * .5
	aaa.draw_triangle_strip_4xyz_uv( 	min_x, y, min_z,
							max_x, y, min_z,
							min_x, y, max_z,
							max_x, y, max_z)
	min_x = center_x + width_min_beams - led_width * .5
	max_x = center_x + width_min_beams + led_width * .5
	aaa.draw_triangle_strip_4xyz_uv( 	min_x, y, min_z,
							max_x, y, min_z,
							min_x, y, max_z,
							max_x, y, max_z)

	min_x = center_x - width_min_beams
	max_x = center_x + width_min_beams

	min_z = center_z + pillar_width * .5 - led_width * .5
	max_z = center_z + pillar_width * .5 + led_width * .5
	aaa.draw_triangle_strip_4xyz_uv( 	min_x, y, min_z,
							max_x, y, min_z,
							min_x, y, max_z,
							max_x, y, max_z)
	min_z = center_z + depth - pillar_width * .5 - led_width
	max_z = center_z + depth - pillar_width * .5
	aaa.draw_triangle_strip_4xyz_uv( 	min_x, y, min_z,
							max_x, y, min_z,
							min_x, y, max_z,
							max_x, y, max_z)

	gol.set_depth( true )
end

function PERGOLA:draw_hessian()
	local cybstores = app:get_cybstores_meu()

	cybstores:set_shader( "hessian" )
	cybstores:draw_layer( 13 )	-- Container for store ceiling
	cybstores:set_gl_parameters( "hessian" )


	local beam_width = app.dimensions.pergola.lames.poutre
	local store_layer = cybstores:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )
	local center_x = param.get( store_layer, "center_x" )
	local center_z = param.get( store_layer, "center_z" )
	local width = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" ) - beam_width * .5
	local depth = param.get( store_model, "size_axe" ) + app.store.deportage / 1000

	local min_x = center_x - width / 2 + beam_width
	local max_x = center_x + width / 2 - beam_width

	local min_z = center_z 		   + beam_width
	local max_z = center_z + depth - beam_width

	local y = height


	-- toile = 120cm x 120cm
	gol.normal3( 0, 1, 0 )
	gol.set_quad_uv( 0, 0, (max_x - min_x) / app.hessian.width, depth / app.hessian.height )
	aaa.draw_triangle_strip_4xyz_uv( 	min_x, y, min_z,
							min_x, y, max_z,
							max_x, y, min_z,
							max_x, y, max_z)
	gol.normal3( 0, -1, 0 )
	aaa.draw_triangle_strip_4xyz_uv( 	min_x, y, max_z,
							min_x, y, min_z,
							max_x, y, max_z,
							max_x, y, min_z)

	-- cybstores:set_gl_parameters( "hessian" )
	-- cybstores:draw_layer( 13 )	-- Container for store ceiling
end

function PERGOLA:draw_zips()
	local cybstores = app:get_cybstores_meu()

	cybstores:set_shader( "zip_tex" )
	cybstores:draw_layer( 13 )	-- Container for store ceiling
	cybstores:set_gl_parameters( "zip_tex" )

	local zip_des = app.zips.length

	if zip_des and zip_des > 0 then
		local store_layer = cybstores:get_layer_bdd( 6 )
		local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )
		local center_x = param.get( store_layer, "center_x" )
		local center_z = param.get( store_layer, "center_z" )
		local width = param.get( store_model, "size_u" )
		local height = param.get( store_model, "size_v" )
		local depth = param.get( store_model, "size_axe" )
		local beam_width = app.dimensions.pergola.lames.poutre
		local pillar_width = app.dimensions.pergola.lames.poteau

		local min_x = center_x - width / 2 + pillar_width * .5
		local max_x = center_x + width / 2 - pillar_width * .5
	
		local min_z = center_z + pillar_width * .5
		local max_z = center_z + depth - pillar_width * .5
	
		local max_y = height - beam_width
		local min_y = math.max( 0, max_y - zip_des / 1000 )

		if app.zips.left then
			gol.set_quad_uv( -(max_z - min_z) / app.zip_tex.width, -(max_y - min_y) / app.zip_tex.height, 0, 0 )
			gol.normal3( 1, 0, 0 )
			aaa.draw_triangle_strip_4xyz_uv( 	min_x, min_y, min_z,
									min_x, max_y, min_z,
									min_x, min_y, max_z,
									min_x, max_y, max_z)
			gol.normal3( -1, 0, 0 )
			gol.set_quad_uv( 0, 0, (max_z - min_z) / app.zip_tex.width, (max_y - min_y) / app.zip_tex.height )
			aaa.draw_triangle_strip_4xyz_uv( 	min_x, max_y, min_z,
									min_x, min_y, min_z,
									min_x, max_y, max_z,
									min_x, min_y, max_z)	
		end

		if app.zips.right then
			gol.set_quad_uv( -(max_z - min_z) / app.zip_tex.width, -(max_y - min_y) / app.zip_tex.height, 0, 0 )
			gol.normal3( 1, 0, 0 )
			aaa.draw_triangle_strip_4xyz_uv( 	max_x, min_y, min_z,
									max_x, max_y, min_z,
									max_x, min_y, max_z,
									max_x, max_y, max_z)
			gol.normal3( -1, 0, 0 )
			gol.set_quad_uv( 0, 0, (max_z - min_z) / app.zip_tex.width, (max_y - min_y) / app.zip_tex.height )
			aaa.draw_triangle_strip_4xyz_uv( 	max_x, max_y, min_z,
									max_x, min_y, min_z,
									max_x, max_y, max_z,
									max_x, min_y, max_z)	
		end

		local pillar_count = app.store.pillar_count
		if app.zips.forward then
			local local_max_x = pillar_count == 4 and max_x or ( max_x + min_x ) * .5

			gol.set_quad_uv( -(local_max_x - min_x) / app.zip_tex.width, -(max_y - min_y) / app.zip_tex.height, 0, 0 )
			gol.normal3( 0, 0, -1 )
			aaa.draw_triangle_strip_4xyz_uv( 		  min_x, min_y, max_z,
										  min_x, max_y, max_z,
									local_max_x, min_y, max_z,
									local_max_x, max_y, max_z)
			gol.normal3( 0, 0, 1 )
			gol.set_quad_uv( 0, 0, (local_max_x - min_x) / app.zip_tex.width, (max_y - min_y) / app.zip_tex.height )
			aaa.draw_triangle_strip_4xyz_uv( 		  min_x, max_y, max_z,
										  min_x, min_y, max_z,
									local_max_x, max_y, max_z,
									local_max_x, min_y, max_z)	
		end
		if app.zips.forward_2 and pillar_count == 6 then
			local local_min_x = ( max_x + min_x ) * .5
			gol.set_quad_uv( -(max_x - local_min_x) / app.zip_tex.width, -(max_y - min_y) / app.zip_tex.height, 0, 0 )
			gol.normal3( 0, 0, -1 )
			aaa.draw_triangle_strip_4xyz_uv( 	local_min_x, min_y, max_z,
									local_min_x, max_y, max_z,
										  max_x, min_y, max_z,
										  max_x, max_y, max_z)
			gol.normal3( 0, 0, 1 )
			gol.set_quad_uv( 0, 0, (max_x - local_min_x) / app.zip_tex.width, (max_y - min_y) / app.zip_tex.height )
			aaa.draw_triangle_strip_4xyz_uv( 	local_min_x, max_y, max_z,
									local_min_x, min_y, max_z,
										  max_x, max_y, max_z,
										  max_x, min_y, max_z)	
		end

		if center_z > 0 and app.zips.wall then
			local local_max_x = pillar_count == 4 and max_x or ( max_x + min_x ) * .5

			gol.set_quad_uv( -(local_max_x - min_x) / app.zip_tex.width, -(max_y - min_y) / app.zip_tex.height, 0, 0 )
			gol.normal3( 0, 0, -1 )
			aaa.draw_triangle_strip_4xyz_uv( 		  min_x, min_y, min_z,
										  min_x, max_y, min_z,
									local_max_x, min_y, min_z,
									local_max_x, max_y, min_z)
			gol.normal3( 0, 0, 1 )
			gol.set_quad_uv( 0, 0, (local_max_x - min_x) / app.zip_tex.width, (max_y - min_y) / app.zip_tex.height )
			aaa.draw_triangle_strip_4xyz_uv( 		  min_x, max_y, min_z,
										  min_x, min_y, min_z,
									local_max_x, max_y, min_z,
									local_max_x, min_y, min_z)	
		end
		if center_z > 0 and app.zips.wall_2 and pillar_count == 6 then
			local local_min_x = ( max_x + min_x ) * .5

			gol.set_quad_uv( -(max_x - local_min_x) / app.zip_tex.width, -(max_y - min_y) / app.zip_tex.height, 0, 0 )
			gol.normal3( 0, 0, -1 )
			aaa.draw_triangle_strip_4xyz_uv( 	local_min_x, min_y, min_z,
									local_min_x, max_y, min_z,
										  max_x, min_y, min_z,
										  max_x, max_y, min_z)
			gol.normal3( 0, 0, 1 )
			gol.set_quad_uv( 0, 0, (max_x - local_min_x) / app.zip_tex.width, (max_y - min_y) / app.zip_tex.height )
			aaa.draw_triangle_strip_4xyz_uv( 	local_min_x, max_y, min_z,
									local_min_x, min_y, min_z,
										  max_x, max_y, min_z,
										  max_x, min_y, min_z)	
		end

		cybstores:draw_layer( 1 )	-- Container for store ceiling

	end	
end

function PERGOLA:draw_blades()
	local cybstores = app:get_cybstores_meu()

	local tex = app.tab3d.tex_lames
	if tex then
		local layer = cybstores:get_layer( 9 )
		aaa.layer.set_bind_2d( layer, tex.bind )
	end

	local beam_width = app.dimensions.pergola.lames.poutre
	local blades_width = app.dimensions.pergola.lames.lame

	local store_layer = cybstores:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybstores:get_layer( 6 ) )

	local center_x = param.get( store_layer, "center_x" )
	local center_z = param.get( store_layer, "center_z" )
	local width  = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" )
	local depth  = param.get( store_model, "size_axe" ) + app.store.deportage / 1000

	local blade_bdd = cybstores:get_layer_bdd( 9 )
	local blade_model = aaa.layer.get_model( cybstores:get_layer( 9 ) )

	-- size_u = x
	-- size_v = y
	-- size_axe = z

	param.set( blade_model, "size_v", 0.04 )
	param.set( blade_model, "size_u", blades_width )
	param.set( blade_model, "size_axe",  depth - beam_width )

	param.set( blade_bdd, "center_x", 0 )
	param.set( blade_bdd, "center_z", 0 )
	param.set( blade_bdd, "center_y", 0 )
	
	local retractation = app.store.retractation
	local left  = app.store.lames_retract_gauche
	local right = app.store.lames_retract_droite


	if retractation == 0 or not left and not right then
		local blade_angle = app.store.blade_angle
		local x = center_x - width * .5 + beam_width + blades_width *.5
		while x < center_x + width * .5 - beam_width do
			gol.push_matrix()
			-- param.set( blade_bdd, "center_y", 5+i * .5 )
			gol.translate( x, height - beam_width * .5, center_z + depth * .5 )
			gol.rotate_z( blade_angle )
			cybstores:draw_layer( 9 )
			gol.pop_matrix ()
			x = x + blades_width
		end
	else
		local count = ( left and 1 or 0 ) + ( right and 1 or 0 )
		local factor = ( 1 - .86 * retractation ) / count
		local half_width = ( width - beam_width - blades_width ) * .5
		local max_width = ( width - beam_width - blades_width ) * factor
		if left then
			local start_x = center_x - half_width
			local end_x = start_x + max_width
			local blade_angle = retractation * .25
			local x = start_x
			while x < end_x do
				gol.push_matrix()
				-- param.set( blade_bdd, "center_y", 5+i * .5 )
				gol.translate( x,height - beam_width * .5, center_z + depth * .5 )
				gol.rotate_z( blade_angle )
				cybstores:draw_layer( 9 )
				gol.pop_matrix ()
				x = x + factor * blades_width * count
			end
		end
		if right then
			local start_x = center_x + half_width
			local end_x = start_x - max_width
			local blade_angle = retractation * .25
			local x = start_x
			while x > end_x do
				gol.push_matrix()
				-- param.set( blade_bdd, "center_y", 5+i * .5 )
				gol.translate( x, height - beam_width * .5, center_z + depth * .5 )
				gol.rotate_z( blade_angle )
				cybstores:draw_layer( 9 )
				gol.pop_matrix ()
				x = x - factor * blades_width * count
			end
		end
	end
end

function PERGOLA:draw()
	local cybstores = app:get_cybstores_meu()
	cybstores:draw_layer( 1 )

	self:draw_pillars()
	self:draw_beams()


	if app.tab3d.pergola_mode == "Toile" then
		self:draw_hessian()
	end
	if app.tab3d.pergola_mode == "Lames" then
		self:draw_blades()
	end

	-- self:draw_leds()

	self:draw_zips()
end
