aaa.lua.global.declare_table( "race_def" )
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

race_def.meduse =
{
	name = "meduse",
	elt_def =
	{
		{
			model = "shark/shark.obj",	--in==unused for now
			maps =	{ "shark/grey_nurse_shark_correct2.jpg",	},
			vertex = 1,
			pixel = 1,
		},
		{
			maps =	{ "Maa/pal.png",	},
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
			maps =	{ "shark/grey_nurse_shark_correct2.jpg", "Maa/pal.png", },
			vertex = 1,
			pixel = 33,
		},
		{
			maps =	{ "shark/grey_nurse_shark_correct2.jpg",	},
			vertex = 1,
			pixel = 33,
		},
		{
			maps =	{ "shark/grey_nurse_shark_correct2.jpg",	},
			vertex = 1,
			pixel = 33,
		},
		{
			maps =	{ "shark/grey_nurse_shark_correct2.jpg",	},
			vertex = 1,
			pixel = 33,
		},
	}
}