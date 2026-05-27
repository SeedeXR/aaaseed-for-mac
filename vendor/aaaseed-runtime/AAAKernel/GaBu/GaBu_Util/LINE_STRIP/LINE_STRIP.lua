if CLASS.DECLARE( "LINE_STRIP" ) then
	--todo have lighter object if required for speed
	LINE_STRIP._b_noname_can = true
end

function LINE_STRIP:create( name )
	local self = LINE_STRIP:create_instance( name )
	self:erase()
	return self
end
function LINE_STRIP:erase()
	self.__indexes = {}
	self.__b_closed = false
end

--function LINE_STRIP:set_point_list( point_list )	self.__point_list = point_list	end
--function LINE_STRIP:get_point_list( point_list )	return self.__point_list		end

function LINE_STRIP:set_indexes_table( indexes )	self.__indexes = indexes			end
function LINE_STRIP:get_index( index )				return self.__indexes[index]		end
function LINE_STRIP:set_indexes( ... )				self:set_indexes_table( pack(...) )	end
function LINE_STRIP:add_index( v )					table.insert( self.__indexes, v )	end
function LINE_STRIP:is_closed( b_closed )			return self.__b_closed				end
function LINE_STRIP:set_closed( b_closed )			self.__b_closed = b_closed			end


function LINE_STRIP:draw_seg_with_method( pts, target, method )
	local fn = target[method]
	if fn then
		local indexes = self.__indexes
		local nb = #indexes
		local a = pts[indexes[1]]
		local b = pts[indexes[2]]
		for i=1,nb-1 do
			fn( target, a, b )
			a = b
			b = pts[indexes[i+2]]
		end
		if self.__b_closed then	fn( target, pts[indexes[nb]], pts[indexes[1]] )	end
	else
		self:print_error( "draw_seg_with_method( "..target..", "..method.." ) : Did not find fn" )
	end
end
function LINE_STRIP:draw_pt_with_method( pts, target, method )
	local fn = target[method]
	if fn then
		local indexes = self.__indexes
		local nb = #indexes
		for i=1,nb do
			local ind = indexes[i]
			fn( target, pts[ind], ind )
		end
	else
		self:print_error( "draw_pt_with_method( "..target..", "..method.." ) : Did not find fn" )
	end
end

function LINE_STRIP:offset_on_normal( src_pts, dst_pts, off, b_relative )
	off = off or 0

	if b_relative then	off = off * .025
	else				off = off * .0065
	end

	local src_index = self.__indexes
	local nb_pt = #src_index
	if nb_pt >= 2 then
		local dst_index = dst_pts:get_nb()
		local function add_pt( pt )
			dst_pts:add_pt_xy( pt[1], pt[2], pt[3] )
			dst_index = dst_index + 1
			self:add_index( dst_index )
		end
		self.__indexes = {}

		local fn_normal
		if b_relative then	fn_normal = V2.get_normal_raw
		else				fn_normal = V2.get_normal_scaled
		end
		local mid = {}
		local a = src_pts:get_pt( src_index[1] )
		local b = src_pts:get_pt( src_index[2] )
		--if b then
			local n = fn_normal( b,a, off )
			add_pt( V2.get_add( a, n ) )
		--end
		for src_i = 3,nb_pt-1 do
			local c = src_pts:get_pt( src_index[src_i] )
			local n = fn_normal( c,a, off )
			add_pt( V2.get_add( b, n ) )
			a,b = b,c
		end
		local n = fn_normal( b,a, off )
		add_pt( V2.get_add( b, n ) )
	end
end

--overwitten in axi_gene
function LINE_STRIP:deform_on_normal( src_pts, dst_pts, b_relative, tab, deform_method, freq, harm_nb )
	local fn = tab[deform_method]

	local src_index = self.__indexes
	local nb_pt = #src_index
	--todo deal with close open ...
	if nb_pt >= 2 then
		local dst_index = dst_pts:get_nb()
		local function add_pt( pt )
			dst_pts:add_pt_xy( pt[1],pt[2],pt[3] )
			dst_index = dst_index + 1
			self:add_index( dst_index )
		end
		self.__indexes = {}

		local fn_normal
		if b_relative then	fn_normal = V2.get_normal_raw
		else				fn_normal = V2.get_normal_scaled
		end

		local a = src_pts:get_pt( src_index[1] )
		local b = src_pts:get_pt( src_index[2] )
		local off = fn( tab, a[1],a[2],a[3], freq, harm_nb )
		local n = fn_normal( b,a, off )
		add_pt( V2.get_add( a, n ) )
		for src_i = 2,nb_pt do
			b = src_pts:get_pt( src_index[src_i] )
			local off = fn( tab, b[1],b[2],b[3], freq, harm_nb )
			local n = fn_normal( b,a, off )
			add_pt( V2.get_add( b, n ) )
			a = b
		end
	end
