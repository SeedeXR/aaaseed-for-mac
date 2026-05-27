
function MEU:build_opencl_ref()
	--was the strategy
	--meu.before_build_refs_update	=	MEU.before_build_refs_update_opencl
	--meu.after_build_refs_update		=	MEU.after_build_refs_update_opencl
	--aaa.print_method()

	local ref = self.ref
	local name = "__MEU_BDD_OCL"
	local opencl_obj = self:get_obj_down( name )
	local opencl = OPENCL:create( name, opencl_obj )
	ref.ocl =  OPENCL:create( name, opencl_obj )

	--[[
	local b = opencl:is_param_active()
	opencl:set_param_active( false )
		MEU.build_ref_more( self )
	opencl:set_param_active( b )
	--]]
end
function MEU:init_opencl()
	self:build_opencl_ref()
end
function MEU:get_opencl()
	return self.ref.ocl
end
function MEU:get_opencl_layer()
	return self.ref.ocl.layer_bdd
end

--todo ocl gl separation or not and clean calls
function MEU:ogl_init( layer_shading, layer_bdd )
	return self:oclgl_init( true, layer_shading, layer_bdd )
end
function MEU:ocl_init( )	-- gl replace ocl
	return self:oclgl_init()
end
function MEU:oclgl_init( b_gl, layer_shading, layer_bdd )	-- gl replace ocl
	local bdd
	if layer_bdd then
		bdd = aaa.layer.get_bdd( layer_bdd )
	else
		bdd = self:get_obj_down_no_error( b_gl and "__MEU_GL_PART" or "__MEU_BDD_OCL" )
		if not bdd then
			bdd = self:get_obj_down_by_class( "bdd_opencl_part" )
		end
		layer_bdd = aaa.obj.get_up_by_class( bdd, "layer" )
	end
	local ocl
	if b_gl then
		ocl = {}
	else
		ocl = OPENCL:create( self:get_name(), bdd )	
	end
	--todoocl whould be nice to get the layer id so add_shading) will be more generic
	ocl.b_gl = b_gl
	ocl.bdd = bdd
	ocl.layer_bdd = layer_bdd
	ocl.layer_shading = layer_shading or layer_bdd
	--self:box_debug( "ocl.layer_shading "..ocl.layer_shading )
	self:add_shading( ocl.layer_shading, "ocl" )	--todoocl
	--ocl.__shading = self:create_shading_from_layer( "ocl", ocl.layer_shading  )
	self.ref.ocl = ocl
end

--	OPENCL
--
function MEU:add_opencl_param( rect, def_table, opencl, i )	-- def_table is { name, min, max, then named fields }
	local par = opencl:get_param_ref( i )

	local def_table_type = type(def_table)
	local name,min,max
	local values_def
	if def_table_type=="string" then
		name = def_table
	elseif def_table_type=="table" then
		name = def_table[1]
		min = def_table[2]
		max = def_table[3]
		values_def = def_table.def	
	else
		name = (i<10) and "param_0" or "param_"
		name = name..i
	end
	--self:print( name )
	local bu = self:add_param(	rect, name, par, min, max )
	if min and max and min < 0 and 0 < max then
		if -min == max then
			bu:set_meter_ref( 0 )
		else
			bu:set_meter( false )
		end
	end

	if values_def then
		bu:add_values_def( unpack(values_def) )
	end
	--strict
	--bu:set_meter( min==0 )
end

function MEU:add_opencl_params( rect, names, opencl, i_end )
	local iy,sy = rect[2],rect[4]
	for i=1,i_end do
		rect[2] = iy + (i-1) * sy
		self:add_opencl_param( rect, names[i], opencl, i )
	end
end

