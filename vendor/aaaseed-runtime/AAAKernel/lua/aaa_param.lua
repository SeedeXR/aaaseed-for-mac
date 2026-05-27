--PARAM
aaa.show_file_begin( "aaa_param" )

--todomona regroup in one c fn
function param.set_and_save_no( ref, val )
	param.set( ref, val )
	param.set_save( ref, false )
end
function param.set_and_save( ref, val, b_save )
	param.set( ref, val )
	if b_save ~= nil then
		param.set_save( ref, b_save )
	end
end

--param = param
function param.set_v2( ref, v )
	local set = param.set
	set( ref[1], v[1] )
	set( ref[2], v[2] )
end
function param.set_v3( ref, v )
	local set = param.set
	set( ref[1], v[1] )
	set( ref[2], v[2] )
	set( ref[3], v[3] )
end
function param.set_v4( ref, v )
	local set = param.set
	set( ref[1], v[1] )
	set( ref[2], v[2] )
	set( ref[3], v[3] )
	set( ref[4], v[4] )
end
function param.get_ref_xyz( obj_ref, name_pre )
	return		param.get_ref( obj_ref, name_pre.."_x" )
			,	param.get_ref( obj_ref, name_pre.."_y" )
			,	param.get_ref( obj_ref, name_pre.."_z" )
end
function param.get_ref_xyz_packed( obj_ref, name_pre )
	return	pack(	param.get_ref_xyz( obj_ref, name_pre )	)
end
function param.get_ref_uva( obj_ref, name_pre )
	return		param.get_ref( obj_ref, name_pre.."_u" )
			,	param.get_ref( obj_ref, name_pre.."_v" )
			,	param.get_ref( obj_ref, name_pre.."_a" )
end
function param.get_ref_uva_packed( obj_ref, name_pre )
	return	pack(	param.get_ref_uva( obj_ref, name_pre )	)
end
function param.get_v2( ref )
	local get = param.get
	return get(ref[1]), get(ref[2])
end
function param.get_v2_packed( ref )
	local get = param.get
	return { get(ref[1]), get(ref[2]) }
end
function param.get_v3( ref )
	local get = param.get
	return get(ref[1]), get(ref[2]), get(ref[3])
end
function param.get_v3_packed( ref )
	local get = param.get
	return { get(ref[1]), get(ref[2]), get(ref[3]) }
end
function param.get_v4( ref )
	local get = param.get
	return get(ref[1]), get(ref[2]), get(ref[3]), get(ref[4])
end
function param.get_v4_packed( ref )
	local get = param.get
	return { get(ref[1]), get(ref[2]), get(ref[3]), get(ref[4]) }
end


aaa.show_file_end( "aaa_param" )
