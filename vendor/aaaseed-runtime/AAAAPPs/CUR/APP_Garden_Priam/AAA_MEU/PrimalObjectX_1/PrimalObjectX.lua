--[[ 3D object set up like a new user (designer) would imagine, Priam said so aha.
it's essentailly owner in many respects, all gates 
]]--

function meu:define_ui()
	local param_set = param.set
	local ref = self.ref
	local ui = self.ui
	local bu
    local par
	local layer			=	self:get_layer(1)
	-- NOW you have to manually switch tranfo_trs / active to ON, it's inaccessible by code as far as i tried for 1h, P
	-- ONLY when you manually switch tranfo_trs / active to ON, is the below going to work:
	local transfo		=	aaa.obj.get_down_by_class	( layer, "transfo_trs" )
	local tra = {}
	tra.x, tra.y, tra.z	=	param.get_ref( transfo, "translate_x" ), param.get_ref( transfo, "translate_y" ), param.get_ref( transfo, "translate_z" )
	ref.tra = tra
	param_set(  transfo, "active", true )
	param_set(  layer, "trs_1", 2 )
	param_set(  transfo , "translate", true )
	param_set(  transfo , "rotate", true )
	param_set(  transfo , "scale", true )


	local lights = aaa.obj.get_down_by_class( layer, "lights_switch" )

	local ix = 1
	local iy = 1
    local ix,iy = 1,1
    local SY,DY = 1,.2  
	local sy = 1

 -------------BUILT-INS 
    self:add_rendering()
	self:add_camera()
	bu = self:add_selector(	{ix+10,iy+10,	4,sy+2}, "UseMultiple")
	bu:set_nb( 1, 3 )
	bu:set_item_text( 1, "Current", "None", "Owner" )
	bu:set_target_obj_param( layer, "use_multiple" )
	bu:set_text_draw( true )

    -- self:add_bu_texture_target_unit( {ix+8,iy+7, 8,6} )
    -- bu = self:add_rgbfa(         nil,	"color_1" )
	
	self:add_slider({ix, iy+1, 4, SY}, "ScaleX", param.get_ref( transfo, "scale_x" ), nil, 1, 0, 8):set_color_back("u") 
    self:add_slider({ix, iy+2, 4, SY}, "ScaleY", param.get_ref( transfo, "scale_y" ), nil, 1, 0, 8):set_color_back("v") 
	self:add_slider({ix, iy+3, 4, SY}, "ScaleZ", param.get_ref( transfo, "scale_z" ), nil, 1, 0, 8):set_color_back("w") 
	self:add_slider({ix, iy+4, 4, SY}, "TranslateX", tra.x, nil, 0, 0, 8):set_color_back("u") -- just so it's shorter
	self:add_slider({ix, iy+5, 4, SY}, "TranslateY", tra.y, nil, 0, 0, 8):set_color_back("v") -- just so it's shorter
	self:add_slider({ix, iy+6, 4, SY}, "TranslateZ", tra.z, nil, 0, 0, 8):set_color_back("w") -- just so it's shorter
	self:add_slider({ix, iy+7, 4, SY}, "RotateX", param.get_ref( transfo, "rotate_x" ), nil, 0, 0, 8):set_color_back("u") 
    self:add_slider({ix, iy+8, 4, SY}, "RotateY", param.get_ref( transfo, "rotate_y" ), nil, 0, 0, 8):set_color_back("v")
	self:add_slider({ix, iy+9, 4, SY}, "RotateZ", param.get_ref( transfo, "rotate_z" ), nil, 0, 0, 8):set_color_back("w") 
	self:add_slider({ix, iy+10, 4, SY}, "Scalefactor", param.get_ref( transfo, "scale_factor" ), nil, 1, 0, 8):set_color_back("black") 
	self:add_slider({ix, iy+11, 8, SY}, "Material Front", param.get_ref( layer, "material_front" ), nil, -1, 0, 127):set_color_back("y") 
	self:add_slider({ix, iy+12, 8, SY}, "Material Back", param.get_ref( layer, "material_back" ), nil, -1, 0, 127):set_color_back("y") 

	bu = self:add_button({ix+4, iy+1, SY, SY}, "UseLight1", lights, "light_switch_0", false):set_text_visible(true) --Checkmark button
   	-- bu = self:add_button( {	ix+4,	iy,		4,SY },	"light0",	lights, "light_switch_0", false	):set_text_visible(true) --regular ON-OFF button
	bu = self:add_button({ix+4, iy+2, SY, SY}, "UseLight2", lights, "light_switch_1", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+3, SY, SY}, "UseLight3", lights, "light_switch_2", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+4, SY, SY}, "UseLight4", lights, "light_switch_3", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+5, SY, SY}, "UseLight5", lights, "light_switch_4", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+6, SY, SY}, "UseLight6", lights, "light_switch_5", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+7, SY, SY}, "UseLight7", lights, "light_switch_6", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+8, SY, SY}, "UseLight8", lights, "light_switch_7", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+9, SY, SY}, "MaterialUseColor", layer, "material_use_color", false):set_text_visible(true) --Checkmark button
	bu = self:add_button({ix+4, iy+10, SY, SY}, "USELIGHTS", layer, "use_lights_switch", false):set_text_visible(true) --Checkmark button
	

	-- bu = self:add_selector(	    {8,11,	9,1},   "UseLights" )
    -- bu:set_item_text( 2, "1", "2", "3", "4", "5", "6", "7" )

	-- bu = self:add_selector(	{ix+7,iy+2,		8,sy}, 	"type" )
	-- 	bu:set_target_lua( self, "type_out" )
	-- 	bu:set_nb(5)
	-- 	bu:set_item_text( 1,  "White", "All", "Crop", "UV Crop", "UV In" )
	-- ui.bu_type = bu	


-------------CUSTOM
    self:add_trig_method(	{ix,iy,		4,SY},	"Load Model", 	self, "load_model"		):set_color_back( "load" )
	self:add_trig_method(	{ix+4,iy,	4,SY},	"load tex", 	self, "load_texture"	):set_color_back( "load" )
end

-----------LOAD MODEL
function meu:load_model()
	param.do_action_open( self:get_layer_bdd(1), "model_filename" )

end

-----------INIT 
function meu:init()
-------------DEFAULT PARAMS PER MEU TYPE (here it is 3D object, imagining beginner defaults) 
local layer			=	self:get_layer(1)
local param_set = param.set
param_set(  layer, "material_use_color", false )
param_set(  layer, "material_use_color_face", 1 ) -- middle option FRONT_AND_BACK
param_set(  layer, "material_use_color_dst", 2 ) -- middle option FRONT_AND_BACK

---	aaa.flatland.set_color_blind( .2, .5, .3 ) 

end
-----------UI UPDATE
-- -- -- meu.update_ui = nil -- dé-définit la fonction!!!
function meu:update_ui()
	-- aaa.draw_disk_axe_z(2, 2, 1, 1)



	-- local bu = self:get_bu_by_key("SizeV")
	-- bu:set_alpha_bu(self.b_diff and 1 or .25)
	-- local bu = self:get_bu_by_key("Size Axe")
	-- bu:set_alpha_bu(self.b_diff and 1 or .25)
end


-- -----------DRAW
-- function meu:draw()
--     self:draw_layers_begin()
-- end

-- -----------UPDATE
-- function meu:update()

-- end