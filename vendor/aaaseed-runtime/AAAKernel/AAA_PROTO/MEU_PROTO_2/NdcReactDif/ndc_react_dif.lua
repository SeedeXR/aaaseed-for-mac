function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "2d", "art", "texture", "procedural", "vj", "unfinished" },
			help = 	{
					"Simulate Reaction Diffusion chemical reaction which display various pattern,",
					"one textures is used as input to influence the process",
					"It output in F11 and use for buffers F12 and F13, in fact it use fbo F13 which define this",
					"Ui need to be stabilized"
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
	local xp, yp = t[1][1]-.5, t[1][2]-.5
	local ds = .1/nb
	local s = ds
	for i=2,nb do
		local xn, yn = t[i][1]-.5, t[i][2]-.5
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

	local ix,iy = 1,1
	local SY = .8
	local DY = .2

	local bind = TEXS:get_bind_by_name( self:get_inst_key() )
	if not bind then
		self:print_error( "No bind will stop define_ui()( here" )
		return
	end
	ix,iy = self:define_ui_ndc( {ix,iy, 8,4, bind=bind }, true, "no" ) 

	--ix,iy = self:define_ui_ndc( {ix,iy, 8,4}, true, "no" )

	self:add_shading_ui( 		{ix,	iy,			8,SY} )
	iy = iy + SY + DY

	local IY = iy
	bu = self:add_trig_method(	{ix+5,	IY,			3,SY},		"Restart", 			self, "restart" ):set_color_back( "Restart" )
	local IX = ix+3.5
	self:add_button(			{IX,	IY,			1.5,SY*.5},	"Mouse Erase", 		sha:get_ref_frag_auto()	)
		:set_text( "Mouse" )
	self:add_slider(			{IX,	IY+SY*.5,	1.5,SY*.5},	"Mouse Radius", 	sha:get_ref_frag_float(24), nil, .1, 0, 1. )
		:set_text( "Size" )

	--bu = self:get_bu_by_key( "shader" )
	--local iys = bu:get_y()
	--self:print( iy )
	--bu = self:add_trig(	{ix+5,iy-1.2,	3,SY},	"Restart" )
	--		:set_target(sha:get_ref_frag_int(2) )
	--		:set_color_back( "Restart" )

	local SYS = SY * 1.
	bu = self:add_button(	{ix,iy,		3,SY},	"How" )
		bu:set_menu{ "Out",	"Prev",		"Src" }
		bu:set_target( self, "s_how" )
		bu:set_text_selector(true)
	iy = iy + SY + DY
-- 	bu = self:add_selector(	{ix,iy,	8,SYS},	"How" )
-- 		bu:set_nb_min_0( 3 )
-- 		bu:set_item_text(	2,				"Prev",		"Src"	)	--,	"Input_Prev",	"Diff" )
-- --		bu:set_item_text(	6,	"Albe",		"Nor",		"Spec",		"Emis"	 )
-- 		bu:set_target( self, "s_how" )
-- 	iy = iy + SYS + DY

	-- self:add_button(			{ix,iy,		2,SYS}, 	"Laplacian Size", 		self, "b_lap_size", 	false	)
	-- 	:set_text( "Size" )
	-- self:add_button(			{ix+2,iy,	2,SYS}, 	"Laplacian Size Inv", 	self, "b_lap_size_inv", 	false	)
	-- 	:set_text( "Inv" )

	self:set_tab_key_def()
	bu = self:add_selector(	{ix,iy,		4,SYS},	"Size" )
		bu:set_nb( 6, 1 )
		bu:set_item_text(	1,				"Min",		"Max",		"Range",	"Inv", "Grad", "GInv" )	
		bu:set_target( self, "s_lap_size" )
	self:add_slider_two(	{ix+4,iy,	4,SYS}, 	"Laplacian Size Range", self, "lap_size_min", "lap_size_max", .2, 2, .1, 8.	)
		:add_values_def( .25, .5, .75, 1, 1.5, 2, 4, 5, 8 )
		:set_text( "Range" )
	iy = iy + SYS

	bu = self:add_selector(	{ix,iy,		4,SYS},	"Laplacian" )
		bu:set_nb_min_0( 4, 1 )
		bu:set_item_text(	1,				"3x3",		"5x5",		"Free",			"Variable" )
