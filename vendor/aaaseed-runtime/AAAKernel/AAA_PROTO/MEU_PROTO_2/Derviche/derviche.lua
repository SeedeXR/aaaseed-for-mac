function meu:define_meu_infos( )
	return { author = "Hugo VERLINDE and Mâa BERRIET", date="1984-", version="2",
				tags = { "3D", "Art", "Draw", "Point", "Procedural" },
				help = "Please this is intended to replace drugs and start meditation"
			}
end

local	SY	= 1/16
local	SX = 1/24

function meu:define_ui()
	local ref = self.ref
	local sha = self:get_shading()
	local ui = self.ui
	local bu
	local par
	local ix = 1
	local iy = 1
	local sy = 1.
	local dy = .3
	local dyb = .5
	local sx
	ix,iy = 9, 2.5
	self:add_camera( {9,1}, 16 )

	ix,iy = 1, 1
	bu = self:add_button(		{ix,iy, 	sy, sy },	"Draw",		self, "b_draw",		true	):set_text_visible( false )

	bu = self:add_text_info(	{ix+1,iy,	1.5,sy},	"To load" )
		bu:set_text_color( "red" )
		--bu:set_visible( false )
		ui.bu_state = bu

	self:add_trig_method(		{ix+2.5,iy,	3.5, sy},	"Edit",		sha, "edit_shader_vert" )
	self:add_trig_method(		{ix+6,iy,	2, sy},		"Focus",	sha, "set_focus" )

	iy = iy + sy + dyb

	bu = self:add_selector(		{ix,iy,		8,sy*1},	"Version" )
		bu:set_nb( 16,1 )
		bu:set_target_param( sha:get_ref_vert_int(1) )
		bu:set_item_text_from_nb( )

	iy = 2.5
	ix = 9
	local SX = 2
	self:add_trig_method(		{ix,iy,		SX, sy},	"Restart"	,self, "restart"		)
	self:add_trig_method(		{ix+SX,iy,	SX, sy},	"ALL"		,self, "restart_all"	):set_color_back("restart")
	bu = self:add_trig_method(	{ix+SX*2,iy,SX, sy},	"REC"		,self, "flip_record"	)
		ui.bu_record = bu
		self:__set_record_button( false )
	bu = self:add_text_info(	{ix+SX*3,iy,SX,sy},		"save_info"	 )
		ui.bu_info_save = bu
		bu:set_text( "" )

	self:set_tab_key_def()

	ui.bu_infos = {}
	local function add_phase_info( ix, iy, sy, tab, id )
		bu = self:add_text_info({ix,iy,		4,sy},		"None_"..id )
			bu.__info_src = tab
			ui.bu_infos[id] = bu
	end

	ix, iy = 1, 4
	bu = self:add_button(		{ix,iy,		sy,sy },	"Play",			self, "b_play",		true	):set_text_visible( false )
	bu = self:add_selector(		{ix+sy,iy,	8-sy,sy},	"time_factor"	):set_nb(7)
		bu:set_item_text( 1, "/1000", "/100", "/10", nil, "x10", "x100", "x1000" )
		bu:set_item_data( 1, .001, .01, .1, 1, 10, 100, 1000 )
		self.bu_factor = bu

	iy = iy + sy
	sx = 1.5
	bu = self:add_button(		{ix, iy,	sx, sy },	"speed_sign",	self, "speed_sign"	):set_multiple( { "-", "+"} )
	bu = self:add_slider(		{ix+sx,iy,	8-sx,sy},	"Speed",		self, "speed",			1,		0, 1 )
	bu = self:add_slider(		{ix+4,iy+sy,4,sy},		"Speed_Start",	self,	"phase_start",	0,		0, 1 ):set_text( "Start" )
	add_phase_info( ix, iy+sy, sy, self, 1 )
		--bu:set_meter_ref( 0 )
	iy = iy + sy*2 + dyb

	sx = 8 / 3
	for i=1,2 do
		local base = (i-1)*3
		for j=0,2 do
			local name = "v"..(base+j+1)
			bu = self:add_slider({ix+j*sx,iy,sx,sy},	name,			sha:get_ref_vert_float(base+j+2), nil,	0,		0, 1 )
		end
		iy = iy + sy
	end

	ix = 9
	iy = 4.5

	sx = 3.5
	bu = self:add_selector(		{ix,iy,		sx,2*sy},	"Primitive" )
		bu:set_nb( 1, 3 )
		bu:set_target_lua( self, "s_draw_mode" )
		bu:set_item_text( 1, "Point", "Both", "Line" )

	bu = self:add_slider(		{ix+sx,iy,	8-sx,sy},	"Point size",	ref.rendering,	"point_size",	1,	0, 32 )
	bu = self:add_slider(		{ix+sx,iy+sy,8-sx,sy},	"Line size",	ref.rendering,	"line_size",	1,	0, 32 )
	iy = iy + 2 * sy

	bu = self:add_button(		{ix,iy, 	sy,sy},		"Depth",		self,			"b_depth",		true	)
	iy = iy + sy + dy

	local sx = 3
	local function add_ui( ix, iy, pre, pre_par, offset, max, values_def )
		local bu
		bu = self:add_slider(	{ix,iy,		4,sy},		pre,			self, pre_par.."_nb",		10,	0, max )
			if values_def then
				bu:add_values_def( values_def )
			end
		iy = iy + sy
		bu = self:add_button(	{ix+offset,iy,	2,sy},	pre.."_distrib",self, pre_par.."_distrib"	)
			bu:set_multiple( { "Start", "Uniform" } )
		bu = self:add_text_info({ix+2-offset,iy,2,sy},	pre.."info"		)
		ui["bu_info_"..pre_par] = bu
		iy = iy + sy
		bu = self:add_slider(	{ix,iy,		4,sy},		pre.."_Used",	self, pre_par.."_used",		1,		0, 1 )
	end
	add_ui(	ix,		iy, "Segment",	"seg",	0,	16, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }	)	--todo make a fn for def value are int values
	add_ui(	ix+4,	iy, "Point",	"pt",	2,	10,	{ 1, 2, 3, 4, 5, 6, 7, 8, 9 }	)
	iy = iy + 3*sy
	--bu = self:add_slider(	{ix,iy,	8,sy},			"Phase",		self, "phase",		1,		0, 1 )
	bu = self:add_slider_two(	{ix,iy,		8,sy},		"Phase", 		self, "phase_min", "phase_max",	0, 1, 0, 1 )


	self:set_tab_key( "Color" )

	ix,iy = 1, 4.5
	bu = self:add_button(		{ix,iy, 	2,sy},		"Trail Use",	self,	"b_trail_use",	true	):set_text( "trail" )
	iy = iy + sy
	local t = { self:add_button({ix,iy, 	sy,sy},		"Trail Active",	self,	"b_trail",		true	):set_text_visible( false ) }
	t[2] 	= self:add_slider(	{ix+1,iy,	7,sy},		"Trail Time",	self,	"trail_time",	1,		0,16 )
	ui.trail = t
	iy = iy + sy
