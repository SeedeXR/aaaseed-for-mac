if CLASS.DECLARE( "BU_TEXT", BU,  {
	ref 			= {},
	__edit 			= nil,
	__edit_type 	= "",
	__tex_back 		= nil,
	__color_back	= BU:get_color_back_named( "BU_TEXT" ),
	__color_border 	= { 0, 1, 0, 1 },
	__max_length 	= 10,
	__b_value_load_save = true,
	} ) then
	BU_TEXT:set_class_status_doc(	CLASS.STATUS.GABU,
									"Bu to edit TEXT" )
end

function BU_TEXT:make_text_info()
	self:set_text_color( "info" )
	self:set_preset_use( false )
	self:set_value_load_save( false )
	self:set_text_draw_always( true )
	--self:set_ui_active( false )
	return self
end

--
-- SET/GET
--
--todo better hacked quickly
function BU_TEXT:get_values()
--	local tab = { self:get_text() }
	return self:get_text()
end
function BU_TEXT:set_values( tab )
	--self:print( "BUI:set_values" )
	if type(tab) == "table" then
		self:set_text( tab[1] )
	else
		self:set_text( tab )	
	end
	return self
end
function BU_TEXT:get_value()
	return self:get_text()
end
function BU_TEXT:set_value( v )
	self:set_text( v )
	return self
end

--todo cleaner (BU_TEXT derived from BUI)
function BU_TEXT:set_value_ui( val, id )	-- id there for add_to_undo_history
	--self:print( "set_value( "..val..", "..id.." )" )
	local prev = self:get_value()
	self:set_value( val )
	self:add_to_undo_history( prev, id )
end
function BU_TEXT:set_values_ui( tab_or_value )
	self:print_debug( "BU_TEXT:set_values" )
	return BUI.set_values_ui( self, tab_or_value )
end
function BU_TEXT:get_id_interacting()		return 1 end

--to make sure we only set ok type and store it lowercase
function BU_TEXT:set_edit_type(type) 		self.__edit_type = type							return self		end
function BU_TEXT:get_edit_type() 			return self.__edit_type 										end

function BU_TEXT:set_color_border( color )	self.__color_border = color 					return self		end
function BU_TEXT:get_color_border()			return self.__color_border										end

function BU_TEXT:set_tex_back( bind )		self.__tex_back = bind							return self		end
function BU_TEXT:get_tex_back()				return self.__tex_back											end

function BU_TEXT:set_max_length( val )		self.__max_length = val 						return self		end
function BU_TEXT:get_max_length()			return self.__max_length										end

--todo be sure self.__edit always defined when called
function BU_TEXT:set_separator( b )			self.__edit.b_separator = b 					return self		end
function BU_TEXT:is_a_separator()			return self.__edit.b_separator 									end

function BU_TEXT:set_cursor_blocked( b )	self.__edit.b_cursor_blocked = b 				return self		end
function BU_TEXT:is_cursor_block()			return self.__edit.b_cursor_blocked								end

--todo target ?
-- function BU_TEXT:get_text()
-- 	local bal = self:__get_balue()
-- 	if bal:is_target() then
-- 		return tostring(bal:get_value())
-- 	end
-- 	return oo.getsuper(BU_TEXT).get_text()
-- end

function BU_TEXT:is_select_text() 			return self.__nb_elem_text_select ~= 0 							end
function BU_TEXT:get_nb_elem_text_select()	return self.__nb_elem_text_select								end
function BU_TEXT:get_select_text()
	local cursor_pos = self:get_cursor_pos()
	local val = self:get_nb_elem_text_select()
	local str = self:get_text()
	local str_prev, str_sel, str_next

	--if select text is before the cursor pos
	if val > 0 then
		str_prev = str:sub( 0, cursor_pos - val )
		str_sel = str:sub( cursor_pos - val + 1, cursor_pos )
		str_next = str:sub( cursor_pos + 1, str:len() )
	--if select text is after the cursor pos
	else
		str_prev = str:sub( 0, cursor_pos )
		str_sel = str:sub( cursor_pos + 1 , cursor_pos - val )
		str_next = str:sub( cursor_pos - val + 1, str:len() )
	end

	return str_sel, str_prev, str_next
