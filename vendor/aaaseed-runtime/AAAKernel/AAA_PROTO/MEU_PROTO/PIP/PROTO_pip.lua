
if CLASS.DECLARE( "MEU_PIP", MEU ) then
	MEU_PIP:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,						
		"Made mainly to display Video, display a Texture as a rectangle.",
		"  Handle automatic fade, Mask, Incrustation, tint and so...",
		"  In general Used in combination with MEUs Video." )
end

function MEU_PIP:define_meu_infos( )
	return { 	name_long = "Picture In Picture",
				author = "Mâa",
				tags = { "Texture", "Vj", "Core", "CoreGraphic", "2d", "Surface", "Draw" }
			}
end


function MEU_PIP:add_incrust_pip( rect, name, group_name, x, b_mode, b_gainbias )
	local ix,iy, sx,sy = unpack(rect)
	local bu
	sy = sy / 2
	local t = { bu={} }
	
	self:begin_bu_group( group_name )
	bu = self:add_button(			{ix+x,iy,		3,sy},		"active",	t,"b_active",	false	):set_text(name)
		:set_text_rect_ratio( 3 )
		:set_text( name )
		t.bu_active = bu
	if group_name == "Incrustation" then
		bu = self:add_button(		{ix+x+3,iy,		2,1},   	"Mode" )
			bu:set_menu{ "Luma", "Color" }
			bu:set_text_selector( true )
			t.bu.method = bu
		iy = iy + sy
		t.bu_color = self:add_rgbf(	{ix+x,iy,		8-x,sy}, 	"Color",	false )
	end
	iy = iy + sy

	bu = self:add_slider_two(		{ix+x,iy,		8-x-1,sy},	"Luma",		t, "min", "max", 0,1, 0,1 )
		bu:set_draw_step( "linear" )
		t.bu.slider = bu
	bu = self:add_button(			{ix+7,iy,		1,sy},		"inverse",	t,"b_inv",	false ):set_text_inside(true):set_text("Inv")
		t.bu.flip = bu
	iy = iy + sy
	if b_gainbias then
	 	sx = (8-x)/2
	 	t.bu.gain = self:add_slider({ix+x,iy,		sx,sy},		"Gain",		t, "gain",	.5,	0,1 )
	 	t.bu.bias = self:add_slider({ix+x+sx,iy,	sx,sy},		"Bias",		t, "bias",	.5, 0,1 )
	 	iy = iy + sy
	end
	self:end_bu_group()
	return  iy, t
end

function MEU_PIP:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local sha = self:get_shading()

	local SX,SY = 8,1
	local SYC = SY*.6
	local SY_TEX = 4.8
	local DY = .2
	local layer = self:get_layer(1)

	ref.map = aaa.layer.build_bank_bind_2d_ref_table( layer, 0,3 )

	local ix,iy = 1,1
	bu = self:add_bu_texture(		{ix,iy,			8,SY_TEX},	"Tex", 1, false ) --todo use dir_maj
		ui.bu_tex = bu
	iy = iy + SY_TEX
	local iy_start = iy

	self:set_tab_key( "Main" )

	self:add_button(				{ix,iy,			2.5,SY},	"Inverse",	self, "b_inverse", false 	)
	iy, self.incrustation =
		self:add_incrust_pip(		{ix,iy,			SX,SY*2},	"Incrust", "Incrustation", 3, true, false	)

	local SYS = SY
	self:begin_bu_group( "mask" )
	local ta = { bu={} }
	self.mask = ta 
	bu = self:add_trig_method(		{ix,iy-SYS+DY,	2.5,SYS},	"Swap",	self, "swap_texture_bind", 1,3 	)

	iy = iy + DY*2
	self:add_button(				{ix,iy,			2,SY},		"active",	ta, "b_active",		false 	):set_text("Mask")
	ta.bu.mul = self:add_button(	{ix,iy+SY,		1.5,SY},	"Multiply",	ta, "b_multiply",	false 	):set_text("Mul")
	ta.bu.see = self:add_button(	{ix+1.5,iy+SY,	1.5,SY},	"See",		ta, "b_see", 		false 	)
	ta.bu.tex =
		self:add_bu_texture(		{ix,iy+SY*2,	8,SY_TEX},	"Tex", 3, false ):set_text( "Mask Tex" ) --todo use dir_maj

	iy, ta.incrustation =
		self:add_incrust_pip(		{ix,iy,			SX,SY*2},	"Remap", "Mask.Remap", 3.5, false, false	)
	

	self:set_tab_key()
	iy = iy + SY_TEX

	self:add_camera()

	ix,iy = 9,1 + 1 + DY
	bu = self:add_slider(			{ix+4,iy,		4,SY},		"delay",	self, "delay",	1, 0,16 )
