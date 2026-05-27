aaa.lua.global.declare_table( "GABUIN" )

--	UTIL
local layers_cached = {}
function GABUIN.get_layers( name )
	local layers = layers_cached[name]
	if layers then
		return layers
	end
	layers = aaa.obj.get_by_class_and_name_symbo( "layers", "GaBuIn_"..name.."_layers" )
	if layers then
		layers_cached[name] = layers
	end
	return layers
end
function GABUIN.set_layers( name, val )
	--aaa.debug.print_traceback()
	local obj = GABUIN.get_layers( name	)
	--print( val )
	aaa.print( "GABUIN.set_layers( \""..name.."\", "..val.." )" )
	param.set( obj, "active", val )
end

function GABUIN.get_cv_multitouch()		return aaa.obj.get_by_class_and_name_symbo( "bdd_img_cv_multitouch", "GaBuIn_cvmultitouch" )		end
function GABUIN.get_dispatch_layers()	return GABUIN.get_layers( "dispatch" )			end
function GABUIN.get_dispatch()			return GABUIN.ref.blob_dispatch	end

function GABUIN.set_dispatch_draw( b )
	aaa.print( "GABUIN.set_dispatch_draw( "..b.." )" )
	param.set( GABUIN.ref.dispatch_use_bdd, b and "update_and_draw" or "update" )
end

if not GABUIN.ref then
	GABUIN.ref = {}
	local ref = GABUIN.ref
	ref.dispatch_layers			= GABUIN.get_dispatch_layers()
	ref.dispatch_layer 			= aaa.layers.get_layer( ref.dispatch_layers, 1 )
	ref.dispatch_use_bdd 		= param.get_ref( ref.dispatch_layer, "use_bdd" )
	GABUIN.set_dispatch_draw( true )

	ref.blob_dispatch			= aaa.obj.get_by_class_and_name_symbo( "bdd_blob", "GaBuIn_blob_dispatch" )
		ref.blob_multiply			= param.get_ref( ref.blob_dispatch, "multiply" )
		ref.blob_multiply_nb		= param.get_ref( ref.blob_dispatch, "multiply_nb" )
		ref.blob_multiply_range_x	= param.get_ref( ref.blob_dispatch, "multiply_range_x" )
		ref.blob_multiply_range_y	= param.get_ref( ref.blob_dispatch, "multiply_range_y" )
		ref.blob_multiply_unstable	= param.get_ref( ref.blob_dispatch, "multiply_unstable" )

	ref.blob_dispatch_model 	= aaa.obj.get_down( ref.dispatch_layers, "GaBuIn_blob_dispatch_model" )
		ref.blob_dispatch_model_u	= param.get_ref( ref.blob_dispatch_model, "size_u" )
			param.set_save( ref.blob_dispatch_model_u, false )

	ref.blob_tracking			= aaa.obj.get_by_class_and_name_symbo( "bdd_blob_tracking", "gabu_blob_tracker" )

--	ref.simul_bdd				= aaa.obj.get( "gabu_simul_bdd" )
--		ref.simul_transfer_size_y	= param.get_ref( ref.simul_bdd, "transfer_size_y" )
end

--	PRIMITIVE
function GABUIN.init()
	local	names = {	"firefly", "ps3eye", "dirty", "touchco",
						"deform", "detect", "dispatch", "render_again", "track"
					}
	for k, v in pairs(names) do
		GABUIN.set_layers( v, false )
	end
end

function GABUIN.set_cam( name, b_send, b_receive )
	if name then
		GABUIN.cam = name
		GABUIN.set_layers( name, 		true )
		GABUIN.set_layers( "deform",	true )
		GABUIN.set_layers( "detect",	true )
	end
-- maa 2024 Jan
--	GABUIN.set_layers( "dispatch",	true )
	local obj = GABUIN.get_dispatch()
	param.set( obj, "net_send", b_send )
	param.set( obj, "net_received", b_receive )
end
function GABUIN.stop_cam_cur()
	local name = GABUIN.cam
	if name then
		GABUIN.cam = nil
		GABUIN.set_layers( name, 		false )
		GABUIN.set_layers( "deform",	false )
		GABUIN.set_layers( "detect",	false )
	end
-- maa 2024 Jan
--	GABUIN.set_layers( "dispatch",	false )
end

function GABUIN.set_render_again( b_on )
	aaa.debug.print_traceback( "set_render_again" )
	GABUIN.set_layers( "render_again", b_on )
