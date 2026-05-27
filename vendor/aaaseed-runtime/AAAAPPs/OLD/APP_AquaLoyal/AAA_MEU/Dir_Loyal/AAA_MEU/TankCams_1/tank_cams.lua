-- SEQ STUFF
function meu:define_seq_ui(bus)
	self.bu_seq = self:add_bu_sequence( bus, self.seqs )
end

function meu:set_back_luminosity( v )
	local t = self:get_table_always( "mu_back" )
	if not t[1] then
		for i=1,4 do
			t[i] = self:get_mu_by_name( "PIP_Loyal"..i )
		end
	end
	if t[1] then for i=1,4 do t[i]:set_value( v ) end end
end

function meu:set_game_seq( name )
	if not self.meu_game then
		self.meu_game = self:get_meu_by_name( "GameBoid_1" )
	end
	if self.meu_game then self.meu_game:set_seq( name ) end
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local ix = 9
	local iy = 3

	self:define_seq_ui( bus_cur )

	self:set_tab_key( "Regular" )

	self:add_camera( nil, 16 )

	self:add_button(	{ix, 		iy, 1,1}, 	"Wall",				self, "b_wall", 		true	)
	iy = iy + 1
	self:add_button(	{ix,		iy },		"Debug Click", 		self, "b_debug_click",	false	)
	self:add_button(	{ix,		iy+1 },		"see touch", 		self, "b_see_touch",	false	)
	self:add_button(	{ix,		iy+2 },		"Clip", 			self, "b_clip", 		true	)
	iy = iy + 4

	self:add_button(	{ix,		iy },		"Force", 			self, "b_force_back", 	false	):set_text_visible( false )
	self:add_slider(	{ix+1,iy,	7,1},		"BackGround",		self, "back_gray",		1		)

	self:set_tab_key( "Sound" )
	ix,iy = 9,1
	local SY = .8
	self.vols = {}
	local function add_slider( name )
		self:add_slider(	{ix,iy,	8,SY},	name,		self.vols, name,		1, .5, 4.		)
		iy = iy + SY
	end
	local videos = self.snds:get_video_table( )
	for key, _ in pairs_sorted(videos) do
		add_slider( key )
	end

end

function meu:load_snds()
	--self:box_debug( "start sound load" )
	--MEDIA.set_dir_media( "Aqualoyal/Sound/Bulle" )
	local dir = app.media_dir_rel.."Aqualoyal/Sound/"
	local player = SNDS_PLAYER:create("Fish")

	player:add( "slurp",			2, dir.."slurp_",					"mp3",	true	)
	player:add( "picasso",			1, dir.."picasso",					"mp3"			)
	player:add( "dauphin",			1, dir.."dauphin",					"mp3"			)
	player:add( "rascasse",			1, dir.."rascasse",					"mp3"			)
	player:add( "cachalot",			1, dir.."cachalot",					"mp3"			)
	player:add( "medusor",			5, dir.."medusor_",					"mp3",	true	)
	player:add( "requin",			1, dir.."requin",					"mp3"			)
	player:add( "angler",			1, dir.."Angler",					"mp3"			)
	player:add( "sunfish",			1, dir.."Sun",						"mp3"			)
	player:add( "raie",				1, dir.."raie",						"mp3"			)
	player:add( "plancton_begin",	1, dir.."Plancton_invitation",		"mp3"			)
	player:add( "menu_1",			1, dir.."Menu_1",					"mp3"			)
	player:add( "personne",			1, dir.."Personne",					"mp3"			)
	player:add( "bonjour",			1, dir.."Bonjour2",					"mp3"			)

	return player
end

function meu:init()
	self.wall_clicks = {}
	self.__clicks_mouse = {}

	self.z0		= 7.5
	self.sx		= 5.68
	self.sxh	= self.sx * .5
	self.sy		= 2.26
	self.syh	= self.sy * .5

	if not self.snds then -- we load only once
		self.snds = self:load_snds()
	end
	if not self.seqs then -- we load only once, we have a redefine button specific foor this
		self:define_seqs()
	end