--		self:add_rgbfa(			{ix,iy,		nil,sy},	"Back_Color" )
--	iy = iy + sy

	ix,iy = 9,4.5
	bu = self:add_button(		{ix,iy,		sy,sy},		"ColorMap",		self, "b_texture",	false	):set_text_rect_ratio(3)
	bu = self:add_selector(		{ix+4,iy,	4,sy},		"texture_id"	):set_nb(8):set_text_visible( false )
		bu:set_item_text_from_nb()
		bu:set_target_lua( self, "texture_id" )
	iy = iy + sy
	bu = self:add_slider(		{ix+1,iy,	3,sy},		"texture_size",	self, "texture_size",	1,	0, 16 ):set_text("Size")
	iy = iy + sy
	local t = {}
	for i = 1,4 do
		--todorgb
		t[i] = self:add_rgbfa(	{ix,iy,		nil,sy},	"Color"..i, false )
		iy = iy + sy
	end
	ui.bu_col = t
	iy = iy + dy
	self:add_rgbf(				{ix,iy,		nil,sy},	"Color_Mul" )
	iy = iy + sy



	self:set_tab_key( "Trans" )

	ix,iy = 1, 4

	bu = self:add_button(		{ix,iy, 	sy,sy},		"2D",			self,	"b_2d",			false	):set_text_rect_ratio(2)
	bu = self:add_slider(		{ix+3,iy,	5,sy},		"2D_dist",		self,	"dist_2d",		4,	0,  16	):set_color_back("z")
	bu = self:add_slider(		{ix+1,iy+1,	7,sy},		"2D_size",		self,	"size_2d",		1,	0,  8	)
	iy = iy + sy * 3.5

	bu = self:add_button(		{ix,iy, 	sy,sy},		"Transfo",		self,	"b_transfo",	true	)
	iy = iy + sy * 1

	sx = 5/3
	local sy = .8
	local function add_time_ui( ix,iy,		sx,		pre, id )
		local tab = { t=0 }
		bu = self:add_button(	{ix,iy, 	sy,sy},		pre.."_active",	tab,	"b_play",		false	):set_text_visible( false )
		bu = self:add_selector(	{ix+4,iy,	4,sy},		pre.."_time_factor"	):set_nb(5)
			bu:set_item_text( 1, "/100", "/10", nil, "x10", "x100" )
			bu:set_item_data( 1, .01, .1, 1, 10, 100 )
			bu:set_text_visible( false )
			tab.bu_factor = bu
		bu = self:add_slider(	{ix+8-sx,iy+sy,	sx,sy},	pre.."_Speed",	tab,	"speed",		0,	0, 1 ):set_text( "Speed" )
		bu = self:add_slider(	{ix+4,iy+sy*2,	4,sy},	pre.."_Start",	tab,	"phase_start",	0,	0, 1 ):set_text( "Start" )
		add_phase_info( ix, iy+sy*2, sy, tab, id )
		return tab
	end
	local ox = 0

	local frustum_def = { -1.5, -1, -.5, 0, .5, 1, 1.5 }
	local f_x = add_time_ui(  ix, iy, 8-sx*3, "X", 6 )
	bu = self:add_button(		{ix,iy, 	sy,sy},		"Frustum X active",	f_x,	"b_play",		false	):set_text_visible( false )
	bu = self:add_slider(		{ix+sy,iy,	4-sy,sy},	"Frustum X Power",	f_x,	"power",		0, 0, 1 ):set_text( "Power" )
	bu = self:add_slider_two(	{ix,iy+sy,	5,sy},		"Frustum X", 		f_x,	"tra_min", "tra_max",	-.5, .5, -2, 2 ):set_color_back("x")
		bu:add_values_def( frustum_def )
	self.frustum_x = f_x
	iy = iy + sy*3 + dy

	local f_y = add_time_ui(  ix, iy, 8-sx*3, "Y", 7 )
	bu = self:add_button(		{ix,iy, 	sy,sy},		"Frustum Y active",	f_y,	"b_play",		false	):set_text_visible( false )
	bu = self:add_slider(		{ix+sy,iy,	4-sy,sy},	"Frustum Y Power",	f_y,	"power",		0, 0, 1 ):set_text( "Power" )
	bu = self:add_slider_two(	{ix,iy+sy,	5,sy},		"Frustum Y", 		f_y,	"tra_min", "tra_max",	-.5, .5, -2, 2 ):set_color_back("y")
		bu:add_values_def( frustum_def )
	self.frustum_y = f_y
	iy = iy + sy*3 + dy

	ix,iy = 9, 4
	self.rots = {}
	for i = 1,2 do
		local pre = "Rotation_"..i
		local rot = add_time_ui(  ix, iy, 8-sx*3, pre, i+1 )
		self.rots[i] = rot
		bu = self:add_selector(	{ix+sy,iy,	4-sy,sy},		pre	):set_nb(3)
			bu:set_item_text( 1, "X", "Y", "Z" )
			bu:set_target_lua( rot, "axe" )
		bu = self:add_slider(	{ix+ox,iy+sy,		sx,sy},		pre.."_Center_X",	rot,	"cen_x",	0,	-32, 32 ):set_color_back("x"):set_text( "X" )
		bu = self:add_slider(	{ix+ox+sx,iy+sy,	sx,sy},		pre.."_Center_Y",	rot,	"cen_y",	0,	-32, 32 ):set_color_back("y"):set_text( "Y" )
		bu = self:add_slider(	{ix+ox+sx*2,iy+sy,	sx,sy},		pre.."_Center_Z",	rot,	"cen_z",	0,	-32, 32 ):set_color_back("z"):set_text( "Z" )
		iy = iy + sy*3 + dy
	end

	local osc = add_time_ui(  ix, iy, 8-sx*3, "osc", 4 )
	self.osc = osc
	bu = self:add_button(		{ix,iy, 	sy,sy},		"Osc active",		osc,	"b_play",		false	):set_text_visible( false )
	bu = self:add_slider(		{ix+sy,iy,	4-sy,sy},	"Osc Power",		osc,	"power",		0,	0, 1 ):set_text( "Power" )
	bu = self:add_slider_two(	{ix,iy+sy,	5,sy},		"Angle", 			osc,	"angle_min", "angle_max",	-45, 45, -45, 45 ):set_color_back("x")
		bu:add_values_def( -30, -15, 0, 15, 30			 )
	iy = iy + sy*3 + dy

	local tra = add_time_ui(  ix, iy, 8-sx*3, "tra", 5 )
	self.tra = tra
	--table.print( self.tra, "self.tra", 2 )
	--self:box_debug( "self.tra" )
	bu = self:add_button(		{ix,iy, 	sy, sy },	"Tra active",		tra,	"b_play",		false	):set_text_visible( false )
	bu = self:add_slider(		{ix+sy,iy,	4-sy,sy},	"Tra Power",		tra,	"power",		0, 0, 1 ):set_text( "Power" )
	bu = self:add_slider_two(	{ix,iy+sy,	5,sy},		"Tra Distance", 	tra,	"tra_min", "tra_max",	0, 1, -10, 10 ):set_color_back("z")
	iy = iy + sy*3 + dy

	self:add_transfo_tab( {9,4,	8,3.2} )
