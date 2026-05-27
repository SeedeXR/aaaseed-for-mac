			function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	ui.cam = self:add_camera()
	local ix,iy = 1,1
	local sx = 1
	local sy = .8
	local dy = .2
	self:add_shading_ui( { ix,iy,	8,sy } )
	local sha = self:get_shading()
	iy = iy + sy + dy
	--bu = self:add_selector(	{ix,iy,	8,1}, "Type" )
	--	bu:set_nb( 8, 1 )
		--bu:set_item_text( 2, "Toon", "BW", "Flag" )
		--bu:set_target_param( sha:get_ref_frag_int( 1 ) )
	--iy = iy + sy

--	self:add_button(	{ix,iy,	sy,sy },	"" )
	self:add_bu_texture_target_unit( { ix,iy }, "Tex Back", 1 )
	iy = iy + 2.5 + dy
	self:add_bu_texture_target_unit( { ix,iy }, "Tex Front", 2 )
	iy = iy + 2.5 + dy

	self:add_rgbf(	{ix,iy,	8,sy })
	iy = iy + sy + dy
	--self:add_rendering( {ix,iy, nil,sy*2} )
	--iy = iy + sy*2 + dy

 	ix,iy = 9,3
	--self:add_button( {ix,iy, sy,sy }, "Double", sha:get_ref_frag_int(2) )
	bu = self:add_selector(	{ix,iy,	6,sy},	"Double" ):set_nb( 3 )
		bu:set_target_param( sha:get_ref_frag_int(2) )
		bu:set_item_text( 2, "Double", "By 3" )
	iy = iy + 1
	self:add_button( {ix,iy, sy,sy }, "Reverse", sha:get_ref_frag_int(1) )
	iy = iy + 1
	sy = 3
	ui.bu_color = {}
	ui.bu_color[1] = self:add_rgbfa(	{ix,iy,	nil,sy}, "Back", 	false )
	iy = iy + sy
	ui.bu_color[2] = self:add_rgbfa(	{ix,iy,	nil,sy}, "Front", 	false )

-- --	self:add_button( {			ix, iy }, 	"Paint", 		self, "b_paint", true )
-- --	self:add_button( {			ix+4, iy }, 	"Add", 			self, "b_add", false )
-- --	self:add_bu_texture_target_unit( {ix, iy+1},	"tex_paint", 2		)

-- 	ix, iy = 9, 3
-- 	bu = self:add_selector(	{ix,iy,	6,sy},	"How" ):set_nb( 3 )
-- 		bu:set_target_lua(	self, "s_how", 1 )
-- 		bu:set_item_text( 1, "2d", "front", "Extruded" )
-- 	iy = iy + sy
-- 	local sx = 5
-- 	bu = self:add_selector(	{ix,iy,	sx,sy},	"Map" ):set_nb( 3 )
-- 		bu:set_target_lua(	self, "s_map", 1 )
-- 		bu:set_item_text( 1, "No", "Map", "LVColors" )
-- 	iy = iy + sy
-- 	sx = 5
end
--[[
function meu:init()
	local obj = self:et_layer( layers, 1 )
	if obj then
		local color = aaa.obj.get_down_by_class_no_error( obj, "color" )
		self.ref.alpha = param.get_ref( color, "global_alpha" )
	end
end
--]]
function meu:update()
	local sha = self:get_shading()
	local function set_color_shader( id )
		local r,g,b,a = self.ui.bu_color[id]:get_rgba()
		sha:set_frag_vec4( id, r, g, b, a )
	end
	set_color_shader( 1 )
	set_color_shader( 2 )
-- --	local sha = self:get_shading()
-- --	param.set( sha:get_ref_frag_int(3), self.b_paint and (self.b_add and 1 or 2) or 0 )
-- --	local a = self.ref.alpha
-- --	if a then
-- --		param.set( a, self:get_mu():get_value() )
-- --	end
end

function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 )
	self:draw_layers_end()
end