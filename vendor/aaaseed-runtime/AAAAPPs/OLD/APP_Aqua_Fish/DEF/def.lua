--
--	we define the race here
--		they can be used or not
--
local race_def = RACES.get_race_def()

--local	VER_STD			= 1
--local	VER_BIOLUM		= 2
local	VER_PM			= 5
local	VER_BLOW		= 9
local	VER_MIN			= 17

--local	PIX_STD			= 1
local	PIX_LIGHT		= 2

--local	PIX_MEDUSE_INT	= 3
--local	PIX_MEDUSE_MID	= 4
local	PIX_MEDUSE_EXT	= 5

local	PIX_ANGLER		= 6
local	PIX_MIN			= 17

--
--	anglerfish
--
race_def.anglerfish =
{
	name = "anglerfish",
	elt_def =
	{
		--corps
		{
			model = "anglerfish/anglerfish.obj",
			maps =	{ "anglerfish/anglerfish_diffus.tga", "anglerfish/anglerfish_masque.tga",   "generique/fondBleu.jpg" },
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
		--lumiere
		{
			model = "anglerfish/anglerfish_light.obj",
			vertex = 2,
			pixel = PIX_LIGHT,
		},
	}
}

--
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
			vertex = 1,
			pixel = 1,
		},
		--peau intermediaire
		{
			model = "bathykorus/bathykorus_2.obj",
			maps =	{ "bathykorus/bathykorus_2_AO.tga", "bathykorus/bathykorus_2_UV.tga"	},
			vertex = 1,
			pixel = 1,
		},
		--exterieur
		{
			model = "bathykorus/bathykorus_3.obj",
			maps =	{ "bathykorus/bathykorus_3_AO.tga", "bathykorus/bathykorus_3_UV.tga"	},
			vertex = 1,
			pixel = 1,
		},
	}
}

