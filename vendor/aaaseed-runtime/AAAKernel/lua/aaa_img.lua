--UTIL
aaa.show_file_begin( "aaa_img" )

aaa.img.__BIND_BY_BANK_2D = 32
aaa.img.snapshot_id = 0

function aaa.img.make_bank_bind_2d( bind )
	local nb = aaa.img.__BIND_BY_BANK_2D
	local r = bind % nb
	return (bind-r) / nb, r
end
--done in C with a trick : aaa.img.make_tex_index( bank, bind )
function aaa.img.make_bind_2d( bank, bind )
 	return aaa.img.make_tex_index( bank, bind )
end

-- scale on load
--todo do it with a table Scale_on_load
function aaa.img.scale_on_load_get_active()		return param.get_bool( aaa.ref.img_master.scale_on_load_active )	end
function aaa.img.scale_on_load_set_active( b )	param.set( aaa.ref.img_master.scale_on_load_active, b )				end
function aaa.img.scale_on_load_factor( f )		param.set( aaa.ref.img_master.scale_on_load_factor, f )				end
function aaa.img.scale_on_load_size_min( i )	param.set( aaa.ref.img_master.scale_on_load_size_min, i )			end
function aaa.img.scale_on_load_save( b )		param.set( aaa.ref.img_master.scale_on_load_save, b )				end
function aaa.img.scale_on_load_path( path )		param.set( aaa.ref.img_master.scale_on_load_path, path )			end

function aaa.img.scale_on_load_disable()		aaa.img.scale_on_load_set_active( false )				end
function aaa.img.scale_on_load_set( factor, pixel_size_min, dir_name )
	aaa.img.scale_on_load_set_active(	true )
	aaa.img.scale_on_load_factor(		factor )
	aaa.img.scale_on_load_size_min(		pixel_size_min )
	aaa.img.scale_on_load_save(			true )
	aaa.img.scale_on_load_path(			dir_name )
end
function aaa.img.scale_on_load_set_half()		aaa.img.scale_on_load_set( 0.5, 512, "HALF" )			end
function aaa.img.scale_on_load_set_quarter()	aaa.img.scale_on_load_set( 0.25, 512, "QUARTER" )		end

function aaa.img.set_free_when_on_board( b )	param.set( aaa.ref.img_master.free_when_on_board, b )	end

function aaa.img.print( bind )
	if bind then
		aaa.img.move_from_gpu( bind )
		local obj = aaa.obj.get_from_top_by_class( "print_master" )
		aaa.print( "obj is "..obj )
		if obj then
			param.set( obj, "from_texture_bind", bind )
			param.set( obj, "print_trig", 1 )
		end
	else
		aaa.print_error( "aaa.img.print() need a bind to print" )
	end
end

--todo add aaa.img.read_dialog
function aaa.img.save_dialog( bind )
	if bind then
		local name
		name = aaa.file.do_dialog_save( "Save Image", "image" )
		if name then
			aaa.img.move_from_gpu( bind )
			aaa.img.save( bind, name )
			return true
		end
	else
		aaa.print_error( "aaa.img.save_dialog() need a bind to save" )
	end
end

aaa.show_file_end( "aaa_img" )
