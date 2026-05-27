function meu:define_ui()
    local ix, iy = 2, 2
	local bu
	local SY = 1
    self:add_button( {ix, iy}, "Grid 1",		self,"b_draw_grid_1",		false )
    iy = iy + SY
    self:add_button( {ix, iy}, "Grid 2",		self,"b_draw_grid_2",		false )
    iy = iy + SY
   	self:add_button( {ix, iy}, "Grid 3",		self,"b_draw_grid_3",		false )
    iy = iy + SY
	self:add_button( {ix, iy}, "Grid Overlap",	self,"b_draw_grid_overlap", false )
    iy = iy + SY
	self:add_button( {ix, iy}, "Cross",			self,"b_draw_cross",		false )
    iy = iy + SY
	self:add_button( {ix, iy}, "Circle",		self,"b_draw_circle",		false )
	iy = iy + SY * 2

	bu = self:add_button(	{ix,	iy,		SY,	SY },	"Circles", 		self, "b_cercles",	false )
	bu = self:add_slider(	{ix+4,	iy,		4,	SY},	"Circles Nb U",	self, "circles_nb_u", 5, 2,16 ):set_value_type_integer(true)
	bu = self:add_slider(	{ix+4,	iy+SY,	4,	SY},	"Circles Nb V",	self, "circles_nb_v", 5, 2,16 ):set_value_type_integer(true)
end

function meu:init()
    local ref = self.ref

end

function meu:draw()
    self:draw_layers_begin()
		self:draw_layer(1)
    if self.b_draw_grid_1 then
        self:draw_layer(3)
    end
    if self.b_draw_grid_2 then
		self:draw_layer(4)
    end
	 if self.b_draw_grid_3 then
		self:draw_layer(5)
    end

    if self.b_draw_grid_overlap then
		self:draw_layer(7)
        self:draw_layer(8)
    end
    if self.b_draw_cross then
        self:draw_layer(10)
    end
    if self.b_draw_circle then
		self:draw_layer(11)
    end

	--aaa.draw_circle_axe_z( 0, 0, 0, 2, 24 )
	if self.b_cercles then
		--local nb_pt = nb/2
		local nb_u = self.circles_nb_u
		local nb_v = self.circles_nb_v
		local su = 2/nb_u
		local sv = 2/nb_v
		for i = -13,10,5 do
			local x = i
			for j = -1.5,1.5,3 do
				local z = j
				aaa.draw_circle_axe_z( x, z,		0, 1, 24 )
				aaa.draw_circle_axe_z( x, z+.25,	0, 1, 24 )
				aaa.draw_circle_axe_z( x, z,		0, 3, 24 )
				aaa.draw_circle_axe_z( x, z,		0, 5, 24 )
				--aaa.draw_line
			end
		end
	end

    self:draw_layers_end()
end
