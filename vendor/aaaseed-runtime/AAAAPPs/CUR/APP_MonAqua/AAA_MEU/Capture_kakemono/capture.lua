function meu.draw_multi( bu )
	--oo.getsuper(SLIDER_MULTI).draw(bu)
	local dx = 1/14 * .5
	local dy = 1/9 * .5
	local rsxh = 7.5/14
	local rsyh = 5/9
	gol.color_white( 1 )
	aaa.draw_bind_rect( 64, -.5, -.5, .5, .5 )

	gol.set_line_width( 1 )
	gol.color_cyan( .5 )
	for i = math.ceil( -LV.zone_sxh ) - .5, math.ceil( LV.zone_sxh ) - .5 do
		aaa.draw_line( i/LV.zone_sx, -.5, i/LV.zone_sx, .5 )
	end
	for i = math.ceil( -LV.zone_syh ) - .5, math.ceil( LV.zone_syh ) - .5 do
		aaa.draw_line( -.5, i/LV.zone_sy, .5, i/LV.zone_sy )
	end
	gol.color_blue( .5 )
	for i = math.ceil( -LV.zone_sxh ), math.ceil( LV.zone_sxh ) do
		aaa.draw_line( i/LV.zone_sx, -.5, i/LV.zone_sx, .5 )
	end
	for i = math.ceil( -LV.zone_syh ), math.ceil( LV.zone_syh ) do
		aaa.draw_line( -.5, i/LV.zone_sy, .5, i/LV.zone_sy )
	end
	-- gol.color_white( .5 )
	-- aaa.draw_rects_line(	-.5+dx, -.5+dy,		.5-dx, .5-dy,
	-- 						-.5, .5-rsyh,		-.5+rsxh, .5,
	-- 						.5-rsxh, .5-rsyh,	.5, .5,
	-- 						-.5, -.5, 			-.5+rsxh, -.5+rsyh,
	-- 						.5-rsxh, -.5, 		.5, -.5+rsyh )
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	self.speed = 1
	self.number = 120
	self.scale = .0125
	self.decal_x = 0
	self.decal_y = 0

	self:add_camera()

	self:set_tab_key_def()
	local ix = 1
	local iy = 1
	local sx = 4
	local sy = 1

	self:add_button( {ix,		iy }, "Capture", self, "b_capture", false ):set_text( "Kins" ):set_text_rect_ratio(2)
	self.b_capture_draw = {}
	local nb = 0

	bu = self:add_button( {ix+3,iy }, "Capture_1", self.b_capture_draw, 1, false ):set_color_back( "green" )
		bu:set_text_visible( false )
	bu = self:add_button( {ix+4,iy }, "Capture_2", self.b_capture_draw, 2, false )
		bu:set_text_visible( false )
--	for j = 1,2 do
--		local pre = j==1 and "L" or "R"
--		for i = 1,4 do
--			local x = ix + 3 + ((j-1) * 2.5 + (i-1)%2) * sy
--			local y = iy + ( (i>2) and sy or 0 )
--			nb = nb + 1
--			bu = self:add_button( {x,y }, "Capture_"..pre..string.char(64+i), self.b_capture_draw, nb, false )
--				bu:set_text_visible( false )
--		end
--	end
	self.capture_nb = nb

	iy = 3
	sx = 2
	self.sx = 13.34
	self.sy = 9.5
	local function add_multi( ix, iy, name, number  )
		local SX = self.sx
		local SY = self.sy
		bu = self:add_slider_multi(	{ 1+(16-SX)/2, iy,	SX, SY }	,name, number )
			bu:set_elt_text_xy_f_ratio( -.5, .1, .4 )
			bu:set_select_on_click_double()
			bu.draw = self.draw_multi
			bu.draw_back = function() end
			return bu
	end
	ui.bu_multi_simul = add_multi(	2, iy+1.25,"Simul", 12 )
	ui.bu_multi_cache = add_multi(	2, iy+1.25, "Cache", 8 )
	bu = self:add_selector(	{12,iy,	5,sy}, "which" )
		bu:set_nb( 2 )
		bu:set_target_lua( self, "s_multi" )
		bu:set_item_text( 1, "Simul", "Cache" )

	self:add_button( {ix,		iy, 	sx, sy },	"Move", self, "b_move", false )
	self:add_button( {ix+sx,	iy, 	sx, sy },	"Play", self, "b_play", false )
	ix = ix + 4
	self:add_trig_method(	{ix,    		iy,			sx, sy}, 	"Flip", 	ui.bu_multi_simul,	"flip_elts_active_all"	)
	self:add_trig_method(	{ix+sx,		iy,			sx, sy}, 	"Set",		ui.bu_multi_simul,	"set_elts_active_all" 	):set_color_back( "on" )
	self:add_trig_method(	{ix+sx*2,	iy,			sx, sy}, 	"Clear",	ui.bu_multi_simul,	"clear_elts_active_all"	):set_color_back( "off" )
	ix = ix - 4
	iy = iy + 11

	sx = 4
	bu = self:add_slider_two(	{ix,iy,	sx,1},	"Speed",	self, "speed_min",	"speed_max", 0, 1, 0, 1 )
		bu:set_meter( false )
	bu = self:add_slider(	{ix+sx,iy,	sx,1},	"Scale",	self, "scale",		self.scale, 0, 2 )
		bu:set_meter( false )
	iy = iy + 1
