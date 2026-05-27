if CLASS.DECLARE( "KEYBOARD", nil,
		{
			__bu_associate 	= nil,
			__clipboard		= "",
			ref = {},
		}
	) then
	KEYBOARD:set_class_status_doc(	CLASS.STATUS.GABU,
									"virtual keyboard, 2023 July need cleaning and test" )
end

--NUMPAD
local letters_numpad = {}
letters_numpad[1] = { "7", "8", "9", "-" }
letters_numpad[2] = { "4", "5", "6" }
letters_numpad[3] = { "1", "2", "3", "RETURN" }
letters_numpad[4] = { "0", ".", "BACKSPACE" }

--WINDOWS 8 KEYBOARD
local letters_keyboard_pc_azerty = {}
letters_keyboard_pc_azerty[1] = { "a", "z", "e", "r", "t", "y", "u", "i", "o", "p", "é", "BACKSPACE" }
letters_keyboard_pc_azerty[2] = { "q", "s", "d", "f", "g", "h", "j", "k", "l", "m", "ù", "RETURN" }
letters_keyboard_pc_azerty[3] = { "LSHIFT", "w", "x", "c", "v", "b", "n", "è", "à", ",", ".", "'", "RSHIFT" }
letters_keyboard_pc_azerty[4] = { "&123", "LCTRL", "LALT", "SPACE", "LEFT", "RIGHT", "TYPE" }

local letters_keyboard_pc_azerty_maj = {}
letters_keyboard_pc_azerty_maj[1] = { "A", "Z", "E", "R", "T", "Y", "U", "I", "O", "P", "É", "BACKSPACE" }
letters_keyboard_pc_azerty_maj[2] = { "Q", "S", "D", "F", "G", "H", "J", "K", "L", "M", "Ù", "RETURN" }
letters_keyboard_pc_azerty_maj[3] = { "LSHIFT", "W", "X", "C", "V", "B", "N", "È", "À", ";", ":", "?", "RSHIFT" }
letters_keyboard_pc_azerty_maj[4] = { "&123", "LCTRL", "LALT", "SPACE", "LEFT", "RIGHT", "TYPE" }

local letters_keyboard_pc_123 = {}
letters_keyboard_pc_123[1] = { "TAB", "!", "@", "#", "$", "€", "%", "&", "1", "2", "3", "BACKSPACE" }
letters_keyboard_pc_123[2] = { "", "(", ")", "-", "_", "~", "=", "+", "4", "5", "6" }
letters_keyboard_pc_123[3] = { "", "\\", ";", ":", "\"", "?", "*", "/", "7", "8", "9", "RETURN" }
letters_keyboard_pc_123[4] = { "&123", "LCTRL", "LALT", "<", ">", "SPACE", "0", ",", "&123" }

