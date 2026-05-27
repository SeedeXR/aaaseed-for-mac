
function meu:define_ui()
	local ref = self.ref
	local bu
	local ix = 9
	local iy = 3

	self:add_camera( nil, 16 )


	self:add_button(	{	ix,	iy },		"Draw", 		self, "b_draw",	false	)
	iy = iy + 1
	self:add_button(	{	ix+1,	iy },		"Boxes", 		self, "b_boxes",	false	)
	iy = iy + 1

end


function meu:draw()
	MEU.draw( self )
	if self.b_draw then
		gol.reset()
		gol.color_cyan(1)
		--aaa.draw_null( 0, 0, 0, 1 )

		local d = 2.10
		local x,y,z = 0,0,0
		local sx,sy,sz = .45, .65, .29
		if self.b_boxes then
			aaa.draw_box_top_line( sx,sy,sz, -d/2-sx/2, sy/2, sz/2+.53   )
			aaa.draw_box_top_line( sx,sy,sz,  d/2+sx/2, sy/2, sz/2+.53   )
			local S = .03
			aaa.draw_box_top_line( 2.38,S,S,  0, sy+S/2, .53+S/2   )
		end
		local sx,sy = 5.68, 2.26
		local oy = .3
		local d = .03
		local l,r = -sx/2,sx/2
		aaa.draw_rect_line_at_z( l, oy, r, oy+sy, -d )
		aaa.draw_rect_line_at_z( l, oy, r, oy+sy, d )
		aaa.draw_rect_line_at_z( l, 0, r, 0, 0 )
		aaa.draw_rect_line_at_z( l, 0, r, 0, d )
		gol.color_cyan(.5)
		aaa.draw_rect_line_at_z( l, oy, r, oy+sy, d+.05 )

		--aaa.draw_rect_line_at_z( l, 0, r, 3, d )
	end
end