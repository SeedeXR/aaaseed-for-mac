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


--define_grea_anim( "C_DOLPH", "C_DOLPH", { "KR4", "KR3", "KR2" } )
--define_grea_anim( "C_WHAL", "C_WHAL", { "KR4", "KR3", "KR2" } )
--define_grea_anim( "C_WHALBB", "C_WHALBB", { "KR4", "KR3", "KR2" } )
--define_grea_b2d ( "B2D_WHITETIPBB", "B2D_WHITETIPBB", { "SO2" }, 6, x, y, z, rotation & degré,"00:00:00")
--define_grea_c2d( "C2D_CARPET_1", "C2D_CARPET", { "SO1", "SO2" } )
--define_grea_c2d( "C2D_CARPET_2", "C2D_CARPET", { "SO1", "SO2" } )


	-- coince dans le decor - impossible a tester
	-- Doit idealement passer de KC a SO (afin d'etre appercu par le troue dans le decore)
	-- grea = self:add_grea( "SHRK_WHAL_KC", "B_SHRK_WHAL", 1,            1, "KC" )
	-- 	grea:init_box_xyz( 30,70, 1,9, -15,15 )

	grea = self:add_grea( "BANNER_KL34", "B_BANNER", 60,                   2, "KL34" ) -- banner fonctionnent par banc
        grea:init_box_xyz( 9,20, 0.5,4, -4.5,-6.5 )
        --grea:set_random_walk({
            --speed 			=    0.7,
            --b_is_stationary =   true,
            --time_stationary =    1.5,	-- required since it's a stationary random walk.
            --time_moving		=    8.5,	-- required since it's a stationary random walk.
--			border 			=    0.5,	-- optionnal
            --radius_max 		=    1.28,
            --radius_min 		=    0.64  	-- optional if radius_max is already defined (will be set to radius_max)
		--})

	grea = self:add_grea( "BAT_KC", "B_BAT", 10,                            3, "KC"	)
		grea:init_box_xyz( 22,27, 1,5, -4.5,4.5 )

--probleme because same name
-- 	grea = self:add_grea( "BANNER_KL234", "B_BANNER", 60,                   5, { "KL2", "KL3", "KL4" } ) -- banner fonctionnent par banc
--         grea:init_box_xyz( 9,20, 0.5,4, -4.5,-6.5 )
--         grea:set_random_walk({
--             speed 			=    0.7,
--             b_is_stationary =   true,
--             time_stationary =    1.5,	-- required since it's a stationary random walk.
--             time_moving		=    8.5,	-- required since it's a stationary random walk.
-- --			border 			=    0.5,	-- optionnal
--             radius_max 		=    1.28,
--             radius_min 		=    0.64  	-- optional if radius_max is already defined (will be set to radius_max)
--         })
	grea = self:add_grea( "BANNER_KC", "B_BANNER", 20,                     6, "KC" )
		grea:init_box_xyz( 21.5,24.5, 0.5,4, -4,4 )
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
	-- grea = self:add_grea( "WHITETIP_KL", "B_WHITETIP", 5,             10, "KL4" )
	-- 	grea:init_box_xyz( -2,22, 0.5,3, -5,-9 )

	grea = self:add_grea( "CARANG_KC", "B_CARANG", 8,                       11, "KC" ) -- reduire la taille
		grea:init_box_xyz( 22,27, 1.4,4.5, -5,5 )

	grea = self:add_grea( "PARROT_BL_M_KL", "B_PARROT_BL_M", 14,                13,  "KL" )
		grea:init_box_xyz( -0.5,22, 0.5,2, -4.5,-8 )


		-- ID 14 free ?

	grea = self:add_grea( "POTATO_KALL", "B_POTATO", 1,                     15, "KALL" )
		--grea:init_box_xyz( 5,22.5, 1.6,5, -7,7 )

		-- OUT? A remettre
	-- grea = self:add_grea( "CORNET_KALL", "B_CORNET", 8,       		   	16, "KALL" )
	-- 	grea:init_box_xyz( 5,22.5, 1.8,7, -6,6 )

-- OUT UNTIL FIXED, BUGGED, FLICKER, KI
		grea = self:add_grea( "GRACE_KL", "B_GRACE", 1,                         17, "KL" ) -- Attention, quand touché, changement de texture + part se cacher (priorité sur se cacher)
		grea:init_box_xyz( 0,20, 0.8,2.5, -4.5,-6 )
	-- 	grea:set_random_walk({
	-- 		speed 			=    0.3,
	-- 		b_is_stationary =   true,
	-- 		time_stationary =    1.0,	-- required since it's a stationary random walk.
	-- 		time_moving		=    8.5,	-- required since it's a stationary random walk.
	-- --			border 			=    0.5,	-- optionnal
	-- 		radius_max 		=    1.2,
	-- -- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)

	-- })


