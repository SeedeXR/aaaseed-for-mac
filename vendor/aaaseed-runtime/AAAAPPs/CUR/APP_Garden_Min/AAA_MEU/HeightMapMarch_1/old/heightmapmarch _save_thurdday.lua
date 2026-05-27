
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local ix,iy
	local sha = self:get_shading()

	ix,iy = 1,3.7
--	self:add_mapping_by_side()
	self:add_shading_ui(	{9,	2.5,8, 1} )
	self:set_tab_key_def()
	self:add_bu_texture_target_unit( 	{ix,1}, 	"tex_height", 1	)
	self:add_bu_texture_target_unit( 	{ix,iy+1}, "tex_refract", 2	)
	self:add_camera()


	local module = aaa.modules.get_cur()
	table.print(module, "nodule" )
	local lights = aaa.obj.get_down_by_class_no_error( module, "lights" )
	table.print(lights, "light" )
	local light0 = aaa.obj.get_down_by_class_no_error( lights, "lights" )
	table.print(light0, "light0" )
	local light_0 = aaa.obj.get_down_no_error( light0, "light_0" )
	table.print(light_0, "light_0")
	bu = self:add_button( {ix, 8, 	3,	.9 }, "b_speed", self, "b_speed"	):set_text_rect_ratio( 2 )
	bu = self:add_slider(	{1,9,	8,1},	"Speed",	self, "speed",	.2,		-1,	1 )
	bu:set_meter( false )

	ix, iy = 9, 4
	bu = self:add_selector(	{ix,iy,	8,1}, "Render" )
		bu:set_nb_min_0( 6, 1 )
		bu:set_item_text( 2, "GradRG", "?", "Normal", "March", "?" )
		bu:set_target_param( sha:get_ref_frag_int( 1 ) )


	self:set_tab_key( "March" )
	ix, iy = 1, 2

	bu = self:add_slider(	{ix,iy,		8,1},	"Iter Num",		self, "n_iter",			250,	0,	500 )
	iy = iy + 1.
	bu = self:add_slider(	{ix,iy,		5,1},	"March_step",	self, "march_step",		.5,  	0,	1. )
	iy = iy + 1.
	bu = self:add_slider(	{ix,iy,		5,1},	"Epsilon",		self, "epsilon",		.0001,  0,	.01 )

	ix, iy = 1, iy+1
	bu = self:add_slider(	{ix,iy,		2,1},	"Pos_x", self, "pos_x", 0.,  -5., 5. ):set_color_back("x")
	bu = self:add_slider(	{ix+2,iy,	2,1},	"Pos_y", self, "pos_y", 0.,  -5., 5. ):set_color_back("y")
	bu = self:add_slider(	{ix+4,iy,	2,1},	"Pos_z", self, "pos_z", -1.,  -5., 5. ):set_color_back("z")

	ix, iy = 1, iy+1
	bu = self:add_slider(	{ix,iy,		2,1},	"Rot_x", self, "rot_x", 0.,  -5., 5. ):set_color_back("x")
	bu = self:add_slider(	{ix+2,iy,	2,1},	"Rot_y", self, "rot_y", 0.,  -5., 5. ):set_color_back("y")
	bu = self:add_slider(	{ix+4,iy,	2,1},	"Rot_z", self, "rot_z", -1.,  -5., 5. ):set_color_back("z")

	bu = self:add_slider(	{1,iy+1,	8,1},	"Height",	self, "height",			1,		0,	1 )

	self:set_tab_key( "Lights" )
	ix, iy = 1, 4

	bu = self:add_slider(	{ix,iy,		2,1},	"Light_x",	self, "light_x", 1.,  -5., 5. ):set_color_back("x")
	bu = self:add_slider(	{ix+2,iy,	2,1},	"Light_y",	self, "light_y", 1.,  -5., 5. ):set_color_back("y")
	bu = self:add_slider(	{ix+4,iy,	2,1},	"Light_z",	self, "light_z", 1.,  -5., 5. ):set_color_back("z")
	bu = self:add_button(	{ix+6,iy, 	3,.9},	"B_Flare",	self, "b_flare"	):set_text_rect_ratio( 2 )


	ix, iy = 9, 4
	ui.bu_col = {}
	for i=1,7 do
		iy = iy + 1.
		bu = self:add_button({ix,iy, 	3,.9}, 	self.lighting[i], self, "b_"..self.lighting[i]	):set_text_rect_ratio( 2 )
		if i<6 then
			ui.bu_col[i] = self:add_rgbf(	{ix+3,iy,	5,.9}, 	self.lighting[i].."_col", false )
		end
	end

	bu = self:add_slider(	{4,8,		5,1},	"Diffuse_Factor",	self, "diffuse",	1.,  0,	10 )
	bu = self:add_slider(	{4,9,		5,1},	"Specular_Term",	self, "specular",	1.,  0,	30 )

	bu = self:add_slider(	{ix+3,iy,	5,1},	"Refract_ind",	self, "refract_ind",	1.333,  0,	3 )

	self:set_tab_key( "Materials" )
