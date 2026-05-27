--v2
-- on demarre une sequence de scratch ici
local seq = app:define_seq( "DJ1" )

seq.time = "00:00:00"	-- mm:ss:cc ---- m pour minute, s pour seconde, c pour centieme de seconde

	--methode target
	--soi on donne une target et ensuite on change des valeurs a l'interieur
	light:set_target()
		color = { 47, 140, 255 }	--couleur de 0 a 255
		power = 100					--de 0 a 100 (voir plus) ?
		direction = { .1, 1., .2 }	-- x,y,z direction de la lumiere
	--methode target.field
	--soi on rajoute a la target le nom du field(champ) dont on change la valeur
		light.color = { 47, 140, 255 }		--couleur de 0 a 255
		light.power = 100					--de 0 a 100 (voir plus) ?
		light.direction = { .1, 1., .2 }	-- x,y,z direction de la lumiere

	shadow.hardness = .5
	shadow.alpha = 0

	light_point_1.position = { 4 , 2, 12 }
	light_point_1.radius = 5

	plancton:set_target()
		color1 = {r1,g1,b2}		-- r1,g1,b1 de 0 a 255
		color2 = {r2,g2,b2}
		alpha = .9				-- alpha de 0 a 1 ou de 0 a 100 ou de 0 a 255 ?
	--question
		speed = { min, max }
		--et/ou
		speed.min = min
		speed.max = max
	--ou bien ?
		speed_min = min
		speed_max = max

		hardness = 8


	-- FOG les trois versions sont toutes valides
	--v1
	if area == "SO" then		--area peut etre "SO","KC","KL","KR" ou "KL1".....
		fog.density = 2.5
		fog.distance = 4
	elseif area == "KC" then
		fog.density = 1.2
		fog.distance = 9
	elseif area == "KL1" then
		fog.density = 2.5
		fog.distance = 3
	else
		fog.density = 2.5
		fog.distance = 4
	end
	--qui peut s'ecrire aussi
	--v2
	fog:set_target()
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
	fog:set_target()
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
	light.power = 100
seq.time = "00:20:10"
	light.power = 250
seq.time = "00:20:20":
	light.power = 100
--ou en version curve
seq.time = "00:20:00"
	light.power:curve( 100,	.1,250, .1, 100 )

seq.time = "01:22:00"
	if area == "KC" then
		light_point_1:set_target()
			radius:curve(	1,		4.,4,	1.,8, 	1.,6 )
			intensity:curve( 0,		4.,50,	2.,100 )
			color:curve( {0,0,0},
							4., {255, 255, 128},
							2.,	{255, 128, 128} )
	end

