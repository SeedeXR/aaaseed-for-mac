function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ui = self.ui
	local ix = 1
	local iy = 2
	local DY = 1
	local sha =  self:get_shading()

	self:set_tab_key( "Wave" )

	ui.color = {}
	ui.color[1] = self:add_rgba(	{9,iy,	nil,1}, "c1", false )
	iy = iy + DY
	ui.color[2] = self:add_rgba(	{9,iy,	nil,1}, "c2", false )
 	--ui.color[3] = self:add_rgba(	{9,10.5,	nil,1}, "c3", false )
	--ui.color[4] = self:add_rgba(	{9,11.5,	nil,1}, "c4", false )
	--ui.color[5] = self:add_rgba(	{9,12.5,	nil,1}, "c5", false )

	iy = 2
	self:add_trig(	{			ix, iy },				"Reload",	sha.frag.reload				)
	iy = iy + DY
	bu = self:add_button(	{ix, iy,	DY, DY }, "Time", 		self, "b_time_use", false  )
		bu:set_text_rect_ratio( 3.5 )
	bu = self:add_slider(	{ix+3,iy,	5,DY},	"Time_factor",		self, "time_factor",	0, -1, 1 )
		bu:set_meter( false )
		self.time = 0
	
	iy = iy + DY
		bu = self:add_param(	{ix,iy,		8,DY},	"Noise type",			sha:get_ref_frag_int(1),	0, 3		)
	iy = iy + DY
		bu = self:add_param(	{ix+1,iy,		7,DY}, 	"Scale",				sha:get_ref_frag_float(2),	0, 64		)
	--	bu:set_meter( false )
	--iy = iy + DY
	--	bu = self:add_param(	{ix+1,iy,		7,DY}, "vec4_y Scale y",		sha:get_ref_frag_vec4_y(6),	-8, 8		)
	--	bu:set_meter( false )
	--iy = iy + DY
	--	bu = self:add_param(	{ix+1,iy,		7,DY}, "vec4_z",		sha:get_ref_frag_vec4_z(6),	-8, 8		)
	--	bu:set_meter( false )
	--iy = iy + DY
	--	bu = self:add_param(	{ix+1,iy,		7,DY}, "vec4_w",		sha:get_ref_frag_vec4_w(6),	-8, 8		)
	--	bu:set_meter( false )

	self:set_tab_key( "Shader" )

	self:add_bu_texture_target_unit()
	self:add_camera()

	self.ui.bu_info = self:add_text_info(	{10,2,	6,.8}, "frag_info" )

	self:add_monitor( {9,3} )

	bu = self:add_selector(	{1,4.5,	8,2}, "Shader" )
		bu:set_nb_min_0( 5, 4 )
		for i = 2,20,2 do
			bu:set_item_text( i, i )
		end
		bu:set_target_param( sha:get_ref_frag_bind() )
		bu:set_min_max( 202, 202+20 )

	ix = 1
	iy = 6.6

	bu = self:add_button(	{	ix, iy },				"Cross",	self, "b_cross"					)
	bu = self:add_slider(	{ix+3,iy,	5,1},	"Fade",		self, "cross_fade", 			0, -1, 1	)
		bu:set_meter( false )
	bu = self:add_param(	{ix,iy+1,		4,1},	"Src",		sha:get_ref_frag_src(),		-2, 2		)
		bu:set_meter( false )
		ui.src = bu

	bu = self:add_param(	{ix+4,iy+1,		4,1},	"Out",		sha:get_ref_frag_out(),		-2, 2		)
		bu:set_meter( false )
		ui.out = bu

		self:add_blending( {9,13} )
	iy = iy+2
	--	sending stuff was
	--	quick hack done with romzy
	--self:add_trig(	{			1, iy },				"Reload",	sha.frag.reload				)

	--ui.float = {}
	--ui.float_on = {}
	--for i=1,8 do
	--	local y = iy+DY*i
	--	--bu = self:add_button(	{ix, y,	DY, DY }, "float_"..i, self, "b_f"..i, true ):set_text_draw(false)
	--	bu = self:add_param(	{ix+1,y,		7,DY}, "f"..i,		sha:get_ref_frag_float(i),	-2, 2		)
	--		bu:set_meter( false )
	--		ui.float[i] = bu
	--end
end

function meu:init()
	self:add_shading()
end

--[[
function meu:draw()
	self:print( "Own draw" )
	MEU.draw( self )
end
--]]

function meu:update()
--	aaa.print_method( "Asia" )

	local ui = self.ui
	local sha =  self:get_shading()

	if self.b_time_use then
		local t = self.time
		t = t + aaa.time.dt * self.time_factor * 10.
		sha:set_frag_float_1 ( t )
		self.time = t
	else
	end

--	for i=1,8 do
--		local b = not self[ "b_f"..i ]
--		--self:print( "b_f"..i.." "..b )
--		ui.float[i]:set_active( not b )
--		ui.float[i]:set_visible( not b )
--	end

	if self.b_cross==1 then
		local c = self.cross_fade * .5 + .5
		ui.src:set_value( 1-c )
		ui.out:set_value( c )
	end

	for i=1,2 do
		local r,g,b,a = ui.color[i]:get_rgba()
		--ui.color[i]:print( i.." "..r.." "..g.." "..b.." "..a)
		sha:set_frag_vec4( i, r,g,b, a*self:get_alpha() )
	end

	--self:print( "toto"..str )
	ui.bu_info:set_text( sha:get_fname_frag() )
	ui.bu_info:set_text_color_problem_info( sha:is_valid() )
end
