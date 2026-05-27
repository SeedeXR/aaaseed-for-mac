APP.DECLARE( "LOTUS", APP_GP )

local L_APP = LOTUS

function L_APP:set_media_dir()
	MEDIA.set_dir_media( "Lotus" )
end

function L_APP:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end
	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

