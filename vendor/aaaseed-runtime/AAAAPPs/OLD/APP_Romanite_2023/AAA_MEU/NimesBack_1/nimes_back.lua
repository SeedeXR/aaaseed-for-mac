function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = 1.2
	local DY = .2
--	self:add_slider(	{9,4,	8,4}, "titi" )
	self.seed = 242 -- pick to be interesting in 5x5 and 7x7
	self.phase = 0

	bu = self:add_slider(		{ix,iy,			4,SY},		"Size",				self, "size",			4,	0,8	)
		bu:add_values_def( 1,3,4,5,6,7 )	
	bu = self:add_slider(		{ix+4,iy,		4,SY*2},	"Factor U",			self, "size_factor_u",	1,	0,8	)
			:set_color_back("u")
	iy = iy + SY
	bu = self:add_slider(		{ix,iy,			4,SY},		"Inside",			self, "size_int", 		1,	0,1	)
		bu:add_values_def( .9,.95,.98)
	iy = iy + SY
	bu = self:add_slider(		{ix,iy,			4,SY},		"Threshold",		self, "threshold", 		.5,	0,1	)
		bu:add_values_def( .5,.7 )
	bu = self:add_slider(		{ix+4,iy,		4,SY},		"Threshold Range",	self, "th_range", 		0,	0,1	)
		bu:add_values_def( .5,.7 )

	iy = iy + SY + DY

	self:add_rendering()

	ix,iy = 9,2+DY
	self:add_camera():set_preset_use( false )
	self:add_bu_texture_target_unit( 	{ix,iy,			8,3},		"Sky",	1, true	)
	iy = iy + 3
	self:add_bu_texture_target_unit( 	{ix,iy,			8,3},		"Hill",	2, true	)
	iy = iy + 3 + DY
	self:add_rgbf(	{ix,iy,	8,SY })
	iy = iy + SY
	self:add_blending( {ix,iy} )

end

function meu:update()
	MEDIA.set_dir_media_silent( "Nimes_2023/Nimes_Analogue" )
	self.bind_sky = IMGS.get_bind( "CIEL DEMI TEINTE 2.tga" )
	self:set_texture_bind_2d( 1, self.bind_sky )
	self.bind_hill = IMGS.get_bind( "COLLINES.tga" )
	self:set_texture_bind_2d( 2, self.bind_hill )
end

function meu:draw()
--	local ui = self.ui
	self:draw_layers_begin()
		self:draw_layer(1)

		local s = 13
		local h = 5.5
		local b = 0
		local hc = 1
		local grey = .7	

		gol.bind_texture( self.bind_sky )
		gol.set_wrap_2d_edge()
		gol.color_white( 1 )
		aaa.draw_rect_uv_at_z( -s,b, s,h,	-20 )
		gol.color_white( grey )
		aaa.draw_rect_uv_at_z( -s,b, s,-h,	-20 )
		
		gol.bind_texture( self.bind_hill )
		gol.color_white( 1 )
		gol.set_wrap_2d_edge()
		aaa.draw_rect_uv_at_z( -s,b, s,hc, -20 )
		gol.color_white( .8 )
		aaa.draw_rect_uv_at_z( -s,b, s,-hc, -20 )

	self:draw_layers_end()
end