
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

	iy = self:define_time( 		{ix,iy, 	1, SY}, 500, false ) 

	--bu = self:add_button(	{	ix, iy,			SY, SY },	"Draw FBX", 		self, "b_draw_fbx",	true )
	iy = iy + SY
	bu = self:add_trig_method(	{ix, 		iy,	SX,SY},	"Audience QUIET", 	self, "audience_quiet" )
	bu = self:add_trig_method(	{ix+SX,		iy,	SX,SY},	"Audience NERVE", 	self, "audience_nerve" )
	bu = self:add_trig_method(	{ix+SX*2,	iy,	SX,SY},	"Audience NONE", 	self, "audience_none" )

	iy = iy + SY

	bu = self:add_button(		{ix,iy,			SY,SY},	"Draw Animation", 	self, "b_draw_anim",	false )
	iy = iy + SY
	bu = self:add_trig_method(	{ix,iy,			SX,SY},	"Restart Anim", 	self, "restart_anim" )
	iy = iy + SY

	ui.bu_animation =
		self:add_text_info(		{ix,iy,		16,SY*.5},	"Animation Name" )
	iy = iy + SY *.5
	ui.bu_animation_time =
		self:add_text_info(		{ix,iy,		16,SY*.5},	"Animation Time :" )
	iy = iy + SY *.5
end

function meu:init()
	local ref = self.ref
	self.animations = {}
	self.b_audience_present = false
	self.b_audience_quiet = false
	self.b_audience_nerve = false
	self.loop_cycle = 0
	self.animation_name = ""
	self.b_anim_active = false
	self.b_animation_done = false
	self.anim_time = 0.0
	self:define_anim()
end

function meu:reset()
	self:print( "reset" )
end

function meu:interaction()
	self.b_interaction = true
end


function meu:format_time( time )
	local seconds = 0
	local minutes = 0
	local hours = 0
	local mins = 0
	if time >= 60 then
		seconds = time - math.floor( time / 60. ) * 60.
		mins = ( time - seconds ) / 60
	else
		seconds = time
	end
	if mins > 60 then
		minutes = mins - math.floor( mins / 60. ) * 60.
		hours = ( mins - minutes ) / 60
	else
		minutes = mins
	end
	local str = ""
	if hours > 0 then
		str = string.format( "%02d:%02d:%.2f", hours, minutes, seconds )
	else
		str = string.format( "%02d:%04.2f", minutes, seconds )
	end
	--	self:print( "format_time "..str)
	return str
end

function meu:update_ui()
	local ui = self.ui

	self:update_time_ui()

	--bu:set_value_load_save( false )
	if self.animations[ self.name ] and self.animations[ self.name ][ self.animation_name ] then
		ui.bu_animation:set_text( "Animation Name : "..self.animations[ self.name ][ self.animation_name ].comment )
		local duration_str = ""
		if self.animation_name ~= "" then
			duration_str = self:format_time( self.animations[ self.name ][ self.animation_name ].duration )
		end
		local anim_time_str = self:format_time( self.anim_time )
		ui.bu_animation_time:set_text( "Animation Time : "..anim_time_str.."/"..duration_str )
	end
end

function meu:audience_quiet()
	self.b_audience_present = true
	self.b_audience_quiet = true
	self.b_audience_nerve = false
end

function meu:audience_nerve()
	self.b_audience_present = true
	self.b_audience_quiet = false
	self.b_audience_nerve = true
end

function meu:audience_none()
	self.b_audience_present = false
	self.b_audience_nerve = false
	self.b_audience_quiet = false
	self.loop_cycle = 0
end

function meu:update_animation( anim )
	for key, val in PAIRS( anim ) do
		val.start_time_s = val.start_time / 60.
		val.end_time_s = val.end_time / 60.
		val.duration = val.end_time_s - val.start_time_s
	end
end

