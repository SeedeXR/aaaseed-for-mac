--
--	compute and draw the 32 screens of the aquarium
--

function AQUA:build_screen_data()
	local scr = self.screen
	local pixel_by_m_x =  (scr.pixel_nb_x / scr.size_x_m)
	local pixel_by_m_y =  (scr.pixel_nb_y / scr.size_y_m)
	scr.pixel_by_m = math.max( pixel_by_m_x, pixel_by_m_y )
	self:print( "pixel by cm : "..pixel_by_m_x.." "..pixel_by_m_y.." -> "..scr.pixel_by_m )

	--	compute screen center of a 13 by 3 wall
	local sr2 = math.sqrt( 2 )
	local sw = scr.size_x_m
	local sh = scr.size_y_m
	local e = scr.dist_m
	scr.dx	=	( sh + e ) * sr2
	scr.dyy	=	( sw + e ) / sr2
	scr.dyx	=	scr.dyy - scr.dx
	self.screen_rect = {}
	local dx	= scr.dx
	local dyx	= scr.dyx
	local dyy	= scr.dyy
	for i=1,13 do
		local cx = (i - 7) * dx
		for j=1,3 do
			local y = (j - 2) * dyy
			local x = cx + (j - 2) * dyx
			--todo	compute rect coor directly on the display side
			--todo	have an offset for inperfections in the wall real setup
			self.screen_rect[ string.char(64+i)..(4-j) ] = { x=x, y=y }
		end
	end

	--	then for each PC find extreme position / display rectangle
	local g_min_x = 0
	local g_min_y = 0
	local g_max_x = 0
	local g_max_y = 0
	local D = ( sw + sh ) / ( 2 * sr2 )
	local pcs = self.pcs
	for ipc = 1, 8 do
		local min_x, max_x = 100, -100
		local min_y, max_y = 100, -100
		local pc	=	pcs:get_pc(ipc)
		local names	=	pc:get_screen_names()
		for i=1,4 do
			local name = names[i]
			local rect = self.screen_rect[ name ]
			local x = rect.x
			local y = rect.y
			pc:set_screen( i, { name=name, x=x, y=y, name_virtual=ipc..i } )
			min_x = math.min( x, min_x )
			min_y = math.min( y, min_y )
			max_x = math.max( x, max_x )
			max_y = math.max( y, max_y )
		end
		pc:set_rect	( {
						x	=	(min_x + max_x) * .5,
						y	=	(min_y + max_y) * .5,
						sx	=	max_x - min_x + D * 2.,
						sy	=	max_y - min_y + D * 2.,
						left	=	min_x - D,
						right	=	max_x + D,
						bottom	=	min_y - D,
						top		=	max_y + D
					} )
		local r = pc:get_rect()
		g_min_x = math.min( g_min_x, r.left )
		g_min_y = math.min( g_min_y, r.bottom )
		g_max_x = math.max( g_max_x, r.right )
		g_max_y = math.max( g_max_y, r.top )
		self:print( "surface pc "..ipc.." : "..(r.sx * r.sy) )
		self:print( "  fbo pc "..ipc.." : "..(scr.pixel_by_m*r.sx).." x "..(scr.pixel_by_m*r.sy) )
	end

	--	compute global center
	local cx = (g_min_x + g_max_x) * .5
	local cy = (g_min_y + g_max_y) * .5

	-- then offset everything
	self.wall_rect = {}
	local r = self.wall_rect
	r.left		= g_min_x - cx
	r.right		= g_max_x - cx
	r.bottom	= g_min_y - cy
	r.top		= g_max_y - cy
	local d = ( sw - sh ) / ( 2 * sr2 )

	for ipc = 1, 8 do
		local pc = self.pcs:get_pc(ipc)
		local r = pc:get_rect()
		r.x			= r.x		-	cx
		r.left		= r.left	-	cx
		r.right		= r.right	-	cx
		r.y			= r.y		-	cy
		r.bottom	= r.bottom	-	cy
		r.top		= r.top		-	cy

		for i=1,4 do
			local screen = pc:get_screen(i)
			screen.x = screen.x - cx
			screen.y = screen.y - cy
			screen.uv = {}
			local uv = screen.uv
			uv[1] = (screen.x - D - r.left) / ( r.right - r.left )
			uv[3] = (screen.x - d - r.left) / ( r.right - r.left )
			uv[5] = (screen.x + d - r.left) / ( r.right - r.left )
			uv[7] = (screen.x + D - r.left) / ( r.right - r.left )

			uv[2] = (screen.y - d - r.bottom) / ( r.top - r.bottom )
			uv[4] = (screen.y - D - r.bottom) / ( r.top - r.bottom )
			uv[6] = (screen.y + D - r.bottom) / ( r.top - r.bottom )
			uv[8] = (screen.y + d - r.bottom) / ( r.top - r.bottom )
		end
	end
