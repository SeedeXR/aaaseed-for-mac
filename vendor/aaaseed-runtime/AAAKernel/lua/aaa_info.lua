
--UTIL
aaa.show_file_begin( "aaa_info" )

--
--	AAA info
--
--
if not aaa.info then
	aaa.info = {}
end
function aaa.info.get_ref()				return aaa.obj.get_from_top_by_class( "info" )				end
function aaa.info.get_exe_path()		return param.get( aaa.info.get_ref(), "exe_path" )			end
function aaa.info.get_exe_name_pure()	return aaa.file.get_name_pure( aaa.info.get_exe_path() )	end

aaa.show_file_end( "aaa_info" )

