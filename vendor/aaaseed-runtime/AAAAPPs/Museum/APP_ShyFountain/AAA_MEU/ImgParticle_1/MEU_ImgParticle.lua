function meu:define_meu_infos()
	return { author = "Abdalight, Maa and Tim", date = "2025",
			   tags = { "2d", "3d", "Art", "draw", "procedural", "unfinished", "vj" },
			   help = "Draws one image per point from a BDD layer witch is associated to an image loaded from a selected folder"
	}
end


local function lerp(a, b, t)
    return t * b + (1 - t) * a
end


function meu:define_ui()
	local ref = self.ref
	local ui  = self.ui
	local ix, iy = 1, 1
	local SY, DY = 1, 0.2
	local bu

	self:add_rendering()
	self:add_camera()
	
	self:add_trig_method(		{ix,iy,		3,SY},			"Restart",			self,		"restart"			)
	self:add_trig_method(		{ix+5,iy,	3,SY},			"Focus",			self,		"focus"				)
	iy = iy + SY
	
	bu = self:add_button(		{ix,iy,			2,SY},		"Draw",				self,		"b_draw",			false			)
	bu = self:add_button(		{ix+2,iy,		2,SY},		"2D",				self,		"b_tex_2d",			false			)
	bu = self:add_button(		{ix+4,iy,		4,SY},		"BDD Type",			 nil,			 nil,               0			)
 		bu:set_menu{ "Boid", "Particle", "Custom" }:set_nb_min_0( 1, 3 )
        bu:set_target_lua( self, "s_bdd" )
        bu:set_text_selector( true )
	iy = iy + SY

	bu = self:add_slider(		{ix,iy,			8,SY},		"Size",				self,		"side_size",		0.1,	0,5		)
	iy = iy + SY + DY

	bu = self:add_slider(		{ix,iy,			8,SY},		"Birth rate",		self,		"birth_rate",		1,		0,16		)
	iy = iy + SY + DY

    bu = self:add_slider(		{ix,iy,			8,SY},		"Particle number",	self,		"part_nb",		    10,		0,self.max_part_nb	)
        bu:set_value_type_integer( true )
        bu:set_min_max_strict( true )
	iy = iy + SY + DY

	bu = self:add_slider_multi(	{ix,iy,			4,6},		"NoGo",	5 )
		bu:set_select_on_click_double()
		ui.bu_repulsor = bu 
	bu = self:add_slider_two(	{ix+4,iy,		4,SY},		"X",		self, "x_min", "x_max",		-2,2,	-2,2  )
		:set_color_back("x")
	iy = iy + SY
	bu = self:add_slider_two(	{ix+4,iy,		4,SY},		"Z",		self, "z_min", "z_max",		0,4,	0,4  )
		:set_color_back("z")
	iy = iy + SY + DY

	bu = self:add_slider(		{ix+4,iy,		4,SY},		"Repulse",	self, "repulse",		 	0,		0,1	)
	iy = iy + SY
	bu = self:add_button(		{ix+4,iy,		4,SY},		"Repulse Draw",	self, "b_repulse_draw",		 	false	)

	ix, iy = 9, 2+DY

	self:add_trig_method(		{ix,iy,			4,SY},		"Load Images",		self,		"load_images_with_dialog" )
		:set_color_back("load")
	self:add_trig_method(		{ix+4,iy,		4,SY},		"Clear Images",		self,		"clear_images" )
	iy = iy + SY 

	self:add_trig_method(		{ix,iy,			4,SY},		"Random Folder",	self,		"select_random_image_folder" )
	iy = iy + SY 

	bu = self:add_slider(		{ix,iy,			8,SY},		"Folder",			self,		"id_folder",		1,		1,15	)
		bu:set_min_max( 1, 15 )
        bu:set_value_type_integer( true )
		bu:set_min_max_strict( true )
        bu:set_method_on_value_change( self, "update_images" )
		ui.bu_folder_nb = bu
	iy = iy + SY

	bu = self:add_text_info(	{ix,iy,			8,SY},		"Folder Name"	)
        ui.bu_folder_name = bu
    iy = iy + SY + DY

	self:add_transfo(		{ix,iy,		   4,2.4},   1   )
	iy = iy + 2.4 + DY

	bu = self:add_slider(		{ix,iy,			8,SY},		"Max rot speed",	self,		"max_rot_speed",	0.1,	0,1		)
	iy = iy + SY
    bu = self:add_slider(		{ix,iy,			8,SY},		"Max hover freq",	self,		"max_hover_freq",	0.1,	0,1		)
	iy = iy + SY + DY
    bu = self:add_slider(		{ix,iy,			8,SY},		"Max hover amp",	self,		"hover_amp",	    0.2,	0,1		)
	iy = iy + SY + DY
