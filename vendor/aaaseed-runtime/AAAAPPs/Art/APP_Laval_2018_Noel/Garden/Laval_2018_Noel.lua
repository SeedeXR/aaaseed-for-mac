if APP.DECLARE( "LAVAL_NOEL", APP_GP ) then
	function LAVAL_NOEL:set_media_dir()
		MEDIA.set_dir_media( "Laval" )
	end
end

local L_APP = LAVAL_NOEL

function L_APP:init_app( ... )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

