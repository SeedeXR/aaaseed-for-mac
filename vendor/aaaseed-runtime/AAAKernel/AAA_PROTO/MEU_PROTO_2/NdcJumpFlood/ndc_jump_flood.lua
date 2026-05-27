function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "2d", "art", "core", "coregraphic", "texture", "procedural", "imageprocessing", "vj", "unfinished" },
			help = 	{
					"Compute distance field but also generate nice graphics using Jump Flood Algorithm.",
					"the instance name should be (Feb 2025) the name of the internal fbo used",
					"It compute distance field and use it to generate distance field and/or related graphics",
					" Init pass: from the input an image is extracted: Inside/Outside/Edges",
					" Loop pass: the distance computation is done (Distance field) using 2 internal fbos",
					" Out  pass: from the distance field and the input it make an output image in a third fbo",
					"It uses the Fbo corresponding to the instance name and more",
					}
		}
end
--todo check if same size src/fbo enforced is a real necessity and not a work around a bug (like bat or not enough inmformation passed to shaders)
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading(1)

	local ix = 1
	local iy = 1
	local SY = .8
	local DY = .2

	self:add_shading_ui_multiple(	{ix,iy,		4,SY}, 	{ "Init", "Loop", "Out" } )
	self:add_button(				{ix+6,iy,	2,SY},	"Verbose",	self,	"b_verbose",	false )
	iy = iy + SY

	--todo should be better and dynamic
	--this this the out texture monitor
	local bind = TEXS:get_bind_by_name( self:get_inst_key() )
	if not bind then
		self:print_error( "No bind will stop define_ui()( here" )
		return
	end
	ix,iy = self:define_ui_ndc( {ix,iy, 8,4, bind=bind }, true, "no" ) 

	iy = iy + DY
	self:add_bu_texture_target_unit( {ix,iy, 8,4, b_compact=true}, 	"Input",		2, false	)
		--todo add_bu_texture_target_unit should return a composite BU
		self:set_bu_texture_preset_use( 2, false )
	iy = iy + 4 + DY


	self:add_slider(	{ix+4,iy,	4,SY},		"Size",		self,	"size",			1, 0,1 )

	iy = iy + SY + DY
	self:add_button(	{ix,iy,		2,SY},		"Init",		self,	"b_init",	true )
	iy = iy + SY

	local t_in = {}
	self.table_in = t_in 
	self:add_slider_two({ix,iy,		4,SY},		"In.Range",	t_in,	"range_min","range_max",	0,1,	0,1 )
		:set_text( "Range" )
	self:add_slider(	{ix+4,iy,	4,SY},		"In.Gamma",	t_in,	"gamma",					1, 		0,8 )
		:set_draw_gamma(true)
		:set_text( "Gamma" )
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},		"In.Repeat",t_in,	"f_repeat",					1, 		0,16 )
		:set_text( "Repeat" )
	self:add_slider_two({ix+4,iy,	4,SY},		"In.Level",	t_in,	"level_min","level_max",	.25,.5,	0,1 )
		:set_text( "Level" )
	iy = iy + SY
	self:add_selector(	{ix,iy,		8,SY},		"In.Mode"		)
		:set_item_text( 1, "Selected", "Inverse", "Edge" )
		:set_target_lua( t_in, "s_mode" )
	iy = iy + SY + DY

	ix,iy = 9,1
	self:add_button(	{ix,iy,		2,SY},		"Loop",				self,	"b_loop",	true )
	bu = self:add_text_info({ix+2,iy,	2,SY},	"Loop.nb"		)
		:set_text( "" )
		ui.bu_loop_nb = bu
	self:add_slider(	{ix+3,iy,	3,SY},		"Loop Nb Skip",		self,	"loop_nb_skip",		0, 	0,16 )
		:set_value_type_integer(true)
	self:add_button(	{ix+6,iy,	2,SY},		"1+Jfa",			self,	"b_1_jfa",	false )
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},		"Dist Max",			self,	"loop_dist_max",	1, 		0,1 )

	iy = iy + SY + DY

	self:add_button(	{ix,iy,		2,SY},		"Out",				self,	"b_out",			true )
	self:add_button(	{ix+5,iy,	3,SY},		"Src to alpha",		self,	"b_src_to_alpha",	false )

	iy = iy + SY + DY

	self:add_selector(	{ix,iy,		8,SY},		"Out_type"		)
		:set_item_text( 2, "Raw", "Dist", "Src+DF", "Exp2", "Exp", "Nimes", "Test" )
		:set_target_lua( self, "s_out_type" )
	iy = iy + SY

	self:add_slider(	{ix+1.5,iy,	2.5,SY},	"Frequency",	self,	"dist_freq",	1, 	0,32 )
	self:add_button(	{ix,iy,		1.5,SY},	"Relative", 	self,	"b_dist_freq_rel", 	false)
	self:add_slider(	{ix+4,iy,	4,SY},		"Gamma",		self,	"dist_gamma",	1, 	0,8 )
		:set_draw_gamma(true)
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},		"Offset",		self,	"dist_offset",	0, 	-1,1 )
	self:add_selector(	{ix+4,iy,	4,SY},		"Wrap"		)
		:set_item_text( 2, "Wrap", "Tri", "Saw" )
		:set_target_lua( self, "s_wrap" )
	--	:set_text( "Flip" )
	iy = iy + SY + DY

	self:set_tab_key_def()

	local t_col = {}
	self:add_button(	{ix,iy,		2,SY},		"col.Flip",		t_col,	"b_flip",	false )
		:set_text( "Flip" )
	self:add_selector(	{ix+2,iy,	6,SY},		"Col.Mode"		)
		:set_item_text( 1, "B&W", "A/B", "Hue" )
		:set_target_lua( t_col, "s_choice" )
	iy = iy + SY
	local SYC = 1
	ui.bu_col_in = self:add_rgbfa(	{ix,iy,	8,SYC}, 		"A",	false )
	iy = iy + SYC
	ui.bu_col_out = self:add_rgbfa(	{ix,iy,	8,SYC}, 		"B",	false )
	self.table_col = t_col
	iy = iy + SYC
	self:add_slider(	{ix,iy,	4	,SY},		"col.Offset",	t_col,	"offset",	0, 	-1,1 )
		:set_text( "Offset" )
	self:add_slider(	{ix+4,iy,	4,SY},		"col.Gamma",	t_col,	"gamma",	1, 	0,8 )
		:set_text( "Gamma" )
		:set_draw_gamma(true)
	iy = iy + SY + DY

	self:add_selector(	{ix,iy,		8,SY},		"Src"		)
		:set_item_text( 1, "No", "Mul", "MulAdd", "Add", "Min", "Max" )
		:set_target_lua( self, "s_out_src" )

	iy = iy + SY
	self:add_selector(	{ix,iy,		8,SY},		"Top_type"		)
		:set_item_text( 2, "Voronoi", "Laplace", "V&L", "Exp" )
		:set_target_lua( self, "s_out_top" )
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},		"Gradient Size",self,	"grad_size",	1, 	0,8 )
	self:add_slider(	{ix+4,iy,	4,SY},		"V to L",		self,	"vl_inter",		0, 	0,1 )
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},		"Top_Factor",	self,	"out_factor",	1, 	0,1 )
	self:add_slider(	{ix+4,iy,	4,SY},		"Top_src",		self,	"out_src",		1, 	0,1 )

	iy = iy + SY + DY
	self:add_rgbf(		{ix,iy,		8,1},		"Color" )
	iy = iy + SY + DY

	self:set_tab_key( "Tex")

