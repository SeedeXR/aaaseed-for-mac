app:begin_gstate_render( "test" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {255,64,255}, {0,68,255}, 3, 2.0
		distance, density 								= { 5, 20.}, 1
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0., {255,255,255}, -45, 22, {0,.8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 0
		y_factor_bottom = 0
		shadow_rgb = {255,0,0}
		shadow_power = 100
		ambient_rgb =  {0,255,0}
		ambient_power = 0
		out_rgb = {255,255,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,position 		= { 255, 255, 255 }, 1, { 0, 10, 4 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .75
		caustic_power = 0
		caustic_scale = 1

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 220, 249, 243 }, 100 , { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 220, 249, 243 }, 100 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 220, 249, 243 }, 100 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 220, 249, 243 }, 100 , { 20 , 2.5, 0 } , 9.

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness = .25
	-- 6 PLANCTON
	-- 	PLANCTON.active = false
	-- 	PLANCTON_1:set_target()
	-- 	color,alpha,size, hardness, birth 		= { 0, 55, 79 } , 20 , 15 , 20 ,  20		-- r1,g1,b1 de 0 a 255, alpha de 0 a 100 , size en millimetre, hardness de 0 a 10 birth en pourcentage
	-- 	speed_min, speed_max, agitation, accel 	= 2 , 6 , 1.1, 0.0 			-- speed cm/m , accel, agitation, facteur acceleration par mêtre

	-- 	PLANCTON_2:set_target()
	-- 	color,alpha,size, hardness, birth 		= { 0, 0, 0 } , 20 , 12 , 20, 20		-- r1,g1,b1 de 0 a 255, alpha de 0 a 100 , size en millimetre, birth en pourcentage
	-- 	speed_min, speed_max, agitation, accel 	= 2 , 6 , 1.1, 0.0 			-- speed cm/m , accel, agitation, facteur acceleration par mêtre

	-- -- 7 FLOW
	-- 	FLOW:set_target()
	-- 	distance,top_density,intensity,speed,dispertion =  800,150,1.2,20,1.1 -- distance par rapport au spectateur, hauteur, intensité du mouvement, vitesse, coefficient dispertion dans la profondeur
app:end_gstate_render()

app:begin_gstate_render( "DJ1" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {00,00,255}, {200,250,255}, 30, 15
		distance, density 								= { 0, 30}, 2
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=0, {0,198,255}, -90, 80, {8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = -0.2
		y_factor_bottom = 0
		shadow_rgb = {75,150,255}
		shadow_power = 100
		ambient_rgb =  {190,255,255}
		ambient_power = 20
		out_rgb = {240,240,255}
		out_power = 120

	LIGHT_DIR:set_target()
		 color_rgb,power,angle_azi 		= { 75, 150, 255 }, 100, { 90, 80 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		 shadow_hardness = .20

		 caustic_power = 1
		 caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		 color_rgb,power,position,radius = { 215, 249, 232 }, 100 , { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		 color_rgb,power,position,radius = { 215, 249, 232 }, 100 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		 color_rgb,power,position,radius = { 215, 249, 232 }, 100 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		 color_rgb,power,position,radius = { 215, 249, 232 }, 100 , { 20 , 2.5, 0 } , 9.

	MATERIAL_MATTE:set_target()
		specular =0
		roughness = 0.5

app:end_gstate_render( )


app:begin_gstate_render( "DJ2" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {45,80,255}, {50,180,255},20, 12
		distance, density 								= { 0, 20}, 80
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=0, {0,198,255}, -90, 80, {8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 0.4
		y_factor_bottom = 0
		shadow_rgb = {75,150,255}
		shadow_power = 70
		ambient_rgb =  {230,255,255}
		ambient_power = 70
		out_rgb = {240,240,255}
		out_power = 85

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 150, 200, 255 }, 100, { 110, 90 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .50

		caustic_power = 150
		caustic_scale = -10

	-- 5 LIGHT POINT

	MATERIAL_MATTE:set_target()
		specular = 0.2
		roughness = 0.5

app:end_gstate_render( )

app:begin_gstate_render( "DJ3" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {45,80,255}, {100,180,255},20, 12
		distance, density 								= { 0, 20}, 3
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=0, {0,198,255}, -90, 80, {8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 0.6
		y_factor_bottom = -0.4
		shadow_rgb = {75,150,255}
		shadow_power = 70
		ambient_rgb =  {230,255,255}
		ambient_power = 70
		out_rgb = {240,240,255}
		out_power = 85

	LIGHT_DIR:set_target()
		 color_rgb,power,angle_azi 		= { 100, 150, 255 }, 80, { 130, 110 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		 shadow_hardness = .30

		 caustic_power = 50
		 caustic_scale = 1

	-- 5 LIGHT POINT

	MATERIAL_MATTE:set_target()
		specular = 0.2
		roughness = 0.5

app:end_gstate_render( )

app:begin_gstate_render( "DJ4" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,20,39}, {1,79,100}, 0, 8
		distance, density 								= { 0, 20}, 0.8
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, -45, 45, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -2
		shadow_rgb = {50,50,50}
		shadow_power = 50
		ambient_rgb =  {0,0,0}
		ambient_power = 100
		out_rgb = {255,255,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 86 }, 70, { 25,45 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .20
		caustic_power = 1
		caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 50 , { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 50 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 50 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 50 , { 20 , 2.5, 0 } , 9.

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness =1

app:end_gstate_render( )