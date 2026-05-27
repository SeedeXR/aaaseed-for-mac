local EMP = .0

if aaa.lua.global.declare_table( "MAAEB" ) then
	MAAEB.alpha = 1
	--Maa do 3d stroke because on Muc / iris 540 there is a bug (gl driver?) when using 2s stroke (random z)
	MAAEB.__stroke_e =	{	0, -1, 0,		0, 1, 0,
							0-EMP, 1, 0,	1, 1, 0,
							0, 0, 0,		1, 0, 0,
							0-EMP, -1, 0, 	1, -1, 0
						}
	MAAEB.__stroke_b =	{	0, -1, 0,		0, 1, 0,
							0-EMP, 1, 0,	.5, 1, 0,
							0, 0, 0,		.5, 0, 0,
							0-EMP, -1, 0,	.5, -1, 0
						}
	MAAEB.__stroke_m =	{	-2, -1, 0,		-2, .5, 0,
							-1, -1, 0,		-1, .5, 0,
							0, -1, 0,  		0, .5, 0
						}
end

function MAAEB.set_alpha( alpha )
	MAAEB.alpha = alpha
end
function MAAEB.draw_e( x, y, a )
	gol.translate( x )
	gol.rotate_z( a )
	gol.draw_lines_3d( MAAEB.__stroke_e )
	gol.rotate_z( -a )
	gol.translate( -x )
end
function MAAEB.draw_b( x, a )
	gol.translate( x )
	gol.rotate_z( a )
	gol.draw_lines_3d( MAAEB.__stroke_b )
	aaa.draw_circle_arc_axe_z( .5, .5, 0, 1, -.25, .5, 47 )
	aaa.draw_circle_arc_axe_z( .5, -.5, 0, 1, -.25, .5, 47 )
	gol.rotate_z( -a )
	gol.translate( -x )
end
function MAAEB.draw_m()
	gol.draw_lines_3d( MAAEB.__stroke_m )
	aaa.draw_circle_arc_axe_z( -.5, .5, 0, 1, 0, .5, 47 )
	aaa.draw_circle_arc_axe_z( -1.5, .5, 0, 1, 0, .5, 47 )
	aaa.draw_circle_arc_axe_z( .5, 1.25, 0, .75, 0, .5, 2 )
end

function MAAEB.draw_grid( line_width_grid, alpha )
	gol.color( 0, .5, .5, .5 * (alpha or 1))
	gol.set_line_width( line_width_grid or 1. )
	local MORE = .5
	for y = -1,1,.5 do
		aaa.draw_line( -2-MORE, y, 2+MORE, y )
	end
	for x = -2,2,.5 do
		aaa.draw_line( x, -1-MORE, x, 1+MORE )
	end
	--aaa.draw_circle_axe_z( .5, -.5, 0, 1, 47 )
	--aaa.draw_circle_axe_z( .5, .5, 0, 1, 47 )
	--aaa.draw_circle_axe_z( -.5, .5, 0, 1, 47 )
end

function MAAEB.draw_text( b_grid, t, line_width, line_width_grid )
	t = t or aaa.time.t
	local le = math.sin( t *.5 ) + .5
	local l = clamp_01( le )
	le = clamp_01( (le-1)*2 )

	local alpha = MAAEB.alpha

	gol.scale( 1/4 )
	if b_grid then
		MAAEB.draw_grid( line_width_grid, alpha )
	end
	gol.color_white( alpha )
	gol.set_line_width( line_width or 2. )
		MAAEB.draw_e(	interpolate( -1, 1, l ),	interpolate( 0, 0, l ),		interpolate( 0, -.25, l )	)
		MAAEB.draw_b(	interpolate( 0, 1, l ),		interpolate( 0, 0.25, l )								)
	gol.color_white( le * alpha )
	MAAEB.draw_m()
end

function MAAEB.draw( x,y,z, sca, ... )
	--if true then return end
	gol.push_matrix()
		gol.translate( x,y,z )
		gol.scale( sca )
		MAAEB.draw_text( ... )
	gol.pop_matrix()
end

