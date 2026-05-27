if CLASS.DECLARE( "MEU_TRAX",	MEU ) then
	MEU_TRAX:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
		"this MEU can connect in and/or out to any BU.",
		"it also generate signal (value changing with time).",
		"Trax are so responsible of flowing information from one place to the other",
		"and make BU values change over time, some people talk about patch.",
		"This correspond the metaphor of the analog synth.",
		"Connections are saved."
		)
	MEU_TRAX.b_link_active = true
end


function MEU_TRAX:define_meu_infos()
	-- local help = self:get_class_doc()
	-- if type(help) == "string" then
	-- 	self:print( "help string is "..help )
	-- else
	-- 	table.print( help, "help" )
	-- end
	return { author = "Mâa",
			tags =	{ "core", "input", "output", "vj", "procedural" },
			}
end

--todo better
-- if not MEU.__meu_trax then
-- 	MEU.__meu_trax = {}
-- end


function MEU_TRAX:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local ix,iy = 1,1
	local SY = .8
	local DY = .2
	local SEP = .05

	local SYT = .8
	local SYB = .6
	local SX1 = 1.25
	local SX2 = 8 - SX1 - 1
	local function add_text( rect, name )
		local bu =  self:add_text( rect, name )
		bu:set_value_load_save(true)
		return bu
	end

	local function add_connection( b_trax_out, id )
		local name = b_trax_out and "Out" or "In"
		self:begin_bu_group( name..id )

			local t = { b_trax_out = b_trax_out, meu_trax = self } 
			bu = self:add_button(	{ix,			iy,		SX1,SYT},	"Active",	t,"b_active", false )
				--bu:set_text_visible( false )
				:set_text( id )
			bu = self:add_slider_two({ix+SX1,		iy,	8-SX1-1,SYT},	"Range",	t,"range_min", "range_max",	0,1,	-8,8	)
				:set_text( "Range "..name )
			self:add_button(		{ix+8-1,		iy,		1,SYT}, 	"Inverse",	t,"b_inv",					false	)
				:set_text( "Inv" )
			iy = iy + SYT
			
			bu = self:add_trig(		{ix+8-1,		iy,		1,SYT},		"Unplug" )
				:set_text( "X" )
				bu:set_method_on_click( self, "disconnect_trax_meu", name:lower(), id )
				--bu:set_color_back( "grey" )
				bu.__b_no_connect = true
			bu = self:add_trig(		{ix,			iy,		SX1,SYT},	"Plug" )
				:set_text( "Plug" )
				bu:set_method_on_click( self, "connect_trax_meu", bu, name:lower(), id )
				bu.__b_no_connect = true
				bu.__b_no_led = true
				t.bu_trax = bu
			bu = add_text(			{ix+SX1,		iy,		SX2,SYB},	"Meu" )
				:set_text( "No" )
				:set_preset_use( true )
				t.bu_meu = bu
			iy = iy + SYB
			
			bu = add_text(			{ix+SX1,		iy,		SX2,SYB},	"Bu" )
				:set_text( "" )
				:set_preset_use( true )
				t.bu_bu = bu
			bu = add_text(			{ix+8-1,		iy,		1,SYB},		"Id" )
				:set_text( "" )
				:set_preset_use( true )
				t.bu_id = bu
			iy = iy + SYB

		self:end_bu_group()
		return t
	end

	self:add_camera()

	local SX_FN = 4 
	local SX_CH = 2

-- INS
	for i=1,self.con_in.nb do
		self.con_in[i] = add_connection( false, i )
	end
	iy = iy + DY

	self:add_button(						{ix,iy,				SX1,SY},	"In",  			self, "b_use_in", false )
	ui.bu_slider_in		= self:add_slider(	{ix+SX1,iy,			4,SY},		"Value In",		self, "value_in" )
		:set_text( "In" )
	iy = iy + SY + DY

	bu = self:add_midi_button_channel(		{ix+SX_FN,iy,		SX_CH,SY},	"Channel In" )
		bu:set_target( self, "ch_in" )
		ui.bu_ch_in = bu
	bu = self:add_midi_button_controller(	{ix+SX_FN+SX_CH,iy,	1,SY},		"Controller In" )
		bu:set_target( self, "ctl_in" )
		ui.bu_ctl_in = bu
	bu = self:add_button(					{ix,iy,				SY,SY },	"Active",		self,"b_active",	true )
		:set_text_visible( false )

