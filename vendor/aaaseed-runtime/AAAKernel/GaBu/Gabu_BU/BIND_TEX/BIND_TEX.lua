if CLASS.DECLARE( "BIND_TEX" ) then
	BIND_TEX:set_class_status_doc(	CLASS.STATUS.CORE,
									"centralize code to deal with bank of texture used by TEXS" )
end


function BIND_TEX:create( name )
	local self = BIND_TEX:create_instance( name )
	self.__texs = {}
	self.__name_to_tex = {}
	self.__bind_to_tex = {}
	self.__groups = {}
	self.__groups[1] = {	name = "Errors", name_lower = "errors", order = 4242424242, texs = {} } 
	return self
end

function BIND_TEX:make_info_str( t )
	local str 
	--if t.group then
		str = string.format( "%-8s | %3d | %s", t.group.name, t.bind_flat, t.name ) 
	--	t.group.." -> "..str 
	--else
	--	str = name
	--end
	if t.name_short then
		str = str.." short is "..t.name_short
	end
	if t.syno then
		str = str.." Synonym is "..t.syno
	end
	
	return str
end

-- ADD
--
function BIND_TEX:make_group( name, order, nb_u, nb_v )
	local name_lower = string.lower(name)
	local group = {	name = name, name_lower = name_lower,
					order = order,
					nb_u = nb_u, nb_v = nb_v,
					texs = {} } 
	return group
end
function BIND_TEX:add_group( name, order, nb_u, nb_v )
	local groups = self.__groups
	local name_lower = string.lower(name)
	local group = groups[name_lower]
	if not group then
		group = self:make_group( name, order, nb_u, nb_v )
		groups[name_lower] = group
	end
	return group
end

BIND_TEX.doc.add_tex = "( group, name, bind [,name_short [,syno] ] )"
function BIND_TEX:add_tex(	group, name,		bind,			name_short,				syno )
	if not group then 
		self:box_error( "in BIND_TEX:update() no group for texture\n"..name )
		return
		--group = groups[1]
	end
	local tex =  { 	name=name,	bind_flat=bind, name_short=name_short,	syno=syno,	group = group }
	-- we add texture to group
	table.insert( group.texs, tex )
	-- we add texture to global list only of not a bank
	if not group.b_bank then 
		table.insert( self.__texs, tex )
	end
	self.__b_need_update = true
end
BIND_TEX.doc.add_tex_table = "( group, tab )"
function BIND_TEX:add_tex_table( group, tab )
	for _,e in IPAIRS( tab ) do
		self:add_tex( group, e[1], e[2], e[3], e[4] )
	end
end
BIND_TEX.doc.add_tex_pre = "( group, pre_or_fn_i, bind_begin, nb )"
function BIND_TEX:add_tex_pre( group, pre_or_fn_i, bind_begin, nb )
	local b_str = type(pre_or_fn_i) == "string"
	for i=1,nb do
		self:add_tex( group, b_str and pre_or_fn_i..i or pre_or_fn_i(i), bind_begin+i-1 )
	end
end
BIND_TEX.doc.add_tex_bank = "( group, pre_or_fn_i, bind_begin, nb )"
function BIND_TEX:add_tex_bank( group, bind_begin, nb )
	group.b_bank = true
	for i=1,nb do
		self:add_tex( group, i-1, bind_begin+i-1 )
	end
end


function BIND_TEX:update( bind_begin, nb )
	if self.__b_need_update then
		local name_to_tex = {}
		local bind_to_tex = {}
		local groups = self.__groups
		local groups_by_name = {}
			
		for _,tex in ipairs(self.__texs) do
			name_to_tex[tex.bind_flat] = tex
			local name = tex.name
			local name_lower = string.lower( name )
			tex.name_lower = name_lower
			local bind_flat = tex.bind_flat
			tex.bank, tex.bind = aaa.img.make_bank_bind_2d( bind_flat )

			-- local group = tex.group
			-- if not group then
			-- 	self:box_error( "in BIND_TEX:update() no group for texture\n"..name )
			-- 	group = groups[1]
			-- end

			-- BY BIND
			bind_to_tex[bind_flat] = tex
			-- BY NAME
			--store under both name and name_short and synonyme
			--todo check collision
			if name then
				name_to_tex[name] = tex
				name_to_tex[name_lower] = tex
			end
			if tex.syno then
				name_to_tex[string.lower(tex.syno)] = tex
			end
			-- IN GROUP
			--table.insert( group.texs, tex )
		end

		self.__name_to_tex = name_to_tex
		self.__bind_to_tex = bind_to_tex

		local groups_sorted = {}
		for _,group in pairs_sorted( groups, function(t,a,b) return t[a].order > t[b].order end ) do
			table.insert( groups_sorted, group )
			groups_by_name[group.name_lower] = group
		end
		self.__groups_sorted = groups_sorted
		self.__groups_by_name = groups_by_name

		self.__b_need_update = false
	end
