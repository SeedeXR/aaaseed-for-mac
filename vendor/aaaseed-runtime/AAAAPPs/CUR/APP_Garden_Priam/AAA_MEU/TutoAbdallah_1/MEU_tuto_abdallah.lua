function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
    local ix,iy = 1,1
    local SY,DY = 1,.2

    self:add_rendering()    --todo why we get remdering from layer_c ???
	self:add_camera()

    -- local bu = self:add_slider( {ix,iy, 8,SY}, "N", self, "N", 10,  0,1000 )
    --     bu:set_value_type_integer( true )
    iy = iy + SY

end

function meu:draw()
    self:draw_layers_begin()
        --  self:print( aaa.time.t )
        self:draw_layer( 1 )
        self:draw_1( self:get_alpha(), 10, false )       -- choose the drawing function here
        self:draw_3( self:get_alpha(), 10, false )       -- choose the drawing function here
    self:draw_layers_end()
end

function meu:distance( x1, y1, x2, y2 )
    return math.sqrt( (x2-x1)^2 + (y2-y1)^2 )
end
-- function meu:distThreshold( x3, y3, x4, y4 )
--     local dist = self:distance ( x3, y3, x4, y4 )
--     return clamp(dist, 1, 4)
-- end

function meu:distThresholdP(r1, r2, x3, y3, x4, y4 ) 
    -- r1 r2 bounds, x3, y3, x4, y4 distance params, below is out-of-bounds as a return
    local val = self:distance ( x3, y3, x4, y4 )
        if (val >= r1) and (val <= r2) then
            return val 
        else
            return 6-- height space 
       end
end



-- documentation is in
--  AAADoc/lua_aaaseed_draw.lua
--  AAADoc/lua_aaaseed_interface.lua

-- Hello world
function meu:draw_1( alpha )

    -- gol.color_white( alpha )
    -- aaa.draw_str( "Bonjour\nil est "..os.date("%H:%M:%S") )
    local r = 1 --- scale of grid increments
    local r2 = r/2
    local r3 = r*math.sqrt(3)/2  -- 1.73 instead of 2 feels so much better! I checked, 2 is right but it really feels optically squeezed, P

    -- local function draw_hex( x, y )
    --     -- gol.color_green( alpha )
    --     -- gol.draw_lines_2d(  x,      r+y,    r3+x,    r2+y   )
    --     -- gol.color_yellow( alpha )
    --     -- gol.draw_lines_2d( r3+x,    r2+y,   r3+x,   -r2+y   )
    --     -- gol.color_red( alpha )
    --     -- gol.draw_lines_2d( r3+x,    -r2+y,   x,     -r+y    )
    --     -- gol.color_green( alpha )
    --     -- gol.draw_lines_2d(  x,      -r+y,    -r3+x, -r2+y   )
    --     -- gol.color_red( alpha )
    --     -- gol.draw_lines_2d( -r3+x,   r2+y,     x,    r+y     )
    --     -- gol.color_yellow( alpha )
    --     -- gol.draw_lines_2d( x,       r+y,     x,     2*r+y   )
    -- end

    local function draw_hex2( x, y )
        gol.draw_line_loop_2d(  x,      r+y,    r3+x,    r2+y,
                            r3+x,    r2+y,   r3+x,   -r2+y,
                            r3+x,    -r2+y,   x,     -r+y, 
                             x,      -r+y,    -r3+x, -r2+y,
                            -r3+x,   r2+y,     x,    r+y,  
                            x,       r+y )
    end


    local N = self.N -- rang N*N

    self.N = 8.0   ----grid size if not using slider above, P

    -- gol.color_white( alpha )
    -- for xTR=-N, N do
    --     local x = 2*r3*xTR
    --     for yTR = -N, N do 
    --         local y = 3*r*yTR
    --         draw_hex2( x, y )
            
    --     end
    -- end

    math.randomseed(42)


self:print("thresh is ".. self:distThresholdP(0, 4, 1.8, 1.8, 9, 9 ) )
-----------------MAAs ORIGINAL GRID
    -- for xTR=-N, N do
    --     local x = 2*r3*xTR
    --     for yTR = -N, N do 
    --         local z = 3.465*r3*yTR --+ 4*r3*xTR  ---- diagonal! local z = 4*r3*yTR + 2*r3*xTR
    --         local yup =  -.3 * self:distThresholdP(6, 10, 0, 0, x, z) 
    --         gol.push_matrix()
    --             gol.translate( x, yup, z )               -- translate random
    --             gol.rotate_x(  .25 )                   -- rotate random
    --             -- gol.rotate_y(  math.random() + aaa.time.t*.1 )   -- rotate random
    --             draw_hex2( 0, 0 ) -- push grid horiz or vert
    --         gol.pop_matrix()
    --         -- repeat & mke 2 offset grids
    --     end

