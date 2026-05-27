--
--	GA
--
if CLASS.DECLARE( "GA", nil, {
	touch_nb_to_reset			= 8,
	b_reset_by_blob				= true,
	b_rotate					= true,
	max_touch_by_object			= 1,
	__page_trs_factor			= 1,
	__page_change_time			= 1,
	__b_inertia					= true,
	stop_time					= .4,
	stop_time_factor_y			= 2.,
	stop_time_factor_x			= 2.,
	calage_nb_u					= 5,
	calage_nb_v					= 3,
	bind_offset					= 0,
	b_calage_allow				= true,
	click_double_time			= .4,
	click_double_dist			= .01,
	click_long_time				= .5,
	b_move_to_gpu				= false,
	b_far						= false,
	time_no_touch				= 0.,
	__transfo_inter_time_factor	= 4.,
	b_verbose_allow				= true,
	__b_tex_draw_custom			= false,
	__b_visu_text_draw_rect 	= false,
	__keyboard_virtual			= nil,
	__update_index				= 0,

	__b_is_onsite 				= false,

} ) then
	GA.__c_node = aaa.obj.create_by_cid( "node_ui", nil, aaa.obj.get_by_id(1) )
	aaa.obj.set_name( GA.__c_node, "GA" )

	GA.__c_node_proto_isolated = aaa.obj.create_by_cid( "node_ui", nil, GA.__c_node )
	aaa.obj.set_name( GA.__c_node_proto_isolated, "MEU_PROTO_ISOLATED" )

	GA.__c_node_proto = aaa.obj.create_by_cid( "node_ui", nil, GA.__c_node )
	aaa.obj.set_name( GA.__c_node_proto, "MEU_PROTO" )

	GA.__c_node_inst = aaa.obj.create_by_cid( "node_ui", nil, GA.__c_node )
	aaa.obj.set_name( GA.__c_node_inst, "MEU_INST" )

	GA.__c_node_tex_video = aaa.obj.create_by_cid( "node_ui", nil, GA.__c_node )
	aaa.obj.set_name( GA.__c_node_tex_video, "TEX_VIDEO" )

	GA.__c_node_rest = aaa.obj.create_by_cid( "node_ui", nil, GA.__c_node )
	aaa.obj.set_name( GA.__c_node_rest, "REST" )

	GA:set_class_status_doc(	CLASS.STATUS.GABU,
								"GA stand for Global Action and infact hold the global preferences",
								"global variable ga give access to the current GA, a unique instance or singleton" )
	GA.b_spy = false
	--aaa.box_warning( "DECLARE "..aaa.lua.global.get( "GA" ).." but GA is ".._G["GA"] )
end


--  tex perso mode
function GA:set_tex_draw_custom( b )	self.__b_tex_draw_custom = b				end
function GA:is_tex_draw_custom() 		return self.__b_tex_draw_custom				end

function GA:get_update_index() 			return self.__update_index					end

-- PASS INFO
--
function GA:get_pass_info()				return self.__pass_info						end
function GA:set_pass_info( pass_info )	self.__pass_info = pass_info				end
--todo use a real stack
function GA:push_pass_info()			self.__pass_info_last = self.__pass_info	end
function GA:pop_pass_info()				self.__pass_info = self.__pass_info_last	end

-- VISUALIZATION
--
--todo check it and rename it accordly to gdebug too
--	visu mode : show more stuff,
function GA:is_visu_text_draw_rect()	return self.__b_visu_text_draw_rect 				end

function GA:set_visualization( b )		self.__b_visualization = b							end
function GA:is_visualization()			return self.__b_visualization						end
function GA:flip_visualization( bu )	self:set_visualization( bu:get_value_as_bool() )	end

-- LOCK
--
function GA:set_lock_app( b_lock )
	if b_lock then
		aaa.flatland.set_draw( false )
	end
	param.set( aaa.ref.pref, "edit", not b_lock	)
	param.set( aaa.ref.pref, "cursor_hide_in_render",	b_lock		)	--don't work have to be done direcltly in the ui
end
function GA:lock_app( )				ga:set_lock_app( true )							end
function GA:unlock_app()			ga:set_lock_app( false )						end


function GA:force_fullscreen()
	aaa.set_fullscreen()
end

function GA:set_render_size( sx, sy, offset_x, offset_y )
	--self:print( "set_render_size" )
	local ref = self.ref
	if sx		then	param.set( ref.render_sx,		sx )			end
	if sy		then	param.set( ref.render_sy,		sy )			end
	if offset_x then	param.set( ref.render_x_offset,	offset_x	)	end
	if offset_y then	param.set( ref.render_y_offset,	offset_y	)	end
--	param.set( ref.render_sfactor,		1.			)
end

GA.doc.get_cam = "() return the camera used for the UI as an AAACAM object"
function GA:get_cam()
	return self.cam
end

function GA:get_ui_ratio_x()
	local cam = self.cam
	return cam and cam:get_ratio_x() or 9/16
end

function GA:set_ui_on_screen( screen_id ) -- 0 is all
	local nb = aaa.screen.get_nb()
	aaa.screen.set_camera( self.cam:get_obj(), screen_id )
	self:print_inverse( "UI set on "..(screen_id==0 and "All Screens" or ("screen "..screen_id)) )
	ga.ui_on_screen_id = screen_id
end
function GA:inc_ui_on_screen( inc ) -- 0 is all
	local nb = aaa.screen.get_nb()
	local id = ga.ui_on_screen_id or 0
	aaa.print_fn()
	self:print( "inc_ui_on_screen() aaa.is_fullscreen() "..aaa.is_fullscreen().." and aaa.get_fullscreen_mode()".. aaa.get_fullscreen_mode() )	
	if aaa.is_fullscreen() and aaa.get_fullscreen_mode()==0 then	
		id = (id+inc) % (nb+1)
	else
		--id = 0
		id = (id+inc) % (nb+1)
	end
	self:print( "ask set_ui_on_screen to ", id )
	self:set_ui_on_screen( id )
end

function GA:init_dialogs()
	self.__dialog	=
	{
		id				= 0,
		by_id			= {},	-- handle dialog todo reuse id
		by_ref			= {},	-- avoid double dialog
		by_id_unique	= {},	-- avoid double dialog when dialog created by bu
	}
	self:init_dummys_used_for_edit()
end

--todo: do we need one ?
-- function GA:free()
-- end

function GA:create( name )
	--self:box_debug( "create one GA" )
	aaa.activate( { "gabu_calage" } )

	local self = GA:create_instance( name  )
	self:init_dialogs()
	self.__uif_bu = {}

	self:print( "create()" )
	IMGS.init()
	VIDEOS:init()
	self.help = GA_HELP:create()

	self.ref = {}
	self.ref.layers_fingers = aaa.obj.get( "gabu_fingers" )

	local pref = aaa.ref.pref
	local ref = self.ref
	ref.render_x_offset	= param.get_ref( pref, "clean_render_x_offset"			)
	ref.render_y_offset	= param.get_ref( pref, "clean_render_y_offset"			)
	ref.render_sx		= param.get_ref( pref, "camera_format_custom_x"			)
	ref.render_sy		= param.get_ref( pref, "camera_format_custom_y"			)
	ref.render_sfactor	= param.get_ref( pref, "camera_format_custom_factor"	)

	self:set_screen_rotate( false )

	if not aaa.b_ios then
		self.calage = CALAGE:create( self:get_name() )
	end

	APP_FACTORY.reset()

	self.__undo_redo = UNDO_REDO:create()

	return self
