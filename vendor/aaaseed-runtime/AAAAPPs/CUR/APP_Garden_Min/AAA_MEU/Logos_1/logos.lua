
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local ix = 1
	local iy = 1
	local SY = 1
	local DY = .2

	self:add_camera()

	iy = 1
	self:add_button(	{ix,iy,		4,1},	"Construction", self, "b_build", true	)
	iy = iy + SY

	
	bu = self:add_slider({ix,iy,	8,SY},	"Nb",	self, "a_nb",		3,	0,64			)
		:set_value_type_integer(true)
	iy = iy + SY + DY

	bu = self:add_slider({ix,iy,	8,SY},	"Radius",	self, "radius",	.5,	0,1			)
	iy = iy + SY

	bu = self:add_slider({ix,iy,	8,SY},	"Top",		self, "top",	.5,	0,1			)
	iy = iy + SY

	bu = self:add_slider({ix,iy,	8,SY},	"Middle",	self, "middle",	.5,	0,1			)
	iy = iy + SY + DY

	bu = self:add_slider({ix,iy,	8,SY},	"angle",	self, "angle",	30,	0,120			)
	iy = iy + SY + DY

	bu = self:add_slider({ix,iy,	8,SY},	"Rotation",	self, "rot",	0,	-180,180			)
	iy = iy + SY
	bu = self:add_slider({ix,iy,	4,SY},	"Tra U",	self, "tra_u",	0,	-1,1			)
	bu = self:add_slider({ix+4,iy,	4,SY},	"Tra V",	self, "tra_v",	0,	-1,1			)
	iy = iy + SY
--	self:add_button(		{ix,iy+SY,	4,1},	"boid", 		self, "b_boid", true	)
--	iy = iy + SY*2
end



function meu:draw()
	local alpha = self:get_alpha() 
	local arc_nb = 24

	local function sin(a) return math.sin( a * math.pi2/360 ) end
	local function cos(a) return math.cos( a * math.pi2/360 ) end
	gol.set_texture_dim( 0 )
	gol.color_white( self:get_alpha() )

	self:draw_layers_begin()

		local r = self.radius
		local rt = r * self.top
		local rm = interpolate( r,rt, self.middle )
		local da = self.angle * .5

		local d = r * 2
		--self:draw_layer( 1 )
		if self.b_build then
			gol.color_cyan( alpha )

			aaa.draw_circle_axe_z( 0,0,0, r*2, arc_nb )
			aaa.draw_circle_axe_z( 0,0,0, rt*2, arc_nb )
			aaa.draw_circle_axe_z( 0,0,0, rm*2, arc_nb )
		end

		local xt,yt = rt, 0
		local xb,yb = r, 0
		local xc,yc = interpolate( xb,xt, .5 ), interpolate( yb,yt, .5 )
		local xbp,ybp = r*cos(-da), r*sin(-da)
		local xbn,ybn = r*cos(da), r*sin(da)
		local xmp,ymp = interpolate( xbp,xt, self.middle ), interpolate( ybp,yt, self.middle )
		local xmn,ymn = interpolate( xbn,xt, self.middle ), interpolate( ybn,yt, self.middle )

		local function draw_a( a )
			a = a + 90

--			local rot = (a+self.rot) * math.pi2/360
--			local ox,oy = self.tra_u*math.cos(rot) - self.tra_v*math.sin(rot), -self.tra_u*math.sin(rot) + self.tra_v*math.cos(rot)

			gol.push_matrix()
				gol.rotate_z( a / 360 )
				gol.translate( xc, yc )
				gol.rotate_z( self.rot / 360 )
				gol.translate( -xc, -yc )

				gol.translate( self.tra_v, self.tra_u )

				gol.set_line_width(4)
				aaa.draw_line( xt,yt, xbp,ybp )
				aaa.draw_line( xt,yt, xbn,ybn )

				gol.set_line_width(4)
				aaa.draw_line( xmp,ymp, xmn,ymn )

			gol.pop_matrix()
		end

		gol.set_line_width(4)
		gol.color_white( alpha )
		local a_nb = self.a_nb
		for a=0,360,360/a_nb do
			draw_a(a + 60)
		end

	self:draw_layers_end()
end