-- WHAT
	self.fn_name_bu	=	{	"Sinus", "Saw Tooth", "Triangle", "Square",
							"RND Linear", "RND Gauss", "Turbulence", "Fractal Sum",
							"FFT", "Beat", "MIDI Ctrl", "MIDI Velo",
							"Variable",	"KEYB Shift", "KEYB Alt", "KEYB Ctrl",
							 "MOUSE X",  "KEYB Ascii", "Slide Index", "In Changed", 
							 "MOUSE Y", "MOUSE Left", "MOUSE Middle", "MOUSE Right", 						
						}
	self.fn_name_trax =	{	"SINUS", "TRIANGLE", "TRIANGLE_UP_THEN_DOWN", "SQUARE",
							"RANDOM_LINEAR", "RANDOM_GAUSS", "TURBULENCE", "FRACTAL SUM",
							"FFT", "BEAT", "MIDI CONTROLLER", "MIDI VELOCITY",
							"NAME_REAL", "KEYBOARD_SHIFT", "KEYBOARD_ALT", "KEYBOARD_CTRL",
							 "MOUSE_X",  "ASCII", "SLIDE_INDEX", "IN_IS_CHANGED",
							 "MOUSE_Y", "MOUSE_CLICK_LEFT", "MOUSE_CLICK_MIDDLE", "MOUSE_CLICK_RIGHT", 
						}
	bu = self:add_button(					{ix+SY,iy,			SX_FN-SY,SY },	"Fn",		nil,nil,			3 )
		bu:set_menu( self.fn_name_bu )
		bu:get_selector():set_nb( 4,6 )
		bu:set_text_selector( true )
 		bu:set_method_on_value_change( self, "set_function_used" )
 --		bu:set_target_param( ref.fn )
 	iy = iy + SY
	bu = add_text(			{ix+1,iy,		7,SY},	"name_picker" ):set_text( "Name" )
		ui.bu_name_picker = bu
	iy = iy + SY + DY

	local SX = 1.5
	local SXR = 8 - SX
	local SXRH = SXR / 2
	--self:add_slider_two(	{ix,	iy,		8,SY}, 		"FFT Freq Min Max",		nil, ref.phase, ref.freq,	0, 1,	0, 3	)
	self:add_slider(		{ix,iy,		4,SY}, 	"Phase",	ref.phase, nil,		0,	-1,1	)
	self:add_slider(		{ix+4,iy,	4,SY}, 	"Freq",		ref.freq, nil,		1,	0,16	)
	iy = iy + SY


-- GAIN BIAS
	self:begin_bu_group( "GainBias" )
		bu = self:add_button(	{ix,			iy,	SX,		SY},	"Active",	self, "b_gb",	false	):set_text( "GB" )
		bu = self:add_slider(	{ix+SX,			iy,	SXRH,	SY},	"Gain",		self, "gain",	.5,		0,1	)
		bu = self:add_slider(	{ix+SX+SXRH,	iy,	SXRH,	SY},	"Bias",		self, "bias",	.5,		0,1	)
	self:end_bu_group()
	iy = iy + SY + SEP

-- THRESHOLD
--	self:add_slider(		{ix,iy+SY,		8,SY},		"Gamma",		self, "gamma",		0.001, 0.001, 8	)
--	iy = iy + SY + DY
--	self:add_slider(		{ix,iy,			7,SY},		"Scope fActor", self, "scope_factor",	1,	0, 64 )
	self:begin_bu_group( "Threshold" )
	local SXTH = 2
		self:add_button(		{ix,iy,		SX,SY},		"Active", 				self, "b_threshold",		false ):set_text( "Thr" )
		self.fn_trig_bu =	{	"Threshold", "Trig Up", "Trig Down", "Trig Both", "Flip On Up", 	}
		self.fn_trig_trax =	{	"FN_THRESHOLD", "FN_TRIGGER_UP", "FN_TRIGGER_DOWN", "FN_TRIGGER_UP_AND_DOWN", "FN_TRIGGER_UP"	}
		bu = self:add_button(	{ix+SX,iy,	SXTH,SY },	"Trigger",		nil,nil,			1 )
			bu:set_menu( self.fn_trig_bu )
			bu:set_text_selector( true )
			bu:set_target( self, "s_fn_trig" )
		self:add_slider(	{ix+SX+SXTH,	iy,	8-SX-SXTH,SY},	"Value",		self, "threshold",	.5, 0,1	):set_text( "Threshold" )
	self:end_bu_group()
	iy = iy  + SY
	-- self:add_button(		{ix+1, 		iy,	SY,SY},		"th_min_active", self,	"b_th_min",	false ):set_text_draw( false )
	-- self:add_slider(		{ix+1+SY,	iy,	7-SY,SY},	"th_min", 		 self,	"th_min",	0,	-8,8	)
	-- iy = iy  + SY
	-- self:add_button(		{ix+1, 		iy,	SY,SY},		"th_max_active", self,	"b_th_max",	false ):set_text_draw( false )
	-- self:add_slider(		{ix+1+SY,	iy,	7-SY,SY},	"th_max", 		 self,	"th_max",	1,	-8,8	)
	-- iy = iy + SY + DY

