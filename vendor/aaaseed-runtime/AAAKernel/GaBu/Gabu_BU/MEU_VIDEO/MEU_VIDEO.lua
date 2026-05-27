if CLASS.DECLARE( "MEU_VIDEO", MEU ) then
	MEU_VIDEO:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
									"Read video in a texture, the video player",
									"Handle its own incrustation",
									"Used in combination with MEUs PIP in general" )
	MEU_VIDEO.CAPTURE_TYPE_DS = 1
	MEU_VIDEO.CAPTURE_TYPE_DISPLAY = 2
end

function MEU_VIDEO:define_meu_infos( )
	return { 	author = "Mâa",
				tags = { "Texture", "Vj", "Core", "CoreGraphic", "2d", "Sound" },
			}
end

function MEU_VIDEO:define_ui_proto( b_ui )
	if b_ui then
		--aaa.box_good( self..":define_ui_proto()" )
		self:add_button( {1,1}, "allow",	self, "b_allow",  true )
		self:add_button( {1,2}, "On CPU",	self, "b_on_cpu", true )
		self:add_button( {1,3},	"On GPU",	self, "b_on_gpu", true )
	else
		self.b_allow	= true
		self.b_on_cpu	= true
		self.b_on_gpu	= true
	end
end


function MEU_VIDEO:define_ui()
	local ui = self.ui
	local video = self.video
	local vref = video.ref

	--local par = param.get_ref( r, "video_name" )
	--	bu:set_target_param( par )


--	self.ui.bu_info = self:add_text_info( 13, 2 )
	local ix = 1
	local iy = 1
	local DY = .2
	local SY = 1

	local bu

-- MAIN
	self:add_button(		{ix,iy,		3,SY},		"Capture",	vref.capture	)

	self:add_text_info(		{ix+3,iy,	10,.5},		"Video Info" )

	self:add_trig_method(	{ix+13,iy,	3,SY},		"Focus",	video,"set_focus"	)
	iy = iy + SY + DY


