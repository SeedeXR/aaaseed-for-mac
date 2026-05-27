function meu:define_ui()
	self.__boids_meus = nil

	local ref = self.ref
	local bu
	local par
	local ui = self.ui

	self:add_camera()

	local ix = 1
	local iy = 1.5
	local SY = 1

	self:add_trig(		{ix,iy,		4,SY}, "Restart" )
		:set_target_lua( self, "b_restart" )
	iy = iy + SY*1.5

	bu = self:add_selector(	{ix,iy,	8,SY}, "Boids nb" )
		bu:set_nb( 12, 1 )
		bu:set_target_lua( self, "boids_nb" )
		bu:set_item_text_from_nb()

	iy = iy + SY
	self:add_slider(	{ix,iy,		8,SY},	"Nb by Boid"		):set_min_max( 1,256 ):set_value_type_integer(true):set_target_lua( self, "nb_by_boid" )
	self:add_slider(	{ix,iy+SY,	8,SY},	"Viscosity" 		):set_min_max( .001, 999 ):set_target_lua( self, "viscosity" )
	self:add_slider(	{ix,iy+SY*2,8,SY},	"Repulse Dist" 		):set_min_max( 0,1 ):set_target_lua( self, "repulse_dist" )
	iy = iy + 4.2
	self:add_button(	{ix,		iy,SY},	"flock active" 		):set_value(false):set_text_visible( false ):set_target_lua( self, "b_flock" )
	self:add_slider(	{ix+1,iy,	7,SY},	"flock dist factor"	):set_min_max( .25, 3 ):set_target_lua( self, "flock_dist_factor" )
	iy = iy + 1.2
	self:add_button(	{ix,		iy,SY},	"Speed active"		):set_value(false):set_text_visible( false ):set_target_lua( self, "b_speed" )
	self:add_slider(	{ix+1,iy,	7,SY},	"Speed"				):set_min_max( -1,1 ):set_target_lua( self, "speed" )
	iy = iy + 1.2
	self:add_button(	{ix,		iy,SY},	"Field active"		):set_value(false):set_text_visible( false ):set_target_lua( self, "b_field" )
	self:add_slider(	{ix+1,iy,	7,SY},	"Field"				):set_min_max( -1,1 ):set_target_lua( self, "field_influence" )

	ix = 9
	iy = 2.5
	--self:add_slider(	{ix,iy+SY,	4,nil}, "Alpha" ):set_target_lua( self, "alpha_regular" )

	self:add_multiple_render(	{ix,iy,		4,SY} )

	self:add_multiple_size(		{ix+4,iy, 	4,SY}, 2 )
	self:add_rendering_size(	{ix,iy+SY,	4,SY}	)
	self:add_slider(	{ix,iy+SY*3,	4,SY}, "Net_alpha" ):set_min_max( 0, 1 ):set_target_lua( self, "net_alpha" )

end

function meu:get_boids()
	local meus = self.__boids_meus
	if not meus then
		local nb = 12
		meus = {}
		for i=1,nb do
			local m = self:get_meu_by_name( "Boid_Def"..i )
			if m then
				local bu_col = m:get_bu_by_key( "color" )
				local boid =  m:get_boid()
				meus[i] = { mu=m:get_mu(), meu=m, bu_color=bu_col, boid=boid  }
			else
				return
			end
		end
		self.__boids_meus = meus
	end
	return meus
end

function meu:update_boids_color()
	local boids_meus = self:get_boids()
	if not boids_meus then return end

	--todo do a palette object MEU BU ... ?
	local def_color =
	{
		{ 1,0,0 },	{ 0,1,0 },	{ 0,0,1 },	{ 1,1,1 },
		{ 1,1,0 },	{ 0,1,1 },	{ 1,0,1 },	{ .5,.5,.5 },
		{ 1,1,.5 },	{ .5,1,1 },	{ 1,.5,1 },	{ .25,.25,.25 },
	}

	--	local col1 = {	25,		103,	88,		1	}
	--	local col2 = {	166,	165,	111,	1	}
	--	local col3 = {	150,	146,	134,	1	}
	local c1 = {	255,	255,	20,		1	}
	local c2 = {	255,	20,		255,	1	}
	--self:print( "alpha is "..self.alpha_regular )

	local nb = self.boids_nb
	--self:print( "begin" )
	for i = 1,nb do
		local e = boids_meus[i]
		local b = e.boid
		local inter = (i-1)/(nb-1)
		local col
		local id = ((i-1) % 12) + 1
		col = {}
		col[1] = interpolate( c1[1], c2[1], inter ) / 256
		col[2] = interpolate( c1[2], c2[2], inter ) / 256
		col[3] = interpolate( c1[3], c2[3], inter ) / 256
		--self:print( id )
		--col = def_color[ id ]
		e.bu_color:set_rgba_t( col )

		b:set_repulse_net_color_define(true)
		b:set_repulse_net_color( col[1], col[2], col[3], self.net_alpha )
		--b:print( "toto" )

	end
end

function meu:init_boids()
	local boids_meus = self:get_boids()
	if not boids_meus then return end

	local nb = #boids_meus
	for i =1,nb do
		local boid = boids_meus[i].boid
		boid:set_id( i )
		boid:set_deform_active( false )
		boid:set_repulse_by_other_type( 2 )
		boid:set_repulse_by_other_influence( 1 )
		-- depreciated
		--boid:set_box_type( "Bounce_side_wrap_axe" )	
		-- replace it but whiuch axe
		boid:set_box_type_xyz( "bounce", "wrap", "bounce" )
		boid:set_box_sx( 3 )
		boid:set_box_sy( 4 )
	end
end

function meu:update_boids()
	local boids_meus = self:get_boids()
	if not boids_meus then return end

	--self:print( "alpha is "..self.alpha_regular )

	local nb = #boids_meus
	for i =1,nb do
		local e = boids_meus[i]
		local b = i <= self.boids_nb
		e.mu:set_value( b )
		if b then
			local inter = (i-1)/(nb-1)

			--todo can't be done this way
			--e.bu_color:set_alpha( self.alpha_regular )
			local boid = e.boid
			boid:set_nb( self.nb_by_boid )
			boid:set_viscosity( self.viscosity )
	--		boid:set_speed_xyz( 0, self.b_speed and (self.speed*(1+inter/2)) or 0, 0 )
			boid:set_speed_xyz( 0, self.b_speed and self.speed or 0, 0 )
			boid:set_field_active( self.b_field )
			boid:set_field_influence( self.field_influence )
			boid:set_repulse_distance( self.repulse_dist )
			if self.b_restart then
				boid:trig_restart()
			end
			boid:set_repulse_by_other_distance( self.repulse_dist * 2 )

			boid:set_box_sx( 3 )
			boid:set_box_sy( 4 )

			boid:set_flock_active( self.b_flock )
			boid:set_flock_influence( .1 )
			boid:set_flock_distance( self.repulse_dist * self.flock_dist_factor )
		end
	end
end

function meu:update()
	local boids_meus = self:get_boids()
	if not boids_meus then return end

	if self.b_restart then
		self:print( "restart is "..self.b_speed )
		self:init_boids()
	end

	--self:init_boids()
	self:update_boids()
	self:update_boids_color()
end 