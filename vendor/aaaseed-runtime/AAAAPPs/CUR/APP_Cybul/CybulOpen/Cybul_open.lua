if APP.DECLARE( "CYBUL", APP_GP ) then
	function CYBUL:set_media_dir()
		MEDIA.set_dir_media( "Cybul" )
	end
end

function CYBUL:init_app( ... )

	self.mode_client = false

	self:set_init_monitor_pass( true )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper( CYBUL ).init_app( self, ... ) then return end

	param.set( aaa.ref.use_filesystem, true )
	param.set( aaa.ref.shader_reload_trig, true )

	self:init_global_table()

	-- photo
	self.photo	= {}
	self.photo.bind		= TEXS:get_bind_by_name( "F1" )
	self.photo.bind_out = TEXS:get_bind_by_name( "F2" )
	self.bind_rendered	= TEXS:get_bind_by_name( "ToneMap" )

	-- logo
	self.logo	= {}
	self.logo.bind		= TEXS:get_bind_by_name( "F3" )
	local path_logo = "CYBSTORES_logo_fond_couleur_RVB.tga"
	self:set_media_dir()
	local fname = MEDIA.make_fname( path_logo )
	local b_ok = aaa.img.read( self.logo.bind, fname )
	
	self:load_media()

	ga:define_ui_customer( self )
	ga:set_ui_group_active( true, "customer" )
	ga:set_ui_group_active( false,	"top" )
	ga:set_ui_group_active( false	)

	if self.mode_client then
		-- remove menu
		local nb = aaa.menu.get_item_nb()
		for i=1,nb do
			aaa.menu.remove_item( 1 )
		end
	end

	param.set( aaa.ref.camera_edit, true )

	self:load_default_project()

	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( CYBUL )
end

function CYBUL:add_img( name, filename, tab )
	local img = IMGS.get_img( filename )
	if img then
		tab[name] = img
		return true
	else
		self:box_error( "Could not read\n  "..filename )
		return false
	end
end

function CYBUL:load_media_in_dir( dir, tab )
	dir = "Cybul/"..dir
	MEDIA.set_dir_media( dir )
	dir = MEDIA.get_dir()

	if aaa.dir.is_exist( dir ) then
		local files = aaa.dir.get_files( dir )
		for i=1,#files do
			local name, ext =  files[i]:match( "([^.]+).([^.]+)" )
			print( name, ext )
			if ext ~= 'tex_def' then
				self:add_img( name, files[i], tab )
			end
		end
	else
		self:box_error( "Directory "..dir.." does not exist" )
	end
end

function CYBUL:load_media()
	print("load media   ", self.logo, app.logo)
	self.tex_toiles  = {}
	self.tex_lames   = {}

	app.tex_armatures   = {}
	app.tex_armatures.banne       = {}
	app.tex_armatures.pergola_mur = {}
	app.tex_armatures.pergola_sol = {}

	local b = aaa.img.scale_on_load_get_active()
	aaa.img.scale_on_load_set_active( false )

	-- self:load_media_in_dir( "Toiles",  self.tex_toiles )
	-- self:load_media_in_dir( "Lames",   self.tex_lames )
	self:load_media_in_dir( "Armatures/Pergola",    self.tex_lames ) -- same texturees for blades and pillars
	self:load_media_in_dir( "Armatures/Banne",      app.tex_armatures.banne )
	self:load_media_in_dir( "Armatures/PergolaMur", app.tex_armatures.pergola_mur )
	self:load_media_in_dir( "Armatures/Pergola",    app.tex_armatures.pergola_sol )

	aaa.img.scale_on_load_set_active( b )

	local count
	count = 0
	for key,val in pairs( self.tex_toiles ) do count = count + 1 end
	self.tex_toiles_count = count
	count = 0
	for key,val in pairs( self.tex_lames ) do count = count + 1 end
	self.tex_lames_count = count
	count = 0
	for key,val in pairs( app.tex_armatures.banne ) do count = count + 1 end
	app.tex_armatures.banne_count = count
	count = 0
	for key,val in pairs( app.tex_armatures.pergola_mur ) do count = count + 1 end
	app.tex_armatures.pergola_mur_count = count
	count = 0
	for key,val in pairs( app.tex_armatures.pergola_sol ) do count = count + 1 end
	app.tex_armatures.pergola_sol_count = count
end

function CYBUL:get_cybstores_meu()
	return  self:get_meu_by_name( "CybStore_1" )
end

