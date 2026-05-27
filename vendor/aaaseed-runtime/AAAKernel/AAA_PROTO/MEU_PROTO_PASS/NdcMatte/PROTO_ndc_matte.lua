function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "2d", "3d", "texture", "renderpass", "unfinished" },
			help = 	{
					"Draw mattes (Matte Painting), in fact draw 4 Back and Front textures on top of each others",
					"with control over size and position, can be done with 8 PIPs as well",
					"Done for Monaco Aquarium but unused, there is 2 version Matte and NdcMatte",
					"Need to be documented",
					"Output to the current Fbo"
					}
		}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	self:add_camera()

	local SX13 = 8 / 3
	self:add_slider(	{ix,iy,			SX13,SY},	"Offset x",	self.offset_pos, "x",	0, 0, 32):set_color_back( "x" )
	self:add_slider(	{ix+SX13,iy,	SX13,SY},	"Offset y",	self.offset_pos, "y",	0, 0, 32):set_color_back( "y" )
	self:add_slider(	{ix+SX13*2,iy,	SX13,SY},	"Offset z",	self.offset_pos, "z",	0, 0, 32):set_color_back( "z" )
	self:add_slider(	{ix+SX13*2,iy+SY,SX13,SY},	"Z Cam",	self, "z_cam",	1.0, 0.001, 10):set_color_back( "z" )
	self:add_slider(	{ix,iy+SY,		SX13,SY},	"AR",		self, "ar",	0.916, 0.1, 5.0):set_color_back( "x" )

	iy = 2
	self:set_tab_key( "BacK" )
	self.matte = {}
	local names = { "Back", "Back_1", "Back_2", "Back_3", "Front_3", "Front_2", "Front_1", "Front" }
	local SYB = 2.1
	for i=1,8 do
		if i==5 then
			self:set_tab_key( "Front" )
		end
		ix = 9
		local y = iy + (i-((i<5) and 1 or 5))*(SYB+SY+DY)

		local tab = { index = i }
		local name = names[i]
		bu = self:add_button(	{ix, y,	SY, SY },	name, 	tab, "b_draw", false )
		self.matte[i] = tab
		local ref = {}
		local lay = { ref=ref }
		self.layers[i] = lay

		self:add_bu_texture(	{ix,y+SY, nil,SYB}, "TEX_"..name, i )

		ix = 1
		local y2 = y + 1

		local pos = { x=0, y=0, z=0}

		local sy = .9
		local sx = 8
		bu = self:add_slider(	{ix,y2 + sy*0,		sx/2,sy},		name.."_Tra-X",	lay, "x",	0,	-10, 10	)
			bu:set_color_back( "x")
			bu:set_text("Tra X")
		bu = self:add_slider(	{ix+sx/2,y2 + sy*0,	sx/2,sy},		name.."_Tra_Y",	lay, "y",	0,	-10, 10	)
			bu:set_color_back( "y" )
			bu:set_text("Tra Y")
		bu = self:add_slider(	{ix,y2 + sy*1,		sx,sy},			name.."_Depth",	lay, "z",	0,	0, 100	)
			bu:set_color_back( "z" )
			bu:set_text("Depth")
		bu = self:add_slider(	{ix,y2 + sy*2,		sx/2,sy},		name.."_Scale",	lay, "scale", 1, 0, 32 )
			bu:set_text("Scale")
		bu = self:add_slider(	{ix+sx/2,y2 + sy*2,	sx/2,sy},		name.."_Scale_y",	lay, "scale_y", 1, 0, 4 ):set_color_back( "y" ):set_text( "Scale Y" )
			bu:set_text("Scale Y")

	end

end

function meu:init()
	local ref = self.ref
	self:add_shading()
	self.offset_pos = { x=0, y=0, z=0 }
	self.fronts_pos = { x=0, y=0, z=0 }
	self.backs_pos = { x=0, y=0, z=0 }
	self.layers = {}
end

function meu:draw()
 	self:draw_layers_begin()
 		self:draw_layer( 1 )
		for i=1,8 do
			local matte = self.matte[i]
			if matte.b_draw then
				local layer = self.layers[i]
				local bind = self:get_texture_bind_2d(i)
				local imgx, imgy = aaa.img.get_size( bind )
				-- get bind AR, somehow self:get_texture_ratio_y( bind ) doesn't work
				if imgx then
					local ar_bind = imgx / imgy
					--self:print( "i "..i..", ar = "..ar_bind )
					local z = self.offset_pos.z + layer.z
					local scale_z = self.z_cam / ( self.z_cam + z )
					local scale_x = layer.scale
					local scale_y = layer.scale * layer.scale_y
					-- take into account AR of binded texture and AR of output
					if ar_bind > self.ar then
						scale_y = scale_y / ar_bind * self.ar
					else
						scale_x = scale_x * ar_bind / self.ar
					end
					-- align on bottom vertically
					local deltaY = -z / self.z_cam * ( -1.0 + layer.y  )

					local xmin = ( layer.x + self.offset_pos.x - scale_x ) * scale_z
					local xmax = xmin + 2.0 * scale_x * scale_z

					-- align on bottom vertically
					local ymin = ( -1.0 + layer.y + self.offset_pos.y - deltaY ) * scale_z
					local ymax = ymin + 2.0 * scale_y * scale_z

					aaa.draw_bind_rect( bind, xmin, ymin, xmax, ymax )
				end
			end
		end
	--	aaa.draw_bind_rect( 367, -0.5, -0.5, 0.5, 0.5 )
	--	aaa.draw_bind_rect( 360, -0.25, -0.25, 0.25, 0.25 )
	self:draw_layers_end()
end




