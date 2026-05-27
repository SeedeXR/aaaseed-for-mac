
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	local ix = 1
	local iy = 2
	local SY = 1
	local DY = .2

	self:add_button(	{			ix,	iy },		"Load FBX Textures", 		self, "b_load_fbx_texs",		true		)
	iy = iy + SY

	iy = iy + SY

	bu = self:add_selector(	{ix,iy,	8,SY},	"Monaco Image Resize" )
		bu:set_nb( 3 )
		bu:set_item_text( 1, "Quarter", "Half", "Full" )
		bu:set_target_lua( MONACO_AQUA, "s_image_resize" )
	iy = iy + SY * 1.4

	bu = self:add_selector(	{ix,iy,	8,SY},	"Dev Image Resize" )
		bu:set_nb( 3 )
		bu:set_item_text( 1, "Quarter", "Half", "Full" )
		bu:set_target_lua( MONACO_AQUA, "s_image_resize_dev" )
	iy = iy + SY * 1.4
end

function MONACO_AQUA:is_fish_boid()
	local b = self.__b_is_fish_boid
	if b==nil then
		b = self:is_pc_mas() or self:is_so() or self:is_pc_dev()
		self.__b_is_fish_boid = b
	end
	return b
end
function MONACO_AQUA:is_fish_send()
	local b = self.__b_is_fish_send
	if b==nil then
		b = self:is_pc_mas()
		self.__b_is_fish_send = b
	end
	return b
end
function MONACO_AQUA:is_fish_draw()
	local b = self.__b_is_fish_draw
	if b==nil then
		b = self:is_pc_dif()
		self.__b_is_fish_draw = b
	end
	return b
end

function MONACO_AQUA:set_party(b)
	self:print( "Party mode set to "..b )
	self.__b_party = b

	-- floor is just one color
	local m = self:get_meu_by_name_no_error( "NdcGrids_1" )
	if m then
		if self:is_so() then
			m:set_mu_value( b and 1 or 0 )
			m:set_background_only(b)
		else
			m:set_mu_value( 0 )
		end
	end

	-- scenes don't change
	local seqs = app.seqs
	if seqs then
		seqs:set_looping(b)
	end
end
function MONACO_AQUA:is_party()
	return self.__b_party
end

function MONACO_AQUA:is_seq_looping()
	local seqs = app.seqs
	if seqs then
		return seqs:is_looping()
	end
	return false
end

function MONACO_AQUA:get_anim_path()
	--self:box_debug( app.media_dir_rel )
	return "Monaco/ANIMATION/__validated/"
end
function MONACO_AQUA:get_anim_path_rel()
	--self:box_debug( app.media_dir_rel )
	return app.media_dir_rel..self:get_anim_path()
