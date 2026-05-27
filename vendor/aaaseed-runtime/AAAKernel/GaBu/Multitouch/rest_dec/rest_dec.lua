if aaa.lua.global.declare_table( "GABU" ) then
--[[
	function GABU.transfo_push( x, y, sx, sy, angle )
		local ref = GABU.transfo.ref
		param.set( ref.tra_x, x )
		param.set( ref.tra_y, y )
		param.set( ref.sca_x, sx )
		param.set( ref.sca_y, sy )
		param.set( ref.rot_z, angle )
		aaa.obj.update_then_draw( ref.transfo_push )
	end
	function GABU.transfo_pop()
		aaa.obj.update_then_draw( GABU.transfo.ref.transfo_pop )
	end
	function GABU.transfo_init()
		aaa.print( "GABU.transfo_init()" )

		GABU.transfo = {}
		GABU.transfo.ref = {}
		local ref = GABU.transfo.ref
		ref.transfo_push = aaa.obj.get_by_name_symbo( "gabu_transfo_push" )
		ref.transfo_pop = aaa.obj.get_by_name_symbo( "gabu_transfo_pop" )
		ref.transfo = aaa.obj.get_by_name_symbo( "gabu_transfo" )
		ref.tra_x = param.get_ref( ref.transfo, "translate_x" )
		ref.tra_y = param.get_ref( ref.transfo, "translate_y" )
		ref.sca_x = param.get_ref( ref.transfo, "scale_x" )
		ref.sca_y = param.get_ref( ref.transfo, "scale_y" )
		ref.rot_z = param.get_ref( ref.transfo, "rotate_z" )
		ref.active = param.get_ref( ref.transfo, "active" )
	end
	GABU.transfo_init()
--]]
--todo move to method and add args
--todo use GABU_OBJ nethod mecanism
	function GABU.add_free_fn( fn, arg )
		local mess
		if fn then
			local str = type(fn)
			if str == "function" then
				table.insert( GABU.__free_fn, { fn = fn, arg = arg } )
			else
				mess = "pass "..fn.." a "..str.." instead of a fn"
			end
		else
			mess = "pass a nil fn"
		end
		if mess then
			aaa.debug.print_traceback()
			aaa.box_error( "in GABU:add_free_fn() : "..mess )
		end
	end
	function GABU.do_free_fn()
		for _, t in IPAIRS( GABU.__free_fn ) do
			t.fn( t.arg )
		end
		GABU.__free_fn = {}
	end
	GABU.do_free_fn()
end

if GABU and ( not GABU.ref or not GABU.ref.simul_transfer_size_factor ) then
	GABU.ref = {}
	local ref = GABU.ref
	--"GaBu/Multitouch/Blob_fbo_out/fx.layers_param" )
	local layers = aaa.obj.get_by_class_and_name_symbo( "layers", "gabu_blob_draw_on_top" )
	ref.draw_on_top_active			=	param.get_ref( layers, "active" )

	function GABU.set_draw_blob_on_top( b_on )	param.set( GABU.ref.draw_on_top_active, b_on )	end
	GABU.set_draw_blob_on_top( 0 )

	ref.draw_on_top_model			=	aaa.obj.get_down( layers, "gabu_blob_draw_on_top_model" )
		ref.draw_on_top_size_u			=	param.get_ref( ref.draw_on_top_model, "size_u" )
			param.set_save( ref.draw_on_top_size_u, false )
		ref.draw_on_top_size_v			=	param.get_ref( ref.draw_on_top_model, "size_v" )
			param.set_save( ref.draw_on_top_size_v, false )


	ref.simul						=	aaa.obj.get_by_class_and_name_symbo( "module", "gabu_simulator" )
		ref.simul_active				=	param.get_ref( ref.simul, "active" )

	ref.simul_bdd					=	aaa.obj.get_by_class_and_name_symbo( "bdd_boxes", "gabu_simul_bdd" )	--todo do faster
		ref.simul_transfer_size_x		=	param.get_ref(	ref.simul_bdd,	"transfer_size_x"		)
		param.set_save( ref.simul_transfer_size_x, false )
		ref.simul_transfer_size_y		=	param.get_ref(	ref.simul_bdd,	"transfer_size_y"		)
		param.set_save( ref.simul_transfer_size_y, false )
		ref.simul_transfer_size_factor	=	param.get_ref(	ref.simul_bdd,	"transfer_size_factor"	)
