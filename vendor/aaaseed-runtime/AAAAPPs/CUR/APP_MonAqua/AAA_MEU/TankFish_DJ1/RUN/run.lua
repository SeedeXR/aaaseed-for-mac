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

	grea = self:add_grea( "BANNER_KL234", "B_BANNER", 60,                 		1, "KL234" )
		--grea:init_box_xyz( 5,20, 0.5,4, -4,-6 )
		--grea:set_random_walk({
			--speed 			=    0.7,
			--b_is_stationary =   true,
			--time_stationary =    1.5,	-- required since it's a stationary random walk.
			--time_moving		=    8.5,	-- required since it's a stationary random walk.
--			border 			=    0.5,	-- optionnal
			--radius_max 		=    1.28,
			--radius_min 		=    0.64  	-- optional if radius_max is already defined (will be set to radius_max)

	grea = self:add_grea( "BANNER_KC", "B_BANNER", 20,                          2,  "KC" )
		--grea:init_box_xyz( 21,23, 0.5,2.5, -4,4 )
		--grea:set_random_walk({
			--speed 			=    0.7,
			--b_is_stationary =   true,
			--time_stationary =    1.5,	-- required since it's a stationary random walk.
			--time_moving		=    8.5,	-- required since it's a stationary random walk.
--			border 			=    0.5,	-- optionnal
			--radius_max 		=    0.44,
-- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
--		})

	--- stopper la box pour laisser place a l'animation MANTA
	--- relancer la box apres l'animation MANTA
	grea = self:add_grea( "BANNER_KR21", "B_BANNER", 30,                       3,  "KR12" )
		--grea:init_box_xyz( -0.2,10, 1.5,3, 4.5,7 )
		--grea:set_random_walk({
			--speed 			=    0.7,
			--b_is_stationary =   true,
			--time_stationary =    1.5,	-- required since it's a stationary random walk.
			--time_moving		=    8.5,	-- required since it's a stationary random walk.
--			border 			=    0.5,	-- optionnal
			--radius_max 		=    0.75,
-- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
--		})


	--- utiliser l'outil naissance/mort une fois celui-ci disponible
	grea = self:add_grea( "CARANG_KL", "B_CARANG", 12,                          4,  "KL" )
		--grea:init_box_xyz( 21,-4, 1.40,4, -4.5,-6.5 )


	grea = self:add_grea( "PARROT_BL_M_KL23", "B_PARROT_BL_M", 14,         		5,  "KL23" )
		--grea:init_box_xyz( 5,16, 0.5,3, -4.8,-8 )
		--grea:set_random_walk({
			--speed 			=    0.3,
			--b_is_stationary =   true,
			--time_stationary =    1.5,	-- required since it's a stationary random walk.
			--time_moving		=    8.5,	-- required since it's a stationary random walk.
--			border 			=    0.5,	-- optionnal
			--radius_max 		=    1.2,
-- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
--		})
	--- stopper la box pour laisser place a l'animation MANTA
	--- relancer la box apres l'animation MANTA
	grea = self:add_grea( "PARROT_BL_F_KR23", "B_PARROT_BL_F", 10,               6,  "KR23" )
		--grea:init_box_xyz( 5,16, 0.5,3, 4.8,8 )
		--grea:set_random_walk({
			--speed 			=    0.3,
			--b_is_stationary =   true,
			--time_stationary =    1.5,	-- required since it's a stationary random walk.
			--time_moving		=    8.5,	-- required since it's a stationary random walk.
--			border 			=    0.5,	-- optionnal
			--radius_max 		=    1.2,
-- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
--		})

--OUT UNTIL FIXED, BUGGED, FLICKER, KI
	grea = self:add_grea( "GRACE_KL", "B_GRACE", 1,                             7,  "KL" )
		--grea:init_box_xyz( 0,21, 0.8,2.5, -4.5,-6.5 )

	grea = self:add_grea( "CONVICT_KL", "B_CONVICT", 15,                     8,  "KL" )
		--grea:init_box_xyz( 0,21, 0.8,1.50, -5,-7 )

--- disabled because of flicker - TODO get new version from KiStudios and apply
-- OUT UNTIL FIXED, BUGGED, FLICKER, KI
--     grea = self:add_grea( "CLOWN_KC", "B_CLOWN", 1,                             9, "KC" )
--         grea:init_box_xyz( 21,23, 0.5,1.7, -4.5,4.5 )
--     grea = self:add_grea( "CLOWN_KL", "B_CLOWN", 1,                             10, "KL" )
--         grea:init_box_xyz( 0,21, 0.5,1.5, -4.5,-5.5 )
--         grea:set_random_walk({
--             speed 			=    0.3,
--             b_is_stationary =   true,
--             time_stationary =    1.5,	-- required since it's a stationary random walk.
--             time_moving		=    8.5,	-- required since it's a stationary random walk.
-- --			border 			=    0.5,	-- optionnal
--             radius_max 		=    1.2,
-- -- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
--         })
--- disabled because of flicker - TODO get new version from KiStudios and apply
--     grea = self:add_grea( "CLOWN_KR", "B_CLOWN", 1,                             11, "KR"  } )
--         grea:init_box_xyz( 0,21, 0.5,1.6, 4.5,5.5 )
--         grea:set_random_walk({
--             speed 			=    0.3,
--             b_is_stationary =   true,
--             time_stationary =    1.5,	-- required since it's a stationary random walk.
--             time_moving		=    8.5,	-- required since it's a stationary random walk.
-- --			border 			=    0.5,	-- optionnal
--             radius_max 		=    1.2,
-- -- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
--         })

	grea = self:add_grea( "BUTTER_KL", "B_BUTTER", 20,                          12, "KL" )
		--grea:init_box_xyz( 0,21, 1.5,3.5, -4.5,-6 )
	grea = self:add_grea( "BUTTER_KR12", "B_BUTTER", 20,                        13, "KR12")
		--grea:init_box_xyz( -0.2,10, 1.2,3, 5,8 )

	grea = self:add_grea( "SURGERY_KL", "B_SURGERY", 7,                         14, "KL" )
		--grea:init_box_xyz( -0.2,21, 1,2.3, -4.5,-6 )
	grea = self:add_grea( "SURGERY_KC", "B_SURGERY", 5,                         15, "KC" )
		--grea:init_box_xyz( 21,24, 1.0,1.8, -4,4 )

	-- todo: rafiner, looks like box issue !!!
	-- grea = self:add_grea( "SURGERY_KR", "B_SURGERY", 6,                         16, "KR"  )
	--     grea:init_box_xyz( 0,21, 0.3,1.1, 5,7 )

	-- conflit entre box et sphere,
	-- difficile a regler, y revenir
--     grea = self:add_grea( "BARAK_KR12", "B_BARAK", 15,                          17, { "KR12" } ) --rétrécir la taille des BARAK --PluS STATIC et doivent rester autour du KR3 pour une anim'
--         grea:init_box_xyz( -2,10, 1.8,3.2, 4.5,6.5 )
--         grea:set_random_walk({
--             speed 			=    0.3,
--             b_is_stationary =   true,
--             time_stationary =    1.5,	-- required since it's a stationary random walk.
--             time_moving		=    8.5,	-- required since it's a stationary random walk.
-- --			border 			=    0.5,	-- optionnal
--             radius_max 		=    1.4,
--  		--	radius_min 		=    0.4  	-- optional if radius_max is already defined (will be set to radius_max)
--         })


	-- a activer apres animation MANTA
	grea = self:add_grea( "TRIGGER_KR3", "B_TRIGGER", 1,                        18, "KR3" )
		--grea:init_box_xyz( 9,16, 1.50,2, 5,7 )
		--grea:set_random_walk({
			--speed 			=    0.3,
			--b_is_stationary =   true,
			--time_stationary =    1.5,	-- required since it's a stationary random walk.
			--time_moving		=    8.5,	-- required since it's a stationary random walk.
--			border 			=    0.5,	-- optionnal
			--radius_max 		=    0.2,
-- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
--		})


	-- OUT FOR NOW, BUGGED, SEE WITH KI STUDIO
	-- grea = self:add_grea( "TITAN_KR3", "B_TITAN", 1,                         19, "KR3" ) -- semble un peu gros
	--     grea:init_box_xyz( 10,15, 1.40,2, 4.5,7.5 )
--         grea:set_random_walk({
--             speed 			=    0.3,
--             b_is_stationary =   true,
--             time_stationary =    1.5,	-- required since it's a stationary random walk.
--             time_moving		=    8.5,	-- required since it's a stationary random walk.
-- --			border 			=    0.5,	-- optionnal
--             radius_max 		=    0.2,
-- -- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
--         })

	grea = self:add_grea( "PARROT_KC_L", "B_PARROT", 6,                           20, "KC" )
		--grea:init_box_xyz( 22,28, 0.0,3.6, -4.4,-1.0 )
		--grea:set_random_walk({
			--speed 			=    0.33,
			--b_is_stationary =   true,
			--time_stationary =    1.0,	-- required since it's a stationary random walk.
			--time_moving		=    9.5,	-- required since it's a stationary random walk.
--			border 			=    0.5,	-- optionnal
			--radius_max 		=    1.2,
-- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
--		})
	grea = self:add_grea( "PARROT_KC_R", "B_PARROT", 6,                           21, "KC" )
		--grea:init_box_xyz( 22,28, 0.0,3.6,  1.0, 4.4 )
		--grea:set_random_walk({
			--speed 			=    0.33,
			--b_is_stationary =   true,
			--time_stationary =    1.0,	-- required since it's a stationary random walk.
			--time_moving		=    0.001,	-- required since it's a stationary random walk.
--			border 			=    0.5,	-- optionnal
			--radius_max 		=    1.2,
-- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
--		})

	-- ON WHEN RECEIVED --
	 grea = self:add_grea( "CUTTLE_KL2", "B_CUTTLE", 1,                       22, "KL2" )
	  grea:init_box_xyz( 5,10, 0.05,0.50, -4.1, -5.5 )
	 grea = self:add_grea( "CUTTLE_KL3", "B_CUTTLE", 1,                       23, "KL3" )
	  grea:init_box_xyz( 13,15, 0.05,0.50, -4.1,-5.5 )
	 grea = self:add_grea( "CUTTLE_KL4", "B_CUTTLE", 1,                       24, "KL4" )
	  grea:init_box_xyz( 15.6,20, 0.25,0.60, -4.1,-5 )

	grea = self:add_grea( "CLEAN_KL123", "B_CLEAN", 20,                         25,  "KL123" )
		--grea:init_box_xyz( -0.5,15, 0.50,2.50, -4.7,-6 )
	grea = self:add_grea( "CLEAN_KR3", "B_CLEAN", 15,                           26,  "KR3" )
		--grea:init_box_xyz( 9,16, 1.5,2, 5,7 )
	grea = self:add_grea( "CLEAN_KR12", "B_CLEAN", 12,                          27,  "KR12" )
		--grea:init_box_xyz( 0.5,10, 1.1,1.6, 4,6 )

		grea = self:add_grea( "CARANG_KR", "B_CARANG", 12,                          28,  "KR" )
		--grea:init_box_xyz( 21,-4, 1.40,4, -4.5,-6.5 )

    -- BEE & GA called from their own FishTank now


end