end

function BU_TEXT:remove_select_text()
	local str_del, str_prev, str_next = self:get_select_text()
	--Set new text
	self:set_text(str_prev..""..str_next)
	--no more text select
	self:set_nb_elem_text_select( 0 )
	--replace cursor
	local len = str_prev:len()
	self:set_cursor_pos( len )
end

function BU_TEXT:set_nb_elem_text_select( val )
	local len = self:get_text():len()
	local cursor_pos = self:get_cursor_pos()

	val = val or len
	--if we can move the cursor, we can select another element of the text
	--self:print("val : "..val.." block ? "..self:is_cursor_block())
	if not self:is_cursor_block() then
		self.__nb_elem_text_select = val
	end
end

function BU_TEXT:is_first_key()				return self.__edit.__cursor_pos == 0						end
function BU_TEXT:get_cursor_pos() 			return self.__edit.__cursor_pos								end
function BU_TEXT:increment_cursor_pos( val )
	local len = self:get_text():len()
	local pos = self:get_cursor_pos() + val

	aaa.print_fn()
	local b_block = false

	if pos > len then
		b_block = true
		pos = len
	elseif pos < 0 then
		b_block = true
		pos = 0
	end

	self:set_cursor_pos( pos )
	self:set_cursor_blocked(b_block)
end
function BU_TEXT:increment_cursor_pos_x( x )
	x = x + .5
	local nb_elem_text_select = self:get_nb_elem_text_select()
	local cursor_pos = self:get_cursor_pos()
	local str = self:get_text()
	local len = str:len()
	local str_tmp
	local sca_x = self:get_ratio_y()
	local sx_str_len = aaa.get_str_translate( str:sub( 0, cursor_pos ) ) * sca_x

	--if x is before cursor pos
	if x < sx_str_len then
		self:increment_cursor_pos( -1 )
		if not self:is_cursor_block() then
			self:set_nb_elem_text_select( nb_elem_text_select - 1 )
		end
		return
	end

	str_tmp = str:sub(  0, cursor_pos + 1 )
	sx_str_len = aaa.get_str_translate( str_tmp ) * sca_x
	--if x is after cursor pos
	if x > sx_str_len then
		self:increment_cursor_pos( 1 )
		if not self:is_cursor_block() then
			self:set_nb_elem_text_select( nb_elem_text_select + 1 )
		end
	end

end

function BU_TEXT:set_cursor_pos( val )
	--aaa.print_fn()
	--aaa.debug.print_traceback()
	--default set the cursor to the end of the text
	val = val or self:get_text():len()
	self:set_cursor_blocked(false)
	self.__edit.__cursor_pos = val
end
function BU_TEXT:set_cursor_pos_from_x( x )
	x = x + .5
	local str = self:get_text()
	local str_tmp
	local len = str:len()
	local sca_x = self:get_ratio_y()
	local sx_str_len = aaa.get_str_translate( str ) * sca_x

	if x > sx_str_len then
		self:set_cursor_pos( len )
		return
	end

	for i = 0, len do
		str_tmp = str:sub( 0, i )
		sx_str_len = aaa.get_str_translate( str_tmp ) * sca_x

		if x < sx_str_len then
			self:set_cursor_pos( i - 1 )
			self:set_nb_elem_text_select( 0 )
			return
		end
	end
end

function BU_TEXT:set_last_state()
	--deal with escape key
	self.last_state = {}
	local state = self.last_state
	state.text = self:get_text()
	state.b_separator = self:is_a_separator()
end
function BU_TEXT:get_last_state()
	local state = self.last_state
	return state.text, state.b_separator
end

function BU_TEXT:is_editable_active()		return self.__edit.b_editable_active	end
function BU_TEXT:set_editable_active( b_edit )
	if self:is_editable_active() and b_edit then return end

	self.__edit.b_editable_active = b_edit
	if b_edit then
		self:set_last_state()
	end
