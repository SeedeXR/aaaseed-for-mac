function meu:define_meu_infos()
	return { author = "Mâa", date = "2025",
			   tags = { "core", "Tutorial", "Texture", "unfinished" },	--todo need more comments in the code
			   help = 	{	"Control Meu used to shoot Tutorial videos.",
			   				"Using a  midi controller we control a video capture and a \"Slide window\"."
						}
			 }
end

function meu:define_ui()
	local ref   = self.ref
	local ui    = self.ui
	local ix,iy = 1,1
	local SY,DY = 1,.2
	local bu

---[[
	-- self:add_selector(			{ix,iy,			8,SY},		"Transition")
	-- 	:set_item_text( 2, "Fade", "Y Full", "fake Rot", "Y"  )
	-- 	:set_target_lua( self, "s_fx" )
	bu = self:add_button(	{ix,iy,	2,SY}, "What" )
 		bu:set_menu{ 	 "Direct", "Fade", "",
						"X 2", "Hori", "X",
						"Y 2", "Vert", "Y"
					}:set_nb_min_0( 3, 3 )
		bu:set_text_selector(true)
			:set_target_lua( self, "s_fx" )
		
	self:add_slider(			{ix+2,iy,			2,SY},		"Delay",	self,"delay",  .2, 0,1	)				
	iy = iy + 1 + DY

	local SXS = 8/5
	bu = self:add_trig_method(  {ix,		iy,		SXS,SY},	"First",	self,   "set_slide",  1 )
	bu = self:add_trig_method(  {ix+SXS,	iy,		SXS,SY},	"Previous",	self,   "inc_slide", -1 )
		:set_text("<-")
	bu = self:add_text_info(  	{ix+SXS*2,	iy,		SXS,SY},	"Slide Id"	):set_text_align_x( "center" )
		ui.info_slide_id = bu
	bu = self:add_trig_method(  {ix+SXS*3,	iy,		SXS,SY},	"Next",		self,   "inc_slide",  1 )
		:set_text("->")
	bu = self:add_trig_method(  {ix+SXS*4,	iy,		SXS,SY}, 	"Last",		self,   "set_slide", -1 )

	bu = self:add_text_info(  	{ix+4,		iy-SY,	4,SY},		"Slide Nb"	)
		ui.info_slide_nb = bu
	iy = iy + SY + DY
	
	local SXM = 8
	local SYM = SXM * 9/16
	bu = self:add_monitor(		{ix,iy,				SXM,SYM},	"Slide prev" )
		ui.bu_monitor_prev = bu
	bu = self:add_monitor(		{ix+8,iy,			SXM,SYM},	"slide next" )
		ui.bu_monitor_next = bu
	iy = iy + SYM
	bu = self:add_monitor(		{ix+4,iy,			SXM,SYM},	"slide current" )
		ui.bu_monitor = bu
		iy = iy + SYM / 2

	iy = iy + SYM / 2 + DY

	bu = self:add_trig_method(	{ix,iy, 		4,SY}, 		"folder", 	self, "get_folder" )
	iy = iy + SY
	bu = self:add_text(			{ix,iy, 		16,SY}, 	"folder path" )
		--bu:set_target_lua( self, "fpath" ) --todo function don't exist yet 
		ui.bu_folder_path = bu
	bu = self:add_trig_method(  {1,15,   		7,SY},  	"Load All Slides",		self,   "load_slides" )
		:set_color_back("load")
--]]
end

function meu:init()
	local name_to_ctl = {
		slider_1 = 1,
		slider_2 = 2,
		fb = 44,
		ff = 45,
		stop = 43,
		play = 42,
		record = 46,

		marker_set = 61,
		marker_prev = 62,
		marker_next = 63,

		prev = 59,
		next = 60,

		cycle = 47,
	}
	local t = {}
	for name,ctl in PAIRS(name_to_ctl) do
		t[name] = { ctl=ctl, value = -42 }
	end
	self.by_name = t

--	local gp = GP.cur
end

function meu:update()
	local gp = GP.cur

	for _,name in PAIRS{ "F15", "F16" } do
		local mon = gp:get_monitor( name )
		mon:set_text_draw( false )
	end

	local powow = self:get_meu_by_name_cached( "powow_1" )

	local get = aaa.midi.get_control
	for name,elt in PAIRS(self.by_name) do
		--table.print( elt, name )
		local value = get( 1, elt.ctl )
		--self:print( name )
		if elt.value ~= value then
			aaa.show( elt.value.." to "..value, name, nil, 1	 )
			if value == 1 then
				if name == "record" then		
					local mon = gp:get_monitor( "F16" )
					mon:set_window_state( "flip_full" )
					if mon:get_window_state() == "full" then
						mon:move_to_front()
					end
				elseif name == "play" then
					local mon = gp:get_monitor( "F16" )
					mon:set_window_state( "flip_mini" )
					if mon:get_window_state() == "full" then
						mon:move_to_front()
					end
				elseif name == "prev" then
					local mon = gp:get_monitor( "F15" )
					mon:set_window_state( "flip_mini" )
					if mon:get_window_state() == "full" then
						mon:move_to_front()
					end
				elseif name == "next" then	
					local mon = gp:get_monitor( "F15" )
					mon:set_window_state( "flip_full" )
					if mon:get_window_state() == "full" then
						mon:move_to_front()
					end
				elseif name == "marker_next" or name == "marker_prev" then
					local inc = name == "marker_prev" and -1 or 1
					--self:print( "marker_next "..value )
					local m = self:get_meu_by_name_cached( "aaaslide_new" )
					--self:print( "do "..m )
					if m then	m:inc_slide( inc )
					else		self:show_error( "No Meu aaaslide_new", nil, 5. )
					end
					if powow then powow:increment( inc ) end
				elseif name == "marker_set" then
					local m = self:get_meu_by_name_cached( "powow_1" )
					if m then	m:begin()
					else		self:show_error( "No Meu powow_1", nil, 5. )
					end
				elseif name == "fb" or name == "ff" then
					--self:print( "marker_next "..value )
					local m = self:get_meu_by_name_cached( "aaaslide_new" )
					if m then	m:set_slide( name == "fb" and 1 or -1 )
					else		self:show_error( "No Meu aaaslide_new", nil, 5. )
					end
					if powow then powow:inc_video( name == "fb" and -1 or 1 ) end
				end
			end
			if name == "slider_1" or name == "slider_2" then
				local mon = gp:get_monitor( name == "slider_1" and "F16" or "F15" )
				mon:set_alpha_bu( value )		
			end
			elt.value = value
		end
	end
end