end

function GA:get_bus_ctx()			return self.__bus_ctx				end
--function GA:get_buss_cur()		return self:get_bus_ctx():get_buss_cur()	end

function GA:is_buss_regular_open()	return self:get_bus_ctx():is_open_regular()	end
function GA:is_buss_top_open()		return self:get_bus_ctx():is_open_top()		end

function GA:add_bus( bus )			self:get_bus_ctx():add_bus( bus )		end
function GA:add_bus_top( bus )		self:get_bus_ctx():add_bus_top( bus )	end

function GA:remove_bus( bus )		return self:get_bus_ctx():remove_bus( bus )	end

function GA:open()
	--todo
	aaa.lua.set_file_force_recheck( true )
	--todo
	self:clear_fn_and_method()

	BU.c_init()
	TEXS:c_init()

	if not aaa.b_ios then
		self.__bus_ctx = BUS_CTX:create( "of_GA" )
	end

	self:set_multitouch()	--by default at start
	self:init_ui_group()

	if not aaa.b_ios then
		if not self.cam then
			local name_symbo = "gabu_cam_default"
			self.cam = AAACAM:create( name_symbo, aaa.obj.get_no_error( name_symbo ) )
			param.set_save( self.cam:get_obj(), "ortho_size", false )
			--self.cam:set_viewport( false )
			name_symbo = "gabu_cam_perlinpinpin"
			self.cam_perlinpinpin = AAACAM:create( name_symbo, aaa.obj.get_no_error( name_symbo ) )
		end
		self.cam:set_ortho_size( 32. )	-- zoom effect at start
		self.half_size_x,self.half_size_y = self.cam:get_ortho_max_size()
		self.half_size_x,self.half_size_y = self.half_size_x*.5, self.half_size_y*.5
	end

	self.__bus_wiz = nil
	self.__b_no_ga_and_bu_windows_yet = true
	self:get_bus_ctx():open_regular()
end

function GA:find_bu_by_name_lowercase( bus_name, bu_name )
	return self:get_bus_ctx():find_bu_by_name_lowercase( bus_name, bu_name )
end

function GA:get_imgs()		return app:get_imgs()						end

--	SEQ
--
function GA:get_seqs()		return app.seqs								end
function GA:get_seq_cur()	return app.seqs:get_seq_cur()				end
function GA:get_seq_name()	return app.seqs:get_seq_cur():get_title()	end


--	BUS UI_GROUP
--	used only to regroup bus si we can switch easily part of the interface on and off
--    is/set/flip_ui_group() are what we call here
--    we use regular, top, and customer ui_group 
--	  customer is when we want the final user to access a dedicated ui and not to see the regular app
--  don't confuse with BUS_CTX and BUSS which does the rest of the job
--  2024 August this is not used for other purpose
--	
function GA:init_ui_group()
	self.__bus_ui_group = { regular={} }	-- regular is the default one
end
function GA:__get_ui_group_direct( name )
	return self.__bus_ui_group[ name or "regular" ]
end
function GA:is_ui_group_active( name )
	local gr = self:__get_ui_group_direct( name )
	return gr and gr[1] and gr[1]:is_active()
end
function GA:register_ui_group( bus, name )
	local gr = self:get_table_always( "__bus_ui_group", name or "regular" )
	table.insert( gr, bus )
end
function GA:remove_ui_group( bus )
	for name,t in PAIRS( self.__bus_ui_group ) do
		if array.remove_by_val( t, bus ) then
			break
		end
	end
end

--todo refine
--todoapp call only for customer this is a special case
function GA:free_ui_group( name )
	name = name or "regular"
	self:print( "free_ui_group( \""..name.."\" )" )
--	table.print( self.__bus_ui_group.customer, "erase_ui_customer() : __bus_ui_group.customer", 1 )
	for _,bus in IPAIRS( self.__bus_ui_group[name] ) do
--		self:print( "            "..bus )
		bus:free()
		self:remove_bus( bus )
	end
	self.__bus_ui_group[name] = {}
end
--todo move to APP
--unused
-- function GA:unregister_ui_group( bus_to_remove )
-- 	name = name or "regular"
-- 	local gr = self.__bus_ui_group[name]
-- 	for i,bus in IPAIRS( gr ) do
-- 		if bus == bus_to_remove then
-- 			self:remove_bus( bus )
-- 			table.remove( gr, i )
-- 			return
-- 		end
-- 	end
-- end
function GA:define_ui_customer( app )
	local b = app.define_ui_customer
	if b then
		self:print_debug( "redefine ui customer using the app")
		self:free_ui_group( "customer" )
		local bus = app:define_ui_customer()
		self:register_ui_group( bus, "customer" )
	else
		self:print_debug( app.." have no customer interface so we can't redefine it" ) 
	end
end

function GA:set_ui_group_active( b, name )
	name = name or "regular"
	--self:box_debug( "set_ui "..name.." "..b )
	local gr = self.__bus_ui_group[name]
	if gr then
		table.apply_method( gr, "set_active", b )
		self:print_inverse( "GA bus_ui_group "..name.." is now "..(b and "ON" or "Off") )
	else
		self:print_error( "No bus_ui_group named "..name )
	end
end

function GA:flip_ui_group_active( name )
	local gr = self:__get_ui_group_direct( name )
	if gr and gr[1] then
		self:set_ui_group_active( not self:is_ui_group_active(name), name )
		return true
	end
end
function GA:set_ui_group_active_all( b )
	for name,gr in pairs( self.__bus_ui_group ) do
		self:set_ui_group_active( b, name )
	end
end

--	BUZ
--
--todo min for now have to be extended
function GA:add_bu_wiz( name, bu )
	self:get_table_always( "__bu_wiz" )[name] = bu
end
function GA:get_bu_wiz( name )
	return self:get_table_always( "__bu_wiz" )[name]
end


--	STOP TIME
--
function GA:set_stop_time( t )			self.stop_time = t 				end
function GA:set_stop_time_factor_y( f )	self.stop_time_factor_y = f 	end
function GA:set_stop_time_factor_x( f )	self.stop_time_factor_x = f 	end

function GA:set_touch_nb( max_by_bu, max_to_interact, to_reset )
	self.max_touch_by_object	=	max_by_bu
	self.max_finger_interact	=	max_to_interact
	self.touch_nb_to_reset		=	to_reset
end

function GA:__mess_interactivity( str )
	aaa.mess.show( "Interactivity switched\nto "..str.."\n" )
end
function GA:set_arte()
	self.b_rotate = false
	self:set_touch_nb( 4, 1, 4 )
	self.__page_trs_factor = 3
	self.stop_time = .4
	self.__b_inertia = false
	self.b_one_active_video = true	--todo
	self.b_border_texture = false
	self.b_calage_allow = false
	self:__mess_interactivity( "Arte" )
