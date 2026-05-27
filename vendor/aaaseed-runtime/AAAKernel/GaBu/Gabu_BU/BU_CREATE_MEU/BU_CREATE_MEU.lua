if CLASS.DECLARE( "BU_CREATE_MEU", BU_WINDOW_LIST, {} ) then
	BU_CREATE_MEU:set_class_status_doc(	CLASS.STATUS.GABU,
									"Window to chosse a MEU type to create" )
end

BU_CREATE_MEU.doc.__get_bu_root_rect = "( id ) return position of bu root used to hold table"
function BU_CREATE_MEU:__get_bu_root_rect( id )
	local hmar = .05
	local DX,DY = 8/6,.25
	local SX,SY = DX-hmar, DY-hmar
	return {-4+DX/2,4-SY/2-hmar-(id+2)*DY, SX,SY}
end
BU_CREATE_MEU.doc.create_bu_root = "( id, name, tab, doc ) create a bu root holding a table and doc"
function BU_CREATE_MEU:create_bu_root( id, name, tab, doc )
	local bus = self:get_bus_down()
	local bu = bus:add_trig( tostring(name), self:__get_bu_root_rect(id), 1 )
		bu:set_text_nice()
		bu:set_method_on_click( self, "open_table", name, tab, doc )
		bu:set_method_on_click_double( self, "delete_table_by_bu", bu )
	return bu
end
BU_CREATE_MEU.doc.__update_bu_root_positions = "() recompute the positions after insertion or deletion of bu_root"
function BU_CREATE_MEU:__update_bu_root_positions()
	local bu_paths = self.bu_paths
	for id, bu in ipairs(self.bu_paths) do
		local r = self:__get_bu_root_rect(id)
		bu:set_xy_sxy( r[1],r[2],  r[3],r[4] )
	end
end

BU_CREATE_MEU.doc.create =	"( name, rect ) Constructor, create a BU_CREATE_MEU window."
function BU_CREATE_MEU:create( name, rect )
	local self = BU_CREATE_MEU:create_instance( name, rect )

	local bus_down = self:define_ui()
	bus_down:set_transfo( 8 )
	self:transform_in_window( bus_down )

	self:__set_window_state( "mini" )
	self:set_text_factor( .05 )

	self:assign_table()

	return self
end

BU_CREATE_MEU.doc.__set_tags_button = "set the state of the whole bu_tag or bu_inv buttons"
function BU_CREATE_MEU:__set_tags_button( bu, field_name, t, balue )
	local val = balue:get_value()
	aaa.print_fn()
	for _,item in ipairs(t) do
		item[field_name] = val
	end
	self:assign_table()
end


BU_CREATE_MEU.doc.define_ui =	{ "() Defines ui in window, inits table exploration vars, assign_table,",
									"and search (with editable bu_text, cf search_in_table method)." }