function meu:define_anim()
	local whale = {
		GR10 = { comment = "arrivee", start_time = 0, end_time = 1067, b_loop = false },
		GR20 = { comment = "observation", start_time = 1264, end_time = 1461, b_loop = true },
		GR31 = { comment = "bb_move", start_time = 1658, end_time = 2643, b_loop = true },
		GR50 = { comment = "bb_looping", start_time = 2840, end_time = 3206, b_loop = true },
		GR60 = { comment = "bb_interactive", start_time = 3797, end_time = 4435, b_loop = true },
		GR70 = { comment = "bb_dos_interactive", start_time = 4829, end_time = 5814, b_loop = true },
		GR80 = { comment = "sortie", start_time = 5814, end_time = 7005, b_loop = false },
	}
--	self.animations[ "WHALE" ] = whale
	self:update_animation( whale )
	self.name = "WHALE"
	self.animations[ self.name ] = whale
	self.fbx_name = "Fbx_WHALE"
end


function meu:set_play( b )
	self.__time.b_play = b
end


function meu:start_anim()
	self.anim_time = 0.0
	self.b_draw_anim = true
	self.b_anim_active = true
	self.b_animation_done = false
	self.start_seq = aaa.time.t
	self:set_play( true )
	self:restart_anim()
end

function meu:stop_anim()
	self.b_draw_anim = false
	self:set_play( false )
	local meu_fbx = self:get_meu_by_name_no_error( self.fbx_name )
	self.b_anim_active = false
	if meu_fbx then
		meu_fbx:set_mu_value( 0 )
	end

	--self:restart_anim()
end


function meu:restart_anim()
	if self.b_anim_active == true then
		self.animation_name = ""
		self.b_animation_done = false
		self.start_anim_t = 0
		self.loop_cycle = 0
		self:audience_none()
		self:restart_time()
	end
end

function meu:update_fbx_start_time( meu_fbx, t )
	local start_time = self.animations[ self.name ][ self.animation_name ].start_time_s
	meu_fbx:set_time( start_time )
	self:fbx_send_net_command( self.animation_name, start_time )
	if app:is_seq_looping() then
		self:restart_time()
		t = 0
	end
	self.start_anim_t = t
	self:print( "Start Animation : "..self.animation_name.." at "..start_time )
end

function meu:fbx_send_net_command( name, start_time )
	if app:is_location( "KR3") then
		app:send_method_verbose( "update_whale_animation", "\""..name.."\","..start_time )
	end
end

function meu:net_fbx_animation( animation_name, start_time )
	if app:is_kr() and not app:is_location( "KR3") then
		self.animation_name = animation_name
		local meu_fbx = self:get_meu_by_name_no_error( self.fbx_name )
		if meu_fbx then
			meu_fbx:set_time( start_time )
		end
	end
end

function meu:fbx_update_time( meu_fbx, t, next )
	local duration = self.animations[ self.name ][ self.animation_name ].duration
	if t - self.start_anim_t >= duration then
		self.animation_name = next
		self:update_fbx_start_time( meu_fbx, t )
	else
		local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
		self.anim_time = t - self.start_anim_t
		meu_fbx:set_time( meu_time )
		self:fbx_send_net_command( self.animation_name, meu_time )
	end
end