end

--[[
function GABU.app.init()
	key_libre = switch {
		[1] = function (x) 	aaa.print( "Deja en mode Libre" ) end,	-- mode libre
		[2] = function (x) 	app_mode_change( x ) end,	-- mode arbre
		[3] = function (x) 	app_mode_change( x ) end,	-- mode edition
		default = function (x)	aaa.print( "Touche non utilisee en mode Libre : "..x ) end,
	}
	key_arbre = switch {
		[1] = function (x) 	app_mode_change( x ) end,	-- mode libre
		[2] = function (x) 	aaa.print( "Deja en mode Arbre" ) end,	-- mode arbre
		[3] = function (x) 	app_mode_change( x ) end,	-- mode edition
		default = function (x)	aaa.print( "Touche non utilisee en mode Arbre : "..x ) end,
	}
	key_edition = switch {
		[1] = function (x) 	app_mode_change( x ) end,	-- mode libre
		[2] = function (x) 	app_mode_change( x ) end,	-- mode arbre
		[3] = function (x) 	aaa.print( "Deja en mode Edition" ) end,	-- mode edition
		default = function (x)	aaa.print( "Touche non utilisee en mode Edition : "..x ) end,
	}
end
--GABU.app.init()
--]]

--todo move to the simulator or even multitouch
function GABU.is_simulation()			return param.get_bool( GABU.ref.simul_active )	end
function GABU.set_simulation( b )		param.set( GABU.ref.simul_active, b )				end
function GABU.update_simul_size( f )	param.set( GABU.ref.simul_transfer_size_factor, 1/f )				end

function GABU.flip_simulation()
	local b = param.flip( "GaBu/AAA_Simulator/default.layerss_param", "active" )
--	b = 1-b
	--param.set( blobs.ref.obj, "use_mouse", 1-b )
--	param.set( "GaBu/AAA_Simulator/default.layerss_param", "active", b )

	--todo really a comment
	--param.set( "GaBu/Stereo_Contour_All/default.layerss_param", "active", b )
	return b
end

local function __do_virtual_keyboard( key, b_special )
	local virtual_keyboard = GA:get_keyboard_virtual()
	if virtual_keyboard then
		local bu = virtual_keyboard:get_attach_bu()
		if bu then
			if bu:is_editable_active() then
				bu:push_letter( key, nil, virtual_keyboard, b_special )
				return true
			end
		end
	end
	return false
end

function GABU.do_action( str_src, which )
	if not aaa.is_edit() then
		local seqs = SEQS.cur
		if seqs then
			return seqs:do_action( str_src, which  )
		end
	end
	return false
end

function GABU.do_key_custom_def( key )
	local b_key_used = false
	--if key == 65 or key==97 then 		--a,A
	--	--ga:set_arte()
	if key == 82 or key == 114 then --r,R
		if GABUIN and GABUIN.cv_reset_background then
			GABUIN.cv_reset_background()
			b_key_used = true
		end
		if aaa.lua.global.get( "cc_cv_reset_background" ) then
			cc_cv_reset_background()
			b_key_used = true
		end
	-- elseif key == 4 then		-- CTRL d
	--	local obj = aaa.obj.get_from_top_by_class( "multi_screen" )
	-- 	local nb = param.get( obj, "x_nb" )
	-- 	param.set( obj, "x_nb", 3-nb )
	-- 	b_key_used = true
	-- elseif key==18 then		-- CTRL r
	--	reload()
	elseif key == 15 then	-- CTRL o
		GABU.load( false )
		b_key_used = true
	elseif key == 32 then	-- space
		b_key_used = GABU.do_action( "key_custom", "flip" )
	elseif (key==83 or key==115) and aaa.keyboard.is_no_modifier() then	--s,S
		--aaa.print( "pb because ?" )
		GABU.flip_simulation()
		b_key_used = true
	end
	if b_key_used then
		aaa.print( "GABU.do_key_custom_def() key "..key.." Used" )
	else
		aaa.print_debug( "GABU.do_key_custom_def() key "..key.." Unused" )
	end
	return b_key_used
end

