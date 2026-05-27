local root = aaa.pc.is_maa() and app.media_dir_rel or APP.media_dir_rel
local folder = root.."SITEM/"

if not aaa.lua.global.get( "sitem" ) then
	aaa.lua.global.declare_table( "sitem" )
	sitem = meu
end

function meu:init_videos()
	self.chapters =
	{
		{
			name = "Arts",
			dir_name = "Arts",
		},
		{
			name = "Sciences",
			dir_name = "sciences",
		},
		{
			name = "Histoire",
			dir_name = "histoire",
		},
		{
			name = "Simulation",
			dir_name = "simulation",
		},
---[[
		{
			name = "Fred",
			dir_name = "Fred",
		},
--]]
---[[
		{
			name = "Fred2",
			dir_name = "Fred2",
		},
--]]
	}
	local function sort_video( a, b )
		--local ai = string.sub( a, 1, 2 )
		--ai = tonumber( ai )
		--local bi = string.sub( b, 1, 2 )
		--bi = tonumber( bi )
		return a < b
	end

	local bind = 0
	local bank_offset = VIDEOS.BIND_VIDEO_START / 32
	for cha_id, cha in ipairs(self.chapters) do
		self:print( "Chapter "..cha.name )
		cha.videos = {}
		local dir_fpath = folder..cha.dir_name
		local files = aaa.dir.get_files( dir_fpath )
		table.sort( files, sort_video )
		--table.print( t_file, "videos"..ch_id, 1 )
		local vid_id = 0
		for _, vid_name in ipairs(files) do
			--avoid DS_Store files
			if string.find( vid_name, ".DS_Store" ) then
			--avoid lua and text (called directly)
			elseif string.find( vid_name, ".lua" ) or string.find( vid_name, ".txt" ) then
			else	--video or slide show
				local video = {}
				video.dir		= dir_fpath
				video.filename	= vid_name

				local file_fpath = dir_fpath.."/"..vid_name
				local len = string.find( vid_name, '%.' ) - 1
				if string.find( vid_name, '_tb' ) then
					video.b_tb = true
					len = len - 3
				end
				local base_name = string.sub( vid_name, 4, len )
				video.base_name = base_name

				local b_slide = false
				local b_valid = false
				if string.find( vid_name, ".sld" ) then
					b_slide = true
					self:print( "\tSlideShow "..vid_name )
					local img_table = aaa.dir.get_files( dir_fpath.."/"..base_name, "*.tga" )
					if img_table then
						--table.print( img_table, "img_table before" )
						for k, elt in IPAIRS( img_table ) do
							img_table[k] = cha.name.."/"..base_name.."/"..elt
						end
						--table.print( img_table, "img_table after" )
						--we need to pass a dir here
						video.seq_img = IMG_SEQ:create_from_array( vid_name, img_table )
						b_valid = true
					else
						self:print_error( "No tga in folder "..dir_fpath )
					end
					--self:box_debug( "slideshow" )
				else
					self:print( "\tVideo "..vid_name )
					local bank = math.floor( bind / 32 )
					video.bind		= bind - bank * 32
					video.bank		= bank + bank_offset
					video.index		= bind
					b_valid = true
				end

				if b_valid then
					local lua_name = base_name..".lua"
					self:print( "lua_name : "..lua_name )
					local lua_path = dir_fpath.."/"..lua_name
					self:print( "lua_path : "..lua_path )
					if aaa.file.is_exist( lua_path ) then
						--self:box_debug( "find "..lua_name )
						self:print( "find "..lua_name )
						--dofile(lua_path)
						local tab = video
						aaa.lua.dofile_protected( lua_path, false )
						table.print( tab, "tab", 2 )
						tab = nil
						--self:box_debug( "done "..lua_name )
					end

					vid_id = vid_id + 1
					cha.videos[vid_id] = video
					if b_slide then
					else
						aaa.video.set_bind_filename( video.bank * 32 + video.bind, file_fpath )
						bind = bind + 1
						if bind >= 128 then
							self:box_error( "Too much video" )
							return
						end
					end
				end
			end
		end
	end
	--table.print( self.chapters, "chapters", 5 )
end


function meu:change_video()
	self:print( "change_video "..self.s_chapter.." "..self.s_video )

	local cha_id = self.s_chapter
	local vid_id = self.s_video
	local index = self.video_index

	if index and cha_id>0 and vid_id>0 then
		local video = self.video[index]
		local cha = self.chapters[cha_id]
		--table.print( cha, "cha", 1 )
		if cha then
			local vid = cha.videos[vid_id]
			table.print( vid, "vid", 1 )
			if vid then
				vid.__time_start = aaa.time.t
				if vid.seq_img then
				else
					video:set_bu_value( "bank", vid.bank )
					video:set_bu_value( "bind", vid.bind )
					video:set_bu_value( "restart", true )
					video:set_bu_value( "play", true )
					self.video_to_check = video
					self.video_to_check_count = 0
					index = 1-index
					self.video_index = index
					self.video[index]:set_bu_value( "play", false )
				end
				self.vid_cur = vid
				if not self.meu_title then
					self.meu_title		= self:get_meu_by_name_no_error	( "typo_1" )
					self.meu_title_sub	= self:get_meu_by_name_no_error	( "typo_2" )
					self.meu_info		= self:get_meu_by_name_no_error	( "typo_3" )
				end
				if self.meu_title then
					self:print( "title "..vid.title )
					self.meu_title:set_text( vid.title or "" )
					self.meu_title_sub:set_text( vid.title_sub or "" )
					self.meu_info:set_text( vid.filename )
				end
			end
		end
	end
end

