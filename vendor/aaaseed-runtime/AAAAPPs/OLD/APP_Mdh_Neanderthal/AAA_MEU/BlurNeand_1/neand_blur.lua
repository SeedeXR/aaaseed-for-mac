function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ix, iy

	ix = 1
	iy = 1

	bu = self:add_slider(	{ix,iy,	8,1}, 	"Blur",		self, "blur", 			0.	)
--	bu = self:add_button( {ix, 12 }, "Scale Z", FOX, "spe_scale_use" ) --todo careful this 0 or 1 not true or false
end

function meu:init()
	local ref = self.ref

	ref.bdd = self:get_layer_bdd( 1 )
	ref.blur_x	=	param.get_ref( ref.bdd,	"blur_size_x" )
	ref.blur_y	=	param.get_ref( ref.bdd,	"blur_size_y" )
end

function meu:update()
	local ref = self.ref

	local blur = self.blur * 31
	param.set( ref.blur_x,	blur )
	param.set( ref.blur_y,	blur )
end

	