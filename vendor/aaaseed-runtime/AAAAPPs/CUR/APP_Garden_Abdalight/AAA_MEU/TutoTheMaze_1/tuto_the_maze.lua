function meu:define_meu_infos( )
	return { author = "Abdalight", date="2024",
            tags = { "3D", "Art", "Draw", "Generator" ,"Geometry", "Procedural", "VJ" },
            help = "Maze Generator" }
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
    local ix,iy = 1,1
    local SY,DY = 1,.2
    local bu


    self:add_rendering()
	self:add_camera()

    bu = self:add_slider(                 {ix,iy,           8,SY},  "Grid Size",         self,   "grid_size",        20, 1,50    )
        bu:set_value_type_integer( true )
    iy = iy + SY + DY

    bu = self:add_slider(                 {ix,iy,           8,SY},  "Cell Size",         self,   "cell_size",        1,  0,5     )
        bu:set_value_type_integer( true )
    iy = iy + SY + DY

    bu = self:add_slider(                 {ix,iy,           8,SY},  "Wall Thickness",    self,   "wall_thick",       0,  0,5     )
    iy = iy + SY + DY

    bu = self:add_bu_texture_target_unit( {ix,iy,         8,6*SY},  "Tex",                  1,   true                            )
    iy = iy + 6 * SY


    ix, iy = 9, 2 + DY
    self:add_transfo( {ix,iy,   8,2.4} )
    iy = iy + 2.4 + DY

end

function meu:init()
    self.interpolation_factor = 0
end

function meu:draw()

    local grid_size = self.grid_size
    local cell_size = self.cell_size

    local mazes = {}
    mazes[1] = { { {-3,-3,0}, {3,-3,0} },
                 { {3,-3,0},  {3,3,0}  },
                 { {3,3,0},   {-3,3,0} },
                 { {-3,3,0},  {-3,-3,0}}
                }
    
    mazes[2] = { { {-3,-3,0}, {3,-3,0} },
                 { {3,-3,0},  {0,3,0}  },
                 { {0,3,0},   {-3,-3,0}}
                }

    mazes[3] = { { {-3,-3,0}, {3,3,0}  },
                 { {3,-3,0},  {-3,3,0} }
                }

    mazes[5] = self:compute_circle()

    math.randomseed(12345)
    mazes[1] = self:generate_grid_lines( grid_size, cell_size )
    mazes[2] = self:generate_grid_lines( grid_size, cell_size )
    mazes[3] = self:generate_grid_lines( grid_size, cell_size )
    mazes[4] = self:generate_grid_lines( grid_size, cell_size )
                   
    local interpol = 0.5 * ( math.cos(aaa.time.t) + 1 )
    
    self:draw_layers_begin()
        self:draw_layer( 1 )
        self:draw_lines_from_table( self:interpolate_mazes( mazes[1], mazes[3], interpol ) )
        --self:draw_border( grid_size, cell_size )
    self:draw_layers_end()
end


function meu:compute_circle()
    local n = 8  -- Nombre de points du cercle
    local r = 3  -- Rayon du cercle
    local angle_step = 2 * math.pi / n  -- Angle entre chaque point
    local points = {}

    -- Générer les points du cercle
    for i = 0, n - 1 do
        local theta = i * angle_step
        local x = r * math.cos(theta)
        local y = r * math.sin(theta)
        table.insert(points, {x, y, 0})
    end

    -- Construire le tableau "mazes[4]"
    local mazes = {}

    -- Relier les points du cercle
    for i = 1, n do
        local start_point = points[i]
        local end_point = points[(i % n) + 1]  -- Relier au point suivant, et boucle à partir du premier
        table.insert(mazes, {start_point, end_point})
    end

    return mazes
end

function meu:interpolate_mazes( maze1, maze2, interpolation_factor )
    local interpolated_maze = {}

    -- Trouver le nombre de lignes des deux labyrinthes
    local num_lines1 = #maze1
    local num_lines2 = #maze2
    local max_lines = math.max( num_lines1, num_lines2 )  -- Le plus grand nombre de lignes

    -- Interpolation
    for i = 1, max_lines do
        local segment1 = maze1[i]
        local segment2 = maze2[i]

        -- Si un tableau est plus court, créez des lignes vides pour l'autre tableau
        if not segment1 then
            segment1 = { {0, 0, 0}, {0, 0, 0} }
        end
        if not segment2 then
            segment2 = { {0, 0, 0}, {0, 0, 0} }
        end

        -- Interpoler entre les deux segments
        local p1_start = segment1[1]
        local p1_end = segment1[2]
        local p2_start = segment2[1]
        local p2_end = segment2[2]

        local interpolated_start = {
            p1_start[1] + (p2_start[1] - p1_start[1]) * interpolation_factor,
            p1_start[2] + (p2_start[2] - p1_start[2]) * interpolation_factor,
            p1_start[3] + (p2_start[3] - p1_start[3]) * interpolation_factor
        }

        local interpolated_end = {
            p1_end[1] + (p2_end[1] - p1_end[1]) * interpolation_factor,
            p1_end[2] + (p2_end[2] - p1_end[2]) * interpolation_factor,
            p1_end[3] + (p2_end[3] - p2_end[3]) * interpolation_factor
        }

        -- Ajouter le segment interpolé au tableau résultant
        table.insert( interpolated_maze, { interpolated_start, interpolated_end } )
    end

    return interpolated_maze
