
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
	-- self:add_button(	{ix,iy,		4,1},	"Construction", self, "b_build", true	)
	-- iy = iy + SY

	
	-- bu = self:add_slider({ix,iy,	8,SY},	"Nb",	self, "a_nb",		3,	0,64			)
	-- 	:set_value_type_integer(true)
	-- iy = iy + SY + DY

	bu = self:add_slider({ix,iy,	8,SY},	"Angle",	self, "angle",	0,	0,90			)
	iy = iy + SY

	-- bu = self:add_slider({ix,iy,	8,SY},	"Top",		self, "top",	.5,	0,1			)
	-- iy = iy + SY

	-- bu = self:add_slider({ix,iy,	8,SY},	"Middle",	self, "middle",	.5,	0,1			)
	-- iy = iy + SY + DY

	-- bu = self:add_slider({ix,iy,	8,SY},	"angle",	self, "angle",	30,	0,120			)
	-- iy = iy + SY + DY

	-- bu = self:add_slider({ix,iy,	8,SY},	"Rotation",	self, "rot",	0,	-180,180			)
	-- iy = iy + SY
	-- bu = self:add_slider({ix,iy,	4,SY},	"Tra U",	self, "tra_u",	0,	-1,1			)
	-- bu = self:add_slider({ix+4,iy,	4,SY},	"Tra V",	self, "tra_v",	0,	-1,1			)
	-- iy = iy + SY

end



function meu:draw()

	self:draw_layers_begin()
		self:draw_layer(1)
		self:draw_layer(2)
		self:draw_layer(3)
		--self:draw_layer(4)
	gol.push_matrix()
		gol.rotate_x( 1/2-self.angle/90*.25 )
		gol.translate( 0,0, -2 )
		gol.translate( 0,0, -.2)

		local nb = 4
		local io = (nb+1)/2
		local d =2
		for i=1,nb do
			gol.push_matrix()
				gol.translate( (i-io)*d )
				self:draw_layer(5)
			gol.pop_matrix()
		end
	gol.pop_matrix()
	self:draw_layers_end()
end

