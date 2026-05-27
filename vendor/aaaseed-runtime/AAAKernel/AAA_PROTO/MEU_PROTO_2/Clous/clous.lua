--todo rename MEU
function meu:define_meu_infos( )
	return { 	author = "Mâa",
				version = 0,
				tags = { "Proprietary", "art", "procedural", "2d","3d", "Draw", "VJ", "texture", "Unfinished" },
				help = "effects displacing big pixels from an Image input: restore or dump",
			}
end
function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ix = 1
	local iy = 1

	self:add_bu_texture( {ix, iy, nil, 2.5 }, "Clous", 1 )
	iy = iy + 2.5
	self:add_bu_texture( {ix, iy, nil, 2.5 }, "Color_Front", 2 )
	iy = iy + 2.5
	self:add_bu_texture( {ix, iy, nil, 2.5 }, "Color_Back", 3 )
	iy = iy + 2.7
	self:add_mapping_by_side( {ix, iy} )
	self:add_rgbfa(	{9,2} )

	self:add_camera()


	self:add_rendering(		{1,12.2} )

	bu = self:add_slider(	{1,14.5,	8,1},	"Speed",			self, "speed",		1,		-8,8 )
		bu:set_meter( false )
	ix, iy = 9, 5
	bu = self:add_button(	{ix,iy-1	},		"spiral active",	self, "b_spiral_active", true )
	bu = self:add_slider(	{ix,iy,		8,1},	"step",				self, "step",		1,		1,256 )
		bu:set_value_type_integer( true )
	bu = self:add_slider(	{ix,iy+1,	8,1},	"size_u",			self, "size_u",		1,		0,1 )
	bu = self:add_slider(	{ix,iy+2,	8,1},	"size_v",			self, "size_v",		1,		0,1 )
	bu = self:add_slider(	{ix,iy+3,	8,1},	"size_axe", 		self, "size_axe",	1,		0,8 )
	bu = self:add_slider(	{ix,iy+4,	8,1},	"freq",				self, "freq",		1,		0,1 )
	bu = self:add_slider(	{ix,iy+5,	8,1},	"bias",				self, "bias",		1,		0,1 )
	bu = self:add_slider(	{ix,iy+6,	8,1},	"spiral",			self, "spiral",		1,		0,16 )
		bu:set_value_type_integer( true )
	bu = self:add_slider(	{ix,iy+7,	8,1},	"threshold",		self, "threshold",	.001,	.001,1 )

	bu = self:add_trig_method(	{ix,	iy+8}, 	"folder", 			self, "get_folder" )
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
	self.bind_k		= self:get_texture_bind_2d( 1 )
	self.bind_kv	= self:get_texture_bind_2d( 2 )
	self.bind_v		= self:get_texture_bind_2d( 3 )
end

function meu:my_draw()
	local sha = self:get_shading() or self:add_shading()
	-- if not sha then
	-- 	self:print_error( "No shading" )
	-- 	return
	-- end
	local sp = self.speed
	local t = self.t + aaa.time.dt * sp
	self.t = t
	local size = 8
	local d = size / self.step
--	self:print( d )
	local fx = d * self.size_u
	local fz = d * self.size_v
	local fu = 1/self.step * self.size_u
	local fv = 1/self.step * self.size_v

	local bind_k = self.bind_k
	local sxk,syk = aaa.img.get_size(		self.bind_k )
	local bind_kv = self.bind_kv
	local sxkv,sykv = aaa.img.get_size(		self.bind_kv )
	local bind_v = self.bind_v
	local sxv,syv = aaa.img.get_size(		self.bind_v )

--	if not sxk or not sxv then return end
	if not sxk then self:print( "no size for bind kinect, check if tex is on cpu" ) return end
	if not sxkv then self:print( "no size for bind kinect video, check if tex is on cpu" ) return end
	if not sxv then self:print( "no size for bind image, check if tex is on cpu" ) return end

	for x = -size*.5, size*.5, d do
		for z = -size*.5, size*.5, d do
			if self.b_spiral_active then
				local angle = math.atan2( x, z ) * self.spiral * .5
				local di = math.sqrt(x*x +z*z) * self.freq * 8 + angle
				local sy = ( math.sin( di * 2. + t ) + 1 ) * .5
				local sx = ( math.sin( di * 2. + t*3 ) + 1 ) * .5
				sx = aaa.math.bias( sx, self.bias )
				local 			sz = sx
				aaa.draw_box( sx * fx, sy * self.size_axe, sz * fz,		x, 0, z )
			else

