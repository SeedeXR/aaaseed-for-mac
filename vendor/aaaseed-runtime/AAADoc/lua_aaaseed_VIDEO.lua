-- AAASeed Lua API for the VIDEO and VIDEOS classes.
-- VIDEO defined in AAAKernel/GaBu/GaBu_Util/VIDEO/VIDEO.lua.
-- VIDEOS defined in AAAKernel/GaBu/GaBu_Util/VIDEOS/VIDEOS.lua.
-- Both loaded via GaBu_Util bind order.
--
-- All entries in this file are pure Lua (no C bindings).
--
-- VIDEO is a high-level wrapper around the C++ tex_video object. It provides
-- file loading, playback transport, scrubbing, audio control, live capture,
-- luma keying, and multi-language video variants.
-- VIDEOS is a singleton pool that pre-allocates VIDEO objects at init and
-- manages a bind range (512..639) for video textures. In normal usage you
-- ask VIDEOS for a video by filename and it returns a ready VIDEO instance.


-- # VIDEO

-- ## create

-- Constructor. Wraps a C++ tex_video object (either an existing reference or
-- cloned from the gabu_video_layer template by index).
-- i_or_ref: a C++ object reference (if aaa.obj.is_ref succeeds) or an integer
-- index into the VIDEOS bind pool.
-- bind_dst: the GPU texture bind that receives decoded frames. When i_or_ref
-- is an integer, bind_dst is auto-assigned from VIDEOS:get_bind_from_id().

	vid = VIDEO:create( name, i_or_ref [, bind_dst] )


-- # VIDEO : file loading

-- ## set_file

-- Loads a video file. Assigns the filename to the video bind, then runs
-- pick_mode_working() to auto-detect the best DirectShow configuration
-- (FFdshow, RenderFile, or generic). Returns true on success.

	b = vid:set_file( filename, video_bind )

-- ## assign_file

-- Sets the filename on the video bind without attempting playback or mode
-- detection. Use this for deferred loading when you want to control timing.

	vid:assign_file( filename, video_bind )


-- # VIDEO : queries

-- ## get_bind

-- Returns the GPU texture bind that receives the decoded video frames.

	bind = vid:get_bind()

-- ## get_size

-- Returns the video pixel dimensions (from the C++ object).

	sx,sy = vid:get_size()

-- ## get_ratio_x / get_ratio_y

-- Aspect ratios computed from pixel size. get_ratio_x = sx/sy, get_ratio_y = sy/sx.

	rx = vid:get_ratio_x()
	ry = vid:get_ratio_y()

-- ## is_usable

-- Returns true if the C++ tex_video reports the video as usable (loaded and ready).

	b = vid:is_usable()

-- ## set_focus

-- Sets the C++ UI focus to this video's tex_video object (for the property panel).

	vid:set_focus()


-- # VIDEO : time

-- ## get_duration

-- Returns the total duration in seconds. Caches the value and the fps internally.

	dur = vid:get_duration()

-- ## get_time

-- Returns the current playback position in seconds (reads from the C++ object).

	t = vid:get_time()

-- ## get_time_cano

-- Returns the current time as a normalized value in [0, 1] (time / duration).

	v = vid:get_time_cano()

-- ## set_time

-- Seeks to an absolute time (seconds). Skips the seek if the video is already
-- on the correct frame (avoids unnecessary decoder work). During scrub mode,
-- pauses/resumes play around the seek.

	vid:set_time( t )

-- ## set_time_cano

-- Seeks using a normalized value in [0, 1] (multiplied by duration internally).

	vid:set_time_cano( val )

-- ## set_time_factor

-- Sets the playback speed multiplier. 1.0 = normal, 2.0 = double speed, etc.

	vid:set_time_factor( val )

-- ## is_frame_for_time

-- Returns true if the video is already displaying the frame that corresponds
-- to time t (at the current fps). Useful to avoid redundant seeks.

	b = vid:is_frame_for_time( t )


-- # VIDEO : transport

-- ## start / stop / play

