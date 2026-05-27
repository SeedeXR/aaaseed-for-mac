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
	--self:print( "update_after()" )
--	if true then return end
--	if not self.races then return end

	if not app:is_pc_mas() then return end

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
	local fn_linear = math.fn_linear
	local grea, boid
	local time = app:get_seq_cur_time()
    aaa.show( time, "Eagle time" )

	grea = self:get_grea_by_name( "EAGLE_KL" )

	if grea then
		--table.print( grea, "Eagle debug", 2 )
		--self:print( app:get_seq_cur_time() )
        boid = grea:get_boid()
            if time<.2 then boid:trig_restart() end
		--boid:set_box_x( 12 + math.sin(time * .2 )*2 )
		local x,y,z = fn_linear( time,3,
		--time,		val1, val2  val3
			0,		  50,	10,   0,
            15,		  30,	4,   -2,
			22,		  21,   2,   -9,
            36,		  13,	4,   -6,
            43,		  11,	8,   -16,
			200,     -15,	1,  -250
        )
		boid:set_box_x(x)
        boid:set_box_y(y)
        boid:set_box_z(z)
    end

    grea = self:get_grea_by_name( "EAGLE_KR" )

	if grea then
		--table.print( grea, "Eagle debug", 2 )
		--self:print( app:get_seq_cur_time() )
        boid = grea:get_boid()
        if time<.2 then boid:trig_restart() end
		--boid:set_box_x( 12 + math.sin(time * .2 )*2 )
		local x,y,z = fn_linear( time,3,
		--time,		val1, val2  val3
            0,		  50,	10,   0,
            15,		  30,	4,    2,
            22,		  21,   2,    10,
            36,		  13,	4,    8,
            45,		   7,	4,    16,
            200,     -15,	8,    250
        )
		boid:set_box_x(x)
        boid:set_box_y(y)
        boid:set_box_z(z)
	end
end

function TANK:init_fish()
	local grea
	-- TANK:add_grea( name, race_name, nb, boid_id )
	-- BOID ID from 1 to 48

	grea = self:add_grea( "Simple",			"B_SIMPLE",			100,		64	)

--	grea = self:add_grea( "poisson_lune", 	"poissonLune",			300,	63	)
--	grea = self:add_grea( "testslot_1", 	"testslot1",			300,	62	)
--	grea = self:add_grea( "testslot_2", 	"testslot2",			300,	61	)


        grea = self:add_grea( "BAT_KL", "B_BAT", 10,                              2, "KL" ) -- Reduire la taille -- Interactif avec les visteurs - Target proche de lécran
            grea:init_box_xyz( -1.5,22, 0.5,5, -4.5,-9 )

		grea = self:add_grea( "BAT_KR", "B_BAT", 15,                              3, "KR" )
            grea:init_box_xyz( -1.5,22, 0.5,5, 4.5,9 )

        grea = self:add_grea( "BANNER_KL34", "B_BANNER", 60,                      4, "KL34" ) -- banner fonctionnent par banc
        grea:init_box_xyz( 9,20, 0.5,4, -4.5,-6.5 )
       -- grea:set_random_walk({
            --speed 			=    0.7,
            --b_is_stationary =   true,
            --time_stationary =    1.5,	-- required since it's a stationary random walk.
            --time_moving		=    8.5,	-- required since it's a stationary random walk.
--			border 			=    0.5,	-- optionnal
            --radius_max 		=    1.28,
            --radius_min 		=    0.64  	-- optional if radius_max is already defined (will be set to radius_max)
        --})
		grea = self:add_grea( "BANNER_KR34", "B_BANNER", 30,                       6,  "KR34" )
			grea:init_box_xyz( 9,20, 1,4, 4.5,6.5 )
            --grea:set_random_walk({
              --  speed 			=    0.7,
                --b_is_stationary =   true,
                --time_stationary =    1.5,	-- required since it's a stationary random walk.
                --time_moving		=    8.5,	-- required since it's a stationary random walk.
    --			border 			=    0.5,	-- optionnal
                --radius_max 		=    0.44,
    -- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
            --})

-- OUT UNTIL FIXED, BUGGED, FLICKER, KI
        grea = self:add_grea( "EAGLE_KL", "B_EAGLE", 15,                    		7,  { 	"KL1",	"KL2",	"KL3",	"KL4", "KC" } ) -- Flockin (se deplacent en formation)
    	--    grea:init_box_xyz( 10,12, 2,4, -5,-7 )
        --    grea:init_box_xyz( -2,22, 1,6, -5,-15 )

        grea = self:add_grea( "EAGLE_KR", "B_EAGLE", 15,                    		8,  { 		"KR1",	"KR2",	"KR3",	"KR4", "KC" } )
        --     grea:init_box_xyz( -2,22, 1,6, 5,15 )

        -- grea = self:add_grea( "EAGLE_KC", "B_EAGLE", 30,                  		9,  "KC" ) -- ouvre le chemin, en se cassant en deux groupes, pour lanim MANTA
        -- 	grea:init_box_xyz( 21,26, 1,4.5, -5,5 )

        grea = self:add_grea( "CARANG_KL", "B_CARANG", 12,                          10, "KL" )
            grea:init_box_xyz( 21,-4, 1.40,4, -4.5,-6.5 )
        grea = self:add_grea( "CARANG_KR", "B_CARANG", 15,                          12, "KR" )
            grea:init_box_xyz( 22,-2, 1.4,4, 5,9 )

		grea = self:add_grea( "PARROT_BL_M_KL", "B_PARROT_BL_M", 14,                 13,  "KL" )
            --grea:init_box_xyz( -0.5,22, 0.5,2, -4.5,-8 )
        grea = self:add_grea( "PARROT_BL_F_KR", "B_PARROT_BL_F", 10,                 14,  "KR" )
            --grea:init_box_xyz( -0.5,22, 0.5,2, 4.5,8 )

