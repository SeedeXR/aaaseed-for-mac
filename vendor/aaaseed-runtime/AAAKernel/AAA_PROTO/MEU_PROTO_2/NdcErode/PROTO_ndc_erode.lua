
function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "2d", "art", "draw", "core", "coregraphic", "texture", "procedural", "imageprocessing", "vj", "unfinished" },
			help = 	{
					"This is an Erode fast implementation (fragment shader on the Gpu) faster than OpenCV.",
					"It is an Image processing process but can also have an artistic use.",
					"need to be refined and Dilate Added",
					"It output in the current Fbo"
					}
		
		}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix,iy
	local SY = .8
	local DY = .2

--	self:add_camera()

	self:set_tab_key_def()

	ix,iy =  1,1
	ix,iy = self:define_ui_ndc( false, true )
--	ix,iy = self:define_ui_ndc( {ix,iy,	8,SY}, true )
	iy = iy + DY

	local SYM = 6
	self:add_bu_texture(	{1,iy, 8,SYM}, "A", 1, false )
	--	self:add_bu_texture(	{9,iy, 8,SYM}, "B", 2, false )

	iy = iy + SYM




	ix,iy = 9,2.2


	bu = self:add_slider(	{ix,iy,		8,SY}, 		"Threshold", 	sha:get_ref_frag_float(1), nil, 0.1, 	0,1 )
	iy = iy + SY + DY

	bu = self:add_selector(	{ix,iy,		8,SY*3},	"Draw" )
		bu:set_nb_min_0( 4, 3 )
		bu:set_item_text(	2,	"Spiral",	"U",		"V"	)
		bu:set_item_text(	5,	"3x3",		"5x5",		"7x7",		"9x9" 		)
		bu:set_item_text(	9,	"11x11",	"13x13",	"15x15",	"17x17" 	)
		bu:set_target_param( sha:get_ref_frag_int(1) )
	iy = iy + SY * 3 + DY

	bu = self:add_slider(	{ix,iy,		8,SY}, 		"Iteration", 	sha:get_ref_frag_int(2), nil, 8, 	2,240 )
		:set_min_max_strict( true )
	iy = iy + SY + DY


--	bu = self:add_slider(	{ix,iy,		4,SY*.8}, 	"Y_Factor_Top", 	sha:get_ref_frag_float(4), nil, 0, -1, 1 )
--	iy = iy + SY*.8
	-- bu = self:add_rgbfa(	{ix,iy,		8,SYC},		"Gamma",		false	)
	-- 	ui.bu_gamma_out = bu

	ix,iy = 9,2.2




end

function meu:init()
	local ref = self.ref

--	self:use_meu_fbo( "light" )

	local sha = self:add_shading()
--	sha:set_save_frag_float(	false,	1,5 )
--	sha:set_save_frag_int(		false,	2,2 )
--	sha:set_save_frag_vec4(		false,	1,4 )
end

function meu:draw()
	local ui = self.ui
	local sha = self:get_shading()

	local sx,sy = self:get_texture_size(1)
	if sx then
		sha:set_frag_vec4( 1, sx,sy, 1/sx,1/sy )
	end

	-- r,g,b,a = ui.bu_gamma_out:get_rgba()
	-- sha:set_frag_vec4( 4, r,g,b,a )

 	self:draw_layers_begin()
 		self:draw_layer( 1 )
		 for i=1,1 do
			self:bind_texture_to_unit( i )
		end
		gol.set_tex_unit_cur(0)
		local s = 1.
		aaa.draw_rect_uv( -s,-s,s,s )
 	self:draw_layers_end()
end



