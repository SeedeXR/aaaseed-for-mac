
if CLASS.DECLARE( "BU_DOC", BU_WINDOW_LIST, {} ) then
	BU_DOC:set_class_status_doc(	CLASS.STATUS.GABU,
									"Window to navigate table and GABU_OBJs, for documentation and debug" )
end

--	local function_to_obj.doc.func_regex = "(\nfunction)( +)(\w+)(?:.|:)(\w+)(\(.*\)(.*end)?)"
--	local function_to_obj.doc.func_replace = "\n\3.doc.\4"
--	local delete_function_body_regex = "((^\t.*\n)|^\n|^end\n|^-.*\n|^].*\n)"

BU_DOC.doc.__get_bu_root_rect = "( id ) return position of bu root used to hold table"
function BU_DOC:__get_bu_root_rect( id )
	local hmar = .1
	local DX,DY = 8/6,.4
	local SX,SY = DX-hmar, DY-hmar
	return {-4+DX/2,2-id*DY, SX,SY}
end
BU_DOC.doc.create_bu_root = "( id, name, tab, doc ) create a bu root holding a table and doc"
function BU_DOC:create_bu_root( id, name, tab, doc )
	local bus = self:get_bus_down()
	local bu = bus:add_trig( tostring(name), self:__get_bu_root_rect(id), 1 )
		bu:set_text_nice()
		bu:set_method_on_click( self, "open_table", name, tab, doc )
		bu:set_method_on_click_double( self, "delete_table_by_bu", bu )
	return bu
end
BU_DOC.doc.__update_bu_root_positions = "() recompute the positions after insertion or deletion of bu_root"
function BU_DOC:__update_bu_root_positions()
	local bu_paths = self.bu_paths
	for id, bu in ipairs(self.bu_paths) do
		local r = self:__get_bu_root_rect(id)
		bu:set_xy_sxy( r[1],r[2],  r[3],r[4] )
	end
end

BU_DOC.doc.create =	"( name, rect ) Constructor, create a Bu_Doc window, calls define_ui."
function BU_DOC:create( name, rect )
	local self = BU_DOC:create_instance( name, rect )

	self.id_cur = 0

	local bus_down = self:define_ui()
	bus_down:set_transfo( 8 )
	self:transform_in_window( bus_down )

	self:__set_window_state( "mini" )
	self:set_text_factor( .05 )

--todo deal better with doc here or in open_table	
	self:open_table( "Global", _G )
	self.bu_paths[1].__b_keep = true

	local t = _G.CLASS.__classes_by_name
	self:open_table( "GaBu Classes", t )
	self.bu_paths[2].__b_keep = true

	return self
end



BU_DOC.doc.__draw_doc_window =	"( bu ) until text rendering is corrected (2024 Sep)\n"..
								"this method is call by the BU_TEXT in the separate doc windows (click long)"
function BU_DOC:__draw_doc_window( bu )
	--if true then return end
	local str = bu:get_text()
	--self:print( "str is "..str )
	gol.color_white()
	--bu:draw_text( str, 0,0, 1,1 )
	local str,nb = string.gsub( str, "\n", "\n  ")
	nb = 1/(nb+1)
	--math.min( 1/6, 1/(nb+1) )
	bu:draw_text_nice( str, -.5,.5-nb,0, .022,nb, "left", "left" )
end


BU_DOC.doc.define_ui =	"() Defines ui in window, inits table exploration vars, assign_table,\n"..
						"and search (with editable bu_text, cf search_in_table method)."
function BU_DOC:define_ui()
	local bu

	self.tables = {}
	self.bu_paths = {}	-- store the level we go thru

	local bus = BUS:create( self:get_name() )
	bus:init_begin()

	-- bu_list with the scrolable table content
		local SX = 5*8/6
		bu = self:create_bu_list( bus, {(8-SX)/2,-.5,  SX,5.5} )
		bu:set_nb_xy( 4,14 )
		bu:adjust_nb( 256 )

	-- Display Doc at the top of the window
		local SY = 1.75
		bu = self:create_bu_info( bus, {0,4-SY/2, 8,SY} )

	-- buttons on the left to choose tables	
	-- Now a selector on top location of the bu_root
		local rect = self:__get_bu_root_rect(0)
		bu = bus:add_selector( "Show", rect )
			bu:set_item_text( 1, "Private", "All", "Public" )
			:set_target_lua( self, "s_private", 2 )
			:set_method_on_value_change( self, "assign_table", nil )	-- we need to put nil here or bu will be received by assign_table

	-- Search
		self:create_bu_search( bus, {-3.2,-3.6, 6,.5}, "BU", true, self, "add_table" )
	-- Add an exclude mecanism

	bus:init_end()

	return bus