end
function GA:set_multitouch()
	self.b_rotate = true
	self:set_touch_nb( 8, 8, 8 )
	self.__page_trs_factor = 2
	self.stop_time = .4
	self.__b_inertia = true
	self.b_one_active_video = false	--todo
	self.b_border_texture = true
	self.b_calage_allow = true
	self:__mess_interactivity( "Multitouch" )
end
function GA:set_win8_samsung()
	self.b_rotate = true
	self:set_touch_nb( 10, 10, nil )
	self.__page_trs_factor = 2
	self.stop_time = .4
	self.__b_inertia = true
	self.b_one_active_video = false	--todo
	self.b_border_texture = true
	self.b_calage_allow = true
	self:__mess_interactivity( "Win8_Samsung" )
end

function GA:flip_calage()
	if not self.b_calage_allow then
		aaa.mess.show( "no table calage allowed : Arte mode ?" )
		return
	end
	if not CALAGE then
		aaa.mess.show( "calage module should be manually switch On" )
	else
		self.calage:set_nb_uv( self.calage_nb_u and self.calage_nb_u or 5, self.calage_nb_v and self.calage_nb_v or 4 )
		self.calage:flip()	--todo calage
	end
end
function GA:set_screen_rotate( b )
	local obj = aaa.obj.get_from_top_by_class( "master_camera" )
	param.set( obj, "global_roll", b and .5 or 0 )
	self.b_screen_rotate = b
end
function GA:get_page_trs_factor()
	local f = self.__page_trs_factor
	if aaa.keyboard.is_ctrl() then  f = f * 4. end
	if aaa.keyboard.is_shift() then f = f * 10. end
	return f
end
local POW = 1
function GA:compute_stop_time_factor( last_time )
	--aaa.print_fn()
	local ft = aaa.time.t_real - last_time
	local stop_time = self.stop_time
	if ft < stop_time then
		ft = (stop_time-ft) / stop_time
		ft = math.pow( ft, POW )
		ft = ft * aaa.time.dt_real
	else
		ft = 0
	end
	--self:show( ft, "time_factor"  )
	return ft
end
function GA:set_reset_by_blob( b )		self.b_reset_by_blob = b		end
function GA:is_reset_by_blob()			return self.b_reset_by_blob 	end

local tab_verbose_ui =
{
	{ "GA", "BUSS", "BUS", "BU", "BUI", "BUTTON", "UNDO_REDO", "BLOBS", "BLOB", "EVENT", "VIDEO", "IMGS"	},	--"NOVATION" } )
	{ "APP", "MUS", "MU", "MEU", "MEDIA" }
}
function GA.force_verbose_def()
	aaa.print_fn()
	
	-- 	aaa.debug.print_traceback()
	--local function make( flag ) return ga.b_verbose_allow and flag or 0 end
	-- local function make( val ) return val or 0 end
	local function init_verbose( key, val )
		_G[key].verbose = val or 0
		--aaa.box_debug( "set "..key..".verbose to "..val )
	end
	
	for _, tab in IPAIRS(tab_verbose_ui) do
		for _, str in IPAIRS(tab) do
			--aaa.print( str..".verbose is ".._G[str].verbose )
			init_verbose( str, 0 )
			--aaa.print( str..".verbose is ".._G[str].verbose )
		end
	end
	--aaa.box_debug( "GA.force_verbose_def()" )
end
function GA.dump_verbose_state()
	aaa.print_fn()
	for _, tab in IPAIRS(tab_verbose_ui) do
		for _, str in IPAIRS(tab) do
			aaa.print( str..".verbose is ".._G[str].verbose )
		end
	end
end

function GA.disable_verbose()	GA.b_verbose_allow = false	end
function GA.enable_verbose()	GA.b_verbose_allow = true	end

