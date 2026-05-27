--v2
-- on demarre une sequence de scratch ici
--cuisine de Maa pour test
local area = "SO"

--c' est a vous
local seq = app:define_seq( "DJ1" )

seq.time = "00:00:00"	-- mm:ss:cc ---- m pour minute, s pour seconde, c pour centieme de seconde

	app:print( "toto" )	--cuisine de Maa pour test
	--methode target
	--soi on donne une target et ensuite on change des valeurs a l'interieur

-- STRUCTURE
		-- je propose d'organiser le fichier RENDER  dans cet ordre
		-- EN Base le PRESET 1 avec la possibilité de faire un PRESET 2 de sortie de sortie de séquence
			-- 1) fog  = color down, color up, hauteur médiane (cm), hardness (cm), distance (cm), density(100), deep of field,
			--(les distance sont données à partir du bord ecran)
					-- SUN, color, radius, power, angle au sol (0 à 180) et angle en hauteur (0 à 90), sun_range
			-- 2) AMBIENT = color (r,v,b), power (100), direction (x,y,z, depuis le répère d'ensemble, 0 entrée de salle, 0 sol, 0 milieu de salle)
			-- 3) Shadows =  hardness(100), transparence (alpha 100)
			-- 4) caustiques = color(r,v,b), size (100 > la taille initiale), transparence (100)
			-- 5) light point = color, power, position (x,y,z), radius (cm), hardness (100)
			-- 6) Plancton = color 1, color 2, size min, size max, hardness 1,hardness 2, transparence 1, transparence 2, Speed min, speed max,
			-- 7) FLOW = effet de flou dynamique de fond genre courant marin
						-- distance de début (cm depuis bord écran), hauteur depuis le sol,
						-- intensité de l'onde > (coefficient)) (1.1), speed de l'ondulation (100)
						-- dispersion ( facteur multiplicateur en fonction de la distance d'eloignement (coefficient 1,1)

			-- 8) les éléments particuliers genre point light qui donnent du reliefs, ou tout autre élément
			-- 9) ensuite lorsque les PRESETS d'entré et de sortie sont calés on passe à la timeline structurée sout forne d' EVENT
					-- les EVENTS sont les éléments qui modifient ponctuellement une scene. dans les event on ne change que les valeurs qui evoluent









-- DJ1 (done)
local seq = app:define_seq( "DJ1" )
	local preset = app:define_preset( "EARLY MORNING_1" )
	seq.time = "00:00:00"


--  1 FOG
	FOG:set_target()
	bottom_color,top_color,horizon,range 		= {17,0,156}, {31,35,255}, 450 , 700
	distance_min,distance_max,density 			= 0 , 1000 , 80
	sun_color,sun_position,radius,sun_range	 	= {0,201,253} , {0,45} , 700 , 1    -- radius en cm

