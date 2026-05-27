function MDH_NEAND:scale_lrbt( l, r, b, t, f )
	local x,y		= (l+r)*.5, (b+t)*.5
	local sxh,syh	= (r-l)*.5*f, (t-b)*.5*f
	return x-sxh, x+sxh, y-syh, y+syh
end
function MDH_NEAND:draw_corner( l, r, b, t, f )
	f = f or 1.
	local s = math.min( .1, f )
	if f ~= 1 then l, r, b, t = self:scale_lrbt( l, r, b, t, f ) end
	gol.draw_lines_2d(
						r,  t,		r,		t-s,
						r,	t,		r-s,	t,

						l,  t,		l,		t-s,
						l,	t,		l+s,	t,

						r,  b,		r,		b+s,
						r,	b,		r-s,	b,

						l,  b,		l,		b+s,
						l,	b,		l+s,	b
					)
end

function MDH_NEAND:draw_corner_center( l, r, b, t, f )
	f = f or 1.
	local s = math.min( .1, f )
	self:draw_corner( l, r, b, t, f )
	if f ~= 1 then l, r, b, t = self:scale_lrbt( l, r, b, t, f ) end
	local cx = (l+r-s)*.5
	local cy = (b+t-s)*.5
	gol.draw_lines_2d(
						l,  cy,		l,	cy+s,
						r,	cy,		r,	cy+s,
						cx,  b,		cx+s,	b,
						cx,  t,		cx+s,	t
					)
end

function MDH_NEAND:update_welcome( seq )
	--self:print( "welcome" )
	local music = self.music
	if not music:is_playing() then
		--self:print( "welcome play" )
		music:play()
	end
	music:set_volume( 1 )
	music:update()
end

function MDH_NEAND:restart_music()
	local music = self.music
	if music:is_playing() then
		music:stop()
		music:restart()
	end
end
function MDH_NEAND:update_wait( seq )
	self:change_lang_by_id( 1 )
	--self:print( "wait" )
	local music = self.music
	self:restart_music()
end
function MDH_NEAND:update_intro( seq )
	self:restart_video_gene()
end
function MDH_NEAND:restart_video_gene()
	self.meu_video_gene:set_time( 0. )
end
function MDH_NEAND:update_end( seq )
	self:reset_presence()
end

function MDH_NEAND:update_sound( volume, vol_mus, vol_voi  )
	--self:print( volume, vol_mus, vol_voi  )
	self.music:set_volume( volume * vol_mus )
	self.music:update()
	local playlist = self:get_playlist()
	playlist:set_volume( volume * vol_voi * self.lang_volume )
	playlist:update()
end

function MDH_NEAND:update_presence_one( id )
	--aaa.print_fn()
	local zone = self:get_zone(id)
	--self:print( zone.presence_raw  )
	local inc = (zone.presence_raw > 0) and 1/self.time_delay_up or -1/self.time_delay_down
	inc = inc * aaa.time.dt
	local pre = zone.presence
	pre = clamp_01( pre + inc )
	zone.presence = pre
	return pre
end
function MDH_NEAND:update_presence()
	local pre = 0
	for id=1,2 do
		pre = math.max( pre, self:update_presence_one(id) )
	end
	self.presence_global = pre
	return pre
end
function MDH_NEAND:reset_presence()
	for id=1,2 do self:get_zone(id).presence = 0 end
end
function MDH_NEAND:force_presence()
	for id=1,2 do self:get_zone(id).presence = 1 end
end
function MDH_NEAND:get_presence(id)
	return self:get_zone(id).presence
end
function MDH_NEAND:get_presence_raw(id)
	return self:get_zone(id).presence_raw
end

function MDH_NEAND:change_lang_by_id( id )
	local la = self.__lang_name[id]
	self.lang_volume = self.__lang_volume[id]
	--self:print( "change_lang_by_id to "..id.." -> "..la )
	self:set_timings( la )
	self.b_lang_sign = (la == "sign")

	if self.b_lang_sign then
		la = "french"
	else
		self.b_lang_sign_draw = false
	end
	if self.mu_lang_sign then
		self.mu_lang_sign:set_value( self.b_lang_sign and 1 or 0 )
	end
	self:set_lang( la )
end

function MDH_NEAND:draw_tex( bind, x, y, sx, sy )
	gol.bind_texture( bind )
	sx, sy = sx*.5, sy*.5
	aaa.draw_rect_uv( x-sx, y-sy, x+sx, y+sy )
end

function MDH_NEAND:draw_after()
	self.__mu_out:set_value( ga:is_ui_group_active() and 0 or 1 )
	--self.playlist:change_track( 3 )
	--self.playlist:update()

	oo.getsuper(MDH_NEAND).draw_after(self)
end

function MDH_NEAND:do_key( key )
	local b_key_used = true

	if key == 113 then key = 97
	elseif key == 81 then key = 97
	elseif key == 66 then key = 98
	elseif key == 67 then key = 99
	elseif key == 68 then key = 100
	end
	if inside( key, 97, 100 ) then
		self.seqs:do_action( "keyboard", "begin" )
		local lang_id = key-96
		self:change_lang_by_id( lang_id )
		self:force_presence()
		self.seqs:do_action( "keyboard", "wait" )
		self.seqs:do_action( "keyboard", "welcome" )
		self:restart_music()
		app.seq_id_last = -42
	else
		b_key_used = false
	end

	return b_key_used or oo.getsuper(MDH_NEAND).do_key( self, key )
end

