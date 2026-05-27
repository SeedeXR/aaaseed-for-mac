
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
	self:add_shading_ui()
	
	iy = iy + SY
	--etienne we set value in meu:draw() below to app.photo_correction.contrast
	bu = self:add_slider(	{ix,iy,	8,SY}, "f1", sha:get_ref_frag_float( 1 ), nil, 0, -1, 1 )
	-- iy = iy + SY
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "color G", sha:get_ref_frag_float( 2 ), nil, .5, 0, 1 )
	-- iy = iy + SY
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "color B", sha:get_ref_frag_float( 3 ), nil, .5, 0, 1 )
	-- iy = iy + SY

	-- bu = self:add_slider(	{ix,iy,	8,SY}, "start", sha:get_ref_frag_float( 4 ), nil, .5, 0, 100 )
	-- iy = iy + SY
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "density", sha:get_ref_frag_float( 5 ), nil, .5, 0, 100 )
	-- iy = iy + SY

	iy = 4
	SY = 2.5

	self:add_bu_texture_target_unit( {9,iy, nil,SY}, "Src", 1	)
	iy = iy + SY
end

function meu:init()
	local ref = self.ref
	local sha = self:add_shading()
	sha:set_save_frag_float( false, 1,3 )
end

function meu:draw()
 	self:draw_layers_begin()
	 	-- attr
 		self:draw_layer( 1 )
		-- if self.b_draw then
		-- 	self:draw_layer( 2 )
		-- end

		local sha = self:get_shading()
		sha:set_frag_float_1_3(	app.photo_correction.contrast,
								app.photo_correction.brightness,
								app.photo_correction.saturation )
	
		local photo = app.photo
		if photo then
			local sx,sy = photo.sx, photo.sy
			if sx then
				gol.bind_texture( photo.bind )
				sx = sx
				sy = sy * 16/9
				aaa.draw_rect_uv( -sx,-sy, sx,sy )
			end
		end
 	self:draw_layers_end()
end




