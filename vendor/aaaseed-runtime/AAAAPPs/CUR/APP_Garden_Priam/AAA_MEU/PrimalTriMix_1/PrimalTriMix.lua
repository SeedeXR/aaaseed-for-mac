--	TriNix.lua"
 --[[ (Dec 2023)  I put that draw function to fix something, Maa did not recommend but I can't tell if it fixed my problem
	or not, Priam ]]--
-- function meu:draw() 
--     self:draw_layers_begin()
--         --  self:print( aaa.time.t )
--         self:draw_layer( 1 )
--         -- self:draw_1( 10, false )   
--         -- self:draw_7( 10, false )    -- choose the drawing function here
--     self:draw_layers_end()
-- end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local param_set = param.set
	local layer			=	self:get_layer(1)

	--[[ NOW you had to manually switch tranfo_trs / active to ON, it was inaccessible by code as far as i tried for, now it works though, P
		ONLY when you manually switched tranfo_trs / active to ON, was the below going to work: ]]--
	local transfo		=	aaa.obj.get_down_by_class( layer, "transfo_trs" )
	local tra = {}
	tra.x, tra.y, tra.z	=	param.get_ref( transfo, "translate_x" ), param.get_ref( transfo, "translate_y" ), param.get_ref( transfo, "translate_z" )
	ref.tra = tra
	param_set(  transfo, "active", true )
	param_set(  layer, "trs_1", 2 )
	param_set(  transfo , "translate", true )
	param_set(  transfo , "rotate", true )
	param_set(  transfo , "scale", true )
	-- -- end of transfo stuff that wasn't executing / activating

	local SY = 1
	local DY = .2
	local ix,iy = 1,1

	self:add_camera()

	self:add_shading_ui( 	{ix,iy,	8,1} )
	iy = iy + SY + DY

 
	bu = self:add_selector(	{ix,iy,	8,SY}, "Mode" )
	bu:set_item_text( 1, "A", "B", "M", "MA", "MB", "MAB" )
	bu:set_target_lua( self, "mode" )
	iy = iy + SY + DY

	local Y = 2.5
	local SX,SYM = 8,5.5*SY

	local YM = 14.7 - SYM - DY
	local names = { "MASK", "A", "B", "TEST" }
	self:add_bu_texture_target_unit( {1,16-SYM*2-SY,		SX,SYM}, names[2], 2	)
	self:add_bu_texture_target_unit( {1,16-SYM,			SX,SYM}, names[3], 3	)
	self:add_bu_texture_target_unit( {17-SX,	YM,			SX,SYM}, names[1], 1	)

	for i=1,3 do		
		self:set_bu_texture_preset_use( i, false )
	end

	local names = { A=2, B=3, M=1 }
	local iy = 16-SYM-SY
	self:add_trig_method( {1,iy,	2,SY},	"A-B", self, "swap_texture_bind", 2, 3 )
	self:add_trig_method( {9,YM-SY,	2,SY},	"A-M", self, "swap_texture_bind", 2, 1 )
	self:add_trig_method( {1+6,iy,	2,SY},	"B-M", self, "swap_texture_bind", 3, 1 )

	self:add_slider({ix+12, iy-5.5, 4, .5}, "ChooseBDD", param.get_ref( layer, "bdd" ), nil, 0, 0, 64):set_color_back("u") 	
	self:add_slider({ix+12, iy-6, 4, .5}, "Scalefactor", param.get_ref( transfo, "scale_factor" ), nil, 1, 0, 8):set_color_back("black") 
	
	self:add_slider({ix+8, iy-6, 4, .5}, "ScaleX", param.get_ref( transfo, "scale_x" ), nil, 1, 0, 8):set_color_back("u") 
    self:add_slider({ix+8, iy-5.5, 4, .5}, "ScaleY", param.get_ref( transfo, "scale_y" ), nil, 1, 0, 8):set_color_back("v") 
	self:add_slider({ix+8, iy-5, 4, .5}, "ScaleZ", param.get_ref( transfo, "scale_z" ), nil, 1, 0, 8):set_color_back("w") 
	self:add_slider({ix+8, iy-4.5, 4, .5}, "TranslateX", tra.x, nil, 0, 0, 8):set_color_back("u") -- just so it's shorter
	self:add_slider({ix+8, iy-4, 4, .5}, "TranslateY", tra.y, nil, 0, 0, 8):set_color_back("v") -- just so it's shorter
	self:add_slider({ix+8, iy-3.5, 4, .5}, "TranslateZ", tra.z, nil, 0, 0, 8):set_color_back("w") -- just so it's shorter
	self:add_slider({ix+8, iy-3, 4, .5}, "RotateX", param.get_ref( transfo, "rotate_x" ), nil, 0, 0, 8):set_color_back("u") 
    self:add_slider({ix+8, iy-2.5, 4, .5}, "RotateY", param.get_ref( transfo, "rotate_y" ), nil, 0, 0, 8):set_color_back("v")
	self:add_slider({ix+8, iy-2, 4, .5}, "RotateZ", param.get_ref( transfo, "rotate_z" ), nil, 0, 0, 8):set_color_back("w") 


--todo go back to this
--	self:add_bu_texture_target_unit(	{1, 13.6, 8, 3.2},	names[4], 4	) 

	local SX = 4
	local SY = 4/1.77
	local pos = {	{ 9, 2.5 }, { 13, 1 }, { 13, 4 } }

	SY = 1
	local ix = 9
	local iy = Y

	bu = self:add_button(	{ix,iy-.5,		2.5,.5},	"Inverse",	self, "b_mask_inv", false )
	bu = self:add_button(	{ix+4,iy-.5,	2.5,.5},	"Alpha",	self, "", false )
	iy = iy + SY
	bu = self:add_slider(	{ix+1,iy-1,	7,.5},	"Min",		self, "min",	0,	0,1	)
	iy = iy + SY
	bu = self:add_slider(	{ix+1,iy-1.5,	7,.5},	"Max",		self, "max",	1,	0,1	)
	iy = iy + SY + DY
end

function meu:init()
	self:add_shading()
end

function meu:update()
	local sha = self:get_shading()
	sha:set_frag_int_1_2( self.mode, self.b_mask_alpha and 1 or 0 )
	if self.b_mask_inv then
		sha:set_frag_float_1_2( self.max, self.min )
	else
		sha:set_frag_float_1_2( self.min, self.max )
	end
		sha:set_frag_vec4_w( 1, self:get_alpha() )
end
