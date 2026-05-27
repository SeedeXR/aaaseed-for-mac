--maj

if aaa.show_file_begin( "UPDATER" ) then
	UPDATER:set_class_status_doc(	CLASS.STATUS.CORE,
									"used by Jean as part of an inside installer, unfinished" )
end

if CLASS.DECLARE( "UPDATER" ) then
	UPDATER:set_class_status_doc(	CLASS.STATUS.CORE,
									"encapsulate a C 2d transformation object defined using 3 points" )
end
function UPDATER:create( name )
	local self = UPDATER:create_instance( name )
	self.__types = { "file", "dir", "app", "exe", "env", "media", "dlls" }
	self.__target = {
				type = nil,
				master_ip = nil,
				location = nil,
				archive_loc = nil,
				}
	--self.__print = self.print
	return self
end

function UPDATER:print_net( str )
	aaa.net.lua_send( 1, 0, "aaa.print( \""..str.."\" )" )
end

function UPDATER:print( str )
	self:print_net( str )
	oo.getsuper( UPDATER ):print( str )
end

function UPDATER:call_net( meu_updater )
	aaa.net.lua_send( 1, 0, "app:get_meu_by_name( \""..meu_updater.."\" ).pong( \""..self.."\" )" )
end

function UPDATER:hi( ip, meu_updater )
	self:print( "aaa.updater.hi( ip : "..ip.." meu_updater : "..meu_updater..")" )
	table.print( self, self.."in UPDATER:hi()")
	table.print( self.__target, "field __target in UPDATER:hi()" )
	self:call_net( meu_updater )
	--aaa.net.lua_send( 1, 0, "\""..meu_updater.."\".pong( \""..updater.."\" )" )
	--aaa.net.lua_send( 0, 0, "\""..meu_updater.."\".pong( \""..updater.."\" )" )
end

function UPDATER:test_log( location, dirbase, filebase )
	aaa.file.begin_copy( location, dirbase, filebase )
	aaa.file.__log:add( "Testing logging, without direct write to file and print to term.", true, true )
	aaa.file.__log:add( "Testing logging, don't worry" )
	aaa.file.__log:add_error( "Just testing error logging, don't worry", true, true )
	aaa.file.__log:add_error( "Just testing error logging, don't worry" )
	aaa.file.end_copy()
end

--[[
	The logic here, in 5 steps, detailled comments above the corresponding function :
		- set_target : sets the ip and folder where the update files are in a 7z
		- check_target : just checking existence of target on network
		- archive : copy the update target, (old version) to an archive folder
		- do_copy : ... (eventually kill AAASeed before)
		- relaunch : ...
	In update process do the following from master machine :
		copy target location to a shared folder without password
			the location is the same as in env AAAGaBuZo and 7zipped
			ex : update type is file, file is location=AAAGaBuZo/AAAKernel/APP/APP_Kinect/AAA_MEU/ExampleMeu_1/examplemeu.lua
					1-copy it to shared folder/location.7z
					2-call set_target from meu maj with ip, type(=file), location
					3-call do_maj
					4-pray in direction of the ISS
--]]

-- Sets target and some option on the maj/update process depending on the type of target
function UPDATER:set_target( master_ip, type, location )
	self:print( "### Now we set target properties for the next update. ###")
	self:print( "Doing a set_target from master ip : "..master_ip..", type : "..type..", location : "..location )
	local target = self.__target
	local types = self.__types

	local function set_target_loc()
		target.location = location
		local count = 1
		local dirs = aaa.dir.get_dirs( "../archives" )
		if dirs == nil then
			aaa.dir.create( "../archives" )
		end
		for i, dir in IPAIRS(dirs) do
			if string.find( dir, type ) then
				count = count + 1
			end
		end
			-- It is a hack, do it in dirparser constructor/get_dir called from ourdir_lua
			local start = aaa.dir.get_dir_start()
			aaa.dir.push_def( start )
		target.archive_loc = "../archives/"..type.."_"..count.."/"..location
		self:print( "setting archive loc to : "..target.archive_loc )
	end

	for t in IPAIRS(types) do
		if type == types[t] then
			target.master_ip = master_ip
			target.type = type
			set_target_loc()
			table.print( target, "target" )
			if type == "file" or type == "dir" or type == "media" then
				self.__b_need_kill = false
			elseif type == "app" or type == "exe" then
				self.__b_need_kill = true
			elseif type == "env" then
				-- We may want to kill watchdog (ourself here!) so either don't copy over it or launch from archive
				self.__b_need_kill = true
				self.__b_copy_over = true
			end
		end
	end
end

