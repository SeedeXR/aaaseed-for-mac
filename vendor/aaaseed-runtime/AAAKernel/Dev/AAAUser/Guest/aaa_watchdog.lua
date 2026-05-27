--master lua
if not aaa.dofile then
	function aaa.dofile( fname )
		if false then	--bad for debug message
			local str = aaa.file.read_text( fname )
			if str then
				local f = loadstring( str )
				if f then
					return f()
				else
					error( "failed to loadstring() text from \""..fname.."\" )" )
				end
			else
				error( "failed to aaa.file.read_text( \""..fname.."\" )" )
			end
		else
			--aaa.print( "Will try dofile( "..fname.." )" )
			return dofile( fname )
		end
	end
	--hide dofile
	--dofile = nil
end

local function doit( name )
	--aaa.print( "will dofile "..name )
	return aaa.dofile( "AAAKernel/lua/"..name..".lua" )
--	return aaa.dofile( name )
--[[
	if b then
		aaa.print( "b is true" )
	else
		aaa.print( b )
	end
	if str then aaa.print( "       str is "..str ) end
--]]
end

if not aaa.__b_first_call_done then
	function aaa.show_file_begin( name )
		aaa.print_inverse( "# do "..name..".lua" )
	end
	function aaa.show_file_end( name )
	end

	local function empty_fn() end
	--function IPAIRS(tab) 	if tab then return ipairs(tab) end return (function() elsend) end
	function IPAIRS(tab) 	if tab then return ipairs(tab) end	return empty_fn end
	function PAIRS(tab) 	if tab then return pairs(tab) end	return empty_fn end

	--[[	done in C++
	aaa.print( "package.path was : "..package.path )
	package.path = package.path..aaa.dir.get_dir_start().."/lua/?.lua;"
	package.path = package.path..aaa.dir.get_dir_start().."/Pref/lua/?.lua;"
	aaa.print( "package.path now is : "..package.path )
	--]]

	doit "aaa_debug"
	--	Don't work well. Maa got message can't find line
	--	local STP = doit "StackTracePlus"
	--	aaa.debug.stp = STP
	--	aaa.debug.print_traceback = aaa.debug.stp.stacktrace
	doit "aaa_jit"

	doit "aaa_table"
	doit "aaa_math"
	doit "aaa_format"
	--doit "aaa_time"
	doit "aaa_string"
	--	doit "aaa_util" -- no pref or app obj in watchdog mode so we can't init aaa.ref
	--doit "aaa_viewport"
	--doit "aaa_camera"
	--doit "aaa_screen"
	--doit "aaa_audio"
	--doit "aaa_stereo"
	--doit "aaa_power"
	doit "aaa_net"
	--doit "aaa_keyboard"
	--doit "aaa_mouse"
	--	doit "aaa_time" -- Can't init aaa.ref in watchdog mode
	doit "aaa_file"
	doit "aaa_obj"
	doit "aaa_param"
	doit "aaa_info"

	doit "aaa_img"
	doit "gol_util"
	doit "aaa_draw"
	doit "csv_util"	--	for multitouch/info
	doit "vector_2d"
	doit "vector_3d"
	doit "RECT"

	--serpent have trouble with strict
	SERPENT = doit "serpent"

	--local bit = require("bit")
	--	AAA OBJ encapsulation
	doit "GABU_OBJ"

	aaa.lua.global.set_strict()

		-- following was introduced for aaaupdate
		doit "UPDATER"

		if not aaa.updater then
			aaa.updater = UPDATER:create( "updater" )
		end

		aaa.dofile( "AAAKernel/GaBu/GaBu_Util/LOG/LOG.lua" )
		aaa.dofile( "AAAKernel/GaBu/GaBu_Util/STACK/STACK.lua" )

	--doit "SHADING"
	--doit "BOID"
	--doit "TRANSFO"
	--doit "OPENCL"
	--doit "aaa_osc"
	--doit "aaa_layer"

	aaa.__b_first_call_done = true
end

function	watchdog.start_exe( exe_name, arg )
	local str = "start "..exe_name
	if arg then
		str = str.." "..arg
	end
	aaa.os.execute_shell( str )	--should be execute_process but need to pass the arg and test