--	bu = self:add_button( {8, iy }, "fade", self, "b_fade", false )
--	self.b_fade = true
	bu = self:add_button(			{ix,iy,			SY,SY},		"Sync",		self, "b_sync",	true	)
		bu:set_text_rect_ratio(1.8)
	--local SY = 1.2
	bu = self:add_trig(				{ix+2.8,iy,	 	1.2,SY},	"go",		self, "b_go",	false	)
		bu:set_text_rect_ratio(1)
		ui.bu_go = bu
	iy = iy + SY + DY

	self:add_size_uvf_video(		{ix,iy,			SX,SY*2}	)
	iy = iy + SY*2 + DY
	self:add_transfo(				{ix,iy,			8,2.4}		)
	iy = iy + 2.4 + DY

	self:add_rgbf(					{ix,iy,			8,SY}		)
	iy = iy + SY
	self:add_button(				{ix,iy,			2,SY},		"Tint",				self,"b_tint",	false )
	ui.bu_tint_back	= self:add_rgbf({ix+2,iy,		6,SY},		"Background", false	)
	iy = iy + SY + DY

	--SY = SY * .8
	ui.bu_offset = self:add_rgbf(	{ix,iy,			8,SY},		"Offset", 	false )
	ui.bu_offset:set_rgb( 0,0,0)
	iy = iy + SY

	ui.bu_gamma = self:add_rgbf(	{ix,iy,			8,SY}, 		"Gamma",	false )
		:set_gamma(true)
	iy = iy + SY + DY
	--SY = SY / .8

	self:add_button(				{ix,iy,			2,SY}, 		"Clamp",			self,"b_clamp",		false	)
	
	self:add_button(				{ix+4-SY,iy,	SY,SY}, 	"Fuzzy active",		self,"b_fuzzy",		false	)
		:set_text_visible( false )
	ui.bu_fuzzy =
		self:add_slider(			{ix+4,iy,		4,SY}, 		"Fuzzy",			self,"fuzzy", 		.01, 0,.25	)
	iy = iy + SY + DY

	self:add_blending( {ix,iy, 2,SY} )

	self:set_tab_key( "Map" )
	ix,iy = 1,iy_start
	self:add_nb_uv(					{ix,iy,			SX,SY}		)
	iy = iy + SY + DY
	self:add_mapping_by_side_only(	{ix,iy, 		SX,SY}		)
	iy = iy + SY + DY
	self:add_mapping_hexa(			{ix,iy,			SX,SY*3}	)
	iy = iy + SY*3 + DY
	self:add_button( 				{ix,iy, 		SY,SY},		"UV Bdd Src", ref.uv_src_bdd_use,nil,	false )

	self:add_rendering()
	--todoadd a text field with the src bdd name Symbo 
end

function MEU_PIP:set_texture( bind )	self:set_texture_bind_2d( 1, bind )	end
function MEU_PIP:set_mask(    bind )	self:set_texture_bind_2d( 3, bind )	end

function MEU_PIP:init()
	local ref = self.ref
	local mapping = aaa.layer.get_mapping( self:get_layer(1) )
	ref.uv_src_bdd_use = param.get_ref( mapping, "uv_src_bdd_use" )
	param.set( mapping, "tex_wrap_u", "MIRRORED_REPEAT" )	-- MIRRORED_REPEAT
	param.set( mapping, "tex_wrap_v", "MIRRORED_REPEAT" )	-- MIRRORED_REPEAT

	local sha = self:add_shading()
	sha:set_save_frag_int(		false,	1,4 )
	sha:set_save_frag_float(	false,	1,5 )
	sha:set_save_frag_vec4(		false,	1,1 )
	sha:set_save_frag_vec4(		false,	4,5 )
	sha:set_save_frag_vec4(		false,	8,8 )
