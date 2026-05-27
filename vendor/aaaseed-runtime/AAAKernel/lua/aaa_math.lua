--MATH
aaa.show_file_begin( "aaa_math" )


--function mod( x, m )
--	return x-math.floor(x/m)*m
--end
math.pi_over = 1. /  math.pi
math.pi2 = math.pi * 2.
math.pi2_over = 1. /  math.pi2

function math.is_nan( x )
	return x ~= x -- specific to nan but don't always work say the net
--	return (not (a<0)) and (not (a>=0))
end
function math.CatmullRoll2d( t, p1, p2, p3, p4 )
	local t2 = t*t
	local t3 = t*t*t
	local v = {} --// Interpolated point

	--Catmull Rom spline Calculation

	v.x = ((-t3 + 2*t2-t)*(p1.x) + (3*t3-5*t2+2)*(p2.x) + (-3*t3+4*t2+t)* (p3.x) + (t3-t2)*(p4.x))/2
	v.y = ((-t3 + 2*t2-t)*(p1.y) + (3*t3-5*t2+2)*(p2.y) + (-3*t3+4*t2+t)* (p3.y) + (t3-t2)*(p4.y))/2
	--self:print("Values of v.x = %f and v.y = %f\n", v.x,v.y);

	return v
end

function math.rotate_ab_turn( a, b, angle )
	angle = angle * math.pi2
	local co, si = math.cos(angle), math.sin(angle)
	return co*a - si*b, si*a + co*b
end
function math.rotate_ab_rad( a, b, angle )
	local co, si = math.cos(angle), math.sin(angle)
	return co*a - si*b, si*a + co*b
