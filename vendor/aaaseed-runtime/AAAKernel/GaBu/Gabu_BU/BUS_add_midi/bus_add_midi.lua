
--
--	MIDI
--
function BUS:add_midi_button( name, rect, nbu,nbv, b_control )
	local bu = self:add_bu(	BUTTON:create( name, rect ) )

	local sel = SELECTOR:create( name.."_selector" )
	sel:set_text_draw( false )
	if b_control then
		sel:set_nb_min_0( nbu, nbv )
		sel:set_item_text_from_nb_minus_1()
	else
		sel:set_nb( nbu, nbv )
		sel:set_item_text_from_nb()
	end
	bu:set_menu( sel )

	bu:set_text_inside( true )
	bu:set_text_selector( b_control )

	return bu
end

local function process_midi_channel_change( bus, bu )
	local ch_new = bu:get_value()
	bu:set_text( "Ch "..ch_new )
	bus:apply_fn_down(
		function(self)
			if self.get_midi then
				local ch,ctl = self:get_midi()
				if ch then
					--self:print( self.." "..ch.."-"..ctl )
					self:set_midi( ch_new, ctl, true )
				else
					--self:print( " rejected" )
				end
			--else
			--	self:print( " rejected" )
			end
		end
	)
end
local function trig_midi_focus( elt )
	aaa.obj.set_focus_ui( elt.obj )
	param.set( elt.obj, "in_enum_trig", true )
	param.set( elt.obj, "out_enum_trig", true )
end
local function flip_midi_active( elt, b_out )
	local t = elt["t_"..(b_out and "out" or "in")]
	param.set( t.open, not param.get_bool(t.opened) )
end
local function draw_midi_slice(self)
	local value = param.get( self.__midi_elt.slice_index_ref )
	local index = value * 8
	self:set_text( "S"..value.."\n"..index.."-"..(index+7) )
	BU_TEXT.draw( self )
end
local function draw_midi_con( self, b_out )
	local t = self.__midi_elt["t_"..(b_out and "out" or "in")]
	if param.get_bool(t.active) then
		local nb = param.get( t.message_nb )
		local d_nb = nb - t.message_nb_last
		if d_nb > 0 then
			self:set_text( d_nb )
			t.message_nb_last = nb
		else
			self:set_text( b_out and "OUT" or "IN" )
		end
		self:set_text_color( param.get_bool(t.opened) and "green" or "red" )
		BU_TEXT.draw( self )
	end
end
local function draw_midi_con_out( self )	draw_midi_con( self, true )		end
local function draw_midi_con_in( self )		draw_midi_con( self, false )	end

