
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()
	--self:add_rgbf(	{1,4.5 } )

	--self:add_bu_texture_target_unit()

	self:add_camera()
	self:add_rendering()

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	self:add_shading_ui( {1,1,	8} )

	iy = iy + .2
--	iy = iy + SY
	iy = iy + SY
	bu = self:add_button(	{ix,	iy,		SY,SY},	"Deform", 			sha:get_ref_vert_int(1), nil,	0 )
	bu = self:add_button(	{ix+4,	iy,		SY,SY},	"Flip_x", 			sha:get_ref_vert_int(2), nil,	0 )
	iy = iy + SY
	bu = self:add_button(	{ix,	iy,		SY,SY},	"Curve", 			self, "b_curve",				false )
	iy = iy + SY
	bu = self:add_button(	{ix+1,	iy,		SY,SY},	"Curve.Line", 		self, "b_curve_line",			false ):set_text("Line"):set_text_rect_ratio(3)
	bu = self:add_button(	{ix+4,	iy,		SY,SY},	"Curve.Point",		self, "b_curve_point",			false ):set_text("Point")
	iy = iy + SY
	bu = self:add_button(	{ix+1,	iy,		SY,SY},	"CatLine", 			self, "b_cat_line",				false ):set_text_rect_ratio(3)
	bu = self:add_button(	{ix+4,	iy,		SY,SY},	"CatCurve",			self, "b_cat_curve",			false )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy+SY*0,	4,SY},	"Radius", 			self, "radius", 				4,	0,8 	)
	bu = self:add_slider(	{ix+4,iy+SY*0,	4,SY},	"Heigth", 			self, "height", 				0,	0,4 	):set_color_back("y")
	bu = self:add_slider(	{ix+4,iy+SY*1,	4,SY},	"Height NB", 		self, "height_nb",				1,	1,32 	):set_color_back("y"):set_value_type_integer( true )
	iy = iy + SY*2 + .2
	bu = self:add_slider(	{ix+1,iy,		7,SY},	"Fish Size", 		sha:get_ref_vert_float(8), nil, 1 )
	iy = iy + SY + .2

	iy = iy + SY*2 + .2
	bu = self:add_slider(	{ix,iy,			3,SY},	"Alpha Threshold", 	sha:get_ref_frag_float(1), nil, 0,0,1 )

	ix = 9
	iy = self:define_time(	{ix,iy,			1,SY}, 1., false )
end

function meu:init()
	local ref = self.ref
	local sha = self:add_shading( 2 )
	--todo add fns helper in SHADING
	sha:set_frag_float(	1, 0. )
	sha:set_save_vert_float(	false, 1, 1 )
	sha:set_save_vert_float(	false, 8, 8 )
	sha:set_save_vert_int(		false, 1, 2 )
	sha:set_save_vert_vec4(		false, 1, 6 )
	sha:set_save_frag_float(	false, 1, 1 )
	sha:set_frag_vec4( 1, 0,0, 1,1 )
	ref.__layer_shading = self:get_layer(2)
	ref.material_front = param.get_ref( ref.__layer_shading, "material_front" )
	ref.material_back = param.get_ref( ref.__layer_shading, "material_back" )
end
function meu:get_ref_material_front()
	return self.ref.material_front
end
function meu:get_ref_material_back()
	return self.ref.material_back
end
function meu:set_material_front( id )
	param.set( self.ref.material_front, id )
end
function meu:set_material_back( id )
	param.set( self.ref.material_back, id )
end
function meu:get_layer_shading()
	return self.ref.__layer_shading
end
function meu:draw_curve( time )
	local r = self.radius
	local h = self.height
	local function circle( s )
		s = s*math.pi2
		local f = 1
		return r * math.cos(s), math.sin(s*self.height_nb)*h, r * math.sin(s)
	end
	local function line( s )
		return r*(1-s*2),math.sin(s*15)*h,0
	end
	local fn = circle
	if self.b_curve_line then
		local x,y,z = fn( 0 )
		local nb = 1024
		local x,y,z = fn(0)
		for i = 1,nb do
			local x2,y2,z2 = fn(i/nb)
			gol.draw_lines_3d( x,y,z, x2,y2,z2 )
			x,y,z = x2,y2,z2
		end
	end
	tab = self.points or {}
	local t = {}
	gol.set_point_size( 16 )
	gol.color_cyan()
	local s = .5
	for i = 1,6 do
		time = math.fmod( time or 0, 1 );
		local x,y,z = fn( (time - .05*i) )
		local it = (i-1)*3 + 1
		tab[it],tab[it+1],tab[it+2] = x, y, z
		it = (i-1)*3 + 1
		t[it],t[it+1],t[it+2] = x, y, z

		if self.b_curve_point then
			gol.color_cyan( .5 + (i-1)/5*.5 )
			gol.draw_points_3d( x,y,z )
			aaa.draw_str_xyz(	tostring(i),	x,y,z,	s,s	)	--	[,sx [,sy ] ]
		end

	end
	if self.b_cat_line then
		gol.draw_line_strip_3d( t )
	end
	if self.b_cat_curve then
		aaa.draw_catmull_rom( tab, 3 )
	end

	local sha = self:get_shading()
	sha:set_vert_vec3_6_table18(tab)
end

function meu:update_ui()	self:update_time_ui()	end
function meu:draw()
	local t = self:update_time()

	--if t then
	self:draw_layers_begin()
		--self:draw_layer( 1 )
		if self.b_curve then
			self:draw_curve(t)
		end

		self:draw_layer( 2 )
 	self:draw_layers_end()
end
