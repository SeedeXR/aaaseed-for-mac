
--EVENTS
add_event( "00:00:00", "fade_in", 2. )
--ACTORS	NOM_

-- On vat specifier de la facon suivante rappel
-- On a les drama, les anims les condition, les boids,les boids2D les trains (graph)
-- je propose la nomenclature suivante a partager avec ki > B_DOLPH (boid), B2_DOLPH (boid2D)
--, C_DOLPH (anim a condition), D_DOLPH_DN1 (drama dn1),T_DOLPH, (train) un chiffre suit pour indiquer une répétition B_DOLPH_1

define_grea_boid( "B2_LADY GREEN_1", 30, KL3, KL4, KC ) -- attention boid system, le nom, le nombre, l'emplacement
define_grea_boid( "B2_LADY BL_2", 20, KR )
define_grea_anim( "B2_RED F", "NOM_ANIM" )
define_grea_anim( "B2_RED M", "NOM_ANIM" )
define_grea_cond( "B_BUTTER", "NOM_ANIM", KL1, KL2 )
define_grea_cond( "B2_NEMO", "NOM_ANIM" )
define_grea_train( "B_SURGERY", "NOM_ANIM" )
define_grea_boid( "B_CONVICT", )
define_grea_anim("B_DARK GUN")
define_grea_boid("B_GUN")
define_grea_anim("A_XMAS")
define_grea_boid("B_LUTJ")
define_grea_boid("B_SWEEP")
define_grea_boid("B_BAT")
define_grea_boid("B_BANNER")
define_grea_boid("B_CARANG")
define_grea_boid("B_MRAY")
define_grea_boid("B_CLEAN")
define_grea_boid("B_POTATO")
define_grea_boid("B_CUTTLE")
define_grea_boid("B_WHITETIPBB")
define_grea_boid("B_TITAN")
define_grea_boid("B_TRIGGER")
define_grea_boid("B_PARROT_BL")
define_grea_boid("B_PARROT")
define_grea_boid("B_EEL")


define_grea_boid( "TIGER", 40, KC )
	--method B_DOLPH:set_target() ok aussi
	B_DOLPH.boid_nb = 30
	B_DOLPH.birth_pos = {2,4,2}	--{x,y,z}
	B_DOLPH.death_pos = {8,2,8}	--{x,y,z}
	B_DOLPH:set_corridor( x,y,z, sx,sy,sz, "corridor_filename" )

	-- a la mano calme
--	B_DOLPH.speed_min =  1
--	B_DOLPH.speed_max =  1

seq.time = "04:00:00"
	-- a la mano speed
	B_DOLPH.speed_min =  4
	B_DOLPH.speed_max =  4

	B_DOLPH:set_calm()

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