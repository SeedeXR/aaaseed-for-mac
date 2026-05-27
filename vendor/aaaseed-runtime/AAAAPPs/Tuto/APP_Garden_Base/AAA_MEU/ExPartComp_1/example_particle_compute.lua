
function meu:get_preset_nb()
	return 32
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = .8
	local DY = .2

	self:add_camera()
	self:add_rendering()
	self:add_shading_ui( {9,2, 8,1} )

	local sha = self:get_shading()
	

	self:add_particle_nb_ui({ix,iy,		8,SY*2}, ref.bdd, "particle" )
	iy = iy + SY*3 + DY


	self:add_sliders_xyz(	{ix,iy,		8,SY},	"Pos",					self.vol_center, 	false,	5	)
	iy = iy + SY
	self:add_sliders_xyz(	{ix,iy,		8,SY},	"Size",					self.vol_size, 		false,	10	)
	iy = iy + SY + DY

	SY = 1
	self:add_slider(		{ix,iy,		4,SY},	"Field Distance Pixel", sha:get_ref_comp_float(4),	nil, 1.,	.1, 256 )
		:set_draw_force_cr( true )
	self:add_slider(		{ix+4,iy,	4,SY},	"Gradient Stability",	sha:get_ref_comp_float(2),	nil, 0,		0,1 )
		:set_draw_force_cr( true )
	iy = iy + SY
	self:add_button(		{ix,iy,	SY,SY},	"Field Distance Use",	self, "b_field_active",		true		)
			:set_text( "Use" ):set_text_inside( true )
	self:add_slider(		{ix+SY,iy,4-SY,SY},	"Field Influence", 	self, "field_inf",			0, -8, 8 )
		:set_draw_force_cr( true )
	iy = iy + SY + DY

	local SYT = 4
	local layer = self:get_layer( 6 )
	self:add_bu_texture(	{ix,iy,		4,SYT}, "Field", 2, false	):set_preset_use( false )
	self:add_bu_texture(	{ix+4,iy,	4,SYT}, "Color", 3, false	):set_preset_use( false )
	iy = iy + SYT + DY

	local SX = 2
	self:add_slider(		{ix,iy,		SX,SY},	"Speed", 				sha:get_ref_comp_float(3),	nil, 0, 0, 1 )
		:set_color_back("grey")
	local function add_slider( index, compo )
		bu = self:add_slider_two(	{ix+SX*index,iy,	SX,SY},	"Speed_"..compo,	self, "sp_min_"..compo, "sp_max_"..compo, 0, 0, -1, 1 )
			:set_color_back(compo)
			:set_text( string.upper(compo) )
			:add_values_def( -.5, 0, .5 )
	end
	add_slider(	1,	"x" )
	add_slider(	2,	"y" )
	add_slider(	3,	"z" )


	ix,iy = 9,3+DY
	SY = 1

	self:add_button(	{ix,iy,		SY,SY},		"Part Size Use",		self, "b_part_size",		true		):set_text( "Use" ):set_text_inside( true )
	self:add_slider(	{ix+SY,iy,	4-SY,SY},	"Part by Unit", 		self, "part_size",			0,	1,256	)
	self:add_button(	{ix+4,iy,	1.5,SY},	"Part dir",				self, "b_part_size_dir",	true		):set_text( "Align" )
	self:add_slider(	{ix+5.5,iy,	2.5,SY},	"Part Ratio",			self, "part_size_ratio",	1,	1,16	):set_color_back( "u" )
	iy = iy + SY
	self:add_button(	{ix,iy,		SY,SY},		"Part Size Factor Use",	self, "b_part_size_factor",	true		):set_text( "Use" ):set_text_inside( true )
	self:add_slider(	{ix+SY,iy,	4-SY,SY},	"Part Size Factor", 	self, "part_size_factor", 	0,	1,256		)
	self:add_slider_two({ix+4,iy,	4,SY},		"Part Size Remap", 		self, "part_size_remap_min", "part_size_remap_max", 0,1,	0,1  )
	iy = iy + SY + DY
	self:add_slider(	{ix+4,iy,	4,SY},		"Part Size Gamma", 		self, "part_size_gamma",	1, .001,8	)
		:set_draw_gamma(true)
	iy = iy + SY + DY
--		:set_text( "Gamma" )

	
	self:add_slider(	{ix,iy,		4,SY},	"Hardness",			self, "hardness",			1,	0,1		)
	self:add_slider(	{ix+4,iy,	4,SY},	"Round",			self, "round",				1,	0,1		)
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},	"Noise",			self, "shape_noise",		1,	0,1		)
	self:add_slider(	{ix+4,iy,	4,SY},	"Freq",				self, "shape_noise_freq",	1,	0,10	)

	iy = iy + SY + DY
	local col = {}
	self:add_slider(		{ix,iy,		4,SY},	"Alpha Influence", sha:get_ref_comp_float(6),nil, 	0,	0,1	)
	iy = iy + SY
	col[1] = self:add_rgba(	{ix,iy,		4,1},	"color_1",	false )
	col[2] = self:add_rgba(	{ix+4,iy,	4,1},	"color_2",	false )
	ui.color = col
	iy = iy + SY + DY
		self:add_blending( 	{ix+4,iy, 2,SY} )
	iy = iy + SY + DY

end

