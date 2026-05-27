function meu:define_meu_infos()
	return { author = "Abdalight", date="2024",
            tags = { "3D", "Art", "Draw", "Generator" ,"Geometry", "Procedural", "VJ" },
            help = " Omae Wa Mou Shindeiru " }
end

function meu:define_ui()
	local ref = self.ref
	local ui  = self.ui
    local ix,iy = 1,1
    local SY,DY = 1,.01
    local bu

    self:add_rendering()
    self:add_camera()

    bu = self:add_trig_method(      {ix+1,iy,             2,SY},  "Reset",            self, "reset"  )
        bu:set_text_visible( true )
        bu:set_color_back( "reset" )

	self:set_tab_key( "Main" )

    iy = iy + SY

    bu = self:add_button(           {ix,iy,               3,SY},  "Grid Geometry",     nil,  nil,               0       )
 		bu:set_menu{ "Hringles", "Cylinder Y" , "Cylinder X" , "Sphere" ,"Grid"   }:set_nb_min_0( 1, 5 )
        bu:set_target_lua( self, "selec_deformation" )
        bu:set_text_selector( true )
    bu = self:add_button(           {ix+3,iy,           2.5,SY},  "mesh",             self, "selec_mesh",       1       )
        bu:set_multiple( { "Triangle" , "Square" }, "mesh" )
    bu = self:add_slider(           {ix+5.5,iy,         2.5,SY},  "Step Size",        self, "step_size",        1,      0,5     )
        bu:add_values_def( .1,.5,1,2,2.5,3,4 )
    iy = iy + SY + DY

    bu = self:add_slider(           {ix,iy,               3,SY},  "Selection Radius", self, "selec_radius",     2,      0,50    )
    bu:add_values_def( .5,1,2,3,5,10,20,30,40 )
    bu = self:add_slider(           {ix+3,iy,           2.5,SY},  "Nb U",             self, "nb_u",             5,      1,100   )
        bu:set_color_back( "u" )
        bu:set_value_type_integer( true )
        bu:add_values_def( 5,10,20,30,40,50,60,70,80,90 )
    bu = self:add_slider(           {ix+5.5,iy,         2.5,SY},  "Nb V",             self, "nb_v",             5,      1,100   )
        bu:set_color_back( "v" )
        bu:set_value_type_integer( true )
        bu:add_values_def( 5,10,20,30,40,50,60,70,80,90 )
    iy = iy + SY + DY

    bu = self:add_slider(           {ix,iy,               4,SY},  "Sphere Radius",    self, "radius",           2,      0.1,10    )
    bu = self:add_button(           {ix+4,iy,             4,SY},  "Sphere Offset",    self, "b_sphere_off",     false   )
    iy = iy + SY + DY

    bu = self:add_button(           {ix,iy,               4,SY},  "Implicit Texture", self, "b_tex_implicit",   false   )
        bu:set_target_param( param.get_ref( self:get_layer_mapping(1), "tex_implicit" ) )
    bu = self:add_slider(           {ix+4,iy,             4,SY},  "Factor",           self, "factor",           1,      0.001,1     )
    iy = iy + SY

    bu = self:add_bu_texture_target_unit(    {ix,iy,       8,5},   "Tex", 1, true )
    iy = iy + 5*SY + DY

    bu = self:add_button(           {ix,iy,             8/3,SY},  "Displace ON/OFF",  self, "b_displace",       false   )
    bu = self:add_slider(           {ix+8/3,iy,         8/3,SY},  "Factor D",         self, "displace_factor",  1,      0,1 )
    bu = self:add_trig_method(      {ix+16/3,iy,        8/3,SY},  "Reset D",          self, "reset_displace"    )
        bu:set_text_visible( true )
        bu:set_color_back( "reset" )
    iy = iy + SY + DY

    bu = self:add_slider(           {ix,iy,             8/3,SY},  "Filter D",         self, "displace_filter",      0,      0,10    )
    bu = self:add_button(           {ix+8/3,iy,         8/3,SY},  "No Center D",      self, "b_displace_no_center", false   )
        bu:set_text_draw( true )
    bu = self:add_button(           {ix+16/3,iy,        8/3,SY},  "Color D",          self, "b_displace_color",     false   )
    iy = iy + SY + DY

    bu = self:add_button(           {ix,iy,               4,SY},  "Dif D",            self, "b_displace_dif",       false   )
    bu = self:add_slider(           {ix+4,iy,             4,SY},  "Dif Filter",       self, "dif_filter",           0.1,    0,1     )
    iy = iy + SY + DY
    


    ix, iy = ix + 8, 1 + SY
    
    bu = self:add_button(           {ix,iy,               4,SY},   "Figure Geometry",   nil, nil,     0 )
        bu:set_menu{ "Polygon" , "Stars" , "Lucky Clover" , "Butterfly" , "Snail" }:set_nb_min_0( 1, 5 )
        bu:set_target_lua( self, "selec_figure" )
        bu:set_text_selector( true )
    bu = self:add_slider(           {ix+4,iy,             4,SY},   "Sides Nb",              self,   "side_nb",          6,    3,20  )
        bu:set_value_type_integer( true )
    iy = iy + SY

    bu = self:add_slider(           {ix,iy,               4,SY},    "Radius Size",          self,   "draw_size",        1,    0,1   )
        bu:add_values_def( .1,.2,.3,.4,.5,.8,.9 )
    bu = self:add_slider(           {ix+4,iy,             4,SY},    "Stars Radius Int",     self,   "stars_radius_int", 0.5,    0,2   )
        bu:add_values_def( .2,.3,.5,1,1.5,1.8,1.9 )
    iy = iy + SY

    bu = self:add_slider(           {ix,iy,             8/3,SY},    "Angle Fan",            self,   "angle_fan",        1,    0,1   )
        bu:add_values_def( .1,.2,.3,.4,.5,.6,.7,.8,.9 )
    bu = self:add_slider(           {ix+8/3,iy,         8/3,SY},    "Center Radius",        self,   "center_radius",    0,    0,1   )
        bu:add_values_def( .1,.2,.3,.4,.5,.6,.7,.8,.9 )
    bu = self:add_slider(           {ix+16/3,iy,        8/3,SY},    "Center Displace",      self,   "center_off",       0,   -1,1   )
        bu:add_values_def( -.5 , -.25 , 0 , .25 , .5 )
        bu:set_color_back( "w" )
    iy = iy + SY  + DY

	bu = self:add_slider_multi_curve({ix,iy+1,          8,7*SY},    "Extra Point",      3 )
		ui.bu_multi = bu
        self:add_trig_method(	    {ix+2,iy,		       1,1},    "+",    bu, "add_elt_ui", 1  ):set_text_inside( true )
        self:add_trig_method(	    {ix+3,iy,	           1,1},    "-",    bu, "add_elt_ui", -1 ):set_text_inside( true )
    bu = self:add_button(	        {ix,iy,               2,SY},    "Curve",         self,  "b_curve",        false      )
    bu = self:add_button(	        {ix+6,iy,             2,SY},    "Fan",           self,  "b_triangle",     true       )
    iy = iy + 8*SY + DY

    bu = self:add_rgbf(	            {ix,iy,               8,SY} )
    ui.color  = bu


    self:set_tab_key( "TRS" )
    
    ix, iy = 1, 2

    bu = self:add_transfo(          {ix,iy,          4,2.4*SY},  1 )
    iy = iy + 2.4 * SY  + DY

    bu = self:add_button(           {ix,iy,              2,SY},     "Move",         self,   "b_move",       false   )
    bu = self:add_button(           {ix+2,iy,            6,SY},     "move_type",    self,   "selec_move",   1       )
        bu:set_multiple(            { "Rotate",  "Translate" },     "move_type" )
    iy = iy + SY

    bu = self:add_slider(           {ix, iy,           8/3,SY},     "Move x",       self,   "move_x",       0,      0,1   )
        bu:add_values_def( .1,.2,.3,.4,.5,.6,.7,.8,.9 )
        bu:set_color_back( "x" )
    bu = self:add_slider(           {ix+8/3,iy,        8/3,SY},     "Move y",       self,   "move_y",       0,      0,1   )
        bu:add_values_def( .1,.2,.3,.4,.5,.6,.7,.8,.9 )
        bu:set_color_back( "y" )
    bu = self:add_slider(           {ix+16/3,iy,       8/3,SY},     "Move z",       self,   "move_z",       0,      0,1   )
        bu:add_values_def( .1,.2,.3,.4,.5,.6,.7,.8,.9 )
        bu:set_color_back( "z" )
    iy = iy + SY

    bu = self:add_slider(           {ix, iy,           8/3,SY},     "Spin X",       self,   "spin_x",       0,      0,20  )
        bu:set_color_back( "x" )
    bu = self:add_slider(           {ix+8/3,iy,        8/3,SY},     "Spin Y",       self,   "spin_y",       0,      0,20  )
        bu:set_color_back( "y" )
    bu = self:add_slider(           {ix+16/3,iy,       8/3,SY},     "Spin Z",       self,   "spin_z",       0,      0,20  )
        bu:set_color_back( "z" )
    iy = iy + SY
