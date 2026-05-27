function meu:define_ui()
    local ix, iy = 2, 2
	local bu
	local SY = 1
    self:add_button( {ix, iy }, "Grid 1", self, "b_draw_grid_1", false )
    iy = iy + SY
    self:add_button( {ix, iy }, "Grid 2", self, "b_draw_grid_2", false )
    iy = iy + SY
   	self:add_button( {ix, iy }, "Grid 3", self, "b_draw_grid_3", false )
    iy = iy + SY
	-- self:add_button( {ix, iy }, "Grid Overlap", self, "b_draw_grid_overlap", false )
    -- iy = iy + SY
	-- self:add_button( {ix, iy }, "Cross", self, "b_draw_cross", false )
    -- iy = iy + SY
	-- self:add_button( {ix, iy }, "Circle", self, "b_draw_circle", false )
	-- iy = iy + SY * 2
  	self:add_button( {ix, iy }, "Auto", self, "b_auto", false )
    iy = iy + SY

	self.bu_color = self:add_rgbfa(	{ix,iy,	8,2}, 	"Color", aaa.obj.get_down_by_class( self:get_layer(3), "color" ) )
	iy = iy + SY

--	bu = self:add_button(	{ix,		iy,		SY, SY },	"Circles", 			self, "b_cercles", false )
--	bu = self:add_slider(	{ix+4,iy,	4,SY},	"Circles Nb U",		self, "circles_nb_u", 5, 2, 16 ):set_value_type_integer(true)
--	bu = self:add_slider(	{ix+4,iy+SY,	4,SY},	"Circles Nb V",		self, "circles_nb_v", 5, 2, 16 ):set_value_type_integer(true)

end

function meu:init()
    local ref = self.ref
end

MONACO_AQUA.fp_color_by_scene =
		{
			DJ1		= {	240/255,	240/255,	250/255,	1.	},
			PIJ1	= {	200/255,	240/255,	250/255,	.8	},
			DJ2		= {	200/255,	240/255,	250/255,	1.0	},
			PIJ2	= {	200/255,	240/255,	250/255,	.8	},
			DJ3		= {	60/255,		130/255,	200/255,	1.0	},
			PIJ3	= {	60/255,		130/255,	200/255,	.8	},
			DJ4		= {	40/255,		200/255,	120/255,	.5	},
			PIN1	= {	40/255,		200/255,	120/255,	.5	},
			DN1		= {	255/255,	205/255,	255/255,	1.0	},
			PIN2	= {	255/255,	205/255,	255/255,	.4	},
			DN2		= {	0/255,		200/255,	100/255,	.5	},
			PIN3	= {	0/255,		70/255,		255/255,	.8	},
			DN3		= {	0/255,		70/255,		255/255,	.6	},
		}

function meu:draw()
	if self.b_auto then
		local name = app:get_scene()
		if name then
			local c= app.fp_color_by_scene[name]
			if c then
				self.bu_color:set_rgba( c[1], c[2], c[3], c[4] )
			end
		end
	end
    self:draw_layers_begin()
		self:draw_layer(1)
   		if self.b_draw_grid_1 then  self:draw_layer(3)	end
    	if self.b_draw_grid_2 then	self:draw_layer(4)	end
		if self.b_draw_grid_3 then	self:draw_layer(5)	end


		-- if self.b_draw_grid_overlap then
		-- 	self:draw_layer(7)
		--     self:draw_layer(8)
		-- end
		-- if self.b_draw_cross then
		--     self:draw_layer(10)
		-- end
		-- if self.b_draw_circle then
		-- 	self:draw_layer(11)
		-- end

		-- --aaa.draw_circle_axe_z( 0, 0, 0, 2, 24 )
		-- if self.b_cercles then
		-- 	--local nb_pt = nb/2
		-- 	local nb_u = self.circles_nb_u
		-- 	local nb_v = self.circles_nb_v
		-- 	local su = 2/nb_u
		-- 	local sv = 2/nb_v
		-- 	for i = -13,10,5 do
		-- 		local x = i
		-- 		for j = -1.5,1.5,3 do
		-- 			local z = j
		-- 			aaa.draw_circle_axe_z( x, z,		0, 1, 24 )
		-- 			aaa.draw_circle_axe_z( x, z+.25,	0, 1, 24 )
		-- 			aaa.draw_circle_axe_z( x, z,		0, 3, 24 )
		-- 			aaa.draw_circle_axe_z( x, z,		0, 5, 24 )
		-- 			--aaa.draw_line
		-- 		end
		-- 	end
		-- end

    self:draw_layers_end()
end