-- REMAP	
	self:begin_bu_group( "Remap" )
		self:add_button(			{ix, 		iy,	SX,SY},		"Active", 	self, "b_remap",		false )
			:set_text( "Remap" )
		bu = self:add_slider_two(	{ix+SX,		iy,	8-SX*2,SY}, "Min Max",	self, "remap_min", "remap_max",		0,1, 0,1	)
			bu:add_values_def( -3, -2, -1, 0, .5, 1, 2, 3 )
		self:add_button(			{ix+8-SX,	iy,	SX,SY}, 	"Inverse",	self, "b_remap_inv",	false	)
			:set_text( "Inv" )
	self:end_bu_group()
	iy = iy + SY + SEP

-- FILTER
	self:begin_bu_group( "Filter" )
		local function add_slider( ox, sx,	name, field_name,	val, min,max )
			local rect = { ix+ox,iy, SY,SY }
			self:add_button(		rect,	name.."_active", 	self,	"b_"..field_name,	false ):set_text_draw( false )
			rect[1] = ix+ox+SY
			rect[3] = sx-SY
			return self:add_slider(	rect,	name, 				self,	field_name,			val,	min,max	)
		end
		bu = self:add_button(		{ix,		iy,	SX,SY},	"Active",		self, "b_filter",	true	)
			:set_text( "Filter" ) 
		add_slider(						SX,			SXRH, 	"Up",	"inter_up",		0.25,	0,1	)
		add_slider(						SX+SXRH,	SXRH, 	"Down",	"inter_down",	0.25,	0,1	)
	self:end_bu_group()
	iy = iy + SY + SEP



-- SCOPE
	ix,iy = 1,10.35
	-- INFO OUT
		ui.bu_info_in		= self:add_text_info(	{1,iy,	4,SY},	"In Value" ):set_text_color( "cyan" )
		ui.bu_info_out		= self:add_text_info(	{4,iy,	4,SY},	"Out Value" )
	
	self:add_button( 		{ix,	15, 2,SY}, "Scope Active",		self, "b_scope_run", 		true	)
		:set_text( "SCOPE" )
	self:add_slider( 		{ix+2,	15,	2,SY}, "Scope Duration",	self, "scope_duration", 	2,	1,32	)
		:set_text( "Duration" )
		:add_values_def( 3,4,5,8,10,12,15,16,20,25,30 )
	self:add_button(		{ix+6,	15,	2,SY},	"Links", 			MEU_TRAX,	"b_link_active"	)
		:set_value_type_bool(true)

	self:add_slider(		{ix+15.1,	iy,	.9,4.45},	"Canonical", 	self,	"value_cano",		0,	0,1	)

