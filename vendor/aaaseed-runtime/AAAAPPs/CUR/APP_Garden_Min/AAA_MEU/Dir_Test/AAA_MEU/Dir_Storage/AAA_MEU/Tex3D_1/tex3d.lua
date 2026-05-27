function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	self:add_slider(	{9,4,	8,4}, "toto" )

	self:add_camera()
	self:add_mapping_and_blending()

	local ix = 1
	local iy = 1
	ui.bu_dst = self:add_texture_selector( {ix,iy, 8,1}, "dst" )

	local sha = self:get_shading()
	local iy = 2
	self:add_trig(		{1,iy			},	"Reload",		sha.frag.reload		)
	self:add_slider(	{1,iy+1,	8,1},	"Alpha_factor",	sha:get_ref_frag_float(1), nil, 1,0,4 )
end

function meu:init()
	self:add_shading( 2 )
end