function BU_CREATE_MEU:define_ui()
	local bu

	self.tables = {}
	self.bu_paths = {}	-- store the level we go thru

	local bus = BUS:create( self:get_name() )
	bus:init_begin()
	
	-- bu_list with the scrolable table content
		local SX = 5*8/6
		local X = (8-SX)/2
		bu = self:create_bu_list( bus, {X,-.5,  	SX,5.5} )
			bu:set_nb_xy( 4,14 )
			bu:adjust_nb( 256 )
			bu:set_color_back( {0,0,0, 1} )

	-- Display Doc at the top of the window
		local SY = 1.75
		bu = self:create_bu_info( bus, {X,4-SY/2, 	SX,SY} )

	-- buttons on the left to choose tables	
		-- Now a selector on top location of the bu_root
		local rect = self:__get_bu_root_rect(-2)
		bu = bus:add_selector( "Proto", rect )
			bu:set_item_text( 1, "Self", "All", "Proto" )
			:set_target_lua( self, "s_proto_wanted", 3 )
			:set_method_on_value_change( self, "assign_table", nil )	-- we need to put nil here or bu will be received by assign_table

		local rect = self:__get_bu_root_rect(-1)
		bu = bus:add_selector( "Tags", rect )
			bu:set_item_text( 1, "No Tag", "All", "Tag" )
			:set_target_lua( self, "s_tag_wanted", 3 )
			:set_method_on_value_change( self, "assign_table", nil )	-- we need to put nil here or bu will be received by assign_table

		local function get_xy_sxy( id, b_right )
			local rect = self:__get_bu_root_rect(id)
			local x,y, sx,sy = unpack(rect)
			if b_right then
				x = x + sx / 2
				sx = sx/4 - sx/32
				x = x - sx
			else
				x = x - sx / 2
				sx = sx * 3/4	
			end
			return {x+sx/2,y, sx,sy}
		end

		local t = {}
		for i,name in ipairs( MEU.__tags_tabs.index_to_name ) do
			t[i] = {}
			bu = bus:add_button(		name,		get_xy_sxy( i, false ) )
				t[i] = { name = string.lower(name), bu_tag = bu }
				bu:set_text_nice()
				bu:set_target_lua( t[i], "b_use", false )
				bu:set_method_on_value_change( self, "assign_table", nil )

			bu = bus:add_button( name.."_include",	get_xy_sxy( i, true ) )
				t[i].bu_inc = bu
				bu:set_text()
				bu:set_text_nice()
				bu:set_target_lua( t[i], "b_included", true )
				bu:set_method_on_value_change( self, "assign_table", nil )
		end
		--table.print( t, "t_tag", 2 ) 
		self.tags = t

	-- buttons at the top left to set previous buttons
		bu = bus:add_button( "All tag",		get_xy_sxy( 0, false ) )
			bu:set_method_on_value_change( self, "__set_tags_button", bu, "b_use", t )
			bu:set_value( false )
			bu:set_text_visible( false )

		bu = bus:add_button( "All inc",		get_xy_sxy( 0, true ) )
			bu:set_method_on_value_change( self, "__set_tags_button", bu, "b_included", t )
			bu:set_value( true )
			bu:set_text_visible( false )

	-- Search
		self:create_bu_search( bus, {X-SX/2,-3.6, SX,.5}, "Tuto", false, self, "update_table" )
	-- Add an exclude mecanism

	bus:init_end()

	return bus
end

BU_CREATE_MEU.doc.__sort_for_doc =	"( table, key_a,key_b ) complex sorting to establish the order in BU_LIST"
--todo add an option to change sort order
function BU_CREATE_MEU.__sort_for_doc( t, key_a,key_b ) -- t not used
	return string.lower(key_a) < string.lower(key_b) 
end

function BU_CREATE_MEU:do_uif_command( bu, uif )
	local command = BU:get_uif_name_sel( uif )
	local t = bu.__item_info
	local proto = t.tab[t.key]
	--aaa.debug.print_traceback()

	local x,y = uif.x,uif.y
	local b_used = true
	if	   command == "lua"	then proto:edit_lua()
	elseif command == "new"	then self:ask_new( proto )
	elseif command == "dir"	then
		local dir = proto:get_dir()
		--self:print( "proto dir is "..dir )
		aaa.os.open_dir( dir )
	else
		b_used = false
	end

	return GABU_OBJ.do_uif_command_with_super( self, b_used, uif, MU )
end

function BU_CREATE_MEU:get_item_uif_zones()
	--aaa.print_fn()
	local zones = {}	--oo.getsuper(BU_RECT).get_uif_zones( self )
	local sx,sy = 2,1
	local dy = .2
	local x,y = -.4,-.8
	local dx = .2
	zones.New	= {	type="button",		rect={x,	y,	3, sy}	}
	y = y - sy - dy 
	zones.Lua	= {	type="button",		rect={x-1.6,	y,	3, sy}	}
	zones.Dir	= {	type="button",		rect={x+2.1,	y,	4, sy }	}

	return zones
end

