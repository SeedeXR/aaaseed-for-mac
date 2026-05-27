if APP.DECLARE( "KINECT_SEND", APP_GP ) then
	function KINECT_SEND:set_media_dir()
		MEDIA.set_dir_media( "KinectSend" )
	end
end

local L_APP = KINECT_SEND

function L_APP:init_app( ... )
	self:set_init_ui_slot_nb( 4 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

