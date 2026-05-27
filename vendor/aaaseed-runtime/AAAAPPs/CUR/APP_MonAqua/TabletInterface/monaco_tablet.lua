
local function format_time( time )
	local str = ""
	if time then
		local seconds = 0
		local minutes = 0
		local hours = 0
		local mins = 0
		if time >= 60 then
			seconds = time - math.floor( time / 60. ) * 60.
			mins = ( time - seconds ) / 60
		else
			seconds = time
		end
		if mins > 60 then
			minutes = mins - math.floor( mins / 60. ) * 60.
			hours = ( mins - minutes ) / 60
		else
			minutes = mins
		end
		if hours > 0 then
			str = string.format( "%02d:%02d:%.2f", hours, minutes, seconds )
		else
			str = string.format( "%02d:%04.2f", minutes, seconds )
		end
	end
	--	self:print( "format_time "..str )
	return str
end

function MONACO_AQUA:define_ui_customer()
	local bus = BUS:create( "MONACO_AQUA_Tablet" )
	bus:set_bu_pos_load_save( false )

	bus:set_active( true )
	bus:init_begin_add_to_ga()
	local ui = self.ui
		local bu
		local ix = -3.
		local iy = 2.

		local SY = .35

		bu = bus:add_selector( "TAB", {2.8,2, 2,SY} )
			bu:set_nb( 3 )
			bu:set_value(2)
			bu:set_item_text( 1, "Pedago", "Control", "Plus" )
			bu:set_text_draw( false )
			bu:set_pos_load_save( false )
			bu:set_method_on_value_change( self, "change_tab", bu )

		ui.bu_name = bus:add_text_info(	"Scene", 	{ix,iy,			.5, .25} )
			:set_text( "Scene Name"	)
			:set_value_load_save( false ):set_ui_active( false )
		ui.bu_info = bus:add_text_info(	"Temps", 	{ix + 1.2,iy,	1.2, .25} )
			:set_text( "Tine Info" )
			:set_value_load_save( false ):set_ui_active( false )

		local SX = SY * 2.
		bu = bus:add_button(			"Soiree",	{ix + 2.4,iy,	SX,SY}		)
			:set_value_load_save( false )
			bu:set_value( false )
			bu:set_method_on_value_change( self, "set_party_tablet", bu )

		self.ui.bu_party = bu

		bus:set_tab_key( "Control" )

		local x = -2.5
		local sx = 1.2
		local y = -.5
		local sy = .28
		local D = .04
		local dy = sy + D

		local x_left = 2.2
		bu = bus:add_button(	"Muet",				{x_left,1,		sy,sy} )
			bu:set_method_on_click( self, "mute_volume" )
			bu:set_value_load_save( false )
		self.ui.bu_mute = bu

		bu = bus:add_button(	"Extinction Auto",	{x_left,.5,		sy,sy} )
			bu:set_value( true )
			bu:set_method_on_click( self, "set_shutdown_auto" )
			bu:set_value_load_save( false )
			self.ui.bu_shutdown = bu
			self:set_shutdown_auto()

		x_left = 2.8
		bu = bus:add_trig(		"Allumer PCs",		{x_left,-1.,	sx,sy} )
			bu:set_method_on_click( self, "wake_pc" )
			bu:set_value_load_save( false )
		self.ui.bu_extinction = bu
		bu = bus:add_trig(		"Eteindre PCs",		{x_left,-1.5,	sx,sy} )
			bu:set_method_on_click( self, "quit_pc" )
			bu:set_value_load_save( false )
		self.ui.bu_extinction = bu

		bu = bus:add_trig(		"Quitter",			{x_left,-2.3,	sx,sy} )
			bu:set_method_on_click( self, "quit_interface" )
			bu:set_value_load_save( false )
		self.ui.bu_quit = bu

		local function add_button_trig( rect, text )
			local bu =  bus:add_trig( text, rect )
			bu:set_method_on_click( self, "click_seq", "Bu", text )
			bu:set_value_load_save( false )
			return bu
		end

		sx = 1.9
		sy = .4
		dy = .2
		iy = iy - 0.6
		local iy_begin = iy
		x = -2.3
		for i=1,7 do
			add_button_trig(	{x,iy,		sx,sy},		self.scenes_name[i]	)
			iy = iy - sy - dy
		end
		x = x + sx + dy
		iy = iy_begin
		for i=8,13 do
			add_button_trig(	{x,iy,		sx,sy},		self.scenes_name[i]	)
			iy = iy - sy - dy
		end

		bus:set_tab_key( "Pedago" )

		sy = .25
		dy = .12
		sx = 1.4
		local dx = dy
		x = -3.2
		y = 1.5

		local bu = bus:add_button( "Fiches", {.2,y+sy*2, sy*1.5,sy*1.5} )	--.." "..fic.location )
			--bu:set_method_on_click( self, "click_seq", "Bu", text )
			bu:set_method_on_click( self, "click_on_fiches", bu )
			bu:set_value_load_save( false )

		local function add_button_pedago( r, c, name, fiche )
			local bu = bus:add_button( name, {x+(sx+dx)*(c-1),y-(sy+dy)*(r-1), sy,sy} )	--.." "..fic.location )
			bu:set_method_on_click( self, "click_on_fiche", bu, fiche )
			bu:set_value_load_save( false )
			fiche.bu = bu
			return bu
		end

		local fic = self:get_fiches()
		local nb = 0
		local names = {}
		local elts = {}
		for name,elt in pairs_sorted(fic) do
			nb = nb + 1
			names[nb] = name
			elts[nb] = elt
		end
		--self:box_debug( nb )
		local col_nb = 4
		local row_nb = math.ceil( nb/col_nb )
		local i = 0
		for r=1,row_nb do
			for c=1,col_nb do
				i = i+1
				if i>nb then
					break
				end
				add_button_pedago(	r, c, names[i], elts[i] )
			end
		end
		-- bu = bus:add_trig(	"test Maa",		{2.4,iy - 1.5,	sy,sy} )
		-- 	--bu:set_method_on_click( self, "quit_pc" )
		-- 	bu:set_value_load_save( false )

	bus:init_end()

	self.__bus_ui_customer_for_tab = bus

	bus:activate_bu_by_tab_key( "Control" )

	return bus
