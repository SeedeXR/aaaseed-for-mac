
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()

	bu = self:add_button(	{12,	2 },				"Grid", 		self,		"b_draw_grid",			true	)

	local iy = 1

	iy = iy + 1
		bu = self:add_button(	{1,	iy },				"Screens", 		self,		"b_draw_screens",		true	)
	iy = iy + 1
		bu = self:add_button(	{2,	iy },				"Cross", 		self,		"b_screen_crosss",		true	)
	iy = iy + 1
		bu = self:add_button(	{1,	iy },				"Pilot", 		self,		"b_pilot",		true	)

	local function add_slider( name )
		return self:add_slider(	{1,iy,	8,1},			name, 		self,			name.."_alpha",		0	)
	end
	iy = iy + 2
		self:add_button(	{1,	iy },				"Force Elt", 		self,		"b_force_elt",			true	)
	iy = iy + 1
		add_slider( "logo" )
	iy = iy + 1
		add_slider( "logo_c" )
	iy = iy + 1
		add_slider( "connect" )
	iy = iy + 1
		add_slider( "give" )
	iy = iy + 1
		add_slider( "logo_big" )
	iy = iy + 1
		add_slider( "logo_uplifted"	)
	iy = iy + 1
		add_slider( "record"	)




	iy = iy + 10
		self.ui.color = self:add_rgba(	{5,iy,	nil,1}, "color", false )

	iy = iy + 1
		bu = self:add_selector(	{1,iy,	4,1},	"Imgs"	)
			--bu:set_text_draw( false )
			bu:set_nb_min_0( 8, 1 )
			bu:set_target_lua( self, "img_id" )
			bu:set_item_text( 1, "No" )
		self.ui.color_img = self:add_rgba(	{5,iy,	nil,1}, "img_col", false )

	iy = iy + 1.4
end

--
--
local SX = 8.*.5
local l,r = -SX, SX
local SY = SX * ( app.ratio_y or 1)
local b,t = -SY, SY
local function draw_rect_full()
	aaa.draw_rect_uv( l,b,	r,t )
end

function meu:set_tex( bind, texture_unit )
	if bind then
	 	 gol.bind_texture( bind )
	else
		self:print( "no_bind_here" )
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
end
--move in utils

function meu:draw_screens()
	gol.set_texture_dim( 0 )
	gol.set_line_width( 4 )
	gol.color_green()
	local sx = SY * 9. / 16.
	local function draw_screen( x )
		local l,r = x-sx, x+sx
		aaa.draw_rect_line(	l, -SY, r, SY )
		if self.b_screen_crosss then
			aaa.draw_line(		l, -SY, r, SY )
			aaa.draw_line(		r, -SY, l, SY )
		end
	end
	draw_screen( -4+sx )
	draw_screen( 0 )
	draw_screen( 4-sx )
end

function meu:draw_grid()
	gol.set_texture_dim( 0 )

	local sx = SY * 9. / 16.

	gol.set_line_width( 4 )
	gol.color_cyan( .4 	)
	aaa.draw_lines_vert_nb( 33, -4,	.25, 	-SY, SY )
	aaa.draw_lines_hori_nb( 17, -2, .25,	-SX, SX )
	gol.color_cyan( .8 	)
	aaa.draw_lines_vert_nb( 9,  -4,	1,  	-SY, SY )
	aaa.draw_lines_hori_nb( 5,  -2,	1,  	-SX, SX )
end

function meu:draw()
	gol.set_depth( false )
	MEU.draw( self )

	local seq = ga:get_seq_cur()
	if not seq then return end
	self.seq = seq

	local id = seq:get_seq_id()
	self.ease = seq:get_ease_in()
	self.phase = seq:get_phase()
	local time = seq:get_time()
	local dur = seq:get_duration()


	local alpha = 1

	--self.pre = alpha


--	gol.reset()
--	gol.set_default()

	local b_fx_seq = inside( id, 3, 5 )
	if id == 1 then
--		typo_time = math.fmod( time, 5. )
--		typo = 1
	elseif id == 2 then
		--compare = math.fn_linear( time, 1, 	0,0,	14.6,0,		15,1, 19,1,	20,0, 	1000,0 	)
		--self:print( "compare "..compare )

	elseif id ==6 then

	end


--	self:set_tex( app.img_fix[1]:get_bind(), 3 )