--				self:draw_layer( 2 )
---[[
				local r,g,b,a = aaa.img.get_color_xy( bind_k, (x/(size)+.5)*(sxk-1), (-z/(size)+.5)*(syk-1) )
				if a<=.01 then
					r,g,b,a = aaa.img.get_color_xy( bind_v, (x/(size)+.5)*sxv, (-z/(size)+.5)*syv )
					--r,g,b,a = 1, .1, .2 , 1.
				end
				a = 1
				if r then
					local h = self.size_axe * (r+g+b)*.33 + .08
					if h < self.threshold then
						local f = h/self.threshold
						f = .6 + f * .4
						--gol.color( (r+g+b) * .333)
--local size2 = size * .5
--						r,g,b = aaa.img.get_color_xy( 42, (x/(size2)+.5)*(sxk-1), (-z/(size2)+.5)*(syk-1) )
						--r,g,b,a = aaa.img.get_color_xy( bind_v, (x/(size)+.5)*sxv, (-z/(size)+.5)*syv )
						--gol.color( r,g,b, a )
--						self:draw_layer( 2 )
						sha:set_frag_int_1( 1 )
						gol.update_uniform_fragment()
						aaa.draw_box( fx*f, 0, fz*f,	x, 0, z )
					else
						--r,g,b = aaa.img.get_color_xy( bind_kv, (x/(size)+.5)*(sxkv-1), (-z/(size)+.5)*(sykv-1) )
						--r,g,b = aaa.img.get_color_xy( 65, (x/(size)+.5)*(sxk-1), (-z/(size)+.5)*(syk-1) )
						--gol.color( r,g,b, a )
						--sha:set_frag_vec4( 1, (x+size*.5)/size, (-z+size*.5)/size, fu, fv ) -- uv pos, uv size
						sha:set_frag_int_1( 0 )
						gol.update_uniform_fragment()
						--gol.set_tex_unit_2d_with_bind( 2, bind_kv )
						aaa.draw_box( fx, h, fz,	x, h*.5, z )
					end
				end
				---]]
			end
		end
	end
end

function meu:draw()
	--
	--local r,g,b,a = 1, .1, .2 , 1.
	--aaa.draw_box( 1, 1, 1, 0, 0, 0 )
	--gol.translate( 0, 0, 0 )
	--self:draw_layer( 1 )
	--	self:my_draw()
	--
	self:draw_layers_begin()
		gol.rotate_x( .25 )
		self:draw_layer( 1 )
		self:my_draw()
		--self:draw_layer( 2 )
	self:draw_layers_end()
--]]--
--[[
	local nb = 3022
	local tab = aaa.array.new_fp32( nb )
	self:print( #tab.." "..type(tab.clear).." "..tab.clear )
--	local tab = aaa.exp.array.new_fp32( nb )
---	self:print( type(tab.ziob).." "..tab.ziob )
	tab:clear()
	tab:set( 1, -1, 2 )
	tab:set( 8, 3020, 100000 )
	tab:resize( 8 )
	self:print( #tab )
	tab:resize( 8000 )
	self:print( #tab )
	tab:set( 8, 4 )
--	tab[3] = 3.4
	tab[#tab-3] = -3.4
	self:print( tab[1].." "..tab[2].." "..tab[3].." "..tab[4] )
	local i = #tab-4
	self:print( tab[i+1].." "..tab[i+2].." "..tab[i+3].." "..tab[i+4] )
	--tab:clear()
	--self:print( tab[1], tab[2], tab[3], tab[4], tab[5], tab[6], tab[7], tab[8] )
	tab:free()
	self:print( #tab )
--	tab[-4] = -3.4
--]]

end
