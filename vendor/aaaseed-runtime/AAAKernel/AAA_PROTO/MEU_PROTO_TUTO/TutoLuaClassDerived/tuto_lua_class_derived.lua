function meu:define_meu_infos( )
	return { author = "Abdalight Mâa", date="2024",
			tags = { "Core", "Tutorial", "3d", "draw" },
			help = { "Show creation of one class derived from an other class.",
                        " look at TutoLuaClass first" }
			}
end

if CLASS.DECLARE( "PRIM", nil ) then
    PRIM:set_class_status_doc(
        CLASS.STATUS.TUTORIAL,
        "PRIM class example",
        "  PRIM_BOX and PRIM_SPHERE will be derived from PRIM,",
        "  PRIM is the super of PRIM_BOX and PRIM_SPHERE",
        "manage 3d data functions for all primitive",
        "For lua object AAASeed use Loop https://renatomaia.github.io/loop/index.html",
        "  using the simple version for now (2025 November)."    )
end

function PRIM.create_instance( class, name, id )
    local self = oo.getsuper(PRIM).create_instance( class, name ) -- create a new instance of the class using GABU_OBJ (the super)
    self.prim_id = id
    self.size   = { 1, 1, 1 }    -- Default size (width, height, depth)
    self.center = { 0, 0, 0 }    -- Default position of the center
    self.color  = { 1, 1, 1, 1 } -- Default color (RGBA: white)
	return self
end

function PRIM:create( id )
    self = PRIM:create_instance( "Prim_"..id, id )
    return self
end

function PRIM:get_id()
   return self.prim_id
end

function PRIM:set_size( width, height, depth )
    local size = self.size
    if width  then size[1] = width  end
    if height then size[2] = height end
    if depth  then size[3] = depth  end
end
function PRIM:get_size()
    local size = self.size
    return size[1], size[2], size[3]
end

function PRIM:set_center( x, y, z )
    local center = self.center
    if x then center[1] = x end
    if y then center[2] = y end
    if z then center[3] = z end
end
function PRIM:get_center()
    local center = self.center
    return center[1], center[2], center[3]
end

function PRIM:set_color( r, g, b, a )
    local color = self.color
    if r then color[1] = r end
    if g then color[2] = g end
    if b then color[3] = b end
    if a then color[4] = a end
end
function PRIM:get_color()
    local color = self.color
    return color[1],color[2],color[3], color[4]
end
function PRIM:draw()
    self:print( "PRIM base class have a non functionaal draw method" ) 
end

if CLASS.DECLARE( "PRIM_BOX", PRIM ) then
    PRIM_BOX:set_class_status_doc(
        CLASS.STATUS.TUTORIAL,
        "PRIM_BOX class example using PRIM as super",
        "we just need the draw function"
    )
end

function PRIM_BOX.create_instance( class, name, id )
    return oo.getsuper(PRIM_BOX).create_instance( class, name, id ) -- create a new instance of the class using GABU_OBJ (the super)
end

function PRIM_BOX:create( id )
    self = PRIM_BOX:create_instance( "Box_"..id, id )
    return self
end

function PRIM_BOX:draw( ox,oy,oz )
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

if CLASS.DECLARE( "PRIM_SPHERE", PRIM ) then
    -- this is executed only at class creation because CLASS.DECLARE returns true when the CLASS is declared (first time it is called)
    -- and define a minimal documentation
    PRIM_SPHERE:set_class_status_doc(
        CLASS.STATUS.TUTORIAL,
        "PRIM_SPHERE class example using PRIM as super",
        "we just need the draw function"
    )
end


function PRIM_SPHERE.create_instance( class, name, id )
    return oo.getsuper(PRIM_SPHERE).create_instance( class, name, id ) -- create a new instance of the class using GABU_OBJ (the super)
end

function PRIM_SPHERE:create( id )
    self = PRIM_SPHERE:create_instance( "Sphere_"..id, id )
    return self
end


function PRIM_SPHERE:draw( ox,oy,oz )
    --no arg -> default to 0
    ox = ox or 0
    oy = oy or 0
    oz = oz or 0
    local c = self.color
    gol.color( c[1],c[2],c[3], c[4] )
    c = self.center
    local s = self.size
    local radius = math.sqrt( s[1] * s[1] + s[2] * s[2] + s[3] * s[3] )
    aaa.draw_sphere_xyz( c[1]+ox,c[2]+oy,c[3]+oz,	radius,	100, 100 )
end


function meu:define_ui()
	local ui      = self.ui
    local ix,iy   = 1,   1
    local SX, SY  = 4/3, 1
    local DY      = .2
    local bu

    self:add_rendering()
	self:add_camera()

    bu = self:add_button( {ix,iy,       4,SY},  "Random",   self,   "b_box_random", false   )
    iy = iy + SY + DY
    
    bu = self:add_button( {ix,iy,       4,SY},  "Animate",  self,   "b_box_anim",   false   )
    iy = iy + SY + DY
  
    ix, iy = 9, 2 + DY
    self:add_transfo( {ix,iy,   8,2.4} )
    iy = iy + 2.4 + DY
end

function meu:init()
   self:generate_random_boxes()
end

function meu:generate_random_boxes()
    math.randomseed( self:get_render_index() )
    self.prims = {} -- Réinitialiser les boîtes
    for i = 1, 20 do
        local prim = ( math.random() < 0.5 ) and PRIM_BOX:create(i) or PRIM_SPHERE:create(i)
        -- Attribuer des propriétés aléatoires
        prim:set_size(math.random(), math.random(), math.random())
        prim:set_center(math.random(-3, 3), math.random(-3, 3), math.random(-3, 3))
        prim:set_color(math.random(), math.random(), math.random(), 1)
        self.prims[i] = prim
    end
end

function meu:update()
    if self.b_box_random then
        self:generate_random_boxes()
    end
end

function meu:draw()
    local ph = ( self.ph or 0 ) + aaa.time.dt
    self.ph = ph
    self:draw_layers_begin()
        self:draw_layer(1)
        if self.b_box_anim then
            for _, prim in ipairs(self.prims) do
                local id = prim:get_id()
                prim:draw( math.sin( ph + id ), math.sin( ph + 2*id ), math.sin( ph + 3*id ) )
            end
        else
            table.apply_method( self.prims, "draw" )
        end
    self:draw_layers_end()
end
