function meu:define_meu_infos( )
	return { author		= "Mâa",
			name_long	= "Floating Point",
			tags		= { "2d", "3D", "art", "Draw", "Core", "CoreGraphic", "Procedural", "point", "VJ", "unfinished" },
			help		=	"Minimal MEU encapsulation of a c_bdd_Boid\n"..
							"Floating Point is an Art piece done by Mâa based on Boids"
		}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgb()
	self:add_camera()
	self:add_mapping_and_blending()
	self:add_bu_texture_target_unit()
	self:add_mapping_hexa()
end