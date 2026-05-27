
aaa.show_file_begin( "csv_util" )

function table:from_csv( str, delimiter )
	str = str .. delimiter  -- ending comma
	local t = {}        -- table to collect fields
	local fieldstart = 1
	repeat
		-- next field is quoted? (start with `"'?)
		if string.find( str, '^"', fieldstart ) then
			local a, c
			local i = fieldstart
			repeat
				-- find closing quote
				a, i, c = string.find( str, '"("?)', i + 1 )
			until c ~= '"'    -- quote not followed by quote?
			if not i then error( 'unmatched "' ) end
				local f = string.sub( str, fieldstart + 1, i - 1 )
				table.insert( t, ( string.gsub( f, '""', '"' ) ) )
				fieldstart = string.find( str, delimiter, i ) + 1
		else                -- unquoted; find next comma
			local nexti = string.find( str, delimiter, fieldstart )
			table.insert( t, string.sub( str, fieldstart, nexti - 1 ) )
			fieldstart = nexti + 1
		end
	until fieldstart > string.len( str )
	return t
end

aaa.show_file_end( "csv_util" )
