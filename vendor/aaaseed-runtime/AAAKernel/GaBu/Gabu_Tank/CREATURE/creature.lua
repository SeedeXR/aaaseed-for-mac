--
--	CREATURE
--		a single "fish" owner is the
--
CLASS.DECLARE( "CREATURE" )

function CREATURE:create_pool( grea, nb, id_offset )
	local tab = {}
	for i = 1, nb do
		tab[i] = CREATURE:create( grea, i, i + id_offset )
	end
	return tab
end

function CREATURE:create( grea, id_in_pool, id_unique )
	local self = CREATURE:create_instance( grea:get_name().."_"..id_in_pool )

	-- 6 points to have 3 catmull there piece of curve 1234 2345  3456
	self.pos = aaa.array.new_fp32(18)
	self.pos:clear()
--	self.pos				=	{	0,0,0,	0,0,0,	0,0,0,	0,0,0,	0,0,0,	0,0,0,	}
--unused for now
--	self.s_draw				=	-42.	--	invalidate cache

	self.s					=	0.		--	in [0,curve_len]
	self.s_speed			=	0.

	self.s_cano				=	0.		--	in [0,1], 	s_cano = s * curve len
	self.s_inter			=	0.

	self.x					=	0.
	self.z					=	0.

	self.len_moved			=	0.		-- use to compute phase to animate part like tail in sync with deplacement
	self.pt_last			=	V3.new_from( 0, 0, 0 )

	--defined when send 	on curve used to introduce variations
--	self.__id_rnd			=	math.random( 0, 256*256*256 )
	self.id_in_pool			=	id_in_pool	-- [1,nb_in_pool] use for rnd param or id in pool
-- mona try something else now we have curve_rt by grea
--	self.__curve_id			=	id_unique	-- [i,i+nb_in_pool-1]
	self.__curve_id			=	id_in_pool
	self.id_unique			=	id_unique

	self.__b_clip			=	false

	self.s_decal			=	0.

	self.__grea				=	grea
	self.__curve_rt_bdd		=	grea.__curve_rt_bdd
	local race				=	grea:get_race()
	self.race				=	race

	local size				=	race.size
	local size_factor
	if size then
		local inter = math.sin( id_unique * 4.2656) * .5 + .5
		if size.gamma then
			--self:print( "size.gamma "..size.gamma )
			inter =  math.pow( inter, size.gamma )
		end
		size_factor =	size.min + size.range * inter
	else
		size_factor =	1
	end
	self.size_factor		=	size_factor

	if race.b_fbx then
		local len_to_anim		= 	race.len_to_anim
		self.len_to_anim_factor	=	len_to_anim.min + len_to_anim.range * ( math.sin(self.id_in_pool * 1.333) * .5 + .5 )
		self.anim_duration		=	len_to_anim.duration
		self.anim_duration_over	=	1. / len_to_anim.duration
	end

	--self:print( "CREATURE:create() race.seg_len "..race.seg_len.." * "..size_factor )
	self.catmull_seg_nb		=	race.catmull_seg_nb
	self.seg_len			=	race.seg_len * size_factor
	self.seg_len_over		=	1. / self.seg_len
	self.__len_curve_needed =	self.seg_len * (self.catmull_seg_nb + 3)	--hack was + 1

	self.len_curve			=	1
	self.len_curve_over		=	1. / self.len_curve

	self.tail				=	table.copy_simple( race.tail )

	-- we generate here the table to avoid creation and destruction of table all the time
	self.morph_value		=	{}

	local meu_fish = race.meu_fish
	if meu_fish and meu_fish.init_creature then
		 meu_fish:init_creature( self )
	end

	return self
end

function CREATURE:get_grea()	return self.__grea	end
function CREATURE:get_race()	return self.race	end

function CREATURE:set_color_gol( alpha )
	local c = self.__grea.__color
	if alpha then
		gol.color( c[1], c[2], c[3], c[4]*alpha )
	else
		gol.color( c )
	end
end

function CREATURE:check()
	if not self.len_curve or self.len_curve == 0 then
		table.print( self, "len_curve problem", 1 )
		self:box_error( "len_curve problem" )
		return false
	end
	return true
end

--[[
function CREATURE.init()
	CREATURE.__pool = CREATURE:create_pool( 4096 )
end
function CREATURE.add( str_id )
end
--]]