function GABU.do_key_special_custom_def( key )
	local is_key_fn = aaa.keyboard.is_key_fn

	local b_key_used = false
	if key == 100 then			--	<- ARROW LEFT
		if aaa.lua.global.get( "calage" ) and calage:is_active() then
			calage.dec_index()
			b_key_used = true
		else
			b_key_used = GABU.do_action( "key_special_custom", "prev" )
		end
	elseif key == 102 then		--	-> ARROW RIGHT
		--aaa.print( "special_special 102 " )
		b_key_used = GABU.do_action( "key_special_custom", "next" )
	elseif key == 106 then		--	Home/Debut
		b_key_used = GABU.do_action( "key_special_custom", "begin" )
	elseif is_key_fn( key, 1 ) then	--F1 key
		--aaa.print( "F1 hola" )	
		if aaa.keyboard.is_shift() then
			if not aaa.keyboard.is_ctrl() then
				aaa.print( "Shift F1" )
				--aaa.print( table.show( aaa, "aaa" ) )
				table.print( aaa, "aaa", 8 )
				b_key_used = true
			end		
		elseif aaa.keyboard.is_ctrl() then
			aaa.print( "Ctrl F1" )
			table.print( _G, "global", 2 )
			b_key_used = true
		end
	elseif is_key_fn( key, 5 ) then	--F5
		if aaa.keyboard.is_shift() and aaa.keyboard.is_ctrl() then
			if GABUIN and GABUIN.cv_reset_background then
				GABUIN.cv_reset_background()
				b_key_used = true
			end
		end
	elseif is_key_fn( key, 6 ) and ga.b_calage_allow then	--F6
		if aaa.keyboard.is_shift() and aaa.keyboard.is_ctrl() then
			ga:flip_calage()
			b_key_used = true
		end
	elseif is_key_fn( key, 8 ) then	--F8
		param.flip( "GaBu/Monitor_All/default.layerss_param", "active" )
		b_key_used = true
	-- elseif is_key_fn( key, 11 ) then	--F11
	-- 	--todoaqua clean ?
	-- 	if TANK.cur and TANK.cur.reload_local then
	-- 		TANK.cur:print_inverse( "will call TANK.cur:reload_local()" )
	-- 		TANK.cur:reload_local()
	-- 		b_key_used = true
	-- 	else
	-- 		aaa.print_inverse( "No app:reload_local() to call" )
	-- 	end
	elseif is_key_fn( key, 12 ) then	--F12
		GABU.load( true )
		b_key_used = true
	end
	if b_key_used then
		aaa.print( "GABU.do_key_special_custom_def() key "..key.." Used" )
	else
		aaa.print_debug( "GABU.do_key_special_custom_def() key "..key.." Unused" )
	end
	return b_key_used
end

function GABU.do_key_irtouch( key )
	aaa.print( "GABU.do_key_irtouch() with "..key )
	local ir_touch = "GaBu/IrTouch_In/IRTouch/fx_a.ir_touch"
	if key==51  or key==163 then -- 3
		param.set( ir_touch, "com_port_nb", 3 )
	elseif key==52 or key==36 then -- 4
		aaa.print( "set com port to 4" )
		param.set( ir_touch, "com_port_nb", 4 )
	elseif key==86 or key==118 then -- v,V
		-- flip IRTouch vertical
		if param.get( ir_touch, "transfer_size_y" ) == 1 then
			param.set( ir_touch, "transfer_size_y", -1 )
			param.set( ir_touch, "transfer_center_y", 1 )
		else
			param.set( ir_touch, "transfer_size_y", 1 )
			param.set( ir_touch, "transfer_center_y", 0 )
		end
	elseif key==72 or key==104 then	-- h,H
		-- flip IRTouch horizontal
		if param.get( ir_touch, "transfer_size_x" ) == 1 then
			param.set( ir_touch, "transfer_size_x", -1 )
			param.set( ir_touch, "transfer_center_x", 1 )
		else
			param.set( ir_touch, "transfer_size_x", 1 )
			param.set( ir_touch, "transfer_center_x", 0 )
		end
	elseif key==43 then	-- +
		local val = param.get( ir_touch, "max_touch" )
		val = clamp( val + 1, 1, 7 )
		param.set( ir_touch, "max_touch", val )
	elseif key==45 then	-- -
		local val = param.get( ir_touch, "max_touch" )
		val = clamp( val - 1, 1, 7 )
		param.set( ir_touch, "max_touch", val )
	end