-- VIDEO
	local iy_beg = iy
	if false then
		bu = self:add_selector(	{1,iy,	8,8}) 	--	"bind" }
			bu:set_nb_min_0( 16, 16 )
			for i=1,32,4 do
				local v = (i-1)*4+1
				bu:set_item_text( v, v-1 )
			end
			bu:set_target_param( vref.src )
			bu:set_method_on_click_double( self, "load_video" )
			self.ui.bu_video_bind = bu
		iy = 4.8	
	else
		bu = self:add_selector(	{1,iy,	4,SY*2},		"bank" )
			bu:set_nb_min_0( 4, 3 )
			bu:set_item_text( 1, 0 )
			bu:set_item_text( 5, 4 )
			bu:set_item_text( 8, "Oh!" )
			bu:set_item_text( 9, "App"  )
			bu:set_method_on_click_double( self, "load_video" )
			self.ui.bu_video_bank = bu

		bu = self:add_selector(	{5,iy,	4,SY*2},		"bind" )
			bu:set_nb_min_0( 8, 4 )
			for i=1,8 do
				local v = (i-1)*4+1
				bu:set_item_text( v, v-1 )
			end
			bu:set_method_on_click_double( self, "load_video" )
			self.ui.bu_video_bind = bu
		iy = iy + SY*2 + DY
	end

	--self:add_param_obj_name(	{ix+1,iy+1}, 	"play",		vid, "play"				)
	bu = self:add_button( 	{ix,	iy,	2,SY},	"Play", 	self, "b_play",			true	)
	--	bu:set_text_rect_ratio(2)
	bu = self:add_button(	{ix+2,	iy,	2,SY},	"Loop",		vref.loop			)
	--	bu:set_text_rect_ratio(2)
	bu = self:add_trig(		{ix+5,	iy,	3,SY},	"Restart",	vref.restart		)
		bu:set_text_rect_ratio(3)
	iy = iy + SY + DY

	local nx = 2.8
	local ny = .8
	local ox = .1
	local oy = .3
	local OY = .3
	local SX = 4-nx

	bu = self:add_slider(	{ix,	iy,		4,SY},		"Speed",		vref.time_factor,nil, 	1,	0,8		)
	-- these 2 invisible BUs are there so the mark data is loaded/saved and can be used in preset also
	local duration_max = 60*60 * 9-- a week should be enough but we use 9 hour because of time code format
	self:add_slider(	{ix+4,	iy,			4,SY/2},	"timecode_in",	self, "__mark_in", 	0, 				0,duration_max )
		:set_visible(false)
	self:add_slider(	{ix+4,	iy+SY/2,	4,SY/2},	"timecode_out",	self, "__mark_out", duration_max,	0,duration_max	)
		:set_visible(false)
	iy = iy + SY + DY

	self.ui.bu_tc		= self:add_text_info(	{1,iy,		nx,ny}, "timecode" )
	self.ui.bu_duration	= self:add_text_info(	{1,iy+ny,	nx,ny},	"duration" )
	self.ui.bu_tc_in	= self:add_text_info(	{1+4,iy,	nx,ny}, "timecode_in_show" )
		:set_method_on_click_double( self, "mark_in", 0 )
	self.ui.bu_tc_out	= self:add_text_info(	{1+4,iy+ny,	nx,ny}, "timecode_out_show" )
		:set_method_on_click_double( self, "mark_out_end" )
	bu = self:add_trig_method(	{1+nx,	iy,		SX,ny},		"In",	self, "mark_in"	)
	bu = self:add_trig_method(	{1+nx,	iy+ny,	SX,ny},		"Out",	self, "mark_out"	)
	local o = 1+nx*2+SX
	bu = self:add_button(		{o, 	iy+ny*.6,	9-o,ny*1.2},"Clip", self, "b_clip", false )
		:set_text_rect_ratio( 2 )
		:set_text_inside(true)

	iy = iy + 2 * ny + DY
	bu = self:add_slider(			{6,iy,	3,SY},		"Step",	nil,nil,			10 )
		bu:set_min_max( 0, 100 )
	bu = self:add_trig_method(		{4,iy,	1,SY},		"-",	self, "add_time", -1	):set_text_inside( true )
	bu = self:add_trig_method(		{5,iy,	1,SY},		"+",	self, "add_time",  1	):set_text_inside( true )

-- CAPTURE
	iy = iy_beg
	self:begin_bu_group( "capture" )
		local SX = 5
		bu = self:add_button( 		{ix,iy,	SX,SY},		"Type"	)
			bu:set_menu( { "Direct Show", "Display Grabber" } )
			bu:set_text_selector( true )
			bu:set_target_lua( self, "s_capture_type" )
		self:add_trig_method( 		{ix+SX,iy,	2,SY},		"Enum",	self,"do_enum_capture"	)
		iy = iy + SY
		bu = self:add_button( 		{ix,iy,	8,SY},		"Device Id"	)
			bu:set_menu( { "Device 0", "Device 1", "Device 2" } )
			bu:set_text_selector( true )
			bu:set_target_lua( self, "capture_device_id" )
		ui.bu_capture_id = bu
		iy = iy + SY + DY
		
		self:add_button(			{ix, iy, 	2,SY},	"Open", 	self,"b_capture_open_asked", false	)
		--bu = self:add_text_info(	{ix+2,iy,	6,SY},	"name" )
			--bu:set_value_load_save( false )
			--ui.bu_name = bu
		iy = iy + SY

		bu = self:add_button( 		{ix,iy,	8,SY},		"Device Mode Id"	)
			bu:set_menu( { "Zero", "One", "Two" } )
			bu:set_text_selector( true )
			bu:set_target_lua( self,"capture_mode_id" )
		ui.bu_capture_mode_id = bu
		iy = iy + SY + DY

		self:add_button(			{ix,iy,		2.,SY},	"Run",		self,"b_capture_run_asked", false	)
		bu = self:add_text_info(	{ix+2,iy,	6,SY},	"State" )
	 		ui.bu_info = bu
		iy = iy + SY + DY
	self:end_bu_group()

	self:begin_bu_group( "crop" )
		local SX = 8/3 
		local crop = {}
		self:add_button(			{ix,iy,				SY,SY},	"Active",	crop,"b_active", false		):set_text( "Crop" ):set_value_type_integer(true)
		self:add_slider(			{ix,iy+SY,			SX,SY},	"Left",		crop,"left", 	0, 0,8182	):set_color_back( "x" ):set_value_type_integer(true)
		self:add_slider(			{ix+SX,iy+SY,		SX,SY},	"Right",	crop,"right", 	0, 0,8182	):set_color_back( "x" ):set_value_type_integer(true)
		self:add_slider(			{ix+8-SX,iy+SY,		SX,SY},	"Bottom",	crop,"bottom", 	0, 0,8182	):set_color_back( "y" ):set_value_type_integer(true)
		self:add_slider(			{ix+8-SX,iy,		SX,SY},	"Top",		crop,"top", 	0, 0,8182	):set_color_back( "y" ):set_value_type_integer(true)
		self.crop = crop
	self:end_bu_group()
	ix = 9
	iy = iy_beg

