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

--test DEF
local size_def = { min=1., max=1. }

local MAT_BOID_DEF = 40
local MAT_LESS_SHINY = 41
local MAT_MIN_SHINY_TIGER = 42

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
--	poissonLune
--
race_def.poissonLune =
{
	name = "poissonLune",
	size = { min = .5 },
	tail = { begin = .6, amp = .3, freq = 10. },
	clip_dist = .1,
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

race_def.B_ANCHOVY =
{
	name = "B_ANCHOVY",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "ANCHOVY",	model = "ANCHOVY",
			maps = {},
			b_skip_anim = true
		},
	}
}

race_def.B_BANNER =
{
	name = "B_BANNER",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2. },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "BANNER",	model = "BANNER",
			maps = {},
		},
	}
}

race_def.B_BARAK =
{
	name = "B_BARAK",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 0.3, max= 0.5 },
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "BARAK",	model = "BARAK",
			maps = {},
		},
	}
}

race_def.B_BAT =
{
	name = "B_BAT",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
	curvature = { limit=true, turn_by_sec=.5 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "BAT",	model = "BAT",
			maps = {},
		},
	}
}


race_def.B_BUTTER =
{
	name = "B_BUTTER",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "BUTTER",	model = "BUTTER",
			maps = {},
		},
	}
}

race_def.B_CARANG =
{
	name = "B_CARANG",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 0.6, max= 0.7 },
	len_to_anim = { min = 1., max = 2, duration=2. },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "CARANG",	model = "CARANG",
			material_front = MAT_LESS_SHINY,
			maps = {},
		},
	}
}

race_def.B_CARPET =
{
	name = "B_CARPET",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 0.4, max= 0.6 },
	len_to_anim = { min = 1., max = 2, duration=2. },
	curvature = { limit=true, turn_by_sec=.3 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "CARPET",	model = "CARPET",
			maps = {},
		},
	}
}

race_def.B_CLEAN =
{
	name = "B_CLEAN",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 1., max= 1.2 },
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "CLEAN",	model = "CLEAN",
			maps = {},
		},
	}
}

race_def.B_CLOWN =
{
	name = "B_CLOWN",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 4, max= 5 },
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "CLOWN",	model = "CLOWN",
			maps = {},
		},
	}
}

race_def.B_CONVICT =
{
	name = "B_CONVICT",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1, max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "CONVICT",	model = "CONVICT",
			maps = {},
		},
	}

}

race_def.B_CORNET =
{
	name = "B_CORNET",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 0.6, max= 0.8 },
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "CORNET",	model = "CORNET",
			maps = {},
		},
	}
}

race_def.B_CORNU =
{
	name = "B_CORNU",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "CORNU",	model = "CORNU",
			maps = {},
		},
	}
}

race_def.B_CUTTLE =
{
	name = "B_CUTTLE",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2. },
	curvature = { limit=true, turn_by_sec=.5 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "CUTTLE",	model = "CUTTLE",
			maps = {},
		},
	}
}

race_def.B_DARK_GUN =
{
	name = "B_DARK_GUN",	--todo check what this repeat mean
	b_fbx = true,
	size = { min = .6, max = .7 },
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "DARK_GUN",	model = "DARK_GUN",
			maps = {},
		},
	}
}

race_def.B_DOLPH =
{
	name = "B_DOLPH",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= .6, max= .8 },
	len_to_anim = { min = 0.5, max = 1 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "DOLPH",	model = "DOLPH",
			maps = {},
		},
	}
}

race_def.B_EAGLE =
{
	name = "B_EAGLE",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= .7, max= .8 },
	len_to_anim = { min = .25, max = .4 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "EAGLE",	model = "EAGLE",
			material_front = MAT_LESS_SHINY,
			maps = {},
		},
	}
}

race_def.B_EMPEROR =
{
	name = "B_EMPEROR",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "EMPEROR",	model = "EMPEROR",
			maps = {},
		},
	}
}

