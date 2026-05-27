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
		color_rgb,power,position 		= { 255, 230, 255 }, 1, { 0, 10, 4 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .75
		caustic_power = 0
		caustic_scale = 1

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 220, 249, 243 }, 30 , { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 220, 249, 243 }, 100 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 220, 249, 243 }, 100 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 220, 249, 243 }, 100 , { 20 , 2.5, 0 } , 9.
		gamma = 2

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
		bottom_rgb, top_rgb, horizon, range 			= {20,70,130}, {200,230,240}, 30, 15
		distance, density 								= { -0.7, 30}, 2
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=0, {0,198,255}, -90, 80, {8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = -0.2
		y_factor_bottom = 0
		shadow_rgb = {75,150,255}
		shadow_power = 40
		ambient_rgb =  {150,235,255}
		ambient_power = 35
		out_rgb = {240,240,255}
		out_power = 120

	LIGHT_DIR:set_target()
		 color_rgb,power,angle_azi 		= { 75, 150, 255 }, 100, { 90, 80 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		 shadow_hardness = .20

		 caustic_power = 1
		 caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		 color_rgb,power,position,radius =  { 20, 100, 200 }, 10 , { 5. , 6.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		 color_rgb,power,position,radius =  { 20, 100, 200 }, 10 , { 10. , 6.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		 color_rgb,power,position,radius = { 20, 100, 200 }, 50 , { 12.5, 4, 3 } , 10.
	LIGHT_POINT_4:set_target()
		 color_rgb,power,position,radius = { 20, 100, 200 }, 10 , { 20 , 6,5, 0 } , 12.

	MATERIAL_MATTE:set_target()
		specular =0
		roughness = 0.5

app:end_gstate_render( )



app:begin_gstate_render( "DIJ1_end" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {20,70,130}, {200,230,240}, 30, 15
		distance, density 								= { -0.7, 30}, 2
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=0, {0,198,255}, -90, 80, {8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = -0.2
		y_factor_bottom = 0
		shadow_rgb = {75,150,255}
		shadow_power = 40
		ambient_rgb =  {150,235,255}
		ambient_power = 35
		out_rgb = {240,240,255}
		out_power = 120

	LIGHT_DIR:set_target()
		 color_rgb,power,angle_azi 		= { 75, 150, 255 }, 100, { 90, 80 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		 shadow_hardness = .20

		 caustic_power = 1
		 caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		 color_rgb,power,position,radius =  { 20, 100, 200 }, 10 , { 5. , 6.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		 color_rgb,power,position,radius =  { 20, 100, 200 }, 10 , { 10. , 6.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		 color_rgb,power,position,radius = { 20, 100, 200 }, 50 , { 12.5, 4, 3 } , 10.
	LIGHT_POINT_4:set_target()
		 color_rgb,power,position,radius = { 20, 100, 200 }, 10 , { 20 , 6,5, 0 } , 12.

	MATERIAL_MATTE:set_target()
		specular =0
		roughness = 0.5

app:end_gstate_render( )




app:begin_gstate_render( "PIJ1" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {17,105,230}, {150,220,230}, 20, 15
		distance, density 								= { -0.7, 30}, 2
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=0, {100,105,230}, 100, 80, {1}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 0
		y_factor_bottom = 0
		shadow_rgb = {75,150,255}
		shadow_power = 50
		ambient_rgb =  {100,150,210}
		ambient_power = 70
		out_rgb = {255,265,255}
		out_power = 100

	LIGHT_DIR:set_target()
		 color_rgb,power,angle_azi 		= { 170, 200, 255 }, 90, { 90, 80 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		 shadow_hardness = .40

		 caustic_power = 70
		 caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		 color_rgb,power,position,radius = { 0, 0, 0 }, 0 , { 5. , 4.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		 color_rgb,power,position,radius =  { 0, 0, 0 }, 0 , { 10. , 4.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		 color_rgb,power,position,radius =  { 200, 200, 255 }, 80 , { 11.5 , 4.5, 2 } , 4.
	LIGHT_POINT_4:set_target()
		 color_rgb,power,position,radius = { 0, 0, 0 }, 0 , { 20 , 4, 0 } , 18.

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness = 0

app:end_gstate_render( )



app:begin_gstate_render("PIJ1_emd" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {50,100,255}, {50,190,255},1, 15
		distance, density 								= { 12, 25} ,1.2
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=0, {0,198,255}, -90, 80, {8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 0
		y_factor_bottom = 0
		shadow_rgb = {75,150,255}
		shadow_power = 60
		ambient_rgb = {230,255,255}
		ambient_power = 70
		out_rgb = {240,240,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 140, 230, 255 }, 100, { 110, 90 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .30

		caustic_power = 150
		caustic_scale = -10

	-- 5 LIGHT POINT

	-- 6 MATERIAL

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness = 0

app:end_gstate_render( )



app:begin_gstate_render( "DJ2" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {50,100,255}, {50,190,255},1, 15
		distance, density 								= { 12, 25} ,1.2
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=0, {0,198,255}, -90, 80, {8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 0
		y_factor_bottom = 0
		shadow_rgb = {75,150,255}
		shadow_power = 60
		ambient_rgb = {230,255,255}
		ambient_power = 70
		out_rgb = {240,240,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 140, 230, 255 }, 100, { 110, 90 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .30

		caustic_power = 150
		caustic_scale = -10

	-- 5 LIGHT POINT

	-- 6 MATERIAL

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness = 0

app:end_gstate_render( )



app:begin_gstate_render( "DJ2_end" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {50,100,255}, {50,190,255},1, 15
		distance, density 								= { 12, 25} ,1.2
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=0, {0,198,255}, -90, 80, {8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 0
		y_factor_bottom = 0
		shadow_rgb = {75,150,255}
		shadow_power = 60
		ambient_rgb = {230,255,255}
		ambient_power = 70
		out_rgb = {240,240,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 140, 230, 255 }, 100, { 110, 90 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .30

		caustic_power = 150
		caustic_scale = -10

	-- 5 LIGHT POINT

	-- 6 MATERIAL

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness = 0

app:end_gstate_render( )




app:begin_gstate_render( "PIJ2" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {50,50,255}, {50,180,255},1, 15
		distance, density 								= { 12, 25} ,1.2
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=0, {0,198,255}, -90, 80, {8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 0
		y_factor_bottom = 0
		shadow_rgb = {75,150,255}
		shadow_power = 50
		ambient_rgb = {230,255,255}
		ambient_power = 70
		out_rgb = {240,240,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 150, 200, 255 }, 100, { 150, 110 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .30

		caustic_power = 150
		caustic_scale = -10

	-- 5 LIGHT POINT

	-- 6 MATERIAL

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness = 0

app:end_gstate_render( )


app:begin_gstate_render( "PIJ2_end" )

		--  1 FOG
		FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,50,110}, {42,120,255}, -1.63, 10
		distance, density 								= { -0.7, 26}, 0.84
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=5, {0,198,255}, 5, 115, {0.8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 0
		y_factor_bottom = 0
		shadow_rgb = {75,150,255}
		shadow_power = 30
		ambient_rgb =  {190,255,255}
		ambient_power = 60
		out_rgb = {200,240,255}
		out_power = 100

	LIGHT_DIR:set_target()
		 color_rgb,power,angle_azi 		= { 75, 150, 255 }, 100, {170, 120 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		 shadow_hardness = .20

		 caustic_power = 50
		 caustic_scale = -0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		 color_rgb,power,position,radius = { 0,0,0 }, 0 , { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		 color_rgb,power,position,radius = { 0,0,0 }, 0 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		 color_rgb,power,position,radius = { 0,0,0 }, 0 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		 color_rgb,power,position,radius = { 0,0,0 }, 0 , { 20 , 4, 0 } , 18.

	MATERIAL_MATTE:set_target()
		specular =0.5
		roughness = 0

app:end_gstate_render( )



app:begin_gstate_render( "DJ3" )

		--  1 FOG
		FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,50,110}, {42,120,255}, -1.63, 10
		distance, density 								= { -0.7, 26}, 0.84
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=5, {0,198,255}, 5, 115, {0.8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 0
		y_factor_bottom = 0
		shadow_rgb = {75,150,255}
		shadow_power = 30
		ambient_rgb =  {190,255,255}
		ambient_power = 60
		out_rgb = {200,240,255}
		out_power = 100

	LIGHT_DIR:set_target()
		 color_rgb,power,angle_azi 		= { 75, 150, 255 }, 100, {170, 120 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		 shadow_hardness = .20

		 caustic_power = 50
		 caustic_scale = -0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		 color_rgb,power,position,radius = { 0,0,0 }, 0 , { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		 color_rgb,power,position,radius = { 0,0,0 }, 0 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		 color_rgb,power,position,radius = { 0,0,0 }, 0 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		 color_rgb,power,position,radius = { 0,0,0 }, 0 , { 20 , 4, 0 } , 18.

	MATERIAL_MATTE:set_target()
		specular =0.5
		roughness = 0

app:end_gstate_render( )


app:begin_gstate_render( "DJ3_end" )

		--  1 FOG
		FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,50,110}, {42,120,255}, -1.63, 10
		distance, density 								= { -0.7, 26}, 0.84
		sun, sun_rgb, sun_lr, sun_azi, sun_range		=5, {0,198,255}, 5, 115, {0.8}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 0
		y_factor_bottom = 0
		shadow_rgb = {75,150,255}
		shadow_power = 30
		ambient_rgb =  {190,255,255}
		ambient_power = 60
		out_rgb = {200,240,255}
		out_power = 100

	LIGHT_DIR:set_target()
		 color_rgb,power,angle_azi 		= { 75, 150, 255 }, 100, {170, 120 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		 shadow_hardness = .20

		 caustic_power = 50
		 caustic_scale = -0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		 color_rgb,power,position,radius = { 0,0,0 }, 0 , { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		 color_rgb,power,position,radius = { 0,0,0 }, 0 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		 color_rgb,power,position,radius = { 0,0,0 }, 0 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		 color_rgb,power,position,radius = { 0,0,0 }, 0 , { 20 , 4, 0 } , 18.

	MATERIAL_MATTE:set_target()
		specular =0.5
		roughness = 0

app:end_gstate_render( )


app:begin_gstate_render( "PIJ3" )

	--  1 FOG
	FOG:set_target()
	bottom_rgb, top_rgb, horizon, range 			= {0,50,127}, {42,180,220}, 27, 18
	distance, density 								= { 0, 24}, 0.6
	sun, sun_rgb, sun_lr, sun_azi, sun_range		=5, {0,198,255}, 5, 115, {0.8}

-- 2 AMBIENT
LIGHT_PASS:set_target()
	power = 100
	y_factor_top = 1
	y_factor_bottom = -1
	shadow_rgb = {75,150,255}
	shadow_power = 60
	ambient_rgb =  {255,255,255}
	ambient_power = 100
	out_rgb = {100,105,255}
	out_power = 100

LIGHT_DIR:set_target()
	 color_rgb,power,angle_azi 		= { 75, 150, 255 }, 30, {170, 120 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
	 shadow_hardness = .20

	 caustic_power = 50
	 caustic_scale = -0.4

-- 5 LIGHT POINT
LIGHT_POINT_1:set_target()
	 color_rgb,power,position,radius = { 215, 249, 232 }, 00 , { 5. , 2.5, 0 } , 10.
LIGHT_POINT_2:set_target()
	 color_rgb,power,position,radius = { 215, 249, 232 }, 000 , { 10. , 2.5, 0 } , 10
LIGHT_POINT_3:set_target()
	 color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 15. , 2.5, 0 } , 10.
LIGHT_POINT_4:set_target()
	 color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 20 , 4, 0 } , 18.

MATERIAL_MATTE:set_target()
	specular =1
	roughness = 1

app:end_gstate_render( )



app:begin_gstate_render( "PIJ3_end" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,20,39}, {1,69,90}, 0, 8
		distance, density 								= { 0, 20}, 0.8
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, -45, 45, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -1.5
		shadow_rgb = {50,50,50}
		shadow_power = 30
		ambient_rgb =  {0,0,0}
		ambient_power = 60
		out_rgb = {255,255,255}
		out_power = 60

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 86 }, 60, { 25,45 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .20
		caustic_power = 1
		caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius =  { 0, 200, 150 }, 5 , { 5. , 4.5, 2 } , 8.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius =  { 30, 200, 150 }, 8 , { 10 , 3.5, 2 } , 9
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 30, 200, 150 }, 13 , { 15 , 3.5, 2 } , 12.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 30, 200, 150 }, 13 , { 18 , 3.5, 2 } , 15.

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness =1

app:end_gstate_render( )



app:begin_gstate_render( "DJ4" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,20,39}, {1,69,90}, 0, 8
		distance, density 								= { 0, 20}, 0.8
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, -45, 45, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -1.5
		shadow_rgb = {50,50,50}
		shadow_power = 30
		ambient_rgb =  {0,0,0}
		ambient_power = 60
		out_rgb = {255,255,255}
		out_power = 80

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 86 }, 60, { 25,45 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .20
		caustic_power = 1
		caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius =  { 0, 200, 150 }, 0 , { 5. , 4.5, 2 } , 8.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius =  { 30, 200, 150 }, 0 , { 10 , 3.5, 2 } , 9
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 100, 150, 220 }, 6, { 15 , 5.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 100, 150, 220 }, 6 , { 18 , 5.5, 0 } , 20

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness =1

app:end_gstate_render( )


app:begin_gstate_render( "DJ4_end" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,20,39}, {1,69,90}, 0, 8
		distance, density 								= { 0, 20}, 0.8
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, -45, 45, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -1.5
		shadow_rgb = {50,50,50}
		shadow_power = 30
		ambient_rgb =  {0,0,0}
		ambient_power = 60
		out_rgb = {255,255,255}
		out_power = 60

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 86 }, 60, { 25,45 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .20
		caustic_power = 1
		caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius =  { 0, 200, 150 }, 5 , { 5. , 4.5, 2 } , 8.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius =  { 30, 200, 150 }, 8 , { 10 , 3.5, 2 } , 9
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 30, 200, 150 }, 13 , { 15 , 3.5, 2 } , 12.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 30, 200, 150 }, 13 , { 18 , 3.5, 2 } , 15.

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness =1

app:end_gstate_render( )






app:begin_gstate_render( "PIN1" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,50,60}, {0,10,0}, 5, 10
		distance, density 								= {-1, 20}, 0.6
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, 115, 65, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -2
		shadow_rgb = {50,50,50}
		shadow_power = 0
		ambient_rgb =  {0,0,0}
		ambient_power = 40
		out_rgb = {255,255,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 100 }, 100, { 115,65 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .30
		caustic_power = 0
		caustic_scale = 0

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0, { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 0, 255, 232 }, 0 , { 19 , 4.5, 0 } , 15.

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness =0.4


app:end_gstate_render( )


app:begin_gstate_render( "PIN1_end" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,30,55}, {0,10,0}, 5, 10
		distance, density 								= {-0.7, 20}, 0.4
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, 115, 65, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -1
		shadow_rgb = {50,50,50}
		shadow_power = 20
		ambient_rgb =  {0,0,0}
		ambient_power = 80
		out_rgb = {255,255,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 100 }, 100, { 115,65 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .30
		caustic_power = 0
		caustic_scale = 0

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0, { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 0, 255, 232 }, 0 , { 19 , 4.5, 0 } , 15.

	MATERIAL_MATTE:set_target()
		specular = 1
		roughness =0.4

app:end_gstate_render( )




app:begin_gstate_render( "DN1" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,30,55}, {0,10,0}, 5, 10
		distance, density 								= {-0.7, 20}, 0.4
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, 115, 65, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -1
		shadow_rgb = {50,50,50}
		shadow_power = 20
		ambient_rgb =  {0,0,0}
		ambient_power = 80
		out_rgb = {255,255,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 100 }, 100, { 115,65 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .30
		caustic_power = 0
		caustic_scale = 0

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0, { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 0, 255, 232 }, 0 , { 19 , 4.5, 0 } , 15.

	MATERIAL_MATTE:set_target()
		specular = 1
		roughness =0.4

app:end_gstate_render( )


app:begin_gstate_render( "PIN2" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {10,0,60}, {0,0,10}, 0, 8
		distance, density 								= { -0.7, 20}, 0.4
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, 115, 65, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -1
		shadow_rgb = {50,50,50}
		shadow_power = 10
		ambient_rgb =  {0,0,0}
		ambient_power = 100
		out_rgb = {255,255,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 15, 30, 100 }, 100, { 115,65 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .30
		caustic_power = 0
		caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0, { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 20, 78, 100 }, 0 , { 19 , 4.5, 0 } , 12.

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness =0


app:end_gstate_render( )



app:begin_gstate_render( "PIN2_end" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {10,0,60}, {0,0,10}, 0, 8
		distance, density 								= { -0.7, 20}, 0.4
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, 115, 65, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -1
		shadow_rgb = {50,50,50}
		shadow_power = 40
		ambient_rgb =  {0,0,0}
		ambient_power = 100
		out_rgb = {255,255,255}
		out_power = 100

		LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 15, 30, 100 }, 100, { 115,65 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .30
		caustic_power = 0
		caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 215, 50, 232 }, 0, { 8. , 2.5, 4 } , 5.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 20, 78, 100 }, 0 , { 21.5 ,2.5, 0 } , 12.

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness =0.4

app:end_gstate_render( )



app:begin_gstate_render( "DN2" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,30,30}, {0,10,0}, 0, 8
		distance, density 								= { -0.7, 20}, 0.4
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, 115, 65, {4}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -1
		shadow_rgb = {50,50,50}
		shadow_power = 40
		ambient_rgb =  {0,0,0}
		ambient_power = 100
		out_rgb = {255,255,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 100 }, 100, { 135,65 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .40
		caustic_power = 0
		caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0, { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 20, 78, 100 }, 20 , { 19.5 ,2.5, 0 } , 16.

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness =0.4

app:end_gstate_render( )


app:begin_gstate_render( "DN2_end" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,20,39}, {0,10,0}, 0, 8
		distance, density 								= { -0.7, 20}, 0.4
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, 115, 65, {4}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -1
		shadow_rgb = {50,50,50}
		shadow_power = 40
		ambient_rgb =  {0,0,0}
		ambient_power = 100
		out_rgb = {255,255,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 100 }, 100, { 135,65 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .40
		caustic_power = 0
		caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0, { 5. , 2.5, 0 } , 10.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 10. , 2.5, 0 } , 10
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 215, 249, 232 }, 0 , { 15. , 2.5, 0 } , 10.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 20, 78, 100 }, 35 , { 21.5 ,2.5, 0 } , 12.

	MATERIAL_MATTE:set_target()
		specular = 0
		roughness =0.4

app:end_gstate_render( )


app:begin_gstate_render( "PIN3" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,20,39}, {0,10,0}, 0, 11
		distance, density 								= { -0.7, 18}, 0.8
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, 115, 65, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 0.5
		y_factor_bottom = -1.5
		shadow_rgb = {30,0,80}
		shadow_power = 0
		ambient_rgb =  {10,0,80}
		ambient_power = 100
		out_rgb = {255,255,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 150 }, 100, { 145,80 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .40
		caustic_power = 0
		caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 0, 150, 255 }, 250, { 6. , .8, -2 } , 2.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 0, 150, 225 }, 100 , { 8. , 2., 4.5 } , 4

	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 0, 150, 255 }, 150 , { 13.2 , .5, -1.1 } , 2.5
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 50, 120, 255 }, 50 , { 14. , 2.,3.2 } , 8.




	MATERIAL_MATTE:set_target()
		specular = 1
		roughness =1

app:end_gstate_render( )




app:begin_gstate_render( "PIN3_end" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,20,100}, {0,0,0}, 3, 11
		distance, density 								= { -5, 18}, 0.99
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, 115, 65, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -1.5
		shadow_rgb = {50,50,50}
		shadow_power = 0
		ambient_rgb =  {0,0,0}
		ambient_power = 100
		out_rgb = {255,255,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 150 }, 30, { 30,65 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .70
		caustic_power = 0
		caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 20, 50, 232 }, 30 , { 5. , 7.5, 2 } , 15.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 20, 50, 232 }, 30 , { 10. , 7.5, 2 } , 15
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 20, 50, 232 }, 30 , { 15. , 7.5, 2 } , 15.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 20, 50, 232 }, 30 , { 19 , 7.5, 2 } , 15.

	MATERIAL_MATTE:set_target()
		specular = 1
		roughness =0

app:end_gstate_render( )



app:begin_gstate_render( "DN3" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,20,100}, {0,0,0}, 3, 11
		distance, density 								= { -5, 25}, 2
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, 115, 65, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -1.5
		shadow_rgb = {50,50,50}
		shadow_power = 0
		ambient_rgb =  {0,0,0}
		ambient_power = 100
		out_rgb = {255,255,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 150 }, 30, { 30,65 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .70
		caustic_power = 0
		caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 20, 50, 232 }, 30 , { 5. , 7.5, 2 } , 15.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 20, 50, 232 }, 30 , { 10. , 7.5, 2 } , 15
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 20, 50, 232 }, 30 , { 15. , 7.5, 2 } , 15.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 20, 50, 232 }, 30 , { 19 , 7.5, 2 } , 15.

	MATERIAL_MATTE:set_target()
		specular = 1
		roughness =0

app:end_gstate_render( )




app:begin_gstate_render( "DN3_end" )

	--  1 FOG
	FOG:set_target()
		bottom_rgb, top_rgb, horizon, range 			= {0,20,100}, {0,0,0}, 3, 11
		distance, density 								= { -5, 18}, 0.99
		sun, sun_rgb, sun_lr, sun_azi, sun_range		= 0, {0,163,185}, 115, 65, {2}

	-- 2 AMBIENT
	LIGHT_PASS:set_target()
		power = 100
		y_factor_top = 1
		y_factor_bottom = -1.5
		shadow_rgb = {50,50,50}
		shadow_power = 0
		ambient_rgb =  {0,0,0}
		ambient_power = 100
		out_rgb = {255,255,255}
		out_power = 100

	LIGHT_DIR:set_target()
		color_rgb,power,angle_azi 		= { 7, 78, 150 }, 30, { 30,65 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere
		shadow_hardness = .70
		caustic_power = 0
		caustic_scale = 0.4

	-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
		color_rgb,power,position,radius = { 20, 50, 232 }, 30 , { 5. , 7.5, 2 } , 15.
	LIGHT_POINT_2:set_target()
		color_rgb,power,position,radius = { 20, 50, 232 }, 30 , { 10. , 7.5, 2 } , 15
	LIGHT_POINT_3:set_target()
		color_rgb,power,position,radius = { 20, 50, 232 }, 30 , { 15. , 7.5, 2 } , 15.
	LIGHT_POINT_4:set_target()
		color_rgb,power,position,radius = { 20, 50, 232 }, 30 , { 19 , 7.5, 2 } , 15.

	MATERIAL_MATTE:set_target()
		specular = 1
		roughness =0

app:end_gstate_render( )