function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local colors = {}
	local ui = self.ui
	ui.colors = colors
	local i = 1
	for ix=1,9,8 do
		for y=1,8 do
			local DY = 1.7
			local iy = 1 + (y-1) * DY
			colors[i] = self:add_rgbfa(	{ix,iy,	8,DY-.01},	"Col_"..i, false )
			i = i + 1
		end
	end
end

function meu:get_color_bu( id )	return self.ui.colors[id] end
function meu:get_rgba( id )		return self:get_color_bu( id ):get_rgba() end