-- MIDI OUT
	ix,iy = 9,10.35-SYT*2
	self:begin_bu_group( "Midi Out" )
		bu = self:add_button(		{ix,		iy,		SX1,		SYT},	"Active",	self, "b_midi_out", 	false )
			:set_text( "Midi" )
		bu = self:add_slider_two(	{ix+SX1,	iy,		8-SX1-1,	SYT}, 	"Range",	self, "midi_out_min", "midi_out_max",		0,1,	0,1	)
		bu = self:add_button(		{ix+8-1,	iy,		1,			SYT}, 	"Inverse",	self, "b_midi_inv", false	):set_text( "Inv" )
			--:set_text_visible( false )
		iy = iy + SYT
		bu = self:add_trig(			{ix,		iy,	 	SX1,		SYT},	"Plug" )
			bu:set_method_on_click( self, "connect_midi", bu )
			bu.__b_no_connect = true
			
		bu = self:add_midi_button_channel(	{ix+SX1,iy,	SX_CH,		SYT},	"Channel" )
			bu:set_target( self, "ch_out" )
			ui.bu_ch_out = bu
		bu = self:add_midi_button_controller({ix+SX1+SX_CH,iy,	1,	SYT},	"Control" )
			bu:set_target( self, "ctl_out" )
			ui.bu_ctl_out = bu

		bu = self:add_trig(		{ix+SX1+SX_CH+1,iy,		1.5,SYT},		"Clear" )
			bu:set_method_on_click( self, "connect_clear", bu )
			bu.__b_no_connect = true
		bu = self:add_trig(		{ix+6.5,iy+SYT*.25,		1.5,SYT*.5},	"Probe" )
			bu:set_method_on_click( self, "connect_probe", bu )
			bu.__b_no_connect = true
	self:end_bu_group()

-- OUTS
	local nb = self.con_out.nb
	local sep = (nb>9) and " - " or "  \n  "
	for j=1,nb,3 do
		local m = j+2
		self:set_tab_key( j..sep..m  )
		ix,iy = 9,2+DY
		for i=j,m do
			self.con_out[i] = add_connection( true, i )
			iy = iy + DY
		end
	end

--	iy = iy + SY + DY
	-- bu = self:add_selector(	{ix,	iy,		4,SY},	"Midi Channel Out" )
	-- 	bu:set_nb_min_0( 4, 1 )
	-- 	bu:set_draw_by_value( 1, "false" )
	-- 	bu:set_item_text_from_nb_minus_1()
	-- 	bu:set_target( self, "ch_out" )
	-- 	--ui.bu_ch_out = bu
	-- iy = iy + SY
	-- bu = self:add_selector(	{ix,	iy,		8,SY*2},"Midi Controller Out" )
	-- 	bu:set_nb( 8, 2 )
	-- 	bu:set_item_text_from_nb()
	-- 	bu:set_target( self, "ctl_out" )
	-- 	bu:set_text_visible( false )
	-- 	ui.bu_ctl_out = bu

	self:set_tab_key()

	bus_cur:set_method( "draw_after",	self,	"draw_ui_after"	)

	self:set_function_used()
end

function MEU_TRAX:init( )
	local lay = self:get_layer(1)
	--self:print( "got "..aaa.obj.get_down_by_class( lay, "color" ) )

	local ref = self.ref
	local ui = self.ui
	local obj_main = self:__get_obj_main()
	ref.trax = aaa.obj.get_down_by_class( obj_main, "trax" )
	--ref.trax = self:get_obj_down( "fx.01.trax" )
	local trax = ref.trax
		ref.fn 				= param.get_ref( trax, "fn"			)
		ref.trax_output		= param.get_ref( trax, "output"		)
		ref.name 			= param.get_ref( trax, "name"		)
		ref.ch 				= param.get_ref( trax, "channel"	)
		ref.ctl 			= param.get_ref( trax, "control_id"	)
		ref.min 			= param.get_ref( trax, "min"		)
		ref.max 			= param.get_ref( trax, "max"		)
		ref.offset 			= param.get_ref( trax, "offset"		)
		param.set( ref.offset, 0 )
		ref.limit_min 		= param.get_ref( trax, "limit_min"	)
		param.set( ref.min, 0 )
		ref.limit_max 		= param.get_ref( trax, "limit_max"	)
		param.set( ref.max, 1 )
		ref.filter 			= param.get_ref( trax, "filter"		)
		param.set( ref.filter, 0 )
		ref.freq 			= param.get_ref( trax, "freq"		)
		ref.phase 			= param.get_ref( trax, "phase"		)

		ref.gain 			= param.get_ref( trax, "gain"			)
		ref.bias 			= param.get_ref( trax, "bias"			)
		ref.threshold 		= param.get_ref( trax, "threshold"		)
		ref.out_fn_value 	= param.get_ref( trax, "out_fn_value"	)
	--	param.set( trax, ref.gain, .5 )
	--	param.set( trax, ref.bias, .5 )

		--trax (2024 April) is now plug_in to values_00 to values_01

		ref.in_active = {}
		ref.value_in =  {}
		self.in_active_nb = 4
		for i = 1,self.in_active_nb do
			local r = param.get_ref( trax, "in_active_"..aaa.format.int_to_char2(i) )
			ref.in_active[i] = r
			param.set( r, false )
			ref.value_in[i]  = aaa.layers.get_value_ref( obj_main, i )
		end
	
		ref.value_out = aaa.layers.get_value_ref( obj_main, 9 )

	self.scope_in 	= SCOPE:create( self:get_inst_key().."_src" )
