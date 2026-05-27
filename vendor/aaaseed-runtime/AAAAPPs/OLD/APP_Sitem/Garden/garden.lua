if APP.DECLARE( "SITEM", APP_GP ) then
	function SITEM:set_media_dir()
		MEDIA.set_dir_media( "Sitem" )
	end
end

local L_APP = SITEM

function L_APP:init_app( ... )
	param.set( aaa.ref.use_filesystem, false )

	self:set_init_monitor_m_nb( 0 )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