end
function MONACO_AQUA:get_fbx_data( b_force )
	if app:is_tablet() then return end

	local fbx_data = self.fbx_data
	if not fbx_data or b_force then
		-- define FBXs data :
		-- each different FBX are defined in the table app.fbx_data
		-- path : path to folder containing FBX data
		-- fbx_fname : path to fbx file (relative to path) with no .FBX
		-- if tex_name define it over write what in FBX file

		fbx_data = {
			MANTA       		= 	{	path = "MANTA",				fbx_fname = "MANTA",												},

			ANCHOVY 			= 	{	path = "ANCHOVY",			fbx_fname = "ANCHOVY",												},
			ANCHOVY_4			= 	{	path = "ANCHOVY",			fbx_fname = "ANCHOVY_RIG_4Bones_V02",	tex_name = "ANCHOVY",		},
			ANCHOVY_7			= 	{	path = "ANCHOVY",			fbx_fname = "ANCHOVY_RIG_7Bones_V02",	tex_name = "ANCHOVY",		},
			BANNER				= 	{	path = "BANNER",			fbx_fname = "BANNER",												},
			BARAK 				= 	{	path = "BARAK",				fbx_fname = "BARAK",												},
			BAT					= 	{	path = "BAT",				fbx_fname = "BAT",													},
			BUTTER				= 	{	path = "BUTTER",			fbx_fname = "BUTTER",												},
			CARANG				= 	{	path = "CARANG",			fbx_fname = "CARANG",												},
			CARPET				= 	{	path = "CARPET",			fbx_fname = "CARPET",												},
			CLEAN				= 	{	path = "CLEAN",				fbx_fname = "CLEAN",												},
			CLOWN				= 	{	path = "CLOWN",				fbx_fname = "CLOWN",												},
			CONVICT 			= 	{	path = "CONVICT",			fbx_fname = "CONVICT",												},
			CORNET				= 	{	path = "CORNET",			fbx_fname = "CORNET",												},
			CORNU				= 	{	path = "CORNU",				fbx_fname = "CORNU",												},
			CUTTLE				= 	{	path = "CUTTLE",			fbx_fname = "CUTTLE",												},
			DARK_GUN 			= 	{	path = "DARK_GUN",			fbx_fname = "DARK_GUN",												},
			DOLPH   			= 	{	path = "DOLPH",				fbx_fname = "DOLPH",												},
			EAGLE   			= 	{	path = "EAGLE",				fbx_fname = "EAGLE",												},
			EMPEROR				= 	{	path = "EMPEROR",			fbx_fname = "EMPEROR",												},
			FLASH				= 	{	path = "FLASH",				fbx_fname = "FLASH",												},
			GRACE				= 	{	path = "GRACE",				fbx_fname = "GRACE",												},
			GUN					= 	{	path = "GUN",				fbx_fname = "GUN",													},
			HACHETTE			= 	{	path = "HACHETTE",			fbx_fname = "HACHETTE",												},
			LADY_BL_M 			= 	{	path = "LADY_BL",			fbx_fname = "LADY_BL_M",											},
			LADY_BL_F 			= 	{	path = "LADY_BL",			fbx_fname = "LADY_BL_F",											},
			LADY_GREEN 			=	{	path = "LADY_GREEN",		fbx_fname = "LADY_GREEN",											},
			LION	 			=	{	path = "LION",				fbx_fname = "LION",													},
			LIPS_A	 			=	{	path = "LIPS_A",			fbx_fname = "LIPS_A",												},
			LIPS_B 				=	{	path = "LIPS_B",			fbx_fname = "LIPS_B",												},
			LUTJ				= 	{	path = "LUTJ",				fbx_fname = "LUTJ",													},
			MAORI				= 	{	path = "MAORI",				fbx_fname = "MAORI",												},
			PARROT				= 	{	path = "PARROT",			fbx_fname = "PARROT",												},
			PARROT_BL_F			= 	{	path = "PARROT_BL_F",		fbx_fname = "PARROT_BL_F",	tex_name = "PARROT_BL_F",				},
			PARROT_BL_M			= 	{	path = "PARROT_BL_M",		fbx_fname = "PARROT_BL_M",											},
			PELAGIA 			= 	{	path = "PELAGIA",			fbx_fname = "PELAGIA",		b_texs_seq = true					},
			POTATO 				= 	{	path = "POTATO",			fbx_fname = "POTATO",												},
			RED_F 				= 	{	path = "RED_F",				fbx_fname = "RED_F",												},
			RED_M 				= 	{	path = "RED_M",				fbx_fname = "RED_M",												},
			SHRK_WHAL			= 	{	path = "SHRK_WHAL",			fbx_fname = "SHRK_WHAL",											},
			SUCKER				= 	{	path = "SUCKER",			fbx_fname = "SUCKER",												},
			SURGERY				= 	{	path = "SURGERY",			fbx_fname = "SURGERY",												},
			TIGER				= 	{	path = "TIGER",				fbx_fname = "TIGER",												},
			TITAN				= 	{	path = "TITAN",				fbx_fname = "TITAN",												},
			TRAVEL				= 	{	path = "TRAVEL",			fbx_fname = "TRAVEL",												},
			TRIGGER				= 	{	path = "TRIGGER",			fbx_fname = "TRIGGER",												},
			TURTL				= 	{	path = "TURTL",				fbx_fname = "TURTL",												},
			TURTL_BB			= 	{	path = "TURTL_BB",			fbx_fname = "TURTL_BB",												},
			WHITETIP			= 	{	path = "WHITETIP",			fbx_fname = "WHITETIP",												},
			WHITETIP_BB			= 	{	path = "WHITETIP_BB",		fbx_fname = "WHITETIP_BB",											},
			YEL_TANG			= 	{	path = "YEL_TANG",			fbx_fname = "YEL_TANG",												},

			FISH_Simple			= 	{	path = "test/FISH_Simple",	fbx_fname = "FISH_Simple",											},

			TURTLE				= 	{	path = "TURTL",				fbx_fname = "TURTL_200519",											},
			EEL 				= 	{	path = "EEL",				fbx_fname = "C_EEL_PIJ_EEL_GR",										},
			WHALE				= 	{	path = "WHALE",				fbx_fname = "C_WHAL_BB_PIJ_WHB_GR",									},
			WHALE_DJ			= 	{	path = "WHALE",				fbx_fname = "D_WHAL_BB_DJ_WHAL_Chore",								},
			WHALE_BB			= 	{	path = "WHALE",				fbx_fname = "D_WHAL_BB_DJ_WHALBB_Big",								},
			C_TITAN				= 	{	path = "TITAN",				fbx_fname = "C_TITAN_PIJ_TITAN_GR",									},
			MANTA_EAT			= 	{	path = "MANTA",				fbx_fname = "D_MANTA_DN2_MANTA_Eat",								},
			MANTA_PARADE		= 	{	path = "MANTA",				fbx_fname = "D_MANTA_DJ2_MANTA_Parade",								},
			MANTA_CLEAN			= 	{	path = "MANTA",				fbx_fname = "D_MANTA_DJ1_MANTA_clean",								},
			MRAY_C_PIJ			= 	{	path = "MRAY",				fbx_fname = "C_MRAY_PIJ_MRAY_GR",									},
			MRAY_D_PIJ			= 	{	path = "MRAY",				fbx_fname = "D_MRAY_DJ1_MRAY_clean",								},
			PARROT_D_PIJ 		= 	{	path = "PARROT",			fbx_fname = "D_PARROT_DJ1_PARROT_bump",								},
			PARROT_D_PIJ_B 		= 	{	path = "PARROT",			fbx_fname = "D_PARROT_DJ1_PARROT_bump",								},
			PARROT_D_PIJ_C 		= 	{	path = "PARROT",			fbx_fname = "D_PARROT_DJ1_PARROT_bump",								},
			PARROT_D_PIJ_D 		= 	{	path = "PARROT",			fbx_fname = "D_PARROT_DJ1_PARROT_bump",								},
			DOLPH_C 			= 	{	path = "DOLPH",				fbx_fname = "C_DOLPH_PIJ_DOLPH_GR",									},
			TURTLE_D 			= 	{	path = "TURTL",				fbx_fname = "D_TURTL_DJ1_TURTL_clean",								},
			POTATO_CLEAN 		= 	{	path = "POTATO",			fbx_fname = "D_POTATO_DJ1_POTATO_clean",							},
			DOLPH_DN2 			= 	{	path = "DOLPH",				fbx_fname = "D_DOLPH_DN2_DOLPH_02",									},
			WHITETIP_DJ5 		= 	{	path = "WHITETIP",			fbx_fname = "D_WHITETIP_DJ5_WHITETIP_Hunt",							},
			WHITETIP_DJ5_B 		= 	{	path = "WHITETIP",			fbx_fname = "D_WHITETIP_DJ5_WHITETIP_Hunt",							},
			WHITETIP_DJ5_C 		= 	{	path = "WHITETIP",			fbx_fname = "D_WHITETIP_DJ5_WHITETIP_Hunt",							},
			TIGER_CHASSE_KR 	= 	{	path = "TIGER",				fbx_fname = "D_TIGER_DJ5_TIGER_Hunt",								},
			TIGER_CHASSE_KC 	= 	{	path = "TIGER",				fbx_fname = "D_TIGER_DJ5_TIGER_Hunt",								},
			SHRK_WHAL_KR 		= 	{	path = "SHRK_WHAL",			fbx_fname = "A_SHRK_WHAL",											},
			SHRK_WHAL_KC 		= 	{	path = "SHRK_WHAL",			fbx_fname = "A_SHRK_WHAL",											},
			SHRK_WHAL_SO 		= 	{	path = "SHRK_WHAL",			fbx_fname = "A_SHRK_WHAL",											},
			TIGER_KR 			= 	{	path = "TIGER",				fbx_fname = "A_TIGER",												},
			TIGER_B 			= 	{	path = "TIGER",				fbx_fname = "A_TIGER",												},


			G_CLAM_01			=	{	path = "CORAUX/G_CLAM",				fbx_fname = "G_CLAM_01",									},
			G_CLAM_02			=	{	path = "CORAUX/G_CLAM",				fbx_fname = "G_CLAM_02",									},
			G_CLAM_03			=	{	path = "CORAUX/G_CLAM",				fbx_fname = "G_CLAM_03",									},
			G_CLAM_04			=	{	path = "CORAUX/G_CLAM",				fbx_fname = "G_CLAM_04",									},
			G_CLAM_05			=	{	path = "CORAUX/G_CLAM",				fbx_fname = "G_CLAM_05",									},
			G_CLAM_06			=	{	path = "CORAUX/G_CLAM",				fbx_fname = "G_CLAM_06",									},
			ALCYON_01			=	{	path = "CORAUX/ALCYON",				fbx_fname = "MATTE_KC_ALCYON_01",							},
			ALCYON_02			=	{	path = "CORAUX/ALCYON",				fbx_fname = "MATTE_KC_ALCYON_02",							},
--pavona slow all
--			PAVONA				=	{	path = "CORAUX/PAVONA",				fbx_fname = "PAVONA",										},
--			PAVONA_FISH			=	{	path = "CORAUX/PAVONA_FISH",		fbx_fname = "C_LADY_BL_PIJ_LADY BL_GR",						},
			PORITE				=	{	path = "CORAUX/PORITE",				fbx_fname = "PORITE",										},
			TURBIN				=	{	path = "CORAUX/TURBIN",				fbx_fname = "TURBIN",										},
			TURBIN_FISH			=	{	path = "CORAUX/TURBIN_FISH",		fbx_fname = "C_LADY_GREEN_PIJ_LADY GR",						},
			XMAS_01				=	{	path = "CORAUX/XMAS",				fbx_fname = "A_XMAS",										},
			XMAS_02				=	{	path = "CORAUX/XMAS",				fbx_fname = "A_XMAS",			tex_name = "XMAS_02"		},
			XMAS_03				=	{	path = "CORAUX/XMAS",				fbx_fname = "A_XMAS",			tex_name = "XMAS_03"		},
			XMAS_04				=	{	path = "CORAUX/XMAS",				fbx_fname = "A_XMAS",			tex_name = "XMAS_04"		},
		--	ANM					=	{	path = "CORAUX/ANM",				fbx_fname = "anemone_cache_4",								},
			}

		self.fbx_group =
			{
			boids =
			 	{	"GUN", "DARK_GUN", "BARAK", "LADY_GREEN", "LADY_BL_M", "LADY_BL_F", "BANNER", "ANCHOVY", "ANCHOVY_4", "ANCHOVY_7",
					"SIMPLE", "CLEAN", "CLOWN", "SURGERY", "CORNU", "HACHETTE", "LUTJ", "EMPEROR", "FLASH", "GRACE", "CARANG", "BAT", "BUTTER","CONVICT",
					"DOLPH", "EAGLE", "RED_F", "RED_M", "SHRK_WHAL", "YEL_TANG", "LIPS_A", "LIPS_B", "MANTA", "PARROT", "PARROT_BL_F", "PARROT_BL_M",
					"TURTL", "TURTL_BB", "CORNET", "MAORI", "POTATO", "TRAVEL", "TRIGGER", "CARPET", "LION", "SUCKER", "TIGER", "TITAN", "WHITETIP", "WHITETIP_BB",
					"CUTTLE", "PELAGIA"
				},
			animations = { "TURTLE", "WHALE", "MANTA_EAT", "MANTA_PARADE", "MANTA_CLEAN", "EEL", "C_TITAN",
							"MRAY_C_PIJ", "DOLPH_C", "POTATO_CLEAN", "SHRK_WHAL_KR", "SHRK_WHAL_KC", "SHRK_WHAL_SO", "TIGER_KR", "TIGER_B"
						},
			drama = { "MRAY_D_PIJ", "PARROT_D_PIJ", "TURTLE_D", "DOLPH_DN2", "WHITETIP_DJ5", "TIGER_CHASSE_KR", "TIGER_CHASSE_KC",
						 "WHALE_DJ", "WHALE_BB", "WHITETIP_DJ5_B", "WHITETIP_DJ5_C", "PARROT_D_PIJ_B", "PARROT_D_PIJ_C", "PARROT_D_PIJ_D"
					},
			tapestry = { "G_CLAM_01", "G_CLAM_02", "G_CLAM_03", "G_CLAM_04", "G_CLAM_05", "G_CLAM_06",
--pavona slow all		"PAVONA_FISH",
						"TURBIN_FISH", "XMAS_01", "XMAS_02", "XMAS_03", "XMAS_04"
					},
			tapestry_static = { "ALCYON_01", "ALCYON_02", "PORITE",
--pavona slow all		"PAVONA",
						 "TURBIN", "ANM"
					},
			}

		-- assign FBXs to Machines
		self.pc_fbxs = {}
		if true or self:is_pc_dev() then
			-- Not a bertha, probably a Dev machine
			self:print( "Dev PC" )
			self.pc_fbxs =
				{
					b_preload = true,
					groups = { self.fbx_group.boids, self.fbx_group.animations,
					 			self.fbx_group.drama, self.fbx_group.tapestry, self.fbx_group.tapestry_static },
				}
		else
			self.pc_fbxs =
			{
				-- preload = false,
				-- count = 3,
			-- 	{ index = 1, name = "GUN" },
			-- 	{ index = 2, name = "DARK_GUN" },
			-- 	{ index = 3, name = "BARAK" },
			-- --	{ index = 4, name = "LADY_GREEN" },
			-- --	{ index = 5, name = "LADY_BL" },
			-- --	{ index = 6, name = "MANTA" },
			-- --	{ index = 7, name = "BANNER" },
			}
		end
		self.fbx_data = fbx_data

		self:assign_fbx_all()
	end
	return fbx_data
