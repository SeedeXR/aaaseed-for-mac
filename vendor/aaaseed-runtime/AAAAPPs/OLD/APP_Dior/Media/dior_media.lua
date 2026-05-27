function DIOR:set_media_dir()
	self.__image_dir = "Images/"
	self.__mask_dir = "Masques/"
	MEDIA.set_dir_media( "Dior/"..app:get_experience_dir() )
end

--todo generalize
--[[
function DIOR:load_image_one( imgs, key, dir, name )
	name = dir.."/"..name
	local img = IMGS.get_img_using_ext( name, { "png", "tga", "jpg" } )
	if img then
		imgs[key] = img
	end
	return img
end

function DIOR:load_images( imgs, dir, tab )
	imgs = imgs or {}
	for i=1,#tab,2 do
		self:load_image_one( imgs, tab[i], dir, tab[i+1] )
	end
	return imgs
end
function DIOR:load_images_helper( imgs, dir, name )
	return self:load_images( imgs, dir,
									{
										"title", 		name.."_title",
										"title_sub",	name.."_title_sub",
--										"footer", 		name.."_footer",
									} )
end
--]]

function DIOR:make_name_image( name )
	return MEDIA.make_fname( self.__image_dir..name )
end

function DIOR:create_anim( group_name, seq_name, fname, ... )
	local t
	if group_name then
		t = self:get_table_always( group_name )
	end
	seq_name = string.lower(seq_name)
	local seq_img = IMG_SEQ:create( seq_name, fname,	"tga", ... )
	local player = IMG_SEQ_PLAYER:create( seq_name, seq_img )
	if t then t[seq_name] = player end
	return player
end
function DIOR:create_anim_lang( group_name, seq_name, fname, ... )
	local pre = self:get_lang_dir()
	return self:create_anim( group_name, seq_name, pre..fname, ... )
end

function DIOR:load_anim_lang_dsa()
	self.please				=	self:create_anim_lang(	nil,		"Please",	"INTRO_PLEASE/INTRO_PLEASE_"		)

	local anim				=	self:create_anim_lang(	"typo",		"wait",		"INTRO_START/INTRO_START_"			)
	self.typo.welcome		=	anim
								self:create_anim_lang(	"typo",		"texture",	"TYPO_TEXTURE/TYPO_TEXTURE_"		)
								self:create_anim_lang(	"typo",		"color",	"TYPO_COLOR/TYPO_COLOR_"			)
								self:create_anim_lang(	"typo",		"light",	"TYPO_LIGHT/TYPO_LIGHT_"			)
								self:create_anim_lang(	"typo",		"end",		"TYPO_FIN/TYPO_FIN_"				)

								self:create_anim_lang(	"typo_b",	"end",		"TYPO_YOU/TYPO_YOU_"				)
end
function DIOR:load_anim_dsa()
								self:create_anim(		"face_on",	"texture",	"INTRO_TEXTURE/INTRO_TEXTURE_"		)
								self:create_anim(		"face_on",	"color",	"INTRO_COLOR/INTRO_COLOR_" 			)
								self:create_anim(		"face_on",	"light",	"INTRO_LIGHT/INTRO_LIGHT_"			)

	self.oval				=	self:create_anim(		nil,		"Oval",		"INTRO_OVAL/INTRO_OVAL_"			)

								self:create_anim(		"palette",	"texture",	"PALETTE_TEXTURE/PALETTE_TEXTURE_"	)
								self:create_anim(		"palette",	"color",	"PALETTE_COLOR/PALETTE_COLOR_"		)
								self:create_anim(		"palette",	"light",	"PALETTE_LIGHT/PALETTE_LIGHT_"		)
end

function DIOR:load_anim_lang_hl()
								self:create_anim_lang(	"typo",		"wait",		"INTRO_PLEASE/INTRO_PLEASE_"		)
								self:create_anim_lang(	"typo",		"welcome",	"INTRO_START/INTRO_START_"			)
	local anim =				self:create_anim_lang(	"typo",		"sorbet",	"TYPO_MASQUE/TYPO_MASQUE_" 			)
	self.typo.nature_a		=	anim
	self.typo.nature_b		=	anim
	self.typo.nature_c		=	anim
	self.typo["end"]		=	anim
