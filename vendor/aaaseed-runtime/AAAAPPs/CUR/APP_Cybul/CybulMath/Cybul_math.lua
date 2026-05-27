function CYBUL:get_pixel_pos_point( pt, width, height )
	if pt and width and height then
		return  {width * pt[1], height * pt[2]}
	else
		return pt
	end
end

function CYBUL:get_intersection( A, B, C, D )
	if A[1] == B[1] and C[1] == D[1] then
		-- both verticals
		if C[1] == A[1] then
			-- same vertical, try to find a point ( note: there might be multiple points )
			if A[2] <= C[2] and C[2] <= B[2] then return {C[1], C[2]} end
			if A[2] <= D[2] and D[2] <= B[2] then return {D[1], D[2]} end
			if B[2] <= C[2] and C[2] <= A[2] then return {C[1], C[2]} end
			if B[2] <= D[2] and D[2] <= A[2] then return {D[1], D[2]} end

			if C[2] <= A[2] and A[2] <= D[2] then return {A[1], A[2]} end
			if C[2] <= B[2] and B[2] <= D[2] then return {B[1], B[2]} end
			if D[2] <= A[2] and A[2] <= C[2] then return {A[1], A[2]} end
			if D[2] <= B[2] and B[2] <= C[2] then return {B[1], B[2]} end
		end

		return {-1, -1} -- no intersection
	end

	if A[1] == B[1] then
		-- only AB vertical
		local m2 = ( C[2] - D[2] ) / ( C[1] - D[1] )
		local b2 = C[2] - m2 * C[1]

		local x = A[1]
		local y = m2 * x + b2
		return {x, y}
	end

	if C[1] == D[1] then
		-- only CD vertical
		local m1 = ( A[2] - B[2] ) / ( A[1] - B[1] )
	    local b1 = A[2] - m1 * A[1]

		local x = C[1]
		local y = m1 * x + b1
		return {x, y}
	end

	-- base case
	local m1 = ( A[2] - B[2] ) / ( A[1] - B[1] )
    local b1 = A[2] - m1 * A[1]

    local m2 = ( C[2] - D[2] ) / ( C[1] - D[1] )
    local b2 = C[2] - m2 * C[1]

    local x = - ( b2 - b1 ) / ( m2 - m1 )
    local y = m1 * x + b1

	local y2 = m2 * x + b2
    return {x, y}
end


