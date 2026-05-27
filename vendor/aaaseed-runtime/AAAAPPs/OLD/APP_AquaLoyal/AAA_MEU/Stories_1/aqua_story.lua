function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
--	local par

	local ix = 1
	local iy = 1

	self:add_camera()

	self:add_button(		{ix,iy},				"Visual Debug", self, "b_debug",		true	):set_text_rect_ratio(7)
	iy = iy + 2

	self:add_button(		{ix,iy},				"Play", 		self, "b_play",			true	):set_text_rect_ratio(3)
	bu = self:add_slider(	{ix+3,iy,		5,1},	"Time",			self, "time",			0,		0, 3	)
		ui.bu_time = bu

	iy = iy + 1
	local SX = 2
	self:add_trig_restart(	{ix+1, iy,		3,1}	)
	self:add_trig_method(	{ix+8-SX*2,iy,	SX,1},	"Prev", 		self, "inc_key",		-1	)
	self:add_trig_method(	{ix+8-SX, iy,	SX,1},	"Next", 		self, "inc_key",		1	)
	iy = iy + 2

	bu = self:add_slider(	{ix,iy,			8,1},	"Mouth Factor",	self, "mouth_factor",	1,		0, 10	)
	bu = self:add_slider(	{ix,iy+1,		8,1},	"Volume",		self, "volume",			1,		0, 8	)
	iy = iy + 3

	bu = self:add_button(	{ix,iy},				"Force", 		self, "b_loyal_force",	false	):set_text_rect_ratio(3)
	bu = self:add_button(	{ix+1,iy+1		},		"Sinus", 		self, "b_loyal_sinus",	true	):set_text_rect_ratio(2)
	bu = self:add_slider(	{ix+4,iy,		4,2},	"Loyal",		self, "loyal",			0,		0, 1	)
		ui.bu_loyal = bu

	iy = iy + 2.5
	bu = self:add_slider(	{ix,iy,			4,1},	"rot_x",		self, "loyal_rot_x",	0,		-.125, .125	):set_meter(false):set_color_back("x")
	bu = self:add_slider(	{ix+4,iy,		4,1},	"rot_y",		self, "loyal_rot_y",	0,		-.125, .125	):set_meter(false):set_color_back("y")
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,			4,1},	"amp_x",		self, "loyal_amp_x",	0,		0, 1	):set_color_back("x")
	bu = self:add_slider(	{ix+4,iy,		4,1},	"amp_y",		self, "loyal_amp_y",	0,		0, 1	):set_color_back("y")
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,			4,1},	"freq_x",		self, "loyal_freq_x",	0,		0, 1	):set_color_back("x")
	bu = self:add_slider(	{ix+4,iy,		4,1},	"freq_y",		self, "loyal_freq_y",	0,		0, 1	):set_color_back("y")

	ix,iy = 9,3
	bu = self:add_trig_method(	{ix+2, iy,	2,1},	"Dir",			self, "open_dir_story"		):set_color_back("lua")
	bu = self:add_trig_method(	{ix+4, iy,	4,1},	"DataGrid",		self, "set_focus_datagrid"	):set_color_back("focus")

	iy = iy + 1
	local nb = self.story_nb
	local nb_x = 3
	local nb_y = math.floor((nb-1)/nb_x) + 1
	--self:box_good( "nb "..nb.." -> "..nb_x.." * "..nb_y )
	bu = self:add_selector(		{ix,iy,	8,nb_y},	"Stories"		):set_nb( nb_x, nb_y )
		bu:set_method_on_value_change( self, "set_story_by_bu", bu )
		local datagrids = self.datagrids
		for i=1,#datagrids do
			bu:set_item_text( i, datagrids[i]:get_name() )
		end
		ui.bu_sel = bu
	iy = iy + nb_y + 1

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
	local ref = self.ref
	local data = {}
	--ref.data		=	data
	--	data.layer		= self:get_layer(1)
	ref.layer_attr	= self:get_layer(2)

	ref.text = {}
		local t = {}
		local ind = 3
		t.layer		= self:get_layer(ind)
		t.bdd		= self:get_layer_bdd(ind)
		t.font		= param.get_ref( t.bdd, "font" )
		t.alignment	= param.get_ref( t.bdd, "alignment" )
		t.text		= param.get_ref( t.bdd, "text" )
		t.x 		= param.get_ref( t.bdd, "origin_x" )
		t.y 		= param.get_ref( t.bdd, "origin_y" )
		t.z 		= param.get_ref( t.bdd, "origin_z" )
		t.llen 		= param.get_ref( t.bdd, "line_length" )
		t.model		= aaa.layer.get_model_no_error( t.layer )
		t.size		= param.get_ref( t.model, "size_factor" )
		ref.text[1] = t

	local bdd		= self:get_layer_bdd(1)
	local story_nb = 0
	local datagrids = {}
	self.base_dir = app.media_dir_rel.."AquaLoyal/"
	local dir_stories =  self.base_dir.."Stories/"
	local dirs = aaa.dir.get_dirs( dir_stories )
	--self:print( tostring( t_dir ) )
	if dirs then
		for _, dir_name in pairs_sorted( dirs ) do
		--local dir_name = "Stories/Story_"..i.."/"
		--if  MEDIA.is_dir_exist( dir_name ) then
			--self:box_good( "found dir \""..dir_name.."\"" )
			local datagrid = DATAGRID:create( dir_name, self:clone_obj(bdd) )
			datagrid.pre_name = "Stories/"..dir_name.."/"
			datagrid:set_file( dir_stories..dir_name.."/"..dir_name..".csv", false )
			datagrid:read()
			local tab = datagrid:get_field_all_value( "sound" )
			if tab then
				local dir = dir_stories..dir_name.."/"
				self:print_debug( "aqua_story now process dir "..dir )
				local snds = SNDS_PLAYER:create(dir_name)
				for i=1,#tab do
					local fname = tab[i]
					self:print_debug( "fname is "..fname )
					local name_pure = aaa.file.get_name_pure( fname )
					self:print_debug( "name_pure is "..name_pure )
					local dir_snd = dir..name_pure.."/"
					self:print_debug( "dir_snd is "..dir_snd )
					snds:add( name_pure, 1, dir_snd..fname )
					self:print_debug( "name_pure again is "..name_pure )
					local video = snds:found_free( name_pure )
					local datagrid_mouth = DATAGRID:create( dir_name.."_mouth", self:clone_obj(bdd)  )
					datagrid_mouth:set_file( dir_snd..name_pure..".csv", false )
					video.datagrid_mouth = datagrid_mouth
				end
				datagrid.snds = snds
			end
			story_nb = story_nb + 1
			datagrids[story_nb] = datagrid
		end
	end
	self.datagrids = datagrids
	self.story_nb = story_nb
