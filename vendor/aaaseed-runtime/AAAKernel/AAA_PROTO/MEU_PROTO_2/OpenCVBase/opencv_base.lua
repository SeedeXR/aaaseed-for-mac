function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "2d", "core", "coregraphic", "texture", "imageprocessing", "unfinished" },
			help = 	{
					"Using OpenCv through c_bdd_img_cv_multitouch do basic passes:",
					"Blur, Brightness Contrast, Erode/Dilate,",
					"Blur, Brightness Contrast, Difference",
					"Blur, Brightness Contrast again",
					"input and output are not set yet by the UI: need to be done", 
					"need to be documented better"		
					}
		}
end

--todo check there an image on cpu
-- then warn or force it
meu.draw_icon = MEU.draw_icon_opencv

function MEU:update_ui_bu_alpha( button, slider )
	slider:interpolate_alpha_bu( button:get_value() )
end
function MEU:update_ui_bu_alpha_b( sel, slider )
	slider:interpolate_alpha_bu( (sel:get_value()>1) )
end

		--todo deal with color
function meu:define_ui()
	self.b_ui_ok = false
	local ref = self.ref
	local ui = self.ui
	local bu
	local bu_b
	local par

	local ix, iy = 1, 1
	local sy = .8
	local SYM = 4
	local dy = .2

	local blur_max = 32

	local bdd
	local pass
-- PASS 1
	pass = ref.pass[1]
	bu = self:add_bu_texture(	{ix,iy,		8,SYM},		"Src", 1 )
		bu:set_bind_2d(64)
		--bu:set_target_lua( self, "bind_src" )
		--self:print( "bind src is "..self.bind_src )
		ui.bu_tex_src = bu
	iy = iy + SYM + dy

	bu_b = self:add_blur_selector(	{ix,iy,		8,sy},		"Method1",		self, "s_method1", 	1 )
	iy = iy + sy
	bu = self:add_slider(			{ix,iy,		8,sy},		"Blur1",		self, "blur1",	4,	1,blur_max ):set_value_type_integer(true)
	self:register_update_ui( "update_ui_bu_alpha_b", bu_b, bu )
	self.ui.bu_blur1 = bu
	iy = iy + sy + .2

	bu = self:add_slider_two(		{ix+sy,iy,	8-sy,sy},	"BC1", 			pass.bdd, "bc_min","bc_max",			0,1,	0,1	)
	bu_b = self:add_button(			{ix, iy, 	sy,sy},		"BC1 ON", 		pass.bdd, "bc" ):set_text_visible( false )
	self:register_update_ui( "update_ui_bu_alpha", bu_b, bu )


	iy = iy + sy + dy

	local SX_FIRST = 1.2
	local SXB = 2.5
	local SXR = 8 - SX_FIRST - SXB
	bu_b = self:add_button(		{ix+SX_FIRST,iy, 	SXB,sy},		"Dilate", 		pass.bdd, "dilate",			false	)
	bu = self:add_slider(		{ix+8-SXR,iy,		SXR,sy},		"Dilate_pass", 	pass.bdd, "dilate_pass",	3,	0,16)
		:set_value_type_integer(true)
		:set_text_visible(false)
	self:register_update_ui( "update_ui_bu_alpha", bu_b, bu )
	iy = iy + sy

	bu = self:add_button(		{ix,iy, 			SX_FIRST,sy}, 	"First", 		pass.bdd, "erode_first",	false	)
	bu_b = self:add_button(		{ix+SX_FIRST,iy, 	SXB,sy}, 		"Erode", 		pass.bdd, "erode",			true	)
	bu = self:add_slider(		{ix+8-SXR,iy,		SXR,sy},		"Erode_pass", 	pass.bdd, "erode_pass",  	3,	0,16)
		:set_value_type_integer(true)
		:set_text_visible(false)
	self:register_update_ui( "update_ui_bu_alpha", bu_b, bu )
	iy = iy + sy + dy*2

	bu_b = self:add_blur_selector(	{ix,iy,			8,sy},		"Method2",		self, "s_method2" )
	iy = iy + sy
	bu = self:add_slider(		{ix,iy,				8,sy},		"Blur2",		self, "blur2", 4, 1, blur_max ):set_value_type_integer(true)
	self:register_update_ui( "update_ui_bu_alpha_b", bu_b, bu )
	self.ui.bu_blur2 = bu
	iy = iy + sy +.2

	bu_b = self:add_button(		{ix,iy, 	sy,sy},		"BC2 ON",		pass.bdd, "bc_out" ):set_text_visible( false )
	bu = self:add_slider_two(	{ix+sy,iy,	8-sy,sy},	"BC2",			pass.bdd, "bc_out_min","bc_out_max", 0, 1., 0, 1. )
	self:register_update_ui( "update_ui_bu_alpha", bu_b, bu )
	iy = iy + sy + dy


	ix,iy = 9,1

	bu = self:add_bu_texture(	{ix,iy,		8,SYM},		"Dst", 2 )
		bu:set_bind_2d(65)
		--bu:set_target_lua( self, "bind_dst" )
		--self:print( "bind dst is "..self.bind_dst )
		ui.bu_tex_dst = bu
	--bu = self:add_monitor(	{ix,iy,		8,SYM},		"Monitor_Dst_A", param.get(pass.bdd,"image_dst_base") )
	iy = iy + SYM + dy

