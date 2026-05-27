

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	self.time = 0.0
	self.b_play = false
	self.b_started = false
	self.b_verbose = false
	self.animation_name = ""
	self.b_interaction = false
	local ix = 1
	local iy = 1
	local SY = 1
	local SY2 = 1 * .5
	local DY = SY*.2
	local SX13 = 8 / 3

	local SX = (8)/3

	iy = iy + SY
	bu = self:add_trig_method(	{	ix, iy,				SX, SY},		"Load Config", 	self, "load_config" )
	bu = self:add_trig_method(	{	ix + 4, iy,			SX, SY},		"Save Config", 	self, "save_config" )

	iy = iy + SY
	bu = self:add_button(	{		ix, iy,			SY, SY },		"Edit", 				self, "b_edit",			false )
	bu:set_value_load_save( false )
	bu = self:add_button(	{		ix + 4, iy,		SY, SY },		"Only Show Selected", 		self, "b_edit_show",	false )
	bu:set_value_load_save( false )

	iy = iy + SY
	ui.bu_index = self:add_slider(	{ix,iy,	8,SY},		"Index",			self, "index_selected",		0, 0, 100 ):set_value_type_integer(true)
	ui.bu_index:set_value_load_save( false )

	iy = iy + SY
	ui.bu_screen_target = self:add_selector(	{ix,iy,	9,1}, "Type" )
	ui.bu_screen_target:set_nb( 4, 1 )
	ui.bu_screen_target:set_item_text( 1, " XMAS 01 ", " XMAS 02 ", " XMAS 03 ", " XMAS 04 " )
	ui.bu_screen_target:set_target_lua( self, "xmas_type" )
	ui.bu_screen_target:set_value_load_save( false )
	iy = iy + SY
	bu = self:add_trig_method(	{	ix, iy,				SX, SY},	"New Xmas", 		self, "add_xmas"				):set_text( "Add Xmas"	)


	iy = iy + SY
	ui.bu_position_x, ui.bu_position_y, ui.bu_position_z = self:add_sliders_xyz(	{ix,iy,	8,SY},	"Position",			self.pos_cur,	false,	25.0	)
	ui.bu_position_x:set_value_load_save( false )
	ui.bu_position_y:set_value_load_save( false )
	ui.bu_position_z:set_value_load_save( false )
	iy = iy + SY
	ui.bu_rotation_x, ui.bu_rotation_y, ui.bu_rotation_z = self:add_sliders_xyz(	{ix,iy,	8,SY},	"Rotation",			self.rot_cur, 	false,	180	)
	ui.bu_rotation_x:set_value_load_save( false )
	ui.bu_rotation_y:set_value_load_save( false )
	ui.bu_rotation_z:set_value_load_save( false )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	SX13,SY},		"Scale",			self, "scale",		1.0, 0.0001, 32.0 )
	bu:set_value_load_save( false )
	iy = iy + SY
--	self:define_time( {ix,iy,	1,SY}, 500, false )

end

function meu:init()
	local ref = self.ref
	self.pos_cur = { x = 0.0, y = 0.0, z = 0.0 }
	self.rot_cur = { x = 0.0, y = 0.0, z = 0.0 }
	self.scale = 1.0
	self.time = 0.0
--	self:define_xmas()
	self.last_index_selected = -1
	self:load_config()
	self.b_init_screens = false
end

function meu:reset()
	self:print( "reset" )
end

function meu:interaction()
	self.b_interaction = true
end

function meu:update_ui()
	local ui = self.ui

	ui.bu_position_x:set_min_max( 0.0, 21.0 )
	ui.bu_position_y:set_min_max( 0.0, 4.0 )
	ui.bu_position_z:set_min_max( -8.0, 8.0 )
	self:update_time_ui()
end

function meu:update()
--	local t = self:update_time()
end