-- start() begins playback (restarts from beginning if at end).
-- stop() pauses playback.
-- play() is an alias for start().
-- All three call update() after setting the transport state.

	vid:start()
	vid:stop()
	vid:play()

-- ## restart

-- Triggers a restart (rewind to frame 0) and updates.

	vid:restart()

-- ## toggle

-- If playing, stops. If stopped, starts.

	vid:toggle()

-- ## toggle_and_reset_at_start

-- If playing, stops. If stopped, restarts then starts.

	vid:toggle_and_reset_at_start()

-- ## toggle_and_reset_at_stop

-- If playing, restarts then stops (parks at frame 0). If stopped, starts.

	vid:toggle_and_reset_at_stop()

-- ## place_at_start

-- Positions the video on its first frame and stops. Internally starts playback
-- briefly to force the decoder to produce frame 0, then stops. Returns false
-- if the video is not usable.

	b = vid:place_at_start()

-- ## set_loop

-- Enables or disables looping.

	vid:set_loop( b )

-- ## is_playing

-- Returns true if the video is currently playing and has not reached the end.

	b = vid:is_playing()

-- ## is_at_end

-- Returns true if playback has reached or passed the total duration.

	b = vid:is_at_end()


-- # VIDEO : scrubbing

-- ## set_scrub

-- Enters or exits scrub mode. On enter, remembers the current playing state
-- and stops. On exit, restores the previous playing state.

	vid:set_scrub( b )

-- ## is_scrub

-- Returns true if scrub mode is active.

	b = vid:is_scrub()

-- ## scrub

-- Sets the scrub target time (seconds). The actual seek happens in update().

	vid:scrub( t )

-- ## scrub_cano

-- Sets the scrub target as a normalized value in [0, 1].

	vid:scrub_cano( val )


-- # VIDEO : audio

-- ## set_sound

-- Enables audio with volume and optional pan. If vol is 0, audio is disabled.

	vid:set_sound( vol [, pan] )

-- ## init_sound_on

-- Shortcut for set_sound( 1, 0 ) (full volume, centered pan).

	vid:init_sound_on()

-- ## set_volume

-- Sets the audio volume (0 to 1).

	vid:set_volume( v )

-- ## set_pan

-- Sets the audio pan position.

	vid:set_pan( v )


-- # VIDEO : capture (webcam / screen grab)

-- ## set_capture_open_run

-- Opens and starts (or closes and stops) the capture device in one call.

	vid:set_capture_open_run( b )

-- ## restart_capture

-- Restarts the capture by closing and reopening the device within one frame.

	vid:restart_capture()

-- ## is_capture_opened / is_capture_running

-- Status queries for the capture device.

	b = vid:is_capture_opened()
	b = vid:is_capture_running()


-- # VIDEO : luma keying

-- ## set_luma

-- Enables or disables luma-based transparency keying on the video output.
-- min/max define the luminance range. b_inverse inverts the key.

	vid:set_luma( b_on [, min, max [, b_inverse]] )


-- # VIDEO : advanced

-- ## set_on_cpu

-- Keeps the decoded video frames in CPU memory (in addition to the GPU texture).
-- Useful for pixel readback or CPU-side processing.

	vid:set_on_cpu( b )

-- ## set_move_to_memory

-- Controls the move-to-texture pipeline parameter.

	vid:set_move_to_memory( b )

-- ## add_play_on_page

-- Associates this video with a page number. The video's duration is cached.

	vid:add_play_on_page( page )

-- ## update

-- Internal per-frame update. Handles scrub seeks, mode detection, and
-- calls aaa.obj.update on the underlying C++ object.

	vid:update()

-- ## release / free

-- release() frees the video's decoder resources (triggers free_all_trig).
-- free() calls release then deletes the C++ object entirely.

	vid:release()
	vid:free()


-- # VIDEO : multi-language

-- ## switch_version

-- Switches to the video variant for the given language code. Preserves the
-- current playback time and playing state across the switch.

	vid:switch_version( lang_code )


-- # VIDEO : codec detection

-- ## pick_mode_working

-- Auto-detects the best DirectShow mode for the current file by trying
-- FFdshow, RenderFile, and generic in sequence. Stops at the first mode
-- that successfully produces a video frame. Called automatically by
-- set_file() and by update() when needed.

	b = vid:pick_mode_working()

