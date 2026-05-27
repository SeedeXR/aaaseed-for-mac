function meu:define_ui()
end

function meu:draw_grid()
	local m = self.meu_grid
	if not m then
		m = self:get_meu_by_name( "Grid_1" )
		self.meu_grid = m
	end
	m:render( 1 )
end

function meu:draw_titres( alpha )
	local seq = self.seq
	local id = seq:get_seq_id()
	local imgs = seq:get_imgs()
	gol.color_white( alpha*2 )
	if id == 1 then
		imgs.title		:draw_xy_sxy( 0, -0.05, 2,  .7 )
		imgs.title_sub	:draw_xy_sxy( 0, 2, 	1.8,.4 )
--lang
--		imgs.footer		:draw_xy_sxy( 0, -2.2, 	3,  .4 )
	else
		local function draw_carton( img, x, y, sx, sy, alpha )
			gol.set_texture_dim( 0 )
			local l,b,r,t = x-sx*.5, y-sy*.5, x+sx*.5, y+sy*.5
			local a1 = clamp_01( alpha )
			--local a2 = clamp_01( alpha*4. - 2. )
			gol.color_black( a1 )
			gol.set_line_width( 1	 )
			aaa.draw_rect( l,b,r,t )
			gol.color_white( a1 )
			gol.set_texture_dim( 2 )
			if type(img)=="table" then
				img:draw_xy_sxy( x, y, sx, sy )
			else
				gol.bind_texture( img )
				sx, sy = sx*.5, sy*.5
				aaa.draw_rect_uv( x-sx, y-sy, x+sx, y+sy )
			end
			gol.set_texture_dim( 0 )
			local g = 150/255
			gol.color4( g, g, g, a1 )
			aaa.draw_rect_line( l,b,r,t )
		end

		imgs.title:draw_xy_sxy( 0, 2.4, 4,.4 )
		gol.color_white( (alpha-.5)*2 )
		imgs.title_sub:draw_xy_sxy( 0, 2., 4,.4 )
		if id==4 or id==5 then
			local x
			if app.b_lang_sign then
				x = { 1.65, 1.05, .35, -.25 }
			else
				x = { 1.25, .65, -.05, -.65 }
			end
			draw_carton( imgs.text_1, 0, x[1], 1.6, .3, id==5 and 1 or (seq:get_time()-1) )
			draw_carton( imgs.text_2, 0, x[2], 1.8, .7, id==5 and 1 or (seq:get_time()-12) )
			if id==5 then
				draw_carton( imgs.text_3, 0, x[3], 1.4, .5, (seq:get_time()-2) )
				draw_carton( imgs.text_4, 0, x[4], 1.4, .5, (seq:get_time()-4) )
			end
		end
		if app.b_lang_sign_draw then
			draw_carton( self:get_bind_by_name( "D" ), 0, -1.6, 2.2, 2., 1 )
		end
	end
end

local l,r,b,t = 0, 4.6, -2.4, 2.6
local function draw_rect( id )
	--aaa.draw_rect_line( l,b,r,t )
	if id == 1 then
		aaa.draw_rect_uv( -r,b, -l,t )
	else
		aaa.draw_rect_uv( l,b,	r,t )
	end
end

function meu:draw_capture_one( alpha, id, b_blur, b_active_zone )
	--something weird here 3-id (oki camera is flipped)
	local r = app:get_face_track_rect( 3-id )

	gol.color_white( alpha )
	if r then --no rect at init
		local name = b_blur and "H" or "F4"
		gol.bind_texture( self:get_bind_by_name(name) )
 		gol.set_quad_uv( r.l, r.b, r.r, r.t )
		draw_rect( id )
	end

	if not b_blur then
		gol.color_white( alpha * (b_active_zone and .6 or .4)  )
		--todo tex_named
		gol.bind_texture( 115 )
		gol.set_wrap_2d_edge()
 		local u1, u2 = -.067, .902
 		if id==1 then u1,u2 = u2,u1 end
		gol.set_quad_uv( u1, .05 , u2, .932 )
		draw_rect( id )
	end
end
function meu:draw_capture( alpha, b_blur, b_active_zone )
	gol.set_texture_dim( 2 )
	gol.color_white( alpha )
	if b_blur then
		local meu = self:get_meu_by_name( "blurneand_1" )
		meu:set_bu_value( "blur", self.seq:get_ease_in() )
	end
	self:draw_capture_one( alpha, 1, b_blur, b_active_zone )
	self:draw_capture_one( alpha, 2, b_blur, b_active_zone )
	gol.set_texture_dim( 0 )
end