function meu:define_xmas()
-- PORITE

	local tab = {
		{ fbx_name = "XMAS_01", position = { x = 5.738, y = -3.163, z = -0.376 }, rotation = { x = .0, y = .0, z = .0 }, scale = 1.0 },
		{ fbx_name = "XMAS_02", position = { x = 5.465, y = -3.148, z = 1.298 }, rotation = { x = 0.0, y = .0, z = 0.0 }, scale = 1.0 },
		{ fbx_name = "XMAS_03", position = { x = 7.207, y = -3.163, z = -1.081 }, rotation = { x = 0.0, y = .0, z = 0.0 }, scale = 1.0 },
		{ fbx_name = "XMAS_04", position = { x = 7.172, y = -3.163, z = 1.237 }, rotation = { x = 0.0, y = .0, z = 0.0 }, scale = 1.0 },
	}
	self.obj_count = 4
	self.data = tab
end

local xmas_name = { "XMAS_01", "XMAS_02", "XMAS_03", "XMAS_04" }
function meu:add_xmas()
	local xmas = {}
	xmas.fbx_name = xmas_name[ self.xmas_type ]
	xmas.position = { x = 0.0, y = 0.0, z = 0.0 }
	xmas.rotation = { x = .0, y = .0, z = .0 }
	xmas.scale = 1.0
	self.obj_count = self.obj_count + 1
	self.data[ self.obj_count ] = xmas
end


function meu:load_config()
	local name = "xmas"
	local filename = self:get_dir_absolute()..name..".lua"
	--local str = aaa.file.read_text( filename )
	--local tab = table.from_string( str )

	local tab4 = {}

	if app:dofile( filename, tab4 ) then
		self:print( "table read" )
		self.data = tab4.xmas
		--table.print( self.data, "tab4", 5 )
		self.obj_count = #self.data
		self.b_init_screens = false
	--	self:get_screen()
		--table.print( tab4, "tab4", 5 )
	else
		self:print_error( "can't read tab4")
	end


end

function meu:save_config()
	local name = "xmas"
	local filename = self:get_dir_absolute()..name..".lua"
	local tab = {}
	tab[ "xmas" ] = self.data
	self:save_table2( tab, filename, "xmas" )

	--local str = table.to_string( tab )
	--self:print( filename )
	--self:print( str )
	--aaa.file.save_text( filename, str )
end

function meu:save_table2( t, fname, name )
	table.print( t, "t", 3 )
	local file = aaa.file.open( fname, "w" )
	if file then
		file:write( "-- config for "..name, "\n" )
		for key, val in pairs_sorted( t ) do
		--	aaa.print( "tab[\""..key.."\"] = "..val:get_value().."\n" )
			local str = "tab[\""..key.."\"] = "
			self:print( "Jey is "..key )
			--table.print( val, "val", 2 )
			str = self:serialize_to_str( str, val )
			str = str.."\n"
			--self:print( str )
			file:write( str )
		end
		file:close()
	end
end

function meu:save_table()
	--self:define_xmas()

	local filename2 = self:get_dir_absolute().."tapestry_data.lua"

	self:save_table2( self.data, filename2, "porite" )

	local tab4 = {}

	if app:dofile( filename2, tab4 ) then
		self:print( "table read" )
		table.print( tab4, "tab4", 5 )
	else
		self:print_error( "can't read tab4")
	end

end


function meu:set_xmas()
	for key2, xmas in pairs( self.data ) do
		xmas.b_show = false
		xmas.anim_time = 0.0
		xmas.anim_start_value = 0.0
		xmas.anim_start_time = aaa.time.t
		xmas.xmas_closing = false
		xmas.xmas_opening = false
		xmas.anim_shape_time = 0.0
		if app:is_pc_dev() then
			self:print( "PC Dev will show "..xmas.fbx_name )
			xmas.b_show = true
		elseif app:is_pc_mas() then
			self:print( "Master will skip "..xmas.fbx_name )
			xmas.b_show = false
		else
			local screen_str = app:get_screen_from_position( xmas.position )
			if app:is_location( screen_str ) then
				self:print( screen_str.." will show "..xmas.fbx_name )
				xmas.b_show = true
			end
		end
	end
end
local closing_time = 0.25
local opening_time = 2.0

