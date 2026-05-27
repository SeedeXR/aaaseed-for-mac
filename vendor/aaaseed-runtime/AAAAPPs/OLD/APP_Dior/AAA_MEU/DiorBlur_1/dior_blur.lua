function meu:define_ui()
	local ref = self.ref
	local bu
	local ix, iy

	ix = 1
	iy = 1

	self.ui.blur = self:add_slider(	{ix,iy,	8,1}, 	"Blur",		self, "blur", 			0.	)
end

function meu:init()
	local ref = self.ref

	ref.bdd = self:get_layer_bdd( 1 )
	ref.blur_x	=	param.get_ref( ref.bdd,	"blur_size_x" )
	ref.blur_y	=	param.get_ref( ref.bdd,	"blur_size_y" )
end

function meu:set_blur( blur )
	self:set_bu_value( "blur", blur )
end

function meu:update()
	local ref = self.ref
	local blur = self.blur * 15		--31 is the limit for cuda in opencv
	param.set( ref.blur_x,	blur )
	param.set( ref.blur_y,	blur )
end
