
function meu:define_ui()
	--self:print( "hhhhhhhhhhhhhhhhhhhhhhhhhhhh" )
	local ref = self.ref
	local ui = self.ui
	local bu
--	local par

	local ix = 1
	local iy = 1

	self:add_camera()

	self:add_button(			{ix,iy },				"Visual Debug", self, "b_debug",		true	):set_text_rect_ratio(7)
	iy = iy + 2

	self:add_button(			{ix,iy },				"Play", 		self, "b_play",			true	):set_text_rect_ratio(3)
	bu = self:add_slider(		{ix+3,iy,		5,1},	"Time",			self, "time",			0,		0, 3	)
		ui.bu_time = bu

	iy = iy + 1
	local SX = 2
	self:add_trig_restart(		{ix+1, iy}		)
	self:add_trig_method(		{ix+8-SX*2,iy,	SX, 1},	"Prev", 		self, "inc_key",		-1	)
	self:add_trig_method(		{ix+8-SX, iy,	SX, 1},	"Next", 		self, "inc_key",		1	)
	iy = iy + 2

	bu = self:add_slider(		{ix,iy,			8,1},	"Mouth Factor",	self, "mouth_factor",	1,		0, 10	)
	bu = self:add_slider(		{ix,iy+1,		8,1},	"Volume",		self, "volume",			1,		0, 8	)
	iy = iy + 3

	bu = self:add_button(		{ix,	iy },			"Force", 		self, "b_loyal_force",	false	):set_text_rect_ratio(3)
	bu = self:add_button(		{ix+1,	iy+1 },			"Sinus", 		self, "b_loyal_sinus",	true	):set_text_rect_ratio(2)
	bu = self:add_slider(		{ix+4,iy,		4,2},	"Loyal",		self, "loyal",			0,		0, 1	)
		ui.bu_loyal = bu

	iy = iy + 2.5
	bu = self:add_slider(		{ix,iy,			4,1},	"rot_x",		self, "loyal_rot_x",	0,		-.125, .125	):set_meter(false):set_color_back("x")
	bu = self:add_slider(		{ix+4,iy,		4,1},	"rot_y",		self, "loyal_rot_y",	0,		-.125, .125	):set_meter(false):set_color_back("y")
	iy = iy + 1
	bu = self:add_slider(		{ix,iy,			4,1},	"amp_x",		self, "loyal_amp_x",	0,		0, 1	):set_color_back("x")
	bu = self:add_slider(		{ix+4,iy,		4,1},	"amp_y",		self, "loyal_amp_y",	0,		0, 1	):set_color_back("y")
	iy = iy + 1
	bu = self:add_slider(		{ix,iy,			4,1},	"freq_x",		self, "loyal_freq_x",	0,		0, 1	):set_color_back("x")
	bu = self:add_slider(		{ix+4,iy,		4,1},	"freq_y",		self, "loyal_freq_y",	0,		0, 1	):set_color_back("y")

	ix,iy = 9,3
	bu = self:add_trig_method(	{ix+2, iy,		2,1},	"Dir",			self, "open_dir_story"		):set_color_back("lua")
	bu = self:add_trig_method(	{ix+4, iy,		4,1},	"DataGrid",		self, "set_focus_datagrid"	):set_color_back("focus")

	iy = iy + 1
	local nb = self.story_nb
	local nb_x = 3
	local nb_y = math.floor((nb-1)/nb_x) + 1
	--self:box_good( "nb "..nb.." -> "..nb_x.." * "..nb_y )
--	bu = self:add_selector(	{ix,iy,	8,nb_y},	"Stories"		):set_nb( nb_x, nb_y )
--		bu:set_method_on_value_change( self, "set_story_by_bu", bu )
--		local datagrids = self.datagrids
--		for i=1,#datagrids do
--			bu:set_item_text( i, datagrids[i]:get_name() )
--		end
--		ui.bu_sel = bu
	iy = iy + nb_y + 1
	--self:print( "hhhhhhhhhhhhhhhhhhhhhhhhhhhhwwwwwwwwwwwwwwwwwwwwwww" )

--	self:read_datagrid( true )
end

function meu:get_datagrid_cur()	return self.__datagrid_cur		end

function meu:open_dir_story()
	local datagrid = self:get_datagrid_cur()
	local dir = self.base_dir..datagrid.pre_name
	self:print( dir )
	aaa.launch_explorer( dir )
end

function meu:init()
	--self:print_inverse( "X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X X " )
	local ref = self.ref
	local data = {}
	--ref.data		=	data
	--	data.layer		= self:get_layer(1)
	ref.layer_attr	= self:get_layer(2)


	local bdd		= self:get_layer_bdd(1)
	local story_nb = 0
	local datagrids = {}
	self.base_dir = aaa.dir.make_media_path("Monaco/") -- app.media_path
	local dir_stories =  self.base_dir
			local datagrid = DATAGRID:create( "Test", self:clone_obj(bdd) )
			self:print( "datagrid set to "..dir_stories.."Kinect.ods")
			datagrid:set_file( dir_stories.."Kinect.ods", false )
			datagrid:read()
			story_nb = story_nb + 1
			datagrids[story_nb] = datagrid
	--	end
	--end
	self.datagrids = datagrids
	self.story_nb = story_nb
	--self:print( "hhhhhhhhhhhhhhhhhhhhhhhhhhhhaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" )

--	self:set_story_by_id( 1 )
end


function meu:set_story_by_id( id )
	self:print( "change story to "..id )
	local id_valid = clamp( id, 1, self.story_nb )
	local datagrid = self.datagrids[id_valid]
	local datagrid_cur = self.__datagrid_cur
	if datagrid_cur ~= datagrid then
		if datagrid_cur then
			--local snds = datagrid_cur.snds
			datagrid_cur.id_last = nil	--used to detect change of sequence, we need it to play sound for example
			--if snds then
			--	snds:stop_all()
			--end
		end
		self.__video_playing = nil
		self.__datagrid_cur = datagrid
	end
	if self.story_cur_id ~= id_valid then
		self.story_cur_id = id_valid
	--	self.ui.bu_sel:set_value( id_valid )
	end
end

function meu:set_focus_datagrid()
--	self.datagrids[ self.story_cur_id ]:set_focus_ui()
	aaa.obj.set_focus_ui( self.datagrids[ self.story_cur_id ].ref.bdd )
end

function meu:read_datagrid( datagrid, b_force )
	--self:box_good( "before" )
	if datagrid then
		--self:print( "after")
		if datagrid:read() or b_force then
			--self:box_good( "after" )
			self:print( "after")
			self:update_time_slider()
		end
	end
end

function meu:update()
	--self:print( "gere "..app.media_path)
end

function meu:draw()
	local ref = self.ref
	
	-- self:draw_layers_begin()

		-- get story datagrid
		local datagrid = self:get_datagrid_cur()
		if not datagrid then
			self:set_story_by_id( 1 )
			datagrid = self:get_datagrid_cur()
		end
		if not datagrid then
			return
		end
		-- and read it if needed
		self:read_datagrid( datagrid, false )
		--self:print( "ggg")

	-- self:draw_layers_end()
end

