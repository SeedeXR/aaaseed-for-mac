
function DIOR:update_welcome( seq )
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

function DIOR:update_wait( seq )
	--self:print( "wait" )
	local music = self.music
	if music then
		if music:is_playing() then
			music:stop()
			music:restart()
		end
	end
end

function DIOR:get_anim_id_name( id, name )
	--if inside( id, 3, 5 ) then
	id = string.lower(id)
	name = string.lower(name)
	local anim = self[name]
	anim = anim and anim[ id ]
	--if anim then
	--	aaa.box_warning( "get_anim_id_name( "..id.." "..name.." -> "..anim )
	--end
	return anim
	--end
end
function DIOR:get_anim( seq, name )
	return self:get_anim_id_name( seq:get_title(), name )
--	return self:get_anim_id_name( seq:get_seq_id(), name )
end
function DIOR:update_anim( seq, name )
	local ease = seq:get_ease_in()
	if ease < .5 then
		--self:print( "toto ease "..ease )
		local anim = self:get_anim( seq, name )
		--self:print( "toto anim "..anim )
		if anim then anim:__set_index_float( -42 ) end
	end
end

function DIOR:update_anims( seq )
	self:update_anim( seq, "face_on"	)
	self:update_anim( seq, "palette"	)
	self:update_anim( seq, "typo"		)
	self:update_anim( seq, "typo_b"		)
end

function DIOR:restart_video_gene()	self.meu_video_gene:set_time( 0. )	end

function DIOR:update_color(		seq )	self:update_anims( seq )	end
function DIOR:update_light(		seq )	self:update_anims( seq )	end
function DIOR:update_texture(	seq )
								--	self:restart_video_gene()
										self:update_anims( seq )	end
function DIOR:update_end(		seq )	self:reset_presence()		end

function DIOR:update_sound( volume, vol_mus, vol_voi  )
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

function DIOR:update_presence( val )
	local pre = 0
		--aaa.print_fn()
	--self:print( zone.presence_raw  )
	local inc = val>.5 and 1/self.time_delay_up or -1/self.time_delay_down
	inc = inc * aaa.time.dt
	local pre = app.presence_global or 0
	pre = clamp_01( pre + inc )
	app.presence_global = pre
	return pre
end
function DIOR:reset_presence()
	self.presence_global = 0
end
function DIOR:get_presence_raw(id)
	return 1
end
function DIOR:get_presence(id)
	return self.presence_global
end

-- line in the middle of the face
function DIOR:draw_face_middle_line( pts, width )
	gol.set_texture_dim( 2 )

	local bot = V2.new()
	local top = V2.new()
	V2.interpolate( bot, pts[9], pts[58], .2 )
	V2.interpolate( top, pts[70], pts[28], .1 )
	local tab = { bot, pts[58], pts[52], pts[34], pts[31], pts[28], top }
	for i = 1,#tab-1 do
		aaa.draw_line_tex(	tab[i], tab[i+1], width )
	end
	gol.set_texture_dim( 0 )
	--gol.set_line_width( 3 )

	if false then
		gol.set_line_smooth( true )
		local S = .02
		for i = 1,#tab do
			local pt = tab[i]
			aaa.draw_crosshair(	pt[1],pt[2], S,S*.3 )
		end
	end
end

--[[
function DIOR:draw_after()
	--self.playlist:change_track( 3 )
	--self.playlist:update()

	oo.getsuper(DIOR).draw_after(self)
end
--]]

if IS_BUSS_OPEN() then
	--aaa.box_warning( "before" )
	APP.CREATE_INST( DIOR )
	--aaa.box_warning( "after" )
end