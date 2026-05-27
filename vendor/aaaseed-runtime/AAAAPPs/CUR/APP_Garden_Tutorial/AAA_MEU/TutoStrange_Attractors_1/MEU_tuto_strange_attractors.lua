function meu:define_meu_infos( )
	return { author = "Abdalight", date="2024",
             tags   = { "3D", "Art", "Draw", "Generator", "Procedural", "Tutorial" },
             help   = "Ploting a strange attractor with Lorentz setup"
            }
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
    local ix,iy = 1,1
    local SY,DY = 1,.2
    local bu

    self:add_rendering()    
	self:add_camera()

    bu = self:add_slider( {ix,iy,         8,SY},  "Number Point",   self,       "num_points", 5000,  0,10000 )
        bu:set_value_type_integer( true )
    iy = iy + SY
    
    

    bu = self:add_slider( {ix,iy,         8,SY},  "Sigma",          self,       "sigma",          10,  1,100 )
    iy = iy + SY

    bu = self:add_slider( {ix,iy,         8,SY},  "Rho",            self,       "rho",            28,  1,100 )
        bu:set_value_type_integer( true )
    iy = iy + SY

    bu = self:add_slider( {ix,iy,         8,SY},  "Beta",           self,       "beta",          8/3,  1,100 )
    iy = iy + 1.1*SY

    bu = self:add_button( {ix,iy,         2,SY},  "Move",           self,       "time_active",       false   )
    iy = iy + 1.1*SY

    bu = self:add_button( {ix,iy,       8/3,SY},  "Symmetry_X",     self,       "x0",                false   )
    bu = self:add_button( {ix+8/3,iy,   8/3,SY},  "Symmetry_Y",     self,       "y0",                false   )
    bu = self:add_button( {ix+16/3,iy,  8/3,SY},  "Symmetry_Z",     self,       "z0",                false   )
    iy = iy + 1.1*SY

    bu = self:add_button( {ix,iy,         2,SY},   "Interpol",      self,       "int",               false   )
    iy = iy + 1.1*SY

    bu = self:add_slider( {ix,iy,         8,SY},   "Speed",         self,       "speed",             0,  0,1 )
    iy = iy + 1.1*SY

    ix, iy = 9,2

    bu = self:add_transfo( { ix, iy,    nil , 2.4 } ,  1 )
    iy = iy + 2.4
end

function meu:draw()
    self:draw_layers_begin()
        self:draw_layer( 1 )
        self:draw_1( self:get_alpha())
    self:draw_layers_end()
end

function meu:init()
    self.phase = { 0,0,0 }
    self.time_active = 0
end

function meu:draw_1( alpha )
    gol.color_white( alpha )

    -- Time management

    
    local ph = { 0,0,0 }
    local dt = aaa.time.dt

    if self.time_active then
        ph = self.phase
        ph[1], ph[2], ph[3] = ph[1] + dt, ph[2] + dt, ph[3] + dt
    else 
        ph = self.phase
    end


    local x0, y0, z0 = .1,.1,.1
    local d_t = 0.01 --* aaa.time.dt
    local num_points = self.num_points


    local _, phase_interpol = math.modf( self.speed*ph[2]  )

    if self.int then
        phase_interpol = phase_interpol
    else 
        phase_interpol = 1
    end
    aaa.show(phase_interpol, "Attractor Phase")
    

    local function generateAttractor( x0, y0, z0, dt, numPoints, equations )
        local points = {}
        local x, y, z = x0, y0, z0
        for i = 1, numPoints do
            local dx, dy, dz = equations(x, y, z, dt)
            x = x + dx
            y = y + dy
            z = z + dz
            table.insert(points, {x = x, y = y, z = z})
        end
        return points
    end
    
    -- Lorenz equations
    local function lorenzEquations( x, y, z, dt )
        local sigma = self.sigma
        local rho = self.rho
        local beta = self.beta
        local dx = sigma * (y - x) * dt 
        local dy = (x * (rho - z) - y) * dt
        local dz = (x * y - beta * z) * dt
        return dx, dy, dz
    end
    

    local attractorPoints = generateAttractor( x0, y0, z0, d_t, num_points, lorenzEquations )


    local function interpol_2point(x1,x2,coef)
        local x
        x = coef * x1 + ( 1 - coef ) * x2
        return x
    end

    local function interpol_xyz(table , i, interpol_function )
        local x1, x2 = table[i].x, table[i+1].x
        local y1, y2 = table[i].y, table[i+1].y
        local z1, z2 = table[i].z, table[i+1].z

        local interpol_X ,interpol_Y, interpol_Z
        interpol_X = interpol_function( x1, x2, phase_interpol )
        interpol_Y = interpol_function( y1, y2, phase_interpol )
        interpol_Z = interpol_function( z1, z2, phase_interpol )
        return interpol_X ,interpol_Y, interpol_Z
    end


    local interpol_X ,interpol_Y, interpol_Z

    for i = 1, num_points - 1 do
        gol.color_white( alpha )
        interpol_X ,interpol_Y, interpol_Z = interpol_xyz( attractorPoints, i, interpol_2point )
        gol.draw_points_3d( interpol_X, interpol_Y, interpol_Z )
        if self.x0                         then gol.draw_points_3d( - interpol_X,   interpol_Y,   interpol_Z ) end
        if self.y0                         then gol.draw_points_3d(   interpol_X, - interpol_Y,   interpol_Z ) end
        if self.z0                         then gol.draw_points_3d(   interpol_X,   interpol_Y, - interpol_Z ) end
        if self.x0 and self.z0             then gol.draw_points_3d( - interpol_X,   interpol_Y, - interpol_Z ) end
        if self.y0 and self.z0             then gol.draw_points_3d(   interpol_X, - interpol_Y, - interpol_Z ) end
        if self.z0 and self.x0 and self.y0 then gol.draw_points_3d( - interpol_X, - interpol_Y, - interpol_Z ) end
    end


