-- AAASeed Lua helpers (extensions to standard math/string/table/array libs)
-- Defined in aaa_*.lua scripts under M:/AAA/AAASeed/AAAKernel/lua/
-- ======================================== MATH / LUA EXTENSIONS ========================================

-- MATH
	v	= aaa.math.get_noise1( x )

	v	= aaa.math.get_noise2( x,y )

	v	= aaa.math.get_noise3					( x,y,z )
	v	= aaa.math.get_noise3_by_perlin			( x,y,z )
	v	= aaa.math.get_noise3_improved			( x,y,z )
	-- v	= aaa.math.get_turbulence3_by_perlin	( x,y,z )
 	-- v is in [-1,1] 
	v	= aaa.math.get_turbulence(			x,y,z, freq, nb_harm )	--	by default freq = 1., nb_haram = 1
	v	= aaa.math.get_fractalsum(			x,y,z, freq, nb_harm )	--	by default freq = 1., nb_haram = 1
	v	= aaa.math.get_turbulence_improved( x,y,z, freq, nb_harm )	--	by default freq = 1., nb_haram = 1
	v	= aaa.math.get_fractalsum_improved(	x,y,z, freq, nb_harm )	--	by default freq = 1., nb_haram = 1
	v	= aaa.math.get_turbulence_wavelet(	x,y,z, freq, nb_harm )	--	by default freq = 1., nb_haram = 1
	v	= aaa.math.get_fractalsum_wavelet(	x,y,z, freq, nb_harm )	--	by default freq = 1., nb_haram = 1

	v	= aaa.math.gain( v, gain )
	v	= aaa.math.bias( v, bias )
	v	= aaa.math.gain_bias( v, gain, bias )			-- do gain then bias
	-- same but here gain and bias are clamp not to be 0 or 1
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

	-- MATH pure-lua helpers (defined in aaa_math.lua, extends global math.*)
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

	-- LOGIC helpers (defined in aaa_math.lua)
	r	= logic.oper( a, b, oper )			-- lua	"or"|"xor"|"and" dispatch
	r	= logic.do_or( a, b )				-- lua
	r	= logic.do_xor( a, b )				-- lua
	r	= logic.do_and( a, b )				-- lua

-- STRING extensions (lua, defined in aaa_string.lua)
	b	= string.is_trailing_slash( str )			-- lua
	b	= string.is_trailing_slash_dialog( str )	-- lua
	str	= string.remove_trailing_slash( str )		-- lua	already exposed earlier in FILE AND DIR
	str	= string.enforce_trailing_slash( str )		-- lua

-- TABLE extensions (lua, defined in aaa_table.lua)
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

-- ARRAY
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
	tab:resize( nb_elt )					-- do a C realloc on the array (we don't loose data)
	tab:free()								-- free the memory on the C side

	-- array.* pure-lua helpers (defined in aaa_table.lua, distinct from aaa.array.* C above)
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

-- FORMAT
	str		= aaa.format.int_to_char2( val )			-- lua	pad int to 2 chars
	str		= aaa.format.int_to_char3( val )			-- lua	pad int to 3 chars
	str		= aaa.format.int_to_char_with_space( nb )	-- lua	int with thousand separator spaces
	str		= aaa.format.real_to_char2( val )			-- lua
	str		= aaa.format.real_dot2( val )				-- lua

