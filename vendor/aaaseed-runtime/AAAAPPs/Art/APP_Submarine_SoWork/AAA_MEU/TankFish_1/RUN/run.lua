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
	--move the tuna away to vary the env
	-- local grea = self:get_grea_by_name( "thon" )
	-- if grea then
	-- 	local v = aaa.math.get_fractalsum( 0, 0, aaa.time.t, .1, 2 )
	-- 	grea:get_boid():set_box_z( -12. * v )
	-- end
end

function TANK:load_races()
	self:print_inverse( "TANK:load_races() Begin" )

	CELT:set_texture_use( false )

	local races = self.__races
	--	create races
	races:add( "poissonLune"			)
	races:add( "poissonLune_rare"		)
	races:add( "maquereau_mix"			)
	races:add( "maquereau_bleu"			)
	races:add( "thon"					)

	self:print_inverse( "TANK:load_races() End" )
end

function TANK:init_fish()

	local grea
	grea = self:add_grea( "maquereau_mix",	"maquereau_mix",		500,	1	)
	grea = self:add_grea( "poisson_lune", 	"poissonLune",			300,	2	)
	grea = self:add_grea( "fish_col1",		"maquereau_bleu",		500,	3	)
	grea = self:add_grea( "fish_col2",		"maquereau_bleu",		400,	4	)
	grea = self:add_grea( "fish_col3",		"maquereau_bleu",		64,		5	)
	grea = self:add_grea( "fish_col4",		"maquereau_bleu",		400,	9	)
	grea = self:add_grea( "fish_col5",		"maquereau_bleu",		400,	10	)
	grea = self:add_grea( "fish_col6",		"maquereau_bleu",		400,	11	)
	grea = self:add_grea( "lune",			"poissonLune",			100*3,	6	)
	grea = self:add_grea( "lune_rare",		"poissonLune_rare",		20*3,	7	)

	grea = self:add_grea( "thon", 			"thon",					32*3,	8	)
		grea:set_curve_dist_nb( .4, 30 )

end
