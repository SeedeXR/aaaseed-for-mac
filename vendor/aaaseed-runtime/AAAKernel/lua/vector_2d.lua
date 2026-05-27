aaa.show_file_begin( "vector_2d" )

if not V2 then
	V2 = { doc = {} }
end

--
--	BASE
--
function V2.new()					return { 0,0 }			end
function V2.set( dst, x, y )		dst[1],dst[2] = x,y		end
function V2.new_from( x, y )		return { x,y }			end
function V2.clone( pt )				return { pt[1], pt[2] }	end
function V2.cpy( dst, src )			dst[1],dst[2] = src[1],src[2]		end

--
--	OPERATION
--
function V2.add( dst, a, b )	dst[1], dst[2] = a[1]+b[1], a[2]+b[2]	end
function V2.get_add( a, b )		return {  a[1]+b[1], a[2]+b[2]	}		end
function V2.add_xy( a, x,y )
	a[1] = a[1] + x
	a[2] = a[2] + y
end

function V2.sub( dst, a, b )	dst[1], dst[2] = a[1]-b[1], a[2]-b[2]	end
function V2.get_sub( a, b )		return {  a[1]-b[1], a[2]-b[2]	}		end

function V2.scale( a, f )		a[1], a[2] = a[1]*f, a[2]*f		end
function V2.mul_xy( a, x,y )
	a[1] = a[1] * x
	a[2] = a[2] * y
end

function V2.add_scale( dst, a, b, f )	dst[1], dst[2] = (a[1]+b[1])*f, (a[2]+b[2])*f	end
function V2.sub_scale( dst, a, b, f )	dst[1], dst[2] = (a[1]-b[1])*f, (a[2]-b[2])*f	end
function V2.scale_add( dst, a, f, b )	dst[1], dst[2] = a[1]*f + b[1], a[2]*f + b[2]	end
function V2.scale_sub( dst, a, f, b )	dst[1], dst[2] = a[1]*f - b[1], a[2]*f - b[2]	end

--
-- BARYCENTER
--
function V2.middle( dst, a, b )	dst[1], dst[2] = (a[1]+b[1])*.5, (a[2]+b[2])*.5	end
function V2.get_middle( a, b )	return { (a[1]+b[1])*.5, (a[2]+b[2])*.5 }		end
function V2.bary( dst, a, b )
	dst[1] = (a[1] + b[1]) * .5
	dst[2] = (a[2] + b[2]) * .5
end
function V2.bary3( dst, a, b, c )
	local f = 1/3
	dst[1] = (a[1] + b[1] + c[1]) * f
	dst[2] = (a[2] + b[2] + c[2]) * f
end
-- 
-- SPECIAL V2
--
function V2.mul_x_table( dst, src, fx )
	for i=1,#src do
		local t = src[i]
		dst[i] = { t[1] * fx, t[2] }
	end
end
function V2.mul_table( dst, src, fx, fy )
	if fx and fx ~= 1. then
		if fy and fy ~= 1. then
			for i=1,#src do
				local t = src[i]
				dst[i] = { t[1]*fx, t[2]*fy }
			end
		else
			for i=1,#src do
				local t = src[i]
				dst[i] = { t[1]*fx, t[2] }
			end
		end
	else
		if fy and fy ~= 1. then
			for i=1,#src do
				local t = src[i]
				dst[i] = { t[1], t[2]*fy }
			end

		else
			for i=1,#src do
				local t = src[i]
				dst[i] = { t[1], t[2] }
			end
		end
	end
end

function V2.mul_add_table( dst, src, fx, fy, ox, oy )
	for i=1,#src do
		local t = src[i]
		dst[i] = { t[1] * fx + ox, t[2] * fy + oy }
	end
end
function V2.add_mul_table( dst, src, ox, oy, fx, fy )
	for i=1,#src do
		local t = src[i]
		dst[i] = { (t[1] + ox) * fx, (t[2] + oy) * fy }
	end
end

