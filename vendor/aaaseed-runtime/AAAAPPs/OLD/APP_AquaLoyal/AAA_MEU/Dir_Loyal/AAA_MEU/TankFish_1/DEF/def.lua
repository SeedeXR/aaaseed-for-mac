--
--	we define the race here
--		they can be used or not
--
local race_def = RACES.get_race_def()

local	VER_STD			= 1
local	VER_BIOLUM		= 2
local	VER_PM			= 5
local	VER_BLOW		= 9
local	VER_MIN			= 17

local	PIX_STD			= 1
local	PIX_LIGHT		= 2

--local	PIX_MEDUSE_INT	= 3
--local	PIX_MEDUSE_MID	= 4
local	PIX_MEDUSE_EXT	= 5

local	PIX_ANGLER		= 6
local	PIX_MIN			= 17

local	VERT_LOYAL_V1	= 5
local	FRAG_LOYAL_v1	= 6

--	THESE ARE NOT USABLE FOR LOYAL FOR NOW
--[[
--	bathykorus
--
race_def.bathykorus =
{
	name = "bathykorus",
	elt_def =
	{
		--interieur
		{
			model = "bathykorus/bathykorus_1.obj",
			maps =	{ "bathykorus/bathykorus_1_AO.tga", "bathykorus/bathykorus_1_UV.tga"	},
			vertex = VER_STD,
			pixel = PIX_STD,
		},
		--peau intermediaire
		{
			model = "bathykorus/bathykorus_2.obj",
			maps =	{ "bathykorus/bathykorus_2_AO.tga", "bathykorus/bathykorus_2_UV.tga"	},
			vertex = VER_STD,
			pixel = PIX_STD,
		},
		--exterieur
		{
			model = "bathykorus/bathykorus_3.obj",
			maps =	{ "bathykorus/bathykorus_3_AO.tga", "bathykorus/bathykorus_3_UV.tga"	},
			vertex = VER_STD,
			pixel = PIX_STD,
		},
	}
}

--
--	base
--
race_def.base =
{
	name = "base",
	size_min = .7,
	size_max = 1.,
	elt_def =
	{
		{
			model = "Maa/FishBase.obj",
			maps =	{ nil, nil,	nil, nil },
			vertex = VER_MIN,
			pixel = PIX_MIN,
		},
	}
}
race_def.base3 =
{
	name = "base3",
	size_min = .7,
	size_max = 1.,
	elt_def =
	{
		{
			model = "Maa/FishBaseS3.obj",
			maps =	{ nil, nil,	nil, nil },
			vertex = VER_MIN,
			pixel = PIX_MIN,
		},
	}
}

--
--	meduse
--
race_def.meduse =
{
	name = "meduse",
	time_factor = 1.,
	elt_def =
	{
		{
			model = "meduse/meduse_1.obj",
			maps =	{ "meduse/meduse_2_AO.tga","generique/noir.tga","generique/fondAbysses.jpg", "generique/stars2.jpg",	},
			vertex = VER_BIOLUM,
			pixel = 3,
		},
		{
			model = "meduse/meduse_2.obj",
			maps =	{ "meduse/meduse_2_AO.tga","generique/noir.tga","generique/fondAbysses.jpg", "generique/stars2.jpg",	},
			vertex = VER_BIOLUM,
			pixel = PIX_MEDUSE_EXT,
		},
		{
			model = "meduse/meduse_3.obj",
			maps =	{ "meduse/meduse_3_diffus.tga","meduse/meduse_3_masque.tga","generique/fondAbysses.jpg", "meduse/meduse_3_masque.tga"},
			vertex = VER_BIOLUM,
			pixel = PIX_MEDUSE_EXT,
		},
	}
}
race_def.poissonLune_kid =
{
	name = "poissonLune_kid",
	morph = { { type = "kid" } },
	size_min = .8,
	size_max = 1.,
	tail = { begin = 1.4, amp = .2, freq = 4. },
	clip_dist = clip_dist_lune,
	elt_def =
	{
		{
			model = "poissonLune/Kid/poissonLune_HD.obj",
			maps =	{
						{
						"poissonLune/poissonLune_diffus_jaune_bis.tga",
						"poissonLune/poissonLune_diffus_jaune_bis.tga",
						"poissonLune/poissonLune_diffus_rouge_bis.tga",
						"poissonLune/poissonLune_diffus_rouge_bis.tga",
						"poissonLune/poissonLune_diffus_bleu_bis.tga",
						"poissonLune/poissonLune_diffus_bleu_bis.tga",
						"poissonLune/poissonLune_diffus_vert_bis.tga",
						"poissonLune/poissonLune_diffus_vert_bis.tga",
						"poissonLune/poissonLune_diffus_maa_bis.tga",
						},
						"poissonLune/poissonLune_masque.tga", "generique/fondBleu.jpg",
					},
			vertex	=	VER_BLOW,
			pixel	=	PIX_ANGLER,
		},
	}
}
race_def.poissonLune_right =
{
	name = "poissonLune_right",
	size_min = .02,
	size_max = .08,
	tail = { begin = .7, amp = .2, freq = 4. }
	clip_dist = clip_dist_lune,
	elt_def =
	{
		{
			model = "poissonLune/poissonLune_TTBD.obj",
			maps =	{
						{
						"poissonLune/poissonLune_diffus_jaune_bis.tga",
						"poissonLune/poissonLune_diffus_jaune_bis.tga",
						"poissonLune/poissonLune_diffus_rouge_bis.tga",
						"poissonLune/poissonLune_diffus_rouge_bis.tga",
						"poissonLune/poissonLune_diffus_bleu_bis.tga",
						"poissonLune/poissonLune_diffus_bleu_bis.tga",
						"poissonLune/poissonLune_diffus_vert_bis.tga",
						"poissonLune/poissonLune_diffus_vert_bis.tga",
						"poissonLune/poissonLune_diffus_maa_bis.tga",
						},
						"poissonLune/poissonLune_masque.tga", "generique/fondBleu.jpg",
					},
			vertex	=	VER_BLOW,
			pixel	=	PIX_ANGLER,
		},
	}
}
--
--	sternoptyx
--
race_def.sternoptyx =
{
	name = "sternoptyx",
	elt_def =
	{
		{
			model = "sternoptyx/sternoptyx.obj",
			maps =	{ "sternoptyx/sternoptyx_diffus.tga","generique/noir.tga",  "generique/fondBleu.jpg",	"sternoptyx/sternoptyx_masque.tga"	},
			vertex = VER_BIOLUM,
			pixel = PIX_MEDUSE_EXT,
		},
	}
}
--
--	tompteris
--
race_def.tompteris =
{
	name = "tompteris",
	elt_def =
	{
		{
			model = "tompteris/tompteris.obj",
			maps =	{ "tompteris/tompteris_AO.tga","tompteris/tompteris_UV.tga",	},
			vertex = 5,
			pixel = 2,
		},
	}
}
--
--	existant Maa
--
race_def.shark =
{
	name = "shark",
	elt_def =
	{
		{
			model = "shark/shark.obj",
			maps =	{ "shark/grey_nurse_shark_correct2.jpg",	},
			vertex = 1,
			pixel = 1,
		},
		{
			maps =	{ "shark/grey_nurse_shark_correct2.jpg", "Maa/pal.png",	},
			vertex = 1,
			pixel = 1,
		},
	}
}
--]]

