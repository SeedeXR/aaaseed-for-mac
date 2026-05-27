
if APP.DECLARE( "GARDEN", APP_GP ) then
end

local L_APP = GARDEN

function L_APP:init_app( ... )
	self:set_init_monitor_pass( false )
	self:set_init_monitor_m_nb( 4 )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end
	self:lock(false)

	return true
end

function L_APP:get_capture_suv_meter()		return 4.,	2.5		end
function L_APP:get_capture_nb_uv()			return 4,	1		end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end


