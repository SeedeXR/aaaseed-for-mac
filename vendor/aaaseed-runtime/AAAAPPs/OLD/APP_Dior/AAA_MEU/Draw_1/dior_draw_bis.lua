--meu:print( "success" )
--aaa.print( "success too" )

function meu:set_tex( bind, texture_unit )
	if bind then
		if texture_unit or self.b_shading then
			local bank_2d, bind_2d = aaa.img.make_bank_bind_2d( bind )
			local ref = self.ref
			local t = ref.tex[ texture_unit or 4 ]
			param.set( t.bank_2d, bank_2d )
			param.set( t.bind_2d, bind_2d )
			if not texture_unit then
				self:draw_layer( 2 )
			end
	 	else
	 	 	gol.bind_texture( bind )
		end
	else
		self:print( "no_bind_here" )
	end
end

function meu:set_color( name, alpha )
	local cui = self.ui[name]
	local r,g,b, a = cui:get_rgba()
	gol.color( r,g,b, a*alpha )
end

