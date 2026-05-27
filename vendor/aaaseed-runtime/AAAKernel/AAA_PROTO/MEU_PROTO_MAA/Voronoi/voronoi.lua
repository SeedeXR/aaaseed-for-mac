function meu:define_meu_infos( )
	return { 	author = "Mâa paying here tribute to Mister Fortune",
				tags = { "2d","3d", "draw", "Vj", "coregraphic", "art", "geometry", "Procedural", "point"  },
				help =	{	"From s set of points (Sites) on the Cpu it computes the voronoi cells (or the Delaunay triangulation).",
							"  This correspond to the frontiers of equal distance between the sites, result evoques cell / soap bubble.",
							"it use c_bdd_voronoi implementing the Fortune Algorithm on the points",
							"  from the current c_bdd (last drawn c_bdd) having points on the Cpu.",
							"Lots of options added to generate rich graphics."
						}
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix, iy = 1,1
	local SY = 1.
	local DY = .2


	self:add_bu_texture(				{ix,iy, 8,5},		"Vell", 2  )
	iy = iy + 5 + DY
	self:add_rgbf(						{ix,iy,	8,SY*1.5}	)
	iy = iy + SY*1.5 + DY
	self:add_mapping_and_blending(		{ix,iy,	8,SY*3}	)
	iy = iy + SY*3 + DY
	self:add_bu_texture_target_unit(	{ix,iy, 8,5}	)

	local bdd = ref.voronoi.bdd
	local ren = ref.rendering
	--table.print( ref, 2 )
	--aaa.box_good( "ren is "..ren )
	ix, iy = 9,2
	self:add_camera()
	iy = 2 + DY
	self:add_size_uvf( {ix,iy, 	8,SY} )
	iy = iy + SY + DY
	self:add_button( {ix,  	iy,	SY,SY},	"Site",			bdd, "draw_site" 					)
	self:add_slider( {ix+4,	iy,	4,SY},	"Site Size",	bdd, "draw_site_size",				.1,	0,1	)
		:set_text( "Size" )
	iy = iy + SY + DY
	self:add_button( {ix,  	iy,	SY,SY},	"Delaunay", 	bdd, "delaunay"						)
	self:add_slider( {ix+4,	iy,	4,SY},	"Delaunay Size",bdd, "delaunay_draw_line_size",	1,	1,32	)
		:set_text( "Size" )
	iy = iy + SY + DY
	self:add_button( {ix,   iy,	SY,SY},	"Voronoi",		bdd, "voronoi"						)
	self:add_button( {ix+4, iy},		"Boundary",		bdd, "draw_boundary"				)
	iy = iy + SY
	self:add_button( {ix+1,	iy,	3,SY},	"Vertice",		bdd, "voronoi_draw_vertice"			)
	self:add_slider( {ix+4,	iy,	4,SY},	"VSize",		ren, "point_size",					1,	0,64	)	--could be more
		:set_text( "Size" )
	iy = iy + SY
	self:add_button( {ix+1,	iy,	3,SY},	"Edge",			bdd, "voronoi_draw_edge"			)
	self:add_slider( {ix+4,	iy,	4,SY},	"ESize",		ren, "line_size",					1,	0,16	)
		:set_text( "Size" )
	iy = iy + SY
	self:add_button( {ix+1,	iy,	3,SY},	"poly",			bdd, "voronoi_draw_poly"			)
	self:add_slider( {ix+4,	iy,	4,SY},	"PSize",		bdd, "voronoi_draw_poly_size",		1,	0,4		)
		:set_text( "Size" )
	iy = iy + SY
	self:add_button( {ix+1,	iy,	3,SY},	"Img",			bdd, "image_color_use"				)
	self:add_slider( {ix+4,	iy,	4,SY},	"Alpha_min",	bdd, "image_color_alpha_thereshold"	)
	iy = iy + SY
	self:add_button( {ix+2,	iy,	3,SY},	"Img_tri",		bdd, "image_color_by_triangle"		)
	self:add_button( {ix+5,	iy,	3,SY},	"Img_point",	bdd, "image_color_by_point"			)
	iy = iy + SY + DY
	self:add_button( {ix,	iy, SY,SY},	"Depth",		ren, "depth"						)
end

function meu:init()
	local ref = self.ref
	ref.voronoi	 = {}
	local vor = ref.voronoi
	vor.bdd				=	aaa.obj.get_branch_by_class( ref.__layer_marked, "bdd_voronoi" )
	vor.image_color_bind	= param.get_ref( vor.bdd, "image_color_bind"	)
--[[
	local bdd = vor.bdd
	vor.site				= param.get_ref( bdd, "draw_site_src" )
	vor.delaunay			= param.get_ref( bdd, "delaunay" )
	vor.voronoi				= param.get_ref( bdd, "voronoi" )
	vor.draw_site 			= param.get_ref( bdd, "draw_site_src" )
--]]
end

function meu:update()
	local bind = self:get_texture_bind_2d( 2 )
	param.set( self.ref.voronoi.image_color_bind, bind )
end

function meu:draw_icon()
	gol.set_line_width( 1 )
	gol.push_translate_scale_2d( -.35, 0, .3, 1. )
		gol.draw_lines_2d( 0,0, .1,.5, 0,0, .4,-.5, 0,0, -.5,-.2 )
		gol.set_point_size( 3 )
		gol.draw_points_2d( -.25, .3,-.05, -.35, .3, .1 )
	gol.pop_matrix()
end
