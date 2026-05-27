if APP.DECLARE( "MDH_NEAND", APP_GP ) then
end

MDH_NEAND.__lang_name	= { "french",	"english",	"spanish",	"sign"	}
MDH_NEAND.__lang_volume = { 1,			.9, 		.9,			1		}
MDH_NEAND.lang_volume	= 1
MDH_NEAND.timing = { 	french		= {		16,	24,		36.5, 15, 49, 13 },
						english		= {		16, 24,		36.5, 14, 49, 13 },
						spanish		= {		20, 25.5,	39.5, 16, 49, 14 },
						sign		= {		20, 26,		36.5, 14, 49, 14 },
					}

function MDH_NEAND:set_media_dir()
	MEDIA.set_dir_media( "MdH/Neand" )
end

--todo generalize
function MDH_NEAND:load_image_one( imgs, key, dir, name )
	name = dir.."/"..name
	local img = IMGS.get_img_using_ext( name, { "png", "tga", "jpg" } )
	if img then
		imgs[key] = img
	end
	return img
end

function MDH_NEAND:load_images( imgs, dir, tab )
	imgs = imgs or {}
	for i=1,#tab,2 do
		self:load_image_one( imgs, tab[i], dir, tab[i+1] )
	end
	return imgs
end
function MDH_NEAND:load_images_helper( imgs, dir, name )
	return self:load_images( imgs, dir,
									{
										"title", 		name.."_title",
										"title_sub",	name.."_title_sub",
--										"footer", 		name.."_footer",
									} )
end
function MDH_NEAND:get_playlist()
	local pl = self.playlist
	if not pl then
		local dir		= MEDIA.get_dir().."sound_mp3/"
		local names		= { "1_welcome", "2_intro", "3_gene_1", "4_gene_2", "5_morph", "6_end" }
		pl = PLAYLIST:create( "voice", false )
		for i=1,#names do
			local v = pl:add_sound_file( dir..names[i].."_fr", "mp3", i )
		end
		self.playlist = pl
	end
	return pl
end


function MDH_NEAND:get_face_trak( facetrak_id )
	local name = "FaceTrak_"..facetrak_id
	return self:get_meu_by_name( name )
end
function MDH_NEAND:get_face_info( facetrak_id, face_id )
	local m = self:get_face_trak( facetrak_id )
	--self:print_error( "m is "..m )
	if m then return m:get_face_info( face_id ) end
end
function MDH_NEAND:get_face_track_rect( facetrak_id )
	local m = self:get_face_trak( facetrak_id )
	--self:print( "m is "..m.." "..m.get_track_rect )
	if m then return m:get_track_rect() end
end
function MDH_NEAND:load_medias()
	self:set_media_dir()
end

function MDH_NEAND:set_timings( la )
	local t = self.timing[la]
	local seqs = self.seqs
	if seqs then
		for i=1,6 do
			local seq = seqs:get_seq( i+1 )
			seq:set_duration( t[i] )
		end
	end
end
function MDH_NEAND:define_seqs()
	--aaa.audio.set_volume( self:is_onsite() and 1 or .8 )
	aaa.audio.set_volume( 1 )

	local seqs = SEQS:recreate( "MdH_Neand", self )

	self:set_media_dir()
	local playlist = self:get_playlist()
 	local function add_seq( name, level_id, seq_id, method_name, ... )
		local seq = seqs:add_seq( SEQ:create( name, level_id, seq_id, ... ) )
		if seq_id > 1 then
			seq:add_playlist( playlist, seq_id-1, 2. )
		end
		seq:set_method( "update", self, method_name, seq )
		seq:set_ease_in_time( 1. )
		return seq
	end

	local dir = app.media_dir_rel.."MdH/Neand/"
	self.music = VIDEOS:get_sound( dir.."sound_mp3/".."musique.mp3" )
	self.music:play()

	aaa.video.set_bind_filename( 96, dir.."film/TRAV_ADN.mp4" )
	aaa.video.set_bind_filename( 96+1, dir.."Text/lsf.mp4" )

	self.imgs = self:load_images( nil, "3d",
									{
										"face_mask",		"face_mask",
										"face_rendered",	"face_rendered",
									} )
	self.imgs = self:load_images( self.imgs, "3d/intro",
									{
										"face_uv1", 		"VISAGE INTRO A",
										"face_uv2", 		"VISAGE INTRO B",
										"face_uv3", 		"VISAGE INTRO A",
										"face_wire_1",		"VISAGE INTRO wire",
										"face_wire_2",		"VISAGE INTRO wireSmooth",
										"face_ambient", 	"VISAGE INTRO Ambient Occlusion",
										"face_complete", 	"VISAGE INTRO CompleteMap",
									} )
	self.imgs = self:load_images( self.imgs, "3d",
									{
										"exportUV1024",		"exportUV1024",
									} )
	local t_mu_face_wait	=	{	"Wait3d_1",		"Wait3d_2"		}
	local t_mu_face_morph	=	{	"Morph3d_1",	"Morph3d_2"		}
	local t_mu_face_track	=	{	"Ref_Fbo_f2",	"facetrak_1",
									"Ref_Fbo_f6",	"facetrak_2"
								}
	local t_mu_face_draw	=	{	"Ref_Fbo_f3",	"faceacc_1",
									"Ref_Fbo_f7",	"faceacc_2"
								}
	local t_mu_face_gene_2 	=	{	"Video_c",		"pre",			"pip_1",	"pip_2" }
	local t_mu_face_blur 	=	{	"blurneand_1"		}

	local imgs
	local seq