BU_CREATE_MEU.doc.draw_item = { "( bu ) draw each bu in the scrolling list." }
function BU_CREATE_MEU:draw_item( bu )
	local t = bu.__item_info
	local proto = t.tab[t.key]
	--self:print( "proto is "..proto )

	if proto then -- could have beeen removed on the fly
		local info = proto:define_meu_infos()
		--local name = proto:get_meu_type_lower()
		--self:print( "draw_item for "..name )
		--if name == "dir" then
			-- self:print( "proto "..proto )
			-- table.print( proto, "proto", 1 )
			-- table.print( proto.__gabu, "proto", 1 )
			--table.print( getmetatable(proto),"mt" ) 
		--end
		local col = BU.__color_named
		bu:set_text_color( info.b_default and col.problem or col.white )
		--end
		-- local str_type_value = t.str_type_value
		-- local type_info = self:get_type_info(str_type_value)
		-- local value = t.tab[t.key]
		-- if str_type_value == "table" and table.is_empty(value) then
		-- 	bu:set_color_back( {0,.25,0, 1} )
		-- else
		-- 	bu:set_color_back( type_info.color_back )
		-- end
		-- if str_type_value == "boolean" then
		-- 	bu:set_text_color( value and { 0,1,0, 1} or {1,0,0, 1} )
		-- else
		-- 	bu:set_text_color( type_info.color_text )
		-- end

		MEU.__b_draw_icon_pure = true

			gol.push_matrix()
				gol.translate_x( .1 )
				gol.scale_x( .8 )
				--todo make this more simple
				bu:set_text_draw( true )
				BUTTON.draw(bu)
				BUTTON.draw_fore(bu)
				--bu:set_text_draw( false )
			gol.pop_matrix()

			if proto.draw_icon ~= MEU.draw_icon then
				--proto:print( "draw_icon" )
				gol.push_matrix()
					gol.translate_x( -.2 )
					gol.scale_x( .75 )
					gol.scale( .8 )
					proto:draw_icon()
				gol.pop_matrix()
			end
		
		MEU.__b_draw_icon_pure = false
	end
end

function BU_CREATE_MEU:__do_new_meu( dialog_table )
	aaa.print_fn()
	table.print( dialog_table, "__do_new_meu() dialog_table" )
 
	local name = dialog_table.value

	if not MEU_CTX.cur:check_name_keep_type( name, dialog_table.value_begin, "Can't create" ) then
		return
	end

-- check no Meu with the same name
	local gp = app:get_gp()
	local mus = gp:get_mus_down()
	if mus:check_by_name( name, "create" ) then
	-- get proto dir
		local proto = dialog_table.proto
		--local dir_proto = aaa.dir.get_dir_kernel().."/"..proto:get_dir_no_slash()
		local dir_proto = proto:get_dir_no_slash()
		-- self:print( "dir_proto before is "..dir_proto )
		-- dir_proto = string.sub( dir_proto, 1, -2 )
		self:print( "dir_proto        is "..dir_proto )

	-- build new dir
		local dir_new = mus:get_dir()..mus:get_dir_MEU()..name
		self:print( "dir_inst is "..dir_new )
		
		--todo check
		aaa.file.copy_dir( dir_proto, dir_new )
		aaa.file.remove( dir_new.."/preset_0.plua" )

		local meu_type, inst_key = MEU_CTX.cur:split_meu_type_inst( name )

		local mu = mus:create_mu( meu_type, inst_key )
		--local mu = mus:create_one_mu( dir_new )		
		if mu then
			--local meu = mu:get_meu()
			local script_name_to_remove = dir_new.."/"..proto:get_lua_filename()
			local err_str = aaa.file.remove( script_name_to_remove )
			--self:box_debug( "aaa.file.remove( "..script_name_to_remove.." ) returned "..err_str )
		-- 	if rect then
		-- 		mu:set_sxy( rect[3],rect[4] )
		-- 		mu:set_xy(  rect[1],rect[2] )
		-- 	end
			mu:arrange_new( mus )
		end
	end
end

function BU_CREATE_MEU:do_dialog_hook( what, value, dialog_table )
	self:print( "BU_CREATE_MEU:do_dialog_hook()" )
	--aaa.print_method()
	if dialog_table.signature == "meu_new" then
		if what == "change_value" then
			self:print( "BU_CREATE_MEU:do_dialog_hook() value is now \""..value.."\"" )
			dialog_table.value = value
		elseif what == "open" then
		elseif what == "close" then                  
			--gol.get_error( "MU:do_dialog_hook() close" )
			local action = dialog_table.action
			self:print( "action \""..action.."\" dialog return with "..value )
			if value == "ok" then
				--table.print( mu_dialog_table, "dialog_table for mu", 1 )		
				if action == "new_meu" then
					self.__table_todo = self:make_table_to_call_method( self, "__do_new_meu", dialog_table )
				end
			end
		else
		end
		return true
	else
		return oo.getsuper(BU_CREATE_MEU).do_dialog_hook( self, what, value, dialog_table )
	end
end

