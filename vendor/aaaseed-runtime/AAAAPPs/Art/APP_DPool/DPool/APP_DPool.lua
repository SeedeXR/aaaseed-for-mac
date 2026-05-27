if APP.DECLARE( "DPOOL", APP_GP ) then
	function DPOOL:set_media_dir()
		MEDIA.set_dir_media( "AAAPool" )
	end
end

local L_APP = DPOOL

function L_APP:init_app( ... )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	self:set_media_dir()

	--todo not sure these anims are uses, if used we need a player perhap (IMG_SEQ_PLAYER)
	self.coraux = IMG_SEQ:create( "coraux", 	"Coraux/",		"png" )
	self.algues = IMG_SEQ:create( "Algues", 	"Algues/",		"png" )

	local anim = {}
	anim[1] = IMG_SEQ:create( "calamar",	"Anim/calamarV8/",	"png" )
	anim[2] = IMG_SEQ:create( "manta",		"Anim/mantaV4/",	"png" )
	anim[3] = IMG_SEQ:create( "shark",		"Anim/sharkV5/",	"png" )
	anim[4] = IMG_SEQ:create( "tuna", 		"Anim/tunaV5/",		"png" )
	anim[4].b_triangle = true
	anim[5] = IMG_SEQ:create( "turtle",		"Anim/turtleV4/",	"png" )
	self.anim = anim

	self.fish = IMG_SEQ:create( "fish", 	"Anim/fish/",		"png" )

	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

