function meu:define_meu_infos( )
	return { author = "Mâa", date="2024",
			tags = { "2d", "Core", "Experimental", "ImageProcessing", "Procedural", "Tutorial" },
			help = "move blob from a force field and detect interactions"
			 }
end

-- add size and bind to the ui
--todo merge with KNF 
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	self:add_camera()
	--error ? self:add_rendering( 	{1,14,	8,2} )

	local ix,iy = 1,1
	local sy = .8
	local dy = .2
	local syc = sy*1.5

	--todo make a generic verbose selector at the MEU level
	--todo deal with verbose and b_verbose
	self:add_trig_method(	{ix,iy,			3, sy},	"Restart",		self, "restart" )
	ui.bu_nb =
		self:add_text_info(	{ix+3,iy,		3,sy},	"None" )
	iy = iy + sy + dy

	self:add_button(		{ix,iy,			sy,sy},	"Track",		self, "b_blob_tracking",		true )
	iy = iy + sy + dy

	self:add_button(		{ix,iy,			sy,sy},	"Verbose",		self, "b_verbose",		true )
	self:add_button(		{ix+4,iy,		sy,sy},	"Show",			self, "b_show",			true )
	iy = iy + sy + dy*2
	bu = self:add_selector(	{ix,iy,			8,sy},	"Draw_Src" )
		bu:set_nb( 3 )
		bu:set_item_text( 1, "No", "Back", "Front" )
		bu:set_target_lua( self, "s_draw_src" )
	iy = iy + sy
	self:add_rgbfa(			{ix,iy,			8,syc}, "Capture",		aaa.obj.get_down_by_class(self:get_layer(5),"color") )
	iy = iy + syc + dy

	self:add_button(		{ix,iy, 		sy,sy},	"Draw_Contour",	self, "b_draw_contour",	true )
	iy = iy + sy + dy

	self:add_slider(		{ix,iy,			4,sy},	"Offset_x",		self, "g_offset_x",		0, -4,4 ):set_color_back( "x" )
	self:add_slider(		{ix+4,iy,		4,sy},	"Offset_Y",		self, "g_offset_y",		0, -4,4 ):set_color_back( "y" )
	--self:add_slider(		{ix,iy+1,		8,1},	"Offset_zoom",	self, "offset_zoom",	0, -4,4 )
	iy = iy + sy
	self:add_slider(		{ix,iy,			4,sy},	"Scale_x",		self, "g_scale_x",		1, .1,2 ):set_color_back( "x" )
	self:add_slider(		{ix+4,iy,		4,sy},	"Scale_y",		self, "g_scale_y",		1, .1,2 ):set_color_back( "y" )
	iy = iy + sy + dy

	self:add_button(		{ix,iy, 		3,sy},	"Draw_rond",	self, "b_draw_rond",	true )
	self:add_button(		{ix+1,iy+sy, 	2,sy},	"Cercle",		self, "b_rond_cercle",	true )
	self:add_slider(		{ix+1,iy+sy*2,	3,sy},	"Radius",		self, "rond_radius",	1, 0,1 )