function meu:novation_change_chapter( on, id )
	if not self.b_novation then return end
	--aaa.print_fn()
	if on>=.5 then
		for i=1,6 do
			if i~=id then
				novation:set_elt_xy_low( 9, i, 0 )
			end
		end
		self:print( "chapter is "..id )
	else
		self:print( "chapter is none" )
		id = 0
	end
	self:set_bu_value( "chapter", id )
end
function meu:novation_change_video( on, id )
	if not self.b_novation then return end

	--aaa.print_fn()
	if on>=.5 then
		for i=1,8 do
			for j=1,2 do
				if (j-1)*8+i~=id then
					novation:set_elt_xy_low( i, j, 0 )
				end
			end
		end
		self:print( "video is "..id )
	else
		self:print( "video is none" )
		id = 0
	end
	self:set_bu_value( "video", id )
end

function meu:__init_meus()
	local tab = {}
	tab[0] = self:get_meu_by_name( "Video_i" )
	tab[0]:set_ui_slot( 3 )
	tab[1] = self:get_meu_by_name( "Video_j" )
	tab[1]:set_ui_slot( 4 )
	self.video = tab
	self.video_index = 0
end

function meu:define_ui()
	self:init_videos()
	self:print( "After init_videos()" )
	
	local ref = self.ref
	local ui = self.ui
	local bu
	local SY

	local ix,iy = 1,1
	bu = self:add_button(	{	ix, iy },				"Run",				self, "b_run",				false )
		bu:set_text_rect_ratio( 2 )
		bu:set_preset_use( false )
	bu = self:add_button(	{	ix+3, iy },			"Table",			self, "b_table",				false )
		bu:set_text_rect_ratio( 2 )
		bu:set_preset_use( false )

	bu = self:add_button(	{	ix+6, iy },			"Novation",			self, "b_novation",				false )
		bu:set_text_rect_ratio( 3 )
		bu:set_preset_use( false )

	bu = self:add_button(	{	ix+12, iy },			"test",				self, "b_test",				false )
		bu:set_text_rect_ratio( 2 )
		bu:set_preset_use( false )
	iy = iy + 1
	bu = self:add_button(	{	ix+1, iy },			"See table",			self, "b_see_table",				false )
		bu:set_text_rect_ratio( 4 )
		bu:set_preset_use( false )

	iy = iy + 3
	bu = self:add_selector(	{ix,iy,	4,7},	"Chapter" )
		bu:set_nb_min_0( 1, 7 )
		bu:set_item_text( 1, "No" )
		bu:set_item_text_from_nb_minus_1( 2 )
		for i=2,#self.chapters+1 do
			bu:set_item_text( i, self.chapters[i-1].name )
		end
		bu:set_target_lua( self, "s_chapter" )
		bu:set_method_on_value_change( self, "change_video" )

	bu = self:add_selector(	{ix+4,iy,	8,2},	"Video" )
		bu:set_nb_min_0( 8, 2 )
		bu:set_item_text( 1, "No" )
		bu:set_item_text_from_nb_minus_1( 2 )
		bu:set_target_lua( self, "s_video" )
		bu:set_method_on_value_change( self, "change_video" )

	if aaa.lua.global.get( "novation" ) then
		for i=1,6 do
			novation:set_method_switch( 9, i, self, "novation_change_chapter", i )
		end
		for i=1,8 do
			for j=1,2 do
				novation:set_method_switch( i, j, self, "novation_change_video", (j-1)*8+i )
			end
		end
	end
end


function meu:init()
	local ref = self.ref

end


function meu:update()
	local ref = self.ref
	local ui = self.ui

	if not self.video then
		self:__init_meus()
	end

	if self.b_run then
	else
--		if not self.__b_onsite_forced and app:is_onsite() then
			-- can't be done in init or define_ui
--			self.__b_onsite_forced = true
--			self:set_bu_value( "run", true )
--		end
	end
	if sitem then
		sitem.meu_sitem = self
	end

	if self.b_table and sitem and sitem.meu_anal then
		local meu_anal = sitem.meu_anal
		--                                               meu_anal:print( "Test" )
		local vid_id = meu_anal:get_bu_value( "video" )
		local cha_id = meu_anal:get_bu_value( "chapter" )
		--self:print( "from table "..cha_id.." / "..vid_id )
		self:set_bu_value( "chapter", cha_id )
		self:set_bu_value( "video", vid_id )
	end

	if self.b_test then
		local count = (self.test_count or 8) - 1
		if count < 0 then
			count = 8
			self:set_bu_value( "chapter", math.random(6)-1 )
			self:set_bu_value( "video", math.random(16)-1 )
		end
		self.test_count = count
	end

	local video_meu = self.video_to_check
	if video_meu then
		local count = self.video_to_check_count + 1
		if count > 1 then
			if video_meu:get_time() > .05 then
				--self.pip:set_bu_value( "tex", 8 + 1-self.video_index )
				self.video_to_check = nil
			end
		end
		self.video_to_check_count = count
	end

	local alpha = 0
	if self.vid_cur then
		local t = aaa.time.t - ( self.vid_cur.__time_start or 0 )
		alpha = clamp_01( 1 - (t - 5) )
	end
	if self.meu_title then
		self.meu_title:get_mu():set_value( alpha )
		self.meu_title_sub:get_mu():set_value( alpha )
		self.meu_info:get_mu():set_value( alpha )
	end
	--if aaa.lua.global.get( "novation" ) then
	--	if novation:is_on( 9, 1 ) then
	--		self:print( "novation ON")
	--	end
	--end
end


function meu:draw()
	MEU.draw(self)

end
