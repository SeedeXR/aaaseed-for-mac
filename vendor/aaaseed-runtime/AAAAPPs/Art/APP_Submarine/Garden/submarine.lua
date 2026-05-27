if APP.DECLARE( "SUBMARINE", APP_GP ) then
	function SUBMARINE:set_media_dir()
		MEDIA.set_dir_media( "SubMarine" )
	end
end

local L_APP = SUBMARINE

function L_APP:init_app( ... )
	self:set_init_monitor_pass( true )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(SUBMARINE).init_app( self, ... ) then return end

	return true
end

-- AQUA STUFF
--
function L_APP:create_tank( meu )
	local tank = TANK:create( "SubMarine", nil, meu )
	return tank
end
--todosubmarine
function L_APP:is_caustic()			return false	end
function L_APP:is_fish_boid()	--we need that to see the curves
	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

