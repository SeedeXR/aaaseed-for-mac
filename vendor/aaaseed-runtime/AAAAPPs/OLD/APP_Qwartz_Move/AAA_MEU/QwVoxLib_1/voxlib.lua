CLASS.DECLARE( "VOX", nil, {
	size 			=	1,
	__b_done		=	false,
	} )

function VOX:create( name )
	local self = VOX:create_instance( name )
	self:init()
	return self
end

function VOX:init()
	self:init_digits()
end
function VOX:do_rnd( t, ... )
	if t == 0 then
		local nb = { ... }
		local sels = FOX.def_ui.rnd_sel
		local rnd = {}
		for i = 1,math.min(#nb,4) do
			local n = nb[i]
			if n then
				local val = sels[i]:get_value()
				self:print( "random["..i.."] = "..val )
				if val==0 then
					rnd[i] = math.random(n)
				else
					rnd[i] = math.min( n, val )
				end
			end
		end
		self.rnd = rnd
	end
	return unpack( self.rnd )
end

function VOX.update_def_color_with_bu( color_1, color_2 )
	VOX.color_1 = { color_1:get_rgba() }
	VOX.color_2 = { color_2:get_rgba() }
end
function VOX:set_color_def_by_id( id1, id2 )
	app:set_color_fox( id1, id2 )
end
function VOX:set_color( index, r, g, b, a )
	aaa.bdd.set_color( index, r, g, b, a )
end
function VOX:set_color_rgb_both( r, g, b, a )
	self:set_color(	1,	r,g,b, 0 )
	self:set_color(	2,	r,g,b, a or 1 )
	--self:print( r,g,b, a )
end

function VOX:set_color_def()
	if self.color_1 then
		--self.print( unpack(self.color_1) )
		self:set_color(	2,	unpack(self.color_1) )
		self:set_color(	1,	unpack(self.color_2) )
	else
		self:set_color(	1,	1,0,1,0 )
		self:set_color(	2,	1,1,1,1 )
	end
end
function VOX:set_color_white_all()
	self:set_color(	1,	1,1,1,1 )
	self:set_color(	2,	1,1,1,1 )
end
function VOX:set_color_def_flip()
	self:set_color(	2,	1,0,1,0 )
	self:set_color(	1,	1,1,1,1 )
end

function VOX:set_meu_voxel( meu )		self.meu_voxel		= meu	end
function VOX:set_meu_mocap( meu )		self.meu_mocap		= meu	end
function VOX:set_meu_particle( meu )	self.meu_particle	= meu	end
function VOX:set_meu_text( meu )		self.meu_text		= meu	end

function VOX:set_before_render()
	self.meu_voxel:set_out_use_alpha( true )
	self.meu_voxel:set_blend_mode_add()
	self.meu_voxel:set_mood( app.meu_fox:get_bu_value( "Mood" ) )
end

function VOX:set_mocap( id )			self.meu_mocap:set_selector_value( id )		end
function VOX:set_particle( id )			self.meu_particle:set_selector_value( id )	end
function VOX:set_text_by_id( id )		self.meu_text:set_text_by_id( id )			end
function VOX:set_text( ... )			self.meu_text:set_text( ... )				end
function VOX:is_text_done()				self.meu_text:is_done()						end

function VOX:is_done()					return self.__b_done					end
function VOX:set_done( b )				self.__b_done = b 						end
------------------------------
--	PLANE
function VOX:render_plane( axe, p_begin, p_end, dp, dpi, t, r, g, b )
	local bdd	=	aaa.bdd
	v = p_begin + (p_end-p_begin) * math.fmod( aaa.time.t/t, 1. )
	bdd.set_color( 1, 0, 0, 0 )
	bdd.set_color( 2, r, g, b )
	bdd.render_plane_axe( axe, v-dp, v-dpi )
	bdd.render_plane_axe( axe, v+dp, v+dpi )
	bdd.set_color( 1, r, g, b )
	bdd.render_plane_axe( axe, v-dpi, v+dpi )
end

-----------------------------
--	SPIRAL
function VOX:render_spiral( t )
--	t =  t or aaa.time.t
	local id_main, id_col = self:do_rnd( t, 2, 3 )
	--if t==0 then
	--	self:set_color_def_by_id( 7, 15 )
	--end
	self:set_lissa_color( id_main, id_col )
	--self:set_color_def()	--self:print( "render_spiral() "..t )
	local bdd	=	aaa.bdd
	local v = 5 - 10 * math.fmod( t/5, 1. )
	local nb = 32
	local dv = -8 / nb
	for i=1,nb do
		local s = ((1+math.sin(t)) * .5) * self.size
		local r = 1.7 	* s
		local ph = v*1.5 + t*-3
		local x = r * math.cos( ph )
		local z = r * math.sin( ph )
		--self:print( i.." : "..x.." "..v.." "..z.." "..(2.+s*1).." "..5 )
		bdd.render_boule(		x,v,z, 2.+s*1, .1 )
		v = v + dv
		if v<-5 then
			v = v + 10
		end
	end
end
function VOX:rot( d, a )
	a =  a * math.pi2
	return d*math.cos(a), d*math.sin(a)
end
function VOX:render_spiral_math_low( t, nb, size, tour, speed )
	local r  = 2.6
	for i=1,nb do
		local x, z = self:rot( r, tour*i/nb - t * speed )
		local y = -4 + 8 * i/nb
		aaa.bdd.render_segment(	x, y, z,	-x, y, -z,	size	, 0.5 )
	end
end

function VOX:render_spiral_math( t )
	local rnd = self:do_rnd( t, 3 )
	if t==0 then
		self:set_color_def_by_id( 7, 15 )
	end
	self:set_color_def()
	--rnd = 4
	if rnd == 1 then
		self:set_color_def()
		self:render_spiral_math_low( t, 16, 1.5, .5, .2 )
	elseif rnd == 2 then
		self:set_color_def()
		self:render_spiral_math_low( t, 16, 1,.5, -.1 )
	elseif rnd == 3 then
		self:set_color_def()
		self:render_spiral_math_low( t, 40, .8,2, .4 )
	elseif rnd == 4 then
		self:set_color_def()
		self:render_spiral_math_low( t, 8, 1,.5, .2 )
	elseif rnd == 5 then
		self:set_color_def()
		self:render_spiral_math_low( t, 40, .5,7, .2 )
	else
		self:set_color_def()
		self:render_spiral_math_low( t, 40, 1,.5, .1 )
	end
end

-----------------------
--	ADN
function VOX:render_adn_low( t, size,  size_int )
	self:set_color_def()
	local 	bdd		=	aaa.bdd

	local radius = 2.3
	local function make_xz( y )
		y = y
		local ph = y * -.45
		local x = radius * math.cos( ph )
		local z = radius * math.sin( ph )
		return x,z
	end

	local s1,s2 = size, size_int
	local step = 1.
	local ya = -5-step
	local xa,za = make_xz( ya )

	bdd.render_segment(	xa,ya,za, -xa,ya,-za,	s1, s2 )

	local off = math.fmod( t, step*2 )
	local i = 0
	for yb=ya+off,5,step do
		local xb,zb  = make_xz( yb + t )
		if i%2 ==1 then
			bdd.render_segment(	 xb, yb, zb,	-xb, yb,-zb,	s1, s2	)
		end
		i = i + 1
		bdd.render_segment(	 xa, ya, za,	 xb, yb, zb,	s1, s2	)
		bdd.render_segment(	-xa, ya,-za,	-xb, yb,-zb,	s1, s2	)
		xa,ya,za = xb,yb,zb
	end
end
function VOX:render_adn( t )
	if t==0 then
		self:set_color_def_by_id( 7, 15 )
	end
	self:render_adn_low( t, 1.5, .5 )
end
---------------------------
--	SUN
function VOX:render_one_sun( t )
	self:set_color_def_by_id( 7, 15 )
	self:set_color_def()
	self.meu_voxel:set_blend_mode_add()
	--self:set_color_white_all()
	local min,max = -1,16
	local function draw_sun( x, y, z, time )
		local size = math.fmod( time, 1 )
		size = ( min + size * max )
		aaa.bdd.render_sphere(	x,	y,	z,	size,	2,  .5	)
	end
	draw_sun(	0, 0, 0,	t		)
end

function VOX:render_suns( t )
	if t == 0 then
		self:set_color_def_by_id( 7, 15 )
	end
	self:set_color_def()
	--self:set_color_white_all()
	local min,max = -1,16
	local function draw_sun( x, y, z, time )
		local size = math.fmod( time, 1 )
		size = ( min + size * max )
		aaa.bdd.render_sphere(	x,	y,	z,	size,	2,  .5	)
	end

	local function draw_sun_multiple( f, nb, alt, cx, cy, cz )
		if cy ~= 0 then
			min, max = -1, 16 + math.abs( cy ) * 1.5
		end
		t = t / f
		for i = 1, nb do
			if alt == 2 then cy = -cy end
			draw_sun(	cx, cy, cz,	t + i/nb	)
		end
	end
	local nb, pos, f, alt = self:do_rnd( t, 6, 7, 3, 2 )

	f = f * 5
	nb = nb + 1
	local y, z
	if pos==1 or pos==7 then
		y = (pos-4) * 1.
		z = 3
	else
		y = clamp( (pos-4) * 3, -6, 6 )
		z = 0
	end

	if not( pos==2 or pos==6) and nb > 4 then
		nb = nb - 3
	end
	draw_sun_multiple( f, nb, alt,		0,y,z	)
end

function VOX:render_suns_70( t )
	if t==0 then
		self:set_color_def_by_id( 7, 15 )
	end
	self:set_color_def()
	local function draw_sun( x, y, z, time, st, to )
		local size = math.fmod( time, 1 )
		--self:print( s )
		size = ( 0 + size  * 16 )
		aaa.bdd.render_sphere(	x,	y,	z,		size,	1,  0 	)
	end

	local f = .1
	local tt = t*f
	local d = 2.3
	local s = (.5 + .5*math.cos(t*.2))
	--self:print( s )
	local h = 1.5
	local oy = 0
	local nb = 3
	for i=1,nb do
		local x,y
		local a = i/nb --+t*.1
		x,y = self:rot( d, a )
		draw_sun(	x,	oy,		y,		s )
		draw_sun(	x,	oy+2*h,	y,		s )
		draw_sun(	x,	oy-2*h,	y,		s )
		x,y = self:rot( d, a+.5 )
		draw_sun(	x,	oy+h,	y,		s )
		draw_sun(	x,	oy-h,	y,		s )
	end
end

function VOX:render_suns_71( t )
	if t==0 then
		self:set_color_def_by_id( 7, 15 )
	end
	self:set_color_def()
	local function draw_sun( x, y, z, size )
		aaa.bdd.render_sphere(	x,	y,	z,	size,	1,  .5 )
	end

	local f_rot = self:do_rnd( t, 4 )
	f_rot = (f_rot-2.5)*.33
	local f = .1
	local tt = t*f
	local d = 2
	local s = .5 + (.5 + .5*math.cos(t*1.)) * 2
	--self:print( s )
	local h = 2
	local oy = 0
	local nb = 3
	for i=1,nb do
		local x,y
		local a = i/nb + t*f_rot
		x,y = self:rot( d, a )
		draw_sun(	x,	oy+h,	y,	s )
				--draw_sun(	x,	oy, 	y,	s )
		--draw_sun(	x,	oy*h,	y,	s )
		--draw_sun(	x,	-oy*h,	y,	s )
		x,y = self:rot( d, a+.5 )
		draw_sun(	x,	oy-h,	y,	s )
	end
end

---------------------------
--	TORUS
function VOX:draw_torus( t, x, y, z, r, nb )
	local a = 0
	local xa, za = self:rot( r, a )
	xa, za = xa+x, za+z
	local xb, zb
	local nb = nb or 6

	for i=1,nb do
		xb, zb = self:rot( r, i/nb )
		xb, zb = xb+x, zb+z
		aaa.bdd.render_segment(	xa, y, za,	xb, y, zb,	1., .5 )
		xa, za = xb, zb
	end
end
function VOX:render_torus( t )
	if t==0 then
		self:set_color_def_by_id( 7, 15 )
	end

	local s  = 2.3
	local ft =  1
	local fs = 1
	local nb = 16
	local rnd = self:do_rnd( t, 4 )
	if rnd == 1 then
	elseif rnd == 2 then
		s = 3.3
		nb = 6
	elseif rnd == 3 then
		fs = 2
		ft = 1
		nb = 16
		s= 7
	else
		nb = 16
		s = 7
	end
	self:set_color_def()

	s =  s / 2
	for i=1,nb do
		local f = i/nb
		local y = -4 + 8*f
		local r = (1 + math.sin( -math.pi2 * i/nb * fs + t*ft ) ) * s
		self:draw_torus(	t, 0, y, 0,	r	)
	end
end

---------------------------
--	DIGIT
local DX = .5
local DY = 1.
function VOX:init_digits( dx, dy )
	local digits = {}
	dx = dx or DX
	dy = dy or DY
	self.digits_lines =
	{
		{	-dx,	-dy,	-dx,	dy	},	--	line 1
		{	dx,		-dy, 	dx,		dy	},	--	line 2
		{	-dx,	-dy,	-dx,	0	},	--	line 3
		{	dx, 	-dy,	dx,		0	},	--	line 4
		{	-dx,	0,		-dx,	dy	},	--	line 5
		{	dx, 	0,		dx,		dy	},	--	line 6
		{	-dx,	-dy,	dx,		-dy	},	--	line 7
		{	-dx,	0,		dx,		0 	},	--	line 8
		{	-dx,	dy,		dx,		dy	},	--	line 9
		{	0,		-dy, 	0,		dy	},	--	like the '1' but centered in x
	}
	self.digits =
	{
		{ 1, 2, 7, 9	},	--	0
		{ 2				},	--	1
		{ 3, 6, 7, 8, 9	},	--	2
		{ 2, 7, 8, 9	},	--	3
		{ 5, 2, 8		},	--	4
		{ 4, 5, 7, 8, 9 },	--	5
		{ 1, 4, 7, 8	},	--	6
		{ 2, 9			},	--	7
		{ 1, 2, 7, 8, 9 },	--	8
		{ 2, 5, 7, 8, 9 },	--	9
		{ 10 },				--	10 used to do a centered 1

	}
end
function VOX:draw_digit( x,y,z, i, f )
	--self:print( i )
	local bdd = aaa.bdd
	local s1,s2 = f*self.size_int, self.size_int
	local lines = self.digits_lines
	local digit = self.digits[i+1]
	for _, v in ipairs(digit) do
		local l = lines[v]
		bdd.render_segment(	x+l[1]*f,y+l[2]*f,z,	x+l[3]*f,y+l[4]*f,z,	s1, s2	)
	end
end

function VOX:draw_numbers( n )
	local z  = 2
	if n >= 10 then
		self:draw_digit( -DX*2.5,0,z, 		1,				1		)
		self:draw_digit( DX*1,0,z, 			math.fmod(n,10),	1		)
	else
		local size = 1+1*(10-n)*.17
		self:draw_digit( 0,0,z,		(n==1) and 10 or n,		size	)
	end
end
function VOX:render_countdown_segment( t )
	local bdd	=	aaa.bdd
	self:set_color_def()
	self.size_int = .6
	self:draw_numbers( 9 - math.floor( math.fmod(t,10) ) )
end

-----------------------
--	POINTS
function VOX:render_point_bdd_low( bdd_src, size )
	local	bdd	=	aaa.bdd
	local	nb	=	aaa.bdd.get_point_nb( bdd_src )
	--self:print( nb)
	local	id, x, y, z
	local	f	=	1
	local	s	=	size
	local	rc	=	.25
	local	get_point = aaa.bdd.get_point_and_id_local
	for i = 1, nb do
		x, y, z, id = get_point( bdd_src, i )
		if true or id%1 == 0 then
			--bdd.set_color( 2, 1, 0, 0 )

			bdd.render_boule(	x*f,	y*f,	z*f,	s,		 rc )
			--bdd.set_color( 2, 1, 1, 0 )
			--bdd.render_sphere(	x*f,	y*f,	z*f,	s,		rc,  0 )
			--aaa.bdd.render_boule(	x,	y+d,	z,	s*.75,	rc )
			--aaa.bdd.render_boule(	x,	y+d/2,	z,	s*.5,	rc )
			--bdd.render_segment( x,y,z, 0,0,0, 4, .8 )
		end
	end
end
function VOX:render_point_bdd( t )
	local bdd_src = self.meu_particle:get_bdd_drawn()
	local rnd, rcol1, rcol2 = self:do_rnd( t, 5, 5, 4 )
	if t==0 then
		self:set_particle( rnd )
		self.b_particle_to_init = true
		return
	end
	if not bdd_src then
		return
	end
	if self.b_particle_to_init then
		self.b_particle_to_init = false
		param.set( bdd_src, "restart_trig", true )
		return
	end
	local function set_fire_color()
		self:set_color( 1, 1,0,0,0 )
		self:set_color( 2, 1,1,0,1 )
	end
	if rcol1 == 1 then
		set_fire_color()
	else
		self:set_color_def_by_id( rcol1+3, rcol2+12 )
		self:set_color_def()
	end
	local size = 1.5
	if rnd == 1 then
		size = 2
	elseif rnd == 2 then
	elseif rnd == 5 then
		size = 2.5
	else
	end
	self:render_point_bdd_low( bdd_src, size )
end

function VOX:render_points_table( tab )
	if #tab <= 0 then return end
	local	bdd	=	aaa.bdd
	--self:print( nb)

	local 	nb_x = 40
	local 	nb_y = 30
	local 	nb_z = 255
	local	fx	=	8/nb_x
	local	fy	=	6/nb_y
	local	fz	=	5/nb_z
	local	s	=	.5 --self.size
	local	rc	=	.5
	local 	d	=	.8
	for _, v in ipairs(tab) do
		bdd.render_boule(	(v[1]-nb_x*.5)*fx,	(v[2]-nb_y*.5)*fy,	v[3]*fz-2,	s,		 rc )
	end
end



----------------------
--	SKEL
function VOX:render_skel_low( bdd_src )
	local ox,oy,oz  = 0,-.4,1.3
	local f = 7
	local fx,fy,fz = f,f,f*.5

	local function transfo(	x,y,z	)
		--aaa.print( "x  "..y)
		local x0,y0,z0	=	ox+x*fx, oy+y*fy, oz+z*fz
		--aaa.print( x0.." "..y0.." "..z0 )
		return x0,y0,z0

	end

	local 	bdd		=	aaa.bdd
	self:set_color_def()

	local	nb		=	bdd.get_segment_nb( 	bdd_src )
	local xa,ya,za, xb,yb,zb
	local ox,oy,oz  = 0,0,0
	local f = 8
	--self:print( "SEGMENT_NB : "..nb )
	for i=1,nb do
		xa,ya,za, xb,yb,zb = bdd.get_segment( bdd_src, i )
		xa,ya,za	=	transfo(	xa,ya,za )	--	f,	dx,dy,dz	)
		xb,yb,zb	=	transfo(	xb,yb,zb )	--	f,	dx,dy,dz	)
		gol.reset()
		--gol.color_white()
		--aaa.draw_line( xa,ya,za, xb,yb,zb )
		bdd.render_segment(	xa,ya,za, xb,yb,zb,	1., .5 )
		--self:print(  i.." "..xa.." "..ya.." "..za.." / "..xb.." "..yb.." "..zb  )
	end
