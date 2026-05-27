if CLASS.DECLARE( "VIDEO", nil,	{	__b_need_to_pick_reading_method = true	} ) then
	VIDEO:set_class_status_doc(	CLASS.STATUS.CORE,
								"high level interface to open and control a VIDEO, using the C tex_video class" )
end 

function VIDEO.__build_ref( ref, ref_tex_video )
	ref.tex_video = ref_tex_video

	local function pgr( name, pname )	ref[name] = param.get_ref( ref_tex_video, pname and pname or name )	end

	pgr(	"active"			)
	param.set( ref.active, true )

	pgr(	"video_src"			)
	pgr(	"video_usable"		)
	pgr(	"bind_dst"			)
	pgr(	"video_name"		)
	--pgr(	"video_lib"			)
	pgr(	"image_index"		)
	--param.set( ref.video_lib, 1 )

	pgr(	"capture"						)
	pgr(	"capture_enum_trig"				)
	pgr(	"capture_type"					)

	pgr(	"capture_open"					)
	pgr(	"capture_opened"				)
	pgr(	"capture_run"					)
	pgr(	"capture_running"				)

	pgr( 	"capture_device_enum_count"		)	
	pgr(	"capture_device_count"			)
	pgr(	"capture_device_index_asked"	)
	pgr(	"capture_device_index_used"		)
	pgr(	"capture_device_id_asked"		)
	pgr(	"capture_device_id_used"		)
	pgr(	"capture_device_name"			)
	pgr(	"capture_crossbar_name"			)


	pgr(	"play"				)
	pgr(	"playing"			)
	--self:print( i.."->play_ref "..ref.play
	pgr(	"loop"				)
	pgr(	"restart",			"restart_trig"		)
	pgr(	"duration"			)
	pgr(	"fps"				)
	pgr(	"time_asked"		)
	pgr(	"time_asked_trig"	)
	pgr(	"time_movie"		)
	pgr(	"time_factor",		"time_factor_asked"			)
	pgr(	"close_unused",		"direct_show_close_unused"	)
	pgr(	"free_unused",		"direct_show_free_unused"	)
	pgr(	"free_trig",		"free_all_trig"		)
	pgr(	"luma",				"incrust_luma"		)
	pgr(	"luma_min",			"incrust_min"		)
	pgr(	"luma_max",			"incrust_max"		)
	pgr(	"luma_inverse",		"incrust_inverse"	)

	pgr(	"on_cpu",			"image_on_cpu_wanted"	)
	pgr(	"move_to_texture"	)
	pgr(	"size_x"			)
	pgr(	"size_y"			)

	pgr(	"sound_active"		)
	pgr(	"volume"			)
	pgr(	"pan"				)

	pgr(	"direct_show_renderfile"	)
	pgr(	"direct_show_ffdshow"		)
	pgr(	"retry_open"				)

	pgr(	"preroll", 			"video_preroll"	)
end
function VIDEO:get_ds_cap_ui()
	local ref = self.ref
	local r = ref.ds_cap_ui
	if not r then
		r = param.get_obj_attached( ref.tex_video, "DirectShow->" )
		ref.ds_ui_obj = r
	end
	return r
end
function VIDEO:get_display_cap_ui()
	local ref = self.ref
	local t = ref.display_cap_ui
	if not t then
		local obj = param.get_obj_attached( ref.tex_video, "Display Grabber->" )
		if obj then
			t = { obj=obj }
			ref.display_cap_ui = t
			t.can         = param.get_ref( obj, "can"			)
			t.crop_active = param.get_ref( obj, "crop_active"	)
			t.crop_left   = param.get_ref( obj, "crop_left"		)
			t.crop_right  = param.get_ref( obj, "crop_right"	)
			t.crop_bottom = param.get_ref( obj, "crop_bottom"	)
			t.crop_top    = param.get_ref( obj, "crop_top"		)
			t.thread_use  = param.get_ref( obj, "thread_use"	)
		end
	end
	return t
end
--------------------
function VIDEO:get_bind()			return self.cur.bind_dst 					end
function VIDEO:get_ratio_x()		return self.cur.__sx / self.cur.__sy		end	--opt this is used in drawing
function VIDEO:get_ratio_y()		return self.cur.__sy / self.cur.__sx		end
function VIDEO:get_image_index()	return param.get( self.ref.image_index )	end

function VIDEO:set_sound( vol, pan )
	local ref = self.cur.ref
	param.set( ref.sound_active, vol > 0 )
	self:set_volume( vol )
	if pan then
		param.set( ref.pan, pan )
	end
end

function VIDEO:init_sound_on()
	self:set_sound( 1, 0 )
end

--todo memorize and propagate when lang change
--todo this should be only an option
function VIDEO:set_volume( v )
	local ref = self.cur.ref
	--local old = param.get( ref.volume )
	--if v ~= old then
	--	v = clamp_01( interpolate( old, v, aaa.time.dt * 5. ) )
		param.set( ref.volume, v )
	--end
end
function VIDEO:set_pan( v )
	local ref = self.cur.ref
	--local old = param.get( ref.pan )
	--if v ~= old then
	--	v = clamp_01( interpolate( old, v, aaa.time.dt * 5. ) )
		param.set( ref.pan, v )
	--end
end
VIDEO.doc.create = "( name, i_or_ref, bind_dst )"
function VIDEO:create( name, i_or_ref, bind_dst )
	local self = VIDEO:create_instance( name )
--	self.id = i -- was unuse
	self.play_on_page = {}	--todo deal with LAng hre
	self.ref = {}
	local ref = self.ref

	local ref_tex_video
	if aaa.obj.is_ref_no_error( i_or_ref ) then
		ref_tex_video = i_or_ref
	else
		if not VIDEO.__ref_tex_video then
			local layer = aaa.obj.get_by_class_and_name_symbo( "layer", "gabu_video_layer" )	--todo do better
			VIDEO.__ref_tex_video = aaa.obj.get_branch_by_class( layer, "tex_video" )
		end
		--	create a tex_video object from the one in the layer
		--aaa.box_warning( "GA found "..aaa.lua.global.get( "GA" ) )
		ref_tex_video = aaa.obj.clone( VIDEO.__ref_tex_video, aaa.lua.global.get( "GA" ) and GA.__c_node_tex_video )
		bind_dst = VIDEOS:get_bind_from_id( i_or_ref )
	end
	self.__build_ref( self.ref, ref_tex_video )

	--param.set( ref.tex_video, "direct_show_renderfile", 1 )
	if bind_dst then
		self.bind_dst 	=	bind_dst
		param.set( ref.bind_dst, self.bind_dst )
	end

--	param.set( ref.direct_show_renderfile, false )
--	param.set( ref.direct_show_ffdshow, true )

	param.set( ref.preroll, 8 )

	self.cur = self
	--self:set_loop( i==1 and 1 or 0 )	-- loop is on for 1

	--self:set_stop()
	--self:set_restart()
	--self:set_update()
	return self
end

--todo deal with version/lang here
function VIDEO:free()
	self:release()
	aaa.obj.delete( self.ref.tex_video )
	self.ref.tex_video = nil
end

function VIDEO:set_focus()
	aaa.obj.set_focus_ui( self.cur.ref.tex_video )
end
--
--	VIDEO TIME
--
function VIDEO:update_duration()
	local ref = self.cur.ref
	self.duration	=	param.get( ref.duration	)
	self.fps		=	param.get( ref.fps		)
end
function VIDEO:get_duration()
	self:update_duration()
	return self.duration
end
function VIDEO:update_time()
	--aaa.debug.print_traceback( "VIDEO:update_time()" )
	--aaa.obj.update( self.cur.ref.tex_video )	--2020 August provoke trouble in GP (wrong bind) so Maa removed it
	self.time = param.get( self.cur.ref.time_movie )
end
function VIDEO:get_time()
	self:update_time()
	return self.time
end
function VIDEO:get_time_cano()		return self:get_time() / self:get_duration()		end
function VIDEO:is_frame_for_time( t )
	t = clamp( t, 0., self:get_duration() )
	local fps 			= self.fps
	local frame_asked	= math.floor( t * fps )
	local frame_cur		= math.floor( self:get_time() * fps )
	return frame_asked == frame_cur
end
function VIDEO:set_time( t )
	if self:is_frame_for_time( t )	 then
		if self:is_scrub() then
			self:__set_play( false )
		end
	else
		local ref = self.cur.ref
		param.set_and_save( ref.time_asked, t, false )
		param.set( ref.time_asked_trig, true )
	--todo	self.time = t
		--self:update()
		if self.verbose >= 1 then self:print( "set_time "..string.format( "%.2f", t ) ) end
		if self:is_scrub() then
			self:__set_play( true )
		end
	end
end
function VIDEO:set_time_factor( val )
	param.set( self.ref.time_factor, val )
end
function VIDEO:set_time_cano( val )
	self:set_time( val * self:get_duration() )
end
function VIDEO:scrub( t )
	--aaa.print_method()
	self.__central.scrub_t = t
end
function VIDEO:scrub_cano( val )
	self:scrub( val * self:get_duration() )
end
function VIDEO:set_scrub( b )
	if b then
		self.__central.b_scrub_mem_playing = self:is_playing()
		self:set_stop()
	else
		if self.__central.b_scrub_mem_playing then
			self:set_start()
		else
			self:set_stop()
		end
	end
	self.__central.b_scrub = b
end
function VIDEO:is_scrub()
	return self.__central and self.__central.b_scrub
end

function VIDEO:check_need_pick_reading_method()
	if self.__b_need_to_pick_reading_method then
		self:pick_mode_working()
	end
end

function VIDEO:update()
	--aaa.debug.print_traceback()
	if self:is_scrub() then
		--self:print( "VIDEO:update() scrub "..self.scrub_t )
		--if not param.get_bool( self.ref.playing ) then
		--if not self:is_playing() then
		--	self:print( "set time "..self.scrub_t )
			self:set_time( self.__central.scrub_t )
		--end
	end
	self:check_need_pick_reading_method()
	aaa.obj.update( self.cur.ref.tex_video )
end

--todo check with lang changes
function VIDEO:release()
	--	we don't use cur here
	--local clo = param.get( self.ref.close_unused )
	--local fre = param.get( self.ref.free_unused )
	--param.set( self.ref.close_unused, 1 )
	--param.set( self.ref.free_unused, 1 )
	param.set( self.ref.free_trig, 1 )
	aaa.obj.update( self.ref.tex_video )
	--param.set( self.ref.close_unused, clo )
	--param.set( self.ref.free_unused, fre )
end


function VIDEO:set_loop( b )		param.set( self.cur.ref.loop, b )						end
function VIDEO:is_playing()
	local b = self:__is_playing()
	--self:print( b )
	if b then
		self:update_duration()	--todo isn't it too much calling
		b = self:get_time() < self.duration
		--self:print( "ref_time "..self:get_time().."  "..self.duration )
	end
	return b
end
function VIDEO:is_at_end()
	local ref = self.cur.ref
	self:update_duration()	--todo isn't it too much calling
	local b = self:get_time() >= self.duration
	if b then
		self:print( "is at end ".. self:get_time() )
	else
		--self:print( "VIDEO:is_at_end() "..self:get_time().." / "..self.duration )
	end
	return b
end

function VIDEO:set_capture_open_run( b )
	local ref = self.ref
	param.set( ref.capture_open, b )
	param.set( ref.capture_run, b )
end
function VIDEO:restart_capture()
	local ref = self.ref
	local b_open = param.get( ref.capture_open )
	local b_run  = param.get( ref.capture_run )

	param.set( ref.capture_open,	false )
	param.set( ref.capture_run,		false )

		aaa.obj.update_then_draw( ref.tex_video )

	param.set( ref.capture_open,	b_open )
	param.set( ref.capture_run,		b_run  )
end
function VIDEO:is_capture_opened()	return param.get_bool( self.ref.capture_opened  )	end
function VIDEO:is_capture_running()	return param.get_bool( self.ref.capture_running )	end

function VIDEO:set_restart()			param.set( self.cur.ref.restart, 1 )				end
function VIDEO:__set_play( b )
	if self.verbose >= 2 then aaa.print_method() end
	param.set( self.cur.ref.play, b )
end
function VIDEO:__is_playing()
	if self.verbose >= 2 then aaa.print_method() end
	return param.get_bool( self.cur.ref.playing )
end
function VIDEO:set_start()
	if self.verbose >= 2 then aaa.print_method() end
	--should not be in the following test
	self:update_duration()
	self:update_time()
	if self.time >= self.duration then
		self:set_restart()
	end
	if not self:__is_playing() then
		self:__set_play( true )
	end
end
function VIDEO:set_stop()
	--error()
	if self.verbose >= 2 then aaa.print_method() end
	if self:__is_playing() then
		self:__set_play( false )
	end
end
function VIDEO:restart()		self:set_restart()		self:update()		end
function VIDEO:start()			self:set_start()		self:update()		end
function VIDEO:stop()			self:set_stop()			self:update()		end
function VIDEO:play()			self:start()								end

--todo check strategy with lang
--the introduction of the param video_usable will probably fuck us this
function VIDEO:toggle_and_reset_at_start()
	if self:is_playing() then
		self:set_stop()
	else
		self:set_restart()
		self:set_start()
	end
end
function VIDEO:toggle_and_reset_at_stop()
	if self:is_playing() then
		self:set_restart()
		self:set_stop()
	else
		self:set_start()
	end
end
function VIDEO:toggle()
	if self:is_playing() then
		if self.verbose >= 2 then self:print( self.." toggle() -> set_stop()" ) end
		self:set_stop()
	else
		if self.verbose >= 2 then self:print( self.." toggle() -> set_start()" ) end
		self:set_start()
	end
end
function VIDEO:is_usable()
	return param.get_bool( self.cur.ref.video_usable )
end
function VIDEO:place_at_start()
	if self.verbose >= 1 then aaa.print_method() end
	self:__set_play( false )
	self:update()
	self:get_duration()
	if not self:is_usable() then
		self:print_error( "video is not usable say tex_video obj" )
		return false
	end
	self:set_restart()
	local count = 100	--todo better than count
	local sx = 0
	while (not self:is_playing()) and count > 0 do
		if self.verbose >= 1 then self:print( "place_at_start() trying to play video "..count ) end
		self:__set_play( true )
		self:update()
		count = count - 1
	end
	local count = 100	--todo better than count
	while self:is_playing() and count > 0 do
		if self.verbose >= 1 then self:print( "place_at_start() trying to stop video "..count ) end
		self:__set_play( false )
		self:update()
		count = count - 1
	end
	return true
end

function VIDEO:get_name_video()	return self.__name_video end

function VIDEO:get_size()
	local ref = self.cur.ref
	return param.get( ref.size_x ), param.get( ref.size_y )
end
function VIDEO:__try_mode()
	if not self:place_at_start() then
		return false
	end

	--do
	local sx,sy = self:get_size()
	--if sx==nil then
	--todo solve this in video object
	if sx == 0 or sy == 0 then
		self:print_error( "in VIDEO:__try_mode() sx or sy is nil, replacing by 1280 x 720" )
		sx = 1280
		sy = 720
		-- self.__sx = sx
		-- self.__sy = sy
		-- return false
	end
	--end
	if self.verbose >= 1 then self:print( self:get_name_video().." size "..sx.." x "..sy ) end

	self.__sx = sx
	self.__sy = sy
	return true
end
function VIDEO:pick_mode_working()
	--aaa.debug.print_traceback()

	local ref = self.cur.ref
	local ds_renderfile	= ref.direct_show_renderfile
	local ds_ffdshow 	= ref.direct_show_ffdshow
	local retry_open 	= ref.retry_open

	--self:box_debug( "pick_mode_working "..self:get_name_video() )
	self:set_need_to_pick_reading_method( false )

	--aaa.print( "Trying to open using neither RenderFile or FFdshow" )
	self:print( "Trying to open using FFdshow" )
		param.set( ds_renderfile, 	0 )
		param.set( ds_ffdshow, 		1 )
		if self:__try_mode() then
			return true
		end
	self:print( "\tFFdshow Failed" )

	self:print( "Trying to open using RenderFile" )
		param.set( ds_renderfile,	1 )
		param.set( ds_ffdshow,		0 )
	--	param.set( retry_open, 1 )
		if self:__try_mode() then
			param.set( retry_open, 0 )
			return true
		end
	self:print( "\tRenderFile Failed" )

	self:print( "Trying to open using neither RenderFile or FFdshow" )
		param.set( ds_renderfile,	0 )
		param.set( ds_ffdshow,		0 )
	--	param.set( retry_open, 1 )
		if self:__try_mode() then
	--		param.set( retry_open, 0 )
			return true
		end
	self:print( "\tNeither Failed" )

--	param.set( ref.retry_open, 0 )
--	param.set( ds_ffdshow, 0 )

end
function VIDEO:set_need_to_pick_reading_method( b )
	self.__b_need_to_pick_reading_method = b
end
function VIDEO:set_video_src( video_bind )
	local name_video = aaa.video.get_bind_filename( video_bind )
	if self.__name_video ~= name_video or self.__video_bind ~= video_bind then
		self.__name_video = name_video
		self.__video_bind = video_bind
		--local old = self.ref.video_src
		param.set( self.cur.ref.video_src, video_bind )
		self:set_need_to_pick_reading_method( true )
		--self:box_debug( "set_video_src "..video_bind )
	end
end
function VIDEO:assign_file( fname, video_bind )
	if self.verbose >= 1 then aaa.print_method() end

	aaa.video.set_bind_filename( video_bind, fname )
	self:set_video_src( video_bind )
end
function VIDEO:set_file( fname, video_bind )
	self:assign_file( fname, video_bind )
	return self:pick_mode_working()
end

--todo lang
function VIDEO:set_on_cpu( b )
	
	--aaa.print_fn()
	param.set( self.cur.ref.on_cpu,	b )
end
function VIDEO:set_move_to_memory( b ) 	param.set( self.cur.ref.move_to_texture, b )	end

function VIDEO:add_play_on_page(page)
	self.play_on_page[page] = true
	self:update_duration()
end

--todo lang
function VIDEO:set_luma( b_on, min, max, b_inverse )
	local ref = self.cur.ref
	param.set( ref.luma, b_on )
	if min then
		param.set( ref.luma_min, min )
		param.set( ref.luma_max, max )
	end
	if b_inverse~= nil then
		param.set( ref.luma_inverse, b_inverse )
	end
end
function VIDEO:switch_version( lang )
	--self:print( "VIDEO:switch_version( \""..lang.."\" )" )

	local ver = self.versions
	if not ver then return end

	--self:print( "VIDEO:switch_version() doit " )
	--table.print( ver, "versions", 2 )

	local vid = ver[lang]
	if vid then
		local b = self:is_playing()
		local t = self:get_time()
		self:stop()
		self.cur = vid
		vid.cur = vid
		self:set_time( t )
		self:play()
		if not b then
			self:stop()
		end
	end
end

