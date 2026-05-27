function meu:define_meu_infos()
    return {
        author = "Abdalight",
        date = "2025",
        tags = { "Tutorial", "Texture" },
        help = { "Using a BU_COLOR to change the color of a graphical element." }
    }
end

function meu:define_ui()
    local ref = self.ref
    local ui = self.ui
    local ix, iy = 1, 1
    local SY, DY = 1, 0.2
    local bu

    self:add_rendering()
    self:add_camera()

    bu = self:add_rgbf(     {ix,iy,             8,2*SY},      "Color1"                )
        ui.color_rgbf = bu
    iy = iy + 2*SY + DY

    bu = self:add_rgbfa(    {ix,iy,             8,2*SY},      "Color2",       false   )
        ui.color_rgbfa = bu
    iy = iy + 2*SY + DY

    --[[ 
        Additional possible functions to add color selection:
        
        -- Basic RGB selector (integer values)
        bu = self:add_rgb({ix, iy, 8, 2 * SY}, "Basic RGB")
        ui.color_rgb = bu
        iy = iy + 2 * SY + DY

        -- RGB with Alpha (integer values)
        bu = self:add_rgba({ix, iy, 8, 2 * SY}, "Basic RGBA")
        ui.color_rgba = bu
        iy = iy + 2 * SY + DY

        -- Only RGB without any transformations
        bu = self:add_rgb_only({ix, iy, 8, 2 * SY}, "RGB Only")
        ui.color_rgb_only = bu
        iy = iy + 2 * SY + DY

        -- RGBF but without transformation options
        bu = self:add_rgbf_only({ix, iy, 8, 2 * SY}, "RGBF Only")
        ui.color_rgbf_only = bu
        iy = iy + 2 * SY + DY

        -- RGBA but without transformation options
        bu = self:add_rgba_only({ix, iy, 8, 2 * SY}, "RGBA Only")
        ui.color_rgba_only = bu
        iy = iy + 2 * SY + DY

        -- RGBAF without transformation options
        bu = self:add_rgbfa_only({ix, iy, 8, 2 * SY}, "RGBAF Only")
        ui.color_rgbfa_only = bu
        iy = iy + 2 * SY + DY
    ]]
end


function meu:draw()
    local r, g, b, a = self.ui.color_rgbfa:get_rgba()

    self:draw_layers_begin()
        self:draw_layer( 1 )
        aaa.draw_disk_axe_z(-1.5,0,0, 3, 50)
        gol.color(r, g, b, a)
        aaa.draw_disk_axe_z(1.5,0,0, 3, 50)
    self:draw_layers_end()
end