end
function VOX:render_skel( t )
	if t==0 then
		local rnd = self:do_rnd( t, 5 )
		self:set_mocap( rnd )
		self.b_mocap_to_init = true
		self:set_color_def_by_id( 8, 16 )
		return
	end
	local bdd_src = self.meu_mocap:get_bdd_drawn()
	if not bdd_src then
		return
	end
	if self.b_mocap_to_init then
		self.b_mocap_to_init = false
		param.set( bdd_src, "restart_trig", true )
		FOX.cur.duration = param.get( bdd_src, "time_len" )
		return
	end
	self:render_skel_low( bdd_src )
end

--	this the kinect skeleton, 10 for the upper part, 12 with the back
--			4
--			|
--		9---3---5
--		|	|	|
--		10	|	6
--		|	|	|
--	12-11	2	7-8
--			|
--		17--1--13
--		|		|
--		|		|
--		18	   14
--		|		|
--		|		|
--	20--19	   15--16

if true then
local segment_kinect =
{
	--	trunk
	{	1,	2, 	size=1.2	},
	{	2,	3,	size=1.2	},
	--	shoulder
	{	3,	5 	},
	{	3,	9 	},
	--	hip
	{	1,	13 	},	--,	r=1, g=1, b=.0},
	{	1,	17 	},		--,	r=1, g=1, b=.0},
	--{	2,	17 	},
	--{	2,	13 	},
	--	arms
	{	5,	6 ,		r=1, g=1, b=.0},
	{	6,	7 ,		r=1, g=1, b=.0},
	{	9,	10 	,	r=1, g=1, b=.0},
	{	10,	11  ,	r=1, g=1, b=.0},
	--  legs
	{	13,	14 	,	r=1, g=1, b=.0},
	{	14,	15 	,	r=1, g=1, b=.0},
	{	17,	18 	,	r=1, g=1, b=.0},
	{	18,	19 	,	r=1, g=1, b=.0},


	--	head
	{	3,	4 	,		r=1, g=1, b=.0},
	--	hands
	{	7,	8, 		r=1, g=0, b=0  },
	{	11,	12, 	r=0, g=1, b=0 	},
	-- feet
	{	15,	16,	r=1, g=0, b=0 	},
	{	19,	20, r=0, g=1, b=0 	},

}
else
local segment_kinect =
{
	{	3,	4 	},
	{	3,	5, 	r=0, g=1, b=0 	},
	{	5,	6 ,	},
	{	6,	7 ,	},
	{	7,	8, 	r=1, g=0, b=0},
	{	3,	9 ,	r=0, g=1, b=0 	},
	{	9,	10 },
	{	10,	11 },
	{	11,	12, r=0, g=1, b=0 	},
	--body
	{	1,	2, 	size=1.2, r=1, g=1, b=0 	},
	{	2,	3,	size=1., r=1, g=1, b=0 	},
	{	1,	13 	},
	{	13,	14 	},
	{	14,	15  },
	{	15,	16,	r=1, g=0, b=0 	},
	{	1,	17 },
	{	17,	18 	},
	{	18,	19 	},
	{	19,	20, r=0, g=1, b=0 	},
	{	2,	17 	},
	{	2,	13 	},
}
end
function VOX:render_kinect_low()

	local ox,oy,oz  = 0,-0.8,.8
	local f = 5
	local fx,fy,fz = f,f*.9,f*.5

	local function transfo(	x,y,z	)
		--aaa.print( "x  "..y)
		local x0,y0,z0	=	ox+x*fx, oy+y*fy, oz+z*fz
		--aaa.print( x0.." "..y0.." "..z0 )
		return x0,y0,z0

	end

	self.meu_voxel:set_blend_mode_replace()
	--self:print( "render_kinect" )

	local 	bdd		=	aaa.bdd

	local src = self.bdd_mocap_kinect

	local	nb	=	#segment_kinect
	local xa,ya,za, xb,yb,zb
	local size
	--self:print( "SEGMENT_NB : "..nb )
	for i=1,nb do
		local seg = segment_kinect[i]
		xa,ya,za = bdd.get_point( src, seg[1] )
		xb,yb,zb = bdd.get_point( src, seg[2] )
		xa,ya,za	=	transfo(	xa,ya,za	)
		xb,yb,zb	=	transfo(	xb,yb,zb	)
		if seg.r then
			self:set_color_rgb_both( seg.r, seg.g, seg.b, 1 )
		else
			self:set_color_rgb_both( 0,1,1, 1 )
		end
		--gol.reset()
		--gol.color_white()
		--aaa.draw_line( xa,ya,za, xb,yb,zb )
		size = seg.size or .75
		bdd.render_segment(	xa,ya,za, xb,yb,zb,	size, 1	 )
		--self:print(  i.." "..xa.." "..ya.." "..za.." / "..xb.." "..yb.." "..zb  )
	end

