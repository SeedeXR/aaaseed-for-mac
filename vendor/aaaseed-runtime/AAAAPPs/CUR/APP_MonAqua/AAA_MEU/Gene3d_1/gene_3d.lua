
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	--self:add_rgbf(	{1,4.5 } )

	--self:add_bu_texture_target_unit()

	self:add_rendering()

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	iy = 2
--	bu = self:add_button(	{		ix, iy,		SY, SY },	"Draw Cube", 	self, "b_draw_cube", false )
--	iy = iy + SY

	iy = iy + SY



	--self:set_tab_key( "TEX" )
	iy = 4
	SY = 2.5
	for i=1,1 do
		self:add_bu_texture_target_unit( {9,iy, nil,SY}, "Tex_"..i, i	)
		iy = iy + SY
	end
	--self:set_tab_key_def()
end

function meu:init()
	local ref = self.ref
end

-- function meu:update_ui()
-- 	local ui = self.ui
-- 	local ref = self.ref
-- 	ui.bu_info:set_text( param.get( ref.filename_fbx ) )
-- end

function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 )	--	set the "attribute" no more shader her
		self:draw_layer( 2 )
	self:draw_layers_end()
end