function MEU:add_opencl_base( ix,iy, SY, b_full, opencl, b_gl )
	local ref = self.ref
	ix = ix or 1
	iy = iy or 1
	SY = SY or 1

	local bu
	local nx = 3
	local sx = (8-nx)*.5

	if self:get_shading() then
		self:add_shading_ui()
	end
	if b_gl then
		-- local sha_ref = self:get_shading()
		-- if sha_ref then
		-- 	self:add_shading_ui( nil, sha_ref )
		-- end
		bu = self:add_trig_fn(	{	1+nx,		1,		sx,SY},		"Focus",	aaa.obj.set_focus_ui, ref.ocl.bdd )
		bu = self:add_trig_fn(	{	1+nx+sx,	1,		sx,SY},		"Restart",	param.set, ref.ocl.bdd, "restart_trig", 1 )
	else
		--2020 August a la Bigue: Maa extended opencl = opencl or ref.ocl
		--	to avoid trouble added during Lv
		if opencl then	ref.ocl = opencl
		else			opencl = ref.ocl
		end

		if false then
			self:add_trig(			{ix, 		iy-SY},	 			"Restart",			opencl.ref.restart_trig )	--}, nil, false			)
			self:add_trig(			{ix, 		iy}, 				"Dataset_Restart",	opencl.ref.dataset_restart_trig )
		else
			local SYB = 1
			self:add_trig_method(	{1,			1,		nx,SYB},	"Restart",	opencl,  "restart" )
			self:add_trig_fn(		{1+nx,		1,		sx,SYB},	"Focus",	aaa.obj.set_focus_ui, opencl:get_obj() )
			self:add_trig_method(	{1+nx+sx,	1,		sx,SYB},	"Ocl.Edit",	opencl,  "trig_edit" )
				:set_text( "Edit" ):set_color_back( "edit" )
			self:add_text_info(		{1,			1+SYB,	8,SYB*.6},	"Ocl Info"	)
			self:register_update_ui( "update_ui_ocl_base", bu )
		end
		if b_full then
			--self:print( "SY is "..SY )
			--todo we should do something to use less generic names
			bu = self:add_slider(	{ix+4,iy,			4,SY},		"color",	opencl.ref.color,	nil, 0, -4, 4 )
				bu:set_meter_ref( 0 )
				bu:add_values_def( -1, 0, 1 )
			bu = self:add_slider(	{ix+4,iy+SY,		4,SY},		"normal",	opencl.ref.normal,	nil, 0, -4, 4 )
				bu:set_meter_ref( 0 )
			bu = self:add_slider(	{ix+4,iy+SY*2,		4,SY},		"attrib",	opencl.ref.attrib,	nil, 0, -4, 4 )
				bu:set_meter_ref( 0 )
		end
		--if opencl.__shading then
		--	self:add_shading_ui( nil, opencl.__shading )
		--end
	end
end

function MEU:add_opencl( rect, names, opencl, i_end )
	local ix,iy, sx,sy = self:unpack_rect_using_def( rect, {9,3, 8,.8} )
	self:add_opencl_base( ix, iy, sy, true, opencl )
	self:add_opencl_params(	{ix,iy+3*sy, sx,sy},	names, opencl, i_end )
end

--todo i_start unclear
function MEU:add_opencl_loop( rect, opencl, i_start,  table )	-- ... is a table of table { name, min, max, then named fields }
	local ix,iy, sx,sy = self:unpack_rect_using_def( rect, {9,3, 8,.8} )
	if i_start==0 then
		self:add_opencl_base( ix, iy, sy, true, opencl )
		iy = iy + sy * 3
	else
		iy = iy + sy * i_start
	end

	for i = 1, #table do
		local tab = table[i]
		local name = tab[1]
		if name then
			self:add_opencl_param( {ix,iy, sx,sy}, tab, opencl, i+i_start )
		end
		iy = iy + sy
		if tab.y_skip then
			iy = iy + tab.y_skip
		end
	end
end

--	OpenCl OpenGl
--
function MEU:add_oclgl_slider(	rect,	name,	key, min, max )
	local bu = self:add_slider(	rect,	name )
	bu:set_target_lua( self, key )
	self[key] = 0.
	if min and max then
		bu:set_min_max( min, max )
		if min ~= 0 then
			bu:set_meter(false)
		end
	end
	return bu
end
--
function MEU:add_oclgl_button(	rect,	name,	key )
	local bu = self:add_button( rect,	name )
	bu:set_target_lua( self, key and key or "b_"..name )
	return bu
end

function MEU:add_oclgl_size( pre, ix,iy, sy )
	ix = ix or 1
	iy = iy or 5
	self:add_oclgl_button( 	{ix,	iy,	sy,sy},	pre.."su_inv"								):set_text_draw( false )
	self:add_oclgl_slider(	{ix+1,	iy,	3,sy},	pre.."su",		pre.."su",		0, 4 		)
	self:add_oclgl_slider(	{ix+4,	iy,	4,sy},	pre.."size",	pre.."size",	0, 4		):set_meter(false)
	iy = iy + sy
	self:add_oclgl_button( 	{ix,	iy,	sy,sy},	pre.."sv_inv"								):set_text_draw( false )
	self:add_oclgl_slider(	{ix+1,	iy,	3,sy},	pre.."sv",		pre.."sv",		0, 4 		)
	self:add_oclgl_slider(	{ix+4,	iy,	4,sy},	pre.."z",		pre.."z",		-16, 16.	):set_meter(false)
	return iy + sy
