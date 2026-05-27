-- AAASeed Lua helpers (extensions to standard math/string/table/array libs)
-- Defined in aaa_*.lua scripts under M:/AAA/AAASeed/AAAKernel/lua/
-- a -- lua tag at end of line means defined in a Lua script; no tag means C binding.
-- Files where every entry is pure Lua (e.g. lua_aaaseed_GABU_OBJ.lua) drop the tag entirely.
--
-- This file documents the language-level helpers (math/string/table/array/format extensions, geometry primitives)
-- from aaa_*.lua plus the top-level helper files vector_2d.lua, vector_3d.lua, RECT.lua, csv_util.lua.
-- GABU_OBJ + CLASS have their own file (lua_aaaseed_GABU_OBJ.lua / .md).
-- SHADING has its own file (lua_aaaseed_SHADING.lua / .md).
-- Class wrappers (TRANSFO, OPENCL) and GaBu/* subfolders are not yet covered.
-- See AAAKernel/Dev/AAAUser/Guest/default.lua_master.lua for the full set of files loaded at runtime.

-- ======================================== MATH / LUA EXTENSIONS ========================================

-- # MATH
	v	= aaa.math.get_noise1( x )

	v	= aaa.math.get_noise2( x,y )

	v	= aaa.math.get_noise3					( x,y,z )
	v	= aaa.math.get_noise3_by_perlin			( x,y,z )
	v	= aaa.math.get_noise3_improved			( x,y,z )
	-- v	= aaa.math.get_turbulence3_by_perlin	( x,y,z )
 	-- v is in [-1,1] 
	v	= aaa.math.get_turbulence(			x,y,z, freq, nb_harm )	--	by default freq = 1., nb_harm = 1
	v	= aaa.math.get_fractalsum(			x,y,z, freq, nb_harm )	--	by default freq = 1., nb_harm = 1
	v	= aaa.math.get_turbulence_improved( x,y,z, freq, nb_harm )	--	by default freq = 1., nb_harm = 1
	v	= aaa.math.get_fractalsum_improved(	x,y,z, freq, nb_harm )	--	by default freq = 1., nb_harm = 1
	v	= aaa.math.get_turbulence_wavelet(	x,y,z, freq, nb_harm )	--	by default freq = 1., nb_harm = 1
	v	= aaa.math.get_fractalsum_wavelet(	x,y,z, freq, nb_harm )	--	by default freq = 1., nb_harm = 1

	v	= aaa.math.gain( v, gain )
	v	= aaa.math.bias( v, bias )
	v	= aaa.math.gain_bias( v, gain, bias )			-- do gain then bias
	-- same but here gain and bias are clamped not to be 0 or 1
	v	= aaa.math.gain_clamped( v, gain )
	v	= aaa.math.bias_clamped( v, bias )
	v	= aaa.math.gain_bias_clamped( v, gain, bias )

	x,y,z	= aaa.math.do_catmull_rom_xyz				( s, ax,ay,az, bx,by,bz, cx,cy,cz, dx,dy,dz )
	x,y,z	= aaa.math.do_catmull_rom_xyz				( s )		-- compute with the previous points
	x,y,z	= aaa.math.do_catmull_rom_derivative_xyz	( s, ax,ay,az, bx,by,bz, cx,cy,cz, dx,dy,dz )
	x,y,z	= aaa.math.do_catmull_rom_derivative_xyz	( s )		-- compute with the previous points

	x,y		= aaa.math.do_catmull_rom_xy				( s, ax,ay,    bx,by,    cx,cy,    dx,dy    )
	x,y		= aaa.math.do_catmull_rom_xy				( s )		-- compute with the previous points
	x,y		= aaa.math.do_catmull_rom_derivative_xy		( s, ax,ay,    bx,by,    cx,cy,    dx,dy    )
	x,y		= aaa.math.do_catmull_rom_derivative_xy		( s )		-- compute with the previous points

	x,y,z	= aaa.math.do_bezier_xyz					( s, ax,ay,az, bx,by,bz, cx,cy,cz, dx,dy,dz )
	x,y,z	= aaa.math.do_bezier_xyz					( s )		-- compute with the previous points

	x,y	 	= aaa.math.do_bezier_xy						( s, ax,ay,    bx,by,    cx,cy,    dx,dy    )
	x,y		= aaa.math.do_bezier_xy						( s )		-- compute with the previous points

	d	= aaa.math.convert_hilbert_xy_to_d( x,y [,n] )	-- n is 256 by default
	x,y	= aaa.math.convert_hilbert_d_to_xy( d [,n] )	-- n is 256 by default

	-- # MATH pure-lua helpers (defined in aaa_math.lua, extends global math.*)
	b	= math.is_nan( x )					-- lua
	v	= math.CatmullRoll2d( t, p1, p2, p3, p4 )		-- lua	2d Catmull-Rom interp
	x,y	= math.rotate_ab_turn( a, b, angle )			-- lua	rotate point (a,b) by angle in turns
	x,y	= math.rotate_ab_rad( a, b, angle )				-- lua	rotate by radians
	a	= math.get_angle( x,y )				-- lua	angle in ]-.5,.5]
	a	= math.get_angle_01( x,y )			-- lua	angle in [0,1[
	d2	= math.get_length_squared( x,y )	-- lua
	d	= math.get_length( x,y )			-- lua
	d	= math.dist_v2r( a,b )				-- lua	euclid distance between two v2 tables
	v	= math.random_centered( size )		-- lua	random in [-size/2, size/2]
	v	= math.random_real( max, min )		-- lua
	v	= math.fn_linear_all( val, nb, ... )	-- lua	piecewise-linear via varargs sample list
	v	= math.fn_linear( val, nb, ... )	-- lua

	--LOGIC helpers (defined in aaa_math.lua)
	r	= logic.oper( a, b, oper )			-- lua	"or"|"xor"|"and" dispatch
	r	= logic.do_or( a, b )				-- lua
	r	= logic.do_xor( a, b )				-- lua
	r	= logic.do_and( a, b )				-- lua

-- # STRING extensions (lua, defined in aaa_string.lua)
	b	= string.is_trailing_slash( str )			-- lua
	b	= string.is_trailing_slash_dialog( str )	-- lua
	str	= string.remove_trailing_slash( str )		-- lua	already exposed earlier in FILE AND DIR
	str	= string.enforce_trailing_slash( str )		-- lua

-- # TABLE extensions (lua, defined in aaa_table.lua)
	table.create( array_elt_nb, hash_elt_nb )	-- create a new table with pre-allocated array part for array_elt_nb elements and hash part for hash_elt_nb elements (both optional, default is 0)
	-- Lua "extension/helper"
	t2	= table.copy_simple( src )				-- lua	1-level copy
	t2	= table.copy_simple_check( src, signature )	-- lua
	t2	= table.copy_shallow( src )				-- lua
	t2	= table.copy_deep( src )				-- lua
	t	= table.get_field_table_always( dst, key, sub_key )	-- lua	get-or-create nested subtable
	str	= table.build_table_header( t )			-- lua
	table.print( tt, str, level, indent )		-- lua	pretty-print nested tables
	table.apply_fn( tab, fn, ... )				-- lua
	table.apply_method( tab, method_name, ... )	-- lua
	t2	= table.slice( tbl, first, last, step )	-- lua
	str	= table.to_string( t )					-- lua
	t	= table.from_string( str )				-- lua
	table.show( t, name, indent )				-- lua
	k	= table.find_key_by_val( t, val )		-- lua
	t2	= table.reverse( t )					-- lua
	n	= table.count_item( t )					-- lua
	b	= table.is_empty( t )					-- lua
	t2	= table.build_array_with_unique_value( t )	-- lua


-- # ARRAY
	tab	= aaa.array.new_int8	( nb )
	tab	= aaa.array.new_uint8	( nb )
	tab	= aaa.array.new_int16	( nb )
	tab	= aaa.array.new_uint16	( nb )
	tab	= aaa.array.new_int32	( nb )
	tab	= aaa.array.new_uint32	( nb )
	tab	= aaa.array.new_fp32	( nb )
	tab	= aaa.array.new_fp64	( nb )
	-- then tab can be use as a regular table with int id which should be between 1 and nb
	-- as in
	print( tab[id] )
	tab[id] = value

	tab:clear()								-- set all elt to 0
	tab:fill( value [,start [,stop] ] )		-- set elts to value
	tab:set( id, value1 [ [,value2 ... ] ] )		-- set consecutive sltws tab[id] = value1, tab[id+1] = value2 ...
	tab:resize( nb_elt )					-- do a C realloc on the array (we don't lose data)
	tab:free()								-- free the memory on the C side

	-- # array.* pure-lua helpers (defined in aaa_table.lua, distinct from aaa.array.* C above)
	array.set( t, val, nb )								-- lua	set first nb elements to val
	t	= array.new( nb, val )							-- lua	new array of nb elements filled with val
	idx	= array.find_index_by_val( t, val )				-- lua
	array.remove_by_val( t, val )						-- lua
	array.remove_by_vals_table( t, vals_table )			-- lua
	idx	= array.find_index_by_id( t, nb, id )			-- lua	for arrays of {id=...} records
	elt	= array.find_elt_by_id( t,nb, id )				-- lua
	b	= array.have_val( t, nb, val )					-- lua
	elt	= array.find_elt_by_key_str_lowercase( t, nb, key, str )	-- lua
	elt	= array.find_elt_by_field_val( t, nb, key, val )	-- lua
	t2	= array.extract_subfield_as_array( t, key )		-- lua
	array.apply_fn( tab, fn, ... )						-- lua
	array.apply_begin_end_fn( tab, i_begin, i_end, fn, ... )	-- lua
	array.apply_method( tab, method_name, ... )			-- lua
	array.apply_method_index( tab, method_name, ... )	-- lua
	t2	= array.copy_simple( tab )						-- lua
	array.append_to( dst, tab )							-- lua	push all of tab onto dst
	v	= array.interpolate( td, t0, t1, inter )		-- lua
	v,i	= array.get_max( t, nb_or_first, last )			-- lua	returns value + index
	v,i	= array.get_min( t, nb_or_first, last )			-- lua

-- # FORMAT
	str		= aaa.format.int_to_char2( val )			-- lua	pad int to 2 chars
	str		= aaa.format.int_to_char3( val )			-- lua	pad int to 3 chars
	str		= aaa.format.int_to_char_with_space( nb )	-- lua	int with thousand separator spaces
	str		= aaa.format.real_to_char2( val )			-- lua
	str		= aaa.format.real_dot2( val )				-- lua


-- ======================================== GEOMETRY ========================================
-- Vectors are plain Lua arrays: V2 = { x, y }, V3 = { x, y, z }.
-- Functions writing into a dst arg mutate it in place; get_* variants return a new table.

-- # V2 (vector_2d.lua)

-- ## BASE
	v	= V2.new()								-- lua	{ 0, 0 }
	V2.set( dst, x, y )							-- lua
	v	= V2.new_from( x, y )					-- lua	{ x, y }
	v	= V2.clone( pt )						-- lua
	V2.cpy( dst, src )							-- lua

-- ## OPERATION
	V2.add( dst, a, b )							-- lua	dst = a + b
	v	= V2.get_add( a, b )					-- lua
	V2.add_xy( a, x, y )						-- lua	a += (x, y)
	V2.sub( dst, a, b )							-- lua	dst = a - b
	v	= V2.get_sub( a, b )					-- lua
	V2.scale( a, f )							-- lua	a *= f
	V2.mul_xy( a, x, y )						-- lua	a[1] *= x; a[2] *= y
	V2.add_scale( dst, a, b, f )				-- lua	dst = (a + b) * f
	V2.sub_scale( dst, a, b, f )				-- lua	dst = (a - b) * f
	V2.scale_add( dst, a, f, b )				-- lua	dst = a * f + b
	V2.scale_sub( dst, a, f, b )				-- lua	dst = a * f - b

-- ## BARYCENTER
	V2.middle( dst, a, b )						-- lua	dst = (a + b) / 2
	v	= V2.get_middle( a, b )					-- lua
	V2.bary( dst, a, b )						-- lua	same as middle
	V2.bary3( dst, a, b, c )					-- lua	dst = (a + b + c) / 3

-- ## SPECIAL (operate on tables of V2 points)
	V2.mul_x_table( dst, src, fx )				-- lua	scale x of every src[i] by fx
	V2.mul_table( dst, src, fx, fy )			-- lua
	V2.mul_add_table( dst, src, fx, fy, ox, oy ) -- lua	dst[i] = src[i] * (fx,fy) + (ox,oy)
	V2.add_mul_table( dst, src, ox, oy, fx, fy ) -- lua	dst[i] = (src[i] + (ox,oy)) * (fx,fy)

-- ## TRANSFO
	V2.rotate_90( a )							-- lua	rotate +90 deg in place
	v	= V2.get_rotate_90( a )					-- lua
	V2.rotate_90_neg( a )						-- lua	rotate -90 deg in place
	v	= V2.get_rotate_90_neg( a )				-- lua

-- ## NORMAL TANGENT
	v	= V2.get_normal_raw( a, b, f )			-- lua	unscaled normal of segment a->b times f
	v	= V2.get_tangent_raw( a, b, f )			-- lua	unscaled tangent of segment a->b times f
	v	= V2.get_normal_scaled( a, b, f )		-- lua	normalized normal times f; { 0,0 } if a == b
	v	= V2.get_tangent_scaled( a, b, f )		-- lua	normalized tangent times f
	a	= V2.atan( a )							-- lua	math.atan2( a[1], a[2] )

-- ## NORM DIST
	n	= V2.norm_squared( a )					-- lua	x*x + y*y
	n	= V2.norm( a )							-- lua	sqrt( x*x + y*y )
	d	= V2.dist_squared( a, b )				-- lua
	d	= V2.dist( a, b )						-- lua
	b	= V2.clamp( a, d )						-- lua	clamp norm to d in place; return true if clamped
	b	= V2.normalize( a )						-- lua	normalize in place; return false if zero length
	d	= V2.dist_from_xyxy( ax, ay, bx, by )	-- lua
	d	= V2.dist_to_line_ab( pt, a, b )		-- lua	distance from pt to line through a,b

-- ## INTERPOLATE
	V2.interpolate( dst, a, b, bf )				-- lua	dst = a*(1-bf) + b*bf
	v	= V2.get_interpolate( a, b, bf )		-- lua
	V2.interpolate_table( dst, src, f )			-- lua	interpolate dst[i] toward src[i] by f
	V2.check_for_nan( a )						-- lua	replace NaN components by 0
	V2.interpolate_table_check_for_nan( dst, src, f )	-- lua

-- ## ANGLE / DOT
	a	= V2.get_angle( a )						-- lua	atan2(x, y) * pi2_over (turn units)
	d	= V2.dot( a, b )						-- lua	a.x*b.x + a.y*b.y


-- # V3 (vector_3d.lua)

-- ## BASE
	v	= V3.new()								-- lua	{ 0, 0, 0 }
	V3.set( dst, x, y, z )						-- lua
	v	= V3.new_from( x, y, z )				-- lua	{ x, y, z }
	v	= V3.clone( pt )						-- lua
	V3.cpy( dst, src )							-- lua

-- ## OPERATION
	V3.add( dst, a, b )							-- lua	dst = a + b
	v	= V3.get_add( a, b )					-- lua
	V3.add_xyz( a, x, y, z )					-- lua	a += (x, y, z)
	V3.sub( dst, a, b )							-- lua	dst = a - b
	v	= V3.get_sub( a, b )					-- lua
	V3.scale( a, f )							-- lua	a *= f
	V3.mul_xyz( a, x, y, z )					-- lua	a[1] *= x; a[2] *= y; a[3] *= z
	V3.add_scale( dst, a, b, f )				-- lua	dst = (a + b) * f
	V3.sub_scale( dst, a, b, f )				-- lua	dst = (a - b) * f
	V3.scale_add( dst, a, f, b )				-- lua	dst = a * f + b
	V3.scale_sub( dst, a, f, b )				-- lua	dst = a * f - b

-- ## BARYCENTER
	V3.middle( dst, a, b )						-- lua	dst = (a + b) / 2
	v	= V3.get_middle( a, b )					-- lua
	V3.bary( dst, a, b )						-- lua	same as middle
	V3.bary3( dst, a, b, c )					-- lua	dst = (a + b + c) / 3

-- ## TRANSFO
	v	= V3.get_rotate_x_90( a )				-- lua	rotate +90 deg around X axis, return new
	v	= V3.get_rotate_y_90( a )				-- lua	rotate +90 deg around Y axis, return new
	v	= V3.get_rotate_z_90( a )				-- lua	rotate +90 deg around Z axis, return new

-- ## NORM DIST
	n	= V3.norm_squared( a )					-- lua	x*x + y*y + z*z
	n	= V3.norm( a )							-- lua
	d	= V3.dist_squared( a, b )				-- lua
	d	= V3.dist( a, b )						-- lua
	b	= V3.clamp( a, d )						-- lua	clamp norm to d in place; return true if clamped
	b	= V3.normalize( a )						-- lua	normalize in place; return false if zero length
	x,y,z	= V3.compute_face_normal( x1,y1,z1, x2,y2,z2, x3,y3,z3 )	-- lua	normalized face normal of triangle (1,2,3)

-- ## INTERPOLATE
	V3.interpolate( dst, a, b, bf )				-- lua	dst = a*(1-bf) + b*bf
	v	= V3.get_interpolate( a, b, bf )		-- lua
	V3.interpolate_table( dst, src, f )			-- lua	interpolate dst[i] toward src[i] by f
	V3.check_for_nan( a )						-- lua	replace NaN components by 0
	V3.interpolate_table_check_for_nan( dst, src, f )	-- lua

-- ## ANGLE
	a	= V3.get_angle_axe_x( a )				-- lua	angle to X axis (turn units)
	a	= V3.get_angle_axe_y( a )				-- lua
	a	= V3.get_angle_axe_z( a )				-- lua
	a	= V3.get_angle_plane_x( a )				-- lua	atan2(y, z) * pi2_over
	a	= V3.get_angle_plane_y( a )				-- lua	atan2(z, x) * pi2_over
	a	= V3.get_angle_plane_z( a )				-- lua	atan2(x, y) * pi2_over
	a	= V3.get_angle( a, b )					-- lua	angle between a and b (turn units)
	a	= V3.get_angle_sum_top( o, a, b )		-- lua
	a	= V3.get_angle_sum_z( o, a, b )			-- lua

-- ## CROSS / DOT
	V3.cross( dst, v1, v2 )						-- lua	dst = v1 x v2
	v	= V3.get_cross( v1, v2 )				-- lua
	v	= V3.get_cross_x( v )					-- lua	v x { 1, 0, 0 } shortcut
	v	= V3.get_cross_y( v )					-- lua
	v	= V3.get_cross_z( v )					-- lua
	d	= V3.dot( a, b )						-- lua

-- ## SPEED / COMBINE / SLERP
	V3.compute_speed( dst, cur, prev, over_dt )	-- lua	dst = (cur - prev) * over_dt
	V3.combine2( dst, fa, a, fb, b )			-- lua	dst = fa*a + fb*b
	V3.combine3( dst, fa, a, fb, b, fc, c )		-- lua	dst = fa*a + fb*b + fc*c
	v	= V3.slerp_unit( a, b, t )				-- lua	spherical linear interpolation, a and b assumed unit length
	v	= V3.slerp( a, b, t )					-- lua	slerp + length interpolation


-- # RECT (RECT.lua)
-- Two layouts coexist:
--   aaa.rect.* operates on rectangles { center_x, center_y, size_x, size_y, rotation_in_turn }
--   aaa.lbrt.* operates on axis-aligned boxes { l = left, b = bottom, r = right, t = top }

-- ## LBRT (axis-aligned)
	aaa.lbrt.interpolate( rect, src1, src2, inter )	-- lua	field-wise lerp into rect
	aaa.lbrt.include_lbrt( r, n )					-- lua	grow r to include rect n
	aaa.lbrt.include_point( r, x, y )				-- lua	grow r to include point (x, y)
	aaa.lbrt.mul_add( r, m, a )						-- lua	r = r * m + a (each field)
	aaa.lbrt.add_mul( r, a, m )						-- lua
	rect	= aaa.lbrt.make_rect_lbrt( le, bo, ri, to )	-- lua	{ l = le, b = bo, r = ri, t = to }
	aaa.lbrt.align_rect_to_x( rect, x, how )		-- lua	how in "left" / "center" / "right"
	aaa.lbrt.align_rect_to_y( rect, y, how )		-- lua	how in "bottom" / "center" / "top"

-- ## RECT (oriented, with rotation)
	b	= aaa.rect.is_intersect_SAT( a, b )			-- lua	separating axis test, full
	b	= aaa.rect.is_intersect_SAT_half( a, b )	-- lua	separating axis test, half (asymmetric, faster)


-- # CSV / SPLIT (csv_util.lua)
	tab	= table:from_csv( str, del )						-- lua	parse a CSV-style string with delimiter del; supports quoted fields with "" escapes
	tab	= str:split( delimiter )					-- lua	split str on delimiter, return array of substrings (string method, defined globally)