function CYBUL:extend_scene()
	-- Note: this only works if the picture was taken from the opposite side and not from the side
	-- ( at least not too much, the horizon line must exit the picture by the vertical sides x=0,x=1 and not the horizontal ones y=0,y=1 )
	-- Showing the angle from the side should be done in the camera tab anyway, and not from the picture if we want a better perspective / data
	-- So the picture must be taken with an angle <45deg roughly
	local pts = app.tab3d.points

	local Fu = app.tab3d.vanishing_point_x
	local Fv = app.tab3d.vanishing_point_y
	local Fw = app.tab3d.vanishing_point_z
	local Fu_b = app.tab3d.vanishing_point_x_bot

	local left_horizon, right_horizon, left_top, right_top
	local bottom_left, bottom_right

	local wall_lh, wall_rh, floor_lh, floor_rh

	local min_x = math.min(pts[1][1], pts[2][1], pts[3][1], pts[4][1], pts[5][1], pts[6][1])
	local max_x = math.max(pts[1][1], pts[2][1], pts[3][1], pts[4][1], pts[5][1], pts[6][1])
	local min_y = math.min(pts[1][2], pts[2][2], pts[3][2], pts[4][2], pts[5][2], pts[6][2])
	local max_y = math.max(pts[1][2], pts[2][2], pts[3][2], pts[4][2], pts[5][2], pts[6][2])
	local mid_x = (min_x + max_x) * .5
	local mid_y = (min_y + max_y) * .5


	if Fv[2] > mid_y then
		wall_lh = self:get_intersection( pts[3], pts[4], {0, 1}, Fv )
		wall_rh = self:get_intersection( pts[3], pts[4], {1, 1}, Fv )
	else
		wall_lh = self:get_intersection( pts[3], pts[4], {0, 1}, {0, 0})
		wall_rh = self:get_intersection( pts[3], pts[4], {1, 1}, {1, 0})
	end
	wall_lh = self:get_intersection( pts[3], pts[4], {0, 1}, Fv )
	wall_rh = self:get_intersection( pts[3], pts[4], {1, 1}, Fv )


	if Fw[1] > mid_x then
		floor_lh = self:get_intersection( pts[3], pts[4], {0, 0}, Fw )
		floor_rh = self:get_intersection( pts[3], pts[4], {1, 0}, Fw )
	else
		floor_lh = self:get_intersection( pts[3], pts[4], {0, 1}, {0, 0})
		floor_rh = self:get_intersection( pts[3], pts[4], {1, 1}, {1, 0})
	end
	floor_lh = self:get_intersection( pts[3], pts[4], {0, 0}, Fw )
	floor_rh = self:get_intersection( pts[3], pts[4], {1, 0}, Fw )

	if floor_lh[1] < wall_lh[1] then
		left_horizon = floor_lh
	else
		left_horizon = wall_lh
	end
	if floor_rh[1] > wall_rh[1] then
		right_horizon = floor_rh
	else
		right_horizon = wall_rh
	end

	local function angle_sign(pt, a, b)
		local vec = V2.get_sub(b, a)
		local dot = V2.dot(vec, V2.get_sub(pt, a))
		local cro = vec[1] * pt[2] - vec[2] * pt[1]
		return math.atan2(cro, dot)
	end


	-- Floor
	bottom_left  = {0, 0}
	bottom_right = {1, 0}
	if angle_sign(Fw, pts[3], pts[4]) > 0 then
		bottom_left = self:get_intersection( Fw, left_horizon, bottom_left, Fu_b )
		bottom_right = self:get_intersection( Fw, right_horizon, bottom_left, Fu_b )
	else
		bottom_right = self:get_intersection( Fw, right_horizon, bottom_right, Fu_b )
		bottom_left = self:get_intersection( Fw, left_horizon, bottom_right, Fu_b )
	end

	if bottom_left[2] > 1 or bottom_right[2] > 1 then
		bottom_left  = {0, 0}
		bottom_right = {1, 0}	
		if Fw[1] > mid_x then
			V2.scale_add(left_horizon, V2.get_sub(left_horizon, right_horizon), 1.2, right_horizon)
		else
			V2.scale_add(right_horizon, V2.get_sub(right_horizon, left_horizon), 1.2, left_horizon)
		end
		if angle_sign(Fw, pts[3], pts[4]) > 0 then
			bottom_left = self:get_intersection( Fw, left_horizon, bottom_left, Fu_b )
			bottom_right = self:get_intersection( Fw, right_horizon, bottom_left, Fu_b )
		else
			bottom_right = self:get_intersection( Fw, right_horizon, bottom_right, Fu_b )
			bottom_left = self:get_intersection( Fw, left_horizon, bottom_right, Fu_b )
		end
	end

	-- Wall
	left_top  = {0, 1}
	right_top = {1, 1}
	if Fv[2] > max_y then
		-- match the top line with Fu perspective
		if Fu[1] < min_x and Fu[2] > max_y or Fu[1] > max_x and Fu[2] < max_y then
			left_top = self:get_intersection( right_top, Fu, left_horizon, Fv )
		end
		if Fu[1] > max_x and Fu[2] > max_y or Fu[1] < min_x and Fu[2] < max_y then
			right_top = self:get_intersection( left_top, Fu, right_horizon, Fv )
		end
	else
		if Fv[1] < min_y then
			left_top = self:get_intersection( Fv, left_horizon, left_top, Fu )
			right_top = self:get_intersection( Fv, right_horizon, left_top, Fu )
		elseif Fv[1] > max_x then
			right_top = self:get_intersection( Fv, right_horizon, right_top, Fu )
			left_top = self:get_intersection( Fv, left_horizon, right_top, Fu )
		else
			if pts[1][2] < pts[2][2] then
				left_top = self:get_intersection( Fv, left_horizon, left_top, Fu )
				right_top = self:get_intersection( Fv, right_horizon, left_top, Fu )
			else
				right_top = self:get_intersection( Fv, right_horizon, right_top, Fu )
				left_top = self:get_intersection( Fv, left_horizon, right_top, Fu )
			end
		end
	end


	-- if Fw[1] > mid_x then
	-- 	print("Fw[1] > mid_x", Fw[1], mid_x, min_x, max_x)
	-- 	bottom_left  = {0, 0}
	-- 	bottom_right = {1, 0}

	-- 	print("      ", Fw[2], mid_y)
	-- 	if angle_sign(Fw, pts[3], pts[4]) > 0 then
	-- 		bottom_left = self:get_intersection( Fw, left_horizon, bottom_left, Fu_b )
	-- 		bottom_right = self:get_intersection( Fw, right_horizon, bottom_left, Fu_b )
	-- 	else
	-- 		bottom_right = self:get_intersection( Fw, right_horizon, bottom_right, Fu_b )
	-- 		bottom_left = self:get_intersection( Fw, left_horizon, bottom_right, Fu_b )
	-- 	end

	-- else
	-- 	-- Floor
	-- 	bottom_left  = {0, 0}
	-- 	bottom_right = {1, 0}

	-- 	if Fu_b[1] < min_x and Fu_b[2] > max_y or Fu_b[1] > max_x and Fu_b[2] < max_y then
	-- 		bottom_left = self:get_intersection( bottom_right, Fu_b, left_horizon, Fw )
	-- 	end
	-- 	if Fu_b[1] > max_x and Fu_b[2] > max_y or Fu_b[1] < min_x and Fu_b[2] < max_y then
	-- 		bottom_right = self:get_intersection( bottom_left, Fu_b, right_horizon, Fw )
	-- 	end
	-- end

	app.tab3d.ext_wall_lt = left_top
	app.tab3d.ext_wall_rt = right_top
	app.tab3d.ext_wall_lh = left_horizon
	app.tab3d.ext_wall_rh = right_horizon
	app.tab3d.ext_floor_bl = bottom_left
	app.tab3d.ext_floor_br = bottom_right

	self:print( "lt", left_top[1], left_top[2] )
	self:print( "rt", right_top[1], right_top[2] )
	self:print( "lh", left_horizon[1], left_horizon[2] )
	self:print( "rh", right_horizon[1], right_horizon[2] )
