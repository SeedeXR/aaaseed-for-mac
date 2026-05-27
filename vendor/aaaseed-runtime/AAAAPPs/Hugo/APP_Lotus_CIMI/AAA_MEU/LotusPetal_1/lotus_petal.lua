function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local ix,iy = 1,2
	self:add_slider(	{ix,iy,	8,1},		"dx",		self, "dx", 		.14 )
	self:add_slider(	{ix,iy+1,	8,1},	"y",		self, "y", 			.76 )
	self:add_slider(	{ix,iy+2,	8,1},	"rot",		self, "rot", 		.125, 0, .25 )

	iy = iy+3
	self:add_button( {ix, iy },			"GDebug",	self, "b_gdebug",	false )
	iy = iy+1
	bu = self:add_slider(	{ix,iy,	8,1},	"nb",		self, "nb_point", 		16, 3, 64 )
		bu:set_value_type_integer( true )
	iy = iy+1
	self:add_button( {ix, iy },			"Point",	self, "b_point",	false )
	self:add_button( {ix, iy+1 },			"Side",		self, "b_side",		false )
	self:add_button( {ix, iy+2 },			"Line",		self, "b_line",		false )
	self:add_button( {ix, iy+3 },			"Quad",		self, "b_quad",		true )

	ix,iy = 9,2.5
	self:add_slider(	{ix,iy,	8,1},	"DiaExt",		self, "dia_ext", 		.8, 0, 1.1 )
	self:add_slider(	{ix+1,iy+1,	7,1},	"RotExt",		self, "rot_ext", 		0, 0, 1 )
	iy = iy + 2
	self:add_slider(	{ix,iy,	8,1},	"DiaInt",		self, "dia_int", 		.5, 0, 1.1 )
	self:add_slider(	{ix+1,iy+1,	7,1},	"RotInt",		self, "rot_int", 		0, 0, 1 )
end
function meu:init()
	local ref = self.ref
	local layers = self:get_layers()
	local obj = aaa.obj.get_down( layers, "lotus_trs1" )
	ref.rot = param.get_ref( obj, "rotate_y" )
	local obj = aaa.obj.get_down( layers, "lotus_trs2" )
	ref.trs_y = param.get_ref( obj, "translate_y" )
	ref.trs_z = param.get_ref( obj, "translate_z" )
	ref.rot2 = param.get_ref( obj, "rotate_x" )

	app.meu_petal = self
end

local side_radius = 1.5
--	develope 1.456
--	longueur entre les points 1.238
--	largeur max .642
local pt_a = { 0.14, 0, 0		}
local pt_b = { 0.24, 0.03, 0.5 }
local pt_c = { 0, 0.39, 1.03}

local function draw_point( id, pt )
	gol.draw_points_3d(	pt[1], pt[2], pt[3],
						-pt[1], pt[2], pt[3]	)
end
function meu:draw_side( a, b, ph_begin, ph_end )
	gol.draw_lines_3d(	a[1],	a[2],	a[3],
					 	b[1],	b[2],	b[3],
						-a[1],	a[2],	a[3],
					 	-b[1],	b[2],	b[3]	)
end
local function draw_line_sym( id, a )
	gol.draw_lines_3d(	a[1],	a[2],	a[3],
					 	-a[1],	a[2],	a[3]	)
end
local function draw_line( a, b )
	gol.draw_lines_3d(	a[1],	a[2],	a[3],
					 	b[1],	b[2],	b[3]	)
end
function meu:draw_vector_at( a, vec )
	local b = V3.get_add( a, vec )
	draw_line( a, b )
end
function meu:draw_quad( a, b, ph_begin, ph_end )
	local nor = V3.get_sub( b, a )
	nor = V3.get_cross_x( nor )
	V3.normalize( nor )
	--self:draw_vector_at( a, nor )
	gol.normal3( nor[1], nor[2], nor[3] )
	gol.draw_triangle_strip_3d(	a[1],	a[2],	a[3],
								-a[1],	a[2],	a[3],
					 			b[1],	b[2],	b[3],
					 			-b[1],	b[2],	b[3]	)
end

--	develope 1.456
--	longueur entre les points 1.238
--	largeur max .642
local pt_a = { 0.14, 0, 0		}
local pt_b = { 0.24, 0.03, 0.5 }
local pt_c = { 0, 0.39, 1.03}