function BU_CREATE_MEU:ask_new( proto )
	local name = proto:get_meu_type()
	local dialog_table = { title="New MEU name", bu=self, param_type="string", value_begin=name.."_New", b_simple=true, obj=self, proto=proto, action="new_meu", signature="meu_new" }
	table.print( dialog_table, "dialog_table", 2 )
	ga:add_dialog_edit( dialog_table )
end

function BU_CREATE_MEU:do_item_click( bu )
	local t = bu.__item_info
	local proto = t.tab[t.key]
	if proto then -- could have beeen removed on the fly
		--proto:edit_lua()
		app:mark_by_type( proto:get_meu_type() )
	end
	return false
end
function BU_CREATE_MEU:do_item_click_double( bu )
 	local t = bu.__item_info
 	local proto = t.tab[t.key]
 	self:ask_new( proto )
end
-- function BU_CREATE_MEU:do_item_click_long( bu )
-- 	-- --aaa.print_method( "do_item_click_long" )
-- end


-- function BU_CREATE_MEU:do_dialog_hook( what, value, dialog_table )
-- 	aaa.print_fn( "BU_CREATE_MEU:do_dialog_hook" )
-- 	if what == "change_value" then
-- 		self:print( "text selector is now "..value )
-- 		--dialog_table.bu:set_text( value )
-- 	elseif what == "open" then
-- 		--param.set( dialog_table.param_ref, dialog_table.bu:get_text() )
-- 	elseif what == "close" then
-- 	end
--	return true
-- end


BU_CREATE_MEU.doc.assign_table = "( table ) make the current table displayed"
function BU_CREATE_MEU:assign_table()
	--self:box_error( "Passing here" )

	local tab = MEU_CTX.cur:get_protos_table()
	local doc_tab = tab.doc

	--aaa.print_fn()

	local ui = self.ui

	local nb = table.count_item( tab )
	local bu_list = ui.bu_list
	bu_list:adjust_nb( nb )

	local tags = self.tags
	local included = {}
	local included_nb = 0
	local excluded = {}
	for _,tag in pairs(tags) do
		local name = tag.name
		if tag.b_use then
			if tag.b_included then
				included[name] = true
				included_nb = included_nb + 1
			else
				excluded[name] = true
			end
		end
	end

	--self:print_inverse( "---------------------------------------------------" )
	--table.print( included, "include", 1 )
	--table.print( excluded, "exclude", 1 )
	--local included_nb = table.count_item(included)

	local i = 0
	local b_test_tag_wanted		= self.s_tag_wanted == 3
	local b_test_tag			= self.s_tag_wanted ~= 2
	local b_test_proto_wanted	= self.s_proto_wanted == 3
	local b_test_proto			= self.s_proto_wanted ~= 2

	local str_search		= self.b_use_search and string.lower( ui.bu_search_text:get_text() )

	local b_verbose = false
	for key, proto in pairs_sorted( tab, self.__sort_for_doc ) do
		--name = self:make_print_name( name, value )

		local doc = doc_tab
		--local str_type_value = type(value)
		local b_used = true

