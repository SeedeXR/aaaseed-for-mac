--UTIL
aaa.show_file_begin( "aaa_debug" )

--DEBUG
--
if not aaa.debug then
	aaa.debug = { doc={} }
	aaa.print_error(  "aaa.debug not defined by AAASeed.\n running a pre Jan 28th 2015 AAASeed version" )
end
aaa.debug.doc = {}	-- .doc not defined in C (yet)

function aaa.debug.debug()
	aaa.os.pop_console()
	debug.debug()
end

aaa.debug.doc.get_fn_info = "( level ) Gets info on fn with debug.getinfo return a table of string"
function aaa.debug.get_fn_info( level )
	--level = level
	local t_str
	local info = debug.getinfo( level, "nlS" )
	if info then
		t_str = {}
		local str = ""
		if info.currentline	and info.currentline > 0 then
			str = str .. string.format( "At %4d ", info.currentline )
		end
		if info.linedefined	and info.linedefined > 0 then
			str = str .. string.format( " [%4d-%4d]", info.linedefined, info.lastlinedefined )
		end

		local source = info.source
		if source then
			local dir
			local count = 0
			if app then
				dir = app:get_dir_absolute()
				source,count = string.gsub( source, dir, "" )
			end
			if count == 0 then
				dir = aaa.dir.get_dir_kernel()
				source,count = string.gsub( source, dir, "" )
			end
			-- local b,e = string.find( source, app:get_dir_absolute() )
			-- if b==1 then
			-- 	source = string.sub( source, b+1 )
			-- end
			if count > 0 then
				table.insert( t_str, dir )
			end
			str = str.."    "..source
		end
		table.insert( t_str, str )

		str = "\t"
		--table.print( info, "get_fn_info "..level, 2  )
		if info.what and info.what~="Lua" then
			str = str..info.what .. " "
		end
		-- if		info.what == "C"	then
		-- 	return "C function "..info.namewhat.." "..info.name.."() in "..info.linedefined..":"..info.short_src
		-- els

		if info.namewhat	then	str = str .. "In " .. info.namewhat	end
		if info.name 		then	str = str .. " ".. info.name .. "()"	end
		--str = str .. string.format( "\n%s", info.source )
		-- if	info.name			then	return info.name.."()\t"..info.namewhat.."\tat "..info.currentline.."\tin "..info.linedefined..":"..info.short_src
		-- else 							return info.namewhat.."\tin\t"..info.linedefined..":"..info.short_src
		-- end
		--aaa.print( "level "..level.." debug str is "..str )
		table.insert( t_str, str )
	end
	return t_str
end

aaa.debug.doc.get_fn_name = "( level ) Gets name of fn with debug.getinfo"
function aaa.debug.get_fn_name( level )
	--level = level
	local info = debug.getinfo( level, "nSl" )
	if info then
		if		info.what == "C" 	then 	return "C function "..info.name
		elseif	info.name 			then 	return info.name
		else 								return info.namewhat
		end
	else
		return nil
	end
end

aaa.debug.doc.get_obj = "( level ) Gets obj with debug.getlocal and prints name and value."
function aaa.debug.get_obj( level )
	local name, value = debug.getlocal( level, 1 )
	aaa.print_inverse( "low ->"..name.." "..value )
	return value
end
aaa.debug.doc.get_obj_method = "( level ) Gets obj and method with aaa.debug.get_fn_name( level+1 ) and return them."
function aaa.debug.get_obj_method( level )
	local name, obj = debug.getlocal( level, 1 )
--	aaa.print_inverse( "low ->"..name.." "..value )
	return obj, aaa.debug.get_fn_name( level+1 )
end
aaa.debug.doc.get_obj_method_name = "( level ) Gets obj and method name with aaa.debug.get_obj_method( level+1 ) and return a string"
function aaa.debug.get_obj_method_name( level )
	local obj, name = aaa.debug.get_obj_method( level+1 )
	return obj.." : "..name
end

function aaa.debug.make_traceback_table()
	local tab = {}
	local level_skip = 4
	local level = level_skip + 1
	while true do
		local t_str = aaa.debug.get_fn_info( level )
		if not t_str then break end
		for i,str in ipairs(t_str) do
			if i==1 then
				str = string.format( "%2d : ", level-level_skip )..str
			else
				str = "      "..str
			end
			table.insert( tab, str )
		end
		level = level + 1
	end
	aaa.debug.__traceback_table = tab
	return tab
