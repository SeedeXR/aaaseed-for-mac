
function meu:init()
	local ref = self.ref

	self.b_draw_cubes     = 0
	self.b_draw_spheres   = 0
	self.b_draw_bunny     = 0
	self.b_draw_armadillo = 0
	self.b_draw_dragon    = 0
end
---[[
function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 ) -- draw elements bug if commented ?!
		self:draw_layer( 2 )
		self:draw_layer( 3 )
		self:draw_layer( 4 )
		self:draw_layer( 5 )
		if self.b_draw_cubes == 1 then
			self:draw_layer(7)
			self:draw_layer(8)
		end
		if self.b_draw_spheres == 1 then
			self:draw_layer(10)
			self:draw_layer(11)
		end
		if self.b_draw_bunny == 1 then
			self:draw_layer(13)
		end
		if self.b_draw_armadillo == 1 then
			self:draw_layer(14)
		end
		if self.b_draw_dragon == 1 then
			self:draw_layer(15)
		end
		self:draw_layer( 17 )
	self:draw_layers_end()
end
---]]

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgbf(	{1,4.5 } )

	self:add_bu_texture_target_unit()

	self:add_camera()

	local ix = 1
	local iy = 6
	local DY = .9

	bu = self:add_button( {ix, iy }, "Cubes", 		self, "b_draw_cubes" )
	iy = iy + DY
	bu = self:add_button( {ix, iy }, "Spheres", 	self, "b_draw_spheres" )
	iy = iy + DY
	bu = self:add_button( {ix, iy }, "Bunny", 		self, "b_draw_bunny" )
	iy = iy + DY
	bu = self:add_button( {ix, iy }, "Armadillo", 	self, "b_draw_armadillo" )
	iy = iy + DY
	bu = self:add_button( {ix, iy }, "Dragon", 	self, "b_draw_dragon" )
	iy = iy + DY

	-- TODO
	-- different scene positions with transfo
	-- visualize light position
	-- object contour with stencil ( OGC )
	--
	-- tab for light mode :
	-- oldschool, new school to implement
	-- -> do some set params for :
	-- light switch, light color, diffuse, spec, amb, spot, dot, sun, dir...
	-- rendering : culling, depth test ...
	--
	-- and lot to come : light / shadow maps, raytrace, raymarch...  ( OGC )
	-- ... shader params ...

end