-- Checks the existence of file source of update on network folder
function UPDATER:check_target()
	local target = self.__target

	if target.master_ip == nil then	self:print( "target.master_ip is nil, we stop here..." )	return	end

	local net_dir = "//"..target.master_ip.."/AAAmaje"
	self:print( "we search for the existence of dir "..net_dir )
	if aaa.dir.is_exist( net_dir ) then
		aaa.print_inverse( "we have a directory \""..net_dir.."\"" )
		target.net_dir = net_dir
	end
	--	try to find the 7z
	local name = net_dir.."/"..target.location
	self:print( "\tnow searching for file : "..name )
	if aaa.file.is_exist( name ) then
		self:print( "maj file found" )
		return name
	end
end

-- Archives current version in C:/AAA/archives
function UPDATER:archive_target( type, location, archive_loc )
	local str_err

	if type == "file"  then
		local loc = string.gsub( location, "__", "/" )
		self:print( "Archiving file : "..loc..", to "..archive_loc )
		str_err = aaa.file.copy( loc, archive_loc )
	elseif type == "dir" or type == "app" then
		local loc = string.gsub( location, "__", "/" )
		loc = string.gsub( loc, ".7z", "" )
		self:print( "Archiving "..type.." : "..loc..", to "..archive_loc )
		str_err = aaa.os.execute_shell( "\"C:/Program Files/7-Zip/7z.exe\" a "..archive_loc.." "..loc )
		--str_err = aaa.file.copy_dir( location, archive_loc )
	elseif type == "exe" then
		self:print( "Archiving exe : "..location..", to "..archive_loc )
		str_err = aaa.file.copy( location, archive_loc )
	elseif type == "env" then
		str_err = aaa.os.execute_shell( "\"C:/Program Files/7-zip/7z.exe\" a "..archive_loc )
	end
	if str_err then
		self:print( str_err )
		return false
	end
	return true
end

