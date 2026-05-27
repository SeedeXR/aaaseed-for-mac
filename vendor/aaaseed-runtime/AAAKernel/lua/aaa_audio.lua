--AUDIO
aaa.show_file_begin( "aaa_audio" )

if not aaa.audio then
	aaa.audio = { ref={}, doc={} }
end

aaa.audio.doc.init_ref = "() Inits aaa.audio.ref with references to obj master and parameters 'volume', 'pan'."
function aaa.audio.init_ref()
	local ref = aaa.audio.ref
	ref.master	= aaa.obj.get_from_top_by_class( "snd_master" )
	ref.volume		= param.get_ref( ref.master,	"volume"	)
	ref.pan			= param.get_ref( ref.master,	"pan"		)
end
aaa.audio.doc.set_volume = "( vol ) Setter for aaa.audio.ref.volume"
function aaa.audio.set_volume( vol )	param.set( aaa.audio.ref.volume, vol )	end
aaa.audio.doc.set_pan = "( pan ) Setter for aaa.audio.ref.pan"
function aaa.audio.set_pan( pan )		param.set( aaa.audio.ref.pan, pan )		end


aaa.show_file_end( "aaa_audio" )