end
function VOX:render_kinect( t )
	if t==0 then
		local rnd = self:do_rnd( t, 1 )
		return
	end

	self:render_kinect_low()
end
function VOX:set_bdd_mocap_kinect( bdd_mocap_kinect )
	self.bdd_mocap_kinect = bdd_mocap_kinect
end

-----------------------
--	LISSAJOU
function VOX:render_lissajous( sx,sy,sz, fx,fy,fz, px,py,pz, phb,phe,nb )
	local 	bdd		=	aaa.bdd

	local function make_xyz( ph )
		local y = sy * math.sin( ph*fy + py )
		local x = sx * math.sin( ph*fx + px )
		local z = sz * math.sin( ph*fz + pz )
		return x,y,z
	end

	local s1,s2 = self.s1, self.s2
	local step = (phe-phb)/nb
	local xb,yb,zb = make_xyz( phb )
	for ph=phb,phe,step do
		local xa,ya,za = xb,yb,zb
		xb,yb,zb = make_xyz( ph )
		bdd.render_segment(	xa,ya,za,	xb,yb,zb,		s1, s2 )
	end
end

local lissa_color =
{
	{ { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } },
	{ { 1, 1, 0 }, { 0, 1, 1 }, { 1, 0, 1 } },
	{ { 0, 0, 1 }, { 0, 1, 1 }, { 1, 0, 1 } },
	{ { 0, 1, 0 }, { 0, 1, 1 }, { 1, 1, 0 } },
	{ { .9, .15, .8 }, { .6, 0, .9 }, { 1., .55, 0. } },
	{ { 1, .52, .0 }, { 1, 1, 0 }, { 1., .0, 0. } },
	{ { .9, .15, .8 }, { .6, .3, .9 }, { 1., .55, 0. } },
	{ { 1, .52, .0 }, { 1, .8, 0 }, { 1., .0, 0. } },
}

