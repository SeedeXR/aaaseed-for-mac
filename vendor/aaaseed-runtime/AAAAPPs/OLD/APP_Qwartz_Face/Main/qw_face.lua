APP.DECLARE( "QWARTZ_FACE", APP_GP )

local L_APP = QWARTZ_FACE

function L_APP:set_media_dir()
	MEDIA.set_dir_media( "Qwartz/Face/UI" )
end

function L_APP:load_medias_ui()
	self:set_media_dir()

	self:load_media( "neutre", 		"ecran-00-bandeau-logos.png"					)
	self:load_media( "approcher",	"ecran-00-bandeau-titre.png"					)
	self:load_media( "qu_pris",		"ecran-01-bandeau-souhaitez-vous-photo.png"		)
	self:load_media( "photo",		"ecran-02-bandeau-photo-dans.png"				)
	self:load_media( "qu_publier",	"ecran-03-bandeau-souhaitez-vous-publier.png"	)
	self:load_media( "publiee",		"ecran-04-bandeau-photo-publiee.png"			)
	self:load_media( "oui",			"oui_move.png"			)
	self:load_media( "non",			"non_move.png"			)
	self:load_media( "amuser",		"non_move_amuser.png"	)
	self.__medias.__count = {}
	--self:box_error( "Que passa" )
	for i = 1, 10 do
		self.__medias.__count[i] = self:load_media( nil, "countdown-"..i..".png"	)
	end
end

--todo APP_GP:free() could be better
function L_APP:free()
	oo.getsuper(L_APP).free( self )
	self.meus	= nil
end
function L_APP:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	ga:set_onsite_by_machine( { "AAA_QWARTZ_01",  "AAA_QWARTZ_02",  "AAA_QWARTZ_03" } )

	self:create_log_in_dir( app.media_dir_rel.."Qwartz/Face/", 1200, true )
	self:add_log( "Start" )
	if self:is_onsite() then
		self:set_shutdown_no_save_on_time( 23, 30 )
	end

	self.meus 	= 	{}
	local meus = self.meus

	meus.mask		=	self:get_meu_by_name( "QwMask_1"		)
	meus.compo		=	self:get_meu_by_name( "QwKinCompo_1"	)
	meus.field		=	self:get_meu_by_name( "QwField_1"		)
	meus.boid		=	self:get_meu_by_name( "Boid_1"			)
	meus.voronoi	=	self:get_meu_by_name( "Voronoi_1"		)
	meus.ui			=	self:get_meu_by_name( "QwUi_1"			)
	meus.dif		=	self:get_meu_by_name( "fbo_DIF"		)
	meus.out		=	self:get_meu_by_name( "Out_1"			)


	local b_onsite = 	self:is_onsite()
	meus.out:get_mu():set_value( b_onsite and 1 or 0 )
	ga:set_ui_group_active( not b_onsite )	--todo should be in APP_GP:init_app()

	self.coverage = { 0, 0 }
	self.compo = 0;
	self:load_medias_ui()

	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