if not aaa.b_ios then
	--todo deal with more BU and so a second colum
	function GA:__define_verbose_ui( rect, name, tab_more )
		
		local function adjust_bu( bu )
			bu:set_pos_load_save( false )
			bu:set_value_load_save( true )
		end


		local DX_INDENT = .7
		local DY_INDENT = .8
		local bus = BUS:create( "ga" )
		bus:set_bu_pos_load_save( true )
		bus:set_fname( "AAA_Ga" )
		bus:init_begin()

			local nb_v = rect.sy * 2
			local bup = bus:add_bup( { 1/10,1/(nb_v+3), 1,nb_v } ):set_sxy_col( .2,.2, 3.8 )
			local DY = .3

			local bu
			bup:move_y( 1 )
			bu = bus:add_but_target_lua(	"UIF",			nil,	BU,		"__b_uif_enabled",		true	)
				adjust_bu( bu )
				bus:move_x( DX_INDENT )
				bu = bus:add_but_target_lua(	"UIF on Shift",	nil,	BU,		"__b_uif_on_shift", 	false	):set_text( "On Shift" )
					adjust_bu( bu )
				bus:move_x( -DX_INDENT )
			bup:move_y( DY_INDENT )


			bu = bus:add_but_target_lua( "Help show",		nil,	self.help, "b_active",			false	)
				adjust_bu( bu )
				bu:set_value_load_save(false)
			bu = bus:add_but_target_lua( "Grid Top",		nil,	self, "__b_draw_grid_top",		false	)
				adjust_bu( bu )
			bu = bus:create_add_bu( SLIDER_TWO,	"Size" )
				bu:set_slider_two_target( self, "ortho_size_min","ortho_size_max",	8,12, 	8,24 )
				adjust_bu( bu )

			bu = bus:add_slider( "Menu Time Alive" ):set_show_value(true)
				:set_target_lua( BU_MENU,"__time_before_close" ):set_min_max(0, 5)
				adjust_bu( bu )
				bu:set_value(4)
			bu = bus:add_slider( "BU_SHOW Time"   ):set_show_value(true)
				:set_target_lua( BU_SHOW,"__alive_time"        ):set_min_max(0,10)
				adjust_bu( bu )
				bu:set_value(5)
			bup:move_y( DY_INDENT )

			bu = bus:add_but_target_lua(		"Finger",			nil,	GA,	"b_draw_finger",			true	)
				bup:move_x( DX_INDENT )
				bu = bus:add_but_target_lua(	"Finger number",	nil,	GA, "b_draw_finger_number", 	true	):set_text( "number" )
				bu = bus:add_but_target_lua(	"Finger as lines",	nil,	GA,	"b_draw_finger_line",		true	):set_text( "as lines" )
				bup:move_x( -DX_INDENT )
			bup:move_y( DY_INDENT )

			bu = bus:add_but_target_lua( "Far draw Less",	nil,	GA, "b_far_draw_less",		true	)
				adjust_bu( bu )
			bu = bus:add_but_target_lua( "Only Mouse",		nil,	self, "__b_mouse_only",		false	)
				adjust_bu( bu )
			bup:move_y( DY_INDENT )

			bu = bus:add_but_target_lua( "Spy",				nil,	GA, "b_spy",				false	)
				adjust_bu( bu )
			bu = bus:add_but_target_lua( "LuaJit",			nil,	GA, "b_luajit",				false	)
				adjust_bu( bu )
			bu = bus:add_but_target_lua( "LuaJit_dump",		nil,	GA, "b_luajit_dump",		false	)
				adjust_bu( bu )
				--bu = bus:add_text( "Bordel" )
			bu = bus:add_but_target_lua( "Visualization",	nil,	self, "__b_visualization",	false	)
				bu:set_pos_load_save( false )
				--	bu:set_value_load_save( true )
			bup:move_y( DY_INDENT )

			--todo 2025 MAy don't function totally (problem to dump on MEU_DIR) 
			adjust_bu( bus:add_slider( "Offset X" ):set_target_lua( self, "ortho_x_offset", 0 ):set_min_max(-8,8 ):set_color_back("x"):set_show_value(true) )
			adjust_bu( bus:add_slider( "Offset Y" ):set_target_lua( self, "ortho_y_offset", 0 ):set_min_max(-8,8 ):set_color_back("y"):set_show_value(true) )

		--	bup:move_y( 100 )
			bup:move_x( 1 )
			bup:move_y( .3 )
			--bus:move_next()
			local rect_def = {nil,nil, 4,.8} 
			bu = bus:add_text( "Verbose", rect_def )
				bu:set_ui_active( false )
				bu:set_value_load_save( false )
				bu:set_pos_load_save( false )

			local function add_sel( name, tab, field_name )

				local bu = bus:add_selector( "verbose_"..name, rect_def  ):set_text(name)
				bu:set_nb_min_0( 4 )			--todo at the end it is giving a work result	
				adjust_bu( bu )
				bu:set_text_xyf( 0,-1, 1 )
				bu:set_text_color( 0,0,0, 1 )
				--self:print( tab.." before "..tab.verbose.." "..BUS.verbose )
				bu:set_target_lua( tab, field_name, 0 )
				--self:print( tab.." after  "..tab.verbose.." "..BUS.verbose )
				return bu
			end

			local function add_sels( tab )
				for i, class_name in IPAIRS(tab) do
					local class = aaa.lua.global.get( class_name )
					if class then
						add_sel( class_name, class, "verbose" )
					else
						self:box_debug( "did not found class "..class_name )
					end
				end
			end

			--todoq we don't see it
			add_sel( "MEU FILE",	MEU, "__verbose_file" )
			add_sel( "UIF",			BU,	"__uif_verbose" )
		
			for _, tab in IPAIRS(tab_verbose_ui) do
				bus:move_next(.5)
				add_sels(tab)
			end
			

			bus:move_next()

			add_sels( tab_more )

			bu = bus:add_but_target_lua( "Display BU cur",	nil,	BU, "__b_debug_bu_cur",	false	)
				bu:set_pos_load_save( false )
			bus:move_next()
			

			--local bup = bus:get_bup()
			local rect = { nil,nil, 4, 1 }
			bu = bus:add_but_trig_method( "GABU_OBJ Tests", rect,  GABU_OBJ, "run_all_unit_tests"	)
			bu:set_pos_load_save( false )

			--bup:move_next()

		bus:init_end()

		return bus
	end

	function GA:get_bus_wiz()
		local bus = self.__bus_wiz
		if not bus then
			--aaa.box_debug( "BUS:create( \"BuGa\" )" )
			bus = BUS:create( "BuGa" )
				self.__bus_wiz = bus
				bus:set_fname( "AAA_BuGa" )
				bus:set_bu_pos_load_save( true )
				bus:set_active( true )
				bus:init_begin_add_to_ga()
				bus:init_end( true )
				self:register_ui_group( bus )
				bus:set_active( self:is_ui_group_active() ) -- when onsite we switch off before
		end
		return bus
	end

	function GA:add_ga_and_bu_windows( rect, tab_more )
		if self.__b_no_ga_and_bu_windows_yet then
			local bus = self:get_bus_wiz()
			bus:push()
				local bu = bus:add_window_with_define_method(  rect or {-.5,1, 1,2.4}, "GA", self, "__define_verbose_ui", tab_more )
					--bu:set_text_nice()
					bu:set_pos_mini( {1,2.1925, 1/8,1/8} )
					bu:set_pos_load_save( true )

				local SX = 1
				local SY = 1.7
				bu = bus:add_window_with_define_method( {.6+SX/2,2-SY*.5, 1,SY},	"BU",	BU, "define_gdebug_ui" )
					--bu:set_text_nice()
					bu:set_pos_mini( {1.175,2.1925, 1/8,1/8} )
					bu:set_gdebug( false )
					--bu:set_border( true )
					--bu:set_border_line( true )

				
			bus:pop()
			bus:load_bu_pos()	--todo find a nice way to force this
			GA.force_verbose_def()
			self.__b_no_ga_and_bu_windows_yet = nil
		end
	end

	function GA:add_wiz( b_fps, b_blob, b_power )
		local bus = self:get_bus_wiz()
		bus:push()	--	this a hack
					--todo make sure that when we create a BU even with no bus_cur thing go thru
					--		eventually flag and defer at bu creation level
			local s=.25
			if b_fps	then bus:add_wiz_fps(	{2,		2,	s} ) end
			if b_blob	then bus:add_wiz_blob(	{2.5,	2,	s} ) end
			if b_power	then bus:add_wiz_power(	{-4,	2,	s} ) end
		bus:pop()
	end
end

function GA:save_bu_pos()	self:get_bus_ctx():save_bu_pos()	end
function GA:save_top_level()
	self:save_bu_pos()
	if app then
		app:save_top_level()
	end
end

function GA:find_bu_by_pos( x,y )
	return self:get_bus_ctx():find_bu_by_pos( x,y )
end


function GA:get_blobs()	return self.blobs	end

