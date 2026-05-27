--[[ 3D object set up like a new user (designer) would imagine, Priam said so aha.
it's essentailly owner in many respects, all gates 
]]--






------------DRAW
function meu:draw()
    self:draw_layers_begin()
        --  self:print( aaa.time.t )
        self:draw_layer( 1 )
		
		self:draw_layer( 2 ) --font -- PUSH transform (PUSH is ON, POP is OFF)

		--self:draw_2( self:get_alpha(), 10, false )      -- line drawing attached to InfoTag -- choose the drawing function here
		self:draw_layer( 3 )  -- 3D primitive
		self:draw_1( self:get_alpha(), 10, false )      -- line drawing hex grid (check its overrides in gol.xx)
		self:draw_layer( 4 ) ---- POP transform (PUSH is OFF, POP is ON)
 
    self:draw_layers_end()
end

------------DEFINE
function meu:define_ui()
	local param_set = param.set

	local ref = self.ref
	local ui = self.ui
	local bu
    local par

	local ix = 1
	local iy = 1
    local ix,iy = 1,1
    local SY,DY = 1,.2  
	local sy = 1
	-- local ramp_info = {}
    -- self.ramp = ramp_info
	local ramp_info2 = {}
	self.ramp2 = ramp_info2
	local ramp_info3 = {}
	self.ramp3 = ramp_info3
	local ramp_info4 = {}
	self.ramp4 = ramp_info4
	local ramp_info5 = {}
	self.ramp5 = ramp_info5
	local ramp_info6 = {}
	self.ramp6 = ramp_info6


	local layerS = self:get_layers()
	ref.layers_trs_use = param.get_ref( layerS, "transfo" )
	local transfoS = aaa.obj.get_branch_by_class_no_error( layerS, "transfo_trs" )
	local traS = {}
	traS.x, traS.y, traS.z	=	param.get_ref( transfoS, "translate_x" ), param.get_ref( transfoS, "translate_y" ), param.get_ref( transfoS, "translate_z" )
	ref.traS = traS

	local layerA =	self:get_layer(1)
	-- NOW you have to manually switch tranfo_trs / active to ON, it's inaccessible by code as far as i tried for 1h, P
	-- ONLY when you manually switch tranfo_trs / active to ON, is the below going to work:
	local transfo		=	aaa.obj.get_down_by_class	( layerA, "transfo_trs" )

	local matFront = param.get_obj_attached( layerA, "material_front") --[[ VERY IMPORTANT, this is how you get the ATTACHED "material_front" object, 
	oh, and if you set the alpha or else, you're acting on the REAL material out there that is shared, so careful mixing up mats.., P ]]-- 
	
	local tra = {}
	tra.x, tra.y, tra.z	=	param.get_ref( transfo, "translate_x" ), param.get_ref( transfo, "translate_y" ), param.get_ref( transfo, "translate_z" )
	ref.tra = tra
	param_set(  transfo, "active", true )
	param_set(  layerA, "trs_1", 2 )
	param_set(  transfo , "translate", true )
	param_set(  transfo , "rotate", true )
	param_set(  transfo , "scale", true )

	

	local layerB =	self:get_layer(2)
	-- NOW you have to manually switch tranfo_trs / active to ON, it's inaccessible by code as far as i tried for 1h, P
	-- ONLY when you manually switch tranfo_trs / active to ON, is the below going to work:
	local transfoB		=	aaa.obj.get_down_by_class	( layerB, "transfo_trs" )
	local traB = {}
	traB.x, traB.y, traB.z	=	param.get_ref( transfoB, "translate_x" ), param.get_ref( transfoB, "translate_y" ), param.get_ref( transfoB, "translate_z" )
	ref.traB = transfoB
	param_set(  transfoB, "active", true )
	param_set(  layerB, "trs_1", 2 )
	param_set(  transfoB , "translate", true )
	param_set(  transfoB , "rotate", true )
	param_set(  transfoB , "scale", true )


	local lights = aaa.obj.get_down_by_class( layerA, "lights_switch" )
	local mappingRef = aaa.obj.get_down_by_class ( layerA, "mapping" )
	param_set(  mappingRef , "camera_name_symbo", "hahaggg" )

	local layerC =	self:get_layer(3)


-------------TAB 1 
self:set_tab_key( "Main" )

	self:add_text_info(	{ix  ,iy,	8,1}, "Main Settings" )

	bu = self:add_camera(		{nil,iy+2,	nil,1.8}, 16 )
	-- bu = self:add_rgbfa(         nil,	"color_1" ) 
-- ---------------SAMPLE CODE
bu = self:add_selector(	{ix,iy+2,	1.4,sy+.8}, "UseMultiplexxxxxxx")
--------------END OF SAMPLE CODE

	bu:set_nb( 1, 3 )
	bu:set_item_text( 1, "Current", "None", "Owner" )
	bu:set_target_obj_param( layerA, "use_multiple" )
	bu:set_text_draw( true )

	self:add_slider({ix, iy+7, 4, SY}, "ScaleX", param.get_ref( transfoS, "scale_x" ), nil, 1, 0, 20):set_color_back("u")
    self:add_slider({ix, iy+8, 4, SY}, "ScaleY", param.get_ref( transfoS, "scale_y" ), nil, 1, 0, 20):set_color_back("v") 
	self:add_slider({ix, iy+9, 4, SY}, "ScaleZ", param.get_ref( transfoS, "scale_z" ), nil, 1, 0, 20):set_color_back("w") 
	self:add_slider({ix, iy+4, 4, SY}, "TranslateX", traS.x, nil, 0, 0, 8):set_color_back("u") -- just so it's shorter
	self:add_slider({ix, iy+5, 4, SY}, "TranslateY", traS.y, nil, 0, 0, 8):set_color_back("v") -- just so it's shorter
	self:add_slider({ix, iy+6, 4, SY}, "TranslateZ", traS.z, nil, 0, 0, 8):set_color_back("w") -- just so it's shorter
	self:add_slider({ix, iy+10, 4, SY}, "RotateX", param.get_ref( transfoS, "rotate_x" ), nil, -1.0, 0, 1.0):set_color_back("u") 
    self:add_slider({ix, iy+11, 4, SY}, "RotateY", param.get_ref( transfoS, "rotate_y" ), nil, -1.0, 0, 1.0):set_color_back("v")
	self:add_slider({ix, iy+12, 4, SY}, "RotateZ", param.get_ref( transfoS, "rotate_z" ), nil, -1.0, 0, 1.0):set_color_back("w") 
	self:add_slider({ix, iy+13, 4, SY}, "ScaleFactor", param.get_ref( transfoS, "scale_factor" ), nil, 1, 0, 8):set_color_back("black") 

	-------------TAB 2
self:set_tab_key( "Plane" )

	self:add_text_info(	{ix  ,iy ,	8,1}, "Background Map" )
	self:add_rendering()
-------------CUSTOM
	self:add_trig_method(	{ix+11,iy+.1,		4,SY},	"Load Model", 		self, "load_model"	):set_color_back( "load" )

 -------------BUILT-INS 

	local iy = iy + 1
	self:add_slider({ix, iy+1, 4, SY}, "ScaModelX", param.get_ref( transfo, "scale_x" ), nil, 16, 0, 20):set_color_back("u") 
    self:add_slider({ix, iy+2, 4, SY}, "ScaModelY", param.get_ref( transfo, "scale_y" ), nil, 9, 0, 20):set_color_back("v") 
	self:add_slider({ix, iy+3, 4, SY}, "ScaModelZ", param.get_ref( transfo, "scale_z" ), nil, 1, 0, 20):set_color_back("w") 
	self:add_slider({ix, iy+4, 4, SY}, "TraModelX", tra.x, nil, 0, 0, 8):set_color_back("u") -- just so it's shorter
	self:add_slider({ix, iy+5, 4, SY}, "TraModelY", tra.y, nil, 0, 0, 8):set_color_back("v") -- just so it's shorter
	self:add_slider({ix, iy+6, 4, SY}, "TraModelZ", tra.z, nil, 0, 0, 8):set_color_back("w") -- just so it's shorter
	self:add_slider({ix, iy+7, 4, SY}, "RotModelX", param.get_ref( transfo, "rotate_x" ), nil, 0, 0, 8):set_color_back("u") 
    self:add_slider({ix, iy+8, 4, SY}, "RotModelY", param.get_ref( transfo, "rotate_y" ), nil, 0, 0, 8):set_color_back("v")
	self:add_slider({ix, iy+9, 4, SY}, "RotModelZ", param.get_ref( transfo, "rotate_z" ), nil, 0, 0, 8):set_color_back("w") 
	self:add_slider({ix, iy+10, 4, SY}, "ScaModelFactor", param.get_ref( transfo, "scale_factor" ), nil, 1, 0, 8):set_color_back("black") 
	self:add_slider({ix+8, iy+1, 8, SY}, "MaterialFront", param.get_ref( layerA, "material_front" ), nil, 32, 0, 127):set_color_back("y") 
	self:add_slider({ix+8, iy+2, 8, SY}, "MaterialFrAlpha", param.get_ref( matFront, "diffuse_alpha" ), nil, 1, 0, 1):set_color_back("y") --[[ VERY IMPORTANT, this is getting the ATTACHED "material_front" object, 
	oh, and if you set the alpha or else, you're acting on the REAL material out there that is shared, so careful mixing up mats.., P ]]-- 


	bu = self:add_button({ix+4, iy+1, SY, SY}, "UseLight1", lights, "light_switch_0", false):set_text_visible(true) --Checkmark button
   	-- bu = self:add_button( {	ix+4,	iy,		4,SY },	"light0",	lights, "light_switch_0", false	):set_text_visible(true) --regular ON-OFF button
	bu = self:add_button({ix+4, iy+2, SY, SY}, "UseLight2", lights, "light_switch_1", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+3, SY, SY}, "UseLight3", lights, "light_switch_2", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+4, SY, SY}, "UseLight4", lights, "light_switch_3", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+5, SY, SY}, "UseLight5", lights, "light_switch_4", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+6, SY, SY}, "UseLight6", lights, "light_switch_5", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+7, SY, SY}, "UseLight7", lights, "light_switch_6", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+8, SY, SY}, "UseLight8", lights, "light_switch_7", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+9, SY, SY}, "MaterialUseColor", layerA, "material_use_color", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+10, SY, SY}, "USELIGHTS", layerA, "use_lights_switch", false):set_text_visible(true) --Checkmark button
	