end

function meu:init()
	self.lighting = { "texture", "amb", "ao", "diffuse", "specular", "shadow", "refract" }
	self.diffuse = 0
	self.specular = 0
	self.b_flare = 0
	self.b_speed = 0
	self.light_x = 0
	self.light_y = 0
	self.light_z = 0
	self.pos_x = 0
	self.pos_y = 0
	self.pos_z = 0
	self.rot_x = 0
	self.rot_y = 0
	self.rot_z = 0
	for i=1,6 do
		self[ "b_"..self.lighting[i] ] = 1
	end
	self.time = 0
--	self:print( "je passe ds l'init" )
	self:add_shading( 1, "HeightMarch" )
end

function meu:update_ui()
	self:update_shader_ui()
	local sha = self:get_shading()
	local lighting = self.lighting

	--for i=2,4 do
	--	sha:set_frag_int( i, self["b_"..lighting[i]] )
	local b_lighting = {}
	for i=1,7 do
		b_lighting[i] = self[ "b_"..lighting[i] ]
	end
	sha:set_frag_int_3( b_lighting[1] + 2 * ( b_lighting[2] + 2 * ( b_lighting[3] + 2 * ( b_lighting[4] + 2 * ( b_lighting[5] + 2 * ( b_lighting[6] + 2 * ( b_lighting[7] + 2 * ( self.b_flare + 2 * self.b_speed ) ) ) ) ) ) ) )
	--	sha:set_frag_vec4( 5, self["b_"..lighting[1]], self["b_"..lighting[2]], self["b_"..lighting[3]], self["b_"..lighting[4]] )
	--	sha:set_frag_vec4( 6, self["b_"..lighting[5]], self["b_"..lighting[6]], 0, 0 )
		--print(self["b_"..self.lighting[i]].."b_"..self.lighting[i])
	--end

	sha:set_frag_float_2( self.march_step )
	sha:set_frag_float_3( self.epsilon )
	sha:set_frag_float_5( self.height )
	sha:set_frag_float_6( 1. / self.refract_ind )
	sha:set_frag_float_7( self.diffuse )
	sha:set_frag_float_8( self.specular )

	sha:set_frag_int_2( self.n_iter )

	local bu_col = self.ui.bu_col
	for i=1,5 do
		local r,g,b,a = bu_col[i]:get_rgba()
		sha:set_frag_vec4( i, r,g,b,a ) -- problem : all slider change all values ..?!
	end
	sha:set_frag_vec4( 6, self.light_x, self.light_y, self.light_z, 0. )
	sha:set_frag_vec4( 7, self.pos_x, self.pos_y, self.pos_z, 0. )
	sha:set_frag_vec4( 8, self.rot_x, self.rot_y, self.rot_z, 0. )
end

function meu:update()
	local t = self.time
	local sha = self:get_shading()
	t = t + aaa.time.dt * math.pow( self.speed*4, 3 )
	self.time = t
	sha:set_frag_float_4( t )

	--self:print( self.ui.bu_col:get_rgba() )
end