end

function AQUA:draw_screens()
	gol.set_default()
	gol.set_line_width( 4. )
--	gol.disable_lighting()

	local ST = .2
	local STH = ST * .5

	local scr	= self.screen
	local sxh	= scr.size_x_m * .5
	local syh	= scr.size_y_m * .5
	for ipc=1,8 do
		local pc = self.pcs:get_pc(ipc)
		gol.color_green()
		for i=1,4 do
			local screen = pc.screen[i]
			--todo compute rect coor directly
			--todo	have an offset for inperfections in the wall real setup
			gol.push_matrix()
				gol.translate( screen.x, screen.y )
				if self.b_draw_screens_nb then
						if self.b_draw_screens_pc then
							gol.color_red()
							aaa.draw_str_maa_xy( screen.name_virtual, -STH, -2.*STH, ST*.5, ST*.5 )
						end
					gol.color_green()
					aaa.draw_str_maa_xy( screen.name, -STH, -STH, ST, ST )
				end
				gol.rotate_z( .125 )
					aaa.draw_rect_line( -sxh, -syh, sxh, syh )
			gol.pop_matrix()
		end
		if self.b_draw_screens_pc then
			gol.color_red()
			gol.begin_line_strip()
				for i = 1, 4 do
					local screen = pc.screen[i]
					gol.vertex2( screen.x, screen.y )
				end
			gol.do_end()
			gol.set_line_width( 16. )
			gol.color_red( .5 )
			gol.begin_line_strip()
				local x, y = pc.screen[1].x, pc.screen[1].y
				gol.vertex2( x, y )
				x = pc.rect.x
				gol.vertex2( x, 1.2 )
			gol.do_end()
			gol.set_line_width( 4. )
			aaa.draw_str_maa_xy( "PC\nD0"..ipc, x-.15, 1.4, ST, ST )
		end
		if self.b_draw_screens_rect then
			gol.color_cyan( .8 )
			local r = pc.rect
			aaa.draw_rect_line( r.left, r.bottom, r.right, r.top )
			gol.draw_lines_2d(	r.left, r.bottom,
								r.right, r.top,
								r.left, r.top,
								r.right, r.bottom
							)
		end
	end
end

function AQUA:draw_4_screen( ipc )
	if not ipc or ipc <= 0 then
		self:print( "draw_4_screen() : ni a SoOuest defined machine" )
		self:print( "\tand not good camera forcing stuff" )
		ipc = 4
	end

	local pc = self.pcs:get_pc(ipc)

	local DX = 8
	local DY = 4.5
	--useful for adjustement on location
	local DDX = DX * 1.
	local DDY = DY * 1.
	local s_id = 0
	for i = 1,-1,-2 do
		for j = -1,1,2 do
			local cx = j * DX
			local cy = i * DY
			--aaa.draw_rect_uv( cx-DX, cy-DY, cx+DX, cy+DY )
			s_id = s_id + 1
			--self:print( pc_id.." "..s_id )
			local uv = pc.screen[s_id].uv
			gol.begin_quad_strip()
				gol.draw_uv_xy(  uv[1],uv[2], cx-DDX,cy+DDY
								,uv[3],uv[4], cx-DDX,cy-DDY
								,uv[5],uv[6], cx+DDX,cy+DDY
								,uv[7],uv[8], cx+DDX,cy-DDY )
			gol.do_end()
		end
	end
end