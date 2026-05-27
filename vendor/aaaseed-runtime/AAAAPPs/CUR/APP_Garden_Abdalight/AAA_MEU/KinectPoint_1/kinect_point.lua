function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()

	local SY = 1
	local ix,iy = 1,1
	local SX = 8/3
	self:add_slider(	{ix,iy,			SX,SY},		"CamX", 	ref.cl[1].bdd, "vec_01_x", 0, -10, 10. ):set_meter(false)
	self:add_slider(	{ix+SX,iy,		SX,SY},		"CamY", 	ref.cl[1].bdd, "vec_01_y", 0, -10, 10. ):set_meter(false)
	self:add_slider(	{ix+SX*2,iy,	SX,SY},		"CamZ", 	ref.cl[1].bdd, "vec_01_z", 0, -10, 10. ):set_meter(false)

	iy = iy + SY
	self:add_slider(	{ix,iy,			SX,SY},		"RotX", 	ref.cl[1].bdd, "vec_02_x", 0, -1, 1. ):set_meter(false)
	self:add_slider(	{ix+SX,iy,		SX,SY},		"RotY", 	ref.cl[1].bdd, "vec_02_y", 0, -1, 1. ):set_meter(false)
	self:add_slider(	{ix+SX*2,iy,	SX,SY},		"RotZ", 	ref.cl[1].bdd, "vec_02_z", 0, -1, 1. ):set_meter(false)

	iy = iy + SY
	self:add_slider(	{ix,iy,			SX,SY},		"TraX", 	ref.cl[1].bdd, "param_03", 0, -10, 10. ):set_meter(false)
	self:add_slider(	{ix+SX,iy,		SX,SY},		"TraY", 	ref.cl[1].bdd, "param_04", 0, -10, 10. ):set_meter(false)
	self:add_slider(	{ix+SX*2,iy,	SX,SY},		"TraZ", 	ref.cl[1].bdd, "param_05", 0, -10, 10. ):set_meter(false)

	iy = iy + SY
	self:add_slider(	{ix+SX,iy,		SX,SY},		"RotVert", 	ref.cl[1].bdd, "param_06", 0, -1, 1. ):set_meter(false)

	iy = iy + SY
	self:add_slider_two(	{ix,iy,		8,SY},		"Dist", 	ref.cl[1].bdd, "param_01", "param_02", 0, 8., -10, 10. )

	iy = iy + SY
	SX = 8
	self:add_slider(	{ix,iy,			SX,SY},		"Calage", 	ref.cl[1].bdd, "param_09", 0, 0, 16. )

end

function meu:init()
	local ref = self.ref


	local function do_comment( ref )
		local bdd = ref.bdd
		ref.param = {}
		local p = ref.param
		for j=1,9 do
			p[j] = param.get_ref( bdd, "param_0"..j )
		end
		p[10] = param.get_ref( bdd, "param_10" )

		ref.ptra = {}
		ref.ptra[1] = param.get_ref( bdd, "vec_01_x" )
		ref.ptra[2] = param.get_ref( bdd, "vec_01_y" )
		ref.ptra[3] = param.get_ref( bdd, "vec_01_z" )

		ref.prot = {}
		ref.prot[1] = param.get_ref( bdd, "vec_02_x" )
		ref.prot[2] = param.get_ref( bdd, "vec_02_y" )
		ref.prot[3] = param.get_ref( bdd, "vec_02_z" )

		p = ref.param
		param.set_comment( p[1], "G camera z close" )
		param.set_comment( p[2], "G camera z far" )
		param.set_comment( p[3], "E 2D translation x" )
		param.set_comment( p[4], "E 2D translation y" )
		param.set_comment( p[5], "E 2D translation z" )
		param.set_comment( p[6], "F 2D rotation" )
		param.set_comment( p[7], "H height factor (0 for calage)" )
		param.set_comment( p[8], "I height min" )
		param.set_comment( p[9], "I cm epaisseur pour calage" )

		p = ref.ptra
		param.set_comment( p[1], "A 3D translation x" )
		param.set_comment( p[2], "A 3D translation y" )
		param.set_comment( p[3], "A 3D translation z" )

		p = ref.prot
		param.set_comment( p[1], "B 3D rotation x" )
		param.set_comment( p[2], "C 3D rotation y" )
		param.set_comment( p[3], "D 3D rotation z" )

		--if id == 2 then
		--	param.set( obj, "texture_1_bind", aaa.pc.is_maa() and 32 or 28 )
		--end
	end
	local function init_layer_col( id )
		local t = self:get_layer_ref_table(id+2)
		ref.cl[id] = t
		t.bdd = aaa.layer.get_bdd( t.obj )
		do_comment( t )
	end
	ref.cl = {}
	init_layer_col( 1 )
	init_layer_col( 2 )

end

function meu:build_one( id )
	local bu

	local function add_slider( par, name, min,max )
		return bus_cur:add_slider_target( name, nil, par,nil, nil, min,max )
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

function meu:build()
	local ref = self.ref

	local bu
	bus_cur:add_bup( {-4,nil, -2,2} ):set_size( 2, .2 )

	bu = bus_cur:add_text_here( "A" )
	bu:set_text_factor( 3. )
	bu:set_text_xy( 4.5 )
	self:build_one( 1 )

	bup:set_lbrt( { 2, nil, 4, 2 } )
	bu = bus_cur:add_text_here( "B" )
	bu:set_text_factor( 3. )
	bu:set_text_xy( 4.5 )
	self:build_one( 2 )

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
	bu:set_target_lua( self, "b_a" )
	bup:move_next()

	bu = add_but( "View" )
	bu:set_target_lua( self, "b_view" )
	bu = add_but( "Cam_Lock" )
	bu:set_target_param( ref.cam_lock )


	bu = add_but( "Helper" )
	bu:set_target_lua( self, "b_visu" )

	bup:move_next()
	bu = add_but( "B" )
	bu:set_target_lua( self, "b_b" )


	bup:set_lbrt( { -1.5, -2.2, 1.5, -2 } )
	bup:set_size( 1, .15 )
	bup:start()

	bu = add_but( "side" )
		bu:set_target_param( ref.cam_trig_x )
		bu:set_trig()
	bu = add_but( "top" )
		bu:set_trig()
		bu:set_target_param( ref.cam_trig_y )
	bu = add_but( "front" )
		bu:set_target_param( ref.cam_trig_z )
		bu:set_trig()
end
