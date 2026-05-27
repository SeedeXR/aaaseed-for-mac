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
	LIGHT:set_target()
		color = { 47, 140, 255 }	--couleur de 0 a 255
		power = 100					--de 0 a 100 (voir plus) ?
		direction = { .1, 1., .2 }	-- x,y,z direction de la lumiere
	--methode target.field
	--soi on rajoute a la target le nom du field(champ) dont on change la valeur
		LIGHT.color = { 47, 140, 255 }		--couleur de 0 a 255
		LIGHT.power = 100					--de 0 a 100 (voir plus) ?
		LIGHT.direction = { .1, 1., .2 }	-- x,y,z direction de la lumiere

	SHADOW.hardness = .5
	SHADOW.alpha = 0

	LIGHT_POINT_1.position = { 4 , 2, 12 }
	LIGHT_POINT_1.radius = 5

	PLANCTON:set_target()
		color1 = {1,1,1}		-- r1,g1,b1 de 0 a 255
		color2 = {.5,.5,.5}
		alpha = .9				-- alpha de 0 a 1 ou de 0 a 100 ou de 0 a 255 ?
		speed_min = min
		speed_max = max

		hardness = 8
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



	-- FOG les trois versions sont toutes valides
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
