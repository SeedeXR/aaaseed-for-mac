if CLASS.DECLARE( "SELECTOR", BUI, {
	__b_draw_grid_line		= true,
	__b_sel_multiple		= false,
	__sel_item 				= {},	--todo init only when needed
	__nb_sel_item 			= 0,
	} ) then
	SELECTOR:set_class_status_doc(	CLASS.STATUS.GABU,
									"SELECTOR is one type of BU to multiple choice with multiple selection eventually" )
end
--todo when multi colonne add an option to change order (col then raw will be nice)
function SELECTOR:get_uif_zones()
	local zones = {}

	if self:is_selection_multiple() then
		BU.add_uif_zones_base( self, zones, 1 )
	else
		BUI.add_uif_zones_base( self, zones, 1 )
	end
	return zones
end

--todo remove should be handle now in balue
function SELECTOR:get_value()
	local val = BUI.get_value( self )
	if type(val) ~= "number" then
		aaa.debug.print_traceback()
		self:print_error( "SELECTOR:get_value() we should not deal with a "..type(val) )
		return 0
	end
	local err = val - math.floor( val+.0000001 )
	if err~=0 then self:print_error( "error is "..err ) end
	return math.floor( val+.0000001 )
end
--function SELECTOR:set_value( val )
--	self:print( "SELECTOR:set_value( "..val.." )" )
--	BUI.set_value( self, val )
--end

--ITEM
--
-- those too areinternal helper for interaction and draw and should ot be used outside SELECTOR
function SELECTOR:__get_item_sel()
	if self:is_selection_multiple() then
		return self.__i_sel_item
	else
		return self:get_value() - self:get_min() + 1
	end
end
function SELECTOR:__set_item_sel( i )
	self.__i_sel_item = i
end


function SELECTOR:get_item_selected_nb() 		return self.__nb_sel_item		end
function SELECTOR:set_item_selected_nb( nb )	self.__nb_sel_item = nb			end
function SELECTOR:is_item_selected( i_sel )
	return self.__sel_item[ i_sel ]
end
--todo merge with next method
--hack for now
function SELECTOR:set_sel_multiple_item( i, b )
	self.__sel_item[i] = b
end
function SELECTOR:set_item_selected_multiple( i_sel, b )
	local nb_sel_item = self:get_item_selected_nb()

		if b then
			if not self.__sel_item[ i_sel ] then
				self.__sel_item[ i_sel ] = true
				nb_sel_item = nb_sel_item + 1
				self:set_value( 1, i_sel )
			end
		else
			if self.__sel_item[ i_sel ] then
				self.__sel_item[ i_sel ] = false
				nb_sel_item = nb_sel_item - 1
				self:set_value( 0, i_sel )
			end
		end

	self:set_item_selected_nb( nb_sel_item )
end


function SELECTOR:get_items_id_on()
	local tab = {}
	for i=1,self:get_nb() do
		local b = self.__sel_item[i]
		if type(b) == "number" then
			b = b >= .5
		end
		if b then
			table.insert( tab, i )
		end
	end
	return tab
end
function SELECTOR:get_items_text_on()
	local tab = {}
	local item_text = self.__item_text
	for i=1,self:get_nb() do
		local b = self.__sel_item[i]
		if type(b) == "number" then
			b = b >= .5
		end
		if b then
			table.insert( tab, item_text[i].str )
		end
	end
	return tab
end

function SELECTOR:set_selection_multiple( b )
	self.__b_sel_multiple = b
	local nb = self:get_nb()
	-- init tab item sel
	if b then
		self.__b_undo_redo_using_values = true
		local t = {}
		for i = 1, nb do
			t[i] = false
		end
		self.__sel_item = t
	else
		self.__b_undo_redo_using_values = nil
	end
	self:__set_item_sel( 1 )
	return self
end
function SELECTOR:is_selection_multiple() 		return self.__b_sel_multiple 	end

--VALUES
--
--todo make a generic convert_to_bool_fn
--or a metamethod get_as_bool()
function SELECTOR:get_values()
	if self:is_selection_multiple() then
		local tab = {}
		for i=1,self:get_nb() do
			local b = self.__sel_item[i]
			if type(b) == "number" then
				b = b >= .5
			end
			tab[i] = b
		end
		return tab
	end
	return self:get_value()
