aaa.show_file_begin( "dior_draw" )

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local iy = 1
		bu = self:add_button(	{	2,	iy },				"Back", 		self,		"b_draw_back",		true	)
		self.ui.color_back = self:add_rgba(	{5,iy,	4,1}, "back_col", false )

		bu = self:add_button(	{	9,	iy },				"Face", 		self,		"b_draw_face",		false	)
		self.ui.color_face = self:add_rgba(	{13,iy,	4,1}, "face_col", false )

	iy = iy + 1
		bu = self:add_button(	{	1,	iy },				"Rect", 		self,		"b_draw_rect",		false	)
--		bu = self:add_button(	{	9,	iy },				"Duplicate", 	self,		"b_face_dup",		false	)
		bu = self:add_selector(	{9,iy,	4,1},	"Duplicate"	)
			bu:set_nb_min_0( 3, 1 )
			bu:set_target_lua( self, "s_duplicate" )
			bu:set_item_text( 1, "No", "Left", "Right" )

	iy = iy + 1
		bu = self:add_selector(	{1,iy,	4,1},	"Imgs"	)
			--bu:set_text_draw( false )
			bu:set_nb_min_0( 8, 1 )
			bu:set_target_lua( self, "img_id" )
			bu:set_item_text( 1, "No" )
		self.ui.color_img = self:add_rgba(	{5,iy,	4,1}, "img_col", false )

		bu = self:add_button(	{	9,	iy },				"Test", 		self,		"b_test_alpha",		false	)
		bu = self:add_slider(	{13,iy,	4,1},	"Alpha",		self,		"test_alpha",		0,		0, 1	)

	iy = iy + 1.4
--[[
		bu = self:add_selector(	{1,iy,	4,1},	"Anims"	)
			--bu:set_text_draw( false )
			bu:set_nb_min_0( 8, 1 )
			bu:set_target_lua( self, "anim_id" )
			bu:set_item_text( 1, "No" )
		self.ui.color_seq = self:add_rgba(	{5,iy,	nil,1}, "anim_col", false )
		bu = self:add_button(	{9,	iy },					"Animate", 		self,		"b_anim",			true	)
		bu = self:add_slider(	{13,iy,	4,1},		"Speed", 		self,		"anim_speed",		1,		0, 8	)
--]]

	iy = iy + 1
		bu = self:add_selector(	{1,iy,	8,1},	"Shade"	)
			--bu:set_text_draw( false )
			bu:set_nb_min_0( 8, 1 )
			bu:set_target_lua( self, "shade_id" )
			bu:set_item_text( 1, "No" )
			--bu = self:add_button(	{9,	iy },				"HemiFace", 	self,		"b_hemiface",		false	)
		bu = self:add_selector(	{9,iy,	8,1},	"How"	)
			--bu:set_text_draw( false )
			bu:set_nb_min_0( 2, 2 )
			bu:set_target_lua( self, "s_shade_how" )
			bu:set_item_text( 1, "Geo", "uv", "HemImage", "HemiFace" )

	iy = iy + 1
		bu = self:add_slider(	{1,iy,	4,1},	"Pos",			self,		"pos", 				.5,		0, 1		)
		bu = self:add_trig(	{	5,	iy,			4, 1},	"ReLoad",		self:get_shading().ref.reload_all,	nil,	false )
		-- BORDER
		bu = self:add_button(	{	9,	iy },				"BBorder",		self,		"b_border",			true	)
			bu:set_text_draw( false )
		bu = self:add_slider(	{10,iy,	3,1},	"Border",		self,		"border",			.5,		0, 1		)
		bu = self:add_slider(	{13,iy,	4,1},	"Factor",		self,		"border_factor",	.25,	-1, 1		)

	iy = iy + 1
		bu = self:add_button(	{	1,	iy },				"Adding", 		self,		"b_shader_add",		false	)
		bu = self:add_slider(	{9,iy,	2,1},	"S Before",		self,		"sat_before",		0,		-.2, .2		)
		bu = self:add_slider(	{11,iy,	2,1},	"H Before",		self,		"hue_before",		0,		-.04, .04	)
		bu = self:add_slider(	{13,iy,	2,1},	"S After",		self,		"sat_after",		0,		-.2, .2		)
		bu = self:add_slider(	{15,iy,	2,1},	"H After",		self,		"hue_after",		0,		-.04, .04	)

	iy = iy + 1

	iy = iy + 1
		bu = self:add_button(	{	1,	iy },				"visu", 		self,		"b_draw_visu",		false	)
			bu:set_text_rect_ratio( 1.6 )
		bu = self:add_button(	{	3.1,	iy },			"Tri", 			self,		"b_draw_tri_line",	false	)
			bu:set_text_rect_ratio( 1.6 )
		bu = self:add_button(	{	5.,	iy },				"Line", 		self,		"b_draw_line",		false	)
			bu:set_text_rect_ratio( 2 )
		bu = self:add_button(	{	7.2, iy },			"All", 			self,		"b_draw_line_all",	false	)
			bu:set_text_rect_ratio( 1. )

		bu = self:add_slider(	{9,iy,	8,1},	"Width",		self,		"line_width",		1,		.5, 5	)
		self.ui.color_line = self:add_rgba(	{9,iy+1,	4,1}, "line_col", false )

	iy = iy + 1
		bu = self:add_button(	{	1,iy },				"Point", 		self,		"b_draw_point",		false	)
		bu = self:add_button(	{	5,iy },				"Number", 		self,		"b_draw_number",	false	)

	iy = iy + 1
		bu = self:add_button(	{	1,iy },				"Skin", 		self,		"b_skin_do",		true	)
			bu:set_text_rect_ratio( 2 )
		self.ui.face_color = self:add_rgb(	{4,iy,	4,1},	"face_color", false )
		local ix = 9
		local dx = 1.8
		bu = self:add_slider(	{ix,iy,			dx,1},		"Hue",			self,		"	",				0,		0, 1	)
		bu = self:add_slider(	{ix+dx,iy,		dx,1},		"Sat",			self,		"skin_sat",			0,		0, 1	)
		bu = self:add_slider(	{ix+dx*2,iy,	dx,1},		"Val",			self,		"skin_val",			0,		0, 1	)
		bu = self:add_button(	{ix+3*dx,		iy },		"Show", 		self,		"b_skin_show",		false	)
			bu:set_text_rect_ratio( 2 )

	iy = iy + 1

	iy = iy + 1
		bu = self:add_button(	{	1,	iy },				"Calage", 		self,		"b_draw_calage",	false	)
		bu = self:add_button(	{	5,	iy },				"Elts", 		self,		"b_draw_elts",		true	)
		bu = self:add_button(	{	9,	iy },				"Palette", 		self,		"b_draw_palette",	true	)
		bu = self:add_button(	{	13,	iy },				"Typo", 		self,		"b_draw_typo",		true	)

	iy = 15
	bu = self:add_selector(	{1,iy,	4,1},	"Certainty" )
		--bu:set_text_draw( false )
		bu:set_nb_min_0( 3, 1 )
		bu:set_target_lua( self, "s_certainty" )
		bu:set_item_text( 1, "Track", "Yes", "No" )