end
function GABUIN.get_obj_tracking() 			return	GABUIN.ref.blob_tracking			end
function GABUIN.set_tracking_tuio_geo( sx, sy, ox, oy )
	local obj = GABUIN.ref.blob_tracking
	param.set( obj, "tuio_in_scale_x"	,	sx and sx or 1. )
	param.set( obj, "tuio_in_scale_y"	,	sy and sy or 1. )
	param.set( obj, "tuio_in_offset_x"	,	ox and ox or 0. )
	param.set( obj, "tuio_in_offset_y"	,	oy and oy or 0. )
end

function GABUIN.set_track( nb_blob_track )
	GABUIN.set_layers( "track", nb_blob_track>0 )
	param.set( GABUIN.get_obj_tracking(),	"blob_max_nb", 					nb_blob_track	)
	param.set( GABUIN.get_obj_tracking(),	"read_bdd_img_contour_as_src",	0				)
	param.set( GABUIN.get_obj_tracking(),	"read_bdd_blob_as_src",			1				)
end

--	CLASSIC SETTINGS
function GABUIN.set_table()
	GABUIN.init()
	GABUIN.set_cam( "firefly", false, false )
	GABUIN.set_track( 32 )
end

function GABUIN.set( cam, b_send, b_receive, b_render_again, nb_blob_track )
	aaa.print_inverse( "GABUIN.set( cam = "..cam.." , send = "..b_send.." , receive = "..b_receive.." , render_again = "..b_render_again.." , nb_blob_track = "..nb_blob_track.." )" )
	GABUIN.init()
	GABUIN.set_cam( cam, b_send, b_receive )
	GABUIN.set_render_again( b_render_again )
	GABUIN.set_track( nb_blob_track )
end

function GABUIN.set_one_blob( id, x, y, sx, sy )
	--todo store in ref table
	local bdd_blob = GABUIN.get_dispatch()
	aaa.bdd.clear_blob( bdd_blob )
	aaa.bdd.add_blob( bdd_blob, id, x, y, sx, sy )
end

function GABUIN.add_blob( nb, x, y )
	--self:print( "GABUIN.add_blob( nb "..nb..", x "..x..", "..y.." )" )
	if nb>0 then
		GABUIN.force_blob = { nb=nb, x=x, y=y }
	else
		GABUIN.force_blob = nil
	end
end

function GABUIN:update()
	local rx = aaa.viewport.get_ratio_x( "render" )
	--aaa.print( rx )
	param.set( self.ref.blob_dispatch_model_u, rx )
end
GABUIN:update()

if GABUIN.force_blob then
	local x,y = GABUIN.force_blob.x, GABUIN.force_blob.y
	--aaa.print( "x y "..x.." "..y.." -> "..x.." "..y )
	GABUIN.set_one_blob( 1, x, y, .0125, .025 )
end

--	SET FNS
--aaa.print( "------" )

function GABUIN.reset_remanence()
	aaa.print( "Push Reset Remanence" )
	if GABUIN and GABUIN.cv_reset_background then GABUIN.cv_reset_background() end
end

local RATIO_X = 752/480
local TAILLE_X = 1.4 ;
local TAILLE_Y = TAILLE_X / RATIO_X;

local SX = TAILLE_X -.05
local SY = .15
local X_SPACE = .05
local DX = TAILLE_X + X_SPACE
local DY = -.25

local function x( index  )		return 0 + (index-3) * DX		end
local function xleft( index  )	return x(index)	- SX*.5 + SY*.5	end
local function xl( index  )		return x(index) - TAILLE_X * .5	end
local function y( index  )
	return 1.6 + (index -1) * DY - TAILLE_Y * .5
end

local function add_tex( name, ix,iy, bind )
	local bu = bus_cur:add_monitor( name, {x(ix),y(iy), TAILLE_X,TAILLE_Y}, bind, false )
end

function GABUIN.define_monitors_out()
	add_tex( "Raw Camera"	,	1,0		,32	)
	add_tex( "Deformation"	,	2,0		,33	)
	add_tex( "Smooth"		,	2,8.5	,34	)
	add_tex( "Remanence" 	,	3,0		,35	)
	add_tex( "Blur"			,	3,8.5	,37	)
	add_tex( "Rectify"		,	4,0		,38	)
	add_tex( "Contours"		,	5,0		,39	)
end

function GABUIN.define_monitors_blobs()
	add_tex( "Blob Dispatch",	5,11.5	,40	)
	add_tex( "Blob render",		4,11.5	,41	)
end

local function add_slider( rect, text, param_ref )
	local bu = bus_cur:add_slider( text, rect )
	bu:set_target_param( param_ref )
	return bu
end

local function add_slider_std( ix, iy, text, param_ref )
	return add_slider( {x(ix),y(iy), SX,SY}, text, param_ref )
end

