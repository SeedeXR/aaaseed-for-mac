
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

	iy = self:define_time( 	{ix,iy,		1,SY}, 500, false )

	--bu = self:add_button(	{	ix, iy,			SY,SY },	"Draw FBX", 		self, "b_draw_fbx",	true )
	iy = iy + SY
	self:add_trig_method(	{ix,iy,			SX,SY},	"Audience QUIET", 	self, "audience_quiet" )
	self:add_trig_method(	{ix+SX,		iy,	SX,SY},	"Audience NERVE", 	self, "audience_nerve" )
	self:add_trig_method(	{ix+SX*2,	iy,	SX,SY},	"Audience NONE", 	self, "audience_none" )

	iy = iy + SY

	self:add_button(		{ix,iy,			SY,SY},	"Draw Animation", 	self, "b_draw_anim",	false )
	iy = iy + SY
	self:add_trig_method(	{ix,iy,			SX,SY},	"Restart Anim", 	self, "restart_anim" )
	iy = iy + SY

	ui.bu_animation =
		self:add_text_info(	{ix,iy,		16,SY*.5},	"Animation Name" )
	iy = iy + SY *.5
	ui.bu_animation_time =
		self:add_text_info(	{ix,iy,		16,SY*.5},	"Animation Time :" )

	iy = iy + SY *.5
end

function meu:init()
	local ref = self.ref
	self.animations = {}
	self.b_animation_done = false
	self.b_audience_present = false
	self.b_audience_quiet = false
	self.b_audience_nerve = false
	self.loop_cycle = 0
	self.animation_name = ""
	self:define_dolph()
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
	if self.animations[ self.name ] and self.animations[ self.name ][ self.animation_name ] then
		ui.bu_animation:set_text( "Animation Name : "..self.animations[ self.name ][ self.animation_name ].comment )
		local duration_str = ""
		if self.animation_name ~= "" then
			duration_str = self:format_time( self.animations[ self.name ][ self.animation_name ].duration )
		end
	--	self:print( "dfgdfg"..self.anim_time )
		local anim_time_str = self:format_time( self.anim_time )
		ui.bu_animation_time:set_text( "Animation Time : "..anim_time_str.."/"..duration_str )
	end

end


function meu:set_play( b )
	self.__time.b_play = b
end


function meu:start_anim()
	self.anim_time = 0.0
	self.b_draw_anim = true
	self.b_anim_active = true
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

function meu:define_dolph()
	local dolph = {
		GR10 = { comment = "GR10", start_time = 250, end_time = 840, b_loop = false },
		GR20 = { comment = "GR20", start_time = 840, end_time = 1530, b_loop = true },
		GR32 = { comment = "GR32", start_time = 1530, end_time = 1900, b_loop = false },
		GR40 = { comment = "GR40", start_time = 1900, end_time = 2510, b_loop = true },
		GR41 = { comment = "GR41", start_time = 2750, end_time = 3740, b_loop = true },
		GR42 = { comment = "GR42", start_time = 2510, end_time = 2750, b_loop = true },
		GR50 = { comment = "GR50", start_time = 3980, end_time = 5100, b_loop = true },
		GR60 = { comment = "GR60", start_time = 5340, end_time = 6700, b_loop = true },
		GR70 = { comment = "GR70", start_time = 6940, end_time = 8550, b_loop = true },
		GR80 = { comment = "GR80", start_time = 8790, end_time = 10510, b_loop = true },
		GR100 = { comment = "GR100", start_time = 10750, end_time = 12190, b_loop = false },
	}
	self:update_animation( dolph )
	self.name = "DOLPH"
	self.animations[ self.name ] = dolph
	self.fbx_name = "Fbx_DOLPH_C"
end

function meu:restart_anim()
	self.animation_name = ""
	self.start_anim_t = 0
	self.loop_cycle = 0
	self.b_animation_done = false
	self:audience_none()
	self:restart_time()
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

