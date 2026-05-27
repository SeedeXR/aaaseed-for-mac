
function ROMA:do_key_special( key )
	if key == 105 or aaa.keyboard.is_key_fn( key, 5 ) then
		self.b_vdebug = not self.b_vdebug
		return true
	end
	return oo.getsuper(ROMA).do_key_special( self, key )
end

function ROMA:select_video( id )
	self:print( "select_video  "..id )
	local sub = self.subs_all[id]
	local video = sub.video
	video:set_focus()
	local bu = self.ui.bu_video
	bu:assign_video( video )
	local sy = 3
	bu:place_video_at_start()
	bu:play_video()
	bu:set_sxy( sy*16/9, sy )
	bu:set_video_button( true )
	bu:set_video_scrub( true )

	bu = self.ui.bu_bandeau
	bu:assign_img( sub.title )
end

function ROMA:stop_video()
	local bu = self.ui.bu_video
	bu:stop_video()
end
function ROMA:set_video_volume( vol )
	--local video = self.ui.bu_video:get_video()
	aaa.audio.set_volume( vol )
end
function ROMA:pick_video( id )
	self:print( "pick_video  "..id )
	if self.s_video == "off" then
		self.s_video = "up"
		self:select_video( id )
	elseif self.s_video == "up" or self.s_video == "on" then
		self:do_click_back()
	end
end

function ROMA:print_click( name )
	self:print( "print_click on "..name )
	if self.s_video == "off" then
		self.s_video = "up"
		self:select_video( 1 )
	elseif self.s_video == "up" or self.s_video == "on" then
		self:do_click_back()
	end
end
function ROMA:do_click_back()
	self:print( "do_click_back()" )
	if self.s_video == "on" or self.s_video  == "up" then
		self.s_video = "down"
	end
end
function ROMA.do_click_down( bu, x, y)
	aaa.print_fn( "ROMA.do_click_down" )
	roma.x_begin = x
	roma.y_begin = y
	local self = roma
	--self:print( self.s_video )
	if y < -.47 and self.s_video == "off" then
		self.s_video = "up"
		--aaa.print( "restart the whiole thing" )
		local bu = self.ui.bu_video
		bu:play_video()
	else
		self:do_click_back()
	end
end
function ROMA.do_click_up( bu, x, y)
	aaa.print_fn( "ROMA.do_click_up" )
	local self = roma
	roma.x_begin = nil
	roma.y_begin = nil
	roma.b_ph_release_trig = true
end
function ROMA.do_mouse_move( bu, x, y)
--aaa.print_fn()
	local self = roma
	roma.ph_dx = roma.x_begin - x
	roma.ph_dy = roma.y_begin - y
end

function ROMA:set_section( id )
	self:print( "set_section( "..id )
	if id ~= self.section_target then
		self.section_target = id
		self.section_inter = 0
	end
end
function ROMA:main_define( rect )
	local bu
	local bus = BUS:create( "main" )
	bus:init_begin()

	local function add_but( x,y, name )
		local bu = bus:add_button( name, {x,y, .25,.25} )
		--bu:set_text_xyf( -1, 0, .5 )
		--bu:set_method_on_click( self, "print_click", name )
		return bu
	end
	if false then
		for i=1,16 do
			bu = add_but( i, 0, "b"..i )
			bu = add_but( i, 1.2, "bb"..i )
			--bu = add_but( i, 2, "bbb"..i )
			bu:set_trig()
		end
	end

	--bu = bus:add_button( "groupement", {0,.3, 2,.6} )
	--	bu:assign_img( self.imgs.groupement, nil , 1 )
	local x = 1.25
	local y = .7
	local dy = .3
	local sy = rect[4] * .66
	local imgs = self.imgs
	bu = bus:add_button_image( "acd",		{x, rect[4]+dy,			nil, sy},			imgs.acd,		true )
	bu = bus:add_button_image( "pns",		{x-.055, rect[4]+.02,	nil, sy*.85},		imgs.pns,		true )
	bu = bus:add_button_image( "emb",		{x, rect[4]-dy,			nil, sy}, 			imgs.emb,		true )
	bu = bus:add_button_image( "romanite",	{4., rect[4], 			nil, rect[4]*2.},	imgs.romanite,	true )
	x = 11
	y = rect[4] + .02
	local dx = 2.2
	sy = rect[4] * 1.2
	self.ui.bu_section = {}
	bu = bus:add_button_image( "museo",		{x-dx,y, 	nil,sy},	imgs.museo, true )
		bu:set_method_on_click( self, "set_section", 1 )
		self.ui.bu_section[1] = bu
	bu = bus:add_button_image( "public",	{x,y, 		nil,sy},	imgs.public, true )
		bu:set_method_on_click( self, "set_section", 2 )
		self.ui.bu_section[2] = bu
	bu = bus:add_button_image( "motion",	{x+dx,y, 	nil,sy},	imgs.motion,true )
		bu:set_method_on_click( self, "set_section", 3 )
		self.ui.bu_section[3] = bu

	bu = bus:add_button_image( "title",		{9.4,.3, 	13.5},		self.subs_all[1].title, true )
		bu:set_ui_active( false )
		--bu:set_method_on_click( self, "set_section", 3 )
		self.ui.bu_bandeau = bu



	bus:init_end()
	return bus
