function meu:set_wait_active( seq, b_on )
	if b_on then
		self.snds:stop_all()
		self:set_bulle_params(0)
		self:trig_fade()
	end
	param.set( self.ref.part.emission_box_size_factor, b_on and 5.4 or 4.7 )
end
--b_sec, speed, angle, size_min, size_max, gain, bias, dechet
function meu:update_wait( seq )
end

function meu:set_select_active( seq, b_on )
	if b_on then
		self.snds:stop_all()
		self:set_bulle_params(0)
	end
	param.set( self.ref.part.emission_box_size_factor, b_on and 5.4 or 4.7 )
	self.b_select = b_on
end
--b_sec, speed, angle, size_min, size_max, gain, bias, dechet
function meu:update_select( seq )
	self:set_bulle_params( 2., .125, 0, .080	, .160, .500, .500, 0 )
end

function meu:set_intro_active( seq, b_on )
	if b_on then
		--self:print( "set_intro_active "..self.volume_back )
		self:play_snd( "back", true )
		self:play_snd( "pre_l1", false )
	end
	self.str_rule = b_on and "L'equipe qui eclate\nle plus de bulles\nde sa couleur\ngagne la partie !" or nil
end
function meu:update_intro(seq)
	self:set_bulle_params( 200, .5, 20, .030, .150, .500, .500, 0 )
end

function meu:set_pre_active( seq, b_on )
	--self:print( "receive_seq_active() " .. b_on .. " " .. seq)
	self.b_decompte = b_on
	if b_on then
		local lev = seq:get_level_id()
		if lev <= 1 then
			self.level_info = nil
		elseif lev == 2 then
			self.level_info = { x={-1,1}, str="Attention\nne vous trompez\npas de couleur\n!!!" }
			self:play_snd( "pre_l2", false )
		elseif lev == 3 then
			self.level_info = { x={-1,1}, str="Nettoyez la mer !\nDes points en bonus." }
			self:play_snd( "pre_l3", false )
		end
	end
end

function meu:set_post_active( seq, b_on )
	--self:print( "receive_seq_active() " .. b_on .. " " .. seq)
	self.b_winner = b_on
	if b_on then
		local teams = self.teams
		for i=1,2 do
			if  teams[i].score > teams[3-i].score then
				self:play_snd( "win_"..i, false )
			end
		end
	end
end

function meu:update_pre(seq)
	self:set_bulle_params( 0, 2, 0 )
	local last = self.decompte
	self.decompte = math.floor( seq:get_time_left() ) + 1
	if last ~= self.decompte then
		self:play_snd( "second", false )
	end
end

function meu:set_game_active( seq, b_on )
	self:print( "receive_seq_active() " .. b_on .. " " .. seq)
	self.b_game = b_on
	if b_on then
		self:play_snd( "go", false )
		for i = 1, 2 do
			self.teams[i].score = 0
		end
	else
		self:trig_fade()
		self:set_bu_value( "nb_sec", 0 )
	end
	self:set_bu_value( "dechet", 0 )
end

function meu:update_level_1( seq )
	local fn_linear = math.fn_linear
	local time = seq:get_time()
	--aaa.show( time, "time_game" )
	-- fn_linear(	 input, nb, 1,	time,value, time,value, time,value, ... )
	local nb_sec = fn_linear( time,1,
		0,	10,
		5,	2,
		27,	15,
		28,	0,
		31,	0,
		32,	100,
		33,	0,
		35,	0,
		36,	250,
		37,	0,
		38,	0,
		39,	500,
		42,	0,
		43,	3,
		55,	10,
		60,	2
	)
	local speed = fn_linear( time, 1, 0, .3, 60, .5 )
	local angle = fn_linear( time, 1, 0, 0, 33, 0, 60, 30 )
	local size_min, size_max = fn_linear( time, 2,
		0,	.3,		.3,
		5,	.1,		.1,
		30,	.05,	.2,
		32,	.2,		.2,
		36,	.1,		.1,
		39,	.01,	.02,
		42,	.01,	.02,
		43,	.025,	.2,
		50,	.1,		.2,
		60,	.05,	.1
	)
	local gain 		= fn_linear( time, 1, 0, .5, 60, .8 )
	local bias		= fn_linear( time, 1, 0, .5, 60, .5 )
	local dechet	= 0
	self:set_bulle_params( nb_sec, speed, angle, size_min, size_max, gain, bias, dechet )