function meu:fbx_update_time( meu_fbx, t, next )
	local duration = self.animations[ self.name ][ self.animation_name ].duration
	if t - self.start_anim_t >= duration then
		self.animation_name = next
		self:update_fbx_start_time( meu_fbx, t )
		self:fbx_send_net_command( self.animation_name, t )
	else
		local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
		meu_fbx:set_time( meu_time )
		self.anim_time = t - self.start_anim_t
		self:fbx_send_net_command( self.animation_name, meu_time )
	end
end

function meu:fbx_send_net_command( name, start_time )
	if app:is_location( "KR4") then
		app:send_method_verbose( "update_dolph_animation", "\""..name.."\","..start_time )
	end
end


function meu:net_fbx_animation( animation_name, start_time )
	if app:is_location( "KR3") or app:is_location( "KR2") or app:is_location( "KR1") then
		self.animation_name = animation_name
		local meu_fbx = self:get_meu_by_name_no_error( self.fbx_name )
		if meu_fbx then
			meu_fbx:set_time( start_time )
		end
	end
end


local seq_time = 117.0
function meu:update_anim()
	local t = self:update_time()
	local meu_fbx = self:get_meu_by_name_no_error( self.fbx_name )
	if t and meu_fbx then
		meu_fbx:set_play(false)
		if app:is_location( "KR4") or app:is_pc_dev() then
			if self.b_animation_done == false then
				if self.animation_name == "" then
					-- must start animation
					self.animation_name = "GR10"
					local start_time = self.animations[ self.name ][ self.animation_name ].start_time_s
					--self:restart_titan()
					self:restart_time()
					self.start_anim_t = 0
					meu_fbx:set_time( start_time )
					self.anim_time = start_time
					self:print( "Start Animation : "..self.animation_name.." at "..start_time )
					self:fbx_send_net_command( self.animation_name, start_time )
				elseif self.animation_name == "GR10" then
					self:fbx_update_time( meu_fbx, t, "GR20" )
				elseif self.animation_name == "GR20" then
					local duration = self.animations[ self.name ][ self.animation_name ].duration
					if t - self.start_anim_t >= duration then
						if self.b_audience_present then
							self.animation_name = "GR32"
							self:update_fbx_start_time( meu_fbx, t )
							self:fbx_send_net_command( self.animation_name, t )
						else
							local seq_time_left = seq_time - ( aaa.time.t - self.start_seq )
						--	self:print( "Seq time left "..seq_time_left)
							--self.loop_cycle = self.loop_cycle + 1
						--	self:print( "Loop cycle 2 : "..self.loop_cycle )
							if seq_time_left < 35.0 then
							--if self.loop_cycle > 4 then
								self:print( "Ending Loop Cycle" )
								self.animation_name = "GR32"
								self:update_fbx_start_time( meu_fbx, t )
								self:fbx_send_net_command( self.animation_name, t )
								self.loop_cycle = 0
							else
								self:update_fbx_start_time( meu_fbx, t )
								self:fbx_send_net_command( self.animation_name, t )
							end
						end
					else
						local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
					--	self:print( "For anim index 2, setting meu time : "..meu_time )
						meu_fbx:set_time( meu_time )
						self.anim_time = t - self.start_anim_t
						self:fbx_send_net_command( self.animation_name, meu_time )
					end
				elseif self.animation_name == "GR32" then
					self:fbx_update_time( meu_fbx, t, "GR40" )
				elseif self.animation_name == "GR40" then
					local duration = self.animations[ self.name ][ self.animation_name ].duration
					if t - self.start_anim_t >= duration then
					--	self.loop_cycle = self.loop_cycle + 1
					--	local seq_time_left = app:get_seq_cur_time_left()
					--	self:print( "Seq time left "..seq_time_left)
						local seq_time_left = seq_time - ( aaa.time.t - self.start_seq )
						if seq_time_left < 30.0 then
					--	if self.loop_cycle > 4 then
							if app:is_seq_looping() then
							--	self:restart_time()
							--	t = 0
								local index = math.random() * 4
								if index < 1.0 then
									self.animation_name = "GR50"
								elseif index < 2.0 then
									self.animation_name = "GR60"
								elseif index < 3.0 then
									self.animation_name = "GR70"
								else
									self.animation_name = "GR80"
								end
								self:fbx_send_net_command( self.animation_name, t )
								self:update_fbx_start_time( meu_fbx, t )

							else
								self.animation_name = "GR100"
								self:update_fbx_start_time( meu_fbx, t )
								self:fbx_send_net_command( self.animation_name, t )
								self.loop_cycle = 0
							end
						else
							if self.b_audience_present then
								if self.b_audience_nerve then
									self.animation_name = "GR41"
									self:update_fbx_start_time( meu_fbx, t )
								elseif self.b_audience_quiet then
									local index = math.random() * 4
									if index < 1.0 then
										self.animation_name = "GR50"
									elseif index < 2.0 then
										self.animation_name = "GR60"
									elseif index < 3.0 then
										self.animation_name = "GR70"
									else
										self.animation_name = "GR80"
									end
									self:fbx_send_net_command( self.animation_name, t )
									self:update_fbx_start_time( meu_fbx, t )
								end
							else
								-- retour en GR20 ou sortie ?
								self.animation_name = "GR60"
								self:fbx_send_net_command( self.animation_name, t )
								self:update_fbx_start_time( meu_fbx, t )
							end
						end
					else
						local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
						meu_fbx:set_time( meu_time )
						self.anim_time = t - self.start_anim_t
						self:fbx_send_net_command( self.animation_name, meu_time )
					end
				elseif self.animation_name == "GR41" then
					self:fbx_update_time( meu_fbx, t, "GR40" )
				elseif self.animation_name == "GR50" then
					self:fbx_update_time( meu_fbx, t, "GR40" )
				elseif self.animation_name == "GR60" then
					self:fbx_update_time( meu_fbx, t, "GR40" )
				elseif self.animation_name == "GR70" then
					self:fbx_update_time( meu_fbx, t, "GR40" )
				elseif self.animation_name == "GR80" then
					self:fbx_update_time( meu_fbx, t, "GR40" )
				elseif self.animation_name == "GR100" then
					local duration = self.animations[ self.name ][ self.animation_name ].duration
					if t - self.start_anim_t >= duration then
						--self.animation_name = 0
						self.b_animation_done = true
					--	self:restart_anim()
					else
						local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
						meu_fbx:set_time( meu_time )
						self.anim_time = t - self.start_anim_t
						self:fbx_send_net_command( self.animation_name, meu_time )
					end
				--	self:print( "Time fbx : ".. meu_fbx:get_time() )
				end
			end
		else
			if self.animation_name == "" then
			else
				--local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
				--meu_fbx:set_time( meu_time )
				--self:fbx_send_net_command( self.animation_name, meu_time )
			end
		end
	end
