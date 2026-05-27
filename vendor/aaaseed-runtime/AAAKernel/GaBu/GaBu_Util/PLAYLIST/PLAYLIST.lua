if CLASS.DECLARE( "PLAYLIST", nil, { verbose = 1 } ) then
	PLAYLIST:set_class_status_doc(	CLASS.STATUS.GABU,
									"regroup several sound tracks" )
end

function PLAYLIST:create( name, b_continuous )
	local self = PLAYLIST:create_instance( name )
	--self:init()
	self.__videos = {}
	self.__b_continuous = b_continuous
	return self
end

function PLAYLIST:add_sound_file( file, ext, key )
	if ext then
		file = file.."."..ext
	end
	local video = VIDEOS:get_sound( file )
	if video then
		if self.verbose>=1 then self:print_inverse(  "sound file added : "..key.."for "..file ) end
		if not key then key = #(self.__videos) + 1 end
		self.__videos[key] = video
	else
		if self.verbose>=1 then self:print_error(  " can't add sound file : "..key.."for "..file ) end
	end
	return video
end

function PLAYLIST:add_sound_in_dir( dir )
	aaa.print_method()
	local files = aaa.dir.get_files( dir )
	if not files then
		self:print( "Folder "..dir.." contain no file" )
		return false
	end
	for i = 1, #files do
		self:add_sound_file( dir..files[i] )
	end
end

--todo key or id_track
function PLAYLIST:get_cur_key()				return self.__cur_key		end
function PLAYLIST:get_cur_key_as_number()
	local key = self:get_cur_key()
	return key or 0
end
function PLAYLIST:__set_cur_key( key )		self.__cur_key = key 		end
function PLAYLIST:__get_key( key )
	return (key or self.__cur_key) or self.__last_key
end
function PLAYLIST:__get_video( key )
	key = self:__get_key( key )
	if key then
		return self.__videos[key]
	end
end

function PLAYLIST:stop_track( key )
	if self.verbose>=2 then self:print( "try stop : "..key ) end
	key = self:__get_key( key )
	local video = self:__get_video( key )
	if video then
		if self.verbose>=1 then self:print( "stop : "..key.." file "..video:get_name_video() ) end
		video:stop()
		self.__last_key = self:get_cur_key()
		self:__set_cur_key( nil )
	end
end
function PLAYLIST:play_track( key )
	if self.verbose>=2 then self:print( "try play : "..key ) end
	key = self:__get_key( key )
	local video = self:__get_video( key )
	if video then
		if self.verbose>=1 then self:print( "play : "..key.." file "..video:get_name_video() ) end
		video:play()
		self:__set_cur_key( key )
	end
end
function PLAYLIST:begin_track( key )
	if self.verbose>=2 then self:print( "try begin : "..key ) end
	key = self:__get_key( key )
	local video = self:__get_video( key )
	if video then
		if self.verbose>=1 then self:print( "begin : "..key.." file "..video:get_name_video() ) end
		video:set_restart()
		video:play()
		self:__set_cur_key( key )
	end
end
function PLAYLIST:toggle_track()
	local key = self:get_cur_key()
	if self.verbose>=2 then self:print( "try toggle : "..key ) end
	local video = self:__get_video( key )
	if video then
		if self.verbose>=1 then self:print( "toggle : "..key.." file "..video:get_name_video() ) end
		video:toggle()
	end
end

function PLAYLIST:change_track( key )
	key = self:__get_key( key )
	local cur = self:get_cur_key()
	self:stop_track( cur )

	local nb = #self.__videos
	if key then
		--todo lua integer ?
		if type(key)=="number" then
			key = math.ceil( key )
			if key < 1 then
				key = nb
			elseif nb < key then
				key = 1
			end
		end
	else
		repeat
			key = math.random( nb )
		until key ~= cur
	end
	--self:print( "change_track : "..key )
	--self:print( "change_track : "..key.." from file "..self:get_video_name() )
	self:begin_track( key )
	--video:set_focus()
end

--todo will be a problem if cur_key is not a number
function PLAYLIST:prev_track()	self:change_track( self:get_cur_key_as_number() - 1 )	end
function PLAYLIST:next_track()	self:change_track( self:get_cur_key_as_number() + 1 )	end

function PLAYLIST:set_volume( vol )
	local video = self:__get_video()
	if video then
		video:set_volume( vol )
	end
end
function PLAYLIST:update()
	local video = self:__get_video()
	if not video then
		if self.__b_continuous then
			self:change_track()
		end
		return
	end
	video:update()
--video:set_focus()	end
	if video:is_at_end() then
		if self.__b_continuous then
			self:change_track()
		else
			self:stop_track()
		end
	end
end

function PLAYLIST:get_video_name()
	local video = self:__get_video()
	return video and video:get_name_video()
end
function PLAYLIST:get_video_time()
	local video = self:__get_video()
	return video and video:get_time()
end
function PLAYLIST:get_video_time_cano()
	local video = self:__get_video()
	return video and video:get_time_cano()
end
function PLAYLIST:get_video_duration()
	local video = self:__get_video()
	return video and video:get_duration()
end
function PLAYLIST:set_video_focus()
	local video = self:__get_video()
	return video and video:set_focus()
end
