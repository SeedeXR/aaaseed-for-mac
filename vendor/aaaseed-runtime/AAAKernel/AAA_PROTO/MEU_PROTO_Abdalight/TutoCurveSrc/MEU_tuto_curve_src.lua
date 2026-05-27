function meu:define_meu_infos( )
	return { author = "Abdalight", date = "2024",
               tags = { "2d", "3D", "Art", "Draw", "Generator", "Geometry", "Procedural" },
               help = "Meu for control the Meu Curve3d" }
end


function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
    local ix,iy = 1,1
    local SY,DY = 1,.2
    local bu

    self:add_rendering()    
    ui.mon = self:add_monitor(  {ix,iy,         16,9},      "Monitor",              114 )
		--ui.mon:set_info_draw( true )
		--ui.mon:set_size_draw( true )
    bu = self:add_slider_xy(    {ix,iy,         16,9},      "Shift to not draw",    self,       "pos_x", "pos_y",       0,0,     -1,1,   -1,1   )
        bu:set_text_visible( false )
		ui.bu_pos = bu


    ix, iy = 1, iy + DY + 9 * SY

    self:begin_bu_group("Drawing Zone")
    bu = self:add_slider(       {ix,iy,         4,SY},      "X size",               self,       "x_size",                16,            0,16    )
        bu:set_value_type_integer( true )
        bu:set_color_back( "x" )
    bu = self:add_slider(       {ix+4,iy,       4,SY},      "X Offset",             self,       "x_off",                  0,            -5,5    )
        bu:set_value_type_integer( true )
        bu:set_color_back( "x" )
    bu = self:add_slider(       {ix+8,iy,       4,SY},      "Y Size",               self,       "y_size",                 9,             0,9    )
        bu:set_value_type_integer( true )
        bu:set_color_back( "y" )
    bu = self:add_slider(       {ix+12,iy,      4,SY},      "Y Offset",             self,       "y_off",                  0,            -5,5    )
        bu:set_value_type_integer( true )
        bu:set_color_back( "y" )
    iy = iy + SY + DY

    bu = self:add_slider(       {ix,iy,         4,SY},      "Z Size",               self,       "z_size",                 1,            0,9     )
        bu:set_value_type_integer( true )
        bu:set_color_back( "z" )
    bu = self:add_slider(       {ix+4,iy,       4,SY},      "Z Offset",             self,       "z_off",                  0,           -5,5     )
        bu:set_value_type_integer( true )
        bu:set_color_back( "z" )
    iy = iy + SY + DY
    self:end_bu_group()
    
    bu = self:add_button(       {ix,iy,         4,SY},      "Canvas",               self,      "b_canvas",          false )
    bu = self:add_button(       {ix+4,iy,       4,SY},      "Screen",               self,      "b_screen",          true )
    iy = iy + SY + DY
    
    self:add_camera(            {ix+8,iy,       8,SY}   )
end


function meu:init()
    local ref     = self.ref
    local bdd     = self:get_layer_bdd( 1 )
    local pgr     = param.get_ref
    self.meu_dst  = self:get_meu_by_name( "TutoCurve3d_1" )

    if bdd then
        ref.bdd   = bdd
        ref.pos_x = pgr( bdd, "emission_origin_x" )
        ref.pos_y = pgr( bdd, "emission_origin_y" )
        ref.pos_z = pgr( bdd, "emission_origin_z" )
    end
    self.pos_z = 0
    self.x_last, self.y_last, self.z_last = 0, 0, 0
end




function meu:distance3D( x1,y1,z1, x2,y2,z2 )
    local dx = x2 - x1
    local dy = y2 - y1
    local dz = z2 - z1
    return math.sqrt( dx * dx + dy * dy + dz * dz )
end

function meu:update()
    local ref = self.ref
    local ui = self.ui
    local meu_dst = self.meu_dst   
    
    local x = self.pos_x * self.x_size + self.x_off
    local y = self.pos_y * self.y_size + self.y_off
    local z = self.pos_z --* self.z_size + self.z_off

    param.set( ref.pos_x, x )
    param.set( ref.pos_y, y )
    param.set( ref.pos_z, z )

    if meu_dst and meu_dst.new_curve and not aaa.keyboard.is_shift() then
        local b_contact = ui.bu_pos:is_contact()
        local b_contact_last = self.b_contact_last
        local x_last, y_last, z_last = self.x_last, self.y_last, self.z_last

        if b_contact  then
            if b_contact ~= b_contact_last then
                meu_dst:next_curve()
                meu_dst:new_curve()
                self.pos_z = self.pos_z + 0.001
            end
            if self:distance3D( x,y,z, x_last, y_last, z_last ) >= .2 then
                meu_dst:new_point_control( x,y,z )
                self.x_last, self.y_last, self.z_last = x,y,z
            end
        end
        
        self.b_contact_last = b_contact
    end

end

function meu:draw()
    self:draw_layers_begin()
        self:draw_layer( 1 )
        self:get_alpha()
        if self.b_screen then 
            self:draw_screen()
        end
        if self.b_canvas then
            self:draw_canvas()
        end
    self:draw_layers_end()
end

function meu:draw_screen()
    local alpha = self:get_alpha()
    gol.color4( 1, 1, 1, alpha )                                              --16:9 white screen defaults settings
    gol.draw_line_strip_3d(	-16,-9,self.pos_z,  16,-9,self.pos_z,   16,9,self.pos_z,    -16,9,self.pos_z,   -16,-9,self.pos_z )
    gol.color4( 1, 0, 0, alpha )                                              --zooming red screen selected by bu
    gol.draw_line_strip_3d(	
        -self.x_size + self.x_off, -self.y_size + self.y_off, self.z_pos,
         self.x_size + self.x_off, -self.y_size + self.y_off, self.z_pos,
         self.x_size + self.x_off,  self.y_size + self.y_off, self.z_pos,
        -self.x_size + self.x_off,  self.y_size + self.y_off, self.z_pos,
        -self.x_size + self.x_off, -self.y_size + self.y_off, self.z_pos  )
end

function meu:draw_canvas()
    local alpha = self:get_alpha()
    gol.color4( 1, 1, 1, alpha )
    gol.draw_quads_2d( -16,-9,  16,-9,  16,9,   -16,9 )
end