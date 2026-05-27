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
--[[
--	if true then return end
	if not self.races then return end

	--todo add different type of drawing
	--	draw 3 differents races on 3 curves
	local nb = 10
	local speed = (self.creature_speed or 1.)
	if false then
		self:draw_race_on_curve( "requin",		nb,		6,	speed )
		self:draw_race_on_curve( "aaaref",		nb,		2,	speed )
		self:draw_race_on_curve( "aaaref",		nb,		3,	speed )
		self:draw_race_on_curve( "meduse",		nb,		3,	speed )
		self:draw_race_on_curve( "bathykorus",	nb,		2,	speed )
		self:draw_race_on_curve( "poissonLune",	nb,		5,	speed )
		self:draw_race_on_curve( "Sternoptyx",	nb,		7,	speed )
		self:draw_race_on_curve( "sunfish",		nb,		8,	speed )
		self:draw_race_on_curve( "tompteris",	20, 	8, 			speed )
	end
	if false then
		self:draw_race_on_curve( "poissonLune",			nb,		2,		speed )
		self:draw_race_on_curve( "poissonLune_jaune",	3,		2,		speed )
		self:draw_race_on_curve( "poissonLune_rouge",	nb,		3,		speed )
		self:draw_race_on_curve( "poissonLune_bleu",	nb,		4,		speed )
		self:draw_race_on_curve( "sunfish",				nb,		1,		speed )
		self:draw_race_on_curve( "maquereau",			nb*50,	1,		speed )
		self:draw_race_on_curve( "anglerfish",			nb,		2,		speed )
		self:draw_race_on_curve( "hippocampe",			20, 	1, 		speed )
		self:draw_race_on_curve( "meduse",				5, 		5, 		speed )
	end

	self:draw_race_on_curve( "requin",				12,		4,		3. )
	self:draw_race_on_curve( "cachalot",			3, 		65, 	speed )
--]]
	--move the tuna away to vary the env
	local grea = self:get_grea_by_name( "thon" )
	if grea then
		local v = aaa.math.get_fractalsum( 0, 0, aaa.time.t, .1, 2 )
		grea:get_boid():set_box_z( -12. * v )
	end
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

--	races:add( "meduse",				3 )
--	races:add( "aaaref",				4 )

	if false then
		races:add( "poissonLune_rouge"		)
		races:add( "poissonLune_jaune"		)
		races:add( "poissonLune_bleu"		)
		races:add( "anglerfish",			2	)
		races:add( "hippocampe"				)
	end

	races:add( "sunfish"				)
	races:add( "poissonLune"			)
	races:add( "poissonLune_rare"		)
	races:add( "maquereau_mix"			)
	races:add( "maquereau_bleu"			)
	races:add( "maquereau_vert"			)
	races:add( "maquereau_turquoise"	)
	races:add( "maquereau_orange"		)

	races:add( "thon"					)
	races:add( "raie"					)
	races:add( "requin"					)
	races:add( "cachalot",			2	)
	races:add( "poissonLune_kid"		)
	races:add( "poissonLune_right"		)

--	races:add( "base"					)
--	races:add( "base3"					)
--	races:add( "meduse",				3	)

	self:print_inverse( "TANK:load_races() End" )
end

function TANK:init_fish()
	local grea
--	grea = self:add_grea( "fish_left",	"maquereau_mix",		500,	1	)
--	grea = self:add_grea( "fish", 		"poissonLune",			300,	2	)

	self.grea_kid = {}
	grea = self:add_grea( "kid_left",		"poissonLune_kid",		32,		14	)
		grea:set_blow( 0 )
		self.grea_kid[1] = grea
	grea = self:add_grea( "kid_right",		"poissonLune_right",	300,	15	)
	--grea:set_blow( 0 )
		self.grea_kid[2] = grea
--	grea = self:add_grea( "fish_right",	"maquereau_mix",		500,	3	)
	grea = self:add_grea( "fish_color",		"maquereau_bleu",		1000,	4	)
--	grea = self:add_grea( "requin", 		"requin",				5,		5	)
--		grea:set_curve_dist_nb( .5, 60 )
	grea = self:add_grea( "lune",			"poissonLune",			100,	6	)
	grea = self:add_grea( "lune_rare",		"poissonLune_rare",		20,		7	)

	grea = self:add_grea( "thon", 			"thon",					32,		8	)
		grea:set_curve_dist_nb( .4, 30 )

--	grea = self:add_grea( "raie", 		"raie",					10,		9	)
--		grea:set_curve_dist_nb( 1.21, 10 )

	grea = self:add_grea( "Cacha",			"cachalot",				12		)
		grea:assign_boid_out( 10, 24, 1., -3. )
		grea:set_launcher(	{ 65, 66, 67, 70, 71 },
							50., 105., 1.5, 2.,
							false
						)
	grea = self:add_grea( "Sun",			"sunfish",				12		)
		grea:assign_boid_out( 11, 3, .5, 0.5 )
		grea:set_launcher(	{ 11, 12, 13 },
							40., 45., .8, 1.2,
							true
						)
	grea = self:add_grea( "Req",			"requin",				12		)
		grea:assign_boid_out( 12, 6, .5, .5 )
		grea:set_launcher(	{ 4, 5, 6, 7, 8, 9, 10 },
							45., 50., 1.2, 2.,
							true
						)

end