end

function CYBUL:update_ext_scene_dimensions()
	if not app.tab3d.ext_wall_lh then return end

	local wall_ratio  = app.scene.wall_height / app.scene.wall_width
	local floor_ratio = app.scene.floor_depth / app.scene.wall_width

	local photo = app.photo
	local width, height = 1, 1
	if photo then
		local bind = photo.bind
		width, height = aaa.img.get_size( bind )
	end
	local lh = self:get_pixel_pos_point( app.tab3d.ext_wall_lh, width, height )
	local rh = self:get_pixel_pos_point( app.tab3d.ext_wall_rh, width, height )
	local lt = self:get_pixel_pos_point( app.tab3d.ext_wall_lt, width, height )
	local rt = self:get_pixel_pos_point( app.tab3d.ext_wall_rt, width, height )

	local tl = self:get_pixel_pos_point( app.tab3d.points[1], width, height )
	local tr = self:get_pixel_pos_point( app.tab3d.points[2], width, height )
	local etl = self:get_pixel_pos_point( app.tab3d.ext_wall_lt, width, height )
	local etr = self:get_pixel_pos_point( app.tab3d.ext_wall_rt, width, height )

	local bl = self:get_pixel_pos_point( app.tab3d.points[5], width, height )
	local br = self:get_pixel_pos_point( app.tab3d.points[6], width, height )
	local ebl = self:get_pixel_pos_point( app.tab3d.ext_floor_bl, width, height )
	local ebr = self:get_pixel_pos_point( app.tab3d.ext_floor_br, width, height )

	local sl = self:get_pixel_pos_point( app.tab3d.points[3], width, height )
	local sr = self:get_pixel_pos_point( app.tab3d.points[4], width, height )

	local hor_vec     = V2.get_sub( sl,  sr )
	local top_vec     = V2.get_sub( tl,  tr )
	local ext_top_vec = V2.get_sub( etl, etr )
	local bot_vec     = V2.get_sub( bl,  br )
	local ext_bot_vec = V2.get_sub( ebl, ebr )

	local hor_norm = V2.norm( hor_vec )

	-- local top_angle     = math.acos( V2.dot( hor_vec,     top_vec ) / ( hor_norm * V2.norm(    top_vec ) ))
	-- local ext_top_angle = math.acos( V2.dot( hor_vec, ext_top_vec ) / ( hor_norm * V2.norm( ext_top_vec ) ))
	-- local top_ratio = math.abs( ext_top_angle / top_angle )
	-- local new_height = self.scene.wall_height * top_ratio
	
	local bot_angle     = math.acos( V2.dot( hor_vec,     bot_vec ) / ( hor_norm * V2.norm(    bot_vec ) ))
	local ext_bot_angle = math.acos( V2.dot( hor_vec, ext_bot_vec ) / ( hor_norm * V2.norm( ext_bot_vec ) ))
	local bot_ratio = math.abs( ext_bot_angle / bot_angle )
	local new_depth = app.scene.floor_depth * bot_ratio
	

	local top_left  = self:get_intersection( tl, tr, lh, lt )
	local top_right = self:get_intersection( tl, tr, rh, rt )
	local top_left_ratio  =  V2.dist( lt, lh ) / V2.dist( top_left,  lh )
	local top_right_ratio =  V2.dist( rt, rh ) / V2.dist( top_right, rh )
	self:print( "top", top_left_ratio, top_right_ratio )

	local bot_left  = self:get_intersection( bl, br, lh, ebl )
	local bot_right = self:get_intersection( bl, br, rh, ebr )
	local bot_left_ratio  =  V2.dist( ebl, lh ) / V2.dist( bot_left,  lh )
	local bot_right_ratio =  V2.dist( ebr, rh ) / V2.dist( bot_right, rh )
	self:print( "bot", bot_left_ratio, bot_right_ratio )

	-- self.scene.ext_wall_width  = V2.dist( lh, rh ) / V2.dist( sl, sr ) * self.scene.wall_width
	-- self.scene.ext_wall_height = new_height
	-- self.scene.ext_floor_depth = new_depth


	-- local function mm_to_m( a ) return {a[1] * 0.001, a[2] * 0.001} end
	app.scene.ext_wall_width  = 0.001 * ( V2.dist( lh, rh ) / V2.dist( sl, sr ) * app.scene.wall_width )
	app.scene.ext_wall_height = 0.001 * ( app.scene.wall_height * top_left_ratio )
	app.scene.ext_floor_depth = 0.001 * ( app.scene.floor_depth * bot_left_ratio )

	app.scene.ext_floor_depth = 0.001 * new_depth

	self:print( "ext_wall_width", app.scene.ext_wall_width )
	self:print( "ext_wall_height", app.scene.ext_wall_height )
	self:print( "ext_floor_depth", app.scene.ext_floor_depth )

	local center_wall = {0, 0}
	V2.bary( center_wall, sl, sr )
	local center_ratio = V2.dist( center_wall, lh ) / V2.dist( lh, rh ) - .5

	app.scene.scene_center_x =  -center_ratio * app.scene.ext_wall_width

	self:print( "scene center x", app.scene.scene_center_x )
