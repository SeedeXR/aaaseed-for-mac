
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

	iy = self:define_time( 		{ix,iy,		1,SY}, 500, false )

	--bu = self:add_button(	{	ix, iy,			SY, SY },	"Draw FBX", 		self, "b_draw_fbx",	true )
	iy = iy + SY
	bu = self:add_trig_method(	{ix, 		iy,		SX,SY},	"Audience QUIET", 	self, "audience_quiet" )
	bu = self:add_trig_method(	{ix+SX,	iy,			SX,SY},	"Audience NERVE", 	self, "audience_nerve" )
	bu = self:add_trig_method(	{ix+SX*2,	iy,		SX,SY},	"Audience NONE", 	self, "audience_none" )

	iy = iy + SY

	bu = self:add_button(		{ix,iy,				SY,SY},	"Draw Animation", 	self, "b_draw_anim",	false )
	iy = iy + SY
	bu = self:add_trig_method(	{ix,iy,				SX,SY},	"Restart Anim", 	self, "restart_anim" )
	iy = iy + SY
	ui.bu_animation =
		self:add_text_info(		{ix,iy,			16,SY*.5},	"Animation Name" )

	iy = iy + SY *.5
	ui.bu_animation_time =
		self:add_text_info(		{ix,iy,			16,SY*.5},	"Animation Time :" )
	iy = iy + SY *.5
end

function meu:init()
	local ref = self.ref
	self.animations = {}
	self.b_audience_present = false
	self.b_audience_quiet = false
	self.b_audience_nerve = false
	--self.loop_cycle = 0
	self.animation_name = ""
	self.anim_time = 0.0
	self:define_titan()
end

function meu:reset()
	self:print( "reset" )
end

function meu:interaction()
	self.b_interaction = true
end

function meu:format_time( time )
	local str = ""
	if time then
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
		if hours > 0 then
			str = string.format( "%02d:%02d:%.2f", hours, minutes, seconds )
		else
			str = string.format( "%02d:%04.2f", minutes, seconds )
		end
	end
	--	self:print( "format_time "..str)
	return str
end

function meu:update_ui()
	local ui = self.ui

	self:update_time_ui()
	ui.bu_animation:set_text( "Animation Name : "..self.animations[ self.name ][ self.animation_name ].comment )
	local duration_str = ""
	if self.animation_name ~= "" then
		duration_str = self:format_time( self.animations[ self.name ][ self.animation_name ].duration )
	end
--	self.anim_time = .0
	local anim_time_str = self:format_time( self.anim_time )
	ui.bu_animation_time:set_text( "Animation Time : "..anim_time_str.."/"..duration_str )

end



function meu:set_play( b )
	self.__time.b_play = b
end


function meu:start_anim()
	self.b_draw_anim = true
	self.b_anim_active = true
	self.anim_time = 0.0
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
	--self.loop_cycle = 0
end

function meu:update_animation( anim )
	for key, val in PAIRS( anim ) do
		val.start_time_s = val.start_time / 60.
		val.end_time_s = val.end_time / 60.
		val.duration = val.end_time_s - val.start_time_s
	end
end

function meu:define_titan()
	local titan = {
		GR10 = { comment = "arrivee", start_time = 0, end_time = 743, b_loop = false },
		GR20= { comment = "sable", start_time = 743, end_time = 1026, b_loop = true },
		GR20a = { comment = "", start_time = 3440, end_time = 3723, b_loop = true },
		GR21 = { comment = "trajet_gr30", start_time = 1026, end_time = 1186, b_loop = false },
		GR30 = { comment = "observation", start_time = 1186, end_time = 1732, b_loop = true },
		GR31 = { comment = "", start_time = 3348, end_time = 3440, b_loop = false },
		GR42a = {  comment = "trajet_gr42", start_time = 1732, end_time = 1919, b_loop = false },
		GR42 = {  comment = "intimidation", start_time = 1919, end_time = 2197, b_loop = false },
		GR42c = {  comment = "", start_time = 2197, end_time = 2360, b_loop = false },
		GR50 = { comment = "attack", start_time = 2633, end_time = 3075, b_loop = true },
		GR60 = {  comment = "sortie", start_time = 4156, end_time = 4560, b_loop = true },
--			{ name = "sortie", start_time = 5814, end_time = 7005, b_loop = true },
	}
	self:update_animation( titan )
--	table.print( titan, "TITAN", 3 )
	self.name = "TITAN"
	self.animations[ self.name ] = titan
	self.fbx_name = "Fbx_C_TITAN"
end

function meu:restart_anim()
	self.animation_name = ""
	self.start_anim_t = 0
--	self.loop_cycle = 0
	self.anim_time = 0.0
	self:audience_none()
	self:restart_time()
end

function meu:update_fbx_start_time( meu_fbx, t )
	local start_time = self.animations[ self.name ][ self.animation_name ].start_time_s
	meu_fbx:set_time( start_time )
	if app:is_seq_looping() then
		self:restart_time()
		t = 0
	end
	self.start_anim_t = t
	--self:print( "Start Animation : "..self.animation_name.." at "..start_time )
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
	end
end

