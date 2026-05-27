function meu:define_meu_infos( )
	return { author = "Mâa", date="2024",
			tags = { "2d", "3D", "Core", "CoreGraphic", "Draw", "Experimental", "Procedural", "Tutorial", "Utility" },
			help= "Very basic MEU just using the c_layers"
			}
end

--todo add transfo
function meu:define_ui()
	local ref = self.ref
	local bu

	self:add_camera()
	self:add_rendering( {9,3})
end