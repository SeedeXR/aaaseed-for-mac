

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

	ui.bu_info_seq =
		self:add_text_info(	{ix+8,	iy,			8,SY2},	"Sequence" )
	ui.bu_info_total_time =
		self:add_text_info(	{ix+8,	iy+SY2,		8,SY2},	"Total Time" )
	ui.bu_info_index =
		self:add_text_info(	{ix+8,	iy+2*SY2,	8,SY2},	"Seq Index" )
	ui.bu_info_anim =
		self:add_text_info(	{ix+8,	iy+3*SY2,	8,SY2},	"Fbx Anim" )
	ui.bu_info_anim_time =
		self:add_text_info(	{ix+8,	iy+4*SY2,	8,SY2},	"Anim Time" )

	iy = self:define_time(	{ix,	iy,			1,SY},	100, false )

	iy = iy + SY
	self:add_trig_method(	{ix,	iy}, 				"Reset", 		self, "reset" )
	self:add_trig_method(	{ix+3,	iy}, 				"Interaction", 	self, "interaction" )
	iy = iy + SY
	bu = self:add_button(	{ix,	iy,			SY,SY},	"Draw Graph", 	self, "b_draw_graph", false )
--	iy = iy + SY
	bu = self:add_button(	{ix+6,	iy,			SY,SY},	"Draw Curve Current", 	self, "b_draw_curve", false )
	iy = iy + SY
	bu = self:add_sliders_xyz(	{ix,iy,			8,SY},	"Pos",			self.pos, 	false,	35.0	)
	iy = iy + SY
	bu = self:add_slider(	{ix,	iy,		SX13,SY},	"Angle",		self,	"angle",		.0,	0,	1	)
	iy = iy + SY
	bu = self:add_button(	{ix,	iy,			SY,SY},	"Verbose", 		self, "b_verbose", false )
end

function meu:init()
	local ref = self.ref

	self.animation_index = 1
	self.last_time = 0
	self.anim_x = .0
	self.anim_y = .0
	self.anim_z = .0
	self.rot = .0
	self.last_rot = .0
	self.pos = { x=0, y=0, z=0 }
	self.curve_pos = { x=0, y=0, z=0 }
	self.b_draw_turtle = false
end


local names = { "Fbx_TURTLE" }
local name = "Fbx_TURTLE"
function meu:reset()
	self:print( "reset" )
	self.graph_index = 1
	self.animation_index = 1
	self.anim_x = .0
	self.anim_y = .0
	self.anim_z = .0
	self.curve_pos = { x = .0, y = .0, z = .0 }
	self.rot = self.angle
	self.last_rot = self.angle
	self.b_started = false
	if not self.animations_loaded or self.animations == false then
		self:define_animations()
	end

	local m = self:get_meu_by_name_no_error( name )
	if m then
		self:print( "self.animation_index "..self.animation_index )
		self:print( "self.seq_cur "..self.seq_cur )
	--	table.print( self.sequences, "self.sequences", 3 )
	--	table.print( self.sequences[ self.seq_cur ], "self.sequences[ self.seq_cur ]", 3 )
	--	table.print( self.sequences[ self.seq_cur ].data[self.animation_index], "self.sequences[ self.seq_cur ].data[self.animation_index]", 3 )
		m:set_animation( self.sequences[ self.seq_cur ].data[self.animation_index], self.anim_x, self.anim_z, self.rot )
	end
	self:restart_time()
end

function meu:set_play( b )
	self.__time.b_play = b
end
function meu:start_anim()
	self:set_play( true )
	self:reset()
end


function meu:stop_anim()
	self:set_play( false )
	local meu_fbx = self:get_meu_by_name_no_error( name )
	if meu_fbx then
		meu_fbx:set_mu_value( 0 )
	end

	--self:restart_anim()
end

function meu:interaction()
	self.b_interaction = true
end

function meu:update_ui()
	local ui = self.ui

	self:update_time_ui()

	if self.seq_cur and self.sequences[ self.seq_cur ] then
		ui.bu_info_seq:set_text( "Sequence : "..self.seq_cur )
		ui.bu_info_total_time:set_text( string.format( "Total Time : %.2f s", self.sequences[ self.seq_cur ].total_time ) )
		ui.bu_info_index:set_text( "Seq Index : "..self.animation_index.." / "..self.sequences[ self.seq_cur ].count )
		local anim_name = self.sequences[ self.seq_cur ].data[self.animation_index]
		ui.bu_info_anim:set_text( "Fbx Anim : "..anim_name )

		local anim_time = 0
		if self.animation_index == 1 then
			anim_time = self.sequences[ self.seq_cur ].time[self.animation_index]
		else
			anim_time = self.sequences[ self.seq_cur ].time[self.animation_index] - self.sequences[ self.seq_cur ].time[self.animation_index - 1]
		end
		ui.bu_info_anim_time:set_text( string.format( "Anim Time : %.2f / %.2f s", self.anim_time, anim_time ) )
	end