--	self.scope_in:set_duration( 2 )

	self.scope_out	= SCOPE:create( self:get_inst_key() )
--	self.scope_out:set_duration( 2 )

	self.con_out = { nb=9 }
	self.con_in = { nb=1 }
end

function MEU_TRAX:draw_icon()
	gol.set_line_width( BU.__draw_text_line_width )
	local x  = -.35
	local dx = .045
	gol.draw_line_strip_2d(	x-dx*2, 0,	x+dx*3,	0		)
	gol.draw_line_strip_2d(	x-dx, .6,	x+dx*3, 0.,		x-dx, -.6	)
end

function MEU_TRAX:connect_midi( bu )
	BUI:__set_do_on_click_next { type="midi",		bu_src=bu, ch=self.ch_out, ctl=self.ctl_out }
end
function MEU_TRAX:connect_clear( bu )
	BUI:__set_do_on_click_next { type="remove",		bu_src=bu, meu=self }
end
function MEU_TRAX:connect_probe( bu )
	BUI:__set_do_on_click_next { type="probe",		bu_src=bu }
end
function MEU_TRAX:connect_trax_meu( bu, con_type, con_id )
	BUI:__set_do_on_click_next { type="trax_meu",	bu_src=bu, meu=self, con_type=con_type, con_id=con_id, b_connect=true }
end
function MEU_TRAX:disconnect_trax_meu( con_type, con_id )
	self:process_connect_request( nil, { con_type=con_type, con_id=con_id } )
end
function MEU_TRAX:__set_c_trax_value_in( id, val )
	return param.set( self.ref.value_in[id], val )
end
function MEU_TRAX:__get_c_trax_value_out()
	return param.get( self.ref.value_out )
end



function MEU_TRAX:__process_connect_request( con_type, con_id, meu_con, bu_con, id_balue )
	--aaa.print_fn()

	local memory = self["con_"..con_type][con_id]
	if meu_con then
		memory.bu_meu:set_text( meu_con:get_name() )
		memory.bu_bu:set_text( bu_con:get_name() )
		memory.bu_id:set_text( id_balue )
	else
		memory.bu_meu:set_text( "No" )
		memory.bu_bu:set_text( "" )
		memory.bu_id:set_text( "" )
	end
end

function MEU_TRAX:process_connect_request( bu_next, con )
	--aaa.print_fn()
	--table.print( con, "con" )

--	local bu_out = self:get_table_always( "bu_out" )
--	local min, max = bu:get_min_max()
--	bu_out[bu] = { bu=bu, min=min, max=max }
	local con_type = con.con_type
	local con_id   = con.con_id
	local memory = self["con_"..con_type][con_id]
	if not memory then
		self:print_error( "don't have table connexion for "..con_type.." "..con_id )
		return
	end

	local bu_prev, id_prev, meu_prev = self:__get_connection_bu_id_meu( memory )

	local meu_next
	if bu_next then
		local mu = bu_next.__mu
		meu_next = mu and mu:get_meu() or bu_next:get_meu_up()
		if not meu_next then
			bu_next:add_dialog_message( "don't find the meu up\nCan't memorize connection" )
			return
		end
	end

	local id_next = con.id_balue
	aaa.print_fn()
	if meu_prev~=meu_next or bu_prev~=bu_next or id_prev~=id_next then
		self:__process_connect_request( con_type, con_id, meu_next, bu_next, id_next )
		local redo = self:make_table_to_call_method( self, "__process_connect_request", con_type, con_id, meu_next, bu_next, id_next )
		local undo = self:make_table_to_call_method( self, "__process_connect_request", con_type, con_id, meu_prev, bu_prev, id_prev )
		ga:add_undo_redo( undo,redo )
	end
