
function GP:free_menu()	--, nb_vid, nb_vid_f, nb_fbo )
	local menu = GP.menu
	if menu then
		aaa.menu.set_cur( menu.main_id )

		local nb = aaa.menu.get_item_nb()	
		for i = 1,menu.bottom_nb or 0 do
			aaa.menu.remove_item( nb )
		end
		for i = 1,menu.top_nb or 0 do
			aaa.menu.remove_item( 1 )
		end
		--aaa.menu.change_item( 1, "Gros Test", -42, 1 )
		GP.menu = nil
	end
end

function GP:make_menu_id( menu, item )
	return menu * 256 * 256 + item
end
function GP:split_menu_id( id )
	local menu = math.floor( id / (256 * 256) )
	return menu, id - menu * 256 * 256
end
function GP:add_menu_item( text, menu, item, check )
	return aaa.menu.add_item( text,	self:make_menu_id( menu, item ), check or 0 )
end
function GP:add_menu_separator()
	return aaa.menu.add_separator()
end

function GP:define_menu_app_file( menu )	--, nb_vid, nb_vid_f, nb_fbo )
	menu.file = aaa.menu.create()
	aaa.menu.set_cur( menu.file )
		self:add_menu_item( "Open\tCtrl o", 			menu.file, 1 )
		self:add_menu_item( "Save All\tShift Ctrl S",	menu.file, 2 )
		self:add_menu_item( "Save Globals\tCtrl s",		menu.file, 3 )
	return menu.file
end

function GP:define_menu_sub( menu )	--, nb_vid, nb_vid_f, nb_fbo )
	menu.meu = aaa.menu.create()
	aaa.menu.set_cur( menu.meu )
		self:add_menu_item( "Rename",				menu.meu, 1 )
		self:add_menu_item( "Instantiate",			menu.meu, 2 )
		self:add_menu_item( "Destroy",				menu.meu, 3 )
		self:add_menu_item( "Import",				menu.meu, 4 )

	menu.dump = aaa.menu.create()
	aaa.menu.set_cur( menu.dump )
		self:add_menu_item( "group",	menu.dump, 1 )
		self:add_menu_item( "bind",		menu.dump, 2 )
		self:add_menu_item( "name",		menu.dump, 3 )

	menu.utils = aaa.menu.create()
	aaa.menu.set_cur( menu.utils )
		self:add_menu_item( "Enable Lua Error Dialog",		menu.utils, 1 )
		self:add_menu_item( "Disable Lua Error Dialog",		menu.utils, 2 )
		self:add_menu_separator()
		aaa.menu.add_menu_sub( "Dump texture name by",	menu.dump	)

	menu.ui_on_screen = aaa.menu.create()
	aaa.menu.set_cur( menu.ui_on_screen )
		self:add_menu_item( "all", menu.ui_on_screen, 1 )
		for i=1,6 do
			self:add_menu_item( tostring(i), menu.ui_on_screen,i+1 )
		end

	menu.custom = aaa.menu.create()
	aaa.menu.set_cur( menu.custom )
		self:add_menu_item( "Flatland Violet",		menu.custom, 1 )
		self:add_menu_item( "Flatland Color Blind",	menu.custom, 2 )

	menu.sub_id = aaa.menu.create()
	aaa.menu.set_cur( menu.sub_id )
		aaa.menu.add_menu_sub( "Ui on Screen",	menu.ui_on_screen	)
		aaa.menu.add_menu_sub( "Custom",		menu.custom			)
		
	return menu.sub_id
end
function GP:define_menu_top( menu )	--, nb_vid, nb_vid_f, nb_fbo )
	aaa.menu.set_cur( menu.main_id )
		aaa.menu.insert_menu_sub(	1,	"File",							menu.file	)
		aaa.menu.insert_menu_sub( 	2,	"APP "..app:get_name(),	menu.sub_id	)
		aaa.menu.insert_menu_sub( 	3,	"MEU", 							menu.meu	)
		aaa.menu.insert_menu_sub( 	4,	"Utils", 						menu.utils	)
		aaa.menu.insert_separator(	5	)	
	menu.top_nb = 5
end

function GP:define_menu_bottom( menu )	--, nb_vid, nb_vid_f, nb_fbo )
	aaa.menu.set_cur( menu.main_id )
		aaa.menu.add_separator()
		aaa.menu.add_menu_sub( "GP "..app:get_name(), menu.sub_id )
	menu.bottom_nb = 2
end
function GP:define_menu()	--, nb_vid, nb_vid_f, nb_fbo )
	--self:box_debug( "GP:define_menu_bottom( menu )" )

	local menu = {}
	GP.menu = menu
	--menu.sub = {}
	menu.main_id = aaa.menu.get_main()

	self:define_menu_app_file( menu )
	self:define_menu_sub( menu )	
--	self:define_menu_bottom( menu )
	self:define_menu_top( menu )
end

function GP:ask_import()
	--local filter = "Reconnus\0*.layerss_param\0Tous\0*.*\0\0"
	local fpath = aaa.file.do_dialog_folder( "Import folder (should be a MU/MEU folder)" )
	if fpath then
		self:get_mus_down():import_mu( fpath )
	end
end

function GP:do_menu( menu_id, item_id )
	aaa.print_fn()
	local menu = self.menu

	if		menu_id == menu.file			then
		-- FILE
		self:print( "Menu File")
		if		item_id == 1	then	GABU.load( false )
		elseif	item_id == 2	then	MEU:save_all()
										aaa.save()
		elseif	item_id == 3	then	aaa.save()
		end
	elseif	menu_id == menu.ui_on_screen	then
		-- UI on screen
		ga:set_ui_on_screen( item_id-1 ) -- 0 is all
	elseif	menu_id == menu.custom				then
		-- CUSTOM
		if		item_id == 1	then	aaa.flatland.set_color_blind( false )
		elseif	item_id == 2	then	aaa.flatland.set_color_blind( true )
		end
	elseif	menu_id == menu.meu				then
		-- MEU
		if				item_id == 4	then	self:ask_import()
		else
			local mu = self:get_mu_ui()
			if mu then
				if		item_id == 1	then	aaa.print( "Menu Rename called for "..mu )
												mu:ask_rename()
				elseif	item_id == 2	then	aaa.print( "Menu Instantiate called for "..mu )
												mu:ask_instantiate()
				elseif	item_id == 3	then	aaa.print( "Menu Destroy called for "..mu )
												mu:ask_destroy()
				end
			else
				ga:add_dialog_message( "No current MU/MEU " )
			end
		end
	elseif	menu_id == menu.utils		then
		-- UTILS
		if item_id == 1	or item_id == 2 then
			aaa.set_lua_silent( item_id == 2 )
		end
	elseif	menu_id == menu.dump		then
		-- DUMP
		if item_id == 1 then	TEXS:get_tex_named():dump_by_group()
		elseif item_id == 2 then	TEXS:get_tex_named():dump_by_bind()
		elseif item_id == 3 then	TEXS:get_tex_named():dump_by_name()	
		end
	end
end

function aaa.menu.hook( id )
	local menu_id, item_id = GP:split_menu_id( id )
	aaa.print( "Menu called "..menu_id.."/"..item_id )
	app:get_gp():do_menu( menu_id, item_id )
end