-----------TAB 3
self:set_tab_key( "Map" )

	param_set( mappingRef, "tex_factor", 1 )
	param_set( mappingRef, "tex_coor_centered", false )
	self:add_text_info(	{ix  ,iy-1 ,	8,1}, "Map Settings" )
	self:add_bu_texture_target_unit( {ix + 8 ,iy+1, 8,6} )
	self:add_trig_method(	{ix+11,iy-.9,		4,SY},	"load Tex", 		self, "load_texture"	):set_color_back( "load" )

	self:add_slider({ix, iy+1, 4, SY}, "texHeight", param.get_ref( mappingRef, "tex_u" ), nil, .09, 0, 1):set_color_back("u") 
    self:add_slider({ix, iy+2, 4, SY}, "texWidth", param.get_ref( mappingRef, "tex_v" ), nil, .16, 0, 1):set_color_back("v") 


-----------TAB 4 ---FONTS

self:set_tab_key( "InfoTag" )

self:add_text_info(	{ix  ,iy -1,	8,1}, "Title" )

local bu = self:add_rgbfa_only(	{ix+8  ,iy +10,	8,1}, 		"TYPEFACE",	aaa.obj.get_down_by_class( layerB, "color" ), false )



iy = iy + SY + DY

self:add_slider({ix, iy+4, 4, SY}, "TraInfoX", traB.x, nil, 0, 0, 8):set_color_back("u") -- just so it's shorter
self:add_slider({ix, iy+5, 4, SY}, "TraInfoY", traB.y, nil, 0, 0, 8):set_color_back("v") -- just so it's shorter
self:add_slider({ix, iy+6, 4, SY}, "TraInfoZ", traB.z, nil, 0, 0, 8):set_color_back("w") -- just so it's shorter
self:add_slider({ix, iy+10, 4, SY}, "ScalInfoX", param.get_ref( transfoB, "scale_x" ), nil, 1, 0, 8):set_color_back("u") 
self:add_slider({ix, iy+11, 4, SY}, "ScalInfoY", param.get_ref( transfoB, "scale_y" ), nil, 1, 0, 8):set_color_back("v") 
self:add_slider({ix, iy+12, 4, SY}, "ScalInfoZ", param.get_ref( transfoB, "scale_z" ), nil, 1, 0, 8):set_color_back("w") 
self:add_slider({ix, iy+7, 4, SY}, "RotInfoX", param.get_ref( transfoB, "rotate_x" ), nil, 0, 0, 8):set_color_back("u") 
self:add_slider({ix, iy+8, 4, SY}, "RotInfoY", param.get_ref( transfoB, "rotate_y" ), nil, 0, 0, 8):set_color_back("v")
self:add_slider({ix, iy+9, 4, SY}, "RotInfoZ", param.get_ref( transfoB, "rotate_z" ), nil, 0, 0, 8):set_color_back("w") 


