-- a BU with ui/values
if CLASS.DECLARE( "BUI", BU,
		{
			__du = 1/16,		--	used in BUI:init_instance()
			__dv = 1/16,		--	used in BUI:init_instance()
			__b_meter = false,	--	used in BUI:init_instance()
			__b_show_value = false,
			--back_color			=	{	0,	0,	0 },
			--frame_color			=	{	1,	0,	0 },
			--front_color			=	{	1,	1,	1 },
			__b_preset_use = true,
			__color_back = BU:get_color_back_named("BUI"),
			__b_value_load_save = true
		}
	) then
	BUI:set_class_status_doc(	CLASS.STATUS.GABU,
								"BU handling one or severals values, in fact BU with a User Interface: BU with UI -> BUI",
								"these values are kept in BALUE objects" )
end

-- UIF
--
function BUI:add_uif_zones_base( zones, dist, x,y, b_skip_dialog )
	dist = dist or 0
	--oo.getsuper(BU_MEU).get_uif_zones(self)
	
	local b_bool = false
	local b_slide = true
	if self:is_value_type_bool() then
		b_bool = true
		b_slide = false
	elseif self:is_derived_from_class( BUTTON ) then
		if self:get_min()==0 and self:get_max()==1 then
			b_slide = false
		end
	end

	-- delegate Slide / Min-Max / True-False to the shared BU helper so MU and
	-- any other caller share the same canonical positions
	self:add_uif_zones_slide( zones, b_bool )
	if b_slide then
		if not b_skip_dialog then
			zones.Dialog	= zones.Dialog	or {	angle=3/4,		dangle=1/8,		dist=.7		}
		end
		dist = dist + 1
	end

	oo.getsuper(BUI).add_uif_zones_base( self, zones, dist, x,y )
	return zones
end

-- these 3 fns used for slider and selector
function BUI:__get_uif_u( uif )
	local uif = uif or self:get_uif_data()
	if not uif then
		aaa.debug.print_traceback()
		self:print_error( "SLIDER:__get_uif_u() ERROR no uif data, skip __get_uif_u()" )
		return
	end
	if not uif.x_begin then
		aaa.debug.print_traceback()
		self:print_error( "ERROR no self.x_begin, skip do_mouse_move( x,y )" )
		return
	end

	--local du = x - self.__x_start_local
	local u	
	local name_sel = BU:get_uif_name_sel( uif )
	--self:show( name_sel, "name_sel" ) 	
	-- if		name_sel=="min"	or name_sel=="false" then u=0
	-- elseif	name_sel=="max"	or name_sel=="true" then u=1
	-- else
	if	name_sel=="slide" then
		u = self.__uif_val_begin.val_cano
		if u then
			local du = uif.__du
			du = du * 4.
			if self:is_meter() then	u = u + du
			else					u = u + du / ( 1 - self.__du )
			end
		else			
			self:print_error( "No u we got a problem" )
		end
	else
		self:print_error( "in __get_uif_u() but name_sel is not slide, we got a problem" )
	end
	return u
end

function BUI:__set_from_u( u, id )
	if u then
		id = id or self:get_id_interacting()
		if id == 3 then
			self:set_value_cano( u, id )
		else
			if self.__str_slider_two_draw_step and self:is_meter_cross() then
				local val_lim = self:get_value_cano_clamped( 3 - id )
				if (u > val_lim and id == 1) or (u < val_lim and id == 2) then
					u = val_lim
				end
			end

			if aaa.keyboard.is_shift() then	self:set_value_cano_closest( u, id )
			else							--self:print( "UIF u is "..u )
											self:set_value_cano( u, id )
			end
		end
	end
end

function BUI:get_uif_bui()
	return self
end
-- was needed for SLIDER then now for selector when doing Slide
function BUI:gol_meter_color( alpha )
	alpha = alpha and (alpha * self:get_alpha_bu_to_draw()) or self:get_alpha_bu_to_draw()
	if self:is_ui_active() then
		gol.color( .5, 1, 1, alpha )
	else
		gol.color_yellow( alpha )
	end
end

function BUI:do_uif_command( uif )
	aaa.print_fn()
	local command = BU:get_uif_name_sel( uif )
	local b_used = true
	-- already done on the fly and on the right value when multiple values (SLIDER_TWO)
	if		command=="min" or command=="false" or command=="max" or command=="true" then
	elseif	command=="dialog"	then	self:edit_dialog_param()
	else
		b_used = false 
	end
	return GABU_OBJ.do_uif_command_with_super( self, b_used, uif, BUI )
end

function BUI:edit_dialog_param()
	local id = self:get_id_interacting()
	local param_ref = self:get_target_param( id )
	aaa.print_fn()
	local param_type	
	if not param_ref then
		param_type = self:is_value_type_integer() and "int32" or "double"