end

function meu:update_interactivity()
	if app.const then
		local interactivity = app:get_presence_SO( "KR4" )
		local delta_x = 0.3
		local screen = 4
		local x_min = app.const.mona.k_lar * ( screen - 1 ) + app.const.mona.kc_interval * ( screen - 1 ) + delta_x
		local x_max = app.const.mona.k_lar * ( screen ) + app.const.mona.kc_interval * ( screen - 1 ) + delta_x
		--local x_max = 20.
		if inside( interactivity.x, x_min, x_max ) then
			if interactivity.coverage > 0.1 then
				self.b_audience_quiet = false
				self.b_audience_nerve = true
				self.b_audience_present = true
			elseif interactivity.coverage > 0.005 then
				self.b_audience_quiet = true
				self.b_audience_nerve = false
				self.b_audience_present = true
			else
				self.b_audience_present = false
			end
		else
			self.b_audience_present = false
		end
	end
end

function meu:update()
	if self.b_draw_anim then
		local meu_fbx = self:get_meu_by_name_no_error( self.fbx_name )
		if meu_fbx then
			local b_is = app:is_kr() or app:is_pc_dev()
			if not b_is then
			-- dev pc
			elseif b_is == true then
				meu_fbx:set_mu_value( 1 )
				if app:is_location( "KR4") then
					self:update_interactivity()
				end
				self:update_anim()
			else
				meu_fbx:set_mu_value( 0 )
			end
		end
	end
end

function meu:draw()
end
