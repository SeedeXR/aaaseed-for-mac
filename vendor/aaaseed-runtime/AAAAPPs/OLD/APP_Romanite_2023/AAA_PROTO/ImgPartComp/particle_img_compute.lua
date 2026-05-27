function meu:define_physic( ix,iy, SY,SYT )
	SY = .8
	local DY = .2
	local phy = { target_offset={x=0,y=0,z=0} }
	self.phy = phy
	self:add_selector(			{ix,iy,		4,SY},	"Magnet use" )
		:set_item_text( 1, "Direct", "Phy", "Magnet" )
		:set_target_lua( phy, "s_magnet" ) 
	self:add_slider(			{ix+4,iy,	4,SY},	"Magnet",		phy,	"magnet",		.1,	0,1		)
	iy = iy + SY + DY

	iy = iy + DY
	self:add_selector(			{ix,iy,		4,SY},	"Physic" )
		:set_nb( 4 )
		:set_target_lua( phy, "s_phy" ) 
	self:add_slider(			{ix+4,iy,	4,SY},	"Viscosity",	phy,	"viscosity",	.3,	0,1 )
	iy = iy + SY
	self:add_slider(			{ix,iy,		4,SY},	"Accel Factor",	phy,	"accel_factor",	1,	0,4 )
	self:add_slider(			{ix+4,iy,	4,SY},	"Accel Max",	phy,	"accel_max",	10,	0,10 )
	iy = iy + SY
	self:add_slider(			{ix,iy,		4,SY},	"Accel Jitter",	phy,	"accel_Jitter",	1,	0,10 )
	iy = iy + SY + DY

	iy = iy + DY
	local fy = 1.
	self:add_selector(			{ix,iy,		8,SY*fy},	"How" )
		:set_nb( 6, 1 )
		:set_item_text( 2, "In", "Out", "Recit", "5", "6" )
		:set_target_lua( phy, "s_how" ) 
	iy = iy + SY*fy
	self:add_sliders_xyz(		{ix,iy,		8,SY},	"Target",		phy.target_offset, false,	10	)
	iy = iy + SY + DY

	local sy_pushed = SY
	SY = SYT / 5
	self:begin_bu_group( "field" )
	self:add_bu_texture(	{ix,iy,		4,SYT}, "Field",	3, false )
	SY = SYT/5
	self:add_button(			{ix+4,iy,	SY,SY},	"Active",		phy,	"b_field_active",	true )
		:set_text( "Active" )
	-- self:add_button(			{ix+6.5,iy,	1.5,SY},"Field Active",	phy,	"b_field_see",		false )
	-- 	:set_text( "Active" )
	SY = SYT*4/5/3
	self:add_slider(			{ix+4,nil,	4,SY},	"Influence",	phy,	"field_inf",		0,	-8,8 )
	self:add_slider(			{ix+4,nil,	4,SY},	"dist",			phy,	"field_dist",		0,	-1,1 )
	self:add_slider(			{ix+4,nil,	4,SY},	"see",			phy,	"field_see",		0,	0,256 )
	self:add_slider(			{ix+4,nil,	4,SY},	"type", 		phy, 	"s_touch_type",		0,	0,32 )
		:set_value_type_integer(true)
		:set_min_max_strict(true)
	self:end_bu_group()
	iy = iy + SYT + DY
	SY = sy_pushed

	return ix,iy
