local cam_switch_ref
if aaa.lua.global.get( "ga" ) then

	function GA:update_simulator()
		if not cam_switch_ref then
			cam_switch_ref = param.get_ref( aaa.layers.get_cur(), "use_camera" )
		end
		if self.cam then
			self.cam:update()
			param.set( cam_switch_ref, "current" )
		end
	end
	ga:update_simulator()
else
	local ref = param.get_ref( aaa.layers.get_cur(), "use_camera" )
	param.set( ref, "owner" )
end