local letters_keyboard_pc_qwerty = {}
letters_keyboard_pc_qwerty[1] = { "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "BACKSPACE" }
letters_keyboard_pc_qwerty[2] = { "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "RETURN" }
letters_keyboard_pc_qwerty[3] = { "LSHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "RSHIFT" }
letters_keyboard_pc_qwerty[4] = { "&123", "LCTRL", "LALT", "SPACE", "LEFT", "RIGHT", "TYPE" }

local letters_keyboard_pc_qwerty_maj = {}
letters_keyboard_pc_qwerty_maj[1] = { "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "BACKSPACE" }
letters_keyboard_pc_qwerty_maj[2] = { "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "RETURN" }
letters_keyboard_pc_qwerty_maj[3] = { "LSHIFT", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "RSHIFT" }
letters_keyboard_pc_qwerty_maj[4] = { "&123", "LCTRL", "LALT", "SPACE", "LEFT", "RIGHT", "TYPE" }

local t_language = { "fr", "us" } --add a language for the keyboard here
--don't forget to give the traduction in the trad{} table under.

local trad = {}
trad.RETURN 	= { fr = "Entree", 		us = "Enter", 		tex = "" }
trad.BACKSPACE 	= { fr = "Ret. Arr",	us = "Backspace", 	tex = "" }
trad.RSHIFT 	= { fr = "Maj", 		us = "Shift", 		tex = "" }
trad.LSHIFT 	= { fr = "Maj", 		us = "Shift", 		tex = "" }
trad.LCTRL 		= { fr = "Ctrl", 		us = "Ctrl", 		tex = "" }
trad.LALT 		= { fr = "Alt", 		us = "Alt", 		tex = "" }
trad.SPACE 		= { fr = " ", 			us = " ", 			tex = "" }
trad.LEFT 		= { fr = "<-",			us = "<-",			tex = "" }
trad.RIGHT 		= { fr = "->", 			us = "->", 			tex = "" }
trad.TYPE 		= { fr = "A/Q",			us = "Q/A",			tex = "" }

local tab_map = {}
tab_map["Azerty"] = letters_keyboard_pc_azerty
tab_map["Qwerty"] = letters_keyboard_pc_qwerty

local tab_map_maj = {}
tab_map_maj["Azerty"] = letters_keyboard_pc_azerty_maj
tab_map_maj["Qwerty"] = letters_keyboard_pc_qwerty_maj


local function define_layout_pc_azerty( sx_w, sy_w )
	--Size layout 13*4
	local nb_elem_max_per_line 	= 13
	local nb_elem_max_per_col 	= 4

	--Offset between the element
	local offset_between_box = 0.05

	--Size of 1 line
	local sx_line = sx_w
	local sy_line = sy_w / (nb_elem_max_per_col + 1) --to keep a blanck at top of the window

	--Size unit of one element
	local sx_elem_unit = sx_line / nb_elem_max_per_line - offset_between_box
	local sy_elem_unit = sy_w / (nb_elem_max_per_col + 1)

	local tab_layout = {}

	for i = 1, nb_elem_max_per_col do
		tab_layout[i] = {}
	end


	--Where we begin to set the position
	local x_begin = 0
	local y_begin = sy_elem_unit * .5

	-- line 4
	for i = 1, 7 do

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		--space barre
		if i == 4 then
			my_box.sx = sx_elem_unit * 7 + offset_between_box * 6
		end

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[4][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	-- line 3
	for i = 1, 13 do
		if i == 1 then
			x_begin = 0
			y_begin = y_begin + sy_elem_unit + offset_between_box
		end

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[3][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	-- line 2
	for i = 1, 12 do
		if i == 1 then
			--Begin at left with an offset
			x_begin = sx_elem_unit * 0.3
			y_begin = y_begin + sy_elem_unit + offset_between_box
		end

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		--enter
		if i == 12 then
			my_box.sx = sx_elem_unit * 1.7 + offset_between_box
		end

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[2][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	-- line 1
	for i = 1, 12 do
		if i == 1 then
			--Begin at left with no offset
			x_begin = 0
			y_begin = y_begin + sy_elem_unit + offset_between_box
		end

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		--backspace
		if i == 12 then
			my_box.sx = sx_elem_unit * 2 + offset_between_box
		end

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[1][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	return tab_layout
end

-- Define the layout of qwerty
local function define_layout_pc_qwerty( sx_w, sy_w )
	--Size layout 12*4
	local nb_elem_max_per_line 	= 12
	local nb_elem_max_per_col 	= 4

	--Offset between the element
	local offset_between_box = 0.05

	--Size of 1 line
	local sx_line = sx_w
	local sy_line = sy_w / (nb_elem_max_per_col + 1) --to keep a blanck at top of the window

	--Size unit of one element
	local sx_elem_unit = sx_line / nb_elem_max_per_line - offset_between_box
	local sy_elem_unit = sy_w / (nb_elem_max_per_col + 1)

	local tab_layout = {}

	for i = 1, nb_elem_max_per_col do
		tab_layout[i] = {}
	end

	--Where we begin to set the position
	local x_begin = 0
	local y_begin = sy_elem_unit * .5

	-- line 4
	for i = 1, 7 do

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		--space barre
		if i == 4 then
			my_box.sx = sx_elem_unit * 6 + offset_between_box * 5
		end

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[4][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	-- line 3
	for i = 1, 12 do
		if i == 1 then
			x_begin = 0
			y_begin = y_begin + sy_elem_unit + offset_between_box
		end

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[3][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	-- line 2
	for i = 1, 11 do
		if i == 1 then
			--Begin at left with an offset
			x_begin = sx_elem_unit * 0.3
			y_begin = y_begin + sy_elem_unit + offset_between_box
		end

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		--enter
		if i == 11 then
			my_box.sx = sx_elem_unit * 1.7 + offset_between_box
		end

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[2][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	-- line 1
	for i = 1, 11 do
		if i == 1 then
			--Begin at left with no offset
			x_begin = 0
			y_begin = y_begin + sy_elem_unit + offset_between_box
		end

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		--backspace
		if i == 11 then
			my_box.sx = sx_elem_unit * 2 + offset_between_box
		end

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[1][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	return tab_layout
end

-- Define the layout of touch "&123"
local function define_layout_pc_123( sx_w, sy_w )
	--Size layout 13*4
	local nb_elem_max_per_line 	= 13
	local nb_elem_max_per_col 	= 4

	--Offset between the element
	local offset_between_box = 0.05

	--Size of 1 line
	local sx_line = sx_w
	local sy_line = sy_w / (nb_elem_max_per_col + 1) --to keep a blanck at top of the window

	--Size unit of one element
	local sx_elem_unit = sx_line / nb_elem_max_per_line - offset_between_box
	local sy_elem_unit = sy_w / (nb_elem_max_per_col + 1)

	local tab_layout = {}

	for i = 1, nb_elem_max_per_col do
		tab_layout[i] = {}
	end

	--Where we begin to set the position
	local x_begin = 0
	local y_begin = sy_elem_unit * .5

	-- line 4
	for i = 1, 9 do

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		--space barre
		if i == 6 then
			my_box.sx = sx_elem_unit * 3 + offset_between_box * 2
		end

		-- 0 numpad with space before
		if i == 7 then
			my_box.sx = sx_elem_unit * 2 + offset_between_box * 1
			x_begin = x_begin + sx_elem_unit * .5
		end

		-- &123 with space before
		if i == 9 then
			x_begin = x_begin + sx_elem_unit * .5
		end

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[4][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	-- line 3
	for i = 1, 12 do
		if i == 1 then
			x_begin = 0
			y_begin = y_begin + sy_elem_unit + offset_between_box
		end

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		-- 7 numpad with space before
		if i == 9 then
			x_begin = x_begin + sx_elem_unit * .5
		end

		-- Enter with space before
		if i == 12 then
			my_box.sy = sy_elem_unit * 2 + offset_between_box
			x_begin = x_begin + sx_elem_unit * .5
		end

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		if i == 12 then
			my_box.y = y_begin + 0.225
		end

		tab_layout[3][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	-- line 2
	for i = 1, 11 do
		if i == 1 then
			x_begin = 0
			y_begin = y_begin + sy_elem_unit + offset_between_box
		end

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		-- 4 numpad with space before
		if i == 9 then
			x_begin = x_begin + sx_elem_unit * .5
		end

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[2][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	-- line 1
	for i = 1, 12 do
		if i == 1 then
			--Begin at left with no offset
			x_begin = 0
			y_begin = y_begin + sy_elem_unit + offset_between_box
		end

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		-- 1 numpad or backspace with space before
		if i == 12 or i == 9 then
			x_begin = x_begin + sx_elem_unit * .5
		end

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[1][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	return tab_layout
end





local function define_layout_numpad( sx_w, sy_w )
	--Size layout 4*4
	local nb_elem_max_per_line  = 4
	local nb_elem_max_per_col 	= 4

	--Offset between the element
	local offset_between_box = 0.05

	--Size of 1 line
	local sx_line = sx_w
	local sy_line = sy_w / (nb_elem_max_per_col + 1) --to keep a blanck at top of the window

	--Size unit of one element
	local sx_elem_unit = sx_line / nb_elem_max_per_line - offset_between_box
	local sy_elem_unit = sy_w / (nb_elem_max_per_col + 1)

	local tab_layout = {}

	for i = 1, nb_elem_max_per_col do
		tab_layout[i] = {}
	end

	--Where we begin to set the position
	local x_begin = 0
	local y_begin = sy_elem_unit * .5

	--line 4
	for i = 1, 3 do
		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[4][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	-- line 3
	for i = 1, 4 do
		if i == 1 then
			x_begin = 0
			y_begin = y_begin + sy_elem_unit + offset_between_box
		end

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		-- RETURN
		if i == 4 then
			my_box.sy = sy_elem_unit * 2 + offset_between_box
			my_box.y = y_begin - (sy_elem_unit + offset_between_box) * .5
		end

		tab_layout[3][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	-- line 2
	for i = 1, 3 do
		if i == 1 then
			x_begin = 0
			y_begin = y_begin + sy_elem_unit + offset_between_box
		end

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		tab_layout[2][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	-- line 1
	for i = 1, 4 do
		if i == 1 then
			x_begin = 0
			y_begin = y_begin + sy_elem_unit + offset_between_box
		end

		local my_box = {}
		my_box.sx = sx_elem_unit
		my_box.sy = sy_elem_unit

		my_box.x = x_begin + my_box.sx * .5
		my_box.y = y_begin

		-- '-'
		if i == 4 then
			my_box.sy = sy_elem_unit * 2 + offset_between_box
			my_box.y = y_begin - (sy_elem_unit + offset_between_box) * .5
		end

		tab_layout[1][i] = my_box

		x_begin = x_begin + my_box.sx + offset_between_box
	end

	return tab_layout
end

function KEYBOARD:set_button_close_active( b_active )	self.__button_close:set_active( b_active ) 		end

function KEYBOARD:add_text_to_clipboard( str )	self.__clipboard = str 		end
function KEYBOARD:get_text_from_clipboard()		return self.__clipboard 	end

function KEYBOARD:set_keyboard_cur( str )
	self:print( "keyboard cur set to "..str )
	self.__keyboard_cur_name = str
	self.__keyboard_cur = self.__keyboards[str]
	self:define_mapping_layout()
end
function KEYBOARD:get_keyboard_cur_name()	return self.__keyboard_cur_name	end
function KEYBOARD:get_keyboard_cur()
	self:print( "keyboard cur is "..self:get_keyboard_cur_name() )
	local keyboard_cur = self.__keyboard_cur
	--table.print( keyboard_cur, "keyboard_cur", 2 ) 
	return keyboard_cur
end

function KEYBOARD:get_layout()			return self:get_keyboard_cur().layout	end

function KEYBOARD:set_key_map( map )	self:get_keyboard_cur().key_map = map 	end
function KEYBOARD:get_key_map()			return self:get_keyboard_cur().key_map	end

function KEYBOARD:set_bus( bus )		self:get_keyboard_cur().bus = bus 		end
function KEYBOARD:get_bus()				return self:get_keyboard_cur().bus		end

function KEYBOARD:is_shift_locked() 	return self.__shift == "locked"								end
function KEYBOARD:is_shift()			return self.__shift	== "true" or self.__shift == "locked"	end
function KEYBOARD:set_shift( str )
	local keyboard_cur_name	= self:get_keyboard_cur_name()

	self:print( keyboard_cur_name )

	if str ~= "locked" then
		if self:is_shift() then
			self:set_key_map( tab_map[keyboard_cur_name] )
			self:define_mapping_layout()
		else
			self:set_key_map( tab_map_maj[keyboard_cur_name] )
			self:define_mapping_layout()
		end
	end

	if str ~= "false" then
		self:set_alteration("SHIFT")
	else
		self:set_alteration(nil)
	end

	self.__shift = str
end

function KEYBOARD:is_123()			return self:get_keyboard_cur_name()	== "&123"	end
function KEYBOARD:set_123( b )
	local name_cur	= self:get_keyboard_cur_name()

	local name_next
	if name_cur == "&123" then
		if self.keyboard_prev_name then
			name_next = self.keyboard_prev_name
		else
			name_next = "Azerty"
		end
	else
		self.keyboard_prev_name = name_cur
		name_next = "&123"
	end
	self:set_keyboard_cur( name_next )

	--self:define_mapping_layout()
end

function KEYBOARD:switch_type()
	local name_cur	= self:get_keyboard_cur_name()

	for k, v in pairs(self.__keyboards) do
		if k ~= name_cur and k ~= "&123" then
			self:set_keyboard_cur(k)
			--self:define_mapping_layout()
			return
		end
	end
end

function KEYBOARD:set_alteration( type ) 	self.__alteration = type 	end
function KEYBOARD:get_alteration()			return self.__alteration	end

function KEYBOARD:get_keyboard_type()
	return self.__keyboard_type
end

function KEYBOARD:attach_bu()
	local bu = app:__get_edit_bu_active( self )
	if bu then
		-- Remove the previous attached bu
		self:remove_attach_bu()
		self:show( bu, "attach with bu : " )
		self.__bu_associate = bu
	else
		self:show( "no bu to attach" )
	end
end

function KEYBOARD:remove_attach_bu()
	self.__bu_associate = nil
end

function KEYBOARD:get_attach_bu()
	return self.__bu_associate
end

function KEYBOARD:define_keyboard_numpad( sx_window, sy_window )
	local keyboard = {}
	keyboard.layout = define_layout_numpad( sx_window, sy_window )
	keyboard.bus  = self:define_bus_layout( "Numpad", keyboard.layout, sx_window, sy_window )
	keyboard.key_map = letters_numpad

	self.__keyboards["Numpad"] = keyboard
	self:set_keyboard_cur( "Numpad" )
--	self:define_mapping_layout()

	return keyboard.bus
end

function KEYBOARD:define_keyboard_pc( sx_window , sy_window )
	local keyboards = self.__keyboards
	keyboards["Azerty"] = {}
	keyboards["Qwerty"] = {}
	keyboards["&123"]   = {}

	--Need to define 3 different layout
	--First for azerty
	keyboards["Azerty"].layout = define_layout_pc_azerty( sx_window, sy_window )
	--Second for qwerty
	keyboards["Qwerty"].layout = define_layout_pc_qwerty( sx_window, sy_window )
	--Third for &123 touch
	keyboards["&123"].layout = define_layout_pc_123( sx_window, sy_window )

	--Need to define 3 different bus for the window
	for k,v in pairs(keyboards) do
		v.bus = self:define_bus_layout( k, v.layout, sx_window, sy_window )
	end

	--Need to stock 2 map of key
	keyboards["Azerty"].key_map = letters_keyboard_pc_azerty
	keyboards["Qwerty"].key_map = letters_keyboard_pc_qwerty
	keyboards["&123"].key_map = letters_keyboard_pc_123

	self:set_keyboard_cur( "Qwerty" )
	
	--Define default mapping in fr
	--self:define_mapping_layout()

	return self:get_keyboard_cur().bus
end

function KEYBOARD:define_bus_layout( name, layout, sx_w, sy_w )
	local nb_line = #layout
	local bu
	local bus = BUS:create( "BUS key "..name )
	bus:set_active( true )

	bus:init_begin()

	--Create BU_KEY following layout
	for i = 1, nb_line do
		local tbox = layout[i]
		for j = 1, #tbox do
			local box = tbox[j]
			local x,y, sx,sy = box.x,box.y, box.sx,box.sy
			bu = bus:create_add_bu(	BU_KEY, i.." "..j,	{x,y, sx,sy},	1, self )
		end
	end

	--Create the language button
	--todo we reuse but it is a problem for set_up
	if not self.__button_language then
		local sx, sy = .2, .1
		bu = bus:add_trig( "us", {0 + sx,sy_w - sy, sx,sy} )-- changed 15-03-2017
		bu:set_text_inside( true )
		bu:set_method_on_click( self, "switch_language" )
		self.__button_language = bu
	else
		bus:add_bu(self.__button_language)
	end

	--Create the menu
	--if not self.__button_menu then
		local sx, sy = .4, .12
		bu = bus:add_button( "menu", {sx_w * .5,sy_w - .1, sx,sy} )
		bu:set_multiple( {"No target", "Real as target"}, "options_menu" )
		bu:set_method_on_click( self, "set_keyboard_target" )
	--	self.__button_menu = bu
	--else
	--	bus:add_bu(self.__button_menu)
	--end

	--Create the close button
	--todo we reuse but it is a problem for set_up
	if not self.__button_close then
		local sx, sy = .1, .1
		bu = bus:add_trig( "Close", {sx_w - sx,sy_w - sy, sx,sy} )-- changed 15-03-2017
		--bu:set_active( false )
		bu:set_method_on_click( self, "close" )
		self.__button_close = bu
	else
		bus:add_bu(self.__button_close)
	end

	bus:init_end()

	return bus
end

function KEYBOARD:set_keyboard_target( menu )
	local value = menu:get_item_data()
	if value == "Real as target" then
		GA:set_keyboard_physical_target_virtual( self )
	elseif value == "No target" then
		GA:set_keyboard_physical_target_virtual( nil )
	end
end

function KEYBOARD:switch_language()
	local nb_switch = #t_language -- number of language avaible
	local button_language = self.__button_language
	local language_current = button_language:get_text() -- the current language of the keyboard

	for i = 0, nb_switch do
		local language = t_language[i]
		if language_current == language then
			if i == nb_switch then
				self:define_mapping_layout( t_language[1] )
			else
				self:define_mapping_layout( t_language[i+1] )
			end
		end
	end
end

function KEYBOARD:close()
--	self:remove_attach_bu()
--	self.bu_window:remove()
	self.bu_window:set_active( false )
	self.b_is_open = false
end

function KEYBOARD:open()
	self.bu_window:set_active( true )
	self.b_is_open = true
end

-- Add the key to the layout
function KEYBOARD:define_mapping_layout( language )
	-- Next few line Maa 2024
	aaa.print_fn()
--	self:print( "keyboard is "..self:get_keyboard_cur_name() )
--	self:set_keyboard_cur( language=="fr" and "Azerty" or "Qwerty" )

	-- Map of key
	local map 		= self:get_key_map()
	local layout 	= self:get_layout()
	local bus 		= self:get_bus()
	local nb_line 	= #layout
	local cpt 		= 1
	local bu -- added 15-03-2017

	local button_language = self.__button_language

	--Change keyboard language if needed
	if language then
		button_language:set_text( language )
	end

	--Get the language of the keyboard
	local language = button_language:get_text()

	--For all line in layout
	for i = 1, nb_line do
		local tbox = layout[i]
		for j = 1, #tbox do
			local str = map[i][j]

			--Trad str if needed
			if trad[str] then
				str = trad[str][language]
			end

			--Get the BU_KEY
			bu = bus:get_bu( cpt )
			--Set value and text
			bu:set_value_symbo( map[i][j] )
			bu:set_text( str )

			--Specific draw of BU_KEY
			if map[i][j] == "LSHIFT" or map[i][j] == "RSHIFT" then
				bu.b_draw_shift = true
			end

			if map[i][j] == "&123" then
				bu.b_draw_123 = true
			end

			cpt = cpt + 1
		end
	end

	--Add the new bus define to the window
	local bu_window = self.bu_window
	if bu_window then
		--Don't forget to delete the old one
		bu_window:remove_bus_down()
		--Use the size of the window stocked previously
		bus:set_transfo( self.sx_window*2, self.sy_window*2, self.sx_window, self.sy_window )	-- here we set the coor system
		--Add bus down
		self.bu_window:transform_in_window( bus )
	end
end

function KEYBOARD:draw_link()
	--self:print("draw")

	local bu = self:get_attach_bu()
	if bu then
		local bus = bu:get_bus_up()
		local bu_window = self.bu_window
		local x_bu, y_bu = bu:convert_xy_local_to_top(-.45, .2)
		local x_bus, y_bus = bu_window:get_xy()
		local sx, sy = bu_window:get_sxy()

		gol.color_orange()

		if y_bus > y_bu then
			aaa.draw_line( x_bus, y_bus - sy * .5, x_bu, y_bu )
		else
			aaa.draw_line( x_bus, y_bus + sy * .5, x_bu, y_bu )
		end
	end
end

function KEYBOARD:init( type )
	self.__keyboard_type		= type
	self.__keyboards			= {}
	self.__keyboard_cur_name	= ""
	self.__t_event				= {}
	self.__shift				= "false"

	local bu

	local name = self:get_name() 
	local bus = BUS:create( "KEYBOARD_"..name )
	bus:set_active( true )
	bus:init_begin_add_to_ga()

		-- If you need an other type of keyboard, define it here
		if type == "NumPad" then
			bu = bus:add_window( "Keyboard "..name,  self:define_keyboard_numpad( .85*2, 1*2 ),	{0,0, 0.85,1} )
			-- Have to sconttock size for later use
			self.sx_window = 0.85
			self.sy_window = 1
		elseif type == "PC" then
			bu = bus:add_window( "Keyboard "..name,  self:define_keyboard_pc( 2.8*2, 1*2 ),		{0,0, 2.8, 1} )
			-- Have to stock size for later use
			self.sx_window = 2.8
			self.sy_window = 1
		end

		bu:set_method_on_click( self, "attach_bu" )

		-- Have to stock for later use
		self.bu_window = bu

		bus:set_method( "draw_before",	self,	"draw_link"	)

	bus:init_end()
end

function KEYBOARD:create( type, id )
	local self = KEYBOARD:create_instance( type.."_"..id )

	--todo this dirty

	self:init( type )
--	KEYBOARD.id = id + 1

	return self
end