end
function	watchdog.kill_exe( exe_name, sleep, max_try )
	aaa.print( "will kill \""..exe_name.."\"" )
	--aaa.print( process.get_id_by_name( exe_name ) )
	while process.get_id_by_name( exe_name ) and max_try > 0 do
		process.kill_by_name( exe_name )
		aaa.sleep( sleep )
		max_try = max_try - 1
	end
	aaa.print( "\tkilled \""..exe_name.."\"" )
end


watchdog.aaaseed_exe_name = "AAASeed_Metal.exe"
function	watchdog.get_aaaseed_pid()			return process.get_id_by_name( watchdog.aaaseed_exe_name )	end
function	watchdog.kill_aaaseed()				process.kill_by_name( watchdog.aaaseed_exe_name )			end
function	watchdog.start_aaaseed_low( arg )	watchdog.start_exe( watchdog.aaaseed_exe_name, arg )		end
--function	watchdog.start_aaaseed_low( arg )	watchdog.start_exe( aaa.dir.get_dir_start().."/"..watchdog.aaaseed_exe_name, arg )		end

watchdog.watchdog_exe_name = "AAASeed_WatchDog.exe"
function	watchdog.get_watchdog_pid()			return process.get_id_by_name( watchdog.watchdog_exe_name )	end
function	watchdog.kill_watchdog()			process.kill_by_name( watchdog.watchdog_exe_name )			end
function	watchdog.start_watchdog_low( arg )	watchdog.start_exe( watchdog.watchdog_exe_name, arg )		end


function	aaa.sepuku( host_id )
	aaa.print( "SEPUKU("..host_id..")" )
	if host_id == aaa.net.get_host_id() then
		aaa.print( "SEPUKU _____________________  !!!!" )
		aaa.param.set( aaa.ref.app, "force_shutdown",	1 )
		aaa.param.set( aaa.ref.app, "quit_no_save_trig",	1 )
	end
end
--[[
function	aaa.quit( host_id )
	aaa.print( "QUIT("..host_id..")" )
	if host_id == aaa.net.get_host_id() then
		aaa.print( "QUIT _____________________  !!!!" )
		aaa.param.set( aaa.ref.app, "quit_no_save_trig", 1 )
	end
end
--]]

function watchdog.check_maj()
	for i=1,26 do
		local drive = string.char( 64+i )..":\\"
		aaa.print( drive )
		aaa.sleep( 100 )
		if aaa.dir.is_exist( drive ) then
			local dir_maj = drive.."AAAMaj"
			aaa.print( "we search for the existence of dir \""..dir_maj.."\"" )
			if aaa.dir.is_exist( dir_maj ) then
				aaa.print_inverse( "we have a directory \""..dir_maj.."\"" )
				local name
				--	try to find and run a script
				aaa.print( "\tnow searching for maj.lua" )
				name = dir_maj.."\\maj.lua"
				if aaa.file.is_exist( name ) then
					aaa.print( "founded : we run it" )
					aaa.dofile( name )
					return true
				end

				--	try to find and run an exe
				aaa.print( "\tnow searching for maj.exe" )
				name = dir_maj.."\\maj.exe"
				if aaa.file.is_exist( name ) then
					aaa.print( "founded : we execute it" )
					aaa.os.execute_process( name )
					return true
				end
			end
		end
	end
	return false
end

--network_send( "aaa.sepuku("..pc.num..")" )
--really unused ? removed on EO v2 2015

function	watchdog.aaaseed_run()
	watchdog.run_trig_last	= watchdog.run_trig
	watchdog.run_trig		= nil
	watchdog.start_aaaseed_low()
end
function	watchdog.aaaseed_run_trig( id )
	watchdog.run_trig 		= id
end

--
--	MAJ BEGIN : DIOR for now
--
function aaa.file.create_copy_log( dir, file, interval_sec )
	-- todo see why we can't use it ( imply activate doit aaa.time -> no aaa.ref ? )
	--local fname  = aaa.time.year_start.."_"..aaa.time.month_start.."_"..aaa.time.day_start
	aaa.file.__log = LOG:create( "copy_log", dir.."/"..file, interval_sec )--"AAA_LOG/AAA_"..fname..".log", interval_sec, b_read )
