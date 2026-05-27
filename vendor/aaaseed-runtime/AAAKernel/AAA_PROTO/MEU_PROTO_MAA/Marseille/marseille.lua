function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "3d", "art", "Procedural", "draw", "unfinished" },
			help = "this MEU draw a structure base om Maa's algo\n"..
					"Mâa need to add Ui for this"
		}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgb(	{1,1 } )
	self:add_camera()
end