function meu:define_ui()
	local ref = self.ref
	local bu

	bu = self:add_button( {3, 6 }, "TRight", self, "tright", 1 )
		bu:set_text_xy( .5, 1 )
		bu:set_text_factor( .7 )

	bu = self:add_button( {3, 3 }, "TLeft", self, "tleft", 1 )
		bu:set_text_xy( -3.5, 1 )
		bu:set_text_factor( 1.2 )

	bu = self:add_button( {3, 10 }, "BRight", self, "bright", 1 )
		bu:set_text_xy( .5, -1 )
		bu:set_text_factor( 1.1 )

	bu = self:add_button( {10, 6 }, "BLeft", self, "bleft", 1 )
		bu:set_text_xy( -3.5, -1 )
		bu:set_text_factor( 1.5 )

	bu =  self:add_button( {10, 10 }, "TOP", self, "TOP", 1 )
		bu:set_text_xy( -1.25, 1.2 )
		bu:set_text_factor( 1 )

end
