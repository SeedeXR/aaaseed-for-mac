
function meu:define_meu_infos( )
	return { author = "Mâa Abdalight", date="2024",
			tags = { "Core", "Tutorial", "3d", "2d", "draw" },
			help = { "Show how to using the Opengl lua glue for different graphic primitives.",
					 "https://math.hws.edu/graphicsbook/c3/s1.html#gl1geom.1.1 simple reference on primitives" }
			 }
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local ix,iy = 1,1
	local SY,DY = 1,.2
	local bu

	self:add_rendering()
	self:add_camera()

	bu = self:add_button( {ix,iy,       4,SY},  "Primitive",    nil, nil,        0   )
 		bu:set_menu{ 	"Points",
						"Lines",		"Line Strip",		"Line Loop",
						"Triangles", 	"Triangle Strip",	"Triangle Fan",
						"Quads" ,		"Quad Strip",		"Polygon"		 }
			:set_nb( 2, 5 )
			:set_target_lua( self, "id_primitive_selected", 4 )
			:set_text_selector( true )
		self.fn_compute = { "points",
							"points",		"points",			"points",
							"points", 		"points_strip",		"points_fan",
							"points_quad",	"points_strip",		"points_fan"	}
		self.fn_draw	= { "points",
							"lines",		"line_strip",		"line_loop",
							"triangles", 	"triangle_strip",	"triangle_fan",
							"quads",		"quad_strip",		"polygon" 		}
	bu = self:add_button( {ix+4,iy,     4,SY},   "Fast",     self,"b_fast",      false   )
	iy = iy + SY + DY

	local SX3 = 8 / 3
	bu = self:add_slider( {ix,      iy, SX3,SY},   "X", 	self,"x",   		0,      -4,4 )
		:set_color_back( "x" )
	bu = self:add_slider( {ix+SX3,  iy,	SX3,SY},   "Y",		self,"y",    		0,      -4,4 )
		:set_color_back( "y" )
	bu = self:add_slider( {ix+SX3*2,iy,	SX3,SY},   "Z",		self,"z",    		0,      -4,4 )
		:set_color_back( "z" )
	iy = iy + SY

	bu = self:add_slider( {ix,iy,       8,SY},   "Size",     self,"size",        5,      0,10 )
	iy = iy + SY + DY

	bu = self:add_bu_texture_target_unit( {ix,iy,       8,6*SY}, "Tex", 1, true )
	iy = iy + 6 * SY
	bu = self:add_slider( {ix,iy,       4,SY},   "u factor", self,"u_factor",    1,      0,5 )
		bu:set_color_back( "u" )
	bu = self:add_slider( {ix+4,iy,     4,SY},   "v factor", self,"v_factor",    1,      0,5 )
		bu:set_color_back( "v" )
	iy = iy + SY + DY

	ix, iy = 9, 2 + DY
	bu = self:add_transfo( {ix,iy,  	8,2.5*SY} )
	iy = iy + 2.5 + DY

	iy = iy + SY
	bu = self:add_button( {ix,iy,    	 8,SY*2},   "Gol Test",     self,"b_gol_test",      false   )

end

function meu:init()
end

-- generate points and uv for 2 triangles
--		   2    3_____5
--		  / \    \   /
--		 /   \    \ /
--		0_____1    4
function meu:generate_points( x,y,z, size )
	local s  = size
	local sh  = s / 2	-- size half
	self.points = { x-s,y-sh,z,		x,   y-sh,z,	x-sh,y+sh,z,
					x,  y+sh,z,		x+sh,y-sh,z,	x+s, y+sh,z		}
	local suh, svh = self.u_factor*.5, self.v_factor*.5
	local l,r = .5-suh, .5+suh
	local b,t = .5-svh, .5+svh
	self.uv		= {	l,b,			r,b,			.5,t,
					l,t,			.5,b,			r,t				}
end
-- generate points for 4 triangles as strip
--		0__2__4
--		| /| /|
--		|/ |/ |
--		1__3__5
function meu:generate_points_strip( x,y,z, size )
	local s  = size
	local sh  = s / 2	-- size half
	self.points = { 	x-s,y+sh,z,		x-s,y-sh,z,
						x,  y+sh,z,		x,  y-sh,z,
						x+s,y+sh,z,		x+s,y-sh,z	}
	local suh, svh = self.u_factor*.5, self.v_factor*.5
	local l,r = .5-suh, .5+suh
	local b,t = .5-svh, .5+svh
	self.uv		= { 	l,t,			l,b,
						.5,t,			.5,b,
						r,t,			r,b			}
end
-- generate points for 3 triangles as fan
--		1_____0_____4
--		 \   / \   /
--		  \ /   \ /
--		   2_____3
function meu:generate_points_fan( x,y,z, size )
	local s  = size
	local sh  = s / 2	-- size half
	self.points = { 	x,   y+sh,z,
						x-s, y+sh,z,	x-sh,y-sh,z,
						x+sh,y-sh,z,  	x+s, y+sh,z		}
	local suh, svh = self.u_factor*.5, self.v_factor*.5
	local l,r = .5-suh, .5+suh
	local b,t = .5-svh, .5+svh
	self.uv		= { 	.5,t,
						l,t,			.5+(l-.5)/2,b,
						.5+(r-.5)/2,b,	r,t				}
