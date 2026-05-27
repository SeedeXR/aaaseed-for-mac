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
-- TANK:add_grea( name, race_name, nb, boid_id, location )

-- RED_F OUT UNTIL FIXED, BUGGED, FLICKER, KI

	-- for all RED: BOX SPHERIQUES A METTRE EN PLACE ET NON CARRÉ + focus/target au centre.
-- KILL THIS RED SINCE BUG WITH NEW KL
	grea = self:add_grea( "RED_M_KL2", "B_RED_M", 30, 					63, "KL" )
		-- grea:init_box_xyz( 4.4,5.6, 1,1.80, -5, -6 )
		-- grea:init_target_active(true)
		-- grea:init_target_xyz(5,1,-6)
		-- grea:init_target_radius(0.4, 0.1)
		-- grea:init_target_influence(1)
	grea = self:add_grea( "RED_F_KL2", "B_RED_F", 20, 					62, "KL" )
		-- grea:init_box_xyz( 4.4,5.6, 1,1.80, -5, -6 )
		-- grea:init_target_active(true)
		-- grea:init_target_xyz(5,1,-6)
		-- grea:init_target_radius(0.4, 0.1)
		-- grea:init_target_influence(1)

	grea = self:add_grea( "RED_M_KL4", "B_RED_M", 30, 					61, "KL" )
		-- grea:init_box_xyz( 19,20.45, 0.50,1.60, -4.4,-5.4 )
		-- grea:init_target_active(true)
		-- grea:init_target_at_box_center()
		-- grea:init_target_radius(0.4, 0.1)
		-- grea:init_target_influence(1)
	grea = self:add_grea( "RED_F_KL4", "B_RED_F", 20, 					60, "KL" )
		-- grea:init_box_xyz( 19,20.45, 0.50,1.60, -4.4,-5.4 )
		-- grea:init_target_active(true)
		-- grea:init_target_at_box_center()
		-- grea:init_target_radius(0.4, 0.1)
		-- grea:init_target_influence(1)

	grea = self:add_grea( "RED_M_KR4", "B_RED_M", 30, 					59, "KR" )
--		grea:init_box_xyz( 19.40,20.40, 1.10,1.75, 6,6.90 )
--		grea:init_target_active(true)
--		grea:init_target_xyz(19.90,1,6.50)
--		grea:init_target_radius(0.4, 0.1)
--		grea:init_target_influence(1)
	grea = self:add_grea( "RED_F_KR4", "B_RED_F", 20, 					58, "KR" )
--		grea:init_box_xyz( 19.40,20.40, 1.10,1.75, 6,6.90 )
--		grea:init_target_active(true)
--		grea:init_target_at_box_center()
--		grea:init_target_radius(0.4, 0.1)
--		grea:init_target_influence(1)

	grea = self:add_grea( "RED_M_KR1", "B_RED_M", 30, 					57, "KR" )
--		grea:init_box_xyz( 1.9,3.80, 0.4,2.4, 5,6.20 )
--		grea:init_target_active(true)
--		grea:init_target_xyz(2.80,1,6.50)
--		grea:init_target_radius(0.6, 0.1)
--		grea:init_target_influence(1)
	grea = self:add_grea( "RED_F_KR1", "B_RED_F", 20, 					56, "KR" )
--		grea:init_box_xyz( 1.9,3.80, 0.4,2.4, 5,6.20 )
--		grea:init_target_active(true)
--		grea:init_target_xyz(2.80,1,6.50)
--		grea:init_target_radius(0.6, 0.1)
--		grea:init_target_influence(1)

end
