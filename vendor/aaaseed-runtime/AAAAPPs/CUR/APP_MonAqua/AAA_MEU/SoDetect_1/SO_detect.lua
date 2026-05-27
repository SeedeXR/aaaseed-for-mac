
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	self:add_camera()

	local ix,iy = 1,2
	local sy = 1.
	local dy = .2
	local syc = 1.5
	ui.bu_nb = self:add_text_info(	{ix,iy,	4,sy},			"None" )

	iy = iy + 1
	self:add_slider(	{ix,iy,		4,sy},	"Offset_x",		self, "g_offset_x",		0, -4,4 ):set_color_back( "x" )
	self:add_slider(	{ix+4,iy,	4,sy},	"Offset_Y",		self, "g_offset_y",		0, -4,4 ):set_color_back( "y" )
	--self:add_slider(	{ix,iy+1,	8,1},	"Offset_zoom",	self, "offset_zoom",	0, -4,4 )
	iy = iy + sy
	self:add_slider(	{ix,iy,		4,sy},	"Scale_x",		self, "g_scale_x",		1, .1,2 ):set_color_back( "x" )
	self:add_slider(	{ix+4,iy,	4,sy},	"Scale_y",		self, "g_scale_y",		1, .1,2 ):set_color_back( "y" )
	iy = iy + sy
	self:add_button(	{ix,iy, 	sy,sy},	"Draw_rond",	self, "b_draw_rond",	true )
	self:add_slider(	{ix,iy+sy,	8,sy},	"Radius",		self, "rond_radius",	1, 0,1 )
	iy = iy + sy*2
--		bu = self:add_selector(	{ix,iy,	8,1}, "Type" )
--		bu:set_nb( 9 )
--		bu:set_target_lua( self, "id_bind")
		local dy_tex = 2.5
		self:add_bu_texture_target_unit( {ix,iy, nil, dy_tex},  	"Rond",		1 )
	iy = iy + dy_tex
	bu = self:add_selector(	{ix,iy,	8,1.5},	"Color_rond" )
		bu:set_nb( 4, 2 )
		bu:set_item_text( 1, "Violet", "Bleu", "Vert", "Jaune", "Rose", "Rouge", "All", "This" )
		bu:set_target_lua( self, "s_rond_color" )
	iy = iy + 1.5
	bu = self:add_rgbfa(	{ix,iy,	8,syc}, "Rond_rgb", false )
		self.ui.bu_rgba = bu

	iy = iy + syc + .2
	self:add_button(	{ix, 	iy,			sy,sy},	"Draw_blob",	self, "b_draw_blob",	false )
	self:add_button(	{ix+sy,	iy+sy,		sy,sy},	"phy",			self, "b_draw_phy",		false )
	self:add_button(	{ix+sy,	iy+sy*2,	sy,sy},	"Id",			self, "b_draw_id",		false )

	ix,iy = 9,3
	self:add_slider(	{ix,iy,				8,sy},	"phy_min",				self, "phy_min",		0, 0,.2 )
	self:add_slider(	{ix,iy+sy,			8,sy},	"Anticipation",			self, "anti_factor",	5, 0,40 )
	self:add_slider(	{ix,iy+sy*2,		8,sy},	"Anticipation Limit",	self, "anti_limit",		1, 0,20 )
	iy = iy + sy*3 + dy

	self:add_button(	{ix,iy, 			sy,sy},	"Draw_Capture",		self, "b_draw_capture",	true )
	iy = iy + sy
	self:add_rgbfa(		{ix,iy,				8,syc}, "Capture", aaa.obj.get_down_by_class( self:get_layer(4), "color" ) )
	iy = iy + syc + dy
end

function meu:init()
	local ref = self.ref
	ref.bdd_contour		=	self:get_layer_bdd(2)
	ref.draw_contour	=	param.get_ref( ref.bdd_contour,	"contour_draw"	)

	ref.bdd_track	=	self:get_layer_bdd(3)

	ref.blob_nb	=	param.get_ref( ref.bdd_track,	"blob_nb"	)
	self.blobs_cur = { nb=0 }
	self.blobs_prev = { nb=0 }
end

function meu:draw_blobs( blobs )
--	if true then return end
	gol.reset()

	gol.set_line_width( 2 )

	local alpha = self:get_alpha()
	local f = 5
	local r = .75
	--local binds = { 128, 129, 130, 38, 39, 40, 41, 70, 71 }
	local bind = self:get_texture_bind_2d( 1 )
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

	for id=1,blobs.nb do
		local b = blobs[id]
		local x,y = b.x_filt + b.dx, b.y_filt + b.dy
		if self.s_rond_color == 7 then
			set_color( b.anim_id  )
		end
		x = x + self.g_offset_x
		x = x * self.g_scale_x
		y = y + self.g_offset_y
		y = y * self.g_scale_y
		if self.b_draw_blob then
			gol.color_green()
			if self.b_draw_phy then
				aaa.draw_null( b.x_filt,b.y_filt, 0, .2 )
				aaa.draw_null( b.x1,b.y1, 0, .2 )
				aaa.draw_null( b.x2,b.y2, 0, .2 )
				gol.draw_lines_2d( b.x_filt,b.y_filt, x,y )
			end
			if self.b_draw_id then
				aaa.draw_str_xy( b.id, x,y, .5,.5 )
			end

		end
		if self.b_draw_rond then
			--gol.color_white()
			r = self.rond_radius
			--gol.set_mask_color( false, true, false, false )
		--	aaa.draw_bind_rect( 101, x-r, y-r, x+r, y+r )
			aaa.draw_bind_rect( bind, x-r, y-r, x+r, y+r )
		end
		--gol.set_mask_color( true, true, true, true )
	end