end

function meu:define_seqs()
	local seqs = SEQS:recreate( "AquaLoyal", self )

	seqs:create_seq(	"wait",		0, 1,	"on attend du public" 	)	:set_methods( self,	"update_wait",		"set_active_wait"		)
	seqs:create_seq(	"Plancton",	0, 2,	"Plancton" 				)	:set_methods( self,	"update_plancton", 	"set_active_plancton"	)
	seqs:create_seq(	"Bulle",	0, 3,	"Bulle"					)	:set_methods( self,	"update_bulle", 	"set_active_bulle"		)
	seqs:create_seq(	"Story",	0, 4,	"Story"					)	:set_methods( self,	"update_story", 	"set_active_story"		)
	seqs:create_seq(	"Test",		0, 5,	"Test"					)	:set_methods( self,	"update_test", 		"set_active_test"		)

	seqs:begin( true )
end


function meu:update_wait( seq )
	if seq:get_time() > self.snd_time_next then
		self:play_sound( "menu_1" )
		self.snd_time_next = seq:get_time() + 75 + math.random(60)
	end
end
function meu:update_plancton( seq )
	--aaa.print_fn()
	local ti = seq:get_time()
	--self:print( " seq "..seq.." "..ti )
	if ti > 60 then
		local seqs = self.seqs
		seqs:set_seq( "wait" )
	end
end
function meu:update_bulle(seq)		end
function meu:update_story(seq)
	local story = self.meu_stories
	if story and story:is_story_done() then
		self:set_seq( "wait" )
	end
end
function meu:update_test(seq)		end

local b_menu_keep = true
function meu:set_active_wait( seq, b_on )
	if b_on then
		self:set_game_seq( "select" )
		if b_on then
			self:play_sound( "menu_1" )
			self.snd_time_next = 75
		end
	else
		if not b_menu_keep then
			self:print_inverse( "trig fade" )
			if self.meu_game then self.meu_game:trig_fade() end
		end
	end
end
function meu:set_active_plancton( seq, b_on )
	local fp = self:get_mu_by_name( "FP4_Loyal" )
	if fp then
		fp:set_value( b_on and 1 or 0 )
		if b_on then
			fp = self:get_meu_by_name( "FP4_Loyal" )
			fp:set_bu_value( "restart", 1 )
			if not b_menu_keep then
				self:set_game_seq( "wait" )
			end
		end
	end
	if b_on then
		self:play_sound( "plancton_begin" )
	end
	self:set_back_luminosity( b_on and .33 or 1 )
end
function meu:set_active_bulle( seq, b_on )
	if b_on then
		self:set_game_seq( "intro" )
	end
end
function meu:set_active_story( seq, b_on )
	if not self.meu_stories then
		self.meu_stories = self:get_meu_by_name( "Stories_1" )
	end
	if b_on then
		if not b_menu_keep then
			self:set_game_seq( "wait" )
		end
		local id = (self.__story_id or 0) + 1
		if id > 3 then id = 1 end
		self.__story_id = id
		if self.meu_stories then self.meu_stories:play_story( id ) end
	else
		if self.meu_stories then self.meu_stories:stop_story() end
	end
end
function meu:set_active_test( seq, b_on )		end

function meu:set_seq( select )
	self:print( "set_seq "..select )
	local seqs = self.seqs
	seqs:set_seq( select )
end

--COORS
--
function meu:set_screen( monitor, sx )
	--todoloyal use dif info
	monitor:suscribe_to_click(self)
	--local bind = BU_MONITOR:get_texture_bind_2d()
	--self:print( "bind is "..bind )
	sx = 5.68
	local sy = 2.26
	--local sy = sx * py / px

	--self:print( "Found monitor "..monitor )
	local t = { sx=sx, sy=sy }
	t.ox = 0
	t.oy = t.sy * .5 + .3
	t.sxh = t.sx * .5
	t.syh = t.sy * .5
	t.b = .3
	t.t = t.sy + t.b
	t.l = -t.sxh
	t.r = t.sxh
	self.screen = t
