function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "2d", "3D", "Art", "Draw", "Procedural", "Texture", "VJ", "Depreciated", "Unfinished" },
			help = "Example of vertex/fragment shader applied to a grid\n"..
					"with UI access to shader uniforms"
			}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	local sha = self:get_shading()

	local ix,iy = 1,1
	local SY = .99
	local DY = .2

	self:add_shading_ui(	{ix,iy,		8,SY} )
	iy = iy + SY + DY

	local y_begin = iy

------------------------------
	self:set_tab_key( "Main" )
	ix, iy = 1,y_begin

	self:add_button(				{ix,iy,		1,SY},	"Time Active",	self,"b_time",	false	):set_text_visible(false)
	self:add_slider(				{ix+1,iy,	5,SY},	"Time",			self,"time",	0,		0,100 )	
	self:add_trig_method(			{ix+6,iy,	2,SY},	"Restart",		self,"restart"	)
	iy = iy + SY + DY

	self:add_bu_texture_target_unit_nb( {ix,iy,	8,10},	"Tex_", 2,2 )	

	ix,iy = 9,y_begin
	self:add_camera()

	--self:begin_bu_group( "grid" )
	self:add_size_uvf_video( {ix,iy,			8,3} )
	--self:end_bu_group()
	iy = iy + 3 + DY

	self:add_transfo( {ix,iy, 8,3} )
	iy = iy + 3 + DY

	self:add_mapping_by_side_only(	{ix,iy,		8,SY}	)
	iy = iy + SY + DY

	self:add_rgbf(		{ix,iy,		8,SY}	)
	iy = iy + SY + DY

	self:add_blending( 				{ix,iy} )
	iy = iy + SY + DY

	self:add_rendering()

------------------------------
	local sha = self:get_shading()
	self:add_shading_sliders_tab(	sha,	{"Vert","Frag"},	y_begin,SY	)

	ui.bu_time_vert = self:get_bu_by_key( "vertex_float_1" )
	ui.bu_time_geom = self:get_bu_by_key( "geometry_float_1" )
	ui.bu_time_frag = self:get_bu_by_key( "fragment_float_1" )

--	self:set_tab_key( "Vert" )

end

function meu:init()
	local ref = self.ref

	self.layer_shading = 2
	local sha = self:add_shading( self.layer_shading )
end

function meu:restart()
	self.time = 0
end

function meu:update()
	local ui = self.ui
	local bu

	local t = self.time
	if self.b_time then
		t = t + aaa.time.dt
		self.time = t
		--aaa.show( t, "t" )
	end

	ui.bu_time_vert:set_value( t )
	bu = ui.bu_time_geom
	if bu then bu:set_value( t ) end
	ui.bu_time_frag:set_value( t )
end

function meu:draw()
	self:draw_layers_begin()

		self:draw_layer( 1 )
		self:draw_layer( 2 )
		self:draw_layer( 3 )
		--gol.set_quad_uv()
		-- aaa.draw_rect_uv(-1, -1, 1, 1)

	self:draw_layers_end()
end