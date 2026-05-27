aaa.show_file_begin( "aaa_draw" )

if not aaa.doc then
	aaa.doc = {}
end
local doc = aaa.doc



doc.draw_line_2d_stipple =	"( xa,ya, xb,yb, nb_seg_by_unit=8, phase=0, b_flip=false )\n"..
						"Draws a stipple line, phase describe a progression from point a execpt if b_flip is true"
aaa.__t_draw_line_stipple = {}
function aaa.draw_line_2d_stipple( xa,ya, xb,yb, nb_seg_by_unit, phase, b_flip )
	if b_flip then xa,ya,xb,yb = xb,yb,xa,ya end
	local dx,dy = xb - xa,  yb - ya
	local len = math.sqrt( dx*dx + dy*dy )
	local nbf = len * (nb_seg_by_unit or 8)
	--local nb, frac = math.modf( nbf )
	phase = ((phase or 0)%1) / nbf
	local dphase = 1 / nbf
	phase = phase - dphase
	local i = 1
	local t = aaa.__t_draw_line_stipple --we never free but this is by purpose 
	for _=1,nbf+2 do
		local p = clamp_01( phase )
		t[i]   = interpolate( xa,xb, p )
		t[i+1] = interpolate( ya,yb, p )
		p = clamp_01( phase + dphase * .8 )
		t[i+2] = interpolate( xa,xb, p )
		t[i+3] = interpolate( ya,yb, p )		
		phase = phase + dphase
		i = i + 4
	end
	gol.draw_lines_2d( t, (i-1)/2 )
end

--
--	TEXTURE
--
doc.draw_tex =		"( bind, x,y, sx,sy, text )\n".. -- deprecated ?
						"Draws texture corresponding to bind, with aaa.draw_tex_uv, and if provided text\n"..
						"inside with aaa.draw_str_maa "
function aaa.draw_tex( bind, x,y, sx,sy, text )
--	gol.push_attrib()
		gol.color_white()
		aaa.draw_bind_rect( bind, x,y, x+sx,y+sy )
		if text then
			gol.color( 0,1,.5, 1 )
				gol.push_translate_scale_2d( x,y -.2,.1, .2 )
					aaa.draw_str_maa( text )
				gol.pop_matrix()
			gol.color_white()
		end

		--gol.bind_texture( -1 )	--todo after this is a bug
--	gol.pop_attrib()
end

doc.draw_line_tex =	"( p1, p2, w )\n"..
						"p1 : start point, p2 : end point, w : width of the line\n"..
						"Draws a textured line from p1 to p2, of width w. Uses aaa.draw_triangle_strip_4xy_uv()."
function aaa.draw_line_tex( p1, p2, w )
	local x1,y1 = p1[1], p1[2]
	local x2,y2 = p2[1], p2[2]
	local dx = x2 - x1
	local dy = y2 - y1
	local d2 = dx*dx + dy*dy
	if d2 > 0 then	-- check p1 ~= p2 to avoid division by 0 
		w = w * .5 / math.sqrt( d2 )
		dx, dy =  -dy*w, dx*w
		aaa.draw_triangle_strip_4xy_uv( x1+dx, y1+dy, x1-dx, y1-dy, x2+dx, y2+dy, x2-dx, y2-dy )
	end
end

--
--	ARROW
--
local function draw_arrow_points_low( fn, x,y,z, sx,sy )
	sx = (sx or 1.) * .166666666
	sy = (sy or 1.) * .166666666
	fn(	x,		y-sy,	z,
		x-3*sx,	y-sy,	z,
		x-3*sx,	y+sy,	z,
		x,		y+sy,	z,
		x,		y+3*sy,	z,
		x+3*sx,	y,		z,
		x,		y-3*sy,	z )
end
function aaa.draw_arrow( x,y,z, sx,sy )
	draw_arrow_points_low( gol.draw_triangle_fan_3d, x,y,z, sx,sy )
end
function aaa.draw_arrow_line( x,y,z, sx,sy )
	draw_arrow_points_low( gol.draw_line_loop_3d, x,y,z, sx,sy )
end

