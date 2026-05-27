
meu.draw_icon = MEU.draw_icon_opencv

		--todo deal with color
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix, iy = 1, 2
	local sy = .8
	local dy = .5
	self:add_blur_selector(	{ix,iy,		8,sy}, "Method",		self, "s_method" )
	iy = iy + sy
	self.ui.bu_blur	= self:add_slider(	{ix,iy,	8,sy},	"Blur", self, "blur", 16, 1, 128 )
	iy = iy + sy + dy
	bu = self:add_button(	{ix, iy, 	sy,sy}, "BC ON",		ref.bdd, "bc" )
	self:add_slider_two(	{ix,iy+sy,	8,sy},	"BC", 			ref.bdd, "bc_min" , "bc_max", 0, 1., 0, 1. )
	iy = iy + sy*2 + dy

	bu = self:add_selector(	{ix,iy,	8,sy}, "Diff" )
		bu:set_nb( 6, 1 )
		bu:set_target_lua( self, "s_diff" )
		bu:set_item_text( 1, "No", "Abs", "Diff", "Inv", "Min", "Max" )
	iy = iy + sy + dy
	bu = self:add_button(	{ix,iy, 	sy,sy}, "Erode First",	ref.bdd, "erode_first" )
	iy = iy + sy
	bu = self:add_button(	{ix,iy, 	sy,sy}, "Dilate", 		ref.bdd, "dilate" )
	self:add_slider(		{ix,iy+sy,	8,sy},	"Dilate_pass", 	ref.bdd, "dilate_pass" ,  1., 0, 128 ):set_value_type_integer(true)
	iy = iy + sy*2
	bu = self:add_button(	{ix, iy, 	sy,sy}, "Erode", 		ref.bdd, "erode" )
	self:add_slider(		{ix,iy+sy,	8,sy},	"Erode_pass", 	ref.bdd, "erode_pass" ,  1., 0, 128 ):set_value_type_integer(true)
	iy = iy + sy*2 + dy

	ix,iy = 9 ,3
	self:add_blur_selector(	{ix,iy,		8,sy},	"Method2",		self, "s_method2" )
	iy = iy + sy
	self.ui.bu_blur2	= self:add_slider(	{ix,iy,	8,sy},	"Blur2", self, "blur2", 16, 1, 128 )
	iy = iy + sy + dy
	bu = self:add_button(	{ix, iy, 	sy,sy}, "BC Out ON",	ref.bdd, "bc_out" )
	self:add_slider_two(	{ix,iy+sy,	8,sy},	"BC OUT", 		ref.bdd, "bc_out_min" , "bc_out_max", 0, 1., 0, 1. )
	iy = iy + sy*2 + dy
end

function meu:init()
	local ref = self.ref
	local bdd = self:get_layer_bdd(2)
	ref.bdd	= bdd
		ref.smooth_bind		=	param.get_ref( bdd,	"image_src"			)
		ref.smooth_dst		=	param.get_ref( bdd,	"image_dst_base"	)

		ref.b_smooth		=	param.get_ref( bdd,	"smooth"			)
		ref.method			=	param.get_ref( bdd,	"smooth_method"		)
		ref.smooth_sx		=	param.get_ref( bdd,	"smooth_size_x"		)
		ref.smooth_sy		=	param.get_ref( bdd,	"smooth_size_y"		)

		ref.b_smooth2		=	param.get_ref( bdd,	"smooth2"			)
		ref.method2			=	param.get_ref( bdd,	"smooth2_method"	)
		ref.smooth2_sx		=	param.get_ref( bdd,	"smooth2_size_x"	)
		ref.smooth2_sy		=	param.get_ref( bdd,	"smooth2_size_y"	)

		ref.b_diff			=	param.get_ref( bdd,	"diff"				)
		ref.diff_method		=	param.get_ref( bdd,	"diff_method"		)

		ref.b_bc			=	param.get_ref( bdd,	"bc_out"			)
end
--todo optimize on change only
function meu:update()
	local	ps	= param.set
	local	ref	= self.ref

	local bind

	local v = self.ui.bu_blur:get_value()
	ps( ref.smooth_sx, v )
	ps( ref.smooth_sy, v )

	local blur_symbo = { "BLUR", "GAUSSIAN", "MEDIAN" }
	ps( ref.b_smooth, self.s_method~=1 )
	ps( ref.method, blur_symbo[self.s_method-1] )

	v = self.ui.bu_blur2:get_value()
	ps( ref.smooth2_sx, v )
	ps( ref.smooth2_sy, v )

	ps( ref.b_smooth2, self.s_method2~=1 )
	ps( ref.method2, blur_symbo[self.s_method2-1] )

	ps( ref.b_diff, self.s_diff~=1 )
	ps( ref.diff_method, self.s_diff-1 )

end