bu = self:add_selector( {ix+8,iy+4,	8,4.4}, "Font" )
bu:set_nb_min_0( 8, 8 )
bu:set_item_text_from_nb()
bu:set_target_param( ref.font )
bu:set_method_on_click_double( self, "load_font" )
ui.bu_font_selector = bu

bu = self:add_text_info( {ix,iy,	16,SY},	"Font Name" ):set_color_back("w")
ui.font_name = bu
iy = iy + SY + DY

bu = self:add_text( {1,iy, 16,SY}, "text" )
bu:set_text( param.get( self.ref.text ) )
--todo this don't function
--bu:set_target_param( ref.text )
ui.bu_text = bu 
iy = iy + SY + DY

-----------TAB 5 ---GRID
-- this hex grid is alignet to the Lentille's grid, see defaults of ZFCT and XFCT (they are also different from the Lentille's ZFCT and XFCT, no fn idea why), P
self:set_tab_key( "Grid" )
local ix,iy = 1, 1

self:add_text_info(	{ix  ,iy ,	8,1}, "Hex Grid" )

local bu = self:add_rgbfa_only(	{ix+8  ,iy +5,	8,1}, 		"GRID",	aaa.obj.get_down_by_class( layerC, "color" ), false )

local bu = self:add_slider( {ix,iy+2, 8,SY}, "Nunits", self, "N", 300,  0,1500 )
-- bu = self:add_slider( {ix,iy+3, 8,SY}, "Radius", ramp_info, "rampOut", .1,  0, 1 ) -- samw param as "Ramp Out" ANIMATION TAB slider
bu = self:add_slider( {ix+8,iy+3, 8,SY}, "RadFact", self, "RR", 30,  1, 100 ) -- cuz the  "Ramp Out" ANIMATION TAB slider is generic and normalized we need a factor here to tweak the range we need in real 3D space
bu = self:add_slider( {ix,iy+4, 8,SY}, "Zfactor", self, "ZFCT", -.15,  -1, 1 ) 
bu = self:add_slider( {ix+8,iy+4, 8,SY}, "Xfactor", self, "XFCT", 0,  -1, 1 ) 
bu = self:add_slider( {ix,iy+5, 8,SY}, "GridAlpha", self, "GAL", 0,  0, 1 )
bu = self:add_slider( {ix,iy+6, 8,SY}, "GridR", self, "GR", 1,  0, 1 ):set_color_back("u")
bu = self:add_slider( {ix,iy+7, 8,SY}, "GridG", self, "GG", 1,  0, 1 ):set_color_back("v")
bu = self:add_slider( {ix,iy+8, 8,SY}, "GridB", self, "GB", 1,  0, 1 ):set_color_back("w")
bu = self:add_slider( {ix,iy+10, 8,SY}, "Nana", app, "strataDateNew", 3,  0,20 )


