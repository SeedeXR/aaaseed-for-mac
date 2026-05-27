--UTIL
aaa.show_file_begin( "aaa_string" )

function string.is_trailing_slash( str )
	return string.sub( str, -1 ) == '/'
end
function string.is_trailing_slash_dialog( str )
	local b = string.is_trailing_slash( str )
	if not b then
		aaa.debug.print_traceback()
		aaa.box_error( "No trailing slash for\n"..str )
	end
	return b
end
function string.remove_trailing_slash( str )
	return string.sub( str, -1 ) == '/' and string.sub( str, 1,-2 ) or str
end
function string.enforce_trailing_slash( str )
	if str and str ~= "" then
		if string.sub( str, -1 ) ~= "/" then
			str = str.."/"
			aaa.print( "add trailing slash : "..str )
			return str
		end
	end
	return str
end

function string.wrap( str, char_max )
	--aaa.print_fn()
    local result = ""
    local current_line = ""
    local words = {}
    
    -- D'abord, on sépare la chaîne en mots
    for word in str:gmatch("%S+") do
        table.insert( words, word )
    end
    
	local nb = 1
    -- On reconstruit les lignes sans dépasser char_max
    for i, word in ipairs(words) do
        if #current_line + #word <= char_max then
            if current_line ~= "" then
                current_line = current_line .. " " .. word
            else
                current_line = word
            end
        else
            result = result .. current_line .. "\n"
			nb = nb + 1
            current_line = word
        end
    end
    
	--table.print( result, "result" )
    -- Ajouter la dernière ligne
    if current_line ~= "" then
        result = result .. current_line
    end
    
    return result, nb
end

--[[ not in global
function split( str, delim, max_nb )
	-- Eliminate bad cases...
	if string.find( str, delim ) == nil then
		return { str }
	end
	if max_nb == nil or max_nb < 1 then
		max_nb = 0    -- No limit
	end
	local	result = {}
	local	pat = "(.-)" .. delim .. "()"
	local	nb = 0
	local	last_pos
	for part, pos in string.gmatch( str, pat ) do
		nb = nb + 1
		result[ nb ] = part
		last_pos = pos
		if nb == max_nb then break end
	end
	-- Handle the last field
	if nb ~= max_nb then
		result[ nb + 1 ] = string.sub( str, last_pos )
	end
	return result
end
--]]

function string:make_from_list( ... )
	local nb = select("#", ...)
	--aaa.print( "nb "..nb )
	if nb == 0 then
		return ""
	else
		local str = tostring( select( 1, ... ) )
		for i=2,nb do
			str = str..", "..select( i, ... )
		end
		return str
	end
end

function string:split( delimiter )
	local	result = {}
	local	from = 1
	local	delim_from, delim_to = string.find( self, delimiter, from  )
	while delim_from do
		table.insert( result, string.sub( self, from , delim_from - 1 ) )
		from  = delim_to + 1
		delim_from, delim_to = string.find( self, delimiter, from  )
	end
	table.insert( result, string.sub( self, from  ) )
	return result
end

aaa.show_file_end( "aaa_string" )