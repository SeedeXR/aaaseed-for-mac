
-- meta behaviors
function GREA:get_behavior_always()
	local be = self.behavior
	if not be then
		be = {}
		self.behavior = be
	end
	return be
end
function GREA:do_behavior( )

	local be = self.behavior	
	--table.print( be, "behavior", 2 )
	if be then
		-- self:print_verbose(be.random_walk.active..'  '..be.random_walk.is_stationary)
		local rw = be.random_walk
		if rw then
			local active = rw.b_active
			if active ~= 0 and active then -- matches 1 and true, and not 0 nor false
				self:print_verbose( "random_walk.b_active "..active )
				self:do_random_walk()
			end
		end

		local ym = be.y_movement
		if ym then
			local active = ym.b_active
			if active ~= 0 and active then -- matches 1 and true, and not 0 nor false
				self:print_verbose( "y_movement.b_active "..active )
				self:move_target_y_axis_randomly()
			end
		end

		local ga = be.go_around
		if ga then
			local active = ga.b_active
			if active ~= 0 and active then -- matches 1 and true, and not 0 nor false
				self:print_verbose( "go_around.b_active "..active )
				self:go_around()
			end
		end

	-- -- self:print_verbose("rotate :".. self.__boid.behavior.target_rotate)
	-- if self.__boid.behavior.target_rotate then
	-- 	self:rotate_target()
	-- end

	-- -- self:print_verbose("rotate :".. self.__boid.behavior.target_noise)
	-- if self.__boid.behavior.target_noise then
	-- 	self:move_target_randomly()
	-- end

	end
end

function GREA:init_behavior_radius_changes()
	local be = self:get_behavior_always()
	be.radius_changes = {
			radius_min 			= 1,
			radius_max 			= 2,

			__radius_last_angle = 0
		}
	return be.radius_changes
end
function GREA:init_behavior_rotation()
	local be = self:get_behavior_always()
	be.rotation = {
			b_active 			= false,
			seconds_per_turn 	= 5,
			b_clockwise 		= true,
			radius 				= 0,
			adherence 			= 1,

			__b_is_rotation 	= true,
			__angle_current 	= 0,
			__angle_min 		= 0,
			__angle_max 		= 0,
		}
	return be.rotation
end

function GREA:init_behavior_random_walk()
	local be = self:get_behavior_always()
	be.random_walk = {
			b_active 		 	= false,
			speed 		 		= 1,
			randomness 	 		= 100,
			border 		 		= 0,

			b_is_stationary 	= false,
			time_stationary 	= 10,
			time_moving 		= 10,

			__last_angle 		= 0,
			__tx 		 		= 0,
			__tz 		 		= 0,
			__time_remaining 	= 0,
			__b_is_moving		= false
		}
	return be.random_walk
end

function GREA:set_random_walk( params )
	local boid = self:get_boid()
	if not boid then return end

	self:print_verbose( "set_random_walk" )
	-- params = {
	-- 		speed,
	-- 		border,
	-- 		radius_max,
	-- 		radius_min

	--		b_is_stationary, (default to false)
	--		time_stationary, (optional, only required if b_is_stationary is true)
	--		time_moving,	 (optional, only required if b_is_stationary is true)
	-- }

	-- activate random walk
	local rw = self:init_behavior_random_walk()
	rw.b_active 	= true
	rw.speed 		= params.speed  or 1
	rw.border 		= params.border or 0

	-- activate y movement
	self:set_y_movement( params )

	-- activate target
	boid:set_target_active( true, false )
	boid:set_target_xyz_save( false )

	-- set radiuses
	local radius_max = params.radius_max or 1
	local radius_min = params.radius_min or radius_max

	local rr = self:init_behavior_radius_changes()
	rr.radius_min = radius_min
	rr.radius_max = radius_max

	boid:set_target_radius(	(radius_max + radius_min) * 0.5,
							(radius_max + radius_min) * 0.2,
							false
							);

	if params.b_is_stationary then
		rw.b_is_stationary   = true
		rw.time_stationary = params.time_stationary or 10
		rw.time_moving 	   = params.time_moving 	or  5
	end
end

