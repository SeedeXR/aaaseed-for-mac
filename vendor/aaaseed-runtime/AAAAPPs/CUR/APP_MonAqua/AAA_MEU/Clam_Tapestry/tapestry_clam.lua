

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

	--bu = self:add_button(	{	ix, iy,			SY, SY },	"Draw FBX", 		self, "b_draw_fbx",	true )
	-- iy = iy + SY
	-- bu = self:add_trig_method(	{	ix, iy,				SX, SY},		"Load Config", 	self, "load_config" )
	-- iy = iy + SY
	-- bu = self:add_trig_method(	{	ix, iy,				SX, SY},		"Save Config", 	self, "save_config" )


end

function meu:init()
	local ref = self.ref
	self.pos_cur = { x = 0.0, y = 0.0, z = 0.0 }
	self.rot_cur = { x = 0.0, y = 0.0, z = 0.0 }
	self.scale = 1.0
	self:define_clam()
	self.b_init_screens = false
	self.b_get_bbox = false
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
--	self:print( "Time "..t )
--self.seq_cur = 6
	--local t = nil
--	self.animations_loaded = false
end


function meu:define_clam()
-- PORITE

	local tab = {
		G_CLAM_01 = {
				{ fbx_name = "G_CLAM_01", position = { x = 5.4, y = -3.163, z = -0.376 }, rotation = { x = .0, y = 120.393, z = .0 }, scale = 0.5, index_anal = 1 },
				{ fbx_name = "G_CLAM_01", position = { x = 9.2, y = -2.7, z = -1.6 }, rotation = { x = .0, y = 80.393, z = .0 }, scale = 0.4, index_anal = 2 },
				},
		G_CLAM_02 = {
				{ fbx_name = "G_CLAM_02", position = { x = 5.1, y = -2.8, z = 1.4 }, rotation = { x = 0.0, y = 34.517, z = -10.0 }, scale = 0.5, index_anal = 3 },
				{ fbx_name = "G_CLAM_02", position = { x = 8.5, y = -2.8, z = 1.0 }, rotation = { x = 0.0, y = 170.517, z = -10.0 }, scale = 0.5, index_anal = 4 },
				},
		G_CLAM_03 = {
				{ fbx_name = "G_CLAM_03", position = { x = 7.207, y = -2.9, z = -1.21 }, rotation = { x = 0.0, y = 100.836, z = -10.0 }, scale = 0.37213, index_anal = 5 },
				},
		G_CLAM_04 = {
				{ fbx_name = "G_CLAM_04", position = { x = 6.6, y = -2.5, z = 2.5 }, rotation = { x = 0.0, y = 70.76, z = -10.0 }, scale = 0.40382, index_anal = 6 },
				},
		G_CLAM_05 = {
				{ fbx_name = "G_CLAM_05", position = { x = 8.213, y = -2.7, z = 2.56 }, rotation = { x = 0.0, y = -85.45, z = -10.0 }, scale = 0.318305, index_anal = 7 },
				},
	}

	self.data = tab
end

function meu:set_clam()
	local index = 0
	for key, clams in pairs( self.data ) do
	--	table.print( val, "val", 3 )
		for key2, clam in pairs( clams ) do
			index = index + 1
			clam.b_show = false
			clam.blend_shape_value = 0.0
			clam.blend_shape_start_value = 0.0
			clam.blend_shape_start_time = 0.0
		--	clam.blend_shape_active = false
			clam.clam_closing = false
			clam.clam_opening = false
			if app:is_pc_dev() then
				self:print( "PC Dev will show "..clam.fbx_name )
				clam.b_show = true
			elseif app:is_pc_mas() then
				self:print( "Master will skip "..clam.fbx_name )
			else
				if app:is_location( "SO" ) then
					self:print( "SO will show "..clam.fbx_name )
					clam.b_show = true
				end
			end
		end
	end
	self.obj_count = index
end

function meu:assign_bbox()
	self.bboxes = {}
	local index = 1
	for key, clams in pairs( self.data ) do
		for key2, clam in pairs( clams ) do
			local fbx_name = "fbx_"..clam.fbx_name
			self:print( "fbx_name"..fbx_name )
			local fbx = self:get_meu_by_name_no_error( fbx_name )
			if fbx then
				if fbx.b_texs_loaded == false then
					fbx:draw()
				end
				local bbox = fbx:get_bbox()
				if bbox then
					--table.print( bbox, "bbox", 5 )
					local center = {}
					center.x = bbox.center.x + clam.position.x
					center.y = bbox.center.y + clam.position.y
					center.z = bbox.center.z + clam.position.z
					local radius = -10000
					if radius < bbox.size.x then radius = bbox.size.x end
					if radius < bbox.size.y then radius = bbox.size.y end
					if radius < bbox.size.z then radius = bbox.size.z end
					radius = radius * .5 * clam.scale
					local boundingbox = {}
					boundingbox.center = center
					boundingbox.radius = radius
					clam.bbox = boundingbox
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

local closing_time = 0.25
local opening_time = 5.0