end
function MONACO_AQUA:disable_fiches_execpt( fiche )
	for name,f in pairs(self:get_fiches()) do
		if f.location==fiche.location and f.slot==fiche.slot and f~=fiche then
			f.bu:set_value(0)
		end
	end
end

function MONACO_AQUA:set_party_tablet( balue )
	local v  = balue:get_value()
	--aaa.print( "set_party_tablet to ".. v )
	self:send_method_verbose( "set_party", v )
end
function MONACO_AQUA:click_on_fiche( bu, fiche )
	local v = bu:get_value_as_bool()
	self:print( "click_on_fiche : "..v )
	local str =	"\""..fiche.location.."\","..fiche.slot
	if v then
		self:disable_fiches_execpt( fiche )
		str = str .. ",\""..fiche.fname.."\""
		--we disable all button with the same location and slot
	end
	self:send_method_verbose( "set_pedago", str )
end
function MONACO_AQUA:click_on_fiches( bu )
	local v = bu:get_value_as_bool()
	self:print( "click_on_fiches : "..v )
	self:send_method_verbose( "set_pedago_general", v )
end

function MONACO_AQUA:change_tab( bu )
	local v = bu:get_value()
	local key = bu:get_item_text( v )
	--self:print( "change_tab -> "..v.." "..key )
	self.__bus_ui_customer_for_tab:activate_bu_by_tab_key( key )
end

function MONACO_AQUA:sync_ui_customer()
	local ui = self.ui

	ui.bu_name:set_text( self.scene_cur=="" and "Scene" or self.scene_cur  )
	local str2 = format_time(self.scene_time).." / "..format_time(self.scene_duration)
	ui.bu_info:set_text( str2 )
end

function MONACO_AQUA:mute_volume()
	local b_mute = self.ui.bu_mute:get_value_as_bool()
	self:send_method_verbose( "net_mute_sound", b_mute )
end
function MONACO_AQUA:set_shutdown_auto()
	local b_shutdown_auto = self.ui.bu_shutdown:get_value_as_bool()
	self:send_method_verbose( "net_shutdown_allowed", b_shutdown_auto )
end

function MONACO_AQUA:click_seq( bu, name )
	self:print( "Click seq "..name )
	self:send_method_verbose( "net_change_seq", "\""..name.."\"" )
end

function MONACO_AQUA:quit_interface()
	aaa.quit_no_save()
end

function MONACO_AQUA:quit_pc()
	--self:print( str )
	if aaa.box_ask( "Attention", "Voulez-vous eteindre les PCs" ) then
		self:send_method_verbose( "net_quit_pc" )
	end
end

function MONACO_AQUA:wake_pc()
	local m = self:get_meu_by_name_no_error( "Exploitation_1" )
	if not m then
		self:print_error( "Couldn't find MEU : "..name )
	else
		m:update_wake()
	end