---[[
--	BEFORE
	local img_dir = "Text"
	seq = add_seq( "wait",		1, 1, "update_wait",	"on attend du public" )	--		, 1. )	--eotodo this is for test
		seq:set_imgs( self:load_images_helper( nil, img_dir, "wait" ) )
		seq:add_mu_used_by_name( t_mu_face_wait )

	seq = add_seq( "welcome",	1, 2, "update_welcome", "experience interactive", 	17 )
		local imgs_wel = seq:set_imgs( self:load_images_helper( nil, img_dir, "welcome" ) )

	seq = add_seq( "intro",		1, 3, "update_intro",	"deux especes", 			25 )
		seq:set_imgs( self:load_images_helper( nil, img_dir, "intro" ) )
		seq:add_mu_used_by_name( t_mu_face_track )

	seq = add_seq( "gene_1",	1, 4, nil,				"adn",						37.5 )
		imgs = seq:set_imgs( self:load_images_helper( nil, img_dir, "gene" ) )
		for i=1,4 do
			local str = "text_"..i
			self:load_image_one( imgs, "text_"..i, img_dir, "gene_"..str )
		end
		seq:add_mu_used_by_name( t_mu_face_track )
		seq:add_mu_used_by_name( t_mu_face_gene_2 )
		--seq:set_method( "update", self, "update_gene", seq )

	seq = add_seq( "gene_2",	1, 5, nil,				"rencontre",				14 )
		seq:set_imgs( imgs )
		seq:add_mu_used_by_name( t_mu_face_track )
		seq:add_mu_used_by_name( t_mu_face_draw )

	seq = add_seq( "morph",		1, 6, nil,				"morph 3d", 				49 )
		seq:set_imgs( self:load_images_helper( nil, img_dir, "morph" ) )
		seq:set_ease_in_time( 3. )
		seq:add_mu_used_by_name( t_mu_face_track )
		seq:add_mu_used_by_name( t_mu_face_draw )
		seq:add_mu_used_by_name( t_mu_face_morph )

	seq = add_seq( "end",		1, 7, "update_end",		"merci", 					14 )
		seq:set_imgs( imgs_wel )
		seq:add_mu_used_by_name( t_mu_face_blur )
		seq:set_ease_in_time( 5. )

	seqs:begin( true )
end

function MDH_NEAND:get_zone( id )
	return self.zone[id]
end

function MDH_NEAND:init_zone()
	self.zone = {}
	for i=1,2 do
		local zone = {
						presence_raw = 0,
						presence = 0,
						tracking = 0,
						inter = 0,
					}
		self.zone[i] = zone
	end
end

function MDH_NEAND:init_app( ... )
	local ln = self.__lang_name
	self:init_lang( ln[1], ln[2], ln[3] )
	self:set_init_monitor_m_nb( 0 )

	if not oo.getsuper(MDH_NEAND).init_app( self, ... ) then return end

	self:load_medias()

	self:init_zone()

	local mu = self:get_mu_by_name( "UI_1" )
	mu:set_ui_slot( 2 )
	--	used
	self.meu_video_gene	= self:get_meu_by_name(	"Video_C"	)
	self.mu_lang_sign	= self:get_mu_by_name(		"Video_D"	)
	self.meu_lang_sign	= self:get_meu_by_name(	"Video_D"	)

	self:define_seqs()

	self:get_meu_by_name( "Video_B" ):set_capture_open_run( true )

	ga:add_ga_and_bu_windows( nil, { "SEQS", "SEQ", "PLAYLIST" } )

	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( MDH_NEAND )
end