function meu:draw_petal_cat()
	local pl = POINT_LIST:create_nb( 5 )
	pl:set_pt( 2, pt_a )
	pl:set_pt( 3, pt_b )
	pl:set_pt( 4, pt_c )
	local plc = pl:create_catmull_rom_points( 7 )
	plc:draw_lines_from_points( self, "draw_quad", 0, 1, plc:get_pts() )
	--plc:process_points_with_fn( draw_point )
end

function meu:draw_petal()
	local GOLD = 1.6180339887
	local dang = math.pi * 2 / (2*GOLD)

	local r = .75
	local u_off = r * (1 - math.cos(dang))
	local v_off = r * math.sin(dang)

	local x = self.dx
	local l = math.sqrt( u_off*u_off + v_off*v_off )
	--self:print( l )
	local y = self.y
	local z = math.sqrt( l*l - x*x - y*y )

	local a = { x, 0, 0 }
	local b = { 0, y, z }
	gol.color_white()
	if self.b_gdebug then draw_point( 1, a ) draw_point( 1, b ) end

	local vx = V3.get_sub( b, a )
	V3.normalize( vx )
	if self.b_gdebug then gol.color_red() self:draw_vector_at( a, vx ) end

	local tmp = V3.get_cross_y( vx )
	--self:draw_vector_at( a, tmp )
	local vy = V3.get_cross( tmp, vx )
	V3.normalize( vy )
	if self.b_gdebug then gol.color_green() self:draw_vector_at( a, vy ) end

	local vz = V3.get_cross( vx, vy )
	V3.normalize( vz )		--todoopt not useful ?
	if self.b_gdebug then gol.color_blue() self:draw_vector_at( a, vz ) gol.color_white() end

	local nb = self.nb_point
	local pl = POINT_LIST:create_nb( nb )
	local ang = - dang * .5
	dang = dang / (nb-1)
	local u_off = r * math.cos(ang)
	local v_off = r * math.sin(ang)
	local rot = self.rot * math.pi * 2.
	local co = math.cos(rot)
	local si = math.sin(rot)
	local pt = {}
	for i=1,nb do
		local u = r * math.cos(ang) - u_off
		local v = r * math.sin(ang) - v_off
		V3.combine3( pt, v, vx, -u*co, vy, -u*si, vz )
		V3.add( pt, pt, a )
		pl:set_pt( i, pt )
		ang = ang + dang
	end
	if self.b_point then
		gol.set_point_size(8)
		pl:process_points_with_fn( draw_point )
	end
	if self.b_side then
		pl:draw_lines_from_points( self, "draw_side", 0, 1, pl:get_pts() )
	end
	if self.b_line then
		pl:process_points_with_fn( draw_line_sym )
	end
	if self.b_quad then
		pl:draw_lines_from_points( self, "draw_quad", 0, 1, pl:get_pts() )
	end
end

function meu:draw_petals( nb, ph, r, ang, y )
	if true then
		gol.push_matrix()
			--gol.rotate_y( ang )
			for i=1,nb do
				gol.push_matrix()
					gol.rotate_y( (i+ph)/nb )
					gol.translate( 0,0,r )
					gol.rotate_x( -ang )
					app.meu_petal:draw_petal()
				gol.pop_matrix()
			end
		gol.pop_matrix()
	else
		local ref = self.ref
		param.set( ref.rot2, ang - .25 )
		param.set( ref.trs_y, y )
		for i=1,nb do
			param.set( ref.trs_z, -r  )
			param.set( ref.rot, (i+ph)/nb		 )
			self:draw_layers()	--draw all layers in layers
		end
	end
end
--	petal
--	petal 126 longueur
--	petal 28 base
--	hauteur 76 et 93
--	projecteur
--	5.64 at 110 / 110 du centre
function meu:draw_flower()
	local ref = self.ref
	--local cam = self:get_camera( 7 )
	--aaa.obj.update_then_draw( cam )

	self:draw_layers_begin()
		local nb = 6
		--self:draw_petal()
		self:draw_petals( nb, 0,	self.dia_ext*.5,	self.rot_ext*.25,	.03 )
		--self:draw_petals( nb, 0.5, 	dia_int*.5, 	.25/6,	.03 )
		self:draw_petals( nb, 0.5, 	self.dia_int*.5, 	self.rot_int*.25,	.03 )
	self:draw_layers_end()
end

function meu:draw()
	local ref = self.ref
	--local cam = self:get_camera( 7 )
	--aaa.obj.update_then_draw( cam )

	local dia_ext = 1.01
	local dia_int = .57
	self:draw_layers_begin()
		self:draw_layers()
		self:draw_petal()
	self:draw_layers_end()
end



