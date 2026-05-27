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


	--- A SURVEILLER (BETTER USING BIRTH/DEATH ?)
	--grea = self:add_grea( "TIGER_KCKR432", "B_TIGER", 1,                        2, "KC" )
		--grea:init_box_xyz( 27,40, 2,5.5, -7,7 )

	grea = self:add_grea( "BAT_KL234", "B_BAT", 10,                             3, "KL234" )
		grea:init_box_xyz( 5,21, 1,2.5, -4,-7 )
	grea = self:add_grea( "BAT_KC", "B_BAT", 15,                                4, "KC" )
		grea:init_box_xyz( 21,27, 1,5, -4,4 )
	grea = self:add_grea( "BAT_KR", "B_BAT", 20,                                5, "KR" )
		grea:init_box_xyz( 0,21, 0.5,5, 4,4.8 )

	grea = self:add_grea( "BANNER_KL234", "B_BANNER", 80,                       6, "KL234" )
		grea:init_box_xyz( 5,20, 0.5,4, -4,-6 )
	grea = self:add_grea( "BANNER_KC", "B_BANNER", 100,                         7, "KC" )
		grea:init_box_xyz( 21,23, 0.5,4, -4,4 )
	grea = self:add_grea( "BANNER_KR432", "B_BANNER", 50,                       8, "KR234" )
		grea:init_box_xyz( 5,20, 1.5,3, 4,6 )


	grea = self:add_grea( "B_TRAVEL", "B_TRAVEL", 25 ,               			9, "KR" )
		grea:init_box_xyz( -2,30, 1.40,5, 4,10 )

	-- !! pb sur queue, ca sent la curve !!!
	grea = self:add_grea( "WHITETIP_KR", "B_WHITETIP", 30,           			10, "KR" )
		--grea:init_box_xyz( 0,35, -2,4, 4,25 )
		--grea:init_target_active(true)
		--grea:init_target_at_box_center() -- ATTENTION, Utiliser soit target at box center soit xyz
		--grea:init_target_xyz(9,1.5,10)
		--grea:init_target_radius(10, 0) -- EXTERNE - INTERNE
		--grea:init_target_influence(1)

	-- !! pb sur queue, ca sent la curve !!!
	grea = self:add_grea( "WHITETIP_KC", "B_WHITETIP", 30,           			11, "KC" )
		grea:init_box_xyz( 22,35, -2,4, -8,8 )
		grea:init_target_active(true)
		grea:init_target_at_box_center() -- ATTENTION, Utiliser soit target at box center soit xyz
		grea:init_target_xyz(9,1.5,10)
		grea:init_target_radius(10, 0) -- EXTERNE - INTERNE
		grea:init_target_influence(1)

	-- !! pb sur queue, ca sent la curve !!!
	grea = self:add_grea( "WHITETIP_KL", "B_WHITETIP", 30,           			12, "KL" )
		--grea:init_box_xyz( 0,35, -2,4, -4,-25 )
		--grea:init_target_active(true)
		--grea:init_target_at_box_center() -- ATTENTION, Utiliser soit target at box center soit xyz
		--grea:init_target_xyz(9,1.5,10)
		--grea:init_target_radius(10, 0) -- EXTERNE - INTERNE
		--grea:init_target_influence(1)


	grea = self:add_grea( "CARANG_KC", "B_CARANG", 28,                          13, "KC" )
		--grea:init_box_xyz( 21,24, 1.30,5, -4,4 )
	grea = self:add_grea( "CARANG_KR", "B_CARANG", 30,                          14, "KR" )
		--grea:init_box_xyz( 20,9, 1.40,5, 4,6 )

	grea = self:add_grea( "PARROT_BL_M_KL3", "B_PARROT_BL_M", 2,				15,  "KL3" )
		grea:init_box_xyz( 10,15, 0.8,2, -4,-6 )
	grea = self:add_grea( "PARROT_BL_F_KR3", "B_PARROT_BL_F", 2,				16,  "KR3" )
		grea:init_box_xyz( 10,15, 0.8,2, 4,6 )

	grea = self:add_grea( "POTATO_SAUFKL1", "B_POTATO", 1,                      17, "KALL" )
