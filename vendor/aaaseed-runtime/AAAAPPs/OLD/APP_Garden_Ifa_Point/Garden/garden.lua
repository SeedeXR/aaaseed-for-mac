if APP.DECLARE( "GARDEN_IFA_POINT", APP_GP ) then
end

local L_APP = GARDEN_IFA_POINT

function L_APP:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	ga:set_onsite_by_machine( { "MAAHEX", "IFA-VAIO" } )
	local b_onsite = 	self:is_onsite()
	ga:set_ui_group_active( not b_onsite )
	if b_onsite then
		ga:set_lock_app( true )
	end

	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

