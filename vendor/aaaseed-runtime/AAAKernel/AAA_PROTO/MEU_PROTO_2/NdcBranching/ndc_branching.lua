function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "2d", "art", "texture", "procedural", "vj", "unfinished" },
			help = 	{
					"Derived from Wyatt works, generate branching pattern,",
					"two textures are used as input to influence the process",
					"It output in F6 and use for buffers F7 and F8, in fact it use fbo F8 which define this",
					"Todo: refine Process and OutPut"
					}
		}
end

function meu:draw_back_multi_low( bu )
	oo.getsuper(SLIDER_MULTI).draw_back( bu )

	local sx,sy = .5,.5
	gol.color_white( 1 )
	aaa.draw_bind_rect( 60, -sx,-sy, sx,sy )

	gol.color_orange( .5 )

	local dx = sx / 2
	local dy = sy / 2
	for i=0,4 do
		local v = -sx + dx * i
		gol.draw_lines_2d( v, -sy, v, sy )
		gol.draw_lines_2d( -sx, v, sx, v )
	end

	local t = bu:get_values()
	local nb = #t
	local xp,yp =  t[1][1]-.5,t[1][2]-.5
	local ds = .1/nb
	local s = ds
	for i=2,nb do
		local xn,yn = t[i][1]-.5,t[i][2]-.5
		--table.print( t, "values", 2 )
	--self:print( bu:get_values() )
		gol.color_white()
		gol.draw_lines_2d( xp,yp, xn,yn )
		gol.color_white(.5)
		aaa.draw_mul_line(	xp,yp, s )
		s = s + ds
		xp,yp = xn,yn
	end
	aaa.draw_mul_line(	xp,yp, s )
end

local function draw_back_multi( bu )
	bu:get_meu_up():draw_back_multi_low( bu )
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()
	local ix = 1
	local iy = 1
	local SY = 1
	local DY = .2

	--todo put it back
	--self:add_camera()

	local bind = TEXS:get_bind_by_name( self:get_inst_key() )
	if not bind then
		self:print_error( "No bind will stop define_ui()( here" )
		return
	end
	ix,iy = self:define_ui_ndc( {ix,iy, 8,4, bind=bind }, true, "no" ) 
	
	iy = iy + DY
	local SYN = SY	-- SY now
	self:add_shading_ui( 		{ix,	iy,			4,SYN} )
	bu = self:add_trig_method(	{ix+5,	iy,			3,SYN},			"Restart", 			self, "restart" ):set_color_back( "Restart" )
	
	local IX = ix+3.5
	self:add_button(			{IX,	iy,			1.5,SYN*.5},	"Mouse Erase", 		sha:get_ref_frag_auto()	)
		:set_text( "Mouse" )
	self:add_slider(			{IX,	iy+SYN*.5,	1.5,SYN*.5},	"Mouse Radius", 	sha:get_ref_frag_float(24), nil, .1, 0, 1. )
		:set_text( "Size" )
	iy = iy + SYN + DY

	--local iys = bu:get_y()
	--self:print( iy )
	--bu = self:add_trig(	{ix+5,iy-1.2,	3,SY},	"Restart" )
	--		:set_target(sha:get_ref_frag_int(2) )
	--		:set_color_back( "Restart" )

	self:set_tab_key_def()

	
	bu = self:add_selector(	{ix,iy,	8,SY},	"How" )
		bu:set_item_text(	1,	"Wyatt",	"Maa", "3", "4" )
		bu:set_target( self, "s_how" )
	iy = iy + SY
	
	bu = self:add_slider(	{ix,iy,		4,SY},		"Iteration",		self, "ite_nb",	16, 1,400 )
	   :set_value_type_integer( true )
	bu = self:add_slider(	{ix+4,iy,	4,SY},		"Maa Factor",		self, "maa_factor",	.25, 0,1 )
	   --:set_min_max_strict( true )
	iy = iy + SY + DY


	local SYS = SY * 1.2
	self:begin_bu_group( "Field" )
	local sx = 4
	self:add_bu_texture_target_unit( {ix,iy, 8,SY*4}, "Field",	2, false):set_preset_use(false)
	iy = iy + SY*4
	ix = ix + 4
	self:add_button(			{ix,iy,		SY,SY}, 	"Amount active", 	self, "b_img_gradient",		false ):set_text_visible(false)
	self:add_slider(			{ix+SY,iy,	sx-SY,SY},	"Amount", 			self, "img_gradient",		1, -4,4 )
 		--:set_text( "Gamma" )
	iy = iy + SY
	self:add_button(			{ix,iy,		SY,SY}, 	"Normal active", 	self, "b_img_normal",		false ):set_text_visible(false)
	self:add_slider(			{ix+SY,iy,	sx-SY,SY},	"Normal", 			self, "img_normal",			1, -4,4 )
	ix = ix-4
	 self:end_bu_group()

 		--:set_text( "Gamma" )
	iy = iy + SY*2 + DY


	ix,iy = 9,1
	self:add_slider(		{ix,iy,		4,SY},		"Particle Gamma",	self,	"part_gamma",		1, 		0,8 )
		:set_draw_gamma(true)
	--	:set_text( "Gamma" )
	iy = iy + SY

	bu = self:add_selector(	{ix,iy,		8,SY*1.8},	"Out" )
		bu:set_nb( 6, 2 )
		bu:set_item_text(	1,				"bw", "Part", "Col", "Color", "A", "Emboss",
											"R", "G", "b", "w", "RGBA", "White" )
