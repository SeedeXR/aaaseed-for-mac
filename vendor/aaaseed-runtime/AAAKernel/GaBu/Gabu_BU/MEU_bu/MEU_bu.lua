--
--	NET
--
--todo moved here should be check extended tested ....
--	CHECK change_slave_value in all the code : BUI...


--	send thru net
--hack unclear what this is doing any more except error message
--todo clean the caller using this search "set_up_send" only used in
function MEU:set_up_send()
--[[
	local tab = self.__bu_ui.preset
	if tab then
		for key,bu in pairs( tab ) do
	--		if bu and bu:is_preset_use() then
				bu:set_method_on_value_change( self, "send", bu )
	--		end
		end
	end
--]]
end
--todo check 
-- function MEU:send_all_to_all()
-- 	local tab = self.__bu_ui.preset
-- 	if tab then
-- 		for key,bu in pairs( tab ) do
-- 			self:__send_bu_low( bu )
-- 		end
-- 	end
-- end

--todo adapt send vocabulary everywhere
function MEU:__send_bu_low( bu )
	local meu_name = self:get_name_lowercase()
	if meu_name then
		local val = bu:get_value()
		--if aaa.net.is_remote() and app:is_master() then
		local str = "MEU.change_slave_value( \""..meu_name.."\", \""..bu:get_name_lowercase().."\", "..val.." )"
		--self:print( "SEND : "..str )
		aaa.net.lua_send( 0, 0, str )
	end
end
function MEU:is_send()
	return self.__bu_send:get_value_as_bool()
end
function MEU:send_bu( bu )
	if self:is_send() then
		self:__send_bu_low( bu )
	end
end

--	receive
function MEU.change_slave_value( meu_name, bu_name, val )
	--if self:is_send() then return end
	local str = "MEU.change_slave_value( \""..meu_name.."\", \""..bu_name.."\", "..val.." )"
	--bu:print( "RECEIVE : "..str )
	local meu
	if app then
		meu = app:get_meu_by_name( meu_name )
		if meu then
			if meu:is_send() then return end

			aaa.print( "REALLY DO" )
			meu:set_bu_value( bu_name, val )
			--if not bu:is_send() then
			--	bu:set_value( val )
			--end
		else
			aaa.print( "MEU.change_slave_value() don't find bu" )
		end
	else
		aaa.print( "MEU.change_slave_value() no app" )
	end
end

function MEU:add_button_send( ix, iy )
	local bu = self:add_button( {ix,iy}, "Send" )
		bu:set_method_on_value_change( self, "set_up_send" )
		bu:set_preset_use( false )
		self.__bu_send = bu
	return bu
end
--
--	BU EASY acces
--
--todo move to BUS
function MEU:__erase_bu_ui()
	local bu_ui = self.__bu_ui
	bu_ui.all	= {}	-- in the form of all[name] = bu
						-- with name procesed (2022 Oct lowercase)
						-- all bu except the one from define_elts_fixed()
						-- used as the base for other table
						-- used in get_bu_by_key()
						-- and build at each define
						-- todo perhaps the define_elts_fixed() excepts should be dealt as bu_group or using a bu flag
	bu_ui.load_save = {}	-- same than previous (save) but load_save flag is use
	bu_ui.preset	= {}	-- same than previous (save) but preset flag is use
		
	bu_ui.syno		= {}	-- in the form syno[name] = bu where name is a synonym
	bu_ui.b_build_preset_needed = true

end

MEU.doc.__build_bu_ui = "( bus, start ) build bu lists for the meu : all, save and preset"
function MEU:__build_bu_ui_all( bus, start )
--	self:box_debug( "MEU:__build_bu_ui_all()" )
	self:__erase_bu_ui()

	local bu_ui = self.__bu_ui
	local all = bu_ui.all	-- all the bu in the MEU (with a name)
	local bu_syno = bu_ui.syno
	for i = start, bus:get_bu_nb() do
		local bu	= bus:get_bu( i )
		local name	= bu:get_name_lowercase()
		if name then
			if all[name] then
				table.print( bu, "bu", 2 )
				table.print( all[name], "all[name]", 2 )
				self:box_error( bu.."\nhave the same name : \""..name.."\" than \n"..all[name].."\nGarden will ignore this BU for load/save preset.." )
			else
				all[name] = bu
				--todo deal with several synonyms
				--todo check colision between name and synonym
				local syno = bu.__syno
				if syno then
					bu_syno[syno] = bu
				end
			end
		end
	end

	if false then
		--if string.lower( string.sub( self:get_name_lowercase(), 1, 8 ) ) == "tankfish" then
		table.print( all, 		"all",		1	)
		table.print( bu_syno, 	"bu_syno",	1	)
		--self:box_look( "bu_ui tables" )
	end