end

function MONACO_AQUA:assign_fbx_all()
	if not self.texs_no_normal_path then
		self.texs_no_normal_path = "Monaco/".."Texture/"
	end
	local fbxs = self.pc_fbxs
	if fbxs then
		for ig,group in IPAIRS(fbxs.groups) do
			for i,name in IPAIRS(group) do
				self:print( "Assigning FBX "..name )
				if self.fbx_data[ name ] then
					--todomona this code is duplicated
					local m
					--= self:get_meu_by_name_no_error( name )
					--if not m then
						m = self:get_meu_by_name_no_error( "fbx_"..name )
					--end
					if m then
						--m:assign_fbx( name, fbxs.preload )
						m:assign_fbx( name, false )
					else
						self:print( "Can't get FBX meu "..name )
					end
				else
					self:print( "FBX meu not defined ".. name )
				end
			end
		end
	else
		-- no config for this machine
	end
end

--todo do an APP generic mecanism
function MONACO_AQUA:enable_img_scale_on_load()
	local s_resize = self:is_pc_dev() and self.s_image_resize_dev or self.s_image_resize
	self:print( "s_resize "..s_resize )
	if s_resize == 1 then
		self:print( "Load quarter texture")
		aaa.img.scale_on_load_set( 0.25, 512,  "QUARTER" )
	elseif s_resize == 2 then
		self:print( "Load half texture")
		aaa.img.scale_on_load_set( 0.5,  1024, "HALF" )
	end