--		if type(key) == "string" then
		if b_used and str_search and (string.find( string.lower(key), str_search )==nil) then
			b_used = false
		end
		if b_used and b_test_proto then
			b_used = proto:is_proto_and_isolated() == b_test_proto_wanted
		end
		if b_used then
			--table.print( proto )
			local inf = proto:get_meu_infos_cleaned()
			local name_long, author, date, version, help, tags = inf.name_long, inf.author, inf.date, inf.version, inf.help, inf.tags

			if tags and tags[1] then
				--self:print( "BU_CREATE_MEU:assign_table() is "..proto )
				if b_test_tag and not b_test_tag_wanted then
					b_used = false
				else
					local tag_found_nb = 0
					for _,name in pairs(tags) do
						local lname = string.lower(name)
						if excluded[lname] then
							b_used = false
							if b_verbose then
								self:print( key.."\t\texcluded by tag "..name )
							end
							break
						end
						if included[lname] then
							tag_found_nb = tag_found_nb + 1
						end
					end
					if b_used then
						if tag_found_nb ~= included_nb then
							b_used = false
							if b_verbose then
								self:print( key.."\t\texcluded only found "..tag_found_nb.."/"..included_nb.." tags" )
							end
						end
					end
				end
			else
				if b_test_tag and b_test_tag_wanted then
					b_used = false
				end
			end

			-- elseif included_nb > 0 then
			-- 	b_used = false
			-- end
			if b_used then

				-- local b_table = str_type_value == "table"
				-- if b_table then
				-- 	if value~=_G then
				-- 		if value[CLASS.__str_class_field] then
				-- 			str_type_value = CLASS.is_class(value) and "class" or "gabu_obj"
				-- 		end
				-- 		doc = value.doc
				-- 	end
				-- end
				
				i = i + 1	
				local bu = bu_list:get_bu(i)
				bu:set_text( proto:get_meu_type() )
				-- we got lowercase only below
				--bu:set_text( (type(key)=="string") and key or aaa.__build_str_for_doc(key, true) )
				bu.__item_info = { tab=tab, key=key, doc_target=doc }
		
				bu:set_method_on_click( self,"do_item_click", bu )
				bu:set_method_on_click_double( self, "do_item_click_double", bu )
				bu:set_method_on_click_long( self, "do_item_click_long", bu )
				--bu:set_method_on_click( self, "do_item_click_long", bu )
		
				bu:set_visible( true )
				
				if self.draw_item then
					bu.draw = function(bu)
						--local up = bu:get_bu_up():get_bu_up()
						--if up.draw_item then
						--	self:print( self.." "..up )
							self:draw_item( bu )
						--else
						--	up:print_error( "can't draw_item no draw method" )
						--end
					end	--indirection thru a function so we can update on the fly
					bu.draw_fore =  function() end
				else
					self:print_error( "can't assign a draw to this bu "..bu )
				end
				
				if self.get_item_uif_zones then
					bu.__b_uif_outside = false
					bu.get_uif_name   = function(bu)      return bu:get_text() end
					bu.get_uif_zones  = function(bu)      return self:get_item_uif_zones() end
					bu.do_uif_command = function(bu, uif) return self:do_uif_command( bu, uif ) end
				else
					self:print_error( "can't assign a get_uif_zones to this bu "..bu )
				end

			end
		end
	end

	bu_list:adjust_nb( i )

	self.info_line_0 = "MEU nb "..i.."/"..nb
	ui.bu_info:set_text( self.info_line_0 )
end


function BU_CREATE_MEU:__get_doc( var, name )
	local doc
	-- if CLASS.is_gabu_obj( var ) then
	-- 	doc = var:get_class_doc()
	-- else
	-- 	doc = self.doc_cur
	-- --	self:print( "doc cur is "..doc )
	-- 	doc = doc and doc[name]
	-- end
	return doc
end

--todo redo this comment
BU_CREATE_MEU.doc.display_item_info = "( var_name, var ) Updates the bu_info at top of bu list with name and value of the var which the mouse is over."
function BU_CREATE_MEU:display_item_info( t )
	--table.print( t, "display_item_info", 2 )

	local proto = t.tab[t.key]
	if proto then -- could have beeen removed on the fly
		local inf = proto:get_meu_infos_cleaned()
		local name_long, author, date, version, help, tags = inf.name_long, inf.author, inf.date, inf.version, inf.help, inf.tags

		local str = proto:get_meu_type()
		if name_long then str = str.."("..name_long..")" end
		if version then str = str.." V"..version end
		author = author or "Mâa"
		if author then str = str.." by "..author.."   " end
		if date then str = str.." "..date end

		str = str.."                    "..self.info_line_0
		if help then
			str = str.."\n"..help
		end

		if tags and tags[1] then
			str = str.."\n\nTAGS:  "
			for _,tag in ipairs(tags) do
				str = str..tag.." "
			end
		end

		self.ui.bu_info:set_text( str )
	-- 	local str = var_name.."        is "..aaa.__build_str_for_doc(var)
	-- 	local doc = self:__get_doc( var, var_name )
	-- 	if doc then str = str.."\n"..doc end
	-- --	self:print(str)
	-- 	self.ui.bu_info:set_text( str )
	end
end