end

function meu:init()
	local ref = self.ref

	self:add_shading()
	ref.primitive	= param.get_ref( ref.rendering, "primitive" )
	ref.depth		= param.get_ref( ref.rendering, "depth" )
	ref.depth_write	= param.get_ref( ref.rendering, "depth_write" )

	local bdd = self:get_layer_bdd( 1 )
	ref.nb_v		= param.get_ref( bdd, "nb_v" )
	ref.nb_axe		= param.get_ref( bdd, "nb_axe" )
	self.t = 0

	local layer = self:get_layer( 1 )
	param.set( layer, "use_mapping",	"CURRENT" )
	param.set( layer, "bank_2d",		"CURRENT" )
	param.set( layer, "bind_2d",		"CURRENT" )

	local app = aaa.obj.get_from_top_by_class( "app" )
	ref.time_step		= param.get_ref( app, "time_step"			)
	ref.time_real_time	= param.get_ref( app, "time_real_time"	)

	self.b_record = false
end

function meu:restart_tab_time( tab )
	tab.t = 0
	tab.t_out = 0
end
function meu:restart()
	self:restart_tab_time( self )
	self:restart_tab_time( self.tra )
	self:restart_tab_time( self.osc )
	self:restart_tab_time( self.rots[1] )
	self:restart_tab_time( self.rots[2] )
	self:restart_tab_time( self.frustum_x )
	self:restart_tab_time( self.frustum_y )
