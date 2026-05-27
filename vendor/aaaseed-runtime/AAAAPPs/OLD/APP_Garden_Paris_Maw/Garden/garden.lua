APP.DECLARE( "GARDEN_PARIS_MAW", APP_GP )

local L_APP = GARDEN_PARIS_MAW

function L_APP:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	return oo.getsuper(L_APP).init_app( self, ... )
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

