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
			0,		  -5,	3.5,   -6.5,
            48,		  24,	3.5,   -6.5,
			64,		  24,   3.5,    6.5,
			112, 	  -5,	3.5,    6.5,
			128, 	  -5,	3.5,   -6.5,
        	176, 	  24,	3.5,   -6.5,
			192,      24,	3.5,    6.5,
			240,      -5,	3.5,    6.5
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
		  0,	  24,   3.5,    6.5,
		 48,  	  -5,	3.5,    6.5,
		 64,  	  -5,	3.5,   -6.5,
		112,	  24,	3.5,   -6.5,
		128,      24,	3.5,    6.5,
		176,      -5,	3.5,    6.5,
		192,	  -5,	3.5,   -6.5,
    	240,	  24,	3.5,   -6.5
        )
		boid:set_box_x(x)
        boid:set_box_y(y)
        boid:set_box_z(z)
	end
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

	grea = self:add_grea( "FLASH_KL_A", "B_FLASH", 30, 				1, "KL34" )
		grea:init_box_xyz( 10.5,20, 3.9,8.5, -4.1,-6 )
	grea = self:add_grea( "FLASH_KL_B", "B_FLASH", 40, 				2, "KL34" )
		grea:init_box_xyz( -0.5,21, 0.2,3.8, -4.1,-5.2 )
	grea = self:add_grea( "FLASH_KC_A", "B_FLASH", 40, 				3, "KC" )
		grea:init_box_xyz( 20.2,24, 3.9,8.6, -4,4 )
	grea = self:add_grea( "FLASH_KC_B", "B_FLASH", 50, 				4, "KC" )
		grea:init_box_xyz( 20.2,23, 0.1,3.8, -4,4 )
	grea = self:add_grea( "FLASH_KR_A", "B_FLASH", 50, 				5, "KR" )
		grea:init_box_xyz( 20,0.1, 3.9,8.6, 4.1,6 )
	grea = self:add_grea( "FLASH_KR_B", "B_FLASH", 60, 				6, "KR" )
		grea:init_box_xyz( 20,0.1, 0.1,3.8, 4.1,5.2 )
	grea = self:add_grea( "EAGLE_KL", "B_EAGLE", 7,                7,  { 	"KL1",	"KL2",	"KL3",	"KL4",	"KR1",	"KR2",	"KR3",	"KR4", "KC" } )
		--grea:init_box_xyz( -0.5,21, 0.2,3.8, -4.1,-5.2 )
	grea = self:add_grea( "EAGLE_KR", "B_EAGLE", 7,                8,  { 	"KL1",	"KL2",	"KL3",	"KL4",	"KR1",	"KR2",	"KR3",	"KR4", "KC" } )
		--grea:init_box_xyz( 20,0.1, 0.1,3.8, 4.1,5.2 )

-- TO ADD ONLY WHEN FISH MEDIA RECEIVED
	--		grea = self:add_grea( "CUTTLE_KL234", "B_CUTTLE", 3, 				7, "KL234" )
	--			grea:init_box_xyz( 5.3,20, 1.2,3, -4.1,-4.8 )
end