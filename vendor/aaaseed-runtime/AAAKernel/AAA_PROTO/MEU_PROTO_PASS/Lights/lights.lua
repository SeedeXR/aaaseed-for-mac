function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "3d", "Core", "CoreGraphic", "draw", "renderpass" },
			help = "this MEU is the UI to edit the lights used in the OpenGL or Gbuffer rendering"
		}
end

function meu:define_light( ix,iy, sx,sy, id )
	local r = self.ref.lights[id]
	local bu
	sy = sy / 6
	bu = self:add_button(		{ix,iy, 		sy,sy},		"Active_"..id,			r.active, nil )
	bu = self:add_slider(		{ix+sx*.5,iy,	sx*.5,sy},	"Intensity"..id,		r.intensity, nil )
	bu = self:add_rgbf(			{ix,iy+sy,		sx,sy}, 	"Ambient_"..id,			r.obj, "ambient_" )
	bu = self:add_rgbf(			{ix,iy+sy*2,	sx,sy}, 	"Diffuse_"..id,			r.obj, "diffuse_" )
	bu = self:add_rgbf(			{ix,iy+sy*3,	sx,sy}, 	"Specular_"..id,		r.obj, "specular_" )
	bu = self:add_sliders_xyz(	{ix,iy+sy*4,	sx,sy},		"Pos_"..id,	r.pos, 	false,	2.5	)
end

function meu:define_light_modern( ix,iy, sx,sy, id )
	local r = self.ref.lights[id]
	local bu
	local pre = "l"..id.."_"
	sy = sy / 5
	bu = self:add_button(	{ix,iy+sy*.1, 	sy*.8,sy*.8},	"L"..id,				r.active, nil, false )
		bu:set_text_rect_ratio( 1.2 )
	bu = self:add_selector(	{ix+sx*2/8,iy,	sx*3/8,sy},		pre.."Type"				):set_text( "Type" )
		bu:set_nb_min_0( 3 )
		bu:set_target_param( param.get_ref( r.obj, "type" ) )
		bu:set_item_text( 1, "Sun", "Point", "Spot" )
		bu:set_text_visible( false )
	-- bu = self:add_slider(	{ix+sx*5/8,iy,	sx*3/8,sy},		pre.."AO_Factor",		r.obj, "Ambient_occlusion" ):set_text( "AO_Factor" )
	bu = self:add_slider(	{ix+sx*5/8,iy,	sx*3/8,sy},		pre.."Gamma",			r.obj, "Gamma", 1, 0.001, 4 ):set_text( "Gamma" )
	iy = iy  + sy
	bu = self:add_trig_fn(	{ix,iy,			sx*1/4,sy}, 	pre.."Focus",			aaa.obj.set_focus_ui, r.obj ):set_text( "Focus" ):set_color_back("focus")
	bu = self:add_rgbf(		{ix+sx/4,iy,	sx*3/4,sy}, 	pre.."Diffuse",			r.obj, "diffuse_" ):set_text( "Diffuse" )
	iy = iy  + sy
