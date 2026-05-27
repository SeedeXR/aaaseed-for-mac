--	all the machine have an Ip 192.168.7.xxx
--	xxx is
--	100 for the master
--	101 to 108 for the slaves (group of 4 screens)
--	97 for the kinect machine
APP.DECLARE( "AQUA", APP )

if aaa.lua.global.get( "aqua" ) then

	--todo generalize
	--todo deal with all the balues
	BUI:set_hook_on_value_change(
		function ( bu, balue )
			local bus = bu:get_bus_up_no_error()
			if bus then
				local bu_name = bu:get_name_lowercase()
				if bu_name then
					if aaa.net.is_remote() and app:is_master() then
						local val = balue:get_value()
						local str = "if aqua then aqua:change_slave_value( \""..bus:get_name_lowercase().."\", \""..bu_name.."\", "..val.." ) end"
						bu:print( str )
						aaa.net.lua_send( 0, 0, str )
					end
				end
			end
		end
	)

	function AQUA:quit_slave_only()
		aaa.print_method()
		if not self:is_master() and self:is_slave() then
			self:print( "self:quit_slave_only() : ok this machine try to quit" )
			aaa.quit_no_save()
		end
	end
	function AQUA:hook_quit( b_shutdown, b_shutdown_reboot ) -- send as int
		if self:is_master() then
			aaa.net.update()
			--aaa.sleep( 1000 )
			local str = "aaa.print( \"bye from "..aaa.net.machine.."\" )"
			aaa.net.lua_send( 0, 0, str )
			local str = "if aqua and aqua.quit_slave_only then aqua:quit_slave_only() end"
			aaa.net.lua_send( 0, 0, str )
			aaa.net.update()
			--aaa.sleep( 2000 )
		end
		APP.hook_quit( self, b_shutdown, b_shutdown_reboot )
	end
end

--local str = "aaa.print( \"toto from "..aaa.net.machine.."\" )"
--aaa.net.lua_send( 0, 0, str )
--str = "if aqua then aqua:quit_slave_only() else print( \"zob\" ) end"
--aaa.net.lua_send( 0, 0, str )

function AQUA:change_slave_value( bus_name, bu_name, val )
	if self:is_master() then return end
	local str = "if aqua then aqua:change_slave_value( \""..bus_name.."\", \""..bu_name.."\", "..val.." ) end"
	bu:print( str )
	local bu = ga:find_bu_by_name_lowercase( bus_name, bu_name )
	if bu then
		bu:set_value( val )
	else
		self:print( "change_slave_value() don't find bu" )
	end
end

function AQUA:set_fbo_grid()
	local fbo_and_out = self.fbo_and_out

	local screen_id = 1
	for i=1,2 do
		for j=1,2 do
			fbo_and_out:set_xy( screen_id, (i-1.5)*16, (1.5-j)*9 )
			screen_id = screen_id + 1
		end
	end
	fbo_and_out:set_size_uv( 16, 9 )
	--fbo:reset_grid()
	--fbo:set_grid_nb_uv( 7, 8 )
end

function AQUA:define_pcs()
	if self.pcs then return end

	self.pcs = PCS:create( "Aqua", 8 )
	local pcs = self.pcs

	for i=1,8 do
		pcs:get_pc(i):set_machine_name( "AQUA-PC-D0"..i )
	end

	--	define screen position by pc on 13 screen by 3 wall
	pcs:get_pc(1):set_screen_names	{ "A2", "B1", "B2", "B3" }
	pcs:get_pc(2):set_screen_names	{ "C2", "D1", "C1", "C3" }
	pcs:get_pc(3):set_screen_names	{ "D3", "E2", "D2", "E3" }
	pcs:get_pc(4):set_screen_names	{ "F2", "G1", "F1", "F3" }
	pcs:get_pc(5):set_screen_names	{ "G2", "H1", "G3", "H2" }
	pcs:get_pc(6):set_screen_names	{ "I2", "J1", "I1", "I3" }
	pcs:get_pc(7):set_screen_names	{ "J2", "K1", "J3", "K2" }
	pcs:get_pc(8):set_screen_names	{ "L2", "M1", "L1", "L3" }

	--	individual data for a screen
	self.screen = { size_x_m = 1.016, size_y_m = 0.570, dist_m = 0.080, pixel_nb_x = 1366, pixel_nb_y = 766 }
end

AQUA.back_color =
{
	{	0.,	0.,	0.	},
	{	.2,	.4,	.7	},
	{	.07,.13,.2	},
	{	.6,	.6,	.6	},
	{	.2,	.2,	.8	},
	{	1.,	.0,	.0	},
	{	.0,	1.,	.0	},
	{	.0,	.0,	1.	}
}

