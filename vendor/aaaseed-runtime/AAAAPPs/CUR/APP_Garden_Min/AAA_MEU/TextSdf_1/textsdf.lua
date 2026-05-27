
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local ix,iy
	local SX, SY = 5, 1
	local DY = .2
	local sha = self:get_shading()

	ix,iy = 1,3.7
--	self:add_mapping_by_side()

	self:add_shading_ui()
	self:add_rendering()
	self:add_rgbf(	{1,12,	nil,nil}, "Color" )

	self:set_tab_key( "Main" )
	self:add_bu_texture_target_unit( 	{ix,1}, "tex_height", 1	)
	self:add_bu_texture_target_unit( 	{ix,iy+1}, "tex_refract", 2	)
	self:add_camera()

	ix, iy = 9, 4
	bu = self:add_selector(	{ix,iy,	8,SY*2}, "Render" )
		bu:set_nb_min_0( 3, 2 )
		bu:set_item_text( 2, "Maa", "MaaBis", "GradRG", "Grad", "Normal" )
		bu:set_target_param( sha:get_ref_frag_int( 1 ) )
	iy = iy + SY * 2 + DY
	ui.bu_col = {}
	iy = iy + 1
	ui.bu_col[1] = self:add_rgbf(	{ix+3,iy,	SX,SY}, 	"Texture_col", false )
	iy = iy + 1
	ui.bu_col[2] = self:add_rgbf(	{ix+3,iy,	SX,SY}, 	"Back_col", false )
--
end

function meu:init()
	self.time = 0
	self:add_shading()
end

function meu:update()
	local sha = self:get_shading()
	self.time = self.time + aaa.time.dt

	local bu_col = self.ui.bu_col
	for i=1,2 do
		local r,g,b,a = bu_col[i]:get_rgba()
		sha:set_frag_vec4( i, r,g,b,a ) -- problem : all slider change all values ..?!
	end

	--self:print( self.ui.bu_col:get_rgba() )
end
