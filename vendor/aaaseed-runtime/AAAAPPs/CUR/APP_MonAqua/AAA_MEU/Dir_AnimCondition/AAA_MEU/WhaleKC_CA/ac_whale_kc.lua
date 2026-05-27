
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
	bu = self:add_trig_method(	{ix,		iy,	SX,SY},	"Audience QUIET", 	self, "audience_quiet" )
	bu = self:add_trig_method(	{ix+SX,		iy,	SX,SY},	"Audience NERVE", 	self, "audience_nerve" )
	bu = self:add_trig_method(	{ix+SX*2,	iy,	SX,SY},	"Audience NONE", 	self, "audience_none" )

	iy = iy + SY

	bu = self:add_button(		{ix,iy,			SY,SY},	"Draw Animation", 	self, "b_draw_anim",	false )
	iy = iy + SY
	bu = self:add_trig_method(	{ix,iy,			SX,SY},	"Restart Anim", 	self, "restart_anim" )
	iy = iy + SY
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
	self:define_anim()
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

function meu:define_anim()
	local whale = {
		GR10 = { comment = "arrivee", start_time = 0, end_time = 1067, b_loop = false },
		GR20 = { comment = "observation", start_time = 1264, end_time = 1461, b_loop = true },
		GR31 = { comment = "bb_move", start_time = 1658, end_time = 2643, b_loop = true },
		GR50 = { comment = "bb_looping", start_time = 2840, end_time = 3206, b_loop = true },
		GR60 = { comment = "bb_interative", start_time = 3797, end_time = 4435, b_loop = true },
		GR70 = { comment = "bb_dos_interactive", start_time = 4829, end_time = 5814, b_loop = true },
		GR80 = { comment = "sortie", start_time = 5814, end_time = 7005, b_loop = false },
	}
	self:update_animation( whale )
	self.name = "WHALE"
	self.animations[ self.name ] = whale
	self.fbx_name = "Fbx_WHALE"
end

function meu:set_play( b )
	self.__time.b_play = b
end

function meu:start_anim()
	self.b_draw_anim = true
	self.b_anim_active = true
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


function meu:restart_anim()
	if self.b_anim_active == true then
		self.animation_name = ""
		self.start_anim_t = 0
		self.loop_cycle = 0
		self:audience_none()
		self:restart_time()
	end
end

function meu:update_fbx_start_time( meu_fbx, t )
	local start_time = self.animations[ self.name ][ self.animation_name ].start_time_s
	meu_fbx:set_time( start_time )
	if app:is_seq_looping() then
		self:restart_time()
		t = 0
	end
	self.start_anim_t = t
--	self:print( "Start Animation : "..self.animation_name.." at "..start_time )
end

function meu:fbx_update_time( meu_fbx, t, next )
	local duration = self.animations[ self.name ][ self.animation_name ].duration
	if t - self.start_anim_t >= duration then
		self.animation_name = next
		self:update_fbx_start_time( meu_fbx, t )
	else
		local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
		meu_fbx:set_time( meu_time )
	end
end

function meu:update_anim()
	local t = self:update_time()
	local meu_fbx = self:get_meu_by_name_no_error( self.fbx_name )
	if t and meu_fbx then
		meu_fbx:set_play(false)
		if self.animation_name == "" then
			-- must start animation
			self.animation_name = "GR10"
			local start_time = self.animations[ self.name ][ self.animation_name ].start_time_s
			self:restart_time()
			self.start_anim_t = 0
			meu_fbx:set_time( start_time )
	--		self:print( "Start Animation : "..self.animation_name.." at "..start_time )
		elseif self.animation_name == "GR10" then
			self:fbx_update_time( meu_fbx, t, "GR60" )
		elseif self.animation_name == "GR60" then
			self:fbx_update_time( meu_fbx, t, "GR80" )
		elseif self.animation_name == "GR80" then
			local duration = self.animations[ self.name ][ self.animation_name ].duration
			if t - self.start_anim_t >= duration then
				self:restart_anim()
			else
				local meu_time = self.animations[ self.name ][ self.animation_name ].start_time_s + t - self.start_anim_t
				meu_fbx:set_time( meu_time )
			end
		--	self:print( "Time fbx : ".. meu_fbx:get_time() )
		end
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
				bdd_fbx:set_translation( 17.235575, 0.0, 14.332170 )
				bdd_fbx:set_rotation( 0.0, 0.25, 0.0 )

				self:update_anim()

				-- tra x = 17.235575
				-- tra z = 14.332170
				-- rot y = 0.25
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