end

--[[    local snake_speed =  .01 
    local snake_size = 30
    
    local function snake_move()
    local id = math.floor(num_points * ( 0.5 * ( 1 + math.cos( snake_speed * ph[1] ) ) ) + 1 )
    if id<= snake_size then return snake_size 
        elseif id>= num_points - snake_size - 1 then return num_points - snake_size
        else return id
    end
end


local snake_start = snake_move()
local snake_end = snake_start + snake_size

local z_decalage = 10
local owl = self.owl
--aaa.show(snake_start, "Tete de Snake")
--if snake_move()>= numPoints then 

--[[for i = 1, snake_start-1 do
    gol.color_white( alpha )
    gol.draw_points_3d(attractorPoints[i].x, attractorPoints[i].y, attractorPoints[i].z - z_decalage)
    if owl == true then
        gol.draw_points_3d(2*phase*attractorPoints[i].x, attractorPoints[i].y, -attractorPoints[i].z - z_decalage)
        gol.draw_points_3d(-attractorPoints[i].x, attractorPoints[i].y, attractorPoints[i].z - z_decalage)
        gol.draw_points_3d(-2*phase*attractorPoints[i].x, attractorPoints[i].y, -attractorPoints[i].z - z_decalage)
    end

end

for i = snake_start, snake_start+10 do
    gol.color_red( alpha )
    gol.draw_points_3d(attractorPoints[i].x, attractorPoints[i].y, attractorPoints[i].z - z_decalage)
    if owl == true then
        gol.draw_points_3d(attractorPoints[i].x, attractorPoints[i].y, -attractorPoints[i].z - z_decalage)
        gol.draw_points_3d(-attractorPoints[i].x, attractorPoints[i].y, attractorPoints[i].z - z_decalage)
        gol.draw_points_3d(-attractorPoints[i].x, attractorPoints[i].y, -attractorPoints[i].z - z_decalage)
    end
end

for i = snake_start+10, snake_start+20 do
    gol.color_yellow( alpha )
    gol.draw_points_3d(attractorPoints[i].x, attractorPoints[i].y, attractorPoints[i].z - z_decalage)
    if owl == true then
        gol.draw_points_3d(attractorPoints[i].x, attractorPoints[i].y, -attractorPoints[i].z - z_decalage)
        gol.draw_points_3d(-attractorPoints[i].x, attractorPoints[i].y, attractorPoints[i].z - z_decalage)
        gol.draw_points_3d(-attractorPoints[i].x, attractorPoints[i].y, -attractorPoints[i].z - z_decalage)
    end
end

for i = snake_start+20, snake_end do
    gol.color_green( alpha )
    gol.draw_points_3d(attractorPoints[i].x, attractorPoints[i].y, attractorPoints[i].z - z_decalage)
    if owl == true then
        gol.draw_points_3d(attractorPoints[i].x, attractorPoints[i].y, -attractorPoints[i].z - z_decalage)
        gol.draw_points_3d(-attractorPoints[i].x, attractorPoints[i].y, attractorPoints[i].z - z_decalage)
        gol.draw_points_3d(-attractorPoints[i].x, attractorPoints[i].y, -attractorPoints[i].z - z_decalage)
    end
end

for i = snake_end + 1,  num_points do
    gol.color_white( alpha )
    gol.draw_points_3d(attractorPoints[i].x, attractorPoints[i].y, attractorPoints[i].z - z_decalage)
    if owl == true then
        gol.draw_points_3d(attractorPoints[i].x, attractorPoints[i].y, -attractorPoints[i].z - z_decalage)
        gol.draw_points_3d(-attractorPoints[i].x, attractorPoints[i].y, attractorPoints[i].z - z_decalage)
        gol.draw_points_3d(-attractorPoints[i].x, attractorPoints[i].y, -attractorPoints[i].z - z_decalage)
    end
end]]
    
--[[     -- Fonction pour dessiner les points de l'attracteur
  local function drawAttractor()
    for i = 1, #attractorPoints do
        gol.draw_points_3d(attractorPoints[i].x, attractorPoints[i].y, attractorPoints[i].z)
    end
    end

-- Boucle principale pour l'animation
local time = 0
    while true do
        -- Mettre à jour le temps
        time = time + dt

        -- Dessiner les points de l'attracteur
        drawAttractor()

        -- Calculer l'index du point en fonction du temps
        local index = math.floor((time * 100) % #attractorPoints) + 1


        gol.color_black( alpha )
        -- Dessiner le point animé
        gol.draw_points_3d(attractorPoints[index].x, attractorPoints[index].y, attractorPoints[index].z)
    end
 ]]