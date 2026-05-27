--todo add threshold
--todo add a "same for map and depth or reverse (generalize too)
function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "Art", "2d", "3d", "draw", "procedural", "vj" },
			help = 	{
						"the Romzy MEU take a texture and represent it as a grid of pixel of different shapes,",
						"  displaced in function of their luminosity",
					}
			}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local ix = 1
	local iy = 1
	local SY = .8
	local DY = .5

	self:add_camera()
	self:add_rendering()

	self:set_tab_key_def()

	local SYT = 4
				self:add_bu_texture_target_unit(	{ix,iy,		4,SYT}, "Src",		2, false	)
	ui.map =	self:add_bu_texture_target_unit(	{ix+4,iy,	4,SYT}, "Map",		1 )
	iy = iy + SYT + DY

	self:add_slider( 		{ix,iy,			4,SY},		"sf",		self, "size",	4, 0,8 )
		:add_values_def( 1, 2, 3, 4, 4.5, 5, 6, 7 )
	self:add_slider( 		{ix+4,iy,		2,SY},		"su",		self, "size_u_ui",	1, 0,8 ):set_color_back("u")
		:add_values_def( .5, 1, 2, 3, 4, 4.5, 5, 6, 7, 9 )
	self:add_slider( 		{ix+6,iy,		2,SY},		"sv", 		self, "size_v_ui",	1, 0,8 ):set_color_back("v")
		:add_values_def( .5, 1, 2, 3, 4, 4.5, 5, 6, 7, 9 )
	iy = iy + SY

	local SX = 8/3
	self:add_slider(		{ix,iy,			SX,SY},		"Step U",	self,	"step_u",	32, 1,256 )
		:set_color_back("u")
		:set_value_type_integer( true )
	bu = self:add_button(	{ix+SX,iy,		8-SX*2,SY},	"Step V How", nil, nil,1 )
 		bu:set_menu( { "Same", "Square", "Free" } )
		bu:set_text_selector(true)
		bu:set_target_lua( self, "s_step_v" )
	self:add_slider(		{ix+8-SX,iy,	SX,SY},		"Step V",	self,	"step_v",	32, 1,256 )
		:set_color_back("v")
		:set_value_type_integer( true )
	iy = iy + SY

	bu = self:add_selector(	{ix,iy,			8,SY},		"How" )
		bu:set_item_text( 1, "No", "Center", "Cross", "Rot", "Vilder", "Tissage" )
		bu:set_target_lua( self, "s_how" )
		bu:set_text_draw( false )
	iy = iy + SY

	self.elt = {}
	local function add_range( x,y, sx,sy, str, l )
		local l = l or string.lower(str)
		self:add_button(		{x,y,		sy,sy},		str.."_active",		self.elt,	"b_"..l,				true	)
			:set_text_visible(false)
		self:add_slider_two(	{x+sy,y,	sx-sy,sy},	"Range "..str,		self.elt,	l.."_min",l.."_max",	0,1, 0,1 )
			:set_color_back( l )
	end
	add_range( ix,iy,	4,SY, "U" )
	add_range( ix+4,iy, 4,SY, "V" )
	iy = iy + SY
	add_range( ix,iy, 8,SY,   "AXE", "a" )
	iy = iy + SY + DY
		self.nb_col = 5
		bu = self:add_selector(	{ix,iy,		8,SY*2},	"Prim" )
		bu:set_item_text( 1, "Line",	"Cercle",	"Ellipse\nLine",	"Sphere",	"Test",
							 "Rect",	"Disk",		"Ellipse",			"Box",		"Grid"	)
		bu:set_target_lua( self, "s_prim" )
		bu:set_nb( self.nb_col, 2 )
		bu:set_text_draw( false )
	iy = iy + SY*2 + DY

	local sx = 8/3
	self:add_button(		{ix,iy,			sx,SY},	"Debug",		self, "b_debug_draw",			false	)
	self:add_button(		{ix+sx,iy,		sx,SY},	"Circle",		self, "b_debug_draw_circle",	false	)
	self:add_button(		{ix+sx*2,iy,	sx,SY},	"Z",			self, "b_debug_draw_z",			false	)