--							self:create_anim_lang( "typo_b",	"end",		"TYPO_YOU/TYPO_YOU_"				)
end

function DIOR:load_anim_hl()
								self:create_anim(		"face_on",	"welcome",	"SCAN_VISAGE/SCAN_VISAGE_"					)
								self:create_anim(		"face_on",	"sorbet",	"MASQUE_HYDRATATION/MASQUE_HYDRATATION_"	)
	local anim				=	self:create_anim(		"face_on",	"nature_a",	"MASQUE_NATURE/MASQUE_NATURE_"				)
	self.face_on.nature_b	=	anim
	self.face_on.nature_c	=	anim

	self.oval				=	self:create_anim(		nil,		"Oval",		"INTRO_OVAL/INTRO_OVAL_"					)

	self.tracker			=	self:create_anim(		nil,		"Tracker",	"TRACKER/TRACKER_"							)

								self:create_anim(		"palette",	"sorbet",	"PALETTE_HYDRATATION/PALETTE_HYDRATATION_"	)
								self:create_anim(		"palette",	"nature_a",	"PALETTE_MASK_A/PALETTE_MASK_A_"			)
								self:create_anim(		"palette",	"nature_b",	"PALETTE_MASK_B/PALETTE_MASK_B_"			)
								self:create_anim(		"palette",	"nature_c",	"PALETTE_MASK_C/PALETTE_MASK_C_"			)
end

function DIOR:load_anim()		self["load_anim_"..string.lower(app:get_experience_code())](self)	end
function DIOR:load_anim_lang()	self["load_anim_lang_"..string.lower(app:get_experience_code())](self)	end
function DIOR:load_anim_all()
	self:load_anim()
	self:load_anim_lang()
end

function DIOR:get_img_fix( id )
	local img_fix = self.img_fix
	if img_fix then
		return img_fix[id]
	end
end

function DIOR:load_imgs_lang()
	local lang = self:get_lang_dir()
	self.img_top[2] 	=	IMGS.get_img_with_error_dialog( lang.."LOGO_HAUT.tga" 	)
	self.img_packshot	=	IMGS.get_img_with_error_dialog( lang.."PACKSHOT.tga"		)
	self.img_with		=	{	IMGS.get_img_with_error_dialog( lang.."WITHOUT.tga" ),	IMGS.get_img_with_error_dialog( lang.."WITH.tga" )			}
end

function DIOR:load_imgs()
	local exp = app:get_experience_code()
	if exp=="DSA" then
		self.img_calage	= { IMGS.get_img_with_error_dialog( "gabarit_19042016.jpg" ) }
	end

	local img = self.__image_dir
	self.img_top = {}
	self.img_top[1]		= IMGS.get_img_with_error_dialog( img.."MASQUE_HAUT.tga"	)

--	self:box_warning( "jklj" )
--	self.img_bottom 	= IMGS.get_img_with_error_dialog( "TEXTE_BAS.tga"		)
	if exp=="DSA" then
		self.img_tracker	= IMGS.get_img_with_error_dialog( img.."CROIX.tga"			)
	end

	local pre = self.__mask_dir
	local imgs = {}
	imgs[1] = IMGS.get_img_with_error_dialog( pre.."MASQUES_Maa_3_correct.tga"	)
	imgs[2]	= IMGS.get_img_with_error_dialog( pre.."face_calage.jpg"			)
	imgs[3] = IMGS.get_img_with_error_dialog( pre.."MAILLAGE_SOURCE_FIN.tga"	)
	imgs[4] = IMGS.get_img_with_error_dialog( pre.."MAILLAGE_SOURCE_LARGE.tga"	)
	imgs[5] = IMGS.get_img_with_error_dialog( pre.."masques_B.tga"				)
	imgs[6] = IMGS.get_img_with_error_dialog( pre.."visage_eau.tga"			)
	self.img_fix = imgs
end

function DIOR:load_medias()
	local exp = app:get_experience_code()
	self.fps_video = exp=="DSA" and 25 or 30
	local str = "DIOR:load_medias() with experience code : "..exp
	self:print_inverse( str )
	--aaa.box_warning( str )

	aaa.img.set_free_when_on_board( true )
	self:update_lang()
	self:set_media_dir()
	self:load_imgs()
	self:load_imgs_lang()
end


