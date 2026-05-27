
function meu:define_ui()
	--self:add_camera()
end

function meu:read_fbo_tex( index, u, v, b_clamped )
	local r,g,b,a = aaa.img.get_color_uv(	index, u, v,	b_clamped )
	return { r, g, b, a }
end

function meu:get_picked_id()
	if self.id_picked then
		return self.id_picked
	else
		return 0
	end
end

function meu:init()
	self.__mouse_pos_render = { 0,0 }
end

function meu:draw()
	local x = self.__mouse_pos_render[1]
	local y = self.__mouse_pos_render[2]

	self:draw_layers_begin()
		self:draw_layer(1)

		gol.bind_texture( 115 )
		local sh = .01
		gol.set_quad_uv( x-sh, y-sh,  x+sh, y+sh )
		aaa.draw_rect_uv(	-4, -4,		4, 4 )

	self:draw_layers_end()
end

function meu:update()
	local mouse_x,mouse_y = aaa.mouse.get_xy_render()
	local bu = ga:find_bu_by_pos( mouse_x, mouse_y )
	local blobs = ga:get_blobs()

	-- Set _b_is_picking to true if screen is touched
	if blobs:get_touch_nb() > 0 then
		self:print("click")
		self:print( "mouse x : " .. mouse_x, "mouse y : " .. mouse_y, "bu : " .. bu )
		if bu then
			self:print( "bu_name : "..bu:get_class_name().." todo : if monitor use it to transform coor" )
		end
		self._b_is_picking = true
	end
	if blobs:get_untouch_nb() > 0 then
		self:print("unclick")
		self._b_is_picking = false
	end
	-- If touching / clicking : pick object
	--if self._b_is_picking == true then
		self.__mouse_pos_render = { mouse_x, mouse_y }
		local tex = self:read_fbo_tex( 75, .5, .5, 0 )
		--self:print( "texture :  r = "..tex[1].." g = "..tex[2].." b = "..tex[3].." a = "..tex[4] )
		local scaled_tex = tex[1] * 64
		local id_picked
		if scaled_tex < 31.5 then
			id_picked = math.floor( scaled_tex )
		else
			id_picked = math.ceil( scaled_tex )
		end
		if id_picked > 0 then self:print( "id : "..id_picked ) end
		self.id_picked = id_picked
	--end
end