function VOX:set_lissa_color( main, sub )
	local col = lissa_color[main][sub]
	self:set_color_rgb_both( col[1], col[2], col[3], .5	 )
end
-----------------------
--	RGB PURSUIT
function VOX:render_rgb_pursuit( dst, buf, t, i_col )
	local bdd	=	aaa.bdd
	bdd.render_op1(	"set_target", buf )
	local op = "add"
	local fx,fy,fz = 2.25*self.fx, 1.5*self.fy, 1.03*self.fz
---[[
	self:set_lissa_color( i_col, 1 )
		bdd.render_op1(	"clear", buf )
		self:render_lissajous( 2,4,2, fx*.9,fy,fz, t,.23,.37, t,t+2, self.nb )
		bdd.render_op2(	op, dst, buf )
--]]
---[[
	self:set_lissa_color( i_col, 2 )
		bdd.render_op1(	"clear", buf )
		self:render_lissajous( 2,4,2, fx,fy,fz, t,.23,.37, t,t+2, self.nb )
		bdd.render_op2(	op, dst, buf )
--]]
---[[
	self:set_lissa_color( i_col, 3 )
		bdd.render_op1(	"clear", buf )
		self:render_lissajous( 2,4,2, fx*1.1,fy,fz, t,.23,.37, t,t+2, self.nb )
			bdd.render_op2(	op, dst, buf )
--]]
end