--todo should be in BU even a BU_MIDI would be nice with menu to change, preset ...
function BUS:add_midi( rect, name, dx,dy, nb_u,nb_v, b_channel_sel, ... )
	local function get_ref( elt, pre )
		local o = elt.obj
		local t = {}
		t.message_nb = param.get_ref( o, pre.."_message_nb" )
		t.active     = param.get_ref( o, pre.."_active"		)
		t.opened     = param.get_ref( o, pre.."_opened"		)
		t.open 		 = param.get_ref( o, pre.."_open"	)
		t.message_nb_last = 0
		elt[ "t_"..pre] = t
	end

	-- build a midi table
	local midi_nb = 0
	local t = aaa.obj.get_intances_by_class( "midi" );
	table.print( t, "Midi instances" )	
	local t_midi = {}
	for i,o in IPAIRS(t) do
		local fname = aaa.obj.get_filename( o )
		self:print( "filename is "..fname )
		fname = aaa.file.get_name_pure( fname )
		self:print( "filename is "..fname )
		local lc = fname:sub(-1)
		local id =  string.byte(lc) - 96
		midi_nb = midi_nb + 1
		local elt = { obj=o; id=id; letter=string.char(64+id) }
		t_midi[midi_nb] = elt
		get_ref( elt, "in" )
		get_ref( elt, "out" )
	end
	--table.print( t_midi, "Midi table", 3  )

	--local count = select( "#", ... )
	--aaa.box_good( count )
	local bu
	local bus = BUS:create( name )
	rect[4] = rect[4] or rect[3]

	bus:init_begin()

		local sx = rect[3]
		local sy = rect[4]

		if b_channel_sel then
			local SX_CH = .125
			local SX_BANK = .15
			local SY = .06
			local DYB = .01
			local DYT = .004
			local r = {}
			local SX_BAR = SY + DYB + DYT
			rect[2] = rect[2] - SX_BAR
			sy = sy - SX_BAR

			r[2] = rect[2] + SY*.5 + DYB
			r[4] = SY

			if midi_nb > 0 then
				r[1] = rect[1] + SX_BANK*.5
				r[3] = SX_BANK
				bu = bus:add_slider( "Bank", r )
					bu:set_value_type_integer(true)
					bu:set_min_max( 0, 999 )
					bu:set_min_max_strict(true)
					bu:set_value_load_save( true )
					bu:set_show_value( true )
					bu:set_target_param( param.get_ref( t_midi[1].obj, "bank_cur" ) )
			end

			r[1] = rect[1] + SX_CH*.5 + dx + SX_BANK
			r[3] = SX_CH
			bu = bus:add_midi_button( "Channel", r, 2,8, false )
			bu:set_function_on_value_change( process_midi_channel_change, bus, bu )

			local SX_MAIN = .06
			local SX_SLI = .08
			local SX_CON = .06
			local MAR = .005
			local DX = SX_MAIN + SX_CON + SX_SLI + MAR * 2
			local nb = #t_midi
			local x = rect[1] + rect[3] - nb * DX - (nb-1) * MAR

			for i=1,nb do
				local elt = t_midi[i]
				r[2] = rect[2] + SY*.5 + DYB
				r[4] = SY
			-- MIDI LETTER
				r[1] = x + SX_MAIN * .5 
				r[3] = SX_MAIN
				bu = bus:add_trig( elt.letter, r ):set_color_back( "Focus" )
					bu:set_text_inside( true )
					elt.bu = bu
					bu:set_function_on_click( trig_midi_focus, elt )

			-- IN OUT
				r[1] = x + SX_MAIN + MAR + SX_CON*.5
				r[3] = SX_CON
				r[2] = rect[2] + SY*.25 + DYB
				r[4] = SY * .5
				bu = bus:add_text_info( "OUT", r )
					bu.__midi_elt = elt
					bu.draw = draw_midi_con_out
					bu:set_function_on_click( flip_midi_active, elt, true )

				r[2] = r[2] + r[4]
				bu = bus:add_text_info( "IN", r )
					bu.__midi_elt = elt
					bu.draw = draw_midi_con_in
					bu:set_function_on_click( flip_midi_active, elt, false )

			-- SLICE
				r[1] = x + SX_MAIN + MAR + SX_CON + MAR + SX_SLI*.5
				r[3] = SX_SLI
				bu = bus:add_text_info( "Slice", r )
					bu.__midi_elt = elt
					elt.slice_index_ref = param.get_ref( elt.obj, "slice_index" )
					bu.draw = draw_midi_slice
					bu:set_text_align_x( "right" )
					bu:set_function_on_click( trig_midi_focus, elt )

				x = x + DX
			end
		end
		

		local su = (sx - dx * (nb_u-1)) / nb_u
		local sv = (sy - dy * (nb_v-1)) / nb_v
		local nb_slider = 8
		local su_one = su / nb_slider
		local sv_one = sv / nb_slider
		local index = 1

		-- debug
		--SLIDER.create_midi_vert(	rect[1],1,	rect[2],sy,		8, 1, 0 )

		local v	= rect[2] - sv
		for iv = 1,nb_v do
			local u	= rect[1]
			for iu = 1,nb_u do
				local ch	= select( index, ... )
				local ctl	= select( index+1, ... )
				index = index + 2
				--todo store the tab in return value, flat or by group ?
				SLIDER.create_midi_vert( u,su, v,sv,	8, ch, ctl )
				u =	u +	su + dx			
			end
			v =	v - sv - dy
		end
	bus:init_end()

	return self:add_bu( BU:create_window_center( name, rect, bus ) )
end