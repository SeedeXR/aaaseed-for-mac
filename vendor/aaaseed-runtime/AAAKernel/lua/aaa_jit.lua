
aaa.show_file_begin( "aaa_jit" )

if not aaa.jit then
	aaa.jit = { __state = -42 }
	--aaa.print_error(  "aaa.debug now define by AAASeed.\n running a pre Jan 28th 2015 AAASeed version" )
end

function aaa.jit.on()
	if jit then
		if aaa.jit.__state ~= true then
			jit.on()
			aaa.print_inverse( "Enabling LuaJit" )
			aaa.jit.__state = true
		end
	else
		aaa.print_error( "no LuaJit : can't switch on" )
	end
end
function aaa.jit.off()
	if jit then
		if aaa.jit.__state ~= false then
			jit.off()
			aaa.print_inverse( "Disabling LuaJit" )
			aaa.jit.__state = false
		end
	else
		aaa.print_error( "no LuaJit : can't switch off" )
	end
end
function aaa.jit.set( b_on )
	if b_on then		aaa.jit.on()
	else				aaa.jit.off()
	end
end


aaa.show_file_end( "aaa_jit" )