end

function meu:create_sequence( meu_fbx, data, name )
	local sequence = {}
	sequence.data = data
	sequence.count = 0
	sequence.time = {}
	sequence.total_time = 0
	sequence.name = name
	for i, val in IPAIRS( sequence.data ) do
		if meu_fbx.animations[ val ] then
			sequence.count = sequence.count + 1
			local time = meu_fbx.animations[ val ].end_time - meu_fbx.animations[ val ].start_time
			sequence.total_time = sequence.total_time + time
			sequence.time[ i ] = sequence.total_time
		else
	--		self:print( "Error finding animation "..val )
		end
	end
	--table.print( sequence, "sequence", 2 )
	return sequence
end

function meu:define_animations()
	self:print( "define aninmations" )
	self.sequences = {}
	self.animations_loaded = false
	local m = self:get_meu_by_name_no_error( name )
	self.seq_count = 0

	if not m then
		self:print( "can't find fbx ")
	end
	if m.b_texs_loaded == false then
		m:draw()
	end

	if m and m.animations and m.b_animations_loaded == true then
		m:enable_animation()
		local data = {}

--		self.seq_count = self.seq_count + 1
		data = { "TURTL_SWIM_SLOW", "TURTL_SWIM_SLOW", "TURTL_SWIM_to_45_Down", "TURTL_45_Down", "TURTL_45_Down", "TURTL_45_Down", "TURTL_45_to_SWIM_Down",
		"TURTL_SWIM_to_45_Down", "TURTL_45_Down", "TURTL_45_to_SWIM_Down", "TURTL_SWIM_SLOW" }
		self.sequences[ "Enter KL" ] = self:create_sequence( m, data, "Enter KL" )

		data = { "TURTL_SWIM_090_Right", "TURTL_SWIM_090_Right", "TURTL_SWIM_090_Right", "TURTL_SWIM_090_Right" }
		self.sequences[ "KL2 Calme" ] = self:create_sequence( m, data, "KL2 Calme" )

		-- junction point
		data = { "TURTL_SWIM_FAST", "TURTL_SWIM_FAST", "TURTL_SWIM_030_Left", "TURTL_SWIM_030_Left", "TURTL_SWIM_FAST", "TURTL_SWIM_FAST", "TURTL_SWIM_FAST",
		"TURTL_SWIM_FAST", "TURTL_SWIM_FAST", "TURTL_SWIM_030_Right", "TURTL_SWIM_FAST", "TURTL_SWIM_030_Right", "TURTL_SWIM_FAST", "TURTL_SWIM_FAST", "TURTL_SWIM_FAST",
		}
		self.sequences[ "Exit KL" ] = self:create_sequence( m, data, "Exit KL" )

		self.graph_index = 1
		self.graph = {
			start_pos = { x = .0, y = .0, z = .0 },		-- start position of the animations
			start_angle = 0.5,							-- start angle of the animation
			animations = {
				{ seq = "Enter KL", interactive = 2, next = 3 },
				{ seq = "KL2 Calme", next = 3 },
				{ seq = "Exit KL" },
			}
		}
		self.animations_loaded = true
	--	table.print( self.sequences, "self.sequences[ 1 ]", 4)
	else
	end
	self.seq_cur = "Enter KL"
end

function meu:update()
	local t = self:update_time()
--	self:print( "Time "..t )
--self.seq_cur = 6
	--local t = nil