end

function LINE_STRIP:convert_to_midpoint( src_pts, dst_pts, interpolator )
	local src_index = self.__indexes
	local nb_pt = #src_index
	if nb_pt >= 2 then
		local dst_index = dst_pts:get_nb()
		local function add_pt( pt )
			dst_pts:add_pt_xy( pt[1], pt[2], pt[3] )
			dst_index = dst_index + 1
			self:add_index( dst_index )
		end
		self.__indexes = {}

		local mid = {}
		local a = src_pts:get_pt( src_index[1] )
		local b = src_pts:get_pt( src_index[2] )

		if not self:is_closed() then add_pt(a) end
		for i = 1,nb_pt-1 do
			V3.interpolate( mid, a, b, interpolator )
			add_pt( mid )
			a = b
			b = src_pts:get_pt( src_index[i+2] )
		end
		if self:is_closed() then
			b = src_pts:get_pt( src_index[1] )
			V3.interpolate( mid, a, b, interpolator )
			add_pt( mid )
		else
			add_pt(a)
		end

	end
end

function LINE_STRIP:__convert_to_catmull_rom_low( src_pts, dst_pts, nb_seg, interpolator, inter_b )
	local src_index = self.__indexes
	local nb_pt = #src_index
	if nb_pt <= 3 then return end

	local dst_index = dst_pts:get_nb()
	local function add( ... )
		local x,y,z = aaa.math.do_catmull_rom_xyz(...)
		dst_pts:add_pt_xy( x,y,z )
		dst_index = dst_index + 1
		self:add_index( dst_index )
	end

	local a,b,c,d
	local function add_seg()
		local ai = V3.get_interpolate( b, a, interpolator )
		local di = V3.get_interpolate( d, c, inter_b )
		add( 0, ai[1],ai[2],ai[3], b[1],b[2],b[3], c[1],c[2],c[3], di[1],di[2],di[3] )
		for i = 1,nb_seg-1 do
			add( i/nb_seg )
		end
	end
	self.__indexes = {}

	b = src_pts:get_pt( src_index[1] )
	c = src_pts:get_pt( src_index[2] )
	d = src_pts:get_pt( src_index[3] )
	if self:is_closed() then
		a = src_pts:get_pt( src_index[nb_pt] )
	else
		a = V3.get_interpolate( b, c, -1 )
	end

	for src_i = 1,nb_pt-2 do
		add_seg()
		a,b,c = b,c,d
		d = src_pts:get_pt( src_index[src_i+3] )
	end

	if self:is_closed() then
		d = src_pts:get_pt( src_index[1] )
		add_seg()
		a,b,c = b,c,d
		d = src_pts:get_pt( src_index[2] )
		add_seg()

	else
		d = V3.get_interpolate( c, b, -1 )
		add_seg()
	end
	add( 1 )

end
function LINE_STRIP:convert_to_catmull_rom( src_pts, dst_pts, nb_seg, interpolator, asymetrisator )
	local inter_b = interpolate( 1-interpolator, interpolator, asymetrisator )
	self:__convert_to_catmull_rom_low( src_pts, dst_pts, nb_seg, interpolator, inter_b )
end

