function meu:define_meu_infos( )
	return {	author	= "Mâa",
				tags	= { "2d", "3D", "Art", "Draw", "Procedural", "Texture", "VJ", "Depreciated", "Unfinished" },
				help	= "Example of vertex/fragment shader using c_bdd_draw_instanced\n"..
							"with UI access to shader uniforms"
			 }
end
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	local sha = self:get_shading()

	local ix,iy = 1,1
	local SY = 1
	local DY = .2

	self:add_shading_ui(			{ix,iy,		8,SY} )
	self:add_slider(				{ix+4,iy,	4,SY},	"Nb",			ref.vertex_nb,nil,	2048,	1,1024*1024*8)
		:set_value_type_integer(true)
	iy = iy + SY + DY

	local y_begin = iy

------------------------------
	self:set_tab_key( "Main" )
	ix, iy = 1,y_begin

	self:add_button(				{ix,iy,		1,SY},	"Time Active",	self,"b_time",		false	):set_text_visible(false)
	self:add_slider(				{ix+1,iy,	5,SY},	"Time",			self,"time",		0,		0,100 )	
	self:add_trig_method(			{ix+6,iy,	2,SY},	"Restart",		self,"restart"		)
	iy = iy + SY + DY

------------------------------
	self:add_bu_texture_target_unit_nb( {ix,5.8,16,8},	"Tex_", 2,2 )	

	ix,iy = 9,y_begin
	self:add_camera()

	self:add_mapping_by_side_only(	{ix,iy,		8,SY}	)
	iy = iy + SY + DY

	self:add_rgbf(		{ix,iy,		8,SY}	)
	iy = iy + SY + DY

	self:add_blending( 				{ix,iy}				)
	iy = iy + SY + DY

	self:add_rendering()

------------------------------
	local sha = self:get_shading()
	self:add_shading_sliders_tab(	sha,	{"Vert","Frag"},	y_begin,SY	)

	ui.bu_time_vert = self:get_bu_by_key( "vertex_float_1" )
	ui.bu_vertex_nb_over = self:get_bu_by_key( "vertex_float_2" )
	ui.bu_time_geom = self:get_bu_by_key( "geometry_float_1" )
	ui.bu_time_frag = self:get_bu_by_key( "fragment_float_1" )
end

function meu:init()
	local ref = self.ref

	self.layer_shading = 2
	local sha = self:add_shading( self.layer_shading )

	ref.bdd			=	self:get_layer_bdd( 3 )
	ref.vertex_nb	=	param.get_ref( ref.bdd, "vertex_nb" )
end

function meu:restart()
	self.time = 0
end

function meu:update()
	local ref = self.ref
	local ui = self.ui
	local bu

	local t = self.time
	if self.b_time then
		t = t + aaa.time.dt
		self.time = t
		--aaa.show( t, "t" )
	end

	ui.bu_time_vert:set_value( t )
	bu = ui.bu_time_geom
	if bu then bu:set_value( t ) end
	ui.bu_time_frag:set_value( t )

	local nb = param.get( ref.vertex_nb )
	ui.bu_vertex_nb_over:set_value( 1./nb )
end

function meu:draw()
	self:draw_layers_begin()

		self:draw_layer( 1 )
		self:draw_layer( 2 )
		self:draw_layer( 3 )
		--gol.set_quad_uv()
		-- aaa.draw_rect_uv(-1, -1, 1, 1)

	self:draw_layers_end()
end