end
function aaa.file.create_copy_log_filename( location, dirname, filebase )
	aaa.print( "We'll compose log filename from : "..location..", "..dirname..", "..filebase )
	if not location then
		aaa.print( "No location argument provided for log : we'll use the default one.")
		location = aaa.dir.get_dir_start()
	end
	if not aaa.dir.is_exist( location ) then
		aaa.print( "Location for log doesn't exist. We'll create it." )
		if not aaa.dir.create( location ) then
			aaa.print( "Error creating default dir for log." )
			return
		end
	end

	if not dirname then
		aaa.print( "No dir argument provided for log : we'll use the default one.")
		dirname = "AAALOG"
	end
	dirname = location.."/"..dirname
	if not aaa.dir.is_exist( dirname ) then
		aaa.print( "The dir for log doesn't exist. We'll create it.")
		if not aaa.dir.create( dirname ) then
			aaa.print( "Error creating dir for log" )
			return
		end
	end

	if not filebase then
		aaa.print( "No file base name argument provided for log : we'll use Noname.")
		filebase = "Noname"
	end

	local count = 1
	local files = aaa.dir.get_files( dirname, filebase.."_*" )
	for i, file in IPAIRS(files) do
		if string.find( file, filebase ) then
			count = count + 1
		end
	end
	local filename = filebase.."_"..count

	return dirname, filename
end
function aaa.file.begin_copy( location, dirname, filebase )
	aaa.file.__nb_copied = 0

	local interval_sec = 2147483647
	local dirname, filename = aaa.file.create_copy_log_filename( location, dirname, filebase )
	aaa.file.create_copy_log( dirname, filename, interval_sec )
end
function aaa.file.end_copy()
	aaa.file.__log:write()
	aaa.file.__log = nil
end
function aaa.file.copy_counting( src, dst, b_overwrite )
	local file = aaa.file
	local nb = file.__nb_copied+1
	if nb % 100 == 0 then
		aaa.print( "Copying file number"..nb.." : "..src )
		file.__log:add( "Copying file number"..nb.." : "..src, true, true )
	end
	local err_str = file.copy( src, dst, b_overwrite )
	if not err_str then
		file.__nb_copied = nb
	else
		file.__log:add_error( err_str, true, true )
	end
	return err_str
end