end

function CYBUL:post_change_scene_dimensions()
	self:update_shadow_box()
	self:update_ext_scene_dimensions()
end

function CYBUL:update_perspective()
	self:print( "==============================================" )
	-- compute vanishing points
	local t = app.tab3d
	local pts = t.points
	t.vanishing_point_x		= self:get_intersection( pts[1], pts[2], pts[3], pts[4] )
	t.vanishing_point_y		= self:get_intersection( pts[1], pts[3], pts[2], pts[4] )
	t.vanishing_point_z		= self:get_intersection( pts[3], pts[5], pts[4], pts[6] )
	t.vanishing_point_x_bot = self:get_intersection( pts[3], pts[4], pts[5], pts[6] )

	if t.vanishing_point_x and t.vanishing_point_y then
		-- compute extra scene by extending as much as possible the perspective planes of the wall-floor
		self:extend_scene()
		self:update_ext_scene_dimensions()
	end
end

function CYBUL:update_output_fbo_mapping()
	local pts = app.tab3d.points_recadrage
	local meu = self:get_meu_by_name( "CybulRecadrage_1" )
	local map = meu:__get_mapping()

	local t_bu_size = meu.ui.__t_bu_size_uv

	local x_min = math.min( pts[1][1], pts[4][1] ) + .5
	local x_max = math.max( pts[1][1], pts[4][1] ) + .5
	local y_min = math.min( pts[1][2], pts[4][2] ) + .5
	local y_max = math.max( pts[1][2], pts[4][2] ) + .5
	param.set( map, "tex_left",   x_min )
	param.set( map, "tex_right",  x_max )
	param.set( map, "tex_top",    y_max )
	param.set( map, "tex_bottom", y_min )

	local gbuffer = self:get_meu_by_name( "Fbo_GBuffer" )
	local buf_sx, buf_sy = gbuffer:get_pixel_size()
	
	local width  = ( x_max - x_min ) * buf_sx
	local height = ( y_max - y_min ) * buf_sy
	local ratio = width / height

	local sx, sy
	if ratio > 16/9 then
		sx = 1920
		sy = math.floor( sx / ratio )
	else
		sy = 1080
		sx = math.floor( sy * ratio )
	end
	local f4 = self:get_meu_by_name( "Ref_Fbo_F4" )
	f4:set_pixel_size( sx, sy )

	local sz
	local orientation
	if sx == 1920 then
		orientation = "hor"
		sz = 4.5 * sy / 1080
		sx = 8
		sy = 4.5
	elseif sx >= sy then
		orientation = "ver"
		sy = 4.5 / sx * 1920
		sx = 8
	else
		orientation = "ver"
		sy = 4.5 / sx * 1920
		sx = 8
	end
	t_bu_size.su:set_value( sx )
	t_bu_size.sv:set_value( sy )
	local model = aaa.layer.get_model( meu:get_layer( 1 ) )
	if orientation == "hor" then
		param.set(model, "size_u", sx)
		param.set(model, "size_v", sz)
	else
		param.set(model, "size_u", sx)
		param.set(model, "size_v", sy)
	end
end