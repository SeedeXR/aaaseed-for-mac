function meu:define_meu_infos( )
	return { author = "Mâa", date="2023",
			tags = { "2d", "Art", "Draw", "Generator" , "Procedural", "Texture", "VJ" },
			help="Inspired from the japanese flags" }
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local sha = self:get_shading()

	local ix,iy = 1,1
	local SY,DY = 1,.5

	self:add_camera()
	self:add_rendering()

	self:add_shading_ui( 	{ix,iy} )
	iy = iy + SY
	bu = self:add_slider_two({ix,iy,	8,SY},	"Radius",	nil,sha:get_ref_frag_float(5),sha:get_ref_frag_float(6),
															1,3,	0,16	)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,SY},	"Ray Nb",	sha:get_ref_frag_int(1),nil,	8,	1,256 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,SY},	"Ray",		sha:get_ref_frag_float(7),nil,	0,	0,1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,SY},	"Rotate",	sha:get_ref_frag_float(8),nil,	0,	-1,1 )
	iy = iy + SY + DY

	self:add_mapping_and_blending(	{ix,iy}	)

	ix,iy = 9,2+DY
	ui.bu_col = {}
	local SYC = SY *1.2
	self:add_transfo( {ix,iy, 8, 2.4} )
	iy = iy + 2.4 + DY

	ui.bu_col[1] = self:add_rgbfa(	{ix,iy,		nil,SYC}, "Sun", false )
	ui.bu_col[2] = self:add_rgbfa(	{ix,iy+SYC,	nil,SYC}, "Sky", false )
end

function meu:init()
	self:add_shading()
end

function meu:update()
	local sha = self:get_shading()
	local ui = self.ui
	local bu_col = ui.bu_col
	for i=1,2 do
		local r,g,b,a = bu_col[i]:get_rgba()
		sha:set_frag_vec4( i, r,g,b, a )
	end
end