--
--	TRANSFO
--
function V2.rotate_90( a )				a[1],a[2] = -a[2], a[1] 	end
function V2.get_rotate_90( a )			return { -a[2], a[1] }		end
function V2.rotate_90_neg( a )			a[1],a[2] = a[2], -a[1] 	end
function V2.get_rotate_90_neg( a )		return { a[2], -a[1] }		end

--
--	NORMAL TANGENT
--
function V2.get_normal_raw( a,b, f )
	return { (b[2]-a[2])*f, (a[1]-b[1])*f }
end
function V2.get_tangent_raw( a,b, f )
	return { (b[1]-a[1])*f, (b[2]-a[2])*f }
end
function V2.get_normal_scaled( a,b, f )
	local x,y = b[2]-a[2], a[1]-b[1]
	local n = x*x + y*y
	if n==0 then
		return { 0,0 }
	end
	f = f / math.sqrt( x*x + y*y )
	return { x*f, y*f }
end
function V2.get_tangent_scaled( a,b, f )
	local x,y = b[1]-a[1], b[2]-a[2]
	local n = x*x + y*y
	if n==0 then
		return { 0,0 }
	end
	f = f / math.sqrt( x*x + y*y )
	return { x*f, y*f }
end

function V2.atan( a )
	return math.atan2( a[1], a[2] )
end

--
--	NORM DIST
--
--todo add if dist_squared_inferior at
function V2.norm_squared( a )
	local x,y = a[1], a[2]
	return x*x + y*y
end
function V2.norm( a )
	local x,y = a[1], a[2]
	return math.sqrt( x*x + y*y )
end
function V2.dist_squared( a, b )
	local x,y = a[1]-b[1], a[2]-b[2]
	return x*x + y*y
end
function V2.dist( a, b )
	local x,y = a[1]-b[1], a[2]-b[2]
	return math.sqrt( x*x + y*y )
end
function V2.clamp( a, d )
	local n = V2.norm_squared( a )
	if n > d*d then
		V2.scale( a, d / math.sqrt(n) )
		return true
	end
	return false
end
function V2.normalize( a )
	local x,y = a[1], a[2]
	local n = math.sqrt( x*x + y*y )
	if n == 0 then return false end
	n = 1/n
	a[1], a[2] = x*n, y*n
	return true
end

function V2.dist_from_xyxy( ax,ay, bx,by )
	local x = bx - ax
	local y = by - ay
	return math.sqrt( x*x + y*y )
end
function V2.dist_to_line_ab( pt, a, b )
	local x = b[1] - a[1]
	local y = b[2] - a[2]
	return math.abs( y*pt[1] - x*pt[2] + b[1]*a[2] - b[2]*a[1] ) / math.sqrt( x*x + y*y )
end

--
-- INTERPOLATE
--
function V2.interpolate( dst, a, b, bf )
	local af = 1-bf
	dst[1] = a[1]*af + b[1]*bf
	dst[2] = a[2]*af + b[2]*bf
end
function V2.get_interpolate( a, b, bf )
	local af = 1-bf
	return { a[1]*af + b[1]*bf, a[2]*af + b[2]*bf }
end
--todo optimize just by developing the call
function V2.interpolate_table( dst, src, f )
	for i=1,#src do
		local d = dst[i]
		V2.interpolate( d, d, src[i], f )
	end
end

function V2.check_for_nan( a )
	local is_nan = math.is_nan
	if is_nan( a[1] ) then a[1] = 0 end
	if is_nan( a[2] ) then a[2] = 0 end
end
function V2.interpolate_table_check_for_nan( dst, src, f )
	for i=1,#src do
		local d = dst[i]
		--aaa.print( d[2] 	)
		V2.interpolate( d, d, src[i], f )
		V2.check_for_nan( dst[i] )
	end
end

--
--	ANGLE
--
function V2.get_angle( a )
	return math.atan2( a[1], a[2] ) * math.pi2_over
end

function V2.dot( a, b )
	return a[1]*b[1] + a[2]*b[2]
end

aaa.show_file_end( "vector_2d" )