----------------PRIAM's NEW GRID
local function createRectangularHexGrid(numPoints)
    local hexGrid = {}
    local numColumns = math.ceil(math.sqrt(numPoints))
    local numRows = math.ceil(numPoints / numColumns)
    local index = 1
   
    -- Calculate the center of the grid
    local centerX = (numColumns - 1) * math.sqrt(3) / 2
    local centerY = (numRows - 1) * 1.5 / 2
    local centerZ = 0

    for i = 1, numRows do
        for j = 1, numColumns do
            local x = (j - 1) * math.sqrt(3) - centerX
            local y = (i - 1) * 1.5 - centerY
            if i % 2 == 0 then
                x = x + 0.5 * math.sqrt(3)
            end
            hexGrid[index] = {x = x, y = y, z = centerZ}
            index = index + 1
            if index > numPoints then
                return hexGrid
            end
        end
    end

    return hexGrid
end

-- Example usage
local numPoints = 300
local hexGrid = createRectangularHexGrid(numPoints)

-- Printing out the generated grid points
for i, point in ipairs(hexGrid) do
   -- 
   -- print("Point " .. i .. ": x = " .. point.x .. ", y = " .. point.y)
    gol.push_matrix()
    gol.scale(.2, .2, .2)
    local yup =  -.3 * self:distThresholdP(6, 10, 0, 0, point.x, point.y)
    gol.translate(point.x, yup*-1, point.y)
    gol.rotate_x(  .25 )  
    draw_hex2( 0, 0 )
    

    gol.pop_matrix()
end




-- Flash
function meu:draw_2( alpha ) 
    gol.color_white( (math.sin(aaa.time.t * 10.) * .5 + .5) * alpha )
    aaa.draw_str( "Flash" )
end

-- Rectangle
function meu:draw_3( alpha )
    gol.color_green( alpha )
    aaa.draw_rect_line( -1,-1, 1,1 )
end

-- Rectangles
function meu:draw_4( alpha, nb_factor )
    gol.color_yellow( alpha )
    local nb = 100 * nb_factor
    local size = 8
    local d = size/(nb-1)
    math.randomseed(42)
    for i=1,nb do
        local x = -size/2 + size * (i-1)/(nb-1)
        aaa.draw_rect_line( x,0, x+d,math.random()*2 )
    end
end

-- more Rectangles
function meu:draw_5( alpha, nb_factor )
    local nb_y = 8 * nb_factor
    local size_y = 8
    local d_y = size_y/(nb_y-1)

    local nb_x = 8 * nb_factor 
    local size_x = 8
    local d_x = size_x/(nb_x-1)
    math.randomseed(42)

    for iy=1,nb_y do
        local y = -size_y/2 + size_y * (iy-1)/(nb_y-1)
        for ix=1,nb_x do
            local x = -size_x/2 + size_x * (ix-1)/(nb_x-1)
            local rnd = math.random()
            --rnd = math.pow( rnd, 4 ) * 4.
            local d = rnd * d_y * .5
            d = d * (math.sin( -aaa.time.t * 5. + iy / 3. ) * .25 + .75 )
            local r,g,b = math.random(), math.random(), math.random()
            gol.color( r,g,b, alpha )  -- red green blue alpha
            --gol.color( r*rnd,g*rnd,b*rnd )
            aaa.draw_rect( x-d,y-d, x+d,y+d)
        end
    end
    
end

-- Some 3d
local function get_color()
    local r = math.random()
    local g = math.random()
    local b = math.random()
    local gam = 1.
    return math.pow(r,gam), math.pow(g,gam), math.pow(b,gam)
end

function meu:draw_6( alpha, nb_factor, b_rotate )
    local nb = 160 * nb_factor * nb_factor
    local size = 8
    math.randomseed(42)

    for i=1,nb do
        local x = size * (math.random() - .5)  
        local y = size * (math.random() - .5)  
        local z = size * math.random() * .1
        local r,g,b = get_color()
        gol.color( r,g,b, alpha )
         --gol.color( r*rnd,g*rnd,b*rnd )
        gol.push_matrix()
            gol.translate( x,y,z )
            if b_rotate then
                gol.rotate_y( math.random() )
                gol.rotate_x( math.random() )
            end
            local s = .5
            local sx = math.random() * s
            local sy = math.random() * s
            local sz = math.random() * s       
            aaa.draw_box( sx,sy,sz )
        gol.pop_matrix()
    end
end
end