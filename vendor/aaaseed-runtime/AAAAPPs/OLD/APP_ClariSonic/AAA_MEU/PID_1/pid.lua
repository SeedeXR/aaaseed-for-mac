
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local layer = self:get_layer(1)
	ref.map = aaa.layer.build_bank_bind_2d_ref_table( layer, 0,3 )

	self.ui.bu_col = self:add_rgbf(	{1,8.2 } )
	self:add_mapping_by_side()

	ui.bu_tex = self:add_bu_texture()

	self:add_camera()

	local iy = 2.
	bu = self:add_slider(	{13,iy,	4,1}, "delay", self, "delay", 1 )
		bu:set_min_max( 0.0001, 16 )

	bu = self:add_slider(	{1,12,	8,2}, "gamma", self, "gamma", 1, 0, 4 )

--	bu = self:add_button( {8, iy }, "fade", self, "b_fade", false )
--	self.b_fade = true

	bu = self:add_button( {9, iy }, "Sync", self, "b_sync", false )
		bu:set_text_rect_ratio(1.8)

	bu = self:add_trig(	{11.2, iy }, "go", self, "b_go", false )
		bu:set_text_rect_ratio(1)
		self.ui.bu_go = bu
	self:add_size_uvf_video()
	self:add_trz()

	self:add_monitor()
end

function meu:init()
	self:add_shading()
end

function meu:__trig_fade()
	--aaa.print_method()
	self.b_go = false
	self.time_left					= self.delay
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

function meu:__draw_pid_low( dim, bind1, bind2, inter, alpha )	--todo restire dim
	local ref = self.ref
	param.set( ref.tex_dim, dim )

	self:set_bind_2d( bind1, 1 )
	self:set_bind_2d( bind2, 2 )

	ref.color:set_alpha( self.ui.bu_col:get_alpha()*alpha )

	self:get_shading():set_frag_float_1( inter )
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
			self:__draw_pid_low( self.dim, self.bind )

			color:set_alpha( alpha * (1-inter)  )
			self:__draw_pid_low( self.dim_next, self.bind_next )

			color:set_alpha( alpha )
		else
]]--			--aaa.print( self.dim )
	--		local color = ref.color
	--		local alpha = color:get_alpha()

		self:__draw_pid_low( self.dim, self.bind, self.bind_next, inter or 0, self:get_alpha() )

	self:draw_layers_end()
end
