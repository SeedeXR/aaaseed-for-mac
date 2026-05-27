
meu.draw_icon = MEU.draw_icon_opencv

		--todo deal with color
function meu:define_ui()
	self.b_ui_ok = false
	local ref = self.ref
	local bu
	local par

	local ix, iy = 1, 2
	local sy = 1
	local dy = .5

	local blur_max = 32

	local bdd
	local pass
	--we do first pass
	pass = ref.pass[1]
	self:add_blur_selector(	{ix,iy,		8,sy},		"Method1",		self, "s_method1" )
	iy = iy + sy
	self.ui.bu_blur1
		= self:add_slider(	{ix,iy,		8,sy},		"Blur1", 		self, "blur1",	4,	1,blur_max ):set_value_type_integer(true)
	iy = iy + sy + .2

	bu = self:add_button(	{ix,iy, 	sy,sy},		"BC1 ON", 		pass.bdd, "bc" ):set_text_visible( false )
	self:add_slider_two(	{ix+sy,iy,	8-sy,sy},	"BC1", 			pass.bdd, "bc_min" , "bc_max",	0,1,	0,1 )
	iy = iy + sy + dy

	bu = self:add_button(	{ix,iy, 	sy, sy },	"Dilate", 		pass.bdd, "dilate" )
	self:add_slider(		{ix,iy+sy,	8,sy},		"Dilate_pass", 	pass.bdd, "dilate_pass",	1,	0,16 ):set_value_type_integer(true)
	iy = iy + sy*2
	bu = self:add_button(	{ix,iy, 	sy,sy},		"Erode", 		pass.bdd, "erode" )
	bu = self:add_button(	{ix+4,iy, 	sy,sy},		"First", 		pass.bdd, "erode_first" )
	self:add_slider(		{ix,iy+sy,	8,sy},		"Erode_pass", 	pass.bdd, "erode_pass",		1,	0,16 ):set_value_type_integer(true)
	iy = iy + sy*2 + dy*2

	self:add_blur_selector(	{ix,iy,		8,sy},		"Method2",		self, "s_method2" )
	iy = iy + sy
	self.ui.bu_blur2
		= self:add_slider(	{ix,iy,		8,sy},		"Blur2",		self, "blur2",	4,	1,blur_max ):set_value_type_integer(true)
	iy = iy + sy +.2

	bu = self:add_button(	{ix,iy, 	sy,sy},		"BC2 ON",		pass.bdd, "bc_out" ):set_text_visible( false )
	self:add_slider_two(	{ix+sy,iy,	8-sy,sy},	"BC2",			pass.bdd, "bc_out_min" , "bc_out_max", 0, 1., 0, 1. )
	iy = iy + sy*2 + dy

	ix,iy = 9 ,2
	--we do second pass
	pass = ref.pass[2]
	bu = self:add_selector(	{ix,iy,		8,sy},		"Diff" )
		bu:set_nb( 6, 1 )
		bu:set_target_lua( self, "s_diff" )
		bu:set_item_text( 1, "No", "Abs", "Diff", "Inv", "Min", "Max" )
	iy = iy + sy + dy*2

	pass = ref.pass[2]
	self:add_blur_selector(	{ix,iy,		8,sy},		"Method3",		self, "s_method3" )
	iy = iy + sy
	self.ui.bu_blur3
		= self:add_slider(	{ix,iy,		8,sy},		"Blur3",		self, "blur3",	16,	1,128 ):set_value_type_integer(true)
	iy = iy + sy + .2

	bu = self:add_button(	{ix,iy,		sy,sy },	"BC3 ON", 		pass.bdd, "bc_out" ):set_text_visible( false )
	self:add_slider_two(	{ix+sy,iy,	8-sy,sy},	"BC3", 			pass.bdd, "bc_out_min" , "bc_out_max", 0, 1., 0, 1. )
	iy = iy + sy*2 + dy

	self.b_ui_ok = true
end

function meu:init()
	local ref = self.ref
	ref.pass = {}
	for i = 1,3 do
		local pass = {}
		local bdd = self:get_layer_bdd(i+1)
		pass.bdd	= bdd
--		ref.smooth_bind		=	param.get_ref( bdd,	"image_src"				)
--		ref.smooth_dst		=	param.get_ref( bdd,	"image_dst_base"	)
		pass.smooth1 =
		{
			active	=	param.get_ref( bdd,	"smooth"					),
			method	=	param.get_ref( bdd,	"smooth_method"				),
			sx		=	param.get_ref( bdd,	"smooth_size_x"				),
			sy		=	param.get_ref( bdd,	"smooth_size_y"				),
		}

		pass.b_bc			=	param.get_ref( bdd,	"bc"					)

		pass.smooth2 =
		{
			active	=	param.get_ref( bdd,	"smooth2"					),
			method	=	param.get_ref( bdd,	"smooth2_method"				),
			sx		=	param.get_ref( bdd,	"smooth2_size_x"				),
			sy		=	param.get_ref( bdd,	"smooth2_size_y"				),
		}

		pass.b_diff			=	param.get_ref( bdd,	"diff"						)
		pass.diff_method	=	param.get_ref( bdd,	"diff_method"			)

		pass.b_bc_out		=	param.get_ref( bdd,	"bc_out"				)
		ref.pass[i] = pass
	end
end

--todo optimize on change only
function meu:update()
	if not self.b_ui_ok then
		return
	end
	local	ps	= param.set
	local	ref	= self.ref

	local bind
	local pass
	local smooth
	local blur_symbo = { "BLUR", "GAUSSIAN", "MEDIAN" }
	local v


	pass = ref.pass[1]

	smooth = pass.smooth1
	v = self.ui.bu_blur1:get_value()
	ps( smooth.sx, v )
	ps( smooth.sy, v )

	ps( smooth.active, self.s_method1~=1 )
	ps( smooth.method, blur_symbo[self.s_method1-1] )

	smooth = pass.smooth2
	v = self.ui.bu_blur2:get_value()
	ps( smooth.sx, v )
	ps( smooth.sy, v )

	ps( smooth.active, self.s_method2~=1 )
	ps( smooth.method, blur_symbo[self.s_method2-1] )

	pass = ref.pass[2]
	ps( pass.b_diff, self.s_diff~=1 )
	ps( pass.diff_method, self.s_diff-1 )

	smooth = pass.smooth2
	v = self.ui.bu_blur3:get_value()
	ps( smooth.sx, v )
	ps( smooth.sy, v )

	ps( smooth.active, self.s_method3~=1 )
	ps( smooth.method, blur_symbo[self.s_method3-1] )

	--self:print( ref.smooth_sx )

end