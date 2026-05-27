
function CLARISONIC:update_welcome( seq )
	--self:print( "welcome" )
	local music = self.music
	if music then
		if not music:is_playing() then
			--self:print( "welcome play" )
			music:play()
		end
		music:set_volume( 1 )
		music:update()
	end
end

function CLARISONIC:update_wait( seq )
	--self:print( "wait" )
--[[
	local music = self.music
	if music then
		if music:is_playing() then
			music:stop()
			music:restart()
		end
	end
--]]
	if app.presence_global and app.presence_global > 0.95 then
		local seqs = ga:get_seqs()
		seqs:inc_seq()
	end
end

function CLARISONIC:get_anim( seq, name )
	local id = seq:get_seq_id()
	--if inside( id, 3, 5 ) then
		local anim = self[name]
		return anim and anim[ id ]
	--end
end
function CLARISONIC:update_anim( seq, name )
	local ease = seq:get_ease_in()
	if seq:get_ease_in() < .5 then
		--elf:print( "toto ease "..ease )
		local anim = self:get_anim( seq, name )
		--self:print( "toto anim "..anim )
		if anim then anim:__set_index_float( -42 ) end	-- could be set_index ?
	end
end

function CLARISONIC:check_restart()
	if app.presence_global and app.presence_global < 0.05 then
		ga:get_seqs():set_seq( 1 )
	end
end

function CLARISONIC:update_welcome(	seq )
	self:check_restart()
end
function CLARISONIC:update_play( seq )
	self:check_restart()
	if app.hotspot and app.hotspot > 0.99 then
		ga:get_seqs():inc_seq()
	end
end
function CLARISONIC:update_record( seq )
	if seq:get_time() > (seq:get_duration()-1) then
		ga:get_seqs():set_seq(3)
	end
end
function CLARISONIC:update_bye( seq )		end
function CLARISONIC:update_end( seq )		end

function CLARISONIC:update_sound( volume, vol_mus, vol_voi  )
	--self:print( volume, vol_mus, vol_voi  )
	local music = self.music
	if music then
		music:set_volume( volume * vol_mus )
		music:update()
	end
	--[[
	local playlist = self:get_playlist()
	playlist:set_volume( volume * vol_voi )
	playlist:update()
	--]]
end

function CLARISONIC:update_presence( val )
	local pre = 0
		--aaa.print_fn()
	--self:print( zone.presence_raw  )
	local inc = val>.0 and 1/self.time_delay_up or -1/self.time_delay_down
	inc = inc * aaa.time.dt
	local pre = app.presence_global or 0
	pre = clamp_01( pre + inc )
	app.presence_global = pre
	return pre
end
function CLARISONIC:reset_presence()
--	self.presence_global = 0
end
function CLARISONIC:get_presence_raw(id)
--	return 1
end
function CLARISONIC:get_presence(id)
--	return self.presence_global
end
function CLARISONIC:draw_tex( bind, x, y, sx, sy )
	gol.bind_texture( bind )
	sx, sy = sx*.5, sy*.5
	aaa.draw_rect_uv( x-sx, y-sy, x+sx, y+sy )
end

function CLARISONIC:draw_after()

	--self.playlist:change_track( 3 )
	--self.playlist:update()

	oo.getsuper(CLARISONIC).draw_after(self)
end