function LINE_STRIP:convert_to_bezier_by3( src_pts, dst_pts, nb_seg, interpolator, asymetrisator )
	local src_index = self.__indexes
	local nb_pt = #src_index

	interpolator = interpolator or .5
	local inter_b = interpolate( interpolator, 1-interpolator, asymetrisator )
	nb_seg = nb_seg or 8
	if nb_pt >= 3 then
		local dst_index = dst_pts:get_nb()
		local function add_pt( x,y,z )
			dst_pts:add_pt_xy( x,y,z )
			dst_index = dst_index + 1
			self:add_index( dst_index )
		end
		local convert = aaa.math.do_bezier_xyz
		self.__indexes = {}
		local x,y,z

		local p1,p2,p3
		local function add_seg()
			local a = V3.get_middle( p1, p2 )
			local d = V3.get_middle( p3, p2 )

			local b = V3.get_interpolate( a, p2, interpolator )
			local c = V3.get_interpolate( d, p2, inter_b )

			--add_pt( a[1],a[2],a[3] )
			--add_pt( b[1],b[2],b[3] )

			--add_pt( mid[1],mid[2],mid[3] )
			--add_pt( d[1],d[2],d[3] )

			x,y,z	= convert( 0, a[1],a[2],a[3], b[1],b[2],b[3], c[1],c[2],c[3], d[1],d[2],d[3] )
			add_pt( x,y,z )
			for i = 1,nb_seg-1 do
				local inter = i/nb_seg
				x,y,z = convert( inter )
				add_pt( x,y,z )
			end
		end

		if self:is_closed() then
			p1 = src_pts:get_pt( src_index[nb_pt] )
			p2 = src_pts:get_pt( src_index[1] )
			p3 = src_pts:get_pt( src_index[2] )
			for src_i = 3,nb_pt+1 do
				add_seg()
				p1,p2 = p2,p3
				p3 = src_pts:get_pt( src_index[src_i] )
			end
			p3 = src_pts:get_pt( src_index[1] )
			add_seg()
			x,y,z	= convert( 1 )
			add_pt( x,y,z )
		else
			p1	= src_pts:get_pt( src_index[1] )
			p2	= src_pts:get_pt( src_index[2] )
			p3  = src_pts:get_pt( src_index[3] )
			for src_i = 4,nb_pt+1 do
				add_seg()
				p1,p2 = p2,p3
				p3 = src_pts:get_pt( src_index[src_i] )
			end
			x,y,z	= convert( 1 )
			add_pt( x,y,z )
		end
	end
end

function LINE_STRIP:convert_to_bezier( src_pts, dst_pts, nb_seg, interpolator )
	local src_index = self.__indexes
	local nb_pt = #src_index
	if nb_pt >= 4 then
		local dst_index = dst_pts:get_nb()
		local function add_pt( x,y,z )
			dst_pts:add_pt_xy( x,y,z )
			dst_index = dst_index + 1
			self:add_index( dst_index )
		end
		local convert = aaa.math.do_bezier_xyz
		self.__indexes = {}
		local x,y,z
		local a = src_pts:get_pt( src_index[1] )
		local b = src_pts:get_pt( src_index[2] )
		local c = src_pts:get_pt( src_index[3] )
		local d = src_pts:get_pt( src_index[4] )
		local nb_seg = self:is_closed() and nb_pt or (nb_pt-3)
		for src_i = 1,nb_seg do
			x,y,z	= convert( 1/3, a[1],a[2],a[3], b[1],b[2],b[3], c[1],c[2],c[3], d[1],d[2],d[3] )
			add_pt( x,y,z )
			for i = 1,nb_seg-1 do
				local inter = 1/3 + i/(nb_seg*3)
				x,y,z	= convert( inter )
				add_pt( x,y,z )
			end
			a = b
			b = c
			c = d
			local n = src_i+4
			if n > nb_pt then n = n - nb_pt end
			d = src_pts:get_pt( src_index[n] )
		end
		x,y,z	= convert( 2/3 )
		add_pt( x,y,z )
	end
end

function LINE_STRIP:convert_to_try( src_pts, dst_pts, nb_seg, interpolator )
	local src_index = self.__indexes
	local nb_pt = #src_index
	if nb_pt >= 4 then
		local dst_index = dst_pts:get_nb()
		local function add_pt( x,y,z )
			dst_pts:add_pt_xy( x,y,z )
			dst_index = dst_index + 1
			self:add_index( dst_index )
		end
		local convert = aaa.math.do_bezier_xyz
		self.__indexes = {}
		local x,y,z
		for src_i = 1,nb_pt-3 do
			local a = src_pts:get_pt( src_index[src_i  ] )
			local b = src_pts:get_pt( src_index[src_i+1] )
			local c = src_pts:get_pt( src_index[src_i+2] )
			local d = src_pts:get_pt( src_index[src_i+3] )

			a = V3.get_middle( a, b )
			b = V3.get_middle( b, c )
			d = V3.get_middle( c, d )

			a = V3.get_middle( a, b )
			d = V3.get_middle( b, d )

			c = V3.get_middle( b, d )
			b = V3.get_middle( a, b )

			x,y,z	= convert( 0, a[1],a[2],a[3], b[1],b[2],b[3], c[1],c[2],c[3], d[1],d[2],d[3] )
			add_pt( x,y,z )
			for i = 1,nb_seg-1 do
				local inter = i/nb_seg
				x,y,z	= convert( inter )
				add_pt( x,y,z )
			end
		end
		x,y,z	= convert( 1 )
		add_pt( x,y,z )
	end
