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

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	self:add_camera()

	self.transfo_ref = {}

	local SX13 = 8 / 3
	self:add_slider(	{ix,iy,				SX13,SY},	"Offset x",		self.offset_pos, "x",	0, 0, 32):set_color_back( "x" )
	self:add_slider(	{ix+SX13,iy,		SX13,SY},	"Offset y",		self.offset_pos, "y",	0, 0, 32):set_color_back( "y" )
	self:add_slider(	{ix+SX13*2,iy,		SX13,SY},	"Offset z",		self.offset_pos, "z",	0, 0, 32):set_color_back( "z" )
	self:add_slider(	{ix+SX13*2,iy+SY,	SX13,SY},	"Depth Begin",	self, "z_cam",	0, -20, 20):set_color_back( "z" )

--	bu = self:add_sliders_xyz(	{ix,iy+.5,	8,.5},	"Fronts Pos",	self.fronts_pos, 	false,	2.5	)

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
		bu = self:add_button(	{ix,y,	SY,SY},	name, 	tab,"b_draw",	false )
			self.matte[i] = tab
		self:add_bu_texture_target_layer(	{ix,y+SY, nil,SYB}, "TEX_"..name, i, true,	self:get_layer(i+1) )
		ix = 1
		local y2 = y + 1
		local transfo =	aaa.obj.get_down_by_class_no_error( self:get_layer(i+1), "transfo_trs" )

		local pos = { x=0, y=0, z=0}

		local ref = {}
		local lay = { ref=ref }
		self.layers[i] = lay

		if transfo then
			--self:print( "transfo "..transfo )
			ref.tx      = param.get_ref( transfo, "translate_x" )
			ref.ty      = param.get_ref( transfo, "translate_y" )
			ref.tz      = param.get_ref( transfo, "translate_z" )
			ref.scale   = param.get_ref( transfo, "scale_factor" )
			ref.scale_y = param.get_ref( transfo, "scale_y" )
			
			local sy = .9
			local sx = 8
			bu = self:add_slider(	{ix,y2 + sy*0,		sx/2,sy},	name.."_Tra-X",		lay,"x",		0,	-10,10	)
				bu:set_color_back( "x")
				bu:set_text("Tra X")
			bu = self:add_slider(	{ix+sx/2,y2 + sy*0,	sx/2,sy},	name.."_Tra_Y",		lay,"y",		0,	-10,10	)
				bu:set_color_back( "y" )
				bu:set_text("Tra Y")
			bu = self:add_slider(	{ix,y2 + sy*1,		sx,sy},		name.."_Depth",		lay,"z",		0,	0,100	)
				bu:set_color_back( "z" )
				bu:set_text("Depth")
			bu = self:add_slider(	{ix,y2 + sy*2,		sx/2,sy},	name.."_Scale",		lay,"scale", 	1,	0,32 )
				bu:set_text("Scale")
			bu = self:add_slider(	{ix+sx/2,y2 + sy*2,	sx/2,sy},	name.."_Scale_y",	lay,"scale_y",	1,	0,4 ):set_color_back( "y" ):set_text( "Scale Y" )
				bu:set_text("Scale Y")
		end
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
--  	self:draw_layers_begin()
 		self:draw_layer( 1 )
		for i=1,8 do
			local matte = self.matte[i]
			if matte.b_draw then
				local pos = { x=0,y=0,z=0}
				-- if i < 5 then
				-- 	pos.x = self.backs_pos.x + self.layers[i].pos.x
				-- 	pos.y = self.backs_pos.y + self.layers[i].pos.y
				-- 	pos.z = self.backs_pos.z + self.layers[i].pos.z
				-- else
				-- 	pos.x = self.fronts_pos.x + self.layers[i].pos.x
				-- 	pos.y = self.fronts_pos.y + self.layers[i].pos.y
				-- 	pos.z = self.fronts_pos.z + self.layers[i].pos.z
				-- end

				local lay = self.layers[i]
				local ref = lay.ref
				--table.print( lay, "lay", 3 )
				local z = self.offset_pos.z + lay.z
				local f = (z-self.z_cam)/(self.offset_pos.z-self.z_cam)
				local x,y
				x = (self.offset_pos.x * f + lay.x)
				y = (self.offset_pos.y * f + lay.y)
				param.set( ref.tx, x*f )
				param.set( ref.ty, y*f )
				param.set( ref.tz, -z )

				param.set( ref.scale, lay.scale*f )
				param.set( ref.scale_y, lay.scale_y )

				self:draw_layer( 1+i )
			end
		end
 	self:draw_layers_end()
end




