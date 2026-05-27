if not kinect then
	kinect = {}
	local obj = aaa.obj.get_from_top_by_class( "kinect_ui" )
	local ref = param.get_ref( obj, "motor_position" )
	if ref then
		local name = aaa.net.machine
		local host = aaa.net.host_id
		aaa.print( "kinect adjust to machine "..name.." "..host )
		if aaa.pc.is_maa() then
			param.set( ref, 1   )
		elseif name=="SPAHIR1-PC" then
			param.set( ref, 1. )
		end
	end
end