end
-- generate points and uv for 2 quads
--		0___3   4___7
--		|   |   |   |
--		|   |   |   |
--		1___2   5___6
function meu:generate_points_quad( x,y,z, size )
	local s  = size
	local sh  = s / 2	-- size half
	local sq  = s / 4	-- size quarter
	self.points = {	x-s, y+sh,z,	x-s, y-sh,z,	x-sq,y-sh,z,	x-sq,y+sh,z,
					x+sq,y+sh,z,	x+sq,y-sh,z,	x+s, y-sh,z,	x+s, y+sh,z	}
	local suh, svh = self.u_factor*.5, self.v_factor*.5
	local l,r = .5-suh, .5+suh
	local b,t = .5-svh, .5+svh
	self.uv		= { l,t,			l,b,			r,b,			r,t,
					l,t,			l,b,			r,b,			r,t	}
end

--this is for the old school way (OpenGL 1.0) to draw using begin/end 
-- we avoid repeating this loop in every caller method  
function meu:draw_vertices_3d()
	local pts = self.points
	local b_uv = self.b_uv
	local uv = self.uv 
	local i = 1
	local i_uv = 1
	for loop_index = 1,#pts / 3 do
		if b_uv then
			gol.texcoor2( uv[i_uv], uv[i_uv+1] )
			i_uv = i_uv + 2
		end
		gol.vertex3( pts[i], pts[i+1], pts[i+2] )
		i = i + 3
	end

end

-- GL_POINTS
function meu:draw_points()
	if self.b_fast then	 -- more efficient/modern method: less lua calls, less opengl calls 
		if self.b_uv then
			gol.set_attrib_uv( self.uv )
			gol.draw_points_uv_3d( self.points )
		else
			gol.draw_points_3d( self.points )
		end
	else	 -- old school method similar to opengl 1.0
		gol.begin_points()
			self:draw_vertices_3d( self.points, self.uv )
		gol.do_end()
	end
end
-- GL_LINES
function meu:draw_lines()
	if self.b_fast then	 -- more efficient/modern method: less lua calls, less opengl calls 
		if self.b_uv then
			gol.set_attrib_uv( self.uv )
			gol.draw_lines_uv_3d( self.points )
		else
			gol.draw_lines_3d( self.points )
		end
	else	 -- old school method similar to opengl 1.0
		gol.begin_lines()
			self:draw_vertices_3d( self.points, self.uv )
		gol.do_end()
	end
end
-- GL_LINE_STRIP
function meu:draw_line_strip()
	if self.b_fast then	 -- more efficient/modern method: less lua calls, less opengl calls 
		if self.b_uv then
			gol.set_attrib_uv( self.uv )
			gol.draw_line_strip_uv_3d( self.points )
		else
			gol.draw_line_strip_3d( self.points )
		end
	else	 -- old school method similar to opengl 1.0
		gol.begin_line_strip()
			self:draw_vertices_3d( self.points, self.uv )
		gol.do_end()
	end
end
-- GL_LINE_LOOP
function meu:draw_line_loop()
	if self.b_fast then
		if self.b_uv then
			gol.set_attrib_uv( self.uv )
			gol.draw_line_loop_uv_3d( self.points )
		else
			gol.draw_line_loop_3d( self.points )
		end
	else	 -- old school method similar to opengl 1.0
		gol.begin_line_loop()
			self:draw_vertices_3d( self.points, self.uv )
		gol.do_end()
	end