function meu:update_titan()
	local t = self:update_time()
	local meu_fbx = self:get_meu_by_name_no_error( self.fbx_name )
	if t and meu_fbx then
		meu_fbx:set_play(false)
		if self.animation_name == "" then
			-- must start animation
			self.animation_name = "GR10"
			local start_time = self.animations[ self.name ][ self.animation_name ].start_time_s
			--self:restart_titan()
			self:restart_time()
			self.start_anim_t = 0
			meu_fbx:set_time( start_time )
		--	self:print( "Start Animation : "..self.animation_name.." at "..start_time )
		elseif self.animation_name == "GR10" then
			self:fbx_update_time( meu_fbx, t, "GR20" )
		elseif self.animation_name == "GR20" then
			local duration = self.animations[ self.name ][ self.animation_name ].duration
			if t - self.start_anim_t >= duration then
				if self.b_audience_present then
					self.animation_name = "GR21"
					self:update_fbx_start_time( meu_fbx, t )
				else
				--	self.loop_cycle = self.loop_cycle + 1
					local seq_time_left = app:get_seq_cur_time_left()
				--	self:print( "Loop cycle 2 : "..self.loop_cycle )
					if seq_time_left < 10.0 then
						self:print( "Ending Loop Cycle" )
						self.animation_name = "GR21"
						self:update_fbx_start_time( meu_fbx, t )
					else
						self:update_fbx_start_time( meu_fbx, t )
					end
				end
			else
				local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
				self.anim_time = t - self.start_anim_t
			--	self:print( "For anim index 2, setting meu time : "..meu_time )
				meu_fbx:set_time( meu_time )
			end
		elseif self.animation_name == "GR21" then
			self:fbx_update_time( meu_fbx, t, "GR30" )
		elseif self.animation_name == "GR30" then
			local duration = self.animations[ self.name ][ self.animation_name ].duration
			if t - self.start_anim_t >= duration then
				local seq_time_left = app:get_seq_cur_time_left()
				if seq_time_left < 10.0 then
					if app:is_seq_looping() then
						self:print( "Sequence is looping" )
					--	self:restart_time()
					--	t = 0
						self.animation_name = "GR30"
						self:update_fbx_start_time( meu_fbx, t )
					else
						self.animation_name = "GR60"
						self:update_fbx_start_time( meu_fbx, t )
					end
				elseif self.b_audience_present then
					if self.b_audience_nerve then
						if math.random() < .5 then
							self.animation_name = "GR42a"
						else
							self.animation_name = "GR50"
						end
						self:update_fbx_start_time( meu_fbx, t )
					else
						self.animation_name = "GR31"
						self:update_fbx_start_time( meu_fbx, t )
					end
				else
					-- retour en GR20 ou sortie ?
					self.animation_name = "GR30"
					self:update_fbx_start_time( meu_fbx, t )
				end
			else
				local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
				self.anim_time = t - self.start_anim_t
				meu_fbx:set_time( meu_time )
			end
		elseif self.animation_name == "GR31" then
			self:fbx_update_time( meu_fbx, t, "GR20" )
		elseif self.animation_name == "GR50" then
			self:fbx_update_time( meu_fbx, t, "GR30" )
		elseif self.animation_name == "GR42a" then
			self:fbx_update_time( meu_fbx, t, "GR42" )
		elseif self.animation_name == "GR42" then
			self:fbx_update_time( meu_fbx, t, "GR42c" )
		elseif self.animation_name == "GR42c" then
			self:fbx_update_time( meu_fbx, t, "GR30" )
		elseif self.animation_name == "GR60" then
			local duration = self.animations[ self.name ][ self.animation_name ].duration
			if t - self.start_anim_t >= duration then
				--self.animation_name = 0
				self:restart_anim()
			else
				local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
				self.anim_time = t - self.start_anim_t
				meu_fbx:set_time( meu_time )
			end
		--	self:print( "Time fbx : ".. meu_fbx:get_time() )
		end
	end
end

function meu:update_interactivity()
	if app.const then
		local interactivity = app:get_presence_SO( "KR3" )
		local delta_x = 0.3
		local screen = 3
		local x_min = app.const.mona.k_lar * ( screen - 1 ) + app.const.mona.kc_interval * ( screen - 1 ) + delta_x
		local x_max = app.const.mona.k_lar * ( screen ) + app.const.mona.kc_interval * ( screen - 1 ) - delta_x
--	self:print( "gere")
--table.print( interactivity, "titan interactivity", 3)
	--if inside( interactivity.x, x_min, x_max ) then
	--	self:print( "gere "..interactivity.coverage)
		if interactivity.coverage > 0.1 then
	--		self:print( "Titan b_audience_nerve")
			self.b_audience_quiet = false
			self.b_audience_nerve = true
			self.b_audience_present = true
		elseif interactivity.coverage > 0.005 then
	--		self:print( "Titan b_audience_quiet")
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
	if self.b_draw_anim then
		local meu_fbx = self:get_meu_by_name_no_error( self.fbx_name )
		if meu_fbx then
			local b_is = app:is_kr( 3 ) or app:is_pc_dev()
			if not b_is then
			-- dev pc
			elseif b_is == true then
				meu_fbx:set_mu_value( 1 )
				self:update_interactivity()
				self:update_titan()
			else
				meu_fbx:set_mu_value( 0 )
			end
		end
	end
end

function meu:draw()
end
