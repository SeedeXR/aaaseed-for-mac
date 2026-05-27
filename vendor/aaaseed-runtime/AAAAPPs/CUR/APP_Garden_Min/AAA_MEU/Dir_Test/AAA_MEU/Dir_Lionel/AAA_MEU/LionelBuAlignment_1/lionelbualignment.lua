--2023 July Bu aligment is buggy in draw for vertical
-- and fucked up eksewhere: inside teste for example
-- so maa commented for now
function meu:define_ui()
	local ref = self.ref

	local bu
	--2023 July Maa commented implementation of alignment is buggy and incomplete
	if false then
		--local bus = BUS:create( "BU ALIGNMENT" )

		local bus = self:get_bus()
		--local bup = bus:get_bup()
		--		bup:set_lbrt( { -4, 2.3, 2.5, 2.4 } )
		--local s = .5
		--local bup = bus:add_bup( {-s,0, -s*2,-s*2} )

		--bup:set_size( .9, .15 )
		local ox,oy = 0,0

		local nb = 3
		local sx = 1/3
		local dx = sx /(nb +2)
		local l = ox-sx/2
		local r = ox+sx/2
		local c = ox+0
		local y = oy+.3
		local sy = .1
		for i=1,nb do
			bu = bus:add_slider( " right", {	 l, y, sx, sy  } )
			bu:set_align_x( "right" )
			bu = bus:add_slider( "left "..i, 	r, y, sx, sy  )
			bu:set_align_x( "left" )
			bu = bus:add_slider( "x"..i,		c, y, sx, sy )
			bu:set_align_x( "center" )
			y = y - sy
			sx = sx - dx
		end

		local x = .3
		local sy = 1/3
		local sx = .1
		for i=1,nb do
			bu = bus:add_slider( " bottom", {	 	x, 0, sx, sy  } )
			bu:set_align_y( "bottom" )
			-- bu = bus:add_slider( "top "..i, 	x, r, sy, sx  )
			-- bu:set_align_y( "top" )
			-- bu = bus:add_slider( "y"..i,		x, c, sy, sx )
			-- bu:set_align_y( "center" )
			x = x - sx
			sy = sy - dx
		end
			-- bu = bus:add_slider( "b1", {	0.5, -1.5, 0.5, 0.2 } )
			-- bu:set_align_y( "bottom" )
			-- bu = bus:add_slider( "b2", {	0.8, -1.5, 0.6, 0.1 } )
			-- bu:set_align_y( "bottom" )
			-- bu = bus:add_slider( "b3", {	0.5, -1.5, 0.3, 0.2 } )
			-- bu:set_align_y( "bottom" )

			-- bu = bus:add_slider( "t1", {	0.5, -1, 0.5, 0.2 } )
			-- bu:set_align_y( "top" )
			-- bu = bus:add_slider( "t2", {	0.8, -1, 0.6, 0.1 } )
			-- bu:set_align_y( "top" )
			-- bu = bus:add_slider( "t3", {	0.5, -1, 0.3, 0.2 } )
			-- bu:set_align_y( "top" )

			-- bu = bus:add_slider( "y1", {	0.5, -2, 0.5, 0.2 } )
			-- bu = bus:add_slider( "y2", {	0.8, -2, 0.6, 0.1 } )
			-- bu = bus:add_slider( "y3", {	0.5, -2, 0.3, 0.2 } )
	end

	--]]
end
