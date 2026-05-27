if not aaa.lua.global.get( "TANK" ) then return end

--[[
function TANK:draw_race_on_curve( name, nb, curve_id, speed )
	local race = self:race_get( name )
	if race then
		race:draw_nb_on_curve( nb, curve_id, speed )
	else
		self:print_error( "race named : "..name.." don't exist do a race_add()" )
	end
end
--]]

function TANK:update_after()
end

function TANK:load_races()
	self:print_inverse( "TANK:load_races() Begin" )

	local races = self.__races
	--	create races
	if false then
		races:add( "bathykorus",			3 	)
		races:add( "Sternoptyx"				)
		races:add( "tompteris"				)
	end

	races:add( "anglerfish"				)
	races:add( "cachalot"				)
	races:add( "maquereau_mix"			)
	races:add( "maquereau_bleu"			)
	races:add( "maquereau_turquoise"	)
	races:add( "maquereau_vert"			)
	races:add( "maquereau_orange"		)
	races:add( "poissonLune"			)
	races:add( "poissonLune_rare"		)
	races:add( "poissonLuneEcran"		)
	races:add( "raie"					)
	races:add( "requin"					)
	races:add( "sunfish"				)
	races:add( "thon"					)
	races:add( "rascasse"				)
	races:add( "hippocampe"				)
	races:add( "picasso"				)
	races:add( "dauphin"				)
--	races:add( "dauphin_bravo"			)
--	races:add( "test"					)
	races:add( "medusor"				)
	races:add( "trisec"					)
	races:add( "loyal",					4	)

	self:print_inverse( "TANK:load_races() End" )
end
--	1	2	3	4	5	6	7	8
--						x	x
--	9	10	11	12	13	14	15	16
--	x	x	x	x		x
--	17	18	19	20	21	22	23	24
--		x	x	x	x	x	x	x
--	25	26	27	28	29	30	31	32
--	x	x	x	x	x	x	x
function TANK:init_fish()
	local grea
	local function add_grea(	name, race_name, nb, boid_id	)
		return self:add_grea( name, race_name, nb, boid_id	)
	end

	grea = add_grea( "Angler",			"anglerfish",			5,		26	)

	grea = add_grea( "Maq_mix",			"maquereau_mix",		100,	31	)
	grea = add_grea( "Maq_bleu",		"maquereau_bleu",		50,		30	)
	grea = add_grea( "Maq_turquoise",	"maquereau_turquoise",	100,	29	)
	grea = add_grea( "Maq_vert",		"maquereau_vert",		20,		28	)
	grea = add_grea( "Maq_orange",		"maquereau_orange",		10,		27	)

--	grea = add_grea( "requin", 			"requin",				5,		5	)
--		grea:set_curve_dist_nb( .5, 60 )
	grea = add_grea( "lune",			"poissonLune",			25,		6	)
	grea = add_grea( "lune_rare",		"poissonLune_rare",		10,		7	)

--	grea = add_grea( "Hippo",			"hippocampe",			5,		17	)
--		grea:set_curve_dist_nb( .05, 32 )
--	grea = add_grea( "Méduse",			"meduse",				5,		17	)

	grea = add_grea( "Meduse",			"medusor",				8,		19	)

	grea = add_grea( "Picasso",			"picasso",				100,	14	)
	grea = add_grea( "TriSec",			"trisec",				20,		23	)
-- 	grea = add_grea( "Test",			"test",					100,	18	)

	grea = add_grea( "rascasse",		"rascasse",				20,		20	)
	grea = add_grea( "thon", 			"thon",					25,		8	)
--		grea:set_curve_dist_nb( .15, 12 )
	grea = add_grea( "raie", 			"raie",					5,		9	)
--		grea:set_curve_dist_nb( 1.21, 10 )

	grea = add_grea( "dauphin", 		"dauphin",				10,		16	)
--	grea = add_grea( "dauphin_bravo", 	"dauphin_bravo",		10,		22	)

	grea = add_grea( "ecran",			"poissonLuneEcran",		500,	21	)
		grea:set_screen_proj(	{ fx=1/(2048*.4), fy=1/(815*.62),  ox_left=-.15, ox_right=-1.35, oy=-.33 } )
--	grea = add_grea( "Maa",				"test",					20,		24	)	--todo same race cause troubles

	grea = add_grea( "Loyal",			"loyal",				1,		1	)
		grea.b_loyal = true

	grea = add_grea( "Cacha",			"cachalot",				12	)
		--assign_boid_out( id, nb, dlen, len_before )
		grea:assign_boid_out( 10, 24, 1., -3. )
		grea:set_launcher(	{ 65, 66 },		-- 65, 66, 67, 70, 71 },
							50., 105., 1.5, 2.,
							false
						)

	grea = add_grea( "Sun",				"sunfish",				12	)
		grea:assign_boid_out( 11, 3, .5, 0.5 )
		grea:set_launcher(	{ 11, 12 },
		-- grea:set_launcher(	{ 11, 12, 13 },
							40., 45., .8, 1.2,
							true
						)
	grea = add_grea( "Req",				"requin",				12	)
		grea:assign_boid_out( 12, 6, .5, .5 )
		grea:set_launcher(	{4, 5 },		--5, 6, 7, 8, 9, 10 },
							45., 50., 1.2, 2.,
							true
						)

--	grea = add_grea( "CurveTest",		"test",					12	)
--		grea:assign_boid_out( 12, 6, .5, .5 )
--		grea:set_launcher(	{ 4, 5, 6, 7, 8, 9, 10 },
--							45., 50., 1.2, 2.,
--							true
--						)

end