function CREATURE:draw_symbo_mono( size )
	local pos = self.pos

	self:set_color_gol()
	aaa.draw_null(	pos[4],		pos[5],		pos[6], size	)
	aaa.draw_null(	pos[7],		pos[8],		pos[9], size * .8	)
	aaa.draw_null(	pos[10],	pos[11],	pos[12], size * .6	)

	self:set_color_gol( .7 )
	gol.begin_line_strip()
	gol.vertex3(
--					pos[1],		pos[2],		pos[3],
	 				pos[4],		pos[5],		pos[6],
	 				pos[7],		pos[8],		pos[9]
				)
	gol.do_end()

--	aaa.draw_null( self.x, self.y, self.z, .8 )
end

function CREATURE:draw_symbo()
	local inter = self.s_inter * self.seg_len_over
	local inter_m1 = 1-inter
	local pos = self.pos
	self:set_color_gol()
	aaa.draw_null(	pos[1]*inter	+	pos[4]*inter_m1,
					pos[2]*inter	+	pos[5]*inter_m1,
					pos[3]*inter	+	pos[6]*inter_m1, .4	)
	self:set_color_gol( .7 )
	aaa.draw_null(	pos[7],
					pos[8],
					pos[9], .2	)

--	aaa.draw_null( self.x, self.y, self.z, .8 )
end

function CREATURE:draw_deformed_line()
	local pos = self.pos
	gol.begin_line_strip()
		gol.vertex3(	pos[1],		pos[2],		pos[3],
						pos[4],		pos[5],		pos[6],
						pos[7],		pos[8],		pos[9],
						pos[10],	pos[11],	pos[12],
						pos[13],	pos[14],	pos[15],
						pos[16],	pos[17],	pos[18]
			)
	gol.do_end()
end
function CREATURE:draw_deformed_catmull_rom()
	aaa.draw_catmull_rom( self.pos, self.catmull_seg_nb )
end
function aaa.draw_catmull_rom_maa( pos, ip, min, max )
	local do_catmull_rom = aaa.math.do_catmull_rom_xyz
	local x,y,z
	local nb = 24
	gol.begin_line_strip()
		local i = (ip-1)*3+1
		x,y,z	= do_catmull_rom( min,		pos[i],		pos[i+1],	pos[i+2],
											pos[i+3],	pos[i+4],	pos[i+5],
											pos[i+6],	pos[i+7],	pos[i+8],
											pos[i+9],	pos[i+10],	pos[i+11]
											)
		gol.vertex3( x, y, z )
		for j=1,nb do
			local s = interpolate( min, max, j/nb )
			x,y,z	= do_catmull_rom( s	)
			gol.vertex3( x, y, z )
		end
	gol.do_end()
end

function CREATURE:draw_deformed_catmull_rom()
	local min, max = .1, .9
	aaa.draw_catmull_rom_maa( self.pos, 1, min-1, max-1 )
	aaa.draw_catmull_rom_maa( self.pos, 1, min, max )
	aaa.draw_catmull_rom_maa( self.pos, 2, min, max )
	aaa.draw_catmull_rom_maa( self.pos, 3, min, max )
	aaa.draw_catmull_rom_maa( self.pos, 3, min+1, max+1 )
end

function CREATURE:draw_deformed_point()
	local pos = self.pos
	gol.begin_points()
		gol.vertex3(	pos[1],		pos[2],		pos[3],
						pos[4],		pos[5],		pos[6]	)
		for ip =2,self.catmull_seg_nb+2 do
			local i = ip*3+1
			gol.vertex3( pos[i],		pos[i+1],	pos[i+2] )
		end
	gol.do_end()
end

function CREATURE:draw_exp()
	local id_curve = self.__curve_id
--	self:print( id_curve )

	local len_over = self.len_curve_over
	if not len_over then return end

	aaa.bdd.set_lua_cur( self.__curve_rt_bdd )
	local fn_get = aaa.bdd.get_point_s_curve_fast
	local m = math.max( .0001, len_over / 5 )	--todo why ?
	gol.begin_points()
		local nb = 4
		--m = m/(nb-1)
		local  s =  1
		for i =1,nb do
			local x,y,z = fn_get( id_curve, s )
			gol.vertex3( x, y, z )
			s =  s - m
		end
	gol.do_end()
--	crea.len_curve = len
--	crea.len_curve_over = 1. / len
--	crea.s_draw = 0.	--todo	invalidate position cache
end

function CREATURE:update_len_moved( x,y,z, d2_max )
	local pt_last = self.pt_last

	local d = pt_last[1] - x
	local d2 = d * d
	if d2 < d2_max then
		d = pt_last[2] - y
		d2 = d2 + d * d
		if d2 < d2_max then
			d = pt_last[3] - z
			d2 = d2 + d * d
			if d2 < d2_max then
				--if d2 == 0. then
				--	self:print( "update_len_moved with 0 distance, check we don't do it twice" )
				--end
				return false
			end
		end
	end
	-- we add a point
	pt_last[1] = x
	pt_last[2] = y
	pt_last[3] = z
	self.len_moved = self.len_moved + math.sqrt(d2)
	return true