end

function meu:init()
	local ref = self.ref
	local pgr = param.get_ref
	local gbc = aaa.obj.get_branch_by_class

	ref.layer_attr = self:get_layer( 1 )
	ref.layer_part = self:get_layer( 2 )
	ref.layer_boid = self:get_layer( 3 )
	
	ref.mapping			= gbc( ref.layer_attr,	"mapping"		)
	ref.rendering		= gbc( ref.layer_attr,	"rendering"		)
    ref.tex_use		    = pgr( ref.mapping,		"tex_use"		)
	ref.rendering_depth = pgr( ref.rendering,	"depth"			)
	ref.use_bdd_part    = pgr( ref.layer_part,	"use_bdd"       )
	ref.use_bdd_boid    = pgr( ref.layer_boid,	"use_bdd"       )

	local part = {	bdd = self:get_layer_bdd(2) }
	local boid = {	bdd = self:get_layer_bdd(3) }
	ref.part = part
	ref.boid = boid

	part.restart    = pgr( part.bdd,	"restart_trig"	)
	boid.restart    = pgr( boid.bdd,	"restart_trig"	)

	part.birth_rate	= pgr( part.bdd,	"nb_by_sec"		)
	boid.birth_rate	= pgr( boid.bdd,	"birth_rate"	)

    self.max_part_nb = 200

    self.glob_part_alpha = 0
    self.dimmed_part_alpha = 0.42
    self.part_alpha_fade_dur = 10

	self:clear_images()
end

function meu:init_after()
	self:restart()
end

function meu:random_custom_positions()
	local t = {}
    
    math.randomseed(aaa.time.t * 345.67)

	local xo,xf = self.x_min,  self.x_max - self.x_min
	local zo,zf = self.z_min,  self.z_max - self.z_min
    for i = 1,self.max_part_nb do
        local x = math.random() * xf + xo
        local y = math.random() * .0
        local z = math.random() * zf + zo
        t[i] = {	x = x, y = y, z = z,
					rnd1 = math.random(),
					rnd2 = math.random(),
					rnd3 = math.random()
				}
    end

    self.custom_part_pos = t
end

function meu:draw()
	local ref = self.ref			
	self:draw_layers_begin()
		self:draw_layer(1)

        if self.s_bdd == 2 then
            self:draw_custom_particles()
        else
            self:draw_layer( self.layer_bdd_id )
            if self.b_draw then
                self:draw_images_looped_by_id( self.bdd_refs.bdd )
            end
        end
		if self.b_repulse_draw then
			self:draw_repulsor()
		end
	self:draw_layers_end()
end


function meu:update()
	local ref = self.ref
	local ps  = param.set

    self:update_global_alpha()

    if self.s_bdd == 2 then
    else

        if self.b_draw then
            ps(ref.use_bdd_part, 3)
            ps(ref.use_bdd_boid, 3)
        else
            self.b_tex_2d = false
            ps(ref.use_bdd_part, 2)
            ps(ref.use_bdd_boid, 2)
        end

        if self.b_tex_2d then
            ps( ref.tex_use, 2 )
        else
            ps( ref.tex_use, 0 )
        end

        ps(ref.rendering_depth, self.bdd_use or 0)

        local s_bdd = self.s_bdd
        local refs, layer_id
        if     s_bdd == 0 then
            refs = ref.boid
            layer_id = 3
        elseif s_bdd == 1 then
            refs = ref.part
            layer_id = 2
        end
        self.bdd_refs = refs
        self.layer_bdd_id = layer_id

        -- param.set( refs.birth_rate, self.birth_rate )
        param.set( refs.birth_rate, 2 ) 
    end

	self:update_repulsor()
end


function meu:restart()
  	local refs = self.bdd_refs
	if refs then 
		param.set( refs.restart, true )
	end
	self:random_custom_positions()

    self.should_select_new_image_folder = true
end


function meu:focus()
	local refs = self.bdd_refs
	if refs then
		aaa.obj.set_focus_ui( refs.bdd )
	end
end


