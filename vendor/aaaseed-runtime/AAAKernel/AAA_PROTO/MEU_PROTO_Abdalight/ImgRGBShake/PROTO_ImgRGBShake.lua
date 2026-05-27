function meu:define_meu_infos( )
	return { author = "Abdalight", date = "2025",
			tags = { "3D", "Art", "Draw", "Generator" },
			help = "RGB channel shake — mix RGB with pixel offsets" }
end

function meu:define_ui()
	local ref   = self.ref
	local ui    = self.ui
	local sha   = self:get_shading()
	local ix,iy = 1,1
	local SY,DY = 1,.2
	local bu

	self:add_shading_ui(		{ix,iy,			8,SY} )
    iy = iy + SY + DY
	

	local y_begin = iy
------------------------------
	self:set_tab_key( "Main" )
	ix, iy = 1,y_begin

	
	bu = self:add_bu_texture_target_unit( {ix,iy,   8,6*SY,b_compact=true},				 "Tex_Input",				1,		true	)
		ui.bu_tex_input = bu
	iy = iy + 6 * SY

	self:add_slider(			{ix,iy,			4,SY},		"Offset",			self,		"offset",				0,	  -20,20	)
	self:add_slider(			{ix+4,iy,		4,SY},		"Angle",			self,		 "angle",				0,	   0,360	)
	iy = iy + 4 * SY

------------------------------

	ix,iy = 9,y_begin
	self:add_camera()
	self:add_rendering()

	ui.bu_rgbf = self:add_rgbf(	{ix,iy,			8,SY}	)
	iy = iy + SY + DY

	self:add_transfo(			{ix,iy,		 nil,2.4},  1 )
	iy = iy + 2.4 * SY + DY

	------------------------------
	local sha = self:get_shading()
	self:add_shading_sliders_tab(	sha,	{"Vert","Frag"},	y_begin,SY	)

	ui.bu_frag_float_1	 = self:get_bu_by_key( "fragment_float_1" )
	ui.bu_frag_float_2 	 = self:get_bu_by_key( "fragment_float_2" )
	ui.bu_frag_float_3 	 = self:get_bu_by_key( "fragment_float_3" )
end


function meu:init()
	self.layer_shading = 2
	local sha = self:add_shading( self.layer_shading )
end


function meu:update()
	local ui = self.ui
	local bu_tex = ui.bu_tex_input

	if bu_tex then
		local b_changed, bind_2d = bu_tex:get_bind_2d_asked()
		if b_changed then
			self.bind_2d_used = bind_2d 
		end
	end
	if ui.bu_frag_float_2 then ui.bu_frag_float_2:set_value( self.offset )	end
	if ui.bu_frag_float_3 then ui.bu_frag_float_3:set_value( self.angle  )	end
end


function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 )
		self:draw_layer( 2 )
		self:draw_canva()
	self:draw_layers_end()
end


function meu:draw_canva() 
	local x, y, z = 0, 0, 0
	local sx, sy = 1, 0.75

	gol.bind_texture( self.bind_2d_used )
	aaa.draw_rect_uv_at_z( x-sx, y-sy, x+sx, y+sy, z )
end