end

-- GET
--
function BIND_TEX:get_tex_from_bind( bind )
	return bind and self.__bind_to_tex[ bind ]
end

BIND_TEX.doc.get_name_from_bind = { "( bind ) return name for this bind.",
								" \"bind_\"..bind is returned if no name exist" }
function BIND_TEX:get_name_from_bind( bind )
	local tex = self:get_tex_from_bind( bind )
	return tex and tex.name or string.format( "bind_%d", bind )
end

BIND_TEX.doc.get_name_or_bind =  { "( bind ) return name for this bind.",
								" bind as string is returned if no name exist" }
function BIND_TEX:get_name_or_bind( bind )
	local tex = self:get_tex_from_bind( bind )
	return tex and tex.name or tostring(bind)
end

-- function BIND_TEX:get_group( bind )
-- 	local info = self:get_info_by_bind( bind )
-- 	return info and info.group or nil
-- end

function BIND_TEX:get_bank_bind_from_name( name )
	local tex = self.__name_to_tex[ string.lower(name) ]
	if tex then
		return tex.bank, tex.bind
	end
end

BIND_TEX.doc.get_bind_from_name = { "( name ) return bind of texture having this name.",
								"  name case is ignored." }
function BIND_TEX:get_bind_from_name( name )
	name = string.lower(name)
	local tex = self.__name_to_tex[name]
	if tex then
		return tex.bind_flat
	else
		self:print_error( "BIND_TEX did not find "..name )
		--table.print( self.__name_to_tex, "name_to_tex" )
	end
end

-- DUMP
--
function BIND_TEX:get_group_from_name( group_name )
	group_name = string.lower(group_name)
	return self.__groups_by_name[group_name]
end

function BIND_TEX:dump_by( tab )
	local nb = 0
	for key,tex in pairs_sorted(tab) do
		self:print( string.format( "%-15s : %s", tex.name, self:make_info_str(tex) ) )
		nb = nb + 1
	end
	return nb
end

function BIND_TEX:dump_by_name()     
	self:print( "----- DUMPING TEXTURE NAME BY NAME -----" ) 
	local nb = BIND_TEX:dump_by( self.__name_to_tex )
	self:print( nb.." texture by name" )
end
function BIND_TEX:dump_by_bind()
	self:print( "----- DUMPING TEXTURE NAME BY BIND -----" )
	table.print(self.__bind_to_tex, "__bind_to_tex", 1 )
	local nb = self:dump_by( self.__bind_to_tex )
	self:print( nb.." textures by bind" )
end
function BIND_TEX:dump_by_group()
	local nb = 0
	--table.print( self.__group, "group" )
	self:print( "----- DUMPING TEXTURE NAME BY GROUP -----" ) 
	self:print( "Texture name groups are " ) 
	for _,group in IPAIRS(self.__groups_sorted) do
		self:print( "    "..group.name ) 
	end
	for _,group in IPAIRS(self.__groups_sorted) do
		if not group.b_bank then 	--these group are hre only for starmenu
			self:print( "group "..group.name.." begin" ) 
			local nb_in_group = self:dump_by( group.texs )
			self:print( "group "..group.name.." have "..nb_in_group.." textures" )
			nb = nb + nb_in_group
		end
	end
	self:print( nb.." textures by group" )
end

--todo deal with sublevels
--todo deal with relation to this with IMG,IMGS...
--todo fn about bank,bind ...
--todo move to TEXS.lua file ?
-- central mecanism to acess texture
if CLASS.DECLARE( "TEXS" ) then
	TEXS:set_class_status_doc(	CLASS.STATUS.CORE,
								"bank of texture should be accessed through here" )
	--old comments 
	--we use intermediate fns to change the low one in real time
	--local function set_tex_src_from_selector( bu, balue ) MEU.set_tex_src_from_selector( bu, balue ) end
end

