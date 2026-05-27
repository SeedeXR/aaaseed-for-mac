if CLASS.DECLARE( "MEU_MIRE", MEU ) then
	MEU_MIRE:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
									"Read video in a texture, the video player",
									"Handle its own incrustation",
									"Generate texture patterns, Color Ramp, test patterns..." )
end

function MEU_MIRE:define_meu_infos( )
	return { author = "Mâa",
				tags = {  "2d", "Texture", "VJ", "Core", "CoreGraphic", "Generator", "Procedural" },
			}
end

function MEU_MIRE:define_ui()
	local ref = self.ref
	local bu
	local par
	local ix,iy = 1,1
	local SY = .8

	ref.bdd = self:get_layer_bdd( 1, "bdd_img_gradient" )
	if not ref.bdd then
		self:box_error( "Mire pb no bdd_img_gradient" )
		return
	end

	self:add_param_obj_name(	{ix,iy,			4,SY},	"Offset",	ref.bdd, "offset_phase",	-1, 8	)
	self:add_param_obj_name(	{ix,iy+1*SY,	8,SY},	"Factor",	ref.bdd, "factor",			0, 64	):add_values_def( 1, 2, 4, 8, .125, .25, .5 )
	self:add_param_obj_name(	{ix,iy+2*SY,	8,SY},	"Factor_v",	ref.bdd, "factor_v",		0, 16	):add_values_def( 1, 2, 4, 8, .125, .25, .5 )
	self:add_param_obj_name(	{ix,iy+3*SY,	8,SY},	"Clamp",	ref.bdd, "clamp",			0, 8	)
	iy = iy + 4*SY
	local sx = 8/3
	self:add_button(			{ix,iy,			sx,SY},	"Circle",	ref.bdd, "curve_circle",	false	)
	self:add_param_obj_name(	{ix+sx,iy,		sx,SY}, "Bias",		ref.bdd, "bias"				)
	self:add_param_obj_name(	{ix+sx*2,iy,	sx,SY}, "Gain",		ref.bdd, "gain"				)
	iy = iy + SY + .2
	
	self:add_bu_texture_target_unit( {ix,iy,	8,6}, "Tex", 1, false	)
	iy = iy + 6.2

	self:add_pixel_size(		{ix,iy, 		4,3},				ref.bdd	)
	self:add_pixel_format(		{ix+4,iy,		4,3},				ref.bdd	)
	iy = iy + 3 + .2

	ix,iy = 9,1
	bu = self:add_selector(	{ix,iy,	8,1.6}, "Mode" )
		--bu:set_text_draw( false )
		--bu.text_u = .22
		--bu.text_v = -1.15
		bu:set_nb_min_0( 10, 2 )
		--bu.do_mouse_move = back_color_do_click_down
		--bu:set_method_on_click(			app, "change_back_color",	bu )
		bu:set_item_text( 1, "Hori", "Vert", "Rad", "SR", "SR2", "SQ", "Lo", "Dot", "Sq", "Lo", "TV", "G8", "G16", "G32", "G64", "G0", "Vert", "Hori", "Dot", "Check" )
		bu:set_target_param( param.get_ref( ref.bdd, "type" ) )
		--bu:set_value_load_save( true )
	iy = 2.6
	bu = self:add_param_obj_name(	{ix,iy,				SY,SY}, "Invert",	ref.bdd, "invert"			)
		bu:set_text_rect_ratio( 2.2 )
	for i=1,3 do
		local short = "C"..i
		bu = self:add_param_obj_name(	{ix+SY*(i*2+.5),iy,		SY,SY}, short.."_active",	ref.bdd, "color_"..i.."_active"	):set_text( short )
			bu:set_text_rect_ratio( 1.2 )
	end


	iy = iy + SY

	local VDY = 2.1
	local function add_color( col_name, col_param_prefix , offset_name, offset_param_name )
		self:add_param_obj_name(	{ix,iy,	 1,VDY}, 	offset_name,	ref.bdd,	offset_param_name, 0, 1 )
		local cref = COLOR_REF:create( col_name, ref.bdd, col_param_prefix )
		self.ui["bu_col_"..string.lower(col_name)]	= self:add_rgbfa(	{ix+1,iy,	7,VDY},		col_name,		cref		)
		iy = iy + VDY
	end

	add_color(		"Begin",	"start_color_",		"Begin_offset",		"start_offset"		)
	for i=1,3 do
		local pre = "color_"..i
		add_color(	 "C"..i,	pre.."_",			"C"..i.."_".."u",	pre.."_".."u"		)
	end
	add_color(		"End",		"stop_color_",		"End_offset",		"stop_offset"		)

	iy = 14
	self:add_param_obj_name(	{ix+2,iy,	SY,SY}, "Hsv",		ref.bdd, "interpolate_hsv"	)
	self:add_param_obj_name(	{ix+5,iy,	SY,SY}, "Nearest",	ref.bdd, "force_nearest"	)
end

function MEU_MIRE:init()
	self.bu_col_name = { "begin", "c1", "c2", "c3", "end" }
end

function MEU_MIRE:set_color_name_rgba( name, r,g,b, a )
	name = "bu_col_"..string.lower(name)
	local bu = self.ui[name]
	table.print( self.ui, "ui", 1 )
	if bu then
		self:print( "color_name is now "..name )
		bu:set_rgba( r,g,b, a )
	end
end

function MEU_MIRE:update()
	local ui =  self.ui
	for i=1,5 do
		ui["bu_col_"..self.bu_col_name[i]]:update()
	end
end

function MEU_MIRE:get_preset_nb() 	return 8	end