end

function meu:init()
	self:add_script( "dior_draw_bis.lua" )

	local ref = self.ref
	local layer = self:get_layer(2)
	self:add_shading( 2 )
	ref.color	= COLOR_REF:create( self:get_name(), aaa.obj.get_down_by_class( layer, "color" ) )

	ref.tex = aaa.layer.build_bank_bind_2d_ref_table( layer, 0,3 )

	self.skin = {}
	self.oval_last = 0
end


--
--
local SYF = 8
local SXF = SYF * 9 / 16

local SY = 8
local SX = SY * 9 / 16
local SX_PIX = 1080
local SY_PIX = 1920

local OX = 0
local OY = 0
local LE = OX - SX * .5
local RI = OX + SX * .5
local BO = OY - SY * .5
local TO = OY + SY * .5

function meu:update_face_info( id )
	local face_info = app:get_face_info( id, 1 )
	self.face_info = face_info
	if face_info then
		-- we do detection on a partial capture from a flip image (y coor)
		-- -SX to flip x coor
		-- 256/358 come from capture size
		--local rx = face_info.__ratio_x
		--self:print( "rx "..rx )
		local oy_tracker = 1. - face_info.__tex_bottom - face_info.__tex_top
		face_info:use_transfo( OX, OY + oy_tracker*SX, -SX, SX/(face_info.__detect_sx/face_info.__detect_sy) )
	end
	return face_info
end

function meu:draw_img_bottom( img )
	--need before set_wrap
	gol.bind_texture( img:get_bind() )
	gol.set_wrap_2d_edge()

	img:draw_lb_sx( LE, BO, SX )
end
function meu:draw_img_top( img )
	gol.bind_texture( img:get_bind() )
	gol.set_wrap_2d_edge()

	img:draw_lt_sx( LE, TO, SX )
end
function meu:draw_rect_uv( l,b, r,t )		aaa.draw_rect_uv( l,b, r,t )	end
function meu:draw_rect_full()				self:draw_rect_uv( LE,BO, RI,TO )	end

function meu:draw_background( alpha )
	if self.b_draw_back then
	--	gol.set_texture_dim( 2 )
	--	gol.color_white( alpha )
		self:set_color( "color_back", alpha )
		gol.set_quad_uv( 0, 1, 1, 0 )
		self:draw_rect_full()
	end
end


function meu:draw_point( id, pt, ph )
	local s = ph * 69 -id
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
		s = s * .02
		--aaa.draw_plus_line( x, y, s, s )
		aaa.draw_crosshair(	x,y, s,s*.3 )
	end
end

function meu:draw_line( a, b, ph_begin, ph_end )
	if ph_begin < ph_end then
		local a2 = { a[1], a[2] }
		local b2 = { b[1], b[2] }
		if ph_begin > 0 then
			a2[1] = interpolate( a[1], b[1], ph_begin )
			a2[2] = interpolate( a[2], b[2], ph_begin )
		end
		if ph_end < 1 then
			b2[1] = interpolate( a[1], b[1], ph_end )
			b2[2] = interpolate( a[2], b[2], ph_end )
		end

		--aaa.draw_line( a[1], a[2], b[1], b[2] )
		aaa.draw_line_tex(	a2, b2, 0.016 )
	end
end

function meu:draw_progress_bar(val)
	if val then
		local exp = app:get_experience_code()
		if exp=="HL" then
			gol.color_255( 103, 190, 186, 255 )
			gol.set_texture_dim( 0 )
			local l = LE+SX*32/SX_PIX
			local r = RI-SX*32/SX_PIX
			aaa.draw_rect( l,BO+SY*22/SY_PIX, l+(r-l)*clamp_01(val),BO+SY*33/SY_PIX )
		end
	end
end
function meu:draw_welcome_dsa( alpha )
	local fi = self.face_info
	--if fi.pre <= 0 then return end

	local function get_rect( ph )
		ph = math.floor( ph )
		local rect
		--self:print( ph )
		if ph == 0 then
			local x,y = fi:get_lines_bary_2d( "eye_right" )
			--self:print( x,y )
			rect = { r=x, b=y, l=x, t=y }
		else
			local liste = {
							{	"eye_right"				},
							{	"eye_right",			"eyebrow_right"			},
							{	"eye_left"				},
							{	"eye_left",				"eyebrow_left"			},
							{	"noise_top",			"noise_bottom"			},
							{	"mouth_outside_top",	"mouth_outside_bot",	"mouth_inside"		},
							{	"face_low" 				},
							{	"face_low",				"face_top"				},
						}
			rect = fi:get_rect_min_from_lines_by_name( liste[ph] )
		end
		return rect
	end

	local function draw_rect( rect, alpha )
		if not rect or alpha <= 0 then return end

		local DX = .08
		local DY = DX
		local l,b,r,t = rect.l-DX, rect.b-DY, rect.r+DX, rect.t+DY

		--table.print( rect )
		if false then
			gol.set_texture_dim( 0 )
			gol.color_black( .1 * alpha )
			aaa.draw_rect( l,b,r,t )
		end

		gol.color_white( alpha )
		gol.set_texture_dim( 2 )
		gol.bind_texture( app.binds.dash )
		gol.set_wrap_2d_repeat()

		local S = .008
		local F = 15
		local ic

		gol.set_quad_uv( .5, 0.25,  .5+(r-l)*F, .75 )
		self:draw_rect_uv( l,t-S, r, t+S )
		self:draw_rect_uv( l,b-S, r, b+S )

		gol.set_quad_uv( 0.25,  .5, .75, .5+(t-b)*F )
		self:draw_rect_uv( l-S, b, l+S, t )
		self:draw_rect_uv( r-S, b, r+S, t )
		--aaa.draw_rect_line( l,b,r,t )
	end

	local ph = self.seq:get_phase()
	--local r1 = self.__rect_trak or {}
	local ph2 = self.seq:get_time() - 2.6 -- 68 frames
	if ph2 > 0 then
		local alpha_rect = 1
		local r1
		local t1 = .12	-- 3 frames
		local t2 = .56	-- 14 frames
		local tt = t1+t2
		local ta = 8 * tt
		local line_begin	= 0
		local line_end		= 1
		local nb = fi:get_lines_nb()
		if ph2 < ta then
			local ind = math.floor( ph2 / tt )
			r1 = get_rect( ind )
			local r2 = get_rect( ind+1 )
			ph2 = math.fmod( ph2, tt )
			--self:print( "ind ph "..ind.." "..ph2 )
			local inter_rect = (ph2 < t1) and ph2/t1 or 1
			--self:print( inter )
			--table.print( r1, "r1" )
			--table.print( r2, "r2" )
			--self:print( "rect "..ind.." "..(ind+1).." "..inter_rect )
			aaa.lbrt.interpolate( r1, r1, r2, inter_rect )
			--table.print( r1, "r1" )

			local progress = { 0, 1, 2, 3, 4, 6, 9, 10, 11 }
			local inter1 =  ( progress[ ind+1 ] )
			local inter2 =  ( progress[ ind+2 ] )
			line_end = (ph2 < t1) and 0 or (ph2 - t1) / t2
			--self:print( "lines "..inter1.." "..inter2.." "..line_end )
			line_end = interpolate( inter1, inter2, line_end ) / nb
			--self:print( line_end )
		else
			r1 = get_rect( 8 )
			ph2 = ph2 - ta
			--self:print( "rect 8 "..ph2 )
			alpha_rect = clamp_01( 1 - (ph2-.12)/.12 ) -- 3 frames
			ph2 = ph2 - 1.52-- 38 frames
			--self:print( ph2 )
			line_end = 1
			line_begin = clamp_01( ph2/.6 ) -- 15 frames
		end
		--table.print( r1, "r1" )
		draw_rect( r1, alpha_rect * alpha )

		gol.set_quad_uv()

		gol.set_texture_dim( 0 )

		--self:print( "welcome "..ph )

		--	gol.color_cyan( .3 )
		--	fi:draw_face_rect_line()
		gol.color_white( alpha )

		gol.set_line_width( 6 )
		gol.set_line_smooth( false )

		--fi:draw_face_points( self, "draw_point", ph*2 )

		--fi:draw_lines()
		--fi:draw_lines_bary_all()
		--gol.set_blend_add_alpha_one()

		gol.bind_texture( app.binds.grad_vert )
		gol.set_texture_dim( 2 )
		gol.set_wrap_2d_repeat_edge()

		fi:draw_lines_progressive( self, "draw_line", line_begin, line_end )
		if false then
			--fi:draw_lines_progressive( nil, nil, inter_line )
			local ph2 = (ph-.6) / .4
			if ph2 > 0 then
				local nb
				gol.color_white( ph2 * 2. )
				--fi:get_pl():draw_lines_random_with_method( 60 * (1-ph2*1.5) )
				if ph2 == 1 then
					math.randomseed( math.floor( 100/ph2 ) )
				end
				--ph2 = clamp_01( ph )
				--local nb = interpolate( 0, 40, ph2 )
				--fi:draw_edges_random( 80 * ph2 )
			end
		end
	end