end
function BU_TEXT:set_editable_active_and_remove_text( b_edit )
	if self:is_editable_active() and b_edit then return end

	self:set_editable_active( b_edit )
	if b_edit then
		if self:get_edit_type() == "NumPad" then
			self:set_separator(false)
		end
		self:set_text("")
		self:set_cursor_pos()
	end
end

function BU_TEXT:is_editable()	return self.__edit ~= nil end
function BU_TEXT:set_editable( b_edit, type )
	self.__edit = {}
	self:set_edit_type( type )

	self:set_cursor_pos()
	self:set_nb_elem_text_select( 0 )
	self:set_editable_active( false )

	if type == "NumPad" then
		self:set_separator( false )
	end

	if b_edit then
		self:set_method_on_click_double( self, "set_editable_active", true )
		self:set_method_on_click_long( self, "set_editable_active_and_remove_text", true )
	end

	app:__set_bu_edit( self )
end

local allow_char_numpad = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", ",", ".", "-" }

function BU_TEXT:push_letter( key, alteration, keyboard, b_special )
	aaa.print_fn()

	if not key then return end
	--self:show(key.." "..alteration.." "..b_special)
	if self:is_editable_active() then
		--enter
		if key == 13 and not b_special then
			--disable edition
			self:set_editable_active( false )
		 --backspace
		elseif key == 8 and not b_special then
			--remove text if selected
			if self:is_select_text() then
				self:remove_select_text()
			else
				--Get the string before and after the cursor pos
				local str_prev, str_next = self:get_strs_cursor()
				local len = str_prev:len()
				--Get the char to remove
				local char_remove
				char_remove = str_prev:sub( len, len )
				--Remove it from the string
				str_prev = str_prev:sub( 0, len - 1 )
				--Set the new string
				self:set_text( str_prev..""..str_next )

				--For NumPad only, deal with separator removal
				if char_remove and self:get_edit_type() == "NumPad" then
					if char_remove == "," or char_remove == "." then
						self:set_separator( false )
					end
				end

				self:increment_cursor_pos( -1 )
			end
		--delete
		elseif key == 127 then
			--remove text if selected
			if self:is_select_text() then
				self:remove_select_text()
			else
				--Get the string before and after the cursor pos
				local str_prev, str_next = self:get_strs_cursor()
				local len = str_next:len()
				--Get the char to remove
				local char_remove
				char_remove = str_next:sub( 0, 1 )
				--Remove it from the string
				str_next = str_next:sub( 2, len )
				--Set the new string
				self:set_text( str_prev..""..str_next )

				--For NumPad only, deal with separator removal
				if char_remove and self:get_edit_type() == "NumPad" then
					if char_remove == "," or char_remove == "." then
						self:set_separator( false )
					end
				end
			end
		--escape
		elseif key == 27 then
			--Cancel operation
			local str, b_separator = self:get_last_state()
			self:set_text( str )
			if self:get_edit_type() == "NumPad" then
				self:set_separator( b_separator )
			end
			--Disable edition
			self:set_editable_active( false )
		--left arrow
		elseif (b_special and key == 274) or (b_special and key == 100) or key == "LEFT" then
			--if shift
			if alteration == "SHIFT" or aaa.keyboard.is_shift() then
				--Select text by left
				local nb_elem_sel = self:get_nb_elem_text_select()
				self:increment_cursor_pos( -1 )
				self:set_nb_elem_text_select( nb_elem_sel - 1 )
			else
				--If select text
				if self:is_select_text() then
					--Deselect it and place the cursor to the right place
					local sel_text = self:get_select_text()
					local nb_elem_text_select = self:get_nb_elem_text_select()
					if nb_elem_text_select > 0 then
						self:set_cursor_pos( self:get_cursor_pos() - sel_text:len() )
					else
						self:set_cursor_pos( self:get_cursor_pos() )
					end

					self:set_nb_elem_text_select( 0 )
				else
					--Else just move cursor to the left
					self:increment_cursor_pos( -1 )
				end
			end
		--right arrow
		elseif (b_special and key == 275) or (key == 102 and b_special) or key == "RIGHT" then
			--if shift
			if alteration == "SHIFT" or aaa.keyboard.is_shift() then
				--Select text by right
				local nb_elem_sel = self:get_nb_elem_text_select()
				self:increment_cursor_pos( 1 )
				self:set_nb_elem_text_select( nb_elem_sel + 1 )
			else
				--If select text
				if self:is_select_text() then
					--Deselect it and place the cursor to the right place
					local sel_text = self:get_select_text()
					local nb_elem_text_select = self:get_nb_elem_text_select()
					if nb_elem_text_select > 0 then
						self:set_cursor_pos( self:get_cursor_pos() )
					else
						self:set_cursor_pos( self:get_cursor_pos() + sel_text:len() )
					end

					self:set_nb_elem_text_select( 0 )
				else
					--Else just move cursor to the right
					self:increment_cursor_pos( 1 )
				end
			end	
		elseif	key == 1 and not b_special then		-- CTRL a
			self:select_all_text()
		elseif 	key == 3 and not b_special then		-- CTRL c
			self:copy_text( keyboard, true )
		elseif 	key == 22 and not b_special then	-- CTRL v
			self:paste_text( keyboard )
		elseif 	key == 24 and not b_special then	-- CTRL x
			self:cut_text( keyboard, true )	
		-- elseif	key == 26 and not b_special then	-- CTRL z
		-- 	self:undo_text()
		--alteration ctrl
		elseif alteration == "LCTRL" then
			self:do_ctrl_alteration( key, keyboard )
		--space
		elseif key == 32 and self:get_edit_type() ~= "NumPad" then
			self:push_letter_alpha( key )
			self:increment_cursor_pos( 1 )
		--other key
		elseif not b_special and key > 33 then
			--If select text, replace it by the char
			if self:is_select_text() then
				self:remove_select_text()
			end
			self:print( "end of push_letter key is "..key )
			if self:get_edit_type() == "NumPad" then
				--val can be 0 or 1
				local val = self:push_letter_num( key )
				self:increment_cursor_pos( val )
			elseif self:get_edit_type() == "PC" then
				local val = self:push_letter_alpha( key )
				self:increment_cursor_pos( val )
			end
		end
	end