end

function aaa.debug.handle_error( err_str )
	--aaa.print( "handle_error() err_str = "..err_str )
	aaa.debug.make_traceback_table()
	err_str = string.gsub( err_str, ":", "\n" )
	--aaa.print( "handle_error() processed err_str = "..err_str )
	return err_str
end

function aaa.lua.pcall_protected( fn, info, b_dialog )
	if false then	--	switch for debugging
		fn()
		return true
	end
	local b_ok, err_str = xpcall( fn, aaa.debug.handle_error )
	if b_ok then
		return true
	end

	info = "PCall failed calling "..info
	if b_dialog then
		if aaa.debug.__traceback_table then
			aaa.box_error_lua( err_str, info, nil, unpack( aaa.debug.__traceback_table ) )
		else
			aaa.box_error_lua( err_str, info, "with no __traceback_table" )
		end
	else
		aaa.print_error( info )
		aaa.print_error( err_str )
	end
	return false, err_str
end

function aaa.lua.dofile_protected( filename, b_dialog )
	return aaa.lua.pcall_protected( function() aaa.dofile(filename) end, filename, b_dialog )
end
--aaa.print_inverse(  "IS "..aaa.lua.dofile_protected )

function aaa.debug.print_traceback( str_in )
	if str_in then
		aaa.print_debug( "traceback : "..str_in )
	end
	local level = 3
	while true do
		local t_str = aaa.debug.get_fn_info( level )
		if not t_str then break end
		for i,str in ipairs(t_str) do
			if i==1 then
				str = string.format( "%2d : ", level-2 )..str
			else
				str = "      "..str
			end
			aaa.print_error( str )
		end

--		local str = aaa.debug.get_fn_info( level )
--		if not str then break end
--		aaa.print_error( (level-2).." "..str )

		level = level + 1
	end
end

function aaa.debug.print_arg( a )
	local ty = type(a)
	if 		ty == "table"	then	table.print( a, "\t", 8 )
	elseif	ty == "boolean" then	aaa.print( "\tboolean : "..a )
	elseif	ty == "string"	then	aaa.print( "\tstring : "..a )
	else 							aaa.print( "\t"..ty.." : "..a )
	end
end

function aaa.debug.print_args( ... )
	local nb = select("#", ...)
	aaa.print( "debug_arg : "..nb.." argument" )
	for i=1,nb do
		aaa.debug.print_arg( select( i, ... ) )
	end
end

function aaa.debug.__show_low( elt_type, ... )
	local bu_show = aaa.debug.__bu_show
	if bu_show then
		--add_element( elt_type, value, str, caller, alive_time )
		bu_show:add_element( elt_type, ... )
	end
end
-- ... is: value, str, caller, alive_time
function aaa.debug.show( ... )			aaa.debug.__show_low( "regular", ... )	end
function aaa.debug.show_warning( ... )	aaa.debug.__show_low( "warning", ... )	end
function aaa.debug.show_error( ... )	aaa.debug.__show_low( "error", ... )	end

function aaa.debug.get_arg_as_str( start )
	local str
	for i = start, math.huge do
		local name, value = debug.getlocal( 2, i )
		if name and name ~= "(*temporary)" then
			local arg = name.."="..value
			if	i==1	then	str = arg
			else				str = str..", ".. arg
			end
		else
			break
		end
	end
	return str
end
function aaa.debug.get_fn_arg_as_str()		return aaa.debug.get_arg_as_str( 1 )	end
function aaa.debug.get_method_arg_as_str()	return aaa.debug.get_arg_as_str( 2 )	end

function aaa.debug.__print_fn( b_inverse, str_or_bool )
	local fn_print = b_inverse and aaa.print_inverse or aaa.print
-- BEFORE
	local arg_type = type(str_or_bool)
	if arg_type == "string" then
		fn_print( "print_fn mess : "..str_or_bool )
	end
-- DO IT
	local level = 3
	--aaa.print( "OUPS : "..aaa.debug.get_fn_name(3) )
	local str = aaa.debug.get_fn_name( level+1 ).."( "
	--[[was
	local nb = select("#", ...)
	if nb == 0 then
		str = str.."()"
	else
		str = str.."( "..select( 1, ... )
		for i=2,nb do
			str = str..", "..select( i, ... )
		end
		str = str.." )"
	end
	--]]
	-- see if we can find a local in the caller's scope with the given value
	for i = 1, math.huge do
		local name, value = debug.getlocal( level, i )
		if name then
			str = (i==1	and str or str..", ")..name.."="..value
			--str = (i==1	and str or str..", ")..name.."="..aaa.__build_str(value)
		else
			str = str..(i==1 and ")" or " )")
			break
		end
	end
	fn_print( str )
