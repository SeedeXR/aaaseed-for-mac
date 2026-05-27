local function appvideomin_pages_define()
	local ratio_x = 1.77
	local taille_x = 3 * ratio_x;
	local taille_y = 3;
	local bu
	local noms

	local bus = BUS:create( "AppVideoMin" )
	bus:init_begin_add_to_ga()

	--START

--	VIDEOS:set_dir_media( "Maashow" )
--	VIDEOS:set_dir( "//192.168.1.42/MaaPrivate/SexStock/CulMovie/FilmCul/FetishNylon/" )

--	bus:add_dir_only_video( "//192.168.1.42/MaaPrivate/SexStock/Fetish" )
--	bus:add_dir( "M:/AAAData/Cul/Movie" )
--	bus:add_dir_only_video( "O:/Download" )
--	bus:add_dir_only_video( "//192.168.1.42/MaaPrivate/SexStock/Dorcel" )
	bus:add_dir_only_video( aaa.dir.make_media_path("Maa/Cul/Movie/cul bis") )


--	local main_boucle = "Video_HD/Planets.avi"


	bus:make_bus_loop()

	--the scrolling bu is on the bottom
	bus:add_scroll( -4, 0 )

	bus:init_end()
end

if IS_BUSS_OPEN() then
	appvideomin_pages_define()
end