function CYBUL:init_global_table()
	if not self.ui.bus then
		self.ui.bus = {}
		self.ui.bus_banne = {}
		self.ui.bus_pergola = {}
		self.ui.bus_pergola_mur = {}
	end

	self.draw_axe_in_stage = true

	self.tab3d = {}
	self.tab3d.points = {	{.255,.745},	{.745,.75},
							{.25,.505},		{.75,.50},
							{.125,.248},	{.875,.25},
						
							{.25, .75}, 	{.75, .75},
							{.75, .25}, 	{.75, .25}}

	self.tab3d.points_recadrage = {	{.2,.8},	{.8,.8},
									{.2,.2},	{.8,.2} }	
	
	self.tab3d.zoom   = 100
	
	-- photo
	self.photo_correction = {}
	self.photo_correction.contrast   = 0
	self.photo_correction.brightness = 0
	self.photo_correction.saturation = 0
	
	-- scene
	self.scene = {}
	self.scene.wall_height     = 1000
	self.scene.wall_width      = 1000
	self.scene.floor_depth     = 1000
	self.scene.ext_wall_height = 1000
	self.scene.ext_wall_width  = 1000
	self.scene.ext_floor_depth = 1000
	self.scene.scene_center_x  = 0
	
	-- tab photo 
	self.tab3d.show_grid = true
	self.tab3d.grid_steps = 1
	self.tab3d.debug = false
	
	-- zips common
	self.zips = {}
	self.zips.length   = 0
	self.zips.opening = 1
	-- zips pergola
	self.zips.left    = true
	self.zips.right   = true
	self.zips.forward = true
	self.zips.wall    = false
	self.zips.deportage   = 0
	self.zips.transparent = false
	
	self.tab3d.store_type = "Banne"
	
	self.tab3d.pergola_mode = "Lames"
	
	self.tab3d.show_store = true
	
	self.tab3d.sun_long = 0
	self.tab3d.sun_lat  = 0
	
	-- stores
	self.store = {}
	self.store.debut = 0
	self.store.fin   = 0
	
	-- pergola
	self.store.blade_angle  = 0
	self.store.pillar_count = 4
	
	-- retractation
	self.store.retractation 		   = 0
	self.store.lames_retract_gauche = false
	self.store.lames_retract_droite = false
	
	-- banne
	self.store.hessian_angle   = 0
	self.store.hessian_opening = 1
	self.store.hauteur_min     = 0
	
	self.store.deportage_mur   = 0
	
	-- constexpr variables, from Jean-Maxime
	self.dimensions = {}
	self.dimensions.pergola = {}
	
	self.dimensions.pergola.lames = {}
	self.dimensions.pergola.lames.poteau = 0.15
	self.dimensions.pergola.lames.poutre = 0.25
	self.dimensions.pergola.lames.lame   = 0.21
	
	-- light
	self.light = {}
	self.light.x = 0
	self.light.y = 0
	self.light.z = 0
	self.light.angle_facade = 0
	self.light.angle_sun    = 0
	self.light.night_mode   = false

	-- lambrequin lumineux
	self.lambrequin_lumineux = {}
	self.lambrequin_lumineux.count = 0
	self.lambrequin_lumineux.height   = 250
	self.lambrequin_lumineux.position_x = 0
	self.lambrequin_lumineux.position_y = 0.5
	self.lambrequin_lumineux.ratio    = 1

	-- hessian parameters
	self.hessian = {}
	self.hessian.width  = 1.200
	self.hessian.height = 1.200
	self.hessian.gl_hor = "repeat"
	self.hessian.gl_ver = "repeat"
	self.zip_tex = {}
	self.zip_tex.width  = 1.200
	self.zip_tex.height = 1.200
	self.zip_tex.gl_hor = "repeat"
	self.zip_tex.gl_ver = "repeat"
end

function CYBUL:reset_hessian_parameters()
	self.hessian.width  = 1.200
	self.hessian.height = 1.200
	self.hessian.gl_hor = "repeat"
	self.hessian.gl_ver = "repeat"
end
function CYBUL:reset_zip_tex_parameters()
	self.zip_tex.width  = 1.200
	self.zip_tex.height = 1.200
	self.zip_tex.gl_hor = "repeat"
	self.zip_tex.gl_ver = "repeat"
end

function CYBUL:do_key( key )
	if key == 91 then
		if self.mode_client then
			self:print("Switching to dev mode")
		else
			self:print("Switching to client mode")
		end
		self.mode_client = not self.mode_client
		return true
	end
	if self.mode_client then
		return true
	end
	return oo.getsuper(CYBUL).do_key( self, key )
end

function CYBUL:do_key_special( key )
	if aaa.keyboard.is_key_fn( key, 11 ) then
		-- replaces reload:local, non-existent for cybul.
		param.set( aaa.ref.fullscreen_flip_trig, true )
		return true
	end

	-- prevent all keys, especially Fn keys
	if self.mode_client then
		return true
	end
	return oo.getsuper(CYBUL).do_key_special( self, key )
end

function CYBUL:event_hook_before( e_type, e_sub_type, a, b, c, d, e )
	if self.mode_client then
		if e_type == "keyboard" then
			return a == 9 or a == 119 -- prevent tab and w
		end
		if e_type == "mouse" and e_sub_type == "button" then
			local but_name = EVENT.mouse.button_name[c]
			-- prevent flatland from clicking in the upperleft corner
			return but_name == "BUTTON_LEFT" and a < 66 and b < 30
		end
	end

	if e_type == "window" then
		if e_sub_type == "close" then
			aaa.os.quit( false, true, false, false )
			return true
		end
		if e_sub_type == "resize" then
			local sx = param.get(aaa.ref.window_sx)
			local sy = param.get(aaa.ref.window_sy)
			local nsy = math.floor(sx * 9 / 16)
			if nsy ~= sy then
				local x = param.get(aaa.ref.window_x)
				local y = param.get(aaa.ref.window_y)

				-- param.set( aaa.ref.window_force_x, x )
				-- param.set( aaa.ref.window_force_y, y )
				
				-- param.set( aaa.ref.window_force_size_x,  sx )
				-- param.set( aaa.ref.window_force_size_y, nsy )
				
				-- self:print("resizing window from "..sx..","..sy.." to "..sx..","..nsy..'  '..x..' '..y)
				-- param.set( aaa.ref.window_force, true )

				-- sx = sx * .66
				-- nsy = nsy * .66
				local gbuffer = self:get_meu_by_name( "Fbo_GBuffer" )
				local buf_sx, buf_sy = gbuffer:get_pixel_size()
				self:print( "resizing gbuffer to "..sx..' '..nsy )
				gbuffer:set_pixel_size( sx,nsy )
				-- param.set( aaa.ref.window_force, true )

			else
				param.set( aaa.ref.window_force, false )
			end
		end
	end

	return false
end

function CYBUL:hook_quit( b_shutdown, b_shutdown_reboot )
	-- save current state
	self:save_preset( self:get_dir_absolute() )

	APP.hook_quit( self, b_shutdown, b_shutdown_reboot )
end