function GA:update()

	aaa.jit.set( self.b_luajit )
	if self.b_luajit_dump then	aaa.jit.dumper.on()
	else						aaa.jit.dumper.off()
	end

	self.__update_index = self.__update_index + 1
	self:set_pass_info( { name="regular", index=1, pass_index=1 } )

	local cam = self.cam
	if self.ortho_size_min then
		local sx,sy				= cam:get_ortho_max_size()
		local sx_over,sy_over	= 1/sx,1/sy

		--todo this half_size_x is unclear: what the fuck it does ?
		self.half_size_x,self.half_size_y = sx,sy

		--hack for now
		param.set(	GABU.ref.draw_on_top_size_u, 	8. )
		param.set(	GABU.ref.draw_on_top_size_v, 	8. * sy * sx_over )
		param.set(	GABU.ref.simul_transfer_size_x,	sx_over	)
		param.set(	GABU.ref.simul_transfer_size_y,	sy_over	)

		--todo do it linked to time
		--aaa.show( self.ortho_size_min, "ortho_size_min")
		--aaa.show( self.ortho_size_max, "ortho_size_max")

	--	local size_target = aaa.flatland.is_draw_focus()
		local size_target = self.b_far and self.ortho_size_max or math.max(8,self.ortho_size_min)
		size_target = interpolate( size_target, cam:get_ortho_size(), .9 )
		cam:set_ortho_size( size_target )
	end

	BU.c_update()
	
	-- done to avoid doing the rest when niot inited yet
	if not self:get_bus_ctx() then return end

	if GA.b_spy then aaa.spy.push_range( self..".update()", 1 ) end

		-- here is the dance to open/close buss 

		-- table.print( self:get_buss_top_top() )
		--table.print( self:get_buss_top() )
		-- table.print( self:get_buss() )
		--self:get_buss_top():set_active( true )

		--aaa.show( self:get_buss_top_top().__bus_active_nb, "top top nb" )
		--self:box_debug( "serie of open tests")
		if self:get_bus_ctx():is_open_regular() then
			--	we need to close it and open __buss_top
			self:get_bus_ctx():open_top()
		elseif self:get_bus_ctx():is_open_top() then

			self:add_ga_and_bu_windows()

			--	self.__buss_top open we need to close it
			self:get_bus_ctx():open_top_top()
		elseif self:get_bus_ctx():is_open_top_top() then
			self:get_bus_ctx():close()
			self:get_undo_redo():reset()
		end

	if not aaa.stereo.is_field_for_update() then
		if GA.b_spy then aaa.spy.pop_range() end
		return
	end

	if self.verbose >= 3 then self:print_inverse( "update() is_field_for_update -> "..aaa.stereo.is_field_for_update() ) end

	self:do_fn( "update_before" )

	local blobs = self.blobs
	if blobs==nil then
		local obj = GABUIN.get_obj_tracking()
		if obj then
			blobs = BLOBS:create( self:get_name() )
			blobs:assign_bdd_blob( obj )
			self.blobs = blobs
		end
	end

	--todo blob are scaled here to the ga.cam but infact blob should stay canonical and accessed thru a camera
	local ox,oy, sx,sy = cam:get_blobs_offset_scale()
	--self:print( "sxy "..sx..", "..sy )
	if GA.b_spy then aaa.spy.push_range( "BLOBS", 2 ) end
		blobs:read( true, not self.__b_mouse_only, ox,oy, sx,sy )
	if GA.b_spy then aaa.spy.pop_range() end
	--blobs:read( -.5, -.5, 2 * ortho_sx, 2 * ortho_sy )
	local blob_nb = blobs:get_blob_nb()
	if blob_nb > 0 then
		self.time_no_touch = 0
		self.frame_no_touch = 0
	else
		self.time_no_touch = (self.time_no_touch or 0) + aaa.time.dt
		self.frame_no_touch = (self.frame_no_touch or 0) + 1
		if self.frame_no_touch > 2 then
			self:unregister_uif_all()	--clean left over from lua error dialog for example
		end
	end

	--	UNTOUCH from top to bottom
	self:get_bus_ctx():deal_with_untouch( blobs ) 

	--	TOUCH from top to bottom
	if blobs:get_touch_nb() > 0 then
		local b_touched = self:get_bus_ctx():deal_with_touch( blobs )
		if not b_touched then
			self:print( "do_touch_none" )
			if app and app.do_touch_none then
				app:do_touch_none( blob_nb )
			end
		end
	end

	--	RESETS
	if self:is_reset_by_blob() then
		if blob_nb == 10 and blobs:is_reset_pos(10) then
			local reset_time = 2.
			blobs.reset_10 = blobs.reset_10 + aaa.time.dt
			if blobs.reset_10 > reset_time then	self:get_bus_ctx():get_regular():reset_all_page()	--not sur that the only buss or the one we should reset
			else 								aaa.mess.show( "reset all pages in "..math.floor((1-blobs.reset_10/reset_time)*10) )
			end
		else
			blobs.reset_10 = 0
		end
		if blob_nb == 8 and blobs:is_reset_pos(8) then
			local reset_time = 1.
			blobs.reset_8 = blobs.reset_8 + aaa.time.dt
			if blobs.reset_8 > reset_time then	BUS.static_reset_page_current()
			else								aaa.mess.show( "reset page in "..math.floor((1-blobs.reset_8/reset_time)*10) )
			end
		else
			blobs.reset_8 = 0
		end
		if blob_nb == 6 and blobs:is_reset_pos(6) then
			local reset_time = .5
			blobs.reset_6 = blobs.reset_6 + aaa.time.dt
			if blobs.reset_6 > reset_time then	aaa.mess.show( "AAATouch By Maa\n  and Franz Hildgen\nVersion GaBu v0.9\nmaa@lagraine.com" )
			end
		else
			blobs.reset_6 = 0
		end
	end


	--	update buss then buss_top
	--todo	should we revert order and even do updates
	if GA.b_spy then aaa.spy.push_range( "BUSS UPDATE", 2 ) end

	--	don't do it when already done
	--self:print_inverse( "---------------------"..blob_nb )
	if blob_nb ~= 0 then
		BUS.__b_clean_contact_need = true
		BUS.__b_clean_contact = false
	else
		if BUS.__b_clean_contact_need then
			BUS.__b_clean_contact_need = false
			BUS.__b_clean_contact = true
		else
			BUS.__b_clean_contact = false
		end
	end

	self:get_bus_ctx():update()
	if app then
		app:push_update_pop()
	end

	if GA.b_spy then aaa.spy.pop_range() end

	self:do_fn( "update_after" )
	if GA.b_spy then aaa.spy.pop_range() end

	--todo check it is ok here and finish move
	GABU.process_key()
end

--
--	PERLINPINPIN
--
if not aaa.b_ios then
	function GA:define_ui_perlinpinpin( bus, tab )
		bus:add_but_target_lua(	"Perlinpinpin",			nil,	self,		"b_perlinpinpin",				true	)
		bus:add_but_target_lua(	"Perlinpinpin Back",	nil,	self,		"b_perlinpinpin_draw_before",	true	)
		for i=1,8 do
			bus:add_but_target_lua( "Perlinpinpin_"..i,	nil,	self,		"b_perlinpinpin_"..i,	tab and tab[i] or false )
		end
	end
	function GA:init_perlinpinpin()
		if not self.__perlinpinpin then
			local factors = { 1, 1, 1, 4 }
			local perlin = {}
			self.__perlinpinpin_module_name	= "gabu_perlinpinpin_white_module"
			local module = aaa.obj.get( self.__perlinpinpin_module_name )
			for i = 1, 8 do
				local per = {}
				per.ref = {}
				per.nb_factor = factors[i]
				local ref = per.ref
				ref.layers		=	aaa.obj.get_branch_by_name_symbo( module, "layers_"..i )
					ref.bdd_part	=	aaa.obj.get_down_by_class( ref.layers, "bdd_particle" )
						ref.part_nb		=	param.get_ref( ref.bdd_part, "nb_to_create_trig" )
						param.set( ref.bdd_part, "render_scale_v", 8. )
						param.set( ref.bdd_part, "render_scale_axe", 4.5 )
				perlin[i] = per
			end
			self.__perlinpinpin = perlin
		end
	end
	function GA:draw_perlinpinpin()
		self:init_perlinpinpin()
		--self:print( "toto" )
		--table.print( self.__perlinpinpin, "from GA" )
		ga.cam_perlinpinpin:update()
		--aaa.draw_axe_and_plane( 4 )
		local factors = { .5, .1, .25, 32, 4.,  5, 5, 1 }
		for i,per in IPAIRS( self.__perlinpinpin ) do
			local f = factors[i]
			if f > .0 and self["b_perlinpinpin_"..i] then
				--self:print( i )
				local ref = per.ref
				param.set( ref.part_nb, self.blobs:get_blob_nb() * f )
				aaa.obj.update_then_draw( ref.layers )
			end
		end
		self.cam:update()
	end
	GA.b_perlinpinpin_draw_before = false
	function GA:draw_perlinpinpin_before()
		if self.b_perlinpinpin and self.b_perlinpinpin_draw_before then self:draw_perlinpinpin() end
	end
	function GA:draw_perlinpinpin_after()
		if self.b_perlinpinpin and not self.b_perlinpinpin_draw_before then self:draw_perlinpinpin() end
	end
