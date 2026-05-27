function meu:define_meu_infos( )
	return { author = "Abdalight", date = "2025",
			tags = { "3D", "Art", "Draw", "Generator" },
			help = "CartoMaton using shader for smooth texturing" }
end

function meu:define_ui()
	local ref   = self.ref
	local ui    = self.ui
	local sha   = self:get_shading()
	local ix,iy = 1,1
	local SY,DY = 1,.2
	local bu

	self:add_shading_ui(		{ix,iy,			8,SY} )

	self:add_trig_method(		{ix+3,iy,		4,SY},		"Load Cards",		self,		"load_images" ):set_color_back("load")
    iy = iy + SY + DY
	

	local y_begin = iy
------------------------------
	self:set_tab_key( "Main" )
	ix, iy = 1,y_begin

	self:add_slider(			{ix,iy,			4,SY},		"Mix Down",			self,		"v_mix_down",	0.3,	   0.0,1.0 )
	self:add_slider(			{ix+4,iy,		4,SY},		"Mix UP",			self,		"v_mix_up",		0.7,	   0.0,1.0 )
	iy = iy + SY + DY
	
	self:add_button(			{ix-1,iy+1,		 3,1},		"Top L",			self,		"b_top_l",		false   ):set_angle(.25)
	bu = self:add_bu_texture_target_unit( {ix+2,iy,   4,3*SY,b_compact=true},				"Tex_Top",				1,      true   )
		ui.bu_tex_1 = bu
	self:add_button(			{ix+6,iy+1,		 3,1},		"Top R",			self,		"b_top_r",		false   ):set_angle(.25)
	iy = iy + 3 * SY

	self:add_button(			{ix-1,iy+1,		 3,1},		"Mid L",			self,		"b_mid_l",		false   ):set_angle(.25)
	bu = self:add_bu_texture_target_unit( {ix+2,iy,   4,3*SY,b_compact=true},				"Tex_Mid",				2,		true   )
		ui.bu_tex_2 = bu
	self:add_button(			{ix+6,iy+1,		 3,1},		"Mid R",			self,		"b_mid_r",		false   ):set_angle(.25)
	iy = iy + 3 * SY

	self:add_button(			{ix-1,iy+1,		 3,1},		"Bot L",			self,		"b_bot_l",		false   ):set_angle(.25)
	bu = self:add_bu_texture_target_unit( {ix+2,iy,   4,3*SY,b_compact=true},				"Tex_Bot",				3,		true   )
		ui.bu_tex_3 = bu
	self:add_button(			{ix+6,iy+1,		 3,1},		"Bot R",			self,		"b_bot_r",		false   ):set_angle(.25)
	iy = iy + 3 * SY

	self:add_slider(			{ix,iy,		  8/3,SY},		"Speed Top",		self,		"speed_top",	0,	   0.0,1.0 )
	self:add_slider(			{ix+8/3,iy,	  8/3,SY},		"Speed Mid",		self,		"speed_mid",	0,	   0.0,1.0 )
	self:add_slider(			{ix+16/3,iy,  8/3,SY},		"Speed Bot",		self,		"speed_bot",	0,	   0.0,1.0 )
	iy = iy + SY

------------------------------

	ix,iy = 9,y_begin
	self:add_camera()
	self:add_rendering()

	ui.bu_rgbf = self:add_rgbf(	{ix,iy,		8,SY}	)
	iy = iy + SY + DY

	self:add_transfo(			{ix,iy,	 nil,2.4},  1 )
	iy = iy + 2.4 * SY + DY

	self:add_trig_method(		{ix,iy,		 4,SY},		"Random",			self,		"random",		false   )
	iy = iy + SY

	------------------------------
	local sha = self:get_shading()
	self:add_shading_sliders_tab(	sha,	{"Vert","Frag"},	y_begin,SY	)

	ui.bu_frag_float_1	 = self:get_bu_by_key( "fragment_float_1" )
	ui.bu_frag_float_2 	 = self:get_bu_by_key( "fragment_float_2" )
	ui.bu_frag_float_3 	 = self:get_bu_by_key( "fragment_float_3" )
end


function meu:init()
	self.layer_shading = 2
	local sha = self:add_shading( self.layer_shading )
	self.all_images = {}
end


function meu:update()
	local ref = self.ref
	local ui = self.ui
	local sha = self:get_shading()
	local bu
	
	if ui.bu_frag_float_1 then ui.bu_frag_float_1:set_value( 0.16 )				end
	if ui.bu_frag_float_2 then ui.bu_frag_float_2:set_value( self.v_mix_down )	end
	if ui.bu_frag_float_3 then ui.bu_frag_float_3:set_value( self.v_mix_up )	end

end


function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 )
		self:draw_layer( 2 )
		self:draw_slot_machine( self.all_images )
	self:draw_layers_end()
