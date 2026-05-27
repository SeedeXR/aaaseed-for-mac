function meu:add_keyboard()
	local function add( name )
		local keyboard = KEYBOARD:create( name )
		return keyboard
	end

	if not self.keyboard1 then
		self.keyboard1 = add( "NumPad" )
	end
	if not self.keyboard2 then
		self.keyboard2 = add( "PC" )
		self.keyboard2:set_button_close_active( true )
	end
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	self:add_rgbfa(	{1,5} )

	self:add_camera()

	self:add_bu_texture_target_unit()

	--CONFIRMATION BUTTON
	bu = self:add_trig_method(	{10, 2}, "Add keyboards",		self, "add_keyboard" )
		bu:set_text_rect_ratio( 6 )

	local bu_1 = self:add_window( {7,8,1,1},	"Attacher" )
	local bu_2 = self:add_window( {7,9,1,1},	"Attacher2" )

	local bu_3 = self:add_window( {9,8,2,2},	"Attachant" )
	local bu_4 = self:add_window( {9,9,2,2},	"Attachant2" )

--[[	removed
	bu_3:set_bu_link(bu_1, "position")
	bu_3:set_bu_link(bu_1, "mini")
	bu_3:set_bu_link(bu_1, "alpha")

	bu_3:set_bu_link(bu_2, "position")
	bu_3:set_bu_link(bu_2, "mini")
	bu_3:set_bu_link(bu_2, "angle")

	bu_4:set_bu_link(bu_1, "size")
	bu_3:set_sxy(0.05,0.05)
	bu_3:set_alpha( 1 )
--]]
--[[
	--Button triple click
	bu = self:add_button( {1, 12 }, "TRIPLE", self, "Triple", 1 )
		bu:set_method_on_click_triple( self, "quit" )


	--Button triple click
	bu =  self:add_button( {1, 13 }, "LONG", self, "Long", 1 )
		bu:set_method_on_click_long( self, "quit" )
--]]


	--Edit text
	local ix,iy = 9,3
	bu = self:add_text(	{ix,iy,	8,1}, "non edit" )
		bu:set_alpha_bu( .25 )
		--bu:set_dplane( -1 )

	bu = self:add_text(	{}, "edit" )
		bu:set_color_back( { 0, 1, 1, .5 } )
		bu:set_editable( true, "PC" )
		bu:set_color_border( { 1, 0, 1, 1 } )
		bu:set_max_length( 16 )

	bu = self:add_text(	{}, "edit 2" )
		bu:set_color_back( { 0, 1, 1, .5 } )
		bu:set_editable( true, "PC" )
		bu:set_color_border( { 1, 0, 1, 1 } )
		bu:set_max_length( 16 )
		--bu:set_text_color( { 1, 0, 0, 1 } )
		--bu:set_tex_back( bind_back )

	bu = self:add_text(	{}, "edit 3" )
		bu:set_color_back( { 0, 1, 1, .5 } )
		bu:set_editable( true, "NumPad" )
		bu:set_color_border( { 1, 0, 1, 1 } )
		bu:set_max_length( 10 )
		--bu:set_text_color( { 1, 0, 0, 1 } )
		--bu:set_tex_back( bind_back )
---[[

	--CONFIRMATION BUTTON
	bu = self:add_button( {1,15, 3,1 }, "Quit " )
		bu:set_confirmation( true )
		bu:set_method_on_click( self, "confirm" )

	self.media_dir = app.media_dir_rel.."Doors/"
	local bind_check_on = IMGS.get_bind( self.media_dir.."/check_on.png" )
	local bind_check_off = IMGS.get_bind( self.media_dir.."/check_off.png" )

	bu = self:add_button( {1, 12 }, "TEST4", self, "test", 1  )
		:set_alpha_bu( .5 )
		bu:set_tex_draw( bind_check_on, bind_check_off )
--]]
end

function meu:confirm()
	self:print( "I confirm" )
end