end

--todo move in table
function LINE_STRIP:__decimate_index_with_fn( rnd_fn, src_index, dst_index, seed, keep_min, keep_max, skip_min, skip_max )
	if seed then math.randomseed(seed) end

	local nb = #src_index
	local keep_range = keep_max - keep_min
	local skip_range = skip_max - skip_min
	local i = 1
	repeat
		local keep = math.min( keep_min + rnd_fn(keep_range), nb-i+1 )
		local i_end = i + keep
		for j = i,i_end-1 do
			table.insert( dst_index, src_index[j] )
		end
		i = i_end
		local skip = math.min( skip_min + rnd_fn(skip_range), nb-i+1 )
		i = i + skip
	until i>=nb
end

function LINE_STRIP:decimate_index( ... )	self:__decimate_index_with_fn( math.random, ... )	end
function LINE_STRIP:decimate_index_gauss( ... )	self:__decimate_index_with_fn( math.gauss, ... )	end


CLASS.DECLARE( "LINE_STRIP_TABLE" )

function LINE_STRIP_TABLE:create( name )
	local self = LINE_STRIP_TABLE:create_instance( name )
	self:erase()
	return self
end
function LINE_STRIP_TABLE:erase()
	self.__strips = {}
end
function LINE_STRIP_TABLE:set_point_list( point_list )	self.__point_list = point_list	end
function LINE_STRIP_TABLE:get_point_list( point_list )	return self.__point_list		end

function LINE_STRIP_TABLE:init_with_empty_point_list()
	self:erase()
	self:set_point_list( POINT_LIST:create() )
	return self
end

function LINE_STRIP_TABLE:add_line_strip( line_strip )
	table.insert( self.__strips, line_strip )
	self.__strip_cur = line_strip
	return line_strip
end

function LINE_STRIP_TABLE:add_new()
	return self:add_line_strip( LINE_STRIP:create() )
end

function LINE_STRIP_TABLE:add_pt_xyz( x, y, z )
	local pl = self.__point_list
	local id = pl:add_pt_xyz( x, y, z )
	self.__strip_cur:add_index( id )
end

function LINE_STRIP_TABLE:set_closed( b )
	self.__strip_cur:set_closed( b )
end
--function LINE_STRIP_TABLE:add_strip( ... )
--	strip.set_indexes(...)
--	return strip
--end
-- function LINE_STRIP_TABLE:add_strip_closed( ... )
-- 	self:add_strip(...):set_closed( true )
-- end

function LINE_STRIP_TABLE:__draw_line( a, b )
	if a and b then
		gol.draw_line_strip_2d( a[1], a[2], b[1], b[2] )
		--gol.draw_points_2d(  a[1], a[2] )
	end
end
function LINE_STRIP_TABLE:__draw_point( a, ia )
	if a then
		gol.draw_points_2d(  a[1], a[2] )
	end
end
function LINE_STRIP_TABLE:__draw_point_colored( a, ia )
	if a then
		ia = (ia- 1) % 8
		if		ia<=3 	then
			if		ia<=1 	then
				if	ia==0 	then gol.color_white() else gol.color_red() end
			else
				if	ia==2 	then gol.color_green() else gol.color_blue() end
			end
		else
			if		ia<=5 	then
				if	ia==4 	then gol.color_white() else gol.color_magenta() end
			else
				if	ia==6 	then gol.color_yellow() else gol.color_cyan() end
			end
		end
		gol.draw_points_2d(  a[1], a[2] )
	end
end

