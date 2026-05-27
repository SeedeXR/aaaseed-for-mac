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

--avant define_grea_boid( grea_name, race_name, location, nb, boid_id )
--apres add_grea( grea_name, race_name, nb, boid_id, location


define_grea_b2d ( "B2D_WHITETIPBB", "B2D_WHITETIPBB", { SO2 }, 6, 800, -50, 0, rotation & degré,"00:00:00")
define_grea_boid( "B_BANNER", "B_BANNER", { KL2, KL3, KL4, KC, KR4, KR3, KR2 }, 100, 600, 150, -400, )
define_grea_anim( "A_MANTA", "A_MANTA", { KR3 }, 1,  )
define_grea_c2d( "C2D_EEL", "A2D_EEL", { KR4 }, 35)
define_grea_anim( "A_MRAY", "A_MRAY", { KL1 }, 1)
define_grea_boid( "B_CARANG", "B_CARANG", { KC, KR4, KR3, KR2, KR1 }, 50)
define_grea_anim( "A_PARROT", "A_PARROT", { KR3, KR2 } )
define_grea_boid( "B_PARROT_BL", "B_PARROT_BL", { KL3 }, 10)
define_grea_anim( "A_POTATO", "A_POTATO", { KL2 }, )
define_grea_boid( "B_GRACE", "B_GRACE", { KL1, KL2, KL3, KL4 }, 1)
define_grea_boid( "B_DARK GUN", "B_DARK GUN", { KL1, KL2, KL3, KL4, KC, KR4, KR3, KR2, KR1 }, 100)
define_grea_boid( "B_CONVICT", "B_CONVICT", { KL2, KL3, KL4 }, 15)
define_grea_c2d ( "C2D_LADY_GREEN_1", "C2D_LADY_GREEN", { KL3 }, 23)
define_grea_c2d ( "C2D_LADY_GREEN_2", "C2D_LADY_GREEN", { KL4 }, 23)
define_grea_c2d ( "C2D_LADY_GREEN_3", "C2D_LADY_GREEN", { KR3 }, 23)
define_grea_c2d ( "C2D_LADY_GREEN_4", "C2D_LADY_GREEN", { KR4 }, 23)
define_grea_c2d ( "C2D_LADY_GREEN_5", "C2D_LADY_GREEN", { SO1 }, 50)
define_grea_c2d ( "C2D_LADY_GREEN_6", "C2D_LADY_GREEN", { SO1 }, 50)
define_grea_c2d ( "C2D_LADY_GREEN_7", "C2D_LADY_GREEN", { SO3 }, 50)
define_grea_c2d ( "C2D_LADY_GREEN_8", "C2D_LADY_GREEN", { SO3 }, 50)
define_grea_c2d ( "C2D_LADY_GREEN_9", "C2D_LADY_GREEN", { SO3 }, 50)
define_grea_c2d ( "C2D_LADY_GREEN_10", "C2D_LADY_GREEN", { SO3 }, 50)
define_grea_c2d ( "C2D_LADY_BL_1", "C2D_LADY_BL", { KL2 }, 40)
define_grea_c2d ( "C2D_LADY_BL_2", "C2D_LADY_BL", { KR4 }, 40)
define_grea_c2d ( "C2D_LADY_BL_3", "C2D_LADY_BL", { KR3 }, 40)
define_grea_c2d ( "C2D_LADY_BL_4", "C2D_LADY_BL", { SO1 }, 40)
define_grea_boid( "B_GUN", "B_GUN", { KL1, KL2, KL3, KL4, KC, KR4, KR3, KR2, KR1 }, 100)
define_grea_boid( "B_RED_M_1", "B_RED_M", {KL2}, 30 )
define_grea_boid( "B_RED_M_2", "B_RED_M", {KL4}, 30 )
define_grea_boid( "B_RED_M_3", "B_RED_M", {KR4}, 30 )
define_grea_boid( "B_RED_M_4", "B_RED_M", {KR1}, 30 )
define_grea_boid( "B_RED_M_5", "B_RED_M", {SO4}, 30 )
define_grea_boid( "B_RED_M_6", "B_RED_M", {SO4}, 30 )
define_grea_boid( "B_RED_M_7", "B_RED_M", {SO4}, 30 )
define_grea_boid( "B_RED_M_8", "B_RED_M", {SO4}, 30 )
define_grea_boid( "B_RED_F_1", "B_RED_F", {KL2}, 30 )
define_grea_boid( "B_RED_F_2", "B_RED_F", {KL4}, 30 )
define_grea_boid( "B_RED_F_3", "B_RED_F", {KR4}, 30 )
define_grea_boid( "B_RED_F_4", "B_RED_F", {KR1}, 30 )
define_grea_boid( "B_RED_F_5", "B_RED_F", {SO4}, 30 )
define_grea_boid( "B_RED_F_6", "B_RED_F", {SO4}, 30 )
define_grea_boid( "B_RED_F_7", "B_RED_F", {SO4}, 30 )
define_grea_boid( "B_RED_F_8", "B_RED_F", {SO4}, 30 )
define_grea_anim( "A_CLEAN_1", "A_CLEAN", { KL3 }, )
define_grea_anim( "A_CLEAN_2", "A_CLEAN", { KC }, )
define_grea_anim( "A_CLEAN_3", "A_CLEAN", { KR4 }, )
define_grea_anim( "A_CLEAN_4", "A_CLEAN", { KL3 }, )
define_grea_c2d( "C2D_SHAG_1", "C2D_SHAG", { S01 }, 1)
define_grea_c2d( "C2D_SHAG_2", "C2D_SHAG", { S02 }, 1)
define_grea_boid( "B_CLOWN_1", "B_CLOWN", { KC }, 1 )
define_grea_boid( "B_CLOWN_2", "B_CLOWN", { KC }, 1 )
define_grea_boid( "B_CLOWN_3", "B_CLOWN", { SO2, SO3, SO4 }, 1 )
define_grea_boid( "B_CLOWN_4", "B_CLOWN", { KL2, KL3, KL4, KC, KR4, KR3, KR2, KR1 }, 1 )
define_grea_boid( "B_BUTTER_2", "B_BUTTER" , { KL3, KL4 }, 20 ) -- ici je commence à BUTTER_2 car le 1st grea jouera au PIJ1
define_grea_boid( "B_BUTTER_3", "B_BUTTER" , { KR4, KR3 }, 45 )
define_grea_boid( "B_BUTTER_4", "B_BUTTER" , { KR3, KR2, KR1 }, 30 )
define_grea_boid( "B_BUTTER_5", "B_BUTTER" , { SO1, SO2, SO3 }, 20 )
define_grea_c2d( "C2D_NEMO_1", "C2D_NEMO", { KL2 }, 12)
define_grea_c2d( "C2D_NEMO_2", "C2D_NEMO", { KR2 }, 18)
define_grea_c2d( "C2D_NEMO_3", "C2D_NEMO", { KR2 }, 18)
define_grea_boid( "B_SURGERY_1", "B_SURGERY", { KL1, KL2, KL3, KL4 }, 5)
define_grea_boid( "B_SURGERY_2", "B_SURGERY", { KC }, 10)
define_grea_boid( "B_SURGERY_3", "B_SURGERY", { KR4, KR3 }, 5)
define_grea_boid( "B_SURGERY_4", "B_SURGERY", { KR3, KR2, KR1 }, 7)
define_grea_boid( "B_SURGERY_5", "B_SURGERY", { KR3, KR2, KR1 }, 3)
define_grea_boid( "B_CORNU_1", "B_CORNU", { KL1, KL2, KL3, KL4, KC, KR4, KR3, KR2, KR1 }, 3 )
define_grea_boid( "B_CORNU_2", "B_CORNU", { KL1, KL2, KL3, KL4, KC, KR4, KR3, KR2, KR1 }, 2 )
define_grea_boid( "B_CORNU_3", "B_CORNU", { SO2 }, 2)
define_grea_boid( "B_CORNU_4", "B_CORNU", { SO3 }, 3)
define_grea_boid( "B_CORNU_5", "B_CORNU", { SO4 }, 2)
define_grea_boid( "B_HACHETTE_1", "B_HACHETTE", { KC } 50)
define_grea_boid( "B_HACHETTE_2", "B_HACHETTE", { KR4, KR3, KR2, KR1 } 70)
define_grea_boid( "B_BARAK_1", "B_BARAK_1", { KR3, KR4 }, 15 )
define_grea_c2d( "C2D_G_CLAM_1", "C2D_G_CLAM", { SO2 }, 1 )
define_grea_c2d( "C2D_G_CLAM_2", "C2D_G_CLAM", { SO2 }, 1 )
define_grea_c2d( "C2D_G_CLAM_3", "C2D_G_CLAM", { SO2 }, 1 )
define_grea_c2d( "C2D_G_CLAM_4", "C2D_G_CLAM", { SO2 }, 1 )
define_grea_c2d( "C2D_G_CLAM_5", "C2D_G_CLAM", { KL4 }, 1 )
define_grea_c2d( "C2D_G_CLAM_6", "C2D_G_CLAM", { KR3 }, 1 )
define_grea_c2d( "C2D_XMAS_1", "C2D_XMAS", { KL1 }, 20)
define_grea_c2d( "C2D_XMAS_2", "C2D_XMAS", { KL3 }, 25 )
define_grea_c2d( "C2D_XMAS_3", "C2D_XMAS", { KR1 }, 30 )
define_grea_boid( "B_LUTJ", "B_LUTJ", { KC, KR4, KR3, KR2, KR1 }, 50)
define_grea_boid( "B_EMPEROR_1", "B_EMPEROR", { KL1, KL2 }, 2)
define_grea_boid( "B_EMPEROR_2", "B_EMPEROR", { KL3, KL4 }, 2)
define_grea_boid( "B_EMPEROR_3", "B_EMPEROR", { KC }, 2)
define_grea_boid( "B_EMPEROR_4", "B_EMPEROR", { KR4, KR3, KR2, KR1 }, 2)
define_grea_boid( "B_TRIGGER", "B_TRIGGER", { KR3 }, 1)
define_grea_boid( "B_TITAN", "B_TITAN", { KR3 }, 1 )
define_grea_boid( "B_CUTTLE_1", "B_CUTTLE", {KL2}, 1 )
define_grea_boid( "B_CUTTLE_2", "B_CUTTLE", {KL3}, 1 )
define_grea_boid( "B_CUTTLE_3", "B_CUTTLE", {KL4}, 1 )









-- define_grea_c2d ( "C2D_NEMO")


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