--		bu:set_item_text(	6,	"Albe",		"Nor",		"Spec",		"Emis"	 )
		bu:set_target( self, "s_lap" )
		bu:set_text_visible( false )
	self:add_slider(		{ix+4,iy,	4,SYS}, "Laplacian Size Gamma", 	self, "lap_size_gamma", 1, 0, 4. )
		:set_text( "Gamma" )
		:set_draw_gamma(true)
	iy = iy + SYS + DY

	bu = self:add_slider(	{ix+4,iy,	4,SY}, 	"Iteration",	self, "ite_nb", 1, 1,400 )
			:set_value_type_integer( true )
			--:set_min_max_strict( true )

	local function add_param( name, i, val, min,max )
		local r = sha:get_ref_frag_float(i)
		local bu = self:add_slider(	{ix,iy,	4,SY}, name, r, nil, val, min,max )
		iy = iy + SY
		return bu
	end

	bu = add_param(	"Diffusion A",	3,	1,		0,1 )
	bu = add_param(	"Diffusion B",	4,	.2,		0,1 )
--	bu = add_param(	"Diffuse",		5,	1,		0, 2.5 )
--	bu = add_param(	"Damp", 		6,	.99,	.9, 1. )
	iy = iy + DY

	
	self:add_button(		{ix,iy,		2,SY},	"Out Inv", self, "b_out_inv", false )
		:set_text( "Inv" )
	self:add_slider_two(	{ix+2,iy,	6,SY},	"Out Min Max", self, "out_min", "out_max", 0, 1, 0, 1	)
		:set_text( "Min Max" )
	iy = iy + SY
	bu = self:add_selector(	{ix,iy,		8,SY*2},"Out" )
		bu:set_nb( 6, 2 )
		bu:set_item_text(	1,		
											"Grey", "ColorMap", "Col", "Color", "Emboss", "Older", 
											"R", "G", "RG", "RGB", "B", "White" )
--		bu:set_item_text(	6,	"Albe",		"Nor",		"Spec",		"Emis"	 )
		bu:set_target( self, "s_out" )
		bu:set_text_visible( false )
	iy = iy + SY*2

	self:add_button(		{ix,iy,		1.5,SY},"Emboss", self, "b_emboss", false )
		:set_text( "Emboss" )
	self:add_slider(		{ix+1.5,iy,	2,SY},	"Emboss Dist", sha:get_ref_frag_float(13), nil, 1, 0, 2 )
		:set_text( "Dist" )
	-- self:add_slider(		{ix+4,iy,	4,SY},	"Out Emboss Factor", sha:get_ref_frag_float(14), nil, 1, 0, 2 )
	-- 	:set_text( "Factor" )
	self:add_button(		{ix+3.5,iy,	1.,SY},	"Emboss Inv", self, "b_emboss_inv", false )
		:set_text( "Inv" )
	self:add_slider_two(	{ix+4.4,iy,	3.5,SY},"Emboss Range", self, "emboss_min", "emboss_max", .5, 1., 0, 2 )
		:set_text( "Range" )
	iy = iy + SY

	bu = self:add_selector(	{ix,iy,		4,SY*1.5},	"Compo" )
		bu:set_nb_min_0( 3, 2 )
		bu:set_item_text(	2,	"Mul", "Add", "Sub", "InvSub" )
		bu:set_target( self, "s_compo" )
		bu:set_value( 0 )
--	self:add_button(		{ix,iy,		1.5,SY},	"Out Mul", self, "b_mul", false )
--		:set_text( "Mul" )
	self:add_slider(		{ix+4,iy,	4,SY},		"Out Mul Factor", sha:get_ref_frag_float(12), nil, 1, 0,1 )
		:set_text( "Out Mul" )
	iy = iy + SY*1.5
	self:add_rgbf(			{ix,iy,		8,1},		"Color" )
	iy = iy + SY + DY

	ix,iy = 9,1
	bu = self:add_slider_multi(	{ix,iy,	8,8},	"Feed Kill", 2 )
