
aaa.show_file_begin( "vector_3d" )

if not V3 then
	V3 = { doc = {} }
end

--
--	BASE
--
function V3.new()					return 	{ 0,0,0	}	end
function V3.set( dst, x, y, z )		dst[1],dst[2],dst[3] = x,y,z	end
function V3.new_from( x, y, z )		return	{ x,y,z }				end
function V3.clone( pt )				return	{ pt[1], pt[2],	pt[3] }	end
function V3.cpy( dst, src )			dst[1],dst[2],dst[3] = src[1],src[2],src[3]		end

--
--	OPERATION
--
function V3.add( v, a, b )	v[1], v[2], v[3] =	a[1]+b[1], a[2]+b[2], a[3]+b[3]		end
function V3.get_add( a, b )	return {			a[1]+b[1], a[2]+b[2], a[3]+b[3] }	end
function V3.add_xyz( a, x,y,z )
	a[1] = a[1] + x
	a[2] = a[2] + y
	a[3] = a[3] + z
end

function V3.sub( v, a, b )	v[1], v[2], v[3] =	a[1]-b[1], a[2]-b[2], a[3]-b[3]		end
function V3.get_sub( a, b )	return {			a[1]-b[1], a[2]-b[2], a[3]-b[3] }	end

function V3.scale( a, f )
	a[1] = a[1] * f
	a[2] = a[2] * f
	a[3] = a[3] * f
end
function V3.mul_xyz( a, x,y,z )
	a[1] = a[1] * x
	a[2] = a[2] * y
	a[3] = a[3] * z
end


function V3.add_scale( dst, a, b, f )
	dst[1] = ( a[1] + b[1] ) * f
	dst[2] = ( a[2] + b[2] ) * f
	dst[3] = ( a[3] + b[3] ) * f
end
function V3.sub_scale( dst, a, b, f )
	dst[1] = ( a[1] - b[1] ) * f
	dst[2] = ( a[2] - b[2] ) * f
	dst[3] = ( a[3] - b[3] ) * f
end
function V3.scale_add( dst, a, f, b )
	dst[1] = a[1] * f + b[1]
	dst[2] = a[2] * f + b[2]
	dst[3] = a[3] * f + b[3]
end
function V3.scale_sub( dst, a, f, b )
	dst[1] = a[1] * f - b[1]
	dst[2] = a[2] * f - b[2]
	dst[3] = a[3] * f - b[3]
end



--
-- BARYCENTER
--
function V3.middle( dst, a, b )
	dst[1] = (a[1]+b[1])*.5
	dst[2] = (a[2]+b[2])*.5
	dst[3] = (a[3]+b[3])*.5
end
function V3.get_middle( a, b )
	return { (a[1]+b[1])*.5, (a[2]+b[2])*.5, (a[3]+b[3])*.5 }
end
function V3.bary( dst, a, b )
	dst[1] = (a[1] + b[1]) * .5
	dst[2] = (a[2] + b[2]) * .5
	dst[3] = (a[3] + b[3]) * .5
end
function V3.bary3( dst, a, b, c )
	local f = 1/3
	dst[1] = (a[1] + b[1] + c[1]) * f
	dst[2] = (a[2] + b[2] + c[2]) * f
	dst[3] = (a[3] + b[3] + c[3]) * f
end
--
--	TRANSFO
--

function V3.get_rotate_x_90( a )	return {  a[1], -a[3],  a[2] }	end
function V3.get_rotate_y_90( a )	return {  a[3],  a[2], -a[1] }	end
function V3.get_rotate_z_90( a )	return { -a[2],  a[1],  a[3] }	end


--
--	NORM DIST
--
--todo add if dist_squared_inferior at
function V3.norm_squared( a )
	local x,y,z = a[1], a[2], a[3]
	return x*x + y*y + z*z
end
function V3.norm( a )
	local x,y,z = a[1], a[2], a[3]
	return math.sqrt( x*x + y*y + z*z )
end
function V3.dist_squared( a, b )
	local x,y,z = a[1]-b[1], a[2]-b[2], a[3]-b[3]
	return x*x + y*y + z*z
end
function V3.dist( a, b )
	local x,y,z = a[1]-b[1], a[2]-b[2], a[3]-b[3]
	return math.sqrt( x*x + y*y + z*z )
end
function V3.clamp( a, d )
	local n = V3.norm_squared( a )
	if n > d*d then
		V3.scale( a, d / math.sqrt(n) )
		return true
	end
	return false
end
function V3.normalize( a )
	local x,y,z = a[1], a[2], a[3]
	local n = math.sqrt( x*x + y*y + z*z )
	if n == 0 then return false end
	n = 1/n
	a[1], a[2], a[3] = x*n, y*n, z*n	
	return true
end
V3.doc.compute_face_normal = "( x1,y1,z1, x2,y2,z2, x3,y3,z3 ) Compute normal normalized for 3 points (x1,y1,z1), (x2,y2,z2), (x3,y3,z3)"
function V3.compute_face_normal( x1,y1,z1, x2,y2,z2, x3,y3,z3 )
    x2, y2, z2 = x2-x1, y2-y1, z2-z1
    x3, y3, z3 = x3-x1, y3-y1, z3-z1

    x1 = y2*z3 - z2*y3
    y1 = z2*x3 - x2*z3 
    z1 = x2*y3 - y2*x3

    local norm = math.sqrt( x1*x1 + y1*y1 + z1*z1 )
    if norm ~= 0 then norm = 1 / norm end

    return x1*norm, y1*norm, z1*norm
end

--
-- INTERPOLATE
--
function V3.interpolate( dst, a, b, bf )
	local af = 1-bf
	dst[1] = a[1]*af + b[1]*bf
	dst[2] = a[2]*af + b[2]*bf
	dst[3] = a[3]*af + b[3]*bf