end

function GA:register_uif( bu )
	if bu then
		self.__uif_bu[ bu ] = bu
	end
end
function GA:unregister_uif( bu )
	self.__uif_bu[ bu ] = nil
end
function GA:unregister_uif_all()
	for _, bu in pairs( self.__uif_bu ) do
		bu:end_uif( false, "GA:unregister_uif_all()" )
	end
end

function GA:is_3d()
--	aaa.show( self.__b_3d )
	return self.__b_3d
end

-- KEYBOARD
--
function GA:do_key( key )
	self:print_debug( "GA:do_key( "..key.." )" )
	local b_key_used = self.help:do_key( key )	
-- BU
	if not b_key_used then
		local bu = GABU.__get_bu_for_key()
		if bu and bu.do_key then
			self:print_debug( "try to use key "..key.." with BU "..bu )
			b_key_used = bu:do_key( key )
			self:print_debug( "BU "..bu..(b_key_used and "" or " did not").." use key "..key )
		end
	end
-- APP
	if not b_key_used and app then
		self:print_debug( "try to use key "..key.." with APP "..app )
		b_key_used = app:do_key( key )
		self:print_debug( "APP "..app..(b_key_used and "" or " did not").. " used key "..key )
	end
-- GA
	local b_ctrl = aaa.keyboard.is_ctrl()
	local b_shift = aaa.keyboard.is_shift()
	if not b_key_used then
		b_key_used = true
		if		key == 26	then				self:undo()			-- CTRL z		
		elseif	key == 25	then				self:redo()			-- CTRL y
		elseif	key == 9 and b_ctrl then		self:do_action( "do_key", "flip_far" )	-- CTRL tab
		elseif	key == 77 or key == 109 then						-- m,M
			--ga:set_multitouch()
			aaa.os.pop_console()
		else
			b_key_used = false
		end
		self:print_debug( "GA "..(b_key_used and "" or "did not ") .. "used key "..key )
	end

	return b_key_used
end

function GA:do_key_special( key )
	aaa.print_fn()
	local b_key_used = self.help:do_key_special( key )
-- BU
	if not b_key_used then
		local bu = GABU.__get_bu_for_key()
		if bu and bu.do_key_special then
			bu:print_debug( "try to use key special "..key )
			b_key_used = bu:do_key_special( key )	
			bu:print_debug( (b_key_used and "" or "did not ").. "used key special "..key )
		end
	end
-- APP
	if not b_key_used and app then
		app:print_debug( "try to use key special "..key )
		b_key_used = app:do_key_special( key )
		app:print_debug( (b_key_used and "" or "did not ").. "used key special "..key )
	end
-- GA
	if not b_key_used then
		local is_key_fn = aaa.keyboard.is_key_fn
		if is_key_fn( key, 1 ) and aaa.keyboard.is_ctrl() and aaa.keyboard.is_shift() then
			-- SHIFT CTRL F1
			self:define_ui_customer( app )
			b_key_used = true
		elseif is_key_fn( key, 2 ) then	--F2 key
			aaa.print_fn()
			-- we never get fn key with only alt
			--aaa.print( "F2 with alt "..aaa.keyboard.is_alt() )
			if aaa.keyboard.is_ctrl() then -- CTRL F2
				local inc = aaa.keyboard.is_shift() and -1 or 1
				self:inc_ui_on_screen( inc )
				self:set_ui_group_active( true )
				self:set_ui_group_active( true, "top" )
			else
				--if aaa.keyboard.is_ctrl() then
				--	BU_MONITOR.__b_lock = ga:is_ui_group_active()
				--end
				if not aaa.keyboard.is_shift() then
					self:flip_ui_group_active( "top" )
				end
				self:flip_ui_group_active()
			end	
			b_key_used = true
		elseif is_key_fn( key, 3 ) then	--F3 key
			--todo	move this to APP_GP
			local gp = GP.cur
			if gp then
				gp:flip_render()
				b_key_used = true
			end
		end
	end

	if not b_key_used then
		self:print_debug( "KEY SPECIAL NOT USED "..key )
	end
	return b_key_used
end

function GA:reset_draw( b_no_fbo )
	--gol.reset()
	gol.set_default()	--todo refine, for now replace reset which don't stop shader
	if b_no_fbo then
		aaa.fbo.unbind()
	end
	if self.cam then
		self.cam:update()
	end
end

function GA:draw_grid()
	aaa.grid.draw()
end

--todo perhaps we should have a more general method to apply a method to all bus_ctx/bus using the down/up mecanisn
function GA:apply_method_to_all_bu( method_name, ... )
	local bus_ctx = self:get_bus_ctx()
	if bus_ctx then
		bus_ctx:apply_method_from_bottom( method_name, ... )
	end
end

function GA:draw()
	if GA.b_spy then aaa.spy.push_range( self..".draw()", 1 ) end
	--aaa.print_method()

	if self.verbose >= 3 then self:print_inverse( "draw() is_field_for_update -> "..aaa.stereo.is_field_for_update() ) end
	-- DRAW
	self:reset_draw()

		self:do_fn( "draw_before" )

			self:draw_perlinpinpin_before()

				self:reset_draw( true )
				if app then
					app:push_draw_pop()
					self:reset_draw( true )
				end
			
-- 					self.__buss:draw()
-- --					self:reset_draw( true )
-- 					self.__buss_top:draw()
-- --					self:reset_draw( true )
-- 					self.__buss_top_top:draw()

				local bus_ctx = self:get_bus_ctx()
				if bus_ctx then
				   	bus_ctx:draw()
				end
				--self:get_bus_ctx():draw()

				--todo why not on top
				--table.print( self.__uif_bu )
				for _, bu in pairs( self.__uif_bu ) do
					--self:print( "try to draw_uif "..bu )
					--if bu:is_uif_running() then
						bu:draw_uif()
					--end
				end

			self:draw_perlinpinpin_after()

			self.help:draw()

			if self.__b_draw_grid_top then
				self:draw_grid()
			end

			if self.b_draw_finger then
				aaa.obj.update_then_draw( self.ref.layers_fingers )
			end

		self:do_fn( "draw_after" )

	--self:print( "value is "..self.__b_draw_grid_top )

	if not aaa.b_ios then
		self.calage:update()
		self.calage:draw()
	end

	if GA.b_spy then aaa.spy.pop_range() end
