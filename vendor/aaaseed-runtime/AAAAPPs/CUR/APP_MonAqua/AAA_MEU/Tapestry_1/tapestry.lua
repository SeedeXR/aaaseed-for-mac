

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
end

function meu:init()
	local ref = self.ref
	--self.pos_cur = {}
	--self.rot_cur = {}
	self:define_coral()
	self.b_init_screens = false
	self.b_get_bbox = false
--	self:get_screen()
--	self.fbx = nil
end

function meu:reset()
	self:print( "reset" )
end

function meu:interaction()
	self.b_interaction = true
end

function meu:update_ui()
	local ui = self.ui

	self:update_time_ui()

end



function meu:update()
	local t = self:update_time()
end


--local name = "G_CLAM_01"


function meu:define_coral()
	local tab =
	{
		PORITE =
		{
			{ fbx_name = "PORITE", position = { x = 2.273, y = -0.085, z = -4.869 }, rotation = { x = .0, y = .0, z = .0 }, scale = 1.0 },
			{ fbx_name = "PORITE", position = { x = 11.961, y = -0.183, z = -4.93 }, rotation = { x = 00.0, y = -40.0, z = -10.0 }, scale = 1.0 },
			{ fbx_name = "PORITE", position = { x = 1.012, y = -0.085, z = 5.486 }, rotation = { x = 00.0, y = 0.0, z = 0.0 }, scale = 1.0 },
		},
		TURBIN =
		{
			--	{ fbx_name = "TURBIN", position = { x = 17.328, y = 0.462, z = -7.7 }, rotation = { x = .0, y = -145.0, z = .0 }, scale = 1.0 },
			{ fbx_name = "TURBIN", position = { x = 7.615, y = 0.893, z = -14.485 }, rotation = { x = .0, y = -105.0, z = .0 }, scale = 1.0 },
			--	{ fbx_name = "TURBIN", position = { x = 11.268, y = 0.071, z = 5.811 }, rotation = { x = .0, y = 25.0, z = .0 }, scale = 1.014780 },
		},
--pavona slow alllight"
--[[
		PAVONA = {
				--	{ fbx_name = "PAVONA", position = { x = 14.354, y = -0.01, z = -5.355 }, rotation = { x = .0, y = 65., z = .0 }, scale = 1.0 },
				--	{ fbx_name = "PAVONA", position = { x = 19.382, y = 0.144, z = -7.721 }, rotation = { x = .0, y = 35., z = .0 }, scale = 1.0 },
					{ fbx_name = "PAVONA", position = { x = 11.231, y = 1.559, z = -13.292 }, rotation = { x = -3.405, y = -109.72, z = -9.408 }, scale = 1.0 },

				--	{ fbx_name = "PAVONA", position = { x = 16.039, y = -0.229, z = 5.125 }, rotation = { x = 0.0, y = -20.0, z = 0.0 }, scale = 1.09453 },
				--	{ fbx_name = "PAVONA", position = { x = 14.113, y = -0.167, z = 5.254 }, rotation = { x = 0, y = 95, z = 0.0 }, scale = 1.0 },
			},
--]]
	}

	self.data = tab
end


function meu:load_data( name )
	local filename = self:get_dir_absolute()..name
	local str = aaa.file.read_text( filename )
	local tab = table.from_string( str )
	return tab
end

function meu:save_data( tab, name )
	local filename = self:get_dir_absolute()..name
	local str = table.to_string( self.def.porite )
	aaa.file.save_text( filename, str )
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
	self:define_coral()

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

function meu:set_coral()
	for key, type in PAIRS( self.data ) do
		for key2, obj in PAIRS( type ) do
			obj.b_show = false
			if app:is_pc_mas() then
				obj.b_show = false
			elseif app:is_pc_dev() then
				self:print( "Must show "..obj.fbx_name )
				obj.b_show = true
			else
				local screen_str = app:get_screen_from_position( obj.position )
				if app:is_location( screen_str ) then
					self:print( "Must show "..obj.fbx_name )
					obj.b_show = true
				end
			end
		end
	end
end

