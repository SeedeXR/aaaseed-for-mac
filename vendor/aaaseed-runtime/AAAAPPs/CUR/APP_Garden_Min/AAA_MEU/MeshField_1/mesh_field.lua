
function meu:init()
	local ref = self.ref


	self.bdd_model = self:get_layer_bdd(1)
	self.bdd_model_pts = aaa.bdd.get_points( self.bdd_model )

	self.b_draw_model     			= 1
	self.b_draw_model_pts_as_cubes	= 0

	self.b_model_loaded				= 0

	self.scale = 10
	self.size = .01

	self.b_draw_tex3d = 0
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	--self:add_rgbf(	{1,4.5 } )

	--self:add_bu_texture_target_unit()

	self:add_camera()

	local ix = 1
	local iy = 3
	local DY = .9

	--bu = self:add_button( {ix, iy }, "Model", 		self, "b_draw_model" )
	--iy = iy + DY
	bu = self:add_button( {ix, iy }, "Model_points", 	self, "b_draw_model_pts_as_cubes" )
	iy = iy + DY
	bu = self:add_slider(	{ix,iy,	4,1},	"Scale" , self, "scale", 1, 0, 10 )
	iy = iy + DY
	bu = self:add_slider(	{ix,iy,	4,1},	"Size" , self, "size", .01, 0, 1 )

	ix, iy = 6, 6
	bu = self:add_button( {ix, iy }, "Tex3D", 		self, "b_draw_tex3d" )
	iy = iy + DY

end

function meu:draw_cubes_at_pts()
	local tab = self.bdd_model_pts
	local scale = self.scale
	--self:print( "tab size : "..#tab )
	for i in IPAIRS( tab ) do
		local pt = tab[i]
		aaa.draw_cube( pt[1] * scale, pt[2] * scale, pt[3] * scale, self.size )
	end
end

---[[
function meu:draw()
	self:draw_layers_begin()
		if self.b_draw_model == 1 then
			--self:print( "b_draw_model : "..self.b_draw_model )
			self:draw_layer( 1 )
		end
		if self.b_draw_tex3d == 1 then
			self:draw_layer( 2 )
		end
		if self.b_draw_model_pts_as_cubes == 1 then
			self:draw_cubes_at_pts()
		end
	self:draw_layers_end()
end
---]]