end

function meu:interpolate_mazes2( maze1, maze2, interpolation_factor )
    local interpolated_maze = {}
    
    -- Trouver le nombre de lignes des deux labyrinthes
    local num_lines1 = #maze1
    local num_lines2 = #maze2
    
    -- Le nombre de lignes intermédiaire à créer, en ajustant les tailles
    local max_lines = math.max(num_lines1, num_lines2)
    
    -- Calculer la différence de taille entre les labyrinthes
    local diff = math.abs(num_lines1 - num_lines2)
    
    -- Interpolation
    for i = 1, max_lines do
        -- Calculer l'indice correspondant dans les deux labyrinthes
        local index1 = math.floor((i - 1) * (num_lines1 - 1) / (max_lines - 1)) + 1
        local index2 = math.floor((i - 1) * (num_lines2 - 1) / (max_lines - 1)) + 1

        local segment1 = maze1[index1] or { {0, 0, 0}, {0, 0, 0} }
        local segment2 = maze2[index2] or { {0, 0, 0}, {0, 0, 0} }

        -- Interpolation des coordonnées (x, y, z) pour chaque segment
        local p1_start = segment1[1]
        local p1_end = segment1[2]
        local p2_start = segment2[1]
        local p2_end = segment2[2]

        -- Interpoler les coordonnées
        local interpolated_start = {
            p1_start[1] + (p2_start[1] - p1_start[1]) * interpolation_factor,
            p1_start[2] + (p2_start[2] - p1_start[2]) * interpolation_factor,
            p1_start[3] + (p2_start[3] - p1_start[3]) * interpolation_factor
        }

        local interpolated_end = {
            p1_end[1] + (p2_end[1] - p1_end[1]) * interpolation_factor,
            p1_end[2] + (p2_end[2] - p1_end[2]) * interpolation_factor,
            p1_end[3] + (p2_end[3] - p1_end[3]) * interpolation_factor
        }

        -- Ajouter le segment interpolé au tableau résultant
        table.insert( interpolated_maze, { interpolated_start, interpolated_end } )
    end

    return interpolated_maze
end



-- Function to generate a structured table with points in a square
function meu:generate_grid_lines( grid_size, cell_size )
    local lines_table = {}
    local half_grid_size = ( grid_size * cell_size ) / 2

    for row = 0, grid_size - 1 do
        for col = 0, grid_size - 1 do
            local x = col * cell_size - half_grid_size
            local y = row * cell_size - half_grid_size

            local p1, p2
            -- Ligne horizontale ou verticale choisie aléatoirement
            if math.random() > 0.5 then
                -- Ligne horizontale
                p1 = { x, y, 0 }
                p2 = { x + cell_size, y, 0 }
            else
                -- Ligne verticale
                p1 = { x, y, 0 }
                p2 = { x, y + cell_size, 0 }
            end

            table.insert(lines_table, { p1, p2 })
        end
    end

    return lines_table
end





function meu:draw_lines_from_table( lines_table )
    local thick = self.wall_thick
    local uv = { 0, 0, 1, 0, 1, 1, 0, 1 }
    
    for _, line in ipairs( lines_table ) do
        local p1, p2 = line[1], line[2]
        -- Dessiner la ligne à z = 0
        gol.draw_lines_3d( p1[1], p1[2], p1[3], p2[1], p2[2], p2[3] )
        
        -- Dessiner la ligne correspondante pour z = 5
        gol.draw_lines_3d( p1[1], p1[2], thick, p2[1], p2[2], thick )
        
        -- Relier les deux plans (z=0 et z=thick) par des lignes
        gol.draw_lines_3d( p1[1], p1[2], p1[3], p1[1], p1[2], thick )
        gol.draw_lines_3d( p2[1], p2[2], p2[3], p2[1], p2[2], thick )

        gol.set_attrib_uv(uv)

        gol.draw_quad_strip_uv_3d( p1[1], p1[2], p1[3], p1[1], p1[2], thick, p2[1], p2[2], p2[3], p2[1], p2[2], thick )

    end
end


function meu:draw_border( grid_size, cell_size )
    local total_size = grid_size * cell_size
    local half_size = total_size / 2

    local x_min, y_min = -half_size, -half_size
    local x_max, y_max =  half_size,  half_size
    local z = self.wall_thick

    gol.draw_lines_3d( 
        x_min, y_min, z, x_max, y_min, z,
        x_max, y_max, z, x_min, y_max, z,
        x_min, y_min, z, x_min, y_max, z,
        x_max, y_min, z, x_max, y_max, z
    )
end