--		param.set_and_save( param_edit_ref, self:get_value(id), false )
	end
	--self:print( "param_ref_or_type is "..param_ref_or_type )
	ga:add_dialog_edit{ title="Edit "..self:get_dialog_name(), bu=self, balue_id=id, param_ref=param_ref, param_type=param_type, obj=self }
	return true
end

function BUI:do_dialog_hook( what, value, dialog_table )
	aaa.print_fn()
	local id = dialog_table.balue_id
	if what == "change_value" then
		--table.print( dialog_table, "BUI:do_dialog_hook() dialog_table" )
		--this is an easy way to do it but each edit will be stored and undo redo have to be check in edit dialog
		self:set_value_ui( value, id )
	elseif what == "open" then
	elseif what == "close" then
		if value == "ok" then
		elseif value == "cancel" then
		end
	end
	 
	return true
end

--done by BU now that we have do_copy/paste
function BUI:do_key(key)
	self:print_do_key( "BUI", key )

	local b_key_used = false
	local id = self:get_id_interacting()
	local b_trig = self.is_trig and self:is_trig()
	if key == 13 and not aaa.keyboard.is_ctrl() then
		if b_trig then
			b_key_used = self:do_fn( "click" )
			self:print_debug( "do_fn(\"click\") return "..b_key_used )		
		else
			self:flip_value_ui( id )
			b_key_used = true
		end	
	elseif not aaa.keyboard.is_alt() then
		if	key==42 or key==43 or key==45 or key==47 or key==46 then -- + - * / .
			if b_trig then	--we need than for BUTTON which is a derived class
				b_key_used = self:do_fn( "click" )
			end
			
			if not b_key_used then
				local new_val
				local old_value = self:get_value(id)
				aaa.print_fn()
				if type(old_value) == "boolean" then
					if		key==42 or key==43  then	new_val = self:get_max(id)
					elseif	key==45 or key==47  then	new_val = self:get_min(id)
					end
				else
					if key==45 or key==43 then	-- we do it separate to wrap around min/max
						self:add_value_ui( key==43 and 1 or -1, id )			--	+ -
						b_key_used = true
					elseif	key == 47 then		new_val = old_value / 2			--	/
					elseif	key == 42 then		new_val = old_value * 2			--	*
					elseif	key == 46 then		new_val = math.floor(old_value)	--	.
					end
				end
				if new_val then
					self:print( "new value is "..new_val )
					b_key_used = true
					self:set_value_ui( new_val, id )
				end
			end
		end
	end

	return b_key_used or oo.getsuper(BUI).do_key( self, key )
end

function BUI:do_key_special( key )
	self:print_do_key_special( "BUI", key )

	local b_key_used = false
	--todo_jean in certain case we should pick an action like "set_min"/"set_max" and pass it to the objet thru a do_action method
	--             so we can use the same thing in menus ou uif for example
	--             the enter key for example trigger a "flip_value"
	local id = self:get_id_interacting()
	local new_val
	if		key == 279 then		new_val = self:get_max(id)	-- PAGE_UP
	elseif	key == 280 then		new_val = self:get_min(id)	-- PAGE_DOWN
	elseif	key == 277 then		new_val = self:get_def(id)	-- HOME
	elseif	key == 278 then		new_val = self:get_ina(id)	-- END
--	elseif	key == 274 then		self:add_value_ui( -1 )		--	<-
--	elseif	key == 275 then		self:ui_sdd_value( 1 )		--	->
	end
	if new_val then
		b_key_used = true
		aaa.print( "--------------------- new_val is "..new_val )
		self:set_value_ui( new_val, id )
	end

	return b_key_used or oo.getsuper(BUI).do_key_special( self, key )
end

function BUI:get_item_text( i )			local bu = self:get_selector()	if bu then return bu:get_item_text(i)			end		end
function BUI:set_item_text( i, ... )	local bu = self:get_selector()	if bu then return bu:set_item_text( i, ... )	end		end

function BUI:get_item_data()			local bu = self:get_selector()	if bu then return bu:get_item_data()			end		end
function BUI:set_item_data( i, ... )	local bu = self:get_selector()	if bu then return bu:set_item_data( i, ... )	end		end

function BUI.init_instance(self)
	--aaa.print_fn( "BUI.init_instance" )
	oo.getsuper(BUI).init_instance(self)
--	self:box_debug( "init_instance()" )

	self:disable_mobile()
	
	--todo check this
	-- this is a hack we can set these 3 to change the defaults at init
	self.__du = BUI.__du
	self.__dv = BUI.__dv
	self.__b_meter = self.__b_meter
	--todo_lio all field should be regrouped in a table instead of having a table by
	self.__balues = {}
	return self