end

function BU_TEXT:push_letter_alpha( key )

	local cur_text
	if self:get_text():len() < self:get_max_length() then
		local char = string.char(key)
		self:show( char )
		local str_prev, str_next = self:get_strs_cursor()
		cur_text = str_prev..""..char..""..str_next
		self:set_text( cur_text )
		return 1
	else
		local str = "reach max length"
		self:show( str )
		self:print_debug( str )
		return 0
	end
end

function BU_TEXT:push_letter_num( key )
	if self:get_text():len() < self:get_max_length() then
		local cur_text
		local char = string.char(key)
		local str_prev, str_next = self:get_strs_cursor()
		local first_char = str_next:sub(0,1)
		local b_separator = self:is_a_separator()

		for i = 1, #allow_char_numpad do
			if allow_char_numpad[i] == char then
				if char == "," or char == "." then
					if not b_separator then
						cur_text = str_prev..""..char..""..str_next
						self:set_separator( true )
					else
						return 0
					end
				elseif char == "-" then
					if self:is_first_key() and first_char ~= "-" then
						cur_text = char..""..str_next
					else
						return 0
					end
				else
					if first_char ~= "-" then
						cur_text = str_prev..""..char..""..str_next
					else
						return 1
					end
				end
				self:set_text( cur_text )
				return 1
			end
		end
	else
		self:show( "reach max length" )
	end

	return 0
end

function BU_TEXT:do_ctrl_alteration( key, keyboard )
	local b_used = true
	if key == 97 then		-- CTRL a
		self:print("CTRL A")
		self:select_all_text()	
	-- elseif key == 122 then	-- CTRL z
	-- 	self:print("CTRL Z")
	-- 	self:undo_text()
	elseif key == 99 then	-- CTRL c
		self:print("CTRL C")
		self:copy_text( keyboard )
	elseif key == 118 then	-- CTRL v
		self:print("CTRL V")
		self:paste_text( keyboard )
	elseif key == 120 then	-- CTRL x
		self:print("CTRL X")
		self:cut_text( keyboard )
	else
		b_used = false
	end
	return b_used
