function MDH_TOUSDIF:update_presence_one( id )
	local player = self:get_player(id)
	local inc = (player.presence_raw > 0) and 1/self.time_delay_up or -1/self.time_delay_down
	inc = inc * aaa.time.dt
	local pre = player.presence
	pre = clamp_01( pre + inc )
	--self:print( id.." "..player.presence_raw.." "..pre )
	player.presence = pre
	return pre
end
function MDH_TOUSDIF:update_presence()
	local pre = 0
	for id=1,2 do pre = math.max( pre, self:update_presence_one(id) ) end
	self.presence_global = pre
	return pre
end
function MDH_TOUSDIF:reset_presence()
	for id=1,2 do self:get_player(id).presence = 0 end
end

function MDH_TOUSDIF:draw_after()
	self.__mu_out:set_value( ga:is_ui_group_active() and 0 or 1 )
	oo.getsuper(MDH_TOUSDIF).draw_after(self)
end
function MDH_TOUSDIF:get_img_id( name, id )
	local imgs = self.imgs
	local str = name..(id==1 and "l" or "r")
	--self:print( str )
	--todo refine for lang : use fn
	return imgs[ str ]
end
function MDH_TOUSDIF:get_img_level_id( name, id )
	local imgs = self.imgs
	local str = name.."_"..self:get_level()
	return self:get_img_id( str, id )
end
function MDH_TOUSDIF:get_img_game( id )
	local imgs = self.imgs
	local post = "_"..self:get_level()..(id==1 and "l" or "r")
	--todo refine for lang : use fn
	local function get( name ) return self:get_img_level_id( name, id ) end
	return get( "game" ), get( "contour" ), get( "photo" )		--, get( "photo_ref" )
end

function MDH_TOUSDIF:update_wait( seq )
	self:set_level( 1 )
end
function MDH_TOUSDIF:update_welcome( seq )
	self:set_level( 1 )
end
function MDH_TOUSDIF:update_end( seq )
	self:reset_presence()
end

function MDH_TOUSDIF:restart_video()
	local meus = self.meu_videos
	for i=1,2 do
		local m = meus[i]
		--m:set_time(0.)
		m:restart()
	end

end
function MDH_TOUSDIF:choose_and_stop_video()
	local meus = self.meu_videos
	for i=1,2 do
		local m = meus[i]
		--todo make it cleaner
		m.ui.bu_video_bind:set_value( (self:get_level()-1)*2+i-1 )
		m:restart()
		m:start()
		m:update()
		m:draw()
		m:stop()
	end
end
function MDH_TOUSDIF:update_video( seq )
	local meus = self.meu_videos
	local b_stop = false
	for i=1,2 do
		local m = meus[i]
		m:start()
		--todo encapsulate
		local d = m:get_duration()
		local t = m:get_time()
		--self:print( t )
		local dt = .01
		if t > (d - dt) then
			self:print( "do the stop" )
			b_stop = true
		end
	end
	if b_stop then
		local seqs = ga:get_seqs()
		if self:get_level() == 7 then
			--if seqs:is_run() then
				seqs:inc_seq()
			--end
		else
			self:inc_level_raw( 1 )
			--if seqs:is_run() then
				seqs:dec_seq()
			--end
		end
	end
end

function MDH_TOUSDIF:get_level()
	if self.seqs:is_test() then
		math.randomseed(aaa.time.second)
		return math.random(7)
	end
	return self.__level
end
function MDH_TOUSDIF:set_level( l )
	--todo real wrap
	l = clamp( l, 1, 7 ) --wrap have side fx at end
	if self.__level ~= l then
		self.__level = l
		self:print( "game change to level "..l )
		if l==1 then
			self:get_player(1):init()
			self:get_player(2):init()
		end
	end
end
function MDH_TOUSDIF:inc_level_raw( inc )
	self:set_level( self:get_level() + inc )
end

function MDH_TOUSDIF:update()
	oo.getsuper(MDH_TOUSDIF).update(self)

	for _, s in pairs(self.snds) do
		s:update()
	end

--[[
	local ran = math.random( 20 )
	if ran<=2 then
		--self:print( ran )
		local m = self:get_meu_by_name( ran==1 and "Video_C" or "Video_D" )
		m.ui.bu_video_bind:set_value( math.random( 14 ) )
	end
]]--
end