end

function MONACO_AQUA:disable_img_scale_on_load()
	aaa.img.scale_on_load_disable()
end

function MONACO_AQUA:is_load_fbx_textures()
--	return self.b_load_fbx_texs
	return true
end

function MONACO_AQUA:get_fbx_matte( b_force )
	if app:is_tablet() then return end

	if not self.texs_no_normal_path then
		self.texs_no_normal_path = "Monaco/".."Texture/"
	end
	local fbx_mattepainting = self.fbx_mattepainting
	if not fbx_mattepainting or b_force then
		self.matte_path = "Monaco/".."MATTEPAINTING/__validated/"
		fbx_mattepainting =
		{
			KR_DAY = {
				path = "KR_DAY",
				fbx_fname = "RECIF_KR_MATTEPAINTING_200712b",
				b_night = false,
				layers = {
					-- need to draw different layers in the right order because of the alpha
				--	{ mesh_name = "MATTE_KR_BG_Sibesoin", 	},
					{ mesh_name = "MATTE_KR_Plan_04", uv_offset = { x = .0, y = 0.13 }	},
					{ mesh_name = "Plane001" },
					{ mesh_name = "MATTE_KR_Plan_03", uv_offset = { x = .0, y = 0.13 }	},
					{ mesh_name = "MATTE_KR_PAVONA_TURBINS_KR1_KR4", uv_offset = { x = .0, y = 0.13 }	},
					{ mesh_name = "MATTE_KR_CORAUX_KR1_KR2", uv_offset = { x = .0, y = 0.13 }	},
					{ mesh_name = "MATTE_KR_Plan_Cythere_03", uv_offset = { x = .0, y = 0.13 }	},
					{ mesh_name = "MATTE_KR_Plan_Cythere_02", uv_offset = { x = .0, y = 0.13 }	},
					{ mesh_name = "MATTE_KR_Plan_Cythere_01", uv_offset = { x = .0, y = 0.13 }	},
					{ mesh_name = "MATTE_KL_ALCYON_03", uv_offset = { x = .0, y = 0.13 }	},
					{ mesh_name = "MATTE_KR_CHAPEAU_CYTHER", uv_offset = { x = .0, y = 0.13 }	},
					{ mesh_name = "MATTE_KL_Plan_Fire_back_01", 	},
					{ mesh_name = "MATTE_KL_Plan_Fire_Front_01", 	},
					{ mesh_name = "MATTE_KL_Plan_Fire_back_03", 	},
					{ mesh_name = "MATTE_KL_Plan_Fire_Front_03", 	},
					{ mesh_name = "MATTE_KR_Plan_02", uv_offset = { x = .0, y = 0.13 }	},
					{ mesh_name = "MATTE_KL_ALCYON_01", 	},
					{ mesh_name = "MATTE_KL_ALCYON_02", 	},
					{ mesh_name = "MATTE_KR_BRAIN_PORITS", uv_offset = { x = .0, y = 0.13 }	},
					{ mesh_name = "MATTE_KL_Plan_Fire_back_02", 	},
					{ mesh_name = "MATTE_KL_Plan_Fire_Front_02",	},
					{ mesh_name = "MATTE_KR_Plan_01", uv_offset = { x = .0, y = 0.13 }	},
				},
			},
			KC_DAY = {
				path = "KC_DAY",
				fbx_fname = "RECIF_KC_MATTEPAINTING_200709b",
				b_night = false,
				layers = {
					-- need to draw different layers in the right order because of the alpha
				--	{ mesh_name = "MATTE_KC_BG_Sibesoin",	tex_name = "MATTE_KC_BG_Sibesoin"					},
					{ mesh_name = "MATTE_KC_Plan_03",		tex_name = "MATTE_KC_Plan_03", uv_offset = { x = .0, y = 0.069 }					},
					{ mesh_name = "MATTE_KC_Plan_02",		tex_name = "MATTE_KC_Plan_02", uv_offset = { x = .0, y = 0.069 }					},
					{ mesh_name = "MATTE_KC_Plan_01",		tex_name = "MATTE_KC_Plan_01", uv_offset = { x = .0, y = 0.069 } },
					{ mesh_name = "MATTE_KC_ALCYON_01_b",	tex_name = "MATTE_KC_ALCYON_01"						},
					{ mesh_name = "MATTE_KC_ALCYON_02_b", 	tex_name = "MATTE_KC_ALCYON_02"						},
					{ mesh_name = "MATTE_KC_ALCYON_02_a",	tex_name = "MATTE_KC_ALCYON_02"						},
					{ mesh_name = "MATTE_KC_ALCYON_01_a", 	tex_name = "MATTE_KC_ALCYON_01"						},
				},
			},
			KL_DAY = {
				path = "KL_DAY",
				fbx_fname = "RECIF_KL_MATTEPAINTING_200714",
				b_night = false,
				layers = {
				--	{ mesh_name = "MATTE_KL_BG_Sibesoin" },
					{ mesh_name = "MATTE_KL_Plan_07", uv_offset = { x = .0, y = 0.13 } },
					{ mesh_name = "MATTE_KL_Plan_05", uv_offset = { x = .0, y = 0.13 }},
					{ mesh_name = "MATTE_KL_Plan_06", uv_offset = { x = .0, y = 0.12 } },
					{ mesh_name = "MATTE_KL_Plan_04", uv_offset = { x = .0, y = 0.13 }},
					{ mesh_name = "Plane001" },
					{ mesh_name = "MATTE_KL_Plan_03", uv_offset = { x = .0, y = 0.12 } },
					{ mesh_name = "MATTE_KL_Plan_03_GROTTE", uv_offset = { x = .0, y = 0.12 } },
					{ mesh_name = "MATTE_KL_Plan_Cythere_03", uv_offset = { x = .0, y = 0.13 } },
					{ mesh_name = "MATTE_KL_Plan_Cythere_02", uv_offset = { x = .0, y = 0.13 } },
					{ mesh_name = "MATTE_KL_Plan_Cythere_01", uv_offset = { x = .0, y = 0.13 } },
					{ mesh_name = "MATTE_KL_Plan_Fire_back_03" },
					{ mesh_name = "MATTE_KL_Plan_Fire_Front_03" },
					{ mesh_name = "MATTE_KL_Plan_Fire_back_02" },
					{ mesh_name = "MATTE_KL_Plan_Fire_Front_02" },
					{ mesh_name = "MATTE_KL_Plan_Fire_back_01" },
					{ mesh_name = "MATTE_KL_Plan_Fire_Front_01" },
					{ mesh_name = "MATTE_KL_Plan_02", uv_offset = { x = .0, y = 0.13 } },
					{ mesh_name = "MATTE_KL_Plan_DIVERS_ALBEDO", uv_offset = { x = .0, y = 0.13 } },
					{ mesh_name = "MATTE_KL_ALCYON_02" },
					{ mesh_name = "MATTE_KL_ALCYON_01" },
					{ mesh_name = "MATTE_KL_Plan_02_GROTTE", uv_offset = { x = .0, y = 0.13 } },
					{ mesh_name = "MATTE_KL_Plan_01", uv_offset = { x = .0, y = 0.13 } },
				},
			},
			SO_DAY = {
				path = "SO_DAY",
				fbx_fname = "RECIF_SO_MATTEPAINTING_200709b",
				b_night = false,
				layers = {
				--	{ mesh_name = "MATTE_SO_BG_Sibesoin" },
					{ mesh_name = "MATTE_SO_Plan_02" },
					{ mesh_name = "MATTE_SO_Plan_01" },
					{ mesh_name = "MATTE_SO_FIRES" },
					{ mesh_name = "MATTE_SO_SPONGES" },
					{ mesh_name = "MATTE_SO_PAVONA" },
				},
			},
			KC_NIGHT = {
				path = "KC_NIGHT",
				fbx_fname = "RECIF_KC_MATTEPAINTING_200319_NIGHT",
				b_night = true,
				layers = {
			--		{ mesh_name = "KC_BG_Si_besoin_NIGHT", tex_name = "MATTE_KC_BG",				},
					{ mesh_name = "KC_PLAN_03_RIGHT_NIGHT", tex_name = "MATTE_KC_Plan_03_RIGHT",	},
					{ mesh_name = "KC_PLAN_03_LEFT_NIGHT", tex_name = "MATTE_KC_Plan_03_LEFT",	},
					{ mesh_name = "RECIF_LOW_NIGHT", tex_name = "MATTE_KC_Plan_02",		uv_offset = { x = .0, y =  0.06914 } },
					{ mesh_name = "KC_PLAN_01_NIGHT", tex_name = "MATTE_KC_Plan_01",		},
				},
			},
			KL_NIGHT = {
				path = "KL_NIGHT",
				fbx_fname = "RECIF_KL_MATTEPAINTING_200314_NIGHT",
				b_night = true,
				layers = {
			--		{ mesh_name = "BG_Si_besoin_NIGHT", tex_name = "MATTE_KL_BG",			},
					{ mesh_name = "Plan_07_1_NIGHT", tex_name = "MATTE_KL_Plan_07",	},
					{ mesh_name = "Plan_07_2_NIGHT", tex_name = "MATTE_KL_Plan_07",	},
					{ mesh_name = "plan_06_NIGHT", tex_name = "MATTE_KL_Plan_06",	uv_offset = { x = .0, y = 0.13 } },
					{ mesh_name = "Plan_05_NIGHT", tex_name = "MATTE_KL_Plan_05",	},
					{ mesh_name = "RECIF_LOW_NIGHT", tex_name = "MATTE_KL_Plan_04",	uv_offset = { x = .0, y = 0.13 } },
					{ mesh_name = "Plan_03_NIGHT", tex_name = "MATTE_KL_Plan_03",	},
					{ mesh_name = "Plan_02_NIGHT", tex_name = "MATTE_KL_Plan_02",	},
					{ mesh_name = "Plan_01_NIGHT", tex_name = "MATTE_KL_Plan_01",	},
				},
			},
			SO_NIGHT = {
				path = "SO_NIGHT",
				fbx_fname = "RECIF_SO_MATTEPAINTING_200314_NIGHT",
				b_night = true,
				layers = {
			--		{ mesh_name = "BG_Si_besoin_NIGHT", tex_name = "MATTE_SO_Plan_03",	},
					{ mesh_name = "RECIF_LOW_NIGHT", tex_name = "MATTE_SO_Plan_02",	},
					{ mesh_name = "Plan_01_NIGHT", tex_name = "MATTE_SO_Plan_01",	},
				},
			},
		}
		self.fbx_mattepainting = fbx_mattepainting

		for name, t in PAIRS(self.fbx_mattepainting) do
			local m
			-- = self:get_meu_by_name_no_error( name )
			--if not m then
				m = self:get_meu_by_name_no_error( "FbxMatte_"..name )
			--end
			if m then
				m:assign_fbx( name, false )
				--m:assign_fbx( t.fbx_name or name, false )
			else
				self:box_debug( "Can't get FBX meu "..name )
			end
		end
	end
	return fbx_mattepainting