end

function MONACO_AQUA:get_fiches()
	return self.__fiches
end

function MONACO_AQUA:build_fiches()
	local t = {}
	self.__fiches = t

	--t.Anchovy = { fname="anchovy",	location="KR3", 	slot=2,		}
	t.Banner  	= { fname="banner", 	location="KL4", 	slot=1,		}
	t.Barak		= { fname="barak", 		location="KR3", 	slot=2,		}
	t.Bat		= { fname="bat", 		location="KR3",		slot=1,		}
	t.Butter	= { fname="butter", 	location="KL2", 	slot=2,		}	--PB 2021 Dec
	t.Carang	= { fname="carang", 	location="KL3", 	slot=1,		}
	t.Carpet	= { fname="carpet", 	location="KR2", 	slot=1,		}
	t.Clean		= { fname="clean", 		location="KR3", 	slot=1,		}
	--t.Clown	= { fname="clown", 		location="KR3", 	slot=1,		}
	--t.Convict = { fname="convict", 	location="KL4", 	slot=1,		}
	t.Cornet  	= { fname="cornet", 	location="KL3", 	slot=2,		}
	t.Cornu   	= { fname="cornu", 		location="KL4", 	slot=1,		}
	--t.Cuttle	= { fname="cuttle", 	location="KR1", 	slot=1,		}
	t.DarkGun	= { fname="darkgun", 	location="KR2", 	slot=2,		}
	t.Dolph		= { fname="dolph", 		location="KR4", 	slot=2,		}
	t.Eagle   	= { fname="eagle", 		location="KL4", 	slot=1,		}
	t.Eel     	= { fname="eel", 		location="KR4", 	slot=2,		}
	--t.Emperor = { fname="emperor", 	location="KL2", 	slot=2,		}
	--t.Flash  	= { fname="flash", 	    location="KL2", 	slot=1,		}
	t.GClam   	= { fname="gclam", 		location="KR2", 	slot=1,		}
	t.Grace   	= { fname="grace", 		location="KL2", 	slot=1,		}	--PB 2021 Dec
	--t.Gun  	= { fname="gun", 		location="KR2", 	slot=1,		}
	t.Hachette	= { fname="hachette",	location="KR2", 	slot=1,		}
	t.LadyGreen = { fname="ladygreen",	location="KR3", 	slot=2,		}
	t.Lion    	= { fname="lion", 		location="KL2", 	slot=1,		}	--PB 2021 Dec
	--t.Lips  	= { fname="lips", 		location="KR3", 	slot=1,		}
	--t.Lutj  	= { fname="lutj", 	    location="KR3", 	slot=2,		}
	t.Manta   	= { fname="manta", 		location="KL4", 	slot=1,		}
	t.Maori   	= { fname="maori", 		location="KR2", 	slot=2,		}
	t.Mray    	= { fname="mray", 		location="KL1", 	slot=2,		}
	t.Nemo    	= { fname="nemo", 		location="KR2", 	slot=2,		}
	t.Parrot  	= { fname="parrot",		location="KL4", 	slot=1,		}
	t.ParrotBl	= { fname="parrotbl",	location="KL3", 	slot=1,		}
	t.Pelagia 	= { fname="pelagia", 	location="KL1", 	slot=2,		}
	t.Potato  	= { fname="potato", 	location="KL2", 	slot=1,		}	--PB 2021 Dec
	t.RedF    	= { fname="redf", 		location="KR1", 	slot=1,		}
	t.Remora  	= { fname="remora",     location="KR3", 	slot=1,		}
	--t.Shag   	= { fname="shag", 		location="KL1", 	slot=2,		}
	t.SharkWhal = { fname="sharkwhal",	location="KR4", 	slot=2,		}
	t.Surgery 	= { fname="surgery", 	location="KR2", 	slot=1,		}
	t.Tiger   	= { fname="tiger", 		location="KL4", 	slot=1,		}
	--t.Titan   = { fname="titan", 	    location="KR3", 	slot=2,		}
	t.Travel   	= { fname="travel", 	location="KR3", 	slot=1,		}
	--t.Trigger = { fname="trigger",    location="KR3", 	slot=2,		}
	t.Turtle  	= { fname="turtle", 	location="KL4", 	slot=1,		}
	t.Whale   	= { fname="whale", 		location="KR2", 	slot=1,		}
	t.WhiteTip	= { fname="whitetip",	location="KR1", 	slot=1,		}
	t.XMas   	= { fname="xmas", 		location="KL2", 	slot=1,		}	--PB 2021 Dec
end