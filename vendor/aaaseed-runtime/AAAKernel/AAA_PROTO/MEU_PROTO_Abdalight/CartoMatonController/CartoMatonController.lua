function meu:define_meu_infos()
    return {
        author = "Abdalight",
        date   = "2025",
        tags   = { "3D", "Art", "Draw" },
        help   = { "MEU for controlling MEU CartoMaton." }
    }
end

function meu:define_ui()
    local ref = self.ref
    local ui = self.ui
    local ix, iy = 1, 1
    local SY, DY = 1, 0.2
    local bu

    local function add_text( rect, name )
		local bu =  self:add_text( rect, name )
		bu:set_value_load_save(true)
		return bu
	end

    self:add_camera()
    self:add_rendering()

    bu = self:add_rgbfa(                    {ix,iy,             8,SY},    "Color",          false )
        ui.color = bu
    iy = iy + SY + DY

    bu = self:add_button(			        {ix,iy,		        8/3,SY},    "Top",			self,		"b_top",		false   )
    bu = self:add_button(			        {ix+8/3,iy,		    8/3,SY},    "Mid",			self,		"b_mid",		false   )
    bu = self:add_button(			        {ix+16/3,iy,        8/3,SY},    "Bot",			self,		"b_bot",		false   )
    iy = iy + SY + DY

    bu = self:add_bu_texture_target_unit(   {ix,iy,             4,3*SY},    "Tex L",           1,   true )
        ui.bu_tex_l = bu
    bu = self:add_bu_texture_target_unit(   {ix+4,iy,           4,3*SY},    "Tex R",           2,   true )
        ui.bu_tex_r = bu
    iy = iy + 3 * SY + DY

    bu = add_text(		  {ix,iy,   8,SY},  "Meu Source"   )
		bu:set_text( "ImgAnalMulti_New" )
		bu:set_preset_use( true )
		ui.meu_src = bu
    iy = iy + SY + DY

    bu = add_text(		  {ix,iy,   8,SY},  "Meu Target"   )
		bu:set_text( "CartoMaton_New" )
		bu:set_preset_use( true )
		ui.meu_tgt = bu
    iy = iy + SY + DY

    ix,iy = 9, 2


    self:add_transfo(			{ix,iy,	 nil,2.4},  1 )
	iy = iy + 2.4 * SY + DY

    self:init_param()
end

function meu:init()
    local ref = self.ref

    self.alpha_top_l, self.alpha_top_r = 1, 1
    self.alpha_mid_l, self.alpha_mid_r = 1, 1
    self.alpha_bot_l, self.alpha_bot_r = 1, 1
end

function meu:init_param()
    local ui = self.ui
    local meu_name_tgt = ui.meu_tgt:get_text() or "CartoMaton_New"
    local meu_name_src = ui.meu_src:get_text() or "ImgAnalMulti_New"
    self.target_meu   = self:get_meu_by_name_no_error(  meu_name_tgt )
    if self.target_meu then
        self.speed_top    = self.target_meu:get_bu_by_key(  "Speed Top"          )
        self.speed_mid    = self.target_meu:get_bu_by_key(  "Speed Mid"          )
        self.speed_bot    = self.target_meu:get_bu_by_key(  "Speed Bot"          )
        self.target_top_l = self.target_meu:get_bu_by_key(  "Top L"              )
        self.target_mid_l = self.target_meu:get_bu_by_key(  "Mid L"              )
        self.target_bot_l = self.target_meu:get_bu_by_key(  "Bot L"              )
    end
    self.source_meu   = self:get_meu_by_name_no_error(  meu_name_src )
    if self.source_meu then
        self.cov_1        = self.source_meu:get_bu_by_key(     "Cov 1"              )
        self.u_1          = self.source_meu:get_bu_by_key(     "Center u 1"         )
        self.cov_2        = self.source_meu:get_bu_by_key(     "Cov 2"              )
        self.u_2          = self.source_meu:get_bu_by_key(     "Center u 2"         )
        self.cov_3        = self.source_meu:get_bu_by_key(     "Cov 3"              )
        self.u_3          = self.source_meu:get_bu_by_key(     "Center u 3"         )
    end
end

function meu:update()
    self:init_param()
    self:update_controller()
end

function meu:draw()
    self:draw_layers_begin()
        self:draw_layer(1)
        self:draw_controller()
    self:draw_layers_end()
end

function meu:update_controller()
    local function update_zone( cov, u, target_l, speed, alpha_l, alpha_r, b_flag )
        if cov:get_value() > 0 then
            self[b_flag] = true
            self[target_l]:set_value(true)

            local u_val = self[u]:get_value()
            local u_speed = 2 * u_val - 1
            local range = 0.1
            local alpha = 1

            if math.abs( u_val - 0.5 ) > range then
                self[speed]:set_value( u_speed )
                alpha = clamp_01( u_val )
                self[alpha_l] = alpha
                self[alpha_r] = 1 - alpha
            else
                self[speed]:set_value(0)
                self[target_l]:set_value(false)
                self[alpha_l] = 0.5
                self[alpha_r] = 0.5
            end
        else
                self[b_flag] = false
                self[target_l]:set_value(false)
        end
    end


    update_zone( self.cov_1, "u_1", "target_top_l", "speed_top", "alpha_top_l", "alpha_top_r", "b_top" )
    update_zone( self.cov_2, "u_2", "target_mid_l", "speed_mid", "alpha_mid_l", "alpha_mid_r", "b_mid" )
    update_zone( self.cov_3, "u_3", "target_bot_l", "speed_bot", "alpha_bot_l", "alpha_bot_r", "b_bot" )
end



function meu:draw_controller()
    local ui = self.ui
    local tex_bind_l = ui.bu_tex_l:get_bind_2d()
    local tex_bind_r = ui.bu_tex_r:get_bind_2d()
    local r, g, b, a = ui.color:get_rgba()

    local function draw_half( b_flag, alpha_l, alpha_r, sy_min, sy_max )
        if self[b_flag] then
            gol.color(r, g, b, self[alpha_l])
            gol.bind_texture( tex_bind_r )
            aaa.draw_rect_uv_at_z( 0.5, sy_min,    1, sy_max, 0 )

            gol.color( r, g, b, self[alpha_r] )
            gol.bind_texture( tex_bind_l )
            aaa.draw_rect_uv_at_z(  -1, sy_min, -0.5, sy_max, 0 )
        end
    end

    draw_half("b_top", "alpha_top_l", "alpha_top_r",  2/9,  2/3 )
    draw_half("b_mid", "alpha_mid_l", "alpha_mid_r", -2/9,  2/9 )
    draw_half("b_bot", "alpha_bot_l", "alpha_bot_r", -2/3, -2/9 )
end