function meu:clear_images()
  	self.all_images_by_folder = {}
    self.n_folders = 0
end


function meu:select_random_image_folder()
    if self.n_folders > 0 then
		math.randomseed(aaa.time.t)
		
        local rand_folder_id = math.floor(math.random() * self.n_folders) + 1
        self:set_bu_value( "Folder", rand_folder_id )
		
		math.randomseed(aaa.time.t + 52.34)
        local rand_number = 25 + math.ceil(math.random() * 10)
        self:set_bu_value( "Particle number", rand_number )

        self:random_custom_positions()
    end

    self.should_select_new_image_folder = false
end


function meu:update_global_alpha()
    local meu_main = self:get_meu_by_name_cached( "ShyFountain_1" )

    if not meu_main then
        self.glob_part_alpha = 1
        return
    end

    local target_alpha = clamp_01(1 - (1 - self.dimmed_part_alpha) * meu_main.fountain_amp)
    
    if self.should_select_new_image_folder then
        target_alpha = 0
        
        if self.glob_part_alpha == 0 then
            self:select_random_image_folder()
        end
    end

    local diff = target_alpha - self.glob_part_alpha
    local sign = diff >= 0 and 1 or -1
    self.glob_part_alpha = clamp_01( self.glob_part_alpha + sign * aaa.time.dt / self.part_alpha_fade_dur )
end


function meu:update_repulsor()	
    local bu = self.ui.bu_repulsor
	local nb = bu:get_elt_nb()
    local id = 1	--we will rtesuse the table later instead of creating it every frame
	local t = {}
	
	for i=1,nb do
		local elt = bu:get_elt(i)
		if elt:get_value_as_bool() then
			local x,z = elt:get_xy()
			table.insert( t, {x= -2 + x*2, z = 4 - z*4 } )
			id = id + 1
		end
	end
    self.repulsor = t
end


function meu:draw_repulsor()
	--table.print( self.repulsor, "repulsor", 2 )
	gol.color_red()
	gol.set_texture_dim( 0 )
	for i,e in IPAIRS(self.repulsor) do
		aaa.draw_disk_axe_y( e.x,e.y,e.z, self.repulse )
	end
end


function meu:draw_custom_particles()
	gol.set_texture_dim( self.b_tex_2d and 2 or 0 )
	
	local repulsor = self.repulsor
    gol.set_quad_uv()
    
    local t = aaa.time.t
    
	local s = self.repulse * .5 + self.side_size * math.sqrt(2) * .5
	local s2 = s*s
    
    local function draw_elt( p, image, alpha )
		local x,y,z = p.x,p.y,p.z

        x = math.min(self.x_max, math.max(x, self.x_min))
        z = math.min(self.z_max, math.max(z, self.z_min))

		for i,e in IPAIRS(repulsor) do
			local dx = x-e.x
			local dz = z-e.z
			local d = dx*dx + dz*dz
			if d < s2 then
				d = math.sqrt(d)
				dx =  dx / d
				dz =  dz / d
				x = e.x + dx * s
				z = e.z + dz * s
				break
			end
		end

        local sx,sy
        if image then
            sx,sy = image.sx, image.sy
            gol.bind_texture( image.bind )
        else
            sx,sy = 1,1
            gol.bind_texture( 0 )	
        end
        
        local y_freq = p.rnd1
        local y_phase =  p.rnd2 * 4
        y_freq = y_freq * self.max_hover_freq
        local hover = math.sin(y_freq * t + y_phase)
        
        local m = (self.glob_part_alpha - self.dimmed_part_alpha) / (1 - self.dimmed_part_alpha)
        hover = lerp(hover, 1, m)
        y = y + (hover - 1) / 2 * self.hover_amp
        
        local rot_freq = p.rnd3 - 0.5
        rot_freq = rot_freq * self.max_rot_speed
        
        alpha = clamp_01( alpha * hover * self.glob_part_alpha )

        gol.color_white(alpha)

        gol.push_matrix()
            gol.translate( x, y, z )
            gol.rotate_y( rot_freq * t )         
            gol.scale( clamp_01(8-z*2) )
            aaa.draw_rect_uv_axe_y( 0,0,0, sx*self.side_size, sy*self.side_size )
        gol.pop_matrix()
    end

	local t = self.custom_part_pos
	if t then
		local nb_images = self.nb_images
		local images = self.images
	
		
		for i = 1, math.min(self.part_nb, self.max_part_nb) do
			local p = t[i]
			local id = i
			local image

			if images and nb_images > 0 then	-- todo too much test in a loop
				image = images[(id - 1) % nb_images + 1]
			end

			draw_elt( p, image, 1 )
		end
	end