end

-- function BU_TEXT:undo_text()
-- 	self:print("undo yet to implement")
-- end

function BU_TEXT:cut_text( keyboard, b_copy_real_clipboard )
	self:print("cut")
	if not self:is_select_text() then
		self:print("nothing to cut")
		return
	end

	self:copy_text( keyboard, b_copy_real_clipboard )
	self:remove_select_text()
end

function BU_TEXT:copy_text( keyboard, b_copy_real_clipboard )
	self:print("copy")
	if not self:is_select_text() then
		self:print("nothing to copy")
		return
	end
	local str = self:get_select_text()
	keyboard:add_text_to_clipboard( str )

	if b_copy_real_clipboard then
		aaa.clipboard.move_to( str )
	end
end

function BU_TEXT:paste_text( keyboard )
	self:print("past")

	local str_copy = keyboard:get_text_from_clipboard()

	if str_copy == "" then
		self:print("nothing to past")
		return
	end

	self:remove_select_text()

	for i = 1, str_copy:len() do
		local key = string.byte(str_copy, i)
		self:push_letter( key )
	end

end

function BU_TEXT:select_all_text()
	self:set_cursor_pos()
	self:set_nb_elem_text_select()
end

function BU_TEXT:get_strs_cursor()
	local cur_text = self:get_text()
	local str_prev, str_next
	local cursor_pos = self:get_cursor_pos()
	local len = cur_text:len()

	if cursor_pos > 0 then
		str_prev = cur_text:sub( 0, cursor_pos)
		str_next = cur_text:sub( cursor_pos + 1 )
	else
		str_prev = ""
		str_next = cur_text
	end

	return str_prev, str_next
end

function BU_TEXT:sel_text_xy( x_begin, x_end )

end

function BU_TEXT:do_click_down( x,y )
	--self:print("click "..x.." "..y)
	if self:is_editable() then
		self:set_cursor_pos_from_x( x )
		-- Here we link with available keyboard (creating one may be better ?)
		-- also we need to set physical target virtual
		--todo with is the monitoring busses used ?
		if GP.cur.__busses.monitoring.__keyboards ~= nil then
			local keyboard = GP.cur.__busses.monitoring.__keyboards[1]
			keyboard:attach_bu()
			GA:set_keyboard_physical_target_virtual( keyboard )
		end
	end
	oo.getsuper(BU_TEXT).do_click_down(self, x, y)
end

function BU_TEXT:do_mouse_move( x,y )
	if self:is_editable() then
		self:increment_cursor_pos_x( x )
	end
	oo.getsuper(BU_TEXT).do_mouse_move(self, x,y )
end

function BU_TEXT:update()
	--self:print( "update()" )

	local par = self.target_param
	if par then
		self:set_text( param.get_str( par ) )
	end

	if self:get_contact_nb() > 0 then
		--self:sel_text_xy( self.__init_pos.x, self.__cur_pos.x )
	end

	oo.getsuper(BU_TEXT).update(self)
end

function BU_TEXT:set_target_param( par )
	self.target_param = par
end

function BU_TEXT:draw_tex_back( bind )
	gol.color_white()
	local DY = .5
	aaa.draw_bind_rect( bind, -.5, -DY, .5, DY )
end

function BU_TEXT:draw_select_text( val )
	local cursor_pos = self:get_cursor_pos()
	local rx, ry = self:get_ratio_xy()
	local sca_x = ry
	local str_select, str_prev, str_next = self:get_select_text()

	--Get the size of the text
	local sx_str_len = aaa.get_str_translate( str_select )
	local sx_str_len_prev = aaa.get_str_translate( str_prev )

	sx_str_len_prev = sx_str_len_prev * sca_x
	sx_str_len = sx_str_len * sca_x

	gol.set_logic_op_invert()
	gol.color_black()
	aaa.draw_rect( sx_str_len_prev - .5, -.5, sx_str_len_prev + sx_str_len - .5, .5 )
	gol.set_logic_op_none()