end

function meu:update_level_2(seq)
	local fn_linear = math.fn_linear
	local time = seq:get_time()
	--aaa.show( time, "time_game" )
	-- fn_linear(	 input, nb, 1,	time,value, time,value, time,value, ... )
	local nb_sec	= fn_linear(	time, 1,	0, 10,	5, 3,		20, 15,		28, 0,	31, 0,	32, 100,	39, 0,	40, 30,	60, 20	)
	local speed		= fn_linear(	time, 1,	0, .2,	35, .5,		60, .1	)
	local angle		= fn_linear(	time, 1,	0, 50,	20, 100,	35, 100,	60, 90	)
	local size_min, size_max = fn_linear(	time, 2,	0, .1, .1,	5, .1, .1,	20, .05, .2,	32, .05, .2,	36, .02, .1,	60, .02, .1	)
	local gain		= fn_linear(	time, 1,	0, .5,	60, .1	)
	local bias		= fn_linear(	time, 1,	0, .5,	60, .5	)
	local dechet	= 0
	self:set_bulle_params( nb_sec, speed, angle, size_min, size_max, gain, bias, dechet )
end

function meu:update_level_3(seq)
	local fn_linear = math.fn_linear
	local time = seq:get_time()
	--aaa.show( time, "time_game" )
	-- fn_linear(	 input, nb, 1,	time,value, time,value, time,value, ... )
	local nb_sec	= fn_linear(	time, 1,	0, 2,	5, 3,		20, 15,	 27,0,	28,0, 	31, 0,  	32, 10, 	39, 10, 	40, 30,	60, 20	)
	local speed		= fn_linear(	time, 1,	0, .2,	35, .3,		60, .1	)
	local angle		= fn_linear(	time, 1,	0, 0,	20, 20,	35, 50,	60, 00	)
	local size_min, size_max = fn_linear(	time, 2,	0, .1, .1,	5, .1, .1,	20, .05, .2,	32, .05, .2,	36, .02, .1,	60, .02, .1	)
	local gain		= fn_linear(	time, 1,	0, .5,	60, .1	)
	local bias		= fn_linear(	time, 1,	0, .5,	60, .5	)
	local dechet	= fn_linear(	time, 1,	0, .0,	5,.1,  20,.05,  32,.5,  40,.2,  50,.3, 60, .2	)
	self:set_bulle_params( nb_sec, speed, angle, size_min, size_max, gain, bias, dechet )
end
function meu:update_post(seq)
end
function meu:update_end(seq)
	self:set_bulle_params( 200, .5, 20, .030, .150, .500, .500, 0 )
end

function meu:define_seqs()
	local seqs = SEQS:recreate( "GameBulle", self )
	local seq

	seq = seqs:create_seq(	"wait",			0, 1,	"on fait rien")			:set_methods( self, "update_wait",		"set_wait_active"	)
	seq = seqs:create_seq(	"intro",		0, 2,	"Rules",	8	)		:set_methods( self, "update_intro", 	"set_intro_active"	)

	seq = seqs:create_seq(	"level1_pre",	1, 3,	"Pre1",		3	)		:set_methods( self, "update_pre",		"set_pre_active"	)
	seq = seqs:create_seq(	"level1_play",	1, 4,	"Play1",	45	)		:set_methods( self, "update_level_1",	"set_game_active"	)

	seq = seqs:create_seq(	"level1_post",	1, 5,	"Post1",	5	)		:set_methods( self, "update_post",		"set_post_active"	)

	seq = seqs:create_seq(	"level2_pre",	2, 6,	"Pre2",		3	)		:set_methods( self, "update_pre",		"set_pre_active"	)
	seq = seqs:create_seq(	"level2_play",	2, 7,	"Play2",	60	)		:set_methods( self, "update_level_2",	"set_game_active"	)
	seq = seqs:create_seq(	"level2_post",	2, 8,	"Post2",	5	)		:set_methods( self, "update_post",		"set_post_active"	)

	seq = seqs:create_seq(	"level3_pre",	3, 9,	"Pre3",		3	)		:set_methods( self, "update_pre",		"set_pre_active"	)
	seq = seqs:create_seq(	"level3_play",	3, 10,	"Play3",	60	)		:set_methods( self, "update_level_3",	"set_game_active"	)
	seq = seqs:create_seq(	"level3_post",	3, 11,	"Post3",	5	)		:set_methods( self, "update_post",		"set_post_active"	)

	seq = seqs:create_seq(	"end",			0, 12,	"Bye",		5	)		:set_methods( self, "update_end"		)
	seq = seqs:create_seq(	"select",		0, 0,	"on attend un choix")	:set_methods( self, "update_select",	"set_select_active"	)

	seqs:begin( true )
