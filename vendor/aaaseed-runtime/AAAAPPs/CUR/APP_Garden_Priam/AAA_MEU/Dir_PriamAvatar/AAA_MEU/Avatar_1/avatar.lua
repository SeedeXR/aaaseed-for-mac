function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
	local par

	local ix,iy = 1,1
	local SY,DY = 1,.2

	self:add_camera()
	self:add_rendering()

	self:add_button(			{ix,iy,		4,SY}, 	"Field Draw",	self, "b_draw_field",	false )
	iy = iy + SY
	bu = self:add_button(		{ix,iy,		3,SY},	"Field Active", ref.field_active )
	:set_text( "Field" )
	bu = self:add_slider(		{ix+3,iy,	5,SY},	"Influence",	ref.field_influence, nil,	0, -1,1 )
	iy = iy + SY + DY

	self:add_bu_texture_target_unit( {ix,iy,	8,6}, 	"Field", 1 )
	iy = iy + 6 + DY

	bu = self:add_trig(			{ix,iy,		4,SY},	"Restart",		ref.restart_trig )
	bu = self:add_trig_method(	{ix+4,iy,	4,SY},	"Focus", 		self, "set_focus_boid" )
	iy = iy + SY
	bu = self:add_slider(		{ix,iy,		8,SY},	"Viscosity",	ref.viscosity, nil,		0, 0,.1 )
	iy = iy + SY
	local t = { "repulse" }	--, "flocking", "steering" }
	for i,name in ipairs(t) do
		bu = self:add_button(	{ix,iy,		3,SY},	name,				ref.boid, name.."_active",		false )
		bu = self:add_slider(	{ix+3,iy,	5,SY},	name.."_Influence", ref.boid, name.."_influence",	0, 0,1 )
			:set_text( "Influence" )
		iy = iy + SY
		bu = self:add_slider(	{ix+2,iy,	6,SY},	name.."_Distance", ref.boid, name.."_distance",	0, 0,1 )
			:set_text( "Distance" )
		iy = iy + SY + DY
	end
--	self:add_nb_uv( {1, 12.5}, self:get_layer_bdd(1) )


	ix,iy = 9,2 + DY
	for i,bum in ipairs(self.bumpers) do 
		bu = self:add_button(		{ix,iy,		3,SY},	"Bumper "..i,	bum, "b_active",		false )
		bu = self:add_trig_method(	{ix+4,iy,	4,SY},	"Reset "..i,	self, "reset_bumper", i	)
		iy = iy + SY
		local sx,sy = app:get_capture_suv_meter()
		sx,sy = sx/2,sy/2
		self:add_slider(		{ix,iy,		4,SY},	"Bumper X "..i, 	bum, "x",	0, -sx,sx )
			:set_text("x"):set_color_back()
		self:add_slider(		{ix+4,iy,	4,SY},	"Bumper Y "..i, 	bum, "y",	0, -sy,sy )
			:set_text("y"):set_color_back()
			iy = iy + SY
		self:add_slider_two(	{ix,iy,		8,SY},	"Bumper Radius "..i, bum,"r_int","r_ext",	.5,1, 0,4 )
			:set_draw_step( "linear_inverse" )
		iy = iy + SY + DY
	end

	local net = {}
	self.network = net
	bu = self:add_button(	{ix,iy,		3,SY},	"Network",	net, "b_active",		false )
	iy = iy + SY
	self:add_slider_two(	{ix,iy,		8,SY},	"Network Radius", net,"r_int","r_ext",	1,2, 0,4 )
		:set_draw_step( "linear_inverse" )
    
    local avatar_table = {}
    -- Important that these are accessible from other MEUs
    app.avatars = avatar_table
    for i = 1,param.get(ref.boid, "nb_alive_max")
    do
        local features = {}
        features.occupied = false
        avatar_table[i] = features
    end
end

function meu:set_focus_boid()
	aaa.obj.set_focus_ui( self.ref.boid )
end
function meu:reset_bumper( id )
	self.bumpers[id].count = 0
end

function meu:init()
	local ref = self.ref
	ref.boid = self:get_layer_bdd(3)

	local lay_boid	=	self:get_layer( 3 )

	ref.boid = self:get_layer_bdd(3)
		ref.restart_trig	=	param.get_ref( ref.boid, "restart_trig" )
		ref.field_active	=	param.get_ref( ref.boid, "field_active" )
		ref.field_influence	=	param.get_ref( ref.boid, "field_influence" )
		ref.viscosity		=	param.get_ref( ref.boid, "viscosity" )

	ref.def_image = aaa.obj.get_down_by_class( lay_boid, "def_image" )
		ref.def_image_src = param.get_ref( ref.def_image, "Image_src" )

	self.bumpers =
	{
		{ free_time={}, count=0, ph=0, col={0,1,0,1} },
		{ free_time={}, count=0, ph=0, col={1,0,0,1} }
	}
    self.box_sx, self.box_sy = param.get(ref.boid, "box_size_x"), param.get(ref.boid, "box_size_y")
