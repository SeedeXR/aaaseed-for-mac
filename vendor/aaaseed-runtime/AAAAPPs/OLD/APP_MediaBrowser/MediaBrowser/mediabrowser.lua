local function aaamedia_update_before( bus )
	--aaa.print( "bus_update_before( "..bus.." )" )
end

local function aaamedia_update_after( bus )
--	aaa.print( "bus_update_after( "..bus.." )" )
	--aaa.print( "bus_update_after( "..bus.." )" )
end

local function media_browser_pages_define_inside()
	local ratio_x = 1.77
	local taille_x = 3 * ratio_x;
	local taille_y = 3;
	local bu
	local noms

	local bus = BUS:create( "MediaBrowser inside" )
	bus:init_begin()
	bus:set_bu_pos_load_save( true )

		bus:set_page( 0 )
		local dir_name = app.media_dir_rel.."Chanel/Parfum_2013/N5"
		bus:add_dir( dir_name, true, true, true, true )

	--	bus:add_page_buttons()
		bus:set_page(0)
		bus:trim_page_end()

	--	bus:make_bus_loop()

		--the scrolling bu is on the bottom
		bus:add_bu_page_trs( BU:create( nil, {0,-2., 10000,1.} ) )

	bus:init_end()

	bus:set_function( "update_before",	aaamedia_update_before	)
	bus:set_function( "update_after",		aaamedia_update_after	)

	bus:set_transfo( 16,9, 8,4.5 )

	return bus
end

local function media_browser_pages_define()
	local bus = BUS:create( "MediaBrowser" )
	bus:set_bu_pos_load_save( true )
--	self.ui.bus = bus
	bus:set_active( true )
	bus:init_begin_add_to_ga()

		local bus_down = media_browser_pages_define_inside()
		bus:add_window( "win Mediabrowser", bus_down,	{0,0, 8,4.5} )
		bus_down:set_transfo( 8,4.5 )

	bus:init_end()

	--this is where we start
	bus:set_page(0)

--	bus:set_method( "draw_before",	self,	"draw_background"	)
--	bus:set_method( "draw_after",	self,	"draw_foreground"	)

--	ga:register_ui_group( bus )
--	ga:set_ui_group_active( false )

	return bus

end

if IS_BUSS_OPEN() then
	GABUIN.set( nil, false, false, false, 4 )
	aaa.lua.global.declare( "media_browser" )
	media_browser = APP.CREATE_INST( APP, "MediaBro" )
	media_browser:init_lang_def()
	media_browser:clear_init_done()
	media_browser_pages_define()
	GABU.do_key_custom = GABU.do_key_irtouch
end

