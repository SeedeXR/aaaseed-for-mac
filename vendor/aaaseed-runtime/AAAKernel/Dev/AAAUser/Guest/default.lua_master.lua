--master lua

if not aaa.__b_first_call_done then

	function aaa.show_file_begin( name )	aaa.print_inverse( "# Begin "..name..".lua" )	   end
	function aaa.show_file_end( name )		aaa.print_inverse( "#     	"..name..".lua Done" ) end

	-- if global capture is present this mean we are running a 201x version on Apple Ios
	--  where we built a reduced AAASeed lua Virtual Machine
	aaa.b_ios = aaa.lua.global.get("capture")~=nil

	if aaa.b_ios then
		aaa.print_debug = print
		unpack = table.unpack
		pack = table.pack
		if not aaa.file then aaa.file = {} end
		aaa.file.read_text = Res.read_text_file
	else
	-- for zeroBrane debugger
		--local pre  = ";M:/AAA/AAAGaBuZo/Pref/lua/luasocket/"
		local pre  = ";Pref/lua/luasocket/"

		aaa.lua.add_package_path( ";M:/Install/Dev/lua/ZeroBraneStudio/lualibs/mobdebug/?.lua" )
	--	package.path = package.path .. "M:/Install/Dev/lua/ZeroBraneStudio/lualibs/?.lua;"
	--	package.cpath = package.cpath .. "M:/AAA/AAAGaBuZo/Pref/lua/luasocket/?.lua;"
		aaa.lua.add_package_path( pre.."lua/?.lua" )


		aaa.lua.add_package_cpath( pre.."socket/?.dll"..pre.."mime/?.dll" )
		--package.cpath = package.cpath..pre.."?.dll"

		dofile( "lua/aaa_jit.lua" )
		--aaa.print_inverse( "Disabling LuaJit on x64" )
		--aaa.jit.set( not param.get_bool( aaa.obj.find_first_by_class_name( "info" ), "x64" ) )
		aaa.jit.dumper = dofile( "lua/aaa_jit_dump.lua" )
		aaa.jit.off()
		aaa.jit.dumper.on()
	end

	dofile( "lua/aaa_debug.lua" )

	if not aaa.dofile then
		function aaa.dofile( fname )

			if aaa.b_ios then
				aaa.print( "try to dofile "..fname )
			end
			--hack
			if false then	--bad for debug message
				local str = aaa.file.read_text( fname )
				if str then
					--aaa.print( fname.."\ncontains\n"..str )
					local fn = aaa.lua.global.get( "fn" ) or loadstring
					--aaa.print( fn.." "..loadstring )
					local f = fn( str )
					if f then
						return f()
					else
						error( "failed to loadstring() text from \""..fname )
					end
				else
					error( "failed to aaa.file.read_text( \""..fname.."\" )" )
				end
			else
				--aaa.print( "Will try dofile( "..fname.." )" )
				return dofile( fname )
			end
		end
		aaa.print_inverse( "aaa.dofile() defined" )
		--hide dofile
		--dofile = nil
	end

	local function doit( name )
		--aaa.print( "will doit "..name )
		return aaa.lua.dofile_protected( name..".lua", true )
		--return aaa.dofile( name..".lua" )
	end
	aaa.doit = doit
	aaa.print_inverse( "aaa.doit defined" )


	--	Don't work well. Maa got message can't find line
	--	local STP = doit "StackTracePlus"
	--	aaa.debug.stp = STP
	--	aaa.debug.print_traceback = aaa.debug.stp.stacktrace
	doit "lua/aaa_table"
	doit "lua/aaa_math"
	doit "lua/aaa_format"
	doit "lua/aaa_info"
	doit "lua/aaa_string"
	doit "lua/aaa_util"

	if not aaa.b_ios then
		doit "lua/aaa_viewport"
		doit "lua/aaa_camera"
		doit "lua/aaa_screen"
		doit "lua/aaa_audio"
		doit "lua/aaa_stereo"
		doit "lua/aaa_power"
		doit "lua/aaa_net"
		doit "lua/aaa_keyboard"
		doit "lua/aaa_mouse"
		doit "lua/aaa_time"
		doit "lua/aaa_file"
		doit "lua/aaa_obj"
		doit "lua/aaa_param"
		doit "lua/aaa_img"
		--serpent have trouble with strict
		SERPENT = doit "lua/serpent"
	end

	doit "lua/gol_util"
	doit "lua/aaa_draw"
	doit "lua/csv_util"	--	for multitouch/info
	doit "lua/vector_2d"
	doit "lua/vector_3d"
	doit "lua/RECT"

	--local bit = require("bit")
	--	AAA OBJ encapsulation
	doit "lua/GABU_OBJ"

    -- LuaSocket
	local path = "lua/luasocket/aaa_"
	aaa.lua.add_package_path( ";lua/luasocket/lua/?.lua" )
	aaa.lua.add_package_cpath( ";lua/luasocket/?.dll" )
	--socket = require( "socket" )
    if not aaa.socket then 
        aaa.socket = {}
        aaa.socket.ref = {}
    end
    doit(path.."ltn12")
    doit(path.."socket")
    doit(path.."headers")
    doit(path.."url")
    doit(path.."http")
	
	aaa.lua.global.set_strict()

	doit "lua/SHADING"

	if aaa.b_ios then
		doit "POINT_LIST"
		doit "FACE_INFO"
		doit "seq"
		doit "seqs"
		doit "STACK"
		doit "APP_FACTORY"
		doit "APP"
		doit "VM_IOS"
	else
		doit "lua/TRANSFO"
		doit "lua/OPENCL"
	--	doit "lua/aaa_osc" -- OSC_MESS now in GaBu_Util (2024 April)
		doit "lua/aaa_layer"
		doit "lua/aaa_layers"
	end

	aaa.lua.global.set( "IS_BUSS_OPEN", 		function() return aaa.lua.global.get( "ga" ) and ga:is_buss_regular_open()	end )
	aaa.lua.global.set( "IS_BUSS_TOP_OPEN",		function() return aaa.lua.global.get( "ga" ) and ga:is_buss_top_open()		end )

	if not aaa.mess then
		aaa.mess = {}
	end
	function aaa.mess.show( str, size, duration )
		if aaa.mess.show_low then
			aaa.mess.show_low( str, size, duration )
		else
			aaa.print( "aaa.mess.show : "..str )
		end
	end

	local function hook_helper( name, b_begin, count )
		if b_begin then
			count = count + 1
		end 
		aaa.print_inverse( "---------------------- Lua hook_"..name.."() "..count..(b_begin and " Begin" or " End") )
		return count
	end
	local count_load_after = 0
	function aaa.hook_env_load_after()
		count_load_after = hook_helper( "env_load_after", true, count_load_after )
		--if aaa.lua.global.get( "ga" ) then ga:save_top_level() end
		hook_helper( "env_load_after", false )
	end
	local count_save_after = 0
	function aaa.hook_env_save_after()
		count_save_after = hook_helper( "env_save_after", true, count_save_after )
		if aaa.lua.global.get( "ga" ) then ga:save_top_level() end
		hook_helper( "env_save_after", false )
	end

	aaa.__b_first_call_done = true

--wall.slide_alpha = 1
--[[
	function aaa.midi.hook_set_control( channel, control, value )
		aaa.print( "aaa.midi.hook_set_control( "..channel..", "..control..", "..value.." )" )
		if channel==4 and control==17 then
			wall.period = wall.period + 1
			if wall.period > 4 then wall.period = 1 end
		end
		if channel==1 then
			if control==19 then
				wall.slide_alpha = value
			end
		end
	end
--]]
	local count_load_before = 0
	function aaa.hook_env_load_before( filename )
		--aaa.box_debug( "before" )
		--aaa.box_debug( "aaa.hook_env_load_before( \""..aaa.info.get_exe_name_pure().."\" )" )

		count_load_before = hook_helper( "env_save_berfore", true, count_load_before )
		aaa.print( "aaa.hook_env_load_before( "..filename.." )" )
		aaa.print( "\tmachine is "..aaa.net.machine )
		if aaa.pc.is_maa() then
			aaa.print_debug( "force opencl for Maa. this is for the tuto" )
			param.set( aaa.ref.pref_start, "opencl_allow", true )
		end
		hook_helper( "env_save_berfore", false )
	end

end