--		bu:set_item_text(	6,	"Albe",		"Nor",		"Spec",		"Emis"	 )
		bu:set_target( self, "s_out" )
		bu:set_text_visible( false )
		iy = iy + SY*1.8
--todo rename this
	self:add_button(		{ix,iy,		2,SY},		"Out Inv", self, "b_out_inv", false )
		:set_text( "Inv" )
	self:add_slider_two(	{ix+2,iy,	8-2-8/6,SY},		"Out Min Max", self, "out_min", "out_max", 0, 1, 0, 1	)
		:set_text( "Min Max" )
	iy = iy + SY + DY





	ui.color = {}
	local SYC = SY * 1.5
--EMBOSS
	self:begin_bu_group( "Emboss" )
	self:add_button(		{ix,iy,		1.5,SY},	"actice",	self, "b_emboss",				false )
		:set_text( "Emboss" )
	self:add_slider(		{ix+1.5,iy,	2,SY},		"Dist",		sha:get_ref_frag_float(13),nil,	1,	0,2 )
	-- self:add_slider(		{ix+4,iy,	4,SY},		"Out Emboss Factor", sha:get_ref_frag_float(14), nil, 1, 0, 2 )
	-- 	:set_text( "Factor" )
	self:add_button(		{ix+3.5,iy,	1.5,SY},	"Inv",		self,"b_emboss_inv",			false )
	self:add_slider_two(	{ix+5,iy,	3,SY},		"Range",	self,"emboss_min","emboss_max", .5, 1., 0, 2 )
	iy = iy + SY
	local sx3 = 8/3
	self:add_slider(		{ix,iy,		sx3,SY},	"Angle",	self,"emboss_angle",		0,	-1,1 )
	self:add_slider(		{ix+sx3,iy,	sx3,SY},	"Azimut",	self,"emboss_azimut",		0,	0,1 )
	self:add_slider(		{ix+sx3*2,iy,sx3,SY},	"Gamma",	self,"emboss_gamma",		1,	0,8 )
		:set_draw_gamma(true)
	iy = iy + SY
	local ox = 2 
	ui.color[1] = self:add_rgbfa(	{ix+ox,iy,		8-ox,SYC},	"Base", 	false )
	iy = iy + SYC
	ui.color[2] = self:add_rgbfa(	{ix+ox,iy,		8-ox,SYC},	"Top", 	false )
	self:add_button(				{ix,iy-SY*.5,	2.,SY},		"Color Inv", self, "b_emboss_color_inv", false )
		:set_text( "Inv" )
	self:end_bu_group()
	iy = iy + SYC + DY
--PART
	ui.color[3] = self:add_rgbfa(	{ix+ox,iy,		8-ox,SYC}, "Part",	false )
	iy = iy + SYC + DY
--OUT
	self:add_button(		{ix,iy,		1.5,SY},	"Out Mul", self, "b_mul", false )
		:set_text( "Mul" )
	self:add_slider(		{ix+1.5,iy,	5,SY},		"Out Mul Factor", sha:get_ref_frag_float(12), nil,	0,	0,1 )
		:set_text( "Out Mul" )
	iy = iy + SY + DY

	self:add_rgbf(			{ix,iy,		8,SYC},		"Color" )
	iy = iy + SYC + DY

	self:set_tab_key( "Tex")
	ix,iy = 9,2
	self:define_ndc_src(	{ix,iy,		8,12 },	{ false, false, "ColorMap" }, false )

	self:__add_debug_fbo( 1,9 )
end


function meu:get_preset_nb() 	return 32	end

function meu:init()
--	local ref = self.ref)

	local sha = self:add_shading()
	for i=1,24 do
		param.set_comment( sha:get_ref_frag_float(i) )
	end

	param.set_comment( sha:get_ref_frag_float(2), "Grey Gamma" )
