function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local iy

	ui.bu_tex = self:add_bu_texture( {9,3} )
	iy = 7
	bu = self:add_slider(	{9,iy,		4,1},	"Tex_x",  self, "tex_x", .5 )
		bu:set_min_max( 0., 1. )
	bu = self:add_slider(	{13,iy,		4,1},	"Tex_y",  self, "tex_y", .5 )
		bu:set_min_max( 0., 1. )
	bu = self:add_slider(	{9,iy+1,	4,1},	"Tex_Sx", self, "tex_sx", .25 )
		bu:set_min_max( .5, 3. )
	bu = self:add_slider(	{13,iy+1,	4,1},	"Tex_Sy", self, "tex_sy", .25 )
		bu:set_min_max( .5, 3. )

	self:add_camera()
	self:add_rgbfa(	{9,11,	nil,nil}, "Test_", nil )

	self.ui.bu_info	= self:add_text_info(	{0,1,	nil,nil},	"info"	)
	self.ui.bu_nbs	= self:add_text_info(	{0,2,	nil,nil},	"nbs" )

	self.geo = {}
	local geo = self.geo
	bu = self:add_button(	{1,4, 1,1}, 		"grid", geo, "b_grid", false )
	bu = self:add_button(	{5,4}, 				"cube", self, "b_cube", false )
	bu = self:add_button(	{2,5}, 				"quinc", geo, "b_hexa", false )

	iy = 6
	bu = self:add_slider(	{1,iy,		8,1},	"r_step", geo, "r_step", .3 )
		bu:set_min_max( .2, .5 )
	bu = self:add_slider(	{1,iy+1,	8,1},	"r_int",  geo, "r_int",  .2 )
		bu:set_min_max( .15, .5 )
	bu = self:add_slider(	{1,iy+2,	8,1},	"r_dec",  geo, "r_dec",  .2 )
		bu:set_min_max( 0, 3 )

	bu = self:add_slider(	{1,iy+4,	8,1},	"d_step", geo, "d_step", .2 )
		bu:set_min_max( .2, .5 )
	bu = self:add_button(	{1,iy+5,	1,1}, 	"sym_4", geo, "b_sym_4", true )

	bu = self:add_slider(	{1,iy+6,	8,1},	"d_y", geo, "d_y", .2 )
		bu:set_min_max( .2, 2. )

	bu = self:add_slider(	{1,iy+8,	8,1},	"dark", self, "dark", .2 )
end

function meu:build()
end

function meu:update()
	local ref = self.ref
end

function meu:build_circle_version()
	local	geo		=	self.geo
	--self:build()
	local	SY		=	geo.sy
	local	ri		=	geo.r_int
	local	re		=	geo.r_ext
	local	S_STEP	=	geo.d_step
	local	r_dec	=	geo.r_dec
	local	SYH		=	SY *.5
	local	sym_nb	=	geo.b_sym_4 and 4 or 2

	local	nb_boule	=	0
	local	nb_ligne	=	0

	local	DY		=	geo.d_y
	local	DYH		=	DY * .5
	local	b_hexa	=	geo.b_hexa

	local	r_nb	=	math.floor( .001 + (re-ri) / geo.r_step ) + 1
	local	r_step	=	(re-ri) / (r_nb-1)

	local	base	=	self.base
	local	pts		=	self.pts

	local	dec		=	0	-- decalage angulaire
	local	dy		=	0.
 	local	r		=	ri
	local	dr		=	1/(r_nb)
	local	nbs = ""
	for ir = 1,r_nb do
		local angle = math.acos( 1 - S_STEP * S_STEP / ( 2 * r * r ) )
		local nb = math.floor( (math.pi2 / sym_nb) / angle )
		nb = nb * sym_nb
		nbs = nbs.." "..nb

		local da = math.pi2 / nb
		--local a = (math.fmod( ir, 2 )==1) and 0 or (da*.5)
		local a = dec * da * r
		dy = math.fmod( dy + DYH, DY )
		for i = 1,nb do
			nb_ligne = nb_ligne + 1
			local x = r * math.cos( a )
			local z = r * math.sin( a )
			base[ nb_ligne ]	=	{ x=x, z=z, dy=dy }
			--	line vertical
			for y = -SYH-dy,SYH,DY do
				nb_boule = nb_boule + 1
				pts[nb_boule] = { x=x, z=z, y=y }
			end
			a =  a + da
		end
		r = r + r_step
		dec = dec + r_dec
	end
	--local layer = self.layer
	self.nb_ligne	=	nb_ligne
	self.nb_boule	=	nb_boule
	self.nbs		=	nbs
end

function meu:build_grid_version()
	--self:build()
	local	geo = self.geo
	local	SY		=	geo.sy
	local	re		=	geo.r_ext
	local	z_dec	=	geo.r_dec
	local	SYH		=	SY *.5

	local	nb_boule	=	0
	local	nb_ligne	=	0

	local	DY		=	geo.d_y
	local	DYH		=	DY * .5
	local	b_hexa	=	geo.b_hexa
	
	local	step	=	geo.r_step
	local	nb		=	math.floor( re*2. / step - .001 ) + 1
	local	sta		=	-((nb-1) * step) / 2

	local	base	=	self.base
	local	pts		=	self.pts

	local	dec		=	0	-- decalage angulaire
	local	dy		=	DYH
	--local	dr		=	1/(r_nb)
	local	nbs = ""
	local 	tmp

	local	d2 = re*re
	local	x = sta
	for ix = 1,nb do
		local z = sta
		tmp = nb_ligne
		for iz = 1,nb do
			if ( x*x + z*z ) < d2 then
				nb_ligne = nb_ligne + 1

				if b_hexa then
					dy = math.fmod( math.fmod( iz, 2 ) + math.fmod( ix, 2 ), 2) * DYH
				else
				end
				table.insert( base, { x=x, z=z, dy=dy } )
				--	line vertical
				for y = -SYH+dy,SYH,DY do
					nb_boule = nb_boule + 1
					pts[nb_boule] = { x=x, z=z, y=y }
				end
			end
			z = z + step
		end
		dec = dec + z_dec
		x = x + step
		nbs = nbs.." "..(nb_ligne-tmp)
	end
	--local layer = self.layer
	self.nb_ligne	=	nb_ligne
	self.nb_boule	=	nb_boule
	self.nbs		=	nbs