--	self:set_story_by_id( 1 )
end

function meu:is_story_done()
	local datagrid = self:get_datagrid_cur()
	if datagrid.duration then
		local ti = self.ui.bu_time:get_value()
		if ti >= datagrid.duration -.001 then
			return true
		end
	end
	return false
end

function meu:update_time_slider()
	local datagrid = self:get_datagrid_cur()
	datagrid.duration = datagrid:get_field_max("time")
	--self:box_good( "duration compute to "..datagrid.duration )
	--self:print( "duration is ", self:get_datagrid_cur().duration )
	self.ui.bu_time:set_min_max( 0, datagrid.duration )
end
function meu:set_story_by_id( id )
	self:print( "change story to "..id )
	local id_valid = clamp( id, 1, self.story_nb )
	local datagrid = self.datagrids[id_valid]
	local datagrid_cur = self.__datagrid_cur
	if datagrid_cur ~= datagrid then
		if datagrid_cur then
			local snds = datagrid_cur.snds
			datagrid_cur.id_last = nil	--used to detect change of sequence, we need it to play sound for example
			if snds then
				snds:stop_all()
			end
		end
		self.__video_playing = nil
		self.__datagrid_cur = datagrid
	end
	if self.story_cur_id ~= id_valid then
		self.story_cur_id = id_valid
		self.ui.bu_sel:set_value( id_valid )
	end
	self:update_time_slider()
	--todo we should deal in SELECTOR with values not used
end
function meu:set_story_by_bu( bu )
	local val = bu:get_value()
	self:set_story_by_id( val )
end

function meu:play_story( id )
	if id then
		self:set_story_by_id( id )
	end
	--todo next story
	self:set_bu_value( "play", true )
	self:restart()
end
function meu:stop_story()
	--todo next story
	self:set_bu_value( "play", false )
	self:restart()
	if self.__video_playing then
		self.__video_playing:stop()
		self.__video_playing = nil
	end
