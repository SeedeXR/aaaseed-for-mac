function meu:define_meu_infos( )
	return { 	author = "Mâa",
				tags = { "3d", "Vj", "Core", "CoreGraphic", "Draw", "procedural", "Sound", "unfinished" },
				help =	{
							"represent the Sound input but mainly do some detection",
							"using c_bdd_sound so have access to Sound wave and spectrum, midi, csv file",
							"It have a lot of problems and the sound input function only in certain case",
							"Unusable for Now (2024 Sep Mâa)"
						}
			}
end

function meu:define_ui()
	self:init()
	local ref = self.ref
	local bu
	local par
	local ui = self.ui
	local ix, iy
	--self:add_camera()

	self:add_camera()

	bu = BU_OBJ:create(  "test", {0,.2, .2} )
		bu:add_obj( self:get_obj_by_name_symbo( "SndVisu" ) )
		bus_cur:add_bu( bu )
		bu:set_dplane( -42 )

	ix = 9
	iy = 8
	ref.snd_obj  = nil
	ui.bu_left 	= self:add_button(	{ix, 	iy,		4},	"Calib Left"	)
	ui.bu_right = self:add_button(	{ix+4,	iy,		4},	"Calib Right"	)
	ui.bu_auto	= self:add_button(	{ix+2,	iy+1,	4},	"Calib Auto"	)

--[[test
	iy = iy + DY
	bu = self:add_button(	{ix+1, iy,	DY, DY }, "Time f1", 		self, "b_time_use", false  )
		bu:set_text_rect_ratio( 3.5 )
	bu = self:add_slider(	{ix+3,iy,	5,DY},	"Time_factor",		self, "time_factor",	0, -4, 4 )
		bu:set_meter( false )
		self.time = 0
--]]

	local function set_color_out( bu )
		bu:set_color_back( { 1, .5, 0, .5 } )
	end
	local DXO = 2
	local function add_output( ix, iy, name )
		local bu = self:add_slider(	{ix,iy,	DXO,4},	name.."_out",		nil, nil,		0,	0,1		)
		bu:set_text( name )
		set_color_out( bu )
		bu:set_meter( true )
		return bu
	end

	iy = 7
	ix = 1
	local DY = .8
	bu = self:add_button(	{ix, iy, 	2,1},	"Mode",			self, "input_mode", 	2	)
		bu:set_multiple( { "Off", "Sound", "Simul", "Save" }, "Mode" )
		--bu:set_min_max( 1, 3 )
		ui.bu_mode = bu
		self.phase = 0
	bu = self:add_slider(	{ix+2,iy,	3,1},	"Cycle",		self, "cycle",			6,	1, 6	)
	bu = self:add_slider(	{ix+5,iy,	3,1},	"Pulse",		self, "pulse",			1,	.1, 1	)
	iy = iy + 1

	bu = self:add_slider(	{ix,iy,		8,1},	"Factor",		self, "factor",			1,	0, 1	)
	bu = self:add_slider(	{ix,iy+1,	8,1},	"Offset",		self, "offset",			0,	0, -1	)
	iy = iy + 2

	bu = self:add_slider(	{ix,iy,		8,DY},	"Input",		nil, nil,				0,	0, 1	)
		bu:set_color_back( { 1,0,0, .5 } )
		bu:set_meter( true )
	iy = iy + DY

	bu = self:add_button(	{ix,iy, 	1,1},	"DoIt",			self, "b_doit", 		1,	0, 1	)
	iy = iy + 1

	bu = self:add_slider(	{ix,iy,		8,1},	"Threshold",	self, "threshold",		0,	0, 1	)
	iy = iy + 1

	bu = self:add_slider(	{ix,iy,		8,1},	"Release",		self, "release_time",	1,	0.01, 1	)
	iy = iy + 1

	bu = self:add_slider(	{ix,iy,		8,1},	"Decay",		self, "decay_time",		1, 	0.01, 4	)

	ix = 9
	iy = 10
	add_output( ix, 		iy, "ReleaseDecay" )
	add_output( ix+DXO, 	iy, "Trig" )
	add_output( ix+DXO*2, 	iy, "Release" )
	add_output( ix+DXO*3, 	iy, "Decay" )
	self.__release 	= 0
	self.__decay 	= 0
	self.__reldec	= 0
	self.__trig		= 0
end

function meu:init()
	local ref = self.ref
	ref.value 		= self:get_value_ref( 1 )
	ref.bdd_sound	= self:get_layer_bdd( 1 )
	ref.trax		= self:get_trax( 1 )
end

function meu:update_snd_input()
	local ref = self.ref

	aaa.obj.update( ref.bdd_sound )
	aaa.obj.update_then_draw( ref.trax )

	local snd_obj

	--snd_obj = param.get_obj_attached( ref.bdd_sound, "channel" )
	--self:print( "ref.bdd_sound "..ref.bdd_sound.." at "..aaa.obj.get_class(ref.bdd_sound) )
	--aaa.obj.set_focus_ui( ref.bdd_sound )

	if ref.snd_obj~=snd_obj then

		local ui = self.ui
		local tab = {}
		if snd_obj then
			tab[1] = param.get_ref( snd_obj, "calibration_left"	 )
			tab[2] = param.get_ref( snd_obj, "calibration_right" )
			tab[3] = param.get_ref( snd_obj, "calibration_auto"	 )
		else
		end
		local function l_set_target( bu, target )
			local val = bu:get_value()
			bu:set_target_param(	target	)
			bu:set_value( val )
		end
		l_set_target(	ui.bu_left,		tab[1]	)
		l_set_target(	ui.bu_right,	tab[2]	)
		l_set_target(	ui.bu_auto,		tab[3]	)
		ref.snd_obj = snd_obj
	end