end


function meu:draw_images_looped_by_id( bdd )
	if not bdd then return end

	local ref = self.ref
	local ui  = self.ui

	gol.set_quad_uv()
	gol.color_white()
	math.randomseed(42)

	local function draw_elt( x,y,z, id, image )
		local sx,sy
		if image then
			sx,sy = image.sx, image.sy
			param.set( ref.tex_use, 2 )
			gol.bind_texture( image.bind )
		else
			sx,sy = 1,1
			param.set( ref.tex_use, 0 )
			gol.bind_texture( 0 )	
		end
        
		gol.push_matrix()
			gol.translate( x,y,z )
			gol.rotate_y(z * 0.24 )
            
			gol.scale( clamp_01(8-z*2) )
			aaa.draw_rect_uv_axe_y( 0,0,0, sx*self.side_size, sy*self.side_size )
		gol.pop_matrix()
	end

	local points = aaa.bdd.get_points_with_id( bdd )
	if not points then return end

	local nb_images = self.nb_images
    local images = self.images

	local sx,sy
	local bind
	for _, pt in ipairs(points) do
		local id = pt[4]
		local image
		if images and nb_images > 0 then	-- todo too much test in a loop
			image = images[(id - 1) % nb_images + 1]
		end
        
		draw_elt( pt[1],pt[2],pt[3], id, image )
	end
end


function meu:update_images()
	if not self.n_folders then return end
	self:print("N folders: " .. self.n_folders)

    local images = nil
	local nb_images = 0
    local ui = self.ui

	--todo avoid at every frame
	if self.all_images_by_folder then
		local folders = {}
		for k in pairs(self.all_images_by_folder) do table.insert(folders, k) end
		table.sort(folders)
		ui.bu_folder_nb:set_min_max( 1, #folders )

		local selected_folder = folders[self.id_folder]
		if selected_folder then
			ui.bu_folder_name:set_text(selected_folder)
			images = self.all_images_by_folder[selected_folder]
			nb_images = images and #images or 0
		else
			self:print("Not enough folders")
		end
	else
		ui.bu_folder_name:set_text("No folder")
	end

    self.images = images
    self.nb_images = nb_images
end


function meu:load_images_with_dialog()
	local base_folder = aaa.file.do_dialog_folder( "Select Base Folder" )
	if base_folder then
		self:load_images( base_folder )
    else
		self:print( "No folder selected." )
    end
end


function meu:load_images( base_folder )
	local folders = aaa.dir.get_dirs( base_folder )
	if not folders or #folders == 0 then
		--todo more of a dialog here ?
		self:print( "No subfolders found in: " .. base_folder )
		return
	end

    local function normalize_size(sx, sy)
		if sx > sy then
			return 1, 1 * sy / sx
		else
			return 1 * sx / sy, 1
		end
	end

	self.all_images_by_folder = {}
    self.n_folders = #folders

	for _, folder_name in ipairs(folders) do
		local full_folder_path = base_folder .. "/" .. folder_name
		local files = aaa.dir.get_files(full_folder_path)
		local images = {}
		local index = 1
		local bind

		if files then

			for _, file_name in ipairs(files) do
				local ext = string.lower(aaa.file.get_ext(file_name))
				if ext == "png" or ext == "jpg" or ext == "jpeg" or ext == 'tif' then
					bind = IMGS.get_bind_free()
					if bind then
						local full_path = full_folder_path .. "/" .. file_name
						local success = aaa.img.read(bind, full_path, false, false, false, true)
						if success then
							local sx, sy = normalize_size( aaa.img.get_size(bind) )

							images[index] = {
								file = file_name,
								bind = bind,
								sx   = sx,
								sy   = sy,
								self:print("Loaded " .. file_name .. " from: " .. folder_name)
							}
							index = index + 1
						else
							self:print("Error: Failed to load image: " .. file_name)
						end
						IMGS.inc_bind_free(1)
					else
						self:print("Warning: No free binds available for " .. file_name)
					end
				end
			end

			self.all_images_by_folder[folder_name] = images
			self:print("Loaded " .. #images .. " images from: " .. folder_name)
			
		end
	end

    self:update_images()
end