-- ## set_need_to_pick_reading_method

-- Flags the video for mode re-detection on next update().

	vid:set_need_to_pick_reading_method( b )


-- # VIDEOS

-- VIDEOS is a singleton pool that pre-allocates VIDEO objects and manages
-- a dedicated bind range for video textures. It handles directory resolution,
-- file lookup/caching, and multi-language variant creation.
-- All methods use colon syntax (VIDEOS is instantiated once at boot).

-- ## Constants

-- BIND_VIDEO_START (512): first GPU texture bind reserved for video.
-- BIND_VIDEO_NB (128): total number of video slots. Bind range is 512..639.

	VIDEOS.BIND_VIDEO_START                       -- 512
	VIDEOS.BIND_VIDEO_NB                          -- 128

-- ## init

-- Pre-allocates BIND_VIDEO_NB VIDEO objects (cloned from the gabu_video_layer
-- template). Called once at boot. Subsequent calls are no-ops.

	VIDEOS:init()

-- ## reinit

-- Resets the pool state: clears the name registry, resets the slot counter
-- and the directory. Does not delete the VIDEO objects themselves.

	VIDEOS:reinit()

-- ## free

-- Releases all video decoder resources (calls release() on each VIDEO)
-- and resets the pool.

	VIDEOS:free()


-- # VIDEOS : directory

-- ## set_dir / get_dir

-- Sets or gets the working directory prepended to all filenames.
-- A trailing "/" is enforced automatically.

	VIDEOS:set_dir( dir_name )
	dir = VIDEOS:get_dir()

-- ## set_dir_media

-- Sets the directory relative to the APP media root (app.media_dir_rel).

	VIDEOS:set_dir_media( dir_name )

-- ## make_fname

-- Prepends the current directory to a filename.

	full = VIDEOS:make_fname( filename )


-- # VIDEOS : getting videos

-- ## get_new

-- Creates a new VIDEO for the given filename: allocates a slot, calls
-- set_file (which triggers codec detection), and registers in the cache.
-- If an APP with LANG is active, also loads translated variants.
-- Returns nil on failure (file missing, pool exhausted, codec failure).

	vid = VIDEOS:get_new( filename [, b_lang] )

-- ## get

-- Returns the VIDEO for filename if already loaded, or calls get_new
-- to load it. The primary entry point for video access.

	vid = VIDEOS:get( filename )

-- ## find

-- Looks up a loaded VIDEO by filename. Returns nil with an error if
-- not found (does not load).

	vid = VIDEOS:find( filename )


-- # VIDEOS : sound

-- ## get_sound

-- Loads a video file as audio-only (checks MEDIA.is_name_sound first).
-- Enables sound with full volume and stops playback. Shows an error dialog
-- on failure unless b_no_error is true.

	vid = VIDEOS:get_sound( filename [, b_no_error] )

-- ## get_sound_new

-- Same as get_sound but always allocates a new slot (never reuses a cached one).

	vid = VIDEOS:get_sound_new( filename [, b_no_error] )


-- # VIDEOS : queries

-- ## get_bind_from_id

-- Converts a 1-based video slot index to its GPU texture bind.
-- bind = (id - 1) + BIND_VIDEO_START. Errors if id is out of [1, BIND_VIDEO_NB].

	bind = VIDEOS:get_bind_from_id( id )

-- ## get_bind_max

-- Returns the first bind after the video range (BIND_VIDEO_START + BIND_VIDEO_NB).

	bind = VIDEOS:get_bind_max()

-- ## get_nb_used

-- Returns the number of video slots currently allocated.

	n = VIDEOS:get_nb_used()

-- ## get_free_index

-- Returns the next available slot index for the given filename, or nil
-- if the file does not exist or the pool is exhausted.

	index = VIDEOS:get_free_index( filename )


-- # VIDEOS : multi-language

-- ## switch_version

-- Switches all loaded VIDEO objects to the variant for the given language code.

	VIDEOS:switch_version( lang_code )
