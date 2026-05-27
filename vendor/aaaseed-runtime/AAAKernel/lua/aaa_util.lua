
--UTIL
aaa.show_file_begin( "aaa_util" )

--aaa.print_err	= aaa.print_error
--aaa.print_dbg	= aaa.print_debug

--aaa.box_err		= aaa.box_error
aaa.box_debug 	= aaa.box_warning
--aaa.box_dbg 	= aaa.box_debug

function pack(...)	return {...}	end

--
--	AAA extension
--
function aaa.activate( t )
	for _, v in ipairs( t ) do
		aaa.print_inverse( "ACTIVATE obj : "..v )
		param.set( v, "active", true )
		aaa.obj.update_then_draw( v )
	end
end

function aaa.deactivate( t )
	for _, v in ipairs( t ) do
		aaa.print_inverse( "DE ACTIVATE obj : "..v )
		param.set( v, "active", false )
	end
end

--
--	LUA
--

function switch( t )
	t.case = function( self, x )
		local f = self[x] or self.default
		if f then
			if type(f) == "function" then
				f( x, self )
			else
				error( "case "..tostring(x).." not a function" )
			end
		end
	end
	return t
end

--
--	REFERENCES
--
if aaa.b_ios then
	return
end

function aaa.init_ref_first()
	local pgr = param.get_ref
	aaa.ref = {}
	local ref = aaa.ref
	ref.pref_start	= aaa.obj.get_from_top_by_class( "pref_start" )
	ref.gol_ui		= aaa.obj.get_from_top_by_class( "gol_ui" )
	ref.pref		= aaa.obj.get_from_top_by_class( "pref" )
		ref.edit							= pgr( ref.pref,		"edit"					)
		ref.camera_edit						= pgr( ref.pref,		"camera_edit"			)
		ref.fullscreen						= pgr( ref.pref,		"fullscreen"			)
		ref.fullscreen_mode					= pgr( ref.pref,		"fullscreen_mode"		)
		ref.fullscreen_set_trig				= pgr( ref.pref,		"fullscreen_set_trig"	)
		ref.fullscreen_flip_trig			= pgr( ref.pref,		"fullscreen_flip_trig"	)
		ref.window_x						= pgr( ref.pref,		"window_x" 				)
		ref.window_y						= pgr( ref.pref,		"window_y" 				)
		ref.window_sx						= pgr( ref.pref,		"window_size_x" 		)
		ref.window_sy						= pgr( ref.pref,		"window_size_y" 		)
		ref.check_memory					= pgr( ref.pref,		"check_memory"			)
		ref.check_heap						= pgr( ref.pref,		"check_heap"			)
		ref.window_force					= pgr( ref.pref,		"window_force"			)
		ref.window_force_x					= pgr( ref.pref,		"window_force_x"		)
		ref.window_force_y					= pgr( ref.pref,		"window_force_y"		)
		ref.window_force_size_x				= pgr( ref.pref,		"window_force_size_x"	)
		ref.window_force_size_y				= pgr( ref.pref,		"window_force_size_y"	)
		ref.window_push_trig				= pgr( ref.pref,		"window_push_trig"		)
		ref.console_push_trig				= pgr( ref.pref,		"console_push_trig"		)
		ref.mouse_x							= pgr( ref.pref,		"mouse_x"		)
		ref.mouse_y							= pgr( ref.pref,		"mouse_y"		)

	ref.flatland	= aaa.obj.get_from_top_by_class( "flatland" )
		ref.draw_focus						= pgr( ref.flatland,	"draw_focus"			)
		--todo make sur set_and_save is defined before
		param.set( ref.draw_focus, false )
		param.set_save( ref.draw_focus, false )
		ref.draw_info						= pgr( ref.flatland,	"draw_info"				)
		ref.draw_focus_offset_x				= pgr( ref.flatland,	"draw_focus_offset_x"	)
		ref.draw_focus_offset_y				= pgr( ref.flatland,	"draw_focus_offset_y"	)
	ref.app		= 	aaa.obj.get_from_top_by_class( "app" )
		ref.frame_index						= param.get_ref( ref.app,	"frame_render_index"	)
		
	local tab = {}
	tab.obj	= aaa.obj.get_from_top_by_class( "lua_master" )
		tab.file_check_active				= pgr( tab.obj,			"file_check_active"			)
		tab.error_trig_dialog				= pgr( tab.obj,			"error_trig_dialog"			)
		tab.error_trig_editor				= pgr( tab.obj,			"error_trig_editor"			)
--		tab.compile_trig_dialog				= pgr( tab.obj,			"compile_trig_dialog"		)
	ref.lua_master = tab
	local img_master = {}
	img_master.obj	= aaa.obj.get_from_top_by_class( "img_master" )
		img_master.free_when_on_board		= pgr( img_master.obj,	"free_when_on_board" )
		img_master.scale_on_load_active		= pgr( img_master.obj,	"scale_on_load_active"		)
		img_master.scale_on_load_factor		= pgr( img_master.obj,	"scale_on_load_factor"		)
		img_master.scale_on_load_size_min	= pgr( img_master.obj,	"scale_on_load_size_min"	)
		img_master.scale_on_load_save		= pgr( img_master.obj,	"scale_on_load_save"		)
		img_master.scale_on_load_path		= pgr( img_master.obj,	"scale_on_load_path"		)
	ref.img_master = img_master

	local file_master = aaa.obj.get_from_top_by_class( "file_master" )
		ref.use_filesystem					= pgr( file_master,		"use_filesystem"		)

	local shader_master = aaa.obj.get_from_top_by_class( "shader_master" )
		ref.shader_reload_trig				= pgr( shader_master,		"force_reload_trig"		)

end

aaa.init_ref_first()