--todo
--	param.set_comment( sha:get_ref_frag_float(5), "Laplacian Size Min" )
--	param.set_comment( sha:get_ref_frag_float(6), "Laplacian Size Range" )
--	param.set_comment( sha:get_ref_frag_float(7), "Laplacian Size Gamma" )
	param.set_comment( sha:get_ref_frag_float(9), "Particle Gamma" )
	param.set_comment( sha:get_ref_frag_float(10), "Out Min" )
	param.set_comment( sha:get_ref_frag_float(11), "Out Factor" )

	param.set_comment( sha:get_ref_frag_float(14), "Emboss Min" )
	param.set_comment( sha:get_ref_frag_float(15), "Emboss Factor" )

	sha:set_save_frag_float(	false,	2,4 )
	sha:set_save_frag_float(	false,	9,16 )
	sha:set_save_frag_float(	false,	24,24 )

	sha:set_save_frag_int(		false,	1,4 )

	self:restart()
end

function meu:restart()
	--aaa.print_method( "Restart" )
	self.b_restart_trig = true
	self.__iteration_count_total = 0
end

function meu:define_fbo_internal( inst_key )
	local meu_fbo = self:use_meu_fbo_flipflop( 2, 0,2, {b_by_fbo=true, ch_nb=4, ch_type="fp32"} )
	return meu_fbo
end

function meu:update()
	local meu_fbo = self:__update_fbo_internal()
	if not meu_fbo then return end

	local ui = self.ui

	local sha = self:get_shading()
 	local v = self.s_how
	if self.b_restart_trig then
		v =  v + 256
		self.b_restart_trig = false
	end
	sha:set_frag_int( 1, v )

	v = self.s_out
	if self.b_mul		then v = v + 256	end
	if self.b_out_inv	then v = v + 512	end
	if self.b_emboss	then v = v + 1024	end

	sha:set_frag_int( 3, v )

	local mi,ma = self.emboss_min,self.emboss_max
	if self.b_emboss_inv then mi,ma = ma,mi end
	sha:set_frag_float_14_16( mi, ma - mi, self.emboss_gamma )

	sha:set_frag_float_2_4( self.b_img_gradient and self.img_gradient or 0,
							self.b_img_normal   and self.img_normal   or 0,
							math.pow( self.maa_factor, 8 ) * 64 )

	-- local mi,ma = self.lap_size_min,self.lap_size_max
	-- if self.b_lap_size_inv then mi,ma = ma,mi end
	-- if self.b_lap_size then
	-- 	sha:set_frag_float( 5, mi )
	-- 	sha:set_frag_float( 6, ma - mi )
	-- else
	-- 	sha:set_frag_float( 5, mi)
	-- 	sha:set_frag_float( 6, 0. )	
	-- end
	-- sha:set_frag_float( 7, self.lap_size_gamma )

	local mi,ma = self.out_min,self.out_max
	sha:set_frag_float_9_11(	self.part_gamma,
								mi,
								1. / (ma - mi) )

	--local sx,sy = self:get_texture_size( 1 )
	local sx,sy = meu_fbo:get_pixel_size()
	if sx then
	 	sha:set_frag_vec4( 1, sx,sy, 1/sx,1/sy )
	end
--	sha:set_frag_float_16( math.sin( aaa.time.t * math.pi2 * self.freq ) )
	local a = self.emboss_angle * math.pi2
	local c,s = math.cos(a), math.sin(a)

	a = self.emboss_azimut * math.pi * .5
	local ca,sa = math.cos(a), math.sin(a)
	sha:set_frag_vec4( 2, ca*c,ca*s,-sa, 1 )

	local function set_color_shader( id, id_dst )
		local r,g,b, a = self.ui.color[id]:get_rgba()
		sha:set_frag_vec4( id_dst, r,g,b, a )
	end

	set_color_shader( 1, self.b_emboss_color_inv and 3 or 4 )
	set_color_shader( 2, self.b_emboss_color_inv and 4 or 3 )
	set_color_shader( 3, 5 )

end

function meu:draw()
	local meu_fbo = self:__update_fbo_internal()
	if not meu_fbo then return end

	local sha = self:get_shading()
	local nb = self.ite_nb	

	self:do_fbo( nb )
	
 	self:draw_layers_begin()
		self:draw_layer( 1 )

		for i=2,0,-1 do
			gol.set_tex_unit_cur( i )
			if true then
				gol.set_minification_linear()
				gol.set_magnification_linear()
				gol.set_wrap_2d_repeat()
			else
				gol.set_minification_nearest()
				gol.set_magnification_nearest()
			end
		end


		--self:draw_layer( 2 )
		sha:set_frag_int( 4, 0 )
		for i=1,nb do
			--self:print( "ite "..i )
			--todo perhaps pass the real counteur since restart
			-- we pass an iteration counter (maintained over remdered frames) to vary algo
			local count = self.__iteration_count_total + 1
			self.__iteration_count_total = count
			sha:set_frag_int( 2, count )

			local b_last = i==nb
			if b_last then
				sha:set_frag_int( 4, 1 )	-- 1 mean the last frame and so shader should write to second attachment
			end
			gol.update_uniform_fragment_int()
			meu_fbo:draw_quad_fbo_pass( 1, false, b_last ) --,  true, false )
		end

	self:draw_layers_end()

	--MEU.draw( self )
end




