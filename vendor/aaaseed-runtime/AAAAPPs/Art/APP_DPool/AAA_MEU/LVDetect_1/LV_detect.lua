
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	self:add_camera()

	local ix,iy = 1,2
	local sy = 1
	local syc = 2
	ui.bu_nb = self:add_text_info(	{ix,iy,	4,sy},			"None" )

	iy = iy + 2
	self:add_slider(	{ix,iy,	8,1},	"Offset",		self, "g_offset",		0, -4,4 )
	--self:add_slider(	{ix,iy+1,	8,1},	"Offset_zoom",	self, "offset_zoom",	0, -4,4 )
	self:add_slider(	{ix,iy+2,	8,1},	"Scale",		self, "g_scale",		1, 1,2 )
	iy = iy + 3
	self:add_slider(	{ix,iy+1,	8,1},	"Radius",		self, "rond_radius",		1, 0,1 )
	self:add_button( {ix, 	iy }, 			"Draw_rond",		self, "b_draw_rond",	true )
	iy = iy + 2
		bu = self:add_selector(	{ix,iy,	8,1}, "Type" )
		bu:set_nb( 9 )
		bu:set_target_lua( self, "id_bind")
	iy = iy + 1
		iy = iy + 2
	bu = self:add_rgbfa(	{ix,iy,	8,syc}, "Rond", false )
		self.ui.bu_rgba = bu


	iy = iy + syc + .2
	self:add_button( {ix, 	iy+1 },			"Draw_blob",	self, "b_draw_blob",	false )
	self:add_button( {ix+1,	iy+2 },			"phy",			self, "b_draw_phy",		false )
	self:add_button( {ix+1,	iy+3 },			"Id",			self, "b_draw_id",		false )

	ix,iy = 9,3
	self:add_slider(	{ix,iy,	8,1},	"phy_min",				self, "phy_min",		0, 0,.2 )
	self:add_slider(	{ix,iy+1,	8,1},	"Anticipation",			self, "anti_factor",	5, 0,40 )
	self:add_slider(	{ix,iy+2,	8,1},	"Anticipation Limit",	self, "anti_limit",		1, 0,20 )
	iy = iy + 4
	sy = 2
	self:add_rgbfa(	{ix,iy,	8,sy}, 	"Caputure", aaa.obj.get_down_by_class( self:get_layer(4), "color" ) )
	iy = iy + syc + .2
	self:add_button( {ix, 	iy }, 			"Capture",		self, "b_draw_capture",	true )
	iy = iy + 1
	self:add_button( {ix, 	iy }, 			"Draz Fond",		self, "b_draw_fond",	true )
	iy = iy + 1
	self:add_rgbfa(	{ix,iy,	8,sy}, 	"Fond" )
	self.ui.bu_rgba_fond = bu

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


-- 								calamar, manta, shark, 	tuna,	turtle
local repulse_plancton	=	{	true,	false,	true,	true,	true	}
local size_plancton		= 	{	.5,		1,		1,		.75,	1		}
local force_plancton	= 	{	1,		1,		.1,		1,		1		}
local repulse_boid		=	{	true,	true,	true,	false,	true	}
local size_boid			= 	{	.5,		2,		3,		1.2,	1		}
local force_boid		= 	{	1,		1,		2,		.17	,	1		}
local anim_factor		= 	{	1,		1,		1,		1,		1		}

function meu:draw_blobs( blobs )
--	if true then return end
	gol.reset()

	local r,g,b,a = self.ui.bu_rgba:get_rgba()
	gol.color4( r,g,b, a*self:get_alpha() )

	local color = { {0,1,1,1}, {1,0,1,1}, {1,1,0,1} }

	gol.set_line_width( 2 )
	local f = 5
	local r = .75
	local binds = { 32, 36, 37, 38, 39, 40, 41, 70, 71 }
	local bind = binds[ self.id_bind ]
	--gol.color_red()
	for id=1,blobs.nb do
		local b = blobs[id]
		local x,y = b.x_filt + b.dx, b.y_filt + b.dy
		local c = color[  b.anim_id % 3 + 1 ]
		r,g,b,a = c[1], c[2], c[3], c[4]*self:get_alpha()
		gol.color4( r,g,b, a*self:get_alpha() )
		x = x + self.g_offset
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

function meu:draw_fond()
	local r,g,b,a = self.ui.bu_rgba_fond:get_rgba()
	gol.color4( r,g,b, a*self:get_alpha() )
	gol.color_white()

	local r = .25
	local rx = r*.5
	local ry = r*.5
	local nx,ny = 10/r+1,10/r+1
	for ix =1,nx do
		local x = (ix-nx*.5)*r
		for iy =1,ny do
			local y = (iy-ny*.5)*r
			aaa.draw_bind_rect( 40, x-rx, y-ry, x+rx, y+ry )
		end
	end
end

--	barycenter is computed with same coordantes as the blob
function meu:read_blobs( blobs, prevs )
	--self:print( "read_blob "..b_mouse.." "..b_mouse_no )
	local obj		= self.ref.bdd_track
	local tmp_nb 	= aaa.bdd.get_point_nb( obj )

	local dt = aaa.time.dt
	local nb = 0
	--we compute barycenter

	if tmp_nb > 0 then
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
	self:read_blobs( self.blobs_cur, self.blobs_prev )

	local ref = self.ref
	local ui = self.ui
--	ui.bu_nb:set_text( "blobs : "..#(self.blobs) )
	param.set( ref.draw_contour, self.b_draw_blob )
end

function meu:update_ui()
	local ref = self.ref
	local ui = self.ui
	ui.bu_nb:set_text( "blobs : "..param.get( ref.blob_nb ) )
end

function meu:my_draw()
--[[
	local bu = self.bu_multi
	for i=1,bu:get_elt_nb() do
		local elt = bu:get_elt( i )
		if elt:is_contact() or elt:get_value() then
			local x,y = elt:get_xy()
			x = (x-.5)*14
			y = (y-.5)*9
			--self:print( i.."drawing elt "..x..","..y )
			local sx,sy = elt:get_sxy()
			sx = sx * 14 *.5
			sy = sy * 9 * .5
			aaa.draw_bind_rect( 2, x-sx, y-sy, x+sx, y+sy )
		end
	end
--]]
end

function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 )
		self:draw_layer( 2 )
		self:draw_layer( 3 )
		if self.b_draw_fond then
			self:draw_fond()
		end
		gol.scale( self.g_scale)
		if self.b_draw_capture then
			self:draw_layer( 4 )
		end
		self:my_draw()
		self:draw_blobs( self.blobs_cur )
	self:draw_layers_end()
end
