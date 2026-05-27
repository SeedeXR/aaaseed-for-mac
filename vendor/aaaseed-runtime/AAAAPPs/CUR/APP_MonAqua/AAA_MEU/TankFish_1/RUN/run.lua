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

function TANK:load_races()
	self:print_inverse( "TANK:load_races() Begin" )

	CELT:set_texture_use( false )

	local races = self.__races
	--	create races

	--races:add( "aaaref",				4 )

	races:add( "B_SIMPLE"		)

	races:add( "B_ANCHOVY"		)
	races:add( "B_BANNER"		)
	races:add( "B_BARAK"		)
	races:add( "B_BAT"			)
	races:add( "B_BUTTER"		)
	races:add( "B_CARANG"		)
	races:add( "B_CARPET"		)
	races:add( "B_CLEAN"		)
	races:add( "B_CLOWN"		)
	races:add( "B_CONVICT"		)
	races:add( "B_CORNET"		)
	races:add( "B_CORNU"		)
	races:add( "B_CUTTLE"		)
	races:add( "B_DARK_GUN"		)
	races:add( "B_DOLPH"		)
	races:add( "B_EAGLE"		)
	races:add( "B_EMPEROR"		)
	races:add( "B_FLASH"		)
	races:add( "B_GRACE"		)
	races:add( "B_GUN"			)
	races:add( "B_HACHETTE"		)
	races:add( "B_LADY_BL_M"	)
	races:add( "B_LADY_BL_F"	)
	races:add( "B_LADY_GREEN"	)
	races:add( "B_LION"			)
	races:add( "B_LIPS_A"		)
	races:add( "B_LIPS_B"		)
	races:add( "B_LUTJ"			)
	races:add( "B_MANTA"		)
	races:add( "B_MAORI"		)
	races:add( "B_PARROT"		)
	races:add( "B_PARROT_BL_F"	)
	races:add( "B_PARROT_BL_M"	)
	races:add( "B_PELAGIA"		)
	races:add( "B_POTATO"		)
	races:add( "B_RED_F"		)
	races:add( "B_RED_M"		)
	races:add( "B_SHRK_WHAL"	)
	races:add( "B_SUCKER"		)
	races:add( "B_SURGERY"		)
	races:add( "B_TIGER"		)
	races:add( "B_TITAN"		)
	races:add( "B_TRAVEL"		)
	races:add( "B_TRIGGER"		)
	races:add( "B_TURTL"		)
	races:add( "B_TURTL_BB"		)
	races:add( "B_WHITETIP"		)
	races:add( "B_WHITETIP_BB"	)


	self:print_inverse( "TANK:load_races() End" )
end

