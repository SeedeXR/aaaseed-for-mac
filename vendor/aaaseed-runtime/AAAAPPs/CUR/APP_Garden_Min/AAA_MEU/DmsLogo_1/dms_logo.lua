function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ix = 1
	local iy = 1

	iy = iy + 1
	self:add_bu_texture_target_unit( {ix, iy, nil, 2.5 } )
	iy = iy + 3
	self:add_mapping_by_side( {ix, iy} )
	self:add_rgbfa(	{9,2} )

	self:add_camera()
	self:add_rendering()
	
	local SY = 1


	bu = self:add_slider(	{1,9,		8,1},	"Speed",		self, "speed",				1, -8,8		)
		bu:set_meter( false )
	ix, iy = 9, 4
	bu = self:add_slider(	{ix,iy,		8,SY},	"step",			self, "step",				1, 1,256	)
		bu:set_value_type_integer( true )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,SY},	"Black Hole",	self, "black_hole",			1,	-2,2	)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,SY},	"Pertubation",	self, "noise_intensity",	1,	-2,2		)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		5,SY},	"Scale",		self, "scale",				10,	.1,100		)
	bu = self:add_slider(	{ix+5,iy,	3,SY},	"Harm",			self, "harm",				1,	1,10		)
		:set_min_max_strict( true )
	iy = iy + SY
	bu = self:add_slider_two({ix,iy,	8,SY},	"Threshold",	self, "th_min",	"th_max",	0,1,	-4,4	)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,SY},	"Origin",		self, "origin",				0,	0,100000		)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,SY},	"radius Int",	self, "radius_int",			1,	0,1		)
	iy = iy + SY
	bu = self:add_slider_two({ix,iy,	8,SY},	"Space",		self, "space_min", "space_max",		0,.1, 0,1 	)
	iy = iy + SY
	--todo regroup
	self:add_slider(		{ix,iy,		4,SY},	"Gain",			self, "gain",				.5,	0.00001,.99999 )
	self:add_slider(		{ix+4,iy,	4,SY},	"Bias",			self, "bias",				.5,	0.00001,.99999  )
	iy = iy + SY
	bu = self:add_trig_method({ix,		iy+8}, 	"folder", 		self, "get_folder"			)
end
function meu:get_folder()
	self:print( "Folder ASK" )
	local filter = "Reconnus\0*.bmp;*.gif;*.jpg;*.sgi;*.rgb;*.tga;*.tif;*.tiff;*.png;*.yuv;*.422\0".."Bmp\0*.bmp\0".."Gif\0*.gif\0".."Jpeg\0*.jpg\0".."Tga\0*.tga\0".."Tiff\0*.tif;*.tiff\0".."Png\0*.png\0".."Tous\0*.*\0".."\0"
	local str = aaa.file.do_dialog_open( "Test of pick a folder" )
	self:print( "Folder is \""..str.."\"" )
end
function meu:init()
	self.t = 0
end
function meu:update()
end

function meu:draw_one( x,z, dx,dz )
	aaa.draw_rect( x-dx, z-dz, x+dx, z+dz )
end

local function linearstep( v_begin, v_end, t )	return clamp_01( (t-v_begin) / (v_end-v_begin) ) end


function meu:my_draw()
	local sp = self.speed
	local t = self.t + aaa.time.dt * sp
	self.t = t
	local factor = 4
	local d = 2 / self.step
--	self:print( d )
	local hsx = d * factor * .5
	local hsz = d * factor * .5
	for x = -1,1, d do
		local x2  = x*x
		for z = -1,1, d  do
			local d2 = x2 + z*z
			local d = math.sqrt(d2) * self.black_hole
			local o = aaa.math.get_fractalsum_improved( x,self.origin,z, self.scale, self.harm ) * self.noise_intensity
			d = d + o
			if inside( d, self.th_min, self.th_max - o ) then
				--d = d / self.th_max
				local spa = linearstep( d, self.th_min, self.th_max )
				spa = aaa.math.gain_bias( spa, self.gain, self.bias )	
				spa = self.space_min + spa * (self.space_max - self.space_min)
				spa = 1 - spa
				self:draw_one( x*factor,z*factor, hsx*spa,hsz*spa )
			end
		end
	end
end

function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 )
		self:my_draw()
	self:draw_layers_end()
end
