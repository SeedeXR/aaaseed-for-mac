function meu:define_meu_infos()
    return {
        author = "Abdalight",
        date = "2025",
        tags = { "Tutorial", "Texture" },
        help = { "Using a texture and a color overlay in an AAASeed MEU." }
    }
end

function meu:define_ui()
    local ref = self.ref
    local ui = self.ui
    local ix, iy = 1, 1
    local SY, DY = 1, 0.2
    local bu

    self:add_camera()  -- Adds a camera to handle perspective
    self:add_rendering()  -- Adds rendering parameters

    -- Adding a texture selector

    bu = self:add_bu_texture_target_unit(   {ix,iy,             8,6*SY}, "Texture", 1, true )
    iy = iy + 6 * SY + DY

    -- Adding a color overlay selector
    bu = self:add_rgbfa(                    {ix,iy,             8,2*SY}, "Color Overlay", false )
        ui.color_overlay = bu
    iy = iy + 2 * SY + DY
end

function meu:init()
    local ref = self.ref
    -- Initialize position and transformation values
    self.pos = { x = 0, y = 0, z = 0 }
    self.size = { width = 4, height = 4 }
end

function meu:draw()
    local tex_bind = self:get_texture_bind_2d() -- Get the texture ID
    local r, g, b, a = self.ui.color_overlay:get_rgba() -- Get overlay color

    self:draw_layers_begin()
        self:draw_layer(1) -- Draw the first layer

        if tex_bind then
            gol.color(r, g, b, a)  -- Apply overlay color
            aaa.draw_bind_rect(tex_bind, 
                -self.size.width / 2, -self.size.height / 2, 
                self.size.width / 2, self.size.height / 2) -- Draw the texture
        end
    self:draw_layers_end()
end
