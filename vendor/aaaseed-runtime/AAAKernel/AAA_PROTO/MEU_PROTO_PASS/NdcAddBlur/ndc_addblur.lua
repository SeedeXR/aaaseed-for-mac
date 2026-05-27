function meu:define_meu_infos()
	return { author = "mrvux Mâa",
			tags = { "2d", "texture", "renderpass", "imageprocessing", "unfinished" },
			help = 	{
					"Use to add blur and eventually Glow (which use Blur result)",
					"Done for Monaco Aquarium but unused, need to be finished and documented or trashed",
					"Output to the fbo AddBlur"
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

	bu = self:add_slider(	{ix,iy,	8,SY},	"blur size",	nil, sha:get_ref_frag_float(3), 	9, 0, 100 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY},	"add factor",	nil, sha:get_ref_frag_float(4), 	0.5, 0, 10 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY},	"add intensity",nil, sha:get_ref_frag_float(5), 	1.0, 0, 10 )
	iy = iy + SY
	--bu = self:add_slider_two(	{ix,iy,		8,SY},	"Near",			nil, sha:get_ref_frag_float(4), sha:get_ref_frag_float(5),  0, 10, 0, 100 )
	--iy = iy + SY
	--bu = self:add_slider_two(	{ix,iy,		8,SY},	"Far", 			nil, sha:get_ref_frag_float(6), sha:get_ref_frag_float(7), 90, 100, 0, 100 )
	--iy = iy + SY
	--bu = self:add_button(		{ix, iy,	SY,SY },	"use texture", 	sha:get_ref_frag_int(1), nil, false )
	--todo slider here
	iy = iy + SY

	iy = 4
	SY = 2.5
	self:add_bu_texture_target_unit( {9,iy, nil,SYT},	"Src", 1	)
	iy = iy + SYT
	self:add_bu_texture_target_unit( {9,iy, nil,SYT},	"Emissive", 2	)
	iy = iy + SY

end

function meu:init()
	self:set_meu_fbo( "addblur" )
	self:add_shading()
end

function meu:draw()
	self:do_fbo()

	local sha = self:get_shading()
 	local sx,sy = self:get_texture_size()
	if sx then
		sha:set_frag_float_1_2( 1./sx, 1./sy )
	end

 	self:draw_layers_begin()
 		self:draw_layer( 1 )
--		sha:set_frag_float_3( self.b_draw and self.blur_size or 0 )
		self:draw_layer( 2 )
 	self:draw_layers_end()
end




