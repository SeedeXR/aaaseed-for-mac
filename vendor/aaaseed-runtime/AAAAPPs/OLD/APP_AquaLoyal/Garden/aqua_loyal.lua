if APP.DECLARE( "AQUA_LOYAL", APP_GP ) then
	function AQUA_LOYAL:set_media_dir()
		MEDIA.set_dir_media( "AquaLoyal" )
	end
end

local L_APP = AQUA_LOYAL

function L_APP:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	return true
end

-- AQUA STUFF
--
function L_APP:create_tank( meu )		return TANK:create( "Aqualoyal", nil, meu )	end
function L_APP:is_caustic()				return true		end
function L_APP:get_tank_grea_max_nb()	return 16	end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