-----------TAB 6 ---ANIMATION

self:set_tab_key( "Animations" )
local ix,iy = 1, 1
self:add_text_info(	{ix  ,iy ,	8,1}, "Animations" )
---RAMP 1
-- bu = self:add_button({ix, iy + 1, SY, SY}, "on", self, "active", true):set_text_visible(true)
-- bu = self:add_button({ix, iy + 2, SY, SY}, "Loop", ramp_info, "loop_ramp", true):set_text_visible(true)
-- bu = self:add_slider_two({ix+3, iy + 1, 6, SY}, "Timer Range", ramp_info, "min", "max", 0, 1, 0, 1):set_color_back("u") 
-- bu = self:add_slider({ix+10, iy + 1, 6, SY}, "Timer Time", ramp_info, "time", 5, 0, 10):set_color_back("u") 
-- bu = self:add_slider({ix+4, iy + 2, 12, SY}, "Timer Out", ramp_info, "rampOut", 0, ramp_info.min, ramp_info.max):set_color_back("u") 
---RAMP 2
bu = self:add_button({ix, iy + 3, SY, SY}, "On2", app, "active2", true):set_text_visible(true)
bu = self:add_button({ix, iy + 4, SY, SY}, "Loop2", ramp_info2, "loop_ramp2", true):set_text_visible(true)
bu = self:add_slider_two({ix+3, iy + 3, 6, SY}, "Ramp Range2", ramp_info2, "min2", "max2", 0, 1, 0, 1)
bu = self:add_slider({ix+10, iy + 3, 6, SY}, "Ramp Time2", ramp_info2, "time2", 5, 0, 10)
bu = self:add_slider({ix+4, iy + 4, 12, SY}, "Ramp Out2", ramp_info2, "rampOut2", 0, ramp_info2.min2, ramp_info2.max2)
---RAMP 3
bu = self:add_button({ix, iy + 5, SY, SY}, "On3", self, "active3", true):set_text_visible(true)
bu = self:add_button({ix, iy + 6, SY, SY}, "Loop3", ramp_info3, "loop_ramp3", true):set_text_visible(true)
bu = self:add_slider_two({ix+3, iy + 5, 6, SY}, "Ramp Range3", ramp_info3, "min3", "max3", 0, 1, 0, 1)
bu = self:add_slider({ix+10, iy + 5, 6, SY}, "Ramp Time3", ramp_info3, "time3", 5, 0, 10)
bu = self:add_slider({ix+4, iy + 6, 12, SY}, "Ramp Out3", ramp_info3, "rampOut3", 0, ramp_info3.min3, ramp_info3.max3)
---RAMP 4
bu = self:add_button({ix, iy + 7, SY, SY}, "On4", self, "active4", true):set_text_visible(true)
bu = self:add_button({ix, iy + 8, SY, SY}, "Loop4", ramp_info4, "loop_ramp4", true):set_text_visible(true)
bu = self:add_slider_two({ix+3, iy + 7, 6, SY}, "Ramp Range4", ramp_info4, "min4", "max4", 0, 1, 0, 1)
bu = self:add_slider({ix+10, iy + 7, 6, SY}, "Ramp Time4", ramp_info4, "time4", 5, 0, 10)
bu = self:add_slider({ix+4, iy + 8, 12, SY}, "Ramp Out4", ramp_info4, "rampOut4", 0, ramp_info4.min4, ramp_info4.max4)
---RAMP 5
bu = self:add_button({ix, iy + 9, SY, SY}, "On5", self, "active5", true):set_text_visible(true)
bu = self:add_button({ix, iy + 10, SY, SY}, "Loop5", ramp_info5, "loop_ramp5", true):set_text_visible(true)
bu = self:add_slider_two({ix+3, iy + 9, 6, SY}, "Ramp Range5", ramp_info5, "min5", "max5", 0, 1, 0, 1)
bu = self:add_slider({ix+10, iy + 9, 6, SY}, "Ramp Time5", ramp_info5, "time5", 5, 0, 10)
bu = self:add_slider({ix+4, iy + 10, 12, SY}, "Ramp Out5", ramp_info5, "rampOut5", 0, ramp_info5.min5, ramp_info5.max5)
---RAMP 6
bu = self:add_button({ix, iy + 11, SY, SY}, "On6", self, "active6", true):set_text_visible(true)
bu = self:add_button({ix, iy + 12, SY, SY}, "Loop6", ramp_info6, "loop_ramp6", true):set_text_visible(true)
bu = self:add_slider_two({ix+3, iy + 11, 6, SY}, "Ramp Range6", ramp_info6, "min6", "max6", 0, 1, 0, 1)
bu = self:add_slider({ix+10, iy + 11, 6, SY}, "Ramp Time6", ramp_info6, "time6", 5, 0, 10)
bu = self:add_slider({ix+4, iy + 12, 12, SY}, "Ramp Out6", ramp_info6, "rampOut6", 0, ramp_info6.min6, ramp_info6.max6)