if false then --was done by Jean and not finised to many changes in the file structure since
	function aaa.file.copy_dir( src, dst, b_overwrite ) -- todo handle an exclude list like a .gitignore
		local exclude_files = { ".DS_Store", "Thumbs.db", "PICMDIF.LOG" }
		local file = aaa.file

		local t_file = aaa.dir.get_files( src )
		array.remove_by_vals_table( t_file, exclude_files )
		for v, name in PAIRS( t_file ) do
			file.copy_counting( src.."/"..name, dst.."/"..name, b_overwrite )
		end

		local t_dir = aaa.dir.get_dirs( src )
		--array.remove_by_vals_table( t_dir, exclude_dirs )
		for v, name in PAIRS( t_dir ) do
			--aaa.print( "\t dir : "..src.."/"..name )
			file.copy_dir( src.."/"..name, dst.."/"..name )
		end
	end

	function aaa.file.copy_dir_app( src, dst, dir_app_name, cur_name )
		local file = aaa.file
		local t_file = aaa.dir.get_files( src )
		array.remove_by_val( t_file, ".DS_Store" )
		array.remove_by_val( t_file, "Thumbs.db" )
		array.remove_by_val( t_file, "PICMDIF.LOG" )
		for v, name in PAIRS( t_file ) do
			local err_str = file.copy_counting( src.."/"..name, dst.."/"..name, true )
			if err_str then
				return err_str
			end
		end

		local t_dir = aaa.dir.get_dirs( src )
		if cur_name == "AAAKernel" then
			array.remove_by_val( t_dir, "APP_Examples" )
			array.remove_by_val( t_dir, "APP_oldschool" )
			array.remove_by_val( t_dir, "Chanel" )
			array.remove_by_val( t_dir, "EO" )
			array.remove_by_val( t_dir, "Jean" )
			array.remove_by_val( t_dir, "romZ_mod" )
		elseif cur_name == "AAAKernel/APP" then
			t_dir = { dir_app_name }
		end

		for v, name in PAIRS( t_dir ) do
			--aaa.print( "\t dir : "..src.."/"..name )
			local new_cur_name = cur_name and cur_name.."/"..name or name
			local err_str = file.copy_dir_app( src.."/"..name, dst.."/"..name, dir_app_name, new_cur_name )
			if err_str then
				return err_str
			end
		end
	end
	function watchdog.do_maj( exe_pure_name )
		--aaa.print_inverse( "start "..aaa.dir.get_dir_start() )
		local drive_src = string.sub( aaa.dir.get_dir_start(), 1, 2 )
		local drive_dst = "C:"
		local pa = "/AAA/AAAGaBuZo"
		local err_str

		local function print( str )
			aaa.print_inverse( "MAJ : "..str )
		end
		local function print_error( str )
			aaa.print_error( "MAJ ERROR : "..str )
		end
		local function remove_dir( path )
			if err_str then
				print_error( "So we will not try to remove dir \""..path.."\"" )
				return
			end
			if aaa.dir.is_exist( path ) then
				print( "delete dir \""..path.."\"" )
				local ret = aaa.dir.remove( path )
				--print( ret )
				if ret then
					print( "Done" )
				else
					err_str = "Could not aaa.dir.remove( \""..path.."\" )"
					print_error( err_str )
				end
			end
		end
		local function copy_dir( text, post, b_remove_first )
			local src, dst = drive_src..post, drive_dst..post
			if b_remove_first then remove_dir( dst ) end

			if err_str then
				print_error( "So we will not try to copy dir to\""..dst.."\"" )
				return
			end
			print( "Copy "..text )
			aaa.file.begin_copy()
			err_str = aaa.file.copy_dir( src, dst )
			if not err_str then
				print( "Done" )
			else
				print_error( err_str )
			end
		end

		print( "start an application update process" )
		if exe_pure_name == "maj" then
			print( "killing WatchDog" )
			watchdog.kill_exe( watchdog.watchdog_exe_name, 1000, 10 )
			print( "Waiting 3 secs" )
			aaa.sleep( 3000 )
			print( "Launching Maj2.exe" )
			aaa.os.execute_process( aaa.dir.get_dir_start().."/Maj2.exe" )
			print( "Waiting 3 secs to exit" )
			aaa.sleep( 3000 )
			aaa.os.exit( false, false )
		elseif exe_pure_name == "maj2" then
			aaa.print( "Waiting 10 secs" )
			aaa.sleep( 10000 )
			aaa.file.begin_copy()

			local dst = drive_dst..pa
			if aaa.dir.is_exist( dst ) then
				print( "Rename Old App folder" )
				--we use random to get a new name
				local name_keep = dst.."_Pre_Hydralife_"..aaa.os.get_exe_start_time()
				--can't remove because of dll is used by maj.exe started in C:/AAA/AAAGaBuZo, but rename work
				--if aaa.dir.is_exist( name_keep ) then
				--	print( name_keep.." already exist so we don't try to copy to it" )
				--	remove_dir( dst )
				--else
					print( "Rename previous APP folder to \""..name_keep.."\"" )
					err_str = aaa.file.rename( dst, name_keep )
				--end
			end
			if err_str then
				print_error( "rename of app dir failed we stop update" )
			else
				print( "done" )
				print( "Copy App folder" )
				err_str = aaa.file.copy_dir_app( drive_src..pa, dst, "APP_Dior" )

				local dir_dior = "/AAA/Media/Dior/"

				copy_dir( "Media DreamskinAdvanced",	dir_dior.."DreamskinAdvanced",	true	)

				copy_dir( "Media HydraLife",			dir_dior.."HydraLife",			true	)

				print( "All copies done" )
			end

			print( "Waiting 15 secs before Shutdown" )
			aaa.sleep( 15000 )

			if not err_str then
				print( "Doing Shutdown now" )
				aaa.os.exit( true, false )
			end
		end
		--	aaa.param.set( aaa.ref.app, "force_shutdown",	1 )
	end
end

--
--	MAJ END
--
if not watchdog.test then
	--print( "in test")
	local ref = {}
	watchdog.ref = ref

	ref.net	= aaa.obj.find_first_by_class_name( "net" )
	ref.blk_nb_received = param.get_ref( ref.net, "blk_nb_received" )

	watchdog.__delay = 50.;
	watchdog.__time_last = watchdog.get_time()

	local host_id	= param.get( 		ref.net, "host_id"	)
	local langroup	= param.get_str(	ref.net, "langroup" )
	local machine	= param.get_str(	ref.net, "machine"	)
	aaa.print( machine.." on "..langroup.." with host_id "..host_id )

-- AQUA or EPIDEMIK
--	if inside( host_id, 100, 108 ) then
--		watchdog.aaaseed_run()
--	end

	param.set( aaa.net.get_link_ref(0), "discard_osc_blk", 1 )
	watchdog.set_loop_time( 1. )

	watchdog.kinect = false
