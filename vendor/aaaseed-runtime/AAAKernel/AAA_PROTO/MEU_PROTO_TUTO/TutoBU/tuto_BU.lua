function meu:define_meu_infos( )
	return { author = "Mâa", date="2024",
			tags = { "Core", "Tutorial", "3d", "2d", "draw" },
			help = "Example for main types of BU"
			 }
end

function meu:do_something()
	self:print( "DO SOMETHING" )
end

function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
	local par

	self:add_camera()

	--self:add_bu_texture_target_unit()

	--self:add_rendering()
	--self:add_nb_uv( {1, 12.5}, self:get_layer_bdd(1) )

	local ix = 1
	local iy = 1
	local SY,DY = 1.2,.2
	local DY_SEC = SY * .5

	--we are using a 16x16 coordonate system
	-- starting at top left

	self:set_tab_key( "Base" )

--BUTTON	
	bu = self:add_text_info(	{ix,iy,		5,SY},	"BUTTON" )
	iy = iy + SY + DY

	bu = self:add_button(		{ix,iy,		4,SY},	"ON/OFF" )			-- will flip fron 0 to 1
	bu = self:add_button(		{ix+4,iy,	SY,SY},	"Check", nil,nil,	false) -- will flip from false to true because boolean valeue set here
	
	iy = iy + SY + DY

	bu = self:add_trig(			{ix,iy,		4,SY},	"Trig" ):set_color_back( "restart" )
			:set_function_on_click( aaa.bell )
			:set_fx_on_click( true )

	bu = self:add_button(		{ix+4,iy,	4,SY*2},	"Do" )
			:set_confirmation( true, "yes", "no" )
			:set_method_on_click( self, "do_something" )
			:set_fx_on_click( true )
	iy = iy + SY + DY

	bu = self:add_button(		{ix,iy,		4,SY},	"Mobile" )
			--:set_fx_on_click( true )
			:set_mobile( true )
			--:set_ui_top_size( true )
	iy = iy + SY + DY_SEC

--SLIDER
	bu = self:add_text_info(	{ix,iy,		4,SY},	"SLIDER" )
	iy = iy + SY + DY

	bu = self:add_slider(		{ix,iy,		8,SY},	"Value",	nil, nil,	1,	0,8	)
		:add_values_def( .5,1,2,3,4,6 )
	iy = iy + SY
	bu = self:add_slider(		{ix,iy,		8,SY},	"Value B",	nil, nil,	1,	-8,8 )
	iy = iy + SY
	local SYB = SY * 1.5
	bu = self:add_slider(		{ix,iy,		4,SYB},	"colored",	nil, nil,	.5,	0,1	)
		:set_color_back( "u" )
	bu = self:add_slider(		{ix+4,iy,	4,SYB},	"Gamma",	nil, nil,	1,	0,8	)
		:set_draw_gamma(true)
	iy = iy + SYB + DY

	bu = self:add_slider_two(	{ix,iy,		8,SY},	"Two" )
		bu:set_value( .25, 1 )
		bu:set_value( .75, 2 )
	iy = iy + SY
	bu = self:add_slider_two(	{ix,iy,		8,SY},	"Two Linear" )
		:set_draw_step( "linear" )
		bu:set_value( .25, 1 )
		bu:set_value( .75, 2 )
	iy = iy + SY + DY

--SELECTOR
	ix,iy = 9,2	
	bu = self:add_text_info(	{ix,iy,		6,SY},	"SELECTOR" )
	iy = iy + SY + DY*2

	bu = self:add_selector(		{ix,iy,		8,SY},	"Mode" )
		:set_item_text( 2, "Yes", "NO", "1", "2", "3", "4" )
	iy = iy + SY + DY*2

	bu = self:add_selector(		{ix,iy,		8,SY},	"Multiple" )
		:set_item_text( 1, "A", "B", "C", "D" )
		:set_selection_multiple( true )
	iy = iy + SY + DY_SEC

--TEXT
	bu = self:add_text_info(	{ix,iy,		3,SY},	"TEXT" )

	iy = iy + SY + DY

	bu = self:add_text_info(	{ix,iy,		4,SY},	"Info" )
	iy = iy + SY + DY/2

	bu = self:add_text(			{ix,iy,		8,SY},	"Edit" )
	iy = iy + SY + DY/2


	self:set_tab_key( "More" )

--MORE SLIDER
	ix,iy = 1,1
	bu = self:add_text_info(	{ix,iy,		8,SY},	"MORE SLIDER" )
	iy = iy + SY + DY

	local SYS = 4
	bu = self:add_slider_xy(	{ix,iy,		8,SYS},	"XY" )
		bu:set_value( .5, 1 )
		bu:set_value( .5, 2 )
	iy = iy + SYS + DY

	bu = self:add_slider_multi(	{ix,iy,		8,SYS},	"MULTI",	4 )
		bu:set_ui_top_size( false )
		bu:set_select_on_click_double()
		bu:set_value( .5, 1 )
		bu:set_value( .5, 2 )
		bu:set_elt_text_xy_f_ratio( -.8,0, .5 )
	iy = iy + SYS + DY

	bu = self:add_slider_multi_curve({ix,iy,8,SYS},	"MULTI_CURVE",	4 )
		bu:set_ui_top_size( false )
		bu:set_value( .5, 1 )
		bu:set_value( .5, 2 )
	iy = iy + SYS + DY

--COMPOSITE BU
	ix,iy = 9,2
	bu = self:add_text_info(	{ix,iy,		8,SY},	"Composite BU" )
	iy = iy + SY + DY

	self:add_blending(			{ix,iy,		2,SY} )
	iy = iy + SY + DY

	bu = self:add_button(		{ix,iy,		4,SY}, 	"What",	self, "s_what",     1   )
		bu:set_multiple( { "Option A" , "Oui", "Non" }, "what" )


	self:set_tab_key( "..." )
--BU_COLOR
	ix,iy = 1,1
	bu = self:add_text_info(	{ix,iy,		6,SY},	"BU_COLOR" )
	iy = iy + SY + DY
	bu = self:add_rgbf( 		{ix,iy,		8,2},	"RGBF"	)
	iy = iy + 2
	bu = self:add_rgb( 			{ix,iy,		8,2},	"RGB",		false	)
	iy = iy + 2 + DY
	bu = self:add_rgbfa( 		{ix,iy,		8,2},	"RGBFA",	false	)
	iy = iy + 2
	bu = self:add_rgba( 		{ix,iy,		8,2},	"RGBA",		false	)
	iy = iy + 2 + DY

--BU_TEXTURE
	ix,iy = 9,2
	bu = self:add_text_info(				{ix,iy,		7,SY},	"BU_TEXTURE" )
	iy = iy + SY + DY
	bu = self:add_bu_texture_target_layer( 	{ix,iy,		8,5},	"Texture", 	1,true,	self:get_layer(1),self:get_layer_mapping(1) )
	iy = iy + 6 + DY
	bu = self:add_bu_texture( 				{ix,iy,		8,4},	"Other", 	2 )
	iy = iy + 4 + DY
end

function meu:init()
end

function meu:draw()
	local alpha = self:get_alpha()
	gol.color_white( alpha )
	MEU.draw( self )
end

