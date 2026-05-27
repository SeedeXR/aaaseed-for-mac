--TABLE
aaa.show_file_begin( "aaa_table" )

local function empty_fn() end
function IPAIRS(tab) 	if tab then return ipairs(tab) end	return empty_fn end
function PAIRS(tab) 	if tab then return pairs(tab)  end	return empty_fn end

function pairs_sorted( t, order )
	-- collect the keys
	local keys = {}

	local id = 0
	for k in pairs(t) do
		id = id + 1
		keys[id] = k
	end

	-- if order function given, sort by it by passing the table and keys a, b,
	-- otherwise just sort the keys
	local fn
	if order then
		fn = function(a,b) return order( t, a,b ) end
	else
		fn = function(a,b)
			local ta, tb = type(a), type(b)
			if ta ~= tb then
				return ta < tb
			end
			if ta == "table" then
				a = a[CLASS.__str_class_field] and aaa.__build_table_str(a) or tostring(a)
				b = b[CLASS.__str_class_field] and aaa.__build_table_str(b) or tostring(b)	
			end
			return a < b
		end
	end
	table.sort( keys, fn )

	-- return the iterator function
	local i = 0
	return function()
				i = i + 1
				if keys[i] then
					return keys[i], t[keys[i]]
				end
			end
end


--
-- COPIES
--
function table.copy_simple( src )
	if type(src) ~= "table" then
		return src
	end
	local dst = {}
	for k,v in pairs(src) do
		dst[k] = v
	end
	return dst
end

--todo add the check
function table.copy_simple_check( src, signature )
	if type(src) ~= "table" then return src end
	local dst = {}
	for k,v in pairs(src) do
		if type(v) == "table" then
			aaa.print_error( "table.copy_simple_check() signed "..signature.." key "..k.." is a table:" )
			table.print( src, "src" ) 
		end
		dst[k] = v
	end
	return dst
end

function table.copy_shallow( src )
	if type(src) ~= "table" then return src end
	local dst = {}
	for k,v in pairs(src) do dst[k] = v end
	return setmetatable( dst, getmetatable(src) )
end

function table.copy_deep( src )
	local lookup = {}
	local function _copy( src )
		if type(src) ~= "table"	then
			return src
		elseif lookup[src] 	then
			return lookup[src]
		end
		local dst = {}
		lookup[src] = dst
		for k, v in pairs(src) do
			dst[_copy(k)] = _copy( v )
		end
		return setmetatable( dst, getmetatable( src ) )
	end  -- function _copy
	return _copy( src )
end

function table.get_field_table_always( dst, key, sub_key )
	local t = dst[key]
	if not t then
		t = {}
		dst[key] = t
	end
	if not sub_key then
		return t
	end

	local sub_t = t[sub_key]
	if not sub_t then
		sub_t = {}
		t[sub_key] = sub_t
	end
	return sub_t
end

function table.__get_array_and_len( t )
	local i = 0
	if t == _G then
		return false
	end
	for _,elt in pairs(t) do
		i = i + 1
		if t[i] == nil then
			return false
		end
	end
	return true, i
end

function table.build_table_header( t )
	local str
	if t~=_G and t[CLASS.__str_class_field] then
		--str = "Table GABU_OBJ"
		str = aaa.__build_table_str( t )
	else
		local b, len = table.__get_array_and_len( t )	
		if b then
			if len == 0 then
				str = "Empty Table {}"
			else
				str = "Array of len "..len
			end
		else
			str = ""
		end
	end
	return str .. "        "..tostring( t )
end

function aaa.__build_str_for_doc( op, b_short, sep )
	sep = sep or "\n  "
	local typ = type(op)
	if typ == "table"		then
		local gabu = rawget( op, CLASS.__str_class_field )
		if gabu~=nil then
			return "CLASS "..gabu.class_name
		end

		return b_short and aaa.__build_table_str(op) or table.build_table_header(op)
		-- if op == _G then
		-- 	aaa.lua.global.get( CLASS.__str_class_field )	-- because in strict.lua we protect access to global table
		-- else
		-- 	gabu = op[CLASS.__str_class_field]
		-- end
		-- if gabu then
		-- 	return "gabu_obj instance "..op:__build_print_name()
		-- end
		-- local nb = table.count_item(op)
		-- return (table.is_empty(op) and "empty table" or ("Table size "..nb)).." @ "..aaa.__get_addr(op)
	elseif typ == "function"	then
		return "Function @ "..aaa.__get_addr(op)
	elseif typ == "string"	then
		return "String \""..tostring(op).."\""
	elseif typ == "number"	then
		--aaa.print( "op is ".. op )
		if aaa.obj.is_ref_no_error( op ) then
			return "c_obj_ui ref        "..tostring(op)..sep..aaa.obj.get_class(op)..sep..aaa.obj.get_name(op)
		elseif param.is_ref_no_error( op ) then
			local obj = param.get_obj(op)
		 	return "c_param ref "..param.get_name(op).."       "..tostring(op)..sep..aaa.obj.get_class(obj)..sep..aaa.obj.get_name(obj)
		end
	end
	return typ.." : "..tostring(op)