end

function meu:draw_face( alpha, s_duplicate )
	gol.set_texture_dim( 2 )
	--gol.bind_texture( 392 + math.floor(ind) )	--385, 1072		src Camera
	--gol.set_blend_add_alpha_one()
	self:set_color( "color_face", alpha )

 	if self.shade_id == 0  then
 		gol.bind_texture( app.binds.src )
 		gol.set_wrap_2d_edge()
 	end
 	local face_info = self.face_info
 	--face_info.ov = (-.2-g_oy_spe)/(g_size_factor*2)	--todo depend on first crop should be dealt with
	face_info.ov = - .1 --	- (g_oy_spe-.2)/(g_size_factor)

 --bricole
 	--self:print( "   "..b_spe )
 	if s_duplicate > 0 then
 		self:update_shader( 0, .5, 1, false, 1, true, false )
 		face_info:draw_tri_tex_spe( self.pts, s_duplicate )
 	else
 		face_info:draw_tri_tex( self.pts )
 	end
 	face_info.ov = 0
end

function meu:update_amin( anim, time )
	if anim then
		time = time * app.fps_video -- * self.anim_speed )
		anim:__set_index_float( time )
		local bind = anim:get_bind()
		--self:print( "    bind -> "..bind )
		--self:print( anim.." gave "..bind )
		self:set_tex( bind )
		gol.set_wrap_2d_edge()
		return bind
	end
end

function meu:draw_anim_begin( anim, alpha, time )
	--self:print( "typo "..anim )
	if anim then
		--self:print( anim.." "..time )
		gol.set_texture_dim( 2 )
		gol.color_white( alpha )
		anim:set_loop( false )
		--gol.set_blend_add()
		return self:update_amin( anim, time )
	end
end
function meu:draw_anim_end( x, y, sx, sy )
	local l = LE + x/SX_PIX * SX
	local b = TO - (y+sy)/SY_PIX * SY
	local r = l + SX*(sx/SX_PIX)
	local t = b + SY*(sy/SY_PIX )
	--self:print( anim.." l b "..l.." "..b.." r t "..r.." "..t )
	self:draw_rect_uv( l, b,	r, t )
	--self:draw_img_bottom( app.img_intro, SYH - 1400/SY_PIX * SY )
end
function meu:do_anim( anim, alpha, time, x, y, sx, sy )
	local bind = self:draw_anim_begin( anim, alpha, time )
	if bind then
		self:draw_anim_end( x, y, sx, sy )
	end
end

function meu:do_anim_typo( name, alpha, time )
	if time and time>=0 and alpha and alpha >0 then
		local anim = app:get_anim( self.seq, name )
		local bind = self:draw_anim_begin( anim, alpha, time )
		--self:print( name.." "..alpha.." "..time.." -> "..anim.." bind "..bind )
		if bind then
			local sx,sy,_ = aaa.img.get_size( bind )
			--self:print( anim.." "..sy )
			self:draw_anim_end( 0, SY_PIX-sy, 	sx,	sy ) --we draw left and bottom
		end
	end
end

function meu:draw_amin_on_face( anim, time )
	if anim then
		gol.set_texture_dim( 2 )
		self:update_amin( anim, time )
		self.face_info:draw_tri_tex( self.pts, self.uvs, false )
	end
end

function meu:draw_img( alpha )
	gol.set_texture_dim( 2 )

--	self:print( "draw_img "..self.img_id )
	local img = app:get_img_fix( self.img_id )
	if img then
--		self:print( "IMG_FIX  bind "..img:get_bind() )
		self:set_tex( img:get_bind() )
	else
		self:set_tex( BU_MONITOR.__last_bind )
	end
	gol.set_wrap_2d_edge()
	self:set_color( "color_img", alpha )
	self.face_info:draw_tri_tex( self.pts, self.uvs, false )
end

function meu:draw_visu( alpha )
	local face_info = self.face_info
	if self.b_draw_visu then
		--self:print( "alpha "..alpha )
		gol.set_texture_dim( 0 )

		self:set_color( "color_line", alpha )
		gol.set_line_width( self.line_width )

		if self.b_draw_tri_line then
			face_info:draw_tri_edge( self.pts )
		end

		if self.b_draw_line then
			self.face_info:draw_lines()
			if self.b_draw_line_all then
				local pl = face_info:get_pl()
				if face_info.b_85 then
					pl:draw_lines_table( nil, nil, 0, 1, { 1, 74, 75, 76, 77, 78, 79, 70, 85, 84, 83, 82, 81, 80, 17 } )
				end
				gol.color_white( 1. )
				pl:draw_lines_table( nil, nil, 0, 1, { 28, 9 } )
			end
		end
	end

	if self.b_draw_point then
		gol.set_texture_dim( 0 )
		face_info:draw_face_points( self, "draw_point_number", .06 )
	end

	self:draw_skin_points( face_info )
