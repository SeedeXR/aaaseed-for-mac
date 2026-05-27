if CLASS.DECLARE( "POINT_LIST" ) then	--todo not sure should be a GABU_OBJ
	--todo have lighter object if required for speed
	POINT_LIST._b_noname_can = true
end

function POINT_LIST:create( name )
	local self = POINT_LIST:create_instance( name )
	self.__pts = {}
	return self
end
function POINT_LIST:create_nb( nb )
	local self = POINT_LIST:create( )
	self:set_size( nb )
	return self
end

function POINT_LIST:get_pts()				return self.__pts		end
function POINT_LIST:set_pts( pts )			self.__pts = pts		end

function POINT_LIST:get_size()				return #(self.__pts)	end
function POINT_LIST:get_nb()				return #(self.__pts)	end

function POINT_LIST:get_pt( id )			return self.__pts[id]	end
function POINT_LIST:set_pt( id, val )
	local pt = self.__pts[id]
	pt[1] = val[1]
	pt[2] = val[2]
	pt[3] = val[3] or 0
end
function POINT_LIST:set_pt_xyz( id, x, y, z )
	local pt = self.__pts[id]
	pt[1] = x
	pt[2] = y
	pt[3] = z or 0
end
function POINT_LIST:set_pt_xy( id, x, y )
	self:set_pt_xyz( id, x, y, 0 )
end
function POINT_LIST:add_pt_xyz( x, y, z )
	local pts = self.__pts
	table.insert( self.__pts, { x,y,z or 0 } )
	return #pts
end
function POINT_LIST:add_pt_xy( x, y )
	return self:add_pt_xyz( x,y,0 )
end
function POINT_LIST:get_pt_xy( id )
	local t=self.__pts[id]
	if t then
		return t[1], t[2]
	end
end
function POINT_LIST:get_pt_xyz( id )
	local t=self.__pts[id]
	return t[1], t[2], t[3]
end
function POINT_LIST:get_sub_xyz( id1, id2 )
	local t1 = self.__pts[id1]
	local t2 = self.__pts[id2]
	return t2[1]-t1[1], t2[2]-t1[2], t2[3]-t1[3]
end
function POINT_LIST:get_sym_xyz( idc, idv )
	local c = self.__pts[idc]
	local pt = self.__pts[idv]
	return 2*c[1]-pt[1], 2*c[2]-pt[2], 2*c[3]-pt[3]
end
function POINT_LIST:set_z( id, val )		self.__pts[id][3] = val	end

function POINT_LIST:get_dist( ida, idb )
	return V3.dist( self:get_pt( ida ), self:get_pt( idb ) )
end

function POINT_LIST:set_size( nb )
	local pts = self:get_pts()
	local n = #pts
	--todo optimize
	if nb > n then
		for i=n+1,nb do table.insert( pts, { 0,0,0 } ) end
	else
		for i=nb+1,n do table.remove( pts ) end
	end
end

function POINT_LIST:init_3d( nb, val )
	self:set_size( nb )
	for id=1,nb do self:set_pt( id, val ) end
end
function POINT_LIST:get_min_y( )
	local nb = self:get_size()
	local y = 100000000
	for id=1,nb do
		local v = self:get_pt( id )[2]
		--self:print( id.."  "..v )
		if v < y then y = v end
	end
	return y
end
function POINT_LIST:get_min_max_z( )
	local nb = self:get_size()
	local min = 100000000
	local max = -min
	for id=1,nb do
		local v = self:get_pt( id )[3]
		if v < min then min = v end
		if v > max then max = v end
	end
	return min, max
end
function POINT_LIST:get_bary_2d( start, stop )
	local x,y = 0,0
	for id = start, stop do
		local v = self:get_pt(id)
		x,y = x+v[1], y+v[2]
	end
	local f = 1/(stop-start+1)
	return x*f, y*f
end

function POINT_LIST:mul_xyz( x,y,z )	for id=1,self:get_size() do V3.mul_xyz( self:get_pt(id), x,y,z )	end 	end
function POINT_LIST:add_xyz( x,y,z )	for id=1,self:get_size() do V3.add_xyz( self:get_pt(id), x,y,z )	end 	end
function POINT_LIST:force_z( z ) 		for id=1,self:get_size() do self:set_z( id, z )						end 	end

function POINT_LIST:cpy_to( dst )
	local nb = self:get_size()
	dst:set_size( self:get_size() )
	for id=1,nb do dst:set_pt( id, self:get_pt(id) ) end
end
function POINT_LIST:mul_add( sx,sy, ox,oy )
	local pts = self:get_pts()
	V2.mul_add_table( pts, pts, sx,sy, ox,oy )
end
function POINT_LIST:mul_add_2d_to( dst, sx,sy, ox,oy )
	self:cpy_to( dst )
	dst:mul_add( sx,sy, ox,oy )