end

-- Print anything - including nested tables
function table.print( tt, str, level, indent )
	if not str then
		aaa.debug.print_traceback()
		aaa.print( "table.print() is recomended to have a str argument" )
	end
	local function print_low( tt, level, indent, done )
		if type( tt ) == "table" then
			for key, value in pairs_sorted( tt ) do
				local pre = string.rep( " ", indent ) -- indent it
				if type( value ) == "table" then
					--local str_value = (value~=_G and value[CLASS.__str_class_field]) and aaa.__build_table_str(value) or tostring(value)
					local str_value = table.build_table_header(value)
					local d = done[value]
					if d then
						local dlevel = level - d.level
						if dlevel == 0 then
							if tt[d.key] then
								aaa.print( pre .. string.format( "[%s] -> %s see [%s]", tostring(key), str_value, d.key ) )
							else
								aaa.print( pre .. string.format( "[%s] -> %s see [%s] in other table at same relative level", tostring(key), str_value, d.key ) )
							end
						else
							aaa.print( pre .. string.format( "[%s] -> %s see [%s] at relative level %d", tostring(key), str_value, d.key, dlevel ) )
						end
					else
						done[value] = { key=key, level=level }
						if level > 1 then
							aaa.print( pre .. string.format( "[%s] -> %s", tostring(key), str_value ) );
							if table.count_item(value) > 0 then 
								aaa.print( pre .. "{" );
									print_low( value, level-1, indent+4, done )
								aaa.print( pre .. "}" );
							end
						else
							aaa.print( pre .. string.format( "[%s] -> %s...", tostring(key), str_value ) );
						end
					end
				elseif type( value ) == "string" then
					aaa.print( pre .. string.format( "[%s] -> "..'"'.."%s"..'"', tostring( key ), value ) )
				else
					local str = aaa.__build_str_for_doc( value, nil, " " )
					aaa.print( pre .. string.format( "[%s] -> %s", tostring( key ), str ) )
				end
			end
		else
			aaa.print( tt )
		end
	end

	if tt then
		local level = level or 1
		local indent = indent or 0
		local done = {}
		aaa.print( string.format( "%s: %s", str or tostring(tt), table.build_table_header(tt) ) )
		if table.count_item(tt) > 0 then 
			aaa.print( "{" );
			print_low( tt, level, indent+4, done )
			aaa.print( "}" )
		end
	else
		if str then
			aaa.print_error( "table.print() \""..str.."\" table is nil" )
		else
			aaa.print_error( "table.print() table and string are both nil" )
		end
	end
end

function table.apply_fn( tab, fn, ... )
	if not tab then return end
	for _, elt in pairs( tab ) do
		fn( elt, ... )
	end
end
function table.apply_method( tab, method_name, ... )
	if not tab then return end
	for _, elt in pairs( tab ) do
		elt[method_name]( elt, ... )
	end
end


-- without Compare fn this have no sense
-- function table.is_deep_equal( tbl1, tbl2 )
-- 	for k, v in pairs( tbl1 ) do
-- 		if type(v) == "table" and type(tbl2[k]) == "table" then
-- 			if not table.Compare( v, tbl2[k] ) then return false end
-- 		else
-- 			if v ~= tbl2[k] then return false end
-- 		end
-- 	end
-- 	for k, v in pairs( tbl2 ) do
-- 		if type(v) == "table" and type(tbl1[k]) == "table" then
-- 			if not table.Compare( v, tbl1[k] ) then return false end
-- 		else
-- 			if v ~= tbl1[k] then return false end
-- 		end
-- 	end
-- 	return true
-- end

-- Slice a table, making a subtable with items at rows 'first' to 'last'
function table.slice( tbl, first, last, step )
	local sliced = {}
	for i = first or 1, last or #tbl, step or 1 do
		table.insert( sliced, tbl[i] )
	end
	return sliced
end