end

BUI.doc.set_id_interacting = "( id ) store id for balue concerned by the cur/last interaction"
BUI.doc.get_id_interacting = "() get which id balue is concerned by the cur/last interaction"
function BUI:set_id_interacting( id )
	aaa.print_fn()
	self.__id_interacting = id
end
function BUI:get_id_interacting()		return self.__id_interacting or 1	end

--todo
--	have rendering option
--		LINE (discrete)
--		BASIC what we have now
--		TEXTURED
--	show previous position until Bui is released
--	have an esc and a an undo
--	a power or a gain/bias
function BUI:__get_balues()
	-- local target = self:get_target_bu()
	-- if target then
	-- 	return target:__get_balues()
	-- end
	-- local selector = self:get_selector()
	-- if selector then
	-- 	return selector.__balues
	-- end
	return self.__balues
--	was just this
--	return self.__balues
end
function BUI:__get_balue( id )
	if id then
		--todo 	check if we need something here
		--		id = math.max( self.__val_nb, id )
	else
		id = 1
	end
	local balues = self:__get_balues()
	local balue = balues[id]
	if balue == nil then
		--todo this is just a check, limit is 2 for now but could change
		if id > 2 then
			aaa.debug.print_traceback()
			self:print_error( "BUI:__get_balue() id is too big " .. id )
		end
		--self:print( "BALUE:create id is "..id )
		balue = BALUE:create( self:get_name() .. "_" .. id, self, id )
		balues[id] = balue
	end
	return balue
end

--
--	VALUE
--
function BUI:set_value_cano( val, id )			self:__get_balue(id):set_value_cano(val)				end
function BUI:set_value( val, id )
	--aaa.print_fn()
	--self:print( "set_value( "..val..", "..id.." )" )
	self:__get_balue(id):set_value(val)
	return self
end

function BUI:set_value_ui(	val, id )			self:__get_balue(id):set_value_ui(	val,	id or 1 )	end
function BUI:add_value_ui(	inc, id )			self:__get_balue(id):add_value_ui(	inc,	id or 1 )	end
function BUI:flip_value_ui(      id )			self:__get_balue(id):flip_value_ui(			id or 1 )	end

--	canonical value (from the math term) standardized value in [0,1] when appropriate to represent [min,max] (should now be check)
function BUI:get_value_cano(id)					return self:__get_balue(id):get_value_cano()			end
BUI.doc.get_value_cano_clamped = "(id) like get_value_cano but clamp to 0 1"
function BUI:get_value_cano_clamped(id)			return self:__get_balue(id):get_value_cano_clamped()	end
function BUI:get_value(id)						return self:__get_balue(id):get_value()					end
function BUI:get_value_as_bool(id)				return self:__get_balue(id):get_value_as_bool()			end
function BUI:get_min(id)						return self:__get_balue(id):get_min()					end
function BUI:get_max(id)						return self:__get_balue(id):get_max()					end
function BUI:is_min_max_equal(id)				return self:__get_balue(id):is_min_max_equal()			end
function BUI:get_ina(id)						return self:__get_balue(id):get_ina()					end
function BUI:get_def(id)						return self:__get_balue(id):get_def()					end
function BUI:set_def( v, id )					return self:__get_balue(id):set_def( v )				end

function BUI:flip_value(id)
	--self:print( "id is "..id )
	--aaa.print_fn( "before is "..self:__get_balue():get_value() )
	return self:__get_balue(id):flip_value()
end

function BUI:set_value_cano_closest( val, id )
	local defs = self:get_values_def_table()
	--table.print( defs, "defs", 1 )
	if defs then
		--todo ajouter valeur courante
		self:__get_balue(id):set_value_cano_closest( val, defs )
	end
end


function BUI:set_show_value( b )				self.__b_show_value = b	return self		end
function BUI:is_show_value()					return self.__b_show_value				end

--	done quickly to clean up eventually with all the side effects on selector for example
--todo	merge with value int
--function BUI:set_value_nb( nb, id )			self:__get_balue( id ):set_value_nb( nb ) 		end

function BUI:set_value_type_integer(	b, id )	self:__get_balue(id):set_value_type_integer(b)			return self	end
function BUI:is_value_type_integer(		id )	return self:__get_balue(id):is_value_type_integer()		end
function BUI:set_value_type_bool(		b, id )	self:__get_balue(id):set_value_type_bool(b)				return self	end
function BUI:is_value_type_bool(		id )	return self:__get_balue(id):is_value_type_bool()		end
--
--	MIN MAX
--
--function BUI:set_value_to_def( id )						self:__get_balue( id ):set_value_to_def() 						end
--function BUI:flip_value_def_ina( id )						self:__get_balue( id ):flip_value_def_ina()						end

