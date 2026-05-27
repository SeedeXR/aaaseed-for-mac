function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ix = 1
	local iy = 1
	local SY = .8
	local DY = .5

	self:add_camera()

	self:set_tab_key_def()

	self:add_slider( 		{ix,iy,			4,SY},		"sf",		self, "size",	4, 0,8 )
	 	:add_values_def( 1, 2, 3, 4, 4.5, 5, 6, 7 )
	self:add_slider( 		{ix+4,iy,		2,SY},		"su",		self, "size_u_ui",	1, 0,8 ):set_color_back("u")
		:add_values_def( .5, 1, 2, 3, 4, 4.5, 5, 6, 7, 9 )
	self:add_slider( 		{ix+6,iy,		2,SY},		"sv", 		self, "size_v_ui",	1, 0,8 ):set_color_back("v")
		:add_values_def( .5, 1, 2, 3, 4, 4.5, 5, 6, 7, 9 )
	iy = iy + SY

	local SX

	SX = 4
	local DX = (4-SX)*.5
	self:add_slider(		{ix+DX,iy,		SX,SY},		"Step U",		self, "step_u",			2,	1,128	)
		:set_color_back("u")
		:set_value_type_integer( true )
--		:set_min_max_strict( true )
	self:add_slider(		{ix+8-SX,iy,	SX,SY},		"Step V",		self, "step_v",			2,	1,128	)
		:set_color_back("v")
		:set_value_type_integer( true )
--		:set_min_max_strict( true )

	iy = iy + SY + DY

	self:add_slider( 		{ix,iy,		8,SY},		"Line Size", 		self, "line_size",	.1, 0,1 )
	iy = iy + SY


--	self:add_mapping_by_side_only( {ix,iy} )

	ix, iy = 9, 4
	local SYT = 3
	self:add_bu_texture_target_unit(	{ix,iy,	4,SYT} )
	--self:add_bu_texture_target_unit( {ix,iy,		4,SYT}, "Src",		2, false	)
	iy = iy + SYT


	local sx = 8/3
	self:add_button(		{ix,iy,		sx,SY},	"Debug",		self,	"b_debug_draw",			false	)
	self:add_button(		{ix+sx,iy,	sx,SY},	"Circle",		self,	"b_debug_draw_circle",	false	)
	self:add_button(		{ix+sx*2,iy,sx,SY},	"Y",			self,	"b_debug_draw_y",		false	)
--	bu = self:add_slider(	{ix,iy,		8,SY},	"threshold",	self,	"threshold",			.001,	.001, 1 )
	iy = iy + SY + DY
	
	self:add_trig_method(	{ix,iy,		sx,SY}, "Test", 		self,	"do_test" )
	iy = iy + SY + DY

	self:add_rendering( {ix, iy} )
	iy = iy + 4*SY


--	self:set_tab_key( "Map" )
--	ix, iy = 1, 3
end

function meu:init()
	local ref = self.ref
end


function meu:draw_line_one( ox,sx, sy, wi, nb )	
	gol.begin_quad_strip()
	sy = sy/2
	wi = wi/2
	for u=0,1,1./nb do 
		local x = ox + (u-.5) * sx
		local y = math.cos( math.pi * u ) * sy
		gol.vertex3	( x,y+wi,0, x,y-wi,0 )
	end		
	gol.do_end()
end

function meu:draw_cord( su,sv, wi, sc )
	local suh,svh = su/2,sv/2

	gol.color_white()
	gol.set_texture_dim(0)
	aaa.draw_rect( -suh,-svh, suh,svh )
 	gol.color_black()
 	aaa.draw_rect( -suh,svh-wi/2, suh,svh+wi/2 )
	aaa.draw_rect( -suh,-svh-wi/2, suh,-svh+wi/2 )

	math.randomseed(42)
	for i=1,self.step_u do
		local u = (i-1)/(self.step_u-1)
		self:draw_line_one( (u-.5) * (su+sc) + math.random()*.05*.25,sc + math.random()*.2*.25, sv, wi, 40 )
	end
end

function meu:update()
	local size = self.size
	local su = size * self.size_u_ui
	local sv = size * self.size_v_ui
	--	self:print( "size is "..su.." x "..sv )
	--	self:print( "Size : "..su.." x "..sv )
	self.size_u,self.size_v = su,sv
end

function meu:draw()
	self:draw_layers_begin()

		self:draw_layer( 1 )	--draw all layers in layers

		self:draw_cord( self.size_u,self.size_v, self.size_v*self.line_size, 1.5*.25  )

	self:draw_layers_end()
end
