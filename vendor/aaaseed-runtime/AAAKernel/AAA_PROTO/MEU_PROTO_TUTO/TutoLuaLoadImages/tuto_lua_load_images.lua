function meu:define_meu_infos()
	return { author = "Abdalight and Mâa", date = "2025",
			   tags = { "Tutorial", "Texture", "unfinished" },	--todo need more comments in the code
			   help = "basic example of how to load images from folder"
			 }
end

function meu:define_ui()
	local ref   = self.ref
	local ui    = self.ui
	local ix,iy = 1,1
	local SY,DY = 1,.2
	local bu

	self:add_rendering()
	self:add_camera()

	bu = self:add_trig_method(            {ix,iy,   7,SY},  "Load using IMG",		self,   "load_generic", self.load_with_IMG, 		"Using IMG"		)
		:set_color_back("load")
	iy = iy + SY + DY

	bu = self:add_trig_method(            {ix,iy,   7,SY},	"Load All using IMG",	self,   "load_generic", self.load_all_with_IMG, 	"All Using IMG"	)
		:set_color_back("load")
	iy = iy + SY + DY

	ix,iy = 9,2
	bu = self:add_bu_texture_target_unit( {ix,iy,   8,6*SY,b_compact=true},					"Tex",			1,							true			)
		ui.bu_tex = bu
	iy = iy + 6 * SY

	bu = self:add_trig_method(            {ix,iy,   7,SY},  "Load at bind", 		self,   "load_generic", self.load_at_bind,			"at bind"		)
		:set_color_back("load")
	iy = iy + SY + DY
end

function meu:draw()
	self:draw_layers_begin()
        self:draw_layer( 1 )
		self:draw_grid()
	self:draw_layers_end() 
end

function meu:draw_grid()
	local images = GABU_OBJ:get_do_table_return_last()
	if not images or #images == 0 then return end

	local sizes = {}
	local max_sx, max_sy = 0, 0
	for i, img in ipairs( images ) do
		local bind = img.bind
		if bind then
			local sx, sy = aaa.img.get_size( bind )
			if sx and sy then
				sx = sx / 512
				sy = sy / 512
				sizes[i] = {sx = sx, sy = sy}
				if sx > max_sx then max_sx = sx end
				if sy > max_sy then max_sy = sy end
			end
		end
	end

	local cols = math.ceil(math.sqrt(#images))
	local rows = math.ceil(#images / cols)

	local spacing_x = max_sx * 1
	local spacing_y = max_sy * 1

	local start_x = -((cols - 1) * spacing_x) / 2
	local start_y = ((rows - 1) * spacing_y) / 2

	for i, img in ipairs( images ) do
		local bind = img.bind
		local size = sizes[i]
		if bind and size then
			local col = (i - 1) % cols
			local row = math.floor((i - 1) / cols)

			local x = start_x + col * spacing_x
			local y = start_y - row * spacing_y

			gol.push_matrix()
				gol.translate(x, y, 0)
				gol.bind_texture(bind)
				gol.set_quad_uv(0, 0, 0, 1, 1, 0, 1, 1)
				aaa.draw_rect_uv_at_z(-size.sx/2, -size.sy/2, size.sx/2, size.sy/2, 0)
			gol.pop_matrix()
		end
	end
end

function meu:init()
end

function meu:is_ext_wanted( file_name )
	--self:print( "check file name "..file_name )
	local b = string.match( file_name, "%.png$" ) or string.match( file_name, "%.jpg$" ) or string.match( file_name, "%.jpeg$" )
	if not b then
		self:print( "Skipping file: " .. file_name.. " because of extension"  )
	end
	return b
end
function meu:add_image_to_table( tab, file_name, bind )
	self:print( "added to table image " .. file_name .. " with bind: " .. bind )
	table.insert( tab, { fname = file_name,	bind = bind } )
end

function meu:load_generic( load_method, str )
	-- select a folder
	local folder_path = aaa.file.do_dialog_folder( "Select Folder for Images" )
	if not folder_path then
		self:print_debug( "the user choose cancel in the dialog" ) 
		return
	end

	-- get all the files in the folder
	self:print( "Will process folder "..folder_path )
	local files = aaa.dir.get_files( folder_path )
	if not files or #files == 0 then
		self:print_error( "No files found in folder : " .. folder_path )
		return
	end

	-- select on filename 
	--table.print( files,  "files" )
	local files_wanted = {}
	for _, file_name in ipairs( files ) do
		if self:is_ext_wanted( file_name ) then
			table.insert( files_wanted, file_name )
		end
	end

	if #files_wanted == 0 then
		self:print_error( "No files with the wanted extensions in folder : " .. folder_path )
		return
	end

	-- now load from the files_wanted table returning a table with the result
	local loaded = load_method( self, folder_path, files_wanted )       

	-- display the result
	if loaded then
		for i, image_data in ipairs( loaded ) do
			self:print( "In bind "..image_data.bind.." loaded "..image_data.fname )
			--table.print( image_data, i.."-image" )
		end
		self:print( "Loaded " ..str..": "..#loaded .. " images from folder : " .. folder_path )
	else
		self:print( "No images loaded." )
	end
	return loaded
end


--method to load images from folder
--using IMGS.get_bind() and setting the bind automatically
function meu:load_with_IMG( folder_path, files )
	local images = {}
	MEDIA.set_dir( folder_path )

	for i, file_name in ipairs( files ) do
		local bind = IMGS.get_bind( file_name )
		if bind then
			self:add_image_to_table( images, file_name, bind )
			if i == 1 then	-- set the bu_textureto the first bind used 
				self.ui.bu_tex:set_bind_2d( bind )
			end
		else
			self:print( "Warning: No free binds available for " .. file_name )
		end
	end
	return images
end

--other method to load images from folder
--using aaa.img.read() and setting the bind automatically
function meu:load_all_with_IMG( folder_path, files )
	local images = {}
	for i, file_name in ipairs( files ) do
		local bind = IMGS.get_bind_free() -- return a free bind
		if bind then
			local full_path = folder_path .. "/" .. file_name
			local success = aaa.img.read( bind, full_path, false, false, false, true )
			if success then
				self:add_image_to_table( images, file_name, bind )
				IMGS.inc_bind_free( 1 ) -- now really allocate the free bind we used
				if i == 1 then	-- set the bu_texture to the first bind used 
					self.ui.bu_tex:set_bind_2d( bind )
				end
			else
				self:print( "Error: Failed to load image: " .. file_name )
			end
		else
			self:print( "Warning: No free binds available for " .. file_name )
		end
	end
	return images
end

-- method to load images from folder using a user-specified bind
function meu:load_at_bind( folder_path, files )
	local images = {}
	local bind = self.ui.bu_tex:get_bind_2d()

	for _, file_name in ipairs(files) do
		local full_path = folder_path .. "/" .. file_name
		local success = aaa.img.read( bind, full_path, false, false, false, true )
		if success then
			self:add_image_to_table( images, file_name, bind )
			bind = bind + 1
		else
			self:print( "Error: Failed to load image: " .. file_name )
		end
	end
	return images
end
