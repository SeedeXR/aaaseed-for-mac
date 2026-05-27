if CLASS.DECLARE( "LUA_WRAP", nil ) then
	LUA_WRAP:set_class_status_doc(	CLASS.STATUS.CORE,
									"wrap a lua script usig c lua_wrap obj, update() execute it" )

end

function LUA_WRAP:set_script_filename( filename )
	param.set( self:get_obj(), "filename", filename )
end

function LUA_WRAP:create( name, script_filename )
	local obj = aaa.obj.create_by_cid( "lua_wrap" )
	param.set( obj, "doit", true )
	param.set( obj, "file_check", true )
	local self = LUA_WRAP:create_instance_with_obj( name, obj )
	if script_filename then
		self:set_script_filename( script_filename )
	end
	return self
end