end

function GA:set_ui_intercept( bdd )
--	if true then return end

	if self.__bdd_ui_intercept == bdd then return end

	if self.__bdd_ui_intercept then
		aaa.bdd.set_ui_intercept( self.__bdd_ui_intercept, false )
		self.__bdd_ui_intercept = nil
	end
	if bdd then
		aaa.bdd.set_ui_intercept( bdd, true )
		self.__bdd_ui_intercept = bdd
	end
end
function GA:is_ui_intercept( bdd )
	return self.__bdd_ui_intercept and self.__bdd_ui_intercept == bdd
end

function GA:set_keyboard_physical_target_virtual( keyboard_virtual )
	if self.__keyboard_virtual and keyboard_virtual then self:show("Already a keyboard targeted") return end

	self.__keyboard_virtual = keyboard_virtual
end

function GA:get_keyboard_virtual()
	return self.__keyboard_virtual
end

function GA:do_action( str_src, str_action, ...  )	--todo use the ...
	local str = string.lower(str_action)
	local mess = "from "..str_src.." GA:do_action( \""..str.."\" )"	--todo add the ..,
	self:print( mess )

	local b_used = true

	if		str == "exit" or
			str == "quit_no_save"	then aaa.quit_no_save()
	elseif	str == "quit"			then aaa.quit()
	elseif	str == "shutdown"		then aaa.shutdown_no_save()
	elseif	str == "reboot"			then aaa.reboot_no_save()
	elseif	str == "save"			then aaa.save()
	elseif  str == "flip_far"		then self.b_far = not self.b_far
										 self:print( "change far to "..self.b_far )
	else
			self:print( "APP:do_action() don't do "..str )
			b_used = false
	end

	return b_used
end

-- these fns are used for undo/redo
-- function GA:change_bu_value( bu, val, id )
-- 	--if self:is_send() then return end	
-- 	local str = "GA:change_bu_value( "..bu..", "..val..", "..id.." )"
-- 	self:print_inverse( str )
-- 	bu:set_value( val, id )
-- end

function GA:get_undo_redo()				return self.__undo_redo				end
function GA:add_undo_redo( undo, redo )	self:get_undo_redo():add_undo_redo( undo, redo )	end
--function GA:add_undo_redo_elt(elt)	self:get_undo_redo():add_elt(elt)	end
function GA:undo()						self:get_undo_redo():undo()			end
function GA:redo()						self:get_undo_redo():redo()			end

-- DIALOG
--
function GA:init_dummys_used_for_edit()
	local obj = aaa.ref.app
	local names = { "int32", "double", "string" }

	local tab = {}
	local pgr = param.get_ref
	for _,s in pairs( names ) do
		local t = {}
		for i=1,8 do
			t[i] = pgr( obj, "Dummy_lua_"..s.."_"..i )
			param.set_save( t[i], false )
		end
		tab[s] = { free=t, used={} }
	end
	self.__dummys = tab
	return tab 
end
function GA:get_dummy_to_use( param_type )
	local dummys = self.__dummys
	if not dummys then
		dummys = self:init_dummys_used_for_edit()
	end
	--self:print( "param_type is "..param_type )
	local dum = dummys[param_type]
	local r = table.remove( dum.free )
	if not r then
		aaa.bell()
		self:print_error( "GA:get_dummy_to_use( "..param_type.." ) no free param: need to close dialogs probably" )
	end
	table.insert( dum.used, r )
	return r
end
function GA:free_dummy( param_type, r )
	local dummys = self.__dummys
	if dummys then
		local dum = dummys[param_type]
		for i,v in PAIRS( dum.used ) do
			if v == r then
				table.remove( dum.used, i )
				table.insert( dum.free, r )
				return
			end
		end
	end
end

	--todo do we need to do better
function GA:get_dialog_bus()
	local buss = self:get_bus_ctx():get_regular()
	local i = -1
	local bus
	repeat
		bus = buss:get_bus(i)
		self:print( "i "..i.." - "..bus )
		if not bus then break end
		i = i - 1
	until bus:is_active()
	if not bus then
		self:print_error( "Ga Don't find a bus for to display dialog." )
	end
	return bus
end

local function count_line_char_max(s)
    local line_count = 0
    local max_chars = 0
    local current_chars = 0
    
    for i = 1,#s do
        local char = s:sub( i,i )
        if char == '\n' then
            line_count = line_count + 1
            if current_chars > max_chars then
                max_chars = current_chars
            end
            current_chars = 0
        else
            current_chars = current_chars + 1
        end
    end
    
    -- Count the last line if the string doesn't end with newline
    if #s > 0 and s:sub(-1) ~= '\n' then
        line_count = line_count + 1
        if current_chars > max_chars then
            max_chars = current_chars
        end
    end
    
    return line_count, max_chars
end

function GA:add_dialog_message( str, rect )
	--aaa.print_fn()
	local bus = self:get_dialog_bus()
	local bu

	if bus then
		bus:push()	-- should it be done in bus ?

			local nb_line, nb_char = count_line_char_max(str)
			--local _, nb = string.gsub( str, "\n", "\n" )
			--nb = nb + 1
			local x = (rect and rect[1]) or 0
			local y = (rect and rect[2]) or 0
			local sx = (rect and rect[3]) or nb_char/16
			local sy = (rect and rect[4]) or nb_line/8

			--aaa.print_fn()
			--self:print( rect[1].." "..rect[2] )
			bu = bus:add_trig( str, {x,y, sx,sy} )	
				bu:set_text( str )
				--todo does not function
				--bu:set_text_align_x( "right" )
				bu:set_text_fxy( 1, 1/nb_line )
				bu:set_text_y( 1 - .1*nb_line/3 - 1/nb_line )
				bu.get_color_back =
					function()
						local ph = triangle_01(aaa.time.t*2)
						ph = aaa.math.gain_bias( ph, .5, .25 )
						return { ph*.75,0,0,  .8 * BU.__class_alpha_factor }
					end
				bu:set_pos_load_save( false )
				bu:set_visible( true )
				bu:move_to_front( true )
				bu:set_text_nice()
				bu:set_ui_top_size( true )
				bu:set_method_on_click( bu, "destroy_self" )
				aaa.bell()
			--	bu:set_confirmation( true, str_ok, str_cancel, b_color_flip, true )

		bus:pop()
		--ga.bu_doc:open_table( "Dialog", bu )
	end

	return bu
end

function GA:add_dialog_confirm( str, rect, str_ok, str_cancel, b_color_flip, ... )
	local bu = self:add_dialog_message( str, rect )
	if bu then
		bu:set_method_on_click( ... )
		bu:set_confirmation( true, str_ok, str_cancel, b_color_flip, true )
	end
end

