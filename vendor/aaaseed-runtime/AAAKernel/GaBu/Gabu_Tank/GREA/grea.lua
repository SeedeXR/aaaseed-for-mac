--
--	GREA
--		this is a Groupe of creature
--		it agglomerate the race and boids
--
CLASS.DECLARE( "GREA" )

function GREA:print_verbose( str )
--	self:print(str)
end
function GREA:alloc_crea( nb, id_offset )
	--the __pool is where all the creature if a GREA are stored
	self.__pool = CREATURE:create_pool( self, nb, id_offset )
end
function GREA:pop_crea_free()
	return table.remove( self.__pool )
end
function GREA:push_crea_free(crea)
--	aaa.print_fn()
	table.insert( self.__pool, crea )
end
function GREA:get_crea_random()
	local pool = self.__pool
	local id = math.random( #pool )
	return pool[id]
end

function GREA:set_curve_dist_nb_direct( dist, nb )
	--aaa.print_method()
	--self.dist = dist
	local f = 1
	dist = dist / f
	self.dist_squared = dist * dist
	self.pt_nb = (nb or 10) * f
end
function GREA:set_curve_dist_nb( dist, nb )
	self:set_curve_dist_nb_direct( dist, nb )
	local aqua = TANK.cur.aqua
	if aqua and aqua:is_master() then
		aqua:send_osc( "grea/set_curve_dist_nb_direct", self.id, dist, nb )
	end
end

function GREA:assign_race( race )
	self.__race = race
--	local pool = self.__pool
--	for i = 1, #pool do
--		pool[i].race = race
--	end
end
function GREA:get_race()
	return self.__race
end

function GREA:erase_curve_rt()
	param.set( self.__curve_rt_curve_erase_ref, true )
end


function GREA:create_empty( name )
	local self = GREA:create_instance(name)
	return self
end

--todo complete
function GREA:free()
	aaa.obj.delete( self.__curve_rt_bdd )
	self.__curve_rt_bdd = nil
end
function GREA:create( name, id, race, nb_allocated, id_offset, boid_id )
	local self = GREA:create_empty(name)
	self.id = id
	self:assign_race( race )
	--todo get_curve_rt_layer_and_bdd() clone an object so we recreate every time we RUN.lua (2023 Jan)
	self.__curve_rt_layer, self.__curve_rt_bdd = TANK.cur:get_ggrea():get_curve_rt_layer_and_bdd()
		self.__curve_rt_use_bdd_ref		= param.get_ref( self.__curve_rt_layer,	"use_bdd"		)
		param.set( self.__curve_rt_bdd, "channel_id", boid_id )
		self.__curve_rt_curve_erase_ref	= param.get_ref( self.__curve_rt_bdd, 	"erase_all"		)
		self.__curve_rt_draw_point_ref	= param.get_ref( self.__curve_rt_bdd,	"draw_point"	)

	self:alloc_crea( nb_allocated, id_offset )

	self:set_curve_dist_nb_direct( race.seg_len )
	if boid_id then
		self:assign_boid(boid_id)
		if app.is_fish_send then
			param.set( self.__curve_rt_bdd, "net_send", 	app:is_fish_send() )
			param.set( self.__curve_rt_bdd, "channel_id",	boid_id	)
		end
	end

	local tank = TANK.cur
	local boid_attr_ref = tank:get_fish_layers( "Boid_Attr"		)

	if not self.ref then self.ref = {} end
	self.ref.distance_field = aaa.obj.get_down_by_class_no_error( boid_attr_ref, "def_distance_field")

	return self
end

-- BOID
--
function GREA:get_boid()
	return self.__boid
end
function GREA:get_layers()
	return self.__boid_layers_ref
end
function GREA:get_boid_and_layers()
	return self.__boid, self.__boid_layers_ref
end

function GREA:set_boid_rgba( r,g,b, a )
	local layer = aaa.layers.get_layer( self.__boid_layers_ref, 1 )
	if layer then
		local color = aaa.obj.get_down_by_class( layer, "color" )
		if color then
			param.set( color, "global_red",		r )
			param.set( color, "global_green",	g )
			param.set( color, "global_blue",	b )
			param.set( color, "global_alpha",	a )
		end
	end
end

function GREA:focus_boid()
	aaa.obj.become_ui( self.__boid_layers_ref )
	aaa.obj.set_focus_ui( self.__boid_bdd_ref )
end
function GREA:restart_boid()
	self:get_boid():trig_restart()
end

function GREA:init_box_xyz( min_x, max_x, min_y, max_y, min_z, max_z )
	local boid = self:get_boid()
	if boid then
		boid:set_box_min_max_xyz( min_x, max_x, min_y, max_y, min_z, max_z )
		boid:set_birth_min_max_xyz( min_x, max_x, min_y, max_y, min_z, max_z )
	end
end

function GREA:init_target_xyz( x, y, z )
	local boid = self:get_boid()
	if boid then
		boid:set_target_xyz( x, y, z )
	end
end
function GREA:init_target_at_box_center()
	local boid = self:get_boid()
	if boid then
		local bx, by, bz = boid:get_box_xyz()
		boid:set_target_xyz( bx, by, bz )
	end
end
function GREA:init_target_radius( external, internal )
	local boid = self:get_boid()
	if boid then
		boid:set_target_radius( external, internal, false )
	end
end
function GREA:init_target_influence( influence )
	local boid = self:get_boid()
	if boid then
		boid:set_target_influence( influence, false )
	end
end
function GREA:init_target_active( b_active )
	local boid = self:get_boid()
	if boid then
		boid:set_target_active( b_active, false )
	end
end


function GREA:assign_boid( id )
	self.__boid_layers_ref = TANK.cur:get_boid_layers( id )
	local comment = self:get_name()
	if self.__boid_out then
		comment = "<- " .. comment
	end
	param.set( self.__boid_layers_ref, "comment", comment )
	self.__boid_layer_ref = aaa.layers.get_layer( self.__boid_layers_ref, 1 )

	self.__use_bdd_ref = param.get_ref( self.__boid_layer_ref, "use_bdd" )
	param.set( self.__use_bdd_ref, "update_and_draw" )
	param.set_save( self.__use_bdd_ref, false )
	self.__boid_bdd_ref = aaa.obj.get_branch_by_class( self.__boid_layer_ref, "bdd_boid" )
	param.set( self.__boid_bdd_ref, "draw_force", true )
	param.set_save( self.__boid_bdd_ref, "draw_force", false )
	self.__boid = BOID:create( "boid_" .. id, self.__boid_bdd_ref )
	self.__color_ref = aaa.layer.create_color( self.__boid_layer_ref, "boid" )

	local boid = self.__boid
	--	self.boids[id] = boid
	--todo if id is not a number ???
	boid:set_param( "id", id )
	if not self.__boid_out then
		boid:set_nb( #self.__pool )
		boid:set_nb_no_save()
	end

	local race = self:get_race()
	if race then
		local curvature = race.curvature
		if curvature then
			--self:box_debug( "have curvature for race "..race )
			boid:set_curvature( curvature.limit, curvature.turn_by_sec )
		end
	else
		self:print_error( "racer should be defined here" )
	end
	--todo have default value in GaBu UI with Force Buttons
	--boid:set_box_influence( 1., 1. )
	--boid:set_box_border_size( .25 )


--nomore	boid:set_accel_early_limit( false )

	boid:set_mocap_feed( false )
	boid:trig_restart()
end

function GREA:assign_boid_out( id, nb, dlen, len_before )
	self.__boid_out = { nb = nb, dlen = dlen, len_before = len_before }
	self:assign_boid( id )
end
function GREA:is_boid_out()	return self.__boid_out ~= nil	end
function GREA:get_boid()	return self.__boid				end

function GREA:start_on_curve_direct( node_id, s, speed, id_rnd )
--	aaa.print_fn()
	local crea = self:pop_crea_free()
	if crea then
		--self:print( "self.on_curve now "..#self.on_curve )
		crea.__curve_bdd = TANK.cur.ref.curve_edit.bdd
		crea.__curve_id = node_id

		crea.len_curve = aaa.bdd.get_len( crea.__curve_bdd, crea.__curve_id )
		if not crea.len_curve then			crea:box_error( "len_curve is nil for "..crea.__curve_id )
											crea.len_curve = 1
		elseif crea.len_curve == 0 then		crea:box_error( "len_curve is 0. for "..crea.__curve_id )
											crea.len_curve = 1
		end
		crea.len_curve_over = 1 / crea.len_curve

		crea.s = s + self:get_race().__len_curve_needed
		crea.s_speed = speed
		crea.age = 0

		crea.__id_rnd = id_rnd

		--table.print( crea, "crea start_on_curve_direct", 1 )
		local on_curve = self:get_table_always( "__on_curve" )
		table.insert( on_curve, crea )
	else
		self:print_error( "no crea available to start_on_curve_direct" )
	end
end
function GREA:start_on_curve( node_id, s, speed )
	math.randomseed( aaa.time.t )
	local id_rnd = math.random( 0, 117 )
	self:start_on_curve_direct( node_id, s, speed, id_rnd )
	local aqua = TANK.cur.aqua
	if aqua and aqua:is_master() then
		aqua:send_osc( "grea/start_on_curve_direct", self.id, node_id, s, speed, id_rnd )
	end
end

function GREA:set_launcher( node_list, t_min, t_max, speed_min, speed_max, b_no_repeat )
	self.__launcher = {
		node_list = node_list,
		t_min = t_min,
		t_max = t_max,
		speed_min = speed_min,
		speed_max = speed_max,
		b_no_repeat = (b_no_repeat or false)
	}
	self:do_launcher(true)
end

local b_verbose = false

function GREA:do_launcher( b_do_it, id_curve_to_pick )
--	aaa.print_fn()
	if not self then
		return
	end

	local l = self.__launcher
	local delay = l.t_min + (l.t_max - l.t_min) * math.random()
	l.t_next = aaa.time.t + delay
	if b_do_it then
		local nl = l.node_list
		local nb = #nl
		while not id_curve_to_pick do
			id_curve_to_pick = math.random(nb)
			--	avoid to pick the same curve twice in a row
			if nb ~= 1 then
				if l.last_index == id_curve_to_pick then
					id_curve_to_pick = nil
				else
					l.last_index = id_curve_to_pick
				end
			end
		end
		if b_verbose then
			self:print("at " .. aaa.time.get_str_hms() .. " curve " .. id_curve_to_pick .. " out of " .. #nl)
		end
		local id = nl[id_curve_to_pick]
		local speed = l.speed_min + (l.speed_max - l.speed_min) * math.random()
		self:start_on_curve( id, 0, speed )
	end
end
function GREA:can_launch_req()
	local b_can = true
	local tank = TANK.cur
	local grea_cacha = tank:get_grea_by_name( "cacha" )
	if grea_cacha.on_curve_younger and grea_cacha.on_curve_younger < 60. then
		if b_verbose then self:print( "Cachalot block other" ) end
		b_can = false
	end
	local grea_req = tank:get_grea_by_name( "req" )
	if grea_req.on_curve_younger and grea_req.on_curve_younger < 8. then
		if b_verbose then self:print( "Req block other" ) end
		b_can = false
	end
	return b_can
end
function GREA:can_launch()
	local b_can = true
	local tank = TANK.cur

	local grea_cacha = tank:get_grea_by_name( "cacha" )
	if grea_cacha.on_curve_younger and grea_cacha.on_curve_younger < 60. then
		if b_verbose then self:print( "Cachalot block other" ) end
		b_can = false
	end
	local grea_sun = tank:get_grea_by_name( "sun" )
	if grea_sun.on_curve_younger and grea_sun.on_curve_younger < 40. then
		if b_verbose then self:print( "Sun block other" ) end
		b_can = false
	end
	local grea_req = tank:get_grea_by_name( "req" )
	if grea_req.on_curve_younger and grea_req.on_curve_younger < 40. then
		if b_verbose then self:print( "Req block other" ) end
		b_can = false
	end
	return b_can
end

function GREA:__update_launcher()
	local l = self.__launcher

	if l.t_next < aaa.time.t then --todo carefull a reset or chenge it time don't lock it out
		--self:print( "try to create on curve" )
		local b_can = self:can_launch()
		local tank = TANK.cur
		if self == tank:get_grea_by_name( "cacha" ) then
			if b_can then
				self:do_launcher( true )
			end
		else
			self:do_launcher( b_can )
		end
	end
end

function GREA:set_blow( blow )
	self.blow = blow
	--self:print( "set_blow( "..blow )
	local aqua = TANK.cur.aqua
	if aqua and app:is_master() then
		local speed = clamp_01( blow * .33 )
		speed = interpolate( 2.5, 1., speed )
		self.__boid:set_speed_min_max( speed * .5, speed )
		aqua:send_osc( "grea/set_blow", self.id, blow )
	end
end

function GREA:__updraw_boid_obj( b_update )
	if GA.b_spy then aaa.spy.push_range( "updraw_boid_obj", 2 ) end

		local tank = TANK.cur
		--self:print( tank.b_boid_draw_ui )

		--self:print( "__updraw_boid_obj() "..tank.b_boid_update )
		b_update = b_update and tank.b_boid_update

		--hackmona
--		if tank.b_boid_draw_ui and (not app.__b_is_fish_boid) then --update if needed then draw
		if tank.b_boid_draw_ui then --update if needed then draw
			--self:print( tank.b_boid_draw_ui )
			--hackmona
			--if b_update and (not app.__b_is_fish_boid) then
				aaa.obj.update_then_draw( tank:get_ggrea().boid_attr_layers )
			--end
			local how = b_update and "update_and_draw" or "draw"
			--self:print( how )
			param.set( self.__use_bdd_ref, how )
			aaa.obj.update_then_draw( self.__boid_layer_ref )
		else	--update if needed no draw
			--self:print( "just update" )
			if b_update then
				param.set( self.__use_bdd_ref, "update" )
				--aaa.obj.update( self.__boid_bdd_ref )
				aaa.obj.update( self.__boid_layer_ref ) --	use deformations doing it with the layer, was : aaa.obj.update( self.__boid_bdd_ref )
			end
		end
		self.__color = self.__color_ref:get_rgba()

	if GA.b_spy then aaa.spy.pop_range() end
end

--todomona see what we do with b_update
function GREA:__updraw_curve_rt( b_update )
	local tank = TANK.cur

	if GA.b_spy then aaa.spy.push_range( "GREA:__updraw_curve_rt ", 4 ) end

		if tank.b_curve_rt_draw then
			--self:print( "Draw Curve" )
			-- draw attrib
			aaa.obj.update_then_draw( GGREA.curve_rt_attr_layer )
			--gol.color( self.__color )
			-- draw curve
			aaa.layer.set_bdd_external( self.__curve_rt_layer, self.__curve_rt_bdd )
			param.set( self.__curve_rt_draw_point_ref, tank.b_curve_rt_point_draw )
			param.set( self.__curve_rt_use_bdd_ref, b_update and "update_and_draw" or "draw" )
			GGREA.curve_rt_color_ref:set_rgba_t( self.__color )
			aaa.obj.update_then_draw( self.__curve_rt_layer )
		else
			if b_update then
				--param.set( self.__curve_rt_use_bdd_ref, "update" )
				--aaa.obj.update( self.__curve_rt_layer )
				aaa.obj.update( self.__curve_rt_bdd )
			end
		end

	if GA.b_spy then aaa.spy.pop_range() end
end

function GREA:__update_creas_len()
	local bdd_curve = self.__curve_rt_bdd
	local get_len = aaa.bdd.get_len

	local pool = self.__pool
	for i = 1, #pool do
		local crea = pool[i]

		local len = get_len( bdd_curve, crea.__curve_id )
		crea.len_curve = len
		crea.len_curve_over = 1. / len
	end
end


function GREA:draw_transparent( info )
	local pool = self.__pool
	info.id_max = #pool * info.density
	for i = 1, #pool do
		local crea = pool[i]
		crea:draw_transparent( info )
	end
end

function GREA:__update_curve_rt_from_boid( boid )

	aaa.bdd.set_lua_cur( boid )
	local boid_nb = aaa.bdd.get_point_nb()
	if boid_nb <= 0 then return end

	if GA.b_spy then aaa.spy.push_range( "GREA:update_curve_rt_from_boid", 3 ) end

		local aqua = TANK.cur.aqua
		local b_master_loop = aqua and aqua.b_master_loop or true
		local pt = V3.new()
		local pool = self.__pool
		local get_point_and_id = aaa.bdd.get_point_and_id_local
-- ui is commented so
--		local f2 = TANK.cur.curve_factor
		local d2_max = TANK.cur.b_machine_mono and (.001*.001) or self.dist_squared
		--local d2_max = .01 * .01
		--local d2_max = self.dist_squared
		--aaa.show(math.sqrt(d2), "d")
		local id, x, y, z, dum

		--todomona index start at 1 ?
		if b_master_loop and app.__b_is_fish_boid and TANK.cur.b_machine_mono then
			for i = 1, #pool do
				--self:print( i )
				x,y,z, id = get_point_and_id(i)
				--todo avoid test in a loop
				if id == 0 then --	boid obj not ready initialized ...
					break	-- we can't just return because of spy.push/pop
				end

				--aaa.print( id.." "..x )
				local crea = pool[id]
				if crea then
					local pt_last = crea.pt_last
					local d = pt_last[1] - x
					local d2 = d * d
					d = pt_last[2] - y
					d2 = d2 + d * d
					d = pt_last[3] - z
					d2 = d2 + d * d
					if d2 == 0. then
						self:print( "update_len_moved with 0 distance, check we don't do it twice" )
					end
					pt_last[1] = x
					pt_last[2] = y
					pt_last[3] = z
					--todo here and in other places we do it this way but after a while we loose precision
					crea.len_moved = crea.len_moved + math.sqrt(d2)
					--self:print( crea.len_moved )
					--	do direct self:add_curve_rt_point( crea, x,y,z )
					local len = aaa.bdd.push_control_point_back_len_max( self.__curve_rt_bdd, crea.__curve_id, x,y,z, crea.__len_curve_needed )
					crea.len_curve = len
					crea.len_curve_over = 1. / len
					
				else
					self:print( "no crea for id : " .. id )
				end
			end
		else
			for i = 1, #pool do
				--self:print( i )
				x,y,z, id = get_point_and_id(i)
				--todo avoid test in a loop
				if id == 0 then --	boid obj not ready initialized ...
					break	-- we can't just return because of spy.push/pop
				end

				--aaa.print( id.." "..x )
				local crea = pool[id]
				if crea then
					--todo opt
					if crea:update_len_moved( x,y,z, d2_max ) then
						if b_master_loop then
							self:add_curve_rt_point( crea, x,y,z )
						else
							aqua:send_osc( "grea/add_curve_rt_point", self.id, id, x,y,z )
						end
					end
				else
					self:print( "no crea for id : " .. id )
				end
			end
		end

	if GA.b_spy then aaa.spy.pop_range() end
end

--this for SoOuest where the kids could virtually blow (gonfler) fish
function GREA:update_blow( blow )

	self:print_verbose( "GREA:update_blow() should only happen to SoOuest kid fish")
	aaa.bdd.set_lua_cur( self.__boid_bdd_ref )
	local boid_nb = aaa.bdd.get_point_nb()
	if boid_nb == 0 then return end

	if GA.b_spy then aaa.spy.push_range( "GREA:update_blow Process", 4 ) end

		local size_min = math.min(blow * .5, .9)
		local size_add = math.min(.3, blow)

		blow = math.max(0., blow - 1.)

		local sel
		if self.blow > 2. then
			sel = (self.blow - 2.) * 34
		else
			sel = -1
		end

		local pool = self.__pool
		local get_point = aaa.bdd.get_point_and_id_local

		local inter = clamp_01( 1. - aaa.time.dt * 16 )

		for i = 1, #pool do
			--		if i == 1 then
			local x, y, z, id = get_point(i)

			--todo avoid test in a loop
			if id == 0 then --	boid obj not ready initialized ...
				break	-- we can't just return because of spy.push/pop
			end
			--local dum
			--dum, id = poid.split_id( id )
			--		end
			--aaa.print( id.." "..x )

			local crea = pool[id]
			local tmp = clamp( blow - crea.id_in_pool / 64, 0., 1.2 )
			crea.blow = tmp * tmp
			if self.blow < .65 then
				crea.b_exploded = false
			end
			local target
			if id < sel or crea.b_exploded then
				target = .02
				crea.b_exploded = true
			else
				math.randomseed(id * 543)
				target = math.min(size_min + size_add * math.random(), 1.)
			end

			local now = crea.force_size_factor or target
			crea.force_size_factor = now * inter + target * (1 - inter)
		end

	if GA.b_spy then aaa.spy.pop_range() end
end

function GREA:interpolate_loyal( pos, tar, t, ib, ie )
	if true then
		local function slerp( i, t )
			local a =	{	pos[i]-pos[ib],	pos[i+1]-pos[ib+1], pos[i+2]-pos[ib+2]	}
			local b =	{	tar[i]-tar[ib],	tar[i+1]-tar[ib+1], tar[i+2]-tar[ib+2]	}
			local c =	V3.slerp( a, b, t )
			pos[i]		= c[1] + pos[ib]
			pos[i+1]	= c[2] + pos[ib+1]
			pos[i+2]	= c[3] + pos[ib+2]
		end
		for i = ib+3,ie,3 do
			--local f = math.pow( t, .1 + .9 * (i-ib-4)/(ie-ib-4) )
			slerp( i, t )
		end
	else
		for i = ib,ie,3 do
			pos[i]		= interpolate(	pos[i],		tar[i],		t )
			pos[i+1]	= interpolate(	pos[i+1],	tar[i+1],	t )
			pos[i+2]	= interpolate(	pos[i+2],	tar[i+2],	t )
		end
	end
end

function GREA:__compute_points( pool, b_on_curve )
	--todoaqua this line should be there
	if not self:is_active() then return end
	if not pool then return end
	local nb = #pool
	if nb == 0 then return end
	--aaa.print_fn()

	if GA.b_spy then aaa.spy.push_range( "__compute_points", 1 ) end

		local race = self:get_race()
		local clip_dist = race.clip_dist

		--todo check
		local tank = TANK.cur
		local b_mono = tank.b_machine_mono

		aaa.show( tank.b_deferred, "Fish deferred" )
		aaa.show( tank.b_machine_mono, "Machine Mono" )
		local method_draw = (tank.b_draw_fish and (not tank.b_deferred)) and CREATURE.draw_deformed
		--aaa.show( method_draw, "method_draw" )

		--todo optimize when no clip testing
		--preprocess to set once
		local clipper	= tank.aqua	and	tank or	tank.__meu_tank_master
		local fn_clip
		if app.clip_fish then
			clipper = app
			fn_clip = app.clip_fish
		end
		if not clipper then
			--todomona was and all this clip stuff below have to be cleaned
			clip_dist = nil
		else
			if not fn_clip then
				fn_clip	= tank.aqua	and	tank.is_fish_clipped or	clipper.clip_fish
			end
			if not clip_dist then
				self:print_error( "we have fn_clip, but clip_dist is "..clip_dist )
				clip_dist = .1
				race.clip_dist = clip_dist --avoid future messages
			end
		end

		aaa.bdd.set_lua_cur(pool[1].__curve_bdd)
		local get_point = aaa.bdd.get_point_s_curve_fast
		local point_i_max = (race.catmull_seg_nb + 1) * 3 + 1

		local creas_nb_cur = tank.__creas_nb
		--self:print( creas_id )
		--self:print( "nb is "..nb )
		--self:print( GGREA.b_deform )
		if not GGREA.b_deform then nb = 1 end
		local b_update_len_move = not app.__b_is_fish_boid
		for crea_i = 1,nb do
			local crea = pool[crea_i]
			crea.dlen = crea.seg_len

	--		if (crea.s_draw ~= s or true) and crea.len_curve and crea.len_curve > 0. then
	--			crea.s_draw = s

			--todo preprocess by id
				local force_size_factor = crea.force_size_factor
				if force_size_factor then
					--self:print( "force_size_factor" )
					crea.size_factor = force_size_factor
				end

				local pos = crea.pos
				local curve_id = crea.__curve_id

				local ds_cano = crea.seg_len * crea.len_curve_over
				local x, y, z
				local b_clip = fn_clip and true or false

				if b_mono then
					local s_cano = b_on_curve and crea.s * crea.len_curve_over or 1

					--we test cliping only on point of segments used
					for i = 4, point_i_max, 3 do
						x,y,z = get_point( curve_id, s_cano )
						pos[i], pos[i+1], pos[i+2] = x,y,z
						if fn_clip and not fn_clip( clipper, x, y, z, clip_dist ) then
							b_clip = false
						end
						s_cano = s_cano - ds_cano
					end
					local i = point_i_max + 3
					x, y, z = get_point( curve_id, s_cano )
					pos[i], pos[i+1], pos[i+2] = x, y, z

					if self.b_loyal then
						s_cano = b_on_curve and crea.s * crea.len_curve_over or 1
						x, y, z = get_point( curve_id, s_cano )

						local tar = crea:get_table_always( "posb" )
						--generate curve
						local dx, dy, dz = 0, 0, -crea.seg_len

						for i = 4, point_i_max+3, 3 do
							local f = (i-1)/3 - 1
							local vx,vy,vz = 0,0,f*dz
							--vx = vx + math.sin( vz*5 + aaa.time.t * .458445 ) * .2 * vz
							vy, vz = math.rotate_ab_turn( vy, vz, tank.loyal_rot_x or 0)
							vx, vz = math.rotate_ab_turn( vx, vz, tank.loyal_rot_y or 0 )

							tar[i], tar[i+1], tar[i+2] = x+vx, y+vy, z+vz
							--tar[i] = tar[i] + math.sin(f+aaa.time.t*4)  * .05
						end

						--self:interpolate_loyal( pos, tar, math.sin(aaa.time.t)*.5+.5, 5, point_i_max+4 )
						self:interpolate_loyal( pos, tar, tank.loyal or 0, 4, point_i_max+3 )
					end

					pos[1] = 2 * pos[4] - pos[7]
					pos[2] = 2 * pos[5] - pos[8]
					pos[3] = 2 * pos[6] - pos[9]

					--hack for lv to Milan use and to clean (generalize)
					--if y< 0 then
					--	b_clip = true
					--end
				else	-- if b_mono
					local s = crea.s
					local s_inter = math.fmod( s - crea.s_decal, crea.seg_len )
					s = s - s_inter
					crea.s_inter = s_inter
					local s_cano = s * crea.len_curve_over

					b_clip = fn_clip and true or false
					pos[1], pos[2], pos[3] = get_point( curve_id, s_cano )
					s_cano = s_cano - ds_cano

					for i = 4, 13, 3 do
						x, y, z = get_point( curve_id, s_cano )
						pos[i], pos[i+1], pos[i+2] = x, y, z
						if fn_clip and not fn_clip( clipper, x, y, z, clip_dist ) then
							b_clip = false
						end
						s_cano = s_cano - ds_cano
					end
					x, y, z = get_point( curve_id, s_cano )
					pos[16], pos[17], pos[18] = x, y, z
				end		-- if b_mono
				crea.__b_clip = b_clip

				--self:print( b_clip )
				--self:print( ds )
				--yodo check if we don't do it twice
				if b_update_len_move then
					crea:update_len_moved( pos[4],pos[5],pos[6], (.001*.001) )
				end

--
--TOUCH
				local t = crea.__touch
				if not t then
					local touch = race.touch
					if touch then
						local b, x = tank.__meu_tank_master:test_fish(pos[7], pos[8], pos[9])
						if b then
							--self:print( crea.." touched" )
							t = { time = 0, val = 0, acc = -9.81, speed = 4, how = touch }
							crea.__touch = t
							if touch.snd then
								tank:play_fish_sound( touch.snd, x ) --touch sound volu,e is out
							end
							if touch.type == "map" then
								crea:pick_touch_map()
							end
						end
					end
				end

				if t then
					local how = t.how
					local asr = how.asr
					local val
					if asr then
						local time = t.time + aaa.time.dt
						t.time = time
						val = math.fn_linear(time, 1, 0, 0, asr[1], 1, asr[2], 1, asr[3], 0)
					else
						local speed = t.speed + t.acc * aaa.time.dt
						t.speed = speed
						val = t.val + speed * aaa.time.dt
					end
					if val > 0 then
						t.val = val
					else
						crea:remove_touch()
					end
				end

				if not b_clip then
					creas_nb_cur = creas_nb_cur + 1
					tank.__creas[creas_nb_cur] = crea
					--todo regroup with below at the beginning
					if method_draw then
						--crea:print("crea:draw_deformed()")
						method_draw(crea)
					end
				end

		--	end		-- if (crea.s_draw ~= s or true) and crea.len_curve and crea.len_curve > 0. then

		end	--loop on creatures in the crea

		tank.__creas_nb = creas_nb_cur
		--self:print( "__creas_nb is "..creas_nb_cur )
		--todo deal with remove
		--todo deal with loop

	if GA.b_spy then aaa.spy.pop_range() end
end

function GREA:move_on_path_v1( curve_rt_bdd, pool, dt )
	local ACC = 4.
	local FLEN = .92

	dt = dt * (TANK.cur.creature_speed or 1.)

	for i = 1, #pool do
		local crea = pool[i]
		--todo remove ?
		crea.__curve_bdd = curve_rt_bdd

		local len_curve = crea.len_curve

		if (not len_curve) or len_curve <= 0. then
			--crea:print_error( "draw_boid() len_curve is nil for id "..id )
		else
			--if GA.b_spy then aaa.spy.add_mark( id, 2 ) end
			local s = crea.s
			if s < .0 then
				s = 0.
			end

			local speed = crea.s_speed
			speed = speed * (1 - dt) --todo use it when far ?
			speed = speed + ACC * (len_curve * FLEN - s) * dt
			if speed < 0. then
				speed = 0.
			end

			local ds = speed * dt
			s = s + ds
			crea.len_moved = crea.len_moved + ds

			--s = len_curve * .7
			--speed = 0.
			s = clamp( s, 0, len_curve * .95 )

			crea.s = s
			crea.s_speed = speed
			crea.s_cano = s * crea.len_curve_over
		end
	end
end

function GREA:__move_on_path()
	if GA.b_spy then	aaa.spy.push_range( "__move_on_path ", 2 ) end

		self:move_on_path_v1( self.__curve_rt_bdd, self.__pool, aaa.time.dt )

	if GA.b_spy then	aaa.spy.pop_range()	end
end

function GREA:__feed_boid( id, crea )
	local bo = self.__boid_out
	local boid = self.__boid_bdd_ref
	local s = ( crea.s - crea.seg_len * 2. - bo.len_before ) * crea.len_curve_over
	local ds = bo.dlen * crea.len_curve_over
	local start = (id - 1) * bo.nb + 1
	local s = s + ds
	local curve_id = crea.__curve_id
	local get_point = aaa.bdd.get_point_s_curve_fast
	aaa.bdd.set_lua_cur( self.__curve_rt_bdd )
	for i = start, start + bo.nb - 1 do
		--self:print( id_curve )
		local x, y, z = get_point( curve_id, s )
		--self:print( id.."-"..i.." "..s.." -> "..x.." "..y.." "..z )
		s = s - ds
		poid.create_at( boid, x, y, z )
	end
end

function GREA:__update_on_curve( pool, b_master )

	--aaa.print_method()
	self.on_curve_nb		= 0
	self.on_curve_older		= -1000
	self.on_curve_younger	= 1000

	--self:print( "__update_on_curve()" )
	local b_feed_boid = self.__boid_out and b_master
	if b_feed_boid then
		poid.kill_all( self.__boid_bdd_ref )
	end

	if GA.b_spy then aaa.spy.push_range( "Update On Curve"..self:get_name(), 1 ) end
	--todoq this should be optimized

		local dt = aaa.time.dt
		local dt_speed = dt * (TANK.cur.creature_speed or 1.)

		local to_remove = {}

		for id = 1, #pool do
			local crea = pool[id]
			local len_curve = crea.len_curve
			if not len_curve then			crea:print_error( "update_on_curve() len_curve is nil for id " .. id )
			elseif len_curve <= 0. then		crea:print_error( "update_on_curve() len_curve is 0. for id " .. id )
			else
				local age = crea.age + dt
				crea.age = age
				--self:print( age )
				--	updare curve younger and older
				if age < self.on_curve_younger	then self.on_curve_younger = age	end
				if age > self.on_curve_older	then self.on_curve_older = age		end

				local speed = crea.s_speed
				local s = crea.s
				s = s + speed * dt
				--crea:print( dt.." me "..s )

				--todo deal with distance at start and end
				if s > crea.len_curve then
					--self:print( "will remove "..id )
					table.insert( to_remove, id )
				else
					if b_feed_boid then
						self:__feed_boid( id, crea )
					end
				end
				--todo stop(remove) or loop
				--self:print( "__update_on_curve "..id.." "..s )

				crea.s = s
				crea.s_cano = s * crea.len_curve_over
				--self:print( s.." -> "..crea.s_cano )
				crea.len_moved = crea.s + crea.__id_rnd * 3.1121
			end
		end

		if b_feed_boid then
			self:__updraw_boid_obj()	--todo opass a correct flag
		end

		for i = #to_remove, 1, -1 do
			--self:print( i.."will remove "..remove_table[i] )
			self:push_crea_free( table.remove( pool, to_remove[i] ) )
		end
		self.on_curve_nb = #pool

	if GA.b_spy then aaa.spy.pop_range() end
end

function GREA:add_curve_rt_point( crea, x,y,z )
	if not TANK.cur.b_curve_rt_update then return end

	local len
	local get_len = aaa.bdd.get_len
	local bdd_curve = self.__curve_rt_bdd
	local id_curve = crea.__curve_id
	if app.__b_is_fish_boid then

	--aaa.print_fn()
		if GA.b_spy then aaa.spy.push_range( "GREA:add_curve_rt_point", 2 ) end
			--	self:print( id_curve )

			if TANK.cur.b_machine_mono then
				if true then
					len = aaa.bdd.push_control_point_back_len_max( bdd_curve, id_curve, x,y,z, crea.__len_curve_needed )
					--self:print( "len "..len )
				else
					aaa.bdd.push_control_point_back( bdd_curve, id_curve, x,y,z )
					len = get_len( bdd_curve, id_curve )
					--self:print( "len ", len )

					local len_needed = crea.__len_curve_needed --todo use addition of dist for new point
					--self:print( len.." "..len_needed )
					if len > len_needed then
						repeat
							aaa.bdd.pop_control_point_front( bdd_curve, id_curve )
							len = get_len( bdd_curve, id_curve )
						until len <= len_needed
					end
				end
			else
				local get_len = aaa.bdd.get_len
				aaa.bdd.push_control_point_back( bdd_curve, id_curve, x,y,z )
				len = get_len( bdd_curve, id_curve )
				--self:print( "len ", len )
				local nb_pt = aaa.bdd.get_control_point_nb( bdd_curve, id_curve )
				local nb_max = self.pt_nb
				--	aaa.show( nb_max, "nb_max" )
				if nb_pt > nb_max then
					--self:print( "have "..nb_pt.." when max is "..nb_max )
					repeat
						aaa.bdd.pop_control_point_front( bdd_curve, id_curve )
						nb_pt = nb_pt - 1
					until nb_pt <= nb_max

					local len_new = get_len( bdd_curve, id_curve )
					local len_lost = len - len_new -- is positive
					crea.s = crea.s - len_lost
					crea.s_decal = math.fmod( crea.s_decal - len_lost, crea.seg_len )
					len = len_new
				end
			end

		if GA.b_spy then aaa.spy.pop_range() end

	else
		len = get_len( bdd_curve, id_curve )
	end
	crea.len_curve = len
	crea.len_curve_over = 1. / len
	--crea.s_draw = 0. --todo	invalidate position cache
end

function GREA:update( b_master, b_update )
	--todoaqua should be there
	if not self:is_active() then return end

	if GA.b_spy then aaa.spy.push_range( self:get_name().." update "..#(self.__pool), self.id ) end

--	local boid = self:get_boid()
	if b_update then
		self:send_fish_length_to_deformer()
		if self.behavior then
			self:do_behavior()
		end
		--was call only here but not useful eg Lv Milan : see below
		--self:__update_creas_len()	--make sure other code path do it and only when needed
	end

	--self:print( "GREA:update() "..b_master )
	local pool_on_curve = self.__on_curve
	if pool_on_curve then
		--self:print( "do pool_on_curve" )
		self:__update_creas_len()	--make sure other code path do it and only when needed
		self:__update_on_curve( pool_on_curve, b_master )
	end

	if b_master then
		if GA.b_spy then aaa.spy.push_range( self:get_name().." "..#(self.__pool), self.id ) end

			if self.__boid then
				if self.__boid_out then
					--this case is when the boid don't update but is feed by another process
					aaa.obj.update_then_draw( GGREA.boid_out_attr_layers )
					aaa.obj.draw( self.__boid_layer_ref )
				else
					--this case is when the boid is the position generator
					self:__updraw_boid_obj( b_update )
					--self:print( "GREA:update() __updraw_boid_obj "..b_update )
					if TANK.cur.b_curve_rt_update then
						if b_update then
							self:__update_curve_rt_from_boid( self.__boid_bdd_ref )
						end
						--self:print( "__updraw_curve_rt call" )
						self:__updraw_curve_rt( b_update )
					end
				end
			else	--hack monaco
				self:print( "On y passe" )
			end

			if b_update and self.__launcher then
				self:__update_launcher()
			end

		if GA.b_spy then aaa.spy.pop_range() end
	else
--		self:__update_curve_rt_len( self.__boid_bdd_ref ) --need it for the number
	end

	if b_update then
		local blow = self.blow
		if blow then
			self:update_blow( blow )
		end
	end

	if GA.b_spy then aaa.spy.pop_range() end
end

function GREA:draw()
	--todoaqua this line should be there
	if not self:is_active() then return end

	--self:print( "GREA:draw()" )
	--aaa.print_fn()
	if GA.b_spy then aaa.spy.push_range( self:get_name().." "..#(self.__pool), self.id ) end
		if self.__boid and not self.__boid_out then
			self:__move_on_path()
			self:__compute_points( self.__pool, false )
		end
		self:__compute_points( self.__on_curve, true )
	if GA.b_spy then aaa.spy.pop_range() end
end

function GREA.__do_click_up( bu, x, y )
	oo.getsuper(BUTTON).do_click_up( bu, x, y )
	local val = bu:get_value()
	bu:print( "do_click_up at "..x..", "..y.." with value "..val )
	if val == 1 then
		TANK.cur:get_ggrea().bu_grea_selector:set_value( bu.__grea_id )
	end
end


function GREA:edit_race()
	local race = self:get_race()
	self:print( "Click Double on "..race )
	local meu = race:get_meu_fbx()
	if meu then
		local meu_tank = TANK.cur.meu
		--todo do better and more generic using GP
		local ui_slot = meu_tank:get_ui_slot() or 4
		meu:set_ui_slot( ui_slot-1 )
	end
end

function GREA:focus_curve_rt()
	aaa.obj.set_focus_ui( self.__curve_rt_bdd )
end

	-- 	bu:set_method_on_value_change(	self, "change_grea" )
	-- 	bu:set_method_on_click(			self, "edit_boid", bu )
	-- 	bu:set_method_on_click_double(	self, "click_double_selector", bu )
function GREA:add_ui( i, bus )
	local name = self:get_name()
	local bu

	--	bus:move_prev()
	bu = bus:add_but_target_lua( name.."_active", nil,	self, "__b_active", true )
		--todo
		bu:set_method_on_click( TANK.cur, "choice_for_hook", self )
		bu:set_text_draw( false )
		bu.__grea_id = i
		bu.__grea = self
		bu.do_click_up = GREA.__do_click_up

	--bu:set_text_rect_ratio( 6 )
	self.bu_active = bu

	-- draw on the side the  color
	bu.draw = function(self)
		BUTTON.draw(self)
	--	self.__color_ref:get_rgba()
		gol.color( self.__grea.__color_ref:get_rgba() )
--		aaa.draw_rgba( self.__color )
		aaa.draw_rect( .7, -.5, 1.7, .5 )
	end

	local SX = 5
	local DX =  (SX-2)/2 + 2
	bus:move_xy( DX, -1 )
	local ggrea = TANK.cur:get_ggrea()
	bu = bus:add_but_trig_method( self:get_name(), nil,	ggrea, "set_grea_cur", self )
		local sx,sy = bu:get_sxy()
		bu:set_sxy( sx*SX, sy )
		bu:set_text_inside( true )
		bu:set_method_on_click(			ggrea, "edit_boid", self )
		bu:set_method_on_click_double(	self, "edit_race" )
		bu:set_method_on_click_triple(	self, "focus_curve_rt" )
	bus:move_xy( -DX, 0 )
end

function GREA:set_screen_proj( tab )	-- fx, fy,  ox_left, ox_right, oy
	self.screen_proj = tab
end
