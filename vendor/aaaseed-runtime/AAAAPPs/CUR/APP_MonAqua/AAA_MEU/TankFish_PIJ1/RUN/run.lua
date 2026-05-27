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

function TANK:init_fish()
	local grea
	-- TANK:add_grea( name, race_name, nb, boid_id )
	-- BOID ID from 1 to 48

	grea = self:add_grea( "Simple",			"B_SIMPLE",			100,		64	)

--	grea = self:add_grea( "poisson_lune", 	"poissonLune",			300,	63	)
--	grea = self:add_grea( "testslot_1", 	"testslot1",			300,	62	)
--	grea = self:add_grea( "testslot_2", 	"testslot2",			300,	61	)

--define_grea_b2d ( "B2D_WHITETIPBB", "B2D_WHITETIPBB", { "SO2" }, 6, x, y, z, rotation & degré,"00:00:00")
--define_grea_anim( "A_TURTLE_1", "A_TURTLE", { "KL4" })
-- define_grea_T( "T_TURTLE_2", "T_TURTLE")


		-- REPOUSSE TOUTES LES ESPECES
		grea = self:add_grea( "MAORI_KR", "B_MAORI", 1,                         1, "KR" )
			grea:init_box_xyz( -3,22, 1.8,4, 6,19 )
            grea:set_random_walk({
                speed 			=    0.7,
                b_is_stationary =   true,
                time_stationary =    1.5,	-- required since it's a stationary random walk.
                time_moving		=    8.5,	-- required since it's a stationary random walk.
    --			border 			=    0.5,	-- optionnal
                radius_max 		=    2.0,
    -- 			radius_min 		=    0.64  	-- optional if radius_max is already defined (will be set to radius_max)
            })

		grea = self:add_grea( "BAT_KL234", "B_BAT", 10,                         2, "KL234" )  -- Interactif avec les visteurs - Target proche de lécran
			--grea:init_box_xyz( 5,21, 1,2.5, -4.3,-9 )
            --grea:set_random_walk({
                --speed 			=    0.5,
                --b_is_stationary =    true,
                --time_stationary =    2,	-- required since it's a stationary random walk.
                --time_moving		=    8.5,	-- required since it's a stationary random walk.
    --			border 			=    0.5,	-- optionnal
                --radius_max 		=    1.4,
    -- 			radius_min 		=    0.64  	-- optional if radius_max is already defined (will be set to radius_max)
--            })

		-- demi tour FLICKER
		grea = self:add_grea( "BAT_KC", "B_BAT", 10,                            3, "KC" )
			--grea:init_box_xyz( 22,27, 1,5, -4.3,4.3 )
		grea = self:add_grea( "BAT_KR", "B_BAT", 15,                            4, "KR" )
			grea:init_box_xyz( -1,21, 0.5,5, 4.3,9 )

		grea = self:add_grea( "BANNER_KL", "B_BANNER", 90,                   5, "KL" ) -- banner fonctionnent par banc
			grea:init_box_xyz( -1,23, 0.5,2.5, -4.3,-6.5 )
            --grea:set_random_walk({
                --speed 			=    0.7,
                --b_is_stationary =   true,
                --time_stationary =    1.5,	-- required since it's a stationary random walk.
                --time_moving		=    8.5,	-- required since it's a stationary random walk.
    --			border 			=    0.5,	-- optionnal
                --radius_max 		=    1.28,
     			--radius_min 		=    0.64  	-- optional if radius_max is already defined (will be set to radius_max)
            --})
		grea = self:add_grea( "BANNER_KC", "B_BANNER", 50,                     6, "KC" )
			grea:init_box_xyz( 21.5,24.3, 0.5,4, -4,4 )
            --grea:set_random_walk({
                --speed 			=    0.7,
                --b_is_stationary =   true,
                --time_stationary =    1.5,	-- required since it's a stationary random walk.
                --time_moving		=    8.5,	-- required since it's a stationary random walk.
    --			border 			=    0.5,	-- optionnal
                --radius_max 		=    0.44,
    -- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
            --})
		grea = self:add_grea( "BANNER_KR", "B_BANNER", 60,                   7,  "KR" )
			grea:init_box_xyz( -1,23, 1,2, 4.3,6.5 )
            --grea:set_random_walk({
                --speed 			=    0.7,
                --b_is_stationary =   true,
                --time_stationary =    1.5,	-- required since it's a stationary random walk.
                --time_moving		=    8.5,	-- required since it's a stationary random walk.
    --			border 			=    0.5,	-- optionnal
                --radius_max 		=    0.44,
    -- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
            --})

	-- OUT, U-Turn issue, test and apply back
		-- grea = self:add_grea( "WHITETIP_KR", "B_WHITETIP", 2,             8,  "KR" ) -- Dorment la journee, se baladent ou evoluent vers le bas, isolé
		-- 	grea:init_box_xyz( -2,22, 0.8,2, 5,9 )
		-- grea = self:add_grea( "WHITETIP_KC", "B_WHITETIP", 3,                9, "KC" )
		-- 	grea:init_box_xyz( 21,26, 0.5,2.5, -5,5 )
		-- grea = self:add_grea( "WHITETIP_KL", "B_WHITETIP", 5,             10, "KL" )
		-- 	grea:init_box_xyz( -2,22, 0.5,3, -5,-9 )

		--- utiliser l'outil naissance/mort une fois celui-ci disponible
        grea = self:add_grea( "CARANG_KL", "B_CARANG", 12,                      11, "KL" )
            grea:init_box_xyz( 21,-4, 1.40,4, -4.3,-6.5 )
		grea = self:add_grea( "CARANG_KC", "B_CARANG", 8,                       12, "KC" ) -- reduire la taille
			grea:init_box_xyz( 22,27, 1.4,4.3, -5,5 )
		grea = self:add_grea( "CARANG_KR", "B_CARANG", 15,                      13, "KR" )
			grea:init_box_xyz( 22,-2, 1.4,4, 5,9 )

		grea = self:add_grea( "PARROT_BL_M_KL", "B_PARROT_BL_M", 24,           14,  "KL" )
		    --grea:init_box_xyz( -0.5,22, 0.5,2, -4.3,-8 )
		grea = self:add_grea( "PARROT_BL_F_KR3", "B_PARROT_BL_F", 20,           15,  "KR" )
			--grea:init_box_xyz( -0.5,22, 0.5,2, 4.3,8 )
