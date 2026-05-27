--FILE
aaa.show_file_begin( "aaa_file" )

if not aaa.file then aaa.file = {}  end

--todo should be aaa.dir.copy
--todo should be aaa.dir.copy
function aaa.file.copy_dir( src, dst, b_overwrite ) -- todo handle an exclude list like a .gitignore
	local verbose = 0
	if verbose > 0 then aaa.print_fn( "aaa.file.copy_dir()" ) end

	local b_dir_created
	local exclude_files = { ".DS_Store", "Thumbs.db", "PICMDIF.LOG" }
	local err_str
	
	local t_file = aaa.dir.get_files( src )
	array.remove_by_vals_table( t_file, exclude_files )
	if verbose > 0 then table.print( t_file, "t_file" ) end
	for v, name in PAIRS( t_file ) do
		local src_name = src.."/"..name
		local dst_name = dst.."/"..name
		local err = aaa.file.copy( src_name, dst_name, b_overwrite )
		if err then
			err_str = err.." when doing aaa.file.copy( "..src_name..", "..dst_name.." )"
			aaa.print_error( err_str )
		else
			if verbose > 0 then aaa.print_debug( "copy file to "..dst_name ) end
			b_dir_created = true
		end
	end

	local t_dir = aaa.dir.get_dirs( src )
	if verbose > 0 then table.print( t_dir, "t_dir" ) end
	--array.remove_by_vals_table( t_dir, exclude_dirs )
	for v, name in PAIRS( t_dir ) do
		--aaa.print( "\t dir : "..src.."/"..name )
		local src_name = src.."/"..name
		local dst_name = dst.."/"..name
		local err = aaa.file.copy_dir( src_name, dst_name, b_overwrite )
		if err then
			err_str = err.." when doing aaa.file.copy_dir( "..src_name..", "..dst_name.." )"
			aaa.print_error( err_str )
		else
			if verbose > 0 then aaa.print_debug( "copy dir to "..dst_name ) end
			b_dir_created = true
		end
	end

	if not b_dir_created then
		if verbose > 0 then aaa.print_debug( "Need to create dir "..dst ) end
		aaa.dir.create(dst)
	end 

	if verbose > 0 then aaa.print_debug( "aaa.file.copy_dir() return "..err_str ) end
	return err_str
end

-- replaced by its equivalent in C
-- function aaa.file.is_exist( filename )
-- 	local file = io.open( filename, "r" )
-- 	if file then
-- 		--aaa.print( "file "..filename.." exists" )
-- 		file:close()
-- 		return true
-- 	end
-- 	--aaa.print( "file "..filename.." don't exists" )
-- 	return false
-- end

function aaa.file.make_name_number( pre, i, post, format )
	local filename = string.format( "%s"..format.."%s", pre, i, post )
	--aaa.print( format.." -> "..filename )
	return filename
end
function aaa.file.is_exist_name_number( pre, i, post, format )
	local filename = aaa.file.make_name_number( pre, i, post, format )
	return aaa.file.is_exist( filename )
end
function aaa.file.get_format_exist_name_number( pre, i, post )
	local formats = { "%09d", "%08d", "%07d", "%06d", "%05d", "%04d", "%03d", "%02d", "%d" }
	for i_f = 1,9 do
		local format = formats[i_f]
		if aaa.file.is_exist_name_number( pre, i, post, format ) then
			return format
		end
	end
end

function aaa.file.__make_mess( fname, how )
	return "Can't open file "..fname.." to "..((how=="w") and "write" or "read")
end
function aaa.file.__open_low( fname, how )
	local file = io.open( fname, how )
	if file then return file end
	--	force creation
	local b = aaa.file.create( fname )
	if b then
		file = io.open( fname, how )
	end
	return file
end
function aaa.file.open_no_error( fname, how )
	local file = aaa.file.__open_low( fname, how )
	if file then return file end
	aaa.print_error( aaa.file.__make_mess( fname, how ) )
end
function aaa.file.open( fname, how )
	local file = aaa.file.__open_low( fname, how )
	if file then return file end
	aaa.box_error( aaa.file.__make_mess( fname, how ) )
end
function aaa.file.save_text( fname, text )
	local file = aaa.file.open( fname, "w" )
	if not file then return false end
	file:write( text )
	file:close()
	return true
end
function aaa.file.do_if_exist( filename )
	if aaa.file.is_exist( filename ) then
		return aaa.dofile( filename )
	end
end

function aaa.file.do_if_exist_protected( filename, b_diag )
	if not aaa.file.is_exist(filename) then
		return false
	end
	return aaa.lua.dofile_protected( filename, b_diag )
end

function aaa.dir.get_dir_media()
	local dir = aaa.dir.get_dir_kernel()
--	aaa.box_debug( "dir kernel is "..dir ) 
	dir = aaa.file.get_dir_name( string.remove_trailing_slash( dir ) )
--	aaa.box_debug( "dir phase 1 is "..dir ) 
	dir = aaa.file.get_dir_name( string.remove_trailing_slash( dir ) )
--	aaa.box_debug( "dir phase 3 is"..dir ) 
	dir = dir .."/Media/"
--	aaa.box_debug( "dir media is "..dir ) 
	return dir
end
function aaa.dir.make_media_path( path_in_media )
	return aaa.dir.get_dir_media()..path_in_media
end
function aaa.dir.get_dir_tracker()
	return aaa.dir.get_dir_user().."/tracker"
end
function aaa.dir.get_dir_snapshot()
	return aaa.dir.make_media_path("AAASnapshots")
end

aaa.show_file_end( "aaa_file" )