end

function MONACO_AQUA:define_all( b_force )
	--self:box_debug( "MONACO_AQUA:define_all()" )
	if b_force then
		aaa.print_fn()
	end

	if self.b_define_all_done and self.b_configure_pc_done == false then
		self:configure_pc()
		self.b_configure_pc_done = true
	end

	if (b_force or not self.b_define_all_done) and self.define_room then
		self:disable_img_scale_on_load()
		self:define_room()
		self:define_cams()
		--self:define_fbx()
		--self:box_debug( "define_fbx_matte()" )
		--self:define_fbx_matte()
		self.b_define_all_done = true
		self.b_configure_pc_done = false
	end
end

--todo move to APP_GP or replace by activate_meu
function MONACO_AQUA:switch_mu( name, b_on, b_verbose )
	if type(b_on)=="boolean" then
		b_on = b_on and 1 or 0
	end
	--self:box_debug( "switch_mu "..name.." "..b_on )
	local meu = app:get_meu_by_name_cached( name )
	if b_verbose then
		self:box_debug( name.." "..b_on.." meu is ".. meu )
	end
	if meu then
		meu:set_mu_value( b_on )
	end
end

function MONACO_AQUA:configure_floating_point()
--	if app:is_pc_dev() then return end
	local b = (app:is_so()==true) and (not app:is_tablet())