end


function meu:read_blobs( blobs, prevs, ox, oy, fx, fy )
	--self:print( "read_blob "..b_mouse.." "..b_mouse_no )
	local obj		= self.ref.bdd_track
	local tmp_nb 	= aaa.bdd.get_point_nb( obj )

	local dt = aaa.time.dt
	local nb = 0
	--we compute barycenter

	if tmp_nb > 0 then
--[[	--unused
		if not sy then	-- if no parameter passed don't transform coordonates
			ox,oy = 0,0
			fx,fy = 1,1
		end
--]]

		local id, x, y
		local anti_f = self.anti_factor

		--todo more refined in loop
		--tmp_nb = math.min( tmp_nb, self.blob_nb_max )
		--local frame = param.get( gref.frame_counter )

		--self:print( l.." "..b.." ------------- "..r.." "..t )
		--
		local get_xyid = aaa.bdd.get_id_and_point
		for i = 1, tmp_nb do
			id, x, y = get_xyid( obj, i )
			--self:print( blob.id )
			--	take in account the viewport
			local b = blobs[i]
			if not b then
				b = {}
				blobs[i] = b
			end
			nb = nb + 1

			b.id = id
			b.anim_id = b.id % 5 + 1

			local prev
			for j=1,prevs.nb do
				if prevs[j].id == id then
					prev = prevs[j]
					--self:print( "found "..id )
					break
				end
			end

			b.x_raw,b.y_raw = x,y
			if false then --prev then
				--todo x_raw used for calage should be refined (which transfo it go thru like mouse coor in C
				local inter = dt*2
				b.x_filt = interpolate( prev.x_filt, x, inter )
				b.y_filt = interpolate( prev.y_filt, y, inter )

				local inter = dt*10
				b.x1 = interpolate( prev.x1, b.x_filt, inter )
				b.y1 = interpolate( prev.y1, b.y_filt, inter )
				b.x2 = interpolate( prev.x2, b.x_filt, inter * .5 )
				b.y2 = interpolate( prev.y2, b.y_filt, inter * .5 )
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
				if b.anim_id == 1 then
					b.calamar_level = clamp_01( prev.calamar_level + dt / 10 )
				end
			else
				b.x_filt,b.y_filt = x,y
				b.x_anti,b.y_anti = x,y
				b.x1, b.y1 = x,y
				b.x2, b.y2 = x,y
				b.dx, b.dy = 0,0
				b.d = 0
				b.ph = 0
				b.t = 0
				if b.anim_id then
					b.calamar_level = 1
				end
			end
		end
	end

	if self.verbose >= 2 then self:print( "blob_tracked "..nb ) end
	--if nb ~= self.blob_nb then
	--	self:print( "blob_nb changed to "..nb.." from "..self.blob_nb )
	blobs.nb = nb
	--end

	aaa.show( blobs.nb, "blob_tracked" )
	--eo.send_img( "aaa.set( \"tube_lr\","..bx..")" )
	--eo.send_img( "aaa.set( \"tube_du\","..by..")" )
end

function meu:update()
	local ref = self.ref
	self.blobs_cur,self.blobs_prev = self.blobs_prev,self.blobs_cur
	self:read_blobs( self.blobs_cur, self.blobs_prev, 0,0, 1,1 )
	app.blobs_cur = self.blobs_cur
	--aaa.show( #app.blobs_cur, "#app.blobs_cur" )

--	ui.bu_nb:set_text( "blobs : "..#(self.blobs) )
	param.set( ref.draw_contour, self.b_draw_blob )
end

function meu:update_ui()
	local ref = self.ref
	local ui = self.ui
	ui.bu_nb:set_text( "blobs : "..param.get( ref.blob_nb ) )
end

function meu:draw()
	self:draw_layers_begin()

		self:draw_layer( 1 )	--attribute
		self:draw_layer( 2 )	--image contour
		self:draw_layer( 3 )	--blob tracking

--		gol.scale( self.g_scale)
		if self.b_draw_capture then
			self:draw_layer( 4 )
		end
		--gol.set_texture_dim(2)
		self:draw_blobs( self.blobs_cur )
	self:draw_layers_end()
end