local function __serialize_table( tab, val, name, skipnewlines, depth )
	skipnewlines = skipnewlines or false
	depth = depth or 0

	-- indentation
    tab[#tab+1] = string.rep( " ", depth )
	if name then
		if not string.match( name, '^[a-zA-Z_][a-zA-Z0-9_]*$' ) then
			tab[#tab+1] = "['"
			tab[#tab+1] = string.gsub( name, "'", "\\'" )
			tab[#tab+1] = "']"
		end
		tab[#tab+1] = " = "
	end

	local typ = type(val)
	if typ == "table" then
		tab[#tab+1] = "{"
		if not skipnewlines then
			tab[#tab+1] = "\n"
		end

		local b_comma = false
		for k, v in pairs(val) do
			if b_comma then
				tab[#tab+1] = ","
				if not skipnewlines then
					tab[#tab+1] = "\n"
				end
			else
				b_comma = true
			end
			__serialize_table( tab, v, k, skipnewlines, depth + 1 )
		end

		tab[#tab+1] = string.rep( " ", depth )
		tab[#tab+1] = "}"
	elseif typ == "number" then
		tab[#tab+1] = tostring( val )
	elseif typ == "string" then
		tab[#tab+1] = string.format( "%q", val )
	elseif typ == "boolean" then
		tab[#tab+1] = val and "true" or "false"
	else
		tab[#tab+1] = "\"[inserializeable datatype:"
		tab[#tab+1] = typ
		tab[#tab+1] = "]\""
	end
	return tab
end


function table.to_string( t )
	local tab = { "return " }
	__serialize_table( tab, t )
	return table.concat(tab)
end
function table.from_string( str )
	local f, err = loadstring( str )
	if f then
		return f()
	else
		aaa.print_error( "table.from_string() loadstring failed with error: "..err )
		return {}
	end
end


--[[
Author: Julio Manuel Fernandez-Diaz
Date:   January 12, 2007
(For Lua 5.1)

Modified slightly by RiciLake to avoid the unnecessary table traversal in tablecount()

Formats tables with cycles recursively to any depth.
The output is returned as a string.
References to other tables are shown as values.
Self references are indicated.

The string returned is "Lua code", which can be procesed
(in the case in which indent is composed by spaces or "--").
Userdata and function keys and values are shown as strings,
which logically are exactly not equivalent to the original code.

This routine can serve for pretty formating tables with
proper indentations, apart from printing them:

print(table.show(t, "t"))   -- a typical use

Heavily based on "Saving tables with cycles", PIL2, p. 113.

Arguments:
t is the table.
name is the name of the table (optional)
indent is a first indentation (optional).
--]]
function table.show( t, name, indent )
	local cart     -- a container
	local autoref  -- for self references

--[[ counts the number of elements in a table
	local function tablecount(t)
	local n = 0
	for _, _ in pairs(t) do n = n+1 end
	return n
	end
]]
-- (RiciLake) returns true if the table is empty
	local function isemptytable(t) return next(t) == nil end

	local function basicSerialize (o)
		local so = tostring(o)
		if type(o) == "function" then
			local info = debug.getinfo(o, "S")
			-- info.name is nil because o is not a calling level
			if info.what == "C" then
				return string.format("%q", so .. ", C function")
			else
				-- the information is defined through lines
				return string.format("%q", so .. ", defined in (" ..
						info.linedefined .. "-" .. info.lastlinedefined ..
							")" .. info.source)
			end
		elseif type(o) == "number" then
			return so
		else
			return string.format("%q", so)
		end
	end

	local function addtocart (value, name, indent, saved, field)
		indent = indent or ""
		saved = saved or {}
		field = field or name

		cart = cart .. indent .. field

		if type(value) ~= "table" then
			cart = cart .. " = " .. basicSerialize(value) .. ";\n"
		else
			if saved[value] then
				cart = cart .. " = {}; -- " .. saved[value]
						.. " (self reference)\n"
				autoref = autoref ..  name .. " = " .. saved[value] .. ";\n"
			else
				saved[value] = name
				--if tablecount(value) == 0 then
				if isemptytable(value) then
					cart = cart .. " = {};\n"
				else
					cart = cart .. " = {\n"
					for k, v in pairs(value) do
						k = basicSerialize(k)
						local fname = string.format("%s[%s]", name, k)
						field = string.format("[%s]", k)
						-- three spaces between levels
						addtocart(v, fname, indent .. "   ", saved, field)
					end
					cart = cart .. indent .. "};\n"
				end
			end
		end
	end

	name = name or "__unnamed__"
	if type(t) ~= "table" then
		return name .. " = " .. basicSerialize(t)
	end
	cart, autoref = "", ""
	addtocart(t, name, indent)
	return cart .. autoref
end

-- ARRAY
--
array = {}

function array.set( t, val, nb )
	if t then
		for i = 1,nb or #t do
			t[i] = val
		end
	end
	return t
end
function array.new( nb, val )
	return array.set( {}, val, nb )
end
function array.find_index_by_val( t, val )
	if not t then return end
	for i = 1, #t do
		if t[i] == val then return i end
	end
end
function array.remove_by_val( t, val )
	local i = array.find_index_by_val( t, val )
	if i then
		return table.remove( t, i )
	end
end
function array.remove_by_vals_table( t, vals_table )
	for k, v in PAIRS( vals_table ) do
		array.remove_by_val( t, v )
	end
end

function array.find_index_by_id( t, nb, id )
--	if not t then return end
	for i = 1,nb do
		if t[i].id == id then
			return i
		end
	end
end
function array.find_elt_by_id( t,nb, id )
--	if not t then return end
	for i = 1, nb do
		local elt = t[i]
		if elt.id == id then
			return elt
		end
	end
end
function array.have_val( t, nb, val )
	if not t then return end
	for i=1,nb do
		local elt = t[i]
		if elt == val then
			return elt
		end
	end
end

function array.find_elt_by_key_str_lowercase( t, nb, key, str )
	for i=1,nb do
		local elt = t[i]
		local str_key = elt[key]
		if str_key and string.lower( str_key ) == str then
			return elt
		end
	end
end

function array.find_elt_by_field_val( t, nb, key, val )
	if not t then return end
	for i=1,nb do
		local elt = t[i]
		if elt[key] == val then
			return elt
		end
	end
end

function array.extract_subfield_as_array( t, key )
	if not t then return end
	local a = {}
	for i, v in ipairs(t) do
		a[i] = v[key]
	end
	return a
end

function table.find_key_by_val( t, val )
	if not t then return end
	for k, v in pairs(t) do
		if v == val then
			return k
		end
	end
end

function table.reverse( t )
	if not t then return end
	local inv = {}
	for k, v in pairs(t) do
		inv[v] = k
	end
	return inv
end

function table.count_item( t )
	local nb = 0
	for _ in pairs(t) do
		nb = nb + 1
	end
	return nb
end
function table.is_empty(t)
    return next(t) == nil
end

function table.build_array_with_unique_value( t )
	local dst = {}
	local hash = {}
	for _,v in pairs(t) do
		if (not hash[v]) then
			dst[#dst+1] = v
			hash[v] = true
		end
	end
	return dst
end


-- APPLY
--
function array.apply_fn( tab, fn, ... )
	if not tab then return end
	for i = 1,#tab do
		fn( tab[i], ... )
	end
end
function array.apply_begin_end_fn( tab, i_begin, i_end, fn, ... )
	for i = i_begin or 1, i_end or #tab do
		fn( tab[i], ... )
	end
end
function array.apply_method( tab, method_name, ... )
	if not tab then return end
	for i = 1,#tab do
		local elt = tab[i]
		elt[method_name]( elt, ... )
	end
end
function array.apply_method_index( tab, method_name, ... )
	if not tab then return end
	for i = 1,#tab do
		local elt = tab[i]
		elt[method_name]( elt, i, ... )
	end
end

function array.copy_simple( tab )
	if not tab then return end
	local t = {}
	for i = 1,#tab do
		t[i] = tab[i]
	end
	return t
end
function array.append_to( dst, tab )
	if not tab then return end
	local nb = #dst
	for i = 1,#tab do
		dst[nb+i] = tab[i]
	end
end
function array.interpolate( td, t0, t1, inter )
	if not t0 or not t1 then return end
	local nb0 = #t0
	local nb1 = #t1
	nb0 = math.min( nb0, nb1 )
	for i = 1,nb0 do
		td[i] = interpolate( t0[i], t1[i], inter )
	end
end

function array.get_max( t, nb_or_first, last )
	if last==nil then
		last = nb_or_first or #t
		nb_or_first = 1
	end
	local max = t[nb_or_first]	 
	for i = nb_or_first+1,last do
		local v = t[i]
		if v > max then max = v end
	end
	return max
end
function array.get_min( t, nb_or_first, last )
	if last==nil then
		last = nb_or_first or #t
		nb_or_first = 1
	end
	local min = t[nb_or_first]	 
	for i = nb_or_first+1,last do
		local v = t[i]
		if v < min then min = v end
	end
	return min
end

aaa.show_file_end( "aaa_table" )