function GABUIN.define_ps3eye_in()
	local	obj		= aaa.obj.get( "gabu_tex_video_ps3eye" )
	local	obj_ps3 = aaa.obj.get_branch_by_class( obj, "ps3_eye_ui" )
	local	param_ref	= param.get_ref( obj_ps3, "setting_trig" )

	local bu = bus_cur:add_trig( "PS3Eye Dialog Trig", {xl(1)+SY*.5,y(1), SY,SY} )
	bu:set_target_param( param_ref )
end

function GABUIN.define_firefly_in()
	local name = "GaBu/GaBu_Input/VideoIn_FireFly/fx_a.cap_flycapture2"
--	local obj = aaa.obj.get( "gabu_firefly_tex_video" )
--	aaa.obj.update_then_draw( obj )
	local obj = aaa.obj.get( name )

	local ix = 1
	local iy = 2.5
	add_slider_std(	ix, iy,		"Brightness",	param.get_ref( name, "brightness_value"	) )
	add_slider_std(	ix, iy+1,	"Exposure",		param.get_ref( name, "exposure_value"	) )
	add_slider_std(	ix, iy+2,	"Shutter",		param.get_ref( name, "shutter_value"	) )
	add_slider_std(	ix, iy+3,	"Gain",			param.get_ref( name, "gain_value"		) )
end

function GABUIN.define_dirty_in()
end

function GABUIN.define_cv_multitouch_ui()
	local bu
	local obj = GABUIN.get_cv_multitouch()

	bu = bus_cur:add_button( "Invert",	{xleft(3),y(2.5), SY,SY}	)
		bu:set_target_obj_param( obj, "invert" )

	bu = bus_cur:add_trig( "Reset",		{xleft(3.5),y(2.5), SY,SY}	)
		bu:set_function_on_click( GABUIN.reset_remanence )

	bu = add_slider( {x(3) + X_SPACE*.5 + TAILLE_X*.5, y(3.5), TAILLE_X*2+X_SPACE, SY}, "diff_min", param.get_ref( obj, "remanence_diff_min"  ) )
		bu:set_min_max( .0, .6 )

	bu = add_slider_std( 3, 4.5, "diff_factor", param.get_ref( obj, "remanence_diff_factor" ) )
		bu:set_min_max( .0, 64 )

	--bu = bus_cur:add_button( "Button", {0,1, 1,.1} )
	--bu.min = 1
	--bu.max = 255
	--bu:set_show_value( true )

	bu = add_slider_std( 4, 2.5, "Rectify", param.get_ref( obj, "rectify_level" ) )
	bu:set_min_max( 0, 1 )

	bu = bus_cur:add_button( "Flip_u", {xleft(2),y(2.5), SY,SY}  )
		bu:set_target_obj_param( "GaBuIn_deform_cam", "rot_y" )
		bu:set_min_max( 0, .5 )

	bu = bus_cur:add_button( "Flip_v", {xleft(2.5),y(2.5), SY,SY} )
		bu:set_target_obj_param( "GaBuIn_deform_cam", "rot_x" )
		bu:set_min_max( 0, .5 )

	local bu_last

	local S = SY
	bu = bus_cur:add_dial(	"Off_x",	xleft(2)+S*.5, y(3.5),	S, S*2,	-0.25 )
	bu:set_target_obj_param( "GaBuIn_deform_cam", "tra_x" )
	bu:set_min_max( -1, 1 )
	--create_reset_button( 2.25, 3.5, bu, 0 )

	bu = bus_cur:add_dial( "Off_y",		xleft(2.5)+S*.5, y(3.5), S, S*2, -0.25 )
	bu:set_target_obj_param( "GaBuIn_deform_cam", "tra_y" )
	bu:set_min_max( -1, 1 )

	--create_reset_button( 2.75, 3.5, bu, 0 )

	bu = bus_cur:add_dial( "zoom",		xleft(2)+S*.5, y(4.5), S, S*2, -0.25 )
	bu:set_target_obj_param( "GaBuIn_deform_cam", "ortho_size" )
	bu:set_min_max( 0.1, 4 )
	bu:add_values_def( 1 )
	--create_reset_button( 2.25, 4.5, bu, 1 )

	local function define_smooth( name, ix, iy )
		bu = bus_cur:add_button( name, {xleft(ix),y(iy), SY,SY} )
		bu:set_target_obj_param( obj, name )

		bu = add_slider_std( ix, iy+1, "size_x", param.get_ref( obj, name.."_size_x" ) )
		bu:set_min_max( 1, 128 )

		bu = add_slider_std( ix, iy+2, "size_y", param.get_ref( obj, name.."_size_y" ) )
		bu:set_min_max( 1, 128 )
	end

	define_smooth( "smooth", 2, 11 )
	define_smooth( "smooth2", 3, 11	 )
end