end

function MEU_PIP:is_fading()
	return self.time_fade ~= nil
end

function MEU_PIP:__trig_fade()
	self.b_go						= false
	--todo dim_next is nil
	if self.time_fade then -- case when a fade is trigged while a fade is already going on
		self.bind, self.dim	= self.bind_next, self.dim_next
		self.bind_next, self.dim_next = self.bind_2d_asked, self.dim_asked
		self.time_fade = self.delay - self.time_fade
	else
		self.bind_next, self.dim_next = self.bind_2d_asked, self.dim_asked
		self.time_fade = 0
	end
end

function MEU_PIP:__stop_fade()
	if self.time_fade then
		--aaa.print_method()
		self.time_fade = nil
		if self.bind_next then
			self.bind		=	self.bind_next
			self.dim		=	self.dim_next
			self.bind_next	= nil
			--self:print( "__stop_fade() bind is "..self.bind )
		end
	end
end


function MEU_PIP:update_ui_incrust( t, b )
	local array_bu = t.bu
	local a = array_bu.slider:interpolate_alpha_bu( b )
	for _,bu in PAIRS(array_bu) do
		bu:set_alpha_bu(a)
	end
end

function MEU_PIP:update_ui()
	local ui = self.ui
	local t = self.mask
	local b = t.b_active
	local array_bu = t.bu
	local a = array_bu.see:interpolate_alpha_bu( b )
	for _,bu in PAIRS(array_bu) do
		bu:set_alpha_bu(a)
	end
	-- local bu = t.bu_color
	-- if bu then
	-- 	--self:print( b and (array_bu.method:get_value() > 1) )
	-- 	bu:interpolate_alpha_bu( b and (array_bu.method:get_value() > 1) )
	-- end
	t = t.incrustation
	t.bu_active:interpolate_alpha_bu( b )
	self:update_ui_incrust( t, t.b_active and b )

	t = self.incrustation
	b = t.b_active
	self:update_ui_incrust( t, b )
	t.bu_color:interpolate_alpha_bu( b and (t.bu.method:get_value() > 1) )

	ui.bu_fuzzy:interpolate_alpha_bu( self.b_fuzzy )
	ui.bu_tint_back:interpolate_alpha_bu( self.b_tint )
end

function MEU_PIP:update()
	local b_changed, bind_2d, dim = self.ui.bu_tex:get_bind_2d_dim_asked()
	if b_changed then
		self.bind_2d_asked, self.dim_asked = bind_2d, dim
	end

	if self.b_sync then
		if b_changed then
			self:__trig_fade()
		end	
	elseif self.b_go then
		self:__trig_fade()
	end

	self.bind_mask = self.mask.b_active and self:get_texture_bind_2d(3) or nil
	--self:print( self.bind_mask )
end

function MEU_PIP:__draw_pip_low( dim, bind1, bind2, inter, alpha, bind_mask )	--todo restore dim
	--aaa.show( bind1.." ->"..bind2.."   "..inter )