--   self.tex_sx, self.tex_sy = self:get_texture_size(1)	--todo strange : not defined yet or dont need it from the ui
end

function meu:restart()
	param.set( self.ref.boid, "restart_trig", 1  )
end

function meu:draw_bumper( bum )
	if not bum.b_active then return end

	bum.col[4] = .75
	gol.color( bum.col )
	local r = interpolate( bum.r_int, bum.r_ext, bum.ph )
	bum.ph = clamp_01( bum.ph - aaa.time.dt * 3. ) 
	aaa.draw_disk_axe_z( bum.x,bum.y,0, r*2,48 )
	bum.col[4] = .5
	gol.color( bum.col )
	aaa.draw_circle_axe_z( bum.x,bum.y,0, bum.r_ext*2,48 )
	gol.color_black(1)
	local st = .4
	aaa.draw_str_xyz( bum.count, bum.x-st*.5,bum.y-st*.4,0, st,st )
end

function meu:update_bumper( bum, id, x,y )
	if not bum.b_active then return end

	local dx = bum.x-x
	local dy = bum.y-y
	local d2 = dx*dx + dy*dy
	local ft = (bum.free_time[id] or 0)
	if d2 <= bum.r_ext*bum.r_ext then
		aaa.draw_line( x,y, bum.x,bum.y )
		if ft > .25 then
			ft = 0
			aaa.bell()
			bum.count = bum.count + 1
			bum.ph = 1
		end
	else
		ft = ft + aaa.time.dt
	end
	bum.free_time[id] = ft
end

function meu:draw_avatars()
	local ref = self.ref
	local all = aaa.bdd.get_points_with_id( ref.boid )
	local net = self.network
	net = net.b_active and net or nil
    local bind = self:get_texture_bind_2d(1)
    -- I think we need to update this here to ensure that we have the correct size if the input texture changes size while running
    self.tex_sx, self.tex_sy = aaa.img.get_size(bind)
	for i,t in IPAIRS(all) do
		local s = 1.2
		local x,y,z = t[1],t[2],t[3]
		--self:print( i.." - "..x..","..y )
		--aaa.draw_rect( x-s,y-s, x+s,y+s )
		-- draw avatar a circle
		gol.color_white()
		aaa.draw_circle_axe_z( x,y,z, s,24  )
        
		-- draw avatar number
		--aaa.draw_str( x,y,z, s,24  )
		local st = .4
		local dum,id = poid.split_id( t[4] )
		aaa.draw_str_xyz( id, x-st*.3,y-st*.4,z, st,st )
        
        -- update occupied states of avatars table
        local x_t, y_t = self:boids_xy_to_capture_xy(x, y)
        local r,g,b,a = aaa.img.get_color_xy(bind, x_t, y_t)
        if a > 0 then
            if app.avatars[id].occupied == false then
                aaa.print("User entered avatar "..id)
                app.avatars[id].occupied = true
            end
        else
            if app.avatars[id].occupied == true then
                aaa.print("User left avatar "..id)
                app.avatars[id].occupied = false
            end
        end
		-- update bumper contacts
		for i,bum in ipairs(self.bumpers) do
			self:update_bumper( bum, id, x,y )
		end

		-- draw relation net between avatar
		if net then
			for j=i+1,#all do
				local tb = all[j]
				local xb,yb,zb = tb[1],tb[2],tb[3]
				local dx = xb-x
				local dy = yb-y
				local d2 = dx*dx + dy*dy
				if d2 <= net.r_ext*net.r_ext then
					d2 = math.sqrt(d2)
					d2 = (d2-net.r_ext) / (net.r_int-net.r_ext)
					gol.color_cyan( d2 )
					aaa.draw_line( x,y, xb,yb )
				end
			end
		end
	end
	
	-- draw_bumpers 
	for i,bum in ipairs(self.bumpers) do
		self:draw_bumper( bum )
	end
end

function meu:draw()
	local ref = self.ref
	self:draw_layers_begin()

		self:draw_layer(1)

		if self.b_draw_field then
			self:draw_layer(2)
		end

		local bind = self:get_texture_bind_2d(1)
		param.set( ref.def_image_src, bind )
		self:draw_layer(3)

	--aaa.draw_rect( -1,-1, 1,1 )
		self:draw_avatars()

	self:draw_layers_end()
end

function meu:boids_xy_to_capture_xy(x, y)
    -- Transform from the box coordinates of the boids to the size of the capture input
    local x_mod, y_mod = self.tex_sx/self.box_sx, self.tex_sy/self.box_sy
    return (x + self.box_sx/2)*x_mod, (y + self.box_sy/2)*y_mod
end