end

function meu:draw_icon()
	gol.set_line_width( 2 )
    self:draw_icon_texture()
    gol.set_texture_dim( 0 )
    gol.push_translate_scale_2d( -.35, 0, .3, 1.2 )
        aaa.draw_circle_axe_z( 0, 0, 0, 1.2, 6 )
    gol.pop_matrix()
end

function meu:get_preset_nb()    return 24   end

function meu:__reset_common()
    self.phase = { 0, 0, 0 }
    self.nb_u,          self.nb_v,               self.selec_radius   =   3,  3,  3
    self.side_nb,       self.step_size,          self.draw_size      =   6,  1,  1
    self.selec_figure,  self.selec_deformation,  self.selec_mesh     =   0,  4,  1
    self.spin_x,        self.spin_y,             self.spin_z         =   0,  0,  0
    self.move_x,        self.move_y,             self.move_z         =   0,  0,  0
    self.radius,        self.center_off                              =  10,  0
    self.b_curve,       self.b_move                                  = false, false
end

function meu:reset()
    self:__reset_common()
    self.color = self.ui.color:set_rgb( 1, 1, 1 )
end

function meu:reset_displace()
    self.b_displace,      self.b_displace_dif,    self.b_displace_color   = false, false, false
    self.displace_factor, self.displace_filter,   self.dif_filter         = 1, 0, .1
