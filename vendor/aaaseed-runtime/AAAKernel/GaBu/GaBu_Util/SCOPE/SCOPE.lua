--
--	TBUF
--
if CLASS.DECLARE( "TBUF", QUEUE ) then
	TBUF:set_class_status_doc(	CLASS.STATUS.CORE,
								"Time buffer, memorize elt for a certain duration" )
end

function TBUF.init_instance(self)
	oo.getsuper(TBUF).init_instance( self )
	self:set_duration( 5. )
end
function TBUF:create( name )
	return TBUF:create_instance( name )
end
function TBUF:set_duration( duration )
	self.duration = duration
end
-- Optimized trim function for circular buffer
function TBUF:trim( t )
	local nb = self:get_nb()
	if nb == 0 then return end
	
	-- Check elements from the front until we find one that should be kept
	while nb > 0 do
		local elt = self:get_front(1)
		if elt and elt.t < t then
			self:pop()  -- O(1) operation
			nb = nb - 1
		else
			break
		end
	end
end
function TBUF:update()
	--self.duration = 120.
	--self:print( "toto" )
	local last = self:get_last()
	self:trim( aaa.time.t - self.duration )
end
function TBUF:add( elt )
	self:trim( elt.t - self.duration )
	self:push( elt )
end

--
--	SCOPE
--
if CLASS.DECLARE( "SCOPE", TBUF ) then
	SCOPE:set_class_status_doc(	CLASS.STATUS.CORE,
								"Oscilloscope, a TBUF which can be drawn in 2d" )
end

function SCOPE:create( name )
	return SCOPE:create_instance( name )
end

function SCOPE:add( val )
	--self:print( aaa.time.t.." "..val )
	local elt = { t=aaa.time.t }
	local last = self:get_last()
	if last then
		local dt = elt.t - last.t
		--todoq was .5 shopuld be set externally
		local inter = 1.
		if self.b_verbose_add then
			self:print( val.."\t"..last.v )
		end
		local v = last.v
		if math.is_nan(v) then
			last.v = 0
			v = 0
		end
		elt.v = inter * val + v * (1. - inter)
		if dt ~= 0 then
			local o_dt = 1 / dt
			elt.dv = ( elt.v - v ) * o_dt
			--self:print( "dv is "..elt.dv )
			local last_dv = last.dv
			if last_dv then
				elt.ddv = ( elt.dv - last_dv ) * o_dt
			else
				elt.ddv = 0
			end
		else
			elt.dv = 0
			elt.ddv = 0
		end
	else
		elt.v = val
		elt.dv = 0
		elt.ddv = 0
	end
	
	TBUF.add( self, elt )
end

function SCOPE:draw( su,sv )
	su = su or 1
	sv = sv or 1
	--self:print( "toto "..self.first.." "..self.last )
	local nb = self:get_nb()
	if nb <= 0 then return end
	local o_u = 0
	local o_v = 0

	local t_beg = self:get_first().t
	--self:print( t_beg )
	local t_fac = su / self.duration

	gol.begin_line_strip()
		for i = 1,nb do
			local elt = self:get_front(i)
			elt.u = (elt.t - t_beg) * t_fac + o_u
			--self:print( i.." "..elt.u.." "..elt.v )
			gol.vertex2( elt.u, (elt.v + o_v) * sv )
			--self:print( "titi "..elt.u.." "..(elt.v + o_v) * f )
		end
	gol.do_end()
end

function SCOPE:draw_speed( f )
	--self:print( "toto "..self.first.." "..self.last )
	local nb = self:get_nb()
	if nb <= 0 then return end
	local o_u = 0
	local o_v = 0

	f = f or 1
	gol.begin_line_strip()
		for i = 1,nb do
			local elt = self:get_front(i)
			gol.vertex2( elt.u, (elt.dv + o_v) * f )
		end
	gol.do_end()
end

function SCOPE:draw_acc( f )
	--self:print( "toto "..self.first.." "..self.last )
	local nb = self:get_nb()
	if nb <= 0 then return end
	local o_u = 0
	local o_v = 0

	f = f or 1
	gol.begin_line_strip()
		--for _,elt in ipairs( self.data ) do
		for i = 1,nb do
			local elt = self:get_front(i)
			gol.vertex2( elt.u, (elt.ddv + o_v) * f )
		end
	gol.do_end()
end

--
--	SCOPE 3D
--
if CLASS.DECLARE( "SCOPE_3D", TBUF ) then
	SCOPE_3D:set_class_status_doc(	CLASS.STATUS.CORE,
									"Oscilloscope 3d, a TBUF which can be drawn in 3d" )
end

function SCOPE_3D:create( name )
	return SCOPE_3D:create_instance( name )
end

function SCOPE_3D:set_distance_min( dist_min )
	--self.dist_min = dist_min
	self.dist_min_squared = dist_min * dist_min
end
function SCOPE_3D:set_map_u( u_min, u_max )
	self.u_min = u_min
	self.u_max = u_max
end