function AQUA:init_app( ... )
	local tank = TANK:create( "SoOuest", self, nil )
	self.tank = tank
	--todoaqua cleaner
	tank:set_module_fish_dir_name( string.remove_trailing_slash( self:get_dir_absolute() ).."_Fish/" )
	MEDIA.set_dir( app.media_dir_rel.."Aqua/ACD/" )

	self.t_activate = { tank:get_module_fish_dir_name().."default.layerss_param" }

	if not oo.getsuper(AQUA).init_app( self, ... ) then return end

	self:print_inverse( "APP INIT              "..self:get_name() )

	self:define_pcs()
	self:build_screen_data()
--
--	KINECT	/	MASTER	/	SLAVE	Dance
--
	local kinect_list =	{
						--	"MAAQUAD",
							"AQUA-PC-K01",
						}

	self.b_kinect	= table.find_key_by_val( kinect_list, aaa.net.machine ) and true or false
	if self.b_kinect then
		self:set_master( false )
		self:set_slave( false )
	else
		if aaa.net.machine == "SPAHIR1-PC" then
			ga:set_onsite( true )
		end
		if false then
			local master_list =	{
								"NORTHERNSTAR",
								"MAAHEX",
								"MAAQUAD",
								"WALLSITE1",
								"AQUA-PC-M01",
								"SPAHIR1-PC",
								"MBPR-PC",
								"ALX-PC",
								"ACD01",
								"AQUA-PC",
								"TAAB18",
								"AAA_QWARTZ_03"	--	EO Nimes
								}
			self:set_master( table.find_key_by_val( master_list, aaa.net.machine ) and true or false )
			self:set_slave( not self:is_master() )
		else
			self:set_master( true )
			self:set_slave( false )
		end

		if self:is_slave() then
			self.cam_wall_id = 4
			for i=1,8 do
				if aaa.net.machine == self.pcs:get_pc(i):get_machine_name() then
					self.cam_wall_id = i
					break
				end
			end
		end

		self.b_slave_onsite = self:is_slave() and self:is_onsite()
		self.b_master_onsite = self:is_master() and self:is_onsite()
	end
--
-- Now we now how to init
--
	self:init_osc()
	if self:is_master() or self.b_kinect then
		aaa.net.set_any( 0 )
		aaa.net.update()
		aaa.sleep( 200 )	-- was 2000
		if not self.b_kinect then
			watchdog.net_send( 0, "watchdog.start_aaaseed()" )
		end
		aaa.net.update()
		aaa.sleep( 200 )	-- was 2000
	end

	param.set( aaa.ref.lua_master.file_check_active, not self:is_onsite() )
	if self:is_onsite() then
		param.set( aaa.ref.pref, "clean_render",			false	)
		ga:force_fullscreen( "all" )
	else
		if aaa.net.machine ~= "MAAHEX" then
			ga:force_fullscreen( "left" )
		end
	end

	ga:set_lock_app( self:is_onsite() )
	ga.b_calage_allow = false

	self:create_log_in_dir( app.media_dir_rel.."Aqua/", 3600, true )
	self:add_log( "Start" )

	if self:is_onsite() then
		self:set_shutdown_no_save_on_time( 22, 30 )
	end

	param.set( aaa.ref.app, "axe", "Invisible" )
	param.set( aaa.ref.app, "origin", "Invisible" )
	if not self:is_master() then
		aaa.flatland.set_draw( false )
		aaa.set_lua_silent()
	end

	local ref = self.ref