end

function meu:draw_point_number( id, pt, size )
	local o = -size * .4
	local x = pt[1]
	local y = pt[2]
	if self.b_draw_point then
		gol.color_white( self.pre )
		aaa.draw_plus_line( x, y, size*.5, size*.5 )
	end
	if self.b_draw_number then
		gol.color_green( self.pre )
		aaa.draw_str_xyz( id, x+o, y, pt[3] or 0, size, size )
	end
end

function meu:get_tracker_xy()
	local fi = self.face_info
	--if fi.pre <= 0 then return end

	if fi then
		local pl = fi:get_pl()
		local a_id, b_id
		local exp = app:get_experience_code()
		if		exp == "DSA" then
			a_id, b_id = 46, 55
		elseif	exp=="HL" then
			local seq_id_name	= string.lower( self.seq:get_title() )
			a_id, b_id = 48, 87
			if	seq_id_name == "nature_b"	then
				a_id, b_id = 22, 79
			elseif	seq_id_name == "nature_c"	then
				a_id, b_id = 5, 49
			end
		end
		local a	= pl:get_pt(a_id)
		local b = pl:get_pt(b_id)
		if a and b then
			local c = V2.get_middle( a, b )
			return c[1], c[2]
		end
	end
end

function meu:draw_tracker( x, y, alpha )
	if not x then return end

--	gol.color_black( alpha )
	gol.color_white( alpha )
	local exp = app:get_experience_code()
	if		exp == "DSA" then
		if false then
			gol.set_texture_dim( 0 )
			aaa.draw_plus_line( x, y, 1, 1 )
			gol.set_texture_dim( 2 )
		end
		local s = SX*56/SX_PIX
		if app.img_tracker then
			app.img_tracker:draw_lb_sx( x, y, s )
		end
	elseif	exp == "HL" then
		--xy is top left now
		local s = 120
		x = (x/SX+.5) * SX_PIX - s/2
		y = (-y/SY+.5) * SY_PIX - s/2
		--self:print( "tracker x y "..x.." "..y.." size "..s.." time "..self.pal_tracker )
		self:do_anim( app.tracker, alpha, self.pal_tracker, x, y, s, s )
	end
end
--[[
	if self.pal_tracker > 0 then
		local s = 260
		self:do_anim( app.pal_tracker, 1, self.pal_tracker, 50, 340, s, s )
	end
--]]
function meu:draw_pal()
	if not self.b_seq_with_pal then	return end

	local exp = app:get_experience_code()

	local x_tracker, y_tracker = self:get_tracker_xy()

	if self.pal_tracker > 0 then
		self:draw_tracker( x_tracker, y_tracker, 1. )
	end

	if self.b_draw_palette and (exp=="HL" or self.pal_inter > 0 ) then
		if self.pal_alpha > 0 and self.pal_time then
			local anim = app:get_anim( self.seq, "palette" )
			--self:print( self.pal_alpha, self.pal_time )
			--self:print( "typo "..anim )
			if anim then
				anim:set_loop( false )
				--gol.set_blend_add()
				local bind = self:update_amin( anim, self.pal_time  )
				local x,y,s
				if exp=="HL" then
					x = LE + SX*20/SX_PIX
					y = BO + SY*380/SY_PIX
					s = SX*332/SX_PIX
					gol.color_white( 1. )
				else
					local inter = clamp_01( self.pal_inter )
					local l = LE + SX*60/SX_PIX
					local b = TO - SY*(1200+320)/SY_PIX
					x = l + x_tracker
					s = SX*320/SX_PIX
					x = math.fn_linear( x, 1, 	-1000,-2,	-3,-2.2,	-2.,-2,		-.5,-.5,	.5,-.3 )
					x = SX * x / SXF + OX
					x = interpolate( x_tracker, x, inter )
					y = interpolate( y_tracker, b, inter )
					s = interpolate( .2, s, inter )
					gol.color_white( self.pal_alpha )
				end

				if false then
					gol.set_texture_dim( 0 )
					gol.color_white( self.pal_alpha * .7 )
					self:draw_rect_uv( x-s, y-sy,	x+s, y+sy )
					gol.set_texture_dim( 2 )
				end

				self:draw_rect_uv( x, y,	x+s, y+s )
			end
		end
	end
end

function meu:draw_line_mid()
	if self.line_mid_alpha > 0 then
		gol.color_white( self.line_mid_alpha )
		gol.set_blend_add_alpha_one()
		gol.bind_texture( app.binds.grad_radial )
		gol.set_wrap_2d_edge()
		app:draw_face_middle_line( self.pts, .01 )

		gol.set_blend_add_alpha_one()
	end
end

function meu:draw_face_rect( face_info )
	if self.b_draw_rect and face_info.pre > 0 then
		gol.color_cyan( .7 )
		gol.set_texture_dim( 0 )
		face_info:draw_face_rect_line()
	end
end

function meu:draw_skin_points( face_info )
	if self.b_skin_show then
		gol.set_blend_add_alpha_one()
		gol.color_cyan()
		gol.set_texture_dim( 0 )
		face_info:draw_points_for_color()
	end
end

function meu:draw_compare()
	if self.compare and self.compare>0. then
		local exp = app:get_experience_code()
		gol.color_white( exp=="HL" and 1 or self.compare )
		for i=1,2 do
			local img = app.img_with[i]
			if img then
				if exp=="HL" then
					local isx = img:get_sx() * self.compare
					local sx = isx/SX_PIX*SX
					local offset = (20+113)/SY_PIX*SY
					img:draw_xy_sx( i==1 and LE+offset or RI-offset, -100/SY_PIX*SY, sx )
				else
					gol.bind_texture( img:get_bind() )
					local sx = (280)/SX_PIX*SX
					local offset = sx * (1-self.compare)
					local l = i==1 and LE-offset or RI-sx+offset
					local b = TO - 450/SY_PIX*SY
					--self:print( "compare "..i.." : "..l.." "..b.." "..sx.." "..100/SY_PIX*SY )
					self:draw_rect_uv( l, b,	l+sx, b + 46/SY_PIX*SY)
				end

			end
		end
	end
end

function meu:draw_packshot( packshot, packshot_white )
	if packshot > 0 then
		gol.set_texture_dim( 2 )
		gol.color_white( packshot )
		--gol.color_white( 1.  )
		--app.img_packshot:draw_lb_sx( -SXH + SX*(-1+self.packshot), -SYH, SX )
		app.img_packshot:draw_lb_sx( LE,BO, SX )
	end
	if packshot_white > 0 then
		gol.set_texture_dim( 0 )
		gol.color_white( packshot_white )
		self:draw_rect_full()
	end
