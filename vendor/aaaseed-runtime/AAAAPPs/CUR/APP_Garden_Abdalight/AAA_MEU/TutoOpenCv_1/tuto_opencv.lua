function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
	local par

	local ix,iy = 1,1
	local DY = .2
	local SY = 1

	local bdd = self.bdd_opencv
	
	self:add_trig_fn( 		{ix,iy,		4,SY}, 	"Focus", 	aaa.obj.set_focus_ui,  bdd )
	iy = iy + SY + DY

	local SYM = 5
	self:add_bu_texture( {ix,iy,	8,SYM},		"Src",		1, false )
	self:add_bu_texture( {ix+8,iy,	8,SYM},		"Dst",		2, false )

	iy = iy + SYM + DY
	local iy_begin = iy
	local SX = 8/3
	self:add_button( 		{ix,iy,		SX,SY}, 	"Mono", 			bdd,  "mono" )
	self:add_button( 		{ix+SX,iy,	SX,SY},		"Float", 			bdd,  "Float" )
	iy = iy + SY
	self:add_button( 		{ix,iy,		SX,SY}, 	"Resize", 			bdd,  "resize" )
	self:add_slider( 		{ix+SX,iy,	8-SX,SY},	"Factor",			self,  "resize_factor",	1,	0,1 )
	iy = iy + SY + DY
	self:add_button( 		{ix,iy,		SX,SY}, 	"Equalize", 		bdd,  "equalize_hist_enable" )
	iy = iy + SY + DY
	self:add_button( 		{ix,iy,		SX,SY}, 	"Edge", 			bdd,  "edge_enable" )
	self:add_button( 		{ix+SX,iy,	SX,SY},		"Laplace", 			bdd,  "edge_laplacian" )
	iy = iy + SY
	self:add_slider( 		{ix,iy,		4,SY},		"Offset",			bdd,  "edge_offset",	0,	-4, 4 )
	self:add_slider( 		{ix+4,iy,	4,SY},		"Scale",			bdd,  "edge_scale",		1,	0, 4 )

	iy = iy + SY + DY
	self:add_button( 		{ix,iy,		4,SY},		"Dilate First", 	bdd,  "dilate_first" )
	iy = iy + SY
	self:add_button( 		{ix,iy,		SX,SY},		"Dilate", 			bdd,  "dilate_enable" )
	self:add_slider( 		{ix+SX,iy,	8-SX,SY},	"Dilate_iteration", bdd,  "dilate_iteration",	4, 0, 128 )
		:set_text( "Iteration" )
	iy = iy + SY
	self:add_button( 		{ix,iy,		SX,SY},		"Erode", 			bdd,  "erode_enable" )
	self:add_slider( 		{ix+SX,iy,	8-SX,SY},	"Erode_iteration",	bdd,  "erode_iteration",	4, 0, 128 )
		:set_text( "Iteration" )

	ix,iy = 9,iy_begin
	self:add_button( 		{ix,iy,		SX,SY}, 	"Invert", 			bdd,  "invert_enable" )

end

function meu:init()
	local ref = self.ref

	local bdd = self:get_layer_bdd(1)
	self.bdd_opencv = bdd
	ref.bind_src = param.get_ref( bdd, "image_src" )
	ref.bind_dst = param.get_ref( bdd, "image_dst_base" )
	ref.resize_fx = param.get_ref( bdd, "resize_fx" )
	ref.resize_fy = param.get_ref( bdd, "resize_fy" )
	
	--ref.bdd_equal = param.get_ref( bdd, "equalize_hist_enable" )

end

function meu:update()
	local ref = self.ref
	local bind_src = self:get_texture_bind_2d( 1 );
	param.set( ref.bind_src, bind_src )
	local bind_dst = self:get_texture_bind_2d( 2 );
	aaa.show( bind_src.." to "..bind_dst, "bind" )
	param.set( ref.bind_dst, bind_dst )

	local resize = self.resize_factor
	param.set( ref.resize_fx, resize )
	param.set( ref.resize_fy, resize )
end