end
function meu:restart_all()
	self:apply_instances( "restart" )
end
function meu:__set_record_button( b_record )
	local bu = self.ui.bu_record
	if b_record then
		bu:set_text( "Stop" )
		bu:set_color_back( "red_full" )
	else
		bu:set_text( "REC" )
		bu:set_color_back( "save" )
	end
end
function meu:flip_record()
	local ref = self.ref
	local b = self.b_record
	local meu_pre = self:get_meu_pre()
	--meu_pre:print( "This is meu_pre" )
	if b then
		b = not b
		param.set( ref.time_step,		self.time_step_last			)
		param.set( ref.time_real_time,	self.time_real_time_last	)
	else
		local folder = aaa.file.do_dialog_folder( "Folder for images" )
		if folder then
			self.time_step_last			= param.get( ref.time_step			)
			self.time_real_time_last	= param.get( ref.time_real_time	)
			param.set( ref.time_step, .03333333333333333	)
			param.set( ref.time_real_time, false )
			self:print( "images will be saved in ", folder )
			meu_pre:set_save_dir( folder )
			b = not b
			self:restart_all()
		end
	end
	self:__set_record_button( b )
	meu_pre:set_bu_value( "fbo.save", b )
	self.b_record = b
end

function meu:update_time( tab )
	tab = tab or self
	local t = tab.t
	--aaa.show( self.speed_sign, "speed_sign" )
	if tab.b_play or tab.b_play == nil then
		local t = tab.t
		local bu = tab.bu_factor
		local v = bu:get_item_data()
		--self:print( math.floor(bu:get_value()).." v is "..v )
		if tab.speed_sign and tab.speed_sign==1 then
			v = -v
		end
		t = t + aaa.time.dt * tab.speed * v
		tab.t = t
	end
	return t
