if APP.DECLARE( "SUBMARINE_EXO", APP_GP ) then
	function SUBMARINE_EXO:set_media_dir()
		MEDIA.set_dir_media( "Exo" )
	end
end

local L_APP = SUBMARINE_EXO

function L_APP:init_app( ... )
	self:set_init_monitor_pass( true )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	local t = {}
	--todonow deal with free of this
	local dir = app.media_dir_rel.."Exo/Fish_dresses/"
	local i = 0
	local function add_seq( name )
		i = i + 1
		local is = IMG_SEQ:create_from_dir( string.lower(name), dir, name.."*.tga" )
		self:print( name.." bind "..is:get_bind(1).." to "..is:get_bind(is:get_nb()) )
		t[i] = is
	end
	add_seq( "ADN" )
	add_seq( "GEO" )
	add_seq( "PIL" )
	add_seq( "SABLE" )
	add_seq( "STRIP" )
	self.img_seq = t
	
	return true
end

-- AQUA STUFF
--
function L_APP:create_tank( meu )
	local tank = TANK:create( "Exo", nil, meu )
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