-- ramp_info.progress = 0
ramp_info2.progress2 = 0
ramp_info3.progress3 = 0
ramp_info4.progress4 = 0
ramp_info5.progress5 = 0
ramp_info6.progress6 = 0

end

-----------INIT - PARAMS
function meu:init()
	-------------DEFAULT PARAMS PER MEU TYPE (here it is 3D object, imagining beginner defaults) 
	local ref = self.ref
	
	local layerA			=	self:get_layer(1)
	local layerB			=	self:get_layer(2)
	local mappingRef = aaa.obj.get_down_by_class ( layerA, "mapping" )
	local param_set = param.set
	
	param_set(  layerA, "material_use_color", false )
	param_set(  layerA, "material_use_color_face", 1 ) -- middle option FRONT_AND_BACK
	param_set(  layerA, "material_use_color_dst", 2 ) -- middle option FRONT_AND_BACK
	
	param_set(  mappingRef, "tex_mode", 1 )
	param_set(  mappingRef, "tex_rot", true )
	param_set(  mappingRef, "tex_coor_centered", true )
	param_set(  mappingRef, "tex_rot", true )
	param_set(  mappingRef, "tex_axe", 2 )
	param_set(  mappingRef, "tex_wrap_u", "CLAMP_TO_BORDER" )
	param_set(  mappingRef, "tex_wrap_v", "CLAMP_TO_BORDER" )
	param_set(  mappingRef, "tex_wrap_w", "CLAMP_TO_BORDER" )
	
	-------------FONTS - TEXT
	ref.bdd		=	self:get_layer_bdd( 2 )
	ref.text	=	param.get_ref( ref.bdd, "text" )
	ref.font	=	param.get_ref( ref.bdd, "font" )
	ref.font_name = param.get_ref( ref.bdd, "font_name" )
	
	local app = aaa.obj.get_from_top_by_class( "app" )
	local param_font_bind = param.get_ref( app, "font->" )
	ref.font_bind = param.get_obj_attached( param_font_bind ) 
	
	end