--	self:box_debug( app:is_so().." "..app:is_tablet().." "..b )
	self:switch_mu( "Ref_Fbo_F5",		b )
	self:switch_mu( "FPlancton_SO",		b )
	self:switch_mu( "TEXCompo_1",		b )
end

--	PEDAGO
--
function MONACO_AQUA:configure_pedago()
--	if app:is_pc_dev() then return end
	local b = app:is_kl() or app:is_kr() or app:is_pc_dev()
	self:switch_mu( "MonacoPedago_1", b )
end
function MONACO_AQUA:get_pedago_meu()
	return app:get_meu_by_name_cached( "MonacoPedago_1" )
end
function MONACO_AQUA:set_pedago( location, slot_id, fname )
	if self:is_location( location ) then
		local meu = self:get_pedago_meu()
		if meu then
			meu:set_pedago( slot_id, fname )
		end
	end
end
function MONACO_AQUA:set_pedago_general( b )
	local meu = self:get_pedago_meu()
	if meu then
		meu:set_pedago_general( b )
	end
end

function MONACO_AQUA:configure_passes()
	local meu = app:get_meu_by_name_cached( "MuBegin_1" )
	local b_dif = self:is_pc_dif()
	if meu then
		meu:set_pass_nb( self:is_pc_dif() and 2 or 1 )	--no shadow pass on master or tablet
	end
	self:switch_mu( "NdcFxaa_1",		b_dif )
	self:switch_mu( "NdcDof_1", 		b_dif )
	self:switch_mu( "NdcTransparent_1", b_dif )
	if self:is_tablet() then
		self:switch_mu( "NdcFogMona_1",			false )
		self:switch_mu( "NdcPost_1",			false )
		self:switch_mu( "LightPassV1_1",		false )
		self:switch_mu( "MonacoTitre_1",		false )
		self:switch_mu( "PlanctonMona_White",	false )
		self:switch_mu( "Pelagia_1",			false )
	end
