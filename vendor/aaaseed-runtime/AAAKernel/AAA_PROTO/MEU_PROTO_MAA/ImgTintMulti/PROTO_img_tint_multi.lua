function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "art", "2d", "draw", "procedural", "texture", "vj" },
			help = 	{
						"pixelize a texture in a semi continous way,",
						" some mode are imitate a vision through a window made to hide what's behind.",
					}
			}
end

function meu:get_preset_nb()	return 24	end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix = 1
	local iy = 1
	local SY = 1.
	local SYH = .4
	local SYT = 2.
	local DY = .2
	local SX3 = 8/3

	self:add_rendering()
	self:add_slider(				{ix,iy,		8,SY}, 	"RGB Divergence U ",	self,"divergence_u",	0, -.5,.5 )
	iy = iy + SY + DY



	self:add_selector(				{ix,iy,		8,SY}, 	"Method"	)
		:set_target_lua( self,"s_method" )
		:set_nb_min_0(5)
	iy = iy + SY
	self:add_slider(				{ix,iy,		8,SY}, 	"Influence",			self,"influence",		0, 0,1	)
	iy = iy + SY + DY

	local t_b = {}
	local t_bu = {}
	local SYC = SY * 1.2
	self.col_nb_max = 8
	for i=1,self.col_nb_max do
		self:add_button( 			{ix,iy,		SYC,SYC}, 	"color_active_"..i,	t_b,i,	false	)
			:set_text_visible(false)
		t_bu[i] = self:add_rgbfa(	{ix+SYC,iy,	8-SYC,SYC}, "color_"..i, false	)
		iy = iy + SYC
	end
	self.b_color = t_b
	ui.bu_color = t_bu
	iy = iy + DY

	ix,iy = 9,1
	self:add_camera()
	iy = iy + 1 + DY
	self:add_shading_ui(			{ix,iy}	)
	iy = iy + 1 + DY

	self:add_size_uvf_video( 		{ix,iy,	8,SY*2}	)
	iy = iy + SY*2 + DY
	local SYC = 6
	self:add_bu_texture_target_unit({ix,iy, 8,SYC}, "Src" )
		:set_preset_use( false )
		--self:set_bu_texture_preset_use( 1, false )
	iy = iy + SYC + DY

	self:add_transfo(				{ix,iy,	8,2.4} ) 
	iy = iy + 2.4 + DY

end

function meu:init()
	local ref = self.ref
	ref.layer_attr = self:get_layer(1)
	-- ref.layer_back = self:get_layer(2)
	-- ref.layer_front = self:get_layer(3)
	-- ref.layer_flex = self:get_layer(4)
	self:add_shading( 2 )
end


function meu:update()
	local ui = self.ui
	local sha = self:get_shading()

	local t_b = self.b_color
	local t_bu = ui.bu_color
	local nb = 0
	for i=1,self.col_nb_max do
		if t_b[i] then
			nb = nb + 1
			local r,g,b, a = t_bu[i]:get_rgba()
			sha:set_frag_vec4( nb,	r,g,b, a	)
			if nb+4<=8 and i+4<=8 then
				local r,g,b, a = t_bu[i+4]:get_rgba()
				sha:set_frag_vec4( nb+4,	r,g,b, a	)
			end	
		end
	end
	sha:set_frag_int( 1, nb )
	sha:set_frag_int( 2, self.s_method )
	sha:set_frag_float_1_2( self.influence, self.divergence_u )
end


function meu:draw()
	local ref = self.ref
	self:draw_layers_begin()
		self:draw_layer(1)
		self:draw_layer(2)
	self:draw_layers_end()
end
