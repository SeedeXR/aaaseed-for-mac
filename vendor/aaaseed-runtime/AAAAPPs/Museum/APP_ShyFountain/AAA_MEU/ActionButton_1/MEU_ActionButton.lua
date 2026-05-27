function meu:define_meu_infos()
	return { author = "Tim", date = "2025",
			   tags = { "2d", "3d", "Art", "draw", "action", "unfinished" },
			   help = "Draws a rotary action button that can toggle between two positions and triggers a sound"
	}
end

function meu:define_ui()
	local ref = self.ref
	local ui  = self.ui
	local ix, iy = 1, 1
	local SY, DY = 1, 0.2
	local bu
	local sha = self:get_shading()
	
	self:add_rendering()
	self:add_camera()

	bu = self:add_button(		{ix,iy,			4,SY},		"Sound",			self,		"b_sound",			false		)
	iy = iy + SY + DY

	bu = self:add_slider(		{ix,iy,			8,SY},		"Size",				self,		"button_size",		0.1,	0,5		)
	iy = iy + SY + 2 * DY

	bu = self:add_trig_method(	{ix,iy,			8,SY},		"Toggle",			self,		"toggle"			)
	iy = iy + SY + 2 * DY

	bu = self:add_slider(		{ix,iy,			8,SY},		"Transition Speed",	self,		"transition_time",		1.5,	0,5		)
	iy = iy + SY + 2 * DY

	bu = self:add_slider(		{ix,iy,			8,SY},		"Sound volume",		self,		"snd_toggle_vol",		0.8,	0,1		)
	iy = iy + SY + 2 * DY

	self:add_shading_ui(		{ix,iy,			8,SY}	)
	iy = iy + SY
	bu = self:add_slider(		{ix,iy,			8,SY},		"Frag A",		sha:get_ref_frag_float(1), nil, 0, 	0,1 )

	-- self:add_trig_method(		{ix,iy,			4,SY},		"Load Button Images",		self,		"set_button_image_folder"						)
	-- 	:set_color_back("load")

	-- bu = self:add_text_info(	{ix,iy,			8,SY},		"Image Folder"	)
	--     ui.bu_folder_name = bu
	-- iy = iy + SY + DY

	ix, iy = 9, 3
	self:add_transfo(		{ix,iy,		   4,2.4},   1   )
	iy = iy + 2.4 + DY
end


function meu:init()
	local ref = self.ref
	local pgr = param.get_ref
	local gbc = aaa.obj.get_branch_by_class

	ref.layer_attr = self:get_layer( 1 )
	
	ref.mapping			= gbc( ref.layer_attr,	"mapping"		)
	ref.rendering		= gbc( ref.layer_attr,	"rendering"		)
	ref.tex_use		    = pgr( ref.mapping,		"tex_use"		)
	ref.rendering_depth = pgr( ref.rendering,	"depth"			)

	self.img_bg = nil
	self.img_fg = nil
    self.img_footprints = nil

    self.alpha_global = 1
    self.alpha_footprints = 1

	self.layer_shading = 2
	local sha = self:add_shading( self.layer_shading )
	
	self:reset()
end


function meu:draw()
	local ref = self.ref			
	self:draw_layers_begin()
		self:draw_layer(1)
		self:draw_layer(2)
		self:draw_button()
	self:draw_layers_end()
end


function meu:update()
	if not self.img_bg then
		self:load_button_images()
		
	end
	if self.b_sound then
		if not self.snds then
			self.snds = self:load_snds()
		end
		if self.snds then
			self.snds:update()
		end
	end
end


function meu:toggle()
	if self.transitioning then
		return
	end
	
	self.button_state_on = not self.button_state_on
    self.transitioning = true
	self.toggle_time = aaa.time.t

	if self.b_sound then
		if self.snds then
			self.snds:play( "toggle", false, self.snd_toggle_vol, 0 )
		end
	end

    if self.button_state_on then
        local meu_part = self:get_meu_by_name_cached( "ImgParticle_1" )
        if meu_part then meu_part.should_select_new_image_folder = true end
    else
        local meu_main = self:get_meu_by_name_cached( "ShyFountain_1" )
        if meu_main then meu_main:start_atmo_sound() end
    end
end


function meu:reset()
    self.button_state_on = true
    self.transitioning = false
	self.toggle_time = aaa.time.t
