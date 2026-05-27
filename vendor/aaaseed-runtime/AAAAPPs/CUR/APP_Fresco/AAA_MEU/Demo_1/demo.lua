function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
	local par

	local ix,iy = 1,1
	local DY = .2


	self:add_bu_texture_target_unit( {ix,iy, 8,6} )

	self:add_rendering()
	self:add_nb_uv( {1, 12.5}, self:get_layer_bdd(1) )

	ix,iy = 1,2+DY
	self:add_camera()
	self:add_transfo( {9,iy, 8,2.4}, 1 )
	iy = iy + 2.4 + DY
end

function meu:init()
end

-- function meu:draw()
-- 	--gol.translate_y(-1)
-- 	MEU.draw( self )
-- end