end

function meu:set_bulle_params( nb_sec, speed, angle, size_min, size_max, gain, bias, dechet )
	--seq defined  before ui so ....
	if not self.__b_ui_defined then return end

	local function set( name, val, ... )
		if val then
			self:set_bu_value( name, val, ... )
			--aaa.show( val, name )
		end
	end
	set( "nb_sec",	nb_sec	)
	set( "speed",	speed	)
	set( "angle",	angle	)
	set( "size",	size_min, 1 )
	set( "size",	size_max, 2 )
	set( "gain",	gain	)
	set( "bias",	bias	)
	set( "dechet",	dechet	)
end

function meu:init_ref()
	local ref = self.ref
	ref.layer_attr = self:get_layer(1)

	local tab
	ref.boid = {}
	tab = ref.boid
	tab.layer = self:get_layer(2)
		tab.bdd = aaa.layer.get_bdd( tab.layer )
			tab.nb_current		= param.get_ref( tab.bdd, "nb_current" )
			tab.restart_trig	= param.get_ref( tab.bdd, "restart_trig" )

	ref.part = {}
	tab = ref.part
	tab.layer = self:get_layer(3)
		tab.bdd = aaa.layer.get_bdd( tab.layer )
			tab.nb_current					= param.get_ref( tab.bdd, "nb_living_now"				)
			tab.nb_by_sec					= param.get_ref( tab.bdd, "nb_by_sec"					)
			tab.restart_trig				= param.get_ref( tab.bdd, "restart_trig"				)
			tab.angle						= param.get_ref( tab.bdd, "jitter_cone_angle"			)
			tab.speed						= param.get_ref( tab.bdd, "emission_speed_factor"		)
			tab.emission_box_size_factor	= param.get_ref( tab.bdd, "emission_box_size_factor"	)

	ref.layer_attr_bulle = self:get_layer(4)
end

function meu:load_snds()
	--self:box_debug( "start sound load" )
	--MEDIA.set_dir_media( "Aqualoyal/Sound/Bulle" )
	local dir = app.media_dir_rel.."Aqualoyal/Sound/"
	local player = SNDS_PLAYER:create("Bulle")

	player:add( "back",		1, dir.."Bulle/Background",		"mp3"			)
	player:add( "pop",		6, dir.."Bulle/pop_",			"mp3",	true	)
	player:add( "second",	2, dir.."Second_",				"mp3",	true	)
	player:add( "go",		1, dir.."Go",					"mp3"			)
	player:add( "pre_l1",	1, dir.."Bulles_niveau1",		"mp3"			)
	player:add( "pre_l2",	1, dir.."Bulles_niveau2",		"mp3"			)
	player:add( "pre_l3",	1, dir.."Bulles_niveau3",		"mp3"			)
	player:add( "win_1",	1, dir.."Bulles_rouges_gagne",	"mp3"			)
	player:add( "win_2",	1, dir.."Bulles_jaune_gagne",	"mp3"			)

	return player
end

