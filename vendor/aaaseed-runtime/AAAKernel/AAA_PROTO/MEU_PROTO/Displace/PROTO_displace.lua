--bigue change resolution : nb , NB grid in UI. Copy Particle ?

function meu:define_meu_infos( )
	return { author = "Mâa", date="2024",
			tags = { "3D", "Art", "Draw", "Point", "Procedural", "Texture", "VJ" },
			help = "Displace in z in function of Luminosity"
			}
end

--todo deal with color
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	local ix,iy = 1,1
	local SX = 8
	local SY = .8
	local DY = .2	
	local SYM = 4
	
	self:add_rendering()
	self:add_camera( nil, 16 ):set_preset_use( false )

	self:add_shading_ui(				{ix,iy,		nil,SY}	)

	iy = iy + DY
	bu = self:add_selector(				{ix+4,iy,	4,SY},	"Axe"	)
		bu:set_item_text( 1, "X", "Y", "Z" )
		bu:set_target_lua( self, "axe" )
	iy = iy + SY

	local value_def_nb = {256,512,768}
	local value_def_size = {2,4,6}
	bu = self:add_slider(				{ix,iy,		4,SY},	"Size U",			ref.model, "size_u", 			1,	0,8	)
		bu:set_color_back("u")
		bu:add_values_def( value_def_size )
	bu = self:add_slider(				{ix+4,iy,	4,SY},	"Nb U",				ref.bdd_grid, "nb_u", 			64,	1,1024	)
		bu:set_color_back("u"):set_value_type_integer(true)
		bu:add_values_def( value_def_nb )
	iy = iy + SY
	bu = self:add_slider(				{ix,iy,		4,SY},	"Size V",			ref.model, "size_v", 				2,	0,8	)
		bu:set_color_back("v")
		bu:add_values_def( value_def_size )
	bu = self:add_slider(				{ix+4,iy,	4,SY},	"Nb V",				ref.bdd_grid, "nb_v", 			64,	1,1024	)
		bu:set_color_back("v"):set_value_type_integer(true)
		bu:add_values_def( value_def_nb )
	iy = iy + SY + DY


	self:add_bu_texture(				{ix,iy,		4,SYM},	"Height",	2 )
	self:set_bu_texture_preset_use( 2, false )
	self:add_bu_texture_target_layer(	{ix+4,iy,	4,SYM},	"Tex",		1, false,		self:get_layer(1) )
	self:set_bu_texture_preset_use( 1, false )

	iy = iy + SYM
	self:add_button( 					{ix+5,iy,			2,SY },	"Tex Use",	self, "b_tex_use",	true )
	self:add_slider(					{ix+5,iy+SYM-SY,	3,SY},	"Blur",		self, "blur",		1,	0,128 )

	self:add_bu_texture(				{ix+1,iy,	4,SYM},	"Blur Dst",	3 )
	self:set_bu_texture_preset_use( 3, false )
	
	bu = self:add_selector(				{ix,iy+.4,	1,4-.4},"Resize"		)
		:set_nb(1,6)
		:set_item_text( 1, "No", "2", "4", "8", "16", "32" )
		:set_item_data( 1, 1, 1/2, 1/4, 1/8, 1/16, 1/32 )
		ui.bu_resize = bu
	iy = iy + 4 + DY

	bu = self:add_selector(				{ix,iy,		8,SY},	"Out" )
		:set_item_text( 2, "Nor", "Nor01", "Nor EC", "Nor EC 01" )
		:set_min_max( 0 )
		:set_target_lua( self, "out_mode" )

	ix = 9
	iy = 2.9

	self:add_transfo( {9,iy, 8,2.4}, 1 )
	iy = iy + 2.4 + DY

	local function add_slider_check( name, val, min,max, b_active, b_w )
		local low = name:lower()
		self:add_button( 				{ix,iy, 	SY,SY},		name.." Active",	self, "b_"..low,	b_active	):set_text_visible(false)
		bu = self:add_slider( 			{ix+SY,iy,	SX-SY,SY},	name,				self, low,			val,	min,max )
		if b_w then bu:set_color_back("w") end
		iy = iy + SY
		return bu
	end


--	self:add_slider( 					{ix,iy, 	SX,SY},		"src_cross_blur",	gep(5), nil,			1, 0,1 )
--	iy = iy + SY + DY
	self:add_button( 					{ix,iy, 	SY,SY},		"Range Active",		self, "b_range",		false	):set_text_visible(false)
	self:add_slider_two(				{ix+SY,iy, 	SX-SY,SY},	"Range in", 		self, "range_min", "range_max", 	0,1, 0,1 ):set_color_back("w")
	iy = iy + SY
	add_slider_check( 	"Gamma",		1, 0.001,8,	false,	true )
	add_slider_check( 	"Cut",			1, 0,1,		false,	true )
	add_slider_check( 	"Threshold",	1, 0,1,		false,	true )
	add_slider_check( 	"Displace",		1, -4,4,	true,	true )
	iy = iy + DY/2

	self:add_slider(					{ix,iy, 	SX,SY},		"Pixel Dist", 	self, "pixel_dist",		1, 0,8	)
	iy = iy + 
	SY
	add_slider_check( 	"Push",			0, -1,1,	false,	false	):set_color_back("factor")
	-- separate factors by axis for Push
	self.t_push = {}
	self:add_sliders_uvw_min_max(		{ix+SY,iy,	8-SY,SY},	"Push", self.t_push, true, 1, -4,4,	{-2,0,2} )
	iy = iy + SY
	add_slider_check( 	"Normal",		1, -4,4,	false,	false	)
	iy = iy + DY/2

	ui.bu_color = self:add_rgbf(		{ix,iy,		nil,SY}, "Main_" )
	iy = iy + SY + DY/2
	self:add_blending( 					{ix,iy,		nil,SY} )
	--todo add this
	-- self:add_slider( 				{ix,iy,		SX,SY}, "Push_Luma_Factor",	gep(9), nil,			1, -1,1 )
	-- iy = iy + SY
