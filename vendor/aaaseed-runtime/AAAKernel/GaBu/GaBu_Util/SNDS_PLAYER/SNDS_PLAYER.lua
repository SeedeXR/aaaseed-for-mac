if CLASS.DECLARE( "SNDS_PLAYER", nil, { verbose = 1 } ) then
	SNDS_PLAYER:set_class_status_doc(	CLASS.STATUS.CORE,
										{	"regroup in one object playing severals sound using VIDEO objects.",
											"Once created you add sound to it with add(), you need to call update() every frame.",
											"looping is not perfect (2025 August), it is a c_tex_video problem to fix."
										}
	)
end

function SNDS_PLAYER:create( name )--, b_continuous )
	local self = SNDS_PLAYER:create_instance( name )
	--self:init()
	self.__videos = {}
--	self.__b_continuous = b_continuous
	return self
end

function SNDS_PLAYER:add( key, nb, file, ext, b_serie )
	nb = nb or 1
	for i = 1, nb do
		local fpath = file
		if b_serie then fpath = file..string.format( "%02d", i ) end
		if ext then fpath = fpath.."."..ext end
		local video = VIDEOS:get_sound_new( fpath )
		if video then
			if self.verbose>=1 then self:print_inverse(  "sound file added : "..key.."for "..fpath ) end
			local videos = self:get_table_always( "__videos" )
			if not videos[key] then videos[key] = {} end
			videos = videos[key]
			videos[#videos+1] = video
			--video:set_volume( 1 )
			--video:set_start()
		else
			if self.verbose>=1 then self:print_error(  " can't add sound file : "..key.."for "..fpath ) end
			break
		end
	end
end

function SNDS_PLAYER:update()
	--aaa.print_fn()
	local videos = self.__videos
	for k, t in pairs(videos) do
		--self:print( "key "..k )
		--table.print( t, k, 2 )
		for i,v in ipairs(t) do
			v:update()
		end
	end
end

function SNDS_PLAYER:set_volume( key, vol )
	local t = self.__videos[key]
	if t then
		for i, v in ipairs(t) do
			v:set_volume( vol )
		end
	end
end
function SNDS_PLAYER:restart( key )
	--aaa.print_fn()
	local videos = self.__videos
	for k, t in pairs(videos) do
		--self:print( "key "..k )
		--table.print( t, k, 2 )
		for i, v in ipairs(t) do
			v:restart()
		end
	end
end

function SNDS_PLAYER:get_video_table( )
	return self.__videos
end

function SNDS_PLAYER:found_free( key )
	local videos = self.__videos[key]
	local found
	if #videos == 1 then
		found = videos[1]
	else
		--use reservoir sampling
    	local count = 0
    	for _, video in ipairs(videos) do
        	if not video:is_playing() then
            	count = count + 1
            	if math.random(count) == 1 then
                	found = video
            	end
        	end
		end
    end
    return found
end

function SNDS_PLAYER:play( key, b_loop, vol, pan )
	local found = self:found_free( key )
	if found then
		found:set_loop( b_loop or false )
		found:set_restart()
		found:play()
		found:set_sound( vol or 1, pan or 0 )
		--self:print( "volume "..vol )
	end
	return found
end

function SNDS_PLAYER:stop( key )
	local videos = self.__videos[key]
	for i, video in ipairs(videos) do
		if video:is_playing() then
			video:stop()
		end
	end
end

function SNDS_PLAYER:stop_all()
	local videos = self.__videos
	for key, _ in pairs(videos) do
		self:stop( key )
	end
end

--todo PLAYLIST vs SNDS_PLAYER
--[[
function SNDS_PLAYER:add_sound_in_dir( dir )
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
--]]
--[[
function SNDS_PLAYER:get_cur_key()				return self.__cur_key		end
function SNDS_PLAYER:get_cur_key_as_number()
	local key = self:get_cur_key()
	return key or 0
end
function SNDS_PLAYER:__set_cur_key( key )		self.__cur_key = key 		end
function SNDS_PLAYER:__get_key( key )
	return (key or self.__cur_key) or self.__last_key
end

function SNDS_PLAYER:__get_video( key )
	key = self:__get_key( key )
	if key then
		return self.__videos[key]
	end
end

function SNDS_PLAYER:stop_track( key )
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
function SNDS_PLAYER:play_track( key )
	if self.verbose>=2 then self:print( "try play : "..key ) end
	key = self:__get_key( key )
	local video = self:__get_video( key )
	if video then
		if self.verbose>=1 then self:print( "play : "..key.." file "..video:get_name_video() ) end
		video:play()
		self:__set_cur_key( key )
	end
end
function SNDS_PLAYER:begin_track( key )
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
function SNDS_PLAYER:toggle_track()
	if self.verbose>=2 then self:print( "try toggle : "..key ) end
	local key = self:get_cur_key()
	local video = self:__get_video( key )
	if video then
		if self.verbose>=1 then self:print( "toggle : "..key.." file "..video:get_name_video() ) end
		video:toggle()
	end
end

function SNDS_PLAYER:change_track( key )
	key = self:__get_key( key )
	local cur = self:get_cur_key( key )
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
	--video:set_	()
end

--todo will be a problem if cur_key is not a number
function SNDS_PLAYER:prev_track()	self:change_track( self:get_cur_key_as_number() - 1 )	end
function SNDS_PLAYER:next_track()	self:change_track( self:get_cur_key_as_number() + 1 )	end

function SNDS_PLAYER:set_volume( vol )
	local video = self:__get_video()
	if video then
		video:set_volume( vol )
	end
end
--]]

--[[
function SNDS_PLAYER:get_video_name()
	local video = self:__get_video()
	return video and video:get_name_video()
end
function SNDS_PLAYER:get_video_time()
	local video = self:__get_video()
	return video and video:get_time()
end
function SNDS_PLAYER:get_video_time_cano()
	local video = self:__get_video()
	return video and video:get_time_cano()
end
function SNDS_PLAYER:get_video_duration()
	local video = self:__get_video()
	return video and video:get_duration()
end
function SNDS_PLAYER:set_video_focus()
	local video = self:__get_video()
	return video and video:set_focus()
end
--]]

