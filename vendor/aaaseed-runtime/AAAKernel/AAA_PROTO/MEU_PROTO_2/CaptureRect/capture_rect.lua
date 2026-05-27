 function meu:define_meu_infos( )
	return { 	author = "Mâa",
				version = 0,
				tags = { "procedural", "2d", "Draw", "camera", "core", "coregraphic", "input", "texture", "unfinished" },
				help = { "Agglomerate camera inputs using a series of adjustable grids,",
							" simulate public bodies. edit and draw caches on top",
						}
			}
end

local function draw_multi( bu )
	gol.color_white( 1 )
	local meu_fbo = bu.__multi_meu_caller:get_meu_fbo_used()
	if meu_fbo then
		local bind = meu_fbo:get_fbo():get_attachment_bind( 1 )
		aaa.draw_bind_rect( bind, -.5,-.5, .5,.5 )
	end
	gol.color_white( .25 )
	gol.set_line_width( 1 )
	local S = .5
	for f=-.5+S,.5-S,S do
		aaa.draw_line( f, -.5, f, .5 )
		aaa.draw_line( -.5, f, .5, f )
	end
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	self.speed = 1
	self.number = 120
	self.decal_x = 0
	self.decal_y = 0

	self:add_camera()

	self:set_tab_key_def()
	local ix = 1
	local iy = 1
	local sx = 4
	local sy = 1

-- CAPTURE INPUT	
	self:add_button( {ix,iy,	2,sy*.999 },	"Capture", self, "b_capture", false )
		:set_text( "In" )
		:set_text_rect_ratio(1)
	self.b_capture_draw = {}
	for j = 1,self.capture_nb_v do
		for i = 1,self.capture_nb_u do
			local capture_id = (j-1) * self.capture_nb_u + i
			bu = self:add_button( {ix+i+1,iy+j-1, sy,sy}, "Capture_"..capture_id, self.b_capture_draw, capture_id, false )
				bu:set_text_visible( false )
		end
	end
	self:add_trig_method( {ix,iy+sy, 2,sy*.999 },	"Focus", self, "set_focus", true )

-- SIMUL CACHE	
	iy = 3.2
	sx = 2
	--todo deal with vertical case too

	local function add_multi( ix,iy, name, number  )
		local SX = 16
		--self:print( self.sx.." by "..self.sy )
		local SY = SX * self.sy/self.sx
		if SY > 10 then
			SY = 9
			SX = SY * self.sx/self.sy
		end
		--self:print( "SY is "..SY )
		--todo -1.2 on y is a pure hack, position of slider multi should be done with a fit to rect fns
		bu = self:add_slider_multi(	{1+(16-SX)/2,iy-1.2,		SX,SY}, name, number )
			bu:set_elt_text_xy_f_ratio( -.5, .1, .4 )
			bu:set_select_on_click_double()
	--		bu.draw = self.draw_multi
			bu.draw_back = draw_multi
			bu.__multi_meu_caller = self
			return bu
	end

	ui.bu_multi_simul = add_multi(	2, iy+2.25, "Simul", 12 )
	ui.bu_multi_cache = add_multi(	2, iy+2.25, "Cache", 8 )

	sx = 4

	sx = 4/3
	local sxs = 1.5
	bu = self:add_selector(	{ix,iy,	sxs,sy}, "move" )
		bu:set_nb( 1, 2 )
		bu:set_item_text( 1, "Simul", "Cache" )
		bu:set_target_lua( self, "s_multi" )
		bu:set_text_visible( false )
	ix = ix + sxs
	--todo apply to the currectly selected multi
	self:add_trig_method(	{ix,    	iy,				sx,sy}, 	"Flip", 		self,"apply_method_to_multi",	"flip_elts_active_all"	)
	self:add_trig_method(	{ix+sx,		iy,				sx,sy}, 	"Set",			self,"apply_method_to_multi",	"set_elts_active_all" 	):set_color_back( "on" )
	self:add_trig_method(	{ix+sx*2,	iy,				sx,sy}, 	"Clear",		self,"apply_method_to_multi",	"clear_elts_active_all"	):set_color_back( "off" )
	self:add_slider(		{ix+sx*2+sx,iy,				2.5,sy}, 	"Alpha",		ui.bu_multi_simul,	"alpha",	1, 0,1 	)

	ix = ix - sxs

	local syt = sy * 2/3

	bu = self:add_selector(	{ix+8,		iy+sy-syt*3,	4,syt*2},	"Simul Draw"	)
		bu:set_nb( 2, 2 )
		bu:set_target_lua( self, "s_simul_draw" )
		bu:set_item_text( 2, "Rect", "Tex", "Bodies" )	
		bu:set_text_visible( false )

	bu = self:add_selector(	{13,		iy-sy,			4,syt*2}, 	"Move Type"		)
		bu:set_nb( 4, 1 )
		bu:set_target_lua( self, "s_move" )
		bu:set_item_text( 1, "No", "Turn", "-X", "X" )
		bu:set_text_visible( false )

	self:add_button(		{ix+8,		iy+sy-syt,		4,syt}, 	"Cache Draw",	self,	"b_cache", 		true	):set_text( "Cache" )
	bu = self:add_selector(	{ix+12,		iy+sy-syt,		4,syt},		"Cache Color"	)
		bu:set_nb( 3 )
		bu:set_target_lua( self, "s_cache_color" )
		bu:set_item_text( 1, "Black", "Grey", "White" )
		bu:set_text_visible( false )
	iy =  14


	sx = 4
	bu = self:add_button(	{ix+sx*2, 	iy,		sy,sy},		"Turb_active",	self, "b_turbulence_active",	false	)
		bu:set_text_visible( false)
	bu = self:add_slider(	{ix+sx*2+1,	iy,		sx-1,sy},	"Turb",			self, "turbulence_factor",		0,		0,1 )
		bu:set_meter( false )
	bu = self:add_slider(	{ix+sx*3,	iy,		sx,sy},		"Turb Speed",	self, "turbulence_speed",		0,		0,1 )
		bu:set_meter( false )
	bu = self:add_button({	ix+sx,		iy,		sy,sy},		"Speed On",		self, "b_speed", 				true	)
		:set_text_visible(false)
	bu = self:add_slider_two({ix+sx+sy,	iy,		sx-sy,sy},	"Speed",		self, "speed_min",	"speed_max", -.5,.5, -1, 1 )
		:set_meter( false )
		:add_values_def( 0 )
	bu = self:add_slider(	{ix,		iy,		sx,sy},		"Radius",		self, "radius",					.25,	0,2 )
		bu:set_meter( false )
	iy = iy + 1