end
function SELECTOR:set_values( val )
	if self:is_selection_multiple() then
		if type(val)=="table" then
			for i=1,self:get_nb() do
				self.__sel_item[i] = val[i]
			end
		else
			local str = "val is "..val
			self:box_debug( "SELECTOR multiple set_values() with no table"..
							"\n  "..str )
		end
	else
		self:set_value( val )
	end
end

function SELECTOR:__compute_index_from_xy( x,y )
	x,y = x+.5,.5-y	-- selector start at left top
	local x_inside,y_inside = clamp_01(x),clamp_01(y)
	local nu,nv = self.__nb_u,self.__nb_v
	local ix = clamp( math.floor( x_inside * nu ) ,0 , nu-1 )
	local iy = clamp( math.floor( y_inside * nv ) ,0 , nv-1 )
	local index = iy * nu + ix
	--self:print( "__compute_index_from_xy( "..x..", "..y.." )".." -> "..index.." "..nu.."/"..nv )
	return index, (x==x_inside) and (y==y_inside)
end

function SELECTOR:__do_mouse_common( x,y )
	local index = self:__compute_index_from_xy( x,y )

	if self:is_selection_multiple() then
		self:__set_item_sel( index+1 )
	else
		local text = self:get_item_text( index+1 )
		--self:print( "item text is "..text )
		if text~=nil or (not self.__item_text_nil_forbid_mouse_selection) then
			self:set_value( (index or 0) + self:get_min() )
		end
	end

	--self:print( "__do_mouse_common( "..x..","..y.." ) -> "..self:get_value().." min "..self:get_min(1) )

end

function SELECTOR:do_mouse_move( x,y )
	--self:print( "BU:is_click_double() "..self:is_click_double()) 
	oo.getsuper(SELECTOR).do_mouse_move( self, x,y )
	--hack to avoid problem after double clidfck with synchronous file dialog
	--if not self:is_click_double() and not self:is_click_triple() then
	if self:is_uif_running() then
		--self:print( "UIF case" )
		--self:__do_mouse_move_slider_base( x,y, 1 )
	else
		self:__do_mouse_common( x,y )
		if self:is_selection_multiple() then
			local i = self:__get_item_sel()
			--self:print( "multiple sel ui "..i )
			local b = self.__sel_value_at_click_down
			if i~=nil and i==self.__sel_item_at_click_down then b = not b end
			self:set_item_selected_multiple( self.__sel_item_at_click_down, b  )
		end
	end
	--end
--	self:print( "do_mouse_move" )
end

function SELECTOR:do_click_down( x,y )
--	self:print( x..", "..y )
	oo.getsuper(SELECTOR).do_click_down( self, x,y )
	self:__do_mouse_common( x,y )
	if self:is_selection_multiple() then
		--todo refine later
		local i = self:__get_item_sel()
		local b = self:is_item_selected( i )
		self.__sel_item_at_click_down = i
		self.__sel_value_at_click_down = b
		self:set_item_selected_multiple( i, not b )
	end
end

function SELECTOR:do_click_up( x,y )
	--aaa.print_method()
	oo.getsuper(SELECTOR).do_click_up( self, x,y )
	local min,max = self:get_min_max()
	self:print( "SELECTOR:do_click_up() value is "..self:get_value().." in ["..min..","..max.."]" )
end

function SELECTOR:update()
	--aaa.print_method()
	oo.getsuper(SELECTOR).update(self)
end

-- NB
--
function SELECTOR:get_nb()		return self.__nb				end
function SELECTOR:get_nb_uv()	return self.__nb_u, self.__nb_v	end

function SELECTOR:__set_nb( nb_u, nb_v )
	self.__nb_u	= nb_u
	self.__nb_u_over = 1 / nb_u

	nb_v = nb_v or 1
	self.__nb_v	= nb_v
	self.__nb_v_over = 1 / nb_v

	self.__nb = nb_u * nb_v
	return self.__nb
end
function SELECTOR:set_nb( nb_u, nb_v )
	local nb = self:__set_nb( nb_u, nb_v )
	--self:print( "nb is "..nb )
	self:set_min_max( 1, nb )
--todoq
--	self:set_value( 2 )	--todo this avoid bug but it is dirty
--	self:set_value( 1 )
	return self