function GREA:init_behavior_go_around()
	local be = self:get_behavior_always()
	be.go_around = {
			b_active 			= false,
			min_pos 			=  0,
			max_pos 			=  0,
			begin_pos			=  0,
			duration 			= 60,
			depth_min 			=  0,
			depth_max 			= 12,

			b_is_rotation		= true,
			b_begin_clockwise	= true,

			randomness			= 0.3,

			__current_depth		= 0,
			__depth_last_angle  = 0,
			__radius_last_angle = 0,
		}
	return be.go_around
end
function GREA:set_go_around( params )
	local boid = self:get_boid()
	if not boid then return end

	self:print_verbose( "set_go_around" )
	-- params = {
	--		b_is_rotation,
	--		b_begin_clockwise
	-- 		duration,
	-- 		begin_pos,
	-- 		depth_min,
	-- 		depth_max,
	-- 		radius_min,
	-- 		radius_max,
	-- 		min_pos,  	(not required if rotation is true)
	-- 		max_pos,  	(not required if rotation is true)
	-- 		y_min,    	(not required)
	-- 		y_max,    	(not required)
	-- }


	-- activate go around
	local ga = self:init_behavior_go_around()
	ga.b_active 	= true
	ga.min_pos 		= params.min_pos 	or -90
	ga.max_pos 		= params.max_pos 	or  90
	ga.begin_pos 	= params.begin_pos  or   0
	ga.duration 	= params.duration   or 120
	ga.depth_min 	= params.depth_min  or   0
	ga.depth_max 	= params.depth_max  or   3

	ga.__current_depth = (ga.depth_min + ga.depth_max) / 2

	-- activate target

	boid:set_target_active( true, false )
	boid:set_target_xyz_save( false )

	local radius_max = params.radius_max or 1
	local radius_min = params.radius_min or radius_max

	-- set radiuses
	local radius_begin = (radius_min + radius_max) / 2
	boid:set_target_radius( radius_begin, radius_begin/2, false )

	local rr = self:init_behavior_radius_changes()
	rr.radius_min = radius_min
	rr.radius_max = radius_max

	-- activate y movement
	self:set_y_movement({
		speed  = 0.5,
		border = 0
	})

	-- set box
	local y_min = params.y_min or  0
	local y_max = params.y_max or 10
	local init_box_depth = ga.__current_depth + radius_begin
	boid:set_box_min_max_xyz(-init_box_depth, 20 + init_box_depth, y_min, y_max, -4 - init_box_depth, 4 + init_box_depth)

	-- set begin pos
	local rot = self:init_behavior_rotation()

	if params.b_is_rotation ~= nil then
		rot.__b_is_rotation = params.b_is_rotation
		ga.b_is_rotation 	= params.b_is_rotation
	else
		ga.b_is_rotation 	= true
		rot.__b_is_rotation = true
	end

	rot.__angle_current = ga.begin_pos / 180 * math.pi
	rot.__angle_min 	= ga.min_pos   / 180 * math.pi
	rot.__angle_max 	= ga.max_pos   / 180 * math.pi
	boid:set_target_x( 300 * math.cos( rot.__angle_current ) )
	boid:set_target_z( 300 * math.sin( rot.__angle_current ) )
	self.__b_etienne_maa_hack_need_restart = true


	if params.b_begin_clockwise ~= nil then
		ga.b_begin_clockwise = params.b_begin_clockwise
		rot.b_clockwise = params.b_begin_clockwise
	else
		ga.b_begin_clockwise = true
		rot.b_clockwise = true
	end

	-- hack: using legacy rotate function with large enough radius so it becomes a square
	rot.seconds_per_turn = ga.duration
	rot.radius = 100
	rot.adherence = 1000 -- instant square
	-- boid:set_target_x(100 * math.cos(begin_pos / 100 * 180))
	-- boid:set_target_z(100 * math.sin(begin_pos / 100 * 180))
	boid:set_target_y( y_min + math.random() * (y_max - y_min) )
	self:clamp_target_coordinates()

	-- spawn fishes near the target
	-- boid:set_birth_min_max_xyz(
	-- 	boid:get_target_x() - radius_max, boid:get_target_x() + radius_max,
	-- 	boid:get_target_y() - radius_max, boid:get_target_y() + radius_max,
	-- 	boid:get_target_z() - radius_max, boid:get_target_z() + radius_max)