function SCOPE_3D:add( pt )
	local elt = { t=aaa.time.t, v={ pt[1], pt[2], pt[3] } }

	--self:print( "before_last" )
	local last = self:get_last()
	-- this is the first point
	if not last then

		elt.dv	= { 0, 0, 0 }
		elt.ddv = { 0, 0, 0 }
		TBUF.add( self, elt )
		return
	end

	local dt = elt.t - last.t
	if dt == 0 then
		return
	end

	--self:print( "dist_squared "..self.dist_min_squared )
	--self:print( "after_last" )

	if self.dist_min_squared then
		--self:print( V3.dist_squared( elt.v, last.v ) )
		if V3.dist_squared( elt.v, last.v ) < self.dist_min_squared then
			if self.dt then
				self.dt = self.dt + dt
			else
				self.dt = dt
			end
			return
		end
	end

	if self.dt then
		dt = dt + self.dt
		self.dt = nil
	end
	--	todo set this thru a fn
	local inter = .5
	if self.b_verbose_add then
		self:print( "X: "..pt[1].."\t"..last.v[1] )
		self:print( "Y: "..pt[2].."\t"..last.v[2] )
		self:print( "Z: "..pt[3].."\t"..last.v[3] )
	end
	V3.interpolate( elt.v, last.v, elt.v, inter )

	local over_dt = 1 / dt
	--	tangent
	elt.dv_norm = {}
	V3.sub( elt.dv_norm, elt.v, last.v )
	--	speed
	elt.dv = V3.clone( elt.dv_norm )
	V3.scale( elt.dv, over_dt )
	--	normalize tangent
	V3.normalize( elt.dv_norm )
	if last.dv_norm then
		local v = {}
		V3.add_scale( v, elt.dv_norm, last.dv_norm, 0.25 )
		elt.normal = { -v[2], v[1], 0 }
	else
	--	if previous point is the first one make him a normal
		last.dv_norm = V3.clone( elt.dv_norm )
		elt.normal = { -elt.dv_norm[2]*.5, elt.dv_norm[1]*.5, 0 }
		last.normal = { -elt.dv_norm[2]*.5, elt.dv_norm[1]*.5, 0 }
	end

	local last_dv = last.dv
	if last_dv then
		elt.ddv = {}
		V3.sub_scale( elt.ddv, elt.dv, last.dv, over_dt )
		if false then
			if V3.clamp( elt.ddv, 100 ) then
				V3.scale_add( elt.dv, elt.ddv, dt, last.dv )
				V3.scale_add( elt.v, elt.dv, dt, last.v )
			end
		end
	else
		elt.ddv = { 0, 0, 0 }
	end
	TBUF.add( self, elt )

end

function SCOPE_3D:draw( o_x, o_y, o_z )
	--self:print( "toto "..self.first.." "..self.last )

	local nb = self:get_nb()
	if nb <= 0 then return end
	local t_beg = self:get_first().t
	local SV = 5.
	local t_fac = SV / self.duration
	local fv = 1

	gol.set_texture_dim( 0 )
	gol.begin_line_strip()
		--for _,elt in ipairs( self.data ) do
		for i =1,nb do
			local elt = self:get_front(i)
			local pt = elt.v
			--self:print( "pt "..elt.t.." "..pt[2] )
			gol.vertex2( pt[1] + o_x, pt[2] + o_y, pt[3] + o_z )
		end
	gol.do_end()
end
function SCOPE_3D:draw_one( x,y,z, a, width, u )
	--local t = {}
	--t = V3.clone( a.normal )
	--V3.scale( t, .5 * width )

	local dx = a.normal[1] * width
	local dy = a.normal[2] * width
	local pt = a.v
	x = pt[1] + x
	y = pt[2] + y
	z = pt[3] + z
	gol.texcoor2( u, 1 )
	gol.vertex3( x + dx, y + dy, z )

	gol.texcoor2( u, 0 )
	gol.vertex3( x - dx, y - dy, z )
end
function SCOPE_3D:draw_xy_tex( o_x,o_y,o_z, bind, width )
	--self:print( "toto "..self.first.." "..self.last )
	local nb = self:get_nb()
	if nb <= 2 then return end

--	gol.push_attrib()		-- push all attrib : GL_ALL_ATTRIB_BITS
	--gol.set_default()
	--gol.set_blend_add()
	--gol.set_back_fill()
--	gol.set_cull_none()
--	gol.set_cull_front()
--	gol.set_cull_back()
	--gol.set_texture_dim( 0 )
--	gol.bind_texture( bind )
--	gol.set_blend_max()

	--	should be dealt by local
	--aaa.obj.update_then_draw( "attrib_light_paint" )
	if false then
--		gol.set_texture_dim( 2 )
		gol.begin_quad_strip()
	else
		gol.set_texture_dim( 0 )
		gol.set_front_line()
		gol.begin_quad_strip()
--		gol.begin_lines()
	end

	width = width * .5
	local u_min = self.u_min or 0.
	local u_max = self.u_max or 1.
	local fu = (u_max - u_min) / nb
	--for _,elt in ipairs( self.data ) do
	for i = 1,nb do
		local elt = self:get_front(i)
		self:draw_one( o_x, o_y, o_z, elt, width, u_min + (i-1)*fu )
	end

	gol.do_end()
--	gol.pop_attrib()
end
