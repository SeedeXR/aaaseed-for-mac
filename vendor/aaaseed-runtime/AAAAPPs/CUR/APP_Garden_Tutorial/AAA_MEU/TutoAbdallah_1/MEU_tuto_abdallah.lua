function meu:define_meu_infos( )
	return { author = "Abdalight", date="2024",
            tags = { "2D", "Art", "Draw", "Generator", "Procedural", "Experimental", "Tutorial" },
            help="Ga Bu Zo Meu" }
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
    local ix,iy = 1,1
    local SY,DY = 1,.05
    local bu

    self:add_rendering()
	self:add_camera()

    bu = self:add_selector(	{ix,iy,         8,SY},		"Shadoko"		)
			bu:set_item_text( 1, "Ga", "Bu", "Zo", "Meu" )
            bu:set_target_lua( self , "shadoko" )
            bu:set_text_draw( false ) 
            bu:set_text_nice( true )
    iy = iy + SY + DY

    bu = self:add_slider(   {ix,iy,         8,SY},    "S_Head",         self,   "head_size",      3,         .5,10 )
    iy = iy + SY + DY

    bu = self:add_slider(   {ix,iy,         8,SY},    "S_Eye",          self,   "eye_size",       1,         .1,10 )
    iy = iy + SY + DY

    bu = self:add_slider(   {ix,iy,         8,SY},    "S_Beak",         self,   "beak_size",      5,          1,10 )
    iy = iy + SY + DY

    bu = self:add_slider(   {ix,iy,         8,SY},    "S_Hair",         self,   "hair_size",      2,          1,10 )
    iy = iy + SY + DY

    bu = self:add_slider(   {ix,iy,         8,SY},    "S_Leg",          self,   "leg_size",       2,          1,10 )
    iy = iy + SY + DY

    bu = self:add_slider(   {ix,iy,         8,SY},    "S_Hand",         self,   "hand_size",      2,          1,10 )
    iy = iy + 1.5*SY + DY

    bu = self:add_button(   {ix,iy,         3,SY},    "Just Pump It",   self,   "b_pump",       false              )
    ix, iy = ix + 8, 1 + SY + DY

    bu = self:add_slider_multi_curve(   {ix,iy+1,        8,7*SY},   "Hand Points",      3 )
        bu:set_text_visible( false )
		ui.bu_multi = bu
        self:add_trig_method(           {ix+2,iy,           1,1},   "+",    bu, "add_elt_ui", 1  ):set_text_inside( true )
        self:add_trig_method(	        {ix+3,iy,	        1,1},   "-",    bu, "add_elt_ui", -1 ):set_text_inside( true )
    iy = iy + 7*SY + DY
end

function meu:draw()
    self:draw_layers_begin()
        --  self:print( aaa.time.t )
        self:draw_layer( 1 )
        self:draw_1( self:get_alpha(), 10, false )       -- choose the drawing function here
    self:draw_layers_end()
end


function meu:update()
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
    
end


-- documentation is in
--  AAADoc/lua_aaaseed_draw.lua
--  AAADoc/lua_aaaseed_interface.lua

