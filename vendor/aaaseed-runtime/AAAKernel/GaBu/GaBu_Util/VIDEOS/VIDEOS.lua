--todo this a singleton should be a regular Instance
if CLASS.DECLARE( "VIDEOS", nil, {
	BIND_VIDEO_START = 512,	-- bank 16
	BIND_VIDEO_NB = 128,	--	to 11
	} ) then
	VIDEOS:set_class_status_doc(	CLASS.STATUS.CORE,
									"encapsulate a group of VIDEO" )
end

function VIDEOS:set_dir( dir_name )
	if self.verbose>0 then aaa.print_method() end
	if dir_name~="" and string.sub( dir_name, -1 )~="/" then
		--GABU_OBJ:box_debug( "need to add / to dir name "..dir_name )
		dir_name = dir_name .. "/"
	end
	--self:box_debug( "dir_name now "..dir_name )
	self.__dir_name = dir_name
end
function VIDEOS:get_dir()
	return self.__dir_name
end
function VIDEOS:set_dir_media( dir_name )
	self:set_dir( (app and app.media_dir_rel or APP.media_dir_rel)..dir_name )
end
function VIDEOS:make_fname( fname )
	local dir = self.__dir_name
	--self:print( "VIDEOS.make_fname() self.__dir_name is "..dir )
	if dir then
		fname = string.enforce_trailing_slash( dir )..fname
	end
	return fname
end

function VIDEOS:get_bind_from_id( id )
	if id < 1 or self.BIND_VIDEO_NB < id then
		if id < 0 then
			error( "no video should have a negative id, here : "..id )
		elseif id == 0 then
			error( "no video should have a zero id" )
		else
			error( "videos are limited to "..self.BIND_VIDEO_NB.." and id is "..id )
		end
		id = self.BIND_VIDEO_NB
	end
	local bind = id - 1 + self.BIND_VIDEO_START
	if self.verbose>0 then self:print( "get_bind_from_id( "..id.." ) return "..bind ) end
	return bind
end

function VIDEOS:get_bind_max()
	return self.BIND_VIDEO_START + self.BIND_VIDEO_NB
end

function VIDEOS:reinit()
	self.__by_name = {}
	self.__slot_used = 0
	self.b_no_dialog_max_yet = true
	self:set_dir( "" )
end

function VIDEOS:get_nb_used()
	return self.__slot_used
end

function VIDEOS:switch_version( la )
--	aaa.print( "VIDEOS:switch_version( \""..la.."\" )" )
--	self:print( "switch_version( \""..la.."\" )" )
	for _,vid in PAIRS( self.__by_name ) do
--		self:print( "\tdo it with "..vid )
		vid:switch_version( la )
	end
end

function VIDEOS:get_free_index( filename )
	local index
	if not aaa.file.is_exist( filename ) then
		self:print_error( "get_free_index(): file "..filename.." don't exist." )
	else
		index = self.__slot_used + 1
		if index > self.nb then
			local str = "While trying to create video for :"
								.."\n\t"..filename
								.."\n\tAAASeed could not create video object"
								.."\n\tbecause this script limit the number of video to "..self.nb
								.."\n\tplease retry with less videos !"
			if self.b_no_dialog_max_yet then
				self:box_error( str )
				self.b_no_dialog_max_yet = false
			else
				self:print_error( str )
			end
			index = nil
		end
	end
	if self.verbose>0 then self:print( "get_free_index( "..filename.." ) return "..index ) end
	return index
end

function VIDEOS:__get_new( filename )
	if self.verbose>0 then aaa.print_method() end
	local index = self:get_free_index( filename )
	if not index then
		self:print_error( "__get_new( "..filename.." ) No free index" )
		return
	end

	local video = self.videos[index]
	if not video then
		self:print_error( "__get_new( "..filename.." ) could not get a video obj for this index "..index )
		return
	end

	local video_bind = index + 32 - 1
	if not video:set_file( filename, video_bind ) then
		self:print_error( "__get_new( "..filename.." ) the video:set_file() failed" )
		return
	end

	self.__slot_used = self.__slot_used + 1

	return video