end

function meu:draw_logo_top()
	--self:print( "a "..app.img_top[1])
	gol.color_white( .4 )
	self:draw_img_top( app.img_top[1] )
	--gol.set_texture_dim( 0 )
	gol.color_white( 1. )
	self:draw_img_top( app.img_top[2] )
end
function meu:draw_oval()
	if self.oval > 0 then
		local sx = SX_PIX
		local sy = 1070
		local s = .8
		self:do_anim( app.oval, 1, self.oval,	0+sx*(1-s)*.5, 430+sy*(1-s)*.5,		sx*s, sy*s )
	end
end
function meu:draw_please()
	if self.please > 0 then
		local s = 260
		self:do_anim( app.please, 1, self.please, 50, 340, s, s )
	end
end

function meu:update_shader_no( b_last )
	self.b_shading = false
	self:draw_layer( b_last and 3 or 1 )
end
--hemi_min have to be < hemi_max
function meu:update_shader( sid, hemi_min, hemi_max, b_mask, hemi, b_hemi_image, b_border )
	local sid = sid or self.shade_id

	if sid > 0  then
		local exp = app:get_experience_code()
		local sha = self:get_shading()
		sid =  (sid <= 4) and -sid or (sid - 4)
		sha:set_frag_int( 1, sid + (exp=="HL" and 256 or 0) )
		sha:set_frag_int( 2, sid==3 and 1 or (b_hemi_image and 2 or 3) )	--	self.s_shade_how )
		sha:set_frag_int( 3, b_mask and 1 or 0 )
		sha:set_frag_int( 4, self.b_shader_add and 1 or 0 )

		--self:print( "face_on "..self.face_on)
		sha:set_frag_float_1_3( hemi, self.face_on_alpha, exp=="HL" and 1 or 0 )
		sha:set_frag_vec4( 1, self.hue_before,	self.sat_before,	self.value_max_offset_before,	self.value_min_before )
		sha:set_frag_vec4( 2, self.hue_after,	self.sat_after,		self.value_max_offset_after,	self.value_min_after )

		-- hemi_min have to be < hemi_max if we want to have 2 zones
		--	hemi face 0 <--> left , 1 <--> right
		--	hemi image reverse 1 <--> left , 0 <--> right
		--sha:set_frag_vec4( 3, nil, nil, nil, nil )
		sha:set_frag_vec4( 3, hemi_min, hemi_max, b_border and self.border or 0, self.border_factor )

		sha:set_frag_vec4( 4, self.skin.r, self.skin.g, self.skin.b )
		if self.b_skin_do then
			--self:print( self.skin_hue )
			sha:set_frag_vec4( 5, self.skin_hue, self.skin_sat, self.skin_val )
		else
			sha:set_frag_vec4( 5, 0, 0, 0 )
		end

		self.b_shading = true
		self:draw_layer( 2 )
	else
		self:update_shader_no()
	end
end

function meu:update()
--	self:box_debug( "before" )
	self:update_script()
--	self:box_debug( "after" )
end


local VOLET_DEF = 1.2

function meu:draw_before()
	--ccalls = {}
	self.frame_count = (self.frame_count or 0) + 1
	--self:print( "frame_count"..self.frame_count )
	if self.frame_count==100 then
		--if not aaa.pc.is_maa() then
		app:load_anim_all()
		--end
	end

	self.seqs = app.seqs
	self.seq = ga:get_seq_cur()
	if not self.seq then return end

	local face_info = self:update_face_info( 1 )
	local alpha = 1
	if face_info then
		face_info.tex_bind = app.binds.src
		alpha = face_info.pre
		if face_info:is_use() then
			self.pts = face_info:get_pts()
			--table.print( self.pts, "pts", 3 )
			local far, fag, fab = face_info:get_points_color( app.binds.src_small )
			self.ui.face_color:set_rgba( far, fag, fab )
			self.skin = { r=far, g=fag, b=fab }
		end
	end
	self.pre = alpha
	self.alpha = alpha

	local presence = self.b_test_alpha and self.test_alpha or alpha
	gol.color_white( presence )
	self.ref.color:set_alpha( presence )
	self.presence = presence

	if aaa.b_ios then
		self.uvs = FACE_INFO.uv_dior
	else
		self.muv = self:get_meu_by_name( "FaceUV_1" )
		local bu_uv = self.muv.ui.uv
		self.uvs = bu_uv and bu_uv:get_values() or nil
	end
end

function meu:compute()
	local seq			= self.seq
	local seq_id		= seq:get_seq_id()
	local seq_id_name	= string.lower( seq:get_title() )
--	self:print( seq_id_name )
	local time			= seq:get_time()
	local fps = app.fps_video

	local face_on_alpha		= 0		-- alpha of mask/scan on the face
	local face_on_time

	local b_hemi_image		= false
	local hemi				= 0
	local hemi_min			= VOLET_DEF
	local hemi_max			= VOLET_DEF

	local attenuation		= 0
	local typo_alpha		= 0
	local typo_time			= 0
	local typo_time_b

	local pal_tracker		= 0
	local pal_alpha			= 0
	local pal_time
	local pal_inter

	local fake_id
	local compare			= 0
	local compare_begin
	local compare_end

	local packshot_begin

	local line_mid_alpha	= 0
	local oval				= 0
	local please			= 0
	local progress_bar

	local fn_linear = math.fn_linear

	local OVAL_NB = 80
	local exp = app:get_experience_code()

	--self:print( seq_id.." "..seq_id_name )
	self.b_seq_with_pal = exp=="HL" and inside( seq_id, 3, 6 ) or inside( seq_id, 3, 5 )

	if seq_id_name == "wait" then
--WAIT
		local otime = math.fmod( time, 4.0 )
		oval		= fn_linear( otime, 1,	0,0,	1.6,OVAL_NB/2,		2,OVAL_NB/2, 3.6,OVAL_NB	)
		self.oval_last = oval
		if exp=="HL" then
			typo_time = math.fmod( time * fps, 240 )
			typo_time = fn_linear( typo_time, 1,	0,0,	30,30,		225,31, 239,45	)
			--self:print( time.." -> "..typo_time )
		elseif exp=="DSA" then
			please		= fn_linear( time, 1,	0,0,	.6,15							)
			typo_time	= fn_linear( time, 1,	0,0,	1.2,0,	2,20					)
		end
		typo_alpha	= 1
	elseif seq_id_name == "welcome" then