race_def.B_FLASH =
{
	name = "B_FLASH",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "FLASH",	model = "FLASH",
			maps = {},
		},
	}
}

race_def.B_GRACE =
{
	name = "B_GRACE",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "GRACE",	model = "GRACE",
			maps = {},
		},
	}
}

race_def.B_GUN =
{
	name = "B_GUN",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 0.6, max= 0.7 },
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "GUN",	model = "GUN",
			maps = {},
		},
	}
}

race_def.B_HACHETTE =
{
	name = "B_HACHETTE",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "HACHETTE",	model = "HACHETTE",
			maps = {},
		},
	}
}

race_def.B_LADY_BL_M =
{
	name = "B_LADY_BL_M",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "LADY_BL",	model = "LADY_BL_M",
			maps = {},
		},
	}
}

race_def.B_LADY_BL_F =
{
	name = "B_LADY_BL_F",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "LADY_BL",	model = "LADY_BL_F",
			maps = {},
		},
	}
}

race_def.B_LADY_GREEN =
{
	name = "LADY_GREEN",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "LADY_GREEN",	model = "LADY_GREEN",
			maps = {},
		},
	}
}

race_def.B_LION =
{
	name = "LION",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 1.7, max= 2.0 },
	len_to_anim = { min = 1., max = 2 },
	curvature = { limit=true, turn_by_sec=.5 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "LION",	model = "LION",
			maps = {},
		},
	}
}

race_def.B_LIPS_A =
{
	name = "LIPS_A",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "LIPS_A",	model = "LIPS_A",
			maps = {},
		},
	}
}

race_def.B_LIPS_B =
{
	name = "LIPS_B",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "LIPS_B",	model = "LIPS_B",
			maps = {},
		},
	}
}

race_def.B_LUTJ =
{
	name = "B_LUTJ",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "LUTJ",	model = "LUTJ",
			maps = {},
		},
	}
}

race_def.B_MANTA =
{
	name = "B_MANTA",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "MANTA",	model = "MANTA",
			maps = {},
		},
	}
}

race_def.B_MAORI =
{
	name = "B_MAORI",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 1.1, max= 1.4 },
	len_to_anim = { min = 1., max = 2 },
	curvature = { limit=true, turn_by_sec=.5 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "MAORI",	model = "MAORI",
			material_front = MAT_LESS_SHINY,
			maps = {},
		},
	}
}

race_def.B_PARROT =
{
	name = "B_PARROT",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 0.7, max= 0.8 },
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "PARROT",	model = "PARROT",
			maps = {},
		},
	}
}

race_def.B_PARROT_BL_F =
{
	name = "B_PARROT_BL_F",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 0.7, max= 0.8 },
	len_to_anim = { min = 1., max = 2 },
	curvature = { limit=true, turn_by_sec=.5 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "PARROT_BL_F",	model = "PARROT_BL_F",
			maps = {},
		},
	}
}

race_def.B_PARROT_BL_M =
{
	name = "B_PARROT_BL_M",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 0.7, max= 0.8 },
	len_to_anim = { min = 1., max = 2 },
	curvature = { limit=true, turn_by_sec=.5 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "PARROT_BL_M",	model = "PARROT_BL_M",
			maps = {},
		},
	}
}

race_def.B_PELAGIA =
{
	name = "B_PELAGIA",	--todo check what this repeat mean
	b_fbx = true,
	transparent_render = { type="plaque", sx=1, sy=1.2, img_nb=150 },
	size = { min= .5, max= 1. },
	len_to_anim = { min = .5, max = 1. },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "PELAGIA",	model = "PELAGIA", meu_name = "PELAGIA",
			maps = {},
		},
	}
}

race_def.B_POTATO =
{
	name = "B_POTATO",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
	curvature = { limit=true, turn_by_sec=.33 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "POTATO",	model = "POTATO",
			maps = {},
		},
	}
}