--	bu = self:add_slider(	{ix,		iy,		8,1},		"Decal_x",		self, "decal_x",				self.decal_x,	-1,1 )
--		bu:set_meter( false )
--	bu = self:add_slider(	{ix+8,		iy,		8,1},		"Decal_y",		self, "decal_y",				self.decal_y,	-1,1 )
--		bu:set_meter( false )

	local function add_selector( ix,	iy,		dir )
		local bu
		bu = self:add_selector(	{ix,	iy,		4,1},		"mul_"..dir )
			bu:set_nb( 6 )
			bu:set_target_lua( self, "nb_"..dir )
		--bu:set_text_draw( false )
		return bu
	end
	bu = add_selector(		1,iy,	"x" )
	bu = add_selector(		5,iy,	"y" )

	ix,iy = 1,1
	self:set_tab_key( "Tex" )
	self:add_bu_texture_target_unit( {ix,iy, 4, 3.2}, "Elt", 1	)

end
function meu:apply_method_to_multi( method, ... )
	local bu = self.ui.bu_multi_simul
	bu[method]( bu, ... )
end
function meu:init()
	self.t = 0

	self.sx,self.sy = app:get_capture_suv_meter()

	local nb_u, nb_v
	if app and app.get_capture_nb_uv then
		nb_u, nb_v = app:get_capture_nb_uv()
	else
		nb_u, nb_v = 1,1
	end
	self.capture_nb_u = nb_u
	self.capture_nb_v = nb_v
end

function meu:init_after()
	--self:print( "On site "..app:is_onsite() )
	--todo understand and fix that the else version don't function
	local b_onsite = app:is_onsite()
	-- if 1 then
	-- 	self:set_capture_active( b_onsite )
	-- 	if b_onsite then
	-- 		self.s_simul_draw = 1
	-- 	end
	-- else
	-- 	self:set_bu_value( "capture", b_onsite )
	-- 	if b_onsite then
	-- 		self:set_bu_value( "Simul Draw", 1 )
	-- 	end
	-- end
end

function meu:update_ui()
	local ui = self.ui

	ui.bu_multi_simul:set_active( self.s_multi == 1 )
	ui.bu_multi_cache:set_active( self.s_multi == 2 )

--	self:box_debug( "Field s_simul_draw is "..self.s_simul_draw )
end

