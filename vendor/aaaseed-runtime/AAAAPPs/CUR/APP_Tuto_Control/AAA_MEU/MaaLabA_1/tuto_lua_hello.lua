
function meu:define_meu_infos( )
	return { author = "Mâa", date = "2024",
			tags = { "Core", "Tutorial", "3d", "2d", "draw" },
			help = "A meu toi test self proto and have some Fun for Maa"
			 }
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
    local ix,iy = 1,1
    local SY,DY = 1,.2

    self:add_rendering()    --todo why we get remdering from layer_c ???
	self:add_camera()

    self:add_slider(  {ix,iy,   8,SY}, "Repeat",    self,"f_repeat",  3, 0,16 )
    iy = iy + SY
    self:add_slider(  {ix,iy,   8,SY}, "Freq",      self,"freq",    3, 0,16 )

    ix, iy = 9, 2 + DY
    self:add_transfo( {ix,iy,   8,2.4} )
    iy = iy + 2.4 + DY

end

function meu:init()
    self.ph = 0
end
function meu:draw_1()
    local ph = self.ph + aaa.time.dt * self.freq
    self.ph = ph
    local n = 128 * 4
    for i = 0, n-1 do
        local a = i/n
        --local c = math.cos(a)
        --local s = math.sin(a)
        gol.push_matrix()
            gol.rotate_z(a)
            gol.translate_x( 1 + math.cos( a*math.pi2 * self.f_repeat + ph ) * .5 )
            gol.rotate_z( ph * .2)
            aaa.draw_rect_size( .2, .01 )
        gol.pop_matrix()
    end
end

function meu:draw()
    self:draw_layers_begin()
        --  self:print( aaa.time.t )
        self:draw_layer( 1 )
        self:draw_1( self:get_alpha(), 1, false )       -- choose the drawing function here
    self:draw_layers_end()
end