end
function meu:cpy_physic_from( src, b_field )
	local sp = src.phy
	local dp = self.phy

	dp.s_magnet			= b_field and sp.s_magnet or 1
	dp.magnet			= sp.magnet
	dp.s_how			= sp.s_how
	dp.s_phy			= sp.s_phy
	dp.accel_factor		= sp.accel_factor
	dp.accel_max		= sp.accel_max
	dp.accel_Jitter		= sp.accel_Jitter
	dp.target_offset.x	= sp.target_offset.x
	dp.target_offset.y	= sp.target_offset.y
	dp.target_offset.z	= sp.target_offset.z
	dp.b_field_active	= sp.b_field_active and b_field
	dp.field_inf		= sp.field_inf
	dp.field_dist		= sp.field_dist
	dp.field_see		= sp.field_see

	self:set_texture_bind_2d( 3, src:get_texture_bind_2d(3) )
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = .9
	local SYT = 3.8
	local SX = 1
	local DY = .2

	local SX_INFO = SX*3
	local X_INFO = ix + SX*8 - SX_INFO

	self:add_camera()
	self:add_rendering(		{nil,nil,		8,2*SY })

	local sha = self:get_shading()
	
	local SXS = (8 - SX_INFO) / 2
	bu = self:add_slider(	{ix,iy, 		SXS,SY},	"Nb U", self, "nb_u", 	128, 1, 4069	)
		:set_value_type_integer(true)
		:set_color_back("u")
	bu = self:add_slider(	{ix+SXS,iy, 	SXS,SY},	"Nb V", self, "nb_v", 	128, 1, 4069	)
		:set_value_type_integer(true)
		:set_color_back("v")

	bu = self:add_trig_method(	{ix,iy+SY,	 	2, SY},	"Restart",	self, "restart" )
	bu = self:add_trig_method(	{ix+2,iy+SY,	2, SY},	"Reset",	self, "reset_particle" )
--		:set_function_on_click( function() param.set( bdd, "restart_trig", true ) end )
--		:set_function_on_click( param.set, ref.bdd, "restart_trig", true )
	bu = self:add_text_info(	{X_INFO,iy,	SX_INFO,SY},	"Nb" )
		bu:set_method_on_click( self, "restart" )
		bu:set_text_color( "focus")
		ui.bu_info_nb = bu
	bu = self:add_text_info(	{X_INFO,iy+SY,	SX_INFO,SY},	"Nb Used Info" )
		ui.bu_info_nb_used = bu
	iy = iy + SY*2 + DY

	self:add_shading_ui(	{ix,iy,		8,SY} )
	self:add_blending(		{ix+6,iy,	2,SY} )
	self:add_button(		{ix+2,iy,	2,SY},	"Recit",		self, "b_recit", 		false  )
	self:add_slider(		{ix+4,iy, 	2,SY},	"Recit Appear",	self, "recit_appear", 	0, 0,1	)
	iy = iy + SY + DY

	self:begin_bu_group( "Decomposition" )
	bu = self:add_button(	{ix,iy, 	2,SY},	"Auto",			self, "b_decomposition_auto", 	false	)
	bu = self:add_slider(	{ix+2,iy, 	4,SY},	"Interpolate",	self, "decomposition_inter", 	0, -1,2	)
	bu = self:add_slider(	{ix+6,iy, 	2,SY},	"Range", 		self, "decomposition_range", 	0, 0,1	)
	self:end_bu_group()
	iy = iy + SY
	self:begin_bu_group( "Target" )
	bu = self:add_button(	{ix,iy, 	2,SY},	"Auto",			self, "b_target_auto", 	false	)
	bu = self:add_slider(	{ix+2,iy, 	4,SY},	"Interpolate",	self, "target_inter", 	0, -1,2	)
	bu = self:add_slider(	{ix+6,iy, 	2,SY},	"Range", 		self, "target_range", 	0, 0,1	)
	self:end_bu_group()
	iy = iy + SY + DY

	self:add_sliders_xyz(	{ix,iy,		8,SY},	"Pos",			self.vol_center, 		false,	5	)
	iy = iy + SY
	self:add_sliders_xyz(	{ix,iy,		8,SY},	"Size",			self.vol_size, 			false,	10	)
	iy = iy + SY
	self:add_slider(		{ix,iy, 	3,SY},	"Z for Rot",	self, "z_for_rot", 		0, -1,1	)
		:set_color_back( "z" )
	self:add_slider(		{ix+3,iy, 	3,SY},	"Rot_Y",		self, "rot_y", 			0, -1,1	)
		:set_color_back( "y" )
	iy = iy + SY + DY

	self:add_button(			{ix,iy, 	SY,SY},		"Displace active",	self, "b_displace", 	true	)
		:set_text_visible(false)
	self:add_slider(			{ix+SY,iy,	4,SY},		"Displace",			self, "displace",		0, 0,4 )
	self:add_slider(			{ix+4+SY,iy,4-SY,SY},	"Displace Gamma",	self, "displace_gamma", .75, 0,4 )
		:set_text( "Gamma" )
		:set_draw_gamma(true)
	iy = iy + SY

	self:add_bu_texture(	{ix,iy,		4,SYT}, "Texture",	1, false )
	self:add_bu_texture(	{ix+4,iy,	4,SYT}, "Depth",	2, false )
	-- and 3 is field
	iy = iy + SYT

		-- local SX = 8/3
		-- self:add_slider_two({ix,iy,		8,SY},	"Speed_range",	self, "speed_min", "speed_max", 0, 1, 0, 1 )
		-- iy = iy + SY
		-- self:add_slider(	{ix,iy,		8,SY},	"Speed", 		self, "speed",			0,	0,1 )
		-- iy = iy + SY

		-- local function add_slider( index, compo )
		-- 	bu = self:add_slider_two(	{ix+SX*index,iy,	SX,SY},	"Speed_"..compo,	self, "sp_min_"..compo, "sp_max_"..compo, 0, 0, -1, 1 )
		-- 	bu:set_color_back(compo)
		-- 	bu:add_values_def( -.5, 0, .5 )
		-- end
		-- add_slider(	0,	"x" )
		-- add_slider(	1,	"y" )
		-- add_slider(	2,	"z" )

		ix,iy = self:define_physic( 9,2.5, 1,3.2 )


	-- local col = {}
	-- col[1] = self:add_rgba(		{ix,iy,		8,1},	"color_1",	false )
	-- col[2] = self:add_rgba(		{ix,iy+1,	8,1},	"color_2",	false )
	-- ui.color = col

