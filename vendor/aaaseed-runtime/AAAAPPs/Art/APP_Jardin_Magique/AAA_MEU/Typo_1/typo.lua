function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()
	self:add_monitor()
end

function meu:init()
	local ref = self.ref

	ref.bdd		=	self:get_layer_bdd( 1 )
	ref.text	=	param.get_ref( ref.bdd, "text" )
end

function meu:set_text( str )
	param.set( self.ref.text, str )
end