end

--todoloyal calib
function meu:read_wall()
	--aaa.print_fn()
	local t = TANK.cur.__loyal_blobs
	if t then
		for id=1,#t do
			local e = t[id]
			--aaa.show( e.x.." "..e.y, "before" )
			local x = e.x * 8.3
			local y = e.y * 2.5 + 2.26*.5 + .3
			---self:print( x.." "..y, "wall" )
			table.insert(	self.wall_clicks,
							{ x=x, y=y, yf=e.y, b_test_contact=true, score_by_team={0,0}, bulle_by_team={0,0} }
						)
		end
	end
end

function meu:intercept_click( x, y )
	local t = self.screen
	x = x * t.sx + t.ox
	y = y * t.sy + t.oy
	--self:print( x.." "..y )
	--aaa.show( x.." "..y, "mouse" )
	table.insert(	self.__clicks_mouse,
					{ x=x, y=y, b_test_contact=true, score_by_team={0,0}, bulle_by_team={0,0} }
				)
end

function meu:read_mouse_clicks()
	self.wall_clicks = {}
	for i=1,#(self.__clicks_mouse) do
		table.insert( self.wall_clicks, self.__clicks_mouse[i] )	--todo append table
	end
	self.__clicks_mouse = {}
end

--was there to draw but show be refined
--[[
function meu:update_clicks( clicks )
	local nb_click = clicks and #clicks or 0
	if nb_click > 0 then
		for i = 1, nb_click do
			local click = clicks[i]
			click.t = click.t - aaa.time.dt * 1.
			click.b_test_contact = false
		end
		while clicks[1] and clicks[1].t <= 0 do
			table.remove( clicks, 1 )
		end
	end

end
]]--

function meu:draw_clicks( clicks )
	local nb_click = clicks and #clicks or 0
	if nb_click > 0 then
		gol.set_line_width( 6. )
		gol.set_line_smooth( true )
		for i = 1, nb_click do
			local click = clicks[i]
			gol.color_yellow( click.t )
			aaa.draw_circle_axe_z( click.x, click.y, 0, .5 + (1 - click.t) * 8, 48 )
		end
	end
end

function meu:project_xyz_at_z( x,y,z, zp )
	x = x * ( zp - self.z0 ) / ( z - self.z0)
	y = y * ( zp - self.z0 ) / ( z - self.z0)
	return x,y
end

function meu:clip_fish( x,y,z, dist_clip )
	if self.b_clip then
		--todo go with fog
		--if z<-12 then return true end
		if z>self.z0 then return true end
		x, y = self:project_xyz_at_z( x, y, z, 0 )
		local d = dist_clip + self.sxh
		if x<-d or d<x then return true end
		d = dist_clip + self.syh
		if y<-d or d<y then return true end
	end
	return false
end

function meu:test_fish( x,y,z )
	local clicks = self.wall_clicks
	local nb_click = clicks and #clicks or 0
	if nb_click == 0 then return end

	local r = .2
	x, y = self:project_xyz_at_z( x, y, z, 0 )
	--self:print( "xy "..x.." "..y )
	if self.b_debug_click then
		gol.reset()
		aaa.draw_circle_axe_z( x ,y, 0, r*2 )
	end
	y = y + self.syh
	local r2 = r * r
	for ic = 1, nb_click do
		local click = clicks[ic]

		--if click.b_test_contact then
			local yf = click.y
			--self:print( click.yf )
			local xc, yc = click.x - x, (yf-.3) - y --todoaqua calage

			local d2 = xc * xc + yc * yc
			if d2 < r2 then
				if self.b_debug_click then
					gol.color_red()
					aaa.draw_disk_axe_z( x ,y - self.syh, 0, r*2 )
				end
				return true, x
			end
		--end
	end
end