function meu:__my_draw_from_multi( bu, b_simul, s_simul_draw, s_color )
	local dim
	--aaa.show( b_simul.." "..s_simul_draw, "Mode multi" )
	if b_simul and s_simul_draw >= 3 then
		--self:print( "There" ) 
		dim = 2
		local bind = self:get_texture_bind_2d( 1 )
		gol.bind_texture( bind )
		--self:print( "There bind is "..bind ) 
		--todo add ui or detection of pixel mode to switch  this
		if true then
			gol.set_blend_add()
		else
			gol.set_blend_max()
		end
		gol.color_white( self:get_alpha() )
	else
		dim = 0
		--self:print( "s_color is "..s_color )
		if s_color == 1 then			
			gol.set_blend_min()
			gol.color_black()
		else
			gol.set_blend_max()
			if s_color == 2 then
				gol.color( .5 )		
			else
				gol.color_white()
			end
		end
	end

	gol.set_texture_dim( dim )
	if dim ~=0 then
		gol.set_quad_uv()
		gol.set_wrap_2d_edge()	-- because in 2022 we store this by texture
	end

	local function draw( i, x,y, sx,sy )
		--self:print( self.nb_x.." "..self.nb_y )
		for iu=1,self.nb_x do
			local x2 = x + (iu-1) * sx*2.1
			for iv=1,self.nb_y do
				local y2 = y + (iv-1) * sy*2.1
				aaa.draw_rect_uv( x2-sx, y2-sy, x2+sx, y2+sy )
			end
		end
	end

	--todo get_ratio of fbo cam ...
	local SX,SY = app:get_capture_suv_meter()

	--aaa.show( self.sx.." "..self.sy, "sxy" )

	local nb = bu:get_elt_nb()
	math.randomseed( 42 )
	for i=1,nb do
		if b_simul then
			gol.color( (.5 + math.random() * .5) * bu:get_alpha_bu_to_draw() )
		end
		local elt = bu:get_elt( i )
		-- if i==1 then
		-- 	self:print( elt.." "..elt:get_value() )
		-- 	table.print( elt.__balues[1], "elt 1" )
		-- end
		if elt:is_contact() or elt:get_value() then
			local x,y = elt:get_xy()
			x = (x-.5) * SX
			y = (y-.5) * SY

			--aaa.show( x, "x" )
			local sx,sy = elt:get_sxy()
			sx = sx * SX * .5
			sy = sy * SY * .5

			--self:print( i.."drawing elt "..x..","..y )

			if b_simul then
				local ph = elt.ph or 0
				if self.b_speed then
					ph = ph + aaa.time.dt * interpolate( self.speed_min, self.speed_max, (i-1)/(nb-1) )
				end
				elt.ph = ph
				if self.b_turbulence_active then
					ph = ph + aaa.math.get_turbulence( x, y, i+ aaa.time.t*self.turbulence_speed, 1, 1 ) * self.turbulence_factor
				end
				if self.s_move==2 then
					ph = ph * math.pi2
					x = x + self.radius * math.sin( ph )
					y = y + self.radius * math.cos( ph )
				elseif self.s_move==3 then
					x = -wrap_01(ph) * 12 + 6
				elseif self.s_move==4 then
					x = wrap_01(ph) * 12 - 6
				end
				if s_simul_draw == 4 then
					local bind = 224 + (i-1) % 5
					gol.bind_texture( bind )
					--self:print( "bind is "..bind )
				end
				draw( i, x,y, sx,sy )
			else
				aaa.draw_rect( x-sx, y-sy, x+sx, y+sy )
			end
		end
	end
end

function meu:draw_frame()
	gol.set_texture_dim(0)
	gol.color_black( 1)
--	aaa.draw_rect( -7, 3.8, 7, 4.5 )
--	aaa.draw_rect( -7, -4.5, 7, -3.8 )
--	aaa.draw_rect( -7, -4, -6.3, 4 )
--	aaa.draw_rect( 	6.3, -4, 7, 4 )
end

function meu:draw()
	self:draw_layers_begin()

		self:draw_layer( 1 )
		self:draw_layer( 3 )
		
		if self.b_capture then
			for j = 1,self.capture_nb_v do
				for i = 1,self.capture_nb_u do
					local capture_id = (j-1) * self.capture_nb_u + i
					if self.b_capture_draw[capture_id] then
						 self:draw_layer( capture_id + 3 )
						-- self:print( "drawing capture "..i)
						-- self:print( "drawing layer "..(i + ((i>4) and 4 or 3 )))
					end
				end
			end
		end

		self:draw_layer( 14 )
		if self.s_simul_draw > 1 then
		 	self:__my_draw_from_multi( self.ui.bu_multi_simul, true, self.s_simul_draw  )
		end
		if self.b_cache then
			self:__my_draw_from_multi( self.ui.bu_multi_cache, false, 2, self.s_cache_color )
		end

--		self:draw_frame()

	self:draw_layers_end()
end

function meu:set_capture_active( b )	self.b_capture = b	end
function meu:set_simul_active( b )		self.b_simul = b	end