--	self:add_bu_texture_target_unit( {ix,9, 8,5,b_compact=true}, "Previous",		1, false	)
--		self:set_bu_texture_preset_use( 1, false )
--	self:define_ndc_src({ix,iy,		8,12 },		{ "Previous", "Input" }, false )

	self:__add_debug_fbo( 9,9 )
end

function meu:get_preset_nb()	return 32	end

function meu:init()
--	local ref = self.ref
	self.layer_attr = 1
	self.layer_shading_ids = { 2, 3, 4 }

	local sha = self:add_shading( self.layer_shading_ids )

--	param.set_comment( sha:get_ref_frag_float(1), "KillFeed nb" )

	for i=1,3 do
		local sha = self:get_shading(i) 
		sha:set_save_frag_float(	false	)
		sha:set_save_frag_int(		false	)
		sha:set_save_frag_vec4(		false	)
	end

end

function meu:update_ui()
	local ui = self.ui
	local bu = ui.bu_loop_nb
	if bu then
		bu:set_text( self.loop_nb or "" )
	end
end

function meu:define_fbo_internal( inst_key )
	local meu_fbo = self:use_meu_fbo_flipflop( 2, 0,2, {b_by_fbo=true, ch_nb=2, ch_type="fp32"} )
	return meu_fbo
