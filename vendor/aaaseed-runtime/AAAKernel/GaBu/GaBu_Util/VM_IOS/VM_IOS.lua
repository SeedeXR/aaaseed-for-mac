
if not aaa.b_ios then
	return
end

aaa.doit "dior_vm_init"
aaa.doit "dior_vm_main"

aaa.lua.global.declare( "ga" )
aaa.lua.global.declare( "dior" )
local dior = APP.CREATE_INST( DIOR )
dior.CAPTURE_SIZE_X = 720
dior.CAPTURE_SIZE_Y = 1280
--VM.__singleton = vm
--function VM:get() return VM.__singleton end

dior:print( "dior is here, dior init begin" )

function dior:need_implement()
	self:print( "we need to implement this" )
end
--MESS
--PRINT
--BOX
function aaa.box_error(...)		dior:print( "_____  BOX ERROR  _____" )	dior:print(...)	end
function aaa.box_warning(...)	dior:print( "_____ BOX WARNING _____" )	dior:print(...)	end
function aaa.box_good(...)		dior:print( "_____  BOX GOOD   _____" )	dior:print(...)	end

--PARAM
--
if not aaa.param then	aaa.param = {}	end

aaa.lua.global.set( "param", aaa.param )

function param.get()		end
function param.get_ref()	end
function param.set()		end

--OBJ
--
function aaa.obj.get()				end
function aaa.obj.update_then_draw() end

--LUA
--
function aaa.lua.set_file_force_recheck( b ) end

if not aaa.time then		aaa.time = {}		end

if not aaa.stereo then		aaa.stereo = {}		end
function aaa.stereo.is_active() return false end

if not aaa.flatland then	aaa.flatland = {}	end
function aaa.flatland.set_draw() end

if not aaa.keyboard then	aaa.keyboard = {}	end
function aaa.keyboard.is_ctrl()		return false	end
function aaa.keyboard.is_shift()	return false	end
function aaa.keyboard.is_alt()		return false	end


--DRAW
--
-- first version of functions to implement
function aaa.draw_mul_line(	x, y, sx, sy )
	aaa.draw_line( x-sx, y-sy, x+sx, y+sy )
	aaa.draw_line( x-sx, y+sy, x+sx, y-sy )
end
function aaa.draw_plus_line(	x, y, sx, sy )
	aaa.draw_line( x, y-sy, x, y+sy )
	aaa.draw_line( x-sx, y, x+sx, y )
end

--function aaa.draw_line( u1, v1, u2, v2, axe )
function aaa.draw_line( x1, y1, x2, y2 )
	local t = { {x1,y1,0}, {x2,y2,0} }
	gol.draw_line_strip( t )
end
--	aaa.draw_null( x, y, z, size )	-- default size is 1
--	aaa.draw_null( x, y, z )		-- aaa.draw_null( x, y, z, 1 )
--	aaa.draw_null( size )			-- aaa.draw_null( 0, 0, 0, size )
--	aaa.draw_null( )				-- aaa.draw_null( 0, 0, 0, 1 )
function aaa.draw_null( x, y, z, size )
	--todo add z really do
	 aaa.draw_mul_line(	x, y, size, size )
end

function aaa.draw_rect_line( min_x, min_y, max_x, max_y )
	local t = { {min_x,min_y,0}, {max_x,min_y,0}, {max_x,max_y,0}, {min_x,max_y,0} }
	gol:draw_line_loop( t )
end


--local __quad_uv
gol.__quad_ind = { 0,2,1, 1,2,3 }

--	use gol.set_quad_uv( u_min, v_min, u_size, v_size ) to set uv
function aaa.draw_triangle_strip_4xy_uv( x0, y0, x1, y1, x2, y2, x3, y3 )
	local vertex = { {x0,y0,0}, {x1,y1,0}, {x2,y2,0}, {x3,y3,0} }
	gol.draw_triangles_uv( gol.__quad_ind, vertex, gol.__quad_uv )
end
--	use gol.set_quad_uv( u_min, v_min, u_size, v_size ) to set uv
--	aaa.draw_rect_uv_at_z	(	min_x, min_y, max_x, max_y, pos_z )
function aaa.draw_rect_uv( min_x, min_y, max_x, max_y )
	local vertex = {  {min_x,max_y,0.}, {min_x,min_y,0.}, {max_x,max_y,0.}, {max_x,min_y,0.} }
	gol.draw_triangles_uv( gol.__quad_ind, vertex, gol.__quad_uv )