end
function V3.get_interpolate( a, b, bf )
	local af = 1-bf
	return { a[1]*af + b[1]*bf, a[2]*af + b[2]*bf, a[3]*af + b[3]*bf }
end
--todo optimize just by developing the call
function V3.interpolate_table( dst, src, f )
	for i=1,#src do
		local d = dst[i]
		V3.interpolate( d, d, src[i], f )
	end
end

function V3.check_for_nan( a )
	local is_nan = math.is_nan
	if is_nan( a[1] ) then a[1] = 0 end
	if is_nan( a[2] ) then a[2] = 0 end
	if is_nan( a[3] ) then a[3] = 0 end
end
function V3.interpolate_table_check_for_nan( dst, src, f )
	for i=1,#src do
		local d = dst[i]
		--aaa.print( d[2] 	)
		V3.interpolate( d, d, src[i], f )
		V3.check_for_nan( dst[i] )
	end
end

--
--	ANGLE
--
function V3.get_angle_axe_x( a )
	local r = V3.norm( a )
	return math.acos( a[1] / r ) * math.pi2_over
end
function V3.get_angle_axe_y( a )
	local r = V3.norm( a )
	return math.acos( a[2] / r ) * math.pi2_over
end
function V3.get_angle_axe_z( a )
	local r = V3.norm( a )
	return math.acos( a[3] / r ) * math.pi2_over
end
function V3.get_angle_plane_x( a )	return math.atan2( a[2], a[3] ) * math.pi2_over	end
function V3.get_angle_plane_y( a )	return math.atan2( a[3], a[1] ) * math.pi2_over	end
function V3.get_angle_plane_z( a )	return math.atan2( a[1], a[2] ) * math.pi2_over	end

--todo promote this one
function V3.cross( dst, v1, v2 )
	dst[1],dst[2],dst[3]	=	v1[2] * v2[3]	-	v1[3] * v2[2]
							,	v1[3] * v2[1]	-	v1[1] * v2[3]
							,	v1[1] * v2[2]	-	v1[2] * v2[1]
end
function V3.get_cross( v1, v2 )
	local vr = V3.new()
	V3.cross( vr, v1, v2 )
	return vr
end
function V3.get_cross_x( v )		return	{	0, v[3], -v[2] }		end
function V3.get_cross_y( v )		return	{	-v[3], 0, v[1] }		end
function V3.get_cross_z( v )		return	{	v[2], -v[1], 0 }		end

function V3.dot( a, b )
	return a[1]*b[1] + a[2]*b[2] + a[3]*b[3]
end
function V3.get_angle( a, b )
	local n = V3.norm( a ) * V3.norm( b )
	local d = V3.dot( a, b ) / n
	return math.acos( d ) * math.pi2_over
end
function V3.get_angle_sum_top( o, a, b )
	local oa = {0,0}
	V3.sub( oa, a, o )
	local ab = {0,0}
	V3.sub( ab, b, a )
	return ( 0.5 - V3.get_angle_axe_y( oa ) ) + V3.get_angle( oa, ab )
end
function V3.get_angle_sum_z( o, a, b )
	local oa = {0,0,0}
	V3.sub( oa, a, o )
	local ab = {0,0,0}
	V3.sub( ab, b, a )
	return ( 0.5 - V3.get_angle_axe_y( oa ) ) + V3.get_angle( oa, ab )
end

function V3.compute_speed( dst, cur, prev, over_dt )
	dst[1] = (cur[1] - prev[1]) * over_dt
	dst[2] = (cur[2] - prev[2]) * over_dt
	dst[3] = (cur[3] - prev[3]) * over_dt
end

function V3.combine2( dst, fa, a, fb, b )
	dst[1] = fa*a[1] + fb*b[1]
	dst[2] = fa*a[2] + fb*b[2]
	dst[3] = fa*a[3] + fb*b[3]
end
function V3.combine3( dst, fa, a, fb, b, fc, c )
	dst[1] = fa*a[1] + fb*b[1] + fc*c[1]
	dst[2] = fa*a[2] + fb*b[2] + fc*c[2]
	dst[3] = fa*a[3] + fb*b[3] + fc*c[3]
end
function V3.slerp_unit( a, b, t )
	-- Dot product - the cosine of the angle between 2 vectors.
	local dot = V3.dot( a, b )
	--aaa.print( dot )

	--Clamp it to be in the range of Acos()
	--This may be unnecessary, but floating point precision can be a fickle mistress.
	dot = clamp( dot, -1, 1 )
	if dot == -1 then
		a[3] = a[3] + .000001	-- thos wa pick in Loyal
		dot = V3.dot( a, b )
	end
	--Acos(dot) returns the angle between start and end,
	--And multiplying that by percent returns the angle between
	--start and the final result.
	local theta = math.acos(dot) * t;
	local vec_rel = V3.clone( a )
	V3.scale( vec_rel, dot )
	V3.sub( vec_rel, b, vec_rel )
	V3.normalize( vec_rel )	-- Orthonormal basis
	-- The final result.
	V3.combine2( vec_rel, math.cos(theta), a, math.sin(theta), vec_rel )
	--return ((start*Mathf.Cos(theta)) + (RelativeVec*Mathf.Sin(theta)));
	return vec_rel
end
function V3.slerp( a, b, t )
	local la = V3.norm( a )
	V3.scale( a, 1/la )
	local lb = V3.norm( b )
	V3.scale( b, 1/lb )
	local c = V3.slerp_unit( a, b, t )
	V3.scale( c, interpolate( la, lb, t ) )
	return c
end

aaa.show_file_end( "vector_3d" )