end

function meu:get_meu_pre()
	local meu_pre = self.meu_pre
	if meu_pre then return meu_pre end

	meu_pre = app:get_meu_by_name( "fbo_pre" )
	self.meu_pre = meu_pre
	return meu_pre
end

function meu:update_ui()
	local ui	= self.ui
	local sha	= self:get_shading()

	for i=1,7 do
		local bu = self.ui.bu_infos[i]
		if bu then
			local tab = bu.__info_src
			if tab.t_out then
				local str = string.format( "%.5f", tab.t_out )
				bu:set_text( str )
			end
		end
	end

	local pre = self:get_meu_pre()
	if pre.get_save_image_count then
		ui.bu_info_save:set_text( string.format( "%d", pre:get_save_image_count() ) )
	end

	ui.bu_state:set_visible( not sha:is_valid() )
	ui.bu_state:set_text( "Error" )


	local b_use = self.b_trail_use
	--local col = b_use and "slider" or "none"
	for _,bu in IPAIRS(ui.trail) do
		--bu:set_color_back( col )
		bu:interpolate_alpha_bu( b_use )
	end

end

function meu:update()
	if not self.imgs then
		local dir = self:build_dirname()
		self:print( "dir is "..dir )
		local imgs = {}
		self.imgs = imgs
		MEDIA.push_dir( nil )
			for i=1,8 do
				imgs[i] = IMGS.get_img( dir.."color_map_"..i..".png" )
			end
		MEDIA.pop_dir()
	end

	local ref	= self.ref
	local sha	= self:get_shading()
	local ui	= self.ui

	self.t_out = self:update_time() + self.phase_start * math.pi2

	--time go to first vertex float
	sha:set_vert_float( 1, self.t_out )
	self.t_out = self.t_out / math.pi2

	--aaa.show( self.pt_distrib, "pt_distrib" )

	local pt_nb		= math.floor( math.pow( 2, self.pt_nb ) )
	ui["bu_info_pt"]:set_text( pt_nb )
	--aaa.show( pt_nb, "pt" )
	local pt_nb_used		=  math.floor( self.pt_used * pt_nb )
	--aaa.show( pt_nb_used, "pt_used" )

	local seg_nb	= math.floor( math.pow( 2, self.seg_nb ) )
	ui["bu_info_seg"]:set_text( seg_nb.."x" )
	--aaa.show( seg_nb, "seg" )
	local seg_nb_used		=  math.floor( self.seg_used * seg_nb )
	--aaa.show( seg_nb_used, "seg_used" )

	param.set( ref.nb_v,	pt_nb_used	)
	param.set( ref.nb_axe,	seg_nb_used	)

	sha:set_vert_vec4( 1,	self.pt_distrib==1		and	1./(pt_nb-1)	or 1./(pt_nb_used-1),
							self.seg_distrib==1		and	1./(seg_nb-1)	or 1./(seg_nb_used-1),
							self.phase_min,
							self.phase_max
							)
	for i = 1,4 do
		sha:set_vert_vec4( i+1, self.ui.bu_col[i]:get_rgba() )
	end

	if self.b_trail_use then
		local fbo = self:get_fbo()
		--self:print( "fbo is "..fbo )
		if fbo then
			local bdd_clear = fbo:get_bdd_clear_screen()
			local b_trail = self.b_trail and (self.trail_time>=0)
			bdd_clear:set_color_active( not self.b_trail )
			if b_trail then
				bdd_clear:set_trail_time( self.trail_time )
			end
		end
	end
	--aaa.show( aaa.midi.get_control( 1, 1),  "midi 1 1 " )
