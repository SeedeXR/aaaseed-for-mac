function meu:define_meu_infos( )
	return { 	author = "Mâa",
				date = "2019",
				version = 0,
				tags = { "2d", "Draw", "coregraphic", "core", "depreciated", "experimental", "texture" },
				help = "erase the current FBO using a bdd_clear_screen",
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local bdd = self:get_layer_bdd( 1 )

	local ix,iy = 1,1
	bu = self:add_button(	{	ix,	iy,	2,2	 },	"Depth", bdd, "depth"	)
	iy = iy + 2
	bu = self:add_button(	{	ix,	iy,	2,2	 },	"color", bdd, "color"	)
end

function meu:draw_icon()
	gol.set_line_width( 2 )
	gol.set_texture_dim( 0 )
	local S = .35
	gol.push_translate_scale_2d( -.35, 0, .3, 1. )
		gol.draw_lines_2d(	-S,-S,	S,S,	-S,S,	S,-S )
	gol.pop_matrix()
end