-- AFTER	
	if arg_type == "boolean" and str_or_bool then
		aaa.debug.print_traceback()
	end
end
aaa.debug.doc.print_fn = "( str_or_bool ), if string it is printed before, if bool and true print_traceback called after"
function aaa.debug.print_fn(         str_or_bool )	aaa.debug.__print_fn( false, str_or_bool ) end
function aaa.debug.print_fn_inverse( str_or_bool )	aaa.debug.__print_fn( true,  str_or_bool ) end

function aaa.debug.print_method( str_or_bool )
-- BEFORE
	local arg_type = type(str_or_bool)
	if arg_type == "string" then
		aaa.print( "print_method mess : "..str_or_bool )
	end
-- DO IT
	--aaa.print( "OOUPS : "..aaa.debug.get_obj_method_name(3) )
	local str = aaa.debug.get_obj_method_name( 3 ).."( "
	  -- see if we can find a local in the caller's scope with the given value
	local name, value
	for i = 2, math.huge do
		name, value = debug.getlocal( 2, i )
		if name then
			if		i~=2					then	str = str..", "
			end
			if		type(value)=="string"	then	str = str..name.."=\""..value.."\""
			else 									str = str..name.."="..value
			end
		else
			if		i==2					then	str = str..")"
			else 									str = str.." )"
			end
			break
		end
	end
	aaa.print( str )
-- AFTER	
	if arg_type == "boolean" and str_or_bool then
		aaa.debug.print_traceback()
	end
end

function aaa.debug.print_local()
	--aaa.print( "OOUPS : "..aaa.debug.get_fn_name( 3 )
	aaa.print( aaa.debug.get_fn_name( 3 ).."()" )
	  -- see if we can find a local in the caller's scope with the given value
		for i=1,math.huge do
		local name, value = debug.getlocal( 2, i )
		if name then
			local str
			local len = string.len( name )
			if len >= 8 then 	str = "\t"..name.."\t= "
			else 				str = "\t"..name.."\t\t= "
			end
			if type(value)=="string" then 	aaa.print( str.."\""..value.."\"" )
			else 							aaa.print( str..value )
			end
		else
			break
		end
	end
end

function aaa.debug.box_debuginfo( info )
	aaa.box_warning( info.short_src,
					info.source,
					"linedefined : "..info.linedefined,
					"currentline : "..info.currentline,
					"what : "..info.what,
					"namewhat : "..info.namewhat,
					"nups : "..info.nups
					)
end

aaa.get_fn_arg_as_str		=	aaa.debug.get_fn_arg_as_str
aaa.get_method_arg_as_str	=	aaa.debug.get_method_arg_as_str
aaa.print_fn				=	aaa.debug.print_fn
aaa.print_fn_inverse		=	aaa.debug.print_fn_inverse
aaa.print_method			=	aaa.debug.print_method

aaa.show = aaa.debug.show

function aaa.__build_table_str( op )
	if CLASS.is_class( op ) then
		return op:__build_print_name()
	elseif CLASS.is_gabu_obj( op ) then
		return op:__build_print_name()
	end
	return tostring(op) --"TABLE"
end

function aaa.__build_str( op )
	local t = type(op)
	if		t == "boolean"	then 	op = op and "true" or "false"
	elseif	t == "table"	then 	op = aaa.__build_table_str( op )
	elseif	t == "nil"		then 	op = "nil"
	elseif	t == "function"	then	op = "FUNCTION-"
	elseif	t == "thread"	then	op = "THREAD-"
	elseif	t == "userdata"	then	op = "USERDATA-"
	else	--"number" , "string"
	end
	return op
end
function aaa.__get_addr( op )
	return string.gsub( tostring(op), type(op)..": ", "" )
end

--
--	CONCAT
--
function aaa.__concat( op1, op2 )
	op1 = aaa.__build_str( op1 )
	op2 = aaa.__build_str( op2 )
	return op1..op2
end

local mt = getmetatable("")
mt.__concat	=	aaa.__concat
--mt.__tostring	=	__aaa_build_name
--test
--aaa.print_inverse( "hello".." ".."world "..true )

aaa.show_file_end( "aaa_debug" )