function aaa.flatland.set_draw_focus( b )		param.set( aaa.ref.draw_focus, b )	end
function aaa.flatland.is_draw_focus()			return param.get_bool( aaa.ref.draw_focus )	end

function aaa.flatland.set_draw_info( b )		param.set( aaa.ref.draw_info, b )	end
function aaa.flatland.set_draw( b )
	aaa.flatland.set_draw_focus( b )
	aaa.flatland.set_draw_info( b )
end
function aaa.flatland.set_focus_offset( x, y )
	local ref = aaa.ref
	param.set( ref.draw_focus_offset_x, x )
	param.set( ref.draw_focus_offset_y, y )
end
function aaa.flatland.set_color_blind( b )
	local ref = aaa.ref
	param.set( ref.flatland, "Interface_Gradient",    not b )
	param.set( ref.flatland, "Back_Color_High_red",   b and .5 or .5 )
	param.set( ref.flatland, "Back_Color_High_green", b and .5 or .5 )
	param.set( ref.flatland, "Back_Color_High_blue",  b and .5 or 1  )
	param.set( ref.flatland, "Back_Color_High_alpha", b and 1  or 1  )
end

function aaa.set_check_memory( b )
	aaa.print_inverse( "Switch check Memory to "..b )
	local ref = aaa.ref
	param.set( ref.check_memory,	b )
	param.set( ref.check_heap,		b )
end

function aaa.set_lua_silent( b )
	b = not( (b==nil) and true or b )
	local ref = aaa.ref.lua_master
	param.set( ref.error_trig_dialog,	b )
	param.set( ref.error_trig_editor,	b )
--	param.set( ref.compile_trig_dialog,	b )
end

if not aaa.pc then	aaa.pc = {}	end
function aaa.pc.is_maa()
	--todo make a separate file and aaa.user to centralize user stuff in APP now
	--return ga:is_user_name( "Maa" )
	local machine_table = { "MAAHEX", "MAAQUAD", "MAAREO", "MSI-3070-D" }
	return table.find_key_by_val( machine_table, aaa.net.machine )
end
function aaa.pc.is_dev()	return	aaa.pc.is_maa() or aaa.net.machine == "TATOON" end
--
--	EDIT
--
function aaa.is_edit()		return param.get_bool( aaa.ref.edit )	end
function aaa.set_edit( b )	return param.set( aaa.ref.edit, b )		end

function aaa.update_util()
	if not aaa.stereo.ref.obj then
		aaa.stereo.init_ref()
		aaa.keyboard.init_ref()
		aaa.mouse.init_ref()
		aaa.audio.init_ref()
		--	April 2013 we need this for fast video on intel
		local b_intel = param.get_bool( aaa.ref.gol_ui, "opengl_is_intel" )
		local texture_flux_master = aaa.obj.get_from_top_by_class( "texture_flux_master" )
		if b_intel then
			param.set( texture_flux_master, "move_to_gpu_flip_bgr", b_intel )	--	flip if intel
		end
	end

	aaa.time.update()
	aaa.stereo.update()
	aaa.keyboard.update()
	aaa.mouse.update()
end

--
--	QUIT EXIT SHUTDOWN
--
function aaa.set_shutdown_reboot( b_on )	param.set( aaa.ref.app, "force_shutdown_reboot",	b_on )	end
function aaa.set_shutdown_on_exit( b_on )	param.set( aaa.ref.app, "force_shutdown",			b_on )	end
function aaa.set_quit_do_save( b_on )		param.set( aaa.ref.app, "quit_do_save",				b_on )	end

function aaa.quit_save()
	aaa.set_quit_do_save( true )
	param.set( aaa.ref.app, "quit_trig", true )
end

function aaa.quit_no_save()
	aaa.set_shutdown_reboot( false )
	aaa.set_shutdown_on_exit( false )
	param.set( aaa.ref.app, "quit_no_save_trig", true )
end
function aaa.quit()
	aaa.set_shutdown_reboot( false )
	aaa.set_shutdown_on_exit( false )
	aaa.quit_save()
end

function aaa.shutdown()
	aaa.set_shutdown_reboot( false )
	aaa.set_shutdown_on_exit( true )
	aaa.quit_save()
end
function aaa.shutdown_no_save()
	aaa.set_shutdown_reboot( false )
	aaa.set_shutdown_on_exit( true )
	param.set( aaa.ref.app, "quit_no_save_trig", true )
end
function aaa.reboot()
	aaa.set_shutdown_reboot( true )
	aaa.set_shutdown_on_exit( true )
	aaa.quit_save()
end
function aaa.reboot_no_save()
	aaa.set_shutdown_reboot( true )
	aaa.set_shutdown_on_exit( true )
	param.set( aaa.ref.app, "quit_no_save_trig", true )
end

--
--	EXPLORER
--
function aaa.kill_explorer()
	aaa.print_inverse( "Terminate explorer.exe process" )
	aaa.os.execute_shell(  "taskkill.exe /f /im explorer.exe" )
end

function aaa.launch_explorer( dir )
	aaa.print_inverse( "launch explorer" )
	if dir then
		aaa.os.execute_shell(  "start %windir%\\explorer.exe "..dir )
	else
		aaa.os.execute_shell(  "start %windir%\\explorer.exe" )
	end
end

function aaa.obj.set_focus_ui( ref )
	aaa.flatland.set_draw_focus( true )
	aaa.obj.become_ui( ref )
	aaa.obj.set_focus( ref )
end

function aaa.os.open_dir( dir )
	if dir then
		aaa.print( "open_dir : "..dir )
		--self:box_debug( dir )
		aaa.os.execute_process(  "explorer", dir )
	else
		aaa.print_error( "Can't open nil dir" )
	end
end


aaa.show_file_end( "aaa_util" )
