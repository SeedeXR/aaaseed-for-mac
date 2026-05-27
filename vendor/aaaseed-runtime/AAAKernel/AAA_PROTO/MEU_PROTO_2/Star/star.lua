function meu:define_meu_infos( )
	return { 	author = "Mâa",
				tags = { "2d", "3d", "Point", "procedural", "art", "vj", "unfinished", "depreciated" },
				help =	{	"MEU just encapsulating a c_bdd_particle, need manipulationd in flatland.",
							"c_bdd_particle deal with a cpu rich particle system.",
							"Depreciated: MEU Part is more advanced"
						}
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgbf(	{1,2 } )
	self:add_blending( {2,3} )

	self:add_camera()
end