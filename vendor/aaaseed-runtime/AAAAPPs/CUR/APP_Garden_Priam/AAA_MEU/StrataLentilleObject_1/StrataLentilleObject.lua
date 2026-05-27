--[[ 3D object set up like a new user (designer) would imagine, Priam said so aha.
it's essentailly owner in many respects, all gates 
variable shared across app in all fucntions:
app.appSharedVar = 3
self:print("name is "..app.appSharedVar )

]]--



------------DRAW
function meu:draw()
	
    self:draw_layers_begin()
        --  self:print( aaa.time.t )
		self:draw_layer (5) -- bringing that layer up in the draw order so the shadow can be seen below the Lentille object (plane + png)
        self:draw_layer( 1 ) -- Lentille object
		self:draw_3( self:get_alpha(), 10, false )      -- draw hex Grid
		self:draw_1( self:get_alpha(), 10, false )      -- line drawing attached to Main -- choose the drawing function here
		
		self:draw_layer( 2 ) ---- PUSH transform (PUSH is ON, POP is OFF)
		self:draw_2( self:get_alpha(), 10, false )      -- line drawing attached to InfoTag -- choose the drawing function here
		self:draw_layer( 3 ) -- object for tag
	
		self:draw_layer( 4 ) ---- POP transform (PUSH is OFF, POP is ON)
		
    self:draw_layers_end()
	--self:print("self.var - printing from Draw function - equal to "..self.caca )
end

------------DEFINE
function meu:define_ui()

	self.caca = 7

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
	local layerE =	self:get_layer(5)
	local transfoE		=	aaa.obj.get_down_by_class	( layerE, "transfo_trs" )
	--"Texture..."

-------------TAB 1 
self:set_tab_key( "Main" )

	self:add_text_info(	{ix  ,iy,	8,1}, "Main Settings" )

	bu = self:add_camera(		{nil,iy+2,	nil,1.8}, 16 )

	bu = self:add_selector(	{ix,iy+2,	1.4,sy+.8}, "UseMultiple")
	bu:set_nb( 1, 3 )
	bu:set_item_text( 1, "Current", "None", "Owner" )
	bu:set_target_obj_param( layerA, "use_multiple" )
	bu:set_text_draw( true )

	self:add_slider({ix, iy+4, 4, SY}, "ScaleX", param.get_ref( transfoS, "scale_x" ), nil, 1, 0, 8):set_color_back("u")
    self:add_slider({ix, iy+5, 4, SY}, "ScaleY", param.get_ref( transfoS, "scale_y" ), nil, 1, 0, 8):set_color_back("v") 
	self:add_slider({ix, iy+6, 4, SY}, "ScaleZ", param.get_ref( transfoS, "scale_z" ), nil, 1, 0, 8):set_color_back("w") 
	self:add_slider({ix, iy+7, 4, SY}, "TranslateX", param.get_ref( transfoS, "translate_x" ), nil, -10, 0, 10):set_color_back("u")
	self:add_slider({ix, iy+8, 4, SY}, "TranslateY", param.get_ref( transfoS, "translate_y" ), 0, 0, 8):set_color_back("v") 	
	self:add_slider({ix, iy+9, 4, SY}, "TranslateZ", param.get_ref( transfoS, "translate_z" ), 0, 0, 8):set_color_back("w") 
	self:add_slider({ix, iy+10, 4, SY}, "RotateX", param.get_ref( transfoS, "rotate_x" ), nil, -1.0, 0, 1.0):set_color_back("u") 
    self:add_slider({ix, iy+11, 4, SY}, "RotateY", param.get_ref( transfoS, "rotate_y" ), nil, -1.0, 0, 1.0):set_color_back("v")
	self:add_slider({ix, iy+12, 4, SY}, "RotateZ", param.get_ref( transfoS, "rotate_z" ), nil, -1.0, 0, 1.0):set_color_back("w") 
	self:add_slider({ix, iy+13, 4, SY}, "ScaleFactor", param.get_ref( transfoS, "scale_factor" ), nil, 1, 0, 8):set_color_back("black") 
	self:print("transfoxxx is ".. transfoS, "translate_x" )

	------SHADOW SETTINGS
	self:add_bu_texture_target_layer(  {ix + 8,iy+8, 8,6}, "Shadow", 6 , true,	layerE )

	self:add_slider({ix+8, iy+4, 4, SY}, "TraShadowX", param.get_ref( transfoE, "translate_x" ), nil, 1, 0, 8):set_color_back("u") 
    self:add_slider({ix+8, iy+5, 4, SY}, "TraShadowY", param.get_ref( transfoE, "translate_y" ), nil, 1, 0, 8):set_color_back("v") 
	self:add_slider({ix+8, iy+6, 4, SY}, "TraShadowZ", param.get_ref( transfoE, "translate_z" ), nil, 1, 0, 8):set_color_back("w")

	--self:add_rgbfa_only(     {ix+8  ,iy +7,	8,1},  "ColorShadow", aaa.obj.get_down_by_class( self:get_layer(5),"color"))