--	anglerfish
--
race_def.anglerfish =
{
	name = "anglerfish",
	time_factor = 1.,
	tail = { begin_seg = 1., amp = .25, freq = 5 },
	clip_dist = .3,
--	touch = { type="morph", morph_id=1, asr={ .1, 1.1, 1.3 }  },
	touch = { type="morph", morph_id=1, snd="angler" },
	elt_def =
	{
		--corps
		{
			model = "anglerfish/anglerfish.obj",
			maps =	{ "anglerfish/anglerfish_diffus.tga", "anglerfish/anglerfish_AO.tga", "anglerfish/anglerfish_masque.tga" },
		},

		--lumiere
--		{
--			model = "anglerfish/anglerfish_light.obj",
--			vertex = VER_BIOLUM,
--			pixel = PIX_LIGHT,
--		},
	}
}

--	cachalot
--
race_def.cachalot =
{
	name = "cachalot",
	size = { min = .8, max = .8 },
	tail = { begin_seg = 2., amp = .7, vert = true, freq = .15 },
	clip_dist = 5.,
	touch = { type= "map", asr={ .2, 2, 3 }, snd="cachalot" },
	elt_def =
	{
		--corps
		{
			model = "cachalot/cachalot.obj",
			maps =
			{
				{
					"cachalot/cachalot_01.tga",
					"cachalot/cachalot_02.tga",
					"cachalot/cachalot_03.tga",
					"cachalot/cachalot_04.tga",
					"cachalot/cachalot_05.tga",
				},
				"cachalot/cachalot_AO.tga",
				"cachalot/cachalot_masque.tga",
				"cachalot/cachalot_touch.tga"
			},
--			vertex = VER_MIN,
--			pixel = PIX_MIN,
		},
--[[
		--oeil
		{
			model = "cachalot/cachalot_oeil.obj",
			maps =	{ "cachalot/cachalot_oeil.tga","generique/blanc.tga", "generique/fondBleu.jpg" },
--			vertex = VER_MIN,
--			pixel = PIX_MIN,
		},
--]]
	}
}