end

function MEU:add_oclgl_sizes( ix,iy, sy )
	ix = ix or 1
	iy = iy or 5
	iy = self:add_oclgl_size( "begin_", ix,iy, 		sy ) + .1
	self:add_button(			{ix+1,iy, sy,sy},	"sync_end_suv", self, "b_sync_suv" )
	iy = self:add_oclgl_size( "end_", 	ix,iy+1, 	sy ) + .1
	return iy
end

function MEU:add_oclgl_fade( ix, iy )
	local ocl = self:get_opencl()
	local bu = self:add_slider_two(	{ix,iy,	8,1},	"Fade" )
		bu:set_target_param( ocl:get_param_ref(2), 1 )
		param.set_comment( ocl:get_param_ref(2), "Fade In" )
		bu:set_target_param( ocl:get_param_ref(3), 2 )
		param.set_comment( ocl:get_param_ref(3), "Fade Out" )
		bu:set_min_max( 0,1, 1 )
		bu:set_min_max( 0,1, 2 )
end

function MEU:add_oclgl_slider_ocl_param(			rect,	name,	param_ref, value, min,max )
	local bu = self:add_slider(	rect, name )
	if min or max or value then
		bu:set_min_max_value( min,max,value )
	end
	param.set_comment( param_ref, name )
	return bu
end
function MEU:add_oclgl_slider_ocl_param_target(		rect,	name,	param_ref, value, min,max )
	local bu = self:add_oclgl_slider_ocl_param(		rect,	name,	param_ref, value, min,max )
	bu:set_target_param( param_ref, 1, name )
	return bu
end

function MEU:update_oclgl_galaxy()
	local ref = self.ref
	local ocl = self:get_opencl()
	local sha = self:get_shading()
	local ui = self.ui

	local alpha = self:get_alpha()
	for i= 1,1 do
		local r,g,b, a = ui.color[i]:get_rgba()
		--sha:set_frag_vec4( i, 1,.5,1,1 * alpha )
		sha:set_frag_vec4( i, r,g,b, a * alpha )
	end

	if not ocl.b_gl then
		--self:print( self.b_sync_suv )
		if self.b_sync_suv==1 then
			self:set_bu_value( "end_su", self.begin_su )
			self:set_bu_value( "end_sv", self.begin_sv )
		end

		local f,fu,fv
		f = self.begin_size
		f = f<0 and -f*f or f*f
		fu = self.b_begin_su_inv==1 and -f or f
		fv = self.b_begin_sv_inv==1 and -f or f
		ocl:set_vec_xyz( 1, self.begin_su * fu, self.begin_sv * fv, self.begin_z )

		f = self.end_size
		f = f<0 and -f*f or f*f
		fu = self.b_end_su_inv==1 and -f or f
		fv = self.b_end_sv_inv==1 and -f or f
		ocl:set_vec_xyz( 2, self.end_su * fu, self.end_sv * fv, self.end_z )

		ocl:set_vec_xyz( 3, self.position_x, self.position_y, self.position_z )

		local sa = math.abs( self.end_z - self.begin_z )
		sa = sa~=0. and (1./sa) or 1
		param.set( ocl:get_param_ref(1), self.speed_z * sa )
		param.set( ocl:get_param_ref(5), self.speed_x * sa )
		param.set( ocl:get_param_ref(6), self.speed_y * sa )

	---[[
		local inf = self.petal
		if self.petal_freq==0 then
			inf = 0
		end
		param.set( sha:get_ref_frag_float( 2 ), inf*.5 )
		param.set( sha:get_ref_frag_float( 3 ), self.petal_freq)
	--]]
	end
end

function MEU:update_ui_ocl_base( bu_info )
	local ocl =  self:get_opencl()
	if ocl then
		if bu_info then
			local b = ocl:is_ready_to_run()
			bu_info:set_text_color_problem_white( b )
			bu_info:set_text( ocl:get_info() )
		end
	else
		self:print_error( "No ocl here pb of ocl update to solve, change done in lv prepa" )
	end
end