function TEXS:c_init()

	local BT = BIND_TEX:create( "TEXS" )

	--		name			bind name_short	synonym
	local t_pass =
	{
		{	"Albedo", 		188, "Albe",	"GBuffer"	},
		{	"Normal", 		189, "Nor",		},
		{	"Specular",		190, "Spec",	},
		{	"Emissive",		191, "Emis"		},

		{	"ZBuf",			187, "ZBuf"		},
		{	"Light",		186, 			},
		{	"Fog",			185, 			},
		{	"Dof",			184, 			},
		{	"Transparent",	183, "Transp"	},
		{	"GodRays",		182, "Rays"		},
		{	"ToneMap",		181, "Tone"		},
		{	"Fxaa", 		180,			},
		{	"Caustic", 		179,			},		--todo remove for a more generic one soon (2021 October)
		{	"FogVolume", 	178, "FogVol"	},
		{	"AddBlur", 		177, "Blur"		},
		{	"Debug", 		176, "Dbg"		},

		{	"Shadow Map 1", 212, "Shadow1"	},	--, "Shadow_1"	},
		{	"Shadow Map 2", 213, "Shadow2"	},	--, "Shadow_2"	},
	}	
	local t_utils =
	{
		{	"Noise", 		63,		},
		{	"Pre", 			112,	},
		{	"Viz", 			113,	},
		{	"DifR",			115,	},
		{	"Dif",			114,	},
		{	"UVMap", 		33,		},
	}
	local CAP_BEGIN = 128
	local CAP_END	= CAP_BEGIN+31

	local t_cap =
	{
		{	"Capture.In1", 			CAP_BEGIN+0,	"Cap.In1"	},
		{	"Capture.In2",			CAP_BEGIN+1,	"Cap.In2"	},
		{	"Capture.In3",			CAP_BEGIN+2,	"Cap.In3"	},
		{	"Capture.In4",			CAP_BEGIN+3,	"Cap.In4"	},
		{	"Capture.In5",			CAP_BEGIN+4,	"Cap.In5"	},
		{	"Capture.In6",			CAP_BEGIN+5,	"Cap.In6"	},
		{	"Capture.In7",			CAP_BEGIN+6,	"Cap.In7"	},
		{	"Capture.In8",			CAP_BEGIN+7,	"Cap.In8"	},
		
		{	"Capture.A", 			CAP_END,		"Cap.A"		},
		{	"Capture.B",			CAP_END-1,		"Cap.B"		},
		{	"Capture.C", 			CAP_END-2,		"Cap.C"		},
		{	"Capture.D",			CAP_END-3,		"Cap.D"		},
		{	"Capture.E",			CAP_END-4,		"Cap.E"		},
		{	"Capture.F",			CAP_END-5,		"Cap.F"		},
		{	"Capture.G",			CAP_END-6,		"Cap.G"		},
		{	"Capture.H",			CAP_END-7,		"Cap.H"		},

		{	"Capture.DistFieldSrc",	CAP_END-16,		"Cap.DFSrc"	},
		{	"Capture.DistField",	CAP_END-17,		"Cap.DF"	},
		{	"Capture.FieldA",		CAP_END-18,		"Cap.FA"	},
		{	"Capture.FieldB",		CAP_END-19,		"Cap.FB"	},

--todo we should generate Monitor on the fly
--todo we should resolve conflic here
		{	"KinA",					256+96,			"M1"		},
		{	"KinB",					256+96+1,		"M2"		},
		{	"KinC",					256+96+2,		"M3"		},		
	-- 	{	"KinA",					256+32,		},	--"M1"		},
	-- 	{	"KinB",					256+64,		},	--"M2"		},
	-- 	{	"KinC",					256+96,		},	--"M3"		},
	}
	local t_ui =
	{
		{	"Rewind"	,120	},
		{	"Pause"		,121	},
		{	"Play"		,122	},
		{	"Stop"		,123	},
		{	"Off"		,124	},
		{	"On"		,125	},
		{	"World"		,126	},
		{	"PB" 		,127 	},
		{	"NoBind" 	,88 	},	
	}

	BT:add_tex_pre( 	BT:add_group( "Video",		100,	4,4	),	function(i) return string.char(64+i) end,	96,			16 )
	BT:add_tex_pre( 	BT:add_group( "F",			90,		8,4	),	"F",										64,			24 )
	BT:add_tex_pre( 	BT:add_group( "In",			50,		8	),	"In",										128,		32 )
	BT:add_tex_pre( 	BT:add_group( "Mire",		40, 	8	),	"Mi",										128 + 32,	16 )
