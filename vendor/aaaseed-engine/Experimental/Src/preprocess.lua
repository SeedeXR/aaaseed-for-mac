

print("Lua script preprocess.")

function format_error_mess( err_code, err_mess )
  file_loc = arg[0] 
  io.stderr:write( file_loc.." : error "..err_code..": "..err_mess.."\n" )  
end

function my_error( err_code, err_mess )
  format_error_mess( err_code, err_mess )
end

local base_folder = "C:\\AAA\\AAAAlast\\AAADev\\Src\\"
local sub_folder = "\\obj_ui\\bdd\\"
local folder = base_folder..sub_folder

function write_in_bdd_hpp( bdd_hpp_file )
  local f = io.open( folder..bdd_hpp_file, "r" )
  if not f then
      my_error( "03", "failed to open "..bdd_hpp_file.." for reading")
  else
    local f_str = f:read( "*all" )
    f:close()
--    local offset = f_str:find( "namespace" )
    local offset_start, offset_end = f_str:find( "REAL%s+_origin%[3%];")
    print( "hpp offset start, end : "..tostring(offset_start)..", "..tostring(offset_end) )

    if offset_end ~= nil then
      local str_before = f_str:sub( 1, offset_end )
      local str_after = f_str:sub( offset_end+1, #f_str )
  
      local str_insert = "// preprocessing wrote a comment in a target file \\o/"
      local f_str_new = str_before..str_insert..str_after
  
      --print( f_str_new )
      f = io.open( base_folder.."\\obj_ui\\bdd\\bdd_ben.h", "w" )
      if not f then
        my_error( "05", "failed to open for writing" )
      else
        f:write( f_str_new )
      end
    else
      my_error( "04", "can't find the place to append text in file" )
    end
  end
end

function write_in_bdd_cpp( bdd_cpp_file )
  local f = io.open( folder..bdd_cpp_file, "r" )
  if not f then
      my_error( "03", "failed to open "..bdd_cpp_file.." for reading")
  else
    local f_str = f:read( "*all" )
    f:close()
--    local offset = f_str:find( "namespace" )
    local offset = f_str:find( "PARAM_DEF_POINT_XYZ%( origin %)")
    if offset then offset = offset + #"PARAM_DEF_POINT_XYZ%( origin %)" end
    print( "cpp offset : "..tostring(offset) )

    if offset ~= nil then
      local str_before = f_str:sub( 1, offset )
      local str_after = f_str:sub( offset, #f_str )
  
      local f_str_new = str_before.."// preprocessing wrote a comment in a target file \\o/\n"..str_after
  
      --print( f_str_new )
      f = io.open( base_folder.."\\obj_ui\\bdd\\bdd_ben.cpp", "w" )
      if not f then
        my_error( "05", "failed to open for writing" )
      else
        f:write( f_str_new )
      end
    else
      my_error( "04", "can't find the place to append text in file" )
    end
  end
end

function check_param_file_date( bdd_param_file )

  local function extract_date_from_file( file )
    local f = io.popen( "dir \""..folder.."\\"..file.."\"" )
    if not f then
        my_error( "02", "failed to test file" )
    else
      while true do
        local line = f:read( "*lines" )
        if line == nil then break end

        if line:find( file ) then
          --print( "line substitution : "..tostring( line:sub( 1, 20 ) ) )
          return line:sub( 1, 20 )
        end
      end
    end
    return nil
  end

  local date = extract_date_from_file( bdd_param_file )
  if date ~= nil then
    print( "date : "..date )
    return true
  else
    my_error( "14", "extract_date_from_file is a mess" )
    return false
  end
end

function find_a_token_in_param_file( bdd_param_file )
  local f = io.open( folder..bdd_param_file, "r" )
  if not f then
      my_error( "01", "failed to open "..bdd_param_file.." for reading")
  else
    local f_str = f:read( "*all" )
    f:close()
--    local offset = f_str:find( "namespace" )
    local offset = f_str:find( "token")
    if offset then offset = offset + #"token" end
    print( "param offset : "..tostring(offset) )
    return offset
  end
  return nil
end


if check_param_file_date( "bdd_ben.param" ) then
  print( "check_param_date no bug for now but not actually working" )
end

if find_a_token_in_param_file( "bdd_ben.param" ) then
--  write_in_bdd_cpp( "bdd_ben.cpp" )
  write_in_bdd_hpp( "bdd_ben.h" )
else
  print( "no token found in param file" )
end

if false then
  --my_error( "00", "preprocessing mock" )
  my_error( "00", "do .h change make recompile project ?" )
--  io.stderr:write( base_folder.."preprocess.lua".." : error 00: preprocessing mock" )  
end