--	self.animations_loaded = false
	self.b_draw_turtle = false
	local m = self:get_meu_by_name_no_error( name )
	if m then
		local b_is = app:is_kl() or app:is_pc_dev()
		if not b_is then
		-- dev pc
		elseif b_is == true then
			m:set_mu_value( 1 )
			--self:update_anim()
			self.b_draw_turtle = true
		else
			m:set_mu_value( 0 )
			return
		end
	end


	if not self.animations_loaded then
		self:define_animations()
	end

	if t and self.animations_loaded then
		local m = self:get_meu_by_name_no_error( name )
		if m then
			m:enable_animation()
			self.seq_cur = self.graph.animations[ self.graph_index ].seq
			local anim_name = self.sequences[ self.seq_cur ].data[self.animation_index]
			if self.animation_index == 1 then
				self.anim_time = t
			else
				local total_time = self.sequences[ self.seq_cur ].time[self.animation_index - 1]
				self.anim_time = t - total_time
			end
			if t > self.sequences[ self.seq_cur ].time[self.animation_index] then
				-- segment finished
				if self.b_verbose then
				--	self:print( "Segment done "..self.animation_index )
				end
				self.animation_index = self.animation_index + 1
				if self.animation_index > self.sequences[self.seq_cur ].count then
					if self.b_verbose then
					--	self:print( "Sequence done "..self.seq_cur )
					end
					if self.graph.animations[ self.graph_index ].interactive and self.b_interaction then
						self.b_interaction = false
						-- interaction triggered
						self:print( "Next sequence interactive" )
						local delta_x, delta_y, delta_z, delta_rot_y = m:get_position_rotation( self.sequences[ self.seq_cur ].data[self.animation_index - 1] )
						local dz, dx = math.rotate_ab_turn( delta_z, delta_x, self.last_rot )
						self.anim_x = self.anim_x + dx
						self.anim_z = self.anim_z + dz
						self.anim_y = self.anim_y + delta_y
						self.curve_pos = { x = self.anim_x, y = self.anim_y, z = self.anim_z }
						self.rot = self.rot + delta_rot_y
						self.rot = wrap_01( self.rot )
						self.graph_index = self.graph.animations[ self.graph_index ].interactive
						self.seq_cur = self.graph.animations[ self.graph_index ].seq
					--	self:print( "Sequence done, next is interactive")
					elseif  self.graph.animations[ self.graph_index ].next then
						self:print( "Next sequence non interactive" )
						-- no interaction
						local delta_x, delta_y, delta_z, delta_rot_y = m:get_position_rotation( self.sequences[ self.seq_cur ].data[self.animation_index - 1] )
						local dz, dx = math.rotate_ab_turn( delta_z, delta_x, self.last_rot )
						self.anim_x = self.anim_x + dx
						self.anim_z = self.anim_z + dz
						self.anim_y = self.anim_y + delta_y
						self.curve_pos = { x = self.anim_x, y = self.anim_y, z = self.anim_z }
						self.rot = self.rot + delta_rot_y
						self.rot = wrap_01( self.rot )
						self.graph_index = self.graph.animations[ self.graph_index ].next
						self.seq_cur = self.graph.animations[ self.graph_index ].seq
						self:print( "new graph index is "..self.graph_index..", "..self.seq_cur)
					else
						-- end of animations
						self:reset()
					end
					self.animation_index = 1
					self.anim_time = .0
					self:restart_time()
				else
					local delta_x, delta_y, delta_z, delta_rot_y = m:get_position_rotation( self.sequences[ self.seq_cur ].data[self.animation_index - 1] )
					local dz, dx = math.rotate_ab_turn( delta_z, delta_x, self.last_rot )
					self.anim_x = self.anim_x + dx
					self.anim_z = self.anim_z + dz
					self.anim_y = self.anim_y + delta_y
					self.rot = self.rot + delta_rot_y
					self.rot = wrap_01( self.rot )
				end
				m:set_animation( self.sequences[ self.seq_cur ].data[self.animation_index], self.pos.x + self.anim_x, self.pos.y + self.anim_y, self.pos.z + self.anim_z, self.rot )
				self.animation_name = anim_name
				self.last_rot = self.rot
			else
				m:set_time(self.anim_time + m.animations[ anim_name ].start_time )
			--	self:get_position( self.anim_time + m.animations[ anim_name ].start_time )
			end
			m:set_play(false)
			m:set_animation_transform( self.pos.x + self.anim_x, self.pos.y + self.anim_y, self.pos.z + self.anim_z, self.rot )
		end
	end
end

function meu:get_colors()
	local color1 = BU_COLOR.COLORS[ self.color_index ]
	local color2 = { r = 1.0 - color1.r, g = 1.0 - color1.g, b = 1.0 - color1.b, a = 1.0 }
	self.color_index = self.color_index + 1
	if self.color_index > #BU_COLOR.COLORS then
		self.color_index = 17
	end
	return color1, color2
end

function meu:draw_graph_sequence( fbx, index, pos, angle )
	local seq_cur = self.graph.animations[ index ].seq
	--self:print( "draw_graph_sequence "..index )
	local end_pos, angle = self:draw_curve( fbx, seq_cur, pos, angle )
	if self.graph.animations[ index ].interactive then
	--	self:print( "interactive" )
		self:draw_graph_sequence( fbx, self.graph.animations[ index ].interactive, end_pos, angle )
	end
	if self.graph.animations[ index ].next then
		self:draw_graph_sequence( fbx, self.graph.animations[ index ].next, end_pos, angle )
	else

	end
