if CLASS.DECLARE( "BALUE" ) then
	BALUE.__b_type_bool = false
	BALUE:set_class_status_doc(	CLASS.STATUS.GABU,
								"abstraction handling value used by BUI" )
end

function BALUE:init( bu, id )
	self.__bu			= bu
	self.__min			= 0
	self.__max			= 1
	self.__ina			= 0
	self.__def			= 1
	self.__id			= id
--	self.__value_nb		= 0		-- nil say value is not integer
--	self.__value_cano	= 0		-- have to be nil at this stage
--	self.__value		= 0
end

function BALUE:create( name, bu, id )
	local self = BALUE:create_instance( name )
	self:init( bu, id )
	return self
end

--
--	MIN MAX
--
function BALUE:get_min()			return self.__min 					end
function BALUE:get_max()			return self.__max 					end
function BALUE:get_min_max()		return self.__min, self.__max		end
function BALUE:is_min_max_equal()	return self.__min == self.__max 	end

function BALUE:clamp_min_max()
	local val = self:get_value()
	if val  then
		if		val < self.__min then	self:set_value(self.__min)
		elseif	self.__max < val then	self:set_value(self.__max)
		end
	end
end
--todo move to set_clamp or set_clamped and propagate
function BALUE:set_min_max_strict( b )
	if b then
		self.__b_min_max_strict = true
		self:clamp_min_max()
		--self:box_debug( "Okhh" )
	else
		self.__b_min_max_strict = nil
	end
end
function BALUE:is_min_max_strict()
	--self:print( "min_max_strict is "..(self.__b_min_max_strict or (BU.__b_uif_on_shift and not aaa.keyboard.is_shift()) ) )
	--self:print( "is_min_max_strict "..self.__b_min_max_strict )
	return self.__b_min_max_strict --or ( BU.__b_uif_on_shift and not aaa.keyboard.is_shift())
end

--
-- integer / step ?
--
function BALUE:__set_value_nb( nb )
	if nb then
		--aaa.debug.print_traceback( "BALUE:set_value_nb()" )
		--self:print( "__set_value_nb( "..nb )
		if nb > 1 then
			self.__value_nb = nb
			self.__value_step_factor  = 1. / (nb-1)
		else	--we need to fill un to keep selector (and other) as integer value in any case
			self.__value_nb = 1
			self.__value_step_factor  = 1.
		end
	else
		self.__value_nb = nil
	end
end
function BALUE:set_value_type_integer( b )
	if b then
		local min, max = self:get_min_max()
		self:__set_value_nb( max - min + 1 )
	else
		self:__set_value_nb( nil )
	end
	return self
end
function BALUE:is_value_type_integer()		return self.__value_nb ~= nil 	end

--
--	BOOLEAN
--
--todo Bool and/or integer ?
function BALUE:set_value_type_bool( b )
	local b_old = self.__b_type_bool
	if b_old ~= b then
		self.__b_type_bool = b
		-- self:print( "type bool is now "..b )
	end
end
function BALUE:is_value_type_bool()			return self.__b_type_bool		end
--
--	TARGET
--
BALUE.doc.__transfert_target_from = "( balue_src ) use by set_menu and set_multiple need eventually to be removed after 2024 Sep changes"
function BALUE:__transfert_target_from( balue_src )
	self.__targets = balue_src.__targets
	balue_src.__targets = nil
end

function BALUE:__get_targets_always()
	return self:get_table_always( "__targets" )
end
function BALUE:get_target_param()
	local targets = self.__targets
	if targets then
		return targets.param
	end
end
function BALUE:set_target_param( param_ref, val, comment )
	--self:print( "BALUE:set_target_param() : Comment is "..comment )
	--aaa.print_fn()

	if param_ref then
		local targets = self:__get_targets_always()
		targets.param = param_ref

		local param_type = param.get_type( param_ref )
		--debug
		-- if val~=nil then
	 	-- 	aaa.print_fn()
		--  	self:print( val.." for "..param_type )
		-- end
		if param.is_type_bool( param_ref ) then
			self:set_value_type_bool( true )
		end

		if comment then
			param.set_comment( param_ref, "-"..comment )
		end
		--local BIG_VALUE = 1000000.
		--local tmp --hack
		local min = param.get_min( param_ref )
		local max = param.get_max( param_ref )