end

function meu:update()

	local meu_fbo = self:__update_fbo_internal()
	if not meu_fbo then return end
	
	local sha
	local ui = self.ui

	-- local sx,sy = self:get_texture_size( 2 )
	-- if sx then
	local sx,sy = meu_fbo:get_pixel_size()
	self.sx_fbo = sx
	self.sy_fbo = sy
	-- end

--	sha = self:get_shading( 1 ) -- 1(init),2(loop) or 3(out)

	-- local v = (self.s_how-1) + self.s_lap * 16
	-- if self.b_restart_trig then
	-- 	v =  v + 256
	-- 	self.b_restart_trig = false
	-- end
	-- sha:set_frag_int( 1, v )
	--sha:set_frag_float_5_7( v1,v2,v3 )
end

function meu:draw_passes( meu_fbo )
	local sha
	local ui = self.ui
	local s = self.size
	local b_verbose = self.b_verbose

	-- set Attributes
	self:draw_layer( self.layer_attr )			--set attr

			--set texture and filtering
		-- for i=2,0,-1 do
		-- 	gol.set_tex_unit_cur( i )
		-- 	if false then
		-- 		gol.set_minification_linear()
		-- 		gol.set_magnification_linear()
		-- 	else
		-- 		gol.set_minification_nearest()
		-- 		gol.set_magnification_nearest()
		-- 	end
		-- end

	local sx_src, sy_src = self:get_texture_size( 2 )
	local sx_fbo, sy_fbo = self.sx_fbo, self.sy_fbo 