end

function GREA:init_behavior_y_movement()
	local be = self:get_behavior_always()
	be.y_movement = {
			b_active 		 	= false,
			speed 		 		= 1,
			randomness 	 		= 100,
			border 		 		= 0,

			__ty 		 		= 0,
			__last_angle 		= 0
		}
	return be.y_movement
end
function GREA:set_y_movement(params)
	local boid = self:get_boid()
	if not boid then return end

	self:print_verbose( "set_y_movement" )
	-- params = {
	-- 		speed,
	-- 		border,
	-- }

	-- activate y movement
	local ym = self:init_behavior_y_movement()
	-- ym.active 		= true -- not active since we call it in move_random_walk directly
	ym.speed 		= params.speed or 1
	ym.randomness 	= 100
	ym.border 		= params.border or 0

	-- set up target
	boid:set_target_active( true, false )
end

function GREA:do_random_walk()
	local boid = self:get_boid()
	if not boid then return end

	self:print_verbose( "do_random_walk" )
	local rw = self.behavior.random_walk
	if rw.b_is_stationary ~= 0 and rw.b_is_stationary then
		-- only move if in moving phase
		if rw.__time_remaining < 0 then
			-- change phase
			rw.__b_is_moving = not rw.__b_is_moving
			rw.__time_remaining = rw.__b_is_moving and rw.time_moving or rw.time_stationary
		end

		if rw.__b_is_moving then
			self:move_target_random_walk()
		end

		rw.__time_remaining = rw.__time_remaining - aaa.time.dt

	else
		self:move_target_random_walk()
	end
end

function GREA:move_target_random_walk()
	local boid = self:get_boid()
	if not boid then return end

	self:print_verbose( "move_target_random_walk" )
	-- todo: consider s-factor
	local bx, by, bz 		= boid:get_box_xyz()
	local sbx, sby, sbz		= boid:get_box_sxyz()
	local radius 			= boid:get_target_radius()

	local rw = self.behavior.random_walk

	-- tx,ty,tz are between -0.5 and 0.5 (if border == 1, can be less if border < 1)
	local tx = rw.__tx
	local tz = rw.__tz

	local noise_power 		= rw.speed
	local noise_activity 	= rw.randomness * 3.6 -- * 3.6 since noise_activity is between 0 and 100
	local border 			= rw.border

	local speed = aaa.time.dt * noise_power
	local new_angle = self.behavior.random_walk.__last_angle +
		2 * (math.random() - 0.5) * (aaa.time.dt * noise_activity / 180 * math.pi)


	local ttx = tx + speed / sbx * math.cos(new_angle)
	local ttz = tz + speed / sbz * math.sin(new_angle)

	local max_x =  0.5 - radius/sbx * (1 - border)
	local min_x = -0.5 + radius/sbx * (1 - border)
	local max_z =  0.5 - radius/sbz * (1 - border)
	local min_z = -0.5 + radius/sbz * (1 - border)

	if ttx < min_x or ttx > max_x or ttz < min_z or ttz > max_z then
		-- pick new angle
		local min_angle
		local max_angle -- useless for now since max_angle - min_angle is always pi
		if ttx < min_x    then min_angle = -math.pi / 2; 	max_angle =     math.pi / 2 end
		if ttx > max_x    then min_angle =  math.pi / 2; 	max_angle = 3 * math.pi / 2 end
		if ttz < min_z    then min_angle =  0; 				max_angle =     math.pi 	end
		if ttz > max_z    then min_angle = -math.pi; 		max_angle =           0		end

		new_angle = math.random() * math.pi + min_angle
		ttx = tx + speed / sbx * math.cos(new_angle)
		ttz = tz + speed / sbz * math.sin(new_angle)

		if ttx < min_x 	then ttx = min_x end
		if ttx > max_x 	then ttx = max_x end
		if ttz < min_z 	then ttz = min_z end
		if ttz > max_z 	then ttz = max_z end
	end

	rw.__tx = ttx
	rw.__tz = ttz
	-- self:print_verbose(ttx..' '..ttz)
	boid:set_target_x(bx + ttx * sbx)
	boid:set_target_z(bz + ttz * sbz)
	self.behavior.random_walk.__last_angle = new_angle

	-- change target radius
	self:change_radius_randomly()

	-- update y position
	local ym = self.behavior.y_movement
	ym.speed = rw.speed
	ym.border = rw.border
	ym.randomness = rw.randomness
	self:move_target_y_axis_randomly()
