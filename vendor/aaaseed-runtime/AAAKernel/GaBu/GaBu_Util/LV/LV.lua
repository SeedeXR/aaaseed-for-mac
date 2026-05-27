
if not aaa.lua.global.get( "LV" ) then

	aaa.lua.global.declare_table( "LV" )


	LV.__colors =  {	{ 5*16+4,	0,			255		},	--	Violet
						{ 0,		10*16+7,	13*16+8 },	--	Bleu
				 		{ 5*16+11,	13*16+13,	4*16+5	},	--	Vert
						{ 255,		14*16+8,	5		},	--	Jaune
						{ 255,		1*16+15,	13*16+5	},	--	Rose
						{ 14*16+10, 4,			3*16+7	},	--	Rouge
						{ 255,		10*16,		2*16+8	},	--	Safran
					}
	LV.__colors_name	=  {	"Violet", "Bleu", "Vert", "Jaune", "Rose", "Rouge", "Safran" }
	LV.__tex_bind		=  {	disk=36,	cross=38,	losange=37,	lv=39	}
	for i = 1,7 do
		for j = 1,3 do
			LV.__colors[i][j] = LV.__colors[i][j] / 255.
		end
		LV.__colors[i][4] = 1.
	end

--todo there is pieece of this in MonAqua capture MEUs: should be dealt with
	LV.sol_sx = 13.34	--	grand cote
	LV.sol_sy = 20.85	--	petit cote
	LV.zone_sx = LV.sol_sx
	LV.zone_rx = 3840 / 2768	--	from simulation then manual ajustement of fbo
	LV.zone_ry = 1 / LV.zone_rx
	LV.zone_sy = LV.sol_sx * LV.zone_ry
	LV.zone_sxh = LV.zone_sx * .5
	LV.zone_syh = LV.zone_sy * .5

end

--aaa.print( LV.zone_sy )
function LV:get_color_nb( id )		return #(LV.__colors)	end
function LV:get_color( id )			return LV.__colors[id] 	end
function LV:get_color_name( id )
	local str = inside(id, 1, 7) and LV.__colors_name[id] or "Free Color"
	return str
end
function LV:get_tex_bind( name )
	return LV.__tex_bind[ string.lower(name) ]
end
function LV:get_rgb( id )
	local t = LV.__colors[id]
	return t[1], t[2], t[3]
end