end
function meu:set_focus_datagrid()
--	self.datagrids[ self.story_cur_id ]:set_focus_ui()
	aaa.obj.set_focus_ui( self.datagrids[ self.story_cur_id ].ref.bdd )
end
function meu:draw_text( i, txt )
	local ref = self.ref.text[1]
	param.set( ref.text, txt or "" )
	if txt then
		--if x then param.set( ref.x, x ) end
		aaa.obj.update_then_draw( ref.layer )
	end
end

function meu:read_datagrid( datagrid, b_force )
	--self:box_good( "before" )
	if datagrid:read() or b_force then
		--self:box_good( "after" )
		self:update_time_slider()
	end
end

function meu:get_time()		return self.ui.bu_time:get_value()	end
function meu:set_time(t)	self.ui.bu_time:set_value(t)		end
function meu:restart()		self:set_time( 0 )					end
function meu:inc_key( inc )
	local t = self:get_time()
	--self:print( "starting with time "..t )
	local datagrid = self:get_datagrid_cur()
	local id, prev, next, inter = datagrid:get_id_prev_next_inter( "time", t )
	local new_id = datagrid:inc_id_prev( id, inc )
	if new_id then
		local time = datagrid:get_field_at_id( "time", new_id )
		--self:print( "new_id "..new_id.." "..time )
		self:set_time( time )
	end
end

function meu:update()
	self:set_ui_slot_at_start( 2 )
end

function meu:update_mouth()
	local mouth = 0
	local video = self.__video_playing
	if video then
		if video:is_playing() then
			local dm = video.datagrid_mouth
			if dm then
				local msec = video:get_time() * 1000
				dm:set_position_by_key_value( "msec", msec )
				mouth = dm:get_field("value") * self.mouth_factor
			end
		else
			self.__video_playing = nil
		end
	end
	return mouth
end

function meu:draw()
	local tank = TANK.cur

	local ref = self.ref
	self:draw_layers_begin()

	-- get story datagrid
	local datagrid = self:get_datagrid_cur()
	if not datagrid then
		self:set_story_by_id( 1 )
		datagrid = self:get_datagrid_cur()
	end
	-- and read it if needed
	self:read_datagrid( datagrid, false )

	-- deal with time
	local t = self:get_time()
	--self:print( "time in update is "..t )
	local duration = datagrid.duration
	t = clamp( t, 0, duration )
	if self.b_play then
		t = clamp( t + aaa.time.dt, 0, duration )
		self:set_time( t )
	end

	--if true then return end
--	if self.b_boid then
--		aaa.obj.update_then_draw( ref.layer_boid )
--	end
--

	local snds = datagrid.snds
	-- set datagrid to the right line
	if datagrid:set_position_by_time( t ) then
		--change of seq
		if snds then
			local fname = datagrid:get_field("sound")
			if fname then
				local name_pure = aaa.file.get_name_pure( fname )
				self.__video_playing = snds:play( name_pure, false, self.volume )
			end
		end
		local v = datagrid:get_field("send_cacha")
		if v and v==1 then
			tank:send_one_fish("cacha")
		end
	end
	if snds then
		--aaa.audio.set_volume( vol )
		snds:update()
	end
	local mouth = self:update_mouth()
	tank.mouth = interpolate( tank.mouth or 0, mouth, .5 )

	--self:print( "mouth "..mouth )

