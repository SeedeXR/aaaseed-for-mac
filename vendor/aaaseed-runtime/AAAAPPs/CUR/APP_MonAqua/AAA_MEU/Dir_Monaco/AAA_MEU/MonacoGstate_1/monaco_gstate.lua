
function MONACO_AQUA:apply_render_gstate( name )
	self:print( "apply_render_gstate" .. name )
	local gs = self:get_gs_render( name )
	if gs then gs:apply() end
end
function MONACO_AQUA:use_target()
end
function MONACO_AQUA:make_gstate_fname( name )
	return self:get_dir_absolute().."Descriptor/"..name..".lua"
end
function MONACO_AQUA:get_gstate_fname()
	return self:make_gstate_fname( "render_olivier" )
end
function MONACO_AQUA:edit_render_file()
	self:print( "Edit Gstate" )
	local fname = self:get_gstate_fname()
	self:print( "file is "..fname )
	aaa.os.open_editor( fname )
end
function MONACO_AQUA:read_gstate_file()
	--self:box_debug( "Read gstate" )
	self:print( "Read gstate" )
	aaa.lua.dofile_protected( self:get_gstate_fname(), true )
--	table.print( self.gstate_render, self..".gstate_render", 2 )
end

function MONACO_AQUA:get_gs_render( name )
	--self:print( "get_gs_render" .. name )
	local gs_render = self.gs_render
	if gs_render then
		local gs = gs_render[name]
		if gs then
			return gs
		else
			self:print( "No render gstate "..name )
		end
	else
		self:print_error( "NO render gstate at all (probably need a read)" )
	end
end
function MONACO_AQUA:begin_gstate_render( name )
	self:print_inverse( "TODO define_gstate_render() use name : "..name )
	local gs = self:init_gstate_render( name )
 	local gs_render = self:get_table_always( "gs_render" )
 	gs_render[name] = gs
	self.gs_render_reading = gs
	gs:bind()
end
function MONACO_AQUA:end_gstate_render()
	self.gs_render_reading:unbind()
	--table.print( self.gs_render, self..".gs_render", 1 )
end

local FOG_syno = {
	sun_rgb				= "sun_color_rgb",
	top_rgb				= "top_color_rgb",
	bottom_rgb			= "bottom_color_rgb",
}
local LIGHT_PASS_syno = {
	power 				= "light_factor_percent",
	shadow_power		= "shadow_grey_percent",
	ambient_power		= "ambient_grey_percent",
	out_power			= "out_grey_percent"
}
local LIGHT_syno = {
	position			= "pos_xyz",
	power 				= "diffuse_grey_percent",
	color_rgb			= "diffuse_rgb",
	shadow_hardness		= "shadow_filter_custom",
	caustic_power		= "intensity_map_factor_percent",
	caustic_scale		= "intensity_map_scale",
	angle_azi			= "angle_azi_custom"
}
local MATERIALS_syno = {
	specular			= "specular_grey",
	roughness			= "shininess",
}

function MONACO_AQUA:init_gstate_render()
	local gs = GSTATE:create( "render" )
	local state
	state = gs:add_state( "FOG", "NdcFogMona_1" )
		state:add_synonyme( FOG_syno )
	state = gs:add_state( "LIGHT_PASS", "LightPassV1_1" )
		state:add_synonyme( LIGHT_PASS_syno )
--	state = gs:add_state( "AMBIENT"			)
--	state = gs:add_state( "SHADOW"	)
--	state = gs:add_state( "CAUSTIC"	)
	state = gs:add_state( "LIGHT_DIR", "Lights_1", "l1_"	)
		state:add_synonyme( LIGHT_syno )
	state = gs:add_state( "LIGHT_POINT_1", "Lights_1", "l2_"	)
		state:add_synonyme( LIGHT_syno )
	state = gs:add_state( "LIGHT_POINT_2", "Lights_1", "l3_"	)
		state:add_synonyme( LIGHT_syno )
	state = gs:add_state( "LIGHT_POINT_3", "Lights_1", "l4_"	)
		state:add_synonyme( LIGHT_syno )
	state = gs:add_state( "LIGHT_POINT_4", "Lights_1", "l5_"	)
		state:add_synonyme( LIGHT_syno )
	state = gs:add_state( "MATERIAL_MATTE", "materials_1", "m32_"	)
		state:add_synonyme( MATERIALS_syno )

	return gs
end

-- READER
--
if CLASS.DECLARE( "GSTATE" ) then
end

function GSTATE:create( name )
	local self = GSTATE:create_instance( name )
	self.states = {}
	return self
end
function GSTATE:add_state( name, meu_name, prefix )
	local state = STATE:create( name, self, meu_name, prefix )
	self.states[name] = state
	return state
end
function GSTATE:__global_meta_index( key )
--	self:print( "GSTATE:global_meta_index called with key "..key )
	local states = self.states
--	table.print( states, "states", 1 )
	local state = states[key]
	if state then
