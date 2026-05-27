
function TANK:load_races_monaco()
	self:print_inverse( "TANK:load_races() Begin" )

	CELT:set_texture_use( false )

	local races = self.__races
	--	create races

	--races:add( "aaaref",		4 )

	races:add( "B_SIMPLE"		)

	races:add( "B_ANCHOVY"		)
	races:add( "B_BANNER"		)
	races:add( "B_BARAK"		)
	races:add( "B_BAT"			)
	races:add( "B_BUTTER"		)
	races:add( "B_CARANG"		)
	races:add( "B_CARPET"		)
	races:add( "B_CLEAN"		)
	races:add( "B_CLOWN"		)
	races:add( "B_CONVICT"		)
	races:add( "B_CORNET"		)
	races:add( "B_CORNU"		)
	races:add( "B_CUTTLE"		)
	races:add( "B_DARK_GUN"		)
	races:add( "B_DOLPH"		)
	races:add( "B_EAGLE"		)
	races:add( "B_EMPEROR"		)
	races:add( "B_FLASH"		)
	races:add( "B_GRACE"		)
	races:add( "B_GUN"			)
	races:add( "B_HACHETTE"		)
	races:add( "B_LADY_BL_M"	)
	races:add( "B_LADY_BL_F"	)
	races:add( "B_LADY_GREEN"	)
	races:add( "B_LION"			)
	races:add( "B_LIPS_A"		)
	races:add( "B_LIPS_B"		)
	races:add( "B_LUTJ"			)
	races:add( "B_MANTA"		)
	races:add( "B_MAORI"		)
	races:add( "B_PARROT"		)
	races:add( "B_PARROT_BL_F"	)
	races:add( "B_PARROT_BL_M"	)
	races:add( "B_PELAGIA"	)
	races:add( "B_POTATO"		)
	races:add( "B_RED_F"		)
	races:add( "B_RED_M"		)
	races:add( "B_SHRK_WHAL"	)
	races:add( "B_SUCKER"		)
	races:add( "B_SURGERY"		)
	races:add( "B_TIGER"		)
	races:add( "B_TITAN"		)
	races:add( "B_TRAVEL"		)
	races:add( "B_TRIGGER"		)
	races:add( "B_TURTL"		)
	races:add( "B_TURTL_BB"		)
	races:add( "B_WHITETIP"		)
	races:add( "B_WHITETIP_BB"	)

	self:print_inverse( "TANK:load_races() End" )
end

function TANK:load_races()
	self:load_races_monaco()
end


