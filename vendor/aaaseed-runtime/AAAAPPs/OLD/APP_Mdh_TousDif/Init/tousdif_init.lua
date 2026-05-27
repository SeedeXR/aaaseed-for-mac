APP.DECLARE( "MDH_TOUSDIF", APP_GP )

function MDH_TOUSDIF:set_media_dir()
	MEDIA.set_dir_media( "MdH/TousDif" )
end

function MDH_TOUSDIF:load_image_one( imgs, key, dir, name )
	local name = dir.."/"..name
	local img = IMGS.get_img_using_ext( name, { "png", "tga", "jpg" } )
	if img then
		imgs[key] = img
	else
		aaa.box_error( "cam't read image "..name.."\n  in dir "..dir  )
	end
end

--[[
function MDH_TOUSDIF:load_images( dir, tab )
	local imgs = {}
	for i=1,#tab,2 do
		self:load_image_one( imgs, tab[i], dir, tab[i+1] )
	end
	return imgs
end
function MDH_TOUSDIF:load_images_key( dir, tab )
	local imgs = {}
	for i=1,#tab do
		self:load_image_one( imgs, tab[i], dir, tab[i] )
	end
	return imgs
end
--]]
function MDH_TOUSDIF:set_active_game( b_on )
	if b_on then
		self.snds.begin:play()
		self:choose_and_stop_video()
	else

	end
end

function MDH_TOUSDIF:define_seqs()

	local seqs = SEQS:recreate( "MdH_TousDif", self )

	self:set_media_dir()

	if true then
		local video_dir = app.media_dir_rel.."MdH/TousDif/films/"

		for i=1,7 do
			aaa.video.set_bind_filename( 96+i*2-2, video_dir.."TEXTE SEQ "..i..".avi" )
			aaa.video.set_bind_filename( 96+i*2-1, video_dir.."FILM SEQ "..i..".avi" )
		end
	end

	local img_dir = "Imgs"
	local imgs = {}
	local function load_image( name, file )
		self:load_image_one( imgs, name,  img_dir, file and file or name )
	end
	local function load_image_lr( name )
		load_image( name.."l" )
		load_image( name.."r" )
	end
	local function load_image_GD( name, filename )
		self:load_image_one( imgs, name.."l", img_dir, filename.."_G" )
		self:load_image_one( imgs, name.."r", img_dir, filename.."_D" )
	end
	local function load_image_iGD( i, name, filename )
		load_image_GD( name..i, filename..i )
	end


	for i=0,50 do
		local filename = "COUNT_"..aaa.format.int_to_char2( i )
		self:load_image_one( imgs, "count_"..i, "count/8bit", filename )
	end
	for i=1,7 do
		if false then
			load_image_lr( "game_"..i )
			load_image_lr( "photo_"..i )
		else
			IMGS.set_force_keep_on_cpu( true )
			load_image_iGD( i, "game_", 		"SILHOUETTE " )

			IMGS.set_force_keep_on_cpu( nil )
			--todo remove
			--load_image_GD( i, "photo_ref_", "TD PHOTO " )
			load_image_iGD( i, "photo_", 		"TD_PHOTOSOLO_"	)
			load_image_iGD( i, "contour_", 		"CONTOUR_" 		)
			--load_image_iGD( i, "contour_", 		"8bit/CONTOUR_" 		)
			load_image_iGD( i, "photo_circle_",	"ROND_" 		)
		end
	end

	for i=1,4 do
		load_image( "txt_intro_"..i )
		load_image_lr( "perso_intro_"..i )
	end

	load_image_GD( "bravo_circle_", "ROND_BRAVO"	)
	load_image_GD( "dom_circle_",	"ROND_DOMMAGE"	)

	--load_image_lr( "perso_welcome_" )
	--load_image_lr( "back_ref_" )

	local function load_text( name, file ) load_image( "txt_"..name,  "txt_"..file ) end
	load_text( "wel",		"welcome"	)
	load_text( "mod",		"modifiez"	)
	load_text( "mer",		"merci"		)
	load_text( "placez",	"placez"	)
	self.imgs = imgs

	local dir = MEDIA.get_dir().."sound/"
	local snds = {}
	local tab = { "begin", "bravo", "dommage" }
	for i=1,#tab do
		local t = tab[i]
		snds[t] = VIDEOS:get_sound( dir..t..".mp3" )
		snds[t]:play()
	end
	self.snds = snds

	local seq
 	local function add_seq( name, level_id, seq_id, update_method, comment, duration )
		local seq = seqs:add_seq( SEQ:create( name, level_id, seq_id, comment, duration ) )
		seq:set_method( "update", self, update_method,	seq )
		seq:set_ease_in_time( 1. )
		return seq
	end

	--	BEFORE
	local dur_def = 20.
	seq = add_seq( "wait",		1, 1, "update_wait",	"invite" )	--		, 1. )

	seq = add_seq( "welcome",	1, 2, "update_welcome",	"instruction", 	5 )

	seq = add_seq( "game",		1, 3, "update_game", 	"game", 		40 )
		seq.game_ph = 0
		seq:set_method( "set_active", self, "set_active_game" )

	seq = add_seq( "video",		1, 4, "update_video",	"video" )

	seq = add_seq( "end",		1, 5, "update_end", 	"end",			5 )

	seqs:begin( true )
end

function MDH_TOUSDIF:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	if not oo.getsuper(MDH_TOUSDIF).init_app( self, ... ) then return end
	-- we read the img to test the positions

	self.__players = { TD_PLAYER:create(1), TD_PLAYER:create(2) }
	self:set_player(1)
	self:set_level(1)

	self.skel_used = { 0, 0 }

	local mu = self:get_mu_by_name( "TdUI_1" )
	mu:set_ui_slot( 2 )

	local meus = {}
	meus[1] = self:get_meu_by_name( "Video_C" )
	meus[2] = self:get_meu_by_name( "Video_D" )
	self.meu_videos = meus

	self:define_seqs()

	ga:add_ga_and_bu_windows( nil, { "SEQS", "SEQ", "PLAYLIST" } )

	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( MDH_TOUSDIF )
end