--	bu = self:add_rgbf(	{ix,iy+sy*3,	sx,sy}, 	"Specular_"..id,		r.obj, "specular_" )
	bu = self:add_sliders_xyz(	{ix,iy,		sx/4*3,sy},		"Pos",					r.pos, 	false,	2.5, nil, pre	)
	bu = self:add_slider(	{ix+sx*3/4,iy,	sx/4,sy},		pre.."Radius",			r.obj, "radius", 1, 0, 16 ):set_text( "Radius" )
	iy = iy  + sy

	bu = self:add_button(	{ix,iy+sy*.2, 	sy*.6,sy*.6},	pre.."Shadow",			r.obj, "shadow_active", false ):set_text("Shadow")
	bu = self:add_slider(	{ix+sx/3,iy,	sx/3,sy},		pre.."Shadow_Filter",	r.obj, "shadow_filter_width",	1, 0, 16 ):set_text("Filter")
	bu = self:add_slider(	{ix+sx*2/3,iy,	sx/3,sy},		pre.."Shadow_Bias",		r.obj, "shadow_bias",			0, 0, 0.1 ):set_text("Bias")
	iy = iy  + sy

	bu = self:add_button(	{ix,iy+sy*.2, 	sy*.6,sy*.6},	pre.."Intensity",		r.obj, "intensity_map_active", false ):set_text("IntMap")
	bu = self:add_selector(	{ix+sx/3,iy,	sx*2/3,sy},		pre.."Intensity Mode"	):set_text( "Mode" )
		bu:set_nb_min_0( 4 )
		bu:set_item_text( 1, "MUL", "ADD", "PureADD", "PureDIF" )
		bu:set_target_param( param.get_ref( r.obj, "intensity_map_mode" ) )
		bu:set_text_visible( false )
	bu = self:add_slider(	{ix+sx*1/16,iy+sy,		sx*(1/3-1/16),sy},	pre.."Intensity_map_Factor",	r.obj, "intensity_map_factor",			1, 0, 1 ):set_text("Factor")
	bu = self:add_slider(	{ix+sx*1/3,iy+sy,		sx*2/8,sy},			pre.."Intensity_map_Scale", 	self,  "intensity_map_scale_"..id,		1, 0, 4 ):set_text("Size")
	bu = self:add_slider(	{ix+sx*(1/3+2/8),iy+sy,	sx*(1/3-2/8),sy},	pre.."Intensity_map_Ratio", 	self,  "intensity_map_ratio_y_"..id,	1, 0, 4 ):set_text("Ratio_y"):set_color_back("y")
	bu = self:add_slider(	{ix+sx*2/3,iy+sy,		sx*1/6,sy},			pre.."Intensity_map_Offset x",	r.obj, "intensity_map_offset_x",		0, 0, 4 ):set_text("Offset_x"):set_color_back("x")
	bu = self:add_slider(	{ix+sx*5/6,iy+sy,		sx*1/6,sy},			pre.."Intensity_map_Offset y",	r.obj, "intensity_map_offset_y",		0, 0, 4 ):set_text("Offset_y"):set_color_back("y")

	--	bu:set_text_rect_ratio( 2 )
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	local mat_front	= ref.material_front

	local ix, iy = 1.2, 1
	local SX, SY = 5, 1

	bu = self:add_rgbf(	{ix,iy,	SX,SY}, 	"Ambient_global",	ref.lights.obj,		"ambient_global_" )

	ix = 9
	bu = self:add_rgbf(	{ix,iy,	SX,SY},		"Ambient_mat",		mat_front,			"ambient_" )

	iy = 1
	if true then
		SX,SY = 7.5,4.7
		local dx = (16 - SX*2) / 3
		local dy = (15 - iy - SY*2) / 3
		ix,iy = 1+dx, iy+dy-.2
		for i=0,4,4 do
			self:set_tab_key( i==0 and "1234" or "5678" )
			self:define_light_modern( ix,iy, 	 			SX,SY, i+1 )
			self:define_light_modern( ix+SX+dx,iy, 			SX,SY, i+2 )
			self:define_light_modern( ix,iy+SY+dy,			SX,SY, i+3 )
			self:define_light_modern( ix+SX+dx,iy+SY+dy,	SX,SY, i+4 )
		end
	else
		SX,SY = 7.5,3
		local dx = (16 - SX*2) / 3
		local dy = (15 - iy - SY*2) / 3
		ix = 1+dx
		for i=1,4 do
			self:define_light_modern( ix,		iy, 	SX,SY, i )
			self:define_light_modern( ix+SX+dx,	iy, 	SX,SY, i+4 )
			iy = iy + SY
		end
	end

--	self:print( param.get_ref( light_refs[i], "on" ) )
--	table.print(light_params_ref[i])
end

function meu:init()
	local lights = {}	-- refs to 8 light objects
	lights.obj	= aaa.lights.get_cur()
	self.ref.material_front	= aaa.material.get_front_cur()
	--self:print( "material front : "..self.ref.material_front )
	--self:print( self.lights_ref )
	local pos_str = { "x", "y", "z" }
	self.light_nb = 8
	for i=1,self.light_nb do
		local t = {}
		local obj = aaa.lights.get_light( i )
		t.obj = obj
		t.active = param.get_ref( obj, "on" )
		t.intensity = param.get_ref( obj, "Intensity" )
		t.intensity_map_scale_x =  param.get_ref( obj, "intensity_map_scale_x" )
		t.intensity_map_scale_y =  param.get_ref( obj, "intensity_map_scale_y" )
		t.pos = {}
		for j=1,3 do
			t.pos[pos_str[j]] = param.get_ref( obj, "Position_"..pos_str[j] )
			--self:print( "lpos[j] : " .. lpos[pos_str[j]] )
		end
		lights[i] = t
	end
	self.ref.lights	= lights
end


function meu:update()
	local lights_ref = self.ref.lights
--	self.ref.material_front	= aaa.material.get_front_cur()
	--self:print( "material front : "..self.ref.material_front )
	for i=1,self.light_nb do
		local t = lights_ref[i]
		local sca = self["intensity_map_scale_"..i]
		local r_y = self["intensity_map_ratio_y_"..i]
		param.set( t.intensity_map_scale_x, sca )
		param.set( t.intensity_map_scale_y, sca * r_y )
	end
end

function meu:get_preset_nb() 	return 32	end