end


function CREATURE:draw_transparent( info )

	local race = self.race
	--local tank = TANK.cur
	--local morph_value = self.morph_value

	local tr = race.transparent_render

	--local inter = self.s_inter * self.seg_len_over
	--local inter_m1 = 1-inter
	local pos = self.pos
	--self:set_color_gol()
--	aaa.draw_null(	pos[4],pos[5],pos[6], .4	)
	-- self:set_color_gol( .7 )
	-- aaa.draw_null(	pos[7],
	-- 				pos[8],
	-- 				pos[9], .2	)

	local y = pos[5]
	if info.time < 1. then
		self.__transparent_use_it = true
	elseif y < 0 then
		--self:print(self.id_in_pool)
		self.__transparent_use_it = self.id_in_pool < info.id_max
	end
	if not self.__transparent_use_it then
		return
	end

	local p1 = V3.new_from( pos[4], 	pos[5], 	pos[6] )
	local p2 = V3.new_from( pos[7], 	pos[8], 	pos[9] )
	local vv = V3.get_sub( p2, p1 )
	V3.normalize( vv )
	V3.scale(  vv, .5 )
	local vu
	if info.axe=="x" then	vu = V3.get_rotate_x_90(vv)
	else					vu = V3.get_rotate_z_90(vv)
	end
	local sf = self.size_factor
	V3.scale(  vu, tr.sx*sf )
	V3.scale(  vv, tr.sy*sf )


	local pa =  V3.get_sub( V3.get_add( p1, vv ), vu )
	local pb =  V3.get_sub( V3.get_sub( p1, vv ), vu )
	local pc =  V3.get_add( V3.get_add( p1, vv ), vu )
	local pd =  V3.get_add( V3.get_sub( p1, vv ), vu )

	--self:print(self.id_in_pool)
	local a = wrap( self.id_in_pool*.112153164, .6, .8 )
	a = a * clamp_01( y * 2 )
	gol.color_white( a )
	-- gol.begin_line_strip()
	-- 	gol.vertex3(	pa[1],		pa[2],		pa[3],
	-- 					pb[1],		pb[2],		pb[3],
	-- 					pc[1],		pc[2],		pc[3],
	-- 					pd[1],		pd[2],		pd[3]
	-- 		)
	-- gol.do_end()

	if info.bind_beg then
		local bind = wrap( info.time * (100+self.id_in_pool * .02) * .5 + self.id_in_pool*343, info.bind_beg, info.bind_end )
		gol.bind_texture( bind )
	end
	aaa.draw_triangle_strip_4xyz_uv(	pa[1],		pa[2],		pa[3],
							pb[1],		pb[2],		pb[3],
							pc[1],		pc[2],		pc[3],
							pd[1],		pd[2],		pd[3]
			)
end