end

if not aaa.img then		aaa.img = {}	end
--	b = aaa.img.set_lua_cur( index )		--	avoid passing index every time, very useful in loops
											--		once set pass a nil index to use it
											--	return true for a non empty image
function aaa.img.set_lua_cur( index )
--	dior:need_implement()
--	aaa.print_fn()
end

--r,g,b,a = aaa.img.get_color_uv(				index, u, v,	b_clamped )				-- get pixel color at uv
function aaa.img.get_color_uv( index, u, v,	b_clamped )
	--todo generalize
	local x = clamp( dior.CAPTURE_SIZE_X*u, 0, dior.CAPTURE_SIZE_X-1 )
	local y = clamp( dior.CAPTURE_SIZE_Y*v, 0, dior.CAPTURE_SIZE_Y-1 )
	--aaa.print( x.." x "..y )
	local r,g,b = capture.get_pixel( y, x )
	return r/255., g/255., b/255., 1.
end

if not aaa.math then	aaa.math = {}	end
--	x,y,z = aaa.math.do_catmull_rom_xyz				( s, ax,ay,az, bx,by,bz, cx,cy,cz, dx,dy,dz )
--	x,y,z = aaa.math.do_catmull_rom_xyz				( s )		--compute with the previous points
function aaa.math.do_catmull_rom_xyz( s, ax,ay,az, bx,by,bz, cx,cy,cz, dx,dy,dz )
	dior:need_implement()
	aaa.print_fn()
end


--GOL
--
function gol.begin_triangles()
	gol.__indices = {}
	gol.__vertex = {}
	gol.__uv = {}
end

function gol.do_end()
	gol.draw_triangles_uv( gol.__indices, gol.__vertex, gol.__uv )
