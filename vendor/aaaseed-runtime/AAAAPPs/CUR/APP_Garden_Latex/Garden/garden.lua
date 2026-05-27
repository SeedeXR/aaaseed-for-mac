
if APP.DECLARE( "GARDEN_LATEX", APP_GP ) then
end

local L_APP = GARDEN_LATEX

function L_APP:init_app( ... )

	self:set_init_monitor_pass( true )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end
	self:lock(false)

	return true
end

function L_APP:get_capture_suv_meter()		return 3,		3*9/16		end
function L_APP:get_capture_nb_uv()			return 2,		1			end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end


