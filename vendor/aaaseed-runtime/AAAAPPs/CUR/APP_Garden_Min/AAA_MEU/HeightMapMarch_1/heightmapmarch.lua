
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local ix,iy
	local SX, SY = 5, 1
	local sha = self:get_shading()

	ix,iy = 1,2.5
--	self:add_mapping_by_side()
	self:add_shading_ui( 	{9,2.5,	8,1} )
	self:set_tab_key_def()
	self:add_bu_texture_target_unit( {ix,iy}, "tex_height", 1	)
	iy = iy + 3.
	self:add_bu_texture_target_unit( {ix,iy}, "tex_refract", 2	)
	iy = iy + 3.
	self:add_bu_texture_target_unit( {ix,iy}, "tex_reflect", 3	)
	self:add_camera()

	--bu = self:add_button( {ix, 8, 	3,	1 }, "b_speed", self, "b_speed"	):set_text_rect_ratio( 2 )
	--bu = self:add_slider(	{1,9,	8,1},	"Speed",	self, "speed",	.2,		-1,	1 )
	--bu:set_meter( false )

	ix, iy = 9, 4
	bu = self:add_selector(	{ix,iy,	8,1}, "Render" )
		bu:set_nb_min_0( 6, 1 )
		bu:set_item_text( 2, "GradRG", "?", "Normal", "March", "?" )
		bu:set_target_param( sha:get_ref_frag_int( 1 ) )


	--self:set_tab_key( "March" )
	--ix, iy = 1, 2

	iy = iy + 1
	bu = self:add_slider(		{ix,iy,		SX,SY},		"Iter Num",		self, "n_iter",			250,	0,	500 ):add_values_def( 14, 42, 69, 100, 150, 200, 300, 350, 400, 450 )
	bu = self:add_slider(		{ix,iy+1,	SX,SY},		"March_step",	self, "march_step",		.5,  	0,	1. ):add_values_def( .001, .1, .5 )
	bu = self:add_slider(		{ix,iy+2,	SX,SY},		"Epsilon",		self, "epsilon",		.0001,  0,	.01 ):add_values_def( .001, .005 )

	ix = ix + 1
	bu = self:add_button(		{ix, iy+3, 	3,SY },		"b_mipmap",		self, "b_mipmap"	)
	bu = self:add_slider(		{ix,iy+4,	SX,SY},		"Mipmap Level",	self, "mipmap_level",	2,	0,	16 )
	ix = ix - 1

	bu = self:add_slider(		{ix,iy+5,	SX,SY},		"Height",		self, "height",			1,	0,	1 )
	bu = self:add_sliders_xyz(	{ix,iy+6,	SX,SY},		"Pos",			self.pos, 		false,	2.5	)
	bu = self:add_sliders_xyz(	{ix,iy+7,	SX,SY},		"Rot_cam",		self.rot_cam, 	false,	2.5	)
	bu = self:add_sliders_xyz(	{ix,iy+8,	SX,SY},		"Rot_plane",	self.rot_plane, false,	2.5	)

	self:set_tab_key( "Lights" )
	ix, iy = 1, 2
	bu = self:add_text_info({1,1,		3,.8}, 		"Lightings" )
	iy = iy + 1
	bu = self:add_button(	{ix,iy, 	3,.9}, 		"Light_Sphere",	self, "b_light_sphere"	)

	ui.bu_col = {}
	local has_col = { "1", "3", "6", "7" }
	for i=1,8 do
		iy = iy + 1
		bu = self:add_button(	{ix,iy, 3,.9}, 	self.lighting[i], self, "b_"..self.lighting[i]	):set_text_rect_ratio( 2 )
		for j=1,4 do
			if tostring(i) == has_col[j] then
				ui.bu_col[j] = self:add_rgbf(	{ix+3,iy,	SX,SY}, 	self.lighting[i].."_col", false )
			end
		end
	end
	bu = self:add_slider(	{ix+3,iy,	5,1},	"Refract_ind",		self, "refract_ind",		1.333,  0,3 )
	bu = self:add_slider(	{ix+8,iy,	5,1},	"Refract_intensity",self, "refract_intensity",	1.,		-2,2 )
	iy = iy + 1
	bu = self:add_button(	{ix,iy,		3,.9}, 	"Reflect",			self, "b_reflect"	)
	bu = self:add_slider(	{ix+3,iy,	5,1},	"Reflect_intensity",self, "reflect_intensity",	1.,		-2,2 )
	iy = iy + 1
	bu = self:add_button(	{ix,iy, 	3,.9}, 	"Gamma",			self, "b_gamma"	)
	bu = self:add_slider(	{ix+3,iy,	5,1},	"Gamma factor",		self, "gamma",		1,  0,3 ):add_values_def( .25, .5, .75, 1.25, 1.5, 1.75, 2., 2.25, 2.5, 2.75 )
	iy = iy + 1

	bu = self:add_slider(	{9,6,		2,1},	"Step_dist",		self, "step_dist",	.1,	0,1 )
	bu = self:add_slider(	{11,6,		2,1},	"Samples",			self, "samples",	1,	0,30 )
	bu = self:add_button(	{13,6,		2,1}, 	"ao_blur",			self, "b_ao_blur"	)

	bu = self:add_slider(	{4,7,		5,1},	"Diffuse_Factor",	self, "diffuse",	1,	0,10 )
	bu = self:add_slider(	{4,8,		5,1},	"Specular_Term",	self, "specular",	1,	0,30 )

	bu = self:add_slider(	{9,9,		3,1},	"Flare_term",		self, "flare_term",	1,	0,1 ):add_values_def( .001, .1, .5 )
	bu = self:add_slider(	{12,9,		3,1},	"Flare_intensity",	self, "flare_intensity",	1.,		-2,	2 )
	bu = self:add_button(	{15,10,		2,.9}, 	"Shadow_invert",	self, "b_shad_invert"	)
	bu = self:add_slider(	{9,10,		2,1},	"Smooth",	self, "shadow_smooth",	2.,		0,	128 ):add_values_def( 4, 8, 16, 32, 64 )
	bu = self:add_slider(	{11,10,		2,1},	"Min_d",	self, "shadow_min_d",	.01,	0,	.5 ):add_values_def( .001, .1, .5 )
	bu = self:add_slider(	{13,10,		2,1},	"Max_d",	self, "shadow_max_d",	.1, 	0,	3 ):add_values_def( .5, 1, 5 )