function meu:init()
	local ref = self.ref

	ref.layer_attr		= self:get_layer(1)
	ref.layer_shading	= self:get_layer(2)
	local sha = self:add_shading( 2 )
	ref.layer_bdd		= self:get_layer(3)
	
	ref.render 			= aaa.obj.get_by_class_no_error( ref.layer_attr, "rendering" )
	ref.prim			= param.get_ref( ref.render,	"primitive"			)

	ref.bdd				= self:get_layer_bdd(3)
	ref.point_nb 		= param.get_ref( ref.bdd,		"point_nb"			)
	ref.point_nb_used	= param.get_ref( ref.bdd,		"point_nb_used"		)
	ref.prim_str		= param.get_ref( ref.bdd,		"primitive_used"	)

	self.vol_center = { x=0, y=0, z=0 }
	self.vol_size   = { x=0, y=0, z=0 }

	sha:set_save_vert_float(	false, 1,3 )
	sha:set_save_vert_int(		false, 2,2 )
	sha:set_save_vert_vec4(		false, 1,3 )

	sha:set_save_frag_float(	false, 1,3 )
	sha:set_save_frag_vec4(		false, 1,3 )

	sha:set_save_comp_float(	false, 1,6 )
	sha:set_save_comp_vec4( 	false, 1,2 )
end


function meu:set_point_nb( nb, nb_used )
	local ref = self.ref
	param.set( ref.point_nb,		nb		)
	param.set( ref.point_nb_used,	nb_used	)
end

-- function meu:set_xyz_sxyz( x, y, z, sx, sy, sz )
-- 	local sha = self:get_shading()
-- 	sha:set_vert_vec4( 1, x, y, z )
-- 	sha:set_vert_vec4( 2, sx, sy, sz )
-- end

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

function meu:update()
	local ref = self.ref
	local sha = self:get_shading()
	local ui = self.ui

	self:update_particle_nb()

	-- pass 2 colors to fragment shader
	for i=1,2 do
		sha:set_frag_vec4( i, ui.color[i]:get_rgba() )
	end

	--inc time
	local time = (self.time or 0) + aaa.time.dt
	self.time = time
	sha:set_comp_float( 1, aaa.time.dt ) 

	sha:set_comp_vec4( 1, self.sp_min_x,				self.sp_min_y,					self.sp_min_z				)
	sha:set_comp_vec4( 2, self.sp_max_x-self.sp_min_x,	self.sp_max_y-self.sp_min_y,	self.sp_max_z-self.sp_min_z	)

	sha:set_comp_float( 5, self.b_field_active and self.field_inf or 0 )
	local c = self.vol_center
	sha:set_vert_vec4( 1, c.x, c.y, c.z )
	local s = self.vol_size
	sha:set_vert_vec4( 2, s.x, s.y, s.z )

	-- Particle Size
	local ps
	if self.b_part_size then
		ps = self.part_size
		if ps < 1. then
			ps = 1.
			self.part_size = ps
		end
		ps = (ps-1)/(256-1)
		ps = .5/(math.pow(ps,2.)*256+1)
	else
		ps = 0
	end
	local psf
	if self.b_part_size_factor then
		psf = self.part_size_factor
		if psf < 1. then
			psf = 1.
			self.b_part_size_factor = psf
		end
		psf = (psf-1)/(256-1)
		psf = .5/(math.pow(psf,2.)*256+1)
	else
		psf = 0
	end
	sha:set_vert_float_1_3( ps,psf, math.max( self.part_size_gamma, .1 ) )
	local ru,rv
	if self.b_part_size_dir then
		ru = self.part_size_ratio
		rv = 1/ru
	else
		rv = self.part_size_ratio
		ru = 1/rv
	end
	sha:set_vert_vec4( 3, ru, rv, self.part_size_remap_min, self.part_size_remap_max )
	-- Particle Noise
	sha:set_frag_float_1_3( self.hardness,	self.round,			
							self.shape_noise
						)

	sha:set_frag_vec4( 3, self.shape_noise_freq, self.shape_noise_freq / ru )

--	local str = param.get( ref.prim_str )
--	sha:set_vert_int_2( gol.prim_str_to_id( str ) )
end

function meu:draw()
	--if ga:get_pass_info().name=="shadow" then return end
	local ref = self.ref
	local sha = self:get_shading()

	self:draw_layers_begin()

		aaa.obj.update_then_draw( ref.layer_attr )
		--this works also
		-- local prim = param.get( ref.prim )
		-- if prim <= 3	then	prim = 1
		-- else					prim = prim + 1
		-- end
		-- self:print( "\t"..prim.." direct "..param.get( ref.prim )+1 )
		-- sha:set_vert_int_2( prim )
		
		aaa.obj.update_then_draw( ref.layer_shading )

		for i=2,3 do
	 		self:bind_texture_to_unit( i )
	 	end
		gol.set_tex_unit_cur( 1 )
			--gol.set_wrap_2d_repeat()
			gol.set_magnification_linear()
			gol.set_minification_linear_mipmap_linear()
		gol.set_tex_unit_cur( 2 )
			--gol.set_wrap_2d_repeat()
			gol.set_magnification_linear()
			gol.set_minification_linear_mipmap_linear()
		gol.set_tex_unit_cur( 0 )

		aaa.obj.update( ref.layer_bdd )
		-- todo this we needed the update before
		local str = param.get( ref.prim_str )
		--	self:print( gol.prim_str_to_id( str ) .. " - " .. str )
		sha:set_vert_int_2( gol.prim_str_to_id( str ) )
		--	gol.update_uniform_render()
		aaa.obj.draw( ref.layer_bdd )

	self:draw_layers_end()
end
