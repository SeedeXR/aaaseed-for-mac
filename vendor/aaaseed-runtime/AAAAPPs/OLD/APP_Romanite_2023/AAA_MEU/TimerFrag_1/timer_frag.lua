
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix = 1
	local iy = 1
	local SY = 1.
	local SYH = .4
	local SYT = 2.
	local DY = .2
	local SX3 = 8/3

	self:add_rendering()
	self:add_camera( nil, 16 )

	self:add_shading_ui()


	bu = self:add_slider(		{ix,iy,		8,SY},		"Phase", nil, nil, .5, 0,1 )
		bu:set_target_param( sha:get_ref_frag_float(1) )	
	iy = iy + SY + DY
	
	-- add_but(	"Draw_box"	)
	-- local SXH = (8-SY)/2
	-- local OX = SXH + SY
	-- local function add_but_center(	name, ... )
	-- 	self:add_button({ix+SXH,iy,	SY,SY},		name,			self,	"b_"..name, 		false	)
	-- end

	--self:add_slider(	{ix+2,iy,	4,SY},	"Rotate",		sha:get_ref_frag_float(9),	nil, 	0, -2,2 )
	--	:set_color_back( "w" )
	--iy = iy + SY
	self:add_size_uvf_video( {ix,iy+.5,	8,3})

	ix,iy = 9,4
	local SYC = 6
	self:add_bu_texture_target_unit( {ix,iy, 8,SYC}, "Src" )
		self:set_bu_texture_preset_use( 1, false )

end

function meu:init()
	local ref = self.ref
	ref.layer_attr = self:get_layer(1)
	-- ref.layer_back = self:get_layer(2)
	-- ref.layer_front = self:get_layer(3)
	-- ref.layer_flex = self:get_layer(4)

	local sha = self:add_shading( 2 )
	sha:set_save_frag_float(	false,	1,2 )

end

-- function meu:update_groups( name )

-- end

-- function meu:update()
-- 	local sha = self:get_shading()
-- end

function meu:draw()
	local ref = self.ref
	self:draw_layers_begin()

		self:draw_layer(1)
		self:draw_layer(2)

	self:draw_layers_end()
end