-- 2 AMBIENT
	AMBIENT:set_target()
	color,power,direction 		= { 0, 185, 253 }, 70, { 1500, 0, 400 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere

-- 3 SHADOWS
	SHADOW:set_target()
	hardness,alpha 				= 15 , 30

-- 4 CAUSTIC
	CAUSTIC.active = false
	CAUSTIC:set_target()
	color,size,alpha 			= {0,0,0} , 0 , 0

-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
	color,power,position,radius = { 220, 249, 243 }, 100 , { 500 , 250, 0 } , 1000

	LIGHT_POINT_2:set_target()
	color,power,position,radius = { 220, 249, 243 }, 100 , { 1000 , 250, 0 } , 1000

	LIGHT_POINT_3:set_target()
	color,power,position,radius = { 220, 249, 243 }, 100 , { 1500 , 250, 0 } , 1000

	LIGHT_POINT_4:set_target()
	color,power,position,radius = { 220, 249, 243 }, 100 , { 2000 , 250, 0 } , 900

-- 6 PLANCTON
	PLANCTON.active = false
	PLANCTON_1:set_target()
	color,alpha,size, hardness, birth 		= { 0, 55, 79 } , 20 , 15 , 20 ,  20		-- r1,g1,b1 de 0 a 255, alpha de 0 a 100 , size en millimetre, hardness de 0 a 10 birth en pourcentage
	speed_min, speed_max, agitation, accel 	= 2 , 6 , 1.1, 0.0 			-- speed cm/m , accel, agitation, facteur acceleration par mêtre

	PLANCTON_2:set_target()
	color,alpha,size, hardness, birth 		= { 0, 0, 0 } , 20 , 12 , 20, 20		-- r1,g1,b1 de 0 a 255, alpha de 0 a 100 , size en millimetre, birth en pourcentage
	speed_min, speed_max, agitation, accel 	= 2 , 6 , 1.1, 0.0 			-- speed cm/m , accel, agitation, facteur acceleration par mêtre

-- 7 FLOW
	FLOW:set_target()
	distance,top_density,intensity,speed,dispertion =  800,150,1.2,20,1.1 -- distance par rapport au spectateur, hauteur, intensité du mouvement, vitesse, coefficient dispertion dans la profondeur









-- DJ2 (to be done)
local seq = app:define_seq( "DJ2" )
	local preset = app:define_preset( "NOON_1" )
	seq.time = "00:00:00"


--  1 FOG
	FOG:set_target()
	bottom_color,top_color,horizon,sun_range 		= {0,83,255} , {0,183,255} , 450 , 700
	distance_min,distance_max,density 			= 0 , 1000 , 80
	suncolor,sun_position,radius,sun_range	 	= {166,246,255} , {10,25} , 400 , 1    -- radius en cm

-- 2 AMBIENT
	AMBIENT:set_target()
	color,power,direction 		= { 0, 185, 255 }, 70, { 1500, 0, 400 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere

-- 3 SHADOWS
	SHADOW:set_target()
	hardness,alpha 				= 15 , 30

-- 4 CAUSTIC
	CAUSTIC:set_target()
	color,size,alpha 			= {240,240,255} , 0 , 0

-- 5 LIGHT POINT
	LIGHT_POINT_1:set_target()
	color,power,position,radius = { 220, 249, 243 }, 100 , { 500 , 250, 0 } , 1000

	LIGHT_POINT_2:set_target()
	color,power,position,radius = { 220, 249, 243 }, 100 , { 1000 , 250, 0 } , 1000

	LIGHT_POINT_3:set_target()
	color,power,position,radius = { 220, 249, 243 }, 100 , { 1500 , 250, 0 } , 1000

	LIGHT_POINT_4:set_target()
	color,power,position,radius = { 220, 249, 243 }, 100 , { 2000 , 250, 0 } , 900

-- 6 PLANCTON
	PLANCTON.active = false
	PLANCTON_1:set_target()
	color,alpha,size, hardness, birth 		= { 0, 55, 79 } , 20 , 15 , 20 ,  20		-- r1,g1,b1 de 0 a 255, alpha de 0 a 100 , size en millimetre, hardness de 0 a 10 birth en pourcentage
	speed_min, speed_max, agitation, accel 	= 2 , 6 , 1.1, 0.0 			-- speed cm/m , accel, agitation, facteur acceleration par mêtre

	PLANCTON_2:set_target()
	color,alpha,size, hardness, birth 		= { 0, 0, 0 } , 20 , 12 , 20, 20		-- r1,g1,b1 de 0 a 255, alpha de 0 a 100 , size en millimetre, birth en pourcentage
	speed_min, speed_max, agitation, accel 	= 2 , 6 , 1.1, 0.0 			-- speed cm/m , accel, agitation, facteur acceleration par mêtre

-- 7 FLOW
	FLOW:set_target()
	distance,top_density,intensity,speed,dispertion =  800,150,1.2,20,1.1 -- distance par rapport au spectateur, hauteur, intensité du mouvement, vitesse, coefficient dispertion dans la profondeur









-- DJ3 (done)
local seq = app:define_seq( "DJ3" )
local preset = app:define_preset( "MID_DAY_1" )
seq.time = "00:00:00"

	--  1 FOG
			FOG.bottom_color, FOG.top_color, FOG.bottom_y, FOG.top_y 	= {0,133,255} , {0,0,255} , 450 , 700
			FOG.distance_min, FOG.distance_max, FOG.density 			= 0 , 3000 , 100
			FOG.sun_position, FOG.sun_range, Fog.suncolor, Fog.radius 	= {2100,900,-2000} , {0,0,0} , {0,198,255} ,200      -- radius en cm


	-- 2 AMBIENT
			AMBIENT.color, AMBIENT.power, AMBIENT.direction 			= { 240, 240, 255 } , 80, { 1000, 0, 0 } 			--couleur de 0 a 255, POWER de 0 a 100 (voir plus) ?
				-- direction  x,y,z direction de la lumiere


	-- 3 SHADOWS
			SHADOW.hardness, SHADOW.alpha 	= 80 , 50


	-- 4 CAUSTIC
			CAUSTIC.color, CAUSTIC.size, CAUSTIC.alpha = {250,250,255} , 80 , 30


	-- 5 LIGHT POINT (OUT)
			LIGHT_POINT_active = false   -- false = absent
			LIGHT_POINT_1.color = { 47, 140, 255 }		--couleur de 0 a 255
			LIGHT_POINT_1.power = 100					--de 0 a 100 (voir plus) ?
			LIGHT_POINT_1.position = { 4 , 2, 12 }
			LIGHT_POINT_1.radius = 5


	-- 6 PLANCTON
			PLANCTON.color1, PLANCTON.color2, = { 0, 55, 79 },{ 0, 0, 0 },			-- r1,g1,b1 de 0 a 255
			PLANCTON.alpha1, PLANCTON.alpha2 	 		= 80,5   -- alpha de 0 a 100
			PLANCTON.size_max, PLANCTON.size_min 		= 20,12   -- en millimetre
			PLANCTON.hardness_1, PLANCTON.hardness_2 	= 8,5   -- hardness de 0 a 10
			PLANCTON.speed_min, PLANCTON.speed_max, PLANCTON.acceleration = 5,10,1.1			-- speed cm/m , facteur acceleration par mêtre


	-- 7 FLOW
			SFX.distance, SFX.top_density, SFX.intensity, SFX.speed, SFX.dispertion =  100 , 150 , 1.2 , 20 , 1.2







-- DJ4 (done)
local seq = app:define_seq( "DJ4" )

	local preset = app:define_preset( "HUNT_1" )
	seq.time = "00:00:00"

	--  1 FOG
			FOG:set_target()
			bottom_color,top_color,horizon,sun_range 		= {0,28,39} , {1,79,100} , 450 , 700
			distance_min,distance_max,density 			= 0 , 600 , 90
			sun.color,sun.position,sun.radius,sun.anchor	 	= {0,163,185} , {10,0}, 500 ,     -- radius en cm

	-- 2 AMBIENT
			AMBIENT:set_target()
			color,power,direction 		= { 7, 78, 86 }, 30, { 1500, 0, 400 }	--couleur de 0 a 255, power de 0 a 100 (voir plus), direction x,y,z direction de la lumiere

	-- 3 SHADOWS
			SHADOW:set_target()
			hardness,alpha 				= 15 , 30

	-- 4 CAUSTIC
			CAUSTIC.active = false
			CAUSTIC:set_target()
			color,size,alpha 			= {250,250,255} , 80 , 30

	-- 5 LIGHT POINT
			LIGHT_POINT_1:set_target()
			color,power,position,radius = { 216, 249, 232 }, 100 , { 500 , 350, 100 } , 1100

			LIGHT_POINT_2:set_target()
			color,power,position,radius = { 216, 249, 232 }, 100 , { 1000 , 350, 100 } , 1100

			LIGHT_POINT_3:set_target()
			color,power,position,radius = { 216, 249, 232 }, 100 , { 1500 , 350, 100 } , 1100

			LIGHT_POINT_4:set_target()
			color,power,position,radius = { 216, 249, 232 }, 100 , { 2000 , 350, 100 } , 900

	-- 6 PLANCTON
			PLANCTON_1:set_target()
			color,alpha,size, hardness, birth 		= { 0, 55, 79 } , 20 , 15 , 20 ,  20		-- r1,g1,b1 de 0 a 255, alpha de 0 a 100 , size en millimetre, hardness de 0 a 10 birth en pourcentage
			speed_min, speed_max, agitation, accel 	= 2 , 6 , 1.1, 0.0 			-- speed cm/m , accel, agitation, facteur acceleration par mêtre

			PLANCTON_2:set_target()
			color,alpha,size, hardness, birth 		= { 0, 0, 0 } , 20 , 12 , 20, 20		-- r1,g1,b1 de 0 a 255, alpha de 0 a 100 , size en millimetre, birth en pourcentage
			speed_min, speed_max, agitation, accel 	= 2 , 6 , 1.1, 0.0 			-- speed cm/m , accel, agitation, facteur acceleration par mêtre

	-- 7 FLOW
			FLOW:set_target()
			distance,top_density,intensity,speed,dispertion =  100,150,1.2,20,1.2 -- distance par rapport au spectateur, hauteur, intensité du mouvement, vitesse, coefficient dispertion dans la profondeur


	local preset = app:define_preset( "HUNT_END" )
	seq.time = "00:03:00"

	--  1 FOG -- je rentre que les valeurs qui changent, par defaut le reste continue
			FOG:set_target()
			sun_color,sun_position,sun_radius,sun_range	 	= {0,0,0} , {0,0} ,0, 0   -- radius en cm














	--v1
	if area == "SO" then		--area peut etre "SO","KC","KL","KR" ou "KL1".....
		FOG.density = 2.5
		FOG.distance = 4
	elseif area == "KC" then
		FOG.density = 1.2
		FOG.distance = 9
	elseif area == "KL1" then
		FOG.density = 2.5
		FOG.distance = 3
	else
		FOG.density = 2.5
		FOG.distance = 4
	end
	--qui peut s'ecrire aussi
	--v2
	FOG:set_target()
	if area == "SO" then		--area peut etre "SO","KC","KL","KR" ou "KL1".....
		density = 2.5
		distance = 4
	elseif area == "KC" then
		density = 1.2
		distance = 9
	elseif area == "KL1" then
		density = 2.5
		distance = 3
	else
		density = 2.5
		distance = 4
	end
	--qui peut s'ecrire aussi (plus dense)
	--v3
	FOG:set_target()
	if		area == "SO" then	density, distance =	2.5,	4
	elseif	area == "KC" then	density, distance = 1.2,	9
	elseif	area == "KL1" then	density, distance = 2.5,	3
	else						density, distance = 2.5,	8
	end

--
-- ON PEUT COMMENTER COMME ON VEUX
--
--un petit eclair sur 0.2 secondes
seq.time = "00:20:00"
	LIGHT.power = 100
seq.time = "00:20:10"
	LIGHT.power = 250
seq.time = "00:20:20"
	LIGHT.power = 100
--ou en version curve
seq.time = "00:20:00"
	LIGHT.power:curve( 100,	.1,250, .1, 100 )

seq.time = "01:22:00"
	if area == "KC" then
		LIGHT_POINT_1:set_target()
			radius:curve(	1,		4.,4,	1.,8, 	1.,6 )
			intensity:curve( 0,		4.,50,	2.,100 )
			color:curve( {0,0,0},
							4., {255, 255, 128},
							2.,	{255, 128, 128} )
	end


--EVENTS
add_event( "00:00:00", "fade_in", 2. )
--ACTORS	NOM_