--		bu = self:add_selector(	{ix,iy,	8,1}, "Type" )
--		bu:set_nb( 9 )
--		bu:set_target_lua( self, "id_bind")
	local dy_tex = 4
	self:add_bu_texture(	{ix+4,iy,		4,dy_tex},	"Rond", 2	)
	iy = iy + dy_tex
	bu = self:add_selector(	{ix,iy,			8,syc},		"Color_rond" )
		bu:set_nb( 4, 2 )
		bu:set_item_text( 1, "Violet", "Bleu", "Vert", "Jaune", "Rose", "Rouge", "All", "This" )
		bu:set_target_lua( self, "s_rond_color" )
	iy = iy + syc
	bu = self:add_rgbfa(	{ix,iy,			8,syc},			"Rond_rgb", false )
		self.ui.bu_rgba = bu

	ix,iy = 9,3

	self:add_button(		{ix,iy, 		sy,sy},		"Polyline",			self, "b_polyline",	true )
	iy = iy + sy + dy

	self:add_slider(		{ix,iy,			8,sy},		"phy_min",			self, "phy_min",		0, 0,.2 )
	self:add_slider(		{ix,iy+sy,		8,sy},		"Anticipation",		self, "anti_factor",	5, 0,40 )
	self:add_slider(		{ix,iy+sy*2,	8,sy},		"Anticipation Limit",self, "anti_limit",		1, 0,20 )
	iy = iy + sy*3 + dy

	self:add_button(		{ix,iy,			sy,sy},		"Draw_blob",		self, "b_draw_blob",	false )
	bu = self:add_selector(	{ix+sy,iy+sy,	8-sy,sy},	"Rect" )
		bu:set_nb( 3 )
		bu:set_item_text( 1, "No", "Raw Rect", "Projected" )
		bu:set_target_lua( self, "s_blob_rect" )
		bu:set_text_visible( false )
	self:add_button(		{ix+sy,	iy+sy*2,	sy,sy},		"phy",				self, "b_draw_phy",		false )
	self:add_button(		{ix+sy,	iy+sy*3,	sy,sy},		"Id",				self, "b_draw_id",		false )
	iy = iy + sy*4 + dy

	iy = iy + sy
	local osc = self.osc
	self:add_button(		{ix,iy, 		2,sy},	"Send",			self, "b_send",		false )
	self:add_button(		{ix+2,iy, 		4,sy},	"Osc Verbose",	osc, "b_verbose",	true )

	iy = iy + sy
	self:add_slider(		{ix,iy,			4,sy},	"Send Offset_x",	osc, "offset_x",	0, -4,4 ):set_color_back( "x" ):set_text("Offset X") 
	self:add_slider(		{ix+4,iy,		4,sy},	"Send Offset_Y",	osc, "offset_y",	0, -4,4 ):set_color_back( "y" ):set_text("Offset Y") 

	iy = iy + sy
	self:add_slider(		{ix,iy,			4,sy},	"Send Scale_x",		osc, "scale_x",		1, .1,2 ):set_color_back( "x" ):set_text("Scale X") 
	self:add_slider(		{ix+4,iy,		4,sy},	"Send Scale_y",		osc, "scale_y",		1, .1,2 ):set_color_back( "y" ):set_text("Scale Y") 
	iy = iy + sy + dy
end

function meu:init()
	local ref = self.ref
	ref.bdd_contour		=	self:get_layer_bdd(3)
		ref.draw_contour	=	param.get_ref( ref.bdd_contour,	"contour_draw"	)

	ref.bdd_track	=	self:get_layer_bdd(4)
		ref.blob_nb	=	param.get_ref( ref.bdd_track,	"blob_nb"		)
		ref.restart =	param.get_ref( ref.bdd_track,	"restart_trig"	)

	self:init_blobs()
	self:init_osc()
end

function meu:init_blobs()
	local ref = self.ref
	param.set( ref.restart, true )
	local blobs = {}
	blobs.curs	= {	blob_nb=0,	by_id={},	touch_nb=0, touch={},	untouch_nb=0, untouch={}, 	}
	blobs.prevs	= {	blob_nb=0,	by_id={},	touch_nb=0, touch={},	untouch_nb=0, untouch={}, 	}
	self.blobs = blobs
end

function meu:restart()
	self:init_blobs()
end

--todotokyoafter
--generalize there is already BLOB and BLOBS GABU_OBJ
function meu:update_blobs()
	local blobs = self.blobs
	blobs.meu_detect = self
	--swap
	local curs,prevs = blobs.prevs,blobs.curs
	blobs.curs,blobs.prevs = curs,prevs
	self:read_blobs( self.ref.bdd_track, curs,prevs )
	app.blobs = blobs

end