--	self:add_slider(	{ix,iy,		4,SY},	"Part Size", 		self, "part_size",			1,	-1,1	)
--	self:add_slider(	{ix+4,iy,	4,SY},	"Part Size Factor", self, "part_size_factor", 	0,	-1,1	)
--	self:add_slider(	{ix+6,iy,	2,SY},	"Part Size Gamma", 	self, "part_size_gamma",				1, .001, 8 )
--		:set_text( "Gamma" )
	iy = iy + SY
--	self:add_slider(	{ix+4,iy,	4,SY},	"Part Size U",		self, "part_size_u_factor",	1,	0,5		)
	iy = 13
	self:add_slider(	{ix,iy,		4,SY},	"Hardness",	self,	"hardness",					1,	0,1		)
	self:add_slider(	{ix+4,iy,	4,SY},	"Round",	self,	"round",					1,	0,1		)
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},	"Noise",	self,	"shape_noise",				1,	0,1		)
	self:add_slider(	{ix+4,iy,	4,SY},	"Freq",		self,	"shape_noise_freq",			1,	0,10	)

end

function meu:restart()
	local ref = self.ref
	param.set( ref.restart_trig, true )
end

function meu:reset_particle()
	self.b_reset_particle = true
end

function meu:init()
	local ref = self.ref

	ref.layer_attr		= self:get_layer(1)
	ref.layer_shading	= self:get_layer(2)
	local sha = self:add_shading( 2 )
	ref.layer_bdd		= self:get_layer(3)
	ref.bdd				= self:get_layer_bdd(3)

--	ref.render 				= aaa.obj.get_by_class_no_error( ref.layer_attr, "rendering" )
--	ref.prim				= param.get_ref( ref.render,	"primitive"			)
	ref.point_nb 			= param.get_ref( ref.bdd,		"point_nb"			)
	ref.point_nb_used		= param.get_ref( ref.bdd,		"point_nb_used"		)
	ref.point_instance_nb	= param.get_ref( ref.bdd,		"point_instance_nb"	)
	ref.prim_str			= param.get_ref( ref.bdd,		"primitive_used"	)
	ref.restart_trig		= param.get_ref( ref.bdd,		"restart_trig"		)

	--todo understand why we get a memory leak when true
	param.set( ref.bdd, "point_nb_alloc_when_changed", false )

	self.vol_center		= { x=0, y=0, z=0 }
	self.vol_size		= { x=0, y=0, z=0 }

	sha:set_save_vert_vec4(		false,	1,2	)
	sha:set_save_vert_float(	false,	1,5 )
	sha:set_save_vert_int(		false,	2,4 )

	sha:set_save_comp_vec4(		false,	1,3		)
	sha:set_save_comp_float(	false,	1,17	)
	sha:set_save_comp_int(		false,	2,4		)

	sha:set_save_frag_vec4(		false,	1,2	)
	sha:set_save_frag_float(	false,	1,5	)
	sha:set_save_frag_int(		false,	2,2	)
