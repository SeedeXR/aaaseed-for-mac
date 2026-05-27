
function meu:define_ui_proto( b_ui )
	if b_ui then
		--todo dseal with this (2025 MAy)
		self:box_debug( "LotusOut Proto" )
		local bu
		bu = self:add_selector(	{1,1,	8,5}, "Shader" )
			bu:set_nb_min_0( 5, 1 )
			for i = 1,5 do
				bu:set_item_text( i, i-3 )
			end
			bu:set_target_lua( MEU.__screen_offset )
	end
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgbf()

	self:add_camera()
	self:add_screen_viewport()
	self:add_mapping_by_side()
	self:add_bu_texture_target_unit()

	self:add_monitor()
	self:add_rendering( {1, 11} )
	--self:add_button( {1, 12.8 }, "TopLine", self.ref.rendering, "top_line" )
--	self:add_button( {1, 12.8 }, "UIDraw", self.ref.grid, "ui_draw", false )

	local S = .6
	local iy = 11.3
	self:add_size_uvf_video( {9,9})

	local ix = 1
	local iy = 14
	local SX = 2
	bu = self:add_trig_method(	{ix,		iy,			SX, 1},	"u-", 			self, "change_nb_uv", -1, 0	)
	bu = self:add_trig_method(	{ix+SX,		iy,			SX, 1},	"u+", 			self, "change_nb_uv", 1, 0	)
	bu = self:add_trig_method(	{ix+SX*2,	iy,			SX, 1},	"v-", 			self, "change_nb_uv", 0, -1	)
	bu = self:add_trig_method(	{ix+SX*3,	iy,			SX, 1},	"v+", 			self, "change_nb_uv", 0, 1	)
--	bu = self:add_trig_method(	{	9,		iy},			"center", 			self, "center"	)
--	bu = self:add_trig_method(	{	9,		iy+1},		"fit in x", 		self, "scale", "fit_x"	)
--	bu = self:add_trig_method(	{	9,		iy+2},		"fit in y", 		self, "scale", "fit_y"	)
--	bu = self:add_trig_method(	{	9,		iy+3},		"fit in screen",	self, "scale", "screen"	)
end

function meu:init()
	local ref=  self.ref
	local sha = self:add_shading()
	if not sha then
		self:box_error( " Need to update this MEU" )
	end
	ref.grid				=	aaa.obj.get_branch_by_class( self:get_layer(1), "bdd_grid_adjustable" )
	ref.grid_ui_draw		= 	param.get_ref( ref.grid, "ui_draw" )
	ref.grid_ui_intercept	= 	param.get_ref( ref.grid, "ui_intercept" )
end

function meu:change_nb_uv( ou, ov )
	local bdd = self:get_layer_bdd(1)
	local nb_u = param.get( bdd, "curve_nb_u" ) + ou
	local nb_v = param.get( bdd, "curve_nb_v" ) + ov
	param.set( bdd, "curve_nb_u_new", nb_u )
	param.set( bdd, "curve_nb_v_new", nb_v )
	param.set( bdd, "curve_nb_new_trig", true )
end

function meu:update()
	--aaa.print_fn()
	local ref = self.ref
	local sha = self:get_shading()
	if not sha then return end

	local meu_ui = app:get_gp():get_meu_ui()
	local b = meu_ui == self
	--self:print( b )
	param.set( ref.grid_ui_draw, b )
	if b then
		aaa.bdd.set_ui( ref.grid )
	else
		param.set( ref.grid_ui_intercept, false )
	--self:print( "zobo" )
	end
end
function meu:draw()
	--aaa.print_fn()
	MEU.draw( self )
	--aaa.print_fn( "End" )
end
