--todo extend to deal with apparition/disparition by square by square (have an analog value by square and paramter on how to use it

function meu:define_meu_infos( )
	return { 	author = "Mâa",
				tags = { "2d", "3d", "Point", "coregraphic", "procedural", "draw", "art", "vj", "Utility" },
				help =	{	"generete space invader pattern with plenty of option.",
							"very useful base MEU to test rendering chain."
						}
			}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = 1.2
	local DY = .2

	self.seed = 242 -- pick to be interesting in 5x5 and 7x7
	self.phase = 0

	bu = self:add_slider(		{ix,iy,			4,SY},		"Size",				self, "size",			4,	0,8	)
		bu:add_values_def( 1,3,4,5,6,7 )	
	bu = self:add_slider(		{ix+4,iy,		4,SY},		"Factor U",			self, "size_factor_u",	1,	0,8	)
			:set_color_back("u")
	iy = iy + SY
	bu = self:add_slider(		{ix,iy,			4,SY},		"Inside",			self, "size_int", 		1,	0,1	)
		bu:add_values_def( .9,.95,.98)
	iy = iy + SY
	bu = self:add_slider(		{ix,iy,			4,SY},		"Threshold",		self, "threshold", 		.5,	0,1	)
		bu:add_values_def( .5,.7 )
	bu = self:add_slider(		{ix+4,iy,		4,SY},		"Threshold Range",	self, "th_range", 		0,	0,1	)
		bu:add_values_def( .5,.7 )

	iy = iy + SY + DY

	bu = self:add_trig_method(	{ix,iy,			2,SY},		"Seed",				self, "reseed"			)
		:set_color_back("restart")
	bu = self:add_button( 		{ix+2,iy,		SY,SY},		"Time active",		self, "b_time_active",	false	)
		bu:set_text_visible(false)
	bu = self:add_slider(		{ix+2+SY,iy,	6-SY,SY},	"BPM",				self, "bpm", 			60,	1,256	)
	iy = iy + SY
	bu = self:add_text_info(	{ix,iy,			8,SY*.5},	"Seed value"		)
		ui.bu_seed_info = bu
--		bu:set_target_lua( self, "seed" )
	iy = iy + SY*.8 + DY

	bu = self:add_selector(		{ix,iy,			8,SY},		"Symetry"			)
		bu:set_nb_min_0( 7 )
		bu:set_item_text( 2, "U", "V", "UV", "/", "\\", "X" )
		bu:set_target_lua( self, "s_sym" )
		bu:set_text_draw( false )		
	iy = iy + SY + DY

	bu = self:add_selector(		{ix,iy,			8,SY},		"Nb_use"			)
		bu:set_nb_min_0( 7 )
		bu:set_item_text( 1, "Free", "=", "5", "6", "7", "8", "9" )
		bu:set_item_data( 3, 5,6,7,8,9 )
		bu:set_target_lua( self, "s_nb" )
		bu:set_text_draw( false )
		ui.bu_nb = bu
	iy = iy + SY

	bu = self:add_slider(		{ix,iy,			4,SY},		"nb_u",				self, "nb_u", 			8,	0,64	)
		bu:set_color_back("u"):set_value_type_integer(true)
		bu:add_values_def( 11,13,17,19,23,29,31 )
	bu = self:add_slider(		{ix+4,iy,		4,SY},		"nb_v",				self, "nb_v", 			8,	0,64	)
		bu:set_color_back("v"):set_value_type_integer(true)
		bu:add_values_def( 11,13,17,19,23,29,31 )	
	iy = iy + SY

	self:add_rendering()

	ix,iy = 9,2.2+DY
	self:add_camera():set_preset_use( false )
	self:add_transfo(			{9,iy,			nil, 2.4}, 1 )
	iy = iy + 2.4 + DY
	self:add_bu_texture_target_unit( 	{ix,iy,			nil,6},		"TEX",	1, true	)
	iy = iy + 6 + DY
	self:add_rgbf(				{ix,iy,			8,SY})
	iy = iy + SY
	self:add_blending( 			{ix,iy})
	iy = iy + SY + DY
--	bu = self:add_slider(		{ix,iy,			8,SY},		"DEMO",	nil, nil, 	8,	0,64	)


end

function meu:update_ui()
	self.ui.bu_seed_info:set_text( self.seed )
end

-- make it more simple to define an adaptated coor system
-- here we use [0,5]
function meu:draw_icon()
	local function draw( x1,y1, x2,y2 )
		aaa.draw_rect(	-.47+x1*.2/5, -.5+y1*.2, 	-.47+x2*.2/5, -.5+y2*.2 )
	end
	draw( 0,0, 1,2 )
	draw( 1,1, 4,3 )
	draw( 4,0, 5,2 )
	draw( 2,3, 3,5 )
end

function meu:draw_invaders( su,sv, nb_u, nb_v, size_int, threshold )
	nb_u = nb_u or 5
	nb_v = nb_v or 5
	su = su or 1
	sv = sv or 1
	local du = su / nb_u
	local dv = sv / nb_v
	local interval = (1-size_int) * dv
	local suh = (du - interval) * .5
	local svh = (dv - interval) * .5
	if self.size_int < 0 then suh = -suh end

	local alpha = self:get_alpha()

	local function random()		return math.random() < threshold	end
	local function draw( iu,iv, b_debug )
		--self:print( iu.." "..iv )
		b_debug = false
		if b_debug then
			gol.color_green( alpha )
		end
		local u = du*iu
		local v = dv*iv
		if true then
			--if b_debug then
			--	aaa.draw_rect_uv( u-suh*.5,v-svh*.5, u+suh*.5,v+svh*.5 )
			--else
				aaa.draw_rect_uv( u-suh,v-svh, u+suh,v+svh )
			--end
		else
			aaa.draw_disk_axe_z( u,v,0, suh*2, 24 )
		end
		if b_debug then
			gol.color_white( alpha )
		end
	end

	local s_sym = self.s_sym
	if s_sym==1 then		-- symmetry in u
		for iu = (-nb_u+1)/2,0 do
			local b = iu < 0
			for iv = 1-(nb_v+1)/2,(nb_v-1)/2 do
				if random() then
					draw( iu,iv )
					if b then draw( -iu,iv, true ) end
				end
			end
		end
	elseif s_sym==2 then	-- symmetry in v
		for iv = 1-(nb_v+1)/2,0 do
			local b = iv < 0
			for iu = 1-(nb_u+1)/2,(nb_u-1)/2 do
				if random() then
					draw( iu,iv )
					if b then draw( iu,-iv, true ) end
				end
			end
		end
	elseif s_sym==3 then	-- symmetry in u and v
		for iu = (-nb_u+1)/2,0 do
			local b = iu < 0
			for iv = 1-(nb_v+1)/2,0 do
				if random() then
					draw( iu,iv )
					if b then
						draw( -iu,iv, true )
						if iv < 0 then 
							draw( -iu,-iv, true )
						end
					end
					if iv < 0 then 
						draw( iu,-iv, true )
					end
				end
			end
		end
	elseif s_sym==4 then	-- symmetry in diagonal
		for iv = 1-(nb_v+1)/2,(nb_v-1)/2 do
			for iu = iv,(nb_v-1)/2 do
				if random() then
					draw( iu,iv )
					if iu~=iv then draw( iv,iu, true ) end
				end
			end
		end
	elseif s_sym==5 then	-- symmetry in other diagonal
		for iv = 1-(nb_v+1)/2,(nb_v-1)/2 do
			for iu = iv,(nb_v-1)/2 do
				if random() then
					draw( -iu,iv )
					if iu~=iv then draw( -iv,iu, true ) end
				end
			end
		end
	elseif s_sym==6 then	-- symmetry in both diagonals
		for iv = 1-(nb_v+1)/2,(nb_v-1)/2 do
			for iu = iv,-iv do
				if random() then
					draw( iu,iv )
					if iu~=iv then
						draw( iv,iu, true )
					end
					if iu~=-iv then
						draw( -iv,-iu, true )
						if iu~=iv then
							draw( -iu,-iv, true )
						end
					end
					--if iu~=iv then draw( -iv,iu, true ) end
				end
			end
		end
	else
		for iu = (-nb_u+1)/2,(nb_u-1)/2 do
			for iv = 1-(nb_v+1)/2,(nb_v-1)/2 do
				if random() then
					draw( iu,iv )
				end
			end
		end
	end

end

function meu:set_seed( seed )
	self.seed = seed
end

function meu:reseed()
--	math.randomseed( math.floor( aaa.time.t * 1024 ) )
	self:set_seed( math.floor( aaa.time.t * 1024 ) )
	--math.random(15668458944256)
end

function meu:draw()
	--aaa.debug.print_traceback()
	local ui = self.ui
	self:draw_layers_begin()
		self:draw_layer(1)
		--aaa.show(self.b_time_active)
		if self.b_time_active then
			local ph = self.phase
			ph = ph + aaa.time.dt * self.bpm / 60
			if ph > 1 then
				self:reseed()
				self.phase = 0
			else
				self.phase = ph
			end
			--aaa.show( ph, "ph" )
		end
		math.randomseed( self.seed )
--		math.randomseed( 242 )

--		math.randomseed( math.floor( aaa.time.t * 7.5 /1 ) )

		local nu, nv
		if self.s_nb>=2 then	nu = ui.bu_nb:get_item_data()
		else					nu = self.nb_u
		end
		if self.s_nb>=1 then	nv = nu
		else					nv = self.nb_v
		end
		local th = self.threshold + self.th_range * (math.random()-.5)
		self:draw_invaders( self.size * self.size_factor_u, self.size, nu,nv, self.size_int, th )
	self:draw_layers_end()
end