--todo we should resolve conflit here	
	BT:add_tex_pre(		BT:add_group( "M",			80,		8,4	),	"M",	256 + 32*3, 32 )
--	BT:add_tex_pre( "M",										256, 		32 )

	BT:add_tex_bank(	BT:add_group( "Bank 0",		0, 		8,8	),			0,			32 )
	BT:add_tex_bank(	BT:add_group( "Bank 1",		1, 		8,8	),			32,			32 )
	BT:add_tex_bank(	BT:add_group( "Bank 7",		7, 		8,8	),			7*32,		32 )


	--todo make sur this mapped well always
	BT:add_group(	"Util", 110, 1 )


	BT:add_tex_table(	BT:add_group( "Utils", 		110,	1	),	t_utils	)
	BT:add_tex_table(	BT:add_group( "Pass", 		50, 	2	),	t_pass	)
	BT:add_tex_table(	BT:add_group( "Capture", 	30, 	4	),	t_cap	)
	BT:add_tex_table(	BT:add_group( "UI", 		-1			),	t_ui	)

	-- local function add_to_mon_list( start, )
	-- local tab = {}
	-- for i = 1,16*5 do tab[i]="No" end

	BT:update()

	TEXS.__bind_tex = BT
	TEXS.BIND_HACK_BEGIN = 256
	TEXS.BIND_HACK_END = TEXS.BIND_HACK_BEGIN + 32 - 1

--	table.print( BT.__name_to_tex, "name_to_tex", 2 )
--	aaa.box_debug( "BT" )
end

function TEXS:get_tex_named()	--todotex rename and restructure for clarity
	return self.__bind_tex
--	local tab = self.__bind_tex
--	return tab and tab or MEU:__init_tex_named()
end
TEXS.doc.get_bind_by_name 	= BIND_TEX.doc.get_bind_by_name 
function TEXS:get_bind_by_name( name )		return self:get_tex_named():get_bind_from_name( name )	end
TEXS.doc.get_name_from_bind = BIND_TEX.doc.get_name_from_bind 
function TEXS:get_name_from_bind( bind )	return self:get_tex_named():get_name_from_bind( bind )	end
TEXS.doc.get_name_or_bind 	= BIND_TEX.doc.get_name_or_bind 
function TEXS:get_name_or_bind( bind )		return self:get_tex_named():get_name_or_bind( bind )	end

function TEXS:set_bank_names( bu, b_full )
	if b_full then
		bu:set_item_text( 2, "1", "2 | F", "3 | Vid", "4 | In", "5 | Mi", "6 | Fbo", "7"	)
		bu:set_item_text( 9, "[  8 | GaBuZo", "9",  "10",  "11 | M  ]", "[  12 | User", "13", "14", "15  ]" )
	else
		bu:set_item_text( 2, "1", "F", "Vid", "In", "Mi", "6", "7"	)
		bu:set_item_text( 9, "[ GaBuZo", "9",  "10",  "M ]", "[ User", "13", "14", "15 ]" )
	end
	bu:set_item_text( 17, "[ VIDEO", "17", "18", "19 ]" )
	bu:set_item_text( 33, "[ IMAGE", "33", "34", "35", "36..." )
end

function TEXS:__adjust_bind_bank_selector( bu, nb_u, line_nb )
	--	table.print( bu," BU_TEX_SEL;__adjust_bind_bank_selector", 2 )
	local nb = nb_u*line_nb
	bu:set_nb_min_0( nb_u, line_nb )
	for i=1,nb,4 do	bu:set_item_text( i, i-1 ) end
	for i=nb_u,nb,nb_u do bu:set_item_text( i, i-1 ) end
	bu:set_item_text( nb, nb-1 )
end
function TEXS:adjust_bank_selector( bu, nb_u, line_nb )
	--	table.print( bu, "TEXS;adjust_bank_selector()", 2 )
	self:__adjust_bind_bank_selector( bu, nb_u, line_nb )
	self:set_bank_names( bu )
end
function TEXS:adjust_bind_selector( bu, nb_u, line_nb )
	--	table.print( bu, "TEXS:adjust_bind_selector()", 2 )
	self:__adjust_bind_bank_selector( bu, nb_u, line_nb )
end