function meu:assign_bbox()
	self.bboxes = {}
	local index = 1
	for key, val in PAIRS( self.data ) do
		for key2, obj in PAIRS( val ) do
			local fbx_name = "fbx_"..obj.fbx_name
			self:print( "fbx_name "..fbx_name )
			local fbx = self:get_meu_by_name_no_error( fbx_name )
			if fbx then
				if fbx.b_texs_loaded == false then
					fbx:draw()
				end
				local bbox = fbx:get_bbox()
				if bbox then
					--table.print( bbox, "bbox", 5 )
					local center = {}
					center.x = bbox.center.x + obj.position.x
					center.y = bbox.center.y + obj.position.y
					center.z = bbox.center.z + obj.position.z
					local radius = -10000
					if radius < bbox.size.x then radius = bbox.size.x end
					if radius < bbox.size.y then radius = bbox.size.y end
					if radius < bbox.size.z then radius = bbox.size.z end
					radius = radius * .5 * obj.scale
					local boundingbox = {}
					boundingbox.center = center
					boundingbox.radius = radius
					obj.bbox = boundingbox
					self.bboxes[ index ]= boundingbox
					self.b_get_bbox = true
					index = index + 1
				end
			end
		end
	end

end

function meu:get_bounding_boxes()
	return self.bboxes
end


function meu:draw()

	--if not self.fbx then
	--	self.fbx = self:get_meu_by_name_no_error( name )
	--end
	if not self.b_do_anim then
		self.b_do_anim = true
	end
	if not self.time then
		self.time = .0
	else
		self.time = self.time + 0.001
	end

	if self.b_init_screens == false then
		self:set_coral()
		self.b_init_screens = true
	end

	if self.b_get_bbox == false then
		self:assign_bbox()
	end
	--table.print( self.bboxes, "self.bboxes", 5 )
	self:draw_layers_begin()
		self.__b_do_anim = self.b_do_anim and ( ga:get_pass_info().name~="shadow" )

		local ms = app:get_meu_by_name_cached( "ShaSkinTapestry_1" )
		if ms then
			local layer_shading	= ms:get_layer_shading()
			self.shading		= ms:get_shading()
			aaa.obj.update_then_draw( layer_shading )
		end

		for key, val in pairs( self.data ) do
			local nb = #val
			if nb > 0 then
				local fbx_name = "fbx_"..val[ 1 ].fbx_name
			--	self:print( "Must draw "..nb.. " instance of "..fbx_name )
				local fbx = self:get_meu_by_name_no_error( fbx_name )
				if fbx then
					if fbx.b_texs_loaded == false then
						fbx:draw()
					end
					local bdd_fbx = fbx.ref.bdd_fbx
					fbx:draw_attr()
					bdd_fbx:set_time_update( self.__b_do_anim )
					if self.__b_do_anim then
						bdd_fbx:set_time( wrap_01(self.time ) * 5.0 )
					end
					local bdd_ref = bdd_fbx:get_obj()
					if val[ 1 ].b_show then
						bdd_fbx:set_translation( val[ 1 ].position.x, val[ 1 ].position.y, val[ 1 ].position.z )
						bdd_fbx:set_rotation_deg( val[ 1 ].rotation.x, val[ 1 ].rotation.y, val[ 1 ].rotation.z )
						bdd_fbx:set_scale( val[ 1 ].scale, val[ 1 ].scale, val[ 1 ].scale )
						aaa.obj.update_then_draw( fbx.ref.layer_fbx )
					end
					for i = 2, nb do
						if val[ i ].b_show then
							bdd_fbx:set_translation( val[ i ].position.x, val[ i ].position.y, val[ i ].position.z )
							bdd_fbx:set_rotation_deg( val[ i ].rotation.x, val[ i ].rotation.y, val[ i ].rotation.z )
							bdd_fbx:set_scale( val[ i ].scale, val[ i ].scale, val[ i ].scale )
							if self.__b_do_anim then
								bdd_fbx:set_time( wrap_01(self.time ) * 5.0 )
								aaa.obj.update(bdd_ref)
								aaa.obj.draw(bdd_ref)
							else
								bdd_fbx:set_transformation_update( true )
								aaa.obj.draw(bdd_ref)
							end
						end
					end
				else
				--	self:print_error( "can't find "..key)
				end
			end
		end
	self:draw_layers_end()
end