--	bu = self:add_slider(	{ix,iy,	8,1},	"Decal_x",	self, "decal_x",		self.decal_x, -1, 1 )
--		bu:set_meter( false )
--	bu = self:add_slider(	{ix + 8,iy,	8,1},	"Decal_y",	self, "decal_y",		self.decal_y, -1, 1 )
--		bu:set_meter( false )

--table.print( self, "nb_x avant" )
	bu = self:add_selector(	{1,iy,	4,1}, "mul_x" )
		bu:set_nb( 6 )
		bu:set_target_lua( self, "nb_x" )
--table.print( self, "nb_x apres" )
		--bu:set_text_draw( false )
	bu = self:add_selector(	{5,iy,	4,1}, "mul_y" )
		bu:set_nb( 6 )
		bu:set_target_lua( self, "nb_y" )
		--bu:set_text_draw( false )

	ix,iy = 1,1
	self:set_tab_key( "Tex" )
	self:add_bu_texture_target_unit( {ix,iy}, "Elt", 1	)

end

function meu:init()
	self.t = 0
end

function meu:update_ui()
	local ui = self.ui
	ui.bu_multi_simul:set_active( self.s_multi == 1 )
	ui.bu_multi_cache:set_active( self.s_multi == 2 )
end
function meu:my_draw( bu, b_simul )
	local bind
	if b_simul then
		gol.set_texture_dim( 2 )
		bind = self:get_texture_bind_2d( 1 )
		--self:print( bind )
	else
		gol.set_texture_dim( 0 )
		gol.color_black()
		gol.set_blend_min()
	end

	local nb = bu:get_elt_nb()
	for i=1,nb do
		local elt = bu:get_elt( i )
		-- if i==1 then
		-- 	self:print( elt.." "..elt:get_value() )
		-- 	table.print( elt.__balues[1], "elt 1" )
		-- end
		if elt:is_contact() or elt:get_value() then
			local x,y = elt:get_xy()
			x = (x-.5)*self.sx
			y = (y-.5)*self.sy
			local sx,sy = elt:get_sxy()
			sx = sx * self.sx *.5
			sy = sy * self.sy * .5
			--self:print( i.."drawing elt "..x..","..y )
			local function draw( x, y ) aaa.draw_bind_rect( bind, x-sx, y-sy, x+sx, y+sy ) end
			if b_simul then
				if self.b_move then
					local ph = elt.ph or 0
					if self.b_play then
						ph = ph + aaa.time.dt * interpolate( self.speed_min, self.speed_max, (i-1)/(nb-1) )
						elt.ph = ph
					end
					local dx = self.scale * math.sin( ph )
					local dy = self.scale * math.cos( ph )
					x,y = x+dx,y+dy
				end
				for j=1,self.nb_x do
					for k=1,self.nb_y do
						draw( x + (j-1) * .6, y + (k-1) * .6 )
					end
				end
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
	self:set_ui_slot_at_start( 4 )

	self:draw_layers_begin()
		self:draw_layer( 1 )
		self:draw_layer( 3 )
		if self.b_capture then
			if self.b_capture_draw[1] then	self:draw_layer( 4 )	end
			if self.b_capture_draw[2] then	self:draw_layer( 5 )	end
		end

		--self:draw_layer( 14 )
		self:my_draw( self.ui.bu_multi_simul, true )
		self:my_draw( self.ui.bu_multi_cache, false )

		self:draw_frame()
	self:draw_layers_end()
end
