
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	local ix = 1
	local iy = 1
	local SY = 1
	local DY = SY*.2

	local SX = 8/3

	self:add_trig_method(	{ix,iy,		SX,SY},	"Load Config", 	self, "load_config" )
	iy = iy + SY

	self.time = 0.0
	self:define_time( 		{ix,iy,		1,SY}, 70, false )

	iy = 6
	self:add_button( 		{ix,iy		}, 		"SO", 	self, "b_so", true )
	iy = iy + SY
	self:add_button( 		{ix,iy		}, 		"KL", 	self, "b_kl", true )
	self:add_button( 		{ix+4,	iy	}, 		"KC", 	self, "b_kc", true )
	self:add_button( 		{ix+8,	iy	}, 		"KR", 	self, "b_kr", true )

	iy = iy + SY
	self:add_trig_method(	{ix,	iy,	SX,SY},	"Start Anims", 	self, "start_anims" )
	self:add_trig_method(	{ix+4,	iy,	SX,SY},	"Stop Anims", 	self, "stop_anim" )
end

function meu:update_ui()
	local ui = self.ui
	self:update_time_ui()
end

function meu:select_dolphins( tab_in )
	local tab_out = {}
	for key, obj in pairs( tab_in ) do
		local b_insert = false
		if app:is_pc_dev() then
			--self:print( "PC Dev will show dolphin "..key )
			b_insert = true
		elseif app:is_pc_mas() then
			--self:print( "Master will skip dolphin "..key )
		else
			if app:is_location_side( obj.location ) then
				--	self:print( screen_str.." will show "..obj.fbx_name )
				b_insert = true
			end
		end
		if b_insert then table.insert( tab_out, obj ) end
	end
	return tab_out
end

function meu:load_config()
	--self:box_debug( "load_config()" )
	local name = "dolph"
	local filename = self:get_dir_absolute()..name..".lua"
	--local str = aaa.file.read_text( filename )
	--local tab = table.from_string( str )

	local tab4 = {}
	if app:dofile( filename, tab4 ) then
		--self:print( "table read" )
		self.data = tab4.dolph
		--table.print( self.data, "adolph table", 5 )
		--self:get_screen()
		self.data = self:select_dolphins( tab4.dolph )
	else
		self:print_error( "can't read tab4")
	end
	--self:box_debug( "after load_config()" )
end

function meu:stop_anim()
	self:restart_time()
	self:time_play( false )
end

function meu:start_anims()
	self:restart_time()
	self:time_play( true )
end

function meu:draw()
	--self:define_dolph()

	if not self.data then
		self:load_config()
		if not self.data then
			return
		end
	end

	--table.print( self.data, "adolph table c", 5 )
	local meu_fbx = self:get_meu_by_name_no_error( "Fbx_DOLPH_DN2" )
	if meu_fbx then
		local b_do_anim = ga:get_pass_info().name~="shadow"
		self:draw_layers_begin()

			local ms = app:get_meu_by_name_cached( "ShaSkinTapestry_1" )
			if ms then
				local layer_shading	= ms:get_layer_shading()
				self.shading		= ms:get_shading()
				aaa.obj.update_then_draw( layer_shading )
			end
			if b_do_anim then
				local t = self:update_time()
				if t then
					self.time = t
				end
			end
			if meu_fbx.b_texs_loaded == false then
				meu_fbx:draw()
				meu_fbx:set_max_time()
				meu_fbx:set_time_looping( false )
				meu_fbx:set_time_speed( .6 )
			end
			self.anim_end_time = meu_fbx:get_max_time()

			local bdd_fbx = meu_fbx.ref.bdd_fbx
			meu_fbx:draw_attr()
		--	bdd_fbx:set_time_update( false )
			--table.print( self.data, "adolph table just before", 5 )
			local nb = #self.data
			for i = 1, nb do

				--self:print( "dolph "..i )
				local adolph = self.data[ i ]
				if (not app:is_pc_dev())	or (self.b_so and adolph.location == "SO")
											or (self.b_kc and adolph.location == "KC")
											or (self.b_kl and adolph.location == "KL")
											or (self.b_kr and adolph.location == "KR") then
					--table.print( adolph, "dolph "..i )
					if self.time > adolph.time_offset * self:get_time_speed() then
						--self:print( "dolph "..i )
						local time = self.time - adolph.time_offset * self:get_time_speed()
						if time < self.anim_end_time / self:get_time_speed() then
							--self:print( "dolph "..i )
							bdd_fbx:set_time( self.time - adolph.time_offset * self:get_time_speed() )

							local bdd_ref = bdd_fbx:get_obj()

							bdd_fbx:set_param( "transform_active", true )
							local pos = adolph.position
							bdd_fbx:set_translation( pos.x, pos.y+3, pos.z )
							local rot = adolph.rotation
							bdd_fbx:set_rotation_deg( rot.x, rot.y, rot.z )
		--					aaa.obj.update(bdd_ref)
		--					aaa.obj.draw(bdd_ref)
						--	else
							--	bdd_fbx:set_transformation_update( true )
							aaa.obj.update_then_draw(bdd_ref)
					--	end
						end
					end
				end
			end
			--else
			--	self:print_error( "can't find "..key)
			--end
		self:draw_layers_end()
	end

end
