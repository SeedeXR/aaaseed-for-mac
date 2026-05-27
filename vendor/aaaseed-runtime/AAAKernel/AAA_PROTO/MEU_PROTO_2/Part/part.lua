function meu:define_meu_infos( )
	return { 	author = "Mâa",
				tags = { "2d", "3d", "Point", "coregraphic", "procedural", "art", "vj", "unfinished" },
				help =	{	"MEU just encapsulating a c_bdd_particle, need manipulationd in flatland.",
							"c_bdd_particle deal with a cpu rich particle system."
						}
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgbf()
	self:add_camera()
	self:add_mapping_and_blending()
	self:add_bu_texture_target_unit()
	self:add_multiple_uvf()
end