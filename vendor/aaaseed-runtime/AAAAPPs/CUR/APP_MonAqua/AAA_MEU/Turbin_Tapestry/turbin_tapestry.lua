

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

	iy = self:define_time( {ix,iy,	1,SY}, 500, false )
end

function meu:init()
	local ref = self.ref
	self.pos_cur = {}
	self.rot_cur = {}
	self.anim_cond = {}
	self:define_turbin()
	self:define_anim()
	self.b_init_screens = false
	self.b_get_bbox = false
	self.b_show_static = false
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
				local obj = val[ i ]
				obj.start_anim_t = 0.0
				obj.animation_name = ""
				obj.time = 0.0
			end
		end
	end
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



function meu:define_turbin()
-- PORITE

	local tab = {
		TURBIN_FISH = {
			{ fbx_name = "Fbx_TURBIN_FISH", position = { x = 17.328, y = 0.462, z = -7.7 },
			 rotation = { x = .0, y = -145.0, z = .0 }, scale = 1.0 },
			{ fbx_name = "Fbx_TURBIN_FISH", position = { x = 11.268, y = 0.071, z = 5.811 },
			 rotation = {  x = .0, y = 25.0, z = .0 }, scale = 1.014780 },
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
	self:define_turbin()

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

function meu:set_turbin()
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
			local fbx_name = obj.fbx_name
			local fbx = self:get_meu_by_name_no_error( fbx_name )
			if fbx then
				self:print( "fbx_name "..fbx_name )
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

function meu:update_animation( anim )
	for key, val in PAIRS( anim ) do
		val.start_time_s = val.start_time / 60.
		val.end_time_s = val.end_time / 60.
		val.duration = val.end_time_s - val.start_time_s
	end
end

function meu:reset()
	self:print( "reset" )
	self:restart_time()
	--self.animation_name = ""
	for key, val in pairs( self.data ) do
		local nb = #val
		if nb > 0 then
			for i = 1, nb do
				local obj = val[ i ]
				obj.start_anim_t = 0.0
				obj.animation_name = ""
				obj.time = 0.0
			end
		end
	end
end


function meu:start_anim()
	self:time_play( true )
	self:reset()
end

function meu:stop_anim()
	--self.b_draw_anim = false
	self:restart_anim()
	self:time_play( false )
end

function meu:define_anim()
	local turbin = {
			GR20 = { comment = "loop defensive", start_time = 0, end_time = 349, b_loop = true },
			GR30 = { comment = "sortent", start_time = 349, end_time = 423, b_loop = false },
			GR40 = { comment = "peur", start_time = 423, end_time = 522, b_loop = false },
			GR50 = { comment = "sortir", start_time = 522, end_time = 768, b_loop = false },
		}
	self:update_animation( turbin )
	self.name = "TURBIN_FISH"
	self.anim_cond = turbin
	self.fbx_name = "Fbx_TURBIN_FISH"
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

function meu:update_fbx_start_time( meu_fbx, obj, t )
	local start_time = self.anim_cond[ obj.animation_name ].start_time_s
	obj.time = start_time
	obj.start_anim_t = t
--	self:print( "Start Animation : "..eel.animation_name.." at "..start_time )
end

function meu:fbx_update_time( meu_fbx, obj, t, next )
	local duration = self.anim_cond[ obj.animation_name ].duration
	if t - obj.start_anim_t >= duration then
		self:print( "Animation "..obj.animation_name.." goto "..next)
		obj.animation_name = next
		self:update_fbx_start_time( meu_fbx, obj, t )
	else
		obj.time = self.anim_cond[ obj.animation_name ].start_time_s + t - obj.start_anim_t
		--meu_fbx:set_time( meu_time )
	end
end

function meu:update_anim( meu_fbx, obj, t )
--	local t = self:update_time()
	--local meu_fbx = self:get_meu_by_name_no_error( self.fbx_name )
	if t and meu_fbx then
		meu_fbx:set_play(false)
	--	self:print( "eel.animation_name "..eel.animation_name )
		if not obj.animation_name then
			obj.animation_name = ""
		end
		if obj.animation_name == "" then
			-- must start animation
			obj.animation_name = "GR20"
			local start_time = self.anim_cond[ obj.animation_name ].start_time_s
			self:print( "self.anim_cond[ obj.animation_name ].start_time_s "..self.anim_cond[ obj.animation_name ].start_time_s )
		--	self:restart_time()
			obj.start_anim_t = 0
			obj.time = start_time
		--	self:print( "Start Animation : "..self.anim_cond[ obj.animation_name ].animation_name.." at "..start_time )
		elseif obj.animation_name == "GR40" then
			local duration = self.anim_cond[ obj.animation_name ].duration
			if t - obj.start_anim_t >= duration then
				self:print( "dgdgf")
				if obj.b_audience_present then
					obj.animation_name = "GR40"
					self:print( "audience present stay in GR40")
					self:update_fbx_start_time( meu_fbx, obj, t )
				else
					self:print( "audience none goto GR50")
					obj.animation_name = "GR50"
					self:update_fbx_start_time( meu_fbx, obj, t )
				end
			else
				obj.time = self.anim_cond[ obj.animation_name ].start_time_s + t - obj.start_anim_t
			--	self:print( "For anim index 2, setting meu time : "..meu_time )
				--meu_fbx:set_time( meu_time )
			end

		--	self:fbx_update_time( meu_fbx, obj, t, "GR20" )
		elseif obj.animation_name == "GR20" then
			local duration = self.anim_cond[ obj.animation_name ].duration
		--	self:print( "GR20 "..duration.. " "..t - obj.start_anim_t)
			if t - obj.start_anim_t >= duration then
				if obj.b_audience_present then
					self:print( "audience presentssssss go to GR30")
					obj.animation_name = "GR30"
					self:update_fbx_start_time( meu_fbx, obj, t )
				else
					self:print( "audience none stay in GR20")
					self:update_fbx_start_time( meu_fbx, obj, t )
				end
			else
				obj.time = self.anim_cond[ obj.animation_name ].start_time_s + t - obj.start_anim_t
			--	self:print( "For anim index 2, setting meu time : "..meu_time )
				--meu_fbx:set_time( meu_time )
			end
		elseif obj.animation_name == "GR30" then
		--	self:print( "go to GR40")
			self:fbx_update_time( meu_fbx, obj, t, "GR40" )
		elseif obj.animation_name == "GR50" then
			--self:print( "GR30" )
	--		self:print( "go to GR50")
			self:fbx_update_time( meu_fbx, obj, t, "GR20" )
		end
	else
		--self:print_error( "gere")
	end
end

function meu:update_interactivity( obj, location )
	if app.const then
		local interactivity = app:get_presence_SO( location )
		local delta_x = 0.5
		local screen = 1
		local x_min = app.const.mona.k_lar * ( screen - 1 ) + app.const.mona.kc_interval * ( screen - 1 ) + delta_x
		local x_max = app.const.mona.k_lar * screen + app.const.mona.kc_interval * ( screen - 1 ) - delta_x
		--if inside( interactivity.x, x_min, x_max ) then
			if interactivity.coverage > 0.002 then
			--	self:print( "audience present "..interactivity.coverage)
				local dx = 1.0 + ( interactivity.coverage - 0.002 ) * ( 3.0 - 1.0 ) / (1.0-0.002)
				if inside( obj.position.x, interactivity.x - dx, interactivity.x + dx ) then
				--	self:print( "obj "..obj.position.x )
				--	self:print( "debut "..interactivity.x - dx )
				--	self:print( "fin "..interactivity.x + dx )
					obj.b_audience_present = true
				else
					obj.b_audience_present = false
				end
			else
			--	self:print("a "..interactivity.coverage )
				obj.b_audience_present = false
			end
		--else
			--self.b_audience_present = false
		--end
	end
end

function meu:set_show_static( b_on )
	self.b_show_static = b_on
end

function meu:draw()
	local t = self:update_time()

	if not self.b_do_anim then
		self.b_do_anim = true
	end
--	self.b_init_screens = false
	if self.b_init_screens == false then
		self:set_turbin()
		self.b_init_screens = true
	end

	--if self.b_get_bbox == false then
	--	self:assign_bbox()
	--end
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
				local obj_1 = val[ 1 ]
				local fbx_name = obj_1.fbx_name
			--	self:print( "Must draw "..nb.. " instance of "..fbx_name )
			--	local fbx = self:get_meu_by_name_no_error(  self.fbx_name )
				local fbx = self:get_meu_by_name_no_error(  fbx_name )
				if t and fbx then
					if fbx.b_texs_loaded == false then
						--self:print( "hrtr")
						fbx:draw()
					end
					local bdd_fbx = fbx.ref.bdd_fbx
					fbx:draw_attr()
					-- bdd_fbx:set_time_update( self.__b_do_anim )
					-- --if eel_1.b_show then
					-- 	if self.__b_do_anim then
					-- 		if app:is_location( "KR4") or app:is_pc_dev() then
					-- 			self:update_interactivity( obj_1 )
					-- 		end
					-- 		self:update_anim( fbx, obj_1, t )
					-- 	--	self:print( "Time eel "..val[ 1 ].time )
					-- 		bdd_fbx:set_time( obj_1.time )
					-- 	end
					-- 	local bdd_ref = bdd_fbx:get_obj()
					-- 	bdd_fbx:set_translation( obj_1.position.x, obj_1.position.y, obj_1.position.z )
					-- 	bdd_fbx:set_rotation_deg( obj_1.rotation.x, obj_1.rotation.y, obj_1.rotation.z )
					-- 	bdd_fbx:set_scale( obj_1.scale, obj_1.scale, obj_1.scale )
					-- --	self:print( "draw ")
					-- 	aaa.obj.update_then_draw( fbx.ref.layer_fbx )
					--end
					--self.b_show_static = false
					if false then

					else
						for i = 1, nb do
							local obj = val[ i ]
							--if val[ i ].b_show then
								bdd_fbx:set_translation( obj.position.x, obj.position.y, obj.position.z )
								bdd_fbx:set_rotation_deg( obj.rotation.x, obj.rotation.y, obj.rotation.z )
								bdd_fbx:set_scale( obj.scale, obj.scale, obj.scale )
								local bdd_ref = bdd_fbx:get_obj()
								--if self.__b_do_anim then
									if i == 1 then
										if app:is_location( "KL4") or app:is_pc_dev() then
											self:update_interactivity( obj, "KL4" )
										end
									elseif i == 2 then
										if app:is_location( "KR3") or app:is_pc_dev() then
											self:update_interactivity( obj, "KR3" )
										end
									end
									if self.b_show_static then
										fbx:set_draw_by_index( true, 24 )
									else
										fbx:set_draw_by_index( false )
										self:update_anim( fbx, obj, t )
										bdd_fbx:set_time( obj.time )
									end
									aaa.obj.update(bdd_ref)
									fbx:draw()
								--	aaa.obj.draw(bdd_ref)
								--else
									--bdd_fbx:set_transformation_update( true )
								--	aaa.obj.update(bdd_ref)
								--	aaa.obj.draw(bdd_ref)
								--end
						end
					end
				else
				--	self:print_error( "can't find "..key)
				end
			end
		end
	self:draw_layers_end()
end
