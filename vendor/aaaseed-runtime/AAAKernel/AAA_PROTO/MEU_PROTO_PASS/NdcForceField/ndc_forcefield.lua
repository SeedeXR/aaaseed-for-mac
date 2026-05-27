function meu:define_meu_infos()
	return { author = "Etienne",
			tags = { "3d", "procedural", "unfinished" },
			help = 	{
					"Done for Aquarium Monaco, checked the shaders to understand.",
					}
		
		}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix = 1
	local iy = 1
	local SX = 4
	local SY = .9
	local DY = .2

	self:add_camera()
	
	ix,iy = self:define_ui_ndc()

	self:add_bu_texture_target_unit( {ix,iy, 		4,4}, "Distance", 1, false )
	self:add_bu_texture_target_unit( {ix+4,iy, 		4,4}, "Mask", 	2, false )

	ix,iy = 9,3
	self:add_button(	{ix,iy,		SY,SY},		"PullBack Active",	self, 	"b_sha_pullback", 		true	)
		:set_text_visible(false)
	self:add_slider(	{ix+SY,iy,	8-SY,SY},	"PullBack Dist",	self,	"sha_pullback_dist",	4,		0,20 )
	iy = iy + SY
	self:add_slider_two({ix,iy,		SX,SY},		"DF Range", 		self,	"sha_df_min", "sha_df_max",	0, .25, -1,2 )
--	self:add_slider(	{ix,iy,		8,SY},		"Field Max Dist",	self,	"sha_field_dist_max",	0.05,	0,1 )
	iy = iy + SY
	self:add_button(	{ix,iy,		SY,SY},		"Field  Active",	self, 	"b_sha_field", 		true	)
		:set_text_visible(false)
	self:add_slider(	{ix+SY,iy,	8-SY,SY},	"Field Str",		self,	"sha_field_strenght",	-100, 	-500,500 )
	iy = iy + SY
	self:add_slider(	{ix+SY,iy,	8-SY,SY},	"Field Gamma",		self,	"sha_field_gamma",		1,		.1,20 )
end

function meu:init()
	local ref = self.ref
	self:set_meu_fbo( "cap.fa" )
	self:add_shading()
		:set_save_frag_float(	false,	1, 2 )
		:set_save_frag_int(		false,	1, 1 )
end

function meu:draw()
	self:do_fbo()
	local sha = self:get_shading()
 	local sx,sy = self:get_texture_size(1)
	if sx then
		sha:set_frag_float_1( 1 / sx )
		sha:set_frag_float_2( 1 / sy )
	end

	sha:set_frag_float_10_14(	self.b_sha_pullback and self.sha_pullback_dist or 10000,
								self.sha_df_min, self.sha_df_max,
								self.b_sha_field and self.sha_field_strenght or 0,
								self.sha_field_gamma )		

	self:bind_texture_to_unit( 1 )
	self:bind_texture_to_unit( 2 )

 	self:draw_layers_begin()
 		self:draw_layer( 1 )
		sha:set_vert_int_1( self.b_draw and 0 or 1 )
		self:draw_layer( 2 )
 	self:draw_layers_end()
end




