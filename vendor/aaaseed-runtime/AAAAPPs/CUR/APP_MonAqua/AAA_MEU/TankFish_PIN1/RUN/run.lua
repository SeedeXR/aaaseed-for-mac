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

-- directly in TANK class just find TANK:load_races_monaco()
function TANK:load_races()
	self:load_races_monaco()
end

function TANK:init_fish()
	local grea
	-- TANK:add_grea( name, race_name, nb, boid_id )
	-- BOID ID from 1 to 48

	grea = self:add_grea( "Simple",			"B_SIMPLE",			100,		64	)

	grea = self:add_grea( "TURTL_BB_KL", "B_TURTL_BB", 800, 					1, "KL" )
		grea:init_box_xyz( -1,21, 0.5,3, -4.2,-6.2 )
	--grea = self:add_grea( "TURTL_BB_KC_A", "B_TURTL_BB", 1, 					2, "KC" )
	--	grea:init_box_xyz( 21.2,24.2, 0.5,3, -4,4 )
	grea = self:add_grea( "TURTL_BB_KR", "B_TURTL_BB", 800, 					3, "KR" )
		grea:init_box_xyz( -1,21, 0.5,3, 4.2,6.2 )
	-- grea = self:add_grea( "TURTL_BB_KC_B", "B_TURTL_BB", 30 , 				4, "KC" )
	-- 	grea:init_box_xyz( 21.2,24.2, 0.5,9, -4,4 )
		-- OUT WHY ?

	--grea = self:add_grea( "FLASH_KL", "B_FLASH", 100, 							5,  "KL" )
		--grea:init_box_xyz( -1,22, 0.3,4, -4.2,-5.5 )
	grea = self:add_grea( "FLASH_KL34", "B_FLASH", 180, 							6,  "KL34" )
		--grea:init_box_xyz( 10.3,20.1, 4,9, -4.1,-7 )
	grea = self:add_grea( "FLASH_KC_A", "B_FLASH", 100, 						7, "KC" )
		grea:init_box_xyz( 21.2,23.2, 0.3,4, -4,4. )
	-- grea = self:add_grea( "FLASH_KC_B", "B_FLASH", 50, 						8, "KC" )
	-- 	grea:init_box_xyz( 21.1,26, 4.2,9, -4,4 )
	-- OUT WHY ?

	--grea = self:add_grea( "FLASH_KR_A", "B_FLASH", 100, 						9, "KR" )
		--grea:init_box_xyz( 22,-1, 0.2,3.8, 4.2,5 )
	grea = self:add_grea( "FLASH_KR_B", "B_FLASH", 180, 						10, "KR" )
		grea:init_box_xyz( 21,0.5, 4,8.2, 4.2,8 )

	-- TO ADD WHEN FISH RECEIVED
	-- grea = self:add_grea( "CUTTLE_KL234", "B_CUTTLE", 3, 						13, "KL234" )
		-- 	grea:init_box_xyz( 5.3,19.5, 0.5,9, -4.1,-6 )

end