end


function meu:draw_card( x, y, z, sx, sy, u_min, u_max, v_min, v_max, texture_bind ) 
	local sha = self:get_shading()
	local sx, sy = sx * 0.5, sy * 0.5

	sha:set_frag_vec4( 1, u_min,u_max, v_min, v_max )
	gol.bind_texture( texture_bind )
	gol.update_uniform_fragment_vec4()
	aaa.draw_rect_uv_at_z( x-sx, y-sy, x+sx, y+sy, z )
end




function meu:draw_slot_machine( tab_images )
	local images = tab_images or {}
	local nb_images = #images or 1
	local sx, sy = 1, 4/6

	local function update_phase( ph, nb_images, b_left, b_right, speed )
		ph = ph or 0
		nb_images = nb_images or 1
	
		if b_left then
			ph = ph + aaa.time.dt * speed
		elseif b_right then
			ph = ph - aaa.time.dt * speed
		else
			local nearest = math.floor( ph + 0.5 )
			ph = ph + (nearest - ph) * aaa.time.dt * 2.0
		end
		local max_ph = math.max( 1, nb_images - 1 )
		return ( ph + max_ph ) % max_ph
	end

	local function draw_band( ph, v_min, v_max )
		local x  = - math.fmod( ph, 1.0 )
		local id = math.floor( 1 + ph )
		for i = id, id + 1 do
			local image = images[i]
			local bind = image and image.bind or 1
			local u_min, u_max = 0-x, 1-x
			self:draw_card( x, 0, 0, sx, 2 * sy, u_min, u_max, v_min, v_max, bind )
			x = x + sx
		end
	end

	local function update_ui_textures( images, ph_top, ph_mid, ph_bot)
		local ui = self.ui
		local function get_image(images, index)
			return images and images[math.floor(index + 0.5)]
		end
	
		local image_top = get_image(images, ph_top)
		if image_top and ui and ui.bu_tex_1 then
			ui.bu_tex_1:set_bind_2d( image_top.bind + 1 )
		end
	
		local image_mid = get_image(images, ph_mid)
		if image_mid and ui and ui.bu_tex_2 then
			ui.bu_tex_2:set_bind_2d( image_mid.bind + 1 )
		end
	
		local image_bot = get_image(images, ph_bot)
		if image_bot and ui and ui.bu_tex_3 then
			ui.bu_tex_3:set_bind_2d( image_bot.bind + 1 )
		end
	end

	self.ph_top = update_phase( self.ph_top or 0, nb_images, self.b_top_l, self.b_top_r, self.speed_top )
	self.ph_mid = update_phase( self.ph_mid or 0, nb_images, self.b_mid_l, self.b_mid_r, self.speed_mid )
	self.ph_bot = update_phase( self.ph_bot or 0, nb_images, self.b_bot_l, self.b_bot_r, self.speed_bot )


	draw_band( self.ph_top,		self.v_mix_up	- 0.1,		1.0				)
	draw_band( self.ph_mid,		self.v_mix_down - 0.1,		self.v_mix_up	)
	draw_band( self.ph_bot,		0.0,						self.v_mix_down	)

	update_ui_textures( self.all_images, self.ph_top, self.ph_mid, self.ph_bot )
end


function meu:load_images()
	--local folder_path = aaa.file.do_dialog_folder( "Select Folder for Images" )
	local folder_path = "C:/AAA/Media/ArtCast4D/CartoMaton/JEU1"
    local images = {}
    local index = 1
    local files = aaa.dir.get_files( folder_path )
    local bind
	self.all_images = {}

    if not files or #files == 0 then
        self:print( "Error : No files found in folder : " .. folder_path )
        return
    end
    for _, file_name in ipairs( files ) do
        local full_path = folder_path .. "/" .. file_name
        if string.match( file_name, "%.png$" ) or string.match( file_name, "%.jpg$" ) or string.match( file_name, "%.jpeg$" ) then
            bind = IMGS.get_bind_free() -- return a free bind
            if bind then
                local success = aaa.img.read( bind, full_path, false, false, false, true )
                if success then
                    images[ index ] = {
                               file = file_name,
                               bind = bind,
                                      }
                    index = index + 1
                else
                    self:print( "Error: Failed to load image: " .. file_name )
                end
                IMGS.inc_bind_free( 1 ) -- now allocate the free bind we used
            else
                self:print( "Warning: No free binds available for " .. file_name )
            end
        else
            self:print( "Skipping non-image file: " .. file_name )
        end
    end
    self.all_images = images
	self.ui.bu_tex_1:set_bind_2d( images[1].bind )
	self.ui.bu_tex_2:set_bind_2d( images[1].bind )
	self.ui.bu_tex_3:set_bind_2d( images[1].bind )
    self:print( "Loaded " .. #images .. " images from folder: " .. folder_path )
end