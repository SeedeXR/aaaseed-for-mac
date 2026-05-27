
--FORMAT
aaa.show_file_begin( "aaa_format" )

if not aaa.format then	aaa.format = { doc={} }	end
function aaa.format.int_to_char2( val )
	local d = val % 10
	return tostring((val -d)/10)..tostring(d)
end
function aaa.format.int_to_char3( val )
	return string.format( "%3d", val )
end
aaa.format.doc.int_to_char_with_space = "( nb ) return str from int with spaces between thousand million ..."
function aaa.format.int_to_char_with_space( nb )
	nb = nb or 0
	local str
	if nb < 0 then
		str="-"
		nb = -nb
	else
		str = ""
	end

	local o0 = math.floor( nb % 1000 )
	local o1 = math.floor( nb / 1000 )
	if o1>1000 then		return str..aaa.format.int_to_char_with_space( o1 )..string.format( " %0.3d", o0 )
	elseif o1>0 then	return str..string.format( "%d", o1 )..string.format( " %0.3d", o0 )
	else				return str..string.format( "%d", o0 )
	end
end

function aaa.format.real_to_char2( val )
	val =  math.floor( val )
	local d = val % 10
	return tostring((val -d)/10)..tostring(d)
end
function aaa.format.real_dot2( val )
	if		val == 0	then	return "0"
	elseif	val == 1	then	return "1"
	end
	return string.format( "%.2f", val )
end




aaa.show_file_end( "aaa_format" )
