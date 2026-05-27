function meu:define_ui()
	local ui = self.ui
    local ix, iy = 2, 2
	local bu
	local SY = 1

	bu = self:add_button( {ix, iy }, "Draw", self, "b_draw", false )
		bu:set_value_load_save( false )
	iy = iy + SY
	bu = self:add_button( {ix+1, iy }, "slot1", self.slot[1], "b_draw", false )
		bu:set_value_load_save( false )
    iy = iy + SY
	bu = self:add_button( {ix+1, iy }, "slot2", self.slot[2], "b_draw", false )
		bu:set_value_load_save( false )
    iy = iy + SY

--	bu = self:add_slider(	{ix+4,iy,	4,SY},	"Circles Nb U",		self, "circles_nb_u", 5, 2, 16 ):set_value_type_integer(true)
--	bu = self:add_slider(	{ix,iy,	8,SY},	"Density",		self, "density", .5, 0, 1. )
end

function meu:init()
	self.slot = { {}, {} }
end

function meu:set_pedago( slot_id, fname )
	if inside( slot_id, 1, 2 ) then
		local slot = self.slot[slot_id]
		local b = false
		if fname then						--hack
			MEDIA.set_dir_media_silent( "Monaco" )
			local img = IMGS.get_img( "Pedago/"..fname..".png"  )
			if img then
				aaa.layer.set_bind_2d( self:get_layer(1+slot_id), img:get_bind() )
				b = true
			end
		end
		slot.b_draw = b
	end
end

function meu:set_pedago_general( b )
	self.b_draw = b
end

function meu:draw()
--	if (not app:is_kake()) and (not app:is_pc_dev() ) or (app:is_tablet())  then return end

	-- local scene_name = app:get_scene()
	-- if not scene_name then return end
	-- --self:print( scene_name )
	-- if scene_name~="PIN2" then
	-- 	self.time = 0	-- reset time
	-- 	return
	-- end

	if self.b_draw then
		self:draw_layers_begin()
			self:draw_layer(1)

			--todo a generic layer fn
			for i=1,2 do
				local slot = self.slot[i]
				if slot.b_draw then
					self:draw_layer(1+i)
				end
			end
    	self:draw_layers_end()
	end
end