-- TEXS.__t_bind_text = {	"A","B","C","D",	"E","F","G","H",	"I","J","K","L",
-- 							"Dif", "Pre", "DifR", "Viz",
-- 						"F1","2","3","4",	"F5","6","7","8",	"F9","10","11","12", 	"F13","14","15","16",
-- 						"In1","2","3","4",	"In5","6","7","8",	"In9","10","11","12",	"In13","14","15","16",
-- 						"Mi1","2","3","4",	"Mi5","6","7","8",	"Mi9","10","11","12",	"Mi13","14","15","16",
-- 						"Albe", "Nor", "Spec", "Emis",
-- 							"ZBuf", "Light", "Caustic", "Fog",
-- 							"Dof", "Transparent", "GodRays", "ToneMap",
-- 							"Fxaa",nil,nil,"UVMap"
-- 						}

TEXS.__t_bind_symbo = {	"A","B","C","D",			"E","F","G","H",			"I","J","K","L",
								"Dif", "Pre", "DifR", "Viz",
						"F1","F2","F3","F4",		"F5","F6","F7","F8",		"F9","F10","F11","F12", 	"F13","F14","F15","F16",
						"In1","In2","In3","In4",	"In5","In6","In7","In8",	"In9","In10","In11","In12",	"In13","In14","In15","In16",
						"Mi1","Mi2","Mi3","Mi4",	"Mi5","Mi6","Mi7","Mi8",	"Mi9","Mi10","Mi11","Mi12",	"Mi13","Mi14","Mi15","Mi16",
						"M1","M2","M3","M4",		"M5","M6","M7","M8",		"M9","M10","M11","M12",		"M13","M14","M15","M16",
						"Albedo", "Normal", "Specular", "Emissive",	
								"ZBuf", "Light", "Caustic", "Fog",
								"Dof", "Transparent", "GodRays", "ToneMap",
								"Fxaa","","","","","","","UVMap"
						}
TEXS.__symbo_to_text =
{
	F2="2", F3="3", F4="4",		F6="6", F7="7", 	F8="8",			F10="10", F11="11", F12="12",		F14="14", F15="15", F16="16",
	In2="2", In3="3", In4="4",	In6="6", In7="7", 	In8="8",		In10="10", In11="11", In12="12",	In14="14", In15="15", In16="16",
	Mi2="2", Mi3="3", Mi4="4",	Mi6="6", Mi7="7", 	Mi8="8",		Mi10="10", Mi11="11", Mi12="12",	Mi14="14", Mi15="15", Mi16="16",
	M2="2", M3="3", M4="4",		M6="6", M7="7",		M8="8",			M10="10", M11="11", M12="12",		M14="14", M15="15", M16="16",
	Albedo="Albe\ndo", Normal="Nor\nmal", Specular="Spe\ncular", Emissive="Emis\nsive",	ZBuf="Z\nBuf",
	Caustic="Caus\ntic", Transparent="Trans\nparent", GodRays="God\nRays", ToneMap="Tone\nMap",
	UVMap = "UV\nMap"
							
}
TEXS.__t_bind_order = {	1,2,3,4,		17,18,19,20,	33,34,35,36,	49,50,51,52,	65,66,67,68,
						5,6,7,8,		21,22,23,24,	37,38,39,40,	53,54,55,56,	69,70,71,72,
						9,10,11,12,		25,26,27,28,	41,42,43,44,	57,58,59,60,	73,74,75,76,
						13,14,15,16,	29,30,31,32,	45,46,47,48,	61,62,63,64,	77,78,79,80,
						81,82,83,84,	85,86,87,88,	89,90,91,92,	93	
						}

TEXS._t_dst_bind_text = {	"A","B","C","D",	"F1","2","3","4",		"In1","2","3","4",						
							"E","F","G","H",	"F5","6","7","8",		"In5","6","7","8",		
							"I","J","K","L",	"F9","10","11","12",	"In9","10","11","12",	
						}
TEXS._t_dst_bind_symbo = {	"A","B","C","D",	"F1","F2","f3","f4",	"In1","In2","In3","In4",	
							"E","F","G","H",	"F5","f6","F7","F8",	"In5","In6","In7","In8",	
							"I","J","K","L",	"F9","F10","F11","F12",	"In9","In10","In11","In12",
						}

