function meu:define_ui()
	local ref = self.ref
	local bu

	self:add_camera()

end

function meu:draw()
	self:draw_layers_begin()

		self:draw_layers()
		local SXH = 5
		for y=-SXH,SXH,1/2-5/32 do
			gol.begin_line_strip()
			for x=-SXH,SXH,1/256 do
	--			local v=wrap( x, .2, 1. )
	--			local v=triangle( x, 0, 1.5 )
	--			local v=triangle_01( x*52 )
	--			local v=triangle_01( x*2 )
				local v = quantize( triangle_01(x)+1/8, 1/4 )
	--			v = quantize( v*v, 1/4 )
				gol.vertex( x, y+v, 0 )
			end
			gol.do_end()
		end

	self:draw_layers_end()
end