end

function BU_TEXT:draw_cursor_pos()
	local cursor_pos = self:get_cursor_pos()
	local rx, ry = self:get_ratio_xy()
	local str = tostring( self:get_text() )
	--Get the string before the cursor
	local len = str:len()
	if cursor_pos > 0 then
		str = str:sub( 0, cursor_pos)
	else
		str = ""
	end
	local sca_x = ry

	--Get the size of the text
	local sx_str_len = aaa.get_str_translate( str )
	sx_str_len = sx_str_len * sca_x

	--Draw the cursor
	gol.color_white()
	aaa.draw_line( sx_str_len - .5, -.5, sx_str_len - .5, .5 )
end

function BU_TEXT:draw()
	--if true then return end

	if GA.b_spy then aaa.spy.push_range( "BU_TEXT", 3 ) end

		--self:print( "draw()" )
		local bind_back = self:get_tex_back()
		if bind_back then
			self:draw_tex_back( bind_back )
		else
			--self:print( "g")
			gol.set_texture_dim( 0 )
			self:gol_draw_rect_back()
		end

		if self:is_text_draw() then
			if GA.b_spy then aaa.spy.push_range( "text", 5 ) end

			local text = self:get_text()
			if text then
				--gol.set_line_width( 2 )
				--	gol.color( border_line_color[1],border_line_color[2],border_line_color[3], border_line_color[4] * self:get_alpha_bu() )
				--	aaa.draw_rect_line_size()

				self:draw_text_color_sel()
				--[[
				local rect = {}
				rect.l = -.5 rect.b = -.5 rect.r = .5 rect.t = .5
				if self:is_editable() and self:is_editable_active() then
					rect.r = 10
				end
				self:draw_text_in_rect( text, rect )
				--]]
				if self:is_editable() and self:is_editable_active() then
					--todo need to be refined
					self:draw_text_lrbt( text, -.5,10, -.5,.5 )
				else
					--self:print( "align_x is : "..self:get_text_align_x() )
					self:draw_text_lrbt( text, -.5,.5, -.5,.5 ) --, self:get_text_align_x() )
				end
			end

			if GA.b_spy then aaa.spy.pop_range() end

			if self:is_editable() then
				local nb_elem_text_select = self:get_nb_elem_text_select()
				if nb_elem_text_select ~= 0 and text then
					self:draw_select_text( nb_elem_text_select )
				end
				if self:is_editable_active() then
					local color_border = self:get_color_border()
					gol.color( color_border )
					--gol.set_line_width( b and 2 or 1 )
					gol.set_line_width( 1 )
					self:gol_draw_rect_line()

					self:draw_cursor_pos()
				end
			end
		end

	if GA.b_spy then aaa.spy.pop_range() end
end

--tmp
--todo merge with set_value/get_value so preset and save fuynctions too
function BU_TEXT:do_dialog_hook( what, value, dialog_table )
	if what == "change_value" then
		self:set_text( value )
--	elseif what == "open" then
--	elseif what == "close" then
	end
	return true
end
function BU_TEXT:edit_dialog_param()
	local param_ref_or_type
	--param_ref_or_type = self:get_target_param()
	if not param_ref_or_type then
		param_ref_or_type = "string"
--		param.set_and_save( param_edit_ref, self:get_value(id), false )
	end
	ga:add_dialog_edit{ title="Edit "..self:get_dialog_name(), bu=self, param_type=param_ref_or_type, value_begin=self:get_text(), obj=self }
	return true
end
function BU_TEXT:do_click_double( x,y )
	--self:box_debug( "SLIDER:do_click_double()" )
	if self:edit_dialog_param() then
		self:end_uif( true, self..":do_click_double()" )
		return false
	end
	return true
end

function BU_TEXT:create( name, rect )
	local self = BU_TEXT:create_instance( name, rect )
	self:set_method_on_click_double( self, "do_click_double" )
	self:disable_mobile()
--	self.__cur_pos = {}
--	self.__init_pos = {}
--	self:set_ui_active( false )	return self
	return self
end