function VOX:render_rgb_pursuit_1( t )
	local col_id, r_size, time_factor, freq = self:do_rnd( t, 8, 3, 3, 3 )
	time_factor = time_factor * .5
	--self:print( col_id.." "..rnd.." "..ft )
	if r_size == 1 then
		self.s1 = 2
		self.s2 = .5
	elseif r_size == 2 then
		self.s1 = 3
		self.s2 = .25
	else
		self.s1 = 4
		self.s2 = 0
	end
	if freq == 1 then
		self.fx, self.fy, self.fz, self.nb = 1,1,1,16
	elseif freq == 2 then
		self.fx, self.fy, self.fz, self.nb = 3,2,4,64
	else
		self.fx, self.fy, self.fz, self.nb = 1,1.7,1.66,32
	end
	self:render_rgb_pursuit( 1, 2, t*time_factor, col_id )
end

-----------------------
--	RGB NOISE
function VOX:draw_rgb_noise_3d( dst, buf, b_rgb, t, f, depha )
	local bdd	=	aaa.bdd
	local s = .5
	bdd.render_op1(	"set_target", buf )
	if b_rgb then		self:set_color_rgb_both( 1,0,0 )
	else				self:set_color_rgb_both( 1,1,0 )
	end
		bdd.render_op1(	"clear", buf )
		bdd.render_op0(	"noise_3d", 0,-t,0, f, f, f )
		bdd.render_op2(	"add", dst, buf )

	if b_rgb then		self:set_color_rgb_both( 0,1,0 )
	else				self:set_color_rgb_both( 0,1,1 )
	end
		bdd.render_op1(	"clear", buf )
		bdd.render_op0(	"noise_3d", math.sin(t)*depha,-t+math.sin(t)*depha,0, f, f, f )
		bdd.render_op2(	"add", dst, buf )

	if b_rgb then		self:set_color_rgb_both( 0,0,1 )
	else				self:set_color_rgb_both( 1,0,1 )
	end
		bdd.render_op1(	"clear", buf )
		bdd.render_op0(	"noise_3d", 0,-t+math.sin(2*t+s)*depha,math.sin(3*t)*depha, f, f, f )
		bdd.render_op2(	"add", dst, buf )
	self.meu_voxel:set_out_use_alpha( false )
