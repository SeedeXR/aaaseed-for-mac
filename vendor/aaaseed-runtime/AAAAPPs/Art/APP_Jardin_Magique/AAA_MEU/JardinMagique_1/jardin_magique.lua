
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local ix = 1
	local iy = 2
	local sy =1

	bu = self:add_button(	{ix, iy,	1,sy },	"capteur", 			self, "b_sensor",		false	)
	bu = self:add_button(	{ix+4, iy,	1,sy },	"Simul", 			self, "b_simul",		false 	)
	iy = iy + sy
	bu = self:add_button(	{ix,iy},			"Verbose", 			self, "b_verbose",		false	)
	iy = iy + sy
	bu = self:add_button(	{ix,iy},			"Min Use", 			self, "b_min_use",		false	)
		bu:set_text_visible( false )
	bu = self:add_slider(	{ix+1,iy,	7,sy},	"Min",				self, "min_threshold", 	100,	12,3000	)
	iy = iy + sy
	bu = self:add_button(	{ix,iy,		1,sy},	"Max Use", 			self, "b_max_use",		false	)
		bu:set_text_visible( false )
	bu = self:add_slider(	{ix+1,iy,	7,sy},	"Max",				self, "max_threshold", 	200,	12,1000	)
	iy = iy + sy
	self.ui.bu_info_capteur = self:add_text_info(	{ix,iy,	8,1}, "info_capteur" )
	iy = iy + sy*2

	ix = 9
	iy = 2
	bu = self:add_slider(	{ix,iy,		8,sy},	"presence",			self, "presence",		0,		0,1	)
	bu = self:add_slider(	{ix,iy+1,	8,sy},	"intro",			self, "intro",			0,		0,1	)
	iy = iy + sy *2 + .2
	bu = self:add_slider(	{ix,iy,		8,sy},	"destroy_up",		self, "destroy_up", 	5,		1,30	)
	bu = self:add_slider(	{ix,iy+1,	8,sy},	"destroy_down",		self, "destroy_down", 	50,		5,60	)
	bu = self:add_slider(	{ix,iy+2,	8,sy},	"destroy",			self, "destroy", 		0,		0,1	)
	iy = iy + sy * 3 + .2
	bu = self:add_slider(	{ix,iy,		8,sy},	"destroy_time",		self, "destroy_t", 		0,		0,50	)
	iy = iy + sy
	self.ui.bu_info_destroy = self:add_text_info(	{ix,iy,	8,sy}, "info" )

end

function meu:init()
	local ref = self.ref
	local bdd = self:get_layer_bdd( 1 )
	ref.text_out = param.get_ref( bdd, "received" )
end

function meu:update()
	self:set_ui_slot_at_start( 5 )
	if not self.meu_wavy			then	self.meu_wavy			= self:get_meu_by_name( "WAVY_1"	)	end

	if not self.meu_vid_corail		then	self.meu_vid_corail = {}										end
	if not self.meu_vid_corail[1]	then	self.meu_vid_corail[1]	= self:get_meu_by_name( "Video_A"	)	end
	if not self.meu_vid_corail[2]	then	self.meu_vid_corail[2]	= self:get_meu_by_name( "Video_B"	)
											self:set_video( 1 )
											self:flip_video() 												end
	if not self.meu_vid_rd			then	self.meu_vid_rd			= self:get_meu_by_name( "Video_C"	)	end
	if not self.meu_tri_mix			then	self.meu_tri_mix		= self:get_meu_by_name( "TriMix_1"	)	end

	if not self.meu_snd_wave		then	self.meu_snd_wave		= self:get_meu_by_name( "Video_D"	)
											self.meu_snd_wave:set_bu_value( "volume", 0 )					end
	if not self.meu_snd_lamer		then	self.meu_snd_lamer		= self:get_meu_by_name( "Video_E"	)
											self.meu_snd_lamer:set_bu_value( "volume", 0 )					end
	if not self.meu_snd_under		then	self.meu_snd_under		= self:get_meu_by_name( "Video_F"	)
											self.meu_snd_under:set_bu_value( "volume", 0 )					end
	if not self.meu_snd_destroy		then	self.meu_snd_destroy	= self:get_meu_by_name( "Video_I"	)
											self.meu_snd_destroy:set_bu_value( "volume", 0 )				end
	if not self.meu_snd_noise		then	self.meu_snd_noise		= self:get_meu_by_name( "Video_J"	)
											self.meu_snd_noise:set_bu_value( "volume", 0 )					end

end

function meu:pick_video_index()
	local t = self.last_video_index or {}

	local id
	repeat
		id = math.random(16)-1
	until ( id~=t[1] and id~=t[2] and id~=t[3] and id~=t[4] )

	t[1] = t[2]
	t[2] = t[3]
	t[3] = t[4]
	t[4] = id
	--table.print( t, "last_video_index" )
	self.last_video_index = t
	return id
end

function meu:test_contact( v )
	local t = self.last_contacts or {}
	local nb = 20
	for i =1,nb-1 do
		t[i] = t[i+1]
	end
	t[nb] = v
	self.last_contacts = t

	local b = false
	local min = 1000000
	local max  = 0
	for i=1,nb do
		if t[i] then
			if t[i] > max then	max = t[i]	end
			if t[i] < min then	min = t[i]	end
		end
	end
	if self.b_verbose then
		self:print( min.." / "..max.." / "..(max-min) )
	end

	if self.b_max_use and max > self.max_threshold	 then
		b = true
	end
	if self.b_min_use and min > self.min_threshold	 then
		b = true
	end

	return b
end

