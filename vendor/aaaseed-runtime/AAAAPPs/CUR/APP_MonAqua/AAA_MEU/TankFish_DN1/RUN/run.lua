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

-- scène de la ponte des coraux
	grea = self:add_grea( "FLASH_KL_A", "B_FLASH", 30, 						1, 		"KL34" )
		grea:init_box_xyz( 10.5,19.5, 0.2,3.8, -4.2,5.5 )
	grea = self:add_grea( "FLASH_KL_B", "B_FLASH", 40,						2,		"KL34" )
		grea:init_box_xyz( 10.5,19.5, 3.9,8, -4.5,7 )
	grea = self:add_grea( "FLASH_KC_A", "B_FLASH", 40, 						3,		"KC" )
		grea:init_box_xyz( 21.1,23, 0.1,3.8, -4,4 )
	grea = self:add_grea( "FLASH_KC_B", "B_FLASH", 50, 						4,		"KC" )
		grea:init_box_xyz( 22,25, 3.9,8, -4,4 )
	grea = self:add_grea( "FLASH_KR_A", "B_FLASH", 50, 						5, 		"KR" )
		grea:init_box_xyz( 20,0.1, 0.1,3.8, 4.1,5 )
	grea = self:add_grea( "FLASH_KR_B", "B_FLASH", 60, 						6,		"KR" )
		grea:init_box_xyz( 20,0.1, 3.9,8.6, 4.3,6 )


-- TO add WHEN RECEIVED from KI
-- grea = self:add_grea( "CUTTLE_KL234", "B_CUTTLE",3, 7, "KL234" )
-- 	grea:init_box_xyz( 5.36,19.8, 0.1,8, 4.1,6 )

end