end
function POINT_LIST:set_interpolate_factor( f )
	self.__inter_factor = f
end

function POINT_LIST:save_csv( id, pt, file, bind, fx,fy )
	local x,y,z = pt[1], pt[2], pt[3]
	file:write( id, ", " )
	file:write( x,  ", " )
	file:write( y,  ", " )
	file:write( z,  ", " )
	local r,g,b = aaa.img.get_color_uv(	bind, (x*fx)+.5, (y*fy)+.5, true )
	file:write( r,  ", " )
	file:write( g,  ", " )
	file:write( b,  "\n" )
end
function POINT_LIST:save( fpath, format, bind, sx,sy )
--	format = format or "csv"
	local file = io.open( fpath, "a+")
	if file then
		--if format == "csv" then
		self:process_points_with_method( self, "save_csv", file, bind, 1./sx, 1./sy )
		file:close()
	end

	self:print( "csv file written !" )
end

function POINT_LIST:read_bdd( bdd, dataset_id )
	--self:print( "POINT_LIST:read_bdd( "..bdd )
	local pts
	if aaa.b_ios then
		pts = capture.get_landmarks()
		if pts then
			for i=1,#pts do
				pts[i][3] = 0.
			end
		end
	--	table.print( pts[1] )
	else
		aaa.bdd.set_lua_cur( bdd )
		if dataset_id then
			aaa.bdd.set_point_dataset( dataset_id )
		end
		if aaa.bdd.get_point_nb() > 0 then
			pts = aaa.bdd.get_points()
	--		table.print( pts[1] )
	--		aaa.show( pts, "bdd" )
	--		aaa.show( bdd.." "..pts )
		end
	end
	if pts then
		local f = self.__inter_factor or 1
		local old = self:get_pts()
		if f~=1 and #old>0 then
			--self:print( #old, #pts )
			V3.interpolate_table_check_for_nan( old, pts, f )
			self:set_pts( old )
		else
			self:set_pts( pts )
		end
	else
		self:print( "no data yet" )
		--we keep old data for now
	end
	return pts
end
function POINT_LIST:build_one( src, id, gene )
	local t = type(gene)
	--self:print( t )
	local v = { 0, 0, 0 }
	if t == "table" then
		local how = gene[1]
		if how == "inter" then V3.interpolate( v, src:get_pt(gene[2]), src:get_pt(gene[3]), gene[4] )
		end
	else
		v = src:get_pt(gene)
	end
	self:set_pt( id, v )
end
function POINT_LIST:build( src, gene )
	local nb = #gene
	self:set_size( nb )
	for id=1,nb do self:build_one( src, id, gene[id] ) end
end

function POINT_LIST:draw_line_default( a, b, ph_begin, ph_end )
	if ph_begin < ph_end then
		local ax, ay = a[1], a[2]
		local bx, by = b[1], b[2]
		if ph_begin > 0 then
			ax = interpolate( ax, bx, ph_begin )
			ay = interpolate( ay, by, ph_begin )
		end
		if ph_end < 1 then
			bx = interpolate( a[1], bx, ph_end )
			by = interpolate( a[2], by, ph_end )
		end
		aaa.draw_line( ax, ay, bx, by )
	end
end
function POINT_LIST:__get_fn_line( target, method )
	if target then		return target, target[method]
	else				return self, POINT_LIST.draw_line_default
	end
end

--todo we draw too much eventually
function POINT_LIST:draw_lines_from_points( target, method, ph_begin, ph_end, tab, ... )
	local nb = tab and (#tab-1) or 0
	if nb <= 0 then return end

	--self:print( "nb "..nb )
	local ph_b = ph_begin * nb
	local ph_e = ph_end * nb
	local fn
	target, fn = self:__get_fn_line( target, method )
	for id=1,nb do
		--self:print( "draw_lines_table() "..id.." "..tab[id].." "..tab[id+1] )
		fn( target, tab[id], tab[id+1], clamp_01( ph_b -id + 1), clamp_01( ph_e - id + 1 ), ... )
	end
end
function POINT_LIST:draw_lines_begin_end( target, method, ph_begin, ph_end, start, stop, b_loop, ... )
	local pts = {}
	for id=start,stop do	table.insert( pts, self:get_pt(id)		)	end
	if b_loop then 			table.insert( pts, self:get_pt(start)	)	end
	self:draw_lines_from_points( target, method, ph_begin, ph_end, pts, ... )
end
function POINT_LIST:draw_lines_table( target, method, ph_begin, ph_end, tab, ... )
	local nb = tab and #tab or 0
	if nb > 0 then
		local pts = {}
		--self:print( "nb "..nb )
		for id=1,nb do
			table.insert( pts, self:get_pt(tab[id]) )
		end
		self:draw_lines_from_points( target, method, ph_begin, ph_end, pts, ... )
	end
end

function POINT_LIST:process_points_with_method_range( target, method, start, stop, ... )
	local fn = target[method]
	for id=start,stop do fn( target, id, self:get_pt(id), ... ) end
end
function POINT_LIST:process_points_with_method( target, method, ... )
	self:process_points_with_method_range( target, method, 1, self:get_size(), ... )
end
function POINT_LIST:draw_lines_random_with_method( nb, seed, target, method, ... )
	if not target then
		target = self
		method = "draw_line_default"
	end
	local fn = target[method]
	local nb_pt = self:get_size()
	if seed then math.randomseed(seed) end
	for i=1,nb do
		local id1 = math.random( nb_pt )
		local id2 = math.random( nb_pt )
		fn( target, self:get_pt(id1), self:get_pt(id2), ... )
	end
end

function POINT_LIST:process_points_with_fn_range( fn, start, stop )
	for id=start,stop do fn( id, self:get_pt(id) ) end
end
function POINT_LIST:process_points_with_fn( fn )
	self:process_points_with_fn_range( fn, 1, self:get_size() )
end

function POINT_LIST:mesure_seg_list( sl )
	--self:print( nb)
	local d = 0
	for i = 1, #sl-1 do
		d = d + self:get_dist( sl[i], sl[i+1] )
	end
	return d
end

-- CATMULL ROM
function POINT_LIST:compute_catmull_rom( pt_by_seg )
	--local nb = tab and (#tab-1) or 0
	--if nb <= 0 then return end

	local nb = self:get_size()
	local pl = POINT_LIST:create_nb( (nb-3) * pt_by_seg + 1 )
	--local pl = POINT_LIST:create()
	--self:print( "nb "..nb )
	local a,b,c,d
	b,c,d = self:get_pt(1), self:get_pt(2), self:get_pt(3)
	local catmull_rom = aaa.math.do_catmull_rom_xyz
	local dph = 1./pt_by_seg
	local x,y,z
	local i = 1
	for id=2,nb-2 do
		a,b,c,d = b,c,d,self:get_pt(id+2)

		local ph = 0
		x,y,z = catmull_rom( ph, a[1],a[2],a[3], b[1],b[2],b[3], c[1],c[2],c[3], d[1],d[2],d[3] )
		pl:set_pt_xyz( i, x,y,z )
		i = i + 1
		local pt = self.__pts[id]
		for id=2,pt_by_seg do
			ph = ph + dph
			x,y,z = catmull_rom( ph )
			pl:set_pt_xyz( i, x,y,z )
			i = i + 1
		end
	end
	x,y,z = catmull_rom( 1 )
	pl:set_pt_xyz( i, x,y,z )
	return pl
end
function POINT_LIST:create_catmull_rom_points( segment_nb )
	local x,y,z
	x,y,z = self:get_sym_xyz( 2, 3 )
	self:set_pt_xyz( 1, x,y,z )

	local nb = self:get_size()
	x,y,z = self:get_sym_xyz( nb-1, nb-2 )
	self:set_pt_xyz( nb, x,y,z )
	--self:process_points_with_fn( fn )

	return self:compute_catmull_rom( segment_nb )
end
-- remove becaise buggy
-- function POINT_LIST:draw_catmull_rom_points_with_fn( fn )
-- 	local pl = self:create_catmull_rom_points( segment_nb )
-- 	pl:process_points_with_fn( fn )
-- end

CLASS.DECLARE( "SEGMENT_LIST" )	--todo not sure should be a GABU_OBJ

function SEGMENT_LIST:create( name )
	local self = SEGMENT_LIST:create_instance( name )
	self.__segs = {}
	return self
end

-- use for skeleton
function SEGMENT_LIST:build_one( id, gene )
	local nb = #gene
	local start = gene[1]
	local t = type(start)
	local segs = self.__segs
	--self:print( t )
	local v
	if t == "table" then
	else
		for i = 1, nb-1 do
			segs[id] = { gene[i], gene[i+1] }
			id = id + 1
		end
	end
	return id
end
function SEGMENT_LIST:build( gene_list )
	self.__segs = {}
	local id = 1
	for i=1,#gene_list do
		id = self:build_one( id, gene_list[i] )
	end
end

function SEGMENT_LIST:set_point_list( pl )	self.__pl = pl		end
function SEGMENT_LIST:get_point_list( pl )	return self.__pl	end

function SEGMENT_LIST:draw_with_method( target, method )
	local segs = self.__segs
--	local pl = self.__pl
	local nb = #segs
	local fn = target[method]
	for i=1,nb do
		local seg = segs[i]
		fn( target, seg[1], seg[2] )
	end
end