-- INCRUSTATION
	iy = self:add_incrust(		{ix,iy,		8,SY*3},	vref.tex_video )
	iy = iy + DY

	--self:add_param_obj_name(	{ix,iy},	"On Cpu",	vid, "image_on_cpu_wanted",		0,1 )
	self:add_button( 			{ix,iy,		2,SY},		"Cpu",		self,"b_on_cpu",	true	)
	self:add_button( 			{ix+2,iy,	2,SY},		"Gpu",		self,"b_on_gpu",	true	)

	ix,iy = 1,15
-- SOUND
	local sy = 1
	bu = self:add_param(		{ix,iy,		1,sy},		"sound",	vref.sound_active,	0,1 )
		bu:set_text_draw(false)
	self:add_param(				{ix+1,iy,	7,sy},		"volume",	vref.volume,		0,1 )

-- MONITORS
	iy = iy - 5.2
	local mon = {}
	mon[1] = self:add_monitor(	{1,iy}, 				"Monitor" )
		:set_text( "" )
		:set_render_mode_alpha( 0 )
		:add_buttons_below( self )
	mon[2]= self:add_monitor( 	{9,iy}, 				"Monitor_bis" )
		:set_text( "" )
		:set_render_mode_alpha( 1 )
		:cpy_bu_flip( mon[1] )
	ui.bu_monitor = mon

	--param.set( aaa.ref.pref, "Verbose_memory", false )
end

function MEU_VIDEO:__assign_texture_from_name()
	local layer = self:get_layer(1)
	if layer then
		local inst_key = self:get_inst_key() 
		local bind = TEXS:get_bind_by_name( inst_key )
		--self:box_debug( "inst_key "..inst_key.." bind is "..bind )
		if bind then
			aaa.layer.set_bind_2d( layer, bind )
			for _,mon in IPAIRS(self.ui.bu_monitor) do
				mon:set_texture_bind_2d( bind )
			end
		else
			self:print_error( "can't find texture with name "..inst_key.." this going to be a weird MEU." )
		end
		GP.cur:__recompute_register_monitor_with_button()
	else
		self:print_error( "can't find layer 1" )
	end
end


function MEU_VIDEO:change_name( name )
	oo.getsuper(MEU_VIDEO).change_name( self, name ) 
	self:__assign_texture_from_name()
end

function MEU_VIDEO:init()
	--param.set( aaa.ref.pref, "Verbose_memory", true )

	local layer = self:get_layer(1)
	if layer then
		--self:box_debug( "MEU_VIDEO:init()" )
		local tex_video = aaa.obj.get_down_by_class_no_error( layer, "tex_video" )
		--aaa.obj.get_branch_by_class( ref.tex_video, "kinect_ui"  )
		if tex_video then
			local video = VIDEO:create( self:get_inst_key(), tex_video )
			self.video = video
			self:__assign_texture_from_name()
			--param.set( video.ref.video, "image_keep_nb", 2 )
		else
			self:box_error( "can't find object c_tex_video down layer 1" )
		end
		--param.set( aaa.ref.pref, "Verbose_memory", false )
	else
		self:box_error( "can't find layer 1" )
	end