--	maquereau
--
local clip_dist_maquereau = 0.1
race_def.maquereau_mix =
{
	name = "maquereau_mix",
	size = { min = .2 },
	tail = { begin = .1, amp = .5, freq	= 3. },
	clip_dist = clip_dist_maquereau,
	touch = { type= "move_y" },
	elt_def =
	{
		{
			model = "maquereau/maquereau_BD.obj",
			maps =	{
						--"maquereau/maquereau_diffus_1.tga",
						{
						"maquereau/maquereau_diffus_bleu.tga",
						"maquereau/maquereau_diffus_turquoise.tga",
						"maquereau/maquereau_diffus_vert.tga",
						"maquereau/maquereau_diffus_orange.tga"
						},
						"maquereau/maquereau_AO.tga",	"maquereau/maquereau_masque.tga"
					},
		},
	}
}
--	 maquereau MONO
race_def.maquereau_bleu =
{
	name = "maquereau_bleu",
	size = { min = .5 },
	tail = { begin = .1, amp = .5, freq	= 3. },
	clip_dist = clip_dist_maquereau,
	touch = { type= "move_y" },
	elt_def =
	{
		{
			model = "maquereau/maquereau_BD.obj",
			maps =	{
						"maquereau/maquereau_diffus_bleu.tga",
						"maquereau/maquereau_AO.tga", "maquereau/maquereau_masque.tga"
					},
		},
	}
}
race_def.maquereau_turquoise =
{
	name = "maquereau_turquoise",
	size = { min = .6 },
	tail = { begin = .1, amp = .5, freq	= 3. },
	clip_dist = clip_dist_maquereau,
	touch = { type= "move_y" },
	elt_def =
	{
		{
			model = "maquereau/maquereau_BD.obj",
			maps =	{
						"maquereau/maquereau_diffus_turquoise.tga",
						"maquereau/maquereau_AO.tga", "maquereau/maquereau_masque.tga"
					},
		},
	}
}
race_def.maquereau_vert =
{
	name = "maquereau_vert",
	size = { min = .6 },
	tail = { begin = .1, amp = .5, freq	= 3. },
	clip_dist = clip_dist_maquereau,
	touch = { type= "move_y" },
	elt_def =
	{
		{
			model = "maquereau/maquereau_BD.obj",
			maps =	{
						"maquereau/maquereau_diffus_vert.tga",
						"maquereau/maquereau_AO.tga", "maquereau/maquereau_masque.tga"
					},
		},
	}
}
race_def.maquereau_orange =
{
	name = "maquereau_orange",
	size = { min = .6 },
	tail = { begin = .1, amp = .5, freq	= 3. },
	clip_dist = clip_dist_maquereau,
	touch = { type= "move_y" },
	elt_def =
	{
		{
			model = "maquereau/maquereau_BD.obj",
			maps =	{
						"maquereau/maquereau_diffus_orange.tga",
						"maquereau/maquereau_AO.tga", "maquereau/maquereau_masque.tga"
					},
		},
	}
}