--BACKGROUND
--	if self.b_draw_back then
--		self:draw_background( 1., id==7, id==2 )
--	end



		--if self.img_id > 0 then
		--	self:draw_img( alpha )
		--end

		gol.set_texture_dim( 0 )
		gol.set_line_width( 3 )
		gol.set_line_smooth( true )

	gol.set_texture_dim( 2 )


	--self:print( self.logo_alpha )

	local TNEW = 40
	local TSPEED = 40
	local TNEW_PLAY = TNEW / 2
	local TSPEED_PLAY = TSPEED / 2
	local NORMAL = 1.5
	local REC_BEGIN = 6.
	local REC_END = 18.
	local logo_alpha			=	1
	local logo_big_alpha		=	0
	local logo_c_alpha			=	0
	local connect_alpha			=	0
	local give_alpha			=	0
	local logo_uplifted_alpha	=	1
	local record				=	0
	local jauge					=	0
	local sound_wave			=	0
	local color					=	1
	local normal				=	NORMAL
	local tnew					=	0
	local tspeed				=	0
	local volume				=	0

	aaa.show( id, "seq" )
	if id == 1 then		--we wait
		connect_alpha = 1 - app.presence_global
		tnew = TNEW
	elseif id == 2 then	--we got presence
		tnew = TNEW
	elseif id == 3 then	--we play
		tnew = TNEW_PLAY
		tspeed = TSPEED_PLAY
	elseif id == 4 then	--we record
		local fn_linear = math.fn_linear_all
		record 		= fn_linear( time, 1, 	0,0,	REC_BEGIN-.01,0,	REC_BEGIN,1,	REC_END-.01,1,	REC_END,0,		dur,0  	)
		logo_alpha = fn_linear( time, 1, 	0,1,	REC_BEGIN,0,	REC_END,0,		REC_END+1,1, dur,1 	)
		logo_big_alpha = fn_linear( time, 1, 	0,0,	REC_END-2.5,0,	REC_END-2,1,		REC_END,1, REC_END+.5,0, dur,0 	)

		give_alpha	= fn_linear( time, 1, 	0,0,	1,0,	1.5,1,		4.5,1,		5,0, dur,0 	)
		aaa.show( give_alpha, "give" )
		color 		= fn_linear( time, 1, 	0,1,			.5,0, 		5,0,	6,1,		dur,1 )
		sound_wave	= fn_linear( time, 1, 	0,0,					2.5,0, 		3,1,	6,1,	7,0,	dur,0 	)
		normal 		= fn_linear( time, 1, 	0,NORMAL,		1,0,		dur-2,0, dur-1,NORMAL )
		tnew 		= fn_linear( time, 1, 	0,TNEW_PLAY,		5,0, 		dur-2,TNEW,		dur-1,TNEW_PLAY )
		tspeed		= fn_linear( time, 1, 	0,TSPEED_PLAY,		10,TSPEED,	dur-2,TSPEED,	dur-1,TSPEED_PLAY )
		volume		= fn_linear( time, 1, 	0,0,		1,1,	dur-3,1,	dur-1,0 )
		logo_uplifted_alpha = logo_alpha
	end

	if self.b_force_elt then
		logo_alpha			= self.logo_alpha
		logo_c_alpha		= self.logo_c_alpha
		connect_alpha		= self.connect_alpha
		give_alpha			= self.give_alpha
		logo_uplifted_alpha	= self.logo_uplifted_alpha
--		record				= self.record_alpha
	end

	local function draw_img( img, x, y, sx,		a, r, g, b,		off )
		if a and a<=0 then return end
		if off then
			gol.color_black( a )
			img:draw_xy_sx( x+off, y-off, sx )
		end
		gol.color(  r or 1, g or 1, b or 1, a or 1 )
		img:draw_xy_sx( x, y, sx )
	end
	draw_img( app.img_logo,				3.25, -1.88	, 1	, 	logo_alpha,				1, .5, 0,	.01 )
	draw_img( app.img_logo_c,	 		0, 0, 1, 			logo_c_alpha,			1, .5, 0,	.03 )
	draw_img( app.img_connect,	 		0, 0, 3.8, 			connect_alpha,			1, 1, 1,	.03 )
	draw_img( app.img_give,		 		0, 0, 3.8, 			give_alpha,				1, 1, 1,	.02 )
	draw_img( app.img_logo_uplifted,	-3.05, -1.74, .7, 	logo_uplifted_alpha,	1, 1, 1,	.01 )
	draw_img( app.img_logo,				0, 0, 2.,			logo_big_alpha,			1, 1, 1,	.01 )

	if self.b_pilot then
		aaa.show( volume, "volume" )
		app.meu_sound:set_volume( volume )
		local vol_last = self.vol_last or 0
		if vol_last==0 and vol_last ~= volume then
			app.meu_sound:restart()
		end
		self.vol_last = volume
		app.meu_clari:set_bu_value( "Sound", sound_wave )
		app.meu_clari:set_bu_value( "Color", color )
		aaa.show( normal, "normal" )
		app.meu_clari:set_bu_value( "normal", normal )
		app.meu_clari:set_bu_value( "trail new", tnew )
		app.meu_clari:set_bu_value( "trail speed", tspeed )
		app.mu_obj3d:set_value( logo_uplifted_alpha )
		aaa.show( record, "record" )
		local writer = app.meu_writer
		if record > 0 then
			if not self.b_recording then
				local index = self.record_index or 0
				index = index + 1
				self.record_index = index
				self.name_index_high	= math.floor( index / 26 ) + 1
				self.name_index_low		= (index % 26) + 1
				local name = "ClariSonic_"..string.char( 64 + self.name_index_high  , 64 + self.name_index_low )
				--self.print( name )
				writer:set_filename( name )
				self.b_recording = true
				writer:start_video()
			end
			local S = .65
			local L = 3.32
			local B = 1.92
			draw_img( app.img_record,			L, B, 1.8*S, 		record,					1, 1, 1,	.01 )
			local l = L - .475 * S
			local b = B - .21 * S
			local s = 1.08 * S

			local sf = .4
			draw_img( app.img_alphabet[self.name_index_high],	L-.05, 		B-S*sf, S*sf,			1,	1, 1, 1,	.01 )
			draw_img( app.img_alphabet[self.name_index_low],	L-.05+S*sf*.7, B-S*sf, S*sf,		1,	1, 1, 1,	.01 )

			gol.set_texture_dim( 0 )
			gol.color_white( 1. )
			local jau = (time-REC_BEGIN)/(REC_END-REC_BEGIN)
			aaa.draw_rect_line( l, b, l+s, b + .05*S )
			aaa.draw_rect( l, b, l+jau*s, b + .05*S )

			gol.color_red( math.sin( time * 10) )
			aaa.draw_disk_axe_z( L-.65 * S, B-.04 * S, 0, .16*S, 36 )

			--aaa.draw_disk_axe_z( L-.65, B-.08, 0, .24, 36 )
		else
			writer:stop_video()
			self.b_recording = false
		end
	end

	if self.b_draw_grid then
		self:draw_grid()
	end

	if self.b_draw_screens then
		self:draw_screens()
	end
end