--		targets.__min = min
--		targets.__max = max
		-- when we deal with param big / infinite value we don't want to set min max from C param
		local limit = 256*256*256*128 -- we were using param.onfini before
		if min <= -limit+10 then min = nil end
		if max >= limit-10 then max = nil end
		--set min max if it have sense
		if min or max then
			--aaa.box_debug( "BIG_VALUE for "..param.get_name(param_ref) )
			--self:print( "cause is here" )
			self:set_min_max( min, max )
		end

		self.__ina = param.get_ina( param_ref )
		self.__def = param.get_def( param_ref )

		if param.is_type_real( param_ref ) then
			self:set_value_type_integer( false )
		else
			self:set_value_type_integer( true )
			local nb = self.__max - self.__min
			--if nb < 64 then
				self:__set_value_nb( nb+1 )
			--end
		end

		if val~=nil then
			val = self:set_value( val )
		end
	else
		local targets = self.__targets
		if targets then
			if targets.lua then -- incase one day we deal with both target at the same
				if targets.param then
					targets.param = nil
				end
			else
				self.__targets = nil
			end
		end
	end
end

function BALUE:set_target_lua( tab, key, val )
	if tab then
		local targets = self:__get_targets_always()
		if key == nil then
			local how = tab.how
			if how and how=="method_v0" then
				targets.lua = tab
				--todo deal with val ?
			else
				self:print_error( "in BALUE:set_target_lua() we have a table (not a procedural one) but no key" )
				aaa.debug.print_traceback()
			end
		else
			if val==nil then
				-- tried in 2025 January but is seems it hase implecation which fick up important values
				-- if type(tab[key]) == "boolean" then
				-- 	self:set_value_type_bool( true )
				-- end

				if self:is_value_type_bool() then
					val = self:get_value_as_bool()
				else
					val = self:get_value()
				end
				if self.b_dbg_value then
					self:print( "in BALUE:set_target_lua() val was nil so we self:get_value() and it is now "..val )
				end
			else
				--set_value will do it
				--if type(val) == "boolean" then
				--	self:set_value_type_bool( true )
				--end
			end

			--make sure to invalidate these for later set_value
			self.__value = nil
			self.__value_cano = nil

			targets.lua = { tab=tab, key=key }
			tab[key] = val
			--local val_cur = tab[key]
			--todo solve other issues before...
			-- if type(tab[key]) == "boolean" then
			-- 	self:set_value_type_bool( true )
			-- end

	-- 		val = val or tab[key] or self:get_min()
			if val ~= nil then
				if self.b_dbg_value then
					self:print( "in BALUE:set_target_lua() will set_value( "..val.." )" )
	 				self:set_value( val )
				 	self:print( "in BALUE:set_target_lua() after case target lua set_value( "..val.." ) get_value() return "..self:get_value() )
	 				self:print( "                                                target is "..tab[key] )
				else
	 				self:set_value( val )
				end

	-- 	--	else
	-- --			self:print( "will set_value to "..self:get_min() )
	-- --			self:set_value( self:get_min() )
 			end

			if self.verbose >= 1 then self:print( "------- BALUE:set_target_lua( "..key..", "..tostring(tab[key]).." )" ) end
		end
	else
		local targets = self.__targets
		if targets then
			if targets.param then -- incase one day we deal with both target at the same
				if targets.lua then
					targets.lua = nil
				end
			else
				self.__targets = nil
			end
		end

		if val ~= nil then
			self:set_value( val )
		end
		if self.b_dbg_value then
			self:print( "BALUE:set_target_lua after case no target set_value( "..val.." ) get_value() return "..self:get_value() )
		end
	end

end

--todo process bool value