-- PARROT IS GOING OUT OF HIS BOX

		-- target monte tout en haut -- should check
		grea = self:add_grea( "POTATO_KC", "B_POTATO", 1,                     16, "KC" )
		--	grea:init_box_xyz( 5,22.5, 1.6,4, -7,7 )

		-- august do it in GA
		-- target monte tout en haut -- should check
		grea = self:add_grea( "CORNET_KALL", "B_CORNET", 8,                  	17, "KALL" )
			--grea:init_box_xyz( -2,22.5, 1,4, -7,7 )

-- OUT UNTIL FIXED, BUGGED, FLICKER, KI
		grea = self:add_grea( "GRACE_KL", "B_GRACE", 1,                         18, "KL" ) -- Attention, quand touché, changement de texture + part se cacher (priorité sur se cacher)
			grea:init_box_xyz( 0,21, 1,2.5, -4.3,-6 )

        grea = self:add_grea( "CONVICT_KL", "B_CONVICT", 15,                    19, "KL" )
            grea:init_box_xyz( 0,21, 0.8,1.50, -5,-7 )

-- OUT UNTIL FIXED, BUGGED, FLICKER, KI
		-- grea = self:add_grea( "CLOWN_KC", "B_CLOWN", 2,                         20, "KC" )  -- les clows doivent rester assez bas -- ATTENTION mauvaise taille, a agrandir ++
		-- 	grea:init_box_xyz( 22,25, 1,4, -5,5 )
		-- grea = self:add_grea( "CLOWN_KL", "B_CLOWN", 1,                         21, "KL" )
		-- 	grea:init_box_xyz( -1,22, 0.5,3, -4.3,7 )
		-- grea = self:add_grea( "CLOWN_KR", "B_CLOWN", 1,                         22, "KR" )
		-- 	grea:init_box_xyz( -1,22, 0.5,3, 4.3,7 )

        grea = self:add_grea( "BUTTER_KL", "B_BUTTER", 30,                          23, "KL" )
            grea:init_box_xyz( -0.5,21.5, 1.5,3, -4.3,-6 )
        grea = self:add_grea( "BUTTER_KR", "B_BUTTER", 50,                        24, "KR" )
			grea:init_box_xyz( -0.5,21.5, 1.5,3, 4.3,6 )

        grea = self:add_grea( "SURGERY_KL", "B_SURGERY", 15,                         25, "KL" )
            grea:init_box_xyz( -0.5,21, 1,2.3, -4.3,-6 )
        grea = self:add_grea( "SURGERY_KC", "B_SURGERY", 10,                         26, "KC" )
            grea:init_box_xyz( 21,24, 1.0,1.8, -4,4 )

        -- conflit entre box et sphere,
        -- difficile a regler, y revenir
    --     grea = self:add_grea( "BARAK_KR12", "B_BARAK", 15,                          27, { "KR12" ) --rétrécir la taille des BARAK --PluS STATIC et doivent rester autour du KR3 pour une anim'
    --         grea:init_box_xyz( -2,10, 1.8,3.2, 4.3,6.5 )
    --         grea:set_random_walk({
    --             speed 			=    0.3,
    --             b_is_stationary =   true,
    --             time_stationary =    1.5,	-- required since it's a stationary random walk.
    --             time_moving		=    8.5,	-- required since it's a stationary random walk.
    -- --			border 			=    0.5,	-- optionnal
    --             radius_max 		=    1.4,
    --  		--	radius_min 		=    0.4  	-- optional if radius_max is already defined (will be set to radius_max)
    --         })

        grea = self:add_grea( "TRIGGER_KR3", "B_TRIGGER", 1,                        28, "KR3" )
            --grea:init_box_xyz( 9,16, 1.50,2, 5,7 )
            --grea:set_random_walk({
                --speed 			=    0.3,
               -- b_is_stationary =   true,
                --time_stationary =    1.5,	-- required since it's a stationary random walk.
                --time_moving		=    8.5,	-- required since it's a stationary random walk.
    --			border 			=    0.5,	-- optionnal
                --radius_max 		=    0.2,
    -- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
            --})

		-- august do it in GA
		grea = self:add_grea( "LION_KALL", "B_LION", 1,                        	29, "KALL" )
			--grea:init_box_xyz( -1,23, 1,5, 5,7 )
			--grea:set_go_around({
				--b_is_rotation 		=  true,
				--b_begin_clockwise 	= false,
				--duration			=  110,	-- Temps pour que la Target fasse le tour de la BOX en s
				--begin_pos			=    0,	-- Position de départ en degrées de la Target, centre en x=11m et z=0m (centre de la salle), 0 au Nord (KC) puis dans le sens des aiguiles d'une montre, 90' (Est, KR)  180' ou -180' (Sud). -90' (Ouest, KL)
				--depth_min			=    2,	-- Allowed distance from the ROOM (from Kakemono) in m
				--depth_max			=   10,
				--radius_min			=    1,	 -- Size of the moving target in m
				--radius_max			=    2, 	 -- Size of the sphere around the Target in m
				--min_pos				= 	 0,
				--max_pos				= 	 0,
				--y_min				= 1.4,
				--y_max				= 2.0
			--})

		-- LIPS A et LIPS B doivent se suivre tel un seul BANC
		grea = self:add_grea( "LIPS_A_KL", "B_LIPS_A", 20,                 30, "KL" )
			grea:init_box_xyz( -1,22.5, 1.4,5, -5,-8 )

		grea = self:add_grea( "LIPS_B_KR", "B_LIPS_B", 14,                 31, "KR" )
			grea:init_box_xyz( -1,22.5, 1.4,5.5, 5,8 )

		-- TO ADD WHEN FISH RECEIVED FRON KI
		-- grea = self:add_grea( "CUTTLE_KL4", "B_CUTTLE", 1,                   32, "KL4" )
		--  grea:init_box_xyz( 16,21, 0.20,0.50, -4,-5 )

        grea = self:add_grea( "CLEAN_KL", "B_CLEAN", 40,                         33, "KL" )
            grea:init_box_xyz( -0.5,21, 0.50,2, -4.7,-6 )
        grea = self:add_grea( "CLEAN_KR3", "B_CLEAN", 20,                           34, "KR3" )
            grea:init_box_xyz( 10,15, 1.5,2.10, 5,7 )
        grea = self:add_grea( "CLEAN_KR12", "B_CLEAN", 20,                          35, "KR12" )
			grea:init_box_xyz( 0.5,10, 1.1,1.6, 4,6 )

		grea = self:add_grea( "TURTL_KL", "B_TURTL", 5,              		    	36, "KL" )
			-- Go Around, but not together
			grea:init_box_xyz(-1,23, 1,3, -6,-15 )
		grea = self:add_grea( "TURTL_KR", "B_TURTL", 4,              		    	37, "KR" )
			-- Go Around, but not together
			grea:init_box_xyz(-1,23, 1,3, 6,15 )

		grea = self:add_grea( "SURGERY_KR", "B_SURGERY", 15,                         38, "KL" )
            grea:init_box_xyz( -0.5,21, 1,2.3, 4.3,6 )

		grea = self:add_grea( "CUTTLE_KL2", "B_CUTTLE", 1,                       39, "KL2" )
			grea:init_box_xyz( 5,10, 0.05,0.50, -4.1, -5.5 )
		grea = self:add_grea( "CUTTLE_KL3", "B_CUTTLE", 1,                       40, "KL3" )
			grea:init_box_xyz( 13,15, 0.05,0.50, -4.1,-5.5 )
		grea = self:add_grea( "CUTTLE_KL4", "B_CUTTLE", 1,                       41, "KL4" )
			grea:init_box_xyz( 15.6,20, 0.25,0.60, -4.1,-5 )

-- BEE & GA called from their own FishTank now

end