end
function SELECTOR:set_nb_min_0( nb_u, nb_v )
	local nb = self:__set_nb( nb_u, nb_v )
	--self:print( "nb is "..nb )
	self:set_min_max( 0, nb-1 )
	--todo setting to 0 or 1 is a problem  and should be solve default behavior is unclear
	--self:set_value( 0 )	--todo this avoid bug but it is dirty
	return self
end
function SELECTOR:__get_iu_iv( i )
	i = i-1	--todo why not self:get_min() ?
	local iu = i % self.__nb_u
	local iv = (i-iu) * self.__nb_u_over
	return iu,iv
end
function SELECTOR:get_item_lb( i )
	local iu, iv = self:__get_iu_iv( i or self:__get_item_sel() )
	return iu * self.__nb_u_over  - .5 , (self.__nb_v-iv-1) * self.__nb_v_over - .5
end
function SELECTOR:get_item_rect( i )
	local iu, iv = self:__get_iu_iv( i or self:__get_item_sel() )
	local dx = self.__nb_u_over
	local dy = self.__nb_v_over
	local x,y = iu * dx  - .5 , (self.__nb_v-iv-1) * dy - .5
	return { l=x, r=x+dx, b=y, t=y+dy }
end


-- ITEM TEXT
--
function SELECTOR:get_item_text( i )
	i = i or self:__get_item_sel()
	local it = self.__item_text
	if it then
		local it = it[i]
		return it and it.str
	end
end

--unused finally
-- SELECTOR.doc.__build_table_and_count = "(...) private helper function to get the variadic arguments as a table or the first variadic arg if only one and it is a table"
-- function SELECTOR:__build_table_and_count(...)
-- 	local nb = select( '#', ... )
-- 	if nb == 1 then
-- 		local arg1 = select( 1, ... )
-- 		if type(arg1) == "table" then
-- 			return arg1, #arg1
-- 		else
-- 			return {arg1}, 1
-- 		end
-- 	end
-- 	return pack(...), nb
-- end
SELECTOR.doc.set_item_text = "(i, text1 [,text...]) set item text starting at item (position) i"
function SELECTOR:set_item_text( i, ... )
	local count = select( "#", ... )
	--self:print( "count is "..count )
	if count > 0 then
		local t = self:get_table_always( "__item_text" )
		i = i - 1
		local nb_max_asked = count + i
		if nb_max_asked > self.__nb then
			self:set_nb( nb_max_asked )
		end
		for ib = 1, count do
			local id = i+ib
			--was
			--if id > self.__nb then
			--	self:box_debug( "set_item_text() on id "..id.."\nbut we have only "..self.__nb.." items" )
			--end
			local str = select( ib, ... )
			--self:print( "str is "..str )
			if str and str ~= "" then
				local _, line_nb = string.gsub( str, "\n", "\n" )
				t[id] = { str = str, line_nb = line_nb + 1 }
			else
				t[id] = nil
			end
		end
	end
	return self
end

function SELECTOR:__set_item_text_from_nb( start, stop, off, pre, step )
	start = start or 1
	stop = stop or self.__nb
	for i = start,stop,(step or 1) do
		local i_out = i + off
		self:set_item_text( i, pre and (pre..i_out) or tostring(i_out) )
	end
	return self
end
function SELECTOR:set_item_text_from_nb( start, stop, pre, step )
	return self:__set_item_text_from_nb( start, stop, 0, pre, step )
end
function SELECTOR:set_item_text_from_nb_minus_1( start, stop, pre, step )
	return self:__set_item_text_from_nb( start, stop, -1, pre, step )
end

-- ITEM DATA
--
SELECTOR.doc.set_item_data = "(i, text1 [,text...]) set item data starting at position i"
function SELECTOR:set_item_data( i, ... )
	local count = select( "#", ... )
	if count > 0 then
		local tab = self:get_table_always( "__item_data" )
		i = i - 1
		for ib = 1, count do
			local id = i + ib
			if id > self.__nb then
				self:box_debug( "set_item_data() on id "..id.."\nbut we have only "..self.__nb.." items" )
			end
			tab[ id ] = select( ib, ... )
		end
	end
	return self
end