-- AQUA or EPIDEMIK
--	if host_id == 139 then
--		watchdog.kinect = true
--	end

-- EPIDEMIK
--	aaa.os.execute_shell( "cd .." )
--	aaa.print( aaa.get_dir_def() )
--	aaa.set_dir_def( "M:\\SME\\AAA\\CSI_sol_all" )

	watchdog.test = 0

	local exe_name = string.lower( aaa.info.get_exe_name_pure() )
	aaa.print( "exe is "..exe_name )
	if exe_name == "maj" or exe_name == "maj2" then
		watchdog.b_maj = true
		watchdog.do_maj( exe_name )
	end

end

function watchdog.start_aaaseed()
	aaa.print( "watchdog.start_aaaseed()" )
	local pid = watchdog.get_aaaseed_pid()
	--avoid starting twice
	if not pid then
		if watchdog.kinect then
			-- one AAASeed for each kinect
			watchdog.start_aaaseed_low( "kinect1" )
			watchdog.start_aaaseed_low( "kinect2" )
			watchdog.start_aaaseed_low( "kinect3" )
			aaa.print( "Starting Kinect AAASeed "..watchdog.aaaseed_exe_name )
		else
			watchdog.start_aaaseed_low()
			aaa.print( "Starting AAASeed "..watchdog.aaaseed_exe_name )
		end
	end
end

--watchdog.start_aaaseed()
--watchdog.trig_exit()


function watchdog.update_low()
	watchdog.b_verbose = true

	if watchdog.run_trig ~= nil then
		--print( "run trig : "..watchdog.run_trig )
		--local pid = watchdog.get_aaaseed_pid()
		--if not pid and
		if ( watchdog.run_trig_last==nil or watchdog.run_trig_last~=watchdog.run_trig ) then
			print( "run trig last : "..watchdog.run_trig_last )
			watchdog.aaaseed_run()
		end
	end
	watchdog.run_trig = 45
--]]

	local time = watchdog.get_time()
	local dt = time - watchdog.__time_last
	watchdog.__time_last = time
	local delay = watchdog.__delay - dt
	--print( delay )
	if delay > 0 then
		watchdog.__delay = delay
	--elseif watchdog.b_updater then
	--	aaa.print( "in updater watchdog, we won't start or kill regular AAASeed from here.")
	--	watchdog.__delay = 60
	else
		local str = aaa.format.real_dot2( watchdog.get_time() )
		local pid = watchdog.get_aaaseed_pid()
		if watchdog.b_verbose then aaa.print( "AAASeed pid is "..pid ) end
		--aaa.print( "process "..name.." have id "..pid )
		if not pid then
			aaa.print( str.." Launching AAASeed" )
			watchdog.__delay = 60	-- time to load App and Media for Dior (40 escs)
			watchdog.start_aaaseed()
		else
			aaa.print( str.." Killing AAASeed" )
			watchdog.__delay = 5
			watchdog.kill_aaaseed()
			--aaa.lua_send( 0, 0, "aaa.sepuku(69)" )
		end
	end
	if watchdog.b_verbose then
		aaa.print( "watchdog received "..param.get(watchdog.ref.blk_nb_received).." blk at time "..time )
	end
	--aaa.print( )
end
function watchdog.update()
	--aaa.print_fn()
	if watchdog.b_maj then
	else
		watchdog.update_low()
	end
end

function watchdog.aaaseed_is_alive( machine_name, machine_ip )
	-- Because that's the interface's ip that app.lua sends too us in the call to this fn via watch.net_send(...)
	--aaa.print( "machine ip : "..machine_ip..", ip[1] : "..aaa.net.ip[1] )
	if machine_ip == aaa.net.ip[1] then
		if watchdog.b_verbose then
			aaa.print( "AAASeed on "..machine_name.." with ip "..machine_ip..
						" is alive "..aaa.format.real_dot2( watchdog.get_time() ) )
		end
		watchdog.__delay = math.max( watchdog.__delay, 30 )
	end
end
--watchdog.trig_exit()
--aaa.print( " hello ")
watchdog.update()

--watchdog.test = nil
--[[
watchdog.test = watchdog.test + 1
if watchdog.test > 10 then
	watchdog.test = 0
	aaa.print( "watchdog alive "..watchdog.test )
end
--]]
