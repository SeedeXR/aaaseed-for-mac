
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local sha = self:get_shading()

	local ix, iy = 1,1
	local SY = .8
	local DY = .2

--	self:add_shading_ui( {1,1} )
	self:add_camera()

	local y_begin = 2 + DY

	bu = self:add_button(		{ix,iy,		SY,SY},	"Center", 		self,	"b_center",		true	)
	iy = iy + SY + DY
	bu = self:add_button(		{ix,iy,		SY,SY},	"Lines", 		self,	"b_lines",		true	)
	iy = iy + SY 
	bu = self:add_slider(		{ix,iy,		8,SY},	"Size", 		self,	"size",			1,	0,4		)
	iy = iy + SY + DY

--------------------
	self:set_tab_key( "Main" )
	ix, iy = 1, y_begin



	ix,iy = 9,5
	local SX = 8/3

	-- self:add_blending( {ix,iy} )
	self:add_rendering( {1,14} )

end
function meu:init()
--	local sha = self:add_shading( 2 )
end

function meu:update()
	self.meu_boid = self:get_meu_by_name_no_error( "Boid_1" )
	if not self.boid and self.meu_boid then
		self.boid = self.meu_boid:get_boid()
	end
end

function meu:draw_info()
	gol.color_yellow()
	gol.push_matrix()
		gol.translate( -3.9,1.6 )
		gol.scale(.2)
		local str = "FPS "..aaa.time.get_str_fps().."\n	Boids: "..self.boid:get_nb_current()
		aaa.draw_str_xy( str, 0,0 )
	gol.pop_matrix()
end

function meu:draw()
	self:draw_layers_begin()
		self:draw_layer(1)
		self:draw_chris()

		self:draw_info()
	self:draw_layers_end()
end

--
-- la tu peux bricoler
--
function meu:draw_chris( )
	if self.b_center then
		aaa.draw_null( 0,0,0, 1 )
	end

	if self.b_lines then
		gol.begin_lines()
			gol.vertex( 0,0 )
			local s = self.size
			gol.vertex( s,s )
		gol.do_end()
	end

end