--WELCOME
		if self.seq_id_name_last ~= seq_id_name then
			-- .5 double speed when found
			self.seq2_oval_t_end = .5 * 3.2 * (OVAL_NB - self.oval_last)/OVAL_NB
		end
		oval		= fn_linear( time, 1, 	0, self.oval_last,	self.seq2_oval_t_end,OVAL_NB		)
		if exp=="HL" then
			face_on_alpha	= fn_linear( time*fps, 1,	164,1, 180,0	)
			typo_time		= fn_linear( time*fps, 1, 	0,0,	122,122,	153,123,	180,150		)
		elseif exp=="DSA" then
			please			= fn_linear( time, 1, 	0,15,	.6,30										)
			typo_time		= fn_linear( time, 1, 	0,20,	1.6,20,	6.6,145,	8,145,		8.4,155		)
		end
		typo_alpha	= 1
	else
--REST
		if exp=="HL" then
			--type is one long video spanning several sequences
			typo_time = self.seqs:get_time_from_title( "sorbet" )
			local duration = self.seqs:get_duration_from_title( "sorbet", "nature_c" )
			progress_bar = typo_time/duration
			typo_time = fn_linear( typo_time*fps, 1,		0,0,	6,6, 249,7, 261,19, 486,20, 498,32, 743,33, 755,45, 941,46, 953,58, 1226,59, 1238,71, 1325,72, 1337,84, 1432,85, 1444,97, 1717,98, 1743,124, 1877,125, 1889,137, 2162, 138, 2174,150, 2197,151, 2209,163, 2408,164 )
			typo_alpha	= 1
		end

		if self.b_seq_with_pal then
--WITH PALETTE L ALL EXCEPT END
			if exp=="HL" then
--HL: SORBET NATURE A B C
				pal_alpha	= 1

				if		seq_id_name == "sorbet"		then
					face_on_alpha	= fn_linear( time*fps,1,						191,1,		471,0	)
					face_on_time	= fn_linear( time*fps,1,	5,0,	165,160							)

					--PALETTE
					--local pal_start = 7.333333333333
					--local pal_stop = 17.01
					--pal_alpha		= fn_linear( time, 1, 		0,0,	pal_start,0,	pal_start+.001,1, pal_stop,1,		pal_stop+.001,0	)
					pal_tracker		= fn_linear( time*fps, 1,	137,0,	505,368, 505.0001,0 )
					pal_time		= fn_linear( time*fps, 1, 	222,0,	422,200, 492,201, 520,229	)
					--self:print( pal_alpha, pal_time )

					--COMPARE
					fake_id = 1
					compare_begin	= 17 + 1/fps
					compare_end		= 23 + 13/fps
				else
					local time_nature = self.seqs:get_time_from_title( "nature_a" )
					face_on_alpha	= fn_linear( time_nature*fps,1,	1451,1,		1496,0	)
					face_on_time	= fn_linear( time_nature*fps,1,	20,0,	134,114,	579,115,	731,267,	1002,268,	1096,362	)

					if		seq_id_name == "nature_a"	then
						pal_tracker		= fn_linear( time*fps, 1,	86,0,	454,368, 454.0001,0 )
						pal_time		= fn_linear( time*fps, 1, 	155,0,	522,367	)
					elseif	seq_id_name == "nature_b"	then
						pal_tracker		= fn_linear( time*fps, 1,	0,0,	368,368, 368.0001,0 )
						pal_time		= fn_linear( time*fps, 1, 	65,0,	432,367	)
					elseif	seq_id_name == "nature_c"	then
						pal_tracker		= fn_linear( time*fps, 1,	0,0,	368,368, 368.0001,0 )
						pal_time		= fn_linear( time*fps, 1, 	42,0,	409,367	)
						--COMPARE
						fake_id = 2
						compare_begin	= 14 + 19/fps
						compare_end		= 21 + 01/fps
					end
				end
				pal_tracker		= pal_tracker / fps
				pal_time		= pal_time / fps
			elseif exp=="DSA" then
--DSA : TEXTURE COLOR LIGHT
				face_on_alpha = 1
				compare = fn_linear( time, 1,	0,0,	13,0,		13.4,1,		19,1,		19.4,0,						1000,0	)
				--self:print( "compare "..compare )

				local lb = .16 -- going to 8%
				if seq_id_name == "texture" then
					attenuation = fn_linear( time, 1,	0,0,	.6,1,		15.2,1,		16,lb,		19.6,lb,	20,0	)
				else
					attenuation = fn_linear( time, 1,	0,0,	.6,1,		13.2,1,		14.2,lb,	19.6,lb,	20,0	)
				end
				attenuation = attenuation * -.3

				if seq_id_name == "texture" then
					hemi_min, hemi	= fn_linear( time, 2,	0,0,0,	13,0,0,		15,1,.75,	19.8,1,1,	20,1,0		)
					line_mid_alpha	= fn_linear( time, 1,	0,0,	15,0,		15.5,1,		19.8,1,		20,0		)
				else
					hemi_min, hemi	= fn_linear( time, 2,	0,0,0,	7,0,0,		10,1,.75,	19.8,1,1,	20,1,0		)
					line_mid_alpha	= fn_linear( time, 1,	0,0,	10,0,		11,1,		19.8,1,		20,0		)
				end
				hemi_min = 1 - .495 * hemi_min

				local inter, alpha 	= fn_linear( time, 2, 	0,0,0,	6.6,0,0,	7,1,1,		13.2,1,1,	13.6,1,0	)

				pal_time = time - 6.6
				pal_alpha = alpha
				pal_inter = inter

				pal_tracker = inside( time, 6, 12.4 ) and 1 or 0

				typo_alpha = fn_linear( time, 1,	0,0,	.99,1,	1,1,	19,1,	20,0	)
				typo_time = fn_linear( time, 1,		0,0,	1,0,	1.2,5,	1.4,5,	2.2,25,	7,25,	8,50,	18.8,50, 19.4,65	)

				if		seq_id_name == "texture"	then	face_on_alpha = clamp_01( (14.2-time)/(14.2-13.2) )
				elseif	seq_id_name == "color"		then	face_on_alpha = clamp_01( (5-time)/(5-4.9) )
				elseif	seq_id_name == "light"		then	face_on_alpha = clamp_01( (5-time)/(5-4.9) )
				end
			end
		elseif seq_id_name == "end" then