function MEU:add_oclgl_xyz( rect, name )
	--table.print( rect )
	local ix = (rect and rect[1]) or 1
	local iy = (rect and rect[2]) or 1
	local sx = (rect and rect[3]) or 8
	local sy = (rect and rect[4]) or 1
	sx = sx / 3
	name = name.."_"
	local name_l = string.lower( name )
	self:add_oclgl_slider(	{ix,iy,			sx,sy},	name.."x", name_l.."x",	-1, 1 ):set_color_back("x")
	self:add_oclgl_slider(	{ix+sx,iy,		sx,sy},	name.."y", name_l.."y",	-1, 1 ):set_color_back("y")
	self:add_oclgl_slider(	{ix+sx*2,iy,	sx,sy},	name.."z", name_l.."z",	-1, 1 ):set_color_back("z")
end

function MEU:add_oclgl_bu_textures( rect, name, nb, tab_name )
	local ix = (rect and rect[1]) or 1
	local iy = (rect and rect[2]) or 4
	local sx = (rect and rect[3]) or 4
	local sy = (rect and rect[4]) or 4
	local x,y = ix,iy

	if tab_name then self:set_tab_key( tab_name ) end
		for i=1,nb do
			local str = "Tex"..string.char(i+64)
			local rect
			if sx > 4 then
				rect = {x,y+i*sy, sx,sy, b_compact = true}
			else
				local c = (i-1)%2
				local l = (i-c-1)/2
				rect = {x+c*4,y+l*sy, sx,sy}
			end
			self:add_bu_texture( rect, str, i )
			--if y > 13 then x,y = ix+8,iy end
		end	
	if tab_name then self:set_tab_key_def() end
end

--GALAXY
function MEU:define_ui_oclgl_galaxy()
	local ref = self.ref
	local ui = self.ui

	local ocl = self:get_opencl()
	local sha = self:get_shading()

	local bu, ix,iy
	self:add_camera()

	self:add_opencl_base( nil, nil, nil, nil, nil, ocl.b_gl )


	if ocl.b_gl then
--		self:add_tex_bind_layer(	{1,4}, 		"TexA", 1,	false,	ocl.layer	)
	else
		ui.color = self:add_color_1( {1,3, 8,1} )

		self:set_tab_key_def()
		ix, iy = 1, 7
		self:add_oclgl_xyz(		{ix,iy}, "Position" )
		iy = iy + 1

		bu = self:add_button(	{ix+4,iy },	"Visu" )
		iy = iy + 1

		self:add_oclgl_bu_textures( {1,4, 4,3}, nil, 1 )

		iy = self:add_oclgl_sizes( ix,iy, 1 )

		self:add_oclgl_fade( ix,iy )

		ix, iy = 9, 4
		self:add_oclgl_xyz( {ix,iy}, "Speed" )

	--	self:add_slider(	{ix,iy+1,	8,1},	"Cone Size",	ocl:get_param_ref(2), nil, 1, 0, 3 )
	--		param.set_comment( ocl:get_param_ref(2), "Cone Size" )
	--	self:add_oclgl_slider_ocl_param( 	ix, iy+4, 	8, 1.5,	"Cling", ocl:get_param_ref(4), 0, 4 )

	---[[
		ix, iy = 9, 10
		bu = self:add_slider_two(	{ix,iy,	8,1},	"Part_Size" )
			bu:set_target_param( sha:get_ref_vert_float(2), 1 )
			bu:set_target_param( sha:get_ref_vert_float(3), 2 )
			bu:set_min_max( 0, 8, 1 )
			bu:set_min_max( 0, 8, 2 )

		self:add_slider(		{ix,iy+1,	8,1},	"Part_Factor",	sha:get_ref_vert_float(1), nil,	1,	0,4 )
		self:add_slider(		{ix,iy+2,	8,1},	"Hardness",		sha:get_ref_frag_float(1), nil,	1,	0,4 )
		self:add_oclgl_slider(	{ix,iy+3,	4,1},	"Petal",		"petal", 		-1, 1 )
		self:add_oclgl_slider(	{ix+4,iy+3,	4,1},	"Petal_freq",	"petal_freq",	0, 16 )

		--self:add_slider(		{ix,iy+2,	8,1},	"Fade_in",		ocl, "param_03", .1, 0, 1 )
		--self:add_slider(		{ix,iy+3,	8,1},	"Fade_Out",		ocl, "param_04", .9, 0, 1 )
	--]]
	end
end

