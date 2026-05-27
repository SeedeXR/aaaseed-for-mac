function meu:define_meu_infos( )
	return { 	author = "Mâa",
				version = 0,
				tags = { "Proprietary", "art", "procedural", "2d","3d", "Draw", "texture", "Unfinished" },
				help = "Probably the nice particle effects made for clarisonic: restore it",
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()

	local SY = 1
	local ix,iy = 1,1
	local SX = 8/3

	local bdd		= ref.cl[1].bdd
		local sha	= ref.cl[1].shading

		self:add_slider(	{ix,iy,	SX,SY},		"CamX", 	bdd, "vec_01_x", 0, -10, 10. ):set_meter(false)
		self:add_slider(	{ix+SX,iy,	SX,SY},		"CamY", 	bdd, "vec_01_y", 0, -10, 10. ):set_meter(false)
		self:add_slider(	{ix+SX*2,iy,	SX,SY},		"CamZ", 	bdd, "vec_01_z", 0, -10, 10. ):set_meter(false)

		iy = iy + SY
		self:add_slider(	{ix,iy,	SX,SY},		"RotX", 	bdd, "vec_02_x", 0, -1, 1. ):set_meter(false)
		self:add_slider(	{ix+SX,iy,	SX,SY},		"RotY", 	bdd, "vec_02_y", 0, -1, 1. ):set_meter(false)
		self:add_slider(	{ix+SX*2,iy,	SX,SY},		"RotZ", 	bdd, "vec_02_z", 0, -1, 1. ):set_meter(false)

	iy = iy + SY
	self:add_slider(	{ix,iy,	SX,SY},		"TraX", 	bdd, "param_03", 0, -10, 10. ):set_meter(false)
	self:add_slider(	{ix+SX,iy,	SX,SY},		"TraY", 	bdd, "param_04", 0, -10, 10. ):set_meter(false)
	self:add_slider(	{ix+SX*2,iy,	SX,SY},		"TraZ", 	bdd, "param_05", 0, -10, 10. ):set_meter(false)

	iy = iy + SY
	self:add_slider(	{ix+SX,iy,	SX,SY},		"RotVert", 	bdd, "param_06", 0, -1, 1. ):set_meter(false)

	iy = iy + SY
	SX = 8
	self:add_slider(	{ix,iy,	SX,SY},		"Calage", 	bdd, "param_09", 0, 0, 16. )

	iy = iy + SY
	self:add_slider_two(	{ix,iy,	SX,SY},		"LimitX", 	bdd, "vec_03_x", "vec_04_x", -4, 4., -10, 10. )
	iy = iy + SY
	self:add_slider_two(	{ix,iy,	SX,SY},		"LimitY", 	bdd, "vec_03_y", "vec_04_y", -4, 4., -10, 10. )
	iy = iy + SY
	self:add_slider_two(	{ix,iy,	SX,SY},		"LimitZ", 	bdd, "vec_03_z", "vec_04_z", 0-4, 4, -10, 10. )


	iy = iy + SY
	iy = iy + SY
	self:add_slider(	{ix,iy,	SX,SY},		"Trail New", 	bdd, "param_12", 0, 0, 128. ):set_value_type_integer( true )
	iy = iy + SY
	self:add_slider(	{ix,iy,	SX,SY},		"Trail Speed", 	bdd, "param_13", 0, 0, 128. ):set_value_type_integer( true )
	iy = iy + SY
	self:add_slider(	{ix,iy,	3,SY},		"Speed_Thesrhold", 	bdd, "param_14", 0, 0, 1. )
	self:add_slider(	{ix+3,iy,	SX-3,SY},	"Z Speed", 			bdd, "param_15", 0, 0, 2. )
	iy = iy + SY
	self:add_slider(	{ix+3,iy,	SX-3,SY},	"XY Speed", 		bdd, "param_16", 0, 0, 2. )
	iy = iy + SY
	self:add_slider(	{ix,iy,	SX,SY},		"Gravity", 			bdd, "param_17", 0, 0, 2. )
	iy = iy + SY
	self:add_slider(	{ix,iy,	SX,SY},		"Floor", 			bdd, "param_18", 0, -2, 0. ):set_meter(false)

	ix, iy = 9, 2
	self:add_button(	{ix,			iy },					"Grid", 	self, "b_draw_grid", false )
	self:add_button(	{ix+4,		iy },					"Particle", self, "b_draw_part", true )
	iy = iy + SY
	self:add_trig(	{	ix,			iy },					"ReLoad", sha, "reload_all", false )
	iy = iy + SY
	bu = self:add_selector(	{ix,iy,	8,SY},		"density" )
		bu:set_nb( 5, 1 )
		bu:set_target_lua( self, "s_million" )
		bu:set_item_text( 1, "16K", "64K", "256K", "1M", "4M" )

	iy = iy + SY * 1.2
	bu = self:add_selector(	{ix,iy,	4,SY},		"Shape" )
		bu:set_nb_min_0( 2, 2 )
		bu:set_target_param( param.get_ref( sha, "gu_int_01" ) )
		bu:set_item_text( 1, "Face", "Cross", "Tri", "Cube" )
	self:add_slider(	{ix+4,iy,	4,SY},		"Size", 	self, "size_part", 1, 0, 1. )
	iy = iy + SY
	bu = self:add_selector(	{ix,iy,	4,SY},		"Map" )
		bu:set_nb_min_0( 2, 1 )
		bu:set_target_param( param.get_ref( sha, "fu_int_01" ) )
		bu:set_item_text( 1, "Square" )
--	self:add_slider(	{ix+4,iy,	4,SY},		"Hardness", sha, "fu_float_01", 1, .5, 5. )
	self:add_slider_two(	{ix+4,iy,	4,SY},		"Hardness", sha, "fu_float_01", "fu_float_02", 0, 1, 0, 1. )

	iy = iy + SY*1.2
	self:add_slider(	{ix,iy,	4,SY},		"Shaded", 	sha, "gu_01_r_x", 1, 0, 3 ):add_values_def( 1 )
	iy = iy + SY*1
	self:add_slider(	{ix,iy,	4,SY},		"Color", 	sha, "gu_01_g_y", 1, 0, 6 ):add_values_def( 1 )
	self:add_slider(	{ix+4,iy,	4,SY},		"Normal", 	sha, "gu_01_b_z", 1, -6, 6 ):add_values_def( 0, 1 )
	iy = iy + SY*1
	self:add_slider(	{ix,iy,	4,SY},		"Sound", 	sha, "gu_01_a_w", 1, 0, 2 ):add_values_def( 1 )
--	self:add_slider(	{ix+4,iy,	4,SY},		"UseZ", 	sha, "gu_01_a_w", 1, 0, 3 )
--[[
	bu = self:add_selector(	{ix+4,iy,	4,SY},		"LType" )
		bu:set_nb_min_0( 4, 1 )
		bu:set_target_param( param.get_ref( sha, "gu_int_02" ) )
		bu:set_item_text( 2, "Raw", "Arc", "Sound" )
--]]
end

function meu:init()
	local ref = self.ref

	local function do_comment( ref )
		local bdd = ref.bdd
		ref.texture_switch = param.get_ref( bdd, "texture_bind_switch" )

		ref.param = {}
		local p = ref.param
		for j=1,9 do
			p[j] = param.get_ref( bdd, "param_0"..j )
		end
		for j=0,8 do
			p[10+j] = param.get_ref( bdd, "param_1"..j )
		end

		ref.vec = {}
		for i=1,4 do
			local pre = "vec_0"..i
			ref.vec[i] = {
				param.get_ref( bdd, pre.."_x" ),
				param.get_ref( bdd, pre.."_y" ),
				param.get_ref( bdd, pre.."_z" ),
				param.get_ref( bdd, pre.."_w" )
			}
		end

		p = ref.param
--		param.set_comment( p[1], "G camera z close" )
--		param.set_comment( p[2], "G camera z far" )
		param.set_comment( p[3], "E 2D translation x" )
		param.set_comment( p[4], "E 2D translation y" )
		param.set_comment( p[5], "E 2D translation z" )
		param.set_comment( p[6], "F 2D rotation" )
		param.set_comment( p[7], "H height factor (0 for calage)" )
		param.set_comment( p[8], "I height min" )
		param.set_comment( p[9], "I cm epaisseur pour calage" )

		p = ref.vec[1]
		param.set_comment( p[1], "A 3D translation x" )
		param.set_comment( p[2], "A 3D translation y" )
		param.set_comment( p[3], "A 3D translation z" )

		p = ref.vec[2]
		param.set_comment( p[1], "B 3D rotation x" )
		param.set_comment( p[2], "C 3D rotation y" )
		param.set_comment( p[3], "D 3D rotation z" )

		p = ref.vec[3]
		param.set_comment( p[1], "G min x" )
		param.set_comment( p[2], "      y" )
		param.set_comment( p[3], "      z" )

		p = ref.vec[4]
		param.set_comment( p[1], "G max x" )
		param.set_comment( p[2], "      y" )
		param.set_comment( p[3], "      z" )
	end
	local function init_layer_col( id )
		local t = self:get_layer_ref_table(id+2)
		ref.cl[id] = t
		t.bdd 		= aaa.layer.get_bdd(		t.obj )
		t.shading	= aaa.layer.get_shading( 	t.obj )
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

function meu:update()
	local ref = self.ref
-- set parameter
	local cl = ref.cl[1]
	--self:print( self.s_million )
	local function set( nb )
		param.set( cl.param[10], nb )
		local nb = math.pow( 2 , nb+3 )
		param.set( cl.bdd, "point_nb_used", nb*nb )
		param.set( cl.bdd, "nb_u",  nb )
		local s = self.size_part
		param.set( cl.shading, "gu_float_01", 8000*s*s/nb )
	end
	
	if 		self.s_million == 5	then	set( 8 )
	elseif	self.s_million == 4	then	set( 7 )
	elseif	self.s_million == 3	then	set( 6 )
	elseif	self.s_million == 2	then	set( 5 )
	else 								set( 4 )
	end

	local ind = self.__frame_ind or 0
	ind = (ind + 1) % 64
	self.__frame_ind = ind
	--self:print( ind )
	param.set( cl.bdd, "param_11", ind )

	local bind = app:get_fbo_double_last()
	--self:print( "bind -> "..bind )

	--todo tex_named
	param.set( cl.texture_switch, bind == 65 )
	self:set_bind_2d( bind )

	param.set( cl.shading, "gu_float_03", -aaa.time.t * .05 )
end

function meu:draw()

	self:draw_layers_begin()

		if self.b_draw_grid then	self:draw_layer( 1 )	end
		if self.b_draw_part then	self:draw_layer( 2 )
									self:draw_layer( 3 )	end

	self:draw_layers_end()

	app.cam_screens = self:get_camera_used()
end