end

function meu:draw_graph_all( start_pos, angle )
-- draw the graph
	local fbx = self:get_meu_by_name_no_error( name )
	if fbx then
		if fbx.animations and fbx.b_animations_loaded == true then
			self:draw_layers_begin()
			self:draw_layer( 1 )
			gol.push_matrix()
			local pos = table.copy_simple( start_pos )
			self:draw_graph_sequence( fbx, 1, pos, angle )
		--	table.print( end_pos, "end_pos", 2 )
			gol.pop_matrix()
			self:draw_layers_end()
		end
	end
end

function meu:draw_graph_index( index, start_pos, angle )
	-- draw the graph
		local fbx = self:get_meu_by_name_no_error( name )
		if fbx then
			if fbx.animations and fbx.b_animations_loaded == true then
				self:draw_layers_begin()
				self:draw_layer( 1 )
				gol.push_matrix()
				local pos = table.copy_simple( start_pos )


				local seq_cur = self.graph.animations[ index ].seq
				local end_pos, angle = self:draw_curve( fbx, seq_cur, pos, angle )
				if self.graph.animations[ index ].interactive then
				--	self:print( "interactive" )
					local seq = self.graph.animations[ self.graph.animations[ index ].interactive ].seq
					self:draw_curve( fbx, seq, end_pos, angle )
				end
				if self.graph.animations[ index ].next then
					local seq = self.graph.animations[ self.graph.animations[ index ].next ].seq
					self:draw_curve( fbx, seq, end_pos, angle )
				end
				gol.pop_matrix()
				self:draw_layers_end()
			end
		end
	end

function meu:draw_curve( fbx_meu, animation, pos, angle )
	local index = 0
	if fbx_meu.animations and fbx_meu.b_animations_loaded == true and self.sequences[ animation ] then
		--self:print( "draw_curves")
		local sequence = self.sequences[ animation ]
		local start_pos = table.copy_simple( pos )
		local last_rot = angle
		local end_pos = { x = .0, y = .0, z =.0 }
		local color1, color2 = self:get_colors()
	--	self:print( "here" )
		for i = 1, sequence.count do
			local id = index % 2
			index = index + 1
			if id == 0 then
				gol.color( color1.r, color1.g, color1.b, color1.a )
			else
				gol.color( color2.r, color2.g, color2.b, color2.a )
			end
			local animation_name = sequence.data[ i ]

			local last_pos = table.copy_simple( start_pos )

			for j = 1, fbx_meu.animations[ animation_name ].point_count do
				local dy = fbx_meu.animations[ animation_name ].points[ j ].y
				local dz, dx = math.rotate_ab_turn( fbx_meu.animations[ animation_name ].points[ j ].z, fbx_meu.animations[ animation_name ].points[ j ].x, last_rot )
				gol.draw_lines_3d( last_pos.x, last_pos.y, last_pos.z, start_pos.x + dx, start_pos.y + dy, start_pos.z + dz )
				last_pos.x = start_pos.x + dx
				last_pos.y = start_pos.y + dy
				last_pos.z = start_pos.z + dz
			end
			local delta_x, delta_y, delta_z, delta_rot_y = fbx_meu:get_position_rotation( sequence.data[ i ] )
			local dz, dx = math.rotate_ab_turn( delta_z, delta_x, last_rot )
			local new_pos = {}
			new_pos.x = start_pos.x + dx
			new_pos.y = start_pos.y + delta_y
			new_pos.z = start_pos.z + dz
			gol.draw_lines_3d( last_pos.x, last_pos.y, last_pos.z, new_pos.x, new_pos.y, new_pos.z )
			start_pos = table.copy_simple( new_pos )
			last_rot = last_rot + delta_rot_y
			end_pos = table.copy_simple( new_pos )
		end
		return end_pos, last_rot
	end
	return nil, .0
end

function meu:draw()
	if self.b_draw_turtle == false then return end
	if self.b_draw_graph then
		self.color_index = 17
		if self.b_draw_curve then
			local pos = {}
			pos.x = self.pos.x + self.curve_pos.x
			pos.y = self.pos.y + self.curve_pos.y
			pos.z = self.pos.z + self.curve_pos.z
			self:draw_graph_index( self.graph_index, pos, self.angle )
		else
			self:draw_graph_all( self.pos, self.angle )
		end
	end
end
