
if APP.DECLARE( "FRESCO", APP_GP ) then
end

local L_APP = FRESCO

function L_APP:init_app( ... )

	self:set_init_monitor_pass( true )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end
	self:lock(false)
	return true
end

function L_APP:get_capture_suv_meter()		return 7,	5		end
---function L_APP:get_capture_nb_uv()			return 2,	1		end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP, "fresco Issy V1" )
end