-------------TAB 2
self:set_tab_key( "Model" )

	self:add_text_info(	{ix  ,iy ,	8,1}, "3D Lentille Settings" )
	self:add_rendering()
-------------CUSTOM
	self:add_trig_method(	{ix+11,iy+.1,		4,SY},	"Load Model", 		self, "load_model"	):set_color_back( "load" )
	

	local iy = iy + 1
	self:add_slider({ix, iy+1, 4, SY}, "ScaModelX", param.get_ref( transfo, "scale_x" ), nil, 1, 0, 8):set_color_back("u") 
    self:add_slider({ix, iy+2, 4, SY}, "ScaModelY", param.get_ref( transfo, "scale_y" ), nil, 1, 0, 8):set_color_back("v") 
	self:add_slider({ix, iy+3, 4, SY}, "ScaModelZ", param.get_ref( transfo, "scale_z" ), nil, 1, 0, 8):set_color_back("w") 
	self:add_slider({ix, iy+4, 4, SY}, "TraModelX", tra.x, nil, -0.059514, 0, 8):set_color_back("u") -- just so it's different
	self:add_slider({ix, iy+5, 4, SY}, "TraModelY", tra.y, nil, -0.052521, 0, 8):set_color_back("v") -- just so it's different
	self:add_slider({ix, iy+6, 4, SY}, "TraModelZ", tra.z, nil, 0, 0, 8):set_color_back("w") -- just so it's different
	self:add_slider({ix, iy+7, 4, SY}, "RotModelX", param.get_ref( transfo, "rotate_x" ), nil, -1.0, 0, 1.0):set_color_back("u") 
    self:add_slider({ix, iy+8, 4, SY}, "RotModelY", param.get_ref( transfo, "rotate_y" ), nil, -1.0, 0.25, 1.0):set_color_back("v")
	self:add_slider({ix, iy+9, 4, SY}, "RotModelZ", param.get_ref( transfo, "rotate_z" ), nil, -1.0, 0, 1.0):set_color_back("w") 
	self:add_slider({ix, iy+10, 4, SY}, "ScaModelFactor", param.get_ref( transfo, "scale_factor" ), nil, 2.62, 0, 8):set_color_back("black") 
	self:add_slider({ix+8, iy+1, 8, SY}, "Material Front", param.get_ref( layerA, "material_front" ), nil, -1, 0, 127):set_color_back("y") 
	self:add_slider({ix+8, iy+2, 8, SY}, "Material Back", param.get_ref( layerA, "material_back" ), nil, -1, 0, 127):set_color_back("y") 



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
	self:add_text_info(	{ix  ,iy-1 ,	8,1}, "Map Settings" )
	self:add_bu_texture_target_unit( {ix + 8 ,iy+1, 8,6}, "Map" , 1 )
	
		self:set_bu_texture_preset_use( 1, false )

	self:add_trig_method(	{ix+11,iy-.9,		4,SY},	"load Tex", 		self, "load_texture"	):set_color_back( "load" )

	self:add_slider({ix, iy+1, 4, SY}, "texHeight", param.get_ref( mappingRef, "tex_u" ), nil, .09, 0, 1):set_color_back("u") 
    self:add_slider({ix, iy+2, 4, SY}, "texWidth", param.get_ref( mappingRef, "tex_v" ), nil, .16, 0, 1):set_color_back("v") 
	local texU = param.get_str(  mappingRef , "tex_u" )
	self:add_slider({ix, iy+3, 4, SY}, "texZoomFactor", param.get_ref( mappingRef, "tex_factor" ) , nil, 1, 0.01, 3):set_color_back("v") --- values order = 0, -1, 1 --> 0 = center -1 = left 1 = right 
		bu:add_values_def( .25, .5, .75, 1, 1.5, 2, 2.5 )