--	bu = self:add_slider(	{ix,iy,			8,SY},	"threshold",	self, "threshold",				0.001,	0.001, 1 )
	iy = iy + SY + DY

	ix, iy = 9, 2 + DY

	self:add_transfo( {ix,iy,	8,2.4} ) 
	iy = iy + 2.4 + DY
	
	ui.color = self:add_rgbfa(	{ix,iy,	8,1.5} )
	iy = iy + 1.5 + DY

	bu = self:add_selector(	{ix,iy,		8,SY},	"Input" )
		bu:set_item_text( 1, "Bias", "Spiral", "Tex" )
		bu:set_target_lua( self, "s_signal" )
		bu:set_text_draw( false )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,SY},	"Speed",		self,	"speed",		1, -8, 8 )
		bu:set_meter( false )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,SY},	"freq",			self,	"freq",			1, 0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		4,SY},	"Gain",			self,	"gain",			.5, 0, 1 )
	bu = self:add_slider(	{ix+4,iy,	4,SY},	"Bias",			self,	"bias",			.5, 0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,SY},	"spiral Nb",	self,	"spiral_nb",	1, 0, 16 )
		bu:set_value_type_integer( true )
	iy = iy + SY + DY

	
	self:add_trig_method(	{ix,iy,		sx,SY}, "Test", 		self, "do_test" )
	iy = iy + SY + DY
	

--	self:set_tab_key( "Map" )
--	ix, iy = 1, 3
end

function meu:init()
	local ref = self.ref
	self.ph = 0
	ref.grid		= self:get_layer_bdd(2)
	ref.grid_nb_u	= param.get_ref( ref.grid, "nb_u" )
	ref.grid_nb_v	= param.get_ref( ref.grid, "nb_v" )

end

function meu:do_test()
	local t = {}
	table.print( t, "before", 2 )
	local function fn( i ) 
		aaa.bdd.__do_dev_test( t, i )
		table.print( t, "after "..i, 2 )
	end
 	fn( 1 )
	fn( 2 )
	-- fn( 9 )
	-- fn( 3 )
	-- fn( 2 )     
end

function meu:update()
	self.r, self.g, self.b, self.a = self.ui.color:get_rgba()
	self.b_color_map = self.ui.map:get_dim() == 2
end

function meu:get_signal( u,v )
	local val
	if self.s_signal == 2 then	-- Spiral
		local x = (.5-v) * self.size_v
		local z = (u-.5) * self.size_u

		local angle = math.atan2( x,z ) * self.spiral_nb * .5
		local di = math.sqrt(x*x+z*z ) * self.freq * 8 + angle
		val = (math.sin( di * 2. + self.ph ) + 1 ) * .5
		val = aaa.math.gain_bias( val, self.gain, self.bias )
		if self.b_color_map then
		 	gol.color( val )
		end
	elseif self.s_signal == 1 then	-- Bias
		val = self.bias
	elseif self.bind then	-- Texture
		local r,g,b,a = aaa.img.get_color_uv( self.bind, u,v, true )
		if self.b_color_map then
			gol.color( r*self.r,g*self.g,b*self.b, a*self.a )
		end
		if r then
			--val = (r+g+b)*.33
			val = r				
			--gol.color( r,g,b, 1 )
			--gol.color( r,0,0, 1 )
			--local h = self.elt_sa * (r+g+b)*.33
			--aaa.draw_box( fx, h, fz,	x, h*.5, z )
		else
			val = 0
		end
		val = aaa.math.gain_bias( val, self.gain, self.bias )
	else
		val = self.bias
	end
	return val
end

function meu:__get_offset_and_factor()
	local e = self.elt
	local uo,uf = e.u_min, e.u_max-e.u_min
	local vo,vf = e.v_min, e.v_max-e.v_min
	local ao,af = e.a_min, e.a_max-e.a_min

	if not e.b_u then		uo,uf = 0,1		end
	if not e.b_v then		vo,vf = 0,1		end
	if not e.b_a then		ao,af = 0,1		end
	return uo,uf, vo,vf, ao,af
end

local fns_draw  =
{
	[1] = function( sx,sy,sz, x,y,z )	aaa.draw_rect_line_axe_z( 		x,y,z, sx,sy ) end,
	[6] = function( sx,sy,sz, x,y,z )	aaa.draw_rect_uv_axe_z(			x,y,z, sx,sy ) end,
	[2] = function( sx,sy,sz, x,y,z )	aaa.draw_circle_axe_z(			x,y,z, sx,24 ) end,
	[7] = function( sx,sy,sz, x,y,z )	aaa.draw_disk_axe_z(			x,y,z, sx,24 ) end,
	[3] = function( sx,sy,sz, x,y,z )	aaa.draw_ellipse_line_axe_z(	x,y,z, sx,sy,24 ) end,
	[8] = function( sx,sy,sz, x,y,z )	aaa.draw_ellipse_axe_z(			x,y,z, sx,sy,24 ) end,
	[9] = aaa.draw_box,
	[4] = function( sx,sy,sz, x,y,z )	aaa.draw_sphere_xyz(			x,y,z, sx, 6,6 ) end,
	[5] = function( sx,sy,sz, x,y,z )	gol.vertex(						x,y,z ) end,
}