function BALUE:set_target( tab_or_ref, name_or_param_ref, val, comment )
	--self:print( "Comment is "..comment )
	if tab_or_ref then
		local typ = type( tab_or_ref )
		if typ == "table" then
			self:set_target_lua( tab_or_ref, name_or_param_ref, val )
		else
			local par
			if aaa.obj.is_ref_no_error( tab_or_ref ) then
				par = param.get_ref( tab_or_ref, name_or_param_ref )
			elseif param.is_ref_no_error( tab_or_ref ) then
				par = tab_or_ref
			else
				self:print_error( tab_or_ref.." should be a ref, but it is not." )
				aaa.debug.print_traceback()
			end
			if par then
				self:set_target_param( par, val, comment )
			end
		end
	else
		local par
		if name_or_param_ref and param.is_ref_no_error( name_or_param_ref ) then
			par = name_or_param_ref
		else
			aaa.debug.print_traceback()
			self:print_debug( name_or_param_ref.." is not a param ref, probably a nil object was passed" )
		end
		if par then
			self:set_target_param( par, val, comment )
		end
	end
end

--todo check 2021 July done for BU_TEXT
function BALUE:is_target()
	return self.__targets ~= nil
end
--todoopt
function BALUE:__get_target_value()
	local targets = self.__targets
	if targets then
		local param_ref = targets.param
		if param_ref then
			if self.__b_type_bool then
				return param.get_bool( param_ref )
			else
				return param.get( param_ref )
			end
		else
			local t = targets.lua
			if t then
				local val
				local how = t.how
				if how and how=="method_v0" then
					local obj = t.obj
					local fn = obj[t.method]
					if fn then
						val = fn( obj, "get", t.args )
						if val == nil then
							self:print_error( "BALUE:__get_target_value() process returned nil" )
							aaa.debug.print_traceback()
						end
					else
						self:print_error( "BALUE:__get_target_value() no method "..t.method.." in obj "..obj )
						aaa.debug.print_traceback()
					end
				else
					val = t.tab[t.key]
					if val == nil then
						table.print( t, "targets.lua", 2 )
						self:print_error( "BALUE:__get_target_value() lua target is nil for key "..t.key )
						aaa.debug.print_traceback()
					end
				end
				return val
			end
		end
	end
end

--[[
--la la
function BALUE:read_target()
	local value = self:__get_target_value()
	if value~=nil then
		self:set_value( value )
	end
end
--]]

--[[
--la do we still need it
--todo refine condition of update in bidirectional ways
--todo deal with midi for each id
--todo reread and clean this deal with multiple targets
function BALUE:__update_target()
	--self:print( "BALUE:__update_target()" )

	-- local b_contact = self.__bu:is_contact()
	-- if not b_contact then
	-- 	if self.__bu:read_midi_control() then
	-- 		b_contact = true	-- force update of targets
	-- 	end
	-- end

	--self:print( "__update_target()" )

	local targets = self.__targets
	if targets then
		local val
		--todo don't work with several
		--	target param case
		local param_ref = targets.param
		if param_ref then
			--if b_contact then
				val = self:get_value()
				--self:print( "target param "..val )
				param.set( param_ref, val )
			--else
				--	val = param.get( par )
				--	self:set_value( val, 1 )
			--end
		end
		--	target lua case
		local lua = targets.lua
		if lua then
			local tab = lua.tab
			if tab then
				local key = lua.key
				local typ = type( tab[ key ] )

				if typ == "boolean" then	val = self:get_value_as_bool()
				else						val = self:get_value()
				end
				--self:print( "------- BUI:update() targets.lua( "..key..", "..tostring(val).." )" )
				tab[ key ] = val
			end
		end
	end
end
--]]

function BALUE:__write_to_target( val )
	--self:print( "__write_to_target val is "..val )
	if val==nil then -- should we allow this ? will be complex probably unecessary
		self:print( "BALUE:__write_to_target() call with nil" )
		return
	end
	if self.b_dbg_value then self:print( "BALUE:__write_to_target( "..val.." )" ) end
--[[
	local b_contact = self.__bu:is_contact()
	if not b_contact then
		if self.__bu:read_midi_control() then
			b_contact = true	-- force update of target
		end
	end
--]]
--todo check val
	local targets = self.__targets
	if targets then

		--todo don't work with several
		--	target param case
		local par = targets.param
		if par then	--todo loop
			param.set( par, val )
			if self.b_dbg_value then self:print( "BALUE:__write_to_target() param set" ) end
		end

		--	target lua case
		local t = targets.lua
		if t then
			local how = t.how
			if how and how=="method_v0" then
				local obj = t.obj
				local fn = obj[t.method]
				if fn then
					fn( obj, "set", t.args, val )
				else
					self:print_error( "BALUE:__write_to_target() no method "..t.method.." in obj "..obj )
					aaa.debug.print_traceback()
				end
			else	
				local tab = t.tab
				if tab then
					tab[t.key] = val
					if self.b_dbg_value then self:print( "BALUE:__write_to_target() set table "..t.tab.." field "..t.key ) end
				end
			end
		end
	end
