
MONACO_AQUA.scenes_name =  {
							"DJ1", "PIJ1",
							"DJ2", "PIJ2",
							"DJ3", "PIJ3",
							"DJ4",
							"PIN1", "DN1",
							"DN2",  "PIN2", 	--todomona flipped October 8th 2020 by Mâa
							"PIN3", "DN3",
						}
MONACO_AQUA.scenes_bee_on = {
							DJ1=true, 	PIJ1=true,
							DJ2=true, 	PIJ2=true,
							DJ3=true, 	PIJ3=true,
							}
MONACO_AQUA.scenes_ga_on = {
							PIJ1=true,
							PIJ2=true,
							PIJ3=true,
							}

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	local ix,iy = 1,2
	local SY = 1
	local DY = .2

	local SX=3

	bu = self:add_trig_method(	{	1, 14,				SX, SY},	"Edit",			app, "edit_render_file" )
	iy = iy + SY
	bu = self:add_trig_method(	{	4, 14,				SX, SY},	"Read",			app, "read_gstate_file" )
	iy = iy + SY
--	bu = self:add_trig_method(	{	ix, iy,				SX, SY},	"Send",			app, "send_gstate"		)

	iy =  2
	bu = self:add_trig_method(	{	ix, iy,				SX, .8},	"IN",			self, "fade_in"		 	):set_color_back("green")
	iy = iy + .8
	bu = self:add_trig_method(	{	ix, iy,				SX, .8},	"OUT",			self, "fade_out"	 	):set_color_back("red")
	iy = iy + .8


	ix,iy = ix+3,3

	SY=.8
	local scenes_name = app.scenes_name
	for i =1,#scenes_name do
		local name = scenes_name[i]
		bu = self:add_trig_method(	{ix, iy,				2, SY},	name,			app, "change_scene",		name ):set_color_back("restart")
		bu = self:add_trig_method(	{ix+SX, iy,			SX, SY},	name.."_beg",	app, "apply_render_gstate",	name )
		bu = self:add_trig_method(	{ix+SX+7, iy,		SX, SY},	"Fish "..name,	app, "change_fishs",		name )
		iy = iy + SY
	end
	iy = 3
	for i =1,#scenes_name do
		local name = scenes_name[i].."_end"
		bu = self:add_trig_method(	{ix+SX*2, iy,		SX, SY},	name, 			app, "apply_render_gstate", name )
		iy = iy + SY
	end

end


function meu:update_fade_low( fade )
	local meu_fader = self:get_meu_by_name_no_error( "NdcPost_1" )
	if meu_fader then
		meu_fader:set_bu_value( "fade", app:is_party() and 1 or fade )
	end
	if app:is_party() then
		app:set_transition_volume( 0 )
	else
		app:set_playlist_volume( fade )
		app:set_transition_volume( 1 )
	end
end

function meu:update_fade()
	local fade = self.fade or 1
	local fade_dir = self.fade_dir or 0
	local step = fade_dir * aaa.time.dt * 1
	fade =  clamp_01( fade + step )
	self.fade = fade
	--self:print( "fade "..fade)
	if app:is_pc_mas() or app:is_pc_dev() and not app:is_tablet() then
		self:update_fade_low( fade )
		app:send_method( "net_update_fade", fade )
	end
end

local dur_fade = 5.0
function meu:fade_in()
	self.fade_dir = 1/dur_fade
	self.fade = 0.0
--	app:play_transition( false )
end
function meu:fade_out()
	self.fade_dir = -1/dur_fade
	app:play_transition( true )
	if app:is_pc_mas() or app:is_pc_dev() then
		app:send_method_verbose( "net_play_transition" )
	end

end

function meu:init()
	self.s_need_read  = 0
end