----------SAMPLE CODE
-- function meu:edit_dialog( arg1, arg2 )
-- 	aaa.print_fn() -- print la fonction et arguments 
-- 	param.do_action_open( aaa.obj.get_down_by_class ( self:get_layer(1), "mapping" ), "camera_name_symbo" )	
-- end
-- ----------END OF SAMPLE CODE

-----------LOAD MODEL
function meu:load_model()
	param.do_action_open( self:get_layer_bdd(1), "model_filename" )	
end



-----------FONTS

function meu:set_text( str )
	param.set( self.ref.text, str )
end

function meu:load_font( id )
	local ref = self.ref
	local ui = self.ui
	local bind = ui.bu_font_selector:get_value()
	local str = "load font at index "..bind
	local filter = "Reconnus\0*.ttf;*.otf\0Ttf\0*.ttf\0Otf\0*.otf\0Tous\0*.*\0\0"
	local filenames = aaa.file.do_dialog_open( str, filter, true )
	table.print( filenames, "filenames" )
	for i,fname in IPAIRS(filenames) do
		self:print( i.." load "..fname.." at "..bind )
		param.set( ref.font_bind, "bind_"..aaa.format.int_to_char2(bind), fname )
		bind = bind + 1
	end
end

-----------UI UPDATE - FONTS
-- -- -- meu.update_ui = nil -- dé-définit la fonction!!!
function meu:update_ui()

	local ref = self.ref
	local ui = self.ui
	local bind = ui.bu_font_selector:get_value()
	local font_name = param.get( ref.font_bind, "bind_"..aaa.format.int_to_char2(bind) )
	ui.font_name:set_text( font_name )

	local str = ui.bu_text:get_text()
	param.set( self.ref.text, str )

end

-- -----------UPDATE - FONTS & ANIM RAMP

function meu:update()


	if app.active2 then
        self:tick_ramp2()
    end
	if self.active3 then
        self:tick_ramp3()
    end
	if self.active4 then
		self:tick_ramp4()
	end
	if self.active5 then
		self:tick_ramp5()
	end
	if self.active6 then
		self:tick_ramp6()
	end
end

-- -----------ANIM RAMPS
----RAMP 1
-- function meu:tick_ramp()
--     local progress_step = aaa.time.dt/self.ramp.time
--     self.ramp.progress = self.ramp.progress + progress_step
--     -- Cap progress at 1
--     if self.ramp.progress >= 1 then
--         self.ramp.progress = 1
-- 		self.ramp.rampOut = self.ramp.max
--     end
--     -- -- Set  ramp out value
-- self.ramp.rampOut = self.ramp.min + ((self.ramp.max - self.ramp.min) * self.ramp.progress)
--     -- -- Reset if looping or stop if not
-- 	if self.ramp.progress == 1 and self.ramp.loop_ramp then
--     	self.ramp.progress = 0
-- 		self.ramp.rampOut = self.ramp.min
--     elseif self.ramp.progress == 1 then
--         self.ramp.progress = 0
-- 		--self.ramp.rampOut = self.ramp.min
-- 		app.active = false
--     end
-- end

----RAMP 2
function meu:tick_ramp2()
    local progress_step2 = aaa.time.dt/self.ramp2.time2
    self.ramp2.progress2 = self.ramp2.progress2 + progress_step2
    -- Cap progress at 1
    if self.ramp2.progress2 >= 1 then
        self.ramp2.progress2 = 1
		self.ramp2.rampOut2 = self.ramp2.max2
    end
    -- -- Set  ramp out value
self.ramp2.rampOut2 = self.ramp2.min2 + ((self.ramp2.max2 - self.ramp2.min2) * self.ramp2.progress2)
    -- -- Reset if looping or stop if not
	if self.ramp2.progress2 == 1 and self.ramp2.loop_ramp2 then
    	self.ramp2.progress2 = 0
		self.ramp2.rampOut2 = self.ramp2.min2
    elseif self.ramp2.progress2 == 1 then
        self.ramp2.progress2 = 0
		self.ramp2.rampOut2 = self.ramp2.min2
		app.active2 = false
    end
