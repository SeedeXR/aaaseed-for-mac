function meu:define_meu_infos( )
	return { author = "Mâa", date="2024",
			tags = { "2d", "Core", "CoreGraphic", "ImageProcessing", "Texture" },
			help = "Blur a texture"
			 }
end

--todo deal with color

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

--	self:add_rgbf(	{1,12 } )
	local ix, iy = 1, 2
	local sy = 1
	local dy = .5
	bu = self:add_blur_selector(	{ix,iy,	8,sy}, "Method",	self, "s_method" )
	iy = iy + sy
	self.ui.bu_blur	= self:add_slider(	{1,3,	8,sy},	"Blur", self, "blur", 16, 1, 128 )

	iy = iy + sy + dy
	bu = self:add_selector(	{ix,iy,	8,sy}, "Diff" )
		bu:set_nb( 6, 1 )
		bu:set_target_lua( self, "s_diff" )
		bu:set_item_text( 1, "No", "Abs", "Diff", "Inv", "Min", "Max" )

end

function meu:set_bind_2d( bind )
	param.set( self.ref.smooth_bind, bind )
	param.set( self.ref.median_bind, bind )
end

function meu:init()
	local ref = self.ref
	ref.bdd_ocv	=	self:get_layer_bdd(1)
		ref.smooth_bind	=	param.get_ref( ref.bdd_ocv,		"image_src"			)
		ref.smooth_dst	=	param.get_ref( ref.bdd_ocv,		"image_dst_base"	)
		local pre = "blur"	--	"smooth"
		ref.smooth_x	=	param.get_ref( ref.bdd_ocv,		pre.."_size_x"		)
		ref.smooth_y	=	param.get_ref( ref.bdd_ocv,		pre.."_size_y"		)

	ref.bdd_multi	=	self:get_layer_bdd(2)
		ref.b_smooth	=	param.get_ref( ref.bdd_multi,	"smooth"			)
		ref.median_bind	=	param.get_ref( ref.bdd_multi,	"image_src"			)
		ref.median_dst	=	param.get_ref( ref.bdd_multi,	"image_dst_base"	)
		ref.method		=	param.get_ref( ref.bdd_multi,	"smooth_method"		)
		ref.smooth_sx	=	param.get_ref( ref.bdd_multi,	"smooth_size_x"		)
		ref.smooth_sy	=	param.get_ref( ref.bdd_multi,	"smooth_size_y"		)
		ref.b_diff		=	param.get_ref( ref.bdd_multi,	"diff"				)
		ref.diff_method	=	param.get_ref( ref.bdd_multi,	"diff_method"		)

end

--todo optimize on change only
function meu:update()
	local	ps	= param.set
	local	ref	= self.ref

	--if true then return end	--hacknow

	local v = self.ui.bu_blur:get_value()
	ps( ref.smooth_x, v )
	ps( ref.smooth_y, v )

	local blur_symbo = { "BLUR", "GAUSSIAN", "MEDIAN" }
	ps( ref.b_smooth, self.s_method~=1 )
	ps( ref.method, blur_symbo[self.s_method-1] )

	ps( ref.b_diff, self.s_diff~=1 )
	ps( ref.diff_method, self.s_diff-1 )

	ps( ref.smooth_sx, v )
	ps( ref.smooth_sy, v )

--[[
	local bind
	
	bind = self:get_texture_bind_2d( 1 )
	self:set_bind_2d( bind, 1 )
	ps( ref.smooth_bind, bind )

	bind = self:get_texture_bind_2d( 2 )
	self:set_bind_2d( bind, 2 )
	ps( ref.smooth_dst, bind )
--]]
end