--local seq_time = 117.0
function meu:update_anim()
	local t = self:update_time()
	--self:set_play( true )
	local meu_fbx = self:get_meu_by_name_no_error( self.fbx_name )
	if t and meu_fbx then
		meu_fbx:set_play(false)
		if app:is_location( "KR3" ) or app:is_pc_dev() then
			if self.b_animation_done == false then
				if self.animation_name == "" then
					-- must start animation
					self.animation_name = "GR10"
					local start_time = self.animations[ self.name ][ self.animation_name ].start_time_s
					--self:restart_titan()
					self:set_time_speed( 0.5 )
					self:restart_time()
					self.start_anim_t = 0
					meu_fbx:set_time( start_time )
					self:fbx_send_net_command( self.animation_name, start_time )
					self:print( "Start Animation : "..self.animation_name.." at "..start_time )
				elseif self.animation_name == "GR10" then
					self:set_time_speed( 0.5 )
					--self:fbx_update_time( meu_fbx, t, "GR20" )
					local duration = self.animations[ self.name ][ self.animation_name ].duration
					if t - self.start_anim_t >= duration then
					--	self:print( "dgdgf")
						if self.b_audience_present then
							self:print( "audience present")
							if self.b_audience_nerve then
								self.animation_name = "GR31"
							--	self:set_time_speed( 1.0 )
							else
								local index = math.random() * 3
								if index < 1.0 then
									self.animation_name = "GR50"
								elseif index < 2.0 then
									self.animation_name = "GR60"
								else
									self.animation_name = "GR70"
								end
							end
							self:update_fbx_start_time( meu_fbx, t )
							self:print( "self.animation_name "..self.animation_name)
						else
							self:fbx_update_time( meu_fbx, t, "GR20" )
						end
					else
						local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
					--	self:print( "For anim index 2, setting meu time : "..meu_time )
						meu_fbx:set_time( meu_time )
						self:fbx_send_net_command( self.animation_name, meu_time )
					end
				elseif self.animation_name == "GR20" then
					self:set_time_speed( 1.0 )
					local duration = self.animations[ self.name ][ self.animation_name ].duration
					if t - self.start_anim_t >= duration then
						if self.b_audience_present then
							self.loop_cycle = self.loop_cycle + 1
						--	self:print( "Loop cycle 2 : "..self.loop_cycle )
							local seq_time_left = app:get_seq_cur_time_left()
					--		local seq_time_left = seq_time - ( aaa.time.t - self.start_seq )
							--if self.loop_cycle > 4 then
							if seq_time_left < 20.0 then
								if app:is_seq_looping() then
									--self:set_time( 0 )
									self:print( "Sequence is looping ")
									self:restart_time()
									t = 0
									if self.b_audience_nerve then
										self.animation_name = "GR31"
									else
										local index = math.random() * 3
										if index < 1.0 then
											self.animation_name = "GR50"
										elseif index < 2.0 then
											self.animation_name = "GR60"
										else
											self.animation_name = "GR70"
										end
									end
									self:set_time_speed( 0.5 )
								else
									self:print( "Ending Loop Cycle" )
									self.animation_name = "GR80"
									self:set_time_speed( 0.5 )
									self:update_fbx_start_time( meu_fbx, t )
									self.loop_cycle = 0
								end
							else
								if self.b_audience_nerve then
									self.animation_name = "GR31"
								--	self:set_time_speed( 1.0 )
								else
									local index = math.random() * 3
									if index < 1.0 then
										self.animation_name = "GR50"
									elseif index < 2.0 then
										self.animation_name = "GR60"
									else
										self.animation_name = "GR70"
									end
								end
								self:set_time_speed( 0.5 )
							--	self:fbx_update_time( meu_fbx, t, self.animation_name )
							end
							self:update_fbx_start_time( meu_fbx, t )
						else
							local seq_time_left = app:get_seq_cur_time_left()
							self.loop_cycle = self.loop_cycle + 1
							--local seq_time_left = seq_time - ( aaa.time.t - self.start_seq )
							--	self:print( "Loop cycle 2 : "..self.loop_cycle )
						--	if self.loop_cycle > 4 then
							if seq_time_left < 20.0 then
								if app:is_seq_looping() then
									self:print( "Sequence is looping 2")
									self:restart_time()
									t = 0
									if self.b_audience_nerve then
										self.animation_name = "GR31"
									else
										local index = math.random() * 3
										if index < 1.0 then
											self.animation_name = "GR50"
										elseif index < 2.0 then
											self.animation_name = "GR60"
										else
											self.animation_name = "GR70"
										end
									end
									self:set_time_speed( 0.5 )
									self:update_fbx_start_time( meu_fbx, t )
								else
									self:print( "Ending Loop Cycle" )
									self.animation_name = "GR80"
									self:set_time_speed( 0.5 )
									self:update_fbx_start_time( meu_fbx, t )
									self.loop_cycle = 0
								end
							else
								self:update_fbx_start_time( meu_fbx, t )
							end
						end
					else
						local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
						self.anim_time = t - self.start_anim_t
					--	self:print( "For anim index 2, setting meu time : "..meu_time )
						meu_fbx:set_time( meu_time )
						self:fbx_send_net_command( self.animation_name, meu_time )
					end
				elseif self.animation_name == "GR31" then
					self:set_time_speed( 0.5 )
					self:fbx_update_time( meu_fbx, t, "GR20" )
				elseif self.animation_name == "GR50" then
					self:set_time_speed( 0.5 )
					self:fbx_update_time( meu_fbx, t, "GR20" )
				elseif self.animation_name == "GR60" then
					self:set_time_speed( 0.5 )
					self:fbx_update_time( meu_fbx, t, "GR20" )
				elseif self.animation_name == "GR70" then
					self:set_time_speed( 0.5 )
					self:fbx_update_time( meu_fbx, t, "GR20" )
				elseif self.animation_name == "GR80" then
					self:set_time_speed( 0.5 )
					local duration = self.animations[ self.name ][ self.animation_name ].duration
					if t - self.start_anim_t >= duration then
						--self.animation_name = 0
						--self:restart_anim()
						self.b_animation_done = true
					else
						local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
						self.anim_time = t - self.start_anim_t
						meu_fbx:set_time( meu_time )
						self:fbx_send_net_command( self.animation_name, meu_time )
					end
				--	self:print( "Time fbx : ".. meu_fbx:get_time() )
				end
			end
		end
	end