end

function ROMA:ui_define()
	local ratio_x = 1.77
	local taille_x = 3 * ratio_x;
	local taille_y = 3;
	local bu
	local noms

	local bus = BUS:create( "slider_test" )
	bus:init_begin()

	--START

	--bus:inc_page()
	local	ortho_sy = 8 / ga.cam:get_ratio_x()

	bu = bus:add_button( "back", {0,0, 8,8*9/16}  )
		bu:set_dplane( -42 )
		bu:set_visible( false )
--		bu:set_method_on_click( self, "do_click_back" )
		bu.do_click_down = ROMA.do_click_down
		bu.do_click_up = ROMA.do_click_up
		bu.do_mouse_move = ROMA.do_mouse_move

	local function add_but( x, y, name )
		local bu = bus:add_trig( name, {x,y, .5,.5} )
		bu:set_text_xyf( -1, 0, .5 )
		bu:set_method_on_click( self, "print_click", name )
		return bu
	end
	local put = {}
	for i=1,20 do
		local y = math.floor(i/12)
		bu = add_but( -3.4 + math.fmod(i,12)*.6, y, "p"..i )
		bu:set_method_on_click( self, "pick_video", i )
		table.insert( put, bu )
	end
	self.bictos = put

	local sy = .5
	local sx = 8
	local rect
	rect = { 0,-ortho_sy*.5+sy*.5+.1, sx,sy }
	bu = bus:add_window( "main", self:main_define( rect ),	rect )
		bu:set_mobile( false )
		bu:set_method_on_click_double()
		self.ui.bu_main = bu
		bu.draw = ROMA.draw_main_bu
		bu.rect = rect
	sy = 3
	rect = { x=0, y=0, sx=sx*16/9, sy=sy }
	bu = BU:create( "Video", {0,-5, sy*16/9,sy} )
		bu = bus:add_bu( bu )
		bu:set_video_button( true )
		bu:set_video_scrub( true )
		bu:set_mobile( false )
		bu:set_video_loop( true )
		--hack
		bu.is_video_button = function()	return true end
--	bu = bus:add_button( "video", {0,-5, sy*16/9,sy} )
		bu:set_dplane( 42 )
		self.ui.bu_video = bu

--	bus:inc_page()

--	bus:make_bus_loop()

	--the scrolling bu is on the bottom
--	bus:add_scroll( -4, -2 )

	bus:init_end()
	return bus
end

function ROMA:do_buss_top_open()
	self:box_good( "Roma top" )

	local bus = self:ui_define()

	self.bus_ui = bus
	self.ph_video = 0
	self.s_video = "off"
	self.s_video = "off"
	self.section_cur = 1
	self.section_target = 1
	self.sub_cur = 1
	local c = self.sections[self.section_cur].center
	self.cam_center = { x=c.x, y=c.y, z=c.z }
	self.cam_speed = { x=0, y=0, z=0 }
	ga:get_bus_ctx():get_top():add_down( bus )

	-- ga:set_ui_group_active( true )
	ga:set_ui_group_active( false )
end

if IS_BUSS_OPEN() then
	--aaa.box_good( "Roma" )
	APP.CREATE_INST( ROMA )
elseif IS_BUSS_TOP_OPEN() then
	if aaa.lua.global.get( "roma" ) then
		roma:do_buss_top_open()
	end
end
