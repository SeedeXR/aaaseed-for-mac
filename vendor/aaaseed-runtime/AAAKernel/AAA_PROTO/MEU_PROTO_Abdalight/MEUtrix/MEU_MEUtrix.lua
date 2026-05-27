function meu:define_meu_infos()
    return {
        author = "Abdalight",
        date   = "2025",
        tags   = { "2D", "VJ", "Procedural" },
        help   = "Falling code Matrix-like using a symbol atlas (grid)."
    }
end

function meu:define_ui()
    local ix, iy, SY = 1, 1, 1
    local ui = self.ui
    local bu

    self:add_rendering()
    self:add_camera()

    self:add_trig_method(               {ix,iy,         4,SY},      "RESTART",          self,       "restart_command"                                   )
    iy = iy + SY

    ui.color = self:add_rgba(           {ix,iy,         8,SY},      "Color",            false                                                           )
    iy = iy + SY

    self:add_slider(                    {ix,iy,         4,SY},      "Fall Speed",       self,       "fall_speed",           0.10,           0.01,1.00   )
    bu = self:add_slider(               {ix+4,iy,       4,SY},      "Density",          self,       "density",                20,              1,1000   )
        bu:set_value_type_integer(true)
    iy = iy + SY

    bu = self:add_slider(               {ix,iy,         4,SY},      "Columns",          self,       "frame_width",           120,             10,400    )
        bu:set_value_type_integer(true)
    bu = self:add_slider(               {ix+4,iy,       4,SY},      "Height",           self,       "frame_height",           60,             10,400    )
        bu:set_value_type_integer(true)
    iy = iy + SY

    self:add_slider(                    {ix,iy,         4,SY},      "Char Spacing",     self,       "x_spacing",            0.90,           0.25,3.00   )
    iy = iy + SY

    bu = self:add_button(               {ix,iy,       8/3,SY},      "Use Tex",          self,       "b_use_tex",            false                       )

    ix, iy = 9, 2

    bu = self:add_bu_texture_target_unit(   {ix,iy,          8,5},      "Matrix_Tex",           1,       true   )
        ui.bu_matrix = bu
    iy = iy + 5*SY

    bu = self:add_bu_texture(               {ix,iy,          8,5},		"Deformer_Tex",         8,       true   )
	    ui.bu_tex_def = bu
    iy = iy + 5*SY

    bu = self:add_transfo(                  {ix,iy,    4,2.4*SY},  1 )
    iy = iy + 2.4 * SY
end

--=== INIT ===--
function meu:init()
    -- paramètres “moteur”
    self.frame_width  = self.frame_width  or 120
    self.frame_height = self.frame_height or 60
    self.fall_speed   = self.fall_speed   or 0.10
    self.density      = self.density      or 20
    self.x_spacing    = self.x_spacing    or 0.90

    self.atlas_cols   = 8
    self.atlas_rows   = 12

    self.c_map      = nil
    self.rain       = nil
    self.symbol_map = nil

    self:reset_animation()
    local matrix_grid_path = aaa.dir.get_dir_start().."/AAAKernel/AAA_PROTO/MEU_PROTO_Abdalight/MEUtrix/matrix_grid_12x8.tga"
    self.matrix_bind = IMGS.get_bind( matrix_grid_path )
end



local function clamp01(x) return (x < 0 and 0) or (x > 1 and 1) or x end

