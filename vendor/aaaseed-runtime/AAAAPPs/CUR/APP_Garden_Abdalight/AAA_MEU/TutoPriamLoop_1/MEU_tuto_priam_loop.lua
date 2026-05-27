function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
    local ix,iy = 1,1
    local SY,DY = 1,.2

    self:add_rendering()    --todo why we get remdering from layer_c ???
	self:add_camera()

    local bu = self:add_slider( {ix,iy, 8,SY}, "Nb", self, "number", 10,  0,1000 )
        bu:set_value_type_integer( true )
    iy = iy + SY

end

function meu:draw()
    self:draw_layers_begin()
        --  self:print( aaa.time.t )
        self:draw_layer( 1 )
        self:draw_priam_loop_demo_pure_bu( self.number )
    self:draw_layers_end()
end


function meu:draw_priam_loop_demo_pure_bu( nb )
    local size = 8

    local name = "Invaders_1"
    local m = self:get_meu_by_name_cached( name )
    if not m then
        self:print_error( "No MEU "..name )
    end

    local bu_nb = m:get_bu_by_key( "nb_u"           )
    local bu_tx = m:get_bu_by_key( "transfo1.tra_x" )
    local bu_ty = m:get_bu_by_key( "transfo1.tra_y" )
    local bu_tz = m:get_bu_by_key( "transfo1.tra_z" )
    local bu_ry = m:get_bu_by_key( "transfo1.rot_y" )

    for i=1,nb do
        math.randomseed( 42 + i )

        local rot = math.random() * math.pi2
        local sin = math.sin( rot )
		local cos = math.cos( rot )

        local r = size * math.random() * .5
        local x = r * cos
        local z = r * sin

        local y = size * (math.random() - .5)
        y = wrap( y - aaa.time.t*3, -size/2, size/2 )
    
        -- set the nb of square in the invader
        bu_nb:set_value( 5 + math.random(6) )
        -- set the invader position (we could also use the gol transfos)
        bu_tx:set_value( x )
        bu_ty:set_value( y )
        bu_tz:set_value( z )

        bu_ry:set_value( aaa.time.t * .4 + i*.1174)
        local alpha = .5 + math.random()*.5                 -- diferrent alpha for each one
        alpha = alpha * clamp_01( size/2 - math.abs(y) )    -- fade at top and bottom
        -- call an invader fn to change each one
        m:set_seed( i )
        -- draw one invader
        m:render(alpha)
    end

end
