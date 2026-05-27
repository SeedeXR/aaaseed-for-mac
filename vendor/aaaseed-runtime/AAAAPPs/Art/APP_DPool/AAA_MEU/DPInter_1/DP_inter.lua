function meu.draw_multi( bu )
	--oo.getsuper(SLIDER_MULTI).draw(bu)
	local dx = 1/14 * .5
	local dy = 1/9 * .5
	local rsxh = 7.5/14
	local rsyh = 5/9
	--gol.color_white( 1 )
--	aaa.draw_bind_rect( 64, -.5, -.5, .5, .5 )
	gol.color_white( .5 )
	gol.set_line_width( 1 )

	gol.color_white( 1. )
	aaa.draw_rect_line( -.5, -.5, .5, .5 )
	gol.color_white( .5 )
	aaa.draw_rects_line(	-.5+dx, -.5+dy,		.5-dx, .5-dy,
							-.5, .5-rsyh,		-.5+rsxh, .5,
							.5-rsxh, .5-rsyh,	.5, .5,
							-.5, -.5, 			-.5+rsxh, -.5+rsyh,
							.5-rsxh, -.5, 		.5, -.5+rsyh )
end
function meu:define_ui()
	local ref = self.ref
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

	self:add_button( {ix,		iy }, "Capture", self, "b_capture", false )
	iy = iy + 1
	self.b_capture_draw = {}
	for i = 1,self.capture_nb do
		bu = self:add_button( {ix + (i-1) * sy,	iy, sy,sy }, "Capture_"..i, self.b_capture_draw, i, false )
			bu:set_text_visible( false )
	end

	local iy = 3
	local sx = 2
	bu = self:add_slider_multi(		{ 2, iy+1,	14, 9 }	,"dsimul", self.number )
		bu:set_elt_text_xy_f_ratio( -.5, .1, .4 )
		bu:set_select_on_click_double()
		bu.draw = self.draw_multi
		bu.draw_back = function() end
		self.bu_multi = bu
	self:add_button( {ix,		iy, 	sx, sy },	"Move", self, "b_move", false )
	self:add_button( {ix+sx,	iy, 	sx, sy },	"Play", self, "b_play", false )
	ix = ix + 4
	self:add_trig_method(	{ix,    		iy,			sx, sy}, 	"Flip", 	bu,	"flip_elts_active_all"	)
	self:add_trig_method(	{ix+sx,		iy,			sx, sy}, 	"Set",		bu,	"set_elts_active_all" 	):set_color_back( "on" )
	self:add_trig_method(	{ix+sx*2,	iy,			sx, sy}, 	"Clear",	bu,	"clear_elts_active_all"	):set_color_back( "off" )
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

	ix,iy = 1,4
--	self:set_tab_key( "Tex" )
--	self:add_bu_texture_target_unit( {ix,iy}, "NoElt", 1	)

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

	self.capture_nb = 4
end

function meu:my_draw()
	local bu = self.bu_multi

	local nb = bu:get_elt_nb()
	for i=1,nb do
		local elt = bu:get_elt( i )
		-- if i==1 then
		-- 	self:print( elt.." "..elt:get_value() )
		-- 	table.print( elt.__balues[1], "elt 1" )
		-- end
		if elt:is_contact() or elt:get_value() then
			local x,y = elt:get_xy()
			x = (x-.5)*14
			y = (y-.5)*9
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
			--self:print( i.."drawing elt "..x..","..y )
			local sx,sy = elt:get_sxy()
			sx = sx * 14 *.25
			sy = sy * 9 * .25
			local function draw( x, y ) aaa.draw_bind_rect( 2, x-sx, y-sy, x+sx, y+sy ) end
			for j=1,self.nb_x do
				for k=1,self.nb_y do
					draw( x + (j-1) * .6, y + (k-1) * .6 )
				end
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
			for i = 1,self.capture_nb do
				if self.b_capture_draw[i] then self:draw_layer( i+3 ) end
			end
		end
		self:my_draw()
		self:draw_layer( 9 )
		self:draw_frame()

	self:draw_layers_end()
end