end
function MEU_VIDEO:init_after()	-- call after init or redefined
	self.b_populate_capture_device = true
end
function MEU_VIDEO:recall_preset( i )
	oo.getsuper(MEU_VIDEO).recall_preset( self, i )

	self.capture_device_enum_count_last = nil
	--self.capture_mode_id = self.ui.bu_capture_mode_id:get_value()
end

function MEU_VIDEO:do_enum_capture()
	param.set( self.video.ref.capture_enum_trig, true )
	self.b_populate_capture_device = true

	--self:update_capture_mode()
end

function MEU_VIDEO:update_menu_button( bu, obj, name, nb_max, col_nb )
	--aaa.print_fn()
	local sel = bu:get_selector() 
	local nb = 0
	for i = 1,nb_max do
		local str = param.get( obj, name..(i-1) )
		if str == "" then
			--self:print( "break at "..i )
			break
		else
			--self:print( str )
			sel:set_item_text( i, str )
			nb = i
		end
	end
	local nb_by_col =  math.ceil(nb/col_nb)
	sel:set_nb( col_nb, nb_by_col )
	self:print_debug(  (nb+1).." *** "..( col_nb * nb_by_col) )
	for i = nb+1,  col_nb * nb_by_col do
		self:print_debug( "complete i "..i )
		sel:set_item_text( i, "" )
	end

	-- local id = bu:get_value()
	-- self:print( id.." on "..nb )
	-- if id > nb then
	-- 	self:print( "force value to "..nb )
	-- 	bu:set_value(nb)
	-- 	sel:set_value(nb)
	-- 	self:print( bu:get_value().." ---- "..sel:get_value() )
	-- end
end
function MEU_VIDEO:populate_capture_device_ask( inc )
	self.populate_capture_device_count = (self.populate_capture_device_count or 0) + inc
end
function MEU_VIDEO:populate_capture_device()
	local vref = self.video.ref
	self:update_menu_button( self.ui.bu_capture_id, vref.tex_video, "capture_device_", 16, 1 )
	self.populate_capture_device_count = nil
end
function MEU_VIDEO:populate_capture_mode()
	local video = self.video
	if video then
		local ds_cap_ui = video:get_ds_cap_ui()
		if ds_cap_ui then
			local bu = self.ui.bu_capture_mode_id
			self:update_menu_button( bu, ds_cap_ui, "format_supported_", 64, 2 )

			-- local nb = bu:get_max()
			-- --self:print( self.capture_mode_id.."|"..bu:get_value() .." on "..nb )
			-- if self.capture_mode_id > nb or self.capture_mode_id == 0 then
			-- 	--bu:print( "force value to "..nb )
			-- 	self.capture_mode_id = nb>0 and nb or 1
			-- 	--self:print( bu:get_value().." ---- " )
			-- end

			self.b_populate_capture_mode = false
			--self.update_capture_mode_device_used = self.capture_device_id_asked
			--self:show( "update_mode device "..self.update_capture_mode_device_used )
		end
	else
		self:print_debug( "video filed is nil" )
	end
end

function MEU_VIDEO:open_capture()
	local vref = self.video.ref
	param.set( vref.capture_open, true )
end

function MEU_VIDEO:set_capture_open_run( b )
	self.video:set_capture_open_run( b )
end
function MEU_VIDEO:is_capture_opened()	return self.video:is_capture_opened()	end
function MEU_VIDEO:is_capture_running()	return self.video:is_capture_running()	end


function MEU_VIDEO:mark_in(t_in)	self.__mark_in  = t_in or self:get_time()	end
function MEU_VIDEO:mark_out(t_in)	self.__mark_out = t_in or self:get_time()	end
function MEU_VIDEO:mark_out_end()	self.__mark_out = self:get_duration()		end

function MEU_VIDEO:get_image_index()
	return self.video:get_image_index()
end
function MEU_VIDEO:get_video()
	return self.video
end
function MEU_VIDEO:free()
	--self.video:free()	--MEU.free() free the layers so the layer so the tex_video
	self.video = nil
	oo.getsuper(MEU_VIDEO).free( self )
end

