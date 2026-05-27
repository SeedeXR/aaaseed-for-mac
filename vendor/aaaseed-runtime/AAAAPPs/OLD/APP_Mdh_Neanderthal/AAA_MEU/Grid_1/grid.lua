
function meu:define_ui()
	local ref = self.ref
	local bu
	local pa
	local ui = self.ui

	self:add_camera()

	ui.color = {}
	local y = 2
	local x = 9
	local dy = 3
	ui.color[1] = self:add_rgbfa(	{x,y,	nil,dy}, "back", 	false )
	y = y+dy
	ui.color[2] = self:add_rgbfa(	{x,y,	nil,dy}, "grid 1", 	false )
	y = y+dy
	ui.color[3] = self:add_rgbfa(	{x,y,	nil,dy}, "grid 2",	false )
end

function meu:init()
	self:add_shading()
end

function meu:update()
	local su, sv = self:get_model_size_uv()

	local sha = self:get_shading()
--	sha:set_frag_float_1_2( su, sv )
	local alpha = self:get_alpha()

	local function set_color_shader( id )
		local r,g,b,a = self.ui.color[id]:get_rgba()
		sha:set_frag_vec4( id, r, g, b, a*alpha )
	end

	set_color_shader( 1 )
	set_color_shader( 2 )
	set_color_shader( 3 )
end
