--
--	VIDEO
--
function BU:detach_video()
	local video = self:get_video()
	if video then	--	release previous video
		video:stop()
		--self.sx = self.sy
		self:set_video()
		self:set_texture_bind_2d()
	end
end

function BU:assign_video( video, sx,sy )
	self:detach_video()
	if video then
		self:print( " assign_video to "..video:get_name_video() )
		-- if sx and sy or not defined scale according to ratio if one us defined or use existing size
		if sx then
			if not sy then
				sy = sx * video:get_ratio_y()
			end
		elseif sy then
			if not sx then
				sx = sy * video:get_ratio_x()
			end
		else
			sx,sy = self:get_sxy()
		end
		self:set_texture_bind_2d( video:get_bind() )
		self:set_video( video )
		local x,y = self:get_xy()
		self:set_pos_orig( {x,y, sx,sy, self:get_angle()} )
	end
	self:set_kind_video()
	return self
end
function BU:create_video( name, video, rect )
	if rect[4]==nil	then
		rect[4] = 2.0
	end
	if video then
		rect[3] = rect[3] or (rect[4] * video:get_ratio_x())
	end
	local self = BU:create( name, rect )
	if video then
		self:set_texture_bind_2d( video:get_bind() )
		self:set_video( video )
	end
	self:set_kind_video()
	return self
end
function BU:set_video_loop( b )
	self.__b_video_loop = b
end
function BU:set_video_play_on_page( b )
	self.__b_video_play_on_page = b
end

function BU:get_video()					return	self.video						end
function BU:set_video( video )			self.video = video						end

function BU:is_video_button()			return self.b_video_button and not ga.b_one_active_video	end
function BU:set_video_button( b )		self.b_video_button = b					end

function BU:is_video_scrub()			return self.__b_video_scrub				end
function BU:set_video_scrub( b )		self.__b_video_scrub = b					end
function BU:set_video_scrub_sy( val )	self.__video_scrub_sy = clamp_01(val)		end

--
--	VIDEO
--
--todo propagate
function BU:play_video()
	if not self:is_video() then return end
	local video = self:get_video()
	video:play()
	local bus = self:get_bus_up()
	if bus.b_video_only_one_playing then
		bus:stop_video_all( self )
	end
end
function BU:stop_video()
	if not self:is_video() then return end
	local video = self:get_video()
	video:stop()
end
function BU:play_video_at_start()
	if not self:is_video() then return end
	local video = self:get_video()
	video:set_restart()
	self:play_video()()
end
function BU:place_video_at_start()
	if not self:is_video() then return end
	local video = self:get_video()
	video:place_at_start()
end
function BU:toggle_video()
	if not self:is_video() then return end
	local video = self:get_video()
	if video:is_playing() then
		if self.verbose >= 1 then self:print( " toggle() -> set_stop()" ) end
		self:stop_video()
	else
		if self.verbose >= 1 then self:print( " toggle() -> set_start()" ) end
		self:play_video()
	end
end
function BU:reset_video()
	if not self:is_video() then return end
	local video = self:get_video()
	local b = video:is_playing()
	video:place_at_start()
	if b then
		self:play_video()
	end
end
function BU:is_video_playing()
	if not self:is_video() then return end
	local video = self:get_video()
	return video:is_playing()
end
function BU:is_video_at_end()
	if not self:is_video() then return end
	local video = self:get_video()
	return video:is_at_end()
end

function BU:do_but_video( i )
	if not i then return end
			--aaa.mess.show( self.." button touched "..i
	if self.verbose >= 1 then self:print( "do_video_but( "..i.." )" ) end
	if self:is_video() then
		local video = self:get_video()
		--	if self.verbose >= 1 then self:print( "video : "..video ) end
		if video then
			local str
			if i == 1 then
				str = "reset"
				if self.verbose >= 1 then self:print( "button trigger -> "..str.." on "..video ) end
				self:reset_video()
			elseif i == 2 then
				if self.verbose >= 1 then self:print( "button trigger -> toogle on "..video ) end
				self:toggle_video()
			end
		end
	end
end

--
--	UI
--
function BU:update_video()
	local video = self:get_video()
	if not video then
		self:print_error( "update_video() have no video" )
		return
	end

	video:set_loop( self.__b_video_loop )

	local bus = self:get_bus_up()
	if self.__b_video_play_on_page or self.__b_video_play_on_page_strict then
		--self:print( self.__page.." bus "..self.bus:get_page() )
		if video.play_on_page[ bus:get_page() ] then
			video:set_start()
		else
			video:set_stop()
		end
	elseif self.__page ~= bus:get_page() then
		--aaa.print( "toto" )
		video:set_restart()
		video:set_stop()
	--elseif self.domino and ga.b_one_active_video and arte and self.id == arte.top_domino_id then domino only the top one play
	elseif self.domino and ga.b_one_active_video then
		--self:print( "bu.__page "..self.__page.." bus.__page "..self.bus:get_page() )
		--local x  = pos.x
		if self == bus.domino[self.__page].top then	-- domino only the top one play
			local x_page = self:get_x() + bus.__x_offset_exp
			if x_page > 2.5 then
				video:set_restart()
			end
			--	video start/stop
			if x_page < 1. then
				video:set_start()
			elseif x_page > 2. then
				video:set_stop()
			end
		else
			video:set_stop()
		end
	--elseif ga.b_one_active_video and not self:is_video_button() then
	--	video:set_stop()
	end
	--else
	--	video:set_stop()
	--end
	--	self:print( "ototo" )
	video:update()
	--end

	self:set_texture_bind_2d( video:get_bind() )
	--was param.set( video.ref.bind_dst, self:get_texture_bind_2d() )

	--self:print( "video_tex_bind = "..self:get_texture_bind_2d().." src "..self.video_bind )
	--param.set( video.ref.src, self.video_bind )

end