end
-- GL_TRIANGLES
function meu:draw_triangles()
	if self.b_fast then -- more efficient/modern method: less lua calls, less opengl calls 
		if self.b_uv then
			gol.set_attrib_uv( self.uv )    -- 6 couples of uv coordonates for 6 points
			gol.draw_triangles_uv_3d( self.points )
		else
			gol.draw_triangles_3d( self.points )       -- here the number of points correspond to the array ysize
			-- direct arguments
			--	local pt = self.points
			--	gol.draw_triangles_3d(	pt[1], pt[2], pt[3],	pt[4], pt[5], pt[6],	pt[7], pt[8], pt[9],
			--								pt[10],pt[11],pt[12],	pt[13],pt[14],pt[15],	pt[16],pt[17],pt[18]
			-- or
			--	gol.draw_triangles_3d( pt, 3 )  -- here the number of points is explicitly passed : 1 triangle -> 3 points
			--	gol.draw_triangles_3d( self.points, 6 )  -- here the number of points is explicitly passed : 2 triangles -> 6 points
			--	gol.draw_triangles_uv_3d( pt, 3, 2 )  -- 1 triangle (3 points) but with an offset of 2 in the points array
		end
	else	-- old school method similar to opengl 1.0
		gol.begin_triangles()
			self:draw_vertices_3d( self.points, self.uv )
		gol.do_end()
	end
end
-- GL_TRIANGLE_STRIP
function meu:draw_triangle_strip()
	if self.b_fast then -- more efficient/modern method: less lua calls, less opengl calls 
		if self.b_uv then
			gol.set_attrib_uv( self.uv )
			gol.draw_triangle_strip_uv_3d( self.points )
		else
			gol.draw_triangle_strip_3d( self.points )
		end
	else	-- old school method similar to opengl 1.0
		gol.begin_triangle_strip()
			self:draw_vertices_3d( self.points, self.uv )
		gol.do_end()
	end
end
-- GL_TRIANGLE_FAN
function meu:draw_triangle_fan()
	if self.b_fast then -- more efficient/modern method: less lua calls, less opengl calls 
		if self.b_uv then
			gol.set_attrib_uv( self.uv )
			gol.draw_triangle_fan_uv_3d( self.points )
		else
			gol.draw_triangle_fan_3d( self.points )
		end
	else	-- old school method similar to opengl 1.0
		gol.begin_triangle_fan()
			self:draw_vertices_3d( self.points, self.uv )
		gol.do_end()
	end
end
-- GL_QUADS
function meu:draw_quads()
	if self.b_fast then -- more efficient/modern method: less lua calls, less opengl calls 
		if self.b_uv then
			gol.set_attrib_uv( self.uv )
			gol.draw_quads_uv_3d( self.points )
		else
			gol.draw_quads_3d( self.points ) 
		end
	else	-- old school method similar to opengl 1.0
		gol.begin_quads()
			self:draw_vertices_3d( self.points, self.uv )
		gol.do_end()
	end
end
-- GL_QUAD_STRIP
function meu:draw_quad_strip()
	if self.b_fast then -- more efficient/modern method: less lua calls, less opengl calls 
		if self.b_uv then
			gol.set_attrib_uv( self.uv )
			gol.draw_quad_strip_uv_3d( self.points )
		else
			gol.draw_quad_strip_3d( self.points ) 
		end
	else	-- old school method similar to opengl 1.0
		gol.begin_quad_strip()
			self:draw_vertices_3d( self.points, self.uv )
		gol.do_end()
	end
end
-- GL_POLYGON (only old school method)
function meu:draw_polygon( b_fast )
	-- polygon mode only exist in old school mode (2026 March) perhaps it will be removed
	-- if b_fast then -- more efficient/modern method: less lua calls, less opengl calls 
	-- 	if self.b_uv then
	-- 		gol.set_attrib_uv( self.uv_fan )    -- 6 couples of uv coordonates for 6 points
	-- 		gol.draw_polygon_uv_3d( self.points_fan )
	-- 	else
	-- 		gol.draw_polygon_3d( self.points_fan )       -- here the number of points correspond to the array ysize
	-- 	end
	-- else
	 -- old school method similar to opengl 1.0
		gol.begin_polygon()
			self:draw_vertices_3d( self.points, self.uv )
		gol.do_end()
	-- end
end

-- arg what select the primitive used to draw
function meu:draw_primitive( what, x,y,z, size )
	-- first we generate point and uv
	local generate_method_name = "generate_"..self.fn_compute[what]
	--self:print( what.." -> Generate "..generate_method_name )
	self[generate_method_name]( self, x,y,z, size )

	-- second we draw
	-- we could have make these 10 fns more compact but we wanted to havethese 10 explicit examples 
	local draw_method_name = "draw_"..self.fn_draw[what]	
	--self:print( what.." -> Draw "..draw_method_name )
	self[draw_method_name]( self, self.b_fast )
end

function meu:draw_primitive_all( ox,oy,oz, size )
	local mar = size * .25
	local dx = size * 2 + mar
	local x_begin = ox - dx
	local dy = size + mar
	local z = oz
	local y = oy + dy * 1.5
	local x = x_begin
	self:draw_primitive(     1,	  x+dx,  y,z, size )
	y = y - dy
	for i=2,10,3 do
		self:draw_primitive( i,   x,     y,z, size )
		self:draw_primitive( i+1, x+dx,  y,z, size )
		self:draw_primitive( i+2, x+dx*2,y,z, size )
		y = y - dy
	end
end

function meu:draw()
	--aaa.show( self.selec_prim, "select primitive" ) -- debug helper
	self:draw_layers_begin()
		self:draw_layer( 1 )    -- set the attributes using Layer A				
		gol.color_white( self:get_alpha() )
		self.b_uv = self:get_texture_dim() ~= 0
		if self.b_gol_test then
			self:draw_primitive_all( self.x,self.y,self.z, self.size )
		else
			self:draw_primitive( self.id_primitive_selected, self.x,self.y,self.z, self.size )
		end
	self:draw_layers_end()
end