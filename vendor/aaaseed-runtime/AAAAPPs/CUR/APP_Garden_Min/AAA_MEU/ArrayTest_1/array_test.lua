function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = 1.2
	local DY = .2

	self.seed = 242 -- pick to be interesting in 5x5 and 7x7
	self.phase = 0

	bu = self:add_trig_method(	{ix,iy,		4,SY},	"Gene",		self, "gene" )
		:set_text_inside( true )
		:set_color_back( "action" )
	iy = iy + SY

	bu = self:add_slider(		{ix,iy,		8,SY},	"Size",		self, "size",	4, 0, 8 )
		bu:add_values_def( 1,3,4,5,6,7 )	
	iy = iy + SY + DY

	bu = self:add_button(		{ix,iy,		SY,SY},	"Draw",		self, "b_draw",			true )
	iy = iy + SY
	bu = self:add_button(		{ix+1,iy,	SY,SY},	"Lines",	self, "b_draw_line",	true )
	iy = iy + SY
	bu = self:add_button(		{ix+1,iy,	SY,SY},	"Points",	self, "b_draw_point",	false )
	iy = iy + SY + DY

-- 	bu = self:add_text_info(	{ix,iy,	8,SY*.5}, "Seed value"	)
-- 		ui.bu_seed_info = bu
-- --		bu:set_target_lua( self, "seed" )
-- 	iy = iy + SY*.8 + DY

-- 	bu = self:add_selector(	{ix,iy,	8,SY},	"Nb_use" )
-- 		bu:set_nb_min_0( 7 )
-- 		bu:set_item_text( 1, "Free", "=", "5", "6", "7", "8", "9" )
-- 		bu:set_item_data( 3, 5,6,7,8,9 )
-- 		bu:set_target_lua( self, "s_nb" )
-- 		bu:set_text_draw( false )
-- 		ui.bu_nb = bu
-- 	iy = iy + SY

 	ix,iy = 9,3
 	self:add_camera():set_preset_use( false )

-- 	self:add_bu_texture_target_unit( {ix,iy, nil,6}, "TEX", 1, true	)
-- 	iy = iy + 6 + DY
-- 	self:add_rgbf(	{ix,iy,	8,SY })
-- 	iy = iy + SY
-- 	self:add_blending( {ix,iy} )

-- end
end

function meu:init()
	self.array = nil
end

function meu:reseed()
--	math.randomseed( math.floor( aaa.time.t * 1024 ) )
	self.seed = math.random(15668458944256)
end

function meu:gene()
	local function transform( t,io, ii )	 
		t[io] = t[ii] + ii /100000.
		t[io+1] = t[ii+1] + ii /100000.
		t[io+2] = t[ii+2] + .001
	end

	local function fill( t, io )
		local si = self.size
		local ph = io*.025
		local d = io * .01
		local r = io * .01
		local s	= math.sin( ph )
		local c = math.cos( ph )
		t:set( io,
				r*s + si, r*c + si, d,
				r*s + si, r*c, d,
				r*s ,r*c ,d
			)
	end

	local nb = 200000
	local t = aaa.array.new_fp32( nb*3*3 )
	for i=1,nb*3*3,9 do
		local id = i
		fill( t, id )
	end
	self.array = t
end

function meu:gene()
	local function transform( t,io, ii )	 
		t[io] = t[ii] + ii /100000.
		t[io+1] = t[ii+1] + ii /100000.
		t[io+2] = t[ii+2] + .001
	end

	local function fill( t, io )
		local si = self.size
		local ph = io*.025
		local d = io * .00001
		local r = io * .0001
		local s	= math.sin( ph )
		local c = math.cos( ph )
		t:set( io, r*s, r*c, d )
		--t[io]	= r*s
		--t[io+1] = r*c
		--t[io+2] = d
	end

	local nb = 5000000
	local t = aaa.array.new_fp32( nb*3 )
	for i=1,nb*3, 3 do
		local id = i
		fill( t, id )
	end
	self.array = t
end

function meu:update()
	if not self.array then
		self:gene()
	end
end

function meu:draw()

	self:draw_layers_begin()
		self:draw_layer(1)
		--gol.draw_triangles_3d( self.array )
		if self.b_draw then
			if self.b_draw_line then
				gol.draw_line_strip_3d( self.array )
			end
			if self.b_draw_point then
				gol.draw_points_3d( self.array )
			end
		end
	self:draw_layers_end()
end