function meu:update()
	if app:is_tablet() then
		app:sync_ui_customer()
		return
	end

	--self:print( "This is the Master" )
	if app:is_pc_mas() then
		--self:print( "This is the Master" )
		if not self.seqs_preroll then
			self:print( "Preroll sequences" )
			ga:get_seqs():begin( true )
			self.seqs_preroll = true
		end
	else
		--self:print( "This is not the Master" )
		if not self.seqs_preroll then
			local meu = self:get_meu_by_name_no_error( "DramaControl_1" )
			if meu then
				meu:preroll_animation()
			end
			self.seqs_preroll = true
		end
	end
	local seqs = self:check_need_define_seq_ui(ga:get_seqs())

	self:set_ui_slot_at_start( 3 )
	self.s_need_read = self.s_need_read + 1
	--self:box_debug( self.s_need_read )
	if self.s_need_read==4 then
		app:read_gstate_file()
	end
	self:update_fade()
end

function MONACO_AQUA:change_anims( name )
	if app:is_tablet() then return end

	local meu = self:get_meu_by_name_no_error( "DramaControl_1" )
	if meu then
		--table.print( mu, "mu", 5)
		meu:change_scene( name )
	else
		self:print_error( "Can't find DramaControl" )
	end
end
--todo MATERIAL_MATTE
function MONACO_AQUA:change_tank( name )
	local b_tablet_not = not app:is_tablet()

	for _,scene_name in pairs(self.scenes_name) do
		local b_on  = (not app:is_so()) and (name==scene_name)
		local tank_name = "TankFish_"..scene_name
		local meu = self:get_meu_by_name( tank_name )
		meu:switch_off_on( b_on and b_tablet_not )
	end

	local meu
	meu = self:get_meu_by_name( "TankFish_1" )
	meu:switch_off_on( false )

	meu = self:get_meu_by_name( "TankFish_BEE" )
	meu:switch_off_on( self.scenes_bee_on[name] and (not app:is_so()) and b_tablet_not )

	meu = self:get_meu_by_name( "TankFish_GA" )
	if true then --true keep go_round by scene
		meu:switch_off_on( self.scenes_ga_on[name] and (not app:is_so()) and b_tablet_not )
	else
		meu:switch_off_on( false )
	end

	meu = self:get_meu_by_name( "TankFish_SO" )
	meu:switch_off_on( app:is_so() and b_tablet_not )
	--todo we need to remove test when we evolve the thank and so want to execute run.lua every time 
	local tank = meu:get_tank()
	if not tank.b_loaded then
		meu:load_tank()
	end
end

function MONACO_AQUA:change_fishs( name )
	self:change_tank( name )
	self:change_anims( name )
end

function MONACO_AQUA:net_change_scene( name )
	if not app:is_pc_mas() and not app:is_tablet() then
		self.scene_cur = name
		self:apply_render_gstate( name )
		app:playlist_play( name )
		self:change_fishs( name )
		self:print_inverse( "scene is now "..self.scene_cur )
		if self:is_pc_dif() then
			aaa.os.set_window_notopmost()
			aaa.os.set_window_topmost()
		end
	end
end

function MONACO_AQUA:change_scene( name )
	if app:is_pc_mas() and (not app:is_pc_dev()) then
		self:send_method_verbose( "net_change_scene", "\""..name.."\"" )
	end
	self:apply_render_gstate( name )
	app:playlist_play( name )
	self:change_fishs( name )
	self.scene_cur = name
	local meu_edit = self:get_meu_by_name_no_error( "MonacoEdit_1" )
	if meu_edit then
		meu_edit:fade_in()
	end
	self:print_inverse( "scene is now "..self.scene_cur)
end

function MONACO_AQUA:get_scene()
	return self.scene_cur
end

function meu:define_seq_ui(bus)
	self.bu_seq = self:add_bu_sequence( bus, ga:get_seqs() )
end

