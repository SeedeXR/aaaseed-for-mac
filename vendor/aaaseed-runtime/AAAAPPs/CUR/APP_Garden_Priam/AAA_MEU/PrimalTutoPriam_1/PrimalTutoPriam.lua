function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
    local ix,iy = 1,1
    local SY,DY = 1,.2  
 
 --   self:add_selector( {ix,iy, 8,SY}, )
    self:add_rendering()
	self:add_camera()

    bu = self:add_slider(		{ix,iy,		8,SY},  "Size",			self, "size",				8,		0,8 )
    bu = self:add_button(		{1,nil,		4,nil}, "Obj",			self, "b_obj",				false)
 
    bu = self:add_button(		{1,nil,		4,nil}, "Box",			self, "b_box",				false)
    bu = self:add_slider(	    {1,nil,     8,nil}, "Size B",		self, "size_b",				1,		0,3 )
    bu = self:add_slider(	    nil,                "Size C",		self, "size_c",				1,		0,3 )
    bu = self:add_slider(		nil,                "Size D",		self, "size_d",				1,		0,3 )
    
    bu = self:add_rgbfa(         nil,	"color_1" )

    bu = self:add_selector(	    {1,10,	8,1},   "Gene" )
    bu:set_item_text( 2, "1", "2", "3", "4", "5", "6" )
    --bu:set_nb_min_0( 7, 1 )
    --bu:set_value(1)
--         bu:set_nb( 7, 1 )
--        -- bu:set_value(0)
--         bu:set_item_text( 2, "1", "2", "3" )
    ui.bu_selector = bu

    bu = self:add_selector(	{ix,iy+10,	8,SY}, "Mode" )
	bu:set_item_text( 1, "A", "B", "M", "MA", "MB", "MAB" )
	bu:set_target_lua( self, "mode" )
end

function meu:draw()
    self:draw_layers_begin()
        --  self:print( aaa.time.t )
        self:draw_layer( 1 )
        if self.b_obj then
            self:draw_layer( 2 )
        end
        if self.b_box then
            aaa.draw_box( self.size_b, self.size_c, self.size_d )
        end
       local sel = self.ui.bu_selector:get_value() - 1
        if sel > 0 then
            local fn = self["draw_"..sel]
            if fn then
                fn( self, 10, false )       -- choose the drawing function here
            end
        end
        self:draw_layer( 3 )
    self:draw_layers_end()
end

-- documentation is in
--  AAADoc/lua_aaaseed_draw.lua
--  AAADoc/lua_aaaseed_interface.lua

-- Hello world
function meu:draw_1()
    gol.color_white()
    aaa.draw_str( "Bonjour\nil est "..os.date("%H:%M:%S") )
end
-- Flash
function meu:draw_2()
    gol.color_white( math.sin(aaa.time.t * 10.) * .5 + .5 )
    aaa.draw_str( "Flash" )
end
-- Rectangle
function meu:draw_3()
    gol.color_green()
    aaa.draw_rect_line( -1,-1, 2,2 )    
end
-- Rectangles
function meu:draw_4( nb_factor )
    gol.color_yellow( 1 )   -- red green blue alpha
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
function meu:draw_5( nb_factor )
    gol.color( 1,1,0, 1 )   -- red green blue alpha
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
            gol.color( r,g,b )
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
function meu:draw_6( nb_factor, b_rotate )
    local nb = 160 * nb_factor * nb_factor
    local size = 8
    math.randomseed(42)
    for i=1,nb do
        local x = size * (math.random() - .5)  
        local y = size * (math.random() - .5)  
        local z = size * math.random() * .1
        local r,g,b = get_color()
        gol.color( r,g,b )
         --gol.color( r*rnd,g*rnd,b*rnd )
        gol.push_matrix()
            gol.translate( x,y,z )
            if b_rotate then
                gol.rotate_y( math.random() )
                gol.rotate_x( math.random() )
            end
            local s = .5
            local sx = math.random()*s
            local sy = math.random()*s
            local sz = math.random()*s       
            aaa.draw_box( sx,sy,sz )
        gol.pop_matrix()
    end
end