function meu:update_clam_blend_shape( clam, b_closing )
	if b_closing then
		if clam.clam_closing == false then
			clam.clam_closing = true
			clam.clam_opening = false
			clam.blend_shape_start_time = aaa.time.t
			clam.blend_shape_start_value = clam.blend_shape_value
			clam.blend_shape_time = ( 1.0 - clam.blend_shape_value ) * closing_time
		end
		local time = aaa.time.t - clam.blend_shape_start_time
		if clam.blend_shape_time > 0.0 then
			clam.blend_shape_value = clam.blend_shape_start_value + time  / clam.blend_shape_time
		else
			clam.blend_shape_value = 1.0
		end
	else
		if clam.clam_opening == false then
			clam.clam_opening = true
			clam.clam_closing = false
			clam.blend_shape_start_value = clam.blend_shape_value
			clam.blend_shape_start_time = aaa.time.t
			clam.blend_shape_time = ( clam.blend_shape_value ) * opening_time
		end
		local time = aaa.time.t - clam.blend_shape_start_time
		if clam.blend_shape_time > 0.0 then
			clam.blend_shape_value = clam.blend_shape_start_value - time / clam.blend_shape_time
		else
			clam.blend_shape_value = 0.0
		end
	end
	clam.blend_shape_value = clamp( clam.blend_shape_value, 0.0, 1.0 )
end

function meu:draw()
--	self:define_clam()
--	self.b_init_screens = false

	if not self.b_do_anim then
		self.b_do_anim = true
	end
	if not self.time then
		self.time = .0
	else
		self.time = self.time + 0.001
	end
	if self.b_init_screens == false then
		self:set_clam()
		self.b_init_screens = true
	end

	if self.b_get_bbox == false then
		self:assign_bbox()
	end
	
	--self.ui.bu_index:set_min_max_value( 1, self.obj_count )
	--self:print( self.obj_count )
	-- if self.b_edit then
	-- 	local index = 0
	-- 	for key, fbx_name in pairs( self.data ) do
	-- 		for key2, obj in pairs( fbx_name ) do
	-- 			index = index + 1
	-- 			if index == self.index_selected then
	-- 				obj.position.x = self.pos_cur.x
	-- 				obj.position.y = self.pos_cur.y
	-- 				obj.position.z = self.pos_cur.z
	-- 				obj.rotation.x = self.rot_cur.x
	-- 				obj.rotation.y = self.rot_cur.y
	-- 				obj.rotation.z = self.rot_cur.z
	-- 				obj.scale = self.scale
	-- 			end
	-- 		end
	-- 	end
	-- else
	-- 	local index = 0
	-- 	for key, fbx_name in pairs( self.data ) do
	-- 		for key2, obj in pairs( fbx_name ) do
	-- 			index = index + 1
	-- 			if index == self.index_selected then
	-- 				self.pos_cur.x = obj.position.x
	-- 				self.pos_cur.y = obj.position.y
	-- 				self.pos_cur.z = obj.position.z
	-- 				self.rot_cur.x = obj.rotation.x
	-- 				self.rot_cur.y = obj.rotation.y
	-- 				self.rot_cur.z = obj.rotation.z
	-- 				local scale = obj.scale
	-- 				self.scale = scale
	-- 			end
	-- 		end
	-- 	end
	-- end
	--table.print( self.pos_cur, "self.pos_cur", 3 )

	self:draw_layers_begin()
		self.__b_do_anim = self.b_do_anim and ( ga:get_pass_info().name~="shadow" )

		local ms = app:get_meu_by_name_cached( "ShaSkinTapestry_1" )
		if ms then
			local layer_shading	= ms:get_layer_shading()
			self.shading		= ms:get_shading()
			aaa.obj.update_then_draw( layer_shading )
		end
		
	--	local index = 1
		local presence = app:get_presence_clam_SO()
		for key, val in PAIRS( self.data ) do
			local nb = #val
			if nb > 0 then
				local clam_1 = val[ 1 ]
				local fbx_name = "fbx_"..clam_1.fbx_name
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
					--	bdd_fbx:set_time( wrap_01(self.time ) * 5.0 )
					end
					local bdd_ref = bdd_fbx:get_obj()
					if clam_1.b_show then
						bdd_fbx:set_translation( clam_1.position.x, clam_1.position.y, clam_1.position.z )
						bdd_fbx:set_rotation_deg( clam_1.rotation.x, clam_1.rotation.y, clam_1.rotation.z )
						bdd_fbx:set_scale( clam_1.scale, clam_1.scale, clam_1.scale )
						if presence[ clam_1.index_anal ] then
				--			fbx:set_blend_shape( 1, true, val[ 1 ].blend_shape_value )
							if presence[ clam_1.index_anal ].coverage > 0.1 then
								--self:print( "Clam "..key.." with index 1 has coverage "..presence[ clam_1.index_anal ].coverage )
								self:update_clam_blend_shape( clam_1, true )
							else
								self:update_clam_blend_shape( clam_1, false )
							end
						else
							clam_1.blend_shape_value = 0.0
						end
						fbx:set_blend_shape( 1, true, clam_1.blend_shape_value )
						aaa.obj.update_then_draw( fbx.ref.layer_fbx )
					end
					for i = 2, nb do
						--index = index + 1
						local clam = val[ i ]
						if clam.b_show then
							bdd_fbx:set_translation( clam.position.x, clam.position.y, clam.position.z )
							bdd_fbx:set_rotation_deg( clam.rotation.x, clam.rotation.y, clam.rotation.z )
							bdd_fbx:set_scale( clam.scale, clam.scale, clam.scale )
							if presence[ clam.index_anal ] then
								if presence[ clam.index_anal ].coverage > 0.1 then
								--	self:print( "Clam "..key.." with index "..i.." has coverage "..presence[ clam.index_anal ].coverage )
									self:update_clam_blend_shape( clam, true )
								else
									self:update_clam_blend_shape( clam, false )
								end
							else
								clam.blend_shape_value = 0.0
							end
							fbx:set_blend_shape( 1, true,clam.blend_shape_value )
							if self.__b_do_anim then
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
