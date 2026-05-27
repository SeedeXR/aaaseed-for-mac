--TIME and fps
aaa.show_file_begin( "aaa_time" )

if aaa.time then
	if not aaa.time.ref then
		aaa.time.ref = {}
	end
else
	aaa.time = { ref={} }
	aaa.time.t_real 	= 0
	aaa.time.t 			= 0
	aaa.time.dt 		= 0
	aaa.time.hour 		= 0
	aaa.time.minute 	= 0
	aaa.time.second 	= 0
end

function aaa.time.init_ref()
	local r_app = aaa.ref.app
	local ref = aaa.time.ref
	local function pgr_app( pname, dst )
		ref[dst or pname] = param.get_ref( r_app, pname )
	end
	pgr_app(	"year"					)
	pgr_app(	"month"					)
	pgr_app(	"day"					)
	pgr_app(	"hour"					)
	pgr_app(	"minute"				)
	pgr_app(	"second"				)
	pgr_app(	"restart_trig",						"time_restart"		)	
	pgr_app(	"time_real"				)
	pgr_app(	"time"					)
	pgr_app(	"time_delta",						"delta"				)
	pgr_app(	"frame_per_sec",					"fps"				)
	pgr_app(	"frame_per_sec_average",			"fps_average" 		)
	pgr_app(	"frame_per_sec_with_swap",			"fps_swap"			)
	pgr_app(	"frame_per_sec_with_swap_average",	"fps_swap_average"	)
	aaa.time.year_start		=	param.get( ref.year )
	aaa.time.month_start	=	param.get( ref.month )
	aaa.time.day_start		=	param.get( ref.day )
end
function aaa.time.init()
	local t = aaa.time
	t.init_ref()
	t.t_real = param.get( aaa.time.ref.time_real )	-- make sure first substraction will be ok
end

aaa.time.init()

if true then	--todo check for speed
	function aaa.time.get_str_hms()
		local t = aaa.time
		return string.format( "%02dh%02dm%02ds", t.hour, t.minute, t.second )
	end
else
	function aaa.time.get_str_hms()
		local t = aaa.time
		local fn = aaa.format.int_to_char2
		return fn(t.hour).."h"..fn(t.minute).."m"..fn(t.second).."s"
	end
end
function aaa.time.get_str_hms_underscore()
	local t = aaa.time
	return string.format( "%02d_%02d_%02d", t.hour, t.minute, t.second )
end
function aaa.time.get_str_hms_semicolon()
	local t = aaa.time
	return string.format( "%02d:%02d:%02d", t.hour, t.minute, t.second )
end
function aaa.time.get_str_ymd_underscore()
	local t = aaa.time
	return string.format( "%04d_%02d_%02d", t.year_start, t.month_start, t.day )
end
function aaa.time.restart()
	param.set( aaa.time.ref.time_restart, true )
end
function aaa.time.update()
	local ref = aaa.time.ref
	local t = aaa.time
	local pg = param.get
	local t_real_last = t.t_real
	t.t_real	= pg( ref.time_real )
	t.dt_real	= t.t_real - t_real_last
	t.t			= pg( ref.time )
	t.dt		= pg( ref.delta )
	t.day		= pg( ref.day )
	t.hour		= pg( ref.hour )
	t.minute	= pg( ref.minute )
	t.second	= pg( ref.second )
end
function aaa.time.triangle( freq )
	return select( 2, math.modf( aaa.time.t * freq ) )
end
--todo cache it and use time or frame index to recompute
function aaa.time.get_str_fps()
	local ref = aaa.time.ref
	--local str = "FPS "..math.floor(param.get(ref.fps_average)).." / "..math.floor(param.get(ref.fps_swap_average))
	local str = string.format( "%3.0f|%.0f", param.get( ref.fps_average ), param.get( ref.fps_swap_average ) )
	return str
end

aaa.show_file_end( "aaa_time" )