--		grea:init_box_xyz( 5,22.5, 1.6,4, -6,6 )

		-- OUT ??? Remettre en ON
	-- grea = self:add_grea( "CORNET_KALL", "B_CORNET", 8,					18,  "KALL" )
	-- 	grea:init_box_xyz( 5,22.5, 1.8,4, -6,6 )

			-- OUT ??? Remettre en ON
	-- grea = self:add_grea( "CONVICT_KL234", "B_CONVICT", 15,                     19, "KL234" )
	-- 	grea:init_box_xyz( 5,20, 1,1.50, -5,-7 )

-- OUT UNTIL FIXED, BUGGED, FLICKER, KI
	-- grea = self:add_grea( "CLOWN_KC", "B_CLOWN", 2,                             20, "KC" )
	-- 	grea:init_box_xyz( 21,22, 1,4, -4,4 )
	-- grea = self:add_grea( "CLOWN_KL", "B_CLOWN", 1,                             21, "KL" )
	-- 	grea:init_box_xyz( 0,20, 0.5,2.5, -4,-4.8 )
	-- grea = self:add_grea( "CLOWN_KR", "B_CLOWN", 1,                             22, "KR" )
	-- 	grea:init_box_xyz( 0,20, 0.5,2.5, 4,4.8 )

	grea = self:add_grea( "BUTTER_KL", "B_BUTTER", 20,                          23, "KL" )
		grea:init_box_xyz( 0,21, 1.5,2.5, -4,-5 )
	grea = self:add_grea( "BUTTER_KR", "B_BUTTER", 30,                          24, "KR" )
		grea:init_box_xyz( 0,21, 1,3, 4,5 )

	grea = self:add_grea( "SURGERY_KL", "B_SURGERY", 7,                         25, "KL" )
		grea:init_box_xyz( 0,20, 1,2.5, -4,-5 )

	grea = self:add_grea( "LION_KALL", "B_LION", 1,                             26, "KALL" )
		--grea:init_box_xyz( -1,22.5, 1.4,6, -5,5 )

	grea = self:add_grea( "BARAK_KR1234", "B_BARAK", 300,                       27, "KR" )
--		grea:init_box_xyz( 0,25, 1.70,8, 4,20 )
--		grea:init_target_active(true)
--		grea:init_target_at_box_center() -- ATTENTION, Utiliser soit target at box center soit xyz
--		grea:init_target_xyz(10,3.5,9)
--		grea:init_target_radius(9, 4) -- EXTERNE - INTERNE
--		grea:init_target_influence(1)

--OUT ??? WORKS FINE, MUST BE PUT BACK ON
	-- grea = self:add_grea( "MAORI_KR", "B_MAORI", 1,                          28, "KR" )
	-- 	grea:init_box_xyz( -1,22, 1.8,3.5, 6,19 )

	grea = self:add_grea( "LIPS_A_KALL", "B_LIPS_A", 12,                     29, "KALL" )
		--grea:init_box_xyz( 5,22.5, 1.4,5, -6,6 )
	grea = self:add_grea( "LIPS_B_KALL", "B_LIPS_B", 14,                     30, "KALL" )
		--grea:init_box_xyz( 5,22.5, 1.4,5, -6,6 )

	grea = self:add_grea( "ANCHOVY_KALL", "B_ANCHOVY", 100,                  31, "KALL" )
		--grea:init_box_xyz( -1,22.5, 2,6, -5,5 )

		--OUT ???  MUST BE PUT BACK ON
	-- grea = self:add_grea( "TRIGGER_KR3", "B_TRIGGER", 1,                      2, "KR3" )
	-- 	grea:init_box_xyz( 10,15, 1.50,2, 5,7 )

	grea = self:add_grea( "SUCKER_KR234", "B_SUCKER", 3,                        33, "KR234" )
		grea:init_box_xyz( 5,21, 1.45,3, 4,7 )

	-- TO ADD WHEN FISH RECEIVED BY KI
	-- grea = self:add_grea( "B_CUTTLE_1", "B_CUTTLE", 1, 						34, "KL2" )
	--  grea:init_box_xyz( 5,10, 0,0.30, -4, -5 )
	-- grea = self:add_grea( "B_CUTTLE_3", "B_CUTTLE", 1, 						35, "KL4" )
	--  grea:init_box_xyz( 16,21, 0.20,0.50, -4,-5 )

-- BEE & GA called from their own FishTank now


end