-- KINECT
	self.kinect_nb = 2
	self.kinect_last = { false, false, false }
	if self.b_kinect then
		self.kinect = {}
		local kinect

		local function find_order( name )
			local name_order = {	{name="USB\\VID_045E&PID_02C2\\6&2B0CC838&0&4", order=2 },
									{name="USB\\VID_0409&PID_005A\\5&259B87DC&0&2", order=1 },
									{name="USB\\VID_045E&PID_02C2\\7&12077E4D&0&2", order=1 },
									{name="USB\\VID_045E&PID_02C2\\7&2EDBFBA2&0&1", order=2 },
									{name="USB\\VID_045E&PID_02C2\\8&37E9F53&0&2",	order=3 },
								}
			local nb = #name_order
			for i=1,nb do
				local elt = name_order[i]
				if elt.name == name then
					return elt.order
				end
			end
			self:box_error( "can find kinect "..name )
		end
		local id
		kinect = KINECT:create( aaa.obj.get( "aqua_kinect_a_layers" ) )
			id = find_order( kinect:get_device_name() )
			self.kinect[id] = kinect
		kinect = KINECT:create( aaa.obj.get( "aqua_kinect_b_layers" ) )
			id = find_order( kinect:get_device_name() )
			self.kinect[id] = kinect
		for i=1,self.kinect_nb do
			kinect = self.kinect[i]
			self:print( kinect:get_device_name() )
			kinect:set_active( true )
			local ref = kinect.ref
			ref.cv = aaa.obj.get_down_by_class( ref.kinect_layers, "bdd_img_cv_multitouch" )
				ref.average = param.get_ref( ref.cv, "grey_average" )
				ref.agitation = param.get_ref( ref.cv, "agitation_result" )
		end
	else

	-- FBO
		self.fbo_and_out = FBO_AND_OUT:create( self:get_name(), 4 )
		--done by default here at init, also done by machine on slave
		if self:is_onsite() then
			self.fbo_and_out:set_pixel_size( 2048, 2048 )
		else
			if aaa.net.machine == "MAAHEX" then
				self.fbo_and_out:set_pixel_size( "hd" )
			else
				self.fbo_and_out:set_pixel_size( "720p" )
			end
			
		end
		self:set_fbo_grid()

	-- FXS
		local module = aaa.module.get_cur()

		local list_back = { "Color", "light_surface_a", "light_surface_b", "light_volume", "Color_Fore", "Fluid", "boid_1", "Bulle", "Plancton", "Algues_1" }

		self.fxs_back = FXS:create( "Back" )
		self.fxs_back:add_one( module, "color_noise_a" )
		self.fxs_back:add_one( module, "color_noise_b" )
		self.fxs_back:add_one( module, "color_noise_c" )
		self.fxs_back:add_one( module, "color_noise_d" )
		self.fxs_back:add_obj( self:get_prefix().."grille_layers", 	"Grille" )
		self.fxs_back:add_list( module, list_back )

		local list_other = { "Meduse_Maa", "Meduse_Annie", "3D", "Tree" }
		self.fxs_other = FXS:create( "Other" )
		self.fxs_other:add_list( module, list_other )


		local list_fore = { "Plancton_a", "Plancton_b", "Visu" }
		self.fxs_fore = FXS:create( "Fore" )
		self.fxs_fore:add_list( module, list_fore )

	-- CAMERAS
		ref.camera = {}
		ref.camera.back = {}
		local r
		r = ref.camera.back
		r.layers	=	aaa.obj.get( self:get_prefix().."camera_back_layers" )
		r.camera			=	aaa.obj.get( self:get_prefix().."camera_back" )
			r.tra_z			=	param.get_ref(	r.camera,	"tra_z"				)
			r.frustrum_x	=	param.get_ref(  r.camera,	"frustum_offset_x"	)
			r.frustrum_y	=	param.get_ref(  r.camera,	"frustum_offset_y"	)

		r = {}
		r.layers	=	aaa.obj.get( "fish_camera_edit_layers" )
			r.camera_index =  param.get_ref( r.layers, "camera_index_ui" )
			ref.camera.edit = r

		ref.camera.wall = {}
		r = ref.camera.wall
		r.camera = aaa.obj.get( "fish_camera_wall_ref" )
			r.tra_z	= param.get_ref( r.camera, "tra_z" )
			r.focal	= param.get_ref( r.camera, "focal" )

		ref.camera.display = {}
		r = ref.camera.display
			r.layers	=	aaa.obj.get( "fish_camera_display_layers" )
			r.camera_index =  param.get_ref( r.layers, "camera_index_ui" )

		for ipc = 1,8 do
			local pc = self.pcs:get_pc(ipc)
			local cam = aaa.layers.get_camera( self.ref.camera.display.layers, ipc-1 )
			pc:set_cam( cam )
			param.set(	cam,	"clip_close_perspective",	6.	)
			param.set(	cam,	"clip_far_perspective",		30.	)
		end

		ref.proj_cone = {}
		ref.proj_cone.obj = aaa.obj.get( "aqua_visu_cone" )
			ref.proj_cone.src_z = param.get_ref(  ref.proj_cone.obj, "src_z" )
			ref.proj_cone.dst_z = param.get_ref(  ref.proj_cone.obj, "dst_z" )
			ref.proj_cone.sx = param.get_ref(  ref.proj_cone.obj, "dst_size_x" )
			ref.proj_cone.sy = param.get_ref(  ref.proj_cone.obj, "dst_size_y" )

	-- CLEANING
--not used		ref.cleaning_layers = aaa.obj.get( "aqua_cleaning_layers" )

	-- BOID / CURVE
		--now in grea

		if self.cam_wall_id then
			self:set_camera_id( self.cam_wall_id + 8 )
		end

		tank:init()
	end
	--todo perhaps reinsatate waiting mecamnism because of window boot but this time inside the capture script

	return true
end