end
function VOX:render_noise_3d( t )
	local rnd = self:do_rnd( t, 4 )
	--rnd = 4

	if rnd == 1 then
		self:draw_rgb_noise_3d( 1, 2, true, t, .1, .5 )
	elseif rnd == 2 then
		self:draw_rgb_noise_3d( 1, 2, false, t, .1, .5 )
	elseif rnd == 3 then
		self:draw_rgb_noise_3d( 1, 2, false, t, .3, .5 )
	elseif rnd == 4 then
		self:draw_rgb_noise_3d( 1, 2, false, t, .5, 5 )
	elseif rnd == 5 then
	else
	end
end

function VOX:draw_rgb_noise_3d_z( dst, buf, b_rgb, t, f, fy, fz )
	fy = fy or f
	fz = fz or f
	local bdd	=	aaa.bdd
	local s = .5
	bdd.render_op1(	"set_target", buf )
	if b_rgb then		self:set_color_rgb_both( 1,0,0 )
	else				self:set_color_rgb_both( 1,1,0 )
	end
		bdd.render_op1(	"clear", buf )
		bdd.render_op0(	"noise_3d", t*.01, t*.0012,-t,		f, fy, fz )
		bdd.render_op2(	"add", dst, buf )


	if b_rgb then		self:set_color_rgb_both( 0,1,0 )
	else				self:set_color_rgb_both( 0,1,1 )
	end
		bdd.render_op1(	"clear", buf )
		bdd.render_op0(	"noise_3d", t*.004, t*.0011, -t*.5,		f, fy, fz )
		bdd.render_op2(	"add", dst, buf )

	if b_rgb then		self:set_color_rgb_both( 0,0,1 )
	else				self:set_color_rgb_both( 1,0,1 )
	end
		bdd.render_op1(	"clear", buf )
		bdd.render_op0(	"noise_3d", t*.0047,  t*.0013, -t*.33, 		f, fy, fz )
		bdd.render_op2(	"add", dst, buf )
	self.meu_voxel:set_out_use_alpha( false )