end

--we don't need synonym when we save
function MEU:__build_bu_ui_save_preset()
	local load_save	= {}	-- the ones which are load or saved with the MEU
	local preset	= {}	-- the ones use by the preset
	local bu_ui = self.__bu_ui
	for name, bu in PAIRS( bu_ui.all ) do
		--self:print( "loop with "..name.." "..bu )
		if bu:is_value_load_save() then	--need to check
			--self:print( "load save "..name.." "..bu )
			load_save[name] = bu
			if bu:is_preset_use() then
				--self:print( "preset "..name.." "..bu )
				preset[name] = bu
			end
		end
	end
	bu_ui.load_save	= load_save
	bu_ui.preset	= preset
	bu_ui.b_build_preset_needed = false
end

function MEU:__get_bu_for_preset( preset_id )
	--aaa.print_fn()
	local bu_ui = self.__bu_ui
	if bu_ui then
		if bu_ui.b_build_preset_needed then
			--self:print( "Called" )
			self:__build_bu_ui_save_preset()
		else
			--self:print( "NOT Called" )
		end
		return preset_id==0 and bu_ui.load_save or bu_ui.preset
	else
		--table.print( self, "table MEU"..self.__proto, 2 )
		self:print_error( "No __bu_ui" )
	end
end

function MEU:__get_bu_all_table()
	--aaa.print_fn()
	local bu_ui = self.__bu_ui
	if bu_ui then
		return  bu_ui.all
	end
end


MEU.doc.get_bu_by_key = "( key ) Retrieve a BU ( Box User ) in the MEU using a key."
function MEU:get_bu_by_key( key )
	--aaa.print_fn()
	
	local bu_ui = self.__bu_ui
	if bu_ui then
		local all = bu_ui.all
		if all then
			-- try directly
			local bu = all[key]
			if bu then
				return bu
			end
			-- try lowercase
			key = string.lower( key )	--todo avoid this when we can
			bu = all[key]
			if bu then
				return bu
			end
		else
			key = string.lower( key )
		end
	else
		key = string.lower( key )
	end

	-- not found so check if __bu_ui.all is empty to search directly, key is now lowercase
	local bus = self:get_bus()
	local nb = bus:get_bu_nb()
	if nb > 0 then
		--todo ahould we start at 1 because user defined start after
		for i = 1, nb do
			local bu = bus:get_bu( i )
			if bu:is_name_lowercase(key) then
				return bu
			end
		end
	end
	--self:print_error( "Can't find bu for \""..key.."\"" )
	--table.print( bu_ui.all, "bu_ui_all" )
end

MEU.doc.set_bu_value = "( key, val, id ) Set the value of a BU identified by key."
function MEU:set_bu_value( key, val, id )
	local bu = self:get_bu_by_key( key )
	if bu then
		if val~=nil then
			if type(val)=="table" then
				for key, value in IPAIRS( val ) do
					bu:set_value( value, key )
				end
			else
				bu:set_value( val, id )
				--aaa.print_fn()
			end
		else
			--error()
			self:print_error( "Can't set bu for \""..key.."\" because value is nil." )
		end
	else
		aaa.debug.print_traceback()
		self:print_error( "Can't find bu for \""..key.."\" so can't set value to "..val )
	end
	return bu
end

MEU.doc.set_bu_values = "( key, values ) Set multiple values for a BU identified by key."
function MEU:set_bu_values( key, values )
	local bu = self:get_bu_by_key( key )
	if bu then
		if values~=nil then
			bu:set_values( values )
		else
			--error()
			self:print_error( "Can't set bu for \""..key.."\" because values is nil." )
		end
	else
		self:print_error( "Can't find bu for \""..key.."\" so can't set values to "..values )
	end
	return bu
end

MEU.doc.get_bu_value = "( key, id ) Retrieve the value of a BU identified by key."
function MEU:get_bu_value( key, id )
	local bu = self:get_bu_by_key( key )
	if bu then
		return bu:get_value( id )
	end
	self:print_error( "Can't find bu for \""..key.."\" so can't get value." )
	return nil
