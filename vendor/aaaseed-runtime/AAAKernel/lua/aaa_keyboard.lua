--KEYBOARD
aaa.show_file_begin( "aaa_keyboard" )

if not aaa.keyboard then
	aaa.keyboard = { ref={} }
	aaa.keyboard.ascii_last = 0
end
function aaa.keyboard.init_ref()
	local ref = aaa.keyboard.ref
		ref.ctrl	= param.get_ref( aaa.ref.pref, "keyboard_ctrl"			)
		ref.shift	= param.get_ref( aaa.ref.pref, "keyboard_shift"			)
		ref.alt		= param.get_ref( aaa.ref.pref, "keyboard_alt"			)
		ref.ascii	= param.get_ref( aaa.ref.pref, "keyboard_ascii_out"		)
		ref.special	= param.get_ref( aaa.ref.pref, "keyboard_special_out"	)
end

function aaa.keyboard.is_ctrl()				return aaa.keyboard.b_ctrl	end
function aaa.keyboard.is_shift()			return aaa.keyboard.b_shift	end
function aaa.keyboard.is_alt()				return aaa.keyboard.b_alt	end

function aaa.keyboard.is_ctrl_only()		return	   aaa.keyboard.is_ctrl() and not aaa.keyboard.is_shift() and not aaa.keyboard.is_alt() end
function aaa.keyboard.is_shift_only()		return not aaa.keyboard.is_ctrl() and     aaa.keyboard.is_shift() and not aaa.keyboard.is_alt() end
function aaa.keyboard.is_alt_only()			return not aaa.keyboard.is_ctrl() and not aaa.keyboard.is_shift() and     aaa.keyboard.is_alt() end
function aaa.keyboard.is_no_modifier()		return not aaa.keyboard.is_ctrl() and not aaa.keyboard.is_shift() and not aaa.keyboard.is_alt() end

function aaa.keyboard.consume_ascii()		aaa.keyboard.b_read_ascii = false									end
function aaa.keyboard.get_ascii()			return aaa.keyboard.ascii											end
function aaa.keyboard.get_ascii_down()		return aaa.keyboard.b_read_ascii and aaa.keyboard.ascii_down or 0	end

function aaa.keyboard.consume_special()		aaa.keyboard.b_read_special = false									end
function aaa.keyboard.get_special()			return aaa.keyboard.special											end
function aaa.keyboard.get_special_down()	return aaa.keyboard.b_read_special and aaa.keyboard.special or 0	end

function aaa.keyboard.is_key_fn( key, nb )
	local b = key == nb or key == (280+nb)	--on old and new design
	if b then
		aaa.print( "F"..nb.." key tested" )
	end
	return b
end

function aaa.keyboard.update()
	aaa.keyboard.ascii = param.get( aaa.keyboard.ref.ascii )
	if aaa.keyboard.ascii ~= aaa.keyboard.ascii_last then
		aaa.keyboard.ascii_down = aaa.keyboard.ascii
		aaa.keyboard.ascii_last = aaa.keyboard.ascii
	else
		aaa.keyboard.ascii_down = 0
	end
	aaa.keyboard.b_read_ascii = true

	aaa.keyboard.special = param.get( aaa.keyboard.ref.special )
	aaa.keyboard.b_read_special  = true

	aaa.keyboard.b_ctrl  = param.get_bool( aaa.keyboard.ref.ctrl )
	aaa.keyboard.b_shift = param.get_bool( aaa.keyboard.ref.shift )
	aaa.keyboard.b_alt   = param.get_bool( aaa.keyboard.ref.alt )
end

aaa.show_file_end( "aaa_keyboard" )
