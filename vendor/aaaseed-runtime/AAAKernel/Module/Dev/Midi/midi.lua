--midi = nil
if midi == nil then
	midi = {}
	midi.obj		= aaa.obj.get_by_name( aaa.dir.get_dir_tracker().."/default_a.midipref" )

	midi.trig		= param.get_ref( aaa.ref.app, "Dummy_BOOL_1"	)
	midi.ch			= param.get_ref( aaa.ref.app, "Dummy_INT32_1"	)
	midi.note		= param.get_ref( aaa.ref.app, "Dummy_INT32_2"	)
	midi.vel		= param.get_ref( aaa.ref.app, "Dummy_INT32_3"	)

	midi.pgm_trig	= param.get_ref( aaa.ref.app, "Dummy_BOOL_4"	)
	midi.pgm		= param.get_ref( aaa.ref.app, "Dummy_INT32_4"	)

	aaa.print( "midi INIT done" )
end

if param.get_bool( midi.trig ) then
	aaa.midi_send_note_on( midi.obj, param.get(midi.ch), param.get(midi.note), param.get(midi.vel) )
	param.set( midi.trig, 0 )
end
if param.get_bool( midi.pgm_trig ) then
	aaa.midi_send_program_change( midi.obj, param.get(midi.ch), param.get(midi.pgm) )
	param.set( midi.pgm_trig, 0 )
end

