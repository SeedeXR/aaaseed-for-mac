

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

	iy = self:define_time( {ix,iy,	1,SY}, 500, false )

end

function meu:init()
	local ref = self.ref
--	self.pos_cur = {}
--	self.rot_cur = {}
	self.anim_cond = {}
	self:define_eel()
	self:define_anim()
	self.b_init_screens = false
	self.b_get_bbox = false
--	self:get_screen()
--	self.fbx = nil
end

function meu:reset()
	self:print( "reset" )
	self:restart_time()
	self.animation_name = ""
	for key, val in pairs( self.data ) do
		local nb = #val
		if nb > 0 then
			for i = 1, nb do
				local eel = val[ i ]
				eel.start_anim_t = 0.0
				eel.animation_name = ""
				eel.time = 0.0
			end
		end
	end
end

function meu:start_anim()
	self:set_time_looping( true )
	self:time_play( true )
	self:reset()
end

function meu:stop_anim()
	--self.b_draw_anim = false
	self:restart_anim()
	self:time_play( false )
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
--	self:print( "Time "..t )
--self.seq_cur = 6
	--local t = nil
--	self.animations_loaded = false
end



function meu:define_eel()
-- PORITE

	local tab = {
		EEL = {
			{ position = { x = 19.296, y = 0.0, z = 7.29 },
			 rotation = { x = .0, y = -90.0, z = .0 }, scale = 0.25 },
			{ position = { x = 17.961, y = -0.1, z = 6.7 },
			 rotation = { x = 0.0, y = -90.0, z = 0.0 }, scale = 0.25 },
			{ position = { x = 17.7179, y = -0.1, z = 6.649 },
			 rotation = { x = 0.0, y = -90.0, z = 0.0 }, scale = 0.25 },
			{ position = { x = 17.2768, y = -0.1, z = 6.62 },
			 rotation = { x = 0.0, y = -90.0, z = 0.0 }, scale = 0.25 },
			{ position = { x = 18.303, y = -0.1, z = 7.04 },
			 rotation = { x = 0.0, y = -90.0, z = 0.0 }, scale = 0.25 },
			{ position = { x = 18.0168, y = 0.0, z = 8.12 },
			 rotation = { x = 0.0, y = 90.0, z = 0.0 }, scale = 0.25 },

			 { position = { x = 17.961, y = -0.1, z = 7.04 },
			 rotation = { x = 0.0, y = -90.0, z = 0.0 }, scale = 0.25 },
			{ position = { x = 17.7179, y = 0.0, z = 8.12 },
			 rotation = { x = 0.0, y = 0.0, z = 0.0 }, scale = 0.25 },
			{ position = { x = 17.2768, y = -0.1, z = 7.04 },
			 rotation = { x = 0.0, y = -90.0, z = 0.0 }, scale = 0.25 },
			{ position = { x = 18.303, y = -0.1, z = 6.7 },
			 rotation = { x = 0.0, y = 0.0, z = 0.0 }, scale = 0.25 },
			{ position = { x = 18.0168, y = -0.1, z = 7.02 },
			 rotation = { x = 0.0, y = -90.0, z = 0.0 }, scale = 0.25 },

			 { position = { x = 18.961, y = -0.1, z = 6.7 },
			 rotation = { x = 0.0, y = -90.0, z = 0.0 }, scale = 0.25 },
			{ position = { x = 17.179, y = -0.1, z = 6.649 },
			 rotation = { x = 0.0, y = -90.0, z = 0.0 }, scale = 0.25 },
			{ position = { x = 16.8768, y = -0.1, z = 6.62 },
			 rotation = { x = 0.0, y = -90.0, z = 0.0 }, scale = 0.25 },
			{ position = { x = 18.8303, y = -0.1, z = 7.04 },
			 rotation = { x = 0.0, y = 0.0, z = 0.0 }, scale = 0.25 },
			{ position = { x = 17.5168, y = 0.0, z = 8.12 },
			 rotation = { x = 0.0, y = 0.0, z = 0.0 }, scale = 0.25 },
			}
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
	self:define_eel()

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

function meu:get_screen()
--	self:define_coral()
	for key, val in pairs( self.data ) do
	--	table.print( val, "val", 3 )
		for key2, obj in pairs( val ) do
			obj.b_show = false
			if app:is_pc_mas() then
				obj.b_show = false
			elseif app:is_pc_dev() then
				self:print( "Must show "..obj.fbx_name )
				obj.b_show = true
			elseif app:is_location( "KR4") then
				self:print( "Must show "..obj.fbx_name )
				obj.b_show = true
			end
		end
	end
end


function meu:get_bounding_boxes()
	return self.bboxes
end

function meu:update_animation( anim )
	for key, val in PAIRS( anim ) do
		val.start_time_s = val.start_time / 60.
		val.end_time_s = val.end_time / 60.
		val.duration = val.end_time_s - val.start_time_s
	end
end

function meu:define_anim()
	local eel = {
			GR20 = { comment = "", start_time = 0, end_time = 360, b_loop = true },
			GR30 = { comment = "", start_time = 360, end_time = 380, b_loop = false },
			GR40 = { comment = "", start_time = 380, end_time = 760, b_loop = false },
		}
	self:update_animation( eel )
	self.name = "EEL"
	self.anim_cond = eel
	self.fbx_name = "Fbx_EEL"
end


function meu:set_condition()
	for key, val in PAIRS( self.data ) do
	--	table.print( val, "val", 3 )
		for key2, obj in PAIRS( val ) do
			obj.start_anim_t = 0
			obj.animation_name = ""
			obj.time = 0
			--eel.time = start_time
			obj.start_anim_t = t
		end
	end
end

function meu:update_fbx_start_time( meu_fbx, eel, t )
	local start_time = self.anim_cond[ eel.animation_name ].start_time_s
	eel.time = start_time
	eel.start_anim_t = t
--	self:print( "Start Animation : "..eel.animation_name.." at "..start_time )
end

function meu:fbx_update_time( meu_fbx, eel, t, next )
	local duration = self.anim_cond[ eel.animation_name ].duration
	if t - eel.start_anim_t >= duration then
		eel.animation_name = next
		self:update_fbx_start_time( meu_fbx, eel, t )
	else
		eel.time = self.anim_cond[ eel.animation_name ].start_time_s + t - eel.start_anim_t
		--meu_fbx:set_time( meu_time )
	end
end

function meu:update_anim( meu_fbx, eel, t )
	--local meu_fbx = self:get_meu_by_name_no_error( self.fbx_name )
	if t and meu_fbx then
		meu_fbx:set_play(false)
	--	self:print( "eel.animation_name "..eel.animation_name )
		if not eel.animation_name then
			eel.animation_name = ""
		end
		if eel.animation_name == "" then
			-- must start animation
			eel.animation_name = "GR40"
			local start_time = self.anim_cond[ eel.animation_name ].start_time_s
			self:print( "self.anim_cond[ eel.animation_name ].start_time_s "..self.anim_cond[ eel.animation_name ].start_time_s )
			eel.start_anim_t = 0
			eel.time = start_time
		--	self:print( "Start Animation : "..self.anim_cond[ eel.animation_name ].animation_name.." at "..start_time )
		elseif eel.animation_name == "GR40" then
			local duration = self.anim_cond[ eel.animation_name ].duration
			if t - eel.start_anim_t >= duration then
				if eel.b_audience_present then
					eel.animation_name = "GR30"
					--self:print( "audience present")
					self:update_fbx_start_time( meu_fbx, eel, t )
				else
					self:fbx_update_time( meu_fbx, eel, t, "GR20" )
				end
			else
				eel.time = self.anim_cond[ eel.animation_name ].start_time_s + t - eel.start_anim_t
			--	self:print( "For anim index 2, setting meu time : "..meu_time )
				--meu_fbx:set_time( meu_time )
			end

		--	self:fbx_update_time( meu_fbx, eel, t, "GR20" )
		elseif eel.animation_name == "GR20" then
			local duration = self.anim_cond[ eel.animation_name ].duration
		--	self:print( "GR20 "..duration.. " "..t - eel.start_anim_t)
			if t - eel.start_anim_t >= duration then
				--self:print( "test GR30" )
				if eel.b_audience_present then
					self:print( "audience presents")
					eel.animation_name = "GR30"
					self:update_fbx_start_time( meu_fbx, eel, t )
				else
					self:update_fbx_start_time( meu_fbx, eel, t )
				end
			else
				eel.time = self.anim_cond[ eel.animation_name ].start_time_s + t - eel.start_anim_t
			end
		elseif eel.animation_name == "GR30" then
			self:fbx_update_time( meu_fbx, eel, t, "GR40" )
		end
	else
		--self:print_error( "gere")
	end
end

function meu:update_interactivity( eel )
	if app.const then
		local interactivity = app:get_presence_SO( "KR4" )
		local delta_x = 0.5
		local screen = 1
		local x_min = app.const.mona.k_lar * ( screen - 1 ) + app.const.mona.kc_interval * ( screen - 1 ) + delta_x
		local x_max = app.const.mona.k_lar * screen + app.const.mona.kc_interval * ( screen - 1 ) - delta_x
		if interactivity.coverage > 0.002 then
			local dx = 1.0 + ( interactivity.coverage - 0.002 ) * ( 3.0 - 1.0 ) / (1.0-0.002)
			if inside( eel.position.x, interactivity.x - dx, interactivity.x + dx ) then
--					self:print( "eel "..eel.position.x )
			--	self:print( "debut "..interactivity.x - dx )
			--	self:print( "fin "..interactivity.x + dx )
				eel.b_audience_present = true
			else
				eel.b_audience_present = false
			end
		else
		--	self:print("a "..interactivity.coverage )
			eel.b_audience_present = false
		end
	end
end

function meu:draw()
	local t = self:update_time()

	if not self.b_do_anim then
		self.b_do_anim = true
	end

	if self.b_init_screens == false then
		self:get_screen()
		self.b_init_screens = true
	end
	--self:set_time_looping( true )
	--self:print( "is looping "..self:is_time_looping() )
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
				local eel_1 = val[ 1 ]
				local fbx_name = self.fbx_name
				--	self:print( "Must draw "..nb.. " instance of "..fbx_name )
				--	local fbx = self:get_meu_by_name_no_error(  self.fbx_name )
				local fbx = self:get_meu_by_name_no_error( fbx_name )
				if t and fbx then
					if fbx.b_texs_loaded == false then
						fbx:draw()
					end
					local bdd_fbx = fbx.ref.bdd_fbx
					fbx:draw_attr()
					bdd_fbx:set_time_update( self.__b_do_anim )
					if self.__b_do_anim then
						if app:is_location( "KR4") or app:is_pc_dev() then
							self:update_interactivity( eel_1 )
						end
						self:update_anim( fbx, eel_1, t )
						bdd_fbx:set_time( eel_1.time )
					end
					local bdd_ref = bdd_fbx:get_obj()
					bdd_fbx:set_translation( eel_1.position.x, eel_1.position.y, eel_1.position.z )
					bdd_fbx:set_rotation_deg( eel_1.rotation.x, eel_1.rotation.y, eel_1.rotation.z )
					bdd_fbx:set_scale( eel_1.scale, eel_1.scale, eel_1.scale )
					aaa.obj.update_then_draw( fbx.ref.layer_fbx )
					if false then
					else
						for i = 2, nb do
							local eel = val[ i ]
							bdd_fbx:set_translation( eel.position.x, eel.position.y, eel.position.z )
							bdd_fbx:set_rotation_deg( eel.rotation.x, eel.rotation.y, eel.rotation.z )
							bdd_fbx:set_scale( eel.scale, eel.scale, eel.scale )
							local bdd_ref = bdd_fbx:get_obj()
							if self.__b_do_anim then
								if app:is_location( "KR4") or app:is_pc_dev() then
									self:update_interactivity( eel )
								end
								self:update_anim( fbx, eel, t )
								bdd_fbx:set_time( eel.time )
								aaa.obj.update(bdd_ref)
								aaa.obj.draw(bdd_ref)
							else
								--bdd_fbx:set_transformation_update( true )
								aaa.obj.update(bdd_ref)
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
