if not aaa.lua.global.get( "TANK" ) then return end

--[[
function TANK:draw_race_on_curve( name, nb, curve_id, speed )
	local race = self:race_get( name )
	if race then
		race:draw_nb_on_curve( nb, curve_id, speed )
	else
		self:print_error( "race named : "..name.." don't exist do a race_add()" )
	end
end
--]]

function TANK:update_after()
--	if true then return end
	if not self.races then return end

	--todo add different type of drawing
	-- local nb = 10
	-- local speed = (self.creature_speed or 1.)
	-- self:draw_race_on_curve( "requin",		nb,		6,	speed )
	-- self:draw_race_on_curve( "aaaref",		nb,		2,	speed )

	-- move the tuna away to vary the env
	-- local grea = self:get_grea_by_name( "thon" )
	-- if grea then
	-- 	local v = aaa.math.get_fractalsum( 0, 0, aaa.time.t, .1, 2 )
	-- 	grea:get_boid():set_box_z( -12. * v )
	-- end
end


--TO DO ON DIFFERENT FISH
-- grea:set_random_walk({
-- 	speed 			=    0.2,
-- 	b_is_stationary =  true,
-- 	time_stationary =    1,		-- required since it's a stationary random walk.
-- 	time_moving		=   5,		-- required since it's a stationary random walk.
-- 	border 			=    0.5,	-- optionnal
-- 	radius_max 		=    0.3,
-- 	 radius_min 		=    0.05  	-- optional if radius_max is already defined (will be set to radius_max)
-- })
-- grea:set_random_walk({
-- 	speed 			=    0.2,
-- 	b_is_stationary =  true,
-- 	time_stationary =    2,		-- required since it's a stationary random walk.
-- 	time_moving		=   5,		-- required since it's a stationary random walk.
-- 	border 			=    0.7,	-- optionnal
-- 	radius_max 		=    0.5,
-- 	 radius_min 		=    0.05  	-- optional if radius_max is already defined (will be set to radius_max)
-- })


function TANK:init_fish()
	local scene_name = app:get_scene()
	local grea
	-- TANK:add_grea( name, race_name, nb, boid_id )
	-- BOID ID from 1 to 48


--    grea = self:add_grea( "Simple",			"B_SIMPLE",			100,		64	)
    grea = self:add_grea( "anchois_test",	"B_ANCHOVY",			3000,		64	)
		grea:init_box_xyz( 0,20.75, -1,-.75, -4,4 )
