aaa.show_file_begin( "RECT" )


if not aaa.rect then
	aaa.rect = { doc = {} }
    aaa.rect.doc.doc = "Rectangle functions for intersection where passed rectangle are array { x,y, size_z,size_y, rotation_in_turn}"
	aaa.lbrt = { doc = {} }
    aaa.lbrt.doc.doc = "LBRT functions where passed lbrt are array { l=left, r=right, b=bottom, t=top}"
end

local RECT = aaa.rect
local LBRT = aaa.lbrt

function LBRT.interpolate( rect, src1, src2, inter )
	--table.print( src1 )
	--table.print( src2 )
	rect.l = interpolate( src1.l, src2.l, inter )
	rect.r = interpolate( src1.r, src2.r, inter )
	rect.b = interpolate( src1.b, src2.b, inter )
	rect.t = interpolate( src1.t, src2.t, inter )
end

-- supose le<ri and bo<to
function LBRT.include_lbrt( r, n )
	if n.l < r.l then r.l = n.l end	
	if r.r < n.r then r.r = n.r end
	if n.b < r.b then r.b = n.b end
	if r.t < n.t then r.t = n.t end
end

function LBRT.include_point( r, x, y )
	if x < r.l then r.l = x end
	if r.r < x then r.r = x end
	if y < r.b then r.b = y end
	if r.t < y then r.t = y end
end

function LBRT.mul_add( r, m, a )
	r.l = (r.l * m) + a
	r.r = (r.r * m) + a
	r.b = (r.b * m) + a
	r.t = (r.t * m) + a
end
function LBRT.add_mul( r, a, m )
	r.l = (r.l * a) + m
	r.r = (r.r * a) + m
	r.b = (r.b * a) + m
	r.t = (r.t * a) + m
end

function LBRT.make_rect_lbrt( le, bo, ri, to )
	return { l=le, b=bo, r=ri, t=to }
end

function LBRT.align_rect_to_x( rect, x, how )
	local s = rect.r - rect.l
	if		how == nil or how == "left"		then	x = x - s
	elseif	how == "center"					then	x = x - s / 2
	elseif	how == "right"					then
	else									aaa.print( "rect: how arg \""..how.."\" not recognized" )
	end
	rect.l = x
	rect.r = x + s
end

function LBRT.align_rect_to_y( rect, y, how )
	local s = rect.t - rect.b
	if		how == nil or how == "bottom"	then	y = y - s
	elseif	how == "center"					then	y = y - s / 2
	elseif	how == "top"					then
	else									aaa.print( "rect: how arg \""..how.."\" not recognized" )
	end
	rect.b = y
	rect.t = y + s
end

function RECT.is_intersect_SAT_half( a, b  )
    --we change coordonate system of b to be in a
    -- center
    local ba_x = b[1] - a[1]
    local ba_y = b[2] - a[2]

    -- angle then sin and cos
    local ba_rot = (b[5] - a[5]) * math.pi2
    local ba_cos,ba_sin = math.cos(ba_rot), math.sin(ba_rot)

    local rot = - a[5] * math.pi2
    local c,s = math.cos(rot), math.sin(rot)
    local cx =  ba_x * c - ba_y * s
    local cy =  ba_x * s + ba_y * c
 
    local v, min,max, sh

    -- half size
    local sxh,syh = b[3]*.5,b[4]*.5
    -- x vec rotated
    local x_x = sxh * ba_cos
    local x_y = sxh * ba_sin
     -- y vec rotated
    local y_x = syh * -ba_sin
    local y_y = syh * ba_cos

    --todo fn of the angle find min max more easily
    min = x_x + y_x
    max = min
    v = x_x - y_x
    if v < min then min = v elseif max < v then max = v end
    x_x = -x_x
    v = x_x + y_x
    if v < min then min = v elseif max < v then max = v end
    v = x_x - y_x
    if v < min then min = v elseif max < v then max = v end
    sh = a[3]*.5
    if max + cx < -sh or sh < min + cx then
        return false
    end

    min = x_y + y_y
    max = min
    v = x_y - y_y
    if v < min then min = v elseif max < v then max = v end
    x_y = -x_y
    v = x_y + y_y
    if v < min then min = v elseif max < v then max = v end
    v = x_y - y_y
    if v < min then min = v elseif max < v then max = v end
    sh = a[4]*.5
    if max + cy < -sh or sh < min + cy then
        return false
    end

    return true
