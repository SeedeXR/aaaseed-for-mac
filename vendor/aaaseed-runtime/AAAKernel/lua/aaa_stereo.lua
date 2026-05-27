--STEREO
aaa.show_file_begin( "aaa_stereo" )

aaa.stereo = { ref={} }
function aaa.stereo.init_ref()
	local ref = aaa.stereo.ref
	ref.obj					= aaa.obj.get_from_top_by_class( "stereo" )
		ref.active				= param.get_ref( ref.obj, "active" )
		ref.field_for_update	= param.get_ref( ref.obj, "field_for_update" )
end

function aaa.stereo.is_active()		return aaa.stereo.b_active		end
function aaa.stereo.is_field_for_update()
	--aaa.box_error( "is_field_for_update "..param.get( aaa.stereo.ref.field_for_update ) )
	--aaa.print( "aaa.stereo.is_field_for_update() "..param.get_bool( aaa.stereo.ref.field_for_update ) )
	return aaa.stereo.b_field_update
end
function aaa.stereo.update()
	local stereo = aaa.stereo
	local ref = stereo.ref
	stereo.b_active 		= param.get_bool( ref.active )
	stereo.b_field_update 	= param.get_bool( ref.field_for_update )
end

aaa.show_file_end( "aaa_stereo" )

