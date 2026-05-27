local function pix_draw()
	local nb = 512 * aaa.midi.get_control( 1, 1 )

	local dx,dy = .5/nb , .5/nb
--	gol.push_attrib()
		gol.color_green()
		local f = .5*aaa.midi.get_control( 1, 2 )
		local x,y
		local gx,gy
		for i = 0, 1, 1/nb do
			for j = 0, 1, 1/nb do
				--c = aaa.img.get_component_uv( 62, (e.x/6)+.5, (e.y/6)+.5, 5, 0, 1 )
				--gx,gy = aaa.img.get_gradient_uv_ellipse( 62, i, j, .02, .02, 12, 1, 1, 1 )--
				gx,gy = aaa.img.get_gradient_uv( 62, i, j, 5, 1, 0 )

				--gol.color( c, 1, 1, 1 )
				x = i*6-3
				y = j*6-3
				--aaa.draw_rect( e.x-dx +gx, e.y-dx+gy, e.x+dx+gx, e.y+dy+gy )

				if not (gx == 0 and gy == 0) then
					--gol.color_green()
					gx = gx*f
					gy = gy*f
					aaa.draw_line_uv( x, y, gx , gy)
				else
					--gol.color_red()
					--aaa.draw_rect( x-dx, y-dy, x+dx, y+dy )
				end
			end
		end
--	gol.pop_attrib()

	aaa.layers.skip_rest()
end
pix_draw()