-- done for IMMERSION but unused as of September 2020

function meu:define_base_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	self:add_shading_ui( {1,1,	8}  )
	self:add_rendering()
	
	iy = 2
	bu = self:add_button(	{ix,iy,		SY,SY},	"Draw Cube", 	self, "b_draw_cube", false )
	iy = iy + SY
	local bdd = self:get_layer_bdd(3)
	bu = self:add_button(	{ix,iy,		SY,SY},	"Draw ids", 	self, "b_draw_ids", false )
	bu = self:add_trig(		{ix+4,	iy,	4,SY},	"Focus"	):set_function_on_click( aaa.obj.set_focus_ui, bdd )
	iy = iy + SY
	local SYB = SY*3
	bu = self:add_sliders_xyz(	{ix,iy,		4,SYB, b_vert=true},	"center",	self.instance_center, 	false,	100.0	)
	bu = self:add_sliders_xyz(	{ix+4,iy,	4,SYB, b_vert=true},	"spread",	self.instance_spread, 	false,	100.0	)
	iy = iy + SY*3

	bu = self:add_slider(	{ix,iy,		8,SY},	"Vertex Nb",	bdd, "vertex_nb", 12, 1, 1024 ):set_value_type_integer(true)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,SY},	"Instance Nb",	bdd, "instance_nb", 64, 1, 1024 ):set_value_type_integer(true)
	iy = iy + SY

	bu = self:add_slider(	{ix,iy,		4,SY},	"Size X",		self, "size_x", 	1, 0, 5 ):set_color_back("x")
	bu = self:add_slider(	{ix+4,	iy,	4,SY},	"Size Y",		self, "size_y",		10, 0, 50 ):set_color_back("y")
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		4,SY},	"Roll",			self, "roll",		0, -.5, .5 ):set_color_back("z")
	bu = self:add_slider(	{ix+4,	iy,	4,SY},	"Rot X",		self, "rot_x",		0, -.5, .5 ):set_color_back("x")
	iy = iy + SY
	self:add_slider_two({	ix,iy,		8,SY},	"intensity range",	nil, sha:get_ref_frag_float(3), sha:get_ref_frag_float(4),  0, 1, 0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		4,SY},	"intensity",	sha:get_ref_frag_float(1), nil, 1, 0, 1 )
	iy = iy + SY

	iy = 2
	local SYT = 2.5
	for i=1,2 do
		self:add_bu_texture_target_unit( {9,iy, nil,SYT}, "Tex_"..i, i	)
		iy = iy + SYT
	end
	ix = 9
	iy = self:define_time(	{ix,iy,		1,SY}	)--, 0.0 )
	iy = iy + SY
	bu = self:add_rgbfa(	{ix,iy,		8,SY*1.5},	"Shadow color",		false	)
		ui.bu_color = bu
	iy = iy + SY*1.5
end

function meu:define_control_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix = 1
	local iy = 1
	local DY = .2

	local SX, SY = 8, 1
	local SYC = SY * 1.5

	bu = self:add_sliders_xyz(	{ix,iy,	SX,SY},		"OBegin",		self.dist_o_beg, 	false,	5	)
	iy = iy + SY	
	bu = self:add_sliders_xyz(	{ix,iy,	SX,SY},		"OEnd",			self.dist_o_end, 	false,	5	)
	iy = iy + SY + DY	

	bu = self:add_slider(	{ix,iy,		2,SYC},		"Horizon",		self, "y_center", 5, -10, 10 ):set_color_back("y")
	bu = self:add_rgbf(		{ix+2,iy,	6,SYC},		"Top_Color",	false )
		self.bu_col_top = bu	
	iy = iy + SYC	
	bu = self:add_slider(	{ix,iy,		2,SYC},		"Range", 		self, "y_range", 5, 0, 10 ):set_color_back("y")
	bu = self:add_rgbf(		{ix+2,iy,	6,SYC},		"Bottom_Color", false )
		self.bu_col_bottom  = bu	
	iy = iy + SYC	

	bu = self:add_slider(	{ix,iy,		4,SYC},		"Horizon_Gamma",sha:get_ref_frag_float(12), nil, 1, 0.2, 5 ):set_color_back("y")
	iy = iy + SY

	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		6,SY},		"Sun_LR",		self, "sun_lr",		0,	-180, 180	)
	bu = self:add_slider(	{ix+6,iy-2*SY,	2,SY*3},"Sun_Azi",		self, "sun_azi",	0,	0, 90	)
	iy = iy + SY
end

function meu:define_ui()
	self:set_tab_key( "Base" )
	self:define_base_ui()

	self:set_tab_key( "Ctrl" )
	self:define_control_ui()

end

function meu:init()
	local ref = self.ref
	local sha = self:add_shading()
	self.instance_spread = { x=5, y=5, z=5 }
	self.instance_center = { x=0, y=0, z=0 }

	self.dist_o_beg	= { x=0, y=0, z=0 }
	self.dist_o_end	= { x=0, y=0, z=0 }
	self.sun_pos	= { x=0, y=0, z=0 }

	sha:set_save_vert_float( false,	1,6 )
	sha:set_save_vert_vec4(  false,	1,2 )

	sha:set_save_frag_float( false,	2,2 )
	sha:set_save_frag_vec4(  false,	1,1 )
end

function meu:update_ui()
	self:update_time_ui()
end

function meu:update()
	local ui = self.ui
	local time = self:update_time()
	local sha = self:get_shading()

	sha:set_frag_float_2( time )

	local spread = self.instance_spread
	sha:set_vert_float_1_3( spread.x, spread.y, spread.z )

	local center = self.instance_center
	sha:set_vert_float_4_6( center.x, center.y, center.z )

	local f = -math.pi / 180
	local a_lr = self.sun_lr * f
	local azi = self.sun_azi * f

	local angle = a_lr
	sha:set_vert_vec4( 1, math.cos(angle), math.sin(angle), self.size_x, self.size_y )
	angle = azi
	sha:set_vert_vec4( 2, math.cos(angle), math.sin(angle) )

	local r,g,b,a = ui.bu_color:get_rgba()
	sha:set_frag_vec4( 1, r,g,b,a )
end

function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 )	--	set the "attribute" no more shader her
		if self.b_draw_cube then
			self:draw_layer( 2 )
		end
		if self.b_draw_ids then
			self:draw_layer( 3 )
		end
	self:draw_layers_end()
end