function meu:init()
	self.__b_ui_defined = false

	--self:box_debug( "ToTo" )
	self:init_ref()
	self.b_boid = false -- we use particle now
	if not self.snds then -- we load only once
		self.snds = self:load_snds()
	end
	self.bulles = {}
	local bulles = self.bulles
	for i = 1, 4096 do
		bulles[i] = BULLE:create(i)
	end
	if not self.pixar then
		self.pixar = PIXAR:create( "Bulle" )
		self.pixar:set_light( false )
	end

	self.defs_bulle =	{
							{	name = "Bulle_rgba",			kind = "bulle",		points = 5,		},
							{	name = "Bulle_jeu",				kind = "bulle",		points = 5,		trigger="bulle"					},
							{	name = "Bulle_plancton",		kind = "bulle",		points = 5,		trigger="plancton"				},
							{	name = "Bulle_story",			kind = "bulle",		points = 5,		trigger="story"					},
							{	name = "bouteille-plastique",	kind = "direct",	points = 30,	size_force = .25,	rot = 180,	},
							{	name = "canette",				kind = "direct",	points = 30,	size_force = .10,	rot = 30,	},
							{	name = "lessive",				kind = "direct",	points = 50,	size_force = .3,	rot = 180,	},
							{	name = "pneu",					kind = "direct",	points = 100,	size_force = .6,				},
							{	name = "sac-plastique-1",		kind = "direct",	points = 20,	size_force = .22,	rot = 180,	},
							{	name = "sac-plastique-2",		kind = "direct",	points = 20,	size_force = .24,	rot = 180,	},
							{	name = "tong",					kind = "direct",	points = 40,	size_force = .25,	rot = 30,	},
						}
	local defs = self.defs_bulle
	MEDIA.set_dir_media( "Aqualoyal/Bulles" )
	local trigger_nb = 0
	for i=1,#defs do
		local def = defs[i]
		def.img		= IMGS.get_img_with_error_dialog( def.name..".tga" )
		if def.kind == "direct" then
			local sx,sy = def.img:get_sxy()
			if sx < sy then
				sx = sx / sy * def.size_force
				sy = def.size_force
			else
				sy = sy / sx * def.size_force
				sx = def.size_force
			end
			def.sx, def.sy = sx, sy
		end
		if def.trigger then
			trigger_nb = trigger_nb + 1
		end
	end
	self.trigger_nb = trigger_nb

	MEDIA.set_dir_media( "Aqualoyal" )

	self.text = TEXT:create( "Loyal_rules" )

	self:restart()
	if not self.seqs then -- we load only once, we have a redefine button specific foor this
		self:define_seqs()
	end
end

if CLASS.DECLARE( "BULLE" ) then
end

function BULLE:create( id )
	local self = BULLE:create_instance( "bulle" .. id )
	self:init( false )
	return self
end
function BULLE:pick_size( min, max, gain, bias )
	local def = self.def
	local size = def.size_force
	if size then
		self.radius = size * .5
	else
		local rnd = math.random()
		rnd = aaa.math.gain( rnd, gain )
		rnd = aaa.math.bias( rnd, bias )
		self.rnd = rnd
		local r = interpolate( min or .01, max or .1, rnd )
		self.radius = r
	end
end
function BULLE:init( b_play )
	self.b_free		= not b_play
	self.b_play		= b_play
	self.b_draw 	= b_play
	self.explode_t	= nil
	self.team		= nil
	self.alpha		= nil
end

function meu:set_seq( name )	self.seqs:set_seq( name )	end

function meu:do_logic_select( select )
	self:print( "do_logic_select "..select )
	local logic = self:get_meu_by_name( "TankCams_1" )
	if logic then logic:set_seq( select ) end
end

function meu:draw_before_bulles()
	gol.set_line_width(4.)
	gol.set_quad_uv( 0, 0, 1, 1 )
	if self.b_debug then
	else
		gol.set_texture_dim(2)
	end
end

function meu:draw_after_bulles()
	if self.b_debug then
	else
		gol.set_texture_dim(0)
	end
end