--	self:set_tab_key( "Materials" )
end

function meu:init()
	self.lighting = { "texture", "amb", "ao", "diffuse", "specular", "flare", "shadow", "refract" }
	self.bools = { "reflect", "ao_blur", "mipmap", "light_sphere", "gamma", "shad_invert" }
	self.xyz = { "x", "y", "z" }
	self.pos = { x=0, y=0, z=0 }
	self.rot_cam = { x=0, y=0, z=0 }
	self.rot_plane = { x=0, y=0, z=0 }

	for i=1,8 do
		self[ "b_"..self.lighting[i] ] = 1
	end
	for i=1,6 do
		self[ "b_"..self.bools[i] ] = 1
	end
--	self.time = 0
--	self:print( "je passe ds l'init" )

--	local layer = self:get_layer(1)
--	self:print( "layer ref : "..layer )
--	self.ref.material = param.get_ref( layer, "Material_front" )
--	self:print( "ref mat : "..self.ref.material )

	self:add_shading( 1, "HeightMarch" )
end

function meu:set_b_flags()
	local flags = 0
	local lighting = self.lighting
	local b_lighting = {}
	for i=1,8 do
		b_lighting[i] = self[ "b_"..lighting[i] ]
	end
	local bools = self.bools
	local b_bools = {}
	for i=1,6 do
		b_bools[i] = self[ "b_"..bools[i] ]
	end
	local function add_bool( v, b )
		return ( v * 2 + ( b ) )
	end
	local v = add_bool( 0, b_bools[6] )
	for i=13,1,-1 do
		--self:print( "b_bools" )
		if i <= 8 then
			--self:print( "b_lighting["..i.."] : "..b_lighting[i] )
			v = add_bool( v, b_lighting[i] )
		else
			--self:print( "b_bools["..i.."-8] : "..b_bools[i-8] )
			--flags = flags + math.pow(2, i-1) * b_bools[i-8]
			v = add_bool( v, b_bools[i-8] )
		end
		--self:print( "flags : "..v )
	end
	local sha = self:get_shading()
	sha:set_frag_int_3( v )
end

function meu:update()
	local sha = self:get_shading()
	--local t = self.time
	--t = t + aaa.time.dt * math.pow( self.speed*4, 3 )
	--self.time = t
	--sha:set_frag_float_4( t )

	self:set_b_flags()

	sha:set_frag_float_1( self.gamma )
	sha:set_frag_float_2( self.march_step )
	sha:set_frag_float_3( self.epsilon )
	sha:set_frag_float_4( self.height )
	sha:set_frag_float_5( 1. / self.refract_ind )
	sha:set_frag_float_6( self.refract_intensity )
	sha:set_frag_float_7( self.diffuse )
	sha:set_frag_float_8( self.specular )
	sha:set_frag_float( 9, self.step_dist )
	sha:set_frag_float( 10, self.samples )
	sha:set_frag_float( 11, self.flare_term )
	sha:set_frag_float( 12, self.shadow_smooth )
	sha:set_frag_float( 13, self.shadow_min_d )
	sha:set_frag_float( 14, self.shadow_max_d )
	sha:set_frag_float( 15, self.reflect_intensity )
	sha:set_frag_float( 16, self.flare_intensity )

	sha:set_frag_int_2( self.n_iter )
	sha:set_frag_int_4( self.mipmap_level )

	local bu_col = self.ui.bu_col
	for i=1,4 do
		local r,g,b,a = bu_col[i]:get_rgba()
		sha:set_frag_vec4( i, r,g,b,a ) -- problem : all slider change all values ..?!
	end
	local pos = self.pos
	local rot_cam = self.rot_cam
	local rot_plane = self.rot_plane
	--[[local cref = self.camera_ref
	if not cref then
		self.layers = meu:get_layers( )
		self.camera_ref	= aaa.layers.get_camera( self.layers )
		cref = self.camera_ref
	end
	if cref then
        -- Get Translation
		local tx_ref = param.get_ref( cref , "tra_x" )
		local ty_ref = param.get_ref( cref , "tra_y" )
		local tz_ref = param.get_ref( cref , "tra_z" )
		pos.x = param.get( tx_ref )
		pos.y = param.get( ty_ref )
		pos.z = param.get( tz_ref )
        -- Get Rotation
		local rx_ref = param.get_ref( cref , "rot_x" )
		local ry_ref = param.get_ref( cref , "rot_y" )
		local rz_ref = param.get_ref( cref , "rot_z" )
		rot.x = param.get( rx_ref )
		rot.y = param.get( ry_ref )
		rot.z = param.get( rz_ref )
	end--]]
	--table.print( self.pos )

	sha:set_frag_vec4( 6, pos.x, pos.y, pos.z, 0. )
	sha:set_frag_vec4( 7, rot_cam.x, rot_cam.y, rot_cam.z, 0. )
	sha:set_frag_vec4( 8, rot_plane.x, rot_plane.y, rot_plane.z, 0. )
end
