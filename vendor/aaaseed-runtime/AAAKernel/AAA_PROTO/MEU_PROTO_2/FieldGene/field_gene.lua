function meu:define_meu_infos( )
	return { author = "Mâa mrvux",
			tags = { "2d", "camera", "CoreGraphic", "input", "Procedural", "texture", "Utility" },
			help = "Use to simulate public, blobs or camera input, by drawing a \"field\" output\n"..
					"when we are not in capture condition or want to debug"
			 }
end

local function draw_back_multi( bu )
	oo.getsuper(SLIDER_MULTI).draw_back( bu )

	local sx,sy = .5,.5

	gol.color_orange( .5 )
	aaa.draw_rect_line( -sx, -sy, sx, sy )

	gol.set_line_width( 1 )
	gol.color_white()
	aaa.draw_rect_line( -.5, -.5, .5, .5 )
end

local function draw_fore_multi( bu )
	oo.getsuper(SLIDER_MULTI).draw_fore( bu )

	gol.set_line_width( 1 )
	gol.color_cyan( .5 )
	for i=1,bu:get_elt_nb() do
		local elt = bu:get_elt( i )
		local x,y = elt:get_xy()
		aaa.draw_plus_line(	x-.5, y-.5, .0125 )
	end
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	self.speed = 1
	self.number = 12
	self.scale = .0125
	self.decal_x = 0
	self.decal_y = 0

	self:add_camera()

	local ix = 1
	local iy = 1
	local sx = 4

	local dx = 3
	ix, iy = 1, iy+1

	self:set_tab_key_def()

	--self.fx,self.fy = 12/8*8,11/7*7
	--local SY = 9
	--hack
	self.fx,self.fy = app:get_capture_suv_meter()

	local SY = 3.5
	iy = iy + .5
	bu = self:add_slider_multi(	{ 1.5, iy+.5,		SY*self.fx/self.fy, SY},		"dsimul", self.number )
--hack
		bu:set_elt_text_xy_f_ratio( -.4, -.2, .4 )
		bu:set_ui_top_size(true)
		bu:set_select_on_click_double()
		bu.draw_back = draw_back_multi
		bu.draw_fore = draw_fore_multi
		self.bu_multi = bu
	iy = iy - .5

	ix = 8
	self:add_trig_method(		{ix,    	iy},	"Flip", 	bu,	"flip_elts_active_all"	)
	self:add_trig_method(		{ix+dx,		iy},	"Set",		bu,	"set_elts_active_all" 	):set_color_back( "on" )
	self:add_trig_method(		{ix+dx*2,	iy},	"Clear",	bu,	"clear_elts_active_all"	):set_color_back( "off" )

	ix = 1
	self:add_button(			{ix,		iy}, 	"Move",		self, "b_move", false )
	self:add_button(			{ix+4,		iy}, 	"Play", 	self, "b_play", false )

	iy = iy + 10
	bu = self:add_slider_two(	{ix,iy,		8,1},	"Speed",	self, "speed_min",	"speed_max", 0, 1, 0, 1 )
		bu:set_meter( false )
	bu = self:add_slider(		{ix+8,iy,	8,1},	"Scale",	self, "scale",		self.scale, 0, 2 )
		bu:set_meter( false )
	iy = iy + 1
	-- bu = self:add_slider(	{ix,iy,		8,1},	"Decal_x",	self, "decal_x",	self.decal_x,	-1, 1 )
	-- 	bu:set_meter( false )
	-- bu = self:add_slider(	{ix+8,iy,	8,1},	"Decal_y",	self, "decal_y",	self.decal_y,	-1, 1 )
	-- 	bu:set_meter( false )
	bu = self:add_selector(		{1,iy,		4,1}, 	"mul_x"		)
		bu:set_nb_min_0( 6 )
		bu:set_target_lua( self, "nb_x" )
		--bu:set_text_draw( false )
	bu = self:add_selector(		{5,iy,		4,1},	"mul_y" 	)
		bu:set_nb_min_0( 6 )
		bu:set_target_lua( self, "nb_y" )
		--bu:set_text_draw( false )

	ix,iy = 1,4
	self:set_tab_key( "Tex" )
	self:add_bu_texture(		{ix,iy,		8,5},	"Elt", 1	)
--	iy = iy + 4
--	self:add_bu_texture( {ix,iy}, "Back", 2	)
end

function meu:init()
	self.t = 0
	local sins = {}
	local coss = {}

	for i=1, 256 do
		sins[i] = math.sin( math.pi / i )
		coss[i] = math.cos( math.pi / i )
	end
	self.sins = sins
	self.coss = coss
end

function meu:my_draw()
	local bu = self.bu_multi

	local bind = self:get_texture_bind_2d(1)

	local fx,fy = self.fx,self.fy
	local nb = bu:get_elt_nb()
	for i=1,nb do
		local elt = bu:get_elt( i )
		if elt:is_contact() or elt:get_value() then
			local x,y = elt:get_xy()
			x = (x-.5) * fx
			y = (y-.5) * fy
---[[
			if self.b_move then
				local ph = elt.ph or 0
				if self.b_play then
					ph = ph + aaa.time.dt * interpolate( self.speed_min, self.speed_max, (i-1)/(nb-1) )
					elt.ph = ph
				end
				local scale = self.scale
				x,y = x + scale * math.sin(ph), y + scale * math.cos(ph)
			end
--]]
			--self:print( i.."drawing elt "..x..","..y )
			local sx,sy = elt:get_sxy()
			sx = sx * fx * .5
			sy = sy * fy * .5
			local function draw( x, y )
				aaa.draw_bind_rect( bind , x-sx, y-sy, x+sx, y+sy )
			end
			draw( x,y )
			--for j=1,self.nb_x do
			--	for k=1,self.nb_y do
			--		draw( x + j * .6, y + k * .6 )
			--	end
			--end
		end
	end
end

function meu:draw_frame()
	gol.set_texture_dim(0)
	gol.color_black(1)
	aaa.draw_rect( -7, 3.8, 7, 4.5 )
	aaa.draw_rect( -7, -4.5, 7, -3.8 )
	aaa.draw_rect( -7, -4, -6.3, 4 )
	aaa.draw_rect( 	6.3, -4, 7, 4 )
end


function meu:update_ui()
	local m = self:get_meu_fbo_used()
	if m then
		local bind = m:get_texture_bind_2d()
		self.bu_multi:set_back_bind( bind )
	end
end

function meu:draw()
	self:set_ui_slot_at_start( 4 )

	self:draw_layers_begin()
		self:draw_layer( 1 )
--		self:draw_layer( 3 )
--		if self.b_abcd then
--			if self.b_a then	self:draw_layer( 4 )	end
--			if self.b_b then	self:draw_layer( 5 )	end
--			if self.b_c then	self:draw_layer( 6 )	end
--			if self.b_d then	self:draw_layer( 7 )	end
--		end

		self:draw_layer( 9 )
		self:my_draw()

--
--		self:draw_frame()
	self:draw_layers_end()
end