--todo ajust
meu.transfo = { ox=2.3, oy=0.1, fx=4.6, fy=5. }
function meu:update_face_info( id )
	local t = self.transfo
	local face_info = app:get_face_info( id, 1 )
	face_info:use_transfo( (id*2-3)*t.ox, t.oy, t.fx, t.fy )
	self.face_info = face_info
	return face_info
end
function meu:draw_gene_one( alpha, id )
	--aaa.print_fn()
	local fi = self:update_face_info( id )
	--elf:print( id.." "..fi.pre)
	if fi.pre <= 0 then return end
	gol.color_white( math.min( alpha, fi.pre ) )
	--gol.set_texture_dim( 0 )
	--self.face_info:draw_face_rect_line()
	gol.set_texture_dim( 2 )
	self.face_info:draw_face_rect_uv()
	--draw_rect( id )
end
function meu:draw_gene( alpha )
	--gol.set_blend_default()
	gol.set_blend_add_alpha_one()
	--gol.set_blend_max()
	gol.set_texture_dim( 2 )
	gol.texcoord_set_unit_0()
	gol.set_quad_uv()

	gol.bind_texture( self:get_bind_by_name( "Pre" ) )
	self:draw_gene_one( alpha, 1 )
	self:draw_gene_one( alpha, 2 )
	gol.set_texture_dim( 0 )
	gol.set_blend_add()
end
local part_data = {
		chin			= 	{	1.2, .7, .5, .118 },
		forehead		=	{	2.2, .8, .5, .865 },
		eyebrow_right	=	{	.8, .2, .75, .75 },
		eyebrow_left	=	{	.8, .2, .24, .75 },
		mouth_inside	=	{	1.6, .8, .5, .27 },
		noise_top		=	{	1,	1,	.5, .55 },
		eye_right		=	{	.8, .4, .75, .655 },
		eye_left		=	{	.8, .4, .24, .655 },
}
local parts_left = { "chin",
				"mouth_inside", "noise_top",
				"eye_left", "eyebrow_left", "forehead"
			}
local parts_right = { "chin",
				"mouth_inside", "noise_top",
				"eye_right", "eyebrow_right", "forehead"
			}
local parts_all = { "forehead", "chin", "eyebrow_right", "eyebrow_left",
				"mouth_inside", "noise_top", "eye_right", "eye_left" }
function meu:draw_intro_one( id )
	local fi = self:update_face_info( id )
	--elf:print( id.." "..fi.pre)
	if fi.pre <= 0 then return end

	local pl	= fi:get_pl()
	local left	= pl:get_pt(1)
	local right	= pl:get_pt(17)
	local top 	= pl:get_pt(60)
	local bot	= pl:get_pt(9)
	local d = V2.dist( left, right ) * .4

	local function draw_xy( ph, x, y, sx, sy, u, v )
		if ph <= 0 or ph >= 1.5 then return end

		local f = .2
		local su = sx * f
		local sv = sy * f * 796/948
		local fs = .5 * d
		sx = sx * fs
		sy = sy * fs
		local ph_tex = ph * 2 - 1
		if ph_tex > 1 then ph_tex = 2 - ph_tex end
		if ph_tex > 0 then
			gol.color_white( self.ease * math.min( ph_tex, fi.pre ) )
			gol.set_texture_dim( 2 )
			gol.set_quad_uv( u-su,v-sv, u+su, v+sv )
			aaa.draw_rect_uv( x-sx, y-sy, x+sx, y+sy )
		end
		ph = ph * 2

		if inside( ph, 0, 2 ) then
			gol.set_texture_dim( 0 )
			local alpha = self.ease * math.min( ph>1 and 2-ph or ph, fi.pre )
			gol.color_white( alpha* .5 )
			--alpha = math.min( alpha, 1-alpha )
			--alpha = clamp_01(alpha)
			ph = clamp_01( ph )
			sx = sx * ph
			sy = sy * ph
			aaa.draw_rect_line( x-sx, y-sy, x+sx, y+sy )
--			aaa.draw_null( x, y, 0, .2*alpha )
		end
	end
	local function draw( name, ph, sx, sy, u, v, oy )
		local x, y
		if name == "chin" then
			local mid = V2.get_middle( bot, top )
			x,y = mid[1], mid[2]
		elseif name == "forehead" then
			local mid = V2.get_middle( pl:get_pt(70), pl:get_pt(28) )
			x,y = mid[1], mid[2]
		else
			x,y = fi:get_lines_bary_2d( name )
		end
		local p = part_data[name]
		draw_xy( ph, x, y, p[1], p[2], p[3], p[4] )
	end

	local ph = self.phase*16
	--self:print( ph )
	if ph < 8 then
		local parts = id==1 and parts_right or parts_left
		for i=1,#parts do
			local v = (ph+1-i) / 2
			draw( parts[i], v )
		end
	else
		ph = ph - 8
		for i=1,#parts_all do
			local v = clamp_01(ph)
			draw( parts_all[i], v )
		end
	end
	--draw_rect( id )
	gol.set_texture_dim( 0 )
	--fi:draw_lines_bary_all()
