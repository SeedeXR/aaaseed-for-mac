--
-- strict.lua
-- checks uses of undeclared global variables
-- All global variables must be 'declared' through a regular assignment
-- (even assigning nil will do) in a main chunk before being used
-- anywhere or assigned to inside a function.
--
-- get global metatable
aaa.print_inverse( "strict.lua" )

local global = { doc={} }
aaa.lua.global = global
local doc = global.doc

local mt = getmetatable(_G)
if mt == nil then
	mt = {}
	mt.__declared = {}
	setmetatable( _G, mt )
end
global._mt = mt


--todo pass arg to init
doc.declare = "pass a str for the name of the global you want to use"
function global.declare( key )
	--aaa.box_error( "aaa.lua.global.declare( "..key.." ) type is "..type(key) )
	local key_type = type(key)
	if key_type=="string" then
		local declared = global._mt.__declared
		if not declared[key] then
			declared[key] = true
			return true
		end
	else
		aaa.debug.print_traceback()
		aaa.box_error( "aaa.lua.global.declare expect a string as key, not "..key.." of type "..key_type )
	end
	return false
end
--todo pass arg to init
doc.declare_table = "same than declare but create the global as an empty table"
function global.declare_table( key )
	if global.get( key ) then return false end
	--aaa.box_error( "Global "..key )
	return global.set( key, {} )
end
doc.undeclare = "erase a global and and forbid its use until declared ago"
function global.undeclare( key )
	--aaa.box_error( "aaa.lua.global.undeclare( "..key.." ) type is "..type(key) )
	if not key then
		aaa.debug.print_traceback()
		aaa.box_error( "aaa.lua.global.undeclare expect a string as key not a nil " )
	elseif type(key)=="string" then
		local declared = global._mt.__declared
		if declared[key] then
			_G[key] = nil
			declared[key] = false
			return true
		end
	else
		aaa.debug.print_traceback()
		aaa.box_error( "aaa.lua.global.undeclare expect a string as key not of type "..type(key) )
	end
	return false
end


function global.set( key, value )
	if global._mt.__declared[key] then
		rawset( _G, key, value )
		return true
	else
		if global.declare( key ) then
			rawset( _G, key, value )
			return true
		else
			return false
		end
	end
end

--[[
function global.is( key )
	local mt = global._mt
	if mt then
  		return mt.__declared[key]
  	end
end
--]]
--todo not sure we don't ignore some metamethod here
function global.get( key )
	return rawget( _G, key )
end
function global.call_fn( key, ... )
	local fn = global.get( key )
	if fn then
		fn(...)
	end
end

function global.set_meta_index_methods( obj, method_index, method_newindex )
	if obj then	global.__meta = { obj=obj, index=method_index, newindex=method_newindex }
	else		global.__meta = nil
	end
end

function global.set_strict()
	aaa.print_inverse( "FLIPPING TO STRICT" )
	local mt = global._mt

	local function meta_index( t, key )
		if not mt.__declared[key] then
			--try with meta_method if it exist
			local gm = global.__meta
			if gm and gm.index then
				local fn = gm.obj[gm.index]
				if fn then
					return fn( gm.obj, key )
				end
			end
			--not found and by meta method so error
			if debug.getinfo( 2, "S" ).what ~= "C" then
				error( "variable '"..key.."' is not declared as global", 2 )
			end
		end
		return rawget( t, key )
	end
	mt.__index = meta_index

	local function meta_newindex( t, key, value )
		if not mt.__declared[key] then
			--try with meta_method if it exist
			local gm = global.__meta
			if gm and gm.newindex then
				local fn = gm.obj[gm.newindex]
				if fn then
					if fn( gm.obj, key, value ) then
						return
					end
				end
			end
			--aaa.print( debug.traceback() )
			--aaa.debug.print_traceback( "Strict new index called" )
			local dbg = debug.getinfo( 2, "Snl" )
			--aaa.debug.box_debuginfo( dbg )
			local w = dbg.what
			if w ~= "C" and dbg.source ~= "net" then	--avoid message cause by net packet--w ~= "main" and
				error( "assign to undeclared variable as global '"..key.."'", 2 )
			end
			--print ("declare "..n.." = "..tostring (v).." at "..tostring (dbg.source)..":"..tostring (dbg.currentline))
			mt.__declared[key] = true
		end
		rawset( t, key, value )
	end
	mt.__newindex = meta_newindex
end

-- get persitent table by lua scripy (by caller infcat: lua_wrap most of the time)
-- return table and boolean saying if table needed to be created (first call with this caller)
__aaa_private_table_by_caller = {}

function aaa.lua.get_table_by_caller()
	local tables = __aaa_private_table_by_caller
	local name = "unique_table_"..aaa.get_caller()
	local tab = tables[name]
	if not tab then
		tab = {}
		tables[name] = tab
		return tab, true 
	end
	return tab 
end

--[[
%function setglobal
%desc Set a Lua global
%long By default, plugins cannot override the main Lua state. Globals are actually locals to the script,
so a plugin cannot break the main Guerilla state. To forcefully set a global in the main state, use this
function. Warning, this function is unprotected and may cause Guerilla to crash if a necessary global is
overwritten.
%arg key object The key to associate in the globals
%arg value object The value to associate in the globals
]]

-- file is a deprecated alias for fs
-- file is a common variable name and can be misleading
-- fs is more self explanatory and less common as a variable name
--file = fs