end

function VIDEOS:get_new( filename, b_lang )
	--self:box_debug( "get_new( "..filename )
	local video = self:__get_new( filename )
	if not video then
		self:print_error( "No video created")
		return
	end

	self.__by_name[filename] = video

	video.__central = {}
	if app then
		local lang = app:get_lang()
		if lang then
			local tab = lang:find_file_translation( filename, true )
			if tab then
				local ver = video.versions
				if ver then
					aaa:box_debug( "VIDEOS:get_new() called existing field versions" )
				else
					video.versions = {}
					ver = video.versions
					ver[lang:get_def()] = video
				end
				for la, fname in pairs(tab) do
					local vid = self:__get_new( fname )
					if vid then
						ver[la] = vid
						vid.versions = ver
						vid.cur = video
					end
				end
			end
		end
	end

--	if b_lang then
--		video.__central = {}
--		video:add_lang_version( filename )
--	end

	return video
end

function VIDEOS:__find( filename )
	if self.verbose>0 then aaa.print_method() end
	filename = self:make_fname( filename )
	local video = self.__by_name[filename]
	return video, filename
end

function VIDEOS:find( filename )
	if self.verbose>0 then aaa.print_method() end
	local video
	video, filename = self:__find( filename )
	if video then
		self:print( filename.." use "..video )
	else
		self:print_error( filename.." is not loaded" )
	end
	return video
end

function VIDEOS:get( filename )
	if self.verbose>0 then aaa.print_method() end
	local video
	video, filename = self:__find( filename )
	if video then
		self:print_debug( "video "..filename.." already exist we reuse "..video )
	else
		self:print_debug( "VIDEOS:get() for "..filename.." did not found: get a new one" )
		video = self:get_new( filename, true )
	end
	return video
end

function VIDEOS:get_sound( filename, b_no_error )
	--self:box_debug( "get_sound( "..filename )
	if self.verbose>0 then aaa.print_method() end
	local video
	if MEDIA.is_name_sound( filename ) then
		if self.verbose>0 then self:print_inverse(  "selected for use : "..filename ) end
		video = self:get( filename )
		if video then
			video:init_sound_on()
			video:stop()
		else
			if not b_no_error then
				aaa.box_error( "VIDEOS:get_sound()\n"..filename.."\nabsent or not usable by tex_video" )
			end
		end
	else
		self:print( filename.." not a sound file" )
	end
	return video
end

function VIDEOS:get_sound_new( filename, b_no_error )
	--self:box_debug( "get_sound( "..filename )
	if self.verbose>0 then aaa.print_method() end
	local video
	if MEDIA.is_name_sound( filename ) then
		if self.verbose>0 then self:print_inverse(  "selected for use : "..filename ) end
		video = self:get_new( filename )
		if video then
			video:init_sound_on()
			video:stop()
		else
			if not b_no_error then
				aaa.box_error( "VIDEOS:get_sound_new() "..filename.." absent or not usable by tex_video" )
			end
		end
	else
		self:print( filename.." not a sound file" )
	end
	return video
end

function VIDEOS:init()
	if self.videos then return end
	self:print( "init()" )

	local name = self:get_name()
	self.nb = self.BIND_VIDEO_NB
	--self.ref = {}
	--self.ref.video_layers = aaa.obj.get_by_name_symbo( "BU_Video" )
	self.videos = {}
	for i=1,self.nb do
		self.videos[i] = VIDEO:create( name.."_"..i, i )
	end
	self:reinit()
end

function VIDEOS:free()
	if not self.videos then return end
	aaa.print( "video_free()" )

	for i=1,self.nb do
		local video = self.videos[i]
		if video then
			video:release()
		end
	end
	self:reinit()
end


