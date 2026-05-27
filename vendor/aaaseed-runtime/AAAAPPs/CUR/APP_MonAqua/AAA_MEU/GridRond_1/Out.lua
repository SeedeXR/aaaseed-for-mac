function meu:define_ui()
    local ix, iy = 2, 2

    self:add_button( {ix, iy }, "Grid 1", self, "b_draw_grid_1", false )
    iy = iy + 1
    self:add_button( {ix, iy }, "Grid 2", self, "b_draw_grid_2", false )
    iy = iy + 1
	self:add_button( {ix, iy }, "Grid Overlap", self, "b_draw_grid_overlap", false )
    iy = iy + 1
	self:add_button( {ix, iy }, "Cross", self, "b_draw_cross", false )
    iy = iy + 1
	self:add_button( {ix, iy }, "Circle", self, "b_draw_circle", false )
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

    if self.b_draw_grid_overlap then
		self:draw_layer(6)
        self:draw_layer(7)
    end
    if self.b_draw_cross then
        self:draw_layer(9)
    end
    if self.b_draw_circle then
		self:draw_layer(10)
    end
    self:draw_layers_end()
end