--
--	cachalot
--
race_def.cachalot =
{
	name = "cachalot",
	tail = { begin = .3, amp = .15, vert = true, freq = .3 },
	clip_dist = 5.,
	elt_def =
	{
		--corps
		{
			model = "cachalot/cachalot.obj",
			maps =	{ "cachalot/cachalot_diffus.tga","cachalot/cachalot_masque.tga", "generique/fondBleu.jpg",	"caustic" },
--			vertex = VER_MIN,
--			pixel = PIX_MIN,
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
		--oeil
		{
			model = "cachalot/cachalot_oeil.obj",
			maps =	{ "cachalot/cachalot_oeil.tga","generique/blanc.tga", "generique/fondBleu.jpg",	"caustic" },
--			vertex = VER_MIN,
--			pixel = PIX_MIN,
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
	}
}

--
--	hippocampe
--
race_def.hippocampe =
{
	name = "hippocampe",
	elt_def =
	{
		{
			model = "hippocampe/hippocampe.obj",
			maps =	{ "hippocampe/hippocampe_diffus.tga",	"hippocampe/hippocampe_masque.tga",	"generique/fondAbysses.jpg", "hippocampe/hippocampe_masque_anim.tga"},
			vertex = 2,
			pixel = 5,
		},
	}
}

--
--	base
--
race_def.base =
{
	name = "base",
	size = { min = .7 },
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
	size = { min = .7 },
	tail = { begin = .7, amp = .2, freq = 10 },
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
--	maquereau
--
local clip_dist_maquereau = 0.1
race_def.maquereau_mix =
{
	name = "maquereau_mix",
	size = { min = .2 },
	tail = { begin = .7, amp = .2, freq = 10 },
	clip_dist = clip_dist_maquereau,
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
						"maquereau/maquereau_masque.tga",	"generique/fondBleu.jpg",	"caustic"		},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
	}
}

--
--	MONO
--
race_def.maquereau_bleu =
{
	name = "maquereau_bleu",
	size = { min = .3, max = .6 },
	tail = { begin = .7, amp = .2, freq = 10 },
	clip_dist = clip_dist_maquereau,
	elt_def =
	{
		{
			model = "maquereau/maquereau_BD.obj",
			maps =	{
						"maquereau/maquereau_diffus_bleu.tga",
						"maquereau/maquereau_masque.tga",	"generique/fondBleu.jpg", "caustic"		},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
	}
}
race_def.maquereau_turquoise =
{
	name = "maquereau_turquoise",
	size = { min = .6 },
	tail = { begin = .7, amp = .2, freq = 10 },
	clip_dist = clip_dist_maquereau,
	elt_def =
	{
		{
			model = "maquereau/maquereau_BD.obj",
			maps =	{
						"maquereau/maquereau_diffus_turquoise.tga",
						"maquereau/maquereau_masque.tga",	"generique/fondBleu.jpg", "caustic"		},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
	}
}
race_def.maquereau_vert =
{
	name = "maquereau_vert",
	size = { min = .6 },
	tail = { begin = .7, amp = .2, freq = 10 },
	clip_dist = clip_dist_maquereau,
	elt_def =
	{
		{
			model = "maquereau/maquereau_BD.obj",
			maps =	{
						"maquereau/maquereau_diffus_vert.tga",
						"maquereau/maquereau_masque.tga",	"generique/fondBleu.jpg", "caustic"		},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
	}
}
race_def.maquereau_orange =
{
	name = "maquereau_orange",
	size = { min = .6 },
	tail = { begin = .7, amp = .2, freq = 10 },
	clip_dist = clip_dist_maquereau,
	elt_def =
	{
		{
			model = "maquereau/maquereau_BD.obj",
			maps =	{
						"maquereau/maquereau_diffus_orange.tga",
						"maquereau/maquereau_masque.tga",	"generique/fondBleu.jpg", "caustic"		},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
	}
}

--
--	meduse
--
race_def.meduse =
{
	name = "meduse",
	time_factor = 1.;
	elt_def =
	{
		{
			model = "meduse/meduse_1.obj",
			maps =	{ "meduse/meduse_2_AO.tga","generique/noir.tga","generique/fondAbysses.jpg", "generique/stars2.jpg",	},
			vertex = 2,
			pixel = 3,
		},
		{
			model = "meduse/meduse_2.obj",
			maps =	{ "meduse/meduse_2_AO.tga","generique/noir.tga","generique/fondAbysses.jpg", "generique/stars2.jpg",	},
			vertex = 2,
			pixel = PIX_MEDUSE_EXT,
		},
		{
			model = "meduse/meduse_3.obj",
			maps =	{ "meduse/meduse_3_diffus.tga","meduse/meduse_3_masque.tga","generique/fondAbysses.jpg", "meduse/meduse_3_masque.tga"},
			vertex = 2,
			pixel = PIX_MEDUSE_EXT,
		},
	}
}

--
--	poissonLune
--
local clip_dist_lune = 0.1
race_def.poissonLune =
{
	name = "poissonLune",
	size = { min = .5 },
	tail = { begin = .6, amp = .3, freq = 10. },
	clip_dist = clip_dist_lune,
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
						"poissonLune/poissonLune_masque.tga", "generique/fondBleu.jpg",	"caustic",
					},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
	}
}
race_def.poissonLune_rare =
{
	name = "poissonLune_rare",
	size = { min = .5 },
	tail = { begin = .6, amp = .3, freq = 10 },
	clip_dist = clip_dist_lune,
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
						"poissonLune/poissonLune_diffus_humide.tga",
						},
						"poissonLune/poissonLune_masque.tga", "generique/fondBleu.jpg",	"caustic",
					},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
	}
}
race_def.poissonLune_kid =
{
	name = "poissonLune_kid",
	morph = { { type = "kid" } },
	size = { min = .8 },
	tail = { begin = 1.4, amp = .2, freq = 4 },
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
						"poissonLune/poissonLune_masque.tga", "generique/fondBleu.jpg",	"caustic",
					},
			vertex	=	VER_BLOW,
			pixel	=	PIX_ANGLER,
		},
	}
}
race_def.poissonLune_right =
{
	name = "poissonLune_right",
	size = { min = .02, max = .08 },
	tail = { begin = .7, amp = .2, freq = 4 },
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
						"poissonLune/poissonLune_masque.tga", "generique/fondBleu.jpg",	"caustic",
					},
			vertex	=	VER_BLOW,
			pixel	=	PIX_ANGLER,
		},
	}
}
--
-- raie
--
race_def.raie =
{
	name = "raie",
	time_factor = 1.;
	elt_def =
	{
		{
			model = "raie/raie.obj",
			maps =	{ "raie/raie_diffus_b.tga","raie/raie_masque_b.tga",  "generique/fondBleu.jpg", "caustic"	},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
	}
}

--
--	requin
--
race_def.requin =
{
	name = "requin",
	morph = { { type = "requin", freq = 3., amp = 1., mod_freq = 1., mod_amp = 3. } },
	size = { min = .8 },
	tail = { begin = .6, amp = .3, freq = 3 },
	clip_dist = .3,
	elt_def =
	{
		{

			model = "requin/requin.obj",
			maps =	{ "requin/requin_diffus.tga","requin/requin_masque.tga",  "generique/fondBleu.jpg",	"caustic"	},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
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
			vertex = 2,
			pixel = PIX_MEDUSE_EXT,
		},
	}
}

--
--	sunfish
--
race_def.sunfish =
{
	name = "sunfish",
	tail = { begin = .8, amp = .15, freq = 5 },
	clip_dist = .5,
	elt_def =
	{
		{
			model = "sunfish/sunfish.obj",
			maps =	{ "sunfish/sunfish_diffus.tga","sunfish/sunfish_masque.tga",	"generique/fondBleu.jpg",	"caustic"	},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
	}
}

--
-- thon
--
race_def.thon =
{
	name = "thon",
	time_factor = 1.;
	tail = { begin = .7, amp = .25, freq = .8 },
	clip_dist = .3,
	morph = {
				{ freq = 3., amp = 1., mod_freq = 1., mod_amp = 3. },
				{ freq = 1., amp = .5, mod_freq = 3., mod_amp = 2. },
			},
	elt_def =
	{
		{
			model = "thon/thon.obj",
			maps =	{ "thon/thon_diffus.tga","thon/thon_masque.tga",  "generique/fondBleu.jpg",	"caustic"	},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
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
race_def.aaaref =
{
	name = "aaaref",
	elt_def =
	{
		{
			--tail = .7,	?????
			maps =	{ "shark/grey_nurse_shark_correct2.jpg", "Maa/pal.png", },
			vertex = 1,
			pixel = 1,
		},
		{
			maps =	{ "shark/grey_nurse_shark_correct2.jpg",	},
			vertex = 1,
			pixel = 1,
		},
		{
			maps =	{ "shark/grey_nurse_shark_correct2.jpg",	},
			vertex = 1,
			pixel = 1,
		},
		{
			maps =	{ "shark/grey_nurse_shark_correct2.jpg",	},
			vertex = 1,
			pixel = 1,
		},
	}
}
race_def.test =
{
	name = "test",
	elt_def =
	{
		{
			maps =	{ "Maa/pal.png", },
			vertex = 1,
			pixel = 1,
		},
	}
}