end

function MEU_TRAX:disconnect_as_dst( bu )
	local bu_out = self.bu_out
	if bu_out then
		 bu_out[bu] = nil
	end
end

function MEU_TRAX:__get_connection_bu_id_meu( con )
	local meu_name = con.bu_meu:get_value()
	if meu_name ~= "No" then
		--self:print( "meu_name is "..meu_name )
		local meu_con = self:get_meu_by_name_cached( meu_name )
		--self:print( "meu_con is "..meu_con )
		if meu_con and not meu_con.__b_freed then
			local bu_name = con.bu_bu:get_value()
			--hack
			if bu_name == "mu_slider" then
				local mu = meu_con:get_mu()
				if mu then
					return mu:get_slider(), 1, meu_con
				end
			else
				local bu = meu_con:get_bu_by_key( bu_name )
				if bu then
					return bu, tonumber(con.bu_id:get_value()), meu_con
				else
					self:print_debug( "__get_connection_bu_id_meu() No bu for "..bu_name )
				end
			end
		end
	end
end

function MEU_TRAX:__set_connection_value( con )
	--table.print( con, "con", 1 )
	local bu, id, meu = self:__get_connection_bu_id_meu( con )
	if bu then
		local value_prev = bu:get_value_cano( id )
		local v = self.value_cano
		--self:print( bu .. "is now " .. v .. " is_trig is "..bu.is_trig )
		local b_done
		if bu.is_trig and bu:is_trig() then
			if bu:has_fn("click") then
				if v==1 then 
					bu:do_fn( "click" )
					b_done = true
				--bu:set_value_cano( 1, id )
				end
				
			else
				if v==1 then
				else
					b_done = true
				end
			end
			--self:print( bu .. " out is done" )
		end
			-- 	local balue = bu:__get_balue(1)
			-- 	bu:print( id.." klo "..balue.__value_cano.." - "..balue:get_value_cano() )
			-- 	if bu:get_value_cano( id ) == 1 then
			-- 		bu:set_value_cano( 0, id )
			-- 	end
			-- end
		if not b_done then		
			--self:print( bu .. "is now " .. v ) 
			local min,max = con.range_min,con.range_max
			if con.b_inv then min,max = max,min end
			v = min + v * (max - min)
			--self:print( "will change to " .. v ) 
			bu:set_value( v, id )
		end
		local gp = app:get_gp()
		if gp then
			local b_changed = value_prev ~= bu:get_value_cano( id )
			gp:__add_gp_connection( con, b_changed )
			gp:__add_gp_trax( meu, bu, "t_out", b_changed )
		end
	end
end

function MEU_TRAX:__get_connection_value( con )
	local bu, id, meu = self:__get_connection_bu_id_meu( con )
	if bu then
		local v = bu:get_value( id )
		--todo add auto calibration
		local min,max = con.range_min,con.range_max
		if con.b_inv then min,max = max,min end
		if type(v)=="boolean" then
			v = v and 1 or 0
		end
		v = (v-min) / (max - min)

		local gp = app:get_gp()
		if gp then
			local b_changed = self.v_in_last ~= v
			gp:__add_gp_connection( con, b_changed )
			gp:__add_gp_trax( meu, bu, "t_in", b_changed )
			self.v_in_last = v
		end
		return v
	end
end

function MEU_TRAX:set_function_used()
--	aaa.print_fn()
	local ref = self.ref
	local ui = self.ui
	--aaa.print_method()
	--aaa.debug.print_traceback()
	local sym = self:get_bu_item_data( "fn" )
	if sym ~= "" then
		self.sym = sym
		--aaa.print_method( sym )
		--self.b_in_mode = sym == "In"
		-- if self.b_in_mode then
		-- 	param.set( ref.trax_output, "IN" )
		-- else
			local name_trax = self.fn_name_trax[ array.find_index_by_val( self.fn_name_bu, sym ) ]
			param.set( ref.fn, name_trax )
			param.set( ref.trax_output, "FN" )

			local b_midi = (name_trax == "MIDI CONTROLLER") or (name_trax == "MIDI VELOCITY")
			local str = "off"
			if b_midi then
				str = "on"
			end
			ui.bu_ch_in:set_color_back(str)
			ui.bu_ctl_in:set_color_back(str)

			self.b_variable = name_trax == "NAME_REAL"
			ui.bu_name_picker:set_alpha_bu( self.b_variable )
		-- end
	end
