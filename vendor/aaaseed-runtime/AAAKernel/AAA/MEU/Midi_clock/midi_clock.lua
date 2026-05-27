if not midi_clock then
	midi_clock = {}
	midi_clock.ref = {}
	local ref = midi_clock.ref
	--todo
	ref.midipref = aaa.obj.get_no_error( aaa.dir.get_dir_tracker().."/default_b.midipref" )
	if ref.midipref then
		ref.clock = param.get_ref( ref.midipref, "date" )
	end
	midi_clock.cl_last =  0
end

if midi_clock.ref.clock then
	if not midi_clock.note_last then
		midi_clock.note_last = { 0, 0, 0, 0 }
		midi_clock.beat_trig = { false, false, false, false }
		midi_clock.beat_mess = { "B", "\tB", "\t\tB", "\t\t\tB" }
	end

	for i=1,4 do
		local value = aaa.midi.get_velocity( 1, 60+i )
		if midi_clock.note_last[i] ~= value then
			midi_clock.note_last[i] = value
			midi_clock.beat_trig[i] = true
			aaa.midi.set_control( 8, i, 1 )
		else
			midi_clock.beat_trig[i] = false
			if aaa.midi.get_control( 8, i ) > 0. then
				aaa.midi.set_control( 8, i, 0 )
			end
		end
	--	if midi_clock.beat_trig[i] then
	--		aaa.print( midi_clock.beat_mess[i] )
	--	end
	end


	--[[
	local cl = param.get( ref.clock )
	cl = math.floor( cl / (1152) )
	if midi_clock.cl_last ~= cl then
		aaa.print( cl )
		midi_clock.cl_last = cl
	end
	--]]
end

	-- local value
	-- for i = 13,16 do
	-- 	value = aaa.midi.get_velocity( 2, i )
	-- 	self:show( value, "v"..i )
	-- end