-- we should generalize this in a BLOB Fn where we pass the parameters in a table (add ox,oy, sx,sy)
function meu:read_blobs( bdd, curs, prevs )

	--todo check the case where blob are identical (partially same frame)
	--self:print( "read_blob "..b_mouse.." "..b_mouse_no )
	local tmp_nb 	= aaa.bdd.get_blob_nb( bdd )

	local dt = aaa.time.dt
	local nb = 0

	curs.touch = {}
	curs.touch_nb = 0
	curs.untouch = {}
	curs.untouch_nb = 0
	curs.by_id = {}

	local curs_by_id = {}
	local prevs_by_id = prevs.by_id

	if tmp_nb > 0 then
		-- anti for anticipation
		local anti_f = self.anti_factor

		--todo more refined in loop
		--tmp_nb = math.min( tmp_nb, self.blob_nb_max )
		--local frame = param.get( gref.frame_counter )	

		--self:print( l.." "..b.." ------------- "..r.." "..t )	--

		local get_id_xy_sxy = aaa.bdd.get_blob_id_xy_sxy
		for i = 1, tmp_nb do

			local id, x,y, sx,sy = get_id_xy_sxy( bdd, i )
			--self:print( blob.id )
			--	take in account the viewport
			local b = curs[i]
			--we reuse the table to avoid erasing it
			-- but blob don't circulate from prev to cur so don't store stuff in
			if not b then
				b = {}
				curs[i] = b
			end
			nb = nb + 1

			if self.b_polyline then
				b.line = b.line or {}
				b.line_point_nb = aaa.bdd.get_blob_polyline_to_table( b.line, bdd, i )
				--table.print( b.line, "line", 2 )
			end
			
			b.id = id
			b.x_raw,b.y_raw = x,y
			b.sx_raw,b.sy_raw = sx,sy
			curs_by_id[id] = b

			local prev = prevs_by_id[id]
			if prev then --prev then
				prevs_by_id[id] = nil	-- remove from table so the left over will be the untouch
										-- could be eventually stored for while in a ghost structure (see bdd_blob_tracking )
				b.data = prev.data
				--todo x_raw used for calage should be refined (which transfo it go thru like mouse coor in C
				--local inter = dt*2
				local inter = .5
				b.x_filt = interpolate( prev.x_filt, x, inter )
				b.y_filt = interpolate( prev.y_filt, y, inter )

				if dt > 0 then
					b.vx_filt = (b.x_filt - prev.x_filt)/dt
					b.vy_filt = (b.y_filt - prev.y_filt)/dt
				else
					b.vx_filt = 0
					b.vy_filt = 0
				end
				inter = dt
				b.vx_filt = interpolate( prev.vx_filt, b.vx_filt, inter )
				b.vy_filt = interpolate( prev.vy_filt, b.vy_filt, inter )

				inter = dt*10
				b.x1 = interpolate( prev.x1, b.x_filt, inter )
				b.y1 = interpolate( prev.y1, b.y_filt, inter )

				b.x2 = interpolate( prev.x2, b.x_filt, inter * .5 )
				b.y2 = interpolate( prev.y2, b.y_filt, inter * .5 )

				inter = .1
				b.sx_filt = interpolate( prev.sx_filt, sx, inter )
				b.sy_filt = interpolate( prev.sy_filt, sy, inter )

				local dx = b.x1 - b.x2
				local dy = b.y1 - b.y2
				local d = math.sqrt( dx*dx + dy*dy )
				if d < self.phy_min then
					b.d, b.dx, b.dy = prev.d, prev.dx, prev.dy
				else
					local f
					if d * anti_f > self.anti_limit then
						f = self.anti_limit / d
					else
						f = anti_f
					end
					b.d, b.dx, b.dy = d*f, dx*f, dy*f
				end
				b.x_anti,b.y_anti = x+dx,y+dy
				b.ph = prev.ph + aaa.time.dt * b.d
				b.t = prev.t + aaa.time.dt
			else
				b.data = {}
				--y = 3
				b.x_filt,b.y_filt = x,y
				b.vx_filt, b.vy_filt = 0,0
				b.x_anti,b.y_anti = x,y

				b.x1, b.y1 = x,y
				b.x2, b.y2 = x,y
				b.dx, b.dy = 0,0
				b.sx_filt, b.sy_filt = sx,sy	-- so they appear small
				b.d = 0
				b.ph = 0
				b.t = 0

				if self.b_show or self.b_verbose then
					local str = id.."     TOUCH "
					if self.b_verbose	then self:print(str)					end
					if self.b_show		then aaa.show(  str, str, nil, 2. )	end
				end

				curs.touch[id] = b
				curs.touch_nb = curs.touch_nb  + 1
			end

			-- if i==1 then
			-- 	aaa.show(  b.x_filt, "x_filt", nil, 2. )
			-- end
		end
	end
	--if nb ~= self.blob_nb then
	--	self:print( "blob_nb changed to "..nb.." from "..self.blob_nb )
	curs.blob_nb = nb
	curs.by_id = curs_by_id


	if self.verbose >= 2 then self:print( "blob_tracked "..nb ) end

	--end
	if self.b_show then
		aaa.show( curs.blob_nb, "blob_tracked" )
		aaa.show( curs.touch_nb, "touch_nb" )
	end

	--eo.send_img( "aaa.set( \"tube_lr\","..bx..")" )
	--eo.send_img( "aaa.set( \"tube_du\","..by..")" )

	for id,b in PAIRS(prevs_by_id) do
		if self.b_show or self.b_verbose then
			local str = id.." UNTOUCH "
			if self.b_verbose	then self:print( str )				end
			if self.b_show		then aaa.show(  str, str, nil, 2. )	end
		end
		curs.untouch[id] = b
		curs.untouch_nb = curs.untouch_nb  + 1
	end

	if self.b_show then
		aaa.show( curs.untouch_nb, "untouch_nb" )
	end

	prevs.by_id = {}	--secu to be 100% sure table don't crazy grow
end

function meu:init_osc()
	self.osc = {}
	local osc = self.osc
	osc.count = 0
	osc.b_verbose = false
	osc.b_send = true
	osc.b_received = true
	osc.b_bundle = true
	osc.dst = 2
end

function meu:send_osc( what, ... )
	local osc = self.osc
	if not osc then return end

	osc.count = osc.count + 1
	local tag = "/"..what
	if osc.b_verbose then
		--self:print( "end_osc() "..eo.osc.count )
		if osc.b_send then
			aaa.print( osc.count.." : osc_send( "..tag..string:make_from_list( ... ).." )" )
		else
			aaa.print( "DONT : osc_send( "..tag..string:make_from_list( ... ).." )" )
		end
	end

	if osc.b_send then
		if osc.b_bundle then
			aaa.net.osc_send( osc.dst, tag, ... )
		else
			aaa.net.osc_send_no_bundle( osc.dst, tag, ... )
		end
	end
	--aaa.sleep( osc_delay )
end


function meu:send_blobs_ref( blobs )
	--	if true then return end
	local osc = self.osc
	local ox,oy, fx,fy = osc.offset_x,osc.offset_y,osc.scale_x,osc.scale_y
	local nb = blobs.blob_nb
	self:send_osc( "blobs/nb", nb )
	for i=1,nb do
		local b = blobs[i]
		--table.print( b )
		local x,y, sx,sy = b.x_raw, b.y_raw, b.sx_raw, b.sy_raw
		--local x,y, sx,sy = b.x_filt, b.y_filt, b.sx_filt, b.sy_filt
		x = (x + ox) * fx
		y = (y + oy) * fy
		sx = sx * fx
		sy = sy * fy
		--self:print( "blob "..id.." -> "..x..","..y )
		self:send_osc( "blobs/blob", b.id, x,y, sx,sy )
	end
end

function meu:send_blobs( blobs )
	--	if true then return end
	local osc = self.osc
	local ox,oy, fx,fy = osc.offset_x,osc.offset_y,osc.scale_x,osc.scale_y
	local nb = blobs.blob_nb
	local t = {nb} 
	local index = 2
	for i=1,nb do
		local b = blobs[i]
		--table.print( b )
		--local x,y, sx,sy = b.x_raw, b.y_raw, b.sx_raw, b.sy_raw
		local x,y, sx,sy = b.x_filt, b.y_filt, b.sx_filt, b.sy_filt
		x = (x + ox) * fx
		y = (y + oy) * fy
		sx = sx * fx
		sy = sy * fy
		t[index] =  b.id
		index = index + 1
		t[index] =  x
		index = index + 1
		t[index] =  y
		index = index + 1
		t[index] =  sx
		index = index + 1
		t[index] =  sy
		index = index + 1
	end
	self:send_osc( "blobs", unpack(t) )
end

function meu:update()
	self:update_blobs()

	local ref = self.ref
	param.set( ref.draw_contour, self.b_draw_contour )

	if self.b_send then
		self:send_blobs( self.blobs.curs )
	end
end

function meu:update_ui()
	local ref = self.ref
	local ui = self.ui
	ui.bu_nb:set_text( "blobs : "..param.get( ref.blob_nb ) )
end


function meu:draw_blobs( blobs )
--	if true then return end
	gol.reset()

	gol.set_line_width( 2 )

	local alpha = self:get_alpha()
	local f = 5
	local r = .75
	--local binds = { 128, 129, 130, 38, 39, 40, 41, 70, 71 }
	local bind = self:get_texture_bind_2d( 2 )

	--self:print( bind )
	--gol.color_red()
	if self.s_rond_color ~= 7 then
		if self.s_rond_color == 8 then
			local r,g,b,a = self.ui.bu_rgba:get_rgba()
			gol.color4( r,g,b, a*alpha )
		else
			local r,g,b = LV:get_rgb( self.s_rond_color )
			gol.color4( r,g,b, alpha )
		end
	end

	local function set_color( id )
		local r,g,b = LV:get_rgb( id % 6 + 1 )
		gol.color4( r,g,b, self:get_alpha() )
	end

	local ox,oy, fx,fy = self.g_offset_x,self.g_offset_y, self.g_scale_x,self.g_scale_y
	for id=1,blobs.blob_nb do
		local b = blobs[id]
		--table.print( b )
		local x,y = b.x_filt + b.dx, b.y_filt + b.dy
		x = (x + ox) * fx
		y = (y + oy) * fy
		if self.b_draw_blob then
			gol.color_green()
			if self.b_draw_phy then
				aaa.draw_null( b.x_filt,b.y_filt, 0, .2 )
				aaa.draw_null( b.x1,b.y1, 0, .3 )
				aaa.draw_null( b.x2,b.y2, 0, .3 )
				gol.draw_lines_2d( b.x_filt,b.y_filt, x,y )
			end

			local sx,sy
			if self.s_blob_rect > 1 then
				if self.s_blob_rect == 2 then
					local x,y = b.x_raw,b.y_raw
					x = (x + ox) * fx
					y = (y + oy) * fy
					sx,sy = b.sx_raw/2,b.sy_raw/2
					sx = sx * fx
					sy = sy * fy
					aaa.draw_rect_line( x-sx,y-sy, x+sx, y+sy )
				else
					sx,sy = b.sx_filt/2, b.sy_filt/2
					sx = sx * fx
					sy = sy * fy
					aaa.draw_rect_line( x-sx,y-sy, x+sx,y+sy )
				end
			end

			if self.b_draw_id then
				aaa.draw_str_xy( b.id, x-sx,y+sy, .25,.25 )
			end
		end
		if self.b_draw_rond then
			if self.s_rond_color == 7 then
				set_color( b.id % 5 + 1 )
			end
			--gol.color_white()
			r = self.rond_radius
			--gol.set_mask_color( false, true, false, false )
		--	aaa.draw_bind_rect( 101, x-r, y-r, x+r, y+r )
			if self.b_rond_cercle then
				aaa.draw_bind_rect( bind, x-r, y-r, x+r, y+r )
			else
				local sx,sy = b.sx_filt/2, b.sy_filt/2
				sx = sx * fx * r
				sy = sy * fy * r
				aaa.draw_bind_rect( bind, x-sx, y-sy, x+sx, y+sy )
			end
			
		end
		--gol.set_mask_color( true, true, true, true )
	end
end

function meu:draw_public()
	self:draw_layer( 5 )
end

function meu:draw_xy( t )
	gol.reset()
	gol.set_line_width( 2 )
	gol.color_green()

	for id=1,#t do
		local b = t[id]
		local x,y = b.x,b.y
		aaa.draw_null( x,y, 0, .2 )

		local s = .1
		local sb = s
		aaa.draw_triangle_strip_4xy_uv(
			x-s,	y-s,
			x+s,	y-s,
			x-sb,	y+s,
			x+sb,	y+s

		)
	end

	if false then
		gol.set_line_width( 1 )
		gol.color_cyan( .4 	)
		aaa.draw_lines_vert_nb( 9, -1, .25, 	-1, 1 )
		aaa.draw_lines_hori_nb( 9, -1, .25,		-1, 1 )
	end
end

function meu:draw()
	local ref = self.ref
	
	self:draw_layers_begin()
		self:draw_layer( 1 )	--attribute

		if self.s_draw_src == 2 then
			self:draw_public()
			self:draw_layer( 1 )	--attribute
		end

		self:draw_layer( 3 )	--image contour

		if self.b_blob_tracking then
			self:draw_layer( 4 )	--blob tracking

	--		gol.scale( self.g_scale)

			--gol.set_texture_dim(2)
			self:draw_blobs( self.blobs.curs )
		end

		--self:print( self.s_draw_src )
		if self.s_draw_src == 3 then
			self:draw_public()
		end


		--local xy = BLOBS:read_bdd_in_array_xy( ref.bdd_contour )
		--self:draw_xy( xy )

	self:draw_layers_end()
end
