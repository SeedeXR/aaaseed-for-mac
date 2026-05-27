local caller = aaa.get_caller()
local key = "maa_ui_"..caller

local pt = aaa[key]
if not pt then
	aaa[key] = {}
	pt = aaa[key]
	pt.layers = aaa.layers.get_cur()
	pt.fbo = 		param.get_ref( pt.layers, "FBO" )

	pt.layer_1 = 	param.get_ref( pt.layers, "Layer_C" )
	pt.layer_2 = 	param.get_ref( pt.layers, "Layer_D" )
	pt.cam_view = 	param.get_ref( pt.layers, "camera_index_view" )
	pt.cam_ui = 	param.get_ref( pt.layers, "camera_index_ui" )

	pt.cam =	aaa.obj.get( "kinect_ui_cam" )
	pt.cam_trig_x	=	param.get_ref( pt.cam, "trig_align_x" )
	pt.cam_trig_y	=	param.get_ref( pt.cam, "trig_align_y" )
	pt.cam_trig_z	=	param.get_ref( pt.cam, "trig_align_z" )
	pt.cam_lock		=	param.get_ref( pt.cam, "ui_lock" )

	pt.layer_ui = 	aaa.obj.get( "kinect_draw_layer_A" )

	local function do_comment( id, name )
		pt[id] = {}
		local t = pt[id]

		local obj = aaa.obj.get( name..id )
		t.obj = obj

		t.param = {}
		local p = t.param
		for j=1,9 do
			p[j] = param.get_ref( obj, "param_0"..j )
		end
		p[10] = param.get_ref( obj, "param_10" )

		t.ptra = {}
		t.ptra[1] = param.get_ref( obj, "vec_01_x" )
		t.ptra[2] = param.get_ref( obj, "vec_01_y" )
		t.ptra[3] = param.get_ref( obj, "vec_01_z" )

		t.prot = {}
		t.prot[1] = param.get_ref( obj, "vec_02_x" )
		t.prot[2] = param.get_ref( obj, "vec_02_y" )
		t.prot[3] = param.get_ref( obj, "vec_02_z" )

		p = t.param
		param.set_comment( p[1], "A camera z close" )
		param.set_comment( p[2], "A camera z far" )
		param.set_comment( p[3], "F 2D translation x" )
		param.set_comment( p[4], "F 2D translation y" )
		param.set_comment( p[5], "G 2D rotation" )
		param.set_comment( p[6], "H height factor (0 for calage)" )
		param.set_comment( p[7], "I height min" )
		param.set_comment( p[8], "I cm epaisseur pour calage" )

		p = t.ptra
		param.set_comment( p[1], "B 3D translation x" )
		param.set_comment( p[2], "B 3D translation y" )
		param.set_comment( p[3], "B 3D translation z" )

		p = t.prot
		param.set_comment( p[1], "C 3D rotation x" )
		param.set_comment( p[2], "D 3D rotation y" )
		param.set_comment( p[3], "E 3D rotation z" )

		if id == 2 then
			param.set( obj, "texture_1_bind", aaa.pc.is_maa() and 32 or 28 )
		end
	end
	do_comment( 1, "kinect_bdd_opencl_" )
	do_comment( 2, "kinect_bdd_opencl_" )

	aaa.lua.global.declare( "kinect_ui" )
	kinect_ui = {}
	kinect_ui.b_view = false
	kinect_ui.b_visu = false
	kinect_ui.b_a = true
	kinect_ui.b_b = true
end

