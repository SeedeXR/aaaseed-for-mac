if CLASS.DECLARE( "BU_WINDOW_LIST", BU_WINDOW, {} ) then
	BU_WINDOW_LIST:set_class_status_doc(	CLASS.STATUS.GABU,
									"Window to handle a BU_LIST, (2024 Sep) base class for BU_DOC and BU_MEU_SELECTOR" )
end

--BU_WINDOW_LIST.create_instance = nil
function BU_WINDOW_LIST.create_instance( class, name, rect )
	local self = oo.getsuper(BU_WINDOW_LIST).create_instance( class, name, rect )

	--aaa.debug.print_traceback()
	self.ui = {}	-- should do in BU_WINDOW ?
	return self
end

BU_WINDOW_LIST.doc.create_bu_list = "( bus ) create the bu_list"
function BU_WINDOW_LIST:create_bu_list( bus, rect )
	local ui = self.ui
	local bu = ui.bu_list
	if bu then	-- when we3 debug we "redefined" this on the fly
		bus:remove_bu( bu )
		bu:free()
		ui.bu_list = nil
	end

	bu = BU_LIST:create( "Field list", rect )
		bus:add_bu( bu )
		--bu:set_dplane( -42 )
		bu:set_text_visible( false )
		--bu:set_ui_active( false )
		bu:set_mobile( false )
		bu:set_ui_top( false )
		bu:set_ui_top_size( false )
	--	bu:set_nb_xy( 4,14 )
	--	bu:adjust_nb( 256 )

	ui.bu_list = bu
	return bu
end

BU_WINDOW_LIST.doc.__draw_doc =	"( bu ) until text rendering is corrected (2024 Sep)\n"..
								"this method is called by the bu_info at the top of window"
function BU_WINDOW_LIST:__draw_doc( bu )
	local str = bu:get_text()
	--self:print( "str is "..str )
	gol.color_white()
	local str,nb = string.gsub(str, "\n", "\n  ")
	nb = math.min( 1/6, 1/(nb+1) )
	bu:draw_text_nice( str, -.5,.5-nb,0, .024,nb, "left", "left" )
end

BU_WINDOW_LIST.doc.create_bu_info = "( bus, rect ) create the bu wherre info is displayed"
function BU_WINDOW_LIST:create_bu_info( bus, rect )
	local bu = bus:add_text( "Info", rect )
		bu:set_ui_active( false )
		-- until we fix text display size we used this hack
		if false then
			bu:set_text_nice()
		else
			bu.draw = function(bu) self:__draw_doc( bu ) end	--indirection thru a function so we can update on the fly
		end
	self.ui.bu_info = bu

	return bu
end

BU_WINDOW_LIST.doc.create_bu_search = "( bus, rect, text_at_start, b_add, obj, method ) create the UI for searching"
function BU_WINDOW_LIST:create_bu_search( bus, rect, text_at_start, b_add, obj, method )
	local ui = self.ui
	local x,y, SX,sy = unpack(rect)
	local dx = .1
	local bu

	--bu = bus:add_trig( "Add keyboards", {-3.5,-3.6, .8,.5} )
	--bu:set_method_on_click( self, "add_keyboard" )

	local sx = (SX-dx*2) / 8
	bu = bus:add_button( "Use", {x+sx/2,y, sx,sy} )
		bu:set_method_on_click( self, "assign_table", nil )
		bu:set_target_lua( self, "b_use_search", false )
	ui.bu_search_use = bu
	x = x + sx + dx

	if b_add then
		bu = bus:add_trig( "Add", {x+sx/2,y, sx,sy} )
			bu:set_method_on_click( self, "add_table" )
		ui.bu_search_do = bu
		x = x + sx + dx
	end

	sx = sx * 6
	bu = bus:add_text( "Input zone", {x+sx/2,y, sx,sy} )
		bu:set_color_back( { 1, 1, 1, .25 } )
		--bu:set_editable( true, "PC" )
		bu:set_color_border( { 1,0,1, 1 } )
		bu:set_method_on_click( ga, "add_dialog_edit", {	title="Field selector",	bu=bu, balue_id=self:get_name()..bu:get_name(),
															param_type="string", b_simple=true, obj=self,
															signature="search" } )
		bu:set_text_nice( true )
		if text_at_start then
			bu:set_text( text_at_start )
		end
		--bu:set_editable( true )
		--bu:set_editable_active( true )
		--bu:set_method_on_value_change( self, "do_text_change" )	-- BU_TEXT don't support this
		--bu:set_max_length( 32 )
	ui.bu_search_text = bu
	x = x + sx*6 + dx
end

function BU_WINDOW_LIST:do_dialog_hook( what, value, dialog_table )
	aaa.print_fn( "BU_WINDOW_LIST:do_dialog_hook" )
	if dialog_table.signature == "search" then
		if what == "change_value" then
			self:print( "text selector is now "..value )
			dialog_table.bu:set_text( value )
		elseif what == "open" then
			param.set( dialog_table.param_ref, dialog_table.bu:get_text() )
		elseif what == "close" then
		end
	end
	return true
end

function BU_WINDOW_LIST:do_key(key)
	self:print_do_key( "BU_WINDOW_LIST", key )

 	local b_key_used = false

 	return b_key_used or oo.getsuper(BU_WINDOW_LIST).do_key( self, key )
end

function BU_WINDOW_LIST:do_key_special(key)
	self:print_do_key_special( "BU_WINDOW_LIST", key )

 	local b_key_used = false

 	return b_key_used or oo.getsuper(BU_WINDOW_LIST).do_key_special( self, key )
end

function BU_WINDOW_LIST:do_search_text_change()
	self:print( "no do_search_text_change() defined" )
end
function BU_WINDOW_LIST:do_item_roll_over( bu )
	self:print( "no do_item_roll_over() defined" )
end

function BU_WINDOW_LIST:update()
	if not self:is_mini() then

		local bu_list = self.ui.bu_list

		if self.b_use_search then
			local text = self.ui.bu_search_text:get_text()
			if self.text_last ~= text then
				self.text_last = text
				self:do_search_text_change()
			end
		end

		local x,y = aaa.mouse.get_xy_render()
		 x,y = EVENT.transform_move_coor( x,y )

		--if bu and b_is_in_list then-- and i_level then
		--todo start at the right level
		local bu
		if false then
			bu = ga:find_bu_by_pos( x, y )
		else	-- this way we search only locally
			x,y = bu_list:convert_xy_top_to_local( x,y )
			--aaa.show( x.." "..y, "xy" )
			bu = bu_list:get_bus_down():find_bu_by_pos( x,y )
		end

		if bu then
			--self:print( "bu found is "..bu )
			self:do_item_roll_over( bu )
		end

		--
		-- Mouse wheel scroll
		--
		--todo reduce scroll to inside this bu
		-- local decal_x,decal_y = self:get_xy()
		-- local min_x = -1.75 + decal_x
		-- local max_x = 1.75 + decal_x
		-- local min_y = -1.5 + decal_y
		-- local max_y = 1.5 + decal_y
	--	if mouse_y > min_y and mouse_y < max_y and mouse_x > min_x and mouse_x < max_x then--if self:is_inside( mouse_x, mouse_y ) then--
		if aaa.mouse.is_wheel_down() then
			bu_list:scroll_v( 1 )
		elseif aaa.mouse.is_wheel_up() then
			bu_list:scroll_v( -1 )
		end
		--	end
	end

	oo.getsuper(BU_WINDOW_LIST).update(self)
end