-- Todo : simplify by doing a get_tmp_dest_str		or directly	copy_net_to_tmp fn
--                            get_extract_dest_str	or directly	extract_from_tmp_to_env fn
function UPDATER:retrieve_target( type, net_name, location )
	local str_err = nil

	if type == "file" or type == "exe" then
		local tmp = string.gsub( location, "__", "/" )
		self:print( "Copying "..net_name.." to "..tmp )
		str_err = aaa.file.copy( net_name, tmp )
	elseif type == "dir" or type == "app" then
		local rev = string.reverse( net_name )
		local rev_cut = string.sub( rev, 0, string.find( rev, "/" ) )
		local name = string.reverse( rev_cut )

		self:print( "Copying "..net_name.." to ../tmp"..name )
		str_err = aaa.file.copy( net_name, "../tmp"..name )
		if str_err then		self:print( str_err )	return false	end

		local tmp = string.gsub( location, "__", "/" )
		local rev_tmp = string.reverse(tmp)
		local loc = string.sub( tmp, 0, #tmp - string.find( rev_tmp, "/" ) )
		--local loc = string.sub( tmp, 0, string.find( tmp, "%.") - 1 ) -- or string.find( tmp, '.', 1, true ) -> true means plain text search

		self:print( "Unzipping from ../tmp"..name.." to "..loc )
		str_err = aaa.os.execute_shell( "\"C:/Program Files/7-Zip/7z.exe\" x -aoa ../tmp\""..name.."\" -o\""..loc.."\"" )
	elseif type == "env" then
		self:print( "Copying "..net_name.." to ../tmp/AAAGabuZo.7z" )
		str_err = aaa.file.copy( net_name, "../tmp/AAAGaBuZo.7z" )
		if str_err then		self:print( str_err )	return false	end

		str_err = aaa.os.execute_shell( "\"C:/Program Files/7-Zip/7z.exe\" x -aoa ../tmp/AAAGaBuZo.7z -o../tmp/\"" )
		if str_err then		self:print( str_err )	return false	end

		self:print( "### It is a whole env update, we have an extra step. ###" )
		self:print( "### Now we copy from temporary extracted env to current. ###")
		watchdog.__delay = 5000
		--self:new_watchdog()
		self:copy_env()
	elseif type == "media" then
		local rev = string.reverse( net_name )
		local rev_cut = string.sub( rev, 0, string.find( rev, "/" ) )
		local name = string.reverse( rev_cut )

		self:print( "Copying "..net_name.." to ../tmp"..name )
		str_err = aaa.file.copy( net_name, "../tmp"..name )
		if str_err then		self:print( str_err )	return false	end

		local tmp = string.gsub( location, "__", "/" )
		local rev_tmp = string.reverse(tmp)
		local last_sep = string.find( rev_tmp, "/" ) and 1 or 0
		local loc = "C:/AAA/"..string.sub( tmp, 0, #tmp - last_sep )

		self:print( "Unzipping from ../tmp"..name.." to "..loc )
		str_err = aaa.os.execute_shell( "\"C:/Program Files/7-Zip/7z.exe\" x -aoa ../tmp\""..name.."\" -o\""..loc.."\"" )
	elseif type == "dlls" then
		local rev = string.reverse( net_name )
		local rev_cut = string.sub( rev, 0, string.find( rev, "/" ) )
		local name = string.reverse( rev_cut )

		self:print( "Copying "..net_name.." to ../tmp"..name )
		str_err = aaa.file.copy( net_name, "../tmp"..name )
		if str_err then		self:print( str_err )	return false	end

		self:print( "Unzipping from ../tmp"..name.." to %AAADll" )
		str_err = aaa.os.execute_shell( "\"C:/Program Files/7-Zip/7z.exe\" x -aoa ../tmp\""..name.."\" -o%AAADll%\"" )
	end
	if str_err then		self:print( str_err )	return false	end
	return true
end

function UPDATER:copy_env()
	local f = aaa.file
	f.begin_copy()

	local files = aaa.dir.get_files( "C:/AAA/tmp/AAAGaBuZo" )
	array.remove_by_val( files, "AAASeed_WatchDog.exe" )
	for i, file in PAIRS( files ) do
		local src_file = "C:/AAA/tmp/AAAGaBuZo/"..file
		local dst_file = "C:/AAA/AAAGaBuZo/"..file
		self:print( "We'll copy "..src_file..", to "..dst_file )
		f.copy_counting( src_file, dst_file, true ) -- ( src, dst, b_overwrite,
	end

	local dirs = aaa.dir.get_dirs( "../tmp/AAAGaBuZo" )
	local exclude_dirs = { ".git", ".vscode", "Pref" }
	array.remove_by_vals_table( dirs, exclude_dirs )
	for i, dir in PAIRS( dirs ) do
		local src_dir = "C:/AAA/tmp/AAAGaBuZo/"..dir
		local dst_dir = "C:/AAA/AAAGaBuZo/"..dir
		self:print( "We'll copy "..src_dir..", to "..dst_dir )
		f.copy_dir( src_dir, dst_dir, true ) -- overwrite = true
	end

	f.end_copy()
end

function UPDATER:new_watchdog()
	--	set watchdog.aaaseed_exe_name

	--  rename watchdog to watchdog_old
	local AAASeed_exe_base = "C:/AAA/AAAGaBuZo/AAASeed_"
	aaa.file.move( AAASeed_exe_path.."watchdog.exe", AAASeed_exe_path.."watchdog_old.exe" ) -- fail if dst exist ?

	--	copy exe to watchdog...
	aaa.file.copy( watchdog.aaaseed_exe_name, AAASeed_exe_path.."watchdog.exe", true )

	--	start it
	watchdog.start_exe( AAASeed_exe_base.."watchdog.exe" )

	--	sepuku
	aaa.param.set( aaa.ref.app, "quit_no_save_trig",	1 )
end

function UPDATER:do_maj()
	local target = self.__target
	self:print( "### Now we check if target is available on network. ###")
	local name = self:check_target()

	if not name then	self:print( "Abort update : Could not reach target file" )	return	end

	if self.__b_need_kill then

		local str_time = aaa.format.real_dot2( watchdog.get_time() )
		self:print( str_time.." Killing AAASeed" )
		watchdog.kill_exe( watchdog.aaaseed_exe_name, 100, 5 ) -- sleep 500ms, retry 5 times
	end
	self:print( "### Now we archive current target file before it is updated. ###")
	if not self:archive_target( target.type, target.location, target.archive_loc ) then
		self:print( "Error archiving target" )
	end
	self:print( "### Now we retrieve target file available from network dir. ###")
	if not self:retrieve_target( target.type, name, target.location ) then
		self:print( "Error retrieving target" )
		aaa.net.lua_send( 1, 0, "app:get_meu_by_name( \"AAAUpdate\" )._b_updating = false" )
		self:print( "### Update FAILED on machine "..aaa.net.machine..", with ip "..aaa.net.ip[1]..". ###")
	else
		aaa.net.lua_send( 1, 0, "app:get_meu_by_name( \"AAAUpdate\" )._b_updating = false" )
		self:print( "### Update SUCCEEDED on machine "..aaa.net.machine..", with ip "..aaa.net.ip[1]..". ###")
	end
end

aaa.show_file_end( "UPDATER" )

