--bigue change resolution : nb , NB grid in UI. Copy Particle ?

function meu:define_meu_infos()
	return
	{	author = "Mâa",
		tags = { "3D", "Art", "Draw", "Point", "Procedural", "Texture", "VJ", "Depreciated" },
		help = "Older version of MEU Displace but less complete and using OpenCl "
	}
end

--todo deal with color
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local pa

	local ix,iy = 1,3
	local SX = 8
	local SY = 1
	local DY = .2	
	local SYM = 4
	
	self:add_rendering()
	self:add_camera( nil, 16 ):set_preset_use( false )

--	self:set_tab_key( "Main" )
	self:add_bu_texture( {ix,iy,		4,SYM},		"Tex",		1 )
	self:set_bu_texture_preset_use( 1, false )

	iy = iy + SYM
	self:add_button(	{ix+4.5,iy-SY,	SY,SY },	"Sync", self, "b_sync", true )

	self:add_bu_texture( {ix,iy,		3.5,SYM},	"Height",	2 )
	self:set_bu_texture_preset_use( 2, false )
	self:add_bu_texture( {ix+4.5,iy,	3.5,SYM},	"Blur Dst",	3 )
	self:set_bu_texture_preset_use( 3, false )
	
	bu = self:add_selector(	{ix+3.5,iy,	1,4},		"Resize"	)
		:set_nb(1,6)
		:set_item_text( 1, "No", "2", "4", "8", "16", "32" )
		:set_item_data( 1, 1, 1/2, 1/4, 1/8, 1/16, 1/32 )
		ui.bu_resize = bu

	iy = iy + 4 + DY
	ui.bu_color = self:add_rgbf(	{1,iy,	nil,1}, "Main_" )


	ix,	iy = 9, 2.5

	local opencl = self:get_opencl()
	local function gep(i) return opencl:get_param_ref(i) end  
	self:add_opencl_base( ix, iy, SY, true, opencl )
	iy = iy + SY*3 + DY
	self:add_slider(	{ix,iy,	SX,SY},	"Blur", 			self, "blur",			1,	0,128 )
	iy = iy + SY
	self:add_slider(	{ix,iy, SX,SY},	"Gradient Dist", 	self, "grad_dist",		1, 0,8	)
	iy = iy + SY
	self:add_slider( 	{ix,iy, SX,SY}, "src_cross_blur",	gep(5), nil,			1, 0,1 )
	iy = iy + SY + DY

	self:add_slider_two({ix,iy, SX,SY},	"Range in", 		nil, gep(6), gep(7),	0,1, 0,1 )
	iy = iy + SY
	self:add_slider( 	{ix,iy, SX,SY}, "Threshold",		gep(3), nil,			1, 0,1 )
	iy = iy + SY + DY

	self:add_slider( 	{ix,iy, SX,SY}, "Displace",			gep(4), nil,			1, -4,4 )
	iy = iy + SY
	self:add_slider( 	{ix,iy, SX,SY}, "Push",				gep(8), nil,			1, -1,1 )
	iy = iy + SY
	self:add_slider( 	{ix,iy, SX,SY}, "Push_Luma_Factor",	gep(9), nil,			1, -1,1 )
	iy = iy + SY
	-- self:add_opencl_loop(	{ix,iy,	SX,SY}, opencl, 0,
	-- 	{
	-- 		{	"Grad Dist X",		-8,	8,	},
	-- 		{	"Grad Dist Y",		-8,	8,	y_skip=SY	},
	-- 		{	"Threshold",		0,	1,	},
	-- 		{	"Displace",			-4,	4,	},
	-- 		{	"src_cross_blur",	0,	1,	},
	-- 		{	"Min",				0,	1,	},
	-- 		{	nil,				0,	1,	},
	-- 		{	"Push",				-4, 4,	},
	-- 		{	"Push_Luma_Factor",	-4, 4,	},
	-- 		{	nil,				0,	1,	},	--{	"Normalize",		0,	1,	},
	-- 		{	"Kinect",			0,	2	},
	-- 	}
	-- )
	self:add_transfo( {5,2.4, 8,3.2}, 1 )
end

function meu:get_preset_nb() 	return 24	end
function meu:init()

	self:init_opencl()

	local ref = self.ref
	ref.bdd_blur	=	self:get_layer_bdd(2)
		ref.smooth_bind	=	param.get_ref( ref.bdd_blur, "image_src" )
		ref.smooth_dst	=	param.get_ref( ref.bdd_blur, "image_dst_base" )
		ref.smooth_x	=	param.get_ref( ref.bdd_blur, "blur_size_x" )
		ref.smooth_y	=	param.get_ref( ref.bdd_blur, "blur_size_y" )
		param.set_save( ref.smooth_x, false )
		param.set_save( ref.smooth_y, false )
		ref.cuda		=	param.get_ref( ref.bdd_blur, "use_cuda"		)
		ref.resize_fx	=	param.get_ref( ref.bdd_blur, "resize_fx"	)
		ref.resize_fy	=	param.get_ref( ref.bdd_blur, "resize_fy"	)

	ref.blur_active = param.get_ref( self:get_layer(2), "active" )
	self:ocl_init()
	--hack to avoid shader stuff: BU...
	self.ref.shading_stuff = nil
end

--todo optimize on change only
function meu:update()
	local	ps	= param.set
	local	ref	= self.ref
	local	ui	= self.ui

	local bind

	bind = self:get_texture_bind_2d( 1 )
	self:set_opencl_bind_2d( 1, bind )

	if self.b_sync then
	else
		bind = self:get_texture_bind_2d( 2 )
	end
	ps( ref.smooth_bind, bind )
--	self:print( bind )

	local v = self.blur
	local b_blur = v > 0  
	if b_blur then
		ps( ref.smooth_x, v )
		ps( ref.smooth_y, v )
		bind = self:get_texture_bind_2d( 3 )
		ps( ref.smooth_dst, bind )
		ps( ref.cuda, v < 32 )
		
		local resize = ui.bu_resize:get_item_data()
		ps( ref.resize_fx, resize )
		ps( ref.resize_fy, resize )
	end
	ps( ref.blur_active, b_blur )		
	self:set_opencl_bind_2d( 2, bind )

	--self:print( "--------  "..ref.smooth_y )

	local ocl = self:get_opencl()
	--self:print( "--------  "..ocl:get_param(6) )
	local v = self.grad_dist
	ocl:set_param(	1,	v	)
	ocl:set_param(	2,	v	)

--	ocl:set_param(	7,	1. / ( self.ui.bu_max:get_value() - ocl:get_param(6) )	)
--	ocl:set_param(	7,	self.ui.bu_max:get_value()	)


	local r,g,b,a = ui.bu_color:get_rgba()
	ocl:set_vec_xyzw( 1, r,g,b, self:get_alpha() )
	--ocl:set_param(	7,	self.ui.bu_max:get_value()							)
end
