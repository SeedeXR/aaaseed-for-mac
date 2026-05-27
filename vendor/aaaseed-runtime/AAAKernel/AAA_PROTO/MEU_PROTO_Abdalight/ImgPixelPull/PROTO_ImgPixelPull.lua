function meu:define_meu_infos()
	return {
		author = "Abdalight", date = "2025",
		tags = { "2d", "3D", "Art", "Draw", "Generator", "Procedural" },
		help = "ImgPixelPull — stretches or compresses the image perpendicular to a segment defined by a center point, an angle, and a length."
	}
end

function meu:define_ui()
	local ui = self.ui
	local sha = self:get_shading()
	local ix, iy = 1, 1
	local SY, DY = 1, 0.2
	local bu 

	self:add_shading_ui({ix, iy, 8, SY})
	iy = iy + SY + DY
	local y_begin = iy
	self:set_tab_key("Main")

	-- input texture

	bu = self:add_button(           {ix,iy,               8,SY},  "Grid Geometry",     nil,  nil,               0       )
 		bu:set_menu{ "Brutal", "Clamp" , "step" , "gauss"    }:set_nb_min_0( 1, 4 )
        bu:set_target_lua( self, "smooth" )
        bu:set_text_selector( true )
	iy = iy + SY

	bu = self:add_bu_texture_target_unit(		{ix,iy,		8,5*SY, b_compact=true}, "Tex_Input", 1, true	)
	ui.bu_tex_input = bu
	iy = iy + 5 * SY + DY

	-- controls
	self:add_slider(	{ix,iy, 		4,SY},		"Center U",		self,		"center_u",				0.5,		0.0,1.0		):set_color_back("u")
	self:add_slider(	{ix+4,iy, 		4,SY}, 		"Center V", 	self, 		"center_v", 			0.5,		0.0,1.0		):set_color_back("v")
	iy = iy + SY + DY

	self:add_slider(	{ix,iy,			4,SY},		   "Angle", 	self, 		   "angle", 			0.0, 		0.0,180.0	)
	self:add_slider(	{ix+4,iy, 		4,SY}, 	 "Half Length", 	self,    "half_length", 		    0.0,		0.0,0.5		)
	iy = iy + SY + DY

	self:add_slider(	{ix,iy,			4,SY},      "Strength", 	self, 	    "strength", 			0.5, 		-5.0,5.0	):set_color_back("r")
	self:add_slider(	{ix+4,iy, 		4,SY}, 		  "Radius", 	self, 		  "radius", 			0.03, 		0.0,1.0		)
	iy = iy + SY + DY

	-- right panel
	ix = 9; iy = y_begin
	self:add_camera()
	self:add_rendering()

	self:add_shading_sliders_tab(sha, {"Vert","Frag"}, y_begin, SY)
end

function meu:init()
	self.layer_shading = 2
	local sha = self:add_shading(self.layer_shading)

	self.center_u, self.center_v = 0.5, 0.5
	self.angle, self.half_length = 0.0, 0.0
	self.strength = 0.5
	self.radius = 0.03
	self.bind_2d_used = 0
end

function meu:update()
	local bu_tex = self.ui.bu_tex_input
	local sha = self:get_shading()

	if bu_tex then
		local changed, bind_2d = bu_tex:get_bind_2d_asked()
		if changed then
			self.bind_2d_used = bind_2d
		end
	end

	sha:set_frag_vec4(	1, self.center_u, self.center_v, self.angle * math.pi / 180, self.half_length )
	sha:set_frag_float(	1, self.strength )
	sha:set_frag_float( 2, self.radius )
	sha:set_frag_int(   1, self.smooth ) -- 0,1,2,3

	gol.update_uniform_fragment_float()
	gol.update_uniform_fragment_vec4()
end

function meu:draw()
	self:draw_layers_begin()
		self:draw_layer(1)
		self:draw_layer(2)
		self:draw_canva()
	self:draw_layers_end()
end


function meu:draw_canva()
	local x, y, z = 0, 0, 0
	local sx, sy = 1, 0.75
	
	gol.bind_texture( self.bind_2d_used )
	aaa.draw_rect_uv_at_z(x - sx, y - sy, x + sx, y + sy, z)
end