end

function GABU.__get_bu_for_key()
	return ga and (not aaa.flatland.is_draw_focus()) and BU:get_bu_cur()
end

function GABU.do_key( key )
--	aaa.print_fn()
	if __do_virtual_keyboard( key, false ) then
		return true
	end

	aaa.print( "ascii key "..key.." in "..aaa.script.get_name( ) )

	local b_used = ga:do_key( key )
	return b_used or GABU.do_key_custom_def( key )
end

function GABU.do_key_special( key )
	aaa.print( "special_key "..key.." in "..aaa.script.get_name( ) )

	if __do_virtual_keyboard( key, true ) then return true end

	local b_used = ga:do_key_special( key )

	return b_used or GABU.do_key_special_custom_def( key )
end

--todo_jean we should do it at the event level so we don't manipulate param and GABu at the same time

function GABU.process_key()
	if not aaa.stereo.is_field_for_update() then return end

	local b_key_used = false

---[[
	local key = aaa.keyboard.get_ascii_down()
	if key ~= 0 then
		b_key_used = GABU.do_key( key )
	end

	key = aaa.keyboard.get_special_down()
	if key ~= 0 then
		b_key_used = GABU.do_key_special( key )
	end
--]]

--2023 November do_key() and do_key_special are now the way to go
--todo	decide for a final strategy
	if not b_key_used then
		if app and app.do_key_custom then
			app:print( "try to use key with do_key_custom()" )
			app:do_key_custom( key )
		end
		if GABU.do_key_custom then
			aaa.print( "try to use key with GABU.do_key_custom()" )
			GABU.do_key_custom( key )
		end
	end

end

--local index_min
--local index_max

function GABU.free()
	aaa.print( "GABU.free()" )

--[[
	if index_min then
		index_max = param.get( aaa.ref.pref, "obj_ui_created" )
	end
--]]
	--	we call all the registered free fn first
	GABU.do_free_fn()

	APP.free_app_cur()
	ga:init_dialogs()

	aaa.lua.set_file_force_recheck( true )
	--local page = bus_root:get_page()
	BU_WWW.free()
	IMGS.init()
	VIDEOS:free()

	--todo bus_root:set_page(page)
	aaa.wipe_focus()
	aaa.bdd.clear_ui_intercept_all()

--[[
	if index_max then
		for i=index_min+1,index_max do
			local obj = aaa.obj.get_by_id( id )
			b = aaa.obj.is_ref_no_error( obj_ref )
		end
	end
	index_min = param.get( aaa.ref.pref, "obj_ui_created" )
--]]
end

function GABU.load( b_reload )
	aaa.print_fn()
	if b_reload then
		aaa.mess.show( "RELOADED ALL Asked" )
			GABU.free()
			GABU.env_init( "F12" )
		aaa.mess.show( "RELOADED ALL" )
	else
		aaa.mess.show( "Open GaBuZoMeu Asked" )
		local filter = "Reconnus\0*.layerss_param\0Tous\0*.*\0\0"
		local fpath = aaa.file.do_dialog_open( "Open GaBuZoMeu App", filter )
		if fpath then
			aaa.print( "will now try to open c_module from "..fpath )
			GABU.free()
			local modules = aaa.modules.get_cur()
			-- local module = aaa.modules.get_module( modules, 69 )
			-- if module then
			-- 	aaa.obj.delete( module )
			-- end
			local module = aaa.modules.new_module( aaa.modules.get_cur(), 33, fpath )
			if module then
				param.set( module, "active", true )
				aaa.obj.update_then_draw( module )
				GABU.env_init( "Load" )
			end
		else
			aaa.print_debug( "No fpath: Seems User canceled loading of c_module" )
		end
	end
end

function GABU.env_init( str )
	aaa.print( "\n\n\n\n                                     --- "..str.." ---\n\n\n" )
	aaa.print( "GABU.env_init() begin" )
	aaa.lua.global.set_strict()
		ga:open()	--by default at start
	aaa.print( "GABU.env_init() end" )
end