end

function VOX:render_noise_3d_z( t )
	local rnd = self:do_rnd( t, 7 )
	if 		rnd == 1 then	self:draw_rgb_noise_3d_z( 1, 2, true, t, 		.1, .1 )
	elseif	rnd == 2 then	self:draw_rgb_noise_3d_z( 1, 2, false, t,		.1, .1)
	elseif	rnd == 3 then	self:draw_rgb_noise_3d_z( 1, 2, false, t,		.3, .3 )
	elseif	rnd == 4 then	self:draw_rgb_noise_3d_z( 1, 2, false, t*.4,	.25, .5 )
	elseif	rnd == 5 then	self:draw_rgb_noise_3d_z( 1, 2, false, t,		1, .2 )
	elseif	rnd == 6 then	self:draw_rgb_noise_3d_z( 1, 2, false, t,		.1, 1 )
	else					self:draw_rgb_noise_3d_z( 1, 2, false, t,		.2, 1, 3 )
	end
end

function VOX:draw_rgb_noise_2d( dst, buf, t, f, s, yo, yf, ys, br_slice )
	--		f -- freq
	--		s -- dephasage (genre)
	local bdd	=	aaa.bdd
	--local yo = 0
	--local yf = 2
	--local ys = 2
	--local br_slice = 1

	bdd.render_op1(	"set_target", buf )
	self:set_color( 1, 1,0,0,1 )
	self:set_color( 2, 1,0,0,0 )
		bdd.render_op1(	"clear", buf )
		bdd.render_op0(	"noise_2d", 0,-t,0, f, f, f, yo, yf, ys, br_slice )
		bdd.render_op2(	"add", dst, buf )

