
function meu:define_meu_infos()
	return { author = "mrvux Mâa",
			tags = { "2d", "texture", "renderpass", "imageprocessing" },
			help = 	{
					"Use to add Depth of Field, doing Blur depending on depth",
					"need to be documented",
					"Output to the Fbo Dof"
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
	local SY = 1.
	local SYT = 2.5 * SY
	local DY = .2

	self:add_camera()

	ix,iy = self:define_ui_ndc()
	bu = self:add_button(	{ix+4,	iy-SY,	SY, SY},	"VDebug", 			sha:get_ref_frag_int(1), nil,	0 )

	bu = self:add_slider(	{ix,iy,			8,SY},		"blur size",		self, "blur_size",				9,		0, 100 )
	iy = iy + SY
	bu = self:add_button(	{ix,	iy,		SY, SY},	"Near", 			self, "b_near",					false ):set_text_visible(false)
	bu = self:add_slider_two({ix+SY,iy,		8-SY,SY},	"Near_Out_In",		self, "near_out", "near_in", 0, 5,	0, 50 )
	iy = iy + SY
	bu = self:add_button(	{ix,	iy,		SY, SY},	"Far", 				self, "b_far",					false ):set_text_visible(false)
	bu = self:add_slider_two({ix+SY,iy,		8-SY,SY},	"Far_In_Out", 		self, "far_in", "far_out",		10, 20, 0, 50 )
	iy = iy + SY
	bu = self:add_button(	{ix,	iy,		SY, SY},	"use texture", 		sha:get_ref_frag_int(2), nil,	0 )
	--todo slider here
	iy = iy + SY
	self:add_bu_texture_target_unit( {ix,iy,			nil,SYT},	"Tex", 3	)
	iy = iy + SYT

	iy = 2.5
	self:define_ndc_src(	{9,iy,			8,8 },		{"Src",	"Depth"} )
end

function meu:init()
	local ref = self.ref
	self:set_meu_fbo( "dof" )

	local sha = self:add_shading()
	sha:set_save_frag_int(		false,	1,2 )
	sha:set_save_frag_float(	false,	1,2 )
end


function meu:draw()
	self:do_fbo()

	local sha = self:get_shading()
 	local sx,sy = self:get_texture_size()
	if sx then
		sha:set_frag_float_1_2( 1./sx, 1./sy )
	end

 	self:draw_layers_begin()
	 	sha:set_frag_float_3_7(	self.b_draw and self.blur_size or 0,
		 							self.b_near and self.near_out or -100,
		 							self.b_near and self.near_in or -99,
		 							self.b_far and self.far_in or 99,
		 							self.b_far and self.far_out or 100
								)
 		self:draw_layer( 1 )
--		sha:set_frag_float_3( self.b_draw and self.blur_size or 0 )
		self:draw_layer( 2 )
 	self:draw_layers_end()
end

function meu:get_preset_nb() 	return 32	end



