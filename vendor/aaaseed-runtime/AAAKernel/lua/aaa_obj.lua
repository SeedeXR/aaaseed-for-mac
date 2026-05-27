--OBJ
aaa.show_file_begin( "aaa_obj" )


function aaa.obj.get_lua_wrap(	ref )	return aaa.obj.get_branch_by_class( ref, "lua_wrap" )	end
function aaa.obj.edit_lua(	ref )
	local wrap = aaa.obj.get_lua_wrap( ref )
	if wrap then
		param.set( wrap, "edit_trig", true )
	end
end

function aaa.obj.clone( src, root )
--	if not dst then
	local cid = aaa.obj.get_class( src )
	-- local str = "CLONE "..cid
	-- if root then
	-- 	str = str .." from root " ..aaa.obj.get_name(root)
	-- end
	-- aaa.print( str )
	if not root then
		if aaa.lua.global.get( "GA" ) then
			root = GA.__c_node_rest
		end
	end
	--aaa.print( "aaa.obj.clone() with src "..cid ) aaa.lua.global.get( "GA" ) and GA.__c_node_tex_video or ni
	local dst = aaa.obj.create_by_cid( cid, nil, root )
--	end
	aaa.obj.set_param_from( dst, src )
	return dst
end

function aaa.obj.is_class( obj, class_name )
	return aaa.obj.get_class( obj ) == class_name
end

aaa.show_file_end( "aaa_obj" )
