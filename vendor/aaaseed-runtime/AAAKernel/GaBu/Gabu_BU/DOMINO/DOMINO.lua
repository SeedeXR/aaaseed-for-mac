-- was use for an ARte TV show production
-- domino only the top one play
aaa.lua.global.declare_table( "DOMINO" )

function DOMINO.create( noms, flip )
	if not bus_cur.domino then bus_cur.domino = {} end
	if not bus_cur.domino[bus_cur:get_page()] then bus_cur.domino[bus_cur:get_page()] = {} end

	local nb = #noms
	local yt = 0.5
	local yb = - 2.5
	local dy = - (yt-yb) / (nb)
	local xl = 2.2
	local xr = 3.1
	local x = xr
	local dx = (xl-xr) / nb
	local y = yt
	local bu
	local t = {}
	local s_max = 0

	for i = 1, nb do
		--todo find video size
		bu = bus_cur:add_video( noms[i], {x,y, nil,.8} )
		if bu then
			t[i] = bu -- memorize this batch
			s_max = math.max( s_max, bu:get_pos_orig_sx() )
			bu:set_video_button( true )
			bu.domino = 1
			bu:set_texture_flip_u( flip )
			y = y + dy
			x = x + dx
			if x < xl then
				x = x + (xr-xl)
			end
			bu:set_method_on_click_double( bu, "do_flip_full_page_and_mini" )
		end
	end
	--aaa.print( "max domino size : "..s_max )
	for i=1,nb do
		bu = t[i]
		--aaa.print( "offset for domino "..i.." : "..(s_max - bu:get_pos_orig_sx() ) )
		if bu then
			bu:set_x_all( bu:get_pos_orig_x() - (s_max - bu:get_pos_orig_sx()) * .5 )
		end
	end
end

function BU:set_full_page_bis()
	local ratio_cam = bus_cur.cam:get_ratio_x()
	local sx = bus_cur.__page_sx
	local sy = sx / ratio_cam
	self:print( sx.." "..sy )

	local sx_ori,sy_ori = self:get_pos_orig_sxy()
	local ratio = sx_ori / sy_ori
	if ratio < ratio_cam then	-- we adjust on heigth
		sx = sy * ratio
	else
		sy = sx / ratio
	end

	local x_ori,	y_ori	=	self:get_pos_orig_xy()
	local x,		y		=	self:get_xy()
	self:print( x.." "..x_ori )
	if x == 0 or y== 0 then
		self:print( "goto full" )
		self:set_xy(	0,		0		)
		self:set_sxy(	sx,		sy		)
	else
		self:print( "back to orig"  )
		self:set_xy(	x_ori,	y_ori	)
		self:set_sxy(	sx_ori,	sy_ori	)
	end
	self:set_angle( 0 )
	self:set_mobile( false )
end

function DOMINO.create_for_kinect( noms, flip )
	if not bus_cur.domino then bus_cur.domino = {} end
	if not bus_cur.domino[bus_cur:get_page()] then bus_cur.domino[bus_cur:get_page()] = {} end

	local nb = #noms
	local yt = 0.5
	local yb = - 2.5
	local dy = - (yt-yb) / (nb)
	local xl = 2.2
	local xr = 3.1
	local x = xr
	local dx = (xl-xr) / nb
	local y = yt
	local bu
	local t = {}
	local s_max = 0

	for i=1,nb do
		--todo find video size
		bu = bus_cur:add_video( noms[i], {x,y, nil,.8} )
		if bu then
			t[i] = bu -- memorize this batch
			s_max = math.max( s_max, bu:get_pos_orig_sx() )
			--bu:set_video_button(true)
			bu.domino = 1
			bu:set_texture_flip_u( flip )
			bu:set_mobile( false )
			bu:set_method_on_click( bu, "set_full_page_bis" )
			y = y + dy
			x = x + dx
			if x < xl then
				x = x + (xr-xl)
			end
		end
	end
--[[
	--aaa.print( "max domino size : "..s_max )
	for i=1,nb do
		bu = t[i]
		--aaa.print( "offset for domino "..i.." : "..(s_max - bu:get_pos_orig_sx() ) )
		if bu then
			bu:set_x_all( bu:get_pos_orig_x() - (s_max - bu:get_pos_orig_sx()) * .5 )
		end
	end
--]]
end

function DOMINO.create_img( dir, noms, flip, b_auto )
	if not bus_cur.domino then bus_cur.domino = {} end
	if not bus_cur.domino[bus_cur:get_page()] then bus_cur.domino[bus_cur:get_page()] = {} end
	local nb = #noms
	local yt = 0.5
	local yb = - 1.5
	local over_nb = 1 / math.max((nb-1),1)
	local dy = - (yt-yb) * over_nb
	local xl = 2.5
	local xr = 3.1
	local x = xl
	local dx = (xr-xl) * over_nb
	local y = yt
	local bu
	local t = {}
	local s_max = 0
	for i=1,nb do
		bu = bus_cur:add_image( noms[i], {x,y, .8*1.77,.8}, dir..noms[i], b_auto )
		t[i] = bu -- memorize this batch
		s_max = math.max( s_max, bu:get_pos_orig_sx() )
		--aaa.print( "max after domino "..i.." : "..s_max )
		--bu:set_video_button(true)
		bu.domino = 1
		bu:set_texture_flip_u( flip )
		y = y + dy
		x = x + dx
		if xr < x then
			x = x - (xr-xl)
		end
	end
	--aaa.print( "max domino size : "..s_max )
	for i=1,nb do
		bu = t[i]
		--aaa.print( "offset for domino "..i.." : "..(s_max - bu:get_pos_orig_sx()) )
		bu:set_x_all( bu:get_pos_orig_x() - (s_max - bu:get_pos_orig_sx()) * .5 )
	end
end