--END
			if exp=="HL" then
				local t_tra_line	= 0. + 22/fps
				local t_wait		= 18/fps
				local a				= 0
				local b				= a	+ t_tra_line
				local c				= b	+ t_wait
				local d				= c	+ t_tra_line
				local dt			= .000001

				compare_begin	= 3 + 20/fps
				compare_end		= 8 + 25/fps

				if time <= compare_begin then
					fake_id		= fn_linear( time,1,	a,3,	b,3,			c,3, c+dt,4,		d,4	)
					hemi_min	= fn_linear( time,1,	a,1,	b,0,	c-dt,0, c,1,				d,0	)

					hemi_min = -.1 + 1.1*(1-hemi_min)
					b_hemi_image = true
				else
					fake_id = 5
				end
				hemi =1
				--self:print( fake_id.." "..hemi_min )

				packshot_begin = 10. + 22/fps
			elseif exp=="DSA" then
				local b_long		= true
				local t_tra_img		= .4 					--transition time
				local t_tra_line	= b_long and 2 or .4 	--transition time
				local t_wait		= 0.6	--wait time
				local t_off 		= (t_tra_line-t_tra_img) * 3.
				local t2			= time - t_off
				local a				= t_tra_line
				local b				= t_tra_line		+ t_wait
				local c				= t_tra_line * 2	+ t_wait
				local d				= t_tra_line * 2	+ t_wait * 2
				local e				= t_tra_line * 3	+ t_wait * 2
				local dt			= .000001


			--SEQ
				--local st =
				typo_alpha 		= fn_linear( t2,1,		0,1,					7.8,1,		7.04,0	)
				compare			= fn_linear( t2,1,		0,0,	3.6,0,	4,1, 	7,1,		7.4,0	)

				--hemi_min = 0
				--typo_time = 80
				if time <= e then
					fake_id		= fn_linear( time,1,	0,3,							b,3,	b+dt,4,							d,4,	d+dt,5			)
					hemi_min	= fn_linear( time,1,	0,1,	a,0,			a+dt,1,	b,1,			c,0,			c+dt,1,	d,1,	e,0				)
					typo_time	= fn_linear( time,1,	0,0,	t_tra_img,10,			b,25,			b+t_tra_img,35,			d,50,	d+t_tra_img,60	)

					hemi_min = -.1 + 1.1*(1-hemi_min)
					b_hemi_image = true
					--hemi_min,hemi = 0.5, 0.5
				else
					fake_id = 5
					typo_time		= fn_linear( t2,1,	2.4,60,	7,60,	7.8,80						)

					hemi_min, hemi	= fn_linear( t2,2,	0,0,0,	3,0,0,	4,1,1,	7,1,1,	7.8,0,0		)
					line_mid_alpha	= fn_linear( t2,1,	0,0,	3.6,0,	4,1,	7,1, 	7.4,0		)
					hemi_min = .505 * hemi_min

					typo_time_b = fn_linear( t2, 1, 	0,-10000,	7.99,-10000, 8,0,	 9.2,31		)
					typo_time_b = typo_time_b / fps
				end

				packshot_begin = 12. + t_off
			end

			attenuation = 0
		else
			self:print( "sequence named \""..seq_id_name.."\" has to be dealt with" )
		end
	end

	if exp=="HL" then
		if compare_begin and inside( time, compare_begin, compare_end ) then
			local l = exp=="HL" and .8 or .4
			compare			= fn_linear( time, 1,	0,0,	compare_begin,0,	compare_begin+l,1,	compare_end-l,1,	compare_end,0	)
			hemi_min		= fn_linear( time, 1,	0,0,	compare_begin,0,	compare_begin+l,1,	compare_end-l,1	)
			hemi			= compare
			line_mid_alpha	= compare
			hemi_min = 1 - .495 * hemi_min
		end
	end

	self.face_on_alpha		= face_on_alpha
	self.face_on_time		= face_on_time and (face_on_time / fps) or (time-1)

	self.b_hemi_image 		= b_hemi_image
	self.hemi 				= hemi
	self.hemi_min 			= hemi_min
	self.hemi_max 			= hemi_max

	self.pal_alpha			= pal_alpha
	self.pal_time			= pal_time
	self.pal_inter			= pal_inter
	self.pal_tracker 		= pal_tracker

	self.typo_alpha 		= typo_alpha
	self.typo_time 			= math.floor(typo_time)	/ fps
	self.typo_time_b		= typo_time_b
	self.line_mid_alpha		= line_mid_alpha * self.presence
	self.packshot 			= packshot_begin and fn_linear( time,1,		0,0,							packshot_begin,0,	packshot_begin+.01,1				) or 0

	self.packshot_white		= packshot_begin and fn_linear( time,1,		0,0,	packshot_begin-.2,0,	packshot_begin,1,	packshot_begin+1.,0				) or 0

	self.compare			= compare
	self.oval 				= math.floor(oval)		/ fps
	self.please				= math.floor(please)	/ fps
	self.progress_bar		= progress_bar

--COLOR CORRECTION
	self.value_max_offset_before	= attenuation

	if exp=="HL" then
		fake_id = fake_id and fake_id+1 or 1
		-- 												1 inactive, 	2 sorbet, 3 nature, 		456 end : 4 first hemimage, 5 second hemimage, 6 hemiface
		self.hue_before					= 0	--select(	fake_id,	0,	0, 0,						0, 0, 0	)
		self.sat_before					= 0	--select(	fake_id,	0,	0, 0,						0, 0, 0	)
		self.value_min_before			= 0	--select(	fake_id,	0,	0, 0,						0, 0, 0	)

		local hue_offset = -.000
		local sat_offset_1 = .02
		local sat_offset_2 = sat_offset_1 * 1.8
		local val_offset = .15
		self.hue_after					= select(	fake_id,	0,	0, hue_offset,					0, hue_offset, hue_offset	)
		self.sat_after					= select(	fake_id,	0,	sat_offset_1, sat_offset_2,		sat_offset_1, sat_offset_2, sat_offset_2		)
		--self:print(  )
		self.value_min_after			= select(	fake_id,	0,	0, val_offset,					0, val_offset, val_offset	)
		self.value_max_offset_after		= select(	fake_id,	0,	0, 0,							0, 0, 0	)

	--	BLUR
	--	self.blur 						= select(	sel,		0, 0,	.2, .25, .3,		.3, 0, 1.		)
		self.blur 						= select(	fake_id,	0,	.1, .23,						.1, .23, .26		)
		self.skin_hue					= select(	fake_id,	0,	0, .2,							0, .2, .25			)

		--self.hue_before = .2
		--self.hue_after = -.2
	elseif exp=="DSA" then
		fake_id = fake_id or seq_id
		self.value_min_before			= select(	seq_id,		0, 0,	0, 0, 0,			0, 0, 0.		)
		self.value_max_offset_after		= select(	seq_id,		0, 0,	0, 0, 0,			0, 0, 0.		)
		self.value_min_after			= select(	seq_id,		0, 0,	0, 0, .1,			0, 0, 0.		)

		self.sat_before					= select(	seq_id,		0, 0,	0, .0, .0,			0, 0, .04		)
		self.hue_before					= select(	seq_id,		0, 0,	0, .0, .0,			0, 0, .04		)

	--	BLUR
	--	self.blur 						= select(	sel,		0, 0,	.2, .25, .3,		.3, 0, 1.		)
		self.blur 						= select(	fake_id,	0, 0,	.2, .23, .26,		.3, 0, 1.		)
		self.sat_after					= select(	fake_id,	0, 0,	0, .05, .025,		.025, 0, .04	)
		self.hue_after					= select(	fake_id,	0, 0,	0, -.005, -.0025,	-.0025, 0, .04	)
		self.skin_hue					= select(	fake_id,	0, 0,	0, .2, .3,			-.0025, 0, .04	)
	end
