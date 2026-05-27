--v2
-- on demarre une sequence de scratch ici
local seq = app:define_seq( "DJ1" )

seq.time = "00:00:00"	-- mm:ss:cc ---- m pour minute, s pour seconde, c pour centieme de seconde

	--methode target
	--soi on donne une target et ensuite on change des valeurs a l'interieur
	light:set_target()
		color = { 47, 140, 255 }	--couleur de 0 a 255
		power = 100					--de 0 a 100
		direction = { .1, 1., .2 }	-- x,y,z direction de la lumiere
	--methode target.field
	--soi on rajoute a la target le nom du field(champ) dont on change la valeur
		light.color = { 47, 140, 255 }		--couleur de 0 a 255
		light.power = 100					--de 0 a 100
		light.direction = { .1, 1., .2 }	-- x,y,z direction de la lumiere

	shadow.hardness = .5
	shadow.alpha = 0

	light_point_1.position = { 4 , 2, 12 }
	light_point_1.radius = 53

	plancton:set_target()
		color1 = {r1,g1,b2}		-- r1,g1,b1 de 0 a 255
		color2 = {r2,g2,b2}
		alpha = .9				-- 0 a 100
	--question
		speed = { min, max }
		--et/ou
		speed.min = min
		speed.max = max
	--ou bien ?
		speed_min = min
		speed_max = max

		hardness = 8


	-- FOG les trois versions sont toutes valides > si je prend la première on peut faire un if area > fog.density, lightpower etc > tout regrouper ?
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

--pour les poissons

--EVENTS
add_event( "00:00:00", "fade_in", 2. )
--ACTORS	NOM_
define_grea_boid( "DOLPH_BOID", 30, KL3, KL4, KC ) -- attention boid system
define_grea_boid( "DOLPH_BOID_BIS", 20, KR )
define_grea_anim( "DOLPH_DRAMA", "NOM_ANIM" )
define_grea_anim( "DOLPH_DRAMA_BIS", "NOM_ANIM" )
define_grea_cond( "DOLPH_COND", "NOM_ANIM", KL1, KL2 )
define_grea_cond( "DOLPH_COND_BIS", "NOM_ANIM" )
define_grea_train( "DOLPH_TRAIN", "NOM_ANIM" )

define_grea_boid( "TIGER", 40, KC )
	DOLPH_BOID.boid_nb = 30
	DOLPH_BOID.birth = {x,y,z}
	DOLPH_BOID.death = {x,y,z}
	DOLPH_BOID:set_corridor( x,y,z, sx,sy,sz, corridor_filename )

	-- a la mano calme
--	DOLPH_BOID.speed_min =  1
--	DOLPH_BOID.speed_max =  1

seq.time = "04:00:00"
	-- a la mano speed
	DOLPH_BOID.speed_min =  4
	DOLPH_BOID.speed_max =  4

	DOLPH_BOID:set_calm()

	--maa pense
	-- function TIGER:set_fear()
	-- 	self.speed = 2.5
	-- 	self.distance = 4
	-- end
	-- if behaviour == "Fear" then		--behaviour = curious,fear,nerve, neutral
	-- 	speed  = 2.5
	-- 	distance = 4
	-- elseif area == "KC" then
	-- 	density = 1.2
	-- 	distance = 9
	-- elseif area == "KL1" then
	-- 	density = 2.5
	-- 	distance = 3
	-- else
	-- 	density = 2.5
	-- 	distance = 4
	-- end
  --ou le comportement visiteur ?
--  if aud.behaviour == "nerve" then		--behaviour = nerve, calm, multiple_nerve, Multiple_calm ?
--    Tiger  == "attack" then

--	end