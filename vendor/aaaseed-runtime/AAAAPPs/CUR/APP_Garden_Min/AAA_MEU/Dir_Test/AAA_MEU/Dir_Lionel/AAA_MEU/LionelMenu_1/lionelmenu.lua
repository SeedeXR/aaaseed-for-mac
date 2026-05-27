function meu:define_ui()
	local ref = self.ref
	local bu

	-- MENU
	--
	bu = self:add_blending( {2,2} )

	bu = self:add_button( {5, 2, 2, 1 }, "MENU" )
		bu:set_multiple( {"file", "open", "save", "quit", "options"}, "file_menu" )
		bu:set_item_data( 1, "1-file", "2-open", "3-save", "4-quit quit" )
	--bu:set_nb( 2 )

	local text_ex = { "A", "Min", "Max", "Ta Mere", "Ta Grand Mere", "Ton Arriere Grand Mere" }
	bu = self:add_button( {2, 8, 4, 1 }, "MMenu1" )
 		bu:set_menu( text_ex, "test1_menu" )
 		bu:set_item_data( 1, 5 )
 		bu:set_item_data( 4, 4, "TGM", "trop trop trop long" )

	bu = self:add_button( {7, 8, 2, 1.95 }, "MMenu2" )
 		bu:set_menu( text_ex, "test2_menu"  )
	bu = self:add_button( {10, 8, 1, 1.01 }, "MMenu3" )
 		bu:set_menu( text_ex, "test3_menu"  )
	bu = self:add_button( {12, 8, .5, 1 }, "MMenu4" )
 		bu:set_menu( text_ex, "test4_menu"  )

 	--MULTIPLE SELECTOR
	bu = self:add_selector(	{2,5,	4,1}, "TEST MULTIPLE" )
			:set_selection_multiple( true )

end