function meu:my_draw( step_u,step_v, size_u,size_v )
-- 	local function draw_sphere( sx,sy,sz, x,y,z )	aaa.draw_sphere_xyz( x,y,z, sx, 6,6 )		end
-- 	local function draw_circle( sx,sy,sz, x,y,z )	aaa.draw_circle_axe_y( x,y,z, sx, 24 )		end
-- 	local function draw_disk( sx,sy,sz, x,y,z )		aaa.draw_disk_axe_y( x,y,z, sx, 24 )		end
-- 	local function draw_ellipse( sx,sy,sz, x,y,z )	aaa.draw_ellipse_axe_y( x,y,z, sz,sx, 24 )	end
-- 	local function draw_rect( sx,sy,sz, x,y,z )		aaa.draw_rect_uv_axe_y( x,y,z, sz,sx )		end
-- --	local fn_draw = aaa.draw_box
-- 	local fn_draw = draw_sphere

--todo redo bdd_src
	local ref = self.ref

	local fn_draw = fns_draw[self.s_prim] or fns_draw[1]

	--self:print( step_u.." x "..step_v )
	local b_feed_bdd = (self.s_prim == self.nb_col + 5)
	if b_feed_bdd then
		param.set( ref.grid_nb_u, step_u )
		param.set( ref.grid_nb_v, step_v )
		aaa.obj.update( ref.grid )
		gol.set_texture_dim(2)
	else
		gol.set_texture_dim(0)
	end

	local bdd_src = nil --GA.__flex_bdd_last
	local nb_u_src,nb_v_src
	if bdd_src then
		nb_u_src = param.get( bdd_src, "nb_u" )
		nb_v_src = param.get( bdd_src, "nb_v" )
	end

	local uo,uf, vo,vf, ao,af = self:__get_offset_and_factor()

	--self:print( uo.." x "..uf.." "..vo.." x "..vf )
	if inside(self.s_how,4,5) then uo,uf = 0,1		end

	local fiu = 1 / (step_u-1)
	local fiv = 1 / (step_v-1)

	local du = math.max( size_u * fiu, .0001 )
	local dv = math.max( size_v * fiv, .0001 )
	--self:print( du.." x "..dv )

	local b_flip_x = false
--	for x = -size_v*.5, size_v*.5, dv do
	for iv = 0,step_v-1 do
		local v =  iv*fiv 
		local y = (v-.5) * size_v
		if self.s_prim == 5 then
			gol.begin_line_strip()	
		end
		local b_flip_z = false