function kinect_ui.build_one( id )
	local bu
	local t = aaa[key][id]

	kinect_ui.bus = bus_cur

	local function add_slider( par, name, min,max )
		return bus_cur:add_slider_target( mame, nil, par,nil, nil, min,max )
	end

	--add_slider( par[1], "dist min", .40, 6.00 )

	local p
	p = t.param
	add_slider( p[8], "cm helper", 2, 40. )
	bus_cur:move_next()

	p = t.ptra
	add_slider( p[1], "Move X", -10, 10 )
	add_slider( p[2], "Move Y", -10, 10 )
	add_slider( p[3], "Move Z", -10, 10 )
	bus_cur:move_next()

	p = t.prot
	add_slider( p[1], "Rotate X", -1, 1 )
	add_slider( p[2], "Rotate Y", -1, 1 )
	add_slider( p[3], "Rotate Z", -1, 1 )
	bus_cur:move_next()

	p = t.param
	add_slider( p[3], "Adjust X", -5, 5 )
	add_slider( p[4], "Adjust Y", -5, 5 )
	add_slider( p[5], "Rotate", -1, 1 )
	bus_cur:move_next()

--	add_slider( p[6], "Height factor", 0, 1 )
	add_slider( p[7], "Height min", 0, 1 )
end

function kinect_ui.build()
	local bu
	bus_cur:add_bup( {-4,nil, -2,2} ):set_size( 2, .2 )

	bu = bus_cur:add_text_here( "A" )
	bu:set_text_factor( 3. )
	bu:set_text_xy( 4.5 )
	kinect_ui.build_one( 1 )

	bup:set_lbrt( { 2, nil, 4, 2 } )
	bu = bus_cur:add_text_here( "B" )
	bu:set_text_factor( 3. )
	bu:set_text_xy( 4.5 )
	kinect_ui.build_one( 2 )

	local function add_but( text, min, max )
		local bu = BUTTON:create( text )
		--bu:set_target_param( par )
		--bu:set_min_max( min, max )
		bus_cur:add_bu( bu )
		return bu
	end

	bup:set_lbrt( { -4, 2.3, 2.5, 2.4 } )
	bup:set_size( .9, .15 )
	bup:start()

	bu = add_but( "A" )
	bu:set_target_lua( kinect_ui, "b_a" )
	bup:move_next()

	bu = add_but( "View" )
	bu:set_target_lua( kinect_ui, "b_view" )
	bu = add_but( "Cam_Lock" )
	bu:set_target_param( pt.cam_lock )


	bu = add_but( "Helper" )
	bu:set_target_lua( kinect_ui, "b_visu" )

	bup:move_next()
	bu = add_but( "B" )
	bu:set_target_lua( kinect_ui, "b_b" )


	bup:set_lbrt( { -1.5, -2.2, 1.5, -2 } )
	bup:set_size( 1, .15 )
	bup:start()

	bu = add_but( "side" )
		bu:set_target_param( pt.cam_trig_x )
		bu:set_trig()
	bu = add_but( "top" )
		bu:set_trig()
		bu:set_target_param( pt.cam_trig_y )
	bu = add_but( "front" )
		bu:set_target_param( pt.cam_trig_z )
		bu:set_trig()

end

function kinect_ui.build_visu()
	local function add_tex( name,	x,y, bind )
		local SY = 2.2
		local SX = SY*4/3
		local D = .02
		local bu
		bu = bus_cur:add_monitor( name, {x*(SX*.5+D),y*(SY*.5+D), SX,SY}, bind )
	end

	add_tex( "A"		,-1,1,	44 )
	add_tex( "B"		,1,1,	45 )
	add_tex( "A Depth"	,-1,-1,	32 )
	add_tex( "B Depth"	,1,-1,	28 )
end

function kinect_ui.update()
	if not kinect_ui.bus then return end

	local b
	b = kinect_ui.b_view and kinect_ui.bus:is_active()
	--aaa.print( b )
	param.set( pt.fbo, b and "none" or "Once" )
	local cam = b and 1 or 0
	param.set( pt.cam_view,	cam )
	param.set( pt.cam_ui,	cam )
	if b then
		--aaa.obj.set_focus_ui( pt.layer_ui )
	end

	param.set( pt.layer_1, kinect_ui.b_a or not b )
	param.set( pt.layer_2, kinect_ui.b_b or not b )
	--	visualisation mode
	b = kinect_ui.b_visu and b
	local f = b and 0 or .5
	param.set( pt[1].param[6], f )
	param.set( pt[2].param[6], f )

end

kinect_ui.update()