function meu:draw_bulle( bul, color, r )
	local alpha = bul.alpha or 1
	if not color or bul.explode_t then
		local team = bul.team
		if not team then
			if bul.b_select then
				color = { .5, .5, .5, alpha }
			else
				color = { 1, 1, 1, alpha }
			end
		else
			local c = self.teams[team].color
			color = { c[1], c[2], c[3], c[4] * alpha }
		end
	end

	r = r or bul.radius
	if bul.b_select then
		local v = aaa.math.get_fractalsum( 0, 0, aaa.time.t + bul.id*.235, .5, 3 )
		r = r * interpolate( .7, 1, v )
	end
	local x, y, z = bul.x, bul.y, bul.z
	if self.b_debug then
		gol.color( color[1], color[2], color[3] )
		if bul.explode_t then
			local s = .2
			aaa.draw_str_xyz( "+" .. bul.score, x - s, y - s, z, s, s )
		else
			aaa.draw_circle_axe_z( x, y, z, r * 2, 16 )
		end
	else
		if bul.explode_t then
			--aaa.draw_str_xyz
			gol.set_texture_dim(0)
			color[4] = color[4] * (bul.explode_t * 3 - 1)
			self:draw_text_pixar( "+" .. bul.score, x, y, color, 10 )
			gol.set_texture_dim(2)
		else
			local def = bul.def
			local img = def.img
			if def.kind == "bulle" then
				-- color background
				gol.color( color[1], color[2], color[3], .4*alpha )
				gol.bind_texture( 106 )
				aaa.draw_rect_uv_at_z(	x-r, y-r,	x+r, y+r,	z	)
				-- white mask in top
				gol.color_white( alpha )
				gol.bind_texture( img:get_bind() )
				aaa.draw_rect_uv_at_z(	x-r, y-r,	x+r, y+r,	z	)

			elseif def.kind == "direct" then
				gol.color_white( 1.*alpha )
				local sx,sy = def.sx, def.sy
				local rot = bul.rot
				if rot then
					rot = rot + bul.rot_speed * aaa.time.dt
					bul.rot = rot
					gol.bind_texture( img:get_bind() )
					aaa.draw_rect_uv_axe_z(	x,y,z,	sx,sy,	rot	)
				else
					img:draw_xy_sxy( x, y, sx, sy )
				end
			end
		end
	end
end

