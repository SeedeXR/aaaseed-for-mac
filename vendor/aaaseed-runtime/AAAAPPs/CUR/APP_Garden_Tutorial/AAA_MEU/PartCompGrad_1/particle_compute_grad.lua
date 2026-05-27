
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = 1
	local DY = .2

	self:add_camera()
	self:add_rendering()
	self:add_shading_ui( {9,2.5, 8,1} )

	local sha = self:get_shading()
	

	self:add_particle_nb_ui({ix,iy,		8,SY*2}, ref.bdd, "particle" )
	iy = iy + SY*2 + DY

	self:add_blending( {ix,iy, 2,SY} )
	iy = iy + SY + DY

	local col = {}
	col[1] = self:add_rgba(	{ix,iy,		4,1},	"color_1",	false )
	col[2] = self:add_rgba(	{ix+4,iy,	4,1},	"color_2",	false )
	ui.color = col
	iy = iy + SY + DY

	self:add_sliders_xyz(	{ix,iy,		8,SY},	"Pos",		self.vol_center, 	false,	5	)
	iy = iy + SY
	self:add_sliders_xyz(	{ix,iy,		8,SY},	"Size",		self.vol_size, 		false,	10	)
	iy = iy + SY + DY

	local SX = 8/3
	iy = iy + SY
	self:add_slider_two(	{ix,iy,		8,SY},	"Speed_range",	nil, sha:get_ref_comp_float(1), sha:get_ref_comp_float(2), 0, 1, 0, 1 )
	iy = iy + SY
	self:add_slider(		{ix,iy,		8,SY},	"Speed", 		sha:get_ref_comp_float(3),	nil, 0, 0, 1 )
	iy = iy + SY

	local function add_slider( index, compo )
		bu = self:add_slider_two(	{ix+SX*index,iy,	SX,SY},	"Speed_"..compo,	self, "sp_min_"..compo, "sp_max_"..compo, 0, 0, -1, 1 )
		bu:set_color_back(compo)
		bu:add_values_def( -.5, 0, .5 )
	end
	add_slider(	0,	"x" )
	add_slider(	1,	"y" )
	add_slider(	2,	"z" )

	local SYT = 4
	ix,iy = 9,4
	self:add_slider(	{ix,iy,		8,SY},	"Field Influence", 	sha:get_ref_comp_float(5),	nil, 0, -8, 8 )
	iy = iy + SY
	local layer = self:get_layer( 6 )
	self:add_bu_texture( {ix,iy,	4,SYT}, "Field", 2, false	)
	self:add_bu_texture( {ix+4,iy,	4,SYT}, "Color", 3, false	)
	iy = iy + SYT + DY

	self:add_slider(	{ix,iy,		4,SY},	"Part Size", 		self, "part_size",			1,	-1,1	)
	self:add_slider(	{ix+4,iy,	4,SY},	"Part Size Factor", self, "part_size_factor", 	0,	-1,1	)
--	self:add_slider(	{ix+6,iy,	2,SY},	"Part Size Gamma", 	self, "part_size_gamma",	1, .001, 8 )
--		:set_text( "Gamma" )
	iy = iy + SY
	self:add_slider(	{ix+4,iy,	4,SY},	"Part Size U",		self, "part_size_u_factor",	1,	0,5		)
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},	"Hardness",			self, "hardness",			1,	0,1		)
	self:add_slider(	{ix+4,iy,	4,SY},	"Round",			self, "round",				1,	0,1		)
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},	"Noise",			self, "shape_noise",		1,	0,1		)
	self:add_slider(	{ix+4,iy,	4,SY},	"Freq",				self, "shape_noise_freq",	1,	0,10	)

end

function meu:init()
	local ref = self.ref

	ref.layer_attr		= self:get_layer(1)
	ref.layer_shading	= self:get_layer(2)
	local sha = self:add_shading( 2 )
	ref.layer_bdd		= self:get_layer(3)
	ref.bdd				= self:get_layer_bdd(3)

	ref.render 			= aaa.obj.get_by_class_no_error( ref.layer_attr, "rendering" )
	ref.prim			= param.get_ref( ref.render,	"primitive"			)
	ref.point_nb 		= param.get_ref( ref.bdd,		"point_nb"			)
	ref.point_nb_used	= param.get_ref( ref.bdd,		"point_nb_used"		)
	ref.prim_str		= param.get_ref( ref.bdd,		"primitive_used"	)

	self.vol_center = { x=0, y=0, z=0 }
	self.vol_size = { x=0, y=0, z=0 }

	sha:set_save_vert_float( false, 1,5 )
	sha:set_save_vert_int(   false, 2,2 )
	sha:set_save_vert_vec4(  false, 1,2 )

	sha:set_save_frag_float( false, 1,4 )
	sha:set_save_frag_vec4(  false, 1,2 )

	sha:set_save_comp_float( false, 4,4 )
	sha:set_save_comp_vec4(  false, 1,2 )
end


function meu:set_point_nb( nb, nb_used )
	local ref = self.ref
	param.set( ref.point_nb,		nb		)
	param.set( ref.point_nb_used,	nb_used	)
end

function meu:set_xyz_sxyz( x, y, z, sx, sy, sz )
	local sha = self:get_shading()
	sha:set_vert_vec4( 1, x, y, z )
	sha:set_vert_vec4( 2, sx, sy, sz )
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

function meu:update()
	local ref = self.ref
	local sha = self:get_shading()
	local ui = self.ui

	self:update_particle_nb()

	for i=1,2 do
		local r,g,b,a = ui.color[i]:get_rgba()
		sha:set_frag_vec4( i, r,g,b,a )
	end
	--inc time
	local time = (self.time or 0) + aaa.time.dt
	self.time = time
	sha:set_comp_float( 4, aaa.time.dt )
	sha:set_comp_vec4( 1, self.sp_min_x,				self.sp_min_y,					self.sp_min_z				)
	sha:set_comp_vec4( 2, self.sp_max_x-self.sp_min_x,	self.sp_max_y-self.sp_min_y,	self.sp_max_z-self.sp_min_z	)

	local c = self.vol_center
	--table.print(c)
	sha:set_vert_vec4( 1, c.x, c.y, c.z )
	--self:print( c[1]..c[2]..c[3] )
	local s = self.vol_size
	sha:set_vert_vec4( 2, s.x, s.y, s.z )

	sha:set_vert_float_1_5( self.part_size, self.part_size_factor*32,
							self.part_size_u_factor
							)
	--, self.part_size_gamma/2. )
	sha:set_frag_float_1_4( self.hardness,	self.round,			
							self.shape_noise, self.shape_noise_freq
						)	

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

		aaa.obj.update( ref.layer_bdd )
		-- todo this we needed the update before
		local str = param.get( ref.prim_str )
		--	self:print( gol.prim_str_to_id( str ) .. " - " .. str )
		sha:set_vert_int_2( gol.prim_str_to_id( str ) )
		--	gol.update_uniform_render()
		aaa.obj.draw( ref.layer_bdd )

	self:draw_layers_end()
end