--todo use the method stuff already done
GA.doc.add_dialog_edit =
{
	"( dialog_table ) which can contain fields: title, bu,balue_id, param_ref, param_type,tab.value_begin,b_simple,",
	"but also obj,method, signature",
	"create a dialog to edit a value,",
	"if bu is passed, a unique id is generated from it to avoid opening a second dialog using the same bu",
	"param_ref_or_type can be a string defining what type of value we edit (string, int32, double 2024 Sep),",
	"or a param_ref and so we will edit its value (and pick the type and some infos from it),",
	"b_simple choose between a simple and more complex dialog (to be refined later 2024 Sep),",
	"a table dialog_table is created where most of information are stored:",
	"  bu,balue_id, param_ref,param_type, obj,method, args=pack(...)", 
	"hook will call obj[method]( obj, what, value, dialog_table )",
	"Where what is a string: open, close, change_value (for now 2024 Sep)",
	"internally it use aaa.edit_dialog and aaa.dialog_hook",
}
function GA:add_dialog_edit( dialog_table )
	local ga_dialog = self.__dialog
	local bu, balue_id = dialog_table.bu, dialog_table.balue_id

	aaa.print_fn( "GA:add_dialog_edit" )

	local id_unique
	if bu then
		--self:print( bu.."" )
		--self:print( bu..""..balue_id )
		id_unique = bu..""
		if balue_id ~= nil then
			id_unique = id_unique..balue_id
		end
		if ga_dialog.by_id_unique[id_unique] then
			aaa.bell()
			self:print_debug( "Only one dialog for a BU value: id used was "..id_unique )
			return
		end	
	end

	-- title = title.." "..obj:get_name()
	-- if bu then
	-- 	local m = bu.get_meu_up and bu:get_meu_up()
	-- 	if m then
	-- 		title = title .. " In " .. m
	-- 	else
	-- 		local bus = bu.get_bus_up and bu:get_bus_up()
	-- 		if bus then
	-- 			title = title .. " In " .. bus
	-- 		end
	-- 	end
	-- end
	
	local param_ref, param_type = dialog_table.param_ref, dialog_table.param_type
	local value_begin = dialog_table.value_begin
	--table.print( dialog_table, "dialog_table" )
	if not param_ref then
		param_ref = ga:get_dummy_to_use( param_type )
		if not param_ref then
			return
		end
		-- type come from the lua caller
		if param_type=="string" then
			local val = value_begin	--or ((bu and bu.get_text) and bu:get_text())	--todo should go thru the value thing
			if val then
				param.set( param_ref, val )
			end
		elseif param_type=="int32" or param_type=="double" then
			local balue = bu:__get_balue(balue_id)
			local val = value_begin or balue:get_value()
			param.set( param_ref, val )
			local min = balue:get_min()
			local max = balue:get_max()
			param.set_min( param_ref, min )
			param.set_max( param_ref, max )
			--todo default value for a menu ?
		else
			aaa.bell()
			self:print_debug( "No dialog for type"..param_type )
			return
		end
	else
		if dialog_table.value_begin then
			param.set( param_ref, value_begin )
		end
		if param.is_type_number(param_ref) then
			if param.is_type_real(param_ref) then
				param_type = "double"
			else
				param_type = "int32"
			end
		elseif param.is_type_text(param_ref) then
			param_type = "string"
		else
			aaa.bell()
			self:print_debug( "No dialog for param type "..param.get_type_str(param_ref) )
			return
		end
	end
	if ga_dialog.by_ref[param_ref] then
		aaa.bell()
		self:print_debug( "Only one dialog for a value" )
		return
	end
	dialog_table.param_type = param_type
	dialog_table.param_ref = param_ref
	dialog_table.method = dialog_table.method or "do_dialog_hook"

	local dialog_id = ga_dialog.id + 1
	ga_dialog.id = dialog_id;
	dialog_table.dialog_id = dialog_id
	ga_dialog.by_id[dialog_id] = dialog_table
	ga_dialog.by_ref[param_ref] = dialog_table

	self:print( "id_unique is " .. id_unique )
	if id_unique then
		dialog_table.id_unique = id_unique
		ga_dialog.by_id_unique[id_unique] = dialog_table
	end
	self:print( "GA:add_dialog_edit() obj is "..dialog_table.obj )

	table.print( dialog_table, "dialog_table" ) 
	aaa.edit_dialog( dialog_id, param_ref, nil, dialog_table.b_simple, dialog_table.title, param_type=="int32" )

	return dialog_table
end


--todo dealloc after use
--todo add id for balue
function aaa.dialog_hook( dialog_id, what, value )
	local b_ret
	aaa.print_fn()
	--aaa.print( "ToTo" )
	if dialog_id and dialog_id > 0 then
		local ga_dialog = ga.__dialog
		local dialog_table = ga_dialog.by_id[dialog_id]
		local obj = dialog_table.obj
		local method = dialog_table.method
		aaa.print( "aaa.dialog_hook \""..what.." \"for "..obj.." with "..method )
		b_ret = obj[method]( obj, what, value, dialog_table )
		if what == "open" then
			aaa.print( "aaa.dialog_hook() dialog "..dialog_id.." open" )
		elseif what == "close" then
			ga_dialog.by_id[dialog_id] = nil
			ga_dialog.by_ref[dialog_table.param_ref] = nil
			if dialog_table.id_unique then
				ga_dialog.by_id_unique[dialog_table.id_unique] = nil
			end
			ga:free_dummy( dialog_table.param_type, dialog_table.param_ref )
			aaa.print( "aaa.dialog_hook() dialog "..dialog_id.." close" )
		elseif what == "change_value" then
		else
			aaa.print_error( "aaa.dialog_hook() dialog "..dialog_id.." unimplemented message : "..what )
		end
	end
	return b_ret
end

if not aaa.b_ios then
	--	OSC
	--
	function OSC_MESS:do_resolution()
		self:pop_tag()
		self:dump( "resolution" )

		local dif = app:get_meu_by_name_no_error( "fbo_dif" )
		if not dif then return end

		local what = self:get_tag()
		if 		what == "x"		then
			aaa.print( "resolution x "..self:get_arg() )
			dif:set_pixel_size( self:get_arg() )
		elseif	what == "y"		then
			aaa.print( "resolution y "..self:get_arg() )
			dif:set_pixel_size( nil, self:get_arg() )
		end
	end

	function OSC_MESS:do_gui()
		self:pop_tag()
		self:dump( "gui" )
		local tag = self:get_tag()
		if tag == "show" then
			self:pop_tag()
			local arg = self:get_arg()
			aaa.print( "gui "..arg )
			if ga then ga:set_ui_group_active( arg~=0 ) end
		end
	end

	function OSC_MESS:do_aaaseed()
		local tag = self:get_tag()

		if tag == "resolution"								then	self:do_resolution	()
		elseif tag == "signature1" or tag == "signature"	then	self:do_signature	( 1 )
		elseif tag == "gui"									then	self:do_gui			()
		elseif tag == "kinect1"	or tag == "kinect"			then	self:do_kinect		( 1 )
		elseif tag == "process"								then	local gp = GP.cur
																	if gp then gp:set_render( self:get_arg()~=0 ) end
		elseif tag == "edit"								then	aaa.set_edit( self:get_arg()~=0 )
		elseif tag == "exit_no_save"						then	aaa.quit_no_save()
		elseif tag == "save"								then	MEU:save_all()
																	aaa.save()
		else		self:dump( "unimplemented" )
		end
	end
end

