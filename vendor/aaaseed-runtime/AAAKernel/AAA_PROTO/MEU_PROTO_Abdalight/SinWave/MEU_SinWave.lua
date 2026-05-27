function meu:define_meu_infos()
    return { author = "Abdalight", date = "2025",
               tags = { "2d", "3d", "Art", "draw", "procedural", "unfinished", "vj" },
               help = "Generates a wave effect with a rotating central sphere inspired by Yann Le Gall work."
    }
end

function meu:define_ui()
    local ref = self.ref
    local ui  = self.ui
    local ix, iy = 1, 1
    local SY, DY = 1, 0.2
    local bu

    self:add_rendering()
    self:add_camera()

    self:add_bu_texture_target_unit( {ix,iy,		8,4},    "Tex",                 2,   false	)
    iy = iy + 4 * SY + DY

    bu = self:add_button(            {ix,iy,     8/3,SY},    "Color Tex",        self,   "b_tex_color",  false               )
    iy = iy + SY + DY

    --self:begin_bu_group("Wave Properties")
    bu = self:add_slider(            {ix,iy,       4,SY},    "Size X",           self,   "size_x",        16,         1,200 )
        bu:set_color_back( "x" )
        bu:set_value_type_integer(true)
    bu = self:add_slider(            {ix+4,iy,     4,SY},    "Size Y",           self,   "size_y",        9,         1,200 )
        bu:set_color_back( "y" )
        bu:set_value_type_integer(true)
    iy = iy + SY

    bu = self:add_slider(            {ix,iy,       8,SY},    "Amplitude",        self,   "amplitude",      1,          0,5   )
    iy = iy + SY

    bu = self:add_slider(            {ix,iy,       4,SY},    "Frequency Inside",   self,   "freq_in",     1,        0.1,100 )
    bu = self:add_slider(            {ix+4,iy,     4,SY},    "Frequency Outside",   self,   "freq_out",     5,        0.1,100 )
    iy = iy + SY

    bu = self:add_slider(            {ix,iy,       8,SY},    "Wave Speed",       self,   "wave_speed",   0.5,          0,2   )
    iy = iy + SY

    bu = self:add_button(            {ix,iy,       4,SY},    "Animate",          self,   "animate",      false               )
    iy = iy + SY
    --self:end_bu_group()



    ix, iy = 9, 2
    self:add_transfo(                {ix,iy,      4,2.4},   1   )
    iy = iy + 2.4 + DY
	
	ui.color_in = self:add_rgbf(       {ix,iy,       8,1.5},    "Color Inside",	false        ) --TODO color inside outside
    iy = iy + 1.5 + DY

    ui.color_out = self:add_rgbf(   {ix+1,iy,     7,1.5},    "Color Outside",	false )
	iy = iy + 1.5 + DY

    bu = self:add_selector(	{ix,iy,		8,SY},	"Input" )
		bu:set_item_text( 1, "Square", "Circle", "Tex"  )
		bu:set_target_lua( self, "s_signal" )
		bu:set_text_draw( false )
	iy = iy + SY


end

function meu:init()
    self.size_x = 16
    self.size_y = 9
    self.amplitude = 1
    self.freq_in = 1
    self.freq_out = 5
    self.wave_speed = 0.5
    self.animate = false
    self.time = 0
    self.radius = 1
    self.waves = {}
end

function meu:draw()
    self:draw_layers_begin()
        self:draw_layer(1)
        self:draw_waves()
    self:draw_layers_end()
end


function meu:update()
    self.bind = self:get_texture_bind_2d( 2 )
    
    if self.animate then
        self.time = self.time + aaa.time.dt * self.wave_speed
    end
end

function meu:compute_waves()
    local half_size_x = self.size_x * .5
    local half_size_y = self.size_y * .5
    local s_ampl = 1
    local s_freq = 1
    local x_step = 0.005
    local y_step = 0.5
    local wave_y = 1
    local dx = - half_size_x
    local waves = {}
    local id = 1

    for y = - half_size_y, half_size_y, y_step do
        for x = - half_size_x, half_size_x, x_step do
            dx = dx + x_step
            s_freq, s_ampl = self:get_frequency_and_amplitude( x, y )
            wave_y = s_ampl * math.sin( dx * s_freq + self.time )
            waves[id], waves[id + 1], waves[id + 2] = x, y + wave_y, 0
            id = id + 3
        end
    end

    self.waves = waves
end

function meu:draw_waves()
    local waves = self.waves
    self:compute_waves()
    gol.draw_points_3d( waves )
end