--	poissonLune
--
local clip_dist_lune = 0.1
race_def.poissonLune =
{
	name = "poissonLune",
	size = { min = .8 },
	tail = { begin_seg = 1., amp = .5, freq = 3. },
	clip_dist = clip_dist_lune,
	touch = { type= "move_y" },
	elt_def =
	{
		{
			model = "poissonLune/poissonLune_TTBD.obj",
			maps =	{
						--"poissonLune/poissonLune_diffus_jaune.tga",
						{
						"poissonLune/poissonLune_diffus_jaune.tga",
						"poissonLune/poissonLune_diffus_rouge.tga",
						"poissonLune/poissonLune_diffus_bleu.tga",
						"poissonLune/poissonLune_diffus_vert.tga",

						},
						"poissonLune/poissonLune_AO.tga", "poissonLune/poissonLune_masque.tga"
					},
		},
	}
}
race_def.poissonLune_rare =
{
	name = "poissonLune_rare",
	size = { min = .5 },
	tail = { begin = .15, amp = .3, freq	= 3. },
	clip_dist = clip_dist_lune,
	touch = { type= "move_y" },
	elt_def =
	{
		{
			model = "poissonLune/poissonLune_TTBD.obj",
			maps =	{
						--"poissonLune/poissonLune_diffus_jaune.tga",
						{
						"poissonLune/poissonLune_diffus_kandinski.tga",
						"poissonLune/poissonLune_diffus_kandinski.tga",
						"poissonLune/poissonLune_diffus_basquiat.tga",
						"poissonLune/poissonLune_diffus_basquiat.tga",
						"poissonLune/poissonLune_diffus_leopard.tga",
						"poissonLune/poissonLune_diffus_leopard.tga",
						"poissonLune/poissonLune_diffus_maa.tga",
						"poissonLune/poissonLune_diffus_maa.tga",
						--"poissonLune/poissonLune_diffus_humide.tga",
						},
						"poissonLune/poissonLune_AO.tga", "poissonLune/poissonLune_masque.tga"
					},
		},
	}
}
race_def.poissonLuneEcran =
{
	name = "poissonLuneEcran",
	size = { min = .8 },
	tail = { begin_seg = 2., amp = .5, freq = 3. },
	clip_dist = clip_dist_lune,
--	touch = { type= "move_y" },
	elt_def =
	{
		{
			model = "poissonLune/poissonLune_TTBD.obj",
			maps =	{
						--"poissonLune/poissonLune_diffus_jaune.tga",
						{
						"poissonLune/poissonLune_diffus_jaune.tga",
						"poissonLune/poissonLune_diffus_rouge.tga",
						"poissonLune/poissonLune_diffus_bleu.tga",
						"poissonLune/poissonLune_diffus_vert.tga",
						},
						"poissonLune/poissonLune_AO.tga", "poissonLune/poissonLune_masque.tga"
					},
		},
	}
}
-- raie
--
race_def.raie =
{
	name = "raie",
	morph = { { freq=2.5, amp = 1, mod_amp = 3 } },
	time_factor = 1.,
	touch = { type= "move_y" },
	touch = { type="map", asr={ .5, 3.5, 4 }, snd="raie" },
	elt_def =
	{
		{
			model = "raie/raie.obj",
			maps =	{ "raie/raie_diffus.tga", "raie/raie_AO.tga", "raie/raie_masque.tga", "raie/raie_touch.tga"	},
		},
	}
}

--	requin
--
race_def.requin =
{
	name = "requin",
	morph = { { type = "requin", freq = 3., amp = 1., mod_freq = 1., mod_amp = 3. } },
	size = { min = .8 },
	tail = { begin_seg = 1, amp = .5, freq	= .3 },
	clip_dist = .3,
	touch = { type="morph", morph_id=1, asr={ .1, 1.1, 1.3 }, snd="requin", snd_vol=.9 },
	elt_def =
	{
		{
			model = "requin/requin.obj",
			maps =	{ "requin/requin_diffus.tga", "requin/requin_AO.tga", "requin/requin_masque.tga"	},
		},
	}
}