end
function meu:draw_intro()
	--gol.set_blend_add()
	--gol.set_blend_add_alpha_one()
	--gol.set_blend_max()

	gol.texcoord_set_unit_0()
	gol.set_quad_uv()
	gol.bind_texture( 385 )
	self:draw_intro_one( 1 )
	self:draw_intro_one( 2 )
	gol.set_blend_add()
end
function meu:draw_point( id, pt, ph  )
	local s = (ph*69-id)
	local x = pt[1]
	local y = pt[2]
	--aaa.draw_rect_line( x-s, y-s, x+s, y+s )
	local max = 11
	if inside( s, 1, max ) then
		if s > (max-1) then
			s = max - s
		elseif s > 1 then
			s = 1
		end
		s = s * .05
		aaa.draw_plus_line( x, y, s, s )
	end
end
function meu:draw_line( a, b, ph  )
	--self:print( ph )
	gol.color_white( ph*self.tmp_alpha )
	aaa.draw_line( a[1], a[2], b[1], b[2] )
end
function meu:draw_tracking_one( id, ph, alpha )
	local fi = self:update_face_info( id )
	if fi.pre <= 0 then return end

	alpha = fi.pre * alpha
	self.tmp_alpha =  alpha

	gol.color_black( .3 )
--	gol.color_white( 1. )
		fi:draw_face_rect_line()
	gol.color_white()
		fi:draw_face_points( self, "draw_point", ph*2 )
		--fi:draw_lines()
		--fi:draw_lines_bary_all()
		fi:draw_lines_progressive( self, "draw_line", 0, ph*2 - .2 )
		local ph2 = (ph-.6) / .4
		if ph2 > 0 then
			local nb
			gol.color_white( ph2 * 2. )
			fi:get_pl():draw_lines_random_with_method( 60 * (1-ph2*1.5) )
			if ph2 == 1 then
				math.randomseed( math.floor( 100/ph2 ) )
			end
			--ph2 = clamp_01( ph )
			--local nb = interpolate( 0, 40, ph2 )
			fi:draw_edges_random( 80 * ph2 )
		end
end
function meu:draw_tracking( alpha )
	local ph = self.seq:get_phase()
	self:draw_tracking_one( 1, ph, alpha )
	self:draw_tracking_one( 2, ph, alpha )
end
function meu:draw_info_one( id )
	local x = id==1 and -3 or 2
	local s= .1
	local inter = app:get_zone(id).inter
	aaa.draw_str_xy( "Neandertal : "..inter, x, -2.4, s, s )
end
function meu:draw_infos()
	gol.set_texture_dim( 0 )
	gol.set_line_width( 1.2 )
	self:draw_info_one( 1 )
	self:draw_info_one( 2 )
end
function meu:draw()
	local seq = ga:get_seq_cur()
	if not seq then return end
	self.seq = seq
	local id = seq:get_seq_id()

	self.ease = seq:get_ease_in()
	self.phase = seq:get_phase()

	MEU.draw( self )

	gol.reset()
	gol.set_default()
	gol.color_white( )

	if inside( id, 2, 5	) or id==7 then
		gol.reset()
		gol.set_default()
		self:draw_capture( id==4 and .5 or .85, id==7, id==2  )
		if id==3 then
			self:draw_intro()
		elseif id==4 then
			self:draw_gene( self.ease )
		elseif id==5 then
			self:draw_tracking( self.ease )
		end
	end

	self:draw_grid()

	gol.reset()
	gol.set_default()
	gol.color_white( )
	gol.set_line_width( 1. )

	--local f = math.sin( aaa.time.t * 2. ) * .5 + .5

	if inside( id, 1, 6 ) then
		local f = clamp_01( self.ease*6 )
		local function draw_corner( l, r, b, t, f, o, fp )
			app:draw_corner_center(	-r, -l,	b, t,	f*clamp_01( o + app:get_zone(1).presence * fp) )
			app:draw_corner_center(	l, r,	b, t,	f*clamp_01( o + app:get_zone(2).presence * fp) )
		end
		if id==1 then
			draw_corner(	2, 4,	-1.4, 1.4,	f*1.25, .8, .2 )
		elseif inside( id, 2, 6 ) then
			draw_corner(	1., 4.4, -1.9, 2.2,	f, 0, 10 )
		end
	end
	if id == 6 then
		self:draw_infos()
	end

	gol.color_white( self.ease )
	gol.set_texture_dim( 2 )
	gol.set_quad_uv()
	self:draw_titres( self.ease )
	gol.set_texture_dim( 2 )
end
