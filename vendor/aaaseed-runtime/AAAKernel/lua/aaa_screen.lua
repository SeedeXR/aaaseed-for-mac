--todo split in window/ screen
--	add aaa.window
--SCREEN
aaa.show_file_begin( "aaa_screen" )

function aaa.force_window( x,y, sx,sy )
	local ref = aaa.ref
	if x then
		param.set( ref.window_force, 		true )
		param.set( ref.window_force_x, 		x )
		param.set( ref.window_force_y, 		y )
		param.set( ref.window_force_size_x, sx )
		param.set( ref.window_force_size_y, sy )
	else
		param.set( ref.window_force, 		false )
	end
end
function aaa.push_window()			param.set( aaa.ref.window_push_trig, 	true )	end
function aaa.push_console()			param.set( aaa.ref.console_push_trig, 	true )	end
function aaa.force_fullscreen()		param.set( aaa.ref.fullscreen_set_trig,	true )	end

function aaa.set_fullscreen( mode )
	if mode then
		mode = string.lower(mode)
		if		mode == "left"	then 	mode = 1
		elseif	mode == "right"	then 	mode = 2	--todo use screen number or even position of screen
		elseif	mode ~= "all"	then	mode = nil
		end
		if mode then
			param.set( aaa.ref.fullscreen_mode, mode )
		end
	end
	aaa.force_fullscreen()
end
function aaa.get_fullscreen_mode()
	return param.get( aaa.ref.fullscreen_mode )
end
function aaa.is_fullscreen()
	return param.get_bool( aaa.ref.fullscreen )
end

function aaa.set_monitor_off()		param.set( aaa.ref.pref, "monitor_set_off_trig", true )	end
function aaa.set_monitor_on()		param.set( aaa.ref.pref, "monitor_set_on_trig",  true )	end

function aaa.set_window_size( x, y )
	local ref = aaa.ref
	param.set( ref.window_sx, x )
	param.set( ref.window_sy, y )
end
function aaa.get_window_size()
	local ref = aaa.ref
	return param.get( ref.window_sx ), param.get( ref.window_sy )
end

--	SCREEN
--
if not aaa.screen then
	local pgr = param.get_ref

	aaa.screen = {}
	local r = {}
	aaa.screen.ref = r
	r.display_info = aaa.obj.get_from_top_by_class( "display_info" )

	local di = r.display_info

	r.window_left			= pgr( di, "window_left"	)
	r.window_right			= pgr( di, "window_right"	)
	r.window_top			= pgr( di, "window_top"		)
	r.window_bottom			= pgr( di, "window_bottom"	)
	r.window_sx				= pgr( di, "window_sx"		)
	r.window_sy				= pgr( di, "window_sy"		)

	r.display_trig			= pgr( di, "update_trig"	)
	r.display_monitor_nb	= pgr( di, "screen_nb"		)

	r.display_left			= pgr( di, "display_left"	)
	r.display_right			= pgr( di, "display_right"	)
	r.display_top			= pgr( di, "display_top"	)
	r.display_bottom		= pgr( di, "display_bottom" )
	r.display_sx			= pgr( di, "display_sx"		)
	r.display_sy			= pgr( di, "display_sy"		)

	r.screen = {}
	for i=1,6 do
		local tab = {}
		r.screen[i] = tab
		local n = "screen_"..i.."_"
		tab.os_id	= pgr( di, n.."os_id"	)
		tab.left	= pgr( di, n.."left"	)
		tab.right	= pgr( di, n.."right"	)
		tab.top		= pgr( di, n.."top"		)
		tab.bottom	= pgr( di, n.."bottom"	)
		tab.sx		= pgr( di, n.."sx"		)
		tab.sy		= pgr( di, n.."sy"		)
		tab.scale	= pgr( di, n.."scale"	)
	end
end

function aaa.screen.get_nb()
	return	param.get( aaa.screen.ref.display_monitor_nb )
end
function aaa.screen.get_lbrt( screen_id )
	local pg = param.get
	local ref = aaa.screen.ref
	local l,b, r,t
	if screen_id==0 then
		l,b, r,t = pg(ref.display_left), pg(ref.display_bottom), pg(ref.display_right), pg(ref.display_top )
	else
		local rs = ref.screen[screen_id]
		if pg(rs.os_id) == 0 then
			rs = ref.screen[1]
		end
		l,b, r,t = pg(rs.left), pg(rs.bottom), pg(rs.right), pg(rs.top)
	end
	return l,b, r,t
end
function aaa.screen.get_sxy( screen_id )
	local l,b, r,t = aaa.screen.get_lbrt( screen_id )
	return r-l, b-t
end
function aaa.screen.get_scale( screen_id )
	local scale = screen_id==0 and 1 or param.get( aaa.screen.ref.screen[screen_id].scale ) * .01
	aaa.print( "scale is "..scale )
	return scale
end
function aaa.screen.get_viewport_cano( screen_id )
	aaa.print_fn()
	local l,b, r,t = aaa.screen.get_lbrt( screen_id )
	local ref =	aaa.screen.ref
	local win_l = param.get( ref.window_left )
	local win_t = param.get( ref.window_top )
	l, r = l-win_l, r-win_l
	b, t = b-win_t, t-win_t

	local osx =	1 /	param.get( ref.window_sx )
	local osy =	1 /	param.get( ref.window_sy )
	l,b, r,t = l*osx, b*osy, r*osx, t*osy
	aaa.print( "get_viewport_cano() -> lbrt "..l.." "..b.." "..r.." "..t )
	return l,b, r,t
end

function aaa.screen.set_camera( cam, screen_id )
	aaa.print_fn()
	if screen_id > 0 then
		local l,b, r,t = aaa.screen.get_viewport_cano( screen_id )
		AAACAM.set_viewport_from_ref( cam, true, l, 1-b, r, 1-t )
	else
		---aaa.print( "yuiyuiyuiyi" )
		AAACAM.set_viewport_from_ref( cam, false )
	end
end


aaa.show_file_end( "aaa_screen" )
