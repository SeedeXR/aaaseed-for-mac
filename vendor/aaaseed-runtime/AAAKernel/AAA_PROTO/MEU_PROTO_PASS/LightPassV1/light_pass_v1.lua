function meu:define_meu_infos( )
	return { author = "Mâa mrvux",
			tags = { "2d", "3d", "texture", "Core", "CoreGraphic", "draw", "renderpass" },
			help = "from the Gbuffer (a set of texture e.g. Albedo, normal, depth...\n"..
					"the state of lights and materials the light is computed in the output texture\n"..
					"the photorealistic rendering of the Monaco Aquarium was using this"
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

	self:add_camera()

	self:set_tab_key_def()

	ix,iy =  1,1
	ix,iy = self:define_ui_ndc( {ix,iy, 8,4}, true )

	bu = self:add_slider(	{ix,iy,		4,SY*.8}, 	"Normal Center", 	sha:get_ref_frag_float(6), nil, 0, 0, 1 )
	iy = iy + SY * .8
	bu = self:add_selector(	{ix,iy,		8,SY*3},	"Draw" )
		bu:set_nb( 5, 3 )
		bu:set_item_text(	1,	"Thru",		"Light",	"Nor01",	"Screen",	"World" )
		bu:set_item_text(	6,	"Albe",		"Nor",		"Spec",		"Emis",		"Depth" )
		bu:set_item_text(	11,	"Rough",	"Ao",		"Reflec",	"Emis.a",	"Shadow" )
		bu:set_target_param( sha:get_ref_frag_int(1) )
	iy = iy + SY * 3. + DY

	bu = self:add_selector(	{ix,iy,		8,SY},		"Lights" ):set_selection_multiple( true )
		bu:set_nb_min_0( 8 )
		bu:set_item_text_from_nb()
		ui.bu_sel_active = bu
	iy = iy + SY + DY

	bu = self:add_slider(	{ix,iy,		4,SY*.8}, 	"Y_Factor_Top", 	sha:get_ref_frag_float(4), nil, 0, -1, 1 )
	bu = self:add_slider(	{ix+4,iy,	4,SY*.8}, 	"Y_Factor_Bottom", 	sha:get_ref_frag_float(5), nil, 0, -1, 1 )
	iy = iy + SY*.8
	bu = self:add_slider(	{ix,iy,		8,SY}, 		"Light_Factor", 	sha:get_ref_frag_float(2), nil, .5, 0, 1 )

	iy = iy + SY + DY

	local SYC = SY * 1.
	bu = self:add_slider(	{ix+6,iy,	2,SYC}, 	"Shadow Iteration",	sha:get_ref_frag_int(2), nil, 4, 0, 15 )
		bu:set_text("Iteration")
		bu:set_min_max_strict(true)
--	bu = self:add_slider(	{ix,iy,		4,SY}, 		"Shadow Factor", 	sha:get_ref_frag_float(3), nil, .5, 0, 1 )
	bu = self:add_rgbfa(	{ix,iy,		6,SYC},		"Shadow",		false	)
		ui.bu_shadow = bu
	iy = iy + SYC
	bu = self:add_rgbfa(	{ix,iy,		8,SYC},		"Ambient",		false	)
		ui.bu_ambient = bu
	iy = iy + SYC + DY

	SYC = SY * 1.
	bu = self:add_rgbfa(	{ix,iy,		8,SYC},		"Offset",		false	)
		bu:set_values( { 1,1,1, 0, 0 } ) 
		ui.bu_color_out_offset = bu
	iy = iy + SYC
	bu = self:add_rgbfa(	{ix,iy,		8,SYC},		"Out",			false	)	
		ui.bu_color_out_factor = bu
	iy = iy + SYC
	bu = self:add_rgbfa(	{ix,iy,		8,SYC},		"Gamma",		false	)
		ui.bu_gamma_out = bu

	ix,iy = 9,2.2

	self.tex_nb = 9
	SY = (14.5-iy) / 3
	local names = { "Diffuse", "Normal AO", "Specular Reflection", "Emissive", "Depth", "Shadow Map 1", "Intensity Map 1", "Shadow Map 2", "Intensity Map 2"}

	local function add_tex( ix,iy, b,e )
		for i = b,e do
			--local ii = i<=5 and i-1 or i
			local ii = i-b
			local iu = ii % 2
			local iv = (ii-iu) / 2
			local tex = self:add_bu_texture( {ix+iu*4,iy+SY*iv, 4,SY}, names[i] or ("Tex_"..i), i, false )
			if i==7 then
				tex:set_send( false )
			end
		end
	end

	add_tex( 9,iy,		1,6 )

	self:set_tab_key( "More" )

	add_tex( 9,iy, 		7,7 )
	add_tex( 9,iy+SY,	8,9 )

end

function meu:init()
	local ref = self.ref

	self:set_meu_fbo( "light" )

	local sha = self:add_shading()
	sha:set_save_frag_float(	false,	1,5 )
	sha:set_save_frag_int(		false,	2,2 )
	sha:set_save_frag_vec4(		false,	1,4 )

	local obj = self:get_layer( 1 )
	if obj then
		local obj = aaa.obj.get_down_by_class( obj, "lights_switch" )
		local t = {}
		for i=1,8 do
			t[i] = param.get_ref( obj, "light_switch_"..(i-1) )
		end
		ref.light_switch = t
	end
end

function meu:set_shadow_rgbf( r,g,b, f )
	self.ui.bu_shadow:set_rgbf( r,g,b, f )
end

function meu:update()
	local t_active = self.ui.bu_sel_active:get_values()
	local r = self.ref.light_switch
	for i=1,8 do
		param.set( r[i], t_active[i] )
	end
end

function meu:set_out_black_level( black_level )
	self.black_level = black_level
end
function meu:draw()
	local ui = self.ui
	self:do_fbo()

	local sha = self:get_shading()
	local r,g,b,a = ui.bu_shadow:get_rgba()
	sha:set_frag_vec4( 1, r,g,b,a )
	r,g,b,a = ui.bu_ambient:get_rgba()
	sha:set_frag_vec4( 2, r,g,b,a )

	local ro,go,bo,ao = ui.bu_color_out_offset:get_rgba()
	local rf,gf,bf,af = ui.bu_color_out_factor:get_rgba()
	local bl = self.black_level 
	if bl and bl~=0. then
		ro = ro - bl
		go = go - bl
		bo = bo - bl
		rf = rf + bl
		gf = gf + bl
		bf = bf + bl
	end
	sha:set_frag_vec4( 5, ro,go,bo,ao )	--todo reorder (in all shaders too)
	sha:set_frag_vec4( 3, rf,gf,bf,af )

	r,g,b,a = ui.bu_gamma_out:get_rgba()
	sha:set_frag_vec4( 4, r,g,b,a )

	for i=2,self.tex_nb do
		self:bind_texture_to_unit( i )
	end

	gol.set_tex_unit_cur(5)	-- shadow map
		-- 	gol.set_minification_linear()
		-- if true then
		-- 	gol.set_magnification_linear()
		-- else
		-- 	gol.set_minification_nearest()
		-- 	gol.set_magnification_nearest()
		-- end
--		gol.set_wrap_2d_border()
	gol.set_tex_unit_cur(6)	-- Intensity Map 1
		gol.set_wrap_2d_repeat()
--	gol.set_tex_unit_cur(7)
--		gol.set_wrap_2d_border()
	gol.set_tex_unit_cur(8)	-- Intensity Map 2
		gol.set_wrap_2d_repeat()
	gol.set_tex_unit_cur(0)

 	self:draw_layers_begin()
 		self:draw_layer( 1 )
		self:draw_layer( 2 )
 	self:draw_layers_end()
end

function meu:get_preset_nb() 	return 32	end