end

-- CONVERT VALUE
--
function BALUE:convert_value_to_cano( val )
	val = val or self:get_value()	-- why ?

	local typ = type( val )
	if typ == "boolean" then
		return val and 1 or 0
	end

	if self.b_dbg_value then
	--if self.b_dbg_value or not self.__min or not self.__max then
		aaa.debug.print_traceback()
		self:print( " BALUE:convert_value_to_cano() BALUE min / max is "..self.__min.." / "..self.__max )
	end
	local f = self.__max - self.__min
	if f==0 then
		return 0
	else
		val = ( val - self.__min ) / f

		--self:print( "set_value : "..val )
		if self:is_min_max_strict() then
			return clamp_01(val)
		else
			return val
		end
	end
end

function BALUE:convert_value_from_cano( val_cano )
--	hack done quickly spread every wheretodo use less
--	if self:is_min_max_strict() then
--		val_cano = clamp_01(val_cano)
--	end

	if self.__b_type_bool then
		return val_cano>.5 and true or false
	else
		local f = self.__max - self.__min
		if f==0 then
			return self.__min
		else
			--todo check all tese calls
			if self:is_min_max_strict() then
				val_cano = clamp_01(val_cano)
			end
			local val = self.__min + val_cano * f
			if self:is_value_type_integer() then
				--todo check it always work, floor is needed for selector which need an int output
				val = math.floor( val + .00001 )
			end
			return val
		end
	end
end

--	VALUE
--
-- this really where values are changed
--   the bootleneck is here
function BALUE:__set_value_cano( val_cano )
	--aaa.debug.print_traceback()
--	if self:is_min_max_strict() then
--		val_cano = clamp_01(val_cano)
--	end

	local val_cano_cur = self:get_value_cano()
	if self.b_dbg_value then self:print( "BALUE:__set_value_cano : val_cano_cur is "..val_cano_cur ) end
	-- local targets = self.__targets
	-- if targets then
	-- 	self:print( "targets" )
	-- 	if self.__b_type_bool then
	-- 		--we do that because when we change param in old school we need to invalidate these
	-- 		--	so the change will go thru
	-- 		if targets.param then
	-- 			self.__value_cano = param.get( targets.param )
	-- 		elseif targets.lua then
	-- 			self.__value_cano =
	-- 		end
	-- 		self:print( "self.__value_cano ="..self.__value_cano )
	-- 		self.__value = nil
	-- 		--self:print( "force __val_cano to "..self.__value_cano )
	-- 	elseif targets.__min == self.__min and targets.__max == self.__max then
	-- 		--todo precompute
	-- 		--todo do it on min and max separatly
	-- 		val_cano = clamp_01( val_cano )
	-- 	end
	-- end

	--todo there a clean strategy to choose betwwen val and val cano
	if self.__value_nb and self.__value_nb > 1 then
		val_cano = math.floor( val_cano * (self.__value_nb - 1.) + .5 ) * self.__value_step_factor
	end
	--self:print( "ask val_cano "..val_cano.." self.__value_cano stored is "..self.__value_cano )

	--todo check all tese calls
	if self:is_min_max_strict() then
		val_cano = clamp_01(val_cano)
	end

	--	call a function if value change
	if val_cano_cur ~= val_cano then
		if self.verbose >= 1 then self:print( "__set_value_cano() value cano asked change from "..val_cano_cur.." to "..val_cano ) end
		if self.b_dbg_value then self:print( "\tchange value CANO to "..val_cano ) end

		self.__value_cano = val_cano
		--self:print( val.." min/max "..self.__min.."/"..self.__max.." "..self.__value_nb )
		local val = self:convert_value_from_cano( val_cano )
		local val_cur = self:get_value()
		if self.b_dbg_value then self:print( "\tvalue asked change from "..val_cur.." to "..val ) end
		if val_cur ~= val then
			if self.b_dbg_value then self:print( "\t\tchange value to "..val ) end
			self.__value = val

			self:__write_to_target( val )
			--aaa.debug.print_traceback()
			--self:print( "value_change from "..self.__value." to "..val )
			--local x,y = self:get_xy()
			--self:set_xy( x, y + math.sin( aaa.time.t_real * 10.)*.001 )

			local bu = self.__bu
