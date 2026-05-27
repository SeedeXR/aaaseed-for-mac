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

--local	PIX_MEDUSE_INT	= 3
--local	PIX_MEDUSE_MID	= 4

local	PIX_ANGLER		= 6

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
			-- model = "maquereau/maquereau_BD.obj",
			-- maps =	{
			-- 			--"maquereau/maquereau_diffus_1.tga",
			-- 			{
			-- 			"maquereau/maquereau_diffus_bleu.tga",
			-- 			"maquereau/maquereau_diffus_turquoise.tga",
			-- 			"maquereau/maquereau_diffus_vert.tga",
			-- 			"maquereau/maquereau_diffus_orange.tga"
			-- 			},
			-- 			"maquereau/maquereau_masque.tga",	"generique/fondBleu.jpg",	"caustic"		},
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
			-- model = "maquereau/maquereau_BD.obj",
			-- maps =	{
			-- 			"maquereau/maquereau_diffus_bleu.tga",
			-- 			"maquereau/maquereau_masque.tga",	"generique/fondBleu.jpg", "caustic"		},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
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
			-- model = "poissonLune/poissonLune_TTBD.obj",
			-- maps =	{
			-- 			--"poissonLune/poissonLune_diffus_jaune.tga",
			-- 			{
			-- 			"poissonLune/poissonLune_diffus_jaune.tga",
			-- 			"poissonLune/poissonLune_diffus_rouge.tga",
			-- 			"poissonLune/poissonLune_diffus_bleu.tga",
			-- 			"poissonLune/poissonLune_diffus_vert.tga",

			-- 			},
			-- 			"poissonLune/poissonLune_masque.tga", "generique/fondBleu.jpg",	"caustic",
			-- 		},
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
			-- model = "poissonLune/poissonLune_TTBD.obj",
			-- maps =	{
			-- 			--"poissonLune/poissonLune_diffus_jaune.tga",
			-- 			{
			-- 			"poissonLune/poissonLune_diffus_kandinski.tga",
			-- 			"poissonLune/poissonLune_diffus_kandinski.tga",
			-- 			"poissonLune/poissonLune_diffus_basquiat.tga",
			-- 			"poissonLune/poissonLune_diffus_basquiat.tga",
			-- 			"poissonLune/poissonLune_diffus_leopard.tga",
			-- 			"poissonLune/poissonLune_diffus_leopard.tga",
			-- 			"poissonLune/poissonLune_diffus_maa.tga",
			-- 			"poissonLune/poissonLune_diffus_maa.tga",
			-- 			"poissonLune/poissonLune_diffus_humide.tga",
			-- 			},
			-- 			"poissonLune/poissonLune_masque.tga", "generique/fondBleu.jpg",	"caustic",
			-- 		},
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
	morph =	{	{ freq = 3., amp = 1., mod_freq = 1., mod_amp = 3. },
				{ freq = 1., amp = .5, mod_freq = 3., mod_amp = 2. },
			},
	elt_def =
	{
		{
			-- model = "thon/thon.obj",
			-- maps =	{ "thon/thon_diffus.tga","thon/thon_masque.tga",  "generique/fondBleu.jpg",	"caustic"	},
			vertex = VER_PM,
			pixel = PIX_ANGLER,
		},
	}
}