--	return data
--		or text if no data
--		or value if no text
function SELECTOR:get_item_data()
	local val = self:get_value()
	--self:print( "kjk "..val.." "..math.floor(val+.0000001) )
	--done in BALUE
	--val = math.floor(val+.0000001) --trouble with 17 on text_bind selector of the MEU

	local sym = self.__item_data
	--table.print( sym, "sym" )
	--self:print( "kjk2 "..type(val+1).." "..val.." "..(val+1) )
	--val = math.floor(val) --trouble with 17 on text_bind selector of the MEU
	--self:print( "kjk3 "..sym[val+1].." "..sym[val+2].." "..sym[val+3] )
	local min, max = self:get_min_max()
	local off = min==0 and 1 or 0

	if sym then
		sym = sym[ val + off ]
		if sym then return sym end
	end

	sym = self.__item_text
	--table.print( sym, "sym" )
	--if val == 16 then self:print( "== 16" ) end
	--self:print( val.." "..sym[val-1].." "..sym[val].." "..sym[val+1] )
	--self:print(  (val+off).." off("..off..") "..sym[val].." "..sym[val+off] )
	sym = sym and sym[ val + off ]
	if sym then return sym.str end

	return val
end

-- ITEM SYMBO
--
function SELECTOR:set_item_symbo( i, ... )
	local count = select( "#", ... )
	if count > 0 then
		local tab = self:get_table_always( "__item_symbo" )
		i = i - 1
		for ib = 1, count do
			local id = i + ib
			if id > self.__nb then
				self:box_debug( "set_item_symbo() on id "..id.."\nbut we have only "..self.__nb.." items" )
			end
			tab[ id ] = select( ib, ... )
		end
	end
	return self
end
--	return data
--		or text if no data
--		or value if no text
function SELECTOR:get_item_symbo( id )
	local val = self:get_value()

	local sym = self.__item_symbo
	if sym then
		local min, max = self:get_min_max( id )
		local off = min==0 and 1 or 0
		sym = sym[ val + off ]
		if sym then return sym end
	end
end

-- KEYBOARD
--
function SELECTOR:do_key( key )
	self:print_do_key( "SELECTOR", key )

	local b_key_used = true
	self:print( "do_key( "..key.." )" )
	local c = string.char(key)
	if		c == "-" then		self:add_value_ui( -1 )
	elseif	c == "+" then		self:add_value_ui(  1 )
	elseif	c == "/" then		self:set_value_ui( self:get_value()/2 )
	elseif	c == "*" then		self:set_value_ui( self:get_value()*2 )
	else	b_key_used = false
	end

	return b_key_used or oo.getsuper(SELECTOR).do_key( self, key )
end

function SELECTOR:do_key_special( key )
	self:print_do_key_special( "SELECTOR", key )

	return oo.getsuper(SELECTOR).do_key_special( self, key )
end


function SELECTOR.init_instance(self)
 	oo.getsuper(SELECTOR).init_instance(self)	--todo why for BUI: remove asymmetry
 	self:set_min_max_strict( true )
 	--self = oo.getsuper(SELECTOR):create( x, y, sx, sy, angle )
 	self:set_value_type_integer( true )
	self:set_value( 1 )	--to avoid a zero

 	return self
end

function SELECTOR:create( name, rect )
	local self = SELECTOR:create_instance( name, rect )
	self:set_nb( 2 )
	return self
end

--todo fo it better with a draw_title
function SELECTOR:get_title()
	return self:get_text()
end

function SELECTOR:set_draw_grid_line( b )	self.__b_draw_grid_line = b 	end

function SELECTOR:draw_back()
	--local draw_by_value = self.__draw_by_value
	if self.__draw_by_value ~= nil then
		--local i_sel = self:get_value()
		--self:print( i_sel .." -- ".. i_sel_off )
		self:__draw_back_by_value()
	else
		self:gol_draw_rect_back()
	end
end

local multiple_factor = .0125
function SELECTOR:draw_items()
	local x,y
	local dx = self.__nb_u_over
	local dy = self.__nb_v_over

	--table.print( item_sel )
	self:gol_color_sel()
	if self:is_selection_multiple() then
		local item_sel = self.__sel_item
		--table.print( item_sel, self.."  multiple selector", 2 )
		local mx = dx * multiple_factor
		local my = dy * multiple_factor
		dx = dx - mx
		dy = dy - my
		for i = 1, self:get_nb() do
			local item = item_sel[i]
			if item  then
				x,y = self:get_item_lb(i)
				aaa.draw_rect( x+mx,y+my, x+dx,y+dy )
			end
		end
	else
		local i_sel = self:__get_item_sel()
		x,y = self:get_item_lb(i_sel)
		aaa.draw_rect( x,y, x+dx,y+dy )
	end