function CREATURE:draw_deformed()
	--self:print( "zobi" )
	
	local race = self.race
	if not race.__b_draw_can then return end
	

	if race.b_v2021 then
		-- prepare tail
		local tail = self.tail
		if tail.amp then
			--self:print( self.len_moved .." "..tail.freq )
			tail.phase = math.fmod( self.len_moved * tail.freq * .25+ self.id_in_pool, math.pi2 )
			--tail.phase = self.len_moved * tail.freq * .25 + self.id_in_pool
			--self:print( tail.phase.." "..tail.amp )
		else
			tail.phase = 0
		end

		if GA.b_spy then aaa.spy.push_range( self:get_name(), self.__grea.id ) end --..self.id
			race.__celts[1]:draw_deformed( self )
		if GA.b_spy then aaa.spy.pop_range() end
		return
	end

	local tank = TANK.cur
	local morph_value = self.morph_value
	if race.b_fbx then
		if race.transparent_render then
			--self:draw_transparent( self )
		else
			if GA.b_spy then aaa.spy.push_range( self:get_name(), self.__grea.id ) end --..self.id
				-- this case is immersion Monaco for now

				-- first deal with morph
				if race.morph then
					for i=1,race.morph_index_max do
						local morph = race.morph[i]
						if morph then --todoaqua requin are for SoOuest
							local type = morph.type
							if type then
								if		type == "fix"		then	morph_value[i] = morph.value
								elseif	type == "mouth"		then	morph_value[i] = tank.mouth
								end
							else
								-- tuna for now
								--self:print( "morph "..i )
								local mod_freq = morph.mod_freq
								local v
								if mod_freq then
									v = math.sin( self.len_moved * mod_freq ) * morph_a.mod_amp
								else
									v = self.len_moved
								end
								v = math.sin( v * morph_a.freq )
								local f = morph_a.amp * .5
								--self:print( "v "..v.." f "..v )
								--self:print( "-> "..v * f + f )
								morph_value[i] = v * f + f
							end
						end -- if morph
					end	-- for
				end	-- if race.morph

				-- no tail for now
				-- set_shader and fbx but here only fbx
				-- mo touch for now
				race.__celts[1]:draw_deformed( self )

			if GA.b_spy then aaa.spy.pop_range() end
		end

	else --if race.b_fbx
		--this is SoOuest and Toulon treated as a separate case so no compatibility problems
		-- first deal with morph
		if GA.b_spy then aaa.spy.push_range( self:get_name(), self.__grea.id ) end --..self.id

			if race.morph then
				for i=1,race.morph_index_max do
					local morph = race.morph[i]
					if morph then --todoaqua requin are for SoOuest
						local type = morph.type
						if type then
							if		type == "fix"		then	morph_value[i] = morph.value
							elseif	type == "mouth"		then	morph_value[i] = tank.mouth
							elseif	type == "requin" 	then
								local s_cano = self.s * self.len_curve_over
								local x,y,z = aaa.bdd.get_point_s_curve_fast( self.__curve_bdd, self.__curve_id, s_cano )
								--	self:print( z )
								z = clamp_01( (z-2)*.5 - math.sin( self.len_moved * .3 ) * .05 )
								morph_value[i] = z*.25
							elseif  type == "kid" then --todoaqua kid are for SoOuest
								-- fish blowned by kids
								if self.blow then
									--self:print( self.id )
									math.randomseed( self.id_in_pool * 12345 )
									local r = {}
									local t = 0
									for i=1,4 do
										local rnd = math.random()
										rnd = rnd * rnd * rnd
										r[i] = rnd
										t = t + rnd
									end
									if t~=.0 then t = 1/t end
									--for i=1,4 do
									--	r[i] = r[i] * t
									--end
									for i=1,4 do
										morph_value[i] = r[i] * self.blow * t --* 1.5/ self.id  )
									end
								end
							end
						else
							-- tuna for now
							--self:print( "morph "..i )
							local mod_freq = morph.mod_freq
							local v
							if mod_freq then
								v = math.sin( self.len_moved * mod_freq ) * morph.mod_amp
							else
								v = self.len_moved
							end
							v = math.sin( v * morph.freq )
							local f = morph.amp * .5
							--self:print( "v "..v.." f "..v )
							--self:print( "-> "..v * f + f )
							morph_value[i] = v * f + f
						end
					end -- if morph
				end	-- for
			end	-- if race.morph

			-- prepare tail
			local tail = self.tail
			if tail.amp then
				--self:print( self.len_moved .." "..tail.freq )
				tail.phase = math.fmod( self.len_moved * tail.freq + self.id_in_pool, math.pi2 )
				--self:print( tail.phase.." "..tail.amp )
			else
				tail.phase = 0
			end

			--set_shader
			local shading = race.__celts[1].__shading
			if shading then
				local t = self.__touch
				if t then
					local type = t.how.type
					if		type=="move_y"	then	shading:set_vert_vec4_y( 7, t.val )
					elseif  type=="map"		then	shading:set_frag_float( 5, t.val )
					elseif	type=="morph"	then	morph_value[t.how.morph_id] = t.val
					end
				else
					shading:set_vert_vec4_y( 7, 0 )
					shading:set_frag_float( 5, 0 )
				end

				--table.print( tank, 2 )
				shading:set_vert_vec4_w( 1, morph_value[1] or tank.morph_value_ui[1] )
				shading:set_vert_vec4_w( 2, morph_value[2] or tank.morph_value_ui[2] )
				shading:set_vert_vec4_w( 3, morph_value[3] or tank.morph_value_ui[3] )
				shading:set_vert_vec4_w( 4, morph_value[4] or tank.morph_value_ui[4] )
			else
				self:print( "CREATURE:draw_deformed() no shading" )
			end

			local celts = race.__celts
			--	self:print( #celts )
			--	for i = #celts, 1, -1 do
			for i = 1, #celts do
				--self:print( i )
				celts[i]:draw_deformed( self )
			end

		if GA.b_spy then aaa.spy.pop_range() end
	end	--if race.b_fbx

end

function CREATURE:remove_touch()
	self.__touch = nil -- remove
	self.__touch_map = nil
end
function CREATURE:pick_touch_map()
	self.__touch_map = self.race.__celts[1]:pick_touch_map()
end