race_def.B_RED_F =
{
	name = "B_RED_F",	--todo check what this repeat mean
	b_fbx = true,
	size =  { min= 2.0, max= 2.2 },
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "RED_F",	model = "RED_F",
			maps = {},
		},
	}
}

race_def.B_RED_M =
{
	name = "B_RED_M",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 1.5, max= 1.7 },
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "RED_M",	model = "RED_M",
			maps = {},
		},
	}
}

race_def.B_SHRK_WHAL =
{
	name = "B_SHRK_WHAL",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 0.6, max= 0.7 },
	len_to_anim = { min = 0.9, max = 1 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "SHRK_WHAL",	model = "SHRK_WHAL",
			maps = {},
		},
	}
}

race_def.B_SUCKER =
{
	name = "B_SUCKER",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "SUCKER",	model = "SUCKER",
			maps = {},
		},
	}
}

race_def.B_SURGERY =
{
	name = "B_SURGERY",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 1.2, max= 1.4 },
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "SURGERY",	model = "SURGERY",
			maps = {},
		},
	}
}

race_def.B_TIGER =
{
	name = "B_TIGER",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 1.4, max= 1.6 },
	len_to_anim = { min = 0.3, max = 0.9 },
	--tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "TIGER",	model = "TIGER",
			material_front = MAT_MIN_SHINY_TIGER,
			maps = {},
		},
	}
}

race_def.B_TITAN =
{
	name = "B_TITAN",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "TITAN",	model = "TITAN",
			maps = {},
		},
	}
}

race_def.B_TRAVEL =
{
	name = "B_TRAVEL",	--todo check what this repeat mean
	b_fbx = true,
	size = { min=.20, max=.25 },
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "TRAVEL",	model = "TRAVEL",
			maps = {},
		},
	}
}

race_def.B_TRIGGER =
{
	name = "B_TRIGGER",	--todo check what this repeat mean
	b_fbx = true,
	size =  { min= 2.4, max= 2.8 },
	len_to_anim = { min = 1., max = 2 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "TRIGGER",	model = "TRIGGER",
			maps = {},
		},
	}
}

race_def.B_TURTL_BB =
{
	name = "B_TURTL_BB",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 2.4, max= 2.8 },
	len_to_anim = { min = 10., max = 16. },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "TURTL_BB",	model = "TURTL_BB",
			maps = {},
		},
	}
}

race_def.B_TURTL =
{
	name = "B_TURTL",	--todo check what this repeat mean
	b_fbx = true,
	size = size_def,
	len_to_anim = { min = 1.3, max = 1.5 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "TURTL",	model = "TURTL",
			maps = {},
		},
	}
}

race_def.B_WHITETIP =
{
	name = "B_WHITETIP",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 3.5, max= 3.8 },
	len_to_anim = { min = 1., max = 2 },
	curvature = { limit=true, turn_by_sec=.5 },

--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "WHITETIP_BB",	model = "WHITETIP_BB",
			material_front = MAT_LESS_SHINY,
			maps = {},
		},
	}
}

race_def.B_WHITETIP_BB =
{
	name = "B_WHITETIP_BB",	--todo check what this repeat mean
	b_fbx = true,
	size = { min= 1.2, max= 1.3 },
	len_to_anim = { min = 1., max = 2 },
	curvature = { limit=true, turn_by_sec=.5 },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "WHITETIP_BB",	model = "WHITETIP_BB",
			material_front = MAT_LESS_SHINY,
			maps = {},
		},
	}
}

race_def.B_SIMPLE =
{
	name = "B_SIMPLE",	--todo check what this repeat mean
	b_fbx = true,
	size = { min = .1, max = 1.5 },
	len_to_anim = { min = 1., max = 2. },
--	tail = { begin = .7, amp = .2, freq = 10 },
--	clip_dist = .1,
	elt_def =
	{
		{
			model_path = "test/FISH_Simple",	model = "FISH_Simple", meu_name = "SIMPLE",
			maps = {},
		},
	}
}
