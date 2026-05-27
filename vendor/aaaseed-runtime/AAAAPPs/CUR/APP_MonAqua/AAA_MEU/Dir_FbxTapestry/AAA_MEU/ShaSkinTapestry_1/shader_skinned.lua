
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

	iy = iy + SY
	bu = self:add_button(	{ix,	iy,			SY,SY },	"Curve", 			self, "b_curve", false )
	bu = self:add_button(	{ix+1,	iy+SY*1,	SY,SY },	"Deform", 			sha:get_ref_vert_int(1), nil, 0 )
	bu = self:add_button(	{ix+1,	iy+SY*2,	SY,SY },	"Maa orientation", 	sha:get_ref_vert_int(2), nil, 0 )
	bu = self:add_button(	{ix+1,	iy+SY*3,	SY,SY },	"Draw Line", 		self, "b_curve_draw", false )
	bu = self:add_button(	{ix+1,	iy+SY*4,	SY,SY },	"Draw Point",		self, "b_curve_draw_point", false )
	iy = iy + SY*5
	bu = self:add_slider(	{ix+1,	iy+SY*0,	4,SY},		"Radius", 			self, "radius", 4, 0,8 )
	bu = self:add_slider(	{ix+1+4,iy+SY*0,	3,SY},		"Heigth", 			self, "height", 0, 0,4 ):set_color_back("y")
	bu = self:add_slider(	{ix+1,	iy+SY*1,	7,SY},		"Fish Size", 		sha:get_ref_vert_float(8), nil, 1 )
end

function meu:init()
	local ref = self.ref
	self:add_shading()
end
function meu:get_layer_shading()
	return self:get_layer(1)
end
function meu:draw_curve()
	local r = self.radius
	local h = self.height

	local function circle( s )
		s = s*math.pi2
		local f = 1
		return r * math.cos(s)*f, math.sin(s*3)*h, r * math.sin(s*f)
	end
	local function line( s )
		return r*(1-s*2),math.sin(s*15)*h,0
	end

	local fn = circle
	if self.b_curve_draw then
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
	gol.set_point_size( 16 )
	gol.color_cyan()
	for i = 1,6 do
		local t = math.fmod( aaa.time.t * .2, 1 );
		local x,y,z = fn( t + .05*i )
		local it = (i-1)*3 + 1
		tab[it],tab[it+1],tab[it+2] = x, y, z
		if self.b_curve_draw_point then
			gol.color_cyan( .5 + (i-1)/5*.5 )
			gol.draw_points_3d( x,y,z )
		end
	end
	local sha = self:get_shading()
	sha:set_vert_vec3_6_table18(tab)

end
function meu:draw()
-- 	self:draw_layers_begin()
		if self.b_curve then
			self:draw_curve()
		end
 		self:draw_layer( 1 )
 	self:draw_layers_end()

end