function meu:update_xmas_anim( xmas, b_closing )
	if b_closing then
		--self:print( "Must close")
		if xmas.xmas_closing == false then
			xmas.xmas_closing = true
			xmas.xmas_opening = false
			xmas.anim_start_time = aaa.time.t
			xmas.anim_start_value = xmas.anim_time
			xmas.anim_shape_time = ( 1.0 - xmas.anim_time ) * closing_time
		end
		local time = aaa.time.t - xmas.anim_start_time
		if xmas.anim_shape_time > 0.0 then
			xmas.anim_time = xmas.anim_start_value + time  / xmas.anim_shape_time
		else
			self:print( "here")
			xmas.anim_time = 1.0
		end
	else
	--	self:print( "must open")
		if xmas.xmas_opening == false then
			xmas.xmas_opening = true
			xmas.xmas_closing = false
			xmas.anim_start_value = xmas.anim_time
			xmas.anim_start_time = aaa.time.t
			xmas.anim_shape_time = ( xmas.anim_time ) * opening_time
			self:print( "Opening start" )
			--self:print( "Opening anim_start_value "..xmas.anim_start_value )
			--self:print( "Opening anim_start_time "..xmas.anim_start_time )
			--self:print( "Opening start "..xmas.anim_shape_time )

		end
		local time = aaa.time.t - xmas.anim_start_time
	--	xmas.anim_shape_time = 0.0
		if xmas.anim_shape_time > 0.0 then
	--		self:print( "xmas.anim_shape_time "..xmas.anim_shape_time )
			xmas.anim_time = xmas.anim_start_value - time / xmas.anim_shape_time
	--		self:print( "xmas.anim_time "..xmas.anim_time )
			if xmas.anim_shape_time > time then
			--	self:print( "xmas.anim_time "..xmas.anim_time)
			end
		else
			xmas.anim_time = 0.0
		end
	end
	xmas.anim_time = clamp( xmas.anim_time, 0.0, 1.0 )
	--self:print( "xmas.anim_time "..xmas.anim_time )
end


function meu:update_interactivity( xmas )
	if app.const then
		local interactivity = app:get_presence_SO( xmas.location )
		local delta_x = 0.5
		local screen = 1
		local x_min = app.const.mona.k_lar * ( screen - 1 ) + app.const.mona.kc_interval * ( screen - 1 ) + delta_x
		local x_max = app.const.mona.k_lar * screen + app.const.mona.kc_interval * ( screen - 1 ) - delta_x
		--if inside( interactivity.x, x_min, x_max ) then
			if interactivity.coverage > 0.002 then
			--	self:print( "audience present "..interactivity.coverage)
				local dx = 1.0 + ( interactivity.coverage - 0.002 ) * ( 3.0 - 1.0 ) / (1.0-0.002)
				if inside( xmas.position.x, interactivity.x - dx, interactivity.x + dx ) then
					self:print( "eel "..xmas.position.x )
				--	self:print( "debut "..interactivity.x - dx )
				--	self:print( "fin "..interactivity.x + dx )
					xmas.b_audience_present = true
				else
					xmas.b_audience_present = false
				end
			else
			--	self:print("a "..interactivity.coverage )
				xmas.b_audience_present = false
			end
		--else
			--self.b_audience_present = false
		--end
	end
end

function meu:draw()
	--self:define_xmas()
	--if not self.fbx then
	--	self.fbx = self:get_meu_by_name_no_error( name )
	--end
	if not self.b_do_anim then
		self.b_do_anim = true
	end
	--if not self.time then
	--	self.time = .0
	--else
	--	self.time = self.time + 0.001
	--end