function meu:draw_points()
	local ref = self.ref
	--	local nb = param.get( ref.nb_current )

	aaa.bdd.set_lua_cur( ref.points.bdd )
	local nb = aaa.bdd.get_point_nb()
	aaa.show( nb, "nb dd" )
	if nb <= 0 then
		return
	end

	self:draw_before_bulles()

	local function kill_bulle( bul )
		poid.kill_by_id( bul.id )
		bul:init( false )
	end

	local function set_bulle( bul, team )
		bul:init( true )
		bul.team	= team
		local defs = self.defs_bulle
		local proba = self.dechet_proba
		local pick
		math.randomseed( aaa.time.t )
		if self.b_select then
			pick = (bul.x < 0 and self.__pick_last_left or self.__pick_last_right) or 0
			pick = pick + 1
			if pick >= self.trigger_nb then pick = 0 end
			if bul.x < 0 then	self.__pick_last_left = pick
			else				self.__pick_last_right = pick
			end
			pick = pick + 2
		elseif proba<=0 or proba<math.random() then
			pick = 1
		else
			local offset = self.trigger_nb + 1
			pick = math.random( #defs - offset ) + offset
			if pick >= 2 then
				bul.rot = math.random()
				bul.rot_speed = ( math.random() - .5 ) * .2
			end
		end
		bul.def = defs[pick]
		bul:pick_size( meu.bul_min, meu.bul_max, meu.bul_size_gain, meu.bul_size_bias )
	end

	local do_logic
	local bulles = self.bulles
	local get_point = aaa.bdd.get_point_and_id_local
	local clicks = TANK.cur.__meu_tank_master.__clicks
	local nb_click = clicks and #clicks or 0
	aaa.show( nb_click, "click_nb_game" )
	local top = {}
	local sel_min_x = 2
	-- deal with all the bulles first
	for i = 1, nb do
		local x, y, z, id = get_point(i)
		--self:print(id)
		local bul = bulles[id]
		if bul then
			--aaa.draw_str_maa_xyz( tostring(id), x, y, z, .08, .08 )
			bul.x = x
			bul.y = y
			bul.z = z
			bul.id = id
			--self:print( "-------------"..id.." -> "..x.." "..y.." "..z )
			if y < -.3 then
				if self.b_fade then
					kill_bulle( bul )
				--self:print( "test init -------------"..id )
				elseif not bul.b_free then
					--self:print( "init -------------"..id )
					bul:init( false )
				end
			elseif 3 < y or x < -3 or 3 < x then	-- on top or left or right
				--	self:print( "kill "..id )
				-- remove bulles too out to come back, this is just an optimization
				kill_bulle( bul )
			else
				--self:print( " "..id.." -> "..x.." "..y.." "..z )
				local SC = .15
				if bul.b_free then
					if self.b_fade then
						kill_bulle( bul )
					else
						--self:print( "init "..bul )
						if self.b_game then
							if x < -SC then		set_bulle( bul, 1 )
							elseif SC < x then	set_bulle( bul, 2 )
							else				kill_bulle( bul )
							end
						else
							if self.b_select and -sel_min_x<x and x<sel_min_x then
								kill_bulle( bul )
							else
								set_bulle( bul, nil )
								bul.b_select = self.b_select
							end
						end
					end
				end
			end

			--if self.b_game and bul.b_play then
			if bul.b_play then
				if nb_click > 0 then
					local r2 = bul.radius + self.click_size
					r2 = r2 * r2
					local team = bul.team
					for ic = 1, nb_click do
						local click = clicks[ic]
						if click.b_test_contact then
							--self:print( "click "..click.x)
							local xc, yc = click.x - x, click.y - y
							local d2 = xc * xc + yc * yc
							if d2 < r2 then
								local pan = x / 16
								--self:print( pan )
								self:play_snd( "pop", false, pan )
								if team then
									bul.b_play = false
									if click.score_by_team[team] == 0 then
										--first click by team, bulle will carry fx (+ score for now)
										click.bulle_by_team[team] = bul
										bul.score = 0
										bul.explode_t = 1
									else
										--second or more click by team we "kill" completly the bulle (no play no draw)
										kill_bulle( bul )
									end
									local inc = bul.def.points
									self.teams[team].score = self.teams[team].score + inc
									click.bulle_by_team[team].score	= click.bulle_by_team[team].score	+ inc
									click.score_by_team[team]		= click.score_by_team[team]			+ inc
								elseif self.b_select and bul.b_select then
									kill_bulle( bul )
									do_logic = bul.def.trigger
								else
									kill_bulle( bul )
								end
							end
						end
					end
				end
			end

			if bul.b_draw then
				if bul.explode_t then
					top[#top + 1] = bul
				else
					local alpha = bul.alpha
					if alpha or self.b_fade then
						alpha = (alpha or 1.) - aaa.time.dt * self.fade_speed
						if alpha <= 0 then
							kill_bulle( bul )
						else
							bul.alpha = alpha
							bul.b_play = false
							self:draw_bulle( bul )
						end
					else
						self:draw_bulle( bul )
					end
				end
			elseif self.b_debug then
				gol.color_white()
				aaa.draw_null( x, y, z, .1 )
			end
			--aaa.draw_null( x, y, z, .1 )
		end
	end

	-- deal with what on top (score for now)
	for i = 1, #top do
		local bul = top[i]
		local explo = bul.explode_t
		local r = bul.radius
		local c = bul.color
		explo = explo - aaa.time.dt * .3333
		bul.explode_t = explo
		if explo < 0 then
			kill_bulle( bul )
		else
			self:draw_bulle( bul )
		end
	end

	self.b_fade = false

	--have to be done after fade because it can trigger fade
	if do_logic then
		self:do_logic_select( do_logic )
	end
	self:draw_after_bulles()
end

function meu:define_seq_ui(bus)
	self.bu_seq = self:add_bu_sequence( bus, self.seqs )
	self:set_seq( "select" )
end

function meu:define_ui()
	local ref = self.ref
	local bu

	self.__b_ui_defined = false

	self:define_seq_ui( bus_cur )

	self:set_tab_key( "Main" )

	local ix = 9
	local iy = 2

	self:add_camera()

	--self:set_tab_key(1)
		self:add_button(	{	ix, iy },				"Draw Debug",	self, "b_debug", 			false)
		--iy = iy + 1
		self:add_trig_restart(	{ix + 5,iy}			)
		--self:add_button( {		ix+4, iy },			"Boid",			self, "b_boid", false )

		iy = iy + 1
		self:add_trig_method(	{ix, iy}, 			"Fade",			self, "trig_fade"			)
		iy = iy + 1
		self:add_slider(	{ix,iy,	8,1},	"Click size",	self, "click_size",			0.02,		0.02, .5		)

	--self:set_tab_key(2)
		iy = iy + 1.5
		self:add_slider(	{ix,iy,	8,1},	"Dechet",		self, "dechet_proba",		0., 		0., 1.			)

		iy = iy + 1.2
		self:add_slider(	{ix,iy,	8,1},	"Nb_sec",		ref.part.nb_by_sec,	nil,	0,			0, 1024.		)
		iy = iy + 1
		self:add_slider(	{ix,iy,	4,1},	"Speed",		ref.part.speed, nil,		.1,			.1, 2.			)
		self:add_slider(	{ix + 4,iy,	4,1},	"Angle",		ref.part.angle, nil,		10,			.0, 60			)
		iy = iy + 1
		self:add_slider_two(	{ix,iy,	8,1},	"Size",			self, "bul_min", "bul_max", .03, .06,	.01, .3			)
		iy = iy + 1
		self:add_slider(	{ix,iy,	4,1},	"Gain",			self, "bul_size_gain",		.5,			.00001, .99999	)
		self:add_slider(	{ix + 4,iy,	4,1},	"Bias",			self, "bul_size_bias",		.5,			.00001, .99999	)
		iy = iy + 1
		--self:add_slider(	{ix,iy,	4,1}, 	"Angle", 		self, "bul_size_gain",		.5,			.00001, .99999	)

	--self:set_tab_key(3)
		iy = iy + 1
		self:add_button(	{	ix, iy },				"Volume_on",	self, "b_snd_on",			true ):set_text_visible(false)
		self:add_slider(	{ix + 1,iy,	7,1},	"Volune",		self, "volume"				)
	--	iy = iy + 1
	--	self:add_slider(	{ix,iy,	4,1},	"Vol_Back",		self, "volume_back"			)
	--	self:add_slider(	{ix + 4,iy,	4,1},	"Vol_Pop",		self, "volume_pop"			)

	self.__b_ui_defined = true

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

function meu:play_snd( name, b_loop, x )
	if self.b_snd_on then
		if self.vols then
			local volume = self.vols[name] * self.volume
			self.snds:play( name, b_loop, volume, -(x or 0) / 5 )
		end
	end
end

function meu:choose_points_src()
	local ref = self.ref
	ref.points = self.b_boid and ref.boid or ref.part
end

function meu:restart()
	self:choose_points_src()
	local ref = self.ref
	param.set( ref.points.restart_trig, true )

	if self.bulles then
		table.apply_method( self.bulles, "init" )
	end

	self.teams = {
		{name = "Les Rouges", color = {1, 0, 0, 1}, score = 0},
		{name = "Les Jaunes", color = {1, 1, 0, 1}, score = 0}
	}
end

function meu:trig_fade()
	self:print_inverse( "trig fade" )
	self.b_fade = true
	self.fade_speed = 4.
end

function meu:update()
	self:set_ui_slot_at_start( 1 )

	--	this way the seqs can be initialized after the GP
	local seqs = self:check_need_define_seq_ui( self.seqs )
	seqs:updraw( true )
	local seq_id = seqs:get_seq_cur_id()
	--self.b_game = table.find_key_by_val( {4,7,10}, seq_id ) ~= nil

	self:choose_points_src()

	--aaa.show( self.b_snd_on, "vol ")
	self.snds:update()
end

function meu:draw_text( str, x,y, color, size, b_shadow )
	local text = self.text
	text:set_font_texture(28)
	text:set_font_type( "font_texture" )
	text:set_alignment( "center" )
--self:print( x.."/"..y )
	if b_shadow then
		gol.color_black()
		text:draw( str, x-size/22, y-size/22, size, size )
	end
	gol.color( color )
	text:draw( str, x,y, size,size )
end

function meu:draw_text_pixar( text, x,y, color, line_by_unit )
	local pixar = self.pixar
	local O = math.min( .015, .006 * 4 / line_by_unit )
	pixar:clear_terminal()
	pixar:prepare_draw( line_by_unit )
	pixar:print_terminal( 0, 0, text )

	gol.color_black( color[4] )
	pixar:set_pos_terminal( x - O, y - O )
	pixar:draw_text()

	gol.color( color )
	pixar:set_pos_terminal( x,y )
	pixar:draw_text()
end

function meu:draw()
	local ui = self.ui
	local seqs = self.seqs
	local seq = seqs:get_seq_cur()

	self:draw_layers_begin()
		self:draw_layer(1) --attr
		self:draw_layer( self.b_boid and 2 or 3 ) -- 2-boid 3=particle
		self:draw_layer(4) --attr_bulle
		gol.reset()
		self:draw_points()

	local title = seq:get_title()
	local screen = TANK.cur and TANK.cur.__meu_tank_master and TANK.cur.__meu_tank_master.screen
	if screen then
		--self:print( seq:get_title() )
		if not (title=="wait") and not (title=="select") then
			local teams = self.teams
			local function hack_draw_pixel( self, id, x, y, sx, sy, csx, csy )
				--gol.set_quad_uv( u, v, u+osu, v+osv )
				--aaa.draw_rect_uv( x-sx, y-sy, x+sx, y+sy )
				--aaa.draw_rect_uv_at_z( x-sx, y-sy, x+sx, y+sy, (1 - math.cos(ph)) * 0 )
				--aaa.draw_disk_axe_z( x*1.2, y, 0, sx*.2+y*.08 )	--, 12 )
				--self:print( csx.." "..csy )
				aaa.draw_disk_axe_z( x, y, 0, sx * (1. + .4 * csy) )
			end

			local pixar = self.pixar
			pixar:set_function_draw_pixel( hack_draw_pixel )
			pixar:set_rgba( 1,1,1, 1 )

			local y_top = screen.t - .4
			for i = 1, 2 do
				local team = teams[i]
				self:draw_text_pixar( string.format( "%04d", team.score ),	i == 1 and -1.1 or .65,	y_top, team.color, 3 )
				self:draw_text_pixar( team.name,							i == 1 and -1.1 or .65, y_top+.1, team.color, 8 )
			end
			--gol.pop_matrix()

			gol.set_line_width( 6. )
			--todo redo on top and separate
			--self:updraw_clicks()

			if ui.bu_cam_show:get_value_as_bool() then
				gol.color_yellow()
				aaa.draw_rect_line( screen.l, screen.b, screen.r, screen.t )
			end

			if self.b_game and seq then
				local SH = .01
				local bot, top = screen.b + .25, screen.t - .1
				gol.color_white(.5)
				aaa.draw_rect_line( -SH, bot, SH, top )
				gol.color_white(1.)
				bot = interpolate( bot, top, seq:get_phase() )
				aaa.draw_rect( -SH, bot, SH, top )
			end

			if self.b_decompte then
				self:draw_text_pixar( self.decompte,	-0.15, 1.8, {1,1,1,1}, 1 )
				if self.level_info then
					for i=1,2 do
						self:draw_text( self.level_info.str, self.level_info.x[i]*1.4, 1.5, teams[i].color, .125, true )
					end
				end
			end
			if self.str_rule then
				self:draw_text( self.str_rule,	0, 1.6, {0,0,0,1}, .125, false )
			end
			if self.b_winner then
				for i=1,2 do
					local b_win =  teams[i].score >= teams[3-i].score
					self:draw_text_pixar( b_win and "BRAVO" or "PERDU",	i == 1 and -1.6 or .45, 1.5, teams[i].color, 2 )
				end
			end
		end
	end

	self:draw_layers_end()
end