--		for z = -size_u*.5, size_u*.5, du do

		for iu = 0,step_u-1 do
			local u =  iu*fiu
			local x = (u-.5) * size_u
			local su,sv,sa
			local val 
			
			if bdd_src then
				val = 1
				x,y = aaa.bdd.get_point( bdd_src, iv * nb_u_src + iu + 1 )
			else
				val = self:get_signal( u,v )
			end

			sa = ao + val*af
			local z = sa * .5
			--self:print(y)
			su,sv = uo+val*uf, vo+val*vf

			if self.b_debug_draw then
				local lz = self.b_debug_draw_z and z or 0
				if self.b_debug_draw_circle then
					aaa.draw_ellipse_line_axe_z	(	x,y,lz, du,dv, 24 )	
				else
					aaa.draw_rect_line_axe_z( 		x,y,lz, du,dv )
				end
			end

			if b_feed_bdd then
				--todo it seems (120213 November that bdd_uv don't draw in the right order)
				local id = iu * step_v + iv + 1
				--local px,py,pz = aaa.bdd.get_point( ref.grid, id )
				--if id < 320 then
					aaa.bdd.set_point( ref.grid, id, x,y,z )
				--end
			else
				if self.s_how == 2 then
					fn_draw( su*du, sv*dv, sa,		x,y,z )
				else
					if self.s_how == 4 then
						if b_flip_x == b_flip_z then			fn_draw( sv*dv, su*du, sa, 	x, y, z )
						else									fn_draw( su*dv, sv*du, sa, 	x, y, z )
						end
					elseif self.s_how == 5 then
						if b_flip_x then	if b_flip_z then	fn_draw( sv*dv, su*du, sa, 	x+(sv-1)*dv*.5,	y, z )
											else				fn_draw( su*dv, sv*du, sa, 	x, 				y, z+(1-sv)*du*.5 )
											end
						else				if b_flip_z then	fn_draw( su*dv, sv*du, sa, 	x, 				y, z+(sv-1)*du*.5 )
											else				fn_draw( sv*dv, su*du, sa, 	x+(1-sv)*dv*.5,	y, z )
											end
						end		
					elseif self.s_how == 3 then
						fn_draw( su*dv, sv*du, sa, 	x, y, z )
						fn_draw( sv*dv, su*du, sa, 	x, y, z )
					else
					end
				end
			end

			b_flip_z = not b_flip_z
		end	--	for iu = 0,step_u-1 do

		b_flip_x = not b_flip_x
		if self.s_prim == 5 then
			gol.do_end()
		end
	end	--	for iv = 0,step_v-1 do

	if b_feed_bdd then
		self:draw_layer( 2 )
	end	
end

if false then
	function meu:get_def( z,x )
		local Z,X = .5,0
		local S = 1
		local g = .5
		local b = .5
		if z < Z then
			if z < Z-S then
				return 0,0
			else
				local z = (Z-S-z)/S
				z = 1. - math.abs( z + .5 ) * 2
				return -aaa.math.gain_bias(z,g,b),0	
			end
		else
			if z > Z+S then
				return 0,0
			else
				local z = (Z+S-z)/S
				z = 1. - math.abs( z - .5 ) * 2
				return aaa.math.gain_bias(z,g,b),0 	
			end
		end	
	end
end

function meu:draw_weaving( step_u,step_v, size_u,size_v )
	--self:print( "step u/v "..step_u.."/"..step_v )
	-- 	local function draw_sphere( sx,sy,sz, x,y,z )	aaa.draw_sphere_xyz( x,y,z, sx, 6,6 )		end
	-- 	local function draw_circle( sx,sy,sz, x,y,z )	aaa.draw_circle_axe_y( x,y,z, sx, 24 )		end
	-- 	local function draw_disk( sx,sy,sz, x,y,z )		aaa.draw_disk_axe_y( x,y,z, sx, 24 )		end
	-- 	local function draw_ellipse( sx,sy,sz, x,y,z )	aaa.draw_ellipse_axe_y( x,y,z, sz,sx, 24 )	end
	-- 	local function draw_rect( sx,sy,sz, x,y,z )		aaa.draw_rect_uv_axe_y( x,y,z, sz,sx )		end
	-- --	local fn_draw = aaa.draw_box
	-- 	local fn_draw = draw_sphere
	local ref = self.ref

	local b_feed_bdd = (self.s_prim == self.nb_col + 5)
	if b_feed_bdd then
		param.set( ref.grid_nb_u, step_v )
		param.set( ref.grid_nb_v, step_u )
		aaa.obj.update( ref.grid )
	end

	local bdd_src = GA.__flex_bdd_last
	local nb_u_src,nb_v_src
	if bdd_src then
		nb_u_src = param.get( bdd_src, "nb_u" )
		nb_v_src = param.get( bdd_src, "nb_v" )
	end

	local uo,uf, vo,vf, ao,af = self:__get_offset_and_factor()

	local fiu = 1. / (step_u-1)
	local fiv = 1. / (step_v-1)

	local du = math.max( size_u * fiu, .0001 )
	local dv = math.max( size_v * fiv, .0001 )
	--self:print( du.." x "..dv )

	local function draw_weave_one ( sup,suc,sun, svp,svc,svn, x,y,z )
		y = y  * .02
		local su = dv*.5
		sup,suc,sun = sup*su, suc*su, sun*su
		local sv = du*.5
		svp,svc,svn = svp*sv, svc*sv, svn*sv
		--gol.set_quad_uv( 0,0,  1,1 )
		--aaa.draw_rect_uv_axe_y( x,math.abs(y),z, svc*2,suc*2 )
		local b_tex_mirror = (y>0)
		if b_tex_mirror then	gol.set_quad_uv( 1,0,  .5,1 )
		else					gol.set_quad_uv( 0,0,  .5,1 )
		end
		aaa.draw_triangle_strip_4xyz_uv(	x-sup,		0,		z-sv,	
								x-suc,		y,		z,		
								x+sup,		0,		z-sv,
								x+suc,		y,		z		)
		aaa.draw_triangle_strip_4xyz_uv(	x-su,		0,		z+svp,
								x,			-y,		z+svc,
								x-su,		0,		z-svp,	
								x,			-y,		z-svc	)

		if b_tex_mirror then	gol.set_quad_uv( .5,0,  0,1 )
		else					gol.set_quad_uv( .5,0,  1,1 )
		end

		aaa.draw_triangle_strip_4xyz_uv(	x-suc,		y,		z,		
								x-sun,		0,		z+sv,	
								x+suc,		y,		z,
								x+sun,		0,		z+sv	)
		aaa.draw_triangle_strip_4xyz_uv(	x,			-y,		z+svc,
								x+su,		0,		z+svn,
								x,			-y,		z-svc,	
								x+su,		0,		z-svn	)
	end

	local b_flip_x = false
--	for x = -size_v*.5, size_v*.5, dv do
	--draw vertical lines
	for iv = 0,step_v-1 do
		local v =  iv*fiv 
		local x_loop = (.5-v) * size_v
		local b_flip_z = b_flip_x
--		for z = -size_u*.5, size_u*.5, du do

		local val
		local nor  = {}
		for iu = 0,step_u-1 do
			local u =  iu*fiu
			local z_loop = (u-.5) * size_u

			local x,z

			-- if true then
			-- 	nor[1],nor[2] = aaa.img.get_gradient_uv( self.bind, u,1-v, 1, true, true )
			-- 	V2.normalize( nor )
			-- 	local r,g,b, a =  aaa.img.get_color_uv( self.bind, u,1-v, true )		
			-- 	local f =.1 * (1-r)
			-- 	x = x_loop + nor[2] * f
			-- 	z = z_loop + nor[1] * f
			-- else
			-- 	local dz,dx = self:get_def( z_loop, x_loop )
			-- 	local f = .2
			-- 	x = x_loop --+ dz * f
			-- 	z = z_loop - dz * f
			-- end
			if bdd_src then
				--val = 1
				x,z = aaa.bdd.get_point( bdd_src, iv * nb_u_src + iu + 1 )
				--self:print( nb_u_src.."/"..nb_v_src )
				--self:print( iu.."/"..iv.." "..x..","..z )
			else
				x = x_loop
				z = z_loop
			end

			if x then
				if self.b_debug_draw then
					gol.set_texture_dim( 0 )
					local ly = self.b_debug_draw_y and y or 0
					if self.b_debug_draw_circle then
						aaa.draw_ellipse_line_axe_y	(	x,ly,z, du,dv, 24 )	
					else
						aaa.draw_rect_line_axe_y( 		x,ly,z, du,dv )
					end
					gol.set_texture_dim( 2 )
				end
				
				local c = self:get_signal( u,v )
				local svc = vo + c*vf
				local suc = uo + c*uf

				local p = (self:get_signal( u-fiu,v ) + c) * .5
				local svp = vo + p*vf
				p = (self:get_signal( u,v+fiv ) + c) * .5
				local sup = uo + p*uf

				local n = (self:get_signal( u+fiu,v ) + c) * .5
				local svn = vo + n*vf
				n = (self:get_signal( u,v-fiv ) + c) * .5
				local sun = uo + n*uf

				--fn_draw( dv, sv*du,	x, y, z )
				local y = b_flip_z and 1 or -1
				draw_weave_one( svp,svc,svn, sup,suc,sun,		x, y, z )
			end
			b_flip_z = not b_flip_z
		end	--	for iu = 0,step_u-1 do
		b_flip_x = not b_flip_x
	end	--	for iv = 0,step_v-1 do
end

function meu:draw()
	self:draw_layers_begin()

		--gol.rotate_y( aaa.time.t * .1 )
		self:draw_layer( 1 )	--draw all layers in layers
		

		local sp = self.speed
		local ph = self.ph + aaa.time.dt * sp
		self.ph = ph
	
		local size = self.size
		local su = size * self.size_u_ui
		local sv = size * self.size_v_ui
		--	self:print( "size is "..su.." x "..sv )
		--	self:print( "Size : "..su.." x "..sv )
		self.size_u,self.size_v = su,sv
		
		local step_u = math.max( self.step_u, 1 )
		local step_v
		--self:print( self.s_step_v )
		if self.s_step_v == 1 then
			step_v = step_u
		elseif self.s_step_v == 2 then
			step_v = math.max( self.step_u * sv/su, 1 )
		else
			step_v = math.max( self.step_v, 1 )
		end
	--	self:print( step_u.." "..step_v )	
	--	self:print( "NB : "..step_u.." x "..step_v )

		self.bind = self:get_texture_bind_2d( 2 )
		if self.bind then
			local img_sx,img_sy	= aaa.img.get_size(	self.bind )
			if not img_sx then
				self.bind = nil
			end
		end

		if self.s_how == 6 then
			self:draw_weaving( step_u,step_v, su,sv )
		else
			self:my_draw( step_u,step_v, su,sv )
		end

	self:draw_layers_end()
end
