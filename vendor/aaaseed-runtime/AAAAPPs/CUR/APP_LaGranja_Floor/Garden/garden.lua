
if APP.DECLARE( "LA_GRANJA_FLOOR", APP_GP ) then
end

local L_APP = LA_GRANJA_FLOOR

function L_APP:init_app( ... )

	self:set_init_monitor_pass( true )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end
	self:lock(false)

	local name = "TutoLuaHello_1"
	local meu = self:get_meu_by_name_no_error( name )
	if meu then
		local mu = meu:get_mu()
		if mu:get_value() > 0 then
			meu:edit_lua()
			aaa.mess.show( "Script ready to be edited for MU "..name )
			mu:set_ui_slot()
		end
	end
	return true
end

function L_APP:get_capture_suv_meter()		return 8,	5.		end
function L_APP:get_capture_nb_uv()			return 2,	1		end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end