--[[
BU_CREATE_MEU.doc.open_table =	"( name, tab, doc ) creates the corresponding new BU on the left and\n"..
								"calls assign_table to display table variables."
function BU_CREATE_MEU:open_table( name, tab, doc )
	-- local tables = self.tables

	-- -- do we have already this table ?
	-- local id_next = array.find_index_by_val( tables, tab )
	-- if not id_next then	
	-- 	id_next = self.id_cur + 1
	-- 	table.insert( self.bu_paths, id_next, self:create_bu_root( id_next, name, tab, doc ) )
	-- 	table.insert( tables, id_next, tab )
	-- end

	-- if self.id_cur ~= id_next then
	-- 	self.id_cur = id_next
	-- 	self:__update_bu_root_positions()
	-- end
	-- self.doc_cur = doc
	-- self:assign_table( tab )
end
--]]



--[[
function BU_CREATE_MEU:add_keyboard()
	local function __add_keyboard( name, id )
		local keyboard =  KEYBOARD:create( name, id )
		return keyboard
	end
	self.keyboard1 = __add_keyboard( "PC", 1 )
	self.keyboard1:set_button_close_active( true )
	table.print( self.keyboard1, "self.keyboard1" )
end--]]

--[[
BU_CREATE_MEU.doc.add_table = "() Search in current table (case sensitive). Opens a table of element containing  the string from the bu_text editable."
function BU_CREATE_MEU:add_table()
	-- local str = self.ui.bu_search_text:get_text()
	-- if str then
	-- 	local selected = {}
	-- 	local tab = self.tables[self.id_cur]
	-- 	local i = 0
	-- 	for key in pairs( tab ) do
	-- 		--print(k)
	-- 		if key:find( str ) then
	-- 			--print( str, k:find( str ), k )
	-- 			selected[key] = tab[key]
	-- 			i = i + 1
	-- 		end
	-- 	end
	-- 	if i > 0 then
	-- 		local doc
	-- 		if tab == _G then
	-- 			doc = aaa.lua.global.get( "doc" )
	-- 		else
	-- 			doc = tab.doc
	-- 		end
	-- 		selected.doc = doc
	-- 		self:open_table( "Sel "..str, selected )
	-- 	end
	-- end
end
--]]

--unused 2024 Sep
-- BU_CREATE_MEU.doc.exec_lua = "() Example of lua execution from text in editable bu_text, only here for demo."
-- function BU_CREATE_MEU:exec_lua()
-- 	local str = self.bu_search_text:get_text()
-- 	local fn = loadstring(str)
-- 	fn()
-- end

function BU_CREATE_MEU:do_key(key)
	self:print_do_key( "BU_CREATE_MEU", key )

 	local b_key_used = false

 	return b_key_used or oo.getsuper(BU_CREATE_MEU).do_key( self, key )
end

function BU_CREATE_MEU:do_key_special(key)
	self:print_do_key_special( "BU_CREATE_MEU", key )

 	local b_key_used = false

 	return b_key_used or oo.getsuper(BU_CREATE_MEU).do_key_special( self, key )
end

--these called by base class BU_WINDOW_LIST
function BU_CREATE_MEU:do_search_text_change()
	self:assign_table()
end
function BU_CREATE_MEU:do_item_roll_over( bu )
	self:display_item_info( bu.__item_info )
end

BU_CREATE_MEU.doc.update = "() just call the super for now"
function BU_CREATE_MEU:update()
	if self.__table_todo then
		self:do_table( self.__table_todo )
		self.__table_todo = nil
	end

	oo.getsuper(BU_CREATE_MEU).update(self)
end

BU_CREATE_MEU.doc.draw = "() just draw on top a rect over the current bu_root"
function BU_CREATE_MEU:draw()
	--self:draw_fore()
	oo.getsuper(BU_CREATE_MEU).draw( self )

	-- if not self:is_mini() then
	-- 	local bu = self.bu_paths[self.id_cur]
	-- 	local bus = bu:get_bus_up()
	-- 	local l,b, sx,sy = bu:get_lb_sxy_cur()
	-- 	--self:print( "sxy "..sx.." "..sy )
	-- 	local r,t = l+sx,b+sy
	-- 	l,b = bus:do_transfo_inverse( l,b )
	-- 	--self:print( "after lb "..l.." "..b )
	-- 	r,t = bus:do_transfo_inverse( r,t )
	-- 	--local a,b = bu:convert_xy_local_to_bus_up( 0,0 )
	-- 	--self:print( "lb "..l.." "..b )
	-- 	gol.color_white()
	-- 	gol.set_texture_dim(0)
	-- 	gol.set_line_width( 4 )
	-- 	aaa.draw_rect_line( l,b, r,t )
	-- end
end