end

function SELECTOR:draw()
	--aaa.debug.print_traceback()
	--if true then return end
	--self:print("get nb item : "..self:get_item_selected_nb() )

	local alpha = self:get_alpha_bu_to_draw()
	local dx = self.__nb_u_over
	local dy = self.__nb_v_over
	local b_mul = self:is_selection_multiple()

	if GA.b_spy then aaa.spy.push_range( "SELECTOR", 3 ) end
		--	Draw line
		if self.__b_draw_grid_line then
			local nb
			if b_mul then
				gol.set_line_width( 2 )
				gol.color_white( .5 * alpha )
				nb = self.__nb_u
				local o = dx * multiple_factor
				aaa.draw_lines_vert_nb(	nb,	-.5+o,		dx,	-.5, .5	)
				aaa.draw_lines_vert_nb(	nb,	-.5+dx-o,	dx,	-.5, .5	)
				nb = self.__nb_v
				o = dy * multiple_factor
				aaa.draw_lines_hori_nb(	nb,	-.5+o,		dy,	-.5, .5	)
				aaa.draw_lines_hori_nb(	nb,	-.5+dy-o,	dy,	-.5, .5	)
			else
				gol.set_line_width( BU.__draw_text_line_width * .5 )
				gol.color_white( .5 * alpha )
				nb = self.__nb_u - 1
				if nb > 0 then aaa.draw_lines_vert_nb(	nb,	-.5+dx, dx,	-.5, .5	) end
				nb = self.__nb_v - 1
				if nb > 0 then aaa.draw_lines_hori_nb(	nb,	-.5+dy, dy,	-.5, .5	) end
			end
		end
		self:draw_items()
		--todo this done quick but not fast
		--	SELECTION
	if GA.b_spy then aaa.spy.pop_range() end

	--	Text
	--	local dxt, dyt
	if TEXT:is_active() then
		if GA.b_spy then aaa.spy.push_range( "text", 5 ) end

			local b_title = self:is_text_draw()
			local t = self.__item_text

			local oy 			= dy * .15
			local item_text_sy	= dy * .55

			--item's text
			if t then
				local col1 = { .25, 0, .25, alpha }
				local col2 = { .8, 1, 1, alpha }
				local col
				local item_sel, i_sel
				if b_mul then
					item_sel = self.__sel_item
				else
					i_sel = self:__get_item_sel()
				end
				for i = 1, self:get_nb() do
					local item_text = t[i]
					if item_text then
						local str = item_text.str
						local line_nb = item_text.line_nb
						if b_mul then	col = item_sel[i] and col1 or col2
						else			col = i == i_sel and col1 or col2
						end
						gol.color( col )
						--if GA.b_spy then aaa.spy.push_range( str, 6 ) end
						local x,y = self:get_item_lb( i )
						y = y + oy
						if line_nb > 1 then
							self:draw_text_lrbt( str, x, x+dx, y+dy*.4, y+dy*.8, "center" )
						else
							self:draw_text_lrbt( str, x, x+dx, y, y+item_text_sy, "center" )	
							--self:draw_text_only( str, x+dx*.1, y+dy*.1, dxt, dyt )
						end
						--if GA.b_spy then aaa.spy.pop_range() end
					end
				end
			end

		--Title
			if b_title then
				local s = 1. - dx*.1
				local ox = self.text_u or 0
				local fx,fy = self:get_text_fxy()
				local xl = -.5 + dx*.1 + ox
				local xr =  xl + s * fx

				local y = .5 + oy*.5 + ( self.text_v or 0 )
				--+1/fx
				self:draw_text_color_sel()
				self:draw_text_lrbt( self:get_title(), xl, xr, y, y+item_text_sy*.8*fy )
			end

		if GA.b_spy then aaa.spy.pop_range() end
	end
	--aaa.draw_line( -.5, -.25, .5 , -.25 )
end