end

function meu:update_ui()
	local ui = self.ui
	local ref = self.ref

	ui.bu_info_nb		:set_text( aaa.format.int_to_char_with_space( param.get(ref.point_nb) ) )
	ui.bu_info_nb_used	:set_text( aaa.format.int_to_char_with_space( param.get(ref.point_nb_used) ) )

	--ref.point_nb 	= param.get_ref( ref.bdd	, "particle_nb_max" )
	--ref.free_nb
	--local free = 1.0
	--free = ref.free_nb / ref.point_nb
end

function meu:set_point_nb( nb, nb_used )
	local ref = self.ref
	param.set( ref.point_nb,		nb		)
	param.set( ref.point_nb_used,	nb_used	)
end

function gol.prim_str_to_id( str )
	local t = {
		POINTS			= 1,
		LINES			= 2,
		LINE_LOOP		= 3,
		LINE_STRIP		= 4,
		TRIANGLES		= 5,
		TRIANGLE_STRIP	= 6,
		TRIANGLE_FAN	= 7,
		QUADS			= 8,
		QUAD_STRIP		= 9,
		POLYGON			= 10
	}
	return t[str]
end

function meu:prepare_for_rendering( sha, b_displace )
	local ref = self.ref
	local ui = self.ui

	--inc time
	local time = (self.time or 0) + aaa.time.dt
	self.time = time

--	self:update_particle_nb()
	local nb = self.nb_u * self.nb_v

	local nb_allocated = param.get( ref.point_nb )
	if nb > nb_allocated then
		self:print( "realloc particle asked from "..nb_allocated.." to "..nb.." asking "..(nb*1.2) )
		param.set( ref.point_nb, nb*1.2 )
--		self:restart()
	end
	param.set( ref.point_nb_used, nb )

	-- for i=1,2 do
	-- 	local r,g,b, a = ui.color[i]:get_rgba()
	-- 	sha:set_frag_vec4( i, r,g,b, a )
	-- end
	
-- size and rotation	
	local c = self.vol_center
	local s = self.vol_size
	local rot_y = self.rot_y * math.pi2

	sha:set_comp_vec4( 1, c.x, c.y, c.z, self.z_for_rot )
	sha:set_comp_vec4( 2, s.x, s.y, s.z, rot_y )

	sha:set_vert_vec4( 1, s.x, s.y, s.z, rot_y )
	sha:set_vert_vec4( 2, 1/s.x, 1/s.y )