end

function meu:get_preset_nb() 	return 24	end

function meu:init()
	local ref = self.ref

	-- shadeing is in layer 3 this create A SHADING Obj that you can get using self:get_shading() from here
	self:add_shading(3)

	local layer			=	self:get_layer(1)
	local pgr			=	param.get_ref
	ref.model		=	aaa.obj.get_branch_by_class( layer, "model" )
		ref.axe 		=	pgr( ref.model, "axe_object" ) 
	ref.bdd_blur	=	self:get_layer_bdd(2)
		ref.smooth_bind	=	pgr( ref.bdd_blur, "image_src" )
		ref.smooth_dst	=	pgr( ref.bdd_blur, "image_dst_base" )
		ref.smooth_x	=	pgr( ref.bdd_blur, "blur_size_x" )
		ref.smooth_y	=	pgr( ref.bdd_blur, "blur_size_y" )
		param.set_save( ref.smooth_x, false )
		param.set_save( ref.smooth_y, false )
		ref.cuda		=	pgr( ref.bdd_blur, "use_cuda"		)
		ref.resize_fx	=	pgr( ref.bdd_blur, "resize_fx"	)
		ref.resize_fy	=	pgr( ref.bdd_blur, "resize_fy"	)

	ref.blur_active = pgr( self:get_layer(2), "active" )

	ref.bdd_grid	=	self:get_layer_bdd(4)

	-- we use it for first texture (unit 0)
	ref.map = aaa.layer.build_bank_bind_2d_ref_table( layer, 0,0 )
end


function meu:update()
	local ps	= param.set
	local ref	= self.ref
	local ui	= self.ui
	local sha	= self:get_shading() 

	-- set axe in the model obj
	param.set( ref.axe, self.axe-1 )
	sha:set_vert_int_1( self.axe-1 )

	sha:set_frag_int_1( self.out_mode )
	self.bind_deform_src = self:get_texture_bind_2d( self.b_tex_use and 1 or 2 )
	
	ps( ref.smooth_bind, self.bind_deform_src  )
--	self:print( bind )

	local v = self.blur
	self.b_blur = v > 0
	if self.b_blur then
		ps( ref.smooth_x, v )
		ps( ref.smooth_y, v )
		local bind = self:get_texture_bind_2d( 3 )
		ps( ref.smooth_dst, bind )
		ps( ref.cuda, v < 32 )
		
		local resize = ui.bu_resize:get_item_data()
		ps( ref.resize_fx, resize )
		ps( ref.resize_fy, resize )
		self.bind_deform_dst = bind
	else
		self.bind_deform_dst = self.bind_deform_src
	end
	--ps( ref.blur_active, b_blur )		

	
end

function meu:draw()
	local sha = self:get_shading() 

	self:draw_layers_begin()
		-- set attributes (model, rendering, ....), don't draw anything 
		self:draw_layer(1)
		-- just do the blur
		if self.b_blur then
			self:draw_layer(2)
		end
		-- set second texture and uniforms for shader
		gol.set_tex_unit_2d_bind( 1, self.bind_deform_dst )
		local min,max
		if self.b_range	then 	min,max = self.range_min, self.range_max
		else					min,max = 0,1
		end
		sha:set_vert_float_1_7( self.pixel_dist,	min, max,
								self.b_gamma and self.gamma or 1,
								self.b_cut and self.cut or 0,
								self.b_threshold and self.threshold or 0,
								self.b_displace and self.displace or 0
							)
		-- no more room in float so we use vec4		sha:set_vert_vec4( 3,	self.b_normal and self.normal or 1	)
		local push = self.b_push and self.push or 0
		local t = self.t_push
		local px,py,pz = t.u * push, t.v * push, t.w * push
		if		self.axe == 2 then	px,py,pz = py,pz,px		-- Y
		elseif	self.axe == 1 then	px,py,pz = -pz,py,px	-- X
		end
		sha:set_vert_vec4( 2,	px,py,pz,
								self.b_normal and self.normal or 1	)
		local sx,sy = aaa.img.get_size( self.bind_deform_dst )
		if sx then
			sha:set_vert_vec4( 1, 1/sx, 1/sy, 0 )
		end
	--	gol.set_tex_unit_2d_bind( 0, self.bind_map )
		self:draw_layer(3)
		-- draw the grid with current attributes and shader set
		self:draw_layer(4)
	self:draw_layers_end()
end