function meu:set_video( id )
	self.video_cur = id
	self.meu_vid_front = self.meu_vid_corail[self.video_cur]
	self.meu_vid_back = self.meu_vid_corail[3-self.video_cur]
	self.meu_wavy:set_bu_value( "TEX", self.video_cur-1 )
end

function meu:flip_video()
	self:set_video( 3-self.video_cur )
	self.meu_vid_front:set_bu_value( "bind", self:pick_video_index() )
	self.meu_vid_front:restart()
	self.meu_vid_front:start()
	self.b_need_stop = true
end

function meu:draw()
	local ref = self.ref

	self:draw_layers_begin()

		local b_presence
		if self.b_sensor then
			self:draw_layer( 1 )
			local text = param.get( self.ref.text_out )
			local str
			b_presence = false
			for line in text:gmatch("[^\r\n]+") do
				local n = tonumber(line)
				str = str and str.." "..n or n
				if self.b_verbose then
					self:print( n )
				end
				b_presence = b_presence or self:test_contact( n )
			end
			if b_presence then
				str = str.."\nTouched"
			end
			self.ui.bu_info_capteur:set_text( str )
		else
			b_presence = self.b_simul
		end

		local d = b_presence and 10 or -5
		self.presence = clamp_01( self.presence + aaa.time.dt * d )
		self:set_bu_value( "presence", self.presence )

		local d_destroy
		if self.presence == 1. then
			d_destroy = 1. / self.destroy_up
			if self.presence_last ~= 1. then
				self.appear = 1
			end
		else
			d_destroy = -1. / self.destroy_down
			-- begin materialization
		end

		if self.appear then
			self.intro = self.intro + self.appear * aaa.time.dt
		end
		self.intro = clamp_01( self.intro )
		self:set_bu_value( "intro", self.intro )

		if self.intro == 1. then
			if self.intro_last ~= 1. then
				self.meu_vid_front:start()
				else
					local meu_video = self.meu_vid_front
				if meu_video:get_time()==meu_video:get_duration() then
					self.appear = -1
				end
			end
		end

		if self.intro == 0. then
			if self.intro_last ~= 0. then
				self:flip_video()
			end
		end

		if self.b_need_stop then
			self.b_need_stop = nil
			self.meu_vid_front:stop()
		end

		local text
		local min, max, alpha, vol_destroy, vol_noise
		local vol_under = 1. - self.intro *.3
		local vol_lamer =.7 + self.intro *.3
		vol_destroy = 0
		vol_noise = 0
		if self.b_destroy then
			local dur = 50
			local t = self.destroy_t
			if t > dur then
				self.b_destroy = false
				self.destroy = 0
				self.meu_vid_rd:restart()
				self.meu_vid_rd:stop()
				min = .99
				max = 1.
				alpha = 1

			else
				local fn_linear = math.fn_linear
				min			= fn_linear( t, 1,	0,.99,	.5,.3,		20,.3,		25,0,		30,0,	31,.99	)
				max			= fn_linear( t, 1,	0,1,	.5,.5,		20.,.5,		25.,.06,	30,.06, 31,1	)
				alpha		= fn_linear( t, 1,	0,1,	25,1,		28,0,		39,0,		50,1	)
				vol_destroy	= fn_linear( t, 1,	0,0,	20,1,		25,1,		30,0,		100,0	)
				vol_noise	= fn_linear( t, 1,	0,0,	10,0,		25,1,		30,0,		100,0	)
				local fac	= fn_linear( t, 1,  0,1,	3,.75,		25,.75	)
				vol_under = vol_under * fac
				vol_lamer = vol_lamer * fac
			end
			text = string.format( "%5f / %d", t, dur )
			self.meu_tri_mix:set_bu_value( "min", min )
			self.meu_tri_mix:set_bu_value( "max", max )
			self.meu_tri_mix:get_mu():set_value( alpha )
			self.destroy_t = t + aaa.time.dt

		else
			self.destroy = self.destroy + d_destroy * aaa.time.dt
			self.destroy = clamp_01( self.destroy )
			self:set_bu_value( "destroy", self.destroy )
			if self.destroy == 1. and self.destroy_last ~= 1. then
				self.meu_vid_rd:restart()
				self.meu_vid_rd:start()
				self.b_destroy = true
				self.destroy_t = 0
				self.meu_snd_destroy:start()
			end
			text = ""
		end
		self.ui.bu_info_destroy:set_text( text )
		self:set_bu_value( "destroy_time", self.destroy_t )

		self.presence_last	= self.presence
		self.intro_last		= self.intro
		self.destroy_last	= self.destroy
--		if self.b_draw_part then	self:draw_layer( 2 )
--									self:draw_layer( 3 )	end
	self:draw_layers_end()

	local wave_volume = self.wave_volume or 0
	if b_presence then
		wave_volume = 1.
	else
		wave_volume = wave_volume - aaa.time.dt * .5
	end
	wave_volume = clamp( wave_volume, 0, 1 )
	self.wave_volume = wave_volume


	self.meu_snd_under:set_bu_value( "volume", vol_under-.05 )
	self.meu_snd_lamer:set_bu_value( "volume", vol_lamer )
	self.meu_snd_destroy:set_bu_value( "volume", .4 + .5*vol_destroy )
	self.meu_snd_noise:set_bu_value( "volume", .4 + .6*vol_noise )

	self.meu_snd_wave:set_bu_value( "volume", .75 + wave_volume * .25 )

	if self.meu_wavy then
		self.meu_wavy:set_bu_value( "wave", 1-self.intro )
	end

--	self.ui.bu_go:print( "TEST" )
	--self:print( "pick "..self:pick_video() )
end



