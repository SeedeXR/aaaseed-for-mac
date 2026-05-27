--todo deal with color
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgbf(	{1,12,	nil,nil}, "Main_" )

	self:add_camera():set_preset_use( false )

	self:add_bu_texture()
	self:add_bu_texture( {1,5}, "TEX2", 2 )
	self:add_rendering()

	local ix = 9
	local SX = 8
	local SY = .8
	local opencl = self:get_opencl()
	self:add_opencl_loop(	{ix,nil,	SX,SY}, opencl, 0,
		{
			{	"Dif",				0,	8,	},
			{	"Dif_A",			0,	1,	},
			{	"dif_B",			0,	1,	},
			{	"React",			0,  4,	},
			{	"Flow",				0,	8,	},
			{	"Flow_A",			0,	1,	},
			{	"Flow_B",			0,	1,	},
--			{	nil,	0, 1, },
--			{	"Push",				-4, 4,	},
			{	"Influence",		0, 120,	},
			{	"FEED",		0, 1,	},
			{	"pull",		0, 1,	},
--			{	"Normalize",		0,	1,	},
--			{	"Kinect",			0,	2,	},
		}
	)

--	local iy = 3
--	self.ui.bu_max	= self:add_param(	{ix,iy+8*SY,		SX,SY},	"Max"	)
--	self.ui.bu_blur	= self:add_param(	{ix,iy+13*SY,		SX,SY},	"Blur", nil, 0, 128 )
end

function meu:init()
	self:init_opencl()
end

--todo optimize on change only
function meu:update()
	local	ps	= param.set
	local	ref	= self.ref

	local bind

	bind = self:get_texture_bind_2d( 1 )
	self:set_opencl_bind_2d( 1, bind )

	bind = self:get_texture_bind_2d( 2 )
	self:set_opencl_bind_2d( 2, bind )
end