end

function meu:init()
    local ref = self.ref
    self:__reset_common()
    self.radius_table, self.angle_table = {}, {}
    self.uv = {}
    self.coordinates = {}
    self.move, self.spin = {}, {}
end

function meu:update()
    if self.side_nb < 2 then self.side_nb = 2 end

    local n = math.sqrt( 2 / ( ( 2 * self.nb_u - 1 ) * ( 2 * self.nb_u - 1 ) + ( 2 * self.nb_v - 1 ) * ( 2 * self.nb_v - 1 ) ) )
    local mapping_ref        = self:get_layer_mapping(1)
    local factor_ref         = param.get_ref(               mapping_ref,       "tex_factor"    )
    local tex_use_ref        = param.get_ref(               mapping_ref,       "tex_use"    )
    local tex_mode_ref       = param.get_ref(               mapping_ref,       "tex_mode"    )
    
    param.set( factor_ref, n  )

    self.tex_use             = param.get(   tex_use_ref   )
    self.tex_mode            = param.get(   tex_mode_ref   )

    self.bind_value          = param.get(                   self:get_layer(1), "bind_2d_out"   )

    local rendering_ref      = aaa.obj.get_branch_by_class( self:get_layer(1),  "rendering"    )
    local normal_ref         = param.get_ref(               rendering_ref,      "normal"       )
    local normal_factor_ref  = param.get_ref(               rendering_ref,      "normal_point" )
    local light_ref          = param.get_ref(               rendering_ref,      "light"       )
    

    self.b_normal            = param.get_bool(   normal_ref          )
    self.normal_fac          = param.get(        normal_factor_ref   )
    self.b_light             = param.get_bool(   light_ref           )
    

    local bu = self.ui.bu_multi
    local extra_coor = {}
	local nb = bu:get_elt_nb()
    local id = 1
	for i=1,nb do
		local elt = bu:get_elt(i)
		extra_coor[id], extra_coor[id+1] = elt:get_xy()
        id = id + 2
	end
    self.extra_coor = extra_coor

    if self.b_normal  or self.b_light or self.tex_mode==2 or self.tex_mode==3 then  self.normal_tab = {} end
    if not self.b_triangle then  self.coordinates_no_fan = {} end
end