--	sunfish
--
race_def.sunfish =
{
	name = "sunfish",
	tail = { begin = .8, amp = .15, freq = .5 },
	clip_dist = .5,
	touch = { type = "map", asr={ .2, 2, 3 }, snd="sunfish" },
	elt_def =
	{
		{
			model = "sunfish/sunfish.obj",
			maps =	{ "sunfish/sunfish_diffus.tga", "sunfish/sunfish_AO.tga", "sunfish/sunfish_masque.tga",  "sunfish/sunfish_touch.tga"	},
		},
	}
}

-- thon
--
race_def.thon =
{
	name = "thon",
	time_factor = 1.,
	size = { min = .5, max = .8 },
	tail = { begin = .15, amp = .15, freq = 2. },
	clip_dist = .3,
	morph = {	{ freq=3, amp = 1, mod_freq = 1, mod_amp = 3 },
				{ freq=1, amp = .5, mod_freq = 3, mod_amp = 2 },
			},
	touch = { type= "map", asr={ .2, 2.2, 4.2 }, snd="slurp" },
	elt_def =
	{
		{
			model = "thon/thon.obj",
			maps =	{ "thon/thon_diffus.tga", "thon/thon_AO.tga", "thon/thon_masque.tga", "thon/thon_touch.tga"	},
		},
	}
}

--RASCASSE
--
race_def.rascasse =
{
	name = "rascasse",
	time_factor = 1.,
	tail = { begin_seg = 1.5, amp = .8, freq = 1. },
	--morph = {	{ freq = 3., amp = 1., mod_freq = 1., mod_amp = 3. },
	--         	{ freq = 1., amp = 0, mod_freq = 3., mod_amp = 2. }
	--		},
	morph = {	{ type="fix", value=0 },
				{ type="fix", value=0 }
			},
	clip_dist = .5,
	touch = { type="morph", morph_id=1, asr={ 1, 1, 4.2 }, snd="rascasse" },
	elt_def =
	{
		{
			model = "rascasse/rascasse.obj",
			maps =	{ "rascasse/rascasse_diffus.tga", "rascasse/rascasse_AO.tga", "rascasse/rascasse_masque.tga"	},
		},
	}
}

--HIPPOCAMPE
--
race_def.hippocampe =
{
	name = "hippocampe",
	clip_dist = .5,
	touch = { type= "move_y" },
	elt_def =
	{
		{
			model = "hippocampe/hippocampe.obj",
			maps =	{ "hippocampe/hippocampe_diffus.tga", "hippocampe/hippocampe_AO.tga", "hippocampe/hippocampe_masque.tga"	},
		},
	}
}

--MEDUSE
--
race_def.medusor =
{
	name = "medusor",
	len_in_curve_segment = 1,
	size = { min = .5, max = .8 },
	time_factor = 1.,
	tail = { begin = .05, amp = .5, freq	= .25 },
	morph = {	{ freq=2, amp=1, mod_freq=1, mod_amp=3 },
			--	{ freq = 1., amp = 0, mod_freq = 3., mod_amp = 2. },
			},
	clip_dist = .3,
	touch = { type="morph", morph_id=1, snd="medusor" },
	elt_def =
	{
		{
			model = "medusor/medusor.obj",
			maps =	{ "medusor/medusor_diffus.tga", "medusor/medusor_AO.tga", "medusor/medusor_masque.tga"	},
		},
	}
}

--[[
--Méduse
--
race_def.meduse =
{
	name = "meduse",
	clip_dist = .5,
	touch = { type= "move_y" },
	elt_def =
	{
		{
			model = "meduse/meduse.obj",
			maps =	{ "meduse/meduse_diffus.tga", "meduse/meduse_AO.tga", "meduse/meduse_masque.tga"	},
		},
	}
}
--]]

--PICASSO
--
race_def.picasso =
{
	name = "picasso",
	size = { min = .5, max = 1 },
	len_in_curve_segment = 3,
	time_factor = 1.,
	tail = {  begin_seg = 1.25, amp = .3, freq = 3 },
	--morph = {	{ freq = 3., amp = 1., mod_freq = 1., mod_amp = 3. },
	--			{ freq = 1., amp = .5, mod_freq = 3., mod_amp = 2. },
	--		},
	clip_dist = .5,
	touch = { type= "map", asr={ .2, 2.2, 4.2 }, snd="picasso" },
	elt_def =
	{
		{
			model = "picasso/picasso.obj",
			maps =	{ "picasso/picasso_diffus.tga", "picasso/picasso_AO.tga", "picasso/picasso_masque.tga", "picasso/picasso_touch.tga"	},
		},
	}
}

