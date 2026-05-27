-- this file deal mainly with the Ui (User interface() of the MEU
---  and in particlar the MEu bar (the ui element the top of the MEU bus visible)

--
-- UI SLOT
--

function MEU:set_ui_slot( id )		return self:get_gp():set_meu_ui_slot( self, nil, id )	end
function MEU:get_ui_slot()			return rawget( self, "__slot_id" )						end

function MEU:set_ui_slot_at_start( id )
	if not self.__b_ui_slot_forced then
		self.__b_ui_slot_forced = true
		self:set_ui_slot( id )
	end
end


--
-- UPDATE_UI
--
function MEU:update_ui_proto()	end
function MEU:update_ui()		end

--todo add object
MEU.doc.register_update_ui = "( method, ... ) use this function to egister calls to be done before meu:update_ui()\""..
							"the calls order is the registration order" 
function MEU:register_update_ui( method, ... )
	local tab = self:get_table_always( "__method_update_ui" )
	local elt = { method }
	local args = pack(...)
	if #args > 0 then
		elt[2] = args
	end
	local found_id
	-- for id, elt in IPAIRS(tab) do
	-- 	if method == elt[1] then
	-- 		found_id = id
	-- 	end
	-- end
	if found_id then
		tab[found_id] = elt
	else
		table.insert( tab, elt )
	end
end
function MEU:unregister_update_ui()
	self.__method_update_ui = nil
end
function MEU:__update_ui_registered()
	local tab = self:get_table_always( "__method_update_ui" )
	for _, elt in IPAIRS(tab) do
		local method = elt[1]
		local args   = elt[2]
		if args then self[method]( self, unpack(args) )
		else		 self[method]( self )
		end
	end
end
function MEU:__do_update_ui( mu_ui )
	if not self.__b_meu_update_ui then return end

	if self:is_proto_and_isolated() then
		if GA.b_spy then aaa.spy.push_range( self.." update_ui_proto()", 4 ) end
			self:update_ui_proto()
		if GA.b_spy then aaa.spy.pop_range() end
	else
		--aaa.print_method()
		if GA.b_spy then aaa.spy.push_range( self.." update_ui()", 3 ) end
			self:__update_ui_registered()
			self:update_ui()
		if GA.b_spy then aaa.spy.pop_range() end
	end
	if MEU.__b_timings then
		local timer = self.__timer_render
		--table.print( timer )
		if timer then
			self.__bu_timing:set_text( timer:get_ms_str() )
		end
	end
	local bu_title = self:get_bu_title()
	local mu = self:get_mu()
	local name
	if		mu:is_contact()	then	name = "green"
	elseif	mu:is_bu_cur()	then	name = "magenta"
	else							name = "white"
	end
	bu_title:set_text_color( name )
end

--
-- UIF
--
function MEU:do_uif_command( uif )
	local command = BU:get_uif_name_sel( uif )
	aaa.print_fn( "MEU:do_uif_command()" )

	local b_used = true 
	if		command == "lua"	then	self:edit_lua()
	elseif	command == "focus"	then	self:set_focus( true )
	-- elseif	string.sub(command,1,2) == "ui"	then
	-- 	self:set_ui_slot( uif.zone_sel.index )
	else
		b_used = false 
	end

	return GABU_OBJ.do_uif_command_with_super( self, b_used, uif, MEU )
end



function MEU:set_tab_key_def()	bus_cur:set_tab_key_def()	end
function MEU:set_tab_key( key )	bus_cur:set_tab_key( key )	end
--todo refine 
function MEU:get_tab_key()		return self.__tab_key_cur	end

function MEU:__do_tab_value_changed( bu )
	local v = bu:get_value()
	local key = bu:get_item_text( v )
	if key then
		self.__tab_key_cur = string.lower(key)
		--self:print( "__do_tab_value_changed -> "..v.." "..key )
		local bus = self:get_bus()
		bus:activate_bu_by_tab_key( key )
	end
end

-- MEU Bar definitions
--
local X_END = 16.2
local SXB = 1.3
local Y = -1
local SY = .8
local YB = Y + SY
local DX = .1

function MEU:define_elts_fixed_but( x,y, sy )
	local bu
	-- CLOSE CROSS
	bu = self:add_trig_method(	{X_END,Y+.2, .6,.6}, "ui_close" )
		bu:set_text_draw( false )
		bu:set_method_on_click( self, "__do_close", GP.cur, bu )
		bu.draw = BUTTON.__draw_close
		bu:set_pos_load_save( false )

	local X = X_END - SXB
	local SYF = SY * .95-- for the top buttons, just avoid them being om top of BU_MEU line 
	local YF = Y + SY - SYF 
	bu = self:add_trig_method(	{X,YF,	SXB,SYF},	"Save",			self,"save"				)
		:set_pos_load_save( false )
		:set_color_back( "save" )
		:set_text_inside( true )
		:set_fx_on_click( true )

		-- bu = self:add_trig_method(	{X,YB,	SXB,SY},"Load",			self, "load"			):set_pos_load_save( false )
		-- 	bu:set_color_back( "load" ):set_text_inside( true ):set_fx_on_click( true )


	if not self:is_proto_and_isolated() then	
		local X_SIZE = .6
		X = X - X_SIZE - DX*3
		bu = self:add_trig_method(	{X,YF,	X_SIZE,SYF},	"Next",		self,"__next_meu"			):set_pos_load_save( false )
			bu:set_text( "->" ):set_text_inside( true )
		X = X - X_SIZE
		bu = self:add_trig_method(	{X,YF,	X_SIZE,SYF},	"Prev",		self,"__prev_meu"			):set_pos_load_save( false )
			bu:set_text( "<-" ):set_text_inside( true )

		local sx = 1
		X = X - sx
		if not self:is_class(MEU_FBO) then
			bu = self:add_trig_method(	{X,YF,	sx,SYF},	"FBO",		self,"__open_meu_fbo"	):set_pos_load_save( false )
				bu:set_text( "Fbo" ):set_text_inside( true )
		end
	end

	local SXC = 1
	x = x - SXB*3 - SXC*1
	self:add_trig_method(			{x,y,	SXB,sy},	"Lua",			self,"edit_lua", true	)
		:set_text_inside( true )
		:set_pos_load_save( false )
		:set_fx_on_click( true, 1, .25 )
	x = x + SXB
	self:add_trig_method(			{x,y,	SXC,sy},	"Def",			self,"redefine_bus", true, true	)
		:set_color_back("restart")
		:set_text_inside( true )
		:set_pos_load_save( false )
		--:set_fx_on_click( true )	// don't function because it redefine it for the moment
		:begin_fx( 1, .25 )
	x = x + SXC
	self:add_trig_method(			{x,y,	SXB,sy},	"Both",			self,"redefine_bus", true, false	):set_color_back("load")
		:set_text_inside( true )
		:set_pos_load_save( false )
		--:set_fx_on_click( true )	// don't function because it redefine it for the moment
		--:begin_fx( 2, .25 )
	x = x + SXB
	self:add_trig_method(			{x,y,	SXB,sy},	"Load",			self,"__load_presets_and_recall_0" )
		:set_text_inside( true )
		:set_pos_load_save( false )
		:set_fx_on_click( true, 1, .1 )
	--aaa.debug.print_traceback()
end

-- MEU Bar fns
--
function MEU:__switch_meu( meu_to_use )
	if meu_to_use then
		local slot_id = self:get_ui_slot()
		meu_to_use:set_ui_slot( slot_id )
	end
end
function MEU:__prev_meu()	self:__switch_meu( self.__meu_prev )	end
function MEU:__next_meu()	self:__switch_meu( self.__meu_next )	end

function MEU:__switch_ui_draw_speed( b_fast )
	if b_fast then
		local b_text = TEXT:is_active()
		local bus = self:get_bus()
		local bu = bus:get_bu_up()
		--local x,y = bu:get_pos_draw()

		-- Get mouse position
		local mouse_x,mouse_y = aaa.mouse.get_xy_render()
		-- Convert mouse position to the main coordinate
		mouse_x,mouse_y = EVENT.transform_move_coor( mouse_x,mouse_y )

		-- Set the new mouse position to the local bu eye
		local x,y = bu:convert_xy_bus_up_to_local( mouse_x, mouse_y )
		-- eventually compensate scale
		--mouse_pos_y = mouse_pos_y / self.__scale_eye_y_base
		--aaa.show( x..""..y, self )
		if math.max( math.abs(x),math.abs(y) ) > 1.5 then
			self.__b_text_active_pushed = b_text
			TEXT:set_active( false )
			BU:set_draw_min( true )
		else
			self.__b_text_active_pushed = nil
			BU:set_draw_min( false ) 
		end
	else
		--self:print( "no text "..b )
		--restore drawing_text
		if self.__b_text_active_pushed then
			TEXT:set_active( self.__b_text_active_pushed )
			self.__b_text_active_pushed = nil
		end
		BU:set_draw_min( false )
	end
	--BU:set_text_draw( not b )
end

function MEU:define_elts_fixed( bus, preset_nb, preset_line )
	local ui = self.ui
	local bu

	local SX = 1
	local X = 2.1

	--todo heavy to have this for 3 button on each MEU, do better by defining a BU method
	local function color_off_from_time(self)
		if self:get_value_as_bool() then
			return BU.get_text_color_table(self)
		end
		local v = 2 - triangle_01( aaa.time.t*2. ) * 3
		return { 1, v, v, 1 }
	end



-- WINDOW BAR

-- Send
	local SX = .6
	X = 1
	bu = self:add_button(		{1,YB,	SX,SY},		"Send",				self,"__b_meu_send",		false	)
		:set_text( "S" )
		:set_text_inside( true )
		:set_pos_load_save( false )
		:set_value_load_save( false )
		ui.__bu_meu_send = bu
	X = X + SX + DX

	-- Switch update / draw
	local SXM = .6
	local X_UD
	if not self:is_proto_and_isolated() then	
		bu = self:add_button(	{X,YB,	SXM,SY},	"update_ui",	self,"__b_meu_update_ui",	true	)
			:set_text( "UI" )
			:set_text_inside( true )
			:set_pos_load_save( false )
			:set_value_load_save( false )
			bu.get_text_color_table = color_off_from_time

		X_UD = X + SXM
		X = X_UD + SXM*2 + DX
		bu = self:add_text_info( {X,YB-.02,	3.,SY},	"Timing"	)
			:set_ui_active( false )
			--:set_text_nice()
			:set_text_align_x( "right" )	--todo check it works
			:set_pos_load_save( false )
			:set_value_load_save( false )
			self.__bu_timing = bu
	end
	
	X = 2
-- TITLE
	local SXT = 4.8
	bu = self:add_text(			{X,Y+SY*.1,	SXT,SY*.8},		"title"  )
	--	:set_text( str )
		:set_ui_active( false )
		:set_text_align_x( "center" )
		:set_text_draw_always( true )
		:set_pos_load_save( false )
		:set_value_load_save( false )
		--:set_text_nice()
		bu:set_method_on_click_double( self, "edit_lua" )
		--bu:set_method_on_click_triple( self, "open_dir" )
		ui.__bu_title = bu
		self:__update_name()	-- update name now than we defined the bu title
	X = X + SXT

--tab
	if not self:is_proto_and_isolated() then
		local D = 0
		local SXT =  4.5
		bu = self:add_selector(	{9-SXT/2,Y,	SXT,SY*2-D}, "TAB" )
			ui.__bu_tab = bu
			bu:set_nb( 3 )
			bu:set_item_text_from_nb()
			bu:set_text_draw( false )
			bu:set_pos_load_save( false )
		--	bu:set_item_text_from_nb_minus_1( 2 )
		--	bu:disable_mobile()

			bu:set_method_on_value_change( self, "__do_tab_value_changed", bu )
		--	bu:set_method_on_click_double( self, "__focus_camera" )

	end

-- PRESET
	--aaa.box_good( "add "..preset_nb  )
	if preset_nb and preset_nb>0 then
		self:__define_preset( preset_nb, preset_line )
	end

-- we don't load/save what is before
-- this is just an optimisation could be dealt with individual flag by BU for algo simplication
	local start_to_save = bus:get_bu_nb() + 1

	--here because need to be saved
	if not self:is_proto_and_isolated() then
		bu = self:add_button(	{X_UD,YB,	SXM,SY},	"update",	self,"__b_meu_update",	true	)
			:set_text( "U" )
			:set_text_inside( true )
			:set_pos_load_save( false )
			bu.get_text_color_table = color_off_from_time
		X_UD = X_UD + SXM

		bu = self:add_button(	{X_UD,YB,	SXM,SY},	"draw",		self,"__b_meu_draw", 	true	)
			:set_text( "D" )
			:set_text_inside( true )
			:set_pos_load_save( false )
			bu.get_text_color_table = color_off_from_time
	end

	return start_to_save
end

function MEU:__define_bus( b_init, b_skip_load )
	--aaa.print_fn()
	aaa.spy.push_range( "__define_bus", 22 )

	self.__bu_ui.b_build_preset_needed = true

	local define_ui_name, init_after_name
	local b_message
	local preset_nb, preset_line
	if self:is_proto_and_isolated() then
		--self:box_debug( "test is_proto_isolated" )
		define_ui_name = "define_ui_proto"
		--hack for now 2025 May: we need global flag to be in their default state
		if self.define_ui_proto then
			self:define_ui_proto( false )
		end
		init_after_name = "init_after_proto"
		b_message = false
		preset_nb, preset_line = self:get_preset_nb_proto()
	else
		-- if init failed we will not call it
		if self.__b_init_needed then
			self:print_error( "Can't define_ui fully because init on this MEU was not done" )
		else
			define_ui_name = "define_ui"
			init_after_name = "init_after"
			b_message = false -- 2023 Feb was true
		end
		if b_init then
			--	this require that the mu was already attached
			--	not anymore
			self:__build_refs()		
			local b_ok = self:call_method_checked( "init", true )
			if b_ok then
				self.__b_init_needed = nil
			else
				self.__b_init_needed = true
			end
			self.__b_define_ui_needed = true
		end
		preset_nb, preset_line = self:get_preset_nb()
	end

	--todo push/pop ?
	BU.__scale_min = .125

	--add one more line go to 17 to have 2 lines for MEU generic interface
	local s = 17/16
--	local bus = BUS:create( self:get_name().."_fix" )--
--	bus:set_transfo( 1,s, 0,.5/17 )
--	self.__bus_fix = bus
--	bus:init_begin()
--		
--	bus:init_end( b_skip_load )


	local bus = BUS:create( self:get_name() )
	bus:set_transfo( 1,s, 0,.5/17 )
	bus:set_fname( "AAA_MUS" )

	--we link both ways
	self:set_bus(bus)
	bus.__meu_owner = self

	bus:init_begin()
		self:begin_bu_group( "__meu_bar")
		self:define_elts_fixed_but( X_END,YB, SY )
-- DEFINE MEU BAR
		local index_start_save = self:define_elts_fixed( bus, preset_nb, preset_line )
		self:end_bu_group()

-- SPECIFIC DEFINE (proto or normal for now 2023 Feb)
		if self.__b_define_ui_needed then
			local b_ok = self:call_method_checked( define_ui_name, true )
			if b_ok then
				self.__b_define_ui_needed = nil
			else
				self.__b_define_ui_needed = true
			end
		end

	bus:init_end( b_skip_load )	--to skip load
	--self:print( "after init end "..param.get( self.__boid.ref.repulse.influence ) )
	
	bus.__draw_bu_all =
		function()
			if GA.b_far_draw_less then
				self:__switch_ui_draw_speed(true)
				BUS.__draw_bu_all( bus )
				self:__switch_ui_draw_speed(false)
			else
				BUS.__draw_bu_all( bus )
			end
		end

--	bus:set_method(	"draw_before",	self, "__switch_ui_draw_speed", true )
--	bus:set_method(	"draw_after",	self, "__switch_ui_draw_speed", false )

	self:__build_bu_ui_all( bus, index_start_save )

	local bu = self.ui.__bu_tab	--todo here as in other area we should do it also dynamicly when do changes on the fly
	if bu then	-- we don't have it for the proto_isolated for now
		local tab_key = bus:get_bu_tab_key()
		local nb = #tab_key
		if nb > 1 then
			if nb <= 3 then
				bu:set_nb( nb )
			else
				bu:set_nb( math.ceil(nb/2), 2 )
			end
			bu:set_item_text( 1, unpack(tab_key) )
			bu:set_value( 2 )
			bu:set_value( 1 )
		else
			bu:set_active( false )
			bu:set_value_load_save( false )
		end
	end

	if not b_skip_load then
		self:__load_presets_and_recall_0()
	end

	--	bad idea with folder
	--self:create_alpha_auto()
	aaa.spy.pop_range()

	if init_after_name then
		local method = self[init_after_name]
		if method then
			method(self)
		end
	end
end