-- PASS 2
	pass = ref.pass[2]
	if pass then
		bu = self:add_button(	{ix,iy,		sy,sy},		"Pass 2",		self, "b_pass2", 	false )
		iy = iy + sy
		bu = self:add_selector(	{ix,iy,		8,sy},		"Diff" )
			bu:set_nb( 6, 1 )
			bu:set_target_lua( self, "s_diff" )
			bu:set_item_text( 1, "No", "Abs", "Diff", "Inv", "Min", "Max" )
		iy = iy + sy + dy*2

		bu_b = self:add_blur_selector(	{ix,iy,		8,sy},		"Method3",		self, "s_method3" )
		iy = iy + sy
		bu = self:add_slider(	{ix,iy,		8,sy},		"Blur3",		self, "blur3", 16, 1, 128 ):set_value_type_integer(true)
		self:register_update_ui( "update_ui_bu_alpha_b", bu_b, bu )
		self.ui.bu_blur3	= bu
		iy = iy + sy + .2

		bu_b = self:add_button(	{ix,iy,		sy,sy },	"BC3 ON", 		pass.bdd, "bc_out" ):set_text_visible( false )
		bu = self:add_slider_two(	{ix+sy,iy,	8-sy,sy},	"BC3", 			pass.bdd, "bc_out_min" , "bc_out_max", 0, 1., 0, 1. )
		self:register_update_ui( "update_ui_bu_alpha", bu_b, bu )
		iy = iy + sy + dy

--		bu = self:add_monitor(	{ix,iy,		8,SYM },	"Monitor_Dst_B", param.get(pass.bdd,"image_dst_base") )
--		iy = iy + SYM
	end

	self.b_ui_ok = true
end

function meu:init()
	local ref = self.ref
	ref.pass = {}
	for i = 1,2 do
		local pass = {}
		local bdd = self:get_layer_bdd(i+1)
		pass.bdd	= bdd
		if bdd then
			pass.bind_src = param.get_ref( bdd,	"image_src"					)
			pass.bind_dst = param.get_ref( bdd,	"image_dst_base"			)

			pass.smooth1 =
			{
				active	=	param.get_ref( bdd,	"smooth"					),
				method	=	param.get_ref( bdd,	"smooth_method"				),
				sx		=	param.get_ref( bdd,	"smooth_size_x"				),
				sy		=	param.get_ref( bdd,	"smooth_size_y"				),
			}

			pass.b_bc	=	param.get_ref( bdd,	"bc" )

			pass.smooth2 =
			{
				active	=	param.get_ref( bdd,	"smooth2"					),
				method	=	param.get_ref( bdd,	"smooth2_method"			),
				sx		=	param.get_ref( bdd,	"smooth2_size_x"			),
				sy		=	param.get_ref( bdd,	"smooth2_size_y"			),
			}

			pass.b_diff			=	param.get_ref( bdd,	"diff"			)
			pass.diff_method	=	param.get_ref( bdd,	"diff_method"	)

			pass.b_bc_out		=	param.get_ref( bdd,	"bc_out"		)
			ref.pass[i] = pass
		end
	end
end

--todo optimize on change only
function meu:update()
	if not self.b_ui_ok then
		return
	end
	local ps	= param.set
	local ref	= self.ref
	local ui	= self.ui

	local blur_symbo = { "BLUR", "GAUSSIAN", "MEDIAN" }
	local v
	local pass
	local smooth

	local bind_src = ui.bu_tex_src:get_bind_2d()
	local bind_dst = ui.bu_tex_dst:get_bind_2d()
-- PASS 1
	pass = ref.pass[1]
	if pass then
		ps( pass.bind_src, bind_src )
		ps( pass.bind_dst, bind_dst )

		smooth = pass.smooth1
		v = self.ui.bu_blur1:get_value()
		ps( smooth.sx, v )
		ps( smooth.sy, v )

		ps( smooth.active, self.s_method1~=1 )
		ps( smooth.method, blur_symbo[self.s_method1-1] )

		smooth = pass.smooth2
		v = self.ui.bu_blur2:get_value()
		ps( smooth.sx, v )
		ps( smooth.sy, v )

		ps( smooth.active, self.s_method2~=1 )
		ps( smooth.method, blur_symbo[self.s_method2-1] )
	end

-- PASS 2
	pass = ref.pass[2]
	if pass then
		ps( pass.bind_src, bind_dst )
		ps( pass.bind_dst, bind_dst )

		ps( pass.b_diff, self.s_diff~=1 )
		ps( pass.diff_method, self.s_diff-1 )

		smooth = pass.smooth2
		v = self.ui.bu_blur3:get_value()
		ps( smooth.sx, v )
		ps( smooth.sy, v )

		ps( smooth.active, self.s_method3~=1 )
		ps( smooth.method, blur_symbo[self.s_method3-1] )
	end

end

function meu:draw()
	local ref = self.ref
	self:draw_layers_begin()
		self:draw_layer( 1 )
		self:draw_layer( 2 )
		if self.b_pass2 and ref.pass[2] then
			--self:print( "pass 2" )
			self:draw_layer( 3 )
		end
	self:draw_layers_end()
end