-- OUT UNTIL FIXED, BUGGED, FLICKER, KI
	-- grea = self:add_grea( "CLOWN_KC", "B_CLOWN", 2,                         18, "KC" )  -- les clows doivent rester assez bas -- ATTENTION mauvaise taille, a agrandir ++
	-- 	grea:init_box_xyz( 22,25, 1,4, -5,5 )
	-- grea = self:add_grea( "CLOWN_KL", "B_CLOWN", 1,                         19, "KL" } )
	-- 	grea:init_box_xyz( -1,22, 0.5,3, -4.5,7 )

	grea = self:add_grea( "BUTTER_KL", "B_BUTTER", 20,                          20, "KL" )
		grea:init_box_xyz( 0,21, 1.5,3.5, -4.5,-6 )

	grea = self:add_grea( "SURGERY_KL", "B_SURGERY", 7,                         21, "KL" )
		grea:init_box_xyz( -0.2,21, 1,2.3, -4.5,-6 )
	grea = self:add_grea( "SURGERY_KC", "B_SURGERY", 5,                         22, "KC" )
		grea:init_box_xyz( 21,24, 1.0,1.8, -4,4 )

-- OUT FOR NOW, BUGGED, SEE WITH KI STUDIO
	-- grea = self:add_grea( "TITAN_KR3", "B_TITAN", 1,                        24,	"KR3" ) -- semble un peu gros
	-- 	grea:init_box_xyz( 10,15, 1.40,2, 4.5,7.5 )

	grea = self:add_grea( "DOLPH_KC", "B_DOLPH",         3 ,             	25, "KC" )
		grea:init_box_xyz( 25,50, 2,8, -6,6 )
-- 		grea:set_random_walk({
-- 			speed 			=    1,
-- 			b_is_stationary =   true,
-- 			time_stationary =    1.5,	-- required since it's a stationary random walk.
-- 			time_moving		=    8.5,	-- required since it's a stationary random walk.
-- --			border 			=    0.5,	-- optionnal
-- 			radius_max 		=    6.0,
-- 			radius_min 		=    0.6  	-- optional if radius_max is already defined (will be set to radius_max)
-- 		})


	grea = self:add_grea( "CONVICT_KL", "B_CONVICT", 15,                    30, "KL" )
		grea:init_box_xyz( 0,21, 0.8,1.50, -5,-7 )


	grea = self:add_grea( "CLEAN_KR12", "B_CLEAN", 12,                          31,  "KR12" )
		grea:init_box_xyz( 0.5,10, 1.1,1.6, 4,6 )

	grea = self:add_grea( "BUTTER_KR12", "B_BUTTER", 20,                        32,  "KR12" )
		grea:init_box_xyz( -0.2,10, 1.2,3, 5,8 )

-- BEE & GA called from their own FishTank now
	grea = self:add_grea( "CUTTLE_KL2", "B_CUTTLE", 1,                       33, "KL2" )
		grea:init_box_xyz( 5,10, 0.05,0.50, -4.1, -5.5 )
	grea = self:add_grea( "CUTTLE_KL3", "B_CUTTLE", 1,                       34, "KL3" )
		grea:init_box_xyz( 13,15, 0.05,0.50, -4.1,-5.5 )
	grea = self:add_grea( "CUTTLE_KL4", "B_CUTTLE", 1,                       35, "KL4" )
		grea:init_box_xyz( 15.6,20, 0.25,0.60, -4.1,-5 )


	end
