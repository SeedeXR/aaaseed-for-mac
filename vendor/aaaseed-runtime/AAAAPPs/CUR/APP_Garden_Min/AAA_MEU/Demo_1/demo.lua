function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
	local par

	self:add_camera()

	self:add_bu_texture_target_unit()

	self:add_rendering()
	self:add_nb_uv( {1, 12.5}, self:get_layer_bdd(1) )

	local ix = 1
	local iy = 1
end

function meu:init()
end