end

function GREA:move_target_y_axis_randomly()
	local boid = self:get_boid()
	if not boid then return end

	self:print_verbose( "move_target_y_axis_randomly" )
	local bx, by, bz 		= boid:get_box_xyz()
	local sbx, sby, sbz		= boid:get_box_sxyz()
	local tx, ty, tz 		= boid:get_target_xyz()
	local radius 			= boid:get_target_radius()

	-- tx,ty,tz are between -0.5 and 0.5 (if border == 1, can be less if border < 1)
	local ym = self.behavior.y_movement
	local ty = ym.__ty

	local noise_power 		= ym.speed
	local noise_activity 	= ym.randomness * 3.6 -- * 3.6 since noise_activity is between 0 and 100
	local border 			= ym.border

	local speed = aaa.time.dt * noise_power

	-- todo: hack: a little cheap, angle here?
	local new_delta_y = ym.__last_angle + 2 * (math.random() - 0.5) * (aaa.time.dt * noise_activity / 180 * math.pi)

	local tty = ty + speed / sby * math.cos(new_delta_y)

	local max_y =  0.5 - radius/sby * (1 - border)
	local min_y = -0.5 + radius/sby * (1 - border)
	if tty > max_y then
		new_delta_y = new_delta_y + math.pi
		if math.cos(new_delta_y) < -0.5 then new_delta_y = 2 * math.pi / 3 end
		tty = max_y
	end
	if tty < min_y then
		new_delta_y = new_delta_y + math.pi
		if math.cos(new_delta_y) > 0.5 then new_delta_y = math.pi / 3 end
		tty = min_y
	end

	ym.__ty = tty
	ym.__last_angle = new_delta_y % (2 * math.pi)
	boid:set_target_y(by + tty * sby)
end

function GREA:go_around()
	local boid = self:get_boid()
	if not boid then return end

	self:print_verbose( "go_around" )
	-- length (walls) = 20.7 + 7.995 + 20.7 = 49.395
	-- phase   0 = KL    entrance
	-- phase  21 = KL-KC junction
	-- phase  29 = KC-KR junction
	-- phase  50 = KR 	 entrance
	-- phase  71 = KR-KC junction
	-- phase  79 = KC-KL junction
	-- phase 100 = KL    entrance

	local ga = self.behavior.go_around
	local radius 		= boid:get_target_radius()

	-- hack: rotation bounded to box is a rectangle
	-- we randomly change the box size within the depth limits

	local noise_activity  = 100

	-- change target depth
	local new_depth_angle = ga.__depth_last_angle + 2 * (math.random() - 0.5) * (aaa.time.dt * noise_activity / 180 * math.pi)
	local new_depth = ga.__current_depth + ga.randomness * aaa.time.dt * math.cos(new_depth_angle)
	local depth_max = ga.depth_max
	local depth_min = ga.depth_min + 2 * radius -- need space to put the target in
	if new_depth > depth_max then
		new_depth = depth_max
		new_depth_angle = new_depth_angle + math.pi
	end
	if new_depth < depth_min then
		new_depth = depth_min
		new_depth_angle = new_depth_angle + math.pi
	end
	ga.__depth_last_angle = new_depth_angle % (2 * math.pi)
	boid:set_box_x_min( 0   - new_depth)
	boid:set_box_x_max(20.7 + new_depth)
	boid:set_box_z_min(-4   - new_depth)
	boid:set_box_z_max( 4   + new_depth)
	-- boid:set_box_y_min( 0)
	-- boid:set_box_y_max(10)
	boid:set_box_sf(1)
	ga.__current_depth = new_depth

	-- change target radius
	self:change_radius_randomly()

	-- u-turn if necessary
	local rot = self.behavior.rotation
	local tx, ty, tz 	= boid:get_target_xyz()

	-- hack: update default value, useful when go_around is triggered via the ui.
	-- todo: triggering go_around via the ui should call set_go_around
	rot.__b_is_rotation = ga.b_is_rotation
	if ga.b_is_rotation then rot.b_clockwise = ga.b_begin_clockwise end

	rot.__angle_min 	 = ga.min_pos  / 180 * math.pi
	rot.__angle_max 	 = ga.max_pos  / 180 * math.pi

	if ga.b_is_rotation then
		rot.seconds_per_turn = ga.duration
	else
		rot.seconds_per_turn = ga.duration * 180 / (ga.max_pos - ga.min_pos) -- the duration is min_pos to max_pos round trip
		rot.seconds_per_turn = 0.45 * rot.seconds_per_turn 	-- hack:	the sec per turn computed here is with the origin as center of the rotation
	end												  		-- 	   		the real value depends on the size of the box, which fluctuates
													  		--	   		0.35-0.50 are good estimators of the expected value.
	self:set_y_movement({
		speed 	= 0.5,
		border 	= 0
	})
	rot.radius = 100
	rot.adherence = 1000

	-- move target
	self:rotate_target()

	-- update y position
	self:move_target_y_axis_randomly()
