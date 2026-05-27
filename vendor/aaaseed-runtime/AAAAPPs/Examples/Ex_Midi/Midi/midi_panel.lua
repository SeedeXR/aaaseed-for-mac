local function midi_panel_update_after( bus )
	for i=1,0	 do
		local bu = bus.get_bu( i + 8 )
		bu:set_value_cano( math.sin( aaa.time.t*.8	 + i*.8 ) * .5 + .5 )
	end
end
local function bu_set_text( ch, ctl, text )
	local bu = bus_cur:find_bu_by_key_pair( "ch", ch, "ctl", ctl )
	if text and bu then
		bu:set_text( text )
		bu:set_text_draw( true )
		if text == "pal V" then
			bu.use_tex = 255
		elseif text == "Hue" then
			bu.use_tex = 253
		end
	else
		bu:set_text_draw( false )
	end
end
local function tab_set_text( tab, ch_offset, ctl_offset )
	for i = 1, #tab do
		local t = tab[i]
		bu_set_text( t[1]+ch_offset, t[2]+ctl_offset, t[3] )
	end
end
local function midi_pages_define()
	local ratio_x = 1.77
	local taille_x = 3 * ratio_x;
	local taille_y = 3;
	local bu
	local noms

	local bus = BUS:create( "slider_test" )
	bus:init_begin_add_to_ga()

	--START

	--bus:inc_page()

	--keep default value
	--local slider_du_prev = SLIDER.__du
	--SLIDER.__du = .06
	--SLIDER:set_meter( true )
	local sy = 2
	local nb_x = 4
	local nb_y = 1


	BUI:set_meter( true )
	local	NB = 8
	local	XB = -3.8
	local	DX = 1.3
	local	DY = .75
	bu = SLIDER.create_midi_vert(	XB, DX,		1, DY,		NB,		1, 0	)
	bu = SLIDER.create_midi_vert(	XB, DX,		0, DY,		NB,		1, 8	)
	bu = SLIDER.create_midi_vert(	XB, DX,		-1, DY,		NB,		1, 16	)
	bu = SLIDER.create_midi_vert(	XB, DX,		-2, DY,		NB,		1, 24	)

--[[
	--todo restore
	DY = .2
	bu = DIAL.create_midi(			XB, DX,		2-DY, DY,	NB, 1,	2, 0	)
	bu = DIAL.create_midi(			XB, DX,		1-DY, DY,	NB, 1,	2, 8	)
	bu = DIAL.create_midi(			XB, DX,		0-DY, DY,	NB, 1,	2, 16	)
	bu = DIAL.create_midi(			XB, DX,		-1-DY, DY,	NB, 1,	2, 24	)
	BUI:set_meter( false )
]]--
	bus:inc_page()
--]]
	--SLIDER:set_meter( false )
	--bus:inc_page()
	--]]

	--restore previous state
	--SLIDER.__du = slider_du_prev

	bus:make_bus_loop()

	--the scrolling bu is on the bottom
	bus:add_scroll( -4, -2 )

	bus:init_end()
end

if IS_BUSS_OPEN() then
	midi_pages_define()
end