end

function meu:draw()
	local ref	= self.ref
	local ui	= self.ui
	local sha	= self:get_shading()

	--aaa.show( self.s_draw_mode )
	local str
	if		self.s_draw_mode == 1 then	str = "POINTS"
	elseif	self.s_draw_mode == 3 then	str = "LINE_STRIP"
	end

	local b = self.b_depth
	param.set( ref.depth		,b )
	param.set( ref.depth_write	,b )

	self:draw_layers_begin()

	if self.b_draw then
		local cam = self:get_camera_used()
		-- TRANSFORMATION BEFORE
		if self.b_transfo then
			gol.push_matrix()
			local tab = self.tra
			tab.t_out = self:update_time( tab ) + tab.phase_start * math.pi2
			local v = math.sin( tab.t_out )
			tab.t_out = tab.t_out / math.pi2
			v = (v+1) * .5
			v = aaa.math.gain( v,  .5 + tab.power * .5 )
			v = interpolate( tab.tra_min, tab.tra_max, v )
			gol.translate_z( v )

			tab = self.osc
			--table.print( tab )
			tab.t_out = self:update_time( tab ) + tab.phase_start * math.pi2
			local v = math.sin( tab.t_out )
			tab.t_out = tab.t_out / math.pi2
			v = (v+1) * .5
			v = aaa.math.gain( v,  .5 + tab.power * .5 )
			v = interpolate( tab.angle_min, tab.angle_max, v ) / 360.
			gol.rotate_x( v )

			for i = 2,1,-1 do
				local tab = self.rots[i]
				gol.translate( tab.cen_x, tab.cen_y, tab.cen_z )
				tab.t_out = self:update_time( tab ) + tab.phase_start
				local axe = tab.axe
				if 		axe == 1	then	gol.rotate_x( tab.t_out )
				elseif	axe == 2	then	gol.rotate_y( tab.t_out )
				elseif	axe == 3	then	gol.rotate_z( tab.t_out )
				end
				gol.translate( -tab.cen_x, -tab.cen_y, -tab.cen_z )
			end

			tab = self.frustum_x
			tab.t_out = self:update_time( tab ) + tab.phase_start * math.pi2
			local v = math.sin( tab.t_out )
			tab.t_out = tab.t_out / math.pi2
			v = (v+1) * .5
			v = aaa.math.gain( v,  .5 + tab.power * .5 )
			v = interpolate( -tab.tra_min, -tab.tra_max, v )
			param.set( cam, "frustum_offset_x", v )

			tab = self.frustum_y
			tab.t_out = self:update_time( tab ) + tab.phase_start * math.pi2
			local v = math.sin( tab.t_out )
			tab.t_out = tab.t_out / math.pi2
			v = (v+1) * .5
			v = aaa.math.gain( v,  .5 + tab.power * .5 )
			v = interpolate( -tab.tra_min, -tab.tra_max, v )
			param.set( cam, "frustum_offset_y", v )
		else
			param.set( cam, "frustum_offset_x", 0 )
			param.set( cam, "frustum_offset_y", 0 )
		end

		-- TEXTURE BEFORE DRAW
		sha:set_vert_float( 8, 1. / self.texture_size )
		local img = self.imgs[ self.texture_id ]
		if img then
			sha:set_vert_int( 2, self.b_texture and 1 or 0 )
			--self:print( "bind is "..img:get_bind() )
			gol.bind_texture( img:get_bind() )
			gol.set_wrap_2d_edge()
		else
			sha:set_vert_int( 2, 0 )
		end

		if self.b_2d then
			sha:set_vert_vec4( 8,	nil, nil, self.dist_2d, self.dist_2d * self.size_2d )
			--self,	"2D_dist",
			--self,	"2D_focal"
		else
			sha:set_vert_vec4( 8,	nil, nil, 0, 0 )
		end
		-- DRAW
		if str then
			param.set( ref.primitive, str )
		else
			param.set( ref.primitive, "LINE_STRIP" )
			self:draw_layers()
			param.set( ref.primitive, "POINTS" )
		end
		self:draw_layers()

		-- TRANSFORMATION AFTER
		if self.b_transfo then
			gol.pop_matrix()
		end
	end
	self:draw_layers_end()
end