end

function MONACO_AQUA:configure_pc()
	aaa.set_lua_silent( not self:is_pc_dev() )
	self:configure_network()
	self:configure_matte()
	self:configure_tapestry()
	self:configure_fbx_anim()
	self:configure_capture()
	self:configure_cam()
	self:pick_screen_all()
	self:configure_passes()
	self:configure_out()
	self:configure_yocto()
	self:configure_floating_point()
	self:configure_pedago()
	self:set_party( false )

	-- make sure all TankFish are off
	if self:is_tablet() then
		self:change_tank( "pipo" )
	end
	self.b_white_screens = false
end


function MONACO_AQUA:configure_network()
	local ref = aaa.net.ref
	--local net_active_ref = param.get_ref( ref.obj, "active" )
	if self:is_pc_dev() then
		--self:print( "not active")
		param.set( ref.obj, "active", 0 )
	else
		--self:print( "active")
		param.set( ref.obj, "active", 1 )
	end
end

function MONACO_AQUA:configure_yocto()
	--todo aqua : must be changed to MAS
	self:activate_meu( "YoctoPuce_1", self:is_pc_mas() and true or false )
--	self:activate_meu( "YoctoPuce_1", false )
end


function MONACO_AQUA:configure_capture()
	local b_on = false
	if self:is_so() and (not self:is_tablet()) then
		local meu_capture = self:get_meu_by_name_no_error( "SOCapture_1" )
		if not meu_capture then
			self:print_error( "Couldn't find MEU : ".."SOCapture_1" )
		else
			meu_capture:set_capture_active( true )
		end
		b_on = true