end

MEU.doc.get_bu_value_as_bool = "( key, id ) Retrieve the value of a BU as a boolean."
function MEU:get_bu_value_as_bool( key, id )
	local bu = self:get_bu_by_key( key )
	if bu then
		return bu:get_value_as_bool( id )
	end
	self:print_error( "Can't find bu for \""..key.."\" so can't get value." )
	return nil
end

MEU.doc.get_bu_item_data = "( key, id ) Retrieve the item data of a BU identified by key."
function MEU:get_bu_item_data( key, id )
	local bu = self:get_bu_by_key( key )
	if bu then
		return bu:get_item_data( id )
	end
	self:print_error( "Can't find bu for \""..key.."\" so can't get_item_data." )
	return nil
end

--to preprocesses Bu list
--lala
--[[
function MEU:read_target()
	for key, val in pairs( self.__bu_ui.load_save  ) do
		--val:print( "idjghjgyu" )
		val:read_target()
	end
end
--]]

--todo make_it recursive

--
--	LERP
--
--todo	stepping don't work with a fix == 0.
--todo  consider multi value BUI
function MEU:update_lerp()
	local l = self.lerp
	local t = l.bu_step:get_value_as_bool()
	if t then
		l.bu_play:set_value(1)
		l.b_stepping = true
	end
	if l.bu_play:get_value_as_bool() then
		local s = 1. / l.bu_dur:get_value()
		local v = l.bu_lerp:get_value()
		local v_beg = l.bu_beg:get_value()
		local v_end = l.bu_end:get_value()
		s = s * aaa.time.dt
		--aaa.print( v )
		if v_beg > v_end then s = -s end
		if l.rev then s = -s end
		v = v + s
		if outside( v, v_beg, v_end ) then
			--aaa.print( "lerp and border" )
			v = clamp( v, v_beg, v_end )
			l.rev = not l.rev
		end
		l.bu_lerp:set_value( v )

		local fix = l.bu_fix:get_value()
		local f = math.floor( v )
		v = v - f

		--[[
		if e~= 0. then
			local oe = 1./e
			local fx
			if v < e then
				fx = -math.sin( math.pi * .5 * oe * v )
			elseif v < (1-e) then
				fx = -1
			else
				fx = -1 + math.sin( math.pi * .5 * oe * ( 1.-e-v) )
			end
			fx = fx * 2. * e / math.pi
			v = (v + fx) / ( 1 - 4. * e / math.pi)
		end
		--]]

		if fix~= 0. then
			if s > 0 then
				v =  1-v
				if v <= fix then
					v = 0
					if l.b_stepping then
						l.b_stepping = nil
						l.bu_lerp:set_value( f+1 )
						l.bu_play:set_value( 0 )
					end
				else
					v = (v-fix) / (1-fix)
				end
				v = 1-v
			else
				if v <= fix then
					v = 0
					if l.b_stepping then
						l.b_stepping = nil
						l.bu_lerp:set_value( f )
						l.bu_play:set_value( 0 )
					end
				else
					v = (v-fix) / (1-fix)
				end
			end
		end

		--v = 3 * v * v - 2 * v * v * v

		-- is slightly smoother
		--v = math.sin( v * math.pi * .5)
		--v = v * v

		-- noticeably smoother
		v = v - math.sin( v*math.pi2 ) * math.pi2_over
		return f, v
	end
end

function MEU:add_lerp( ix, iy )

	local function add_slider( rect, name, min, max, val )
		local bu = self:add_slider(	rect, name )
			bu:set_preset_use( false )
			if min or max then
				bu:set_min_max( min, max )
			end
			if val then
				bu:set_value( val )
			end
		return bu
	end

	local l = {}
	ix = ix or 9
	iy = iy or 10
	l.bu_play = self:add_trig(	{ix,	iy },			"Play"	)
	l.bu_step = self:add_trig(	{ix+4,	iy },			"Step"	)

	l.bu_dur	= add_slider(	{ix,	iy+1,	4,1},	"Dur",	.01, 10, 1	)
	l.bu_fix	= add_slider(	{ix+4,	iy+1,	4,1},	"Fix",	0, 1.		)
	l.bu_beg	= add_slider(	{ix, 	iy+2,	4,1},	"beg",	1, 17, 1	)
	l.bu_end	= add_slider(	{ix+4,	iy+2,	4,1},	"end",	1, 17, 4	)
	l.bu_lerp	= add_slider(	{ix,	iy+3,	8,1},	"lerp", 0, 17		)

	return l