--	bind1 = 33
--	bind2 = 37
	--aaa.print_fn()
	local ref = self.ref
	param.set( ref.tex_dim, dim )

	if bind1 then
	--	next line removed a bug 2022 April searching in C what is the problem	
	---		gol.set_tex_unit_2d_bind( 0, bind1  + 500 )
		self:set_bind_2d( bind1, 1 )
	--	gol.set_wrap_2d_mirror()
	--	gol.set_tex_unit_2d_bind( 0, bind1 )
	end
	if bind2 then
		self:set_bind_2d( bind2, 2 )
	--	gol.set_wrap_2d_mirror()
	--	gol.set_tex_unit_2d_bind( 1, bind2 )
	end

	ref.color:set_alpha( alpha )

	local sha = self:get_shading()
	sha:set_frag_float_1( inter )

	local t = self.incrustation
	local s_incrust_method 
	if t.b_active then
		if t.b_inv then
			sha:set_frag_float_2_3( t.max, t.min )
		else
			sha:set_frag_float_2_3( t.min, t.max )
		end
		--sha:set_frag_float_6_7( t.gain, t.bias )
		s_incrust_method = t.bu.method:get_value()
		if s_incrust_method >=2 then
			local r,g,b = t.bu_color:get_rgb()
			sha:set_frag_vec4( 7, r,g,b )	
		end	
	else
		--sha:set_frag_float_2_3( 0, 0 )
		s_incrust_method = 0
	end
	sha:set_frag_int_2( s_incrust_method )

	local mask = self.mask
	local bf_mask
	if mask.b_active then
		gol.set_tex_unit_cur( 2 )
			gol.set_texture_dim( 2 )
			gol.bind_texture( bind_mask )
			--gol.set_wrap_2d_mirror()
			--gol.set_texture_dim( 2 )
		gol.set_tex_unit_cur( 0 )

		--gol.set_tex_unit_2d_bind( 2, bind_mask )
		local t = mask.incrustation
		if t.b_active then	sha:set_frag_float_4_5( t.min, t.max )
		else				sha:set_frag_float_4_5( 0, 0 )
		end

		bf_mask = 1
		if mask.b_see		then	bf_mask = bf_mask + 16		end
		if mask.b_multiply	then	bf_mask = bf_mask + 16*16	end
	else
		bf_mask = 0
	end
	sha:set_frag_int( 4, bf_mask )

	local ui = self.ui
	local r,g,b,a

	r,g,b = ui.bu_gamma:get_rgb()
	sha:set_frag_vec4( 4, r,g,b )

	r,g,b = ui.bu_offset:get_rgb()
	sha:set_frag_vec4( 5, r,g,b )

	if self.b_tint then
		r,g,b = ui.bu_tint_back:get_rgb()
		sha:set_frag_vec4( 6, r,g,b )
	end

	local f = 100000
	if self.b_fuzzy then
		local fy = 1 / math.max(self.fuzzy*.5,1/f)
		--todo use grid size not texture
		local fx = fy * self:get_texture_ratio_x()
		sha:set_frag_vec4( 8,fx,fy )	
	else
		sha:set_frag_vec4( 8, f,f )
	end
	--self:print( r.." "..g.." "..b.." "..a )

	sha:set_frag_int( 1, (self.b_inverse and 1 or 0) + (self.b_tint and 2 or 0) + (self.b_clamp and 4 or 0) )

	--gol.dump_tex_unit()
	self:draw_layer( 1 )
	--gol.dump_tex_unit()
	--aaa.obj.update_then_draw( ref.__layer_marked )
end

function MEU_PIP:draw()
	local ref = self.ref
	--self:print( "fade "..self.b_fade )
	local inter
	--if self.b_fade then
		local delay = self.delay
		if delay > 0 then
			inter = self.time_fade		
			if inter then
				inter = inter + aaa.time.dt
				--self:print( "time_fade -> "..inter )
				if inter > delay then
					self:__stop_fade()
					inter = nil
				else
					self.time_fade = inter
					inter = inter / delay
					--self:print( "fade inter is "..inter  )
				end
			end		
		else
			self:__stop_fade()
			inter = nil
		end
	-- else
	-- 	self:__stop_fade()
	-- 	inter = nil
	-- end
	self.ui.bu_go:set_text_color_busy( inter~=nil )

	--aaa.debug.debug()
	self:draw_layers_begin()
--[[
		if inter then
			--todo deal with no texture / fade to black
			--todo	fade to black/white then other
			--todo  have a nice fade even with no alpha
			--todo move to spip
			local color = ref.color
			local alpha = color:get_alpha()

			self:print( "\tinter -> "..inter )

			color:set_alpha( alpha )
			self:__draw_pip_low( self.dim, self.bind )

			color:set_alpha( alpha * (1-inter) )
			self:__draw_pip_low( self.dim_next, self.bind_next )

			color:set_alpha( alpha )
		else
]]--	
		--aaa.print( self.dim )
		local color = ref.color
		local alpha = color:get_alpha()

		self:__draw_pip_low( self.dim, self.bind, self.bind_next, inter or 0, self:get_alpha(), self.bind_mask )
		--self:__draw_pip_low( self.dim, 0, 32, 0, self:get_alpha() )
		--self:draw_layer( 1 )
	self:draw_layers_end()

--	self.ui.bu_go:print( "TEST" )
end