-----------TAB 4 ---FONTS
self:set_tab_key( "InfoTag" )

self:add_text_info(	{ix  ,iy -1,	8,1}, "Lentille Info", param.get_ref( transfo, "rotate_x" ) )
bu = self:add_rgbfa(     {ix+8  ,iy -1,	8,1},  "ColorText", aaa.obj.get_down_by_class( self:get_layer(2),"color"))

self:add_slider({ix, iy+4, 4, SY}, "TraInfoX", traB.x, nil, 0, 0, 8):set_color_back("u") -- just so it's different
self:add_slider({ix, iy+5, 4, SY}, "TraInfoY", traB.y, nil, 0, 0, 8):set_color_back("v") -- just so it's different
self:add_slider({ix, iy+6, 4, SY}, "TraInfoZ", traB.z, nil, 0, 0, 8):set_color_back("w") -- just so it's different
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

self:set_tab_key( "GridHexFX" )
local ix,iy = 1, 1

self:add_text_info(	{ix  ,iy ,	8,1}, "Hex Grid FX" )


local bu = self:add_rgbfa_only(	{ix+8  ,iy +4,	8,1}, 	"GRID",	aaa.obj.get_down_by_class( layerC, "color" ), false )

local bu = self:add_slider( {ix,iy+2, 8,SY}, "Nunits", self, "N", 2200,  0,1500 )
bu = self:add_slider( {ix,iy+3, 8,SY}, "Zfactor", self, "ZFCT", -.15,  -1, 1 ) 
bu = self:add_slider( {ix+8,iy+3, 8,SY}, "Xfactor", self, "XFCT", 0,  -1, 1 ) 
bu = self:add_slider( {ix,iy+4, 8,SY}, "GridAlpha", self, "GAL", 0,  0, 1 )
bu = self:add_slider( {ix,iy+5, 8,SY}, "GridR", self, "GR", 1,  0, 1 ):set_color_back("u")
bu = self:add_slider( {ix,iy+6, 8,SY}, "GridG", self, "GG", 1,  0, 1 ):set_color_back("v")
bu = self:add_slider( {ix,iy+7, 8,SY}, "GridB", self, "GB", 1,  0, 1 ):set_color_back("w")
bu = self:add_slider( {ix,iy+10, 8,SY}, "Nana", app, "strataDateNew", 3,  0,20 ) -- shared app.var - 


--self:print("Xfact is " .. self.XFCT)
end

