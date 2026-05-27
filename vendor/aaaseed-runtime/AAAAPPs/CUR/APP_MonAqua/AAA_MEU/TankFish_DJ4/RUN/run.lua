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


	-- !! pb sur queue, ca sent la curve !!!
	grea = self:add_grea( "WHITETIP_KR", "B_WHITETIP", 30,           			1, "KR" )
		-- grea:init_box_xyz( 0,35, -2,4, 4,25 )
		--grea:init_target_active(true)
		--grea:init_target_at_box_center() -- ATTENTION, Utiliser soit target at box center soit xyz
		--grea:init_target_xyz(9,1.5,10)
		--grea:init_target_radius(10, 0) -- EXTERNE - INTERNE
		--grea:init_target_influence(1)

	-- !! pb sur queue, ca sent la curve !!!
	grea = self:add_grea( "WHITETIP_KC", "B_WHITETIP", 30,           			13, "KC" )
		-- grea:init_box_xyz( 22,35, -2,4, -8,8 )
		--grea:init_target_active(true)
		--grea:init_target_at_box_center() -- ATTENTION, Utiliser soit target at box center soit xyz
		--grea:init_target_xyz(9,1.5,10)
		--grea:init_target_radius(10, 0) -- EXTERNE - INTERNE
		--grea:init_target_influence(1)

	-- !! pb sur queue, ca sent la curve !!!
	grea = self:add_grea( "WHITETIP_KL", "B_WHITETIP", 30,           			14, "KL" )
		-- grea:init_box_xyz( 0,35, -2,4, -4,-25 )
		--grea:init_target_active(true)
		--grea:init_target_at_box_center() -- ATTENTION, Utiliser soit target at box center soit xyz
		--grea:init_target_xyz(9,1.5,10)
		--grea:init_target_radius(10, 0) -- EXTERNE - INTERNE
		--grea:init_target_influence(1)

    grea = self:add_grea( "TURTL_KR12", "B_TURTL", 1,       		2, "KR12" ) --ATTENTION, ce n'est pas une TURTL_BB mais un boid TURTL adulte
        -- grea:init_box_xyz( 7.5,1, 1.7,0.5, -5,-10 )

    grea = self:add_grea( "TIGER_KC", "B_TIGER", 1,             	3, "KC" )
        grea:init_box_xyz( 24,45, 6,1.40, -6,6 )

    grea = self:add_grea( "TRAVEL_KR", "B_TRAVEL", 20,          	4,  "KR" )
        -- grea:init_box_xyz( -0.5,22, 0.80,4, 4.40,7 )

    grea = self:add_grea( "TRAVEL_KC", "B_TRAVEL", 10,          	5,  "KC" )
        -- grea:init_box_xyz( 28,21.50, 0.80,5, -4.40,4.40 )

    grea = self:add_grea( "HACHETTE_KC", "B_HACHETTE", 200,     	6,  "KC" )
		-- grea:init_box_xyz( 25,21.50, 0.50,5.10, -4.40,5 )
		--grea:init_target_active(true)
		--grea:init_target_at_box_center() -- ATTENTION, Utiliser soit target at box center soit xyz
		--grea:init_target_xyz(9,1.5,10)
		--grea:init_target_radius(10, 0) -- EXTERNE - INTERNE
		--grea:init_target_influence(1)

    grea = self:add_grea( "HACHETTE_KR", "B_HACHETTE", 450,     	7,  "KR" )
        -- grea:init_box_xyz( 23,0.80, 1.50,5.10, 5,7 )

    grea = self:add_grea( "BARAK_KR34","B_BARAK", 6,            	8,  "KR34" )
        -- grea:init_box_xyz( 10.50,21.50, 1.40,4, 4,8 )

    grea = self:add_grea( "LUTJ_KALL", "B_LUTJ", 200,           	9,  "KALL" )
        -- grea:init_box_xyz( -1,25, 1.6,7, -6,6 )

    grea = self:add_grea( "ANCHOVY_KR", "B_ANCHOVY", 200,       	10, "KR" )
        -- grea:init_box_xyz(  28,0.50, 0.80,5, 4.40,7 )
    grea = self:add_grea( "ANCHOVY_KC", "B_ANCHOVY", 200,       	11, "KC" )
		-- grea:init_box_xyz(  24,21, 0.80,5, -4.40,4.40 )
		--grea:init_target_active(true)
		--grea:init_target_at_box_center() -- ATTENTION, Utiliser soit target at box center soit xyz
		--grea:init_target_xyz(9,1.5,10)
		--grea:init_target_radius(10, 0) -- EXTERNE - INTERNE
		--grea:init_target_influence(1)

    grea = self:add_grea( "SUCKER_KR", "B_SUCKER", 3,           	12, "KR" )
		-- grea:init_box_xyz( 0,21, 1.45,3, 4.40,7.8 )

end