function MEU:draw_oclgl_galaxy_visu()
	local ox,oy,oz = self.position_x,self.position_y,self.position_z
	local function draw_rect_crossed( sx,sy, x,y,z )
		gol.translate( x,y,z )
		aaa.draw_rect_line( -sx,-sy, sx,sy )
		local s = .1
		--aaa.draw_rect_line( -sx*s, -sy*s, sx*s,sy*s, z )
		aaa.draw_plus_line( 0,0, sx*s,sy*s )
		gol.draw_lines_2d(	sx,sy,	-sx,-sy,	-sx,sy,		sx,-sy	)
		gol.translate( -x,-y,-z )
	end

	gol.set_default()
	gol.color_blue()
	gol.set_line_width( 4 )
	local sxb,syb,szb
	local sb = self.begin_size * self.begin_size * .5
	sxb = sb * self.begin_su
	if self.b_begin_su_inv==1 then sxb = -sxb end
	syb = sb * self.begin_sv
	if self.b_begin_sv_inv==1 then syb = -syb end
	szb = self.begin_z
	draw_rect_crossed( sxb, syb, ox,oy,oz+szb )

	gol.color_cyan()
	local sxe,sye,sze
	local se = self.end_size * self.end_size * .5
	sxe = se * self.end_su
	if self.b_end_su_inv==1 then sxe = -sxe end
	sye = se * self.end_sv
	if self.b_end_sv_inv==1 then sye = -sye end
	sze = self.end_z
	draw_rect_crossed( sxe, sye, ox,oy,oz+sze )

	local function draw_line( xb,yb,zb, xe,ye,ze )
		gol.color_blue()	gol.vertex3( xb, yb, zb )
		gol.color_cyan()	gol.vertex3( xe, ye, ze )
	end
	gol.translate( ox,oy,oz )
	gol.begin_lines()
		draw_line(  sxb,  syb, szb , 	 sxe,  sye, sze )
		draw_line( -sxb,  syb, szb , 	-sxe,  sye, sze )
		draw_line( -sxb, -syb, szb , 	-sxe, -sye, sze )
		draw_line(  sxb, -syb, szb , 	 sxe, -sye, sze )
	gol.do_end()
	gol.translate( -ox,-oy,-oz )
end

if true then
	function MEU:get_opencl_bind_2d( index )		return	self.ref.ocl:get_tex_src( index or 1 )	end
	function MEU:set_opencl_bind_2d( index, bind )	self.ref.ocl:set_tex_src( index, bind )			end
else
	function MEU:get_opencl_bind_2d( index )			end
	function MEU:set_opencl_bind_2d( index, bind )		end
end

function MEU:update_particle_nb()
	local ui = self.ui

	local nb = ui.bu_nb:get_item_data()	--max nb of particle we use
	nb = nb * self.particle_nb_factor
	local nb_used = nb * self.particle_nb_used
	self:set_point_nb( nb, nb_used )
end

function MEU:update_ui_particle_nb( bu_info_nb, bu_info_nb_used, bu_info_free )
	local ref = self.ref

	bu_info_nb		:set_text( aaa.format.int_to_char_with_space( param.get(ref.point_nb) ) )
	bu_info_nb_used	:set_text( aaa.format.int_to_char_with_space( param.get(ref.point_nb_used) ) )
	if bu_info_free then
		local pct = param.get(ref.free_nb) / param.get(ref.point_nb)  * 100.0
		pct = math.floor(pct)
		bu_info_free:set_text(  pct .. "% "..aaa.format.int_to_char_with_space( param.get(ref.free_nb) ) )
	end
	--ref.point_nb 	= param.get_ref( ref.bdd	, "particle_nb_max" )
	--ref.free_nb
	--local free = 1.0
	--free = ref.free_nb / ref.point_nb
end