-- Hello world
function meu:draw_1( alpha )
    gol.color_white( alpha )

    local  function v(x,y,z, freq)
        local v = 2 *aaa.math.get_turbulence( x, y, z, freq ) + self.head_size
        return v
    end
    
    local vertices = {}


    local function draw_circle(x, y, radius, forme)
        local vertices = {}
        local segments = 200
   
        forme = forme + self.head_size
        vertices[1]  = x 
        vertices[2]  = y 
        local id = 3
        local angle = math.pi2 / segments
        local r
        local c , s
        for i = 0, segments do
            c ,s = math.cos(angle*i) * radius, math.sin(angle*i) * radius
            --if i % 1 == 1 then r = radius * v(c,s,aaa.time.t, .2 )*.2 else r = radius end
            r = radius * v(c,s,forme, .2 )*.35
            c ,s = math.cos(angle*i) * r, math.sin(angle*i) * r
            vertices[id]    = x + c 
            vertices[id+1]  = y + s 
            id = id + 2
        end
        --gol.draw_lines_2d(vertices)
        gol.draw_triangle_fan_2d(vertices )
        return vertices
    end

    self.vertices = vertices
    local function draw_shadok(x, y , forme)
        local head_radius = self.head_size
        local eye_radius, pupil_radius = self.eye_size, self.eye_size/5
        
        local eye_position = head_radius*0.7

        local eye_space = .1
        local d_3d = .001
        local z = 0

        local pup_mov = math.sin(aaa.time.t)/2
        local pup_mov2 = math.cos(aaa.time.t)/2
        local mov = 0.5*math.sin(aaa.time.t)+0.5

        --y = y + mov

        -- Draw head
        gol.color_orange( alpha )
        --aaa.draw_circle_axe_z(x,y,0, 2*head_radius, 100)
        vertices = draw_circle(x, y, head_radius, forme)
 
        eye_radius = eye_space + eye_radius
        -- Draw eyes
        gol.color_white( alpha )
        aaa.draw_disk_axe_z(x + eye_radius, y,d_3d, 2*eye_radius, 100)
        aaa.draw_disk_axe_z(x - eye_radius, y,d_3d, 2*eye_radius, 100)

        gol.color_black( alpha )
        aaa.draw_disk_axe_z(x + pup_mov+eye_radius, y+pup_mov2,2*d_3d, 2*pupil_radius, 100)
        aaa.draw_disk_axe_z(x - pup_mov-eye_radius, y+pup_mov2,2*d_3d, 2*pupil_radius, 100)

        -- Draw beak
        local beak_size = self.beak_size

        gol.color_black( alpha )
        gol.draw_lines_3d( x + 2*eye_radius,    y - eye_radius, z,	x,  y - eye_radius-pup_mov*.25,    beak_size   )
        gol.draw_lines_3d( x - 2*eye_radius,   y - eye_radius, z,	x,  y - eye_radius-pup_mov*.25,    beak_size   )
        gol.draw_lines_3d( x + 2*eye_radius,    y - eye_radius, z,  x,  y - eye_radius-pup_mov*.5-.5,beak_size*.45)
        gol.draw_lines_3d( x - 2*eye_radius,   y - eye_radius, z,  x,  y - eye_radius-pup_mov*.5-.5,beak_size*.45)

        -- Draw hair

        local hair_size = self.hair_size
        gol.color_black( alpha )
        gol.draw_lines_3d(x,y,z, x,y+hair_size,mov )
        gol.draw_lines_3d(x,y,z, x+.5,y+hair_size,mov )
        gol.draw_lines_3d(x,y,z, x-.5,y+hair_size,mov )
   
    end

    local function draw_hand(center_x,center_y,center_z,witch)
        -- Draw arms
        gol.color_black( alpha )
        local hand = {}
        local extra_coor = self.extra_coor
        local id = 1
        local x,y,z = center_x,center_y,center_z
        local id_bis = 1
        local size = self.hand_size 

        local c,s
        c, s = math.cos( aaa.time.t ), math.sin( aaa.time.t )
        if witch == 1 then
        --hand[1], hand[2], hand[3] = x+.5, y, z
            for j = 1, #extra_coor/2 do
                hand[id]   = x + extra_coor[id_bis]*size 
                hand[id+1] = y - extra_coor[id_bis + 1]*size 
                hand[id+2] = z
                id = id + 3
                id_bis = id_bis + 2
            end
        else
            for j = 1, #extra_coor/2 do
                hand[id]   = x - extra_coor[id_bis]*size
                hand[id+1] = y - extra_coor[id_bis + 1]*size
                hand[id+2] = z
                id = id + 3
                id_bis = id_bis + 2
            end
        end

        
        --gol.draw_triangle_fan_3d( hand )
        gol.draw_line_strip_3d(hand)
    end
    

    local function draw_leg(center_x,center_y,center_z,witch)
        -- Draw legs
        gol.color_yellow( alpha )

        local foot_size = 1

        local leg_length = self.leg_size
        local r_leg = {vertices[math.floor(#vertices*0.8)],vertices[math.floor(#vertices*0.8)+1],0}

        local y_floor = -6


        gol.draw_line_strip_3d( r_leg[1], r_leg[2], 0,      r_leg[1]+witch, r_leg[2] - leg_length/2, 0,        r_leg[1], y_floor, 0 )

        gol.draw_lines_3d(r_leg[1],y_floor, 0,r_leg[1],  y_floor, foot_size )
        gol.draw_lines_3d(r_leg[1],y_floor, 0,r_leg[1]+1,y_floor, foot_size )
        gol.draw_lines_3d(r_leg[1],y_floor, 0,r_leg[1]-1,y_floor, foot_size )

        --left
        local l_leg = {vertices[math.floor(#vertices*0.75)],vertices[math.floor(#vertices*0.75)+1],0}
        gol.draw_line_strip_3d( l_leg[1],l_leg[2],0,        l_leg[1]-witch,l_leg[2] - leg_length/2,0,          l_leg[1],y_floor,0)

    
        gol.draw_lines_3d(l_leg[1],y_floor, 0,l_leg[1],  y_floor, foot_size )
        gol.draw_lines_3d(l_leg[1],y_floor, 0,l_leg[1]+1,y_floor, foot_size )
        gol.draw_lines_3d(l_leg[1],y_floor, 0,l_leg[1]-1,y_floor, foot_size )
    end


    local mov


    if self.b_pump then mov = 0.5*math.sin(aaa.time.t)+0.5 else mov = 0 end
    local x,y,z = 0, 0, 0.1
    draw_shadok(x, y+mov, 4)

    draw_hand(x-1,y-1+mov,z,0)
  
    draw_hand(x+1,y-1+mov,z,1)
    

    draw_leg(x,y,z,mov)







    

    


end