-- function MEU_VIDEO:is_render( val )
-- 	if self.verbose >= 2 then self:print( "Video Force is_render()" ) end
-- 	--	when meu should be off force render when video is playing to stop
-- 	return val > 0. or self.video:is_playing()
-- end




function MEU_VIDEO:start()			self:set_bu_value( "play", 1 )		end
function MEU_VIDEO:stop()			self:set_bu_value( "play", 0 )		end
function MEU_VIDEO:restart()
	--todo address these double definition of restart here and 28 lines below
	self:set_bu_value( "restart", 1 )
	--return self.video:set_restart()
end
function MEU_VIDEO:play_at_start()
	self:start( )
	self:restart()
end

function MEU_VIDEO:set_time( t )
	self.video:set_time( t )
end
function MEU_VIDEO:add_time( factor )
	local t = self:get_time()
	local s = self:get_bu_value( "Step" )
	t = t + s * factor
	local duration = self:get_duration()
	if t < 0. then
		t = duration + t
	else
		if t > duration then
			t = t - duration
		end
	end	
	self:set_time( t )
end

function MEU_VIDEO:get_duration()	return self.video:get_duration()	end
function MEU_VIDEO:get_time()		return self.video:get_time()		end


local function make_tc( t )
	local fn = aaa.format.real_to_char2
	local r = math.fmod( t, 1. )
	t = t - r
	local str = ":"..fn( r * 25 )

	t = t / 60.
	r = math.fmod( t, 1. )
	str = ":"..fn( r * 60 )..str
	t = t - r

	t = t / 60.
	r = math.fmod( t, 1. )
	str = math.floor( t )..":"..fn( r * 60 )..str

	return str
end


function MEU_VIDEO:update_ui()
	--if true then return end
	--param.set( aaa.ref.pref, "Verbose_memory", true )

	local video = self.video
	if not video then
		return
	end
	local all = self:__get_bu_all_table()
	if not all then
		return
	end

	local ui = self.ui
	local bu
	local vref = video.ref
	
	--local	sx,sy = video:get_size()
	--ui.bu_info:set_text( sx.." by "..sy )
	if self.b_capture then
		bu = ui.bu_capture_mode_id
		bu:set_visible( self.s_capture_type == MEU_VIDEO.CAPTURE_TYPE_DS )

		local str
		if		self.b_capture_running	then	str = "Running"
		elseif	self.b_capture_opened	then	str = "Opened"
		else									str = "Closed"
		end

		-- bu = ui.bu_name
		-- bu:set_text_color( str )
		-- bu:set_text( "Direct Show Cam" )

		bu = ui.bu_info
		bu:set_text_color( str )
		bu:set_text( str )

		bu = all["video info"]
		bu:set_text( "Capture : "..param.get_str( vref.capture_type ) )
		bu:set_text_color( str )
	else	
		bu = all["video info"]
		local filename = video:get_name_video()
		if filename then
			bu:set_text( aaa.file.get_file_name(filename).."\n  In "..aaa.file.get_dir_name(filename) )
			bu:set_text_color_problem_info( video:is_usable() )
		else
			--todo need this
		end

		ui.bu_tc:set_text(		 make_tc( self:get_time()	  ) )
		ui.bu_duration:set_text( make_tc( self:get_duration() ) )

		local m_in, m_out = self:get_marks()
		ui.bu_tc_in:set_text(    make_tc( m_in  ) )
		ui.bu_tc_out:set_text(   make_tc( m_out ) )
	end

	--param.set( aaa.ref.pref, "Verbose_memory", false )
	local bu = all.bind
	local a_prev = bu:get_alpha_bu_to_draw() 
	local a = bu:interpolate_alpha_bu( self.b_capture and 0 or 1 )
	local function set( bu )
		bu:set_alpha_bu( a )
		bu:set_active( a >= .1 )
	end

	if math.abs( a - a_prev ) > .00001 then
		aaa.show( a, "a" )
		set( all.bind		)
		set( all.bank		)
		set( all.play		)
		set( all.loop		)
		set( all.restart	)
		set( all.speed		)
		set( ui.bu_tc		)
		set( ui.bu_duration	)
		set( ui.bu_tc_in	)
		set( ui.bu_tc_out	)
		set( all["in"]		)
		set( all.out		)
		set( all.clip		)
		set( all.step		)
		set( all["-"]		)
		set( all["+"]		)
	end

	self:set_bu_group_active( "capture", self.b_capture )
	local b_crop_group_active = self.b_capture and (self.s_capture_type == MEU_VIDEO.CAPTURE_TYPE_DISPLAY )
	self:set_bu_group_active( "crop", b_crop_group_active )
	if b_crop_group_active then
		local t = self:get_bu_group( "crop" )
		--table.print( t, "crop_table" )
		local alpha = t[2]:interpolate_alpha_bu( self.crop.b_active )
		for i = 3,#t do
			t[i]:set_alpha_bu( alpha )
		end
	end
