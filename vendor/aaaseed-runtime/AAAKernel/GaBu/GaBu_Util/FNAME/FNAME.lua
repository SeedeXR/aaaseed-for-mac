--
--	FNAME
--
aaa.lua.global.declare_table( "FNAME" )

function FNAME.split_ext(fname)
    -- Find last slash (path separator)
    local path, filename = string.match(fname, "(.-)([^\\/]-)$")
    path = path or ""
    filename = filename or fname

    -- Find last dot in the filename
    local dot_pos = filename:match(".*()%.")
    
    if dot_pos then
        local name = path .. filename:sub(1, dot_pos-1)
        local ext  = filename:sub(dot_pos+1)
        return name, ext
    else
        -- no dot -> no extension
        return fname, ""
    end
end