-- COMPUTE distance for Jumop_flood stuff	
	local sx,sy = sx_fbo, sy_fbo
	local size_max_tex = math.max( sx,sy )
	
	local size_max_used = size_max_tex * self.loop_dist_max

	local max_level = math.log(size_max_used) / math.log(2)
	max_level = math.ceil(max_level)
	local size_max_p2 = math.pow( 2, max_level )


	local fx,fy = 1/sx_fbo,1/sy_fbo
		
	local t = self.table_in 
	local mi,ma = t.level_min, t.level_max
	if mi > ma then
		mi,ma = ma,mi
	end

	-- local function draw_quad( b_flip )
	-- 	if b_flip then
	-- 		self:do_fbo_flipflop( b_verbose )
	-- 	end		
	-- 	aaa.draw_rect_uv( -s,-s, s,s ) -- draw with shader
	-- end
	--INIT
	if self.b_init then
		sha = self:get_shading( 1 )


		sha:set_frag_float_1_8( fx,fy,
								t.range_min, 1/(t.range_max-t.range_min),
								t.gamma,t.f_repeat,
								mi,ma )
		sha:set_frag_int_1( t.s_mode )
		sha:set_frag_int_2( 0 )
		-- 	gol.update_uniform_fragment_float()
		-- 	gol.update_uniform_fragment_int()
		self:draw_layer( self.layer_shading_ids[1] )	--set shader init
		if b_verbose then
			self:print( "init pass 0" )
		end
		meu_fbo:draw_quad_fbo_pass( s, b_verbose,  false,	true, false )

		--todo if we don't loop just pass 0 is ok
		-- if self.b_loop then
			if b_verbose then
				self:print( "init pass 1" )	
			end
			sha:set_frag_int_2( 1 )
			gol.update_uniform_fragment_int()
			meu_fbo:draw_quad_fbo_pass( s, b_verbose,  false,	true, false )
		-- end	
	end
	
	--LOOP
	if self.b_loop then
		--SETUP
		sha = self:get_shading( 2 )
		--, max_level )	--max_level not used anymore
		sha:set_frag_float_3( size_max_used*size_max_used )
		--sha:set_frag_vec4( 1, fx,fy )
		--self:print( "size_max_used is "..size_max_used )
		self:draw_layer( self.layer_shading_ids[2] )
		self.loop_nb = max_level - self.loop_nb_skip
		if self.b_verbose then
			self:print( "will do "..self.loop_nb.." out of "..max_level.." needed" )
		end

		local tr = sha.frag.vec4
		local ps2 = param.set_real_2
		local f = 1
		local function do_loop_one( i, size, b_flipflop )
			if b_verbose then
				self:print( "loop "..i.." step is "..size )
			end
			--was
			--sha:set_frag_int( 1, size )
			--gol.update_uniform_fragment_int()

			local vx,vy = size*fx*f	,size*fy*f

			-- sha:set_frag_vec4( 1,  vx,  0 )
			-- sha:set_frag_vec4( 2, -vx,  0 )
			-- sha:set_frag_vec4( 3,  0,   vy )
			-- sha:set_frag_vec4( 4,  0,  -vy )
			-- sha:set_frag_vec4( 5,  vx,  vy )
			-- sha:set_frag_vec4( 6, -vx,  vy )
			-- sha:set_frag_vec4( 7,  vx, -vy )
			-- sha:set_frag_vec4( 8, -vx, -vy )

			ps2( tr[ 1],  vx,  0 )
			ps2( tr[ 5], -vx,  0 )
			ps2( tr[ 9],  0,   vy )
			ps2( tr[13],  0,  -vy )
			ps2( tr[17],  vx,  vy )
			ps2( tr[21], -vx,  vy )
			ps2( tr[25],  vx, -vy )
			ps2( tr[29], -vx, -vy )
			gol.update_uniform_fragment_vec4()
			--gol.update_uniform_fragment_float()
			meu_fbo:draw_quad_fbo_pass( s, b_verbose, false,	true, false )

		end

		-- 1+JfaBergerac, 24100
		if self.b_1_jfa then
			do_loop_one( 0, 1, true )
		end
		local step_size = size_max_p2
		for i=1,self.loop_nb do
			step_size = step_size / 2
			do_loop_one( i, step_size, true )
		end
	end

	--OUT
	if self.b_out then
		local t_col = self.table_col 

		sha = self:get_shading( 3 )
		sha:set_frag_float_1_4( fx,fy, mi,ma )
		sha:set_frag_float_5_7( self.b_dist_freq_rel and self.dist_freq/size_max_used or self.dist_freq/size_max_tex,
									self.dist_offset, self.dist_gamma )

		sha:set_frag_float_12_13( t_col.offset, t_col.gamma )
		
		sha:set_frag_float_21( self.grad_size )
		sha:set_frag_float_22( self.vl_inter )
		sha:set_frag_float_23( self.out_src )
		sha:set_frag_float_24( self.out_factor )

		local id = t_col.b_flip and 2 or 1 
		if t_col.s_choice == 2 then
			sha:set_frag_vec4( id, ui.bu_col_in:get_rgba() )
			sha:set_frag_vec4( 3-id, ui.bu_col_out:get_rgba() )
		elseif t_col.s_choice == 1 then
			sha:set_frag_vec4( id, 0,0,0, 1 )
			sha:set_frag_vec4( 3-id, 1,1,1, 1 )
		end
		sha:set_frag_int_1_4(	t_col.s_choice + (self.b_src_to_alpha and 256 or 0),
								self.s_wrap,
								self.s_out_type + (256*256)*self.s_out_top,
								self.s_out_src )
		self:draw_layer( self.layer_shading_ids[3] )	--set shader init

		meu_fbo:draw_quad_fbo_pass( s, b_verbose,  true,	true, true )
		--gol.finish()
	end
end

function meu:draw()
	local meu_fbo = self:__update_fbo_internal()
	if not meu_fbo then return end


	--self:print( "sx_fbo is "..self.sx_fbo )
	if (not self.sx_fbo) or (self.sx_fbo <= 0) then
		return
	end

	self:do_fbo( 1 )
	self:draw_layers_begin()
		--ACTUAL DRAW
		self:draw_passes( meu_fbo )
	self:draw_layers_end()
end