function meu:draw()
    self:draw_layers_begin()
        self:draw_layer( 1 )
        self:draw_grid( self:get_alpha() )
    self:draw_layers_end()
end


function meu:deform_table(table)
    -- Sphere or cylinder deformation radius
    local deformation_radius = self.radius
    -- Sphere offset initialization
    local sphere_offset = self.b_sphere_off and 0 or deformation_radius
    
    local inverse_deformation_radius = 1 / deformation_radius
    local pi_half = math.pi / 2

    -- Grid deformation function
    local function grid(x, y, z)
        return x, y, z + (self.b_sphere_off and deformation_radius or 0)
    end

    -- Tube deformation functions
    local function deform_tube_x(x, y, z)
        y = y * inverse_deformation_radius - pi_half
        local rho = z + deformation_radius
        return x, rho * math.cos(y), - rho * math.sin(y) - sphere_offset
    end

    local function deform_tube_y(x, y, z)
        x = x * inverse_deformation_radius - pi_half
        local rho = z + deformation_radius
        return rho * math.cos(x), y, - rho * math.sin(x) - sphere_offset
    end

    -- Sphere deformation function
    local function deform_sphere(x, y, z)
        x = x * inverse_deformation_radius
        y = y * inverse_deformation_radius
        local rho = z + deformation_radius
        local cy = math.cos(y)
        return rho * cy * math.sin(x), rho * math.sin(y), rho * cy * math.cos(x) - sphere_offset
    end

    -- Pringles deformation function
    local function deform_pringles(x, y, z)
        return x, y, z + (x / 3)^2 - (y / 3)^2
    end

    -- Get appropriate deformation function based on selection
    local selec = self.selec_deformation+1
    local deformation = ({deform_pringles, deform_tube_y, deform_tube_x, deform_sphere, grid })[selec]

    -- Deform table function
    local id = 1
    while id <= #table do
        table[id], table[id + 1], table[id + 2] = deformation(table[id], table[id + 1], table[id + 2])
        id = id + 3
    end
    return table
end

function meu:compute_displace( x,y, nb_u,nb_v )
    local red, green, blue, alpha = self.ui.color:get_rgba()
    local z
    local grid_id = 1
    local compo_value 
    self.grid_compo_value = self.grid_compo_value or {}

    local last = self.grid_compo_value

    local displace_factor = 25 * self.displace_factor

    local displace_dif = self.b_displace_dif and 1 or 0

    local r_col, g_col, b_col, a_col
    local bind_x_coef = 0.5 / nb_u
    local bind_y_coef = 0.5 / nb_v

    compo_value                 = aaa.img.get_component_uv( self.bind_value,  x * bind_x_coef + 0.5,     y * bind_y_coef + 0.5, 5,  false,  true    )
    r_col, g_col, b_col, a_col  = aaa.img.get_color_uv    ( self.bind_value,  x * bind_x_coef + 0.5,     y * bind_y_coef + 0.5,     false           )

    local dif
    if compo_value then
        dif = math.abs(compo_value - displace_dif*(last[grid_id] or 0))
        last[grid_id] = compo_value
        compo_value = compo_value * ((dif < self.dif_filter) and 0 or dif)
        else compo_value = 0
    end
    z = displace_factor * compo_value
    if self.b_displace_color and r_col then gol.color4( r_col*red, g_col*green, b_col*blue, a_col*alpha ) else gol.color4( red, green, blue, alpha ) end
    grid_id = grid_id + 1
    return z
end