end
function MEU_VIDEO:get_bind_video()
	local ui = self.ui
	--todotex encapsulate video mecanism too 
	return ui.bu_video_bank:get_value() * aaa.img.__BIND_BY_BANK_2D + ui.bu_video_bind:get_value()
end
function MEU_VIDEO:set_bind_only_video( bind )
	local ui = self.ui
	ui.bu_video_bind:set_value( bind )
end
function MEU_VIDEO:load_video()
	local bind = self:get_bind_video()
	local str = "load video at index "..bind
	local filter = "Avi\0*.*\0Movie\0*.avi;*.mpg;*.mpeg;*.mov;*.qt;*.wmv;*.mp4\0Tous\0*.*\0\0"
	local filenames = aaa.file.do_dialog_open( str, filter, true )
	--table.print( filenames, "filenames" )
	for i,fname in IPAIRS(filenames) do
		self:print( i.." assign video "..fname.." at "..bind )
		self.video:assign_file( fname, bind )
		bind = bind + 1
	end
end
function MEU_VIDEO:get_marks()
	return self.__mark_in or 0, self.__mark_out or self:get_duration()
end

function MEU_VIDEO:draw_icon()
	if not self:is_proto() then
		self:draw_icon_texture()
		local video = self.video
		if video then
			self:draw_icon_frame( video:get_image_index() )
		end
	end
end


function MEU_VIDEO:update_capture( video, vref )
	local s_capture_type = self.s_capture_type

	if self.s_capture_type_last ~= s_capture_type then
		param.set( vref.capture_type, (s_capture_type == MEU_VIDEO.CAPTURE_TYPE_DS) and "DirectShow" or "Display Grabber" )
		self:populate_capture_device_ask( 2 )
		self.s_capture_type_last = s_capture_type
		if s_capture_type == MEU_VIDEO.CAPTURE_TYPE_DS then
			self.b_populate_capture_mode = true
		elseif s_capture_type == MEU_VIDEO.CAPTURE_TYPE_DISPLAY then
			local t = video:get_display_cap_ui()
			if t then
				param.set( t.thread_use, true )	-- we ask the C side to use a separate thread
			end
		end
	end

	if s_capture_type == MEU_VIDEO.CAPTURE_TYPE_DISPLAY then
		local t = video:get_display_cap_ui()
		if t then
			local crop = self.crop
			param.set( t.crop_active, crop.b_active )
			param.set( t.crop_left,   crop.left     )
			param.set( t.crop_right,  crop.right    )
			param.set( t.crop_bottom, crop.bottom   )
			param.set( t.crop_top,    crop.top      )
		end
	end
		-- local b_capture_opened_last = self.b_capture_opened

	self.b_capture_opened = self:is_capture_opened()
	self.b_capture_running	= self:is_capture_running()

	-- if b_capture_opened_last ~= self.b_capture_opened then
	-- 	self.b_need_update_capture_mode = true
	-- end

	-- C++ update the count when a device is added or removed
	local count = param.get( vref.capture_device_enum_count )
	if self.capture_device_enum_count_last ~= count then
		self.capture_device_enum_count_last = count
		self:populate_capture_device_ask( 1 )
	-- 	self.b_need_update_capture_mode = true
	end

	local capture_device_index_asked = param.get( vref.capture_device_index_asked )
	local device_id
	if self.capture_device_index_asked ~= capture_device_index_asked then
		self.capture_device_index_asked = capture_device_index_asked
		device_id = capture_device_index_asked + 1
		self.capture_device_id = device_id
	else
		device_id = self.capture_device_id
	end
	if self.capture_device_id_last ~= device_id then
	 	self.capture_device_id_last = device_id
	 	param.set( vref.capture_device_index_asked, device_id-1 )
		--self:box_debug( "Toto" ) 
	 	self.b_populate_capture_mode = true
	end

	local ds_cap_ui = video:get_ds_cap_ui()
	if ds_cap_ui then
		local mode_id = self.capture_mode_id
		if self.capture_mode_id_last ~= mode_id then
	 		self.capture_mode_id_last = mode_id
	 		param.set( ds_cap_ui, "format_index_asked", mode_id-1 )	
	-- -- 	count = param.get( ds_ui_obj, "format_supported_enum_count" )
	-- -- 	if self.format_supported_enum_count_last ~= count then
	-- -- 		self.format_supported_enum_count_last = count
	-- -- 		self.b_need_update_capture_mode = true
	-- -- 	end
		end
	end
	

	if self.b_capture_open_asked ~= self.b_capture_opened then
		param.set( vref.capture_open, self.b_capture_open_asked )
	end
	if self.b_capture_run_asked ~= self.b_capture_running then
		param.set( vref.capture_run, self.b_capture_run_asked )
	end
