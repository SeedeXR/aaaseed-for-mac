CLASS.DECLARE( "TD_PLAYER" )

function TD_PLAYER:__init_new( id )
	self.__id = id
	self:init()
	self.alpha = .0
end

function TD_PLAYER:create( id )
	local self = TD_PLAYER:create_instance( "player_"..id )
	self:__init_new( id )
	return self
end

function TD_PLAYER:get_id()		return self.__id		end

function TD_PLAYER:restart_level()
	self.conformity 		= 0
	self.conformity_draw	= 0
	self.b_made				= false
	self.state				= "playing"
	--self.alpha				= 1
	self.ph_result			= 0
end
function TD_PLAYER:init()
	local id = self:get_id()
	self.score 				= 0
	self.level 				= { false, false, false, false, false, false, false }
	self.made 				= { false, false, false, false, false, false, false }
	self.presence_raw		= 0
	self.presence			= 0
	self.point_conformity	= {}
	self:restart_level()
--tracking = 0,
--inter = 0,
end

function TD_PLAYER:set_made()
	--self:print( "made" )
	self.b_made				= true
	self.made[app:get_level()]	= true
end
function TD_PLAYER:set_won()
	--self:print( "won" )
	self.state				= "won"
	self.level[app:get_level()]	= true
end
function TD_PLAYER:set_lost()
	self.state				= "lost"
	self.level[app:get_level()]	= false
end

TD_PLAYER.conformity = .95
function TD_PLAYER:update_game( b_can_win )
	if self.state == "playing" then
		--self:print( "player "..self:get_id().." "..self.conformity_draw )
		if self.conformity_draw >= TD_PLAYER.conformity then
			self:set_made()
			if b_can_win then
				self:set_won()
				return true
			end
		end
	else

	end
end

function MDH_TOUSDIF:get_player( id )
	local p = self.__players[id]
	--self:print( p.." my id is "..id )
	return p
end
function MDH_TOUSDIF:set_player( id )		self.__player = self:get_player( id )	return self.__player	end

function MDH_TOUSDIF:get_player_cur()		return self.__player										end
function MDH_TOUSDIF:get_player_id()		return self.__player:get_id()								end

function MDH_TOUSDIF:start_game_level()
	self.b_started			= false
	self.b_playing			= false
	self.b_result 			= false
	self.b_done 			= false
	self.result_ph			= 0
	self.result_time		= 0
	self.result_time_start	= 0
	for i=1,2 do
		self:get_player(i):restart_level()
	end
end

function MDH_TOUSDIF:update_game( seq )

	local dur	= seq:get_duration()
	local t		= seq:get_time()
	local ease	= seq:get_ease_in()

	local ph = (t - ease) / self.duration_game
	seq.game_ph = ph

	local function start_result()
		self.b_playing = false
		local b_win = false
		for i=1,2 do
			if self:get_player(i).state == "won" then
				b_win = true
				self:get_player(3-i):set_lost()
				self.snds.dommage:play()
			end
		end
		if not b_win then
			self:get_player(1):set_lost()
			self:get_player(2):set_lost()
			self.snds.dommage:play()
		end
		self.b_result = true
		self.result_time_start = seq:get_time()
	end
	--self:print( "update_game" )
	--self:print( t.." "..ease )
	if ease < 1 then
		--self:print( "ease" )
		self:start_game_level()
	else
		if self.b_result then
			self.result_time = seq:get_time() - self.result_time_start
			self.result_ph = self.result_time / self.duration_result
			if not self.b_done then
				self:restart_video()
				local tl = self.result_time - self.duration_result - self.duration_photo
				if tl >= 0 then
					ga:get_seqs():inc_seq()
					self.b_done = true
				end
			end
		elseif self.b_started then
			--self:print( "started" )
			if self.b_playing then
				--self:print( "playing" )
				local made = 0
				for id=1,2 do
					if self:get_player(id).b_made then
						made = made + 1
					end
				end

				if made == 2 then
					start_result()
				else
					for id=1,2 do
						if self:get_player(id):update_game( made == 0 ) then
							self.snds.bravo:play()
						end
					end

					if ph > 1 then
						start_result()
					end
				end
			end
		else
			--todo understand if commenting was an error or not
			--self:print_inverse( "start" )	--was printing all the time ?
			self.b_started = true
			self.b_playing = true
		end
	end

	if t >= dur-.2 then
		self.b_done = true
	end
	if self.b_done then
		self:start_game_level()
	end

end