--	if self.prev then
	if self.b_debug then
		gol.reset()
		self:draw_text( 1, datagrid:get_name().."\n"..datagrid:get_field("title")  )
		if self.__video_playing then
			gol.set_texture_dim(0)
			local B = .95
			local SY = .7
			aaa.draw_rect_line( 0, B, .4, B + SY )
			aaa.draw_rect( 0, B, .4, B + mouth * SY )
		end
	end

	if tank:get_ggrea() then
		self:do_csv( datagrid )
	end
	--self.imgs.logo:draw_xy_sx( 3.9,-2, .5  )
	self:draw_layers_end()
	
	if self.b_loyal_force then
		if self.b_loyal_sinus then
			self.ui.bu_loyal:set_value( math.sin( aaa.time.t ) *.5 + .5 )
		end
		tank.loyal = self.loyal
	end
	--tank.loyal_rot_x = math.sin( aaa.time.t *.75851) * .05
	--tank.loyal_rot_y = math.sin( aaa.time.t ) * .05
	local rot_x = self.loyal_rot_x
	local rot_y = self.loyal_rot_y
	local public = TANK.cur.__public
	if public then
		local rx
		local ry
		if public.presence == 1 then	rx = (public.center_v - .275) * -.2
										ry = (public.center_u - .5) * -.3
		else							rx = 0
										ry = 0
		end
		rx = interpolate( self.public_rot_x_last or 0, rx, aaa.time.dt * 8. )
		ry = interpolate( self.public_rot_y_last or 0, ry, aaa.time.dt * 8. )
		self.public_rot_x_last = rx
		self.public_rot_y_last = ry
		rot_x = rot_x + rx
		rot_y = rot_y + ry
	end
	local freq = .5
	self.loyal_ph_x = (self.loyal_ph_x or 0) + self.loyal_freq_x * aaa.time.dt
	tank.loyal_rot_x = rot_x + (aaa.math.get_fractalsum( 1, 1, self.loyal_ph_x,	freq, 3 ) - .5) * self.loyal_amp_x * 2
	self.loyal_ph_y = (self.loyal_ph_y or 0) + self.loyal_freq_y * aaa.time.dt
	tank.loyal_rot_y = rot_y + (aaa.math.get_fractalsum( 0, 0, self.loyal_ph_y,	freq, 3 ) - .5) * self.loyal_amp_y * 2
end

function meu:get_preset_nb()	return 16	end

function meu:do_csv( datagrid )
	local tank = TANK.cur
--ECRAN
	local grea = tank:get_grea_by_name( "ecran" )
	if grea then
		local race = grea:get_race()
		local sha = race:get_shading()
		local boid = grea:get_boid()

		local visibility = datagrid:get_field("img")
		local img_name = datagrid:get_field("img_name")
		if img_name then
			local img = IMGS.get_img( datagrid.pre_name..img_name )
			if img then
				if self.b_debug then
					gol.set_texture_dim(2)
					gol.color_white( visibility )
					img:draw_xy_sx( -.6, 1.3, 1  )
					gol.color_white( 1 )
				end
				gol.set_tex_unit_2d_bind( 7, img:get_bind() )
			end
		end

		if visibility then sha:set_val( "frag", "float", 6, visibility ) end
		local screen_x = datagrid:get_field("screen_x")
		if screen_x then
			sha:set_val( "frag", "float", 8,	screen_x )
			boid:set_force( "target",	"x", 	screen_x*2.25 )
		end
		--self:print( grea.." "..(x*2.33) )
		local target = datagrid:get_field("target")
		if target then
			boid:set_target_influence( target )
			boid:set_force( "box",		"sy",	interpolate(	2,		1.51,	target ) )
			boid:set_force( "box",		"y",	interpolate(	3.5,	0,		target ) )
			boid:set_force( "box",		"x",	interpolate(	0,		0,		target ) )
		end
		boid:set_speed_min_max( datagrid:get_field("speed_min"), datagrid:get_field("speed_max") )
		boid:set_param( "viscosity", datagrid:get_field("viscosity") )
	end
--LOYAL
	local grea = tank:get_grea_by_name( "loyal" )
	if grea then
		--self:print( "loyal" )
		local race = grea:get_race()
		local sha = race:get_shading()
		local boid = grea:get_boid()

		boid:set_force( "target",	"x",	datagrid:get_field("loyal_target_x") )
		boid:set_force( "target",	"y",	datagrid:get_field("loyal_target_y") )
		boid:set_force( "target",	"z",	datagrid:get_field("loyal_target_z") )
		--self:print( datagrid:get_field("loyal_viscosity") )
		boid:set_param( "viscosity", datagrid:get_field("loyal_viscosity") )

		--boid:set_speed_min_max( datagrid:get_field("speed_min"), datagrid:get_field("speed_max") )
	end
--ENABLE DISABLE by group in "cut"
	local t_active = { "Angler"	, "lune", "lune_rare", "Meduse", "Picasso", "rascasse", "thon", "raie", "dauphin", "Sun", "Req" }
--	local t_active = { "Test", "rascasse" }
	local b_vis =  datagrid:get_field_as_bool("fish_visible")
	for _,name in pairs( t_active ) do
		local grea = tank:get_grea_by_name( name )
		grea:set_active( b_vis )
	end
end