function meu:define_meu_infos()
	return { author = "Maa Then Tim", date = "2025",
			   tags = { "Art", "procedural", "unfinished" },
			   help = "Main logic of ShyFountain APP"
	}
end

function meu:define_ui()
	local ref = self.ref
	local ui  = self.ui
	local ix, iy = 1, 1
	local SY, DY = 1, 0.2
	local bu

    bu = self:add_trig_method(	{ix,iy,	8,SY},		"Reset",		  self,		"reset"			)
	iy = iy + SY + 2 * DY

    bu = self:add_slider(	{ix,iy,		8,SY},		"Fountain amp",	  self, "fountain_amp",	        1, 0,1	)
        bu:set_method_on_value_change( self, "fountain_amp_changed" )
    iy = iy + SY + DY


    bu = self:add_slider(	{ix,iy,		8,SY},		"Fountain max",	  self, "fountain_max_amp",	        8, 5,15	)
        bu:set_method_on_value_change( self, "fountain_amp_changed" )
    iy = iy + SY + DY


    bu = self:add_slider(	{ix,iy,		8,SY},		"Fountain time out (sec)",    self, "fountain_time_out",	20, 10,120	)
    iy = iy + SY + DY


    bu = self:add_slider(	{ix,iy,		8,SY},		"Atmo sound volume",        self,   "atmo_volume",	    .8,     0.6,1.0 )
    iy = iy + SY + DY
    
    bu = self:add_button(   {ix,iy,     8,SY},		"Allow fountain mod",      self,   "b_fountain_mod",    true		)
    iy = iy + SY + DY

	ix, iy = 9, 2
end


function meu:init()
    local base_dir = aaa.dir.get_dir_media() .. "ShyFountain/"
    self.images_folder  = base_dir .. "Image_banks"
    self.sounds_folder  = base_dir .. "Sounds"
    self.sprites_folder = base_dir .. "Sprites"

    self.snds = nil

    self.toggle_signal = 0 -- not a boolean but a float so the interaction is less abrupt
    
    self.t_last_burst = -1
    self.t_burst_timeout = 2
    self.burst_reset = true

    self.should_reset = true
end


function meu:reset()
    self:set_bu_value( "Fountain amp", 1 )

    local meu_button = self:get_meu_by_name_cached( "ActionButton_1" )
    if meu_button then
        meu_button:reset()
    end

    if self.snds then
        self:start_water_sound()
    end
    
    self.should_reset = false
end


function meu:fountain_amp_changed()
    local fnt_amp = clamp_01(self.fountain_amp)

    local meu_fog = self:get_meu_by_name_cached( "Fog_1" )
    if meu_fog then
        meu_fog.target_amount = fnt_amp
    end
end


function meu:update()
    local ref = self.ref
    local ui = self.ui

    local t = aaa.time.t
    local dt = aaa.time.dt

    if self.should_reset then
        self:reset()
    end

    -- init and loading stuff
	local meu_part, b_first = self:get_meu_by_name_cached( "ImgParticle_1" )
	if not meu_part then return end
    
	if b_first then
		meu_part:load_images( self.images_folder )
        meu_part:select_random_image_folder()
	end
    
    
    if not self.snds then
        self.snds = self:load_snds()
        if self.snds then
            self:start_water_sound()
        end
    end
    
    
    self:update_sounds()
    
    -- get the MEU analyse
    local meu_anal = self:get_meu_by_name_cached( "ImgAnalMulti_1" )
    if not meu_anal then return end
    local cov_button = meu_anal:get_bu_value( "cov 1" )
    
    -- handle button interaction
    local meu_button = self:get_meu_by_name_cached( "ActionButton_1" )
    self.toggle_signal = clamp_01( self.toggle_signal - dt * 0.5 )
    if meu_button.alpha_footprints > 0.9 then
        self.toggle_signal = clamp_01( self.toggle_signal + dt * cov_button )
    end
    
    if meu_button then
        if meu_button.button_state_on then
            -- if button is in on position (i.e. the fountain is on), then toggle if singal is 1
            if self.toggle_signal > 0.99 then
                meu_button:toggle()
            end
            
        elseif not meu_button.button_state_on then
            -- if button is in off position (i.e. the fountain is off), then toggle back after timeout
            if t - meu_button.toggle_time > self.fountain_time_out then
                meu_button:toggle()
            end
        end
    end


    -- change fountain flux depending on presence and position of visitors
    local meu_fountain = self:get_meu_by_name_cached( "Flex_ShyA" )

    -- local det_global_cov    = meu_anal:get_bu_value( "cov 2" )
    local det_global_x      = meu_anal:get_bu_value( "center u 2" )
    local det_global_y      = meu_anal:get_bu_value( "center v 2" )

    if meu_fountain then
        math.randomseed(t)
        
        local fnt_amp = clamp_01(self.fountain_amp)
        fnt_amp = fnt_amp * self.fountain_max_amp
        meu_fountain:set_bu_value( "fall speed", fnt_amp )

        if self.b_fountain_mod then
            local mod_x = 2 * clamp_01((det_global_x - 0.5) / 0.5)
            local mod_y = 50 * clamp_01((det_global_y - 0.5) / 0.5)
            local mod_burst = math.sin(det_global_x * 60)
            

            meu_fountain:set_bu_value( "gravity_x", mod_x )
            meu_fountain:set_bu_value( "gravity_y", -9.81 + mod_y )

            -- if mod_burst > 0.0 then
            --     meu_fountain:set_bu_value( "shock propagation", mod_burst - 0.5 )
            -- else
            --     meu_fountain:set_bu_value( "shock propagation", 0.0 )
            -- end
        end

    end
end


function meu:update_sounds()
    if not self.snds then return end

    local t = aaa.time.t
    local dt = aaa.time.dt

    if t - self.t_water_snd > 599 then
        self.snds:restart( "water" )
        self.t_water_snd = t
    end
    
    self.snds:set_volume( "water", self.fountain_amp^0.1 )
    self.snds:set_volume( "atmo", (1 - self.fountain_amp) * self.atmo_volume )
        
    self.snds:update()
end


function meu:start_water_sound()
    self.snds:play( "water", true, 0.1, 0 )
    self.t_water_snd = aaa.time.t
end


function meu:start_atmo_sound()
    self.snds:play( "atmo", false, 0.1, 0 )
end


function meu:load_snds()
	if aaa.dir.is_exist( self.sounds_folder ) then
		local player = SNDS_PLAYER:create("ShyFountainMain")
		player:add( "water", 1, self.sounds_folder .. "/Water3", "mp3" )
		player:add( "atmo",  1, self.sounds_folder .. "/Atmo_2min", "mp3" )
		return player
    else
        return nil
	end
end