--DAUPHIN
--
race_def.dauphin =
{
	name = "dauphin",
	len_in_curve_segment = 1.2,
	clip_dist = 1.,
	tail = { begin = .5, amp = .3, vert = true, freq = 1  },
	touch = { type= "map", asr={ .5, 2, 5 }, snd="dauphin" },
	elt_def =
	{
		{
			model = "dauphin/dauphin.obj",
			maps =	{
						"dauphin/dauphin_diffus.tga",
						"dauphin/dauphin_AO.tga",
						"dauphin/dauphin_masque.tga",
						{
							"dauphin/dauphin_touch_01.tga",
							"dauphin/dauphin_touch_02.tga",
							"dauphin/dauphin_touch_03.tga",
							"dauphin/dauphin_touch_04.tga",
							"dauphin/dauphin_touch_05.tga",
							"dauphin/dauphin_touch_06.tga",
							"dauphin/dauphin_touch_07.tga",
							"dauphin/dauphin_touch_08.tga",
							"dauphin/dauphin_touch_09.tga",
							"dauphin/dauphin_touch_10.tga",
							"dauphin/dauphin_touch_11.tga",
							"dauphin/dauphin_touch_12.tga",
							"dauphin/dauphin_touch_13.tga",
							"dauphin/dauphin_touch_14.tga",
							"dauphin/dauphin_touch_15.tga",
							"dauphin/dauphin_touch_16.tga",
							"dauphin/dauphin_touch_17.tga",
							"dauphin/dauphin_touch_18.tga",
						}
					}
		},
	}
}
race_def.dauphin_bravo =
{
	name = "dauphin_bravo",
	elt_def =
	{
		{
			model = "dauphin/dauphin.obj",
			maps =	{	"dauphin/dauphin_diffus_bravo.tga", "dauphin/dauphin_AO_bravo.tga",  "dauphin/dauphin_masque_bravo.tga"	},
		},
	}
}

--TEST
--
race_def.test =
{
	name = "test",
	morph = {	{ type="fix", value = 0 },
				{ type="fix", value = 0 },
			},
	clip_dist = .5,
	touch = { type= "map" },
	elt_def =
	{
		{
			model = "test/test_V1.obj",
			maps =	{	"test/test_diffus.tga", "test/test_AO.tga", "test/test_masque.tga" },
		},
	}
}

race_def.trisec =
{
	name = "trisec",
	tail = { begin_seg = 1, amp=1, freq = 1 },
	size = { min = .125, max = 1 },
	clip_dist = .7,
--	touch = { type= "map" },
	elt_def =
	{
		{
			model = "Maa/trisecsub.obj",
		},
	}
}

--RASCASSE
--
race_def.loyal =
{
	name = "loyal",
	time_factor = 1.,
	tail = { begin_seg = 1.5, amp = .8, freq = 3. },
	clip_dist = .4,
	morph = {	{ type="mouth", value = 0 },
			},
--	touch = { type="morph", morph_id=1, asr={ 1, 1, 4.2 }, snd="rascasse" },
	elt_def =
	{
		{
			model = "loyal/loyal.obj",
			maps =	{ "loyal/loyal_diffus.tga", "loyal/loyal_masque.tga", "loyal/loyal_masque.tga",	},
		},
		{
			model = "loyal/loyal_yeux.obj",
			maps =	{ "loyal/Loyal_yeux_diffus.tga", "loyal/loyal_masque.tga", "loyal/loyal_masque.tga"	},
		},
		{
			model = "loyal/loyal_lunettes.obj",
			maps =	{ "loyal/Loyal_lunettes_diffus.tga", "loyal/loyal_masque.tga", "loyal/loyal_masque.tga"	},
		},
		{
			model = "loyal/loyal_chapeau.obj",
			maps =	{ "loyal/Loyal_chapeau_diffus.tga", "loyal/loyal_masque.tga", "loyal/loyal_masque.tga"	},
		},
	}
}