--hack
--		bu:set_elt_text_xy_f_ratio( -.4, -.2, .4 )
--		bu:set_select_on_click_double()
		bu.draw_back = draw_back_multi
--		bu.draw_fore = draw_fore_multi
		ui.bu_kill_feed = bu
	iy = iy + 8
	local sx = 2
	--todo this should be part of multi
	bu = self:add_trig_method(	{ix,iy,		sx,SY},		"inc",		ui.bu_kill_feed, "add_elt_ui", -1 )
		bu:set_text( "-" )
	bu = self:add_trig_method(	{ix+sx,iy,	sx,SY},		"dec",		ui.bu_kill_feed, "add_elt_ui", 1 )
		bu:set_text( "+" )
--	iy = iy + SY
	self:add_slider_two(		{ix+4,iy,	4,SYS}, 	"KF Grey Range", self, "kf_grey_min", "kf_grey_max", 0, 1, 0, 1	)
		:set_text( "Range" )
	iy = iy + SY

	self:add_button( 			{ix+4,iy,	2,SY*1.},	"KF Grey Inv", self, "b_kf_grey_inv", false )
		:set_text( "Inv" )
	self:add_slider(			{ix+6,iy,	2,SY*1.},	"Grey Gamma", sha:get_ref_frag_float(2), nil, 1, 0,4 )
		:set_text( "Gamma")
		:set_draw_gamma(true)
	iy = iy + SY

	self:add_bu_texture_target_unit( {ix,iy,	8,4, b_compact=true}, "Input",	2, false	)
		self:set_bu_texture_preset_use( 2, false ) 

	self:set_tab_key( "Tex")
	ix,iy = 9,2
	self:define_ndc_src(		{ix,iy,		8,12},		{ false, false, "ColorMap" }, false )

	self:__add_debug_fbo()
end


function meu:get_preset_nb() 	return 32	end

function meu:init()
--	local ref = self.ref)

	self.layer_attr = 1
	self.layer_shading_ids		= {2}

	local sha = self:add_shading( self.layer_shading_ids )

	param.set_comment( sha:get_ref_frag_float(1), "KillFeed nb" )
	param.set_comment( sha:get_ref_frag_float(2), "Grey Gamma" )
	param.set_comment( sha:get_ref_frag_float(5), "Laplacian Size Min" )
	param.set_comment( sha:get_ref_frag_float(6), "Laplacian Size Range" )
	param.set_comment( sha:get_ref_frag_float(7), "Laplacian Size Gamma" )
	param.set_comment( sha:get_ref_frag_float(8), "KF Grey Min" )
	param.set_comment( sha:get_ref_frag_float(9), "KF Grey Range" )
	param.set_comment( sha:get_ref_frag_float(10), "Out Min" )
	param.set_comment( sha:get_ref_frag_float(11), "Out Factor" )

	param.set_comment( sha:get_ref_frag_float(14), "Emboss Min" )
	param.set_comment( sha:get_ref_frag_float(15), "Emboss Factor" )

	sha:set_save_frag_float(	false,	1,	16	)
	sha:set_save_frag_float(	false,	24,	24	)
	sha:set_save_frag_int(		false,	1,	1	)
	sha:set_save_frag_int(		false,	3,	4	)
	sha:set_save_frag_vec4(		false	)

	self:restart()
end

function meu:restart()
	self.b_restart_trig = true
end

function meu:define_fbo_internal( inst_key )
	local meu_fbo = self:use_meu_fbo_flipflop( 2, 0,2, {b_by_fbo=true, ch_nb=4, ch_type="fp32"} )
	return meu_fbo
end