end

function MEU_TRAX:update()
	local ref =  self.ref
	local ui = self.ui
	
	local in_active = ref.in_active
	local con_in = self.con_in
	local val
	for i=1,con_in.nb do
		local con = con_in[i]
		if con.b_active then
			val = self:__get_connection_value( con )
			if i == 1 then
				if val then
					self.value_in = val
				end
				if self.b_use_in then
					if not val then
						val = self.value_in
					end
				else
					val = nil
				end
			end
			if val then
				self:__set_c_trax_value_in( i, val )
				param.set( in_active[i], true )
			else
				param.set( in_active[i], false )
			end
		end
	end


	if self.b_variable then
		local name = ui.bu_name_picker:get_text()
		--self:print( "Nmae is "..name )
		param.set( ref.name, name )
	end

	-- now the old c_trax will process
	if self.b_active then
		param.set( ref.ch, self.ch_in )
		param.set( ref.ctl, self.ctl_in )

		if self.b_gb then
			param.set( ref.gain, self.gain )
			param.set( ref.bias, self.bias )
		else
			param.set( ref.gain, .5 )
			param.set( ref.bias, .5 )
		end

		local str
		if self.b_threshold then
			param.set( ref.threshold, self.threshold )
			str = self.fn_trig_trax[self.s_fn_trig]
		else
			str = "FN"
		end
		--aaa.show( str, "fn_th" )
		param.set( ref.trax_output, str )

		aaa.obj.update( ref.trax )


		if self.b_scope_run then
		 	self.scope_in:add( param.get( ref.out_fn_value ) )
		end
	
		-- local function make_f( v )
		-- 	v = math.pow( clamp_01(v) * .if self.b_midi_out then999, .4 )
		-- 	return v
		-- end
		local function make_f( v )
			v = clamp_01(v) * .95
			v = math.pow( v, .5 )
			return v
		end

		--aaa.print_method()
		val = self:__get_c_trax_value_out()

		if self.b_threshold and self.s_fn_trig==5 then
			if val == 1 then
				--self:print( "TRIG" )
				val = self.push_state or 0
				val = 1 - val
				self.push_state = val
			else
				val = self.push_state or 0	
			end
		end

		--self.value_out = val 
		-- if self.b_scope_run then
		-- 	self.scope_in:add( val )
		-- end

		-- if self.b_gb then
		-- 	val = aaa.math.gain_bias( val, self.gain, self.bias )
		-- end

		-- if self.b_th then
		-- 	if val < self.th_min then
		-- 		val = 0
		-- 	elseif self.th_max < val then
		-- 		val = 1
		-- 	end
		-- 	-- 	if self.b_th_min then val = self.th_min end
		-- 	-- elseif val > self.th then
		-- 	-- 	if self.b_th_max then val = self.th_max end
		-- 	-- end
		-- end

	--Avoid NAN propagating
		local v_last = self.v_last or val
		--aaa.mess.show( val.." "..v_last )
		if math.is_nan(v_last) then
			v_last = 0
		end

	--FILTER
		if self.b_filter then
			--self:print( val.." last "..v_last )
			if val > v_last then
				if self.b_inter_up then
					val = interpolate( val, v_last, make_f( self.inter_up   ) )
				end
			else
				if self.b_inter_down then
					val = interpolate( val, v_last, make_f( self.inter_down ) )
				end
			end
		end
		self.v_last = val
	--REMAP
		--	val = math.pow( val, self.gamma )
		if self.b_remap then
			local min,max = self.remap_min,self.remap_max
			if self.b_remap_inv then min,max = max,min end
			val = min + val * (max - min)
		end
	--SCOPE
		if self.b_scope_run then
			self.scope_out:add( val )
		end
		self.value_cano = val
	else	-- not active
		if not val then
			val = self.value_cano
		end
	end


	local con_out = self.con_out
	for i=1,con_out.nb do
		local con = con_out[i]
		if con.b_active then
			self:__set_connection_value( con )
		end
	end
	-- local bu_out = self.bu_out
	-- if bu_out then
	-- 	local v_out = self.trax_out_min + val * (self.trax_out_max - self.trax_out_min)
	-- 	for bu,t in pairs(bu_out) do
	-- 		bu:set_value( v_out )
	-- 	end
	-- end

	--self:print( val )
	--self:print( self.ch_out )
	if self.b_midi_out then
		local min,max = self.midi_out_min,self.midi_out_max
		if self.b_midi_inv then min,max = max,min end
		aaa.midi.set_control( self.ch_out, self.ctl_out, min + val * (max-min) )	--math.sin( aaa.time.t ) )
	end

	--aaa.midi.set_control( 3, 3, .42 )
	-- local trax_out = self.trax_out
	-- if trax_out then
	-- 	MEU.__meu_trax[trax_out] = self
	-- end