-----------INIT - PARAMS
function meu:init()
	
	-------------DEFAULT PARAMS PER MEU TYPE (here it is 3D object, imagining beginner defaults) 
	local ref = self.ref
	
	local layerA			=	self:get_layer(1)
	local layerB			=	self:get_layer(2)
	aaa.obj.update( self:get_layer(5) )
	local mappingRef = aaa.obj.get_down_by_class ( layerA, "mapping" )
	local param_set = param.set
	
	param_set(  layerA, "material_use_color", false )
	param_set(  layerA, "material_use_color_face", 1 ) -- middle option FRONT_AND_BACK
	param_set(  layerA, "material_use_color_dst", 2 ) -- middle option FRONT_AND_BACK
	
	param_set(  mappingRef, "tex_mode", 0 )
	param_set(  mappingRef, "tex_rot", true )
	param_set(  mappingRef, "tex_coor_centered", true )
	param_set(  mappingRef, "tex_axe", 1 )
	param_set(  mappingRef, "tex_wrap_u", "CLAMP_TO_BORDER" )
	param_set(  mappingRef, "tex_wrap_v", "CLAMP_TO_BORDER" )
	param_set(  mappingRef, "tex_wrap_w", "CLAMP_TO_BORDER" )
	
	-------------FONTS - TEXT
	ref.bdd		=	self:get_layer_bdd( 2 )
	ref.text	=	param.get_ref( ref.bdd, "text" )
	ref.font	=	param.get_ref( ref.bdd, "font" )
	ref.font_name = param.get_ref( ref.bdd, "font_name" )
	
	local app_ref = aaa.obj.get_from_top_by_class( "app" )
	local param_font_bind = param.get_ref( app_ref, "font->" )
	ref.font_bind = param.get_obj_attached( param_font_bind ) 

    -- Insert reference into global table of StrataLentilleObjects
    if (app.StrataLentilleObjects == nil) then
        app.StrataLentilleObjects = {}
    end
    local idx = table.getn(app.StrataLentilleObjects) + 1
    self.idx = idx
    app.StrataLentilleObjects[idx] = self.ref
    self:print("Hello from StrataLentilleObject_"..idx)
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

	self:set_tex_slide_from_main_pos()

end


-- -----------UPDATE - FONTS
function meu:update()
	local str = self.ui.bu_text:get_text()
	param.set( self.ref.text, str )

	-- -----------AVATAR POINTS
	local a = self:get_meu_by_name("Avatar_1")
    local a_ref = a.ref
    local points = aaa.bdd.get_points_with_id(a_ref.boid)
    -- Ensure points exists before we attempt to iterate through it, otherwise AAASeed becomes very unhappy
    if points then
        for i,t in ipairs(points) do
            local x,y,z = t[1],t[2],t[3]
            local dum,id = poid.split_id( t[4] )

            -- For now, I'm only setting a value based on the first point. However, this same code could apply to multiple sliders if you use the index (i) and conditionally assign sliders.
            if id == self.idx then
                self:set_bu_value("TranslateX", x * 2 )
                self:set_bu_value("TranslateZ", -y * 2 )
            -- EXAMPLE for if you were to want to set a second value based on the 2nd avatar x and y coordinates
            -- elseif i == 2 then
            --     self:set_bu_value("some other value", x)
            --     self:set_bu_value("yet another value", y)
            end
		end
	end
	
end
-------------PROPORTIONATE TEXTURE SLIDING (called in update_ui)
function meu:set_tex_slide_from_main_pos()
	local ref = self.ref
	local param_set = param.set
	local layerS = self:get_layers()
	local layerA =	self:get_layer(1)
	ref.layers_trs_use = param.get_ref( layerS, "transfo" )
	local transfoS = aaa.obj.get_branch_by_class_no_error( layerS, "transfo_trs" )
	local traS = {}
	traS.x, traS.y, traS.z	=	param.get_ref( transfoS, "translate_x" ), param.get_ref( transfoS, "translate_y" ), param.get_ref( transfoS, "translate_z" )
	ref.traS = traS
	local mappingRef = aaa.obj.get_down_by_class ( layerA, "mapping" )
	local translateX = param.get_str( traS.x )
	local translateZ = param.get_str( traS.z )
	param_set(  mappingRef , "tex_u_ori",  translateX * - 0.06255 ) --   -first good numbers:  .0926 factor .715 map is NOT CENTERED
	param_set(  mappingRef , "tex_v_ori",  translateZ *  0.1113) --    -first good numbers: .0926 factor  .164

-- -- Lentille txture size h = 0.126 in relation to 16 x 9 units background map below
-- -- Lentille txture size w =  0.223 in relation to 16 x 9 units background map below
end