end

--
--	PRESET and BU
--
--		0 used to store state and used differently
function MEU:make_preset_filename( i )
	return "preset_"..i..".plua"
end

function MEU:__make_preset_fpath( i, dirname )
	local fname = self:make_preset_filename(i)
	local str = dirname
	if not str then
		str = self:build_dirname()
		--self:print( "dir name is "..str )
		--table.print( self, "MEU:__make_preset_fpath", 1 )
	end
	if str then
		--todo understand why it happens (after copy, instantiate, move. it is unclear )
		if not string.is_trailing_slash(str) then
			self:print_error( "str is  "..str.." fname is "..fname )
			str = string.enforce_trailing_slash( str )
			self:print_error( "str now "..str )
		end
		str = str..fname
	else
		str = "AAAPreset/"..self.."_"..fname
		if app then
			str = app.dir..str
		else
			str = MEU.__no_app_root_dir..str
		end
	end
	return str
end

function MEU:__define_preset( preset_nb, preset_line )
	local bu
	--aaa.box_good( "add "..preset_nb )
	if preset_line then
		local nb_by_line = math.floor( preset_nb / preset_line )
		local fy = preset_line > 1 and 2/3 or 4/3
		bu = self:add_preset(	{17-nb_by_line*fy,16-preset_line*fy,	nb_by_line*fy,preset_line*fy })
	elseif inside( preset_nb, 1, 7 ) then
		--aaa.box_good( "add 1 "..preset_nb  )
		bu = self:add_preset(	{17-preset_nb,15,	preset_nb,1}, "Preset Proto" )
		preset_line = 1
	elseif inside( preset_nb, 8, 12 ) then
		--aaa.box_good( "add 1 "..preset_nb  )
		bu = self:add_preset(	{9,15,		8,1}	)
		preset_line = 1
	elseif inside( preset_nb, 13, 23 ) then
		--aaa.box_good( "add 2 "..preset_nb )
		bu = self:add_preset(	{9,14,		8,2 }	)
		preset_line = 2
	elseif inside( preset_nb, 24, 35 ) then
		bu = self:add_preset(	{9,14+2/3,	8,4/3}	)
		preset_line = math.floor( 2 )
	elseif inside( preset_nb, 35, 36 ) then
		--aaa.box_good( "add "..preset_nb )
		bu = self:add_preset(	{9,14+2/3,	8,4/3}	)
		preset_line = math.floor( 2 )
	else
		bu = self:add_preset(	{1,14+2/3,	16,4/3}	)
		preset_line = math.floor( 2 )
	end

	if bu then
		bu:set_nb( preset_nb/preset_line, preset_line )
		bu:set_preset_target( self )
		bu:set_pos_load_save( false )
	end

	local step
	if preset_nb >= 16 then		step = 4
	else						step = 2
	end
	for i = step,preset_nb, step do
		bu:set_item_text( i, tostring(i) )
	end

end

function MEU:get_preset_nb() 		return 8					end
function MEU:get_preset_nb_proto() 	return 4					end
function MEU:get_preset( i )		return self.__presets[i]	end
function MEU:__get_preset_always( i )
	local preset = self:get_preset( i )
	if not preset then
		preset = {}
		self.__presets[i] = preset
	end
	return preset
end
function MEU:lerp_preset( i, f, key )
	local bu = self:get_bu_by_key( key )
	if not bu then return end
--	aaa.print( key )

	local ret
	local pp = self:get_preset( i )
	local pn = self:get_preset( i + 1 )
	if pp then
		local vp = pp[key]
		if pn then
		local vn = pn[key]
			ret = interpolate( vp, vn, f )
		else
			ret = vp
		end
	else
		if pn then
			ret = pn[key]
		else

		end
	end
	if ret then
		bu:set_value( ret )
	end
end
function MEU:lerp_presets( i, f, tab )
	for _, name in PAIRS( tab ) do
		self:lerp_preset( i, f, name )
	end
end

function MEU:is_preset_recalling()	return self.__b_preset_recalling	end

