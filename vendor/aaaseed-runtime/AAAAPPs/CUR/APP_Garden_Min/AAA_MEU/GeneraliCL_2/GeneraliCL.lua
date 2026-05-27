	--todo deal with color
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	ui.bu_color = self:add_rgbfa(	{1,12 } )

	self:add_camera():set_preset_use( false )

	self:add_bu_texture()
		self:set_bu_texture_preset_use( 1, false )
	self:add_bu_texture( {1}, "TEX2", 2 )
		self:set_bu_texture_preset_use( 2, false )

	self:add_rendering()

	local ix = 9
	local SX = 8
	local SY = .7
	if true then
		local opencl = self:get_opencl()
		self:add_opencl_loop(	{ix,nil,	SX,SY}, opencl, 0,
			{
				{	"Grad Dist X",		-8,	8,	},
				{	"Grad Dist Y",		-8,	8,	},
				{	nil,				0,	1,	},
				{	"Grad Factor",		-1, 1,	},
				{	"Origin",			0,	1,	},
				{	"Speed X",			-1,	1,	},
				{	"Speed Y",			-1,	1,	},
				{	"Damp",				0,	1,	},
--				{	nil,				0,	1,	},
--				{	"Push",				-4,	4,	},
--				{	"Push_Luma_Factor",	-4, 4,	},
--				{	"Normalize",		0,	1,	},
--				{	"Kinect",			0,	2	},
			}
		)
	end

	local iy = 3
	self.ui.bu_max	= self:add_param(	{ix,iy+12*SY,		SX,SY},	"Max"	)
	self.ui.bu_blur	= self:add_param(	{ix,iy+4*SY,		SX,SY},	"Blur", nil, 0, 128 )
end

function meu:get_preset_nb()	return 16	end

function meu:init()

	self:init_opencl()

--todonow this should be regrouped also
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
	local	ui	= self.ui

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
	--self:print( "--------  "..ref.smooth_y )

	local opencl = self:get_opencl()
	--self:print( "--------  "..opencl:get_param(6) )
	--opencl:set_param(	7,	1. / ( self.ui.bu_max:get_value() - opencl:get_param(6) )	)
	--self:print( opencl:get_param(6) )
	opencl:set_param(	9,	self.ui.bu_max:get_value() )
	--self:print( opencl:get_param(6) )

	local r,g,b,a = ui.bu_color:get_rgba()
	opencl:set_vec_xyzw( 1, r,g,b,a )

end