--	elseif not self:is_pc_dev() then
	end
	self:switch_mu( "Ref_Fbo_F1", b_on )
	self:switch_mu( "SOCapture_1", b_on )
	self:switch_mu( "SOOpenCV_1", b_on )
	self:switch_mu( "SOAnals_1", b_on )
	self:switch_mu( "Ref_Fbo_F4", b_on )

	if self:is_pc_mas() then
		self:switch_mu( "Dir_Etienne", false )
	elseif not self:is_pc_dev() then
		self:switch_mu( "Dir_Etienne", false )
	end
end

function MONACO_AQUA:configure_fbx_anim()
	if self:is_tablet() then
		self:activate_meu( "Dir_FbxAnim", false )
	elseif not self:is_pc_mas() and not self:is_pc_dev() and not self:is_tablet() then
		self:activate_meu( "Dir_FbxAnim", true )
	end
end

function MONACO_AQUA:configure_tapestry()
	if self:is_tablet() then					self:activate_meu( "Clam_Tapestry", false )
	elseif self:is_so() then					self:activate_meu( "Clam_Tapestry", true )
	elseif not self:is_pc_dev() then			self:activate_meu( "Clam_Tapestry", false )
	end

	if self:is_tablet() then					self:activate_meu( "Tapestry_1", false )
	elseif self:is_kr() or self:is_kl() then	self:activate_meu( "Tapestry_1", true )
	elseif not self:is_pc_dev() then			self:activate_meu( "Tapestry_1", false )
	end

	if self:is_tablet() then					self:activate_meu( "Eel_Tapestry", false )
	elseif self:is_location( "KR4") then		self:activate_meu( "Eel_Tapestry", true )
	elseif not self:is_pc_dev() then			self:activate_meu( "Eel_Tapestry", false )
	end
end


function MONACO_AQUA:configure_matte()
--	self:box_debug( "configure_matte() is pc dif "..self:is_pc_dif().." kl: "..self:is_kl().." kr: "..self:is_kr() )
	self:activate_meu( "Dir_Matte3D",		self:is_pc_dif()	)
	self:activate_meu( "FbxMatte_KL_DAY",	self:is_kl() 		)
	self:activate_meu( "FbxMatte_KR_DAY",	self:is_kr() 		)
	self:activate_meu( "FbxMatte_KC_DAY",	self:is_kc()		)
	self:activate_meu( "FbxMatte_SO_DAY",	self:is_so()		)
end

--pick_screen
function MONACO_AQUA:configure_cam()
	if self:is_pc_dif() then
		local gbuf = self:get_meu_by_name_no_error( "Fbo_GBuffer" )
		if gbuf then
			local bu = gbuf.ui.bu_cam_sel
			bu:set_value( 16 )
		end
	end
end

function MONACO_AQUA:pick_screen_all()
	local loc = self:get_location()
	if loc then
		local meu_cam = self:get_meu_by_name_no_error( "MonacoCam_1" )
		if not meu_cam then
			self:print_error( "Couldn't find MonacoCam_1" )
			return
		end
		meu_cam:pick_screen( loc )
	end
	if self:is_pc_dif() or self:is_pc_mas() then
		ga:set_ui_on_screen(1)
		ga:set_ui_group_active_all( false )
		ga:get_bu_wiz("cam"):set_value( false )
	end
end

function MONACO_AQUA:configure_out()
	local b_dif = self:is_pc_dif()
	local b_mas = self:is_pc_mas()
	if b_dif or self:is_tablet() then
		ga:force_fullscreen()
	end
	if self:is_tablet() then
		self:activate_meu( "Dir_Out", 	false )
		self:activate_meu( "Out_Dif", 	false )
	else
		self:activate_meu( "Dir_Out", 	b_dif or b_mas )
		self:activate_meu( "Out_Dif", 	not b_dif )
		for i=1,10 do
			local name = self:get_location(i)
			if name then	--todo refine: test done for pc_dev
				self:activate_meu( "Dir_Out"..name, self:is_location( name ) )
			end
		end
	end

end

function MONACO_AQUA:set_screens_white( b )
	self.b_white_screens = b
--	local obj = aaa.obj.get_from_top_by_class( "tex_video_master" )
--	param.set( "obj, "audio_allow", b and 0 or 1 )
end

function MONACO_AQUA:get_screens_white()
	return self.b_white_screens or false
end

function MONACO_AQUA:update_dolph_animation( animation_name, start_time )
	local meu_fbx = self:get_meu_by_name_no_error( "DOLPH_CA" )
	if meu_fbx then
		meu_fbx:net_fbx_animation( animation_name, start_time )
	end
end

function MONACO_AQUA:update_whale_animation( animation_name, start_time )
	local meu_fbx = self:get_meu_by_name_no_error( "WHALE_ca" )
	if meu_fbx then
		meu_fbx:net_fbx_animation( animation_name, start_time )
	end
end