--		self:print( "global_meta_index found state "..key )
--		table.print( state, "state "..state, 2 )
		return state
	end
	if inside( key:byte(1), 65, 91 ) then
		self:print( "no state "..key..". target should be one of " )
		for k, v in PAIRS(states) do
			self:print( "\t"..k )
		end
	else
		self:print( "index field "..key )
	end
end
function GSTATE:__global_meta_newindex( key, value )
--	self:print( "GSTATE:global_meta_newindex for "..key )
	local state = self.state_cur
	if state then
--		self:print( "global_meta_newindex found key "..key.." for target "..state )
--		table.print( state, "state", 2 )
		state:set_value( key, value )
		return true
	end
	return false
end
function GSTATE:bind()
	--self:print( "bind()" )
	aaa.lua.global.set_meta_index_methods( self, "__global_meta_index", "__global_meta_newindex" )
end
function GSTATE:unbind()
	--self:print( "unbind()" )
	aaa.lua.global.set_meta_index_methods()
end
function GSTATE:read_file( fname )
	self.state_cur = nil
end
function GSTATE:set_state_cur( state )
	self.state_cur = state
end
function GSTATE:apply()
	--self:print( "apply()")
	for name, state in PAIRS( self.states ) do
		--self:print( name )
		state:apply()
	end
end

-- TARGET
--
if CLASS.DECLARE( "STATE" ) then
end

--[[
	local function STATE_meta_index( t, key )
	aaa.print( "STATE target meta_index for \""..key.."\"" )
	local field = rawget( t, key )
	--aaa.print_fn()
	if field then
		return field
	end
	field = rawget( STATE, key )
	if field then
		return field
	end
	aaa.print( "STATE index no field \""..key.."\"" )
end
local function STATE_meta_new_index( t, key, val )
	aaa.print( "STATE target meta_index for \""..key.."\"" )
	local field = rawget( t, key )
	--aaa.print_fn()
	if field then
		table.print( t, 2, "new_index" )
		rawset( t, key, val )
		return
	end
	aaa.print( "STATE new_index no field \""..key.."\"" )
end
--]]

function STATE:create( name, gstate, meu_name, prefix )
	local self = STATE:create_instance( name )
	if meu_name then
		self.meu = app:get_meu_by_name( meu_name )
	end
	self.prefix = prefix
	self.gstate = gstate
	self:init()
--	local mt = {}
--	mt.__index = DESC_TARGET_meta_index
--	mt.__newindex  = DESC_TARGET_meta_new_index
--	setmetatable( self, mt )
	return self
end

function STATE:init()
	self.values = {}
end

function STATE:set_target()
	--self:print( "is now target" )
	self.gstate:set_state_cur( self )
end

function STATE:add_synonyme( table_syno )
	local synos = self:get_table_always( "synos" )
	for key, val in PAIRS( table_syno ) do
		synos[key] = val
	end
end

function STATE:set_value( key, value )
	--self:print(  key.." set_value to : "..value )
-- synonymes
	local synos = self.synos
	if synos then
		local syno = synos[key]
		if syno then
			key = syno
		end
	end
	local pre = self.prefix
	if pre then
		key = pre..key
	end
-- _rgb case
	local str_end = key:sub( -4, -1 )
	if str_end == "_rgb" then
		key = key:gsub( "_rgb", "_" )
		self.values[key.."red"]		= value[1] / 255
		self.values[key.."green"]	= value[2] / 255
		self.values[key.."blue"]	= value[3] / 255
	elseif str_end == "_xyz" then
		key = key:gsub( "_xyz", "_" )
		self.values[key.."x"]		= value[1]
		self.values[key.."y"]		= value[2]
		self.values[key.."z"]		= value[3]
	else
		str_end = key:sub( -7, -1 )
		if str_end == "percent" then
			key = key:gsub( "_percent", "" )
			self.values[key] = value / 100
		elseif str_end == "_custom" then
			key = key:gsub( "_custom", "" )
			self:print( "custom key "..key.." "..key:sub(-13,-1).." "..key:sub(-9,-1) )
			if key:sub(-13,-1) == "shadow_filter" then
				self.values[key] = (1-value) * 8
			elseif key:sub(-9,-1) == "angle_azi" then
				local ang = value[1] * math.pi / 180
				local azi = value[2] * math.pi / 180
				local r,y = math.cos(azi), math.sin(azi)
				local z,x = r * math.sin(ang), r*math.cos(ang)
				self.values[pre.."pos_x"] = x * 8
				self.values[pre.."pos_y"] = y * 8
				self.values[pre.."pos_z"] = z * 8
			end
		else
			self.values[key] = value
		end
	end

--	table.print( self.values, self..".values", 2 )
end


function STATE:apply()
	local meu = self.meu
	if meu then
		for key, value in PAIRS( self.values ) do
			meu:set_bu_value( key, value )
		end
	end
end
