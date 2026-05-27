
if APP.DECLARE( "APP_SHY_FOUNTAIN", APP_GP ) then
end

local L_APP = APP_SHY_FOUNTAIN

function L_APP:init_app( ... )
	self:set_init_monitor_pass( false )
	self:set_init_monitor_m_nb( 4 )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end
	self:lock(true)

	return true
end

--need to be adapted for each setting when used 
function L_APP:get_capture_suv_meter()		return 8.,	8.		end
function L_APP:get_capture_nb_uv()			return 1,	1		end

--function L_APP:is_onsite()	return true	end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end