-- du, dv and displace
	local du,dv = 1/self.nb_u, 1/self.nb_v
	local f_displace = b_displace and self.b_displace and self.displace or 0

	sha:set_comp_float_1_4( du,dv, f_displace, self.displace_gamma )
	sha:set_vert_float_1_4( du,dv, f_displace, self.displace_gamma )
	
	local phy = self.phy
	local magnet = select( phy.s_magnet, 1000000, 0, phy.magnet )

	local tg = phy.target_offset
	sha:set_comp_vec4( 3, tg.x, tg.y, tg.z )

	local test = (self.b_reset_particle and 256*256 or 0) + ( 256 * phy.s_touch_type ) 
	self.b_reset_particle = nil 
	sha:set_vert_int_3_4( self.nb_u, self.nb_v )
	sha:set_comp_int_1_4( test, 	phy.s_phy*256 + phy.s_how,	self.nb_u, self.nb_v )

	local v = self.b_recit and 1 or 0
	--aaa.show( v, "b_recit")
	sha:set_frag_int_2( v )
	if self.b_recit then
		sha:set_frag_float_5( 1./math.sin(self.recit_appear*math.pi*.5) )
	end

	local decomposition_inter 
	if self.b_decomposition_auto then
		decomposition_inter = -1.2 + 2.5 * triangle_01(self.time*.20)
	else
		decomposition_inter = self.decomposition_inter
	end
	local target_inter 
	if self.b_tartget_auto then
		target_inter = -1.2 + 2.5 * triangle_01(self.time*.20)
	else
		target_inter = self.target_inter
	end

	local dt = aaa.time.dt
	sha:set_comp_float_5_9(	dt, self.time, decomposition_inter, self.decomposition_range, target_inter )
	sha:set_comp_float_10_13(	magnet, math.max(  0, 1 - phy.viscosity * dt * 60 ), phy.accel_max, phy.accel_factor*100. )
	sha:set_comp_float_14_17(	phy.accel_Jitter,
								phy.b_field_active and phy.field_inf or 0, phy.field_dist, phy.field_see )



	sha:set_frag_float_1_4( self.hardness,	self.round,			
							self.shape_noise, self.shape_noise_freq
						)	

	local bind = self:get_texture_bind_2d(1)
	--aaa.show( bind, "bind" )
	aaa.layer.set_bind_2d( ref.layer_bdd, bind )					
--	local str = param.get( ref.prim_str )
--	sha:set_vert_int_2( gol.prim_str_to_id( str ) )
	gol.set_tex_unit_2d_bind( 4, 79 ) -- out of JumpFlood F14					
	for i=3,1,-1 do
		self:bind_texture_to_unit( i )
	end
	gol.set_tex_unit_cur( 1 )
	gol.set_wrap_2d_edge()	--	we need this on depth because we don't use the texture c_obj_ui to set it as for the color texture
	gol.set_tex_unit_cur( 0 )

--	bind = self:get_texture_bind_2d(4)
	--	gol.set_tex_unit_cur( 3 )
--	gol.set_tex_unit_2d_bind( 3, bind )
--	gol.bind_texture( bind )

--	gol.set_wrap_2d_edge()	
--	gol.set_tex_unit_cur( 0 )

	local str = param.get( ref.prim_str )
	--	self:print( gol.prim_str_to_id( str ) .. " - " .. str )
end

-- function meu:update()
-- end

function meu:draw_icon()
	self:draw_icon_texture( self:get_texture_bind_2d( 1 ) )
end

function meu:render_min( sha, b_compute )
	--if ga:get_pass_info().name=="shadow" then return end
	local ref = self.ref
	if not sha then
		sha = self:get_shading()
	end

--	aaa.obj.update_then_draw( sha.ref.obj )

	self:prepare_for_rendering( sha, true )
	--gol.update_uniform_vertex()
	--gol.update_uniform_geometry()
	--gol.update_uniform_fragment()
	--gol.update_uniform_render()
	--end
	--gol.update_uniform_compute()

	--aaa.obj.update_then_draw( ref.layer_shading )	
	--aaa.obj.update_then_draw( sha.ref.obj )
	--gol.update_uniform_render()
	param.set( ref.bdd, "compute", b_compute )
	aaa.obj.update_then_draw( ref.layer_bdd )
	--aaa.obj.update( ref.layer_bdd )
	--aaa.obj.draw( ref.layer_bdd )

end

function meu:draw()
	--if ga:get_pass_info().name=="shadow" then return end
	local ref = self.ref
	local sha = self:get_shading()

	--self:set_icon_bind( self:get_texture_bind_2d( 1 ) )

	self:draw_layers_begin()

		--if self:get_inst_key()=="1" then 
			--self:print( self:get_inst_key() )
		aaa.obj.update_then_draw( ref.layer_attr )
		--this works also		
		-- local prim = param.get( ref.prim )
		-- if prim <= 3	then	prim = 1
		-- else					prim = prim + 1
		-- end
		-- self:print( "\t"..prim.." direct "..param.get( ref.prim )+1 )
		-- sha:set_vert_int_2( prim )

		aaa.obj.update_then_draw( ref.layer_shading )	

		self:render_min( sha, true )

	self:draw_layers_end()
end

