function meu:define_ui()
	local ref = self.ref
	local bu
	local par


	self:add_bu_texture_target_unit()

	self:add_mapping_by_side_only()
	self:add_rgbf(	{1,6.8,	1,1 })
	self:add_blending()

	self:add_camera()

	self:add_rendering()

	self:add_size_uvf_video()

	self:add_monitor()
end

function meu:init()
	local ref = self.ref
	ref.layer1 = self:get_layer(1)
	ref.layer2 = self:get_layer(2)
	ref.layer3 = self:get_layer(3)
	ref.bdd_text = self:get_layer_bdd(1)
	ref.font		= param.get_ref( ref.bdd_text, "font" )
	ref.alignment	= param.get_ref( ref.bdd_text, "alignment" )
	ref.text		= param.get_ref( ref.bdd_text, "text" )
end


function meu:set_text( txt )
	local ref = self.ref
	param.set( ref.text, txt )
end