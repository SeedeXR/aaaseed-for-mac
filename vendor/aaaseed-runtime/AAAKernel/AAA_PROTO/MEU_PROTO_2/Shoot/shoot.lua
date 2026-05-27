
function meu:define_ui()
	--local ref = self.ref
	--local ui = self.ui
	local ix, iy = 1, 1

	self:add_camera()
	self:add_bu_texture_target_unit( {ix, iy, nil, 2.5 }, "Src", 1 )

	--ui.bu_cover = self:add_text_info(	{1,1,	8,1}, "Cover" )
end

function meu:init()
	--local ref = self.ref
end


--function meu:update()
--	local ref = self.ref
--end