end

function meu:draw_central()
	local seq			= self.seq
--	local seq_id		= seq:get_seq_id()
	local seq_id_name	= string.lower( seq:get_title() )
	local b_seq_end		= seq_id_name=="end"

	local face_info		= self.face_info

	local exp = app:get_experience_code()

--BACKGROUND
	self:update_shader( 5, b_seq_end and self.hemi_min or VOLET_DEF, self.hemi_max, false, b_seq_end and 0 or 1, self.b_hemi_image, self.b_hemi_image )
	self:draw_background( 1. )

	--aaa.debug.show( self.face_info.certainty, "Certainty" )
	--aaa.debug.show( self.s_certainty, "Use Certainty" )
	--self:print( "s_certainty is "..self.s_certainty )
	local b_tracking = (self.s_certainty==0) and ((face_info.certainty or 0) > 1) or (self.s_certainty==1)

	--aaa.debug.show( b_tracking, "Use Certainty" )
	if b_tracking then
		self.__b_points_ready = true 		--avoid access to non existing points
--RECT
		if not b_seq_end then
			self:update_shader_no()
			self:draw_face_rect( face_info )
		end
--FACE
		if seq_id_name ~= "wait" then
			gol.color_white( 1. )
			--gol.reset()
			--gol.set_default()
			self:update_shader( nil, self.hemi_min, self.hemi_max, true, self.hemi, b_seq_end, b_seq_end and true or self.b_border )
--ANIM ON FACE
			gol.color_white( 1. )
			local anim = app:get_anim( seq, "face_on" )
			if anim then
				anim:set_loop( false )
				--self:set_color( "color_seq", self.alpha )
				self:draw_amin_on_face( anim, self.face_on_time )
			end
--FACE on seq 6 : sumup at end
			if seq_id_name == "end" then
				--sid, hemi_min, hemi_max, b_mask, hemi, b_hemi_image, b_border
				--self:update_shader( 4, self.hemi_min, self.hemi_max, true, self.hemi, b_seq_end, b_seq_end and true or self.b_border )
				--self:update_shader( 4, .4, .6, false, 1, true, true )
				--self:print( self.hemi_min )
				self:update_shader( 5, self.hemi_min, 1.2, true, self.hemi, self.b_hemi_image, self.b_hemi_image )
				face_info:draw_tri_tex( self.pts, self.uvs, false )
			end
--ANIM on Top
			if seq_id_name == "color" or seq_id_name == "light" then
				local m = self:get_meu_by_name( seq_id_name == "color" and "HemColor_1" or "HemLight_1" )
				local ph = 1 - self.hemi_min
				m:set_phase( ph )
				--self:print(  ph )
				if ph > 0 then --avoid intermediate bug at start of seq
					self:set_tex( app.binds.pre ) -- GP fbo PRE
					--					sid,	hemi_min,		hemi_max,		b_mask, hemi,	b_hemi_image, 	b_border
					self:update_shader( 7,		self.hemi_min,	self.hemi_max, true,	0,		false,			false		)
					face_info:draw_tri_tex( self.pts, self.uvs, false )
				end
			end

			--self:print( "imgs_id is "..self.img_id )
			if self.img_id > 0 then
				self:draw_img( self.alpha )
			end

			if self.b_draw_face then -- hemi_min, b_mask, hemi, b_hemi_image, b_border
				--self:update_shader( nil, self.hemi_min, self.hemi_max, true, b_seq_end and 1 or self.hemi, b_seq_end, b_seq_end and true or self.b_border )
				--self:print( self.b_face_dup )
				self:draw_face( self.alpha, self.s_duplicate )
			end

			self:update_shader_no()
--	VISU
			self:draw_visu( self.alpha )
			--self:draw_mapped( 1 )

--LINE ON MIDDLE OF FACE
 			self:draw_line_mid()
			--if true then return end
		end
	end

	self:update_shader_no()
--WELCOME
	gol.color_white( 1. )
	if seq_id_name=="welcome" and self.__b_points_ready then
		if exp=="DSA" then
			self:draw_welcome_dsa( self.presence )
		end
	end

	self:update_shader_no( false )
	gol.color_white( 1. )
	gol.set_texture_dim( 2 )

--CALAGE REF
	if self.b_draw_calage and app.img_calage then
		self:draw_img_bottom( app.img_calage[1] )
	end

	if self.b_draw_elts then
		self:update_shader_no()

		self:draw_logo_top()

		self:draw_oval()
		self:draw_please()

--WITHOUT / WITH
		self:draw_compare()

--TEXTURE COLOR LIGHT
--CROSSHAIR and PALETTE
		self:draw_pal()

--TYPO LOW
		if self.b_draw_typo then
			self:do_anim_typo( "typo",		self.typo_alpha,	self.typo_time		)
			self:do_anim_typo( "typo_b",	1,					self.typo_time_b	)
			self:draw_progress_bar( self.progress_bar )
		end

		self:draw_packshot( self.packshot, self.packshot_white )
	end

	self.seq_id_name_last = seq_id_name

--	self.ui.color_img:print( "titi" )
	--table.print( ccalls )
end

-- 2023 Sept had to reintegrate these 2 fns from another file (not sure it is the right fn)
function meu:set_tex( bind, texture_unit )
	if bind then
		if texture_unit or self.b_shading then
			local bank_2d, bind_2d = aaa.img.make_bank_bind_2d( bind )
			local ref = self.ref
			local t = ref.tex[ texture_unit or 4 ]
			param.set( t.bank_2d, bank_2d )
			param.set( t.bind_2d, bind_2d )
			if not texture_unit then
				self:draw_layer( 2 )
			end
	 	else
	 	 	gol.bind_texture( bind )
		end
	else
		self:print( "no_bind_here" )
	end
end
function meu:set_color( name, alpha )
	local cui = self.ui[name]
	local r,g,b, a = cui:get_rgba()
	gol.color( r,g,b, a*alpha )
end

function meu:draw()
	--ccalls = {}
--	gol.reset()
--	gol.set_default()
	self:draw_before()
	if not self.seq then return end

	self:compute()
		--if self.blur > 0 then
		local m = self.meu_blur
		if not m then
			m = self:get_meu_by_name( "diorblur_1" )
			self.meu_blur = m
		end
		m:set_blur( self.blur )
	--end

	self:draw_layers_begin()

		self:set_tex(	app.binds.src,					1 )
		self:set_tex(	app.binds.blur,					2 )
		self:set_tex(	app.img_fix[1]:get_bind(),		3 )
		gol.color_white( 1. )

		if self.face_info then
			self:draw_central()
		else
			self:print_error( "No face_info" )
		end

	self:draw_layers_end()
end

aaa.show_file_end( "dior_draw" )