end


-- next two fns only used by third
-- BU_DOC.doc.make_print_name = "( key, value ) Format string from variable name."
-- function BU_DOC:make_print_name( key, value )
-- 	local name
-- 	if type(key) == 'number' and type(value) == 'table' then
-- 		name = aaa.__build_str( value )
-- 	else
-- 		name = tostring( key )
-- 	end
-- 	return name
-- end
-- BU_DOC.doc.get_max_text_len = "( table ) Gets the max length over variable names in table."
-- function BU_DOC:get_max_text_len( tt )
-- 	local max_len = 0
-- 	local len = 0

-- 	for k, v in pairs(tt) do
-- 		len = #(self:make_print_name( k, v ) )
-- 		if len > max_len then
-- 			max_len = len
-- 		end
-- 	end
-- 	return max_len
-- end

--toto change it to never erase and so recreate elt: reuse"
BU_DOC.__color_var	= { 1,1,1, .125}
BU_DOC.__color_spe	= { 1,0,0, .25}
BU_DOC.__white		= { 1,1,1, 1}
BU_DOC.__type_info =
{
	type_class		=  { color_back = { 0, .8, 0, 	1 },	color_text = BU_DOC.__white,		short="gabu", 	order=8, b_order_conversion=true },
	type_gabu_obj	=  { color_back = { 0, .6, 0, 	1 },	color_text = BU_DOC.__white,		short="gabu", 	order=1, b_order_conversion=true },
	type_table		=  { color_back = { 0, .4, 0,	1 },	color_text = BU_DOC.__white,		short="tab"	, 	order=2, b_order_conversion=true },
	type_string		=  { color_back = BU_DOC.__color_var,	color_text = { 1, 1, 0, 	1 },	short="str" , 	order=4 },
	type_number		=  { color_back = BU_DOC.__color_var,	color_text = { 0, 1, 1, 	1 },	short="num" , 	order=4 },
	type_boolean	=  { color_back = BU_DOC.__color_var,	color_text = { 0,1,1, 		1 },	short="b"	, 	order=4 },
	type_nil		=  { color_back = {1.0,0, 1},			color_text = { 1,1,1,		1 },	short="nil" , 	order=5 },
	type_function	=  { color_back = {1,.5,0, .5 }, 		color_text = BU_DOC.__white,		short="fn"	, 	order=3 },
	type_userdata	=  { color_back = BU_DOC.__color_spe,	color_text = { 0, .75, 0, 	1 },	short="ud"	, 	order=6 },
	type_thread		=  { color_back = BU_DOC.__color_spe, 	color_text = { 0, .75, 0, 	1 },	short="th"	, 	order=7 },
}
function BU_DOC:get_type_info( str_type )
	return self.__type_info["type_"..str_type]
end


BU_DOC.doc.__sort_for_doc =	"( table, key_a,key_b ) complex sorting to establish the order in BU_LIST"
--todo add an option to change sort order
function BU_DOC.__sort_for_doc( t, key_a,key_b ) -- t not used
	if key_a=="doc" then
		return true
	elseif key_b=="doc" then
		return false
	end

	--aaa.print( key_a.."   "..key_b )
	local value_a = t[key_a]
	local kind_a = type( value_a )
	if kind_a == "table" then
		if value_a~=_G then
			local gabu = value_a[CLASS.__str_class_field]
			if gabu then
				kind_a = CLASS.is_class(value_a) and "class" or "gabu_obj"
			end
		end
	end
	local info_a = BU_DOC:get_type_info( kind_a )
	local order_a = info_a.order

	local value_b = t[key_b]
	local kind_b = type( value_b )
	if kind_b == "table" then
		if value_b~=_G then
			local gabu = value_b[CLASS.__str_class_field]
			if gabu then
				kind_b = CLASS.is_class(value_b) and "class" or "gabu_obj"
			end
		end
	end
	local info_b = BU_DOC:get_type_info( kind_b )
	local order_b = info_b.order
	--aaa.print( key_a.." "..kind_a.." "..order_a.."\t"..key_b.." "..kind_b.." "..order_b )

	if order_a == order_b then
		--aaa.print( type(key_)a.." "..kind_b )
		if type(key_a)=="number" and type(key_b)=="number" then
			return key_a < key_b
		end
		if info_a.b_order_conversion then key_a = aaa.__build_str_for_doc(key_a, true) end
		if info_b.b_order_conversion then key_b = aaa.__build_str_for_doc(key_b, true) end
		--aaa.print( "same order : "..key_a.." < "..key_b.." is "..(key_a < key_b) )
		-- we could also use uppercase so underscore (_) is always after letters
		key_a = string.lower(key_a)
		if string.sub( key_a, 1,2 )=="__" then key_a = string.sub( key_a, 3 ) end
		key_b = string.lower(key_b)
		if string.sub( key_b, 1,2 )=="__" then key_b = string.sub( key_b, 3 ) end
		return key_a < key_b
	end
	--aaa.print( order_a.." < "..order_b.." is "..(order_a < order_b) )
	return order_a < order_b
