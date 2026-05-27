function meu:define_meu_infos( )
	return { author = "Abdalight Mâa", date="2024",
			tags = { "Core", "Tutorial", "3d", "draw" },
			help = { "Show creation of one class derived from GABU_OBJ.",
                     " look at TutoLuaClassDerived for a more complex example after."
                    }
			}
end

--------------------------------------------------------------------------------
-- this is a very simple class example where BOX is a class derived from GABU_OBJ
-- at the bottom level AAASeed use Loop https://renatomaia.github.io/loop/index.html using the simple version for now (2024 November)"
if CLASS.DECLARE( "BOX", nil ) then
    -- this is executed only at class creation because CLASS.DECLARE returns true when the CLASS is declared (first time it is called)
    -- and define a minimal documentation
    BOX:set_class_status_doc(
        CLASS.STATUS.TUTORIAL,
        "BOX class example",
        "  for 3d box data and drawing functions.",
        "For lua object AAASeed use Loop https://renatomaia.github.io/loop/index.html",
        "  using the simple version for now (2025 November)."
    )
end

function BOX.create_instance( class, name, id )
    local self = oo.getsuper(BOX).create_instance( class, name ) -- create a new instance of the class using GABU_OBJ (the super)
    self.box_id = id
    self.size   = { 1, 1, 1 }    -- Default size (width, height, depth)
    self.center = { 0, 0, 0 }    -- Default position of the center
    self.color  = { 1, 1, 1, 1 } -- Default color (RGBA: white)
	return self
end

function BOX:create( id )
    self = BOX:create_instance( "Box_"..id, id )
    return self
end
function BOX:get_id()
   return self.box_id
end

function BOX:set_size( width, height, depth )
    local size = self.size
    if width  then size[1] = width  end
    if height then size[2] = height end
    if depth  then size[3] = depth  end
end
function BOX:get_size()
    local size = self.size
    return size[1], size[2], size[3]
end

function BOX:set_center( x, y, z )
    local center = self.center
    if x then center[1] = x end
    if y then center[2] = y end
    if z then center[3] = z end
end
function BOX:get_center()
    local center = self.center
    return center[1], center[2], center[3]
end

function BOX:set_color( r, g, b, a )
    local color = self.color
    if r then color[1] = r end
    if g then color[2] = g end
    if b then color[3] = b end
    if a then color[4] = a end
end
function BOX:get_color()
    local color = self.color
    return color[1],color[2],color[3], color[4]
end

function BOX:draw( ox,oy,oz )
    --no arg -> default to 0
    ox = ox or 0
    oy = oy or 0
    oz = oz or 0
    local c = self.color
    gol.color( c[1],c[2],c[3], c[4] )
    c = self.center
    local s = self.size
    aaa.draw_box( s[1],s[2],s[3], c[1]+ox,c[2]+oy,c[3]+oz )
end


function meu:define_ui()
	local ui      = self.ui
    local ix,iy   = 1,   1
    local SX, SY  = 4/3, 1
    local DY      = .2
    local bu

    self:add_rendering()
	self:add_camera()

    bu = self:add_button( {ix,iy,       4,SY},  "Random",  self, "b_box_random",     false   )
    iy = iy + SY + DY
    
    bu = self:add_button( {ix,iy,       4,SY},  "Animate",  self, "b_box_anim",     false   )
    iy = iy + SY + DY
  
    if false then   -- ui to edit box but need more work
        bu = self:add_button( {ix,iy,       4,SY},  "Box Edit",     self, "b_edit",         false   )
        bu = self:add_slider( {ix+4,iy,     4,SY},  "Box Select",   self, "box_selected",   1,      1,15)
            bu:set_value_type_integer( true )
        iy = iy + SY + DY


        iy = iy + SY + DY
        ui.color = self:add_rgbf( {ix,iy,  8*SY,SY} )
        iy = iy + SY + DY

        bu = self:add_slider( {ix,iy,      2*SX,SY},    "X",        self,   "center_x",   0,    -5,5 )
            bu:set_color_back( "x" )
        bu = self:add_slider( {ix+2*SX,iy, 2*SX,SY},    "Y",        self,   "center_y",   0,    -5,5 )
            bu:set_color_back( "y" )
        bu = self:add_slider( {ix+4*SX,iy, 2*SX,SY},    "Z",        self,   "center_z",   0,    -5,5 )
            bu:set_color_back( "z" )
        iy = iy + SY + DY

        bu = self:add_slider( {ix,iy,      2*SX,SY},    "Width",    self,   "width",      1,     0,5 )
            bu:set_color_back( "x" )
        bu = self:add_slider( {ix+2*SX,iy, 2*SX,SY},    "Height",   self,   "height",     1,     0,5 )
            bu:set_color_back( "y" )
        bu = self:add_slider( {ix+4*SX,iy, 2*SX,SY},    "Depth",    self,   "depth",      1,     0,5 )
            bu:set_color_back( "z" )
        iy = iy + SY + DY
    end


    ix, iy = 9, 2 + DY
    self:add_transfo( {ix,iy,   8,2.4} )
    iy = iy + 2.4 + DY
end

function meu:init()
   self:generate_random_boxes()
end

function meu:generate_random_boxes()
    math.randomseed( self:get_render_index() )
    self.boxes = {} -- Réinitialiser les boîtes
    for i = 1, 20 do
        local box = BOX:create(i)
        -- Attribuer des propriétés aléatoires
        box:set_size(math.random(), math.random(), math.random())
        box:set_center(math.random(-3, 3), math.random(-3, 3), math.random(-3, 3))
        box:set_color(math.random(), math.random(), math.random(), 1)
        self.boxes[i] = box
    end
end

function meu:update()
    if self.b_box_random then
        self:generate_random_boxes()
    end

end

function meu:draw()  
    local ph = (self.ph or 0) + aaa.time.dt
    self.ph = ph
    self:draw_layers_begin()
        self:draw_layer(1)
        if self.b_box_anim then
            for _, box in ipairs(self.boxes) do
                local id = box:get_id()
                box:draw( math.sin(ph+id ), math.sin(ph+2*id ), math.sin(ph+3*id ) )
            end
        else
            table.apply_method( self.boxes, "draw" )
        end
    self:draw_layers_end()
end