function meu:compute_radius_angle(  radius,   side_nb   )
    local extra_coor = self.extra_coor
    local radius_table, angle_table = self.radius_table, self.angle_table
    
    local function       radius_hex(i) return radius end
    local function     radius_stars(i) return (i % 2 == 0) and self.stars_radius_int * radius or radius end
    local function    radius_clover(i) return radius * 1.2 * (1 +  0.3 * math.cos(2 * i / math.pi)) * math.sin(i / math.pi) end
    local function radius_butterfly(i) return radius * 0.8 * (1 +  0.6 * self.stars_radius_int * math.cos(5 * i / math.pi2)) * math.sin(i / math.pi) end
    local function     radius_snail(i) return radius * (1.1 * math.sin(0.1 * i/ math.pi + math.pi/2)) end
    

    local radius_func = ({  radius_hex,    radius_stars,    radius_clover,    radius_butterfly, radius_snail})[self.selec_figure+1]
    local           n = ({  side_nb,    2 * side_nb,    10 * (side_nb-2),   10 * (side_nb-2),   10 * (side_nb-2)})[self.selec_figure+1]

    local angle   = 0
    local d_angle = math.pi2/n

    self.rad_number = n + 1

    if not self.b_curve then extra_coor[2], extra_coor[#extra_coor] = 1, 1 end
    
    local id = 1
    for i = 1,  n do
        radius_table[id] = extra_coor[#extra_coor] * radius_func(i)
        angle_table[id]  = angle
        
        if self.b_curve then
            self.rad_number = n * (#extra_coor/2 + 1)
            local id_bis = #extra_coor - 1
            for j = 1, #extra_coor/2 do
                radius_table[id + 1] = extra_coor[id_bis + 1] * radius_func(i)
                angle_table[id + 1]  = angle + d_angle * ( 1 - extra_coor[id_bis] )
                
                id = id + 1
                id_bis = id_bis - 2
            end
        end
        
        id = id + 1
        angle = angle + d_angle
    end 

    radius_table[id] = extra_coor[2] * radius_func( n + 1 )
    angle_table[id]  = angle
    return radius_table, angle_table
end


function meu:compute_uv(radius_table, angle_table)
    local uv = self.uv
    if not uv then
        return
    end
    local n = self.rad_number - 1
    local text_fact = self.factor / array.get_max( radius_table, 1, self.rad_number )
    local pi_half = math.pi / 2
    local id = 1
    --aaa.show(n,"n")
    if self.b_triangle then 
        id = 3
        uv[1], uv[2] = 0.5, 0.5

        for i = 1, n do
            uv[id], uv[id + 1] = 0.5 * ( 1 + radius_table[i]   * text_fact * math.cos(angle_table[i] + pi_half )),
                                 0.5 * ( 1 + radius_table[i]   * text_fact * math.sin(angle_table[i] + pi_half ))
            id = id + 2
        end

        uv[id], uv[id + 1]     = 0.5 * ( 1 + radius_table[n-1] * text_fact * math.cos(angle_table[1] + pi_half )),
                                 0.5 * ( 1 + radius_table[n-1] * text_fact * math.sin(angle_table[1] + pi_half ))
    else 
        local id_bis = 1
        
        for i = 1, n  do
            uv[id],     uv[id+1] = 0.5,     0.5
            uv[id+2],   uv[id+3] = 0.5 * ( 1 + radius_table[id_bis]   * text_fact * math.cos(angle_table[id_bis]   + pi_half )),
                                   0.5 * ( 1 + radius_table[id_bis]   * text_fact * math.sin(angle_table[id_bis]   + pi_half ))
            uv[id+4],   uv[id+5] = 0.5 * ( 1 + radius_table[id_bis+1] * text_fact * math.cos(angle_table[id_bis+1] + pi_half )),
                                   0.5 * ( 1 + radius_table[id_bis+1] * text_fact * math.sin(angle_table[id_bis+1] + pi_half ))
            id_bis = id_bis + 1
            id = id + 6
        end
    end
    return uv
end


function meu:compute_coordinates_fan(xc, yc, zc, radius_table, angle_table, spin_table)
    local center_off = self.center_off * self.draw_size * self.step_size / 2
    local coordinates = self.coordinates

    local n = self.rad_number
    local id = 4
    local z_no_center
    

    if self.b_displace_no_center == true and  self.b_displace then z_no_center = 0 else z_no_center = zc end

    -- Variables for spins
    local cos_sx, sin_sx = math.cos(spin_table.x), math.sin(spin_table.x)
    local cos_sy, sin_sy = math.cos(spin_table.y), math.sin(spin_table.y)



    local r, a, c, s
    local pi_half = math.pi / 2

    coordinates[1], coordinates[2], coordinates[3] = xc - center_off * sin_sy, yc - center_off * sin_sx, z_no_center + center_off * cos_sy * cos_sx
    local point_number = 1

    for i = 1, n  do
        r = radius_table[i]
        a = angle_table[i] * self.angle_fan + spin_table.z + pi_half
        c, s = r * math.cos( a ), r * math.sin( a )
       
        coordinates[id], coordinates[id + 1], coordinates[id + 2] =   xc + c * cos_sy, yc + s * cos_sx, zc + c * sin_sy + s * sin_sx
     
        id = id + 3
        point_number = point_number + 1 
    end
    self.number_n = point_number
    --aaa.show(self.number_n, "coor")

    return coordinates
end

function meu:compute_normal(coordinates)
    local normal_tab = self.normal_tab
    local n = self.number_n - 2
    local id = 3
    local id_bis = 1

    --self:print( "start n is "..n )
    for i = 1,  n  do

        local nx, ny, nz = V3.compute_face_normal( coordinates[1],    coordinates[2],    coordinates[3],
                                                   coordinates[id+1], coordinates[id+2], coordinates[id+3],
                                                   coordinates[id+4], coordinates[id+5], coordinates[id+6] )

        normal_tab[id_bis],     normal_tab[id_bis + 1], normal_tab[id_bis + 2] = nx, ny, nz
        normal_tab[id_bis + 3], normal_tab[id_bis + 4], normal_tab[id_bis + 5] = nx, ny, nz
        normal_tab[id_bis + 6], normal_tab[id_bis + 7], normal_tab[id_bis + 8] = nx, ny, nz
        
        id = id + 3
        id_bis = id_bis + 9
    end
    self.normal_number = id_bis
    --self:print( "end n is "..n )
    return normal_tab
end


function meu:draw_normal(coordinates, normal_tab)
    local red, green, blue, alpha = self.ui.color:get_rgba()
    gol.color(1, 0, 0, alpha)
    local n = self.number_n

    if self.b_triangle then n = self.number_n else n = self.number_n * 3 - 6 end
    local id = 1
    local nx,ny,nz
    aaa.show(self.number_n, "normal")
    for i = 1, n do          
        nx, ny, nz = coordinates[id] + self.normal_fac * normal_tab[id],  coordinates[id+1] + self.normal_fac * normal_tab[id+1], coordinates[id+2] + self.normal_fac * normal_tab[id+2]
        
        gol.draw_lines_3d(  coordinates[id], coordinates[id+1],  coordinates[id+2],  nx, ny, nz )
        id = id + 3
    end
    gol.color(1, 1, 1, alpha)
end


function meu:convert_coordinates(coordinates,xc,yc,zc)
    -- convert from triangle fan to triangle
    local center_radius = self.center_radius
    local n = self.number_n - 2
    local coor_out = self.coordinates_no_fan
    local id = 1 
    local id2 = 4
    for i = 1, n do
        coor_out[id], coor_out[id+1], coor_out[id+2] = coordinates[1] + center_radius * (( coordinates[id2]   + coordinates[id2+3] ) / 2 - xc),
                                                       coordinates[2] + center_radius * (( coordinates[id2+1] + coordinates[id2+4] ) / 2 - yc),
                                                       coordinates[3] + center_radius * (( coordinates[id2+2] + coordinates[id2+5] ) / 2 - zc)
        coor_out[id+3], coor_out[id+4], coor_out[id+5] = coordinates[id2], coordinates[id2+1], coordinates[id2+2]
        coor_out[id+6], coor_out[id+7], coor_out[id+8] = coordinates[id2+3], coordinates[id2+4], coordinates[id2+5]

        id = id + 9
        id2 = id2 + 3
    end
    return coor_out
end

function meu:convert_normal(normal)
    -- convert from triangle to triangle fan
    local normal_tab = self.normal_tab
    local n = #normal/9
    local id = 10
    local id_bis = 7

    local normal_center = { normal[1], normal[2], normal[3] }
    local normal_p1     = { normal[4], normal[5], normal[6] }
    local normal_p2     = { normal[7], normal[8], normal[9] }

    normal_tab[4], normal_tab[5], normal_tab[6] = normal_p1[1], normal_p1[2], normal_p1[3]

    for i = 1,  n-1 do
        normal_center[1], normal_center[2], normal_center[3] =         normal_center[1] + normal[id],       normal_center[2] + normal[id+1],       normal_center[3] + normal[id+2]
        normal_p1[1],     normal_p1[2],     normal_p1[3]     = 0.5 * ( normal_p2[1] + normal[id+3] ), 0.5 * ( normal_p2[2] + normal[id+4] ), 0.5 * ( normal_p2[3] + normal[id+5] )
        normal_p2[1],     normal_p2[2],     normal_p2[3]     =                          normal[id+6],                          normal[id+7],                          normal[id+8]

        normal_tab[id_bis], normal_tab[id_bis + 1], normal_tab[id_bis + 2] = normal_p1[1],  normal_p1[2],   normal_p1[3]
        id_bis = id_bis + 3
        id = id + 9
    end

    if n ~= 0 then n = 1 / n end

    normal_tab[1],      normal_tab[2],          normal_tab[3]          = normal_center[1] * n,  normal_center[2] * n, normal_center[3] * n
    normal_tab[id_bis], normal_tab[id_bis + 1], normal_tab[id_bis + 2] =    normal[#normal-2],     normal[#normal-1], normal[#normal]

    return normal_tab
end


function meu:norm2(x, y)
    return x*x + y*y
end

-- Hello YOU
function meu:draw_grid(alpha)

    
    -- Choose colors
    local red, green, blue = self.ui.color:get_rgb()
    gol.color(red, green, blue, alpha)

    -- Time management
    local speed = self.b_move and 1 or 0
    if self.b_move then
        local dt = aaa.time.dt * speed
        local ph = self.phase
        ph[1], ph[2], ph[3] = ph[1] + dt, ph[2] + dt, ph[3] + dt
    end
    
    -- Grid dimensions
    local nb_u, nb_v = self.nb_u, self.nb_v

    -- Mesh selection
    local step_size = self.step_size

    -- Define parameters for each mesh type
    local mesh_params = {
        [1] = { stepx = step_size * math.sqrt(3) / 2, stepy = step_size * 3 / 4 },
        [2] = { stepx = step_size * math.sqrt(3) / 2, stepy = step_size }
    }

    -- Set parameters based on selected mesh type
    local params = mesh_params[self.selec_mesh]
    local stepx, stepy = params.stepx, params.stepy


    local dx = math.sqrt(3) * self.step_size / 4 -- x offset for triangular mesh

    -- Set the maximum radius of the grid
    local radius_max = self.selec_radius * self.selec_radius
    local center_x, center_y
    if self.b_move then
        center_x, center_y = aaa.math.get_turbulence( 0,  aaa.time.t,0, 5 ), aaa.math.get_turbulence(  aaa.time.t, 0,0, 5 )
    else
        center_x, center_y = 0, 0
    end
    -- Hexagon dimensions
    local r = self.draw_size * self.step_size / 2
    local side_nb = self.side_nb
    

    -- Rotation and spin parameters for hexagon drawing axes
    local params = {
        {axis = "x", move = self.move_x, spin = self.spin_x, phase = self.phase[1]},
        {axis = "y", move = self.move_y, spin = self.spin_y, phase = self.phase[2]},
        {axis = "z", move = self.move_z, spin = self.spin_z, phase = self.phase[3]}
    }

    local move, spin = self.move, self.spin

    for _, p in ipairs(params) do
        move[p.axis] = p.move * ( p.phase + 1 ) / 100
        spin[p.axis] = p.spin * ( p.phase + 1 )
    end

    local move_x, move_y, move_z = move.x, move.y, move.z


    

    -- Function to apply rotation on X, Y, and Z axes
    local function rotation_xyz(x, y, z)
        gol.rotate_x(x)
        gol.rotate_y(y)
        gol.rotate_z(z)
    end

    -- Movement function selection
    local move = ({rotation_xyz, gol.translate})[self.selec_move]



    local radius_table, angle_table = self.radius_table, self.angle_table
    radius_table, angle_table = self:compute_radius_angle( r,  side_nb )

 

    if self.tex_use == 2  or self.b_tex_implicit then
        self.uv = self:compute_uv( radius_table, angle_table )
        gol.set_attrib_uv( self.uv )
    else 
        self.uv = {}
    end

    --
    
    local x, y, z
    --local coordinates = self.coordinates

    gol.push_matrix()
    for ytr = -nb_v + 1, nb_v - 1 do
        y = stepy * ytr
        for xtr = -nb_u + 1, nb_u - 1 do
            x= stepx * xtr
            if ytr % 2 == 1 or self.selec_mesh ~= 1 then x = x + dx end

            if self:norm2( x - center_x, y - center_y ) <= radius_max then

                move( move_x, move_y, move_z )

                if self.b_displace then z = self:compute_displace(x,y,nb_u,nb_v) else z = 0 end

                self.coordinates = self:deform_table( self:compute_coordinates_fan( x, y, z, radius_table, angle_table, spin ) )

                if self.uv == nil then 
                    if self.b_normal  or self.b_light or self.tex_mode==2 or self.tex_mode==3 then  
                        self.normal_tab = self:compute_normal( self.coordinates )
                        if self.b_triangle then 
                            gol.set_attrib_normal( self.normal_tab )
                            --if self.b_displace and z >= self.displace_filter  then gol.draw_triangle_fan_uv_normal_3d( self.coordinates ) else gol.draw_triangle_fan_uv_normal_3d( self.coordinates ) end
                            gol.draw_triangle_fan_normal_3d( self.coordinates )
                            if self.b_normal then self:draw_normal( self.coordinates,  self:convert_normal( self.normal_tab ) ) end
                        else 
                            self.coordinates_no_fan = self:convert_coordinates( self.coordinates, x, y, z )
                            gol.set_attrib_normal( self.normal_tab )
                            --if self.b_displace and z >= self.displace_filter  then gol.draw_triangles_uv_normal_3d( self.coordinates ) else gol.draw_triangles_uv_normal_3d( self.coordinates ) end
                            gol.draw_triangles_normal_3d( self.coordinates_no_fan )
                            if self.b_normal then self:draw_normal( self.coordinates_no_fan,  self.normal_tab ) end
                        end
                    else
                        if self.b_triangle then 
                            gol.draw_triangle_fan_3d( self.coordinates )
                            --if self.b_displace and z >= self.displace_filter  then gol.draw_triangle_fan_uv_3d( self.coordinates ) else gol.draw_triangle_fan_uv_3d( self.coordinates ) end
                        else
                            --if self.b_displace and z >= self.displace_filter  then gol.draw_triangles_uv_3d( self:convert_coordinates( self.coordinates, x, y, z ) ) else gol.draw_triangles_uv_3d( self:convert_coordinates( self.coordinates, x, y, z ) ) end
                            gol.draw_triangles_3d( self:convert_coordinates( self.coordinates, x, y, z ) )
                        end
                    end
                else
                    if self.b_normal or self.b_light or self.tex_mode==2 or self.tex_mode==3 then  
                        self.normal_tab = self:compute_normal( self.coordinates )
                        if self.b_triangle then 
                            gol.set_attrib_normal( self.normal_tab )
                            --if self.b_displace and z >= self.displace_filter  then gol.draw_triangle_fan_uv_normal_3d( self.coordinates ) else gol.draw_triangle_fan_uv_normal_3d( self.coordinates ) end
                            gol.draw_triangle_fan_uv_normal_3d( self.coordinates , self.number_n )
                            if self.b_normal then self:draw_normal( self.coordinates,  self:convert_normal( self.normal_tab ) ) end
                        else 
                            self.coordinates_no_fan = self:convert_coordinates( self.coordinates, x, y, z )
                            gol.set_attrib_normal( self.normal_tab )
                            --if self.b_displace and z >= self.displace_filter  then gol.draw_triangles_uv_normal_3d( coordinates ) else gol.draw_triangles_uv_normal_3d( coordinates ) end
                            gol.draw_triangles_uv_normal_3d( self.coordinates_no_fan)
                            if self.b_normal then self:draw_normal( self.coordinates_no_fan,  self.normal_tab ) end
                        end
                    else
                        if self.b_triangle then 
                            gol.draw_triangle_fan_uv_3d( self.coordinates, self.number_n )
                            --if self.b_displace and z >= self.displace_filter  then gol.draw_triangle_fan_uv_3d( coordinates ) else gol.draw_triangle_fan_uv_3d( coordinates ) end
                        else
                            --if self.b_displace and z >= self.displace_filter  then gol.draw_triangles_uv_3d( convert_coordinates( coordinates, x, y, z ) ) else gol.draw_triangles_uv_3d( convert_coordinates( coordinates, x, y, z ) ) end
                            gol.draw_triangles_uv_3d( self:convert_coordinates( self.coordinates, x, y, z ) )
                        end
                    end
                end
            end
        end
    end
    gol.pop_matrix()
end
