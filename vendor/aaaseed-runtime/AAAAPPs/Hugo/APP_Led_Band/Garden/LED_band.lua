if APP.DECLARE( "GARDEN_LED", APP_GP ) then
end

local L_APP = GARDEN_LED

function L_APP:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

