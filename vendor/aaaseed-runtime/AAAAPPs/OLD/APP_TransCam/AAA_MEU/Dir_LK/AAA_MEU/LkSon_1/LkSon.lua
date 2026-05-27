function meu:init()
	local ref = self.ref
	local layer = self:get_layer(1)
	ref.material_front = param.get( layer, "material_front" )
	ref.material_back = param.get( layer, "material_back" )
	--todo access this in a smarter way
	-- like the 9 th branch or the material used
	-- ref.alpha = param.get_ref( "material/mat_008.mat", "Diffuse_alpha" )	
end
function meu:set_material_alpha( ref_param )
	local obj = pram.get_obj_attached(ref_param)
	param.set( obj, "Diffuse_alpha" )
end
function meu:draw()
	local v = aaa.midi.get_control( 1, 56 )
	if v > 0. then
		local ref = self.ref
		MEU.draw( self )
		set_material_alpha( ref.material_back )
		set_material_alpha( ref.material_front )
	end
end
