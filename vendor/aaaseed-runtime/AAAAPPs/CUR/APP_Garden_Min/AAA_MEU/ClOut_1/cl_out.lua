--todo rename
function meu:define_meu_infos( )
	return { 	author = "Mâa",
				version = 0,
				tags = { "Proprietary", "art", "procedural", "2d", "Draw", "texture", "Unfinished" },
				help = "Used for Jeddah, use OpenCl: restore or dump",
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgbf(	{1,13.6,	nil,nil}, "Main_" )

	self:add_camera():set_preset_use( false )

	local ix = 1
	local SX = 8
	local SY = .8
	local opencl = self:get_opencl()
	self:add_opencl_loop(	{1,2,			SX,SY}, opencl, 0,
		{
			{	"p01",	.125,	1.,	},
			{	"p02",	.125,	1,	},
			{	"p03",	0,		1,	},
			{	"p04",	-4,		4,	},
			{	"p05",	0,		1,	},
			{	"p06",	0,		1,	},
			{	"p07",	0,		1,	},
			{	"p08",	-4,		4,	},
			{	"p09",	-4,		4,	},
			{	"p10",	0,		1,	},
		}
	)
	self:add_opencl_loop(	{9,2,			SX,SY}, opencl, 10,
		{
			{	"c1x",	0,	1,	},
			{	"c1y",	0,	1,	},
			{	"c1z",	0,	1,	},
			{	"c1w",	0,	1,	},
		}
	)
	self:add_opencl_loop(	{9,2+SY*4+.2,	SX,SY}, opencl, 14,
		{
			{	"c2x",	0,	1,	},
			{	"c2y",	0,	1,	},
			{	"c2z",	0,	1,	},
			{	"c2w",	0,	1,	},
		}
	)
--	local iy = 3
--	self.ui.bu_max	= self:add_param(	{ix,iy+8*SY,		SX,SY},	"Max"	)
--	self.ui.bu_blur	= self:add_param(	{ix,iy+13*SY,		SX,SY},	"Blur", nil, 0, 128 )
end


function meu:init()
	self:init_opencl()
end