end


function meu:draw_button()
	gol.set_quad_uv()
    
	local function draw_elt( rot, image, alpha, scale )
		local sx,sy
        gol.color_white( alpha )

		if image then
			sx,sy = image.sx, image.sy
			gol.bind_texture( image.bind )
		else
			sx,sy = 1,1
			gol.bind_texture( 0 )	
		end

		gol.push_matrix()
			gol.rotate_y(rot)
			aaa.draw_rect_uv_axe_y( 0,0,0, sx*self.button_size*scale, sy*self.button_size*scale )
		gol.pop_matrix()
	end

	local button_pos = 0
    local t = aaa.time.t
    
    local meu_main = self:get_meu_by_name_cached( "ShyFountain_1" )

	if self.transitioning then
		button_pos = clamp_01( (t - self.toggle_time) / self.transition_time )
        self.transitioning = button_pos < 1

		if self.button_state_on then
			button_pos = 1 - button_pos
		end

        if meu_main then
            local fountain_amp = (1 - button_pos)^4
            meu_main:set_bu_value( "Fountain amp", fountain_amp )
        end
    elseif not self.button_state_on then
        button_pos = 1
	end

    local rot_base = 0.25

    if self.button_state_on then
        -- when button is in on position (i.e. fountain is on), gradually bring back the alpha as the button toggles
        self.alpha_global = 1 - button_pos
    else
        -- when button is in off position, slowly fade out button 2 seconds after toggle is finished
        local fade_delay = 2
        local fade_dur = 2
        self.alpha_global = clamp_01( 1 - (t - self.toggle_time - fade_delay) / fade_dur )
    end

	if self.img_bg then
        local max_fg_rot = -0.16
		local rot_fg = rot_base + 0.08 + button_pos * max_fg_rot

		draw_elt( rot_base, self.img_bg, self.alpha_global, 1 )
		draw_elt( rot_fg, self.img_fg, self.alpha_global, 1 )
	end

    if self.img_footprints and self.alpha_footprints then
        if self.button_state_on then
            -- when button is on, wait 10 seconds after toggle before footprints become visible again
            local fade_delay = 8
            local fade_dur   = 6
            self.alpha_footprints = clamp_01((t - self.toggle_time - fade_delay) / fade_dur)
        end

        local alpha = clamp_01(math.min(self.alpha_footprints, self.alpha_global))
        local scale = 0.38
        
        if meu_main then
            scale = scale + 0.08 * meu_main.toggle_signal
        end

        draw_elt( rot_base, self.img_footprints, alpha, scale)
    end

end


function meu:load_button_images()
	local index = 1
	local bind

    local meu_main = self:get_meu_by_name_cached( "ShyFountain_1" )
    if not meu_main then return end
    
    local sprites_folder = meu_main.sprites_folder

    if not aaa.dir.is_exist( sprites_folder ) then return end

	local function load_image( file_name )
		local img = nil

        local function normalize_size(sx, sy)
            if sx > sy then
                return 1, 1 * sy / sx
            else
                return 1 * sx / sy, 1
            end
        end

		bind = IMGS.get_bind_free()
		if bind then
			local full_path =  sprites_folder .. "/" .. file_name
			local success = aaa.img.read(bind, full_path, false, false, false, true)
			if success then
				local sx, sy = normalize_size(aaa.img.get_size(bind))
				img = {
					file = file_name,
					bind = bind,
					sx   = sx,
					sy   = sy,
					self:print("Loaded " .. file_name .. " from: " .. sprites_folder)
				}
				index = index + 1
			else
				self:print("Error: Failed to load image: " .. file_name)
			end
			IMGS.inc_bind_free(1)
		end

		return img
	end

	self.img_bg = load_image( "button_faces_arc.png" )
	self.img_fg = load_image( "button_arrow.png" )
    self.img_footprints = load_image( "footprints_white.png" )
end


function meu:load_snds()
    local meu_main = self:get_meu_by_name_cached( "ShyFountain_1" )
    if not meu_main then return nil end
    
    local sounds_folder = meu_main.sounds_folder
	if not aaa.dir.is_exist( sounds_folder ) then return nil end

    local player = SNDS_PLAYER:create("ActionButton")
    player:add( "toggle", 1, sounds_folder .. "/StoneTurnSnap", "mp3" )
    return player
end