--
--	Fan of lines in quad
--
doc.draw_lines_nb_4xy = { "( nb, x0,y0, x1,y1, x2,y2, x3,y3 ) draw horizontal lines of a grid defined by 4 corners,",
							"23   2 is left/top,    3 is right/top.",
							"01   0 is left/bottom, 1 is right/bottom" }
function aaa.draw_lines_nb_4xy( nb, x0,y0, x1,y1, x2,y2, x3,y3 )
	if nb == nil or nb < 2 then return end
	local f = 1/(nb-1)
	local vx0, vy0 = (x2-x0)*f, (y2-y0)*f
	local vx1, vy1 = (x3-x1)*f, (y3-y1)*f
	for i=0,nb-1 do
		aaa.draw_line(		x0 + i*vx0,		y0 + i*vy0,		x1 + i*vx1, 	y1 + i*vy1	)
	end
end

doc.draw_grid_nbxy_4xy = { "( nb_x,nb_y, x0,y0, x1,y1, x2,y2, x3,y3 ) draw a grid defined by 4 corners,",
							"23   2 is left/top,    3 is right/top.",
							"01   0 is left/bottom, 1 is right/bottom" }
function aaa.draw_grid_nbxy_4xy( nb_x,nb_y, x0,y0, x1,y1, x2,y2, x3,y3 )
	aaa.draw_lines_nb_4xy( nb_x, x0,y0, x1,y1, x2,y2, x3,y3 )
	aaa.draw_lines_nb_4xy( nb_y, x0,y0, x2,y2, x1,y1, x3,y3 )
end
doc.draw_grid_lbrt = { "( nb_x,nb_y, left,bottom, right,top ) draw a grid in a rectangle." }
function aaa.draw_grid_lbrt( nb_x,nb_y, l,b , r,t )
	aaa.draw_lines_nb_4xy( nb_y, l,b, r,b, l,t, r,t )
	aaa.draw_lines_nb_4xy( nb_x, l,b, l,t, r,b, r,t )
end
doc.draw_crosshair = { "( x,y, size_external,size_interior ) draw a cross at x,y of size size_external/2,",
						"the center of the cross is not drawn inside a circle of radius size_interior." }
function aaa.draw_crosshair( x,y, se,si )
	gol.draw_lines_2d(	x+se, y,	x+si, y,
						x-se, y,	x-si, y,
						x, y+se,	x, y+si,
						x, y-se,	x, y-si		)
end

function aaa.draw_annulus_color_gradient( x,y, r_int, r_ext, angle_begin, angle_end, col_int, col_ext, arc_nb )
	arc_nb = arc_nb or 12
	local a = angle_begin * math.pi2
	local da = (angle_end - angle_begin) * math.pi2 / arc_nb

	gol.begin_triangle_strip()
	for i=0,arc_nb do
		local c,s = math.cos(a), math.sin(a)
		gol.color( col_int )
		gol.vertex2( x + r_int*c, y + r_int*s )
		gol.color( col_ext )
		gol.vertex2( x + r_ext*c, y + r_ext*s )
		a = a + da
	end
	gol.do_end()
end

--todo can be optimized
function aaa.draw_catmull_rom( pos, nb_seg )
	local do_catmull_rom = aaa.math.do_catmull_rom_xyz
	local nb = 6
	local o_nb = 1/nb
	gol.begin_line_strip()
		for ip =1,nb_seg do
			local i = (ip-1)*3+1
			gol.vertex3( pos[i+3],pos[i+4],pos[i+5] )
			local x,y,z = do_catmull_rom( o_nb,	pos[i],		pos[i+1],	pos[i+2],
												pos[i+3],	pos[i+4],	pos[i+5],
												pos[i+6],	pos[i+7],	pos[i+8],
												pos[i+9],	pos[i+10],	pos[i+11]
												)
			gol.vertex3( x,y,z )
			for j=2,nb-1 do
				x,y,z = do_catmull_rom( j*o_nb )
				gol.vertex3( x,y,z )
			end
		end
		local i = (nb_seg+1) * 3 + 1
		gol.vertex3( pos[i],pos[i+1],pos[i+2] )
	gol.do_end()
end


aaa.show_file_end( "aaa_draw" )