end

function MEU_VIDEO:update()
	--if true then return end
	--param.set( aaa.ref.pref, "Verbose_memory", true )

	local video = self.video
	local vref = video.ref
	video:set_on_cpu( self.b_on_cpu and self:get_proto().b_on_cpu )
	--self:print( "inst is "..self.b_on_cpu.." proto is "..self:get_proto().b_on_cpu )
	--self:print( "which does "..(self.b_on_cpu and self:get_proto().b_on_cpu) )	video:set_move_to_memory( self.b_on_gpu and self:get_proto().b_on_gpu )

	if self.verbose >= 2 then self:print( "update() capture is " ) end

	self.b_capture = param.get_bool( vref.capture )
	if self.b_capture then
		self:update_capture( video, vref )
	else
		video:set_video_src( self:get_bind_video() )

		--self:print( "update() "..self:get_proto().." "..self:get_proto().b_allow.." "..self:get_alpha() )
		local b_play = (self:get_mu_value()>0.) and self.b_play -- and self:get_proto().b_allow
		--self:print(  (1).." "..(self:get_alpha()>0.), (2).." "..self.b_play, (3).." "..self:get_proto().b_allow )	
		--self:print( self.b_play.." "..b_play )
		--self:print( "b_allow "..self:get_proto().b_allow )
		--table.print( self:get_proto() )
		--self:print( video:is_playing().." "..b_play )
		if video:is_playing() ~= b_play then
			video:__set_play( b_play )
		end
		if b_play then
			video:check_need_pick_reading_method()
		end
	
		--self:print( self.b_clip )
		if b_play and self.b_clip then
			local m_in, m_out = self:get_marks()
			local t = self:get_time()
			if outside( t, m_in, m_out ) then
				self:set_time( m_in )
			end
		end

	end
end

--todo this is a quick hack
function MEU_VIDEO:draw()
	--param.set( aaa.ref.pref, "Verbose_memory", true )

	if GA.b_spy then aaa.spy.push_range( "VIDEO-"..self..".draw()", 7 ) end
		MEU.draw( self )

		if self.b_capture then
			--local vref = self.video.ref
			local count = self.populate_capture_device_count
			if count then
				count = count - 1
				if count < 0 then
					self:populate_capture_device()
				else
					self.populate_capture_device_count = count
				end
			end
			if self.b_populate_capture_mode then
		 		self:populate_capture_mode()
			end
			--self.b_capture_open_asked = param.get( vref.capture_opened )
			--self.b_capture_run_asked = param.get( vref.capture_running )
		end

	if GA.b_spy then aaa.spy.pop_range() end

	--param.set( aaa.ref.pref, "Verbose_memory", false )
end