function LINE_STRIP_TABLE:__draw_pt_gene( method_name )
	local pl = self.__point_list
	if pl then	--self:print( "draw using "..method_name )
		local pts = pl:get_pts()
		for _,strip in IPAIRS(self.__strips) do
			strip:draw_pt_with_method( pts, self, method_name  )
		end
	end
end
function LINE_STRIP_TABLE:__draw_seg_gene( method_name )
	--self:print( "draw using "..method_name )
	local pl = self.__point_list
	if pl then
		local pts = pl:get_pts()
		for _,strip in IPAIRS(self.__strips) do
			strip:draw_seg_with_method( pts, self, method_name  )
		end
	end
end
--todo refine
function LINE_STRIP_TABLE:draw_point()					self:__draw_pt_gene( "__draw_point"  )			end
function LINE_STRIP_TABLE:draw_point_colored()			self:__draw_pt_gene( "__draw_point_colored"  )	end

function LINE_STRIP_TABLE:draw()						self:__draw_seg_gene( "__draw_line"  )			end


function LINE_STRIP_TABLE:__transform_with_method( method, ... )	--src_pts, dst_pts )
	local src_pts = self:get_point_list()
	local dst_pts = POINT_LIST:create()
	local fn = LINE_STRIP[method]
	--self:print( "LINE_STRIP_TABLE:draw()" )
	for _,strip in IPAIRS(self.__strips) do
		--self:print( "LINE_STRIP_TABLE:draw() "..strip )
		fn( strip, src_pts, dst_pts, ... )
	end
	self:set_point_list( dst_pts )
end
function LINE_STRIP_TABLE:offset_on_normal(...)			self:__transform_with_method( "offset_on_normal",		... )	end
function LINE_STRIP_TABLE:deform_on_normal(...)			self:__transform_with_method( "deform_on_normal",		... )	end
function LINE_STRIP_TABLE:convert_to_midpoint(...)		self:__transform_with_method( "convert_to_midpoint",	... )	end
function LINE_STRIP_TABLE:convert_to_catmull_rom(...)	self:__transform_with_method( "convert_to_catmull_rom",	... )	end
function LINE_STRIP_TABLE:convert_to_bezier(...)		self:__transform_with_method( "convert_to_bezier",		... )	end
function LINE_STRIP_TABLE:convert_to_bezier_by3(...)	self:__transform_with_method( "convert_to_bezier_by3",	... )	end
function LINE_STRIP_TABLE:convert_to_try(...)			self:__transform_with_method( "convert_to_try",			... )	end


function LINE_STRIP_TABLE:__transform_index_with_method( method, ... )	--src_pts, dst_pts )
	local fn = LINE_STRIP[method]
	--self:print( "LINE_STRIP_TABLE:draw()" )
	for _,strip in IPAIRS(self.__strips) do
		local index = {}
		fn( strip, strip.__indexes, index, ... )
		strip.__indexes = index
	end
end
function LINE_STRIP_TABLE:decimate_point(...)			self:__transform_index_with_method( "decimate_index",		... )	end
function LINE_STRIP_TABLE:decimate_point_gauss(...)		self:__transform_index_with_method( "decimate_index_gauss",	... )	end


local function isnan(n) return n ~= n end

function LINE_STRIP_TABLE:draw_order()
	local pl = self.__point_list
	--self:print( "LINE_STRIP_TABLE:draw()" )
	local i = 0
	local nan_nb = 0
	gol.begin_line_strip()
	for _,strip in IPAIRS(self.__strips) do
		if strip.__b_invalid then
			--self:print( "b_invalid" )
			--gol.do_end()
			--gol.begin_line_strip()
		else
			local index = strip:get_index( 1 )
			--self:print(index)
			local pt = pl:get_pt(index)
			if pt then
				if isnan(pt[1]) or isnan(pt[2]) then
					nan_nb = nan_nb + 1
					--self:print( "point with nan coor "..pt[1]..", "..pt[2] )
				else
					gol.vertex( pt[1], pt[2], 0 )
					--self:print( i.." --> "..pt[1]..", "..pt[1] )
					i = i + 1
					if i==4096 then
						i=0
						gol.do_end()
						gol.begin_line_strip()
						gol.vertex( pt[1], pt[2], 0 )
					end
				end
			end
		end
	end
	gol.do_end()
	if nan_nb>0 then
		self:print_error( nan_nb.." points with nan coor" )
	end