--[[
	self:set_color( 1, 0,1,0,1 )
	self:set_color( 2, 0,1,0,0 )
		bdd.render_op1(	"clear", buf )
		bdd.render_op0(	"noise_2d", math.sin(t)*s,-t+math.sin(t)*s,0, f, f, f, yo, yf, ys, br_slice )
		bdd.render_op2(	"add", dst, buf )

	self:set_color( 1, 0,0,1,1 )
	self:set_color( 2, 0,0,1,0 )
		bdd.render_op1(	"clear", buf )
		bdd.render_op0(	"noise_2d", 0,-t+math.sin(2*t+s)*s,math.sin(3*t)*s, f, f, f, yo, yf, ys, br_slice )
		bdd.render_op2(	"add", dst, buf )
--]]

end
function VOX:render_noise_2d( t )
	local rnd = self:do_rnd( t, 1 )
	--rnd = 4
	if rnd == 1 then
		self:set_color_def()
		self:draw_rgb_noise_2d( 1, 2, t,		.1, 3,		0, 1, .3, 1 )
	elseif rnd == 2 then
	elseif rnd == 3 then
	elseif rnd == 4 then
	elseif rnd == 5 then
	else
	end
end

---------------
--	TEXT
function VOX:render_text_low()
	local bdd	=	aaa.bdd
	if true then
		bdd.set_color_map( 2, 110, .5, .45, .06, .12 )
	else
		bdd.set_color_map( 2, 110, .5, -.05, .115, .24 )
	end
--bdd.set_color_map( 1, 115, 0, .5 )
	--	tex_index, curvature, offset, size, size_inside
	local r = 2.3
	local s = 1.
	--bdd.render_op0(	"render_texture", 2, .5, r-s*.5, s, 1 )
	bdd.render_op0(	"render_texture", 2, .5, 2, s, .75 )
end
function VOX:set_text_color()
	self:set_color_def_by_id( 8, 16 )
end
function VOX:render_text( t )
	if t==0 or self:is_text_done() then
		local rnd = self:do_rnd( t, 2 )
		self:set_text_by_id( rnd )
		--self.b_text_to_init = true
		self:set_text_color()
		return
	end
	--local bdd_src = self.meu_mocap:get_bdd_drawn()
	--if not bdd_src then
	--	return
	--end
	--if self.b_mocap_to_init then
		--self.b_text_to_init = false
		--param.set( bdd_src, "restart_trig", true )
		--FOX.cur.duration = param.get( bdd_src, "time_len" )
		--return
	--end
	self:set_color_def()
	self:render_text_low()
end
function VOX:render_countdown( t )
	if t==0 then
		local rnd = self:do_rnd( t, 2 )
		self:set_text_color()
	end
	self:set_color_def()
	local nb = 9 - math.floor( t )
	if nb < 0 then
		nb = 0
		self:set_done( true )
	end
	self:set_text( nb, -.37, -.37 )
	if t>0 then
		self:render_text_low()
	end
end