-----------LINE DRAWING MAIN - Yellow tetrahedron
function meu:draw_1()
    gol.set_line_width( 5 )
	gol.set_texture_dim(0)
	gol.disable_lighting(false)
	gol.color( 1,1,.1, 1 )   -- red green blue alpha
		gol.push_matrix()
		gol.scale( .5, .2, .5)
		gol.translate( 0, 0, 0)
    gol.draw_lines_3d( 
    -2.5, -2.25, -2.0,
    -1.5, -2.25, -7.0,
      2.5, -2.25, -2.75,
      -1.5, -2.25, -7.0,
      2.5, -2.25, -2.75,
      0, 4.5, -5.0,
      0, 4.5, -5.0,
      -2.5, -2.25, -2.0,
      -2.5, -2.25, -2.0,
      2.5, -2.25, -2.75,
      -1.5, -2.25, -7.0,
      0, 4.5, -5.0
    )
		gol.pop_matrix()
end

-----------LINE DRAWING INFO TAG - white tetrahedron
function meu:draw_2()
	gol.set_line_width( 5 )
	gol.set_texture_dim(0)
	gol.disable_lighting(false)
	gol.color( 1,1,1, 1 )   -- red green blue alpha
		gol.push_matrix()
	 	gol.rotate_x( .0)
		local scalf = 2.0
		gol.scale( scalf * .1, scalf * .1, scalf * .1)
		gol.translate( 0, 0, 0)
		
    gol.draw_lines_3d( 
    -2.5, -2.25, -2.0,
    -1.5, -2.25, -7.0,
      2.5, -2.25, -2.75,
      -1.5, -2.25, -7.0,
      2.5, -2.25, -2.75,
      0, 4.5, -5.0,
      0, 4.5, -5.0,
      -2.5, -2.25, -2.0,
      -2.5, -2.25, -2.0,
      2.5, -2.25, -2.75,
      -1.5, -2.25, -7.0,
      0, 4.5, -5.0
    )
		gol.pop_matrix()
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

function meu:draw_3()

--[[ invert transform patch for hexgrid, sorry couldnt get abything better going cuz my setup is all wrong, 
it wont stay in place so i had to invert the transform, P 
]]--

 	local ref = self.ref
	local param_set = param.set
	local layerS = self:get_layers()
	local layerA =	self:get_layer(1)
	ref.layers_trs_use = param.get_ref( layerS, "transfo" )
	local transfoS = aaa.obj.get_branch_by_class_no_error( layerS, "transfo_trs" )
	local traS = {}
	traS.x, traS.y, traS.z	=	param.get_ref( transfoS, "translate_x" ), param.get_ref( transfoS, "translate_y" ), param.get_ref( transfoS, "translate_z" )
	ref.traS = traS
	
	local translateSX = param.get_str( traS.x )
	local translateSZ = param.get_str( traS.z )
	local Xgrid = translateSX * -1
	local Zgrid = translateSZ * -1

--[[ end of invert transform patch 
]]--

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
		local numColumns = math.ceil(math.sqrt(numPoints))--*.9
		local numRows = math.ceil(numPoints / numColumns) --*.8
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
	--local HPX = self.HPX
	-- local TRAXX = traS.x


	local hexGrid = createRectangularHexGrid(numPoints)
	gol.push_matrix() ---- this encapsulates the transform of the grid so it doesnt impact the layers below
	gol.translate( Xgrid + self.XFCT, -.065, Zgrid - self.ZFCT )-- ideal height: -.065 -- keeps grid in center by inverting the transform, sorry patch above, 
	-- and also adjusts the geometry to the Lentille model...P
 	gol.set_texture_dim(0)
	gol.disable_lighting(false)
	gol.set_line_width( 2 )
	gol.color( self.GR,self.GG,self.GB, self.GAL )   -- red green blue alpha
	gol.rotate_x(  .5 )
	gol.scale(.2, .2, .2)

		for i, point in ipairs(hexGrid) do
		-- print("Point " .. i .. ": x = " .. point.x .. ", y = " .. point.y)
			gol.push_matrix()
			local yup =  self:distThresholdP(0, 5.5, translateSX * 5, translateSZ *-5, point.x, point.y) -- 5 & - 5 are multipliers for gris exact translation
			if yup > 1 then
				gol.translate(point.x, yup*-.2, point.y)
				draw_hex2( 0, 0 )
			end
			gol.pop_matrix()
		end
	gol.pop_matrix() ---- this encapsulates the transform of the grid so it doesnt impact the layers below
end
