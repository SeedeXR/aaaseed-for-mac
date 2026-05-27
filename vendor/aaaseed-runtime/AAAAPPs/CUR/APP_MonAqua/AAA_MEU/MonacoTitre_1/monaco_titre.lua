function meu:define_ui()
	local ui = self.ui
    local ix, iy = 2, 2
	local bu
	local SY = 1

	bu = self:add_text_info(	{ix,iy,	15,SY}, "Time Info" )
		ui.bu_info = bu
	iy = iy + SY*1.5

	bu = self:add_button( {ix, iy }, "Draw", self, "b_draw", false )
	iy = iy + SY
	bu = self:add_button( {ix+1, iy }, "Force Draw", self, "b_draw_force", false )
	iy = iy + SY
	-- bu = self:add_button( {ix+1, iy }, "slot1", self.slot[1], "b_draw", false )
	-- 	bu:set_value_load_save( false )
    -- iy = iy + SY
	-- bu = self:add_button( {ix+1, iy }, "slot2", self.slot[2], "b_draw", false )
	-- 	bu:set_value_load_save( false )
    -- iy = iy + SY

--	bu = self:add_slider(	{ix+4,iy,	4,SY},	"Circles Nb U",		self, "circles_nb_u", 5, 2, 16 ):set_value_type_integer(true)
--	bu = self:add_slider(	{ix,iy,	8,SY},	"Density",		self, "density", .5, 0, 1. )
end

function meu:load_textures()
	if not app:is_kc() then return end

	self.__binds = {}
	MEDIA.set_dir_media_silent( "Monaco" )
	for _,scene_name in pairs(app.scenes_name) do
		local fname = "Titres/"..scene_name..".png"
		local img = IMGS.get_img( "Titres/"..scene_name..".png"  )
		if img then
			local bind = img:get_bind()
			self.__binds[scene_name] = bind
			--self:box_debug( "bind "..bind.." for "..fname )
		else
			self:print_error( "Can't read "..fname )
			--self:box_debug( "Can't read "..fname )
		end
	end
end
-- function meu:titres( b )
-- 	self.b_draw = b
-- end
function meu:update_ui()
	local bu = self.ui.bu_info
	if app:is_kc() then
		bu:set_text( "Time is "..self.time )
	else
		bu:set_text( "Only on KC" )
	end
end

function meu:draw()
	if not app:is_kc() and not self.b_draw_force then return end

	local scene_name = app:get_scene()
	if not scene_name then return end

	local time
	if self.scene_name_last ~= scene_name then
		self.scene_name_last = scene_name
		time = 0
	else
		time = self.time + aaa.time.dt
	end
	self.time = time

	if not self.__binds then
		self:load_textures()
	end
	if self.__binds then
		local alpha = math.fn_linear( time, 1,	0,1,	4,1,	6,0,	1000,0	)
		if (alpha>0 and self.b_draw) or self.b_draw_force then
			local bind = self.__binds[scene_name]
			if bind then
				self:draw_layers_begin()
					self:draw_layer(1)
					aaa.layer.set_bind_2d( self:get_layer(2), bind )
					gol.color_white( self.b_draw_force and 1 or alpha )
					self:draw_layer(2)
				self:draw_layers_end()
			end
		end
	end

end

