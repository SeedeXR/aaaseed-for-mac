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


        grea = self:add_grea( "BAT_KL234", "B_BAT", 10,                         1, "KL234" ) -- Reduire la taille -- Interactif avec les visteurs - Target proche de lécran
        --grea:init_box_xyz( 5,21, 1,2.5, -4.5,-9 )
        --grea:set_random_walk({
            --speed 			=    0.5,
            --b_is_stationary =    true,
            --time_stationary =    2,	-- required since it's a stationary random walk.
            --time_moving		=    8.5,	-- required since it's a stationary random walk.
        --			border 			=    0.5,	-- optionnal
            --radius_max 		=    1.4,
        -- 			radius_min 		=    0.64  	-- optional if radius_max is already defined (will be set to radius_max)
        --})
		grea = self:add_grea( "BAT_KR", "B_BAT", 15,                            3, "KR" )
			grea:init_box_xyz( -1,21, 0.5,5, 4.5,9 )

        grea = self:add_grea( "BANNER_KL34", "B_BANNER", 30,                   4, "KL34" ) -- banner fonctionnent par banc
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
        grea = self:add_grea( "BANNER_KR34", "B_BANNER", 25,                   6, "KR34" )
            grea:init_box_xyz( 9,20, 1,4, 4.5,6.5 )
            --grea:set_random_walk({
                --speed 			=    0.7,
                --b_is_stationary =   true,
                --time_stationary =    1.5,	-- required since it's a stationary random walk.
                --time_moving		=    8.5,	-- required since it's a stationary random walk.
    --			border 			=    0.5,	-- optionnal
                --radius_max 		=    0.44,
    -- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
            --})

        grea = self:add_grea( "CARANG_KL", "B_CARANG", 12,                          7,	"KL" )
            grea:init_box_xyz( 21,-4, 1.40,4, -4.5,-6.5 )
        grea = self:add_grea( "CARANG_KR", "B_CARANG", 15,                          8,	"KR" )
            grea:init_box_xyz( 22,-2, 1.4,4, 5,9 )

        grea = self:add_grea( "PARROT_BL_M_KL", "B_PARROT_BL_M", 14,                9,	"KL" )
            --grea:init_box_xyz( -0.5,22, 0.5,2, -4.5,-8 )
        grea = self:add_grea( "PARROT_BL_F_KR3", "B_PARROT_BL_F", 10,               10,	"KR" )
            --grea:init_box_xyz( -0.5,22, 0.5,2, 4.5,8 )

        grea = self:add_grea( "CONVICT_KL234", "B_CONVICT", 15,                    14, "KL234" )
            grea:init_box_xyz( 0,21, 0.8,1.50, -5,-7 )

-- OUT UNTIL FIXED, BUGGED, FLICKER, KI
        -- grea = self:add_grea( "CLOWN_KC", "B_CLOWN", 2,                     15, "KC" )
        --     grea:init_box_xyz( 21,22, 1,4, -4,4 )
        -- grea = self:add_grea( "CLOWN_KL", "B_CLOWN", 1,                     16, "KL" )
        --     grea:init_box_xyz( 0,20, 0.5,2.5, -4,-4.8 )
        -- grea = self:add_grea( "CLOWN_KR", "B_CLOWN", 1,                     17, "KR" )
        --     grea:init_box_xyz( 0,20, 0.5,2.5, 4,4.8 )

        grea = self:add_grea( "BUTTER_KL", "B_BUTTER", 20,                     18, "KL" )
            grea:init_box_xyz( 0,21, 1.5,3.5, -4.5,-6 )
        grea = self:add_grea( "BUTTER_KR12", "B_BUTTER", 20,                   19, "KR12" )
            grea:init_box_xyz( -0.2,10, 1.2,3, 5,8 )

        grea = self:add_grea( "SURGERY_KL", "B_SURGERY", 7,                    20, "KL" )
            grea:init_box_xyz( -0.2,21, 1,2.3, -4.5,-6 )

        -- REPOUSSE TOUTES LES ESPECES
        grea = self:add_grea( "MAORI_KR", "B_MAORI", 1,                        23, "KR" )
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


        grea = self:add_grea( "TRIGGER_KR3", "B_TRIGGER", 1,                   27, "KR3" )
            --grea:init_box_xyz( 9,16, 1.50,2, 5,7 )
            --grea:set_random_walk({
                --speed 			=    0.3,
                --b_is_stationary =   true,
                --time_stationary =    1.5,	-- required since it's a stationary random walk.
                --time_moving		=    8.5,	-- required since it's a stationary random walk.
    --			border 			=    0.5,	-- optionnal
                --radius_max 		=    0.2,
    -- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
            --})

-- OUT FOR NOW, BUGGED, SEE WITH KI STUDIO
        -- grea = self:add_grea( "TITAN_KR3", "B_TITAN", 1,                    28, "KR3" )
        --     grea:init_box_xyz( 10,15, 1.40,2, 4.5,7.5 )

-- OUT UNTIL FIXED, BUGGED, FLICKER, KI
        grea = self:add_grea( "GRACE_KL", "B_GRACE", 1,                     30, "KL" )
            grea:init_box_xyz( -0.40,21, 0.8,2.5, -4,-5 )
    --         grea:set_random_walk({
    --             speed 			=    0.3,
    --             b_is_stationary =   true,
    --             time_stationary =    1.0,	-- required since it's a stationary random walk.
    --             time_moving		=    8.5,	-- required since it's a stationary random walk.
    -- --			border 			=    0.5,	-- optionnal
    --             radius_max 		=    1.2,
    -- -- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
    --         })

    grea = self:add_grea( "CUTTLE_KL2", "B_CUTTLE", 1,                       31, "KL2" )
	    grea:init_box_xyz( 5,10, 0.05,0.50, -4.1, -5.5 )
	grea = self:add_grea( "CUTTLE_KL3", "B_CUTTLE", 1,                       32, "KL3" )
	    grea:init_box_xyz( 13,15, 0.05,0.50, -4.1,-5.5 )
	grea = self:add_grea( "CUTTLE_KL4", "B_CUTTLE", 1,                       33, "KL4" )
	    grea:init_box_xyz( 15.6,20, 0.25,0.60, -4.1,-5 )



-- BEE & GA called from their own FishTank now


end