
--to use the generixc one if still needed
-- incrust used but no ui defined here ????
-- function meu:add_incrust( ix, iy )
-- 	local bu
-- 	self:add_button(	{	ix,		iy },				"Incrust",	self, "b_incrust",			false		):set_text_rect_ratio( 3 )
-- 	self:add_button(	{	ix+4,	iy },				"Inv",		self, "b_incrust_inverse",	false 	)

-- 	self:add_slider_two(	{ix,iy+1,	8,1},	"Luma", 	self, "incrust_min", "incrust_max", 0, 1, 0, 1 )
-- --	self:add_param_obj_name(	{ix,iy+1},			"Min",		self, "incrust_min",	0,1 )
-- --	self:add_param_obj_name(	{ix,iy+2},			"Max",		self, "incrust_max",	0,1 )
-- end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local layer = self:get_layer(1)
	ref.map = aaa.layer.build_bank_bind_2d_ref_table( layer, 0,3 )

	local sha =  self:get_shading()

	self:add_mapping_by_side_only()
	self:add_rgbf(	{3,6.8,	6,1 })
	self:add_blending( {1,6.8} )
	self:add_button(		{1,8},			"Inverse",	self, "b_inverse", false 	)

	bu = self:add_slider(	{1,9,	8,1},	"Wave",		sha:get_ref_frag_float(4), nil 	)
		bu:set_meter( true )

	ui.bu_tex = self:add_bu_texture()

	self:add_nb_uv( {1, 12} )

	self:add_camera()

	local iy = 2.
	bu = self:add_slider(	{13,iy,	4,1},	"delay", self, "delay", 1 )
		bu:set_min_max( 0.0001, 16 )

--	bu = self:add_button(	{8,iy},			"fade", self, "b_fade", false )
--	self.b_fade = true

	bu = self:add_button(	{9,iy},			"Sync", self, "b_sync", false )
		bu:set_text_rect_ratio(1.8)

	--local SY = 1.2
	bu = self:add_trig(	{11.8, iy,	1.2,1}, "go",	self, "b_go", false )
		bu:set_text_rect_ratio(1)
		self.ui.bu_go = bu
	self:add_size_uvf_video()
	self:add_trz()

	self:add_monitor()
end

function meu:init()
	self:add_shading()
end

function meu:is_fading()
	return self.time_left ~= nil
end

function meu:__trig_fade()
	--todo what happen if trig a fade while a fade is already going on
	self.b_go						= false
	self.time_fade					= 0
	--todo dim_next is nil
	self.bind_next, self.dim_next	= self.bind_2d_asked, self.dim_asked 
end

function meu:__stop_fade()
	if self.time_left then
		--aaa.print_method()
		self.time_left = nil
		if self.bind_next then
			self.bind		=	self.bind_next
			self.dim		=	self.dim_next
			self.bind_next = nil
		end
	end
end

function meu:update()
	local b_changed, bind_2d, dim = self.ui.bu_tex:get_bind_2d_dim_asked()
	if b_changed then
		self.bind_2d_asked, self.dim_asked = bind_2d, dim
	end

	if self.b_sync then
		if b_changed then
			self:__trig_fade()
		end	
	elseif self.b_go then
		self:__trig_fade()
	end
end

function meu:__draw_wavy_low( dim, bind1, bind2, inter, alpha )	--todo restore dim
	--aaa.print_fn()
	local ref = self.ref
	param.set( ref.tex_dim, dim )

	self:set_bind_2d( bind1, 1 )
	self:set_bind_2d( bind2, 2 )

	ref.color:set_alpha( alpha )

	local sha =  self:get_shading()
	sha:set_frag_float_1( inter )

	--self:print( self.incrust_min.." "..self.incrust_max )
	if self.b_incrust then
		if self.b_incrust_inverse then
			sha:set_frag_float_2_3( self.incrust_max, self.incrust_min )
		else
			sha:set_frag_float_2_3( self.incrust_min, self.incrust_max )
		end
	else
		sha:set_frag_float_2_3( 0, 0 )
	end
	sha:set_frag_int( 1, self.b_inverse and 1 or 0 )
	aaa.obj.update_then_draw( ref.__layer_marked )
end

function meu:draw()
	local ref = self.ref
	--self:print( "fade "..self.b_fade )
	local inter
--	if self.b_fade then
		inter = self.time_left
		if inter then
			inter = inter - aaa.time.dt
			--self:print( "time_left -> "..inter )
			if inter < 0 then
				self:__stop_fade()
				inter = nil
			else
				self.time_left = inter
				inter = inter / self.delay
			end
		end
	-- else
	-- 	self:__stop_fade()
	-- 	inter = nil
	-- end
	self.ui.bu_go:set_text_color_busy( inter~=nil )

	self:draw_layers_begin()
--[[
		if inter then
			--todo deal with no texture / fade to black
			--todo	fade to black/white then other
			--todo  have a nice fade even with no alpha
			--todo move to spip
			local color = ref.color
			local alpha = color:get_alpha()

			self:print( "\tinter -> "..inter )

			color:set_alpha( alpha )
			self:__draw_wavy_low( self.dim, self.bind )

			color:set_alpha( alpha * (1-inter)  )
			self:__draw_wavy_low( self.dim_next, self.bind_next )

			color:set_alpha( alpha )
		else
]]--			--aaa.print( self.dim )
		local color = ref.color
		local alpha = color:get_alpha()

		self:__draw_wavy_low( self.dim, self.bind, self.bind_next, inter or 0, self:get_alpha() )
	
	self:draw_layers_end()

--	self.ui.bu_go:print( "TEST" )
end
