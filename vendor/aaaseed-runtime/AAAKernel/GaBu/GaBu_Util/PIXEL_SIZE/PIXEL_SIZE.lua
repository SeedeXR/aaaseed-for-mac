
if CLASS.DECLARE( "PIXEL_SIZE" ) then
	PIXEL_SIZE:set_class_status_doc(	CLASS.STATUS.GABU,
										"helper for pixel size interfaces" )
	local t =
	{
		Square		= {	512,		512 	},
		VGA			= {	640,		480 	},
		PAL			= {	575*4/3,	575 	},
		XGA			= {	1024,		768		},

		SXGA		= {	1280,		1024	},
		FWXGA		= {	1366,		768		},
		HD			= {	1920,		1080	},

		WUXGA		= {	1920,		1200	},
		WQHD		= {	2560,		1440	},

		DPool 		= {	6240,		3840	},
		LVFloor		= {	3840,		2768	},
		LVStageLed	= {	3000,		1400	},
	}
	t["720p"]	= {	1280,		720		}
	t["2160p"]	= {	3840,		2160	}
	t["4320p"]	= {	7680,		4320	}
	PIXEL_SIZE.__sxy = t

	PIXEL_SIZE.__ui_order =
	{
		"Square",	"VGA",		"PAL",		"XGA",
		"720p",		"SXGA",		"FWXGA",	"HD",	
		"WUXGA",	"WQHD",		"2160p",	"4320p",
		--"DPool",	"LVFloor",	"LVStageLed"
	}
	local str_to_id = {}
	for i,str in ipairs(PIXEL_SIZE.__ui_order) do
		str_to_id[string.lower(str)] = i
	end
	PIXEL_SIZE.__str_to_id = str_to_id
end

function PIXEL_SIZE:get_sxy( id_or_str )
	local id
	if type(id_or_str) == "string" then
		id = PIXEL_SIZE.__str_to_id[string.lower(id_or_str)]
	else
		id = id_or_str
	end
	local t = PIXEL_SIZE.__sxy[PIXEL_SIZE.__ui_order[id]]
	if t then
		return t[1], t[2]
	else
		self:print( "format "..id.." don't exist" )
	end
end
function PIXEL_SIZE:get_ui_str()
	return PIXEL_SIZE.__ui_order
end 


