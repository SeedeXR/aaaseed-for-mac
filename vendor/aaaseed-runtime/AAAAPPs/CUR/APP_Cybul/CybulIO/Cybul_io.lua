function CYBUL:save_process( folder )
	self:save_preset( folder )
end

function CYBUL:get_preset()
	local tab = {}
	for key,bu in pairs( self.ui.bus ) do
		if bu.get_value and bu.set_value and (not bu.is_trig or not bu:is_trig()) then
			local name = bu:get_name()
			if tab[name] then
				self:print( "Error: "..name.." already in preset" )
			end
			tab[name] = bu
		end
	end
	return tab
end

--etienne todo : une petite fns pour encapsuler (et donc expliciter) la repetition
function CYBUL:save_preset( folder )
	local preset = self:get_preset()
	if not preset then return end

	local fname
	if folder then
		if folder[#folder] == "/" then fname = folder .. "config"
		else						   fname = folder .. "/config"
		end
	else
		fname = aaa.file.do_dialog_save( "Parametres", "" )
	end

	if not fname then return end
		
	local file = aaa.file.open( fname, "w" )
	if file then
		local function write_to_file( key, val )
			local str = "tab[\""..key.."\"] = "
			str = self:serialize_to_str( str, val )
			self:print( str )
			str = str.."\n"
			file:write( str )
		end
		local function write_slider_multi( bu_key, bu )
			for i=1,bu:get_elt_nb() do
				local elt = bu:get_elt( i )
				local x, y = elt:get_xy()
				local key = bu_key.."_"..i.."_"
				write_to_file( key .. "x", x )
				write_to_file( key .. "y", y )
			end
		end

		file:write( "-- preset v0 for "..self, "\n" )

			-- preset
			for key,bu in pairs_sorted( preset ) do
				write_to_file( key, bu:get_value() )
			end	

			-- dots + compass
			write_slider_multi( "ELT", self.ui_bu_dots )
			write_slider_multi( "CMP", self.ui_bu_compass )

			-- image path
			write_to_file( "PHOTO", aaa.img.get_bind_filename( self.photo.bind ) )

			local function get_name( tex )
				local str 
				if tex then
					str = tex:get_name()
				end
				return str or ""
				--was	return (tex and tex.__name) and tex.__name or ""
			end

			-- logo path
			write_to_file( "LOGO", get_name( self.tab3d.tex_lambrequin_lumineux ) )
			-- hessian path
			write_to_file( "HESSIAN", get_name( self.tab3d.tex_toile ) )
			-- zips path
			write_to_file( "ZIPS", get_name( self.tab3d.tex_zips ) )

		file:close()
	end
end

function CYBUL:load_process( bu )
	self:load_preset()
	self:draw_photo_dots( self.ui_bu_dots ) -- a bit brutal
	-- self:update_perspective()
end

function CYBUL:load_preset( filename )
	if not filename then
		filename = aaa.file.do_dialog_open( "Parametres" )
	end

	if not filename then return end
	if not aaa.file.is_exist( filename ) then return end

	local preset = self:get_preset()
	local loaded = {}
	app:dofile( filename, loaded )

	local bu_dots = self.ui_bu_dots
	local bu_compass  = self.ui_bu_compass
	local elt, id, coord

	local bu, x, y
	local key_number
	for key, val in pairs_sorted( loaded ) do
		if val ~= nil then
			key_number = key

			if key_number ~= "Stage" and preset[key_number] then
				bu = preset[key_number]
				if val == true or val == false then
					-- hack Etienne
					-- set to opposite value first to force a method_on_value_change call
					preset[key_number]:set_value( not val )
				end
				preset[key_number]:set_value( val )
			else
				if string.sub( key, 1, 3 ) == "ELT" then
					local id    = string.sub( key, 5, #key - 2 )
					local coord = string.sub( key, #key, #key )

					elt = bu_dots:get_elt( tonumber( id ) )
					if coord == "x" then elt:set_x( val ) end
					if coord == "y" then elt:set_y( val ) end
				elseif string.sub( key, 1, 3 ) == "CMP" then
					local id    = string.sub( key, 5, #key - 2 )
					local coord = string.sub( key, #key, #key )

					elt = bu_compass:get_elt( tonumber( id ) )
					if coord == "x" then elt:set_x( val ) end
					if coord == "y" then elt:set_y( val ) end
				elseif string.sub( key, 1, 5 ) == "PHOTO" then
					self:print( "image name = ", val )
					self:load_image( val )
				elseif string.sub( key, 1, 7 ) == "HESSIAN" then
					self:print( "texture toile = ", val )
					self:load_texture( "hessian", val )
				elseif string.sub( key, 1, 4 ) == "ZIPS" then
					self:print( "texture zips = ", val )
					self:load_texture( "zip_tex", val )
				elseif string.sub( key, 1, 4 ) == "LOGO" then
					self:print( "texture logo = ", val )
					self:load_texture( "logo", val )
				else
					self:print( "No bu associated with key "..key )
				end
			end
		end
	end
end

function CYBUL:load_texture( name, path )
	if path and aaa.file.is_exist( path ) then
		local img = IMGS.get_img( path )
		if name == "hessian" 	then self.tab3d.tex_toile 					= img end
		if name == "zip_tex"  	then self.tab3d.tex_zips  					= img end
		if name == "logo"  		then self.tab3d.tex_lambrequin_lumineux  	= img end
      
		if name ~= "logo" then
	    	self:read_param_file( name, path, img )
		end
	end
end


function CYBUL:export_pergola_text()
	local str = ""

	-- type
	local bu = self.ui.bu_store_selector
	local v = bu:get_value()
	local store_type = bu:get_item_text( v )
	str = str .. store_type .. "\n"

	-- dimensions
	local cybdraw = app:get_cybstores_meu()
	local store_layer = cybdraw:get_layer_bdd( 6 )
	local store_model = aaa.layer.get_model( cybdraw:get_layer( 6 ) )
	local width  = param.get( store_model, "size_u" )
	local height = param.get( store_model, "size_v" )
	local depth = param.get( store_model, "size_axe" )
	local ecart_mur = param.get( store_layer, "center_z" )
	width  = math.floor( 1000 * width ) 
	height = math.floor( 1000 * height )
	depth  = math.floor( 1000 * depth ) 

	str = str .. "Largeur    : " .. width  .."mm\n"
	str = str .. "Profondeur : " .. depth  .."mm\n"
	str = str .. "Hauteur    : " .. height .."mm\n"
	str = str .. "Dimensions : " .. width .. "x" .. depth .. "x" .. height .. "\n"

	local function get_filename( img )
		local path = img and img:get_name()
		if not path then return "" end

		path = str.split( path, "/" )
		path = string.gmatch( path[#path], "(.+)%.(.+)" ) 
		local name,ext = path()	-- because pathis an iterator
		return name
	end

	str = str .. "Toile      : " .. get_filename( self.tab3d.tex_toile ) .. "\n"

	local val
	if store_type == "Banne"       then val = app.tex_armatures.banne_selected       end
	if store_type == "Pergola Mur" then val = app.tex_armatures.pergola_mur_selected end
	if store_type == "Pergola Sol" then val = app.tex_armatures.pergola_selected     end
	if val then
		str = str .. "Armature    : " .. get_filename( val ) .. "\n"
	end

	if    store_type == "Banne" then
		str = str .. "Angle      : " .. app.store.hessian_angle .. "\n"
	elseif store_type == "Pergola Mur" then
		str = str .. "Angle      : " .. app.store.hessian_angle .. "\n"
		str = str .. "Deportage  : " .. app.store.deportage_mur .. "mm\n"
	elseif store_type == "Pergola Sol" then
		bu = self.ui.bu_pergola_type
		v = bu:get_value()
		local pergola_type = bu:get_item_text( v )
	
		str = str .. "Type       : " .. pergola_type .. "\n"
		if pergola_type == "Lames" then
			val = get_filename( self.tab3d.tex_lames )
			str = str .. "Lames      : " .. string.sub( val, 3, #val ) .. "\n"
		end

		local pillar_count = 4
		bu = self.ui.bu_pieds
		local v = bu:get_value()
		if bu:get_item_text( v ) == "6 Pieds" then
			pillar_count = 6
		end
		if ecart_mur == 0 then
			pillar_count = pillar_count / 2
		end
	
		str = str .. "Pieds      : " .. pillar_count .. "\n"
		str = str .. "Deportage  : " .. app.zips.deportage .. "mm\n"

		str = str .. "Zips       : "
		if self.ui.bu_gauche  :get_value() then str = str .. "Gauche " end
		if self.ui.bu_droite  :get_value() then str = str .. "Droite " end
		if self.ui.bu_devant  :get_value() then str = str .. "Devant " end
		if self.ui.bu_derriere:get_value() then str = str .. "Derriere " end
		str = str .. "\n"
	end

	self:print( str )

	aaa.clipboard.move_to( str )
end

function CYBUL:reset_project()
	self:load_default_project()
end

function CYBUL:load_default_project()
	self:load_preset( self:get_dir_absolute().."/config" )
end

function CYBUL:save_project()
	local folder = aaa.file.do_dialog_folder( "Dossier" )
	self:save_process( folder )
	self:save_image( folder )
end

function CYBUL:load_image( name )
	if name and aaa.file.is_exist( name ) then
		--todo define a bind for this
		--todo compute image ratios and use it
		--todo make lib of examples
		local photo = self.photo
		local bind = photo.bind
      	local b_ok = aaa.img.read( bind, name )
		if b_ok then
			self:update_photo_zoom()
		else
			photo.sx, photo.sy = 0,0
			self:box_error( "Je ne peux pas lire le fichier :\n"..name )
		end
	else
		self:print("The image does not exist")
	end
end

function CYBUL:save_image( folder )
	local post = self:get_meu_by_name( "CybulRecadrage_1" )
	local mfbo = post:get_meu_fbo_used()

	local bu = self.ui.bu_image_format
	local v = bu:get_value()
	local format = bu:get_item_text( v )

	mfbo:save_fbo_interactive( format, folder )
end

function CYBUL:get_tex_param_file( name )
	local reg = string.gmatch( name, "(.+)%/([^/]+)%.(.+)" )
	local folder, fname, ext = reg()
	local param_file = folder .. '/' .. fname .. '.tex_def'
	if not aaa.file.is_exist( param_file ) then
		param_file = folder .. '/default.tex_def'
		while not aaa.file.is_exist( param_file ) and folder do
			reg = string.gmatch( folder, "(.+)%/[^/]+" )
			folder = reg()
			param_file = folder .. '/default.tex_def'
		end
	end

	return param_file
end

function CYBUL:read_param_file( tex_type, filename, img )
	if tex_type == "hessian" then self:reset_hessian_parameters() end
	if tex_type == "zip_tex" then self:reset_zip_tex_parameters() end

	local param_file = self:get_tex_param_file( filename )

	if not aaa.file.is_exist( param_file ) then
		self:print_error("No config file for the loaded texture, using default parameters for "..filename)

		return
	end

	self:print("parameter file: "..param_file )
	local file = aaa.file.open( param_file, "r" )
	local line = file:read()

	local table
	if tex_type == "hessian" then table = app.hessian end
	if tex_type == "zip_tex" then table = app.zip_tex end

	if not table then return end

	while line do
		local reg = string.gmatch(line, "(.*)%=(.*)")
		local key,val = reg()
		key = key:gsub("%s+", "")
		val = val:gsub("%s+", "")


		if key == "largeur" then
			table.width  = tonumber(val) * 0.001
			table.height = table.width * img.sy / img.sx
		end

		if key == "options" then
			local options = val:split(",")
			for i=1,#options do
				local option = options[i]
				if option == "repeat_horizontal" then table.gl_hor = "repeat" end
				if option == "repeat_vertical"   then table.gl_ver = "repeat" end
				if option == "miroir_horizontal" then table.gl_hor = "mirror" end
				if option == "miroir_vertical"   then table.gl_ver = "mirror" end
			end
		end

		line = file:read()
	end
end