--bigue removed done on top or done by here
--			bu:write_midi_control()	--	this have to be done AFTER self.__value_cano is assigned
									--	self.__value is not needed
			local plugged_in = self.__plugged_in
			if plugged_in then
				self:print_error( "Maa detexted in 2024 Sep that we shoudl not pass here because self.__plugged_in is never set" )
				local midi = plugged_in.midi
				if midi then
					local ch =  midi.ch
					if ch > 0 then
				--self:print( "aaa.midi.set_control( "..midi.ch..", "..(midi.ctl + i - 1)..", "..self:get_value_cano(i) )
						aaa.midi.set_control( ch, midi.ctl, clamp_01(self.__value_cano) )
					end
				end
			end

			--todo refine doing it at this level
			if bu:is_send() then
				bu:send()
			end
			--	this is really the last thing called when a change is made
			--	set_value also call it last
			--	so every thing is already done for this balue
			--self:print( "__set_value_cano() value cano asked change from "..val_cano_cur.." to "..val_cano )
			--table.print( bu.__fn_table, "bu before bu:__do_value_change()", 3 )
			bu:__do_value_change( self )
		end
		--table.print( self, "BALUE", 2 )
	end
	--hack should not be here
	--todo check what the status of this
	--self.__value = val

--[[
	local targets = self.__targets
	if targest then
		local bui = targets.__bui		--todo store id with and use it
		if bui then
			bui:set_value( val_cano )
		end
	end
--]]

end

function BALUE:set_value_cano( val_cano )
	--if self.__value_nb then
	--	val = math.floor( val * self.__value_nb + .5 ) * self.__step_factor
	--end
	--self:print( "set_value_cano "..val_cano )
	if type(val_cano) == "boolean" then
		self:set_value_type_bool( true )
		val_cano = val_cano and 1 or 0
	else
		--val_cano = clamp_01( val_cano )
	end
	if self.b_dbg_value then
		self:print( "BALUE:set_value_cano() will now call BALUE:__set_value_cano( "..val_cano.." )" )
	end
	self:__set_value_cano( val_cano )
end

function BALUE:set_value_cano_closest( val_cano, tab, id )
	--todo ajouter min / max
	local nearest_cano
	local min_diff = math.huge

	for i=1,#tab do
		local def = tab[i]
		def = self:convert_value_to_cano( def )
		local diff = math.abs( def - val_cano )
		if diff < min_diff then
			min_diff = diff
			nearest_cano = def
		end
	end

	self:__set_value_cano( nearest_cano )
end
function BALUE:set_value( val )
	--self:print_error( "BALUE:set_value() with "..val )
	if val==nil then
		self:print_error( "BALUE:set_value() with a nil value" )
		aaa.debug.print_traceback()	--todo color of this print
	end

	--aaa.debug.print_traceback()
	--self:print_error( "BALUE:set_value( "..val.." )" )

	if type(val) == "boolean" then
		--self:print( "will force type to bool" )
		self:set_value_type_bool( true )
		val = val and 1 or 0
	else
		val = self:convert_value_to_cano( val )
	end

	--self:print( "set_value : "..val )
	self:__set_value_cano( val )
end

function BALUE:set_value_ui( val, id )	-- id there for add_to_undo_history
	--self:print( "set_value( "..val..", "..id.." )" )
	local prev = self:get_value()
	self:set_value( val )
	self.__bu:add_to_undo_history( prev, id )
end

function BALUE:get_value_cano()
	local value = self:__get_target_value()
	if value~=nil then
		value = self:convert_value_to_cano( value )
		return value
	end
	return self.__value_cano or 0
end

BALUE.doc.get_value_cano_clamp = "() like get_value_cano() but clamp to 0 1"
function BALUE:get_value_cano_clamped()		return clamp_01( self:get_value_cano() )	end