end

function meu:fill_boules_color_from_img()
	local	fx		=	self.tex_sx/4.6
	local	fy		=	self.tex_sy/self.geo.sy
	local	ox = 	self.tex_x
	local	oy = 	self.tex_y

	local	pts = self.pts
	local	x,y,z
	local	pt

	for i=1,self.nb_boule do
		pt = pts[i]
		x, y, z = pt.x, pt.y, pt.z
		local b_color
		if inside( z, -.7, .7 ) then
			local tx = x * fx + ox
			if inside_01( tx ) then
				local ty = y * fy + oy
				if inside_01( ty ) then
					local r,g,b,a = aaa.img.get_color_uv( nil, tx, ty, true )
					pt.r, pt.g, pt.b = r,g,b
				end
			end
		end
	end
end

function meu:erase_boules( r,g,b )
	local	pts = self.pts
	local	pt
	for i=1,self.nb_boule do
		pt = pts[i]
		pt.r, pt.g, pt.b = r,g,b
	end
end

function meu:draw_boules( fn )
	local	pts		=	self.pts
	local	pt
	local 	da		=	self.dark
	da = da or .2
	local	dab		=	1-da
	for i=1,self.nb_boule do
		pt = pts[i]
		gol.color( da+pt.r*dab, da+pt.g*dab, da+pt.b*dab )
		fn( pt.x, pt.y, pt.z )
	end
end

local function table_is_similar( tbl1, tbl2 )
	for k, v in pairs( tbl1 ) do
		if v ~= tbl2[k] then return false end
	end
	return true
end

function meu:build()
	local geo = self.geo
	geo.r_ext		=	2.3
	geo.sy			=	8
	if not self.geo_last or not table_is_similar( self.geo, self.geo_last ) then
		self.base	=	{}
		self.pts	=	{}
		self:print( "build()" )
		if geo.b_grid then
			self:build_grid_version()
		else
			self:build_circle_version()
		end
		self.geo_last = table.copy_shallow( self.geo )
	end
end

function meu:draw_led3d()
	self:build()
--	self.dark = .2

	local bu_tex = self.ui.bu_tex
	local bind = bu_tex:get_bind_2d()

	local fn
	if self.b_cube then
		fn = function(x,y,z) aaa.draw_cube( x, y, z, .08 ) end
	else
		fn = function(x,y,z) aaa.draw_sphere_xyz( x,y,z, .08, 6,3 ) end
		--fn = function(x,y,z) aaa.draw_disk_axe_z( x, y, z, .08, 6 ) end
	end

	self:erase_boules( 0,0,0 )
	if bind and aaa.img.set_lua_cur(bind) then
		self:fill_boules_color_from_img()
		self:draw_layers_begin()
			self:draw_layer(1)
			self:draw_boules( fn )
		self:draw_layers_end()
	end
end

function meu:read_data()
	--self:draw_led3d()
	self.data = {}
	local d = aaa.obj.get( aaa.dir.get_dir_user().."/default.07.bdd_datagrid" )
	if not d then return end
	local bx = 0
	local by = 0
	local nb = 0
	for l=2,509 do
		--aaa.print( l )
		local name = aaa.obj_get_str( d, 2, l )
		local x = aaa.obj_get_real( d, 10, l ) / 100000
		local y = aaa.obj_get_real( d, 11, l ) / 1000000
		local zone = aaa.obj_get_real( d, 13, l )
		if zone > 5 then zone = 5 end
		self.data [l] = { name= name, x=x, y=y, zone=zone }
		--aaa.print( l.."\t"..name.." "..zone.." "..x.." "..y.." " )
		nb = nb+1
		bx = bx+x
		by = by+y
	end
	self.bx = bx / nb
	self.by = by / nb
end

function meu:draw_sncf()
	--self:draw_led3d()
	--if not self.data then
		self:read_data()
	--end
	--self:print( "draw_sncf" )
	self:draw_layers_begin()
		self:draw_layer(1)

		local bx,by = self.bx, self.by
		local S = .01
		for l=2,509 do
			local d = self.data[l]
			local si = S*d.zone
			--self:print( "draw_sncf "..d.y )
			aaa.draw_box( si, si, si,	(d.x-bx)*4, si*.5, (d.y-by)*40	)
		end
	self:draw_layers_end()
end

function meu:draw()
	self:draw_led3d()
	aaa.draw_circle_axe_y( 0, 4.2, 0, 5, 48 )
	self:draw_sncf()
end

function meu:update_ui()
	local ui = self.ui
	ui.bu_info:set_text( self.nb_ligne.." lignes with "..self.nb_boule.." boules" )
	ui.bu_nbs:set_text( self.nbs )
end
