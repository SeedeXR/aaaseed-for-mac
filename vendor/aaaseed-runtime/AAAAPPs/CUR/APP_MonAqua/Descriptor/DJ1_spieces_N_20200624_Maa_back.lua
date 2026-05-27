local seq = app:define_seq( "DJ1" )

--EVENTS
add_event( "00:00:00", "fade_in", 2. )

-- On va specifier de la facon suivante example pour le DOLPH
--	B_DOLPH (boid),
--	B2D_DOLPH (boid mais suite image)
--	D_DOLPH (Anim 3D)
--	A_DOLPH (Anim 3D)	--idem D_DOLPH en moins sympa
-- 	A2D_DOLPH (Amin2D)	--not used for now
--  C_DOLPH (anim condition)
--  C2D_DOLPH (anim condition)
--	T_DOLPH (anim train) un chiffre suit pour indiquer une répétition B_DOLPH_1

-- emplacement
--	soit 1 parmi KL, KR, KC, SO, KL1, KL2, KL3, KL4, KR1, KR2, KR3, KR4
--	soit plusieurs parmi la liste ci-dessus exemple: { KL3, KL4, KC }

--	x : profondeur de la salle: 0 debut de la salle qui est cote +
--	y : hauteur: 0 sol + vers le haut
--	z : gauche - / droite +
--	x,y,z et size en cm

--define_grea_boid( nom_du_groupe (grea), nom_de_race/espece, emplacement, boid_nb_max )
define_grea_boid( "B_LADY_GREEN_1", "B_LADY_GREEN", { KL3, KL4, KC }, 30 )
--define_grea_boid( "B_LADY_GREEN_2", "B_LADY_GREEN", {KR3, KR4, KC }, 25 )

--define_c2d( "NOM_DBU", "NOM_FICHIER_AMIM", emplacement, x, y, z, size_en_cm, rot_y_deg )
define_c2d( "C2D_LADY_GREEN_1", "C2D_LADY_GREEN", KL3, 1500, 25, -540, 50, 0 )
define_c2d( "C2D_LADY_GREEN_2", "C2D_LADY_GREEN", KL3, 1550, 15, -530, 55, 5 )
define_c2d( "C2D_LADY_GREEN_3", "C2D_LADY_GREEN", KL4, 1510, 25, -530, 45, -5 )


--define_anim( "NOM_DBU", "NOM FICHIER", emplacement, time )
--en fait les anims sont place dans l'univers donc les position ici ne sont qu'un decalage et sont donc optionelle
define_anim(	"D_DOLPH_1", "D_DOLPH", {KL1, KL2, KL3}, "01:30:00" ):set_speed( 110 )
define_anim(	"D_DOLPH_2", "D_DOLPH", {KL1, KL2, KL3}, "01:35:00" ):set_xyz( 10, 10, -40 ):set_speed( 150 ) --percent
define_anim(	"D_DOLPH_3", "D_DOLPH", {KL2, KL3, KL4}, "01:31:00" )
	D_DOLPH_3:set_xyz( 100, 0, -50 )
	D_DOLPH_3:set_rot(5):set_speed( 250 ) --percent


define_anim(	"C_DOLPH_1", "C_DOLPH", KL4, "01:20:00" )
	C_DOLPH_1:set_speed( 200 ) --percent
define_anim(	"C_DOLPH_2", "C_DOLPH", KL3, "01:25:00" )
	C_DOLPH_2:set_xyz( 100, 0, -50 )
	C_DOLPH_2:set_speed( 120 ) --percent


-- define_grea_anim( "B2_RED_F", "NOM_ANIM" )
-- define_grea_anim( "B2_RED_M", "NOM_ANIM" )
-- define_grea_cond( "B_BUTTER", "NOM_ANIM", KL1, KL2 )
-- define_grea_cond( "B2_NEMO", "NOM_ANIM" )
-- define_grea_train( "B_SURGERY", "NOM_ANIM" )
-- define_grea_boid( "B_CONVICT", )
-- define_grea_anim( "B_DARK GUN")
-- define_grea_boid( "B_GUN")
-- define_grea_anim( "A_XMAS")
-- define_grea_boid( "B_LUTJ")
-- define_grea_boid( "B_SWEEP")
-- define_grea_boid( "B_BAT")
-- define_grea_boid( "B_BANNER")
-- define_grea_boid( "B_CARANG")
-- define_grea_boid( "B_MRAY")
-- define_grea_boid( "B_CLEAN")
-- define_grea_boid( "B_POTATO")
-- define_grea_boid( "B_CUTTLE")
-- define_grea_boid( "B_WHITETIPBB")
-- define_grea_boid( "B_TITAN")
-- define_grea_boid( "B_TRIGGER")
-- define_grea_boid( "B_PARROT_BL")
-- define_grea_boid( "B_PARROT")
-- define_grea_boid( "B_EEL")

define_grea_boid( "B_TIGER", "TIGER", KC, 40 )
	--method B_DOLPH:set_target() ok aussi
	B_TIGER.boid_nb = 30
	B_TIGER.birth_pos = {200,400,200}	--{x,y,z}
	B_TIGER.death_pos = {800,200,800}	--{x,y,z}
	B_TIGER:set_corridor( x,y,z, sx,sy,sz, "corridor_filename" )

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