function TANK:init_fish()
	local grea
	-- TANK:add_grea( name, race_name, nb, boid_id )
	-- BOID ID from 1 to 48

	grea = self:add_grea( "Simple",			"B_SIMPLE",			10,		64	)
		grea:set_go_around({
			b_is_rotation 		=  true,
			b_begin_clockwise 	= false,
			duration			=    20,	-- Temps pour que la Target fasse le tour de la BOX en s
			begin_pos			=    30,	-- Position de départ en degrées de la Target, centre en x=11m et z=0m (centre de la salle), 0 au Nord (KC) puis dans le sens des aiguiles d'une montre, 90' (Est, KR)  180' ou -180' (Sud). -90' (Ouest, KL)
			depth_min			=     9,	-- Allowed distance from the ROOM (from Kakemono) in m
			depth_max			=    15,
			radius_min			=     1,	 -- Size of the moving target in m
			radius_max			=     2, 	 -- Size of the sphere around the Target in m
		})

		-- pour un couloir de Go Arround, proche des Kakemono, mettre un radius reduit + deph max petit et prendre la meme speed que le BOIDS ou legerement plus.

	grea = self:add_grea( "Anchovy",		"B_ANCHOVY",		200,	1	)
		-- grea:init_box_xyz(		4,10,		4,7,		-10,-6	)
		-- grea:set_random_walk({
		-- 	speed 		=   0.4,
		-- 	border 		=   0.1, 	-- (Rayon en m) Possibilité de dépasser de la box, MIN 0= sphere stuck in the box, 1= sphere peux dépasser de la box mais pas le centre de la target, 2+= target peux depasser du centre.
		-- 	radius_max 	=   2,
		-- 	radius_min 	=   1.5  	-- optional if radius_max is already defined (will be set to radius_max)
		-- })

	grea = self:add_grea( "Banner",			"B_BANNER",			5,		2	)
		grea:init_box_xyz(		2,8,		8,11,		5,9	)
		grea:set_random_walk({
			speed 			=    0.7,
			b_is_stationary = true,
			time_stationary =    3,		-- required since it's a stationary random walk.
			time_moving		=   10,		-- required since it's a stationary random walk.
--			border 			=    0.5,	-- optionnal
			radius_max 		=    0.5,
-- 			radius_min 		=    0.1  	-- optional if radius_max is already defined (will be set to radius_max)
		})

	grea = self:add_grea( "Barak",			"B_BARAK",			5,		3	)
		grea:set_go_around({
			b_is_rotation 		= false,
			b_begin_clockwise 	=  true,
			duration			=    20,
			begin_pos			=    30,
			depth_min			=     0,
			depth_max			=     3,
			radius_min			=     1,
			radius_max			=     2,
			min_pos 			=   -90,	-- required since it's not a rotation
			max_pos 			=    40		-- required since it's not a rotation
		})

	grea = self:add_grea( "Bat",			"B_BAT",	    	4,		4	)
	grea = self:add_grea( "Butter",			"B_BUTTER",		    5,		5	)
	grea = self:add_grea( "Carang",			"B_CARANG",			2,		6	)
	grea = self:add_grea( "Carpet",			"B_CARPET",			2,		7	)
	grea = self:add_grea( "Clean",			"B_CLEAN",			5,		8	)
	grea = self:add_grea( "Clown",			"B_CLOWN",			5,		9	)
	grea = self:add_grea( "Convict",		"B_CONVICT",	    5,		10	)
	grea = self:add_grea( "Cornet",			"B_CORNET",			5,		11	)
	grea = self:add_grea( "Cornu",			"B_CORNU",			50,		12	)
	grea = self:add_grea( "Cuttle",			"B_CUTTLE",			1,		13	)
	grea = self:add_grea( "Dark_Gun",		"B_DARK_GUN",		5,		15	)
	grea = self:add_grea( "Dolph",	 		"B_DOLPH",			5,		16	)
	grea = self:add_grea( "Eagle",	 		"B_EAGLE",			5,		17	)
	grea = self:add_grea( "Emperor",		"B_EMPEROR",		5,		18	)
	grea = self:add_grea( "Flash",			"B_FLASH",			100,	19	)
	grea = self:add_grea( "Grace",			"B_GRACE",			5,		20	)
	grea = self:add_grea( "Gun",			"B_GUN",			10,		21	)
	grea = self:add_grea( "Hachette",		"B_HACHETTE",		5,		22	)
	grea = self:add_grea( "Lady_BL_M",		"B_LADY_BL_M",		5,		23	)
	grea = self:add_grea( "Lady_BL_F",		"B_LADY_BL_F",		5,		24	)
	grea = self:add_grea( "Lady_Green",		"B_LADY_GREEN",		5,		25	)
	grea = self:add_grea( "Lion",			"B_LION",			5,		26	)
	grea = self:add_grea( "Lips_A",			"B_LIPS_A",			5,		27	)
	grea = self:add_grea( "Lips_B",			"B_LIPS_B",			5,		28	)
	grea = self:add_grea( "LUTJ",			"B_LUTJ",	    	5,		29	)
	grea = self:add_grea( "Manta",			"B_MANTA",	    	2,		30	)
	grea = self:add_grea( "Maori",			"B_MAORI",	    	1,		31	)
	grea = self:add_grea( "Parrot",			"B_PARROT",	    	5,		32	)
	grea = self:add_grea( "Parrot_BL_F",	"B_PARROT_BL_F", 	2,		33	)
	grea = self:add_grea( "Parrot_BL_M",	"B_PARROT_BL_M",   	2,		34	)
	--grea = self:add_grea( "PELAGIA",		"B_PELAGIA",  	 	2,		35	)
	grea = self:add_grea( "Potato",			"B_POTATO",	  	 	5,		36	)
	grea = self:add_grea( "Red_F",			"B_RED_F",	    	5,		37	)
	grea = self:add_grea( "Red_M",			"B_RED_M",	    	5,		38	)
	grea = self:add_grea( "Shrk_Whal",		"B_SHRK_WHAL",	   	2,		39	)
	grea = self:add_grea( "Sucker",			"B_SUCKER",	    	5,		40	)
	grea = self:add_grea( "Surgery",		"B_SURGERY",	   	5,		41	)
	grea = self:add_grea( "Tiger",			"B_TIGER",	    	5,		42	)
	grea = self:add_grea( "Titan",			"B_TITAN",	    	5,		43	)
	grea = self:add_grea( "Travel",			"B_TRAVEL",	    	5,		44	)
	grea = self:add_grea( "Trigger",		"B_TRIGGER",	   	5,		45	)
	grea = self:add_grea( "Turtl_BB",		"B_TURTL_BB",	   	5,		46	)
	grea = self:add_grea( "Whitetip",		"B_WHITETIP",	  	5,		47	)
	grea = self:add_grea( "Whitetip_BB",	"B_WHITETIP_BB",  	5,		48	)
	grea = self:add_grea( "Turtl",			"B_TURTL",	 	  	5,		49	)



--	grea = self:add_grea( "fish_col3",		"maquereau_bleu",		64,		ID	)
--	grea = self:add_grea( "fish_col4",		"maquereau_bleu",		400,	ID	)
--	grea = self:add_grea( "fish_col5",		"maquereau_bleu",		400,	ID	)
--	grea = self:add_grea( "fish_col6",		"maquereau_bleu",		400,	ID	)
--	grea = self:add_grea( "lune",			"poissonLune",			100*3,	ID	)
--	grea = self:add_grea( "lune_rare",		"poissonLune_rare",		20*3,	ID	)

--	grea = self:add_grea( "thon", 			"thon",					32*3,	ID	)
--		grea:set_curve_dist_nb( .4, 30 )

--	grea = self:add_grea( "raie", 		"raie",					10,		ID	)
--		grea:set_curve_dist_nb( 1.21, 10 )

-- 	grea = self:add_grea( "Cacha",		"cachalot",				12		)
-- 		grea:assign_boid_out( 10, 24, 1., -3. )
-- 		grea:set_launcher(	{ 65, 66, 67, 70, 71 },
-- 							50., 105., 1.5, 2.,
-- 							false
-- 						)
-- 	grea = self:add_grea( "Sun",		"sunfish",				12		)
-- 		grea:assign_boid_out( 11, 3, .5, 0.5 )
-- 		grea:set_launcher(	{ 11, 12, 13 },
-- 							40., 45., .8, 1.2,
-- 							true
-- 						)
-- 	grea = self:add_grea( "Req",		"requin",				12		)
-- 		grea:assign_boid_out( 12, 6, .5, .5 )
-- 		grea:set_launcher(	{ 4, 5, 6, 7, 8, 9, 10 },
-- 							45., 50., 1.2, 2.,
-- 							true
-- 						)


end