-- crée une nouvelle colonne (longueur aléatoire <= density)
function meu:_make_column()
    local len = math.max(1, math.random(1, self.density))
    -- on ne se sert pas du contenu, seule la longueur compte (#column)
    return ("0"):rep(len)
end

-- map (x,i) -> { col,row,ttl }
function meu:init_symbol_map()
    self.symbol_map = {}
    local cols = self.atlas_cols
    local rows = self.atlas_rows
    for x = 1, self.frame_width do
        self.symbol_map[x] = {}
        for i = 1, self.frame_height do
            self.symbol_map[x][i] = {
                col = math.random(0, cols-1),
                row = math.random(0, rows-1),
                ttl = math.random(20, 80)   -- durée de vie d’un symbole (frames)
            }
        end
    end
end

--=== SIMU ===--
function meu:reset_animation()
    aaa.print_debug("Matrix reset...")
    self:init_symbol_map()
    self.c_map = {}
    self.rain  = {}

    for x = 1, self.frame_width do
        self.c_map[x] = self:_make_column()
        -- position de la tête + vitesse individuelle
        self.rain[x]  = { pos = math.random(1, self.frame_height), speed = math.random(1, 5) }
    end
    
end

function meu:restart_command()
    self:reset_animation()
end

function meu:update()
    local ui = self.ui
	local ref = self.ref
	local bu_tex = ui.bu_tex_def
	if bu_tex then
		local b_changed, bind_2d = bu_tex:get_bind_2d_asked()
		if b_changed then
			self.bind_2d_used = bind_2d 
		end
	end

    self.ui.bu_matrix:set_bind_2d( self.matrix_bind )

    if type(self.rain) ~= "table" or type(self.c_map) ~= "table" then
        self:reset_animation()
        return
    end

    for x, drop in ipairs(self.rain) do
        if drop and drop.pos and drop.speed then
            drop.pos = drop.pos - self.fall_speed * drop.speed
            if drop.pos < 1 then
                drop.pos   = self.frame_height
                self.c_map[x] = self:_make_column() -- nouvelle longueur pour varier la traînée
            end
        end
    end
end



function meu:matrix_draw()
    -- sécu : si l’utilisateur change les sliders à chaud
    if (not self.symbol_map) or (#self.symbol_map ~= self.frame_width) then
        self:init_symbol_map()
    end

    local r, g, b, a = self.ui.color:get_rgba()
    local cols, rows = self.atlas_cols, self.atlas_rows
    local start_x    = - self.frame_width * self.x_spacing * 0.5
    local start_y    = - self.frame_height * 0.5
    local z_decal    = 0

    for x = 1, self.frame_width do
        local column = self.c_map[x]
        local drop   = self.rain[x]
        --z_decal = math.random(0, 0.1)
    
        if drop and column then
            local draw_x = start_x + (x - 1) * self.x_spacing
            local draw_y = start_y + drop.pos
            local col_len = #column

            for i = 1, col_len do
                local char_y = draw_y + i
                if char_y >= start_y and char_y <= (start_y + self.frame_height) then

                    -- symbole (lent) depuis la cache
                    local row_cache = self.symbol_map[x]
                    if not row_cache then
                        self.symbol_map[x] = {}
                        row_cache = self.symbol_map[x]
                    end
                    local cell = row_cache[i]
                    if not cell then
                        cell = { col = math.random(0, cols-1), row = math.random(0, rows-1), ttl = math.random(20, 80) }
                        row_cache[i] = cell
                    end

                    cell.ttl = cell.ttl - 1
                    if cell.ttl <= 0 then
                        cell.col = math.random(0, cols-1)
                        cell.row = math.random(0, rows-1)
                        cell.ttl = math.random(20, 80)
                    end

                    -- UV pour la cellule (col,row) dans une grille cols x rows
                    local u0 = cell.col / cols
                    local v0 = cell.row / rows
                    local u1 = (cell.col + 1) / cols
                    local v1 = (cell.row + 1) / rows
                    gol.set_quad_uv(u0, v0, u1, v1)

                    -- couleur de base Matrix (tête blanche + atténuation progressive)
                    local base_r, base_g, base_b, base_a
                    if i == 1 then
                        base_r, base_g, base_b, base_a = 1, 1, 1, 1
                    else
                        local fade = 1.0 - (i / col_len)
                        local jitter = 0.9 + math.random() * 0.1
                        local aa = clamp01(a * fade * jitter)
                        base_r, base_g, base_b, base_a = r, g, b, aa
                    end

                    -- coordonnées UV normalisées (en fonction de la frame)
                    local u_mid = clamp01( (draw_x - start_x) / self.frame_width  )
                    local v_mid = clamp01( (char_y - start_y) / self.frame_height )

                    -- couleur vidéo
                    local r_col, g_col, b_col, a_col = aaa.img.get_color_uv(self.bind_2d_used, u_mid, v_mid, false)

                    if not r_col then
                        r_col, g_col, b_col, a_col = 1, 1, 1, 1
                    end

                    gol.color4(base_r, base_g, base_b, base_a)
                    if self.b_use_tex then
                        -- mix : couleur vidéo * couleur Matrix
                        local final_r = r_col * base_r
                        local final_g = g_col * base_g
                        local final_b = b_col * base_b
                        local final_a = a_col * base_a
                        
                        gol.color4(final_r, final_g, final_b, final_a)
                    end        

                    aaa.draw_rect_uv_axe_z(draw_x, char_y, z_decal,1,1)
                end
            end
        end
    end
end

function meu:draw()
    self:draw_layers_begin()
        self:draw_layer(1)
        self:matrix_draw()
    self:draw_layers_end()
end