end
--	angle in ]-.5,.5]
function math.get_angle( x,y )			return math.atan2( y,x ) / ( math.pi2 )		end
--	angle in [0,1[
function math.get_angle_01( x,y )
	local a = math.get_angle( x,y )
	if a < 0 then a = a + 1. end
	return a
end
function math.get_length_squared( x,y )	return x * x + y * y							end
function math.get_length( x,y )			return math.sqrt( math.get_length_squared( x,y ) )	end
function math.dist_v2r( a,b )			return math.get_length( a.x - b.x, a.y - b.y )		end

--
--	CLAMP
--
function clamp( x, a, b )
	if a then
		if b then
			if a<b then
				if x<a then return a	elseif b<x then return b	else return x	end
			else
				if x<b then return b 	elseif a<x then return a	else return x	end
			end
		else
			if x<a then return a else return x end
		end
	else
		if b then
			if b<x then return b else return x end
		else
			return x
		end
	end
end
function clamp_01( x )
	if		x<0 then 	return 0
	elseif	1<x then 	return 1
	end
	return x
end
function clamp_abs( x, val )
	if	val<x  then		return val
	else
		val = -val
		if x<val then 	return val
		else			return x
		end
	end
end
function clamp_max( x, max )	--	this is in fact a min fn, but "min( x, max )" is inclear for mist people
	if x<max then return x else return max end
end
function clamp_min( x, min )	--	this is in fact a min fn, but "max( x, min )" is inclear for mist people
	if x>min then return x else return min end
end

-- 2023 November this work fine for int
function wrap_int( x, min, max )
	if x <= max then
		if min <= x then
			return x
		end
	end
	return (x-min) % (max-min+1) + min
end
--to check this not sure of this one
function wrap( x, min, max )
	local a = x - min
	if 0 <= a then
		if x < max then
			return x
		else
			return math.fmod( a, max - min ) + min
		end
	else
		return max - math.fmod( -a, max - min )
	end
end
--to check this and the calls
function wrap_01( x )
	if 0 <= x then
		if x < 1 then
			return x
		else
			return math.fmod( x, 1 )
		end
	else
		return math.fmod( x, 1 ) + 1
	end
end


function triangle( x, min, max )
	local a = x-min
	
	-- if 0 <= a then
	-- 	if x < max then
	-- 		return x
	-- 	else
	-- 		local max_mod = max*2 - min
	-- 		if x < max_mod then
	-- 			return max_mod - a
	-- 		end
	-- 	end
	-- else
	-- 	a = -a
	-- end
	local b = max-min
	a = math.fmod( a, b*2  )
	if a < 0 then
		a = -a
	end
	if a < b then
		return a + min
	else
		return max*2 - min - a
	end
end
function triangle_01( x )
	x = math.fmod( x, 2  )
	if x < 0 then
		x = -x
	end
	if x < 1 then
		return x
	else
		return 2 - x
	end
end

function quantize( v, step )
	return step * math.floor( v / step )
end

function min_max( a, b )
	if a<=b then	return a,b
	else			return b,a
	end
end
function step_smooth_01( t )		return t*t*(3-2*t)				end
function step_smoother_01( t )		return t*t*t*(10+t*(6*t-15))	end

function interpolate( a,b, t )		return a + (b-a)*t 		end
--function interpolate( a, b, t )	return a*(1-t) + b*t 	end
function interpolate_to0( a, t )	return a*(1-t)			end
--function interpolate_to1( a, t )	return a*(1-t) + t		end
function interpolate_to1( a, t )	return a + (1-a)*t		end
function feed_secure( a, b, t )
	if not a or math.is_nan(a) then
		return b
	end
	if not b or math.is_nan(b) then
		return a
	end
	return a + (b-a)*t
end


function inside( x, a, b )
	if a<=b then
		return (a<=x and x<=b)
	end
	return (b<=x and x<=a)
end

--todo document global level
--doc.inside_01 = "like inside( v, 0, 1 )"
function inside_01( x )				return 0<=x and x<=1	end
function inside_min_max( x, a, b )	return a<=x and x<=b	end
function inside_lbrt( lbrt, x,y )	return lbrt.l<=x and x<=lbrt.r and lbrt.b<=y and y<=lbrt.t	end

function outside( x, a, b )
	if a<=b then	return (x<a or b<x)
	else			return (x<b or a<x)
	end
end
function outside_01( x )			return x<0 or 1<x	end

function average( v, new_v, weight )
	if not v then v=0 end
	if not weight then weight = .67 end
	if weight > 1 then weight = 1 end
	return v*weight + new_v*(1-weight)
end

function math.random_centered( size )
	return (math.random()-.5)*size
end
function math.random_real( max, min )
	if min then
		return min + math.random() * (max-min)
	end
	return math.random()*max

end

function math.fn_linear_all( val, nb, ... )
	--local arg = pack(...)
	local inc = nb+1
	local loop_nb = math.floor( select("#", ...) / inc ) - 1
--	aaa.print( loop_nb )
	local ind = 1
	local found
	local pa
	local pb = select( 1, ... )
	for i=1,loop_nb do
		pa = pb
		ind = ind + inc
		pb = select( ind, ... )
		if val<=pb then
			break
		end
	end
	found = ind - inc
--	if found then
		local to = {}
--		aaa.print( "found "..pa.." < "..val.." < "..pb )
		local inter = ( val - pa ) / ( pb - pa )
--		aaa.print( "   inter "..inter )
		for i=1,nb do
			found = found + 1
			to[i] = interpolate( select(found, ...), select(found+inc, ...), inter )
		end
		return unpack(to)
--	end
end

function math.fn_linear( val, nb, ... )
	--local arg = pack(...)
	local inc = nb+1
	local pb = select( 1, ... )
	local to = {}
	if val <= pb then
		for i=1,nb do
			to[i] = select(1+i, ...)
		end
	else
		local loop_nb = math.floor( select("#", ...) / inc ) - 1
	--	aaa.print( loop_nb )
		local ind = 1
		local found
		local pa
		for i=1,loop_nb do
			pa = pb
			ind = ind + inc
			pb = select( ind, ... )
			if val<=pb then
				found = ind
				break
			end
		end
		if found then
	--		aaa.print( "found "..pa.." < "..val.." < "..pb )
			local inter = ( val - pa ) / ( pb - pa )
	--		aaa.print( "   inter "..inter )
			for i=1,nb do
				found = found + 1
				to[i] = interpolate( select(found-inc, ...), select(found, ...), inter )
			end
		else
			for i=1,nb do
				to[i] = select(ind+i, ...)
			end
		end
	end
	return unpack(to)
end

if not aaa.logic then
	local logic = {}
	logic.__OR	= 1
	logic.__XOR	= 3
	logic.__AND	= 4

	function logic.oper( a, b, oper )
		local r, m, s = 0, 2^52
		repeat
			s,a,b = a+b+m, a%m, b%m
			r,m = r + m*oper%(s-a-b), m/2
		until m < 1
		return r
	end
	function logic.do_or(  a, b )		return logic.oper( a, b, logic.__OR )	end
	function logic.do_xor( a, b )		return logic.oper( a, b, logic.__XOR )	end
	function logic.do_and( a, b )		return logic.oper( a, b, logic.__AND )	end


--	print( "test logic "..logic.do_or(  6,3 ) )  --> 7
--	print( "test logic "..logic.do_xor( 6,3 ) )  --> 5
--	print( "test logic "..logic.do_and( 6,3 ) )  --> 2

	aaa.logic = logic
end



aaa.show_file_end( "aaa_math" )