end

function LINE_STRIP_TABLE:plot( target, method )
	local fn = target[method]
	for _,strip in IPAIRS(self.__strips) do
		fn( target, strip )
	end
end

function LINE_STRIP_TABLE:sort_xy( )
	local pl = self.__point_list

	local function sort( a, b )
		local pa = pl:get_pt( a.__indexes[1] )
		local pb = pl:get_pt( b.__indexes[1] )
		local f = 4
		local xa = math.floor( pa[1] * f )
		local xb = math.floor( pb[1] * f )
		local ya = math.floor( pa[2] * f*2 )
		local yb = math.floor( pb[2] * f*2 )
		return xa == xb and ya < yb or xa < xb
	end

	table.sort( self.__strips, sort )
end

function LINE_STRIP_TABLE:sort_hilbert( )
	local pl = self.__point_list

	local nb = 1024*16
	local size = 8
	local hsize = size / 2
	local f = nb / size

	for _,strip in IPAIRS(self.__strips) do
		local pt = pl:get_pt( strip.__indexes[1] )
		local x,y = pt[1], pt[2]
		local x = math.floor( (x + hsize) * f )
		local y = math.floor( (y + hsize) * f )
		local hash = aaa.math.convert_hilbert_xy_to_d( x, y, nb )
		strip.__pos_hash = hash
		--self:print( x.." "..y.." "..strip.__pos_hash )
		--if (hash % 31) < 7 then
			--self:print( "hash "..hash.." --* make it invalid " )
		--	strip.__b_invalid = true
		--end
	end

	local function sort( a, b )
		return a.__pos_hash < b.__pos_hash
	end

	local function sort_old( a, b )
		local pa = pl:get_pt( a.__indexes[1] )
		local pb = pl:get_pt( b.__indexes[1] )

		local nb = 256
		local f = nb * .125

		local xa = math.floor( (pa[1] + 4.) * f )
		local ya = math.floor( (pa[2] + 4.) * f )
		local da = aaa.math.convert_hilbert_xy_to_d( xa, ya, nb )

		local xb = math.floor( (pb[1] + 4.) * f )
		local yb = math.floor( (pb[2] + 4.) * f )
		local db = aaa.math.convert_hilbert_xy_to_d( xb, yb, nb )

		return da < db
	end

	table.sort( self.__strips, sort )
end

function LINE_STRIP_TABLE:decimate_random_one_by_one( seed, th )
	math.randomseed(seed)

	local dst = {}
	for _,strip in IPAIRS(self.__strips) do
		--local pt = pl:get_pt( strip.__indexes[1] )
		--strip.__pos_hash = hash
		if math.random() > th then
			table.insert( dst, strip )
		end
	end
	self.__strips = dst
end

function LINE_STRIP_TABLE:decimate_random_section( seed, keep_min, keep_max, skip_min, skip_max )
	math.randomseed(seed)

	local dst = {}
	local keep_range = keep_max - keep_min
	local skip_range = skip_max - skip_min
	local strips = self.__strips
	local i = 1
	local nb = #strips
	repeat
			local keep = math.min( keep_min + math.random(keep_range), nb-i+1 )
			local i_end = i + keep
			for j = i,i_end-1 do
				table.insert( dst, strips[j] )
			end
			i = i_end
			local skip = math.min( skip_min + math.random(skip_range), nb-i+1 )
			i = i + skip
	until i>=nb

	self.__strips = dst
end

function math.gauss(v)
	local t = math.random(v) + math.random(v) + math.random(v)
	return math.floor ( t/3 )
end
function LINE_STRIP_TABLE:decimate_gauss_section( seed, keep_min, keep_max, skip_min, skip_max )
	math.randomseed(seed)

	local dst = {}
	local keep_range = keep_max - keep_min
	local skip_range = skip_max - skip_min
	local strips = self.__strips
	local i = 1
	local nb = #strips
	repeat
			local keep = math.min( keep_min + math.gauss(keep_range), nb-i+1 )
			local i_end = i + keep
			for j = i,i_end-1 do
				table.insert( dst, strips[j] )
			end
			i = i_end
			local skip = math.min( skip_min + math.gauss(skip_range), nb-i+1 )
			i = i + skip
	until i>=nb

	self.__strips = dst
end