-- WHY IN COMMENT ? MUST ADD THEM, THEY WORK FINE.
		-- grea = self:add_grea( "CORNET_KALL", "B_CORNET", 8,                  	15,  "KALL" )
    	-- 	grea:init_box_xyz( -2,22.5, 1.8,4, -6,6 )

-- BUGGED, FLICKER, KI, FIXED BY STOPING BLEND SHAPE 2.
        grea = self:add_grea( "GRACE_KL", "B_GRACE", 1,                             16, "KL" )
            grea:init_box_xyz( -0.5,21, 0.8,2.5, -4.5,-6.5 )

        grea = self:add_grea( "CONVICT_KL", "B_CONVICT", 15,                        17,  "KL" )
            grea:init_box_xyz( 0,21, 0.8,1.50, -5,-7 )


-- OUT UNTIL FIXED, BUGGED, FLICKER, KI
        --grea = self:add_grea( "CLOWN_KC", "B_CLOWN", 3,                             18, "KC" ) -- AUGMENTER LA TAILLE
        --     grea:init_box_xyz( 21,22.5, 1,4, -4,4 )
        --grea = self:add_grea( "CLOWN_KL", "B_CLOWN", 1,                             19,  "KL" )
        --     grea:init_box_xyz( -0.3,21, 0.5,2.5, -4.5,-5.5 )
        --grea = self:add_grea( "CLOWN_KR", "B_CLOWN", 1,                             20,  "KR" )
        --     grea:init_box_xyz( -0.3,21, 0.5,2.5, 4.5,5.5 )

        grea = self:add_grea( "BUTTER_KL", "B_BUTTER", 20,                          21, "KL" )
            grea:init_box_xyz( 0,21, 1.5,3.5, -4.5,-6 )
        grea = self:add_grea( "BUTTER_KR12", "B_BUTTER", 20,                        22, "KR12" )
            grea:init_box_xyz( -0.2,10, 1.2,3, 5,8 )

        grea = self:add_grea( "SURGERY_KL", "B_SURGERY", 7,                         23, "KL" )
            grea:init_box_xyz( -0.2,21, 1,2.3, -4.5,-6 )
        grea = self:add_grea( "SURGERY_KC", "B_SURGERY", 5,                         24, "KC" )
            grea:init_box_xyz( 21,24, 1.0,1.8, -4,4 )

        -- doit repousser legerement les autres especes
        grea = self:add_grea( "LION_KALL", "B_LION", 1,                            25,  "KALL" )
            --grea:init_box_xyz( -1,23, 1,3, -7,7 )

            -- REPOUSSE TOUTES LES ESPECES, DANGEROUS
		grea = self:add_grea( "MAORI_KR", "B_MAORI", 1,                             26, "KR" )
            grea:init_box_xyz( -3,22, 1.8,4, 6,19 )

        --     -- LIPS A et LIPS B doivent se suivre tel un seul BANC + Eviter si possible le KL1 (GO AROUND RANDOM)
        grea = self:add_grea( "LIPS_A_KL", "B_LIPS_A", 12,                          27, "KL" )
            grea:init_box_xyz( -1,22.5, 1.4,3.7, -5,-8 )
        grea = self:add_grea( "LIPS_B_KC", "B_LIPS_B", 14,                          28, "KR" )
            grea:init_box_xyz( -1,22.5, 1.4,4, 5,8 )


        grea = self:add_grea( "TRIGGER_KR3", "B_TRIGGER", 1,                        29, "KR3" )
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
        -- grea = self:add_grea( "TITAN_KR3", "B_TITAN", 1,                            30, "KR3" ) -- semble un peu gros
        --     grea:init_box_xyz( 10,15, 1.40,2, 4.5,7.5 )

		--todo what to do with this one
        grea = self:add_grea( "POTATO_KALL", "B_POTATO", 1,                         31, "KALL" )
            --grea:init_box_xyz( 5,22.5, 1.3,4, -7,7 )

        grea = self:add_grea( "CLEAN_KL123", "B_CLEAN", 20,                         32, "KL123" )
            grea:init_box_xyz( -0.5,15, 0.50,2.50, -4.7,-6 )
        grea = self:add_grea( "CLEAN_KR3", "B_CLEAN", 15,                           33, "KR3" )
            grea:init_box_xyz( 9,16, 1.5,2, 5,7 )
        grea = self:add_grea( "CLEAN_KR12", "B_CLEAN", 12,                          34, "KR12" )
            grea:init_box_xyz( 0.5,10, 1.1,1.6, 4,6 )

		-- AJOUTER DES BARAKES ?
		-- MAT tous les poissons sinon CARANG et BARAK BRILLAN

end