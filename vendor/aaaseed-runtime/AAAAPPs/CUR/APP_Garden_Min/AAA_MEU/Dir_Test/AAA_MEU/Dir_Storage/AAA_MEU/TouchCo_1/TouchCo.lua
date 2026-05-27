function meu:define_meu_infos( )
	return { 	author = "Mâa ",
				tags = { "device", "input", "output", "unfinished", "2d", "texture", "tutorial" },
				help =	{	"MEU dealing with the TouchCo device."
						}
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local iy

	self:add_bu_texture_target_unit( {1,1} )
	bu = self:add_button(	{0,	5 }, 	"Active", ref.bdd, "active", false )
		self:add_monitor( {1,10.5} )
end

function meu:init()
	local ref = self.ref
	ref.bdd = aaa.obj.get_down_by_class( ref.__layer_marked, "bdd_touchco" )
end