end

function meu:update_interactivity()
	if app.const then
		local interactivity = app:get_presence_SO( "KR3" )
		local delta_x = 0.3
		local screen = 3
		local x_min = app.const.mona.k_lar * ( screen - 1 ) + app.const.mona.kc_interval * ( screen - 1 ) + delta_x
		local x_max = app.const.mona.k_lar * screen + app.const.mona.kc_interval * ( screen - 1 ) - delta_x
	--	self:print( "gere")
		--if inside( interactivity.x, x_min, x_max ) then
		--	self:print( "gere "..interactivity.coverage)
			if interactivity.coverage > 0.3 then
			--	self:print( "here 0.5")
				self.b_audience_quiet = false
				self.b_audience_nerve = true
				self.b_audience_present = true
			elseif interactivity.coverage > 0.005 then
			--	self:print( "here 0.1")
				self.b_audience_quiet = true
				self.b_audience_nerve = false
				self.b_audience_present = true
			else
				self.b_audience_present = false
			end
	--	else
	--		self.b_audience_present = false
	--	end
	end
end

function meu:update()

	local meu_fbx = self:get_meu_by_name_no_error( self.fbx_name )
	self.b_anim_active = false
	if meu_fbx then
		if self.b_draw_anim then
			local b_is = app:is_kr() or app:is_pc_dev()
			if not b_is then
			-- dev pc
			elseif b_is == true then
				meu_fbx:set_mu_value( 1 )
				self.b_anim_active = true
				local bdd_fbx = meu_fbx.ref.bdd_fbx
				bdd_fbx:set_translation( 0.0, 0.0, 0.0 )
				bdd_fbx:set_rotation( 0.0, 0.0, 0.0 )
				if app:is_location( "KR3") or app:is_pc_dev() then
					self:update_interactivity()
				end
				self:update_anim()
			else
				meu_fbx:set_mu_value( 0 )
			end
		else
			meu_fbx:set_mu_value( 0 )
		end
	end
end

function meu:draw()
end
