GA.__user_machines		=
{
	{	user = {name="Maa"},
		-- fill the name of your machines in this field in your table
		machines =	{	"MAAHEX", "MAAQUAD", "MAAREO", "MSI-3070-D", "ALIENMAA", "AAAROG", "LEGION-3070-B" },
		b_onsite = false,
	},
	{	user = {name="Louis"},	machines = { "AAAROG" },	},
	{	user = {name="Priam"},	machines = {},	},
	{	user = {name="Franz"},	machines = { "FR3950X", "FSB2"},	},		
	{	user = {name="Fred"},	machines = {	"DESKTOP-RAS5GHB" },	},
	{	user = {name="Yaniss"},	machines = {	"YANISS-PORTABLE" },	},
	{ 	user = {name="Galaad"},	machines = {},	},
	{	user = {name="Etienne"},machines = { "MSAA", "DESKTOP-FNA3B89"},	},
-- UP Saclay
	{	user = {name="Tim"},	machines = { "BIOPT076" },	},
	{	user = {name="Faycal"},	machines = { "LAPTOP-IKGGVVT3" },	},

	{	user = {name="Romain", b_color_blind=true },	--color_blind change flatlamd color for now (2023 October)
		machines = {},
	},
	{	user = {name="Julien"},	machines = { "DESKTOP-HK44II3"},	},
	{	user = {name="Dev"},
		machines =	{	"BERTA",	"TRANSUS_1", "TRANSDELL_1",
						"MBPR-PC",	"ALX-PC",	"ACD01",
						"SPAHIR1-PC", "DESKTOP-UGUOVQF",
					},
	},
	{	user = {name="Jaime Old",	b_color_blind=true},	machines =	{"LENOVO_LEGION",},	},
	{	user = {name="Jaime",		b_color_blind=true},	machines =	{"LEGION_2024_a",},	},
	{	user = {name="Abdallah"		},	machines =	{"LIGHT",},	},
	{	user = {name="Monaco"},
		machines =	{	"BERTHA-KL1",	"BERTHA-KL2",	"BERTHA-KL3",	"BERTHA-KL4",
						"BERTHA-SO", 	"BERTHA-KC", 	"BERTHA-MAS",
						"BERTHA-KR1",	"BERTHA-KR2",	"BERTHA-KR3",	"BERTHA-KR4",
						"NUCL1",		"NUCL2",		"NUCL3",		"NUCL4",
						"NUCR1",		"NUCR2",		"NUCR3",		"NUCR4",
					},
		b_onsite = true,
	},
	{	user = {name="CybStores"},
		machines =	{	"TECHNOSTOR-P13",	"TECHBOSTOR-P14",	"TECHNOSTOR-P15",
						"TECHNOSTOR-P18", 	"TECHNOSTOR-P20", 	"TECHNOSTOR-P21",
						"TECHNOSTOR-P31",	"TECHNOSTOR-P34",
						"CYBS_P03"      ,   "CYBS_P30"      ,   "CYBS_P41",
						"CYBS_P47"
					},
	},
	{	user = {name="LV_Wall"},
		machines =	{	"MSI-3070-A",	"MSI-3070-B",	-- Tokyo
--								"MSI-3070-D",
						"MSI-3070-E",	-- Hangzhou
						"MSI",							-- Dubai this is b machine renamed
					},
		b_onsite = true,
	},
	{	user = {name="Romanite"},	machines = {	"AAA-LELEG" },	b_onsite = true, ui_on_screen_id=2 },

}
GA.__host_uk_keyboard	= {	"MAAHEX", "MAAQUAD", "MAAREO", "MBPR-PC"  }
GA.__host_dev_ip_low	= { MAAREO=69, AAAROG=71, ALIENMAA=51, MAAHEX=84 }

-- GA handle this but it is read through APP see APP:is_onsite()		
function GA:is_onsite()	return self.__b_is_onsite	end
function GA:set_onsite( b )
	--self:box_debug( "GA:set_onsite( "..b.." )" )
	self.__b_is_onsite = b
end
function GA:set_onsite_by_machine( machines )	self:set_onsite( table.find_key_by_val( machines, aaa.net.machine ) and true or false )	end


function GA:do_config_user( user )
	user = user or { name="user_not_defined" }
	self:print_inverse( "User is "..user.name )
	aaa.mess.show( "Hello\n"..user.name, 6, 1 )
	aaa.flatland.set_color_blind( user.b_color_blind )
	if user.ui_on_screen_id then
		self:set_ui_on_screen( user.ui_on_screen_id )
	end
end

function GA:get_user_name()
	return self.user and self.user.name
end
function GA:is_user_name( name )
	local user_name = self:get_user_name()
	return user_name and string.lower(name) == string.lower(user_name)
end

function GA:init_by_user()
	local tab_found
	for _,tab in PAIRS(self.__user_machines) do
		if table.find_key_by_val( tab.machines, aaa.net.machine ) then
			tab_found = tab
			break
		end
	end

	if tab_found then
		--self:box_debug( "onsite is "..tab_found.b_onsite )
		if tab_found.b_onsite ~= nil then
			self:set_onsite( tab_found.b_onsite )
		end
		self.user = tab_found.user
		self.machines = tab_found.machines
	else
		self:set_onsite( true )
		self.user = {name="Unkowned"}
	end
	self:do_config_user( self.user )

	local keyboard = "France"
	if table.find_key_by_val( self.__host_uk_keyboard, aaa.net.machine ) then
		keyboard = "UK"
	end
	param.set( aaa.ref.pref, "keyboard_country", keyboard )
end