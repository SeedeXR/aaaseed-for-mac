function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local bdd = self:get_layer_bdd( 1 )

	local ix,iy = 1,1
--	bu = self:add_button(	{	ix,	iy,	2,2	 },	"Depth", bdd, "depth"	)
--	iy = iy + 2
--	bu = self:add_button(	{	ix,	iy,	2,2	 },	"color", bdd, "color"	)
	self:add_bu_texture_target_unit( { ix,iy }, "Tex", 1 )
	iy = iy + 5
	self:add_blending( {ix,iy} )
end