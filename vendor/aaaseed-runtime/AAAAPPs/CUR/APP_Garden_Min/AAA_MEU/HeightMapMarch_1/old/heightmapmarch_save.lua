--[[
function meu:flip_phong()
	local b_phong = self.b_phong
	if b_phong == 1 then
		b_phong = 0
		self.bu_phong:set_color_back( "off" )
	elseif b_phong == 0 then
		b_phong = 1
		self.bu_phong:set_color_back( "on" )
	end
	self:get_shading():set_frag_int_2( b_phong )
	self.b_phong = b_phong
end
function meu:flip_refract()
	local b_refract = self.b_refract
	if b_refract == 1 then
		b_refract = 0
		self.bu_refract:set_color_back( "off" )
	elseif b_refract == 0 then
		b_refract = 1
		self.bu_refract:set_color_back( "on" )
	end
	self:get_shading():set_frag_int_3( b_refract )
	self.b_refract = b_refract
end
function meu:flip_amb()
	local b_amb = self.b_amb
	if b_amb == 1 then
		b_amb = 0
		self.bu_amb:set_color_back( "off" )
	elseif b_amb == 0 then
		b_amb = 1
		self.bu_amb:set_color_back( "on" )
	end
	self:get_shading():set_frag_int_4( b_amb )
	self.b_amb = b_amb
end
--]]
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local ix,iy
	local sha = self:get_shading()

	ix,iy = 1,3.7
--	self:add_mapping_by_side()
	self:set_tab_key_def()
	self:add_bu_texture_target_unit()
	self:add_bu_texture_target_unit( 	{ix,iy+1}, "tex_refract", 2	))

	bu = self:add_slider(	{1,8,	8,1},	"Speed",	self, "speed",	.2,		-1,	1 )

	self:add_camera()
	self:add_shading_ui(	{9,2.5,	8,1} )

	ix, iy = 9, 4
	bu = self:add_selector(	{ix,iy,	8,1}, "Render" )
		bu:set_nb_min_0( 6, 1 )
		bu:set_item_text( 2, "GradRG", "?", "Normal", "March", "?" )
		bu:set_target_param( sha:get_ref_frag_int( 1 ) )

	ui.bu_col = {}
	for i=1,6 do
		iy = iy + 1.
		bu = self:add_button(	{ix,iy, 3,.9}, 	self.lighting[i], self, "b_"..self.lighting[i]	):set_text_rect_ratio( 2 )
		if i<5 then
			ui.bu_col[i] = self:add_rgbf(	{ix+3,iy,	5,.9}, 	self.lighting[i].."_col", false )
		end
	end
	--self.bu_phong	=	self:add_trig_method(	{ix,	iy},		"Phong",	self, "flip_phong" )--:set_color_back( "on" )
	--self.bu_amb		=	self:add_trig_method(	{ix+3,	iy},		"Amb",		self, "flip_amb" )--:set_color_back( "on" )
	--self.bu_refract	=	self:add_trig_method(	{ix,	iy+1},	"Refr",		self, "flip_refract" )--:set_color_back( "on" )

	--bu = self:add_slider(	{ix,iy,	8,1},	"Tex0 factor",	self, "tex0_factor",	1.,  	0,	1 )
	bu = self:add_slider(	{ix+3,iy,	5,1},	"Refract_ind",	self, "refract_ind",	1.333,  0,	3 )
	bu = self:add_slider(	{1,iy,		8,1},	"Height",		self, "height",			1,		0,	1 )
	bu:set_meter( false )

	self:set_tab_key( "March" )

	self:set_tab_key( "Lights" )

	self:set_tab_key( "Materials" )
end

function meu:init()
	self.lighting = { "texture", "phong", "ao", "amb", "shadow", "refract" }
	for i=1,6 do
		self[ "b_"..self.lighting[i] ] = 1
	end
	self.time = 0
--	self:print( "je passe ds l'init" )
	self:add_shading( 1, "HeightMarch" )
end

function meu:update_ui()
	self:update_shader_ui()
end

function meu:update()
	local t = self.time
	t = t + aaa.time.dt * math.pow( self.speed*4, 3 )
	self.time = t
	local sha = self:get_shading()
	local lighting = self.lighting

	--for i=2,4 do
	--	sha:set_frag_int( i, self["b_"..lighting[i]] )
		sha:set_frag_vec4( 5, self["b_"..lighting[1]], self["b_"..lighting[2]], self["b_"..lighting[3]], self["b_"..lighting[4]] )
		sha:set_frag_vec4( 6, self["b_"..lighting[5]], self["b_"..lighting[6]], 0, 0 )
		--print(self["b_"..self.lighting[i]].."b_"..self.lighting[i])
	--end

	sha:set_frag_float_4( t )
	sha:set_frag_float_5( self.height )
	sha:set_frag_float_6( 1. / self.refract_ind )
	--sha:set_frag_float_7( self.tex0_factor )

	local bu_col = self.ui.bu_col
	for i=1,4 do
		local r,g,b,a = bu_col[i]:get_rgba()
		sha:set_frag_vec4( i, r,g,b,a ) -- problem : all slider change all values ..?!
	end
	--self:print( self.ui.bu_col:get_rgba() )
end