function BUI:set_min_max_value( min, max, value, id )
	self.__values_def_used = nil
	self:__get_balue(id):set_min_max_value( min, max, value )
	return self
end
function BUI:set_min_max( min, max, id )
	self.__values_def_used = nil
	self:__get_balue(id):set_min_max( min, max )
	return self
end

function BUI:get_min_max( id )				return self:__get_balue(id):get_min_max()	end
function BUI:clamp_min_max( id )			self:__get_balue(id):clamp_min_max()		return self	end
BUI.doc.set_min_max_strict = "( bool, id ) is set to true the BUI balue will be constraint in [min,max]"
function BUI:set_min_max_strict( b, id )	self:__get_balue(id):set_min_max_strict(b)	return self	end

function BUI:set_draw_by_value( val, mode  )
	local sel = self:get_selector()
	if sel then
		sel:set_draw_by_value( val, mode )
	end
	self.__draw_by_value = { value = val, mode = mode }
end
--
--	VALUES
--
function BUI:get_values()
	local balues = self:__get_balues()
	local nb = #balues
--	aaa.print_fn()
	if nb == 1 then
		return self:get_value(1)
	elseif nb == 0 then
		return
	else
		local tab = {}
		for i = 1,nb do
			tab[i] = self:get_value(i)	--todo value could be a table too ?
		end
		return tab
	end
end
function BUI:set_values( tab )
	--self:print( "BUI:set_values" )
	if type(tab) == "table" then
		--table.print( tab, "tab", 2 )
		local nb = #tab
		for id = 1, nb do
			--self:print( "id is "..id )
			self:set_value( tab[id], id )
		end
	else
		self:set_value( tab, 1 )
	end
	return self
end

--todo replace by a begin/end meacanism ? should work for groups too
--todo hierarchical ?
--todo check if we should get the prev values and pass it to add_to_undo_history
function BUI:set_values_ui( tab_or_value )
	self:print_debug( "BUI:set_values" )
	if type(tab_or_value) == "table" then
		self:set_values( tab_or_value )
		
		--todo we had prev
		self:add_to_undo_history()
		--self:add_to_undo_history( prev )

		--table.print( tab, "tab", 2 )
		-- local nb = #tab_or_value
		-- for id = 1, nb do
		-- 	self:print( "for id "..id.." value is "..tab_or_value[id] )
		-- 	self:set_value_ui( tab_or_value[id], id )
		-- end
	else
		self:set_value_ui( tab_or_value, self:get_id_interacting() )
	end
	return self
end

--
--	VALUE DEF
--
function BUI:add_values_def_table( t )
	self.__values_def_used = nil
	array.append_to( self:get_table_always("__values_def"), t )
	return self
end
function BUI:add_values_def( arg1, ... )
	if type(arg1)=="table" then
		return self:add_values_def_table( arg1 )
	else
		return self:add_values_def_table( pack(arg1,...) )
	end
