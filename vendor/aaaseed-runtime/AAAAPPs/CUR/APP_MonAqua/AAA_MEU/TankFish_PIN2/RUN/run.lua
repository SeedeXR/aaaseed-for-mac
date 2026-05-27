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

--	grea = self:add_grea( "poisson_lune", 	"poissonLune",			300,	63	)
--	grea = self:add_grea( "testslot_1", 	"testslot1",			300,	62	)
--	grea = self:add_grea( "testslot_2", 	"testslot2",			300,	61	)

-- TO ADD WHEN FISH RECEIVED FROM KI STUDIO
		-- grea = self:add_grea( "B_PELAGIA_1", "B_PELAGIA", 300, 				1, "KL234" )
		-- 	grea:init_box_xyz( 5.3,20, 0.1,8.6, -4.1,-7 )
		-- grea = self:add_grea( "B_PELAGIA_1", "B_PELAGIA", 300, 				2, "KC" )
		-- 	grea:init_box_xyz( 20.1,25, 3.9,8.6, -4,4 )
		-- grea = self:add_grea( "B_PELAGIA_2", "B_PELAGIA", 500, 				3, "KR" )
		-- 	grea:init_box_xyz( 20,0.1, 0.1,8.6, 4.1,7 )
		-- grea = self:add_grea( "B_PELAGIA_2", "B_PELAGIA", 500, 				4, "KC" )
		-- 	grea:init_box_xyz( 20.1,25, 0.1,3.8, -4,4 )

		grea = self:add_grea( "PELAGIA_KL", "B_PELAGIA", 800, 					11, "KL" )
			grea:init_box_xyz( -1,22, -2,12, -4.1,-8 )
		grea = self:add_grea( "PELAGIA_KR", "B_PELAGIA", 800, 					12, "KR" )
			grea:init_box_xyz( -1,22, -2,12, 4.1,8 )
		grea = self:add_grea( "PELAGIA_KC", "B_PELAGIA", 300, 					13, "KC" )
			grea:init_box_xyz( 22,26, -2,12, -6,6 )

		grea = self:add_grea( "FLASH_KL34_A", "B_FLASH", 30, 					5, "KL34" )
			grea:init_box_xyz( 10.5,20, 0.6,4, -4.1,-5 )
		grea = self:add_grea( "FLASH_KL34_B", "B_FLASH", 40, 					6, "KL34" )
			grea:init_box_xyz( 10.5,20, 4.1,8.8, -4.1,-5 )
		grea = self:add_grea( "FLASH_KC_A", "B_FLASH", 40,						7, "KC" )
			grea:init_box_xyz( 21.1,23, 0.8,3.6, -4,4 )
		grea = self:add_grea( "FLASH_KC_B", "B_FLASH", 50, 						8, "KC" )
			grea:init_box_xyz( 21.1,23.5, 3.7,6, -4,4 )
		grea = self:add_grea( "FLASH_KR_A", "B_FLASH", 50, 						9, "KR" )
			grea:init_box_xyz( 21,0.1, 3.9,8.6, 4.1,5 )
		grea = self:add_grea( "FLASH_KR_B", "B_FLASH", 60, 						10, "KR" )
			grea:init_box_xyz( 21,0.1, 0.6,3.8, 4.1,5 )

-- BEE & GA called from their own FishTank now


end