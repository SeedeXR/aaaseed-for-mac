
function DIOR:define_ui_customer()
	local bus = BUS:create( "DIOR_customer" )
	bus:set_bu_pos_load_save( true )

	bus:set_active( true )
	bus:init_begin_add_to_ga()

		local bu
		--bu = bus:add_window( "Base", self:base_define(),	{0,0, 2,2} )
		--s		bu:set_texture_flip_u( true )
		local SY = 6
		local x = SY*.25 + 1 - .1
		local y = SY*.5 - .2
		bu = bus:add_monitor( "Used",	{-x,y,	SY*.5,SY}, "F7"	)
			--bu:set_texture_flip_u( true )
			bu:set_texture_flip_v( true )
			bu:set_pos_load_save( false )
			bu:set_mobile( false )

		bu = bus:add_monitor( "Src",	{x,y,	SY*.5,SY}, "B" )
			--bu:set_texture_flip_u( true )
			bu:set_texture_flip_v( true )
			bu:set_pos_load_save( false )
			bu:set_mobile( false )

		local x = -2.5
		local sx = 3
		local y = -.5
		local sy = .28
		local D = .04
		local dy = sy + D


		local function add_slider( name, min, max, ax, asx )
			ax = ax or x
			asx = asx or sx
			local bu = bus:add_slider( name, {ax,y, asx-D,sy} )
				name = string.lower(name)
				if min and max then
					bu:set_min_max( min, max )
				end
				bu:add_values_def( 1 )
				bu:disable_uif()
				bu:set_pos_load_save( false )
				bu:set_value_load_save( true )
				bu:set_show_value( true )
				self.ui["bu_"..name] = bu
				y = y - dy
				return bu
		end


		bu = bus:add_button(	"Auto",				{-4+sy*.5,y,	sy,sy} )
			self.b_auto = false
			bu:set_target_lua( self, "b_auto" )
			--bu:set_method_on_click( self, "set_customer_camera_def" )
			bu:set_value_load_save( true )
			--bu:set_pos_load_save( false )
			bu:set_text_rect_ratio( 2.6666-D )
			self.ui.bu_auto = bu
		add_slider( "White", 0, 2, -2, 2 ) -- if i pit 0,2 I get troubles

		bu = bus:add_trig(		"Default",			{-3.5,y,		1-D,sy}  )
			bu:set_method_on_click( self, "set_customer_camera_def" )
		add_slider( "black", nil, nil, -2, 2 )

		y = y - dy
		add_slider( "gain_red",		0,	2 ):set_color_back( "x" )
		add_slider( "gain_green",	0,	2 ):set_color_back( "y" )
		add_slider( "gain_blue",	0,	2 ):set_color_back( "z" )

		y = -.5
		bu = bus:add_button(	"Language",			{-x,y, sx,sy} )
			self:make_lang_button( bu )
			bu:set_pos_load_save( false )
			bu:set_value_load_save( true )

		y = y - dy
		bu = bus:add_trig(		"Load Language",	{-x,y,	sx*.8,sy}	):set_color_back("load")
			bu:set_method_on_click( self, "load_lang" )
			bu:set_pos_load_save( false )

		y = y - dy*1.5
		bu = bus:add_button(	"Experience",		{-x,y,	sx,sy}		)
			self:make_experience_button( bu )
			bu:set_pos_load_save( false )
			bu:set_value_load_save( true )

		y = y - dy * 3
		bu = bus:add_trig(		"Save",				{-x,y,	2,1}		):set_color_back("save")
			bu:set_method_on_click( bus, "save_bu_pos" )
			bu:set_pos_load_save( false )

	bus:init_end()

	return bus
end

function DIOR:set_customer_camera_def()
	local function set( name, val )
		local bu = self.ui["bu_"..name]
		bu:set_value( val )
	end

	set( "black", 		0.077	)
	set( "white",		0.693	)
	set( "gain_red",	1.24	)
	set( "gain_green",	.980	)
	set( "gain_blue",	1.0		)
end

function DIOR:sync_ui_customer()
	local mu = self.mu_texcorrect_cur
	if not mu then return end
	local meu = mu:get_meu_used()
	if not meu then return end

	local ui = self.ui

	local function sync_out( name )
		local bu = ui["bu_"..name]
		local val = bu:get_value()
		meu:set_bu_value( name, val )
	end
	local function sync_in( name )
		local bu = ui["bu_"..name]
		local val = bu:set_value( meu:get_bu_value( name ) )
	end

	ui.bu_white:set_highlight( not self.b_auto )

	sync_out( "auto" )
	--self:print( self.b_auto )
	if self.b_auto then
--		sync_in( "black" )
		sync_in( "white" )
		ui.bu_white:set_color_back( { 1, .5, .5, .5 } )
	else
		sync_out( "white" )
		ui.bu_white:set_color_back()
	end
	sync_out( "black" )
	sync_out( "gain_red")
	sync_out( "gain_green" )
	sync_out( "gain_blue" )
end