-- panel are 1120 * 3, minus 97 - 4 of border = 3172 - 40 (screen border) = 3132
--        by 2400 - (434 + 313) = 1653 - 20 (screen border) = 1633	-> 1632
APP.DECLARE( "CLARISONIC", APP_GP )

function CLARISONIC:set_media_dir()
	MEDIA.set_dir_media( "Clarisonic" )
end

--todo generalize
function CLARISONIC:load_image_one( imgs, key, dir, name )
	name = dir.."/"..name
	local img = IMGS.get_img_using_ext( name, { "png", "tga", "jpg" } )
	if img then
		imgs[key] = img
	end
	return img
end

function CLARISONIC:load_images( imgs, dir, tab )
	imgs = imgs or {}
	for i=1,#tab,2 do
		self:load_image_one( imgs, tab[i], dir, tab[i+1] )
	end
	return imgs
end
function CLARISONIC:load_images_helper( imgs, dir, name )
	return self:load_images( imgs, dir,
									{
										"title", 		name.."_title",
										"title_sub",	name.."_title_sub",
--										"footer", 		name.."_footer",
									} )
end
function CLARISONIC:load_medias()
	self:set_media_dir()
end
-- function CLARISONIC:load_anim()
-- 	self.anim = {}
-- 	local pre
-- 	local function create( name, fname )
-- 		return IMG_SEQ:create( name,	pre..fname,	"tga" )
-- 	end
-- end

function CLARISONIC:get_img_fix( id )
	local img_fix = self.img_fix
	if img_fix then
		return img_fix[id]
	end
end

function CLARISONIC:define_seqs()
	--aaa.audio.set_volume( self:is_onsite() and 1 or .8 )
	aaa.audio.set_volume( 1 )

	local seqs = SEQS:recreate( "Clarisonic", self )
	self:set_media_dir()
	--local playlist = self:get_playlist()

	local dir = app.media_dir_rel.."clarisonic/"
	--self.music = VIDEOS:get_sound( dir.."sound_mp3/".."musique.mp3" )
	--self.music:play()

--[[
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
--]]
	local pre = "Elt/"
	local function get_img( name )	return  IMGS.get_img( dir..name ) end

	self.img_logo			= get_img( "logo.tga"			)
	self.img_logo_c 		= get_img( "logo_c.tga" 		)
	self.img_connect 		= get_img( "connect_4.tga"		)
	self.img_give  			= get_img( "give_4.tga"			)
	self.img_logo_uplifted	= get_img( "logo_Uplifted.tga"	)
	self.img_record			= get_img( "recording_simple.tga"			)

	self.img_alphabet		= {}
	for i = 1,26 do
		self.img_alphabet[i] = get_img( "ALPHABET/ascii_"..(i+64)..".tga" )
	end

	--todo make sure it is called on target
	--self:load_anim()

 	local function add_seq( name, level_id, seq_id, method_name, comment, duration )
		local seq = seqs:add_seq( SEQ:create( name, level_id, seq_id, comment, duration ) )
		--if seq_id>1 then
		--	seq:add_playlist( playlist, seq_id-1, 3. )
		--end
		seq:set_method( "update", self, method_name, seq )
		seq:set_ease_in_time( 1. )
		return seq
	end

	local seq
---[[
--	BEFORE
	local dur_def = 20.
	seq = add_seq( "wait",		1, 1, "update_wait",	"on attend du public" )	--		, 1. )	--eotodo this is for test
	seq = add_seq( "welcome",	1, 2, "update_welcome", "we start", 	2 )
	seq = add_seq( "Play",		1, 3, "update_play",	"we play" )
	seq = add_seq( "Record",	1, 4, "update_record",	"we record", 		22 )
--	seq = add_seq( "Bye",		1, 4, "update_bye",		"bye bye",		20 )
--	seq = add_seq( "end",		1, 6, "update_end",		"SumUp", 	15 )

	seqs:begin( false )
end

function CLARISONIC:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	if not oo.getsuper(CLARISONIC).init_app( self, ... ) then return end

	self:load_medias()

	local gp = self:get_gp()

	local mu = self:get_mu_by_name( "UI_1" )
	mu:set_ui_slot( 2 )

	--	used
	self.mu_obj3d 		= self:get_mu_by_name(		"Obj3D_1"			)
	self.meu_sound 		= self:get_meu_by_name(	"MultiTrack_1"		)
	self.meu_writer 	= self:get_meu_by_name(	"MovieWriter_1"		)
	self.meu_writer:stop_video()	--avoid crash
	self.meu_kin 		= self:get_meu_by_name(	"Kinect_2"			)
	self.meu_clari 		= self:get_meu_by_name(	"KinClariSonic_1"	)
	self.mu_displace 	= self:get_mu_by_name(		"DepthDisplace_1"	)
	self.meu_displace 	= self:get_meu_by_name(	"DepthDisplace_1"	)
	self.mu_pip_1 		= self:get_mu_by_name(		"PIP_1"				)
	self.mu_ref = {}
	for i=1,4 do
		self.mu_ref[i]	= self:get_mu_by_name(		"Ref_Fbo_f"..i			)
	end

	--self.meu_video_gene	= self:get_meu_by_name(	"Video_C"	)

	self:define_seqs()

	ga:add_ga_and_bu_windows( nil, { "SEQS", "SEQ", "PLAYLIST" } )

	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( CLARISONIC )
end