end

function BU_DOC:draw_item( bu )
	local t = bu.__item_info
	local kind = t.kind
	local type_info = self:get_type_info(kind)
	local value = t.tab[t.key]
	local color = type_info.color_back
	if value == nil then
		--table.print( t, "BU_DOC:draw_item() table "..bu.__item_info, 2 )
		--self:print_error( "we got a nil value" )
	end
	if kind == "table" and (value==nil or table.is_empty(value)) then
		color = {0,.25,0, 1}
	end
	bu:set_color_back( color )
	if kind == "boolean" then
		bu:set_text_color( value and { 0,1,0, 1} or {1,0,0, 1} )
	else
		bu:set_text_color( type_info.color_text )
	end
	BUTTON.draw(bu)
end
function BU_DOC:do_item_click_double( bu )
	--aaa.print_fn()
	local t = bu.__item_info
	local value =  t.tab[t.key]
	if type(value)=="table" then
		self:open_table( t.key, value, t.doc_target )
	end
end
function BU_DOC:do_item_click_long( bu )
	--aaa.print_method( "do_item_click_long" )
	local t = bu.__item_info
	local key = t.key
	local doc = self:__get_doc( t.tab[key], key )
	if not doc then
		doc = t.doc_target
	--aaa.print_fn( "do_item_click_long" )
	--aaa.print_method( "do_item_click_long" )
	--table.print( doc_target, "doc_target "..key )
		doc = doc and doc[key]
	end
	if doc then
		self:open_doc_window( key, t.tab[key], doc )
	end
	return true
end


BU_DOC.doc.assign_table = "( table ) make the current table displayed"
function BU_DOC:assign_table( tab )

	if tab == nil then
		tab = self.tables[self.id_cur] 
	end

	local ui = self.ui
	local nb = table.count_item( tab )
	--self:print( "nb is "..nb )
	--table.print( tab, "will parse ", 1 )

	local bu_list = ui.bu_list
	bu_list:adjust_nb( nb )

	local doc_tab
	if tab == _G then
		doc_tab = aaa.lua.global.get("doc")
	else
		doc_tab = tab.doc
	end
	local i = 0
	local b_private_test_private	= self.s_private==3
	local b_private_test			= self.s_private ~=2
	local str_search				= self.b_use_search and ui.bu_search_text:get_text()
	for key, value in pairs_sorted( tab, self.__sort_for_doc ) do
		--name = self:make_print_name( name, value )

		local doc = doc_tab
		local kind = type(value)
		local b_used = true
		if type(key) == "string" then
			if b_private_test then
				--self:print( "private test: key "..key.." -> "..(string.sub( key, 1,2 )=="__") )
				b_used = (string.sub( key, 1,2 )=="__")~=b_private_test_private
			end
			if b_used and str_search and (string.find( key, str_search )==nil) then
				b_used = false
			end
		end

		if b_used then
			local b_table = kind == "table"
			if b_table then
				if value~=_G then
					if value[CLASS.__str_class_field] then
						kind = CLASS.is_class(value) and "class" or "gabu_obj"
					end
					doc = value.doc
				end
			end

			i = i + 1
			local bu = bu_list:get_bu(i)
			bu:set_text( (type(key)=="string") and key or aaa.__build_str_for_doc(key, true) )
			bu.__item_info = { tab=tab, key=key, kind=kind, b_table=b_table, doc_target=doc }
	
			bu:set_method_on_click()
			bu:set_method_on_click_double( self, "do_item_click_double", bu )
			bu:set_method_on_click_long( self, "do_item_click_long", bu )
			--bu:set_method_on_click( self, "do_item_click_long", bu )
	
			bu:set_visible( true )
	
			bu.draw = function(bu)	self:draw_item( bu )		end	--indirection thru a function so we can update on the fly	
		end
	end

	bu_list:adjust_nb( i )