function GABUIN.define_cv_contour_ui()
	local bu
	local obj = aaa.obj.get( "GaBuIn_detect_img_contour" )

	bu = add_slider_std( 5, 2.5, "area_min", param.get_ref( obj, "blob_area_min" ) )
	bu:set_min_max( 0, 500 )

	bu = add_slider_std( 5, 3.5, "area_max", param.get_ref( obj, "blob_area_max" ) )
	bu:set_min_max( 0, 50000 )
end


function GABUIN.define_firefly_bus()
	local bu

	local bus = BUS:create( "Firefly" )
	bus.b_mouse_only = true

	bus:init_begin()

	--START

	--todo keep default value ?

	local slider_du_prev = SLIDER.__du
	SLIDER.__du = .06
	SLIDER:set_meter( true )

		--	CAMERA
		if GABUIN.cam then
			GABUIN.define_monitors_out()
			if GABUIN.cam == "ps3eye" then
				GABUIN.define_ps3eye_in()
			elseif GABUIN.cam == "firefly" then
				GABUIN.define_firefly_in()
			else
				GABUIN.define_dirty_in()
			end
			--	CV MULTITOUCH
			GABUIN.define_cv_multitouch_ui()
			--	CONTOURS
			GABUIN.define_cv_contour_ui()
		end

		GABUIN.define_monitors_blobs()

	--restore previous state
	SLIDER:set_meter( false )
	SLIDER.__du = slider_du_prev

	local button_du_prev = BUTTON.__du
	BUTTON.__du = .06
	BUTTON:set_meter( true )

	-- sliders for Firefly In
	--BUTTON


	--bu = bus:add_button( "Button Two", {1,1, 1,.1} )
	--bu.min = 1
	--bu.max = 255
	--bu:set_show_value( true )
	--sli_fly[ "but_2" ] = bu

	--restore previous state
	BUTTON.__du = button_du_prev

--	bus:add_button_home()
--	bus:add_button_refresh()

	bus:init_end()

	return bus
end

function GABUIN.define_blob_multiply_ui()
	local bu
	local ys = 3
	local xls = 3.6
	local xlb = xleft(xls)
	bu = bus_cur:add_button( "Multiply", {xlb,y(ys), SY,SY} )
	bu:set_target_param( GABUIN.ref.blob_multiply )

	bu = add_slider_std( xls, ys+1, "multiply_nb",	GABUIN.ref.blob_multiply_nb )
	bu:set_min_max( 1, 1024 )
	bu = add_slider_std( xls, ys+2, "range_x",		GABUIN.ref.blob_multiply_range_x )
	bu:set_min_max( 0, 2. )
	bu = add_slider_std( xls, ys+3, "range_y",		GABUIN.ref.blob_multiply_range_y )
	bu:set_min_max( 0, 2. )

	bu = bus_cur:add_button( "Unstable", {xlb,y(ys+4), SY,SY} )
	bu:set_target_param( GABUIN.ref.blob_multiply_unstable )
end

function GABUIN.define_blob_bus()
	local bu
	local bus = BUS:create( "Blob" )
	bus.b_mouse_only = true
	bus:init_begin()
		GABUIN.define_blob_multiply_ui()
	bus:init_end()

	return bus
end

function GABUIN.define_firefly_ui()
	local bus_fire = GABUIN.define_firefly_bus()
	local bu
	local bus = BUS:create( "Firefly_WINDOW" )
		bus:set_active( false )
		bus:init_begin_add_to_ga()
			bu = bus:add_window( "FireFly", bus_fire,	{0,0, 8,4} )
			bus_fire:set_transfo( 8,4 )
			bu:set_pos_mini( {0.,1.7, .5,.25} )
			bu:__set_window_state( "mini" )
			bu:set_color_back( {0,.125,.125,.5} )
		bus:init_end()
	return bus
end

function GABUIN.define_blob_ui()
	local bus_blob = GABUIN.define_blob_bus()
	local bu
	local bus = BUS:create( "Blob_WINDOW" )
	bus:set_active( false )
	bus:init_begin_add_to_ga()
		bu = bus:add_window( "Blob", bus_blob,	{1,0, 2,2} )
		bus_blob:set_transfo( 2,2, 1,0 )
		bu:set_pos_mini( {1,1.7, .5,.25} )
		bu:__set_window_state( "mini" )
		bu:set_color_back( {0,0,0,.5} )
	bus:init_end()
	return bus
end

function GABUIN.add_ui()
	ga:register_ui_group( GABUIN.define_firefly_ui() )
	ga:register_ui_group( GABUIN.define_blob_ui() )
end

--if IS_BUSS_TOP_OPEN() then
--	ga:register_ui_group( GABUIN.define_firefly_ui() )
--	ga:register_ui_group( GABUIN.define_blob_ui() )
--end

