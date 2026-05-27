--todo ndc / fbo is still ambigious (2022 August)

-- add monitor, a draw button except if asked no, and shading_ui or not
-- shader_str can be "no", "before" (nil the same), "after"
function MEU:__define_ui_ndc_low( rect_monitor, b_no_draw_button, shader_str )
	local ix,iy, sx,sy = self:unpack_rect_using_def( rect_monitor, {1,1, 8,4} )
	local SY = .8
--	local DY = .2

	shader_str = shader_str or "before"

-- ADD SHADER UI MIN
	local function define_shader()
		self:add_shading_ui( {ix,iy,	sx,SY} )
		iy = iy + SY
	end
	
-- ADD MONITOR WITH THE RIGHT BIND
	local function define_monitor()
--		self:print( "SY is "..SY)
--todo extend Monitor to deal with the whole FBO
		local bind = rect_monitor and rect_monitor.bind
		if not bind then
			local name = self:get_meu_fbo_name_ask()
			if name then
				bind = self:get_bind_by_name( self:get_meu_fbo_name_ask() )
			end
		end
		--self:box_debug( "bind is "..bind )
		local bu = self:add_monitor( {1+8-sx,iy, sx,sy}, "Ndc_monitor", bind )
		self.ui.bu_ndc_monitor = bu
		if bind then
			self:update_ui_ndc_bind( bind )
		end
		iy = iy + sy
	end

	if shader_str   == "before" then define_shader()  end
	if rect_monitor ~= false    then define_monitor() end
	if shader_str   == "after"  then define_shader()  end

-- ADD EVENTUALLY A DRAW BUTTON
--todo extend to all MEU
	if not b_no_draw_button then
		local SY = .8
		self:add_button(	{ix,iy,		SY,SY },	"Draw",		self, "b_draw",		false )
		iy = iy + SY
	end
	return ix,iy
end

function MEU:define_ui_ndc(          false_or_rect_monitor, b_no_draw_button, shader_str )
	return self:__define_ui_ndc_low( false_or_rect_monitor, b_no_draw_button, shader_str )
end

function MEU:define_ndc_src( rect, names, b_preset_use )
	local nb = #names
	rect = self:get_rect_using_def( rect, {9,2.5,	8,nb*3} )
	rect[4] = rect[4] / nb
	rect.b_compact = true
	for i=1,nb do
		local name = names[i]
		if name ~= false then
			self:add_bu_texture_target_unit( rect, name,		i, false	)
			if b_preset_use ~= nil then
				self:set_bu_texture_preset_use( i, b_preset_use )
			end
			rect[2] = rect[2] + rect[4]
		end
	end
end

function MEU:update_ui_ndc_bind( bind )
	--bind = self:get_bind_by_name( self:get_meu_fbo_name_ask() )
	self:set_icon_bind( bind )
	local mon = self.ui.bu_ndc_monitor
	if mon then
		mon:set_texture_bind_2d( bind )
	end
end