end

function BU_DOC:__get_doc( var, name )
	local doc
	if CLASS.is_gabu_obj( var ) then
		doc = var:get_class_doc()
	else
		doc = self.doc_cur
	--	self:print( "doc cur is "..doc )
		doc = doc and doc[name]
		if doc and type(doc)=="table" then
			local str = doc[1]
			local i = 2
			while true do
				local add = doc[i]
				if not add then
					break
				end
				str = str.."\n"..add
				i = i + 1
			end
			return str
		end
	end
	return doc
end

--todo redo this comment
BU_DOC.doc.display_item_info = "( bu ) Updates the bu_info at top of bu list from the bu which the mouse is over."
function BU_DOC:display_item_info( bu )
	local str = ""
	if bu then
		local t = bu.__item_info
		if t~=nil then
			local var_name, var = bu:get_text(), t.tab[t.key]
			str = var_name.."        is "..aaa.__build_str_for_doc(var)
			local doc = self:__get_doc( var, var_name )
			if doc then
				str = str.."\n"
				if type(var)=="function" then
					str = str..var_name
				end
				str = str..doc
			end
		end
	else
		str = ""
	end
	self.ui.bu_info:set_text( str )
end


BU_DOC.doc.add_doc_window =	"( var_name, var, doc, bus_down, line_nb, ry ) Creates a window to display documentation text."
function BU_DOC:add_doc_window( var_name, var, doc, bus_down, line_nb, ry )
	--aaa.print_fn()
	local bus = BUS:create( var_name )
	bus:init_begin()

		local sy = 4
		if doc and type(doc) == "string" then
			if true then
				--line_nb = line_nb + .5
				--local sy = 1/(line_nb+1.5)
				--local bu = bus:add_text( "One Line", {0,0.5-(1)*sy, 1,sy} )

				local bu = bus:add_text( "One Line", {0,0, 1,1-.5/line_nb} )

				bu:set_text( doc )
				bu:set_text_nice()
				-- local bu = bus:add_text( "two Line", {0,0, 1,1/line_nb} )
				-- bu:set_text( local_doc )
				--bu:set_text_nice()
				bu:set_ui_active( false )
				bu.draw = function(bu)	self:__draw_doc_window( bu )	end
			else
				-- local cursor_start = 1
				-- local cursor_end = 81
				-- local sx = 7
				-- sy = 7 / nb_line
				-- local y = 4 - sy * 1.5
				-- local x = 0
				-- for i=1, nb_lines do
				-- 	if cursor_end > #local_doc then
				-- 		cursor_end = #local_doc
				-- 		sx = ( #local_doc - cursor_start ) / 11.4
				-- 		x = -3.5 + sx * .5
				-- 	end
				-- 	bu = bus:add_text( string.sub( local_doc, cursor_start, cursor_end ), {x,y, sx,sy} )
				-- 	bu:set_text_nice()
				-- 	y = y - sy
				-- 	cursor_start = cursor_start + 81
				-- 	cursor_end = cursor_end + 81
				-- end
			end
			
			local sx = .04
			local sy = sx / ry
			local bu = bus:add_trig( "X", {.5-sx,.5-sy, sx,sy}  )
			bu:set_text_inside( true )
			bu:set_dplane(42)
			bu:set_color_back( {0,0,0, 0} )
			bu:set_method_on_click( self, "close_doc_window", bu )
		end

	bus:init_end()

	return bus
end

BU_DOC.doc.open_doc_window = "( name, var, doc ) Calls add_doc_window with doc string associated to the bu on which a click was made."
function BU_DOC:open_doc_window( name, var, doc )
	--aaa.print_fn()
	
	if doc then
		local doc_type = type(doc)
		if doc_type == "string" then
			local bus_up = self:get_bus_up()
			if type(var)=="function" then
				doc = name..doc
			end
			local doc,nb = string.gsub( doc, "\n", "\n  " )
			nb = nb + 1
			local sx = 4
			local sy = (nb+1) * sx/32
			--aaa.print_fn()
			local win = self:add_doc_window( name, var, doc, bus_up, nb, sy/sx )
			local bu = bus_up:add_window( name, win, {0,0, sx,sy} )
			bu:get_bus_down():set_transfo( 1 )
		else
			self:print_error( "doc is not a string but a "..doc_type )
		end
	end
end

BU_DOC.doc.close_doc_window = "( bu ) Close the window displaying documentation text."
function BU_DOC:close_doc_window( bu )
	local bus_up = bu:get_bus_up()
	local bu = bus_up:get_up()
	bus_up = bu:get_bus_up()
	--self:print( "self.doc_win_bu : "..self.doc_win_bu )
	--self:print( "self:get_bus_down() : "..self:get_bus_down() )
	bus_up:remove_bu( bu )
end


BU_DOC.doc.open_table =	"( name, tab, doc ) creates the corresponding new BU on the left and\n"..
						"calls assign_table to display table variables."
function BU_DOC:open_table( name, tab, doc )
	local tables = self.tables

	-- do we have already this table ?
	local id_next = array.find_index_by_val( tables, tab )
	if not id_next then	
		id_next = self.id_cur + 1
		table.insert( self.bu_paths, id_next, self:create_bu_root( id_next, name, tab, doc ) )
		table.insert( tables, id_next, tab )
	end

	if self.id_cur ~= id_next then
		self.id_cur = id_next
		self:__update_bu_root_positions()
	end
	self.doc_cur = doc
	self:assign_table( tab )
end

function BU_DOC:delete_table_by_bu( bu )
	if bu.__b_keep then
		self:print( "this bu is set to stay" )
	else
		local id = array.find_index_by_val( self.bu_paths, bu )
		if id then
			self:get_bus_down():remove_bu( bu ):free()
			table.remove( self.bu_paths, id )
			table.remove( self.tables, id )
			self:__update_bu_root_positions()
			local nb = #(self.bu_paths) 
			if self.id_cur > nb then
				self.id_cur = nb
				self:assign_table()
			end
		end
	end
end


--[[
function BU_DOC:add_keyboard()
	local function __add_keyboard( name, id )
		local keyboard =  KEYBOARD:create( name, id )
		return keyboard
	end
	self.keyboard1 = __add_keyboard( "PC", 1 )
	self.keyboard1:set_button_close_active( true )
	table.print( self.keyboard1, "self.keyboard1" )
end--]]


BU_DOC.doc.add_table = "() Search in current table (case sensitive).\n"..
						" Opens a table of element containing the string from the bu_text editable."
function BU_DOC:add_table()
	local str = self.ui.bu_search_text:get_text()
	if str then
		local selected = {}
		local tab = self.tables[self.id_cur]
		local i = 0
		for key in pairs( tab ) do
			--print(k)
			if key:find( str ) then
				--print( str, k:find( str ), k )
				selected[key] = tab[key]
				i = i + 1
			end
		end
		if i > 0 then
			local doc
			if tab == _G then
				doc = aaa.lua.global.get( "doc" )
			else
				doc = tab.doc
			end
			selected.doc = doc
			self:open_table( "Sel "..str, selected )
		end
	end
end

function BU_DOC:do_key(key)
	self:print_do_key( "BU_DOC", key )

 	local b_key_used = false
 	if key == 127 then
		self:delete_table_by_bu( self.bu_paths[self.id_cur] )
		b_key_used = true
	end

 	return b_key_used or oo.getsuper(BU_DOC).do_key( self, key )
end

function BU_DOC:do_key_special(key)
	self:print_do_key_special( "BU_DOC", key )

 	local b_key_used = false

 	return b_key_used or oo.getsuper(BU_DOC).do_key_special( self, key )
end

--these called by base class BU_WINDOW_LIST
function BU_DOC:do_search_text_change()
	self:assign_table()
end
function BU_DOC:do_item_roll_over( bu )
	self:display_item_info( bu )
	self.__bu_for_info = bu
end

BU_DOC.doc.update = "() update item info and call the super for now"
function BU_DOC:update()
	self:display_item_info( self.__bu_for_info )
	oo.getsuper(BU_DOC).update(self)
end

BU_DOC.doc.draw = "() just draw on top a rect over the current bu_root"
function BU_DOC:draw()
	oo.getsuper(BU_DOC).draw( self )

	if not self:is_mini() then
		local bu = self.bu_paths[self.id_cur]
		local bus = bu:get_bus_up()
		local l,b, sx,sy = bu:get_lb_sxy_cur()
		--self:print( "sxy "..sx.." "..sy )
		local r,t = l+sx,b+sy
		l,b = bus:do_transfo_inverse( l,b )
		--self:print( "after lb "..l.." "..b )
		r,t = bus:do_transfo_inverse( r,t )
		--local a,b = bu:convert_xy_local_to_bus_up( 0,0 )
		--self:print( "lb "..l.." "..b )
		gol.color_white()
		gol.set_texture_dim(0)
		gol.set_line_width( 4 )
		aaa.draw_rect_line( l,b, r,t )
	end
end