function MEU:add_particle_nb_ui( rect, bdd, pre, b_free_read )
	local ui = self.ui
	
	local ix,iy, sx,sy = unpack(rect)
	local SX = sx/8
	local SY = sy/2
	local SYB = SY* (b_free_read and 2/3 or 1) 
	local SX_A = SX*3
	local SX1 = SX * 1.5
	local X_A = ix + SX1
	local SX_INFO = SX*3.5
	local X_INFO = ix + SX*8 - SX_INFO

	local bu

	local function make_name( pre, str )
		return pre and pre.." "..str or str
	end
	local str = "Nb Max"
	self:add_trig_fn(	{ix,iy,		SX1,SYB},	"Focus",	aaa.obj.set_focus_ui, bdd )
	bu = self:add_button( {ix,iy+SYB,	SX1,SYB}, make_name(pre,str) )

 		bu:set_menu{ 	"512",	"1K",	"2K",	"4K",
						"8K",	"16K",	"32K",	"64K",
						"128K",	"256K", "512K",	"1M",
						"2M",	"4M",	"8M",	"16M"
						}:set_nb_min_0( 4, 4 )
		bu:set_text_selector(true)
 		bu:set_item_data( 1,	512, 1024, 1024*2, 1024*4,
		 						1024*8, 1024*16, 1024*32, 1024*64,
								1024*128, 1024*256, 1024*512, 1024*1024,
								1024*1024*2,  1024*1024*4,  1024*1024*8,  1024*1024*16
		 				)
		bu:set_color_back( "restart" ) 
		bu:set_method_on_click_double( self, "restart" ) 
		ui.bu_nb = bu
	bu = self:add_trig(	{ix,iy+SYB*2,	 SX1,SYB}, "Restart" )
--		:set_function_on_click( function() param.set( bdd, "restart_trig", true ) end )
		:set_function_on_click( param.set, bdd, "restart_trig", true )

	bu = self:add_text_info(	{X_INFO,iy,	SX_INFO,SYB},	"Nb" )
		bu:set_function_on_click( aaa.obj.set_focus_ui, bdd	 )
		--bu:set_text_color( "focus" )
		bu:set_text_color( "magenta" )

	local bu_info_nb = bu		
	str = "Nb Factor"
	bu = self:add_slider(	{X_A,iy,	SX_A,SYB},	make_name(pre,str),	self, "particle_nb_factor", 1, 0, 1 )
		bu:set_text( "Factor" )
		bu:add_values_def( .1,.25,.5,.75 )
		bu:set_color_back( "restart" ) 
	--todo specialize focus/restart on bdd, and both
	-- bu = self:add_trig(	{ix,iy,			2, SY},	"Focus"	)
	-- 	bu:set_function_on_click( aaa.obj.set_focus_ui, ref.bdd	 )
	str = "Nb Used"
	bu = self:add_slider(	{X_A,iy+SYB,	SX_A,SYB},	make_name(pre,str),	self, "particle_nb_used", 1, 0, 1 )
		bu:set_text( "Used" )
		bu:add_values_def( .1,.2,.25,.3,.4,.5,.6,.7,.75,.8,.9 )
		--bu:set_color_back( "restart" ) 
	bu = self:add_text_info(	{X_INFO,iy+SYB,	SX_INFO,SYB},	"Nb Used Info" )
	local bu_info_nb_used = bu		

	local bu_info_free
	if b_free_read then
		str = "Free Read"
	--	bu = self:add_trig(	{	ix,iy+SY,		2, SY},  "Restart",	ref.bdd, "restart_trig" )
		bu = self:add_button( {	X_A,iy+SYB*2,	SX_A,SYB },	make_name(pre,str),	bdd, "free_indices_counter_read", 			false	)--:set_text( "Free" )
			self.ui.bu_free_read = bu
		bu= self:add_text_info(	{X_INFO,iy+SYB*2,	SX_INFO,SYB},	"Free Info" )
		bu_info_free = bu
	end

	self:register_update_ui( "update_ui_particle_nb", bu_info_nb, bu_info_nb_used, bu_info_free )
end

--todo move to LV ?
--todo generalize caller also ?
function MEU:update_ocl_dpool()
	local ref = self.ref
	local ocl = self:get_opencl()
	local sha = self:get_shading()
	local ui = self.ui

	local alpha = self:get_alpha()
	for i= 1, 1 do
		local r,g,b,a = ui.color[i]:get_rgba()
		--ui.color[i]:print( i.." "..r.." "..g.." "..b.." "..a)
		--sha:set_frag_vec4( i, 1,.5,1,1 * alpha )
		sha:set_frag_vec4( i, r,g,b, a * alpha )
	end

	if true then
---		local sa = math.abs( self.end_z - self.begin_z )
--		sa = sa~=0. and (1./sa) or 1
		param.set( ocl:get_param_ref(1), self.speed_z ) --they were multiply by sa
		param.set( ocl:get_param_ref(5), self.speed_x )
		param.set( ocl:get_param_ref(6), self.speed_y )
	end
--[[
	local inf = self.petal
	if self.petal_freq==0 then
		inf = 0
	end
	param.set( sha:get_ref_frag_float( 2 ), inf*.5 )
	param.set( sha:get_ref_frag_float( 3 ), self.petal_freq)
--]]
end