end
function BUI:add_values_def_integer()
	local min, max = self:get_min_max()
	local t = {}
	local fmax = math.floor(max)
	for i = math.floor(min + 1), ((fmax == max) and (max - 1) or fmax) do
		t[#t + 1] = i
	end
	return self:add_values_def_table(t)
end
function BUI:add_values_def_integer_half()
	local min, max = self:get_min_max()
	local t = {}
	local fmax = math.floor(max)
	for i = math.floor(min + 1), ((fmax == max) and (max - .5) or fmax),.5 do
		t[#t + 1] = i
	end
	return self:add_values_def_table(t)
end

function BUI:get_values_def_table()
	local dst = self.__values_def_used
	if not dst then
		local t = self.__values_def
		local tmp = t and array.copy_simple(t) or {}
		local min, max = self:get_min_max()
		if min then
			tmp[#tmp + 1] = min
		end
		if max then
			tmp[#tmp + 1] = max
		end
		dst = table.build_array_with_unique_value(tmp)
		table.sort(dst)
		self.__values_def_used = dst
	end
	return dst
end

function BUI:get_values_def_next( cur )
	local i = self.__values_def_cur or 0
	local defs = self:get_values_def_table()
	local nb = #defs
	if i >= nb then
		i = 1
	else
		i = i + 1
	end
	local val = defs[i]
	if val == nil then
		table.print( defs, "__values_def has a problem", 2 )
	end

	if cur == val then
		if i >= nb then
			i = 1
		else
			i = i + 1
		end
		val = defs[i]
		if val == nil then
			table.print( defs, "__values_def has a problem", 2 )
		end
	end
	self.__values_def_cur = i
	return val
end
--lala
--[[
function BUI:read_target()
	local balues = self:__get_balues()
	for i = 1, #balues do
		balues[i]:read_target(i)
	end
end
--]]

--todo deal with all target I guess (Maa)
function BUI:free()
	app:unregister_for_target_update( self )
	local bu_menu = self:get_menu()
	if bu_menu then
		--self:print( "have a menu to free "..bu_menu )
		bu_menu:free()
		--local bu_win = bu_menu.__bu_window
		--if bu_win then
		--	self:print( "have a win to free  "..bu_win )
		--	bu_win:free()
		--end
	end
	oo.getsuper(BUI).free(self)
end

--	CHANGE VALUE
--

--todotex do it directly in BALUE
BUI.doc.set_function_on_value_change = "( fn, ... ) once set when a balue change\n"..
										"fn( ..., balue ) will be automaticly called\n"..
										"call it with nil obj to remove it.\n"..
										"use set_method_on_value_change() when possible is more flexible"
function BUI:set_function_on_value_change( fn, ... )
	local sel = self:get_selector()
	if sel then
		return sel:set_function_on_value_change( fn, ... )
	end
	self:set_function( "value_change", fn, ... )
end
BUI.doc.set_method_on_value_change = "( obj, method_name, ... ) once set when a balue change\n"..
										"obj[method_name]( obj, ..., balue ) will be automaticly called\n"..
										"call it with nil obj to remove it"
function BUI:set_method_on_value_change( obj, method_name, ... )
	local sel = self:get_selector()
	if sel then
		return sel:set_method_on_value_change( obj, method_name, ... )
	end
	self:set_method( "value_change", obj, method_name, ... )
end
--
function BUI:set_hook_on_value_change( fn )
	self.__hook_value_change = fn
end
function BUI:__do_value_change( balue )
	--self.__balue_changed = balue
	if self.__fn_table then
		self:do_fn( "value_change", balue )
	end

	--todo 2 mecanism it is to much
	if self.__hook_value_change then
		--self:print( "hook_value_change in BUI" )
		self.__hook_value_change( self, balue )
	end
	--self.__balue_changed = nil
end

--
--	NET
--
--[[
function BUI:__make_change_table()
	local bus = self:get_bus_up()
	if not bus then
		self:print_error( "BUI:__make_change_table() no bus up returned by get_bus_up()" )
		return
	end

	local name = self:get_name_lowercase()
	return { bus_name = bus:get_name_lowercase(), bu_name = name, val = self:get_value() }
end
--]]

--todomona
function BUI:send()
	-- local t = self:__make_change_table()
	-- if t then
	--	local 	
	-- 	--if aaa.net.is_remote() and app:is_master() then
	-- 	local str = 'BUI.change_slave_value( "' .. t.bus_name .. '", "' .. t.bu_name .. '", ' .. t.val .. " )"
	-- 	self:print("SEND : " .. str)
	-- 	aaa.net.lua_send( 0, 0, str )
	-- end
end
function BUI.change_slave_value( bus_name, bu_name, val )
	-- --if self:is_send() then return end
	-- local str = 'BUI.change_slave_value( "' .. bus_name .. '", "' .. bu_name .. '", ' .. val .. " )"
	-- self:print(str)
	-- local bu = ga:find_bu_by_name_lowercase( bus_name, bu_name )
	-- if bu then
	-- 	if not bu:is_send() then
	-- 		bu:set_value(val)
	-- 	end
	-- else
	-- 	aaa.print("BUI.change_slave_value() don't find bu")
	-- end
end

function BUI:set_meter(b)
	self.__b_meter = b
	return self
end
function BUI:is_meter()
	return self.__b_meter
end
function BUI:set_meter_ref_cano(val)
	self.__meter_ref = val
	return self
end
--todo deal with id here
function BUI:set_meter_ref(val)
	self:set_meter_ref_cano(self:__get_balue(1):convert_value_to_cano(val))
end
function BUI:get_meter_ref_cano()
	return self.__meter_ref
end


--
--	MIDI
--
--TODO deal with multiple value
function BUI:read_midi_control()
	local b_ret = false
	local plugged_in = self.__plugged_in
	if plugged_in then
		local midi = plugged_in.midi
		if midi then		
			local nb = #(self:__get_balues())

			--todo 2024 Sep we should perhaps deal at BALUE level
			for i = 1, nb do
				local ch = midi.ch
				local ctl = midi.ctl+i-1
				local v
--was trax connection
				-- if ch == -69 then
				-- 	local m = MEU.__meu_trax[ctl]
				-- 	if m then
				-- 		self:set_value( m:get_value_out(), i )
				-- 	end
				-- else
					--clamp_01 is a secu
					v =  clamp_01( aaa.midi.get_control( ch, ctl ) )
					self:set_value_cano( v, i )
					b_ret = true --todo	refine for several val
				--end
			end
		--self:print( "read_midi_control() : midi.get_control( "..midi.ch..", "..midi.ctl.." ) -> "..self:get_value_cano(i) )
		end
	end
	return b_ret
end

--lv unused form now
function BUI:write_midi_control()
	local plugged_in = self.__plugged_in
	if plugged_in then
		local midi = plugged_in.midi
		if midi then
			local nb = #(self:__get_balues())
			for i = 1, nb do
				local ch =  midi.ch
				if ch > 0 then
				--self:print( "aaa.midi.set_control( "..midi.ch..", "..(midi.ctl + i - 1)..", "..self:get_value_cano(i) )
					aaa.midi.set_control( ch, midi.ctl + i - 1, self:get_value_cano(i) )
				end
			end
		--self:print( "write_midi_control() : midi.set_control( "..midi.ch..", "..midi.ctl..", "..self:get_value_cano() )
		end
	end
end


--
--	CONNEX
--
--todolv
function BUI:remove_connex()
	local plugged_in = self.__plugged_in
	if plugged_in then
		local midi = plugged_in.midi
		if midi then
			plugged_in.midi = nil
			self:pop_text()
			--if gp then
			app:unregister_for_target_update( self )
		else
			local trax = plugged_in.trax
			if trax then
				trax.meu_src:disconnect_as_dst( self )
				plugged_in.trax = nil
				self:pop_text()
			else
				self:add_dialog_message( "not connected\nso can't be disconnected" )
			end
		end
	end
end
function BUI:set_connex( con )
--if true then return end
	if		not con or con.type == "remove"	then
		self:remove_connex()
	elseif	con.type == "midi"				then
		self:set_midi( con.ch, con.ctl )
	elseif con.type == "probe"				then
		local probe = self.probe
		if probe then
			probe(self)
		end
	elseif con.type=="trax_meu"				then
		local meu_trax = con.meu
		con.id_balue = self:get_id_interacting()
		--if self:set_trax_meu( meu_trax ) then
			meu_trax:process_connect_request( self, con )
		--end
	end
end

function BUI:get_midi()
	local plugged_in = self.__plugged_in
	if plugged_in then
		local midi = plugged_in.midi
		if midi then
			return midi.ch, midi.ctl
		end
	end
end
function BUI:set_midi( ch, ctl, b_short )
	--self:print_inverse( "BUI:set_midi( "..ch..", "..ctl.." )" )
	if ch and ctl then
		local plugged_in = self:get_table_always( "__plugged_in" )
		if plugged_in.midi then
			self:pop_text()
			plugged_in.midi = nil
			--self:add_dialog_message( "already connected with midi\nwill not connect again" )
		elseif plugged_in.trax then
			self:add_dialog_message( "already connected with midi\nwill not connect again" )
			return
		end

		plugged_in.midi = { ch = ch, ctl = ctl }
		--if gp then
		--todoq 2023 May Maa remove ? (done by BUI:update() called from BU:update() by BUS (check done only when draw ?))
		if app then
			app:register_for_midi_update( self )
		else
			--self:print_debug( "BUI:set_midi() No app to register midi update" )
		end
		--end
		local str = ch.."-".. ctl
--was for trax			if ch == -69 then	str = "T "..ctl
--			else				str = ch.."-".. ctl
--			end
		if not b_short then
			str = str..' | '..self:get_text()
		end
		self:push_text( str )
		self:read_midi_control()
	end
	return self
end

-- 2024 Sep unused
-- function BUI:set_trax_meu( meu_src )
-- 	self:print_inverse( "set_trax_meu( "..meu_src.." )" )
-- 	local plugged_in = self:get_table_always( "__plugged_in" )
-- 	if plugged_in.midi then
-- 		self:add_dialog_message( "already connected with midi\nwill not connect again" )
-- 	else
-- 		plugged_in.trax = { meu_src=meu_src }
-- 		local _, name = MEU_CTX.cur:split_meu_type_inst( meu_src:get_name() )
-- 		local str = "T"..name.."->"..self:get_name()
-- 		self:push_text( str )
-- 		return true
-- 	end
-- end

--todo deal with id ?
-- BUI.doc.__transfert_target_from = "use by set_menu and set_multiple need eventually to be removed after 2024 Sep changes"
-- function BUI:__transfert_target_from( bui_src )
-- 	local dst = self:__get_balue(1)
-- 	local src = bui_src:__get_balue(1)
-- 	dst:__transfert_target_from(src)
-- end

--todo adjust to the midi method and update (in particular sharingf a target should work fine)
BUI.doc.set_target_param = "( param_ref, id, comment ) the param_ref will become the value for the balue id\n"..
							"comment will be set to the param (the BUI name will be used if comment is nil)\n"..
							"a nil param_ref will remove the link balue have with a param"
function BUI:set_target_param( param_ref, id, comment )
	comment = comment or self:get_name()
	self:__get_balue(id):set_target_param( param_ref, nil, comment )
	--todo if we call BUI:set_target() we don't pass here
	if param_ref then
		if param.is_type_real(param_ref) then
		else
			self:set_meter(false)
		end
	end
	return self
end
function BUI:get_target_param( id )	return self:__get_balue(id):get_target_param()	end

BUI.doc.set_target_obj_param = "( obj, param_name, id, comment ) same as set_target_param()\n"..
							"with different argument to specify the param"
function BUI:set_target_obj_param( obj, param_name, id, comment )
	local param_ref = param.get_ref( obj, param_name )
	self:set_target_param( param_ref, id, comment )
end
function BUI:set_target_lua( tab, key, val, id )
	self:__get_balue(id):set_target_lua( tab, key, val )
	return self
end
function BUI:set_target( tab_or_ref, name, val, id )
	self:__get_balue(id):set_target( tab_or_ref, name, val, self:get_name() )
	return self
end

--todonow how this work for multiple values e.g. SLIDER_TWO
function BUI:update_target_midi()
	local b_contact = self:is_contact()
	if b_contact then
		self:write_midi_control()
	else
		if self:read_midi_control() then
			b_contact = true -- force update of target
		end
	end
end

--
--	MIDI UI and soon connection UI
--
--todo we work with the class now we have to create instances

--	prepare the connection
function BUI:__store_connex_next( tab )
	--aaa.print_fn()
	if tab then
		--table.print( tab, "store connect next", 1)
		local bu = tab.bu_src
		if bu then
			bu:push_color_back_busy()
		end
	end
	BUI.__connex_next = tab
end

function BUI:__set_do_on_click_next( tab )
	--table.print( tab, "connection table", 2 )
	local do_type = tab.type
	if do_type == "midi" then
		if tab.b_BUI then
			local t = BUI.__connex
			table.print( t, "BUI.__connex", 2 )
			tab.ch = t.bu_ch:get_value()
			tab.ctl = t.bu_slice:get_value() * 8 + t.bu_ctl:get_value()
		end
		self:__store_connex_next( tab )
	elseif do_type == "trax_meu" then
		if not tab.meu then
			self:print_error( "need a meu to do this" )
		else
			self:__store_connex_next( tab )
		end
	elseif do_type == "remove" then
		self:__store_connex_next( tab )
	elseif do_type == "probe" then
		self:__store_connex_next( tab )
	else
		self:print_error( "BUI:__set_do_on_click_next() do not deal yet with "..do_type )
	end
end

--	do it here
function BUI:try_to_connect()
	local con = BUI.__connex_next
	local b_ret = false
	if con then
		local bu = con.bu_src
		if bu then 
			bu:pop_color_back()
			--con.bu_src = nil
		end
		if self.__b_no_connect then
			self:add_dialog_message( "this BU refuse connection" )
		else
			self:set_connex( con )
			b_ret = true
		end
		-- remove 
		BUI:__store_connex_next( nil )
	end
	return b_ret
end

--	but really ask for it here
function BUI:do_click_down( x,y )
	--aaa.debug.print_traceback()
	if self:try_to_connect() then
	else
		oo.getsuper(BUI).do_click_down( self, x,y )
	end
end

-- function BUI:do_click_up( x,y )
-- 	--aaa.debug.print_traceback()self:print( "BUI:do_click_up "..self:get_value_cano(2) )
-- 	--self:print( "BUI:do_click_up "..self:get_value_cano(1) )
-- 	oo.getsuper(BUI).do_click_up( self, x,y )
-- 	--self:print( "BUI:do_click_up after "..self:get_value_cano(1) )
-- end

function BUI:define_ui_midi()
	local bus = BUS:create("MIDI")

	bus:init_begin()
	--
	--	MIDI
	--
	local x = 0
	local SX = 1.
	local y = .25
	local SY = .125 * .8
	local DY = SY * .5

	local bu

	BUI.__connex = {}
	bu = bus:add_selector( "Channel",	{x,y, 	SX,SY}	)
		--bu:print( "value before "..bu:get_value() )
		bu:set_nb( 10 )
		--bu:print( "value after "..bu:get_value() )
		bu:set_item_text_from_nb()
		bu.__b_no_connect = true
		BUI.__connex.bu_ch = bu
	y = y - SY * 1.5 - DY * 2

	-- we need have of prev + half of current
	bu = bus:add_selector( "Slice",		{x,y, 	SX,SY*2}	)
		--bu:print( "value before "..bu:get_value() )
		bu:set_nb_min_0( 8,2 )
		bu:set_value(0)
		--bu:print( "value after "..bu:get_value() )
		bu:set_item_text_from_nb()
		bu.__b_no_connect = true
		BUI.__connex.bu_slice = bu
	y = y - SY - DY * 2

	bu = bus:add_selector( "Control", 	{x,y, 	SX,SY}	)
		bu:set_nb_min_0(8)
		bu:set_item_text_from_nb_minus_1()
		bu.__b_no_connect = true
		BUI.__connex.bu_ctl = bu
	y = y - SY - DY

	bu = bus:add_trig( "CONNECT", {x - SX*.25,y,	 SX*.45,SY} )
		bu:set_method_on_click( BUI, "__set_do_on_click_next", { type="midi", bu_src=bu, b_BUI=true } )
		bu.__b_no_connect = true

	bu = bus:add_trig( "clear",   {x + SX*.25,y,	SX*.45, SY} )
		bu:set_method_on_click( BUI, "__set_do_on_click_next", { type="remove", bu_src=bu } )
		bu.__b_no_connect = true

--	bu = bus:add_trig( "TRAX CONNECT", {x - SX*.25, y - 4.4 * DY - 3 * DDY, SX * .5, DY} )
--		bu:set_method_on_click( BUI, "__set_do_on_click_next", { type="trax_meu", bu_src=bu } )
--		bu.__b_no_connect = true

	bus:init_end()
	return bus
end

function BUI:probe()
	local min, max = self:get_min_max()
	aaa.show( min.." "..max, self )
end

--
--	MENU stuff was in button
--
--
--	SELECTOR
--
-- used for BU_MENU and BUTTON (multiple case)
--todo perhaps we should clarify
function BU:set_selector( bu )
	--self:print( ":set_selector( "..bu.." )" )
	--aaa.debug.print_traceback()
	self.__bu_selector = bu
end
function BU:get_selector()			return self.__bu_selector			end

function BU:set_text_selector( b )	self.__b_text_from_selector = b		return self		end
function BU:is_text_selector()		return self.__b_text_from_selector	end

function BU:set_target_bu( bu )
	--local min,max = self:get_min_max()
	--self:print( min, max )
	if bu then
		if self.__balues_original then
			self:print( "set_target_bu() __balues_original already defined" )
		else
			self.__balues_original = self.__balues
		end
		self.__balues = bu.__balues
	else
		if self.__balues_original then
			self.__balues = self.__balues_original
			self.__balues_original = nil
		else
			self:print_error( "set_target_bu() __balues_original should exist" )
		end
	end
end
--function BU:get_target_bu()			return self.__target_bu				end

BUI.doc.set_menu = "( tab_name_or_selector, name, val ) attach a menu to the BUI,\n"..
					"tab_name_or_selector set the content of this menu (see BU_MENU:create()),\n"..		
					"name is the name of the BU_MENU created, if nil the BUI name will be used,\n"..	
					"val if passed is used set the value of this BUI once the memu is created."		
function BUI:set_menu( tab_name_or_selector, name, val )
	local nb
	--table.print( tab_name_or_selector, "tab_name_or_selector", 1 )
	local b_selector_in = CLASS.is_gabu_obj( tab_name_or_selector, SELECTOR )
	if b_selector_in then
		nb = tab_name_or_selector:get_nb()
	else
		nb = #tab_name_or_selector
	end
	--self:box_debug( "selector have "..nb.." item" )
	--self:box_debug( "set_menu of "..self:get_name().." with menu name "..name )
--	self:set_min_max( 1, nb )	--todo range [0,nb] ?

	local bu_menu = BU_MENU:create( name or self:get_name(), self, tab_name_or_selector )
		self:set_method_on_click( bu_menu, "flip_menu_display" )
		local bu_selector = bu_menu:get_selector()
--		self:set_min_max( bu_selector:get_min_max() )

		--todonow check this
		--bu_selector:__transfert_target_from( self )

		self.__bu_menu = bu_menu
		self:set_selector( bu_selector )
		self:set_target_bu( bu_selector )
		bu_selector:set_preset_use( false )	-- avoid bu_selector saving preset

	if not b_selector_in then
		bu_selector:set_nb( 1, nb )
	end
	if val then
		self:set_value( val )
	end

	return bu_menu
end

function BUI:is_menu()  return self.__bu_menu ~= nil	end
function BUI:get_menu()	return self.__bu_menu 			end


function BUI:update()
	if self:is_active() then
		self:update_target_midi()
	end

	--self:print( "BUI:update()" )
	--aaa.debug.print_traceback()
	local bu_menu = self:get_menu()
	if bu_menu then
		--	self:print( "BU_MENU:__update_presence()" )
		bu_menu:__update_presence()
	end

	--self:print( "update()" )
	oo.getsuper(BUI).update(self)
end