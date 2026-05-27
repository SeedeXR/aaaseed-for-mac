function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "art", "2d", "coregraphic", "draw", "procedural", "texture", "vj", "unfinished" },
			help = 	{
						"draw a grid textured with a spiral using the input texture.",
						"need to adjust the size/speed UI.",
					}
			}
end

--todo refime the parameter
--todo add a kaledescope mode

function meu:get_preset_nb()	return 16	end
	
function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ui = self.ui
	local SY = 1.
	local DY = .2

	local ix,iy

	self:add_camera()

	ix,iy = 1,1
	self:add_shading_ui(		{ix, iy,		8, SY} )
	local sha = self:get_shading()
	iy = iy + SY + DY

	bu = self:add_button(		{ix,iy,			3,SY },	"Cross",	self, "b_cross", 			false	)
		bu:set_text_rect_ratio( 2.5 )
	bu = self:add_slider(		{ix+3,iy,		5,SY}, 	"Fade",		self, "cross_fade", 		0, -1, 1	)
		bu:set_meter( false )
	bu = self:add_param(		{ix,iy+SY,		4,SY}, 	"Src",		sha:get_ref_frag_src(),		-2, 2		)
		bu:set_meter( false )
		ui.src = bu

	bu = self:add_param(		{ix+4,iy+SY,	4,SY}, 	"Out",		sha:get_ref_frag_out(),		-2, 2		)
		bu:set_meter( false )
		ui.out = bu

	iy = iy+SY+DY
	--	sending stuff was
	--	quick hack done with romzy

	iy =  iy + SY
	bu = self:add_slider(		{ix,iy,			6,SY},	"Decalage",		self, "nb_decal", 2, -16, 16 )
		bu:set_meter( false )
		bu:set_value_type_integer( true )
	local bu_target = bu
	bu = self:add_trig_method(	{ix+6, iy,		1,SY},	"dec_decalage",	bu_target, "add_value_ui", -1	)
		bu:set_text( "-" )
		bu:set_text_inside( true )
	bu = self:add_trig_method(	{ix+7, iy,		1,SY},	"inc_decalage",	bu_target, "add_value_ui", 1	)
		bu:set_text( "+" )
		bu:set_text_inside( true )

	iy =  iy+SY
	bu = self:add_slider(		{ix,iy,			6,SY},	"nb_u",		self, "nb_u", 2, 1, 32 )
		bu:set_value_type_integer( true )
	local bu_target = bu
	bu = self:add_trig_method(	{ix+6, iy,		1,SY},	"dec_u",	bu_target, "add_value_ui", -1	)
		bu:set_text( "-" )
		bu:set_text_inside( true )
	bu = self:add_trig_method(	{ix+7, iy,		1,SY},	"inc_u",	bu_target, "add_value_ui", 1	)
		bu:set_text( "+" )
		bu:set_text_inside( true )
	iy =  iy+SY+DY

	bu = self:add_button( 		{ix, iy,		3,SY},	"U Mirror", self.ref.mapping, "tex_wrap_u" ):set_min_max(2,3)
	bu = self:add_button( 		{ix+3, iy,		2,SY},	"Vu" )
		bu:set_target_param( sha:get_ref_frag_int( 3 )  )
		bu:set_min_max( 0, 1 )
	bu = self:add_button( 		{ix+5, iy,		3,SY},	"V Mirror", self.ref.mapping, "tex_wrap_v" ):set_min_max(2,3)

	iy =  iy+SY+DY