--	self.b_init_screens = false
	if self.b_init_screens == false then
		self:set_xmas()
		self.b_init_screens = true
	--	table.print( self.data, "self.data2", 3)
	end

	self.ui.bu_index:set_min_max_value( 1, self.obj_count )
	--self:print( self.obj_count )
	if self.b_edit then
		if self.index_selected ~= self.last_index_selected and self.data[ self.index_selected ] then
			self.pos_cur.x = self.data[ self.index_selected ].position.x
			self.pos_cur.y = self.data[ self.index_selected ].position.y
			self.pos_cur.z = self.data[ self.index_selected ].position.z
			self.rot_cur.x = self.data[ self.index_selected ].rotation.x
			self.rot_cur.y = self.data[ self.index_selected ].rotation.y
			self.rot_cur.z = self.data[ self.index_selected ].rotation.z
			local scale = self.data[ self.index_selected ].scale
			self.scale = scale
			self.last_index_selected = self.index_selected
		elseif self.data[ self.index_selected ] then
			self.data[ self.index_selected ].position.x = self.pos_cur.x
			self.data[ self.index_selected ].position.y = self.pos_cur.y
			self.data[ self.index_selected ].position.z = self.pos_cur.z
			self.data[ self.index_selected ].rotation.x = self.rot_cur.x
			self.data[ self.index_selected ].rotation.y = self.rot_cur.y
			self.data[ self.index_selected ].rotation.z = self.rot_cur.z
			self.data[ self.index_selected ].scale = self.scale
		end
	else
		if self.index_selected ~= -1 and self.data[ self.index_selected ] then
			self.pos_cur.x = self.data[ self.index_selected ].position.x
			self.pos_cur.y = self.data[ self.index_selected ].position.y
			self.pos_cur.z = self.data[ self.index_selected ].position.z
			self.rot_cur.x = self.data[ self.index_selected ].rotation.x
			self.rot_cur.y = self.data[ self.index_selected ].rotation.y
			self.rot_cur.z = self.data[ self.index_selected ].rotation.z
			local scale = self.data[ self.index_selected ].scale
			self.scale = scale
		end
	end
	--table.print( self.pos_cur, "self.pos_cur", 3 )
	self:draw_layers_begin()
		self.__b_do_anim = self.b_do_anim and ( ga:get_pass_info().name~="shadow" )

		local ms = app:get_meu_by_name_cached( "ShaSkinTapestry_1" )
		if ms then
			local layer_shading	= ms:get_layer_shading()
			self.shading		= ms:get_shading()
			aaa.obj.update_then_draw( layer_shading )
		end
		
		if self.__b_do_anim then
			-- local t = self:update_time()
			-- if t then
			-- 	self.time = t
			-- end
		--	bdd_fbx:set_time( wrap_01(self.time ) * 5.0 )
		end
		local presence = app:get_presence_SO( "KL1" )
	--	table.print( presence, "presence",3)
		local nb = #self.data
		--nb = 1
		for i = 1, nb do
			local xmas = self.data[ i ]
			local fbx_name = "fbx_"..xmas.fbx_name
			local fbx = self:get_meu_by_name_no_error( fbx_name )
			if fbx then
				local b_show = xmas.b_show
				if self.b_edit_show and i ~= self.index_selected then
					b_show = false
				end
				if b_show then
					if fbx.b_texs_loaded == false then
						fbx:draw()
					end
					local bdd_fbx = fbx.ref.bdd_fbx
					fbx:draw_attr()
					if app:is_location( xmas.location ) or app:is_pc_dev() then
						self:update_interactivity( xmas )
					end
					if xmas.b_audience_present then
						self:update_xmas_anim( xmas, true )
					else
						self:update_xmas_anim( xmas, false )
					end
					-- if presence then
					-- 	--			fbx:set_blend_shape( 1, true, val[ 1 ].blend_shape_value )
					-- 	if presence.coverage > 0.1 then
					-- 	--	self:print( "Xmas with index "..i.." has coverage "..presence.coverage )
					-- 		self:update_xmas_anim( xmas, true )
					-- 	else
					-- 		self:update_xmas_anim( xmas, false )
					-- 	end
					-- else
					-- 	xmas.anim_time = 0.0
					-- end
				--	self:print( "general xmas.anim_time"..xmas.anim_time)
					bdd_fbx:set_time_update( self.__b_do_anim )

					if self.__b_do_anim then
						bdd_fbx:set_time( xmas.anim_time )
					end
					local bdd_ref = bdd_fbx:get_obj()
					bdd_fbx:set_translation( xmas.position.x, xmas.position.y, xmas.position.z )
					bdd_fbx:set_rotation_deg( xmas.rotation.x, xmas.rotation.y, xmas.rotation.z )
					bdd_fbx:set_scale( xmas.scale, xmas.scale, xmas.scale )
					aaa.obj.update_then_draw( fbx.ref.layer_fbx )
				end
			else
			--	self:print_error( "can't find "..key)
			end
		end
	self:draw_layers_end()
end
