
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()
	local ix = 1
	local iy = 1
	local SY = .8
	local DY = .2

	self:add_camera()
	
	ix,iy = self:define_ui_ndc( nil, true )

	bu = self:add_selector(	{ix,iy,	8,SY*3},	"Draw" )
		bu:set_nb_min_0( 5, 3 )
		bu:set_item_text(	1,	"Thru",		"Light",	"Nor01",	"Screen",	"World" )
		bu:set_item_text(	6,	"Albe",		"Nor",		"Spec",		"Emis",		"Depth" )
		bu:set_item_text(	11,	"Rough",	"Ao",		"Reflec",	"Emis.a",	"Shadow" )
		bu:set_target_param( sha:get_ref_frag_int(1) )

	iy = iy + SY * 3.5
	bu = self:add_slider(	{ix,iy,	8,SY}, 		"Ambient Offset", 	sha:get_ref_frag_float(1), nil, .5, 0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, 		"Light Factor", 	sha:get_ref_frag_float(2), nil, .5, 0, 1 )
	iy = iy + SY + DY

	bu = self:add_rgbfa(	{ix,iy,	8,SY*1.5},	"Shadow color",		false	)
		ui.bu_shadow_color = bu
	iy = iy + SY * 1.5
	bu = self:add_slider(	{ix,iy,	4,SY}, 		"Shadow Factor", 	sha:get_ref_frag_float(3), nil, .5, 0, 1 )
	bu = self:add_slider(	{ix+4,iy,	4,SY}, 	"Shadow Iteration", sha:get_ref_frag_int(2), nil, 4, 0, 15 )

	iy = iy + SY * 1.5

	bu = self:add_selector(	{ix,iy,	8,SY},		"Lights" ):set_selection_multiple( true )
		bu:set_nb_min_0( 8 )
		bu:set_item_text_from_nb()
		ui.bu_sel_active = bu

	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, 		"Y Factor Top", 		sha:get_ref_frag_float(4), nil, 0, -1, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, 		"Y Factor Bottom", 		sha:get_ref_frag_float(5), nil, 0, -1, 1 )

	iy = 2.2
	self.tex_nb = 9
	SY = (16-1-iy) / self.tex_nb
	local names = { "Diffuse", "Normal AO", "Specular Reflection", "Emissive", "Depth", "Shadow Map 1", "Intensity Map 1", "Shadow Map 2", "Intensity Map 2"}
	self:add_bu_texture_target_unit(	{9,iy,	nil,SY},	names[1], 1, false )

--todo why this here (hidden ?)
	for i=2,self.tex_nb do
		self:add_bu_texture(	{9,iy+SY*(i-1), nil,SY}, names[i] or ("Tex_"..i), i, false )
	end
end

function meu:init()
	local ref = self.ref

	self:set_meu_fbo( "light" )

	local sha = self:add_shading()
	sha:set_save_frag_float(	false,	1,5 )
	sha:set_save_frag_int(		false,	2,2 )
	sha:set_save_frag_vec4(		false,	1,1 )

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

function meu:update()
	local t_active = self.ui.bu_sel_active:get_values()
	local r = self.ref.light_switch
	for i=1,8 do
		param.set( r[i], t_active[i] )
	end
end

function meu:draw()
	local ui = self.ui
	self:do_fbo()

	local sha = self:get_shading()
	local r,g,b,a = ui.bu_shadow_color:get_rgba()
	sha:set_frag_vec4( 1, r,g,b,a )

	for i=2,self.tex_nb do
		self:bind_texture_to_unit( i )
	end
--	gol.set_tex_unit_cur(5)
--		gol.set_wrap_2d_border()
	gol.set_tex_unit_cur(6)
		gol.set_wrap_2d_repeat()
--	gol.set_tex_unit_cur(7)
--		gol.set_wrap_2d_border()
	gol.set_tex_unit_cur(8)
		gol.set_wrap_2d_repeat()
	gol.set_tex_unit_cur(0)

 	self:draw_layers_begin()
 		self:draw_layer( 1 )
		self:draw_layer( 2 )
 	self:draw_layers_end()
end