function BALUE:get_value()
	local value = self:__get_target_value()
	if value==nil then
		--if b then self:print( "self.__value is "..self.__value ) end
		value = self.__value
		if value==nil then
			if self:is_value_type_bool() then
				value = false
			else
				value = 0
			end 
		end
	end

 	if self.b_dbg_value then
		self:print( "get_value -> "..value )
-- 		aaa.debug.print_traceback()
 	end

	return value
end

function BALUE:get_value_as_bool()
	local v = self:get_value_cano()
	if self.b_dbg_value then
		self:print( "get_value_as_bool -> "..(v and v > .5) )
	end
	return v and v > .5
end
function BALUE:flip_value()
	--aaa.print_fn()
	--self:print( "----------------- from "..self:get_value().." to "..self:__get_value_flipped() ) 
	local v = self:__get_value_flipped()
	self:set_value( v )
	--self:print( "------------------------------------------------ value is now "..self:get_value() ) 
end

function BALUE:set_min_max_value( min, max, value )
	if self.verbose >= 1 then	self:print( "set_min_max_value( "..min..", "..max..", "..value.." )" )	end
	--if GA.__b_pixel_debug then
	--	self:print( "set_min_max_value( "..min..", "..max..", "..value.." )" )
	--	self:box_debug( "la set_min_max_value" )
	--end
	--aaa.print_fn()

	if min or max then
		if value==nil then
			value = self:get_value()
		end
		--if not value then self:print( "set_min_max_value( "..min..", "..max..", "..value.." )" ) end
		min = min or self.__min
		max = max or self.__max

		if self.__min ~= min or self.__max ~= max then
			-- we change min or max so we need to set value so value cano will be good
			--self:print( "value pre "..value )
			--self:print( "value post "..value )
			--if value < min then value = min end
			--if max < value then value = max end
			self.__value_cano = nil	--make sure it will be updated when calling set_value
			--self:print( "set_min_max_value( "..min..", "..max..", "..value.." )" )
			self.__min, self.__max = min, max

			if self:is_value_type_integer() then
				self:__set_value_nb( max - min + 1 )
			end
		end
	end
	if value~=nil then
		--hack check when it is strict
	--	if min and value < min then value = min end
	--	if max and value > max then value = max end
		--self:print( "value set to "..value )
		--table.print( self, "before", 3 )
		self:set_value( value )
	end
end
BALUE.set_min_max =	BALUE.set_min_max_value

function BALUE:__build_add_value( inc )
	local val = self:get_value() + inc
	--self:print( "[min,max] is ".."["..self.__min..","..self.__max.."]" )
	if self.__value_nb then	--todo at some point we need to check int/float path
		val = wrap( val, self.__min, self.__max + 1 )
	else
		val = wrap( val, self.__min, self.__max )
	end
	return val
end
function BALUE:add_value_ui( inc, id )
	if inc ~= 0 then
		local val = self:__build_add_value( inc )
		self:set_value_ui( val, id )
	end
end

 function BALUE:__get_value_flipped()
	if self:is_value_type_bool() then
		--self:print( "BUI:__get_value_flipped() on bool" )
		return not self:get_value_as_bool()
	else
		--self:print( "BUI:__get_value_flipped()" )
		local min = self:get_min()
		local max = self:get_max()
		if min * max < 0 then
			return -self:get_value()
		else
			local v = self:get_value()
			local d_min = math.abs( v-min )
			local d_max = math.abs( v-max )
			--self:print( "distance to min max "..d_min.." "..d_max )
			return (d_min < d_max) and max or min
		end
	end
end
function BALUE:flip_value_ui( id )
	local v = self:__get_value_flipped()
	--self:print( "__get_value_flipped() returned "..v )
	self:set_value_ui( v, id )
end
--function BALUE:set_value_to_def()	self:set_value( self.__def )		end
--[[
function BALUE:flip_value_def_ina()
	if self.verbose >= 1 then self:print( "BUI:flip_value_def_ina()" ) end
	local val = self:get_value()
	local ina = self.__def	-- yes we cross pb in aaaased
	local def = self.__ina
	if		val == def	then	val = ina
	elseif	val == ina	then 	val = def
	else						val = def
	end
	self:set_value( val )
end
--]]

function BALUE:get_ina()	return self.__ina		end
function BALUE:set_def( v )	self.__def = v			end
function BALUE:get_def()	return self.__def		end