end

----RAMP 3
function meu:tick_ramp3()
    local progress_step3 = aaa.time.dt/self.ramp3.time3
    self.ramp3.progress3 = self.ramp3.progress3 + progress_step3
    -- Cap progress at 1
    if self.ramp3.progress3 >= 1 then
        self.ramp3.progress3 = 1
		self.ramp3.rampOut3 = self.ramp3.max3
    end
    -- -- Set  ramp out value
self.ramp3.rampOut3 = self.ramp3.min3 + ((self.ramp3.max3 - self.ramp3.min3) * self.ramp3.progress3)
    -- -- Reset if looping or stop if not
	if self.ramp3.progress3 == 1 and self.ramp3.loop_ramp3 then
    	self.ramp3.progress3 = 0
		self.ramp3.rampOut3 = self.ramp3.min3
    elseif self.ramp3.progress3 == 1 then
        self.ramp3.progress3 = 0
		-- self.ramp3.rampOut3 = self.ramp3.min3
		self.active3 = false
    end
end

----RAMP 4
function meu:tick_ramp4()
	local progress_step4 = aaa.time.dt/self.ramp4.time4
	self.ramp4.progress4 = self.ramp4.progress4 + progress_step4
	-- Cap progress at 1
	if self.ramp4.progress4 >= 1 then
		self.ramp4.progress4 = 1
		self.ramp4.rampOut4 = self.ramp4.max4
	end
	-- -- Set  ramp out value
self.ramp4.rampOut4 = self.ramp4.min4 + ((self.ramp4.max4 - self.ramp4.min4) * self.ramp4.progress4)
	-- -- Reset if looping or stop if not
	if self.ramp4.progress4 == 1 and self.ramp4.loop_ramp4 then
		self.ramp4.progress4 = 0
		self.ramp4.rampOut4 = self.ramp4.min4
	elseif self.ramp4.progress4 == 1 then
		self.ramp4.progress4 = 0
		self.ramp4.rampOut4 = self.ramp4.min4
		self.active4 = false
	end
end

----RAMP 5
function meu:tick_ramp5()
	local progress_step5 = aaa.time.dt/self.ramp5.time5
	self.ramp5.progress5 = self.ramp5.progress5 + progress_step5
	-- Cap progress at 1
	if self.ramp5.progress5 >= 1 then
		self.ramp5.progress5 = 1
		self.ramp5.rampOut5 = self.ramp5.max5
	end
	-- -- Set  ramp out value
self.ramp5.rampOut5 = self.ramp5.min5 + ((self.ramp5.max5 - self.ramp5.min5) * self.ramp5.progress5)
	-- -- Reset if looping or stop if not
	if self.ramp5.progress5 == 1 and self.ramp5.loop_ramp5 then
		self.ramp5.progress5 = 0
		self.ramp5.rampOut5 = self.ramp5.min5
	elseif self.ramp5.progress5 == 1 then
		self.ramp5.progress5 = 0
		self.ramp5.rampOut5 = self.ramp5.min5
		self.active5 = false
	end
end

----RAMP 6
function meu:tick_ramp6()
	local progress_step6 = aaa.time.dt/self.ramp6.time6
	self.ramp6.progress6 = self.ramp6.progress6 + progress_step6
	-- Cap progress at 1
	if self.ramp6.progress6 >= 1 then
		self.ramp6.progress6 = 1
		self.ramp6.rampOut6 = self.ramp6.max6
	end
	-- -- Set  ramp out value
self.ramp6.rampOut6 = self.ramp6.min6 + ((self.ramp6.max6 - self.ramp6.min6) * self.ramp6.progress6)
	-- -- Reset if looping or stop if not
	if self.ramp6.progress6 == 1 and self.ramp6.loop_ramp6 then
		self.ramp6.progress6 = 0
		self.ramp6.rampOut6 = self.ramp6.min6
	elseif self.ramp6.progress6 == 1 then
		self.ramp6.progress6 = 0
		self.ramp6.rampOut6 = self.ramp6.min6
		self.active6 = false
	end
end



-----------LINE DRAWING GRID
-----------Hex Grid  with dist threshold to middle
function meu:distance( x1, y1, x2, y2 )
    return math.sqrt( (x2-x1)^2 + (y2-y1)^2 )
end
-- function meu:distThreshold( x3, y3, x4, y4 )
--     local dist = self:distance ( x3, y3, x4, y4 )
--     return clamp(dist, 1, 4)
-- end