function meu:get_frequency_and_amplitude( x, y )
    local distance = x^2 + y^2

    local radius = 3

    local freq_inside = 20 * self.freq_in
    local freq_outside = self.freq_out

    local amp_inside = 0.25 * 0.5 * self.amplitude
    local amp_outside = 0.5 * 0.5 * self.amplitude

    local freq_mod = ( 1 + math.cos( self.time ) ) * 0.5

    local cos_a = math.cos( 0.5 * self.time )
    local sin_a = math.sin( 0.5 * self.time )

    local shadow_wave = 9 * freq_mod

    local xr = x * cos_a + y * sin_a
    local yr = x * sin_a - y * cos_a

    local red_in, green_in, blue_in, alpha_in = self.ui.color_in:get_rgba()
	local red_out, green_out, blue_out, alpha_out = self.ui.color_out:get_rgba()
    
    if self.s_signal == 2 then	-- Circle
		if distance <= radius * radius then
            gol.color( red_in, green_in, blue_in, alpha_in )
            return freq_mod * freq_inside, amp_inside
        elseif distance <= (radius + shadow_wave)^2 and xr > 0 and yr > 0 then
            gol.color( red_in * red_out, green_in * green_out, blue_in * blue_out, alpha_in * alpha_out )
            return freq_mod * freq_outside, amp_inside
        else
            gol.color( red_out, green_out, blue_out, alpha_out )
            return freq_outside, amp_outside
        end
	elseif self.s_signal == 1 then	-- Square
		if x^2 <= radius * radius and y^2 <= radius * radius then
            gol.color( red_in, green_in, blue_in, alpha_in )
            return  freq_mod * freq_inside, amp_inside
        elseif x^2 <= ( radius + shadow_wave )^2 and y^2 <= ( radius + shadow_wave )^2  and xr > 0 and yr > 0 then
            gol.color( red_in * red_out, green_in * green_out, blue_in * blue_out, alpha_in * alpha_out )
            return freq_mod * freq_outside, amp_inside 
        else
            gol.color( red_out, green_out, blue_out, alpha_out )
            return freq_outside, amp_outside
        end
	elseif self.bind then	-- Texture
		local r,g,b,a = aaa.img.get_color_xy( self.bind, x*30+250,y*30+100 )
        local brightness = (r + g + b) / 3

        if brightness > 0.9 then
            if self.b_tex_color then gol.color( r*red_in, g*green_in, b*blue_in, a*alpha_in )       end
            return  freq_mod * freq_inside, amp_inside  
        else
            if self.b_tex_color then gol.color( r*red_out, g*green_out, b*blue_out, a*alpha_out )   end
            return freq_outside, amp_outside
        end
	end
end


-------------------------------------------------------------------------

function meu:draw_waves2()
    local half_grid_size = self.grid_size / 2
    local time = self.time
    local x_step = 0.01  -- Adjust step size for smoother curves
    local y_step = 0.1
    local wave_height = 1  -- Base amplitude

    for y = -half_grid_size, half_grid_size, y_step do
        for x = -half_grid_size, half_grid_size, x_step do
            -- Get smooth frequency and amplitude based on position
            local freq, amp = self:get_frequency_and_amplitude(x, y, 3, 10 * self.base_freq, self.base_freq, 1, 1)
            
            -- Improve continuity by considering y in the wave function
            local phase = (x * 0.1 + y * 0.05) * freq + time
            
            -- Apply a damping effect over distance to smooth out transitions
            local distance_factor = math.exp(-math.abs(x + y) * 0.1)
            
            -- Compute the final wave height with a sine function
            local wave = amp * wave_height * distance_factor * math.sin(phase)
          
            -- Draw the point with smooth continuity
            gol.draw_points_3d(x, y + wave, 0)
        end
    end
end

function meu:get_frequency_and_amplitude2(x, y, radius, freq_inside, freq_outside, amp_inside, amp_outside)
    local distance = math.sqrt( x^2 + y^2 )
    local t = math.min( distance / radius, 1 ) 
    local freq =  ( math.cos(0.1 * self.time) ) * freq_inside * ( 1 - t ) + freq_outside * t
    local amp  = ( math.cos(0.1 * self.time)  ) * amp_inside  * ( 1 - t ) + amp_outside  * t
    return freq, amp
end


function meu:get_frequency_and_amplitude3(x, y, radius, freq_inside, freq_outside, amp_inside, amp_outside)
    local distance = x^2 + y^2
    local shadow_width = 9  -- largeur de la zone d’ombre autour du rayon

    -- Rotation autour du centre
    local angle = self.time  -- contrôle la rotation
    local cos_a = math.cos(angle)
    local sin_a = math.sin(angle)
    local xr = x * cos_a + y * sin_a
    local yr = x * sin_a - y * cos_a

    if distance <= radius * radius then
        return freq_inside, amp_inside

    elseif distance <= (radius + shadow_width)^2 and xr > 0 and yr > 0 then
        -- Appliquer un dégradé basé sur la distance dans la zone d’ombre
        local t = (distance - radius^2) / ((radius + shadow_width)^2 - radius^2)
        t = math.min(math.max(t, 0), 1)

        -- Ajoute une modulation sinusoïdale subtile dans la zone d’ombre
        local shadow_freq = ((1 + math.sin(self.time)) * 0.5 + 0.5) * freq_inside
        local freq = shadow_freq * (1 - t) + freq_outside * t
        local amp = amp_inside * (1 - t) + amp_outside * t
        return freq, amp

    else
        return freq_outside, amp_outside
    end
end