end

RECT.doc.is_intersect_SAT = "SAT test for intersection of rectangle"
function RECT.is_intersect_SAT( a, b  )
    --we change coordonate system of b to be in a
    -- vector ba
    local ba_x = b[1] - a[1]
    local ba_y = b[2] - a[2]

    -- half size
    local b_sxh,b_syh = b[3]*.5,b[4]*.5
    local a_sxh,a_syh = a[3]*.5,a[4]*.5

    local a_rot = - a[5] * math.pi2
    local b_rot = - b[5] * math.pi2

    -- angle then sin and cos
    local ba_rot = a_rot - b_rot
    local ba_cos,ba_sin = math.cos(ba_rot), math.sin(ba_rot)

    local c,s = math.cos(a_rot), math.sin(a_rot)
    local cx = ba_x * c - ba_y * s
    local cy = ba_x * s + ba_y * c
 
    local v, min,max, sh

    -- x vec rotated
    local x_x = b_sxh * ba_cos
    local x_y = b_sxh * ba_sin
     -- y vec rotated
    local y_x = b_syh * -ba_sin
    local y_y = b_syh * ba_cos

    --todo fn of the angle find min max more easily
    min = x_x + y_x
    max = min
    v = x_x - y_x
    if v < min then min = v elseif max < v then max = v end
    x_x = -x_x
    v = x_x + y_x
    if v < min then min = v elseif max < v then max = v end
    v = x_x - y_x
    if v < min then min = v elseif max < v then max = v end
    if max + cx < -a_sxh or a_sxh < min + cx then
        return false
    end

    min = x_y + y_y
    max = min
    v = x_y - y_y
    if v < min then min = v elseif max < v then max = v end
    x_y = -x_y
    v = x_y + y_y
    if v < min then min = v elseif max < v then max = v end
    v = x_y - y_y
    if v < min then min = v elseif max < v then max = v end
    if max + cy < -a_syh or a_syh < min + cy then
        return false
    end

--now we do the same swapping a and b and reusing what we already have
    c,s = math.cos(b_rot), math.sin(b_rot)
    ba_x = - ba_x   -- we use sign inversion below for y
    cx = ba_x * c + ba_y * s
    cy = ba_x * s - ba_y * c
 
    -- ba_sin change sign
    -- x vec rotated
    x_x = a_sxh * ba_cos
    x_y = a_sxh * -ba_sin
     -- y vec rotated
    y_x = a_syh * ba_sin
    y_y = a_syh * ba_cos

    --todo fn of the angle find min max more easily
    min = x_x + y_x
    max = min
    v = x_x - y_x
    if v < min then min = v elseif max < v then max = v end
    x_x = -x_x
    v = x_x + y_x
    if v < min then min = v elseif max < v then max = v end
    v = x_x - y_x
    if v < min then min = v elseif max < v then max = v end
    if max + cx < -b_sxh or b_sxh < min + cx then
        return false
    end

    min = x_y + y_y
    max = min
    v = x_y - y_y
    if v < min then min = v elseif max < v then max = v end
    x_y = -x_y
    v = x_y + y_y
    if v < min then min = v elseif max < v then max = v end
    v = x_y - y_y
    if v < min then min = v elseif max < v then max = v end
    if max + cy < -b_syh or b_syh < min + cy then
        return false
    end

    return true
end

aaa.show_file_end( "RECT" )