--	bu = self:add_param(		{ix,iy,			8,DY},	"i1",		sha:get_ref_frag_int(1),	1, 32 )
--[[
	iy = iy + DY
	bu = self:add_button(		{ix+1, iy,		DY,DY}, "Time f1", 		self, "b_time_use", false  )
		bu:set_text_rect_ratio( 3.5 )
	bu = self:add_slider(		{ix+3,iy,		5,DY},	"Time_factor",	self, "time_factor",	0, -4, 4 )
		bu:set_meter( false )
		self.time = 0
--]]

	self:add_bu_texture_target_unit(	{ix,iy,	8,6} )
	self:set_bu_texture_preset_use( 1, false )
	--	self.ui.bu_info = self:add_text_info(	{10,2,	6,.8 } )

	ix,iy = 9,2 + DY

	self.f_offset = {}
	self.f_time = {}
	self.f_ph = {}
	ui.float_text = {}
	ui.float_on = {}

	local function get_y(i)	return iy+SY*(i-2) end
	local function add_time_factor( i, sx )
		local y = get_y(i)
		bu = self:add_slider(		{ix,y,		sx,SY},	"f"..i.." time",	self.f_time,	i,		0,	-1, 1 )
			bu:set_meter( false )
			self.time = 0
		self["time_"..i] = 0
	end
	local function add_pixel_slider( i, sx, min, max, b_meter )
		local y = get_y(i)
	--	bu = self:add_button(	{ix, y,	DY, DY }, nil, self, "b_f"..i, true )
		if sx==8 then
			bu = self:add_param(	{ix,y,		sx,SY}, "f"..i,				sha:get_ref_frag_float(i),	min, max	)
		else
			bu = self:add_slider(	{ix+3,y,	sx,SY}, "f"..i,	 			self.f_offset,	i,		0,	min, max )
			self.f_ph[i] = 0
		end
		bu:set_meter( b_meter )
		sha:set_save_frag_float( false, i,i )
		if sx ~= 8 then
			bu = self:add_text_info( {ix+6,y,	2,SY}, "Out_f"..i )
			ui.float_text[i] = bu
			bu:set_method_on_click( self, "reset_phase", i )
			bu:set_text_color( "restart" )
		end
	end
	add_time_factor( 2, 3 )
	add_pixel_slider( 2, 3, -1, 1, false )

	add_pixel_slider( 3, 8, 0, 32, true )
	
	add_time_factor( 4, 3 )
	add_pixel_slider( 4, 3, -1, 1, false )

	add_pixel_slider( 5, 8, 0, 1 )
	--	add_pixel_slider( 6, 8, 0, 32 )


	ix,iy = 9,8.5
	self:set_tab_key( "Main" )
	ui.color = {}
	ui.color[1] = self:add_rgba(	{ix,iy,			nil,SY}, "c1", false )
	ui.color[2] = self:add_rgba(	{ix,iy+SY,		nil,SY}, "c2", false )
 	ui.color[3] = self:add_rgba(	{ix,iy+SY*2,	nil,SY}, "c3", false )

	self:add_transfo_tab( {ix,iy, nil, 3.2} )

 	self:add_blending( {9,12} )
end

function meu:reset_phase( i )
	self.f_ph[i] = 0
end

function meu:add_decal( val )
	local v = self:get_bu_value( "decalage" )
	v = v + val
	self:set_bu_value( "decalage", v )
end

function meu:init()
	local sha = self:add_shading()
--	sha:set_save_frag_float( false,	2,6 )
	sha:set_save_frag_int(   false,	2,3 )
	sha:set_save_frag_vec4(  false,	1,3 )
end

function meu:update()
	local ui = self.ui
	local sha = self:get_shading()
--[[
	if self.b_time_use then
		local t = self.time
		t = t + aaa.time.dt * self.time_factor
		sha:set_frag_float_1 ( t )
		self.time = t
	else
	end
--]]

	local function do_f(i)
		local fac = self.f_time[i]
		local ph = self.f_ph[i]
		if fac~=0 then
			ph = ph + aaa.time.dt * fac
			self.f_ph[i] = ph
		end
		ph = ph + self.f_offset[i]
		ui.float_text[i]:set_text( string.format( "%.2f", ph ) )
		sha:set_frag_float( i, ph )
	end
	do_f(2)
	do_f(4)
	
	sha:set_frag_float( 1, self.nb_u )
	sha:set_frag_int( 2, self.nb_decal )
	--[[
	for i=2,6 do
		b = self["b_f"..i]==0
		ui.float[i]:set_active( not b )
		ui.float[i]:set_visible( not b )
	end
	__]]
	if self.b_cross then
		local c = self.cross_fade * .5 + .5
		ui.src:set_value( 1-c )
		ui.out:set_value( c )
	end

	local alpha = self:get_alpha()
	for i=1,3 do
		local r,g,b,a = ui.color[i]:get_rgba()
		--ui.color[i]:print( i.." "..r.." "..g.." "..b.." "..a)
		sha:set_frag_vec4( i, r,g,b, a*alpha )
	end
end