function meu:play_fish_sound( name, x )
	if self.vols then
		local volume = self.vols[name]
		self.snds:play( name, false, volume, -x / 5 )
	end
end
function meu:play_sound( name )
	self:play_fish_sound( name, 0 )
end

function meu:update()
	self.snds:update()

	if TANK.cur then
		TANK.cur.__meu_tank_master = self
	end
	self:set_ui_slot_at_start( 3 )

	--	this way the seqs can be initialized after the GP
	local seqs = self:check_need_define_seq_ui( self.seqs )
	seqs:updraw( true )
	local seq_id = seqs:get_seq_cur_id()

	self:read_mouse_clicks()
	if self.b_wall then
		self:read_wall()
	end
	aaa.show( #(self.wall_clicks), "click_nb" )
	if TANK.cur then
		TANK.cur.__clicks = self.wall_clicks
	end

	if self.b_force_back then
		self:set_back_luminosity( self.back_gray )
	end

	local public = TANK.cur.__public
	if public then
		local empty = public.time_empty
		local presence = public.time_presence
		empty = math.floor( math.fmod( empty, 60. ) )
		if empty==15 and self.time_empty_last == 14 then
		--	self:play_sound( "personne" )
		end
		if presence>.2 and self.time_presence_last and self.time_presence_last<.2 then
		--	self:play_sound( "bonjour" )
		end
		self.time_empty_last = empty
		self.time_presence_last = presence
	end
end

function meu:draw()
	MEU.draw( self )

	self.frame = (self.frame or 0) + 1	--unused for nowi think

	--self:print( "Monitor is "..GP.cur:get_monitor("DIF") )
	local monitor = GP.cur:get_monitor("DIF")
	if monitor then
		--table.print( self.screen, "screen" )
		self:set_screen( monitor, 5.68 )	--todoloyal calage
	end


	self.z0 = 7.5*.5
	gol.reset()
	aaa.draw_rect_line_at_z( -self.sxh, -self.syh, self.sxh, self.syh, 0 )
	local x, y, z
	for z=-6,0,1 do
		gol.color_yellow( (z+6)/8 +.25)
		x, y = self:project_xyz_at_z( self.sxh, self.syh, 0, z )
		aaa.draw_rect_line_at_z( -x, -y, x, y, z )
	end
	z = -6
	x, y = self:project_xyz_at_z( self.sxh, self.syh, 0, z )
	gol.draw_lines_3d(	-x,	-y,	z,	-self.sxh,	-self.syh,	0,
						-x,	y,	z,	-self.sxh,	self.syh,	0,
						x,	-y,	z,	self.sxh,	-self.syh,	0,
						x,	y,	z,	self.sxh,	self.syh,	0
					)
	gol.set_depth( false )
	local t = TANK.cur.__loyal_blobs
	if t then
		for id=1,#t do
			local e = t[id]
			e.x = e.x * 8.3
			e.y = e.y * 2.5
			if self.b_see_touch then
				aaa.draw_null( e.x, e.y, 0, .4 )
			end
		end
	end
	--aaa.draw_rect_line( -.5,-.5, .5,.5 )
	--aaa.draw_rect_line( -1.,-1, 1,1	 )

	if false then
		gol.reset()
		gol.set_texture_dim(0)
		gol.set_line_width( 4 )
		local a = { .8,0,0 }
		local b = { 1,1,0 }
		local function draw_vec( v )		gol.draw_lines_3d(	0,0,0,	v[1],v[2],v[3]	)	end
		local function draw_vec_xyz( v )	gol.draw_lines_3d(	0,0,0,	v.x, v.y, v.z	)	end
		gol.color_red()
		draw_vec( a )
		gol.color_green()
		draw_vec( b )

		--local q = QUATERNION.make_rotate( a, b )
		local c = V3.slerp( a, b, wrap_01( aaa.time.t ) )
		gol.color_white()
		draw_vec( c )
		--draw_vec_xyz( q )
	end
end