function MONACO_AQUA:define_seqs()

	--aaa.audio.set_volume( self:is_onsite() and 1 or .8 )

	local seqs = SEQS:recreate( "MonacoAqua", self )

 	local function add_seq( seq_id, duration, method_name )
	 	local name = self.scenes_name[seq_id]
		local seq = seqs:add_seq( SEQ:create( name, 1, seq_id, name, duration ) )
		seq:set_method( "update", self, method_name or "update_seq", seq )
		seq:set_ease_in_time( 1. )
		--seq:set_ease_out_time( 1. )
		return seq
	end

	local seq
	seq = add_seq( 	1,	81	)
	seq = add_seq( 	2,	117	)
	seq = add_seq( 	3,	197	)
	seq = add_seq( 	4,	117	)
	seq = add_seq( 	5,	103	)
	seq = add_seq( 	6,	90	)
	seq = add_seq( 	7,	65 	)
	seq = add_seq( 	8,	107	)
	seq = add_seq( 	9,	90	)
	seq = add_seq( 	10,	90	)
	seq = add_seq( 	11,	126	)
	seq = add_seq( 	12,	85	)
	seq = add_seq( 	13,	101	)

--	table.print( self.seqs, "self.seqs", 3 )
end


function MONACO_AQUA:net_update_fade( val )
	if not app:is_pc_mas() then
		local meu_edit = self:get_meu_by_name_no_error( "MonacoEdit_1" )
		if meu_edit then
	--	self:print( "fading out")
			meu_edit:update_fade_low( val )
		end
	end
end

function MONACO_AQUA:net_play_transition()
	if not app:is_pc_mas() then
		app:play_transition( true )
	end
end

function MONACO_AQUA:get_seq_cur_time()
	local seqs = self.seqs
	if seqs then
		local id = seqs:get_seq_cur_id()
		if id then
			return seqs:get_time_from_id(id)
		end
	end
	return 0.0
end

function MONACO_AQUA:get_seq_cur_time_left()
	local seqs = self.seqs
	if seqs then
		local id = seqs:get_seq_cur_id()
		if id then
			return seqs:get_time_left_from_id(id)
		end
	end
	return 0.0
end

-- function MONACO_AQUA:set_seq( name )
-- 	local seqs = self.seqs
-- 	if seqs and app:is_pc_mas() then
-- 		seqs:set_seq( name )
-- 	end
-- end

function MONACO_AQUA:update_seq( seq )
--	local id = self.seqs:get_seq_cur_id()
	--self:print( "Update seq called")
	local seq_time = seq:get_time()
	if self.last_seq_id ~= seq:get_seq_id() then
		self.last_seq_id = seq:get_seq_id()
		self:change_scene( seq:get_title() )
		self.b_fadeout_active = false
		self.b_fadein_active = false
	end
	if seq:get_duration() - seq_time < dur_fade then
		if self.b_fadeout_active == false then
			self.b_fadeout_active = true
			local meu_edit = self:get_meu_by_name_no_error( "MonacoEdit_1" )
			if meu_edit then
				meu_edit:fade_out()
			end
		end
	--elseif seq_time < dur_fade then
		-- if self.b_fadein_active == false then
		-- 	self.b_fadein_active = true
		-- 	local meu_edit = self:get_meu_by_name_no_error( "MonacoEdit_1" )
		-- 	if meu_edit then
		-- 		meu_edit:fade_in()
		-- 	end
		-- end
	end
	if app:is_pc_mas() or app:is_pc_dev() then
		self:send_method( "net_update_seq", "\""..seq:get_title().."\","..seq:get_duration()..","..seq_time )
	end

end

function MONACO_AQUA:net_update_seq( name, duration, time )
	if app:is_tablet() then
		--self:print( "Receiving "..name.." : "..time.."/"..duration )
		self.scene_cur = name
		self.scene_duration = duration
		self.scene_time = time
	end
end

function MONACO_AQUA:net_change_seq( name )
	if app:is_pc_mas() then
		local seqs = self.seqs
		if seqs then
			seqs:set_seq( name )
		end
	end
end

function MONACO_AQUA:net_quit_pc()
	if not app:is_tablet() then
		aaa.shutdown_no_save()
	end
end