end
function gol.draw_uv_xy( u,v, x,y )
	table.insert( gol.__vertex, 	{x,y,0.}			)
	table.insert( gol.__uv,			{u,1.-v} 			)
	table.insert( gol.__indices,	#(gol.__indices)	)
end

--gol.draw_line_strip_2d(	x,y,	x,y		[	,x,y	] )
--function gol.draw_line_strip_2d( ... )
--	print_need_implement()
--	aaa.print_fn()
--end

--gol.draw_line_loop_2d(	x,y,	x,y 	[	,x,y	] )
function gol.draw_line_loop_2d( ... )
	local t = {}
	for i=1,select("#", ...),2 do
		table.insert( t, { select(i,...), select(i+1,...), 0. } )
	end
	gol.draw_line_loop( t )
end

gol.__color = { 1,1,1,1 }
function gol.color3( ... )	gol.__color = { ..., 1. }	end
function gol.color4( ... )	gol.__color = { ... }		end

function gol.color( ... )
	local arg_nb = select("#", ...)
	if arg_nb == 1 then
		local arg = select(1, ...)
		if type(arg) == "table" then
			gol.__color = arg
		else
			gol.__color = { arg, arg, arg, 1. }
		end
	elseif arg_nb == 3 then
		gol.__color = { ..., 1. }
	elseif arg_nb == 4 then
		gol.__color = { ... }
	else
		error( "gol.color() should have 1,3 or 4 arguments" )
	end
end

--[[
gol.__bind = { 0,0,0,0 }
function gol.bind_tex2d( self.texture, tex_unit, loc )
function gol.bind_tex2d( bind, tex_unit )
	gol.__bind[(tex_unit or 0) + 1] = bind
end
--]]

function gol.set_tex_dim( dim )			gol.__texture_dim = dim		end
function gol.set_texture_dim( dim )		gol.set_tex_dim( dim )		end
gol.set_texture_dim( 0 )

gol.__bind_tex2d = gol.bind_tex2d
gol.bind_tex2d = nil
function gol.bind_texture( bind, tex_unit )
	gol.__bind_tex2d( bind, tex_unit or 0 )
end

gol.__draw_element = gol.draw_element
function gol.draw_element( element )
	local loc = SHADING.cur.loc
	gol.__draw_element( element, loc.vertex, loc.uv )
end

gol.__draw_line_loop = gol.draw_line_loop
function gol.draw_line_loop( vertex )
	local loc = SHADING.cur.loc
	gol.__draw_line_loop( loc.vertex, vertex )
end

gol.__draw_line_strip = gol.draw_line_strip
function gol.draw_line_strip( vertex )
	local loc = SHADING.cur.loc
	gol.__draw_line_strip( loc.vertex, vertex )
end

function gol.draw_triangles_uv( indice, vertex, uv )
	local elem = gol.create_element( indice, vertex, uv )
	local loc = SHADING.cur.loc
	gol.draw_element( elem, loc.vertex, loc.uv )
	gol.delete_element( elem )
end


--	gol.set_quad_uv( u_min, v_min,  u_max, v_max )			--		13	max
--	gol.set_quad_uv( u0, v0,  u1, v1,  u2, v2,  u3, v3 )	--	min 02
--	gol.set_quad_uv()	--	do gol.set_quad_uv( 0, 0,  1, 1 )
function gol.set_quad_uv( u0,v0,  u1,v1,  u2,v2,  u3,v3 )
	if not u2 then
		if not u0 then u0,v0, u1,v1 = 0,0, 1,1 end
		u2,v2 = u1,v0
		u3,v3 = u1,v1
		u1 = u0
	end
	gol.__quad_uv = { {u0,v0},  {u1,v1},  {u2,v2},  {u3,v3} }
end
gol.set_quad_uv()


dior:print( "___________ vm init END" )


--[[
	aaa.doc.box_error = ""
	aaa.doc.box_warning = ""
	aaa.doc.box_good = ""
	if not aaa.param then	aaa.param = {}	end
	aaa.lua.global.set( "param", aaa.param )
	param.doc.get = ""
	param.doc.get_ref = ""
	param.doc.set = ""
	aaa.doc.get_obj = ""
	aaa.doc.update_then_draw = ""
	function aaa.lua.set_file_force_recheck( b ) end
	if not aaa.time then		aaa.time = {}		end
	if not aaa.stereo then		aaa.stereo = {}		end
	function aaa.stereo.is_active() return false end
	if not aaa.flatland then	aaa.flatland = {}	end
	function aaa.flatland.set_draw() end
	if not aaa.keyboard then	aaa.keyboard = {}	end
	function aaa.keyboard.is_ctrl()		return false	end
	function aaa.keyboard.is_shift()	return false	end
	function aaa.keyboard.is_alt()		return false	end
	aaa.doc.draw_mul_line = ""
	aaa.doc.draw_line = ""
	aaa.doc.draw_null = ""
	aaa.doc.draw_rect_line = ""
	gol.__quad_ind = { 0,2,1, 1,2,3 }
	aaa.doc.draw_triangle_strip_4xy_uv = ""
	aaa.doc.draw_rect_uv = ""
	if not aaa.img then		aaa.img = {}	end
	function aaa.img.set_lua_cur( index )
	function aaa.img.get_color_uv( index, u, v,	b_clamped )
	if not aaa.math then	aaa.math = {}	end
	function aaa.math.do_catmull_rom( s, ax,ay,az, bx,by,bz, cx,cy,cz, dx,dy,dz )
	gol.doc.begin_triangles = ""
	gol.doc.do_end = ""
	gol.doc.draw_uv_xy = ""
	gol.doc.draw_line_loop_2d = ""
	gol.__color = { 1,1,1,1 }
	gol.doc.color3 = ""
	gol.doc.color4 = ""
	gol.doc.color = ""
	gol.__bind = { 0,0,0,0 }
	gol.doc.bind_tex2d = ""
	gol.doc.bind_tex2d = ""
	gol.doc.set_tex_dim = ""
	gol.doc.set_texture_dim = ""
	gol.set_texture_dim( 0 )
	gol.__bind_tex2d = gol.bind_tex2d
	gol.bind_tex2d = nil
	gol.doc.bind_texture = ""
	gol.__draw_element = gol.draw_element
	gol.doc.draw_element = ""
	gol.__draw_line_loop = gol.draw_line_loop
	gol.doc.draw_line_loop = ""
	gol.__draw_line_strip = gol.draw_line_strip
	gol.doc.draw_line_strip = ""
	gol.doc.draw_triangles_uv = ""
	gol.doc.set_quad_uv = ""
	gol.set_quad_uv()
dior:print( "___________ vm init END" )
--]]