function meu:update()
	local meu_fbo = self:__update_fbo_internal()
	if not meu_fbo then return end

	local s_how = self.s_how
	if not s_how then return end	--if init did not when thru

	local ui = self.ui

	-- self:print(aaa.time.t)
	if aaa.time.t == 0 then
		self:restart()
	end

	local sha = self:get_shading()
 	local v = s_how-1 + self.s_lap * 16
	if self.b_restart_trig then
		v =  v + 256
		self.b_restart_trig = false
	end
	sha:set_frag_int( 1, v )

	v = self.s_out
	if self.s_compo			then v = v + 256*self.s_compo	end
	if self.b_out_inv		then v = v + 256*256			end
	if self.b_emboss		then v = v + 256*256*2			end
	if self.s_lap_size>=5	then v = v + 256*256*4			end

	sha:set_frag_int( 3, v )

	local mi_o,ma_f = self.lap_size_min,self.lap_size_max
	if		self.s_lap_size==1						 then ma_f = 0
	elseif	self.s_lap_size==2						 then mi_o,ma_f = ma_f,0
	elseif	self.s_lap_size==3 or self.s_lap_size==5 then ma_f = ma_f-mi_o
	elseif	self.s_lap_size==4 or self.s_lap_size==6 then mi_o,ma_f = ma_f,mi_o-ma_f
	end
	sha:set_frag_float_5_7( mi_o, ma_f, self.lap_size_gamma )

	local mi,ma = self.kf_grey_min,self.kf_grey_max
	if self.b_kf_grey_inv then mi,ma = ma,mi end
	sha:set_frag_float_8_9( mi, ma-mi )

	local mi,ma = self.out_min,self.out_max
	sha:set_frag_float_10_11( mi, 1/(ma-mi) )

	local mi,ma = self.emboss_min,self.emboss_max
	if self.b_emboss_inv then mi,ma = ma,mi end
	sha:set_frag_float_14_15( mi, 1/(ma-mi) )

	-- local sx,sy = self:get_texture_size( 2 )
	-- if sx then
	local sx,sy = meu_fbo:get_pixel_size()
	--self.sx_fbo = sx
	--self.sy_fbo = sy
	--local sx,sy = self:get_texture_size( 1 )
	if sx then
	 	sha:set_frag_vec4( 1, sx,sy, 1/sx,1/sy )
	end

--	sha:set_frag_float_16( math.sin( aaa.time.t * math.pi2 * self.freq ) )

	local function fn_k(k) return .018 + k * (.074 -.018) end
	local function fn_f(f) return f * .112 end
	local kf_t = ui.bu_kill_feed:get_values()
	local kf_nb = #kf_t
	for i=1,kf_nb do
		local k,f = kf_t[i][1], kf_t[i][2]
		sha:set_frag_vec4( i+1, fn_k(k),fn_f(f) )
	end
	sha:set_frag_float( 1, kf_nb-1 )
end

function meu:draw()
	local meu_fbo = self:__update_fbo_internal()
	if not meu_fbo then return end

	local nb = self.ite_nb
	if not nb then return end	--if init did not when thru

	local sha = self:get_shading()
	

	self:do_fbo( nb )

 	self:draw_layers_begin() 	

	--set attr
 		self:draw_layer( self.layer_attr )				
	--set shader
	 	self:draw_layer( self.layer_shading_ids[1] )

	--set texture and filtering	
		for i=2,0,-1 do
			gol.set_tex_unit_cur( i )
			if true then
				gol.set_minification_linear()
				gol.set_magnification_linear()
			else
				gol.set_minification_nearest()
				gol.set_magnification_nearest()
			end
		end

		local size = 1.
		for i=1,nb do
			--self:print( "pass "..i )
			local b_last = i==nb
			if b_last then
				sha:set_frag_int( 4, 1 )	-- 1 mean the last frame and so shader should write to second attachment
				gol.update_uniform_fragment_int()
			end
			--infact we draw here		
			meu_fbo:draw_quad_fbo_pass( size, false, b_last )
		end
		
 	self:draw_layers_end()
	--MEU.draw( self )

end




