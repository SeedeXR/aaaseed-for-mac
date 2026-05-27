function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "3D", "Art", "Draw", "Point", "Procedural", "Texture", "VJ", "Depreciated" },
			help = "like MEU DisplaceCV using OpenCl on Particle. Need to be evaluted "
			 }
end

--todo deal with color
--todo what the particle stuff is doing here
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgbf(	{1,14,	nil,1}, "Main_" )
	self:add_camera():set_preset_use( false )

	self:add_bu_texture( {1,3} )
		self:set_bu_texture_preset_use( 1, false )
	self:add_bu_texture( {1,6.5}, "TEX2", 2  )
		self:set_bu_texture_preset_use( 2, false )

	self:add_rendering( {1,11, nil,1.6} )

	local ix = 9
	local iy = 2
	local SX = 8
	local SY = .8
	local Y_SKIP = .2
	self:add_opencl_loop(	{ix,iy,	SX,SY}, self:get_opencl(), 0,
		{
			{	"Min",				0, 1,	},
			{	"Max",				0, 1,	},
			{	"Threshold",		0, 1,	y_skip=Y_SKIP },
			{	"Grad Dist X",		-8, 8,	def = { .5, 1, 2, 3, 4 }		},
			{	"Grad Dist Y",		-8, 8,	def = { .5, 1, 2, 3, 4 }		},
			{	"src_cross_blur",	0, 1,	y_skip=Y_SKIP+SY },
			{	"Displace",			-4, 4,	def = { 0., .5, 1., -1., -.5 }	},
			{	"Push",				-1, 1,	},
			{	"Push_Luma_Factor",	-4, 4,	y_skip=.1 },
			{	"Normalize",		0, 1,	},
			{	"Kinect",			0, 2,	},
		}
	)

	iy = 3 
--	self.ui.bu_max	= self:add_param(	{ix,iy+8*SY,		SX,SY},	"Max"	)
	self.ui.bu_blur	= self:add_param(	{ix,iy+8*SY+Y_SKIP,		SX,SY},	"Blur", nil, 0, 128 )
end

function meu:get_preset_nb() 	return 24	end

function meu:init()
--	aaa.debug.print_traceback( "meu:init()" )
--	self:box_debug( "meu:init()" )

	self:init_opencl()

	local ref = self.ref
	ref.bdd_multi	=	aaa.obj.get_down_by_class( self:get_layer(2), "bdd_img_opencv" )
		ref.smooth_bind	=	param.get_ref( ref.bdd_multi, "image_src" )
		ref.smooth_dst	=	param.get_ref( ref.bdd_multi, "image_dst_base" )
		local pre = "blur"	--	"smooth"
		ref.smooth_x	=	param.get_ref( ref.bdd_multi, pre.."_size_x" )
		ref.smooth_y	=	param.get_ref( ref.bdd_multi, pre.."_size_y" )
	
end

--todo optimize on change only
function meu:update()
	local	ps	= param.set
	local	ref	= self.ref

	local bind

	bind = self:get_texture_bind_2d( 1 )
	self:set_opencl_bind_2d( 1, bind )
	ps( ref.smooth_bind, bind )

	bind = self:get_texture_bind_2d( 2 )
	self:set_opencl_bind_2d( 2, bind )
	ps( ref.smooth_dst, bind )

	local v = self.ui.bu_blur:get_value()
	ps( ref.smooth_x, v )
	ps( ref.smooth_y, v )
	--self:print( "--------  "..ref.smooth_y )					)
end

function meu:draw()
	MEU.draw(self)
end