MEU.doc.recall_preset = "(i) 0 for MEU saved stuff, 1 <= for preset"
function MEU:recall_preset( i )	--0 for saved stuff , 1... for the preset
	--aaa.print_fn()
	local preset = self:get_preset(i)
	if not preset then return end
	--self:print_method()

	local tab_bu = self:__get_bu_for_preset( i )
	if not tab_bu then
		self:box_error( "No bu_ui in this object : Houston should ask Maa !!!" )
	end

	-- we need this so "trig" selectors like the MEU video source selector can know how they proceed
	self.__b_preset_recalling = true

		--table.print( preset, self.." recall("..i..")" )
		for key, val in pairs( preset ) do
			--self:print( "recall_preset() "..key.." --> "..val )
			key = string.lower( key )
			local bu = tab_bu[key]
			if bu then	--test protect from old key still there
				if bu.set_values then
					--self:print( key )
					--table.print( val, "val", 2 )
					bu:set_values( val )
				else
					self:box_error( bu.." don't have a set_values() method\n So Gabu can't set bu for key : "..key.."\nto value : "..val )
				end
			end
		end

	self.__b_preset_recalling = false

	-- local method = self.recall_preset_after
	-- if method then
	-- 	method( self, i, preset, tab_bu )
	-- end
end

function MEU:erase_preset( i )
	self.__presets[i] = nil
	local path = self:__make_preset_fpath( i )
	os.remove( path )
end

function MEU:save_preset_to_file( preset, fname )
	if not preset then
		return false, "nil preset to save in file \""..fname.."\""
	end

--	aaa.print_fn()
	local file = aaa.file.open( fname, "w" )
	if file then
		file:write( "-- preset v0 for "..self, "\n" )
		for key, val in pairs_sorted( preset ) do
			--aaa.print( "tab[\""..key.."\"] = "..val:get_value().."\n" )
			file:write( "tab[\"", key, "\"] = ", self:serialize_to_line( nil, val ), "\n" )
		end
		file:close()
		return true
	else
		return false, "Can't save preset because can't open file \""..fname.."\""
	end
end

function MEU:save_preset( i, dirname )
	local preset = self:get_preset( i )
	local fpath = self:__make_preset_fpath( i, dirname )
	return self:save_preset_to_file( preset, fpath )
end

function MEU:store_preset( i, dirname )
	local bu_use = self:__get_bu_for_preset( i )
	if not bu_use then return end
	--self:print( "store_preset( "..i..", "..dirname.." )" )
	--table.print( bu_use, "bus_use", 1 )
	local preset = self:__get_preset_always( i )
	for key, bu in pairs( bu_use ) do
		if bu then	--test protect from old key still there
			--self:print( "MEU:store_preset() "..bu.." "..key )
			if bu.get_values then
				--self:print( bu.." with key "..key )
				preset[key] = bu:get_values()
				--self:print( key.."-> "..preset[key] )
			else
				self:print( bu.." with key "..key.." have no get_values()" )
			end
			--self:print( key.." <-- "..val:get_value() )
		end
	end
	self:save_preset( i, dirname )
end

--load the preset file into the preset table
--2022 Oct dirname never used
function MEU:load_preset( i, dirname )
	local fpath = self:__make_preset_fpath( i, dirname )
	if not aaa.file.is_exist( fpath ) then return end
	--aaa.print_method()

	local preset = self:__get_preset_always( i )
	app:dofile( fpath, preset )
	-- now clean all the key which has no BU
	local bu_use = self:__get_bu_for_preset( i )
	local bu_syno = self.__bu_ui.syno
	local translated = {}
	for key, _ in pairs( preset ) do
		if not bu_use[key] then	--test protect from old key still there
			local bu = bu_syno[key]
			if bu then
				local name = bu:get_name_lowercase()
				local value = preset[key]
				--self:print( i.." translated key "..key.."\tto "..name.."\twith value "..value )
				table.insert( translated,  { name, value } )
			end
			preset[key] = nil
		end
	end
	for _, t in IPAIRS(translated) do
		preset[t[1]] = t[2]
	end
end
--load the preset files into the preset tables
function MEU:load_presets()
	if GA.b_spy then aaa.spy.push_range( self..":load_presets()", 4 ) end
		local start = 0
		for i = start, self:get_preset_nb() do
			self:load_preset( i )
		end
	if GA.b_spy then aaa.spy.pop_range() end
end
function MEU:save_presets( start, dirname )
	for i = start, self:get_preset_nb() do
		--self:print( i )
		if self:get_preset(i) then
			self:save_preset( i, dirname )
		end
	end
end

function MEU:change_preset( val )
	self.ui.bu_preset:set_value( val )
	self:recall_preset( val )
end

