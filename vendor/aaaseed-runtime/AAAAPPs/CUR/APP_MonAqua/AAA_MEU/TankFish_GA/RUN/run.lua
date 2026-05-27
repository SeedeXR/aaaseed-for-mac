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
	local scene_name = app:get_scene()
	local grea
-- TANK:add_grea( name, race_name, nb, boid_id, location )

	-- no go around to avoid fish going inside the whale
	if scene_name == "PIJ2" then return end

--number go down and start at 55
	grea = self:add_grea( "HACHETTE_GA", "B_HACHETTE", 70, 				55, "KALL" )
		grea:set_go_around({
			b_is_rotation 		=  true,
			b_begin_clockwise 	= false,
			duration			=    300,	-- Temps pour que la Target fasse le tour de la BOX en s
			begin_pos			=    45,	-- Position de départ en degrées de la Target, centre en x=11m et z=0m (centre de la salle), 0 au Nord (KC) puis dans le sens des aiguiles d'une montre, 90' (Est, KR)  180' ou -180' (Sud). -90' (Ouest, KL)
			depth_min			=     9,	-- Allowed distance from the ROOM (from Kakemono) in m
			depth_max			=    15,
			y_min 				=		1,
			y_max				= 		5,
			radius_min			=     3,	 -- Size of the moving target in m
			radius_max			=     4, 	 -- Size of the sphere around the Target in m
			-- min_pos 			=   -90,	-- required since it's not a rotation
			-- max_pos 			=    40		-- required since it's not a rotation
		})


	grea = self:add_grea( "LUTJ_GA", "B_LUTJ", 50, 						54, "KALL" )
		grea:set_go_around({
			b_is_rotation 		=  true,
			b_begin_clockwise 	= false,
			duration			=    300,	-- Temps pour que la Target fasse le tour de la BOX en s
			begin_pos			=    135,	-- Position de départ en degrées de la Target, centre en x=11m et z=0m (centre de la salle), 0 au Nord (KC) puis dans le sens des aiguiles d'une montre, 90' (Est, KR)  180' ou -180' (Sud). -90' (Ouest, KL)
			depth_min			=     9,	-- Allowed distance from the ROOM (from Kakemono) in m
			depth_max			=    15,
			y_min 				=	1.5,
			y_max				=	7,
			radius_min			=     3,	 -- Size of the moving target in m
			radius_max			=     4, 	 -- Size of the sphere around the Target in m
		})

	grea = self:add_grea( "DARK_GUN_GA", "B_DARK_GUN", 100, 			53, "KALL" )
		grea:set_go_around({
			b_is_rotation 		=  true,
			b_begin_clockwise 	= 	true,
			duration			=    400,	-- Temps pour que la Target fasse le tour de la BOX en s
			begin_pos			=    -135,	-- Position de départ en degrées de la Target, centre en x=11m et z=0m (centre de la salle), 0 au Nord (KC) puis dans le sens des aiguiles d'une montre, 90' (Est, KR)  180' ou -180' (Sud). -90' (Ouest, KL)
			depth_min			=     9,	-- Allowed distance from the ROOM (from Kakemono) in m
			depth_max			=    15,
			y_min 				=		2,
			y_max				=		6,
			radius_min			=     3,	 -- Size of the moving target in m
			radius_max			=     4, 	 -- Size of the sphere around the Target in m
		})

	grea = self:add_grea( "GUN_GA", "B_GUN", 100, 						52, "KALL" )
		grea:set_go_around({
			b_is_rotation 		=  true,
			b_begin_clockwise 	= true,
			duration			=    400,	-- Temps pour que la Target fasse le tour de la BOX en s
			begin_pos			=    -45,	-- Position de départ en degrées de la Target, centre en x=11m et z=0m (centre de la salle), 0 au Nord (KC) puis dans le sens des aiguiles d'une montre, 90' (Est, KR)  180' ou -180' (Sud). -90' (Ouest, KL)
			depth_min			=     9,	-- Allowed distance from the ROOM (from Kakemono) in m
			depth_max			=    15,
			y_min 				=	2.5,
			y_max				=	6,
			radius_min			=     3,	 -- Size of the moving target in m
			radius_max			=     4, 	 -- Size of the sphere around the Target in m
		})

	grea = self:add_grea( "EMPEROR_GA", "B_EMPEROR", 15, 				51, "KALL" )
		grea:set_go_around({
			b_begin_clockwise 	= false,
			duration			=    400,	-- Temps pour que la Target fasse le tour de la BOX en s
			begin_pos			=    135,	-- Position de départ en degrées de la Target, centre en x=11m et z=0m (centre de la salle), 0 au Nord (KC) puis dans le sens des aiguiles d'une montre, 90' (Est, KR)  180' ou -180' (Sud). -90' (Ouest, KL)
			depth_min			=     9,	-- Allowed distance from the ROOM (from Kakemono) in m
			depth_max			=    15,
			y_min 				=		1.5,
			y_max				=		5.5,
			radius_min			=     3,	 -- Size of the moving target in m
			radius_max			=     4, 	 -- Size of the sphere around the Target in m
		})

	grea = self:add_grea( "CORNU_GA", "B_CORNU", 18, 					50, "KALL" )
		grea:set_go_around({
			b_is_rotation 		=  true,
			b_begin_clockwise 	= false,
			duration			=    400,	-- Temps pour que la Target fasse le tour de la BOX en s
			begin_pos			=    -45,	-- Position de départ en degrées de la Target, centre en x=11m et z=0m (centre de la salle), 0 au Nord (KC) puis dans le sens des aiguiles d'une montre, 90' (Est, KR)  180' ou -180' (Sud). -90' (Ouest, KL)
			depth_min			=     10,	-- Allowed distance from the ROOM (from Kakemono) in m
			depth_max			=    16,
			y_min 				=		1,
			y_max				=		6,
			radius_min			=     3,	 -- Size of the moving target in m
			radius_max			=     4, 	 -- Size of the sphere around the Target in m
		})


end
