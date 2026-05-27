if not aaa.lua.global.declare_table( "MEDIA" ) then return end
MEDIA.verbose = 0

MEDIA.__st_dir = STACK:create( "MEDIA" )

MEDIA.image_ext		= { "png", "jpg", "tga"		}
MEDIA.sound_ext 	= { "m4a", "mp3"			}	--, "wav"		}
MEDIA.videos_ext	= { "mov", "mp4", "avi", "wmv", "m4v", "mpg", "mpeg", "m4a", "mp3"	}

function MEDIA.is_type( str, ext_table )
	--aaa.print( "MEDIA.is_type( "..str.." )" )
	if not ext_table then return false end
	local ext = aaa.file.get_ext( str )
	if ext then
		ext = string.lower( ext )
		for i = 1, #ext_table do
			local str = ext_table[ i ]
			if str ~= nil then
				if ext == str then
					return true
				end
			end
		end
	end
	return false
end

function MEDIA.is_name_image( str ) 		return MEDIA.is_type( str, MEDIA.image_ext )	end
function MEDIA.is_name_video( str ) 		return MEDIA.is_type( str, MEDIA.videos_ext )	end
function MEDIA.is_name_sound( str ) 		return MEDIA.is_type( str, MEDIA.sound_ext )	end

function MEDIA.set_dir_silent( dir_name )
	MEDIA.__st_dir:set_top( dir_name )
	--MEDIA.__dir_name = dir_name
	--aaa.box_good( dir_name )
end
function MEDIA.set_dir( dir_name )
	aaa.print_inverse( "MEDIA.set_dir( \""..dir_name.."\" )" )
	MEDIA.set_dir_silent( dir_name )
end
function MEDIA.push_dir( dir_name )
	aaa.print_inverse( "MEDIA.push_dir( \""..dir_name.."\" )" )
	MEDIA.__st_dir:push( dir_name )
end
function MEDIA.pop_dir()
	MEDIA.__st_dir:pop()
	aaa.print_inverse( "MEDIA.pop_dir() : dir now \""..MEDIA.__st_dir:get_top( ).."\"" )
end
function MEDIA.get_dir()
	return MEDIA.__st_dir:get_top()
end

function MEDIA.__make_dir_media( dir_name )
	local str 
	if app then
		str = app.media_dir_rel..dir_name
	else
		str = dir_name
	end
	if dir_name~="" and string.sub( dir_name, -1 )~="/" then
		--GABU_OBJ:box_debug( "need to add / to dir name "..dir_name )
		str = str .. "/"
	end
	return str
end
function MEDIA.set_dir_media_silent( dir_name )	MEDIA.set_dir_silent( MEDIA.__make_dir_media(dir_name) )	end
function MEDIA.set_dir_media(        dir_name )	MEDIA.set_dir(        MEDIA.__make_dir_media(dir_name) )	end
function MEDIA.push_dir_media(       dir_name )	MEDIA.push_dir(       MEDIA.__make_dir_media(dir_name) )	end

function MEDIA.make_fname( fname )
	--aaa.box_good( fname.." -> "..string.sub( fname,2,2 ))
	if string.sub( fname, 2, 2 ) == ":" then	--todo compatibility
		return fname
	end

	local dir = MEDIA.get_dir()
	--self:print( "MEDIA.make_fname() MEDIA.get_dir() returned"..dir )
	if dir then
		fname = string.enforce_trailing_slash( dir )..fname
	end
	return fname
end
function MEDIA.is_exist( fname )
	local fname = MEDIA.make_fname( fname )
	return aaa.file.is_exist( fname )
end
function MEDIA.is_dir_exist( dname )
	local dname = MEDIA.make_fname( dname )
	--aaa.box_good( "\""..dname.."\" tested" )
	return aaa.dir.is_exist( dname )
end