function meu:distThresholdP(r1, r2, x3, y3, x4, y4 ) 
    -- r1 r2 bounds, x3, y3, x4, y4 distance params, below is out-of-bounds as a return
    local val = self:distance ( x3, y3, x4, y4 )
        if (val >= r1) and (val <= r2) then
            return val 
        else
            return 0-- height space 
       end
end

function meu:draw_1()

    local r = 1 --- scale of grid increments
    local r2 = r/2
    local r3 = r*math.sqrt(3)/2  -- 1.73 instead of 2 feels so much better! I checked, 2 is right but it really feels optically squeezed, P

    local function draw_hex2( x, y )
		gol.rotate_x(  .25 ) 
        gol.draw_line_loop_2d(  x,      r+y,    r3+x,    r2+y,
                            r3+x,    r2+y,   r3+x,   -r2+y,
                            r3+x,    -r2+y,   x,     -r+y, 
                             x,      -r+y,    -r3+x, -r2+y,
                            -r3+x,   r2+y,     x,    r+y,  
                            x,       r+y )		 
    end

    local N = self.N -- rang N*N
  -- self.N = 8.0   ----grid size if not using slider above, P

----------------PRIAM's NEW GRID
local function createRectangularHexGrid(numPoints)
	-- 	aaa.print_fn() -- print la fonction et arguments 
    local hexGrid = {}
    local numColumns = math.ceil(math.sqrt(numPoints+numPoints*.5))
    local numRows = math.ceil(numPoints / numColumns)
    local index = 1
   
    -- Calculate the center of the grid
    local centerX = (numColumns - 1) * math.sqrt(3) / 2
    local centerY = (numRows - 1) * 1.5 / 2
    local centerZ = 0

    for i = 1, numRows do
        for j = 1, numColumns do
            local x = (j - 1) * math.sqrt(3) - centerX
            local y = (i - 1) * 1.5 - centerY
            if i % 2 == 0 then
                x = x + 0.5 * math.sqrt(3)
            end
            hexGrid[index] = {x = x, y = y, z = centerZ}
            index = index + 1
            if index > numPoints then
                return hexGrid
            end
        end
    end

    return hexGrid
end

-- hex Grid drawing
local numPoints = N
local R = self.R
local RR = self.RR

local RAD = self.ramp2.rampOut2 * RR -- multiply normalized value by factor 

local hexGrid = createRectangularHexGrid(numPoints)
gol.push_matrix() ---- this encapsulates the transform of the grid so it doesnt impact the layers below
gol.translate( self.XFCT, self.ZFCT, .02 ) -- .02 is the height of the grid ideally above the map plane slightly
gol.set_texture_dim(0)
gol.disable_lighting(false)
gol.set_line_width( 2 )
gol.color( self.GR,self.GG,self.GB, self.GAL )   -- red green blue alpha
-- Printing out the generated grid points
gol.rotate_x(  .25 )
gol.scale(.2, .2, .2)

for i, point in ipairs(hexGrid) do
   -- print("Point " .. i .. ": x = " .. point.x .. ", y = " .. point.y)
    gol.push_matrix()
    local yup =  self:distThresholdP(-5, RAD, 0, 0, point.x, point.y)
	if yup < 1 then
    	gol.translate(point.x, yup*-.23, point.y)
		draw_hex2( 0, 0 )
	end
    gol.pop_matrix()
	end
gol.pop_matrix() ---- this encapsulates the transform of the grid so it doesnt impact the layers below
end
-----------LINE DRAWING INFO TAG
-- function meu:draw_2()
-- 	gol.set_line_width( 1 )
-- 	gol.color( 1,1,1, 1 )   -- red green blue alpha
-- 		gol.push_matrix()
-- 	 	gol.rotate_x( .0)
-- 		local scalf = 2.0
-- 		gol.scale( scalf * .1, scalf * .1, scalf * .1)
-- 		gol.translate( 0, 0, 0)
		
--     gol.draw_lines_3d( 
--     -2.5, -2.25, -2.0,
--     -1.5, -2.25, -7.0,
--       2.5, -2.25, -2.75,
--       -1.5, -2.25, -7.0,
--       2.5, -2.25, -2.75,
--       0, 4.5, -5.0,
--       0, 4.5, -5.0,
--       -2.5, -2.25, -2.0,
--       -2.5, -2.25, -2.0,
--       2.5, -2.25, -2.75,
--       -1.5, -2.25, -7.0,
--       0, 4.5, -5.0
--     )
-- 		gol.pop_matrix()
-- end