end

function MEU_TRAX:get_value_out()
	return self.value_cano
end

function MEU_TRAX:update_ui()
	if self.__b_init_needed then
		self:print( "don't update ui because init was not fully done" )
		return
	end

	local ui = self.ui
	local ref = self.ref

	ui.bu_ch_in:set_text( "Ch "..ui.bu_ch_in:get_value() )

	local str = self.b_midi_out and "on" or "off"
	ui.bu_ch_out:set_color_back(str)
	ui.bu_ctl_out:set_color_back(str)
	ui.bu_ch_out:set_text( "Ch "..ui.bu_ch_out:get_value() )

	--aaa.debug.print_traceback()
	--aaa.print_method( "good object in print" )
	--aaa.print_method()

	ui.bu_info_in:set_text(  self.value_in   and string.format( "%.3f", param.get( ref.out_fn_value ) )  or "" )
	ui.bu_info_out:set_text( self.value_cano and string.format( "%.3f", self.value_cano ) or "" )

--	local str = ""
--	if self.trax_out > 0	then	str = "T"..self.trax_out				end
--	if self.ch_out > 0		then	str = str.." Midi "..self.ch_out.."-"..self.ctl_out	end
--	ui.bu_info_out:set_text( str )

	--self:draw_ui_after()
	--ui.bu_ctl_out:set_active( self.ch_out > 0 )

	if self.b_scope_run then
		local dur = self.scope_duration
		self.scope_in :set_duration(dur)
		self.scope_out:set_duration(dur)
	end
end


function MEU_TRAX:draw_curves( l,b, r,t )
	--if true then return end

	local sx = r-l
	local sy = t-b
		
	gol.push_matrix()
		gol.translate( l, b )
		gol.scale( r-l, t-b )

		gol.set_texture_dim( 0 )

	-- GRID 
		gol.color_cyan( .5 )
		gol.set_line_width( 1 )
		for y=0,1,1/4		do	aaa.draw_line( 0,y, 1,y )	end
		local dx = 1/16
		for x=0+dx,1-dx,dx	do	aaa.draw_line( x,0, x,1 )	end

	-- CURVES
		gol.set_line_width( 2 )
		--for x=,4,1 do
		--	aaa.draw_line( x,0, x,1 )
		--end
		--aaa.draw_rect_line( 0,0, 1,1 )

		gol.color_cyan( self.b_use_in and 1 or .5 )
		--aaa.draw_mul_line(	0,0, 1,1 )
		--aaa.draw_mul_line(	r,t, 1,1 )
		self.scope_in:draw()

		local y
		gol.color_orange( self.b_threshold  and .75 or .4 )
		y = self.threshold 
		aaa.draw_line( 0,y, 1,y )

		gol.color_yellow( self.b_remap and .5 or .25 )
		y = self.remap_min
		aaa.draw_line( 0,y, 1,y )
		y = self.remap_max
		aaa.draw_line( 0,y, 1,y )
--			local min,max = self.remap_min,self.remap_max

		gol.color_yellow()
		self.scope_out:draw( )
		--self.scope_out:draw_speed( .1 )
		--self.scope_out:draw_acc( .01 )
		--aaa.draw_rect_line( 0,0, 5,1 )

		--gol.color_yellow()
		--self.scope_out:draw_acc( .01 )
	gol.pop_matrix()
end

function MEU_TRAX:draw_ui_after()
	self:draw_curves( -.5,-.42,.45,-.15 )
end

function MEU_TRAX:draw()
	self:draw_layers_begin()
		self:draw_layer(1)
		self:draw_curves( -4,0,4,2 )
	self:draw_layers_end()
end
