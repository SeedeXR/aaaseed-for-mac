

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = 1
	local DY = .2

	local SXS = 4
	local SYS = SY * 2

	local osc = self.osc

	self:add_button(     	{ix,iy,			2,SY},	"Send", 	osc,	"b_send",		false	)
	self:add_button(     	{ix+2,iy,		2,SY},	"Verbose", 	osc,	"b_verbose",	false	)
	iy = iy + SY 
--	self:add_trig_method(	{ix,iy,			4,SY},	"OSC init", 	app,	"init_osc"				):set_color_back( "load" )
	ui.bu_osc_info =
	self:add_text_info(		{ix+4,iy,		4,SY},	"OSC Info"		):set_text_align_x( "right" )
	iy = iy + SY

	self:add_trig_method(	{ix,iy,			4,SY},	"Test", 	osc,	"send_mess_test", "Osc Lab"	):set_color_back( "reset" )
	iy = iy + SY + DY

	bu = self:add_text(		{ix,iy,			5,SY},	"Text Value" )
	self:add_trig_method(	{ix+5,iy,		3,SY},	"Text Send", 	self,	"send_str", bu	):set_color_back( "reset" )
	iy = iy + SY + DY

	self:add_slider(		{ix,iy,			5,SY},	"Int32 Value", 	self,	"val_int32",	1,	0,1024	)
		:set_value_type_integer( true )
	self:add_trig_method(	{ix+5,iy,		3,SY},	"Int32 Send", 	self,	"send_int32"	):set_color_back( "reset" )
	iy = iy + SY + DY

	bu = self:add_slider(	{ix,iy,			5,SY},	"Fp32 Value", 	self,	"val_fp32",		0,	-8,8	)
		bu:set_method_on_value_change( self, "update_midi", bu )
	self:add_trig_method(	{ix+5,iy,		3,SY},	"Fp32 Send", 	self,	"send_fp32"	):set_color_back( "reset" )
	iy = iy + SY

	bu = self:add_button(				{ix,iy,		3,	SY},	"MIDI",		self, "b_midi",	false )
	bu = self:add_midi_button_channel(	{ix+3,iy,	2,	SY},	"Channel"	)
		bu:set_text_selector( true )
		bu:set_target( self, "ch_out" )
		ui.bu_ch_out = bu
	bu = self:add_midi_button_controller({ix+5,iy,	1,	SY},	"Control" )
		bu:set_target( self, "ctl_out" )
		ui.bu_ctl_out = bu

--	self:add_trig_method(	{ix+4,iy,		4,SY},	"OSC Quit", 	self,	"send_osc_quit"	):set_color_back( "reset" )
	iy = iy + SY + DY
	bu = self:add_button(			{ix,iy,		3,	SY},	"Frame",	self, "b_frame",	false )
	iy = iy + SY 
	bu = self:add_button(			{ix+1,iy,	3,	SY},	"3 dots",	self, "b_test_a",	false )
	iy = iy + SY 
	bu = self:add_button(			{ix+1,iy,	3,	SY},	"B",		self, "b_test_b",	false )
	iy = iy + SY 
	bu = self:add_button(			{ix+1,iy,	3,	SY},	"C",		self, "b_test_c",	false )
	iy = iy + SY 
	bu = self:add_button(			{ix+1,iy,	3,	SY},	"Random",	self, "b_test_random",	false )
	iy = iy + SY 
end

function meu:update_midi( bu )
	if self.b_midi then
		self.osc:send_midi( self.ch_out, self.ctl_out, bu:get_value() )
	end
end

function meu:send_int32()	self.osc:send_value( "Test int32",	self.val_int32 ) end
function meu:send_fp32()	self.osc:send_value( "Test fp32", 	self.val_fp32  ) end
function meu:send_str( bu )	self.osc:send_value( "Test str",	bu:get_value() ) end
-- function meu:send_osc_quit()
-- 	app:send_osc( "romanite/audio/quit" )
-- end

function meu:init()
--	local ref = self.ref
	self.osc = OSC:create( "OSC_"..self:get_name(), 2 )
end

function meu:update_ui()
	local ui = self.ui
	local osc = self.osc

	ui.bu_osc_info:set_text( osc.count )
end

function meu:send_frame()
	local osc = self.osc
	local frame_id = (self.frame_id or 0) + 1 
	self.frame_id = frame_id
	local name = "test"

	local function fill_tab( nb, ox,oy, s, r )
		local tab = {}
		local ph = aaa.time.t * 2
		local x = -s/2
		local dx = s / (nb-1)
		for i=1,nb do
			local id = (i-1)*3 + 1
			tab[id] = i
			tab[id+1] = ox + x + r * math.cos(ph+i)
			tab[id+2] = oy + 0 + r * math.sin(ph+i)
			x = x + dx
		end
		return tab
	end

	local function fill_random( seed, nb_min,nb_max, ox,oy,oz, s )
		local tab = {}
		math.randomseed( seed )
		local nb = math.floor( nb_min + math.random() * (nb_max-nb_min) )
		for i=1,nb do
			local id = (i-1)*4 + 1
			tab[id] = i
			tab[id+1] = ox + (math.random() - .5) * s
			tab[id+2] = oy + (math.random() - .5) * s
			tab[id+3] = oz + (math.random() - .5) * s
		end
		return tab
	end

	osc:send_ndim_begin( name, frame_id, 2, 3 )
	if self.b_test_a then
		osc:send_ndim_group( name, frame_id, "3 dots", 1, 2, fill_tab( 3,	0,1,	4, .5 ) )
	end
	if self.b_test_b then
		osc:send_ndim_group( name, frame_id, "Test B", 1, 2, fill_tab( 540,	0,0,	6, .2 ) )
	end
	if self.b_test_c then
		local x,sx,dx = -2, 1, .5
		for i=1,4 do 
			osc:send_ndim_group( name, frame_id, "C", i, 2, fill_tab( 64,	x,-1,	sx, .2 ) )
			x = x + sx + dx
		end
	end
	if self.b_test_random then
		local x,sx,dx = -2, 1, .5
		for i=1,4 do 
			osc:send_ndim_group( name, frame_id, "Random", i, 3, fill_random( math.floor( aaa.time.t + i/4 + i*12158), 405,405, x,0,1, dx ) )
			x = x + sx + dx
		end
	end	
			osc:send_ndim_end(   name, frame_id )
end

function meu:update()
	if self.b_frame then
		self:send_frame()
	end

--		app:send_osc( "romanite/volume", self.volume )

-- 	local count = self.s_init
-- 	--self:print( "s_init "..count )
-- 	if count then
-- 		count = count + 1
-- 		if count > 8 then
-- 			self:launch_pd( true )
-- 			self:send_osc_info( "osc_begin" )
-- 			self:send_osc_info( "begin"		)
-- --			self:configure()
-- 			count = nil
-- 		end
-- 		self.s_init = count
-- 	end

end