function TEXS:attach_src_menu( bu, bind_symbo, bind_order )
	bind_symbo = bind_symbo or self.__t_bind_symbo
	bind_order = bind_order or self.__t_bind_order
	
	local nb = #bind_symbo
	local nb_u = 20
	--bu:set_fxy_max_for_menu( .3, 1, .417 )	--todo refine the max stuff just a hack now
	bu:set_fxy_max_for_menu( .25, 1, .4)	--todo refine the max stuff just a hack now
	bu:set_menu( bind_symbo, bu:get_name() ):set_nb_min_0( nb_u, 5 )
--	bu:set_value(80) --todo better. Set to an empty value for now

	local bu_sel = bu:get_selector()
	self.__item_text_nil_forbid_mouse_selection = true
	for i=1,nb do
		local i_src = bind_order and bind_order[i] or i
		local symbo = bind_symbo[i_src]
		local text
		if symbo == "" then
			bu_sel:set_item_text( i, nil )
			bu_sel:set_item_data( i, "none" )
		else
			text = TEXS.__symbo_to_text[symbo] or symbo
			bu_sel:set_item_text( i, text  )
			bu_sel:set_item_data( i, self:get_bind_by_name(symbo) )
		end
		bu_sel:set_item_symbo( i, symbo )
		--self:print( name.." - "..self:get_tex_named():get_bind_by_name( name ) )
	end

	-- for i=1,nb do
	-- 	local i_src = bind_order and bind_order[i] or i
	-- 	bu_sel:set_item_text( i, bind_text[i_src]  )
	-- 	bu_sel:set_item_symbo( i, bind_symbo[i_src] )
	-- 	--self:print( name.." - "..self:get_tex_named():get_bind_by_name( name ) )
	-- 	local name = bind_symbo[i_src]
	-- 	if name then		
	-- 		bu_sel:set_item_data( i, self:get_bind_by_name(name) )
	-- 	end
	-- end

	bu_sel.draw = 
		function(self)
			SELECTOR.draw(self)
			gol.draw_lines_2d(
				-.3,-.3, -.3, .5,
				-.1,-.3, -.1, .5,
				 .1,-.3,  .1, .5,
				 .3,-.3,  .3, .5,
				-.5,-.3,  .5,-.3,
				-.5,-.1, -.3,-.1 )
		end

	bu_sel:set_value( nb - 2 )
	return bu
end

--todo make it work then move it to aaa.img
--todo in a seconf phase the C should build these tables
local img_format_load_supported =
{
	{ "Bmp",		"bmp" },
	{ "Gif",		"gif" },
	{ "Jpeg",		{ "jpg", "jpeg" } },
	{ "Targa",		"tga" },
	{ "Tiff", 		{ "tif", "tiff" } },
	{ "Png", 		"png" },
	{ "Exr", 		"exr" },
	{ "HDR", 		"hdr" },
	{ "DDS", 		"dds" },
	{ "YUV/422",	{ "yuv", "422" } },
	{ "RGB Sgi",	{ "sgi", "rgb" } },
}							
local img_format_save_supported =
{
	{ "Jpeg",	"jpg" },
	{ "Targa",	"tga" },
	{ "Tiff", 	"tif" },
	{ "Png",	"png" },
	{ "Exr",	"exr" },
}

function TEXS:build_filter( t )
	local str_all = "Reconnus\0"
	--"Tiff\0*.tif;*.tiff\0"
	local str_elt = ""
	--todo
	for _,elt in ipairs(t) do
	end
end

function TEXS:load_texture( bind )
	--aaa.print_fn()
	local str = "load textures at index "..bind
	local filter = "Reconnus\0*.bmp;*.gif;*.jpg;*.sgi;*.rgb;*.tga;*.tif;*.tiff;*.png;*.yuv;*.422;*.exr;*.dds\0".."Bmp\0*.bmp\0".."Gif\0*.gif\0".."Jpeg\0*.jpg\0".."Tga\0*.tga\0".."Tiff\0*.tif;*.tiff\0".."Png\0*.png\0".."Exr\0*.exr\0".."DDS\0*.dds\0".."Tous\0*.*\0".."\0"
	--todo replace
	--local filter = self:build_filter( img_format_load_supported )
	local filenames = aaa.file.do_dialog_open( str, filter, true )
	--table.print( filenames, "filenames" )
	for i,fname in IPAIRS(filenames) do
		self:print( i.." load "..fname.." at "..bind )
		aaa.img.read( bind, fname )
		bind = bind + 1
	end
end