end

function GREA:change_radius_randomly()
	local boid = self:get_boid()
	if not boid then return end

	self:print_verbose( "change_radius_randomly" )

	local rr = self.behavior.radius_changes
	local radius 		= boid:get_target_radius()

	local noise_activity  = 100

	local new_radius_angle = rr.__radius_last_angle + 2 * (math.random() - 0.5) * (aaa.time.dt * noise_activity / 180 * math.pi)
	local new_radius = radius + 0.1 * aaa.time.dt * math.cos(new_radius_angle) -- magic number.

	if new_radius > rr.radius_max then
		new_radius = rr.radius_max
		new_radius_angle = new_radius_angle + math.pi
	end
	if new_radius < rr.radius_min then
		new_radius = rr.radius_min
		new_radius_angle = new_radius_angle + math.pi
	end
	rr.__radius_last_angle = new_radius_angle % (2 * math.pi)
	boid:set_target_radius( new_radius, new_radius/2 )
end

function GREA:clamp_target_coordinates()
	local boid = self:get_boid()
	if not boid then return end

	self:print_verbose( "clamp_target_coordinates" )
	if true then return end
	-- note: this clamp is done to prevent the target from being out of box when the box is moved.
	-- random movement resulting in out of box target should already handle and prevent clamping.
	-- still a bit hacky.
	local bx, by, bz 		= boid:get_box_xyz()
	local sbx, sby, sbz		= boid:get_box_sxyz()
	local tx, ty, tz 		= boid:get_target_xyz()
	local radius 			= boid:get_target_radius()

	local y_border = self.behavior.y_movement.b_active and self.behavior.y_movement.border or 0
	local max_y = by + sby/2 - radius * (1 - y_border)
	local min_y = by - sby/2 + radius * (1 - y_border)
	if ty > max_y then boid:set_target_y(max_y) end
	if ty < min_y then boid:set_target_y(min_y) end

	local xz_border = self.behavior.random_walk.b_active and self.behavior.random_walk.border or 0
	local max_x = bx + sbx/2 - radius * (1 - xz_border)
	local min_x = bx - sbx/2 + radius * (1 - xz_border)
	if tx > max_x then boid:set_target_x(max_x) end
	if tx < min_x then boid:set_target_x(min_x) end
	local max_z = bz + sbz/2 - radius * (1 - xz_border)
	local min_z = bz - sbz/2 + radius * (1 - xz_border)
	if tz > max_z then boid:set_target_z(max_z) end
	if tz < min_z then boid:set_target_z(min_z) end
end