end

function meu:update()
	--self:print( "input_mode "..self.input_mode.." "..self:get_bu_value( "mode") )
	--self:show( "input_mode", self.input_mode  )

--	self:print( "meu:update() " )
--	self:update_snd_input()
	self:update_snd_input()
	local verbose = 0
	if verbose > 0 then aaa.print_method() end

	local dt = aaa.time.dt

	local val
	local b_can_stop = false
	--self:show( meu_dif, "DIF" )
	if self.input_mode == 1 then
	else
		if self.input_mode == 2 then
			val = param.get( self.ref.value )
			--self:print( val )
			--self:show( "input", val  )
			val = val * self.factor * 16 + self.offset
		else
			if self.input_mode == 4 and self.input_mode_last ~= 4 then
				self.phase = 0
			end
			local phase = self.phase
			phase = clamp_01( phase + dt / self.cycle )
			if phase == 1 then
				phase = 0
			end
			self.phase = phase
			val = ( phase * self.cycle ) / self.pulse
			if val > 1 then
				val = 1
				b_can_stop = true
			end
			val = val * val
			val = 1 - val
		end
		val = clamp_01( val )
		self:set_bu_value( "input", val )
	end

	val = self:get_bu_value( "input" )
	aaa.midi.set_control( 1, 1, 	val		)
	aaa.midi.set_control( 1, 2,	    val*2	)
	aaa.midi.set_control( 1, 3, 	val*3	)
	aaa.midi.set_control( 1, 4, 	val*4	)

	local le_mur = aaa.lua.global.get( "LeMur" )
	if le_mur then
		le_mur.input = val
	end

	if self.b_doit > 0 then
		if verbose > 0 then self:print( "update() Do It" ) end
		local reldec 	= self.__reldec
		local trig
		local release 	= self.__release
		local decay 	= self.__decay

		if val > self.threshold then
			trig = (self.__trig==0) and 1 or 0
			-- oki we trig
			decay 	= 1
			release = 1
			reldec 	= 1
		else
			trig = 0
			if verbose > 0 then self:print( "update() dt : "..dt ) end
			if decay > 0 then
				if release > 0 then
					release = clamp_01( release - dt / self.release_time )
				else
					decay = clamp_01( decay - dt / self.decay_time )
				end
			end
			reldec = clamp_01( reldec - dt / (self.decay_time + self.release_time) )
		end
		self.__release	= release
		self.__decay 	= decay
		self.__reldec	= reldec
		self.__trig		= trig

			--self:print( "reldec = "..reldec )
		self:set_bu_value( "releaseDecay_out",	reldec	)
		self:set_bu_value( "trig_out", 			trig	)
		self:set_bu_value( "release_out", 		release	)
		self:set_bu_value( "decay_out", 		decay	)

		--local reldec 	= self:get_bu_value( "ReleaseDecay" )
		--local trig 	= self:get_bu_value( "trig" )
		--local release = self:get_bu_value( "Release" )
		--local decay 	= self:get_bu_value( "Decay" )

		if le_mur then
			le_mur.reldec 	= reldec
			le_mur.trig 	= trig
			le_mur.release 	= release
			le_mur.decay 	= decay
		end
		--foreldec = reldecr i = 1,8 do

		if verbose > 0 then self:print( "update() MIDI" ) end
		aaa.midi.set_control( 3,1, reldec		 )
		aaa.midi.set_control( 3,2, trig			)
		aaa.midi.set_control( 3,3, release		)
		aaa.midi.set_control( 3,4, decay		)
		aaa.midi.set_control( 3,5, 1-reldec 	)
		aaa.midi.set_control( 3,6, 1-trig   	)
		aaa.midi.set_control( 3,7, 1-release	)
		aaa.midi.set_control( 3,8, 1-decay		)

	--[[
			aaa.midi.set_control( 3, 2, .25 + .75 * output )
			aaa.midi.set_control( 3, 3, .5 + .5 * output )
			aaa.midi.set_control( 3, 4, .75 + .25 * output )
			aaa.midi.set_control( 3, 5, .25 - .25 * output )
			aaa.midi.set_control( 3, 6, .5 - .5 * output )
			aaa.midi.set_control( 3, 7, .75 - .75 * output )
	--]]
	end
	local meu_dif = self:get_meu_by_name( "fbo_DIF" )
	local save = 0
	if self.input_mode == 4 then
		save = 1
		if le_mur then
			if le_mur.reldec <= 0 then
				if b_can_stop then
					save = 0
					self:set_bu_value( "Mode", 2 )
				end
			end
		end
	end
	self.input_mode_last = self.input_mode
	meu_dif:set_bu_value( "save", save )
--	meu_dif:set_bu_value( "save", 1 )
end