---[[
-- RED_F OUT UNTIL FIXED, BUGGED, FLICKER, KI



	local sel = { DJ1=true, DJ2=true, DJ3=true, PIJ1=true, PIJ2=true, PIJ3=true }
	-- REDs DOIVENT JOUER DANS LES SCENES: DJ1, DJ2, DJ3, PIJ1, PIJ2, PIJ3
	-- De bas en haut sur l'axe des X, a gauche devant les KL, les RED A, B et C, puis D a droite.
	if sel[scene_name] then
		grea = self:add_grea( "RED_M_SO4_A", "B_RED_M", 30, 				1, "SO" )
			grea:init_box_xyz( 8.50,9.5, -0.20,-0.60, -1.80,-2.80 )
			grea:init_target_active(true)
			grea:init_target_at_box_center()
			grea:init_target_radius(0.4, 0.1) -- EXTERNE - INTERNE
			grea:init_target_influence(2)
		grea = self:add_grea( "RED_F_5O4_A", "B_RED_F", 20, 				2, "SO" )
			grea:init_box_xyz( 8.50,9.5, -0.20,-0.60, -1.80,-2.80 )
			grea:init_target_active(true)
			grea:init_target_at_box_center()
			grea:init_target_radius(0.4, 0.1)
			grea:init_target_influence(2)

		grea = self:add_grea( "RED_M_SO4_D", "B_RED_M", 30, 				3, "SO" )
			grea:init_box_xyz( 13.70,15.1, -0.1,-0.40, 1.40,2.70 )
			grea:init_target_active(true)
			grea:init_target_at_box_center()
			grea:init_target_radius(0.4, 0.1)
			grea:init_target_influence(2)
		grea = self:add_grea( "RED_F_SO4_D", "B_RED_F", 20, 				4, "SO" )
			grea:init_box_xyz( 13.70,15.1, -0.1,-0.40, 1.40,2.70 )
			grea:init_target_active(true)
			grea:init_target_at_box_center()
			grea:init_target_radius(0.4, 0.1)
			grea:init_target_influence(2)

		grea = self:add_grea( "RED_M_SO4_B", "B_RED_M", 30, 				5, "SO" )
			grea:init_box_xyz( 13.60,15.20, -0.10,-0.40, -1.60,-2.45 )
			grea:init_target_active(true)
			grea:init_target_at_box_center()
			grea:init_target_radius(0.4, 0.1)
			grea:init_target_influence(2)
		grea = self:add_grea( "RED_F_SO4_B", "B_RED_F", 20, 				6, "SO" )
			grea:init_box_xyz( 13.60,15.20, -0.10,-0.40, -1.60,-2.45 )
			grea:init_target_active(true)
			grea:init_target_at_box_center()
			grea:init_target_radius(0.4, 0.1)
			grea:init_target_influence(2)

		grea = self:add_grea( "RED_M_SO_C", "B_RED_M", 30, 					7, "SO" )
			grea:init_box_xyz( 18.20,19.20, -0.10,-0.40, -1.40,-2.40 )
			grea:init_target_active(true)
			grea:init_target_at_box_center()
			grea:init_target_radius(0.4, 0.1)
			grea:init_target_influence(1)
		grea = self:add_grea( "RED_F_SO_C", "B_RED_F", 20, 					8, "SO" )
			grea:init_box_xyz( 18.20,19.20, -0.10,-0.40, -1.40,-2.40 )
			grea:init_target_active(true)
			grea:init_target_at_box_center()
			grea:init_target_radius(0.4, 0.1)
			grea:init_target_influence(1)
	end

	sel = { DJ2=true, DJ3=true, PIJ1=true, PIJ2=true, PIJ3=true }
	-- DOIT JOUER DANS LES SCENES: DJ2, DJ3, PIJ1, PIJ2, PIJ3
	if sel[scene_name] then
    	grea = self:add_grea( "CARPET_SO1", "B_CARPET", 1,                   9, "SO" )
        	grea:init_box_xyz( 5,10, 0,-0.80, -4,4 )
   		grea = self:add_grea( "CARPET_SO2", "B_CARPET", 1,                   10, "SO")
        	grea:init_box_xyz( 0,5, 0,-0.80, -4,4 )
	end

	sel = { PJ1=true, PJ2=true, DJ1=true, DJ2=true, DJ3=true }
-- DOIT JOUER DANS LES SCENES : PJ1, PJ2, DJ1, DJ2, DJ3
	if sel[scene_name] then
    	grea = self:add_grea( "CORNU_SO234", "B_CORNU", 30,                 	11, "SO" )
        	grea:init_box_xyz( 5.10,21, -0.20,-0.80, -1.5, 1.5)
-- DOIT JOUER DANS LES SCENES : PJ1, PJ2, DJ1, DJ2, DJ3
		grea = self:add_grea( "SURGERY_SO", "B_SURGERY", 50,                  	18, "SO" )
			grea:init_box_xyz( -2,22, -0.10,-1, -4,4 )
	end

	sel = { DJ2=true, DJ3=true, PIJ1=true, PIJ2=true }
-- DOIT JOUER DANS LES SCENES: DJ2, DJ3, PIJ1, PIJ2
	if sel[scene_name] then
    	grea = self:add_grea( "LION_SO234", "B_LION", 1,                    	12, "SO" )
        	grea:init_box_xyz( 5.10,21, -0.20,-0.80, -2,2 )
	end


	if scene_name=="PIJ1" then
-- DOIT JOUER DANS LES SCENES:  PIJ1
    	grea = self:add_grea( "TURTL", "B_TURTL", 5,              		    	14, "SO" )
    	-- Go Around, but not together
			grea:init_box_xyz(-2,21.5, 1,-1, -4,4 )
	end


	if scene_name=="PIN1" then
-- DOIT JOUER DANS LES SCENES: SCENES DE NUIT: PIN1
    	grea = self:add_grea( "FLASH_SO_1", "B_FLASH", 50, 						13, "SO" )
        	grea:init_box_xyz( 0.1,22, -0.1,-0.4, -3.9,3.9 )
-- DOIT JOUER DANS LES SCENES: PIN1
    	grea = self:add_grea( "TURTL_BB_SO", "B_TURTL_BB", 900, 				15, "SO" )
        	grea:init_box_xyz( -0.5,21.5, -0.2,-0.5, -4,4 )
	end

	sel = { PIJ1=true, DJ2=true, PIJ2=true, DJ3=true }
-- DOIT JOUER DANS LES SCENES: PIJ1, DJ2, PIJ2, DJ3
	if sel[scene_name] then
		grea = self:add_grea( "WHITETIP_BB_SO2", "B_WHITETIP_BB", 6,        	16, "SO" )
    	-- STATIC - Doivent rester sur place !!! Trop grand 14/07
			grea:init_box_xyz( 5,11, -1.5,-3, -2,3 )
    end

-- DOIT JOUER DANS LES SCENES:	PIJ2, PIJ3, PIN1
	--  EAU DANS LA BOUCHE (BLEU) de loin
	-- grea = self:add_grea( "SHRK_WHAL_SO", "B_SHRK_WHAL", 1,            		17, "SO" )
	-- -- Target a fixer sur le troue -- Possible de l'ajouter en KC pour une transition SO to KC et inversement
	-- -- Naissance - Mort ou donner une direction afin de s'assurer qu'il passe sous le troue
	-- 	grea:init_box_xyz( -5,35, -5,-10, -4,4 )



-- DOIT JOUER DANS LES SCENES: PIJ1, DJ2, PIJ2, DJ3
-- TO ADD WHEN RECEIVED ROM KI
    -- grea = self:add_grea( "CUTTLE_S01", "B_CUTTLE", 1,               19, "SO" )
    --  grea:init_box_xyz( 0,4.90, -0.30,-1, 0,-2 )
    -- grea = self:add_grea( "CUTTLE_S02", "B_CUTTLE", 1,               20, "SO" )
    --  grea:init_box_xyz( 5.30,10 , -0,30,-1, 1,3 )
    --  OR
	-- grea = self:add_grea( "B_CUTTLE_SO12", "B_CUTTLE", 3,            21, "SO" )
	--  grea:init_box_xyz( 0,10,12 -0.30,-1, -3,-3 )

-- DOIT JOUER DANS LES SCENES: DN1, PIN2, DN2, PIN3
-- TO ADD WHEN RECEIVED ROM KI
	-- grea = self:add_grea( "B_CUTTLE_HYPNO_SO12", "B_CUTTLE_HYPNO", 5,            22, "SO" )
	--  grea:init_box_xyz( 0,10,12 -0.30,-0.80, -4,4 )
	-- grea = self:add_grea( "B_CUTTLE_HYPNO_SO34", "B_CUTTLE_HYPNO", 5,            23, "SO" )
	--  grea:init_box_xyz( 12,23 -0.30,-0.80, -4,4 )

-- DOIT JOUER DANS LES SCENES: DN1, PIN2, PIN3
	sel = { DN1=true, PIN2=true, PIN3=true }
	if sel[scene_name] then
		grea = self:add_grea( "FLASH_SO_2", "B_FLASH", 800, 					24, "SO" )
			grea:init_box_xyz( 0,21, -0.05,-0.40, -3.9,3.9 )
	end

-- DOIT JOUER DANS LES SCENES: DN1, PIN2, PIN3
	sel = { DN1=true, PIN2=true, PIN3=true }
	if sel[scene_name] then
		grea = self:add_grea( "EAGLES", "B_EAGLE", 4, 					25, "SO" )
			grea:init_box_xyz( 0,21, 3.15,4.40, -3.9,3.9 )
	end

     -- A UTILISER POUR SHADOW SO, from ID 35:
     grea = self:add_grea( "TURTL_SHADO", "B_TURTL", 20,                  	35, "SO" )
        grea:init_box_xyz( -2,21, 4,8, 4,-4 )
     grea = self:add_grea( "SHRK_WHAL_SHADO", "B_SHRK_WHAL", 1,            36, "SO" )
		grea:init_box_xyz( -5,40, 10,40, -10,10 )
	grea = self:add_grea( "WHITETIP_SHADOW", "B_WHITETIP", 6,        	37, "SO" )
		grea:init_box_xyz( -2,23, 4,10, -10,10 )


end