-- legacy funtion, still used in go_around as a patchwork.
function GREA:rotate_target()
	local boid = self:get_boid()
	if not boid then return end

	self:print_verbose( "rotate_target" )
	local rot = self.behavior.rotation
	local ga = self.behavior.go_around

	local  tx,  ty,  tz		= boid:get_target_xyz()
	local  bx,  by,  bz 	= boid:get_box_xyz()
	local sbx, sby, sbz		= boid:get_box_sxyz()
	local target_radius		= boid:get_target_radius()
	local rotation_radius = rot.radius

	local rotation_seconds_per_turn = rot.seconds_per_turn
	local rotation_speed = 1 / rotation_seconds_per_turn * 360
	local direction = rot.b_clockwise and 1 or -1

	local delta_angle = direction * aaa.time.dt * rotation_speed / 180 * math.pi
	local cos_d = math.cos(delta_angle)
	local sin_d = math.sin(delta_angle)

	local ttx = tx - bx
	local ttz = tz - bz
	local new_ttx = ttx * cos_d - ttz * sin_d;
	local new_ttz = ttz * cos_d + ttx * sin_d;

	local new_tx = bx + new_ttx
	local new_tz = bz + new_ttz

	-- case too small
	if rotation_radius ~= 0 then
		local norm2 = new_ttx * new_ttx + new_ttz * new_ttz
		if rotation_radius * rotation_radius > norm2 then
			local adherence = rot.adherence
			local norm = math.sqrt(norm2)
			local new_norm = math.min(rotation_radius, norm + aaa.time.dt * adherence)
			new_ttx = new_ttx * new_norm / norm
			new_ttz = new_ttz * new_norm / norm
			new_tx = bx + new_ttx
			new_tz = bz + new_ttz
			end
		if rotation_radius * rotation_radius < norm2 then
			local adherence = rot.adherence
			local norm = math.sqrt(norm2)
			local new_norm = math.max(rotation_radius, norm - aaa.time.dt * adherence)
			new_ttx = new_ttx * new_norm / norm
			new_ttz = new_ttz * new_norm / norm
			new_tx = bx + new_ttx
			new_tz = bz + new_ttz
		end
	end
	-- self:print_verbose(rotation_radius)
	-- self:print_verbose(math.sqrt(new_ttx * new_ttx + new_ttz * new_ttz))
	-- case out of bound
	if     new_tx - target_radius < bx - sbx/2 or new_tx + target_radius > bx + sbx/2
		or new_tz - target_radius < bz - sbz/2 or new_tz + target_radius > bz + sbz/2 then
			-- todo: double check: seems a bit brutal / wrong
			local adherence = self.behavior.rotation.adherence
			local norm = math.sqrt(new_ttx * new_ttx + new_ttz * new_ttz)
			local delta_x = math.max(math.abs(new_ttx - target_radius), math.abs(new_ttx + target_radius))
			local delta_z = math.max(math.abs(new_ttz - target_radius), math.abs(new_ttz + target_radius))
			local norm_ratio = math.min((sbx/2 - target_radius) / delta_x, (sbz/2- target_radius) / delta_z)
			-- self:print_verbose(sbx ..'  '.. ttx)
			-- self:print_verbose(delta_x.." "..delta_z)
			-- self:print_verbose(norm_ratio)
			norm_ratio = math.max(norm_ratio, (norm - adherence) / norm)
			new_tx = bx + new_ttx * norm_ratio
			new_tz = bz + new_ttz * norm_ratio
	end

	-- self:print_verbose(new_tx.." "..new_tz)
	boid:set_target_x(new_tx)
	boid:set_target_z(new_tz)

	if self.__b_etienne_maa_hack_need_restart then
		-- spawn fishes near the target
		local r = boid:get_target_radius_internal()
		local x,y,z = boid:get_target_x(), boid:get_target_y(), boid:get_target_z()
		boid:set_birth_min_max_xyz( x-r, x+r, y-r, y+r, z-r, z+r )
		boid:trig_restart()
		self.__b_etienne_maa_hack_need_restart = false
	end

	if not rot.__b_is_rotation then
		rot.__angle_current = math.atan2(new_ttz, new_ttx)
		if rot.__angle_current < rot.__angle_min then rot.b_clockwise = true  end
		if rot.__angle_current > rot.__angle_max then rot.b_clockwise = false end
	end
end

function GREA:send_fish_length_to_deformer()
	local field = self.ref.distance_field
	if field then
		aaa.bdd.update_fish_length( field, self.__race.__len_max )
	end
end