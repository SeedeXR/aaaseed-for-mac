local caller = aaa.get_caller()
local key = "maa_ui_"..caller

local pt = aaa[key]
if not pt then
	aaa[key] = {}
	pt = aaa[key]
	pt.layers = aaa.layers.get_cur()

--	pt.layer_1 = 	param.get_ref( pt.layers, "Layer_C" )
--	pt.layer_2 = 	param.get_ref( pt.layers, "Layer_D" )
--	pt.cam_view = 	param.get_ref( pt.layers, "camera_index_view" )
--	pt.cam_ui = 	param.get_ref( pt.layers, "camera_index_ui" )

--	pt.cam =	aaa.obj.get( "kinect_ui_cam" )
--	pt.cam_trig_x	=	param.get_ref( pt.cam, "trig_align_x" )
--	pt.cam_trig_y	=	param.get_ref( pt.cam, "trig_align_y" )
--	pt.cam_trig_z	=	param.get_ref( pt.cam, "trig_align_z" )
--	pt.cam_lock		=	param.get_ref( pt.cam, "ui_lock" )

--	pt.layer_ui = 	aaa.obj.get( "kinect_draw_layer_A" )

	local function do_comment( id, name )
		pt[id] = {}
		local t = pt[id]

		local obj = aaa.obj.get( name )	--aaa.obj.get( name..id )
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
	do_comment( 1, "kinect_bdd_opencl_fool" )
--	do_comment( 2, "kinect_bdd_opencl_" )
end
