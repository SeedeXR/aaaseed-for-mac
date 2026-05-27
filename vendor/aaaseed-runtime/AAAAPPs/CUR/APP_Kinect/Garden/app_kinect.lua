if APP.DECLARE( "APP_KINECT", APP_GP ) then
end

local L_APP = APP_KINECT

function L_APP:init_app( ... )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	local ref = aaa.net.ref
	param.set( ref.obj, "active", 1 )
	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

