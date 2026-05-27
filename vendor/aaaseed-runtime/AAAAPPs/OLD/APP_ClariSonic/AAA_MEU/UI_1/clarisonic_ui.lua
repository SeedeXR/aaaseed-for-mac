
function meu:define_seq_ui( bus )
	self.bu_seq = self:add_bu_sequence( bus, ga:get_seqs() )
end

function meu:set_vol( vol )
	aaa.audio.set_volume( vol )
end

function meu:init()
	local ref = self.ref
	local bdd_img = self:get_layer_bdd( 1 )
	ref.image_agitation_src = param.get_ref( bdd_img, "image_src")
	ref.average 			= param.get_ref( bdd_img, "grey_average" )
	ref.agitation 			= param.get_ref( bdd_img, "agitation_result")
	local bdd_anal = self:get_layer_bdd( 2 )
	ref.coverage			= param.get_ref( bdd_anal, "out_coverage" )

end
function meu:define_ui()
	local bu
	local par

	self.__b_need_seqs_define_ui = true

	local ix, iy = 9, 1
	local dx = 2.5
	bu = self:add_button( {ix+dx,	iy },			"Simul", app, "b_simul", false )
		bu:set_text_rect_ratio( 3 )
	bu = self:add_slider(	{ix,iy,	dx,1},	"A" )
	bu = self:add_slider(	{ix+8-dx,iy,	dx,1},	"B" )

	iy = iy + 1.4
	self:add_slider_two(	{ix,iy,	4,1},	"PreMixMax", 		app, "pre_min", "pre_max",	0, 1, 0, 1 )
	self:add_slider_two(	{ix+4,iy,	4,1},	"agitMixMax", 		app, "agi_min", "agi_max",	0, 1, 0, 1 )
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	4,1}, 	"presence_raw",		app, "presence_raw", 		0, 0, 1  )
	bu = self:add_slider(	{ix+4,iy,	4,1}, 	"agitation_raw",	app, "agitation_raw", 		0, 0, 1  )
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	4,1}, 	"presence",			app, "presence", 			0, 0, 1  )
	bu = self:add_slider(	{ix+4,iy,	4,1}, 	"agitation",		app, "agitation", 			0, 0, 1  )

	iy = iy + 1.4
	bu = self:add_slider(	{ix+4,iy,	4,1}, 	"Hotspot_time",		app, "hotspot_time", 		3, 1, 5  )
	bu = self:add_slider(	{ix,iy,	4,1}, 	"Hotspot_raw",		app, "hotspot_raw", 		0, 0, 1  )
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	4,1}, 	"Hotspot",			app, "hotspot", 			0, 0, 1  )

	iy = iy + 3
	bu = self:add_slider(	{ix,iy,	4,1}, 	"Time Up",			app, "time_delay_up", 		2, 1, 10  )
	bu = self:add_slider(	{ix,iy+1,	4,1}, 	"Time Down",		app, "time_delay_down",		4, 1, 10  )

	local function add_slider_out( ix, iy, name )
		local bu = self:add_slider(	{ix,iy,	2,2}, name )
		bu:set_ui_active( false )
		return bu
	end
	bu = add_slider_out( ix+5,		iy,			"Pre" )


	iy = iy + 2

	bu = self:add_trig_method(	{9, 14},			"load anims",		app, "load_anim" ):set_color_back("load")
--[[
	iy = 8
	bu = self:add_selector(	{ix,iy,	8,1},	"Langue" )	--notext not save in preset
		bu:set_nb_min_0( 4, 1 )
		bu:set_item_text( 1, "Francais", "Anglais", "Espagnol", "Signes" )

	iy = iy + 2
	bu = self:add_trig_method(	{ix, iy,			4, 1}, "Normal",		self, "set_vol", 1	)
	bu = self:add_trig_method(	{ix+4, iy,		4, 1}, "Low",		self, "set_vol", .5	)
	iy = iy + 1
 	bu = self:add_slider(	{ix,iy,	8,1}, "Volume", 	self, "volume_ui",	1, 0, 1 )
	bu = self:add_slider(	{ix,iy+1,	8,1}, "Music",  	self, "vol_mus_ui",	1, 0, 1 )
	bu = self:add_slider(	{ix,iy+2,	8,1}, "Voix",  		self, "vol_voi_ui",	1, 0, 1 )
--]]
end

function meu:update()
	local ref = self.ref
	local pre
	if app.b_simul then
		pre = self:get_bu_value( "a" )
	else
--todo
--		pre = app:get_face_info( 1, 1 ):get_face_nb()
	end
	app.ratio_x = 3132 / 1632
	app.ratio_y = 1 / app.ratio_x

	self:set_bu_value( "presence_raw",  param.get( ref.average ) / 256. )
	self:set_bu_value( "agitation_raw", param.get( ref.agitation ) / 16. )
	local presence = app.b_simul and self:get_bu_value( "a" )  or ((app.presence_raw-app.pre_min)/(app.pre_max-app.pre_min))
	local agitation = app.b_simul and self:get_bu_value( "b" ) or ((app.agitation_raw-app.agi_min)/(app.agi_max-app.agi_min))
	self:set_bu_value( "presence",  presence )
	self:set_bu_value( "agitation",  agitation )

	local pre = app:update_presence( app.presence )
	self:set_bu_value( "pre", pre )

	self:set_bu_value( "hotspot_raw",  param.get( ref.coverage ) )
	if not app.b_simul then
		local hotspot = app.hotspot
		hotspot = hotspot + aaa.time.dt * ((app.hotspot_raw>.99) and 1 or -1 ) / app.hotspot_time
		hotspot = clamp_01( hotspot )
		self:set_bu_value( "hotspot",  hotspot )
	end
--todo
--	self:set_bu_value( "pre", pre )


	--	this way the seqs can be initialized after the GP
	local seqs = ga:get_seqs()
	self:check_need_define_seq_ui( seqs )

	app:update_sound( self.volume_ui, self.vol_mus_ui, self.vol_voi_ui  )

	--self:print( pre )
	if seqs:is_run() then
		local seq_id = seqs:get_seq_cur_id()
		if seq_id == 4 then
			self:set_bu_value( "hotspot",  0 )
		end
--[[
		if seq_id == 1 then
			if pre == 1 then
				seqs:inc_seq()
			end
		else
			if seq_id ~= 7 then
				if pre == 0 then
					seqs:restart(1)
				end
			end
		end
--]]
	end
end
