--MOUSE
aaa.show_file_begin( "aaa_mouse" )

if not aaa.mouse.ref then
	aaa.mouse.ref={}
end
function aaa.mouse.init_ref()
	local ref = aaa.mouse.ref
		ref.but_left	= param.get_ref( aaa.ref.pref, "mouse_button_left"	)
		ref.but_middle	= param.get_ref( aaa.ref.pref, "mouse_button_middle"	)
		ref.but_right	= param.get_ref( aaa.ref.pref, "mouse_button_right"	)
		ref.wheel_up	= param.get_ref( aaa.ref.pref, "mouse_wheel_up"		)
		ref.wheel_down	= param.get_ref( aaa.ref.pref, "mouse_wheel_down"	)
end

function aaa.mouse.is_but_left()	return param.get_bool( aaa.mouse.ref.but_left	)	end
function aaa.mouse.is_but_middle()	return param.get_bool( aaa.mouse.ref.but_middle	)	end
function aaa.mouse.is_but_right()	return param.get_bool( aaa.mouse.ref.but_right	)	end
function aaa.mouse.is_wheel_up()	return param.get_bool( aaa.mouse.ref.wheel_up	)	end
function aaa.mouse.is_wheel_down()	return param.get_bool( aaa.mouse.ref.wheel_down	)	end

function aaa.mouse.update()
end


aaa.show_file_end( "aaa_mouse" )
