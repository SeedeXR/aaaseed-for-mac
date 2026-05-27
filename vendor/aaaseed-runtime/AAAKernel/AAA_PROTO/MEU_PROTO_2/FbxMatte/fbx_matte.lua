function meu:define_meu_infos( )
	return { author = "Mâa Franz mrvux Romain",
			tags = { "3D", "Draw", "Core", "CoreGraphic", "Geometry" },
			help = "Draw static geometry, encapsulate c_bdd_mesh_static reading fbx static geometry.\n"..
					"compatible with AAASeed Gbuffer rendering\n"..
					"used a lot for Monaco Aquarium static geometry exported from 3DMax\n"..
					"probably a little flaky as much as the Fbx format is weird and implementations can diverge"
			 }
end


function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	--self:add_rgbf(	{1,4.5 } )

	--self:add_bu_texture_target_unit()
	self.b_load_tried = false

	self.b_day = true	-- Day or Night
	self.b_texs_loaded = false
	self.b_mesh_loaded = false

	self.b_draw_index = false
	self.b_draw_layer = false
	self.b_uv_tex = false
	self.mesh_fbx_index = 0
	self.mesh_nb = 0
	self.b_limit_layer = false
	self.b_mesh_color = false

	self:add_camera()
	self:add_rendering()

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	local fbx = ref.bdd_fbx


	iy = 2
--	bu = self:add_button(	{		ix, iy,		SY, SY },	"Draw Cube", 	self, "b_draw_cube", false )
--	iy = iy + SY

	ui.bu_info = self:add_text_info(	{ix,iy,	16,SY},	"Info" )

	self:set_tab_key( "Main" )
	iy = iy + SY
	bu = self:add_button(	{		ix, 	iy,			SY, SY },	"Draw FBX", 		self, "b_draw_fbx",	true )
	iy = iy + SY
	bu = self:add_button(	{		ix, 	iy,			SY, SY },	"verbose", 			ref.verbose, nil,	true )
	iy = iy + SY
	bu = self:add_button(	{		ix, 	iy,			SY, SY },	"Validate at load", ref.validate, nil,	true )
	iy = iy + SY

	local SX = (8)/3
	bu = self:add_trig_method(	{	ix,      iy,		SX, SY },	"FBX.Load", 		self, "load_fbx" )
		bu:set_text( "Load" ):set_color_back()
	bu = self:add_trig_method(	{	ix+SX,   iy,		SX, SY },	"FBX.ReLoad", 		self, "reload_fbx" )
		bu:set_text( "ReLoad" ):set_color_back()
	bu = self:add_trig_fn(	{		ix+SX*2, iy,		SX, SY },	"FBX.Focus",		aaa.obj.set_focus_ui, ref.bdd_fbx	)
		bu:set_text( "Focus" ):set_color_back()
	bu = self:add_trig_method(	{	ix+SX*2, iy - SY,	SX, SY },	"FBX.ReLoadTex", 		self, "reload_texture" )
		bu:set_text( "ReLoad Texs" ):set_color_back("reload")

--	iy = iy + SY
--	bu = self:add_button(	{		ix, iy,				SY, SY },	"SDK evaluator", 	ref.use_sdk, nil, true )

	iy = iy + SY
	bu = self:add_button(	{		ix, iy,				SY, SY },	"Reverse Layer Order", 	self, "b_draw_reverse",	false )

	iy = iy + SY
	-- Draw one FBX layer
	bu = self:add_button(	{		ix, iy,				SY, SY },	"Draw by Layer", 	self, "b_draw_layer",	false )
	ui.bu_layer_index = self:add_slider(	{ix+4,iy,	4, SY },	"Index",			self, "mesh_index",		0, 0, 16 ):set_value_type_integer(true)
	iy = iy + SY
	bu = self:add_button(	{		ix, iy,				SY, SY },	"Draw by Index", 	self, "b_draw_index",	false )
	iy = iy + SY

	-- Draw a range of layers
	bu = self:add_button(	{		ix, iy,				SY, SY },	"Limit Layer Index", 	self, "b_limit_layer",	false )
	ui.bu_layer_min = self:add_slider(	{ix+4,iy,		4, SY },	"Min Layer Index",		self, "min_layer_index",		0, 0, 30 ):set_value_type_integer(true)
	iy = iy + SY
	bu = self:add_slider(	{ix+4,iy,	4,SY},	"Max Layer Index",		self, "max_layer_index",		0, 0, 30 ):set_value_type_integer(true)
		bu:set_value_load_save( false )
	 	ui.bu_layer_max = bu

	iy = iy + SY

	-- Use UV texture as albedo for debugging purpose
	bu = self:add_button(	{		ix, iy,				SY, SY },	"Use UV Tex", 		self, "b_uv_tex",		false )
	iy = iy + SY
	-- Use colors for debugging purpose
	bu = self:add_button(	{		ix, iy,				SY, SY },	"Use Color", 		self, "b_mesh_color",		false )
	iy = iy + SY

	-- Draw one FBX layer
	ui.bu_mesh_nb = self:add_text_info(	{ix,iy,		16,SY * .5 },	"Mesh Nb" )
	ui.bu_mesh_name = self:add_text_info(	{ix,iy + SY * .5,	16,SY * .5},	"Mesh Name" )

--	bu = self:add_button(	{		ix+1, iy,			SY,SY },	"Grid", 			ref.bdd_fbx, "grid_display" )
	iy = iy + SY + DY

	ix,iy = 9,3
	SY = 2.5
	local SXM = 12/5
	if true then
		for i=1,5 do
			self:add_monitor_id( {ix+8-SXM,iy+(i-1)*SXM, SXM,SXM}, BDD_FBX.tex_name[i].."Visu", i	)
		end
	else
		for i=1,4 do
			self:add_bu_texture_target_unit( {9,iy, nil,SY}, BDD_FBX.tex_name[i], i	)
			--todo this means if we change it will not be save find a solution
			self:set_bu_texture_save( i, false )
			iy = iy + SY
		end
	end

	self:set_tab_key( "TEX" )
	ix,iy = 9,3
	SY = 12/5
--	ui.bu_mon = {}
	for i=1,5 do
		local tex = self:add_bu_texture_target_unit( {9,iy, nil,SY}, BDD_FBX.tex_name[i], i, false	)
		--todo this means if we change it will not be save find a solution
		self:set_bu_texture_save( i, false )
		tex:set_send( false )
		iy = iy + SY
	end

end

function meu:init()
	local ref = self.ref

	self.b_static = true

	--todomonaq use BDD_FBX as for non matte stuff
	local bdd_fbx = self:get_layer_bdd( 2 )
	ref.bdd_fbx = bdd_fbx
	local layer = self:get_layer( 1 )
	ref.layer_attr = layer

--	ref.px = param.get_ref( bdd_fbx, "position_x" )
--	ref.py = param.get_ref( bdd_fbx, "position_y" )
--	ref.pz = param.get_ref( bdd_fbx, "position_z" )
	ref.filename_fbx	= param.get_ref( bdd_fbx, "filename" )
--	ref.time			= param.get_ref( bdd_fbx, "Time" )
	ref.validate		= param.get_ref( bdd_fbx, "validate_at_load" )
	ref.fbx_loaded		= param.get_ref( bdd_fbx, "fbx_loaded" )
	ref.verbose			= param.get_ref( bdd_fbx, "verbose" )
	ref.mesh_index		= param.get_ref( bdd_fbx, "mesh_index")
	ref.mesh_nb			= param.get_ref( bdd_fbx, "mesh_nb")
	ref.mesh_name		= param.get_ref( bdd_fbx, "mesh_name")

	ref.translate_x		= param.get_ref( bdd_fbx, "translate_x" )
	ref.translate_y		= param.get_ref( bdd_fbx, "translate_y" )
	ref.translate_z		= param.get_ref( bdd_fbx, "translate_z" )

	ref.texture_nb				= param.get_ref( bdd_fbx, "texture_nb" )
	ref.texture_index			= param.get_ref( bdd_fbx, "texture_index" )
	ref.texture_path			= param.get_ref( bdd_fbx, "texture_path" )
	ref.texture_normal			= param.get_ref( bdd_fbx, "normal_texture_index" )
	ref.texture_bump			= param.get_ref( bdd_fbx, "bump_texture_index" )
	ref.texture_diffuse			= param.get_ref( bdd_fbx, "diffuse_texture_index" )
	ref.texture_specular		= param.get_ref( bdd_fbx, "specular_texture_index" )
	ref.texture_specular_factor	= param.get_ref( bdd_fbx, "specular_factor_texture_index" )
	ref.texture_shininess		= param.get_ref( bdd_fbx, "shininess_texture_index" )
	ref.texture_emissive		= param.get_ref( bdd_fbx, "emissive_texture_index" )


--	param.set_save( ref.time,		false );
--	param.set_save( ref.use_sdk,	false );
	param.set_save( ref.validate,	false );
	param.set_save( ref.verbose,	false );
	param.set_save( ref.mesh_index,	false );

	self.last_tex = {}
	self.last_tex.albedo = -42
	self.last_tex.metal = -42
	self.last_tex.roughness = -42
	self.last_tex.normal = -42
	self.last_tex.emissive = -42

	self.b_verbose_load = false
end

function meu:set_focus_on_obj()
	local bdd = self.ref.bdd_fbx
	if bdd then	--no bdd in proto
		bdd:set_focus()
	end
end

function meu:load_fbx()
	param.do_action_open( self.ref.bdd_fbx, "filename" )
end
function meu:reload_fbx()
	param.set( self.ref.bdd_fbx, "reload_trig", true )
	self.b_texs_loaded = false
end

function meu:assign_fbx( name, preload )
	self:print( "Setting fbx to "..name )
	local ref = self.ref
	self.fbx_name = name
	local fbx_matte = app:get_fbx_matte()
	local fbx_data = fbx_matte[ name ]
	if fbx_data then
		local path = app.media_dir_rel..app.matte_path..fbx_data.path.."/"..fbx_data.fbx_fname..".FBX"
		self:print( "Setting FBX path to "..path )
		param.set( ref.filename_fbx, path )
		param.set_save( ref.filename_fbx, false )
		if not fbx_data.layer_count or fbx_data.layer_count == 0 then
			fbx_data.layer_count = #fbx_data.layers
		end

		if preload then
			self:print( "FBX "..name.." : preloading textures" )
			self:load_textures()
		end
	end
end


function meu:basename(path)
	local path_find = path:find("\\[^\\]*$")
	if path_find == nil then
		return path
	else
		return path:sub(path_find + 1)
	end
end

function meu:get_texture_list()
	local ref = self.ref
	self.texture_nb = param.get( ref.texture_nb )
	self:print( "FBX Matte detecting "..self.texture_nb.." textures" )
	for i = 0, self.texture_nb - 1 do
		local texture = {}
		param.set( ref.texture_index, i )
		param.set_save( ref.texture_index, false )
		aaa.obj.update( ref.bdd_fbx )
		local path = param.get( ref.texture_path )
		if path then
			texture.name = self:basename( path )
		end
		--self:print( "Texture Filename "..texture.name )
		texture.bind = 0
		self.textures[ i ] = texture
	end
	--table.print( self.textures, "self.textures", 3 )
end

function meu:get_mesh_list()
	local ref = self.ref
	local ui = self.ui
	self.mesh_nb = param.get( ref.mesh_nb )
	self.mesh_indexes = {}
	self:print( "FBX Matte detecting "..self.mesh_nb.." meshes" )
	for i = 0, self.mesh_nb - 1 do
		param.set( ref.mesh_index, i )
		aaa.obj.update( ref.bdd_fbx )
		local mesh_name = param.get( ref.mesh_name )
		self:print( "Mesh "..i.." :"..mesh_name )
		self.mesh_indexes[ mesh_name ] = i
	end
	self.b_mesh_loaded = true
	self.ui.bu_layer_min:set_min_max_value( 0, self.mesh_nb - 1, 0 )
	self.ui.bu_layer_max:set_min_max_value( 0, self.mesh_nb - 1, self.mesh_nb - 1 )
	self.ui.bu_layer_index:set_min_max_value( 0, self.mesh_nb - 1)
	--table.print( self.mesh_indexes, "self.mesh_indexes", 3 )
end

function meu:load_texture_low( path, premultiply )
	local bind_tex = 0
	local img	-- todo we don't need an img a bind should be enough
	if premultiply then
		img = IMGS.get_img_premultiply( path )
	else
		img = IMGS.get_img( path )
	end
	if img then
		local bind = img:get_bind()
		bind_tex = bind
		if self.b_verbose_load then
			self:print( "FBX "..self.fbx_name.." : "..path.." at bind "..bind )
		end
	else
		self:print( "Could not load img :  "..path )
	end
	return bind_tex
end

function meu:load_texture_from_list( index )
	local bind_tex = 0
	--self:print( "load_texture_from_list" )
	if index >= 0 and index < self.texture_nb and self.textures[ index ] then
		local name_pure	= aaa.file.get_name_pure( self.textures[ index ].name )
		local astc_name = name_pure..".dds"
		if app:use_compressed_texture() then
			bind_tex = self:load_texture_low( astc_name, false )
		end
		if bind_tex == 0 then
			bind_tex = self:load_texture_low( self.textures[ index ].name, true )
		end
		self.textures[ index ].bind = bind_tex
	else
		if self.b_verbose_load then
			self:print( "Can't get texture from list "..index )
		end
	end
	return bind_tex
end


-- load textures from fbx : extract materials from fbx
function meu:load_textures_from_fbx()
	local ref = self.ref
	self.textures = {}
	self.texture_nb = 0
	local fbx_data = app:get_fbx_matte( true )
	local data = fbx_data and fbx_data[ self.fbx_name ]
	if data then
		aaa.obj.update( ref.bdd_fbx )
		self:get_mesh_list()
		self:get_texture_list()

		aaa.img.set_free_when_on_board( true )
	--	self:print( "app.texs_no_normal_path "..app.texs_no_normal_path )
		MEDIA.set_dir_media( app.texs_no_normal_path )
		local texture = {}
		texture.name = "NO_NORMAL"
		texture.bind = self:load_texture_low( "MAP_NO_NORMAL.tga", true )
		self.textures[ self.texture_nb ] = texture
	--	self:print( "Bind MAP_NO_NORMAL.TGA "..texture.bind)
		local texture_black = {}
		texture_black.name = "BLACK"
		texture_black.bind = self:load_texture_low( "BLACK.tga", true )
		self.textures[ self.texture_nb + 1 ] = texture_black

		MEDIA.set_dir_media( app.matte_path..data.path )
			app:enable_img_scale_on_load()

		self.mesh_nb = param.get( ref.mesh_nb )
		self.mesh_material = {}

		-- get materials for each layer we need to display
		for i = 1, data.layer_count do
			local plane = data.layers[ i ]
			local material = {}
			material.albedo 	= 0
			material.normal		= 0
			material.metal		= 0
			material.roughness	= 0
			material.emissive	= 0
			local mesh_index = self.mesh_indexes[ plane.mesh_name ]
			if mesh_index then
				if plane.tex_name and plane.tex_name ~= "" then
					--self:print( "Getting Texture from lua")
					-- get texture name from lua
					local ext = ".tga"
					local astc_ext = ".dds"
					if data.b_night then
						ext = "_NIGHT.tga"
						astc_ext = "_NIGHT.dds"
					end
					for i = 1, 5 do
						local fname = plane.tex_name.."_"..BDD_FBX.tex_name_for_file[i]..ext
						local fname_dds = plane.tex_name.."_"..BDD_FBX.tex_name_for_file[i]..astc_ext
						local name = BDD_FBX.tex_name[i]:lower()
						local bind = 0
					--	self:print( "Getting texture "..name.." : "..fname )
						if app:use_compressed_texture() then
							bind = self:load_texture_low( fname_dds, false )
						end
						if bind == 0 then
							bind = self:load_texture_low( fname, true )
					--		material[ name ] = self:load_texture_low( fname, true )
							if bind == 0 then
								if i == 2 then
									if self.b_verbose_load then
										self:print( "Use NO normal map at bind "..self.textures[ self.texture_nb ].bind)
									end
									material[ name ] = self.textures[ self.texture_nb ].bind
								elseif i == 3 then
									if self.b_verbose_load then
										self:print( "Use BLACK for METAL")
									end
									material[ name ] = self.textures[ self.texture_nb + 1 ].bind
								elseif i == 4 then
									if self.b_verbose_load then
										self:print( "Use BLACK for ROUGHNESS ")
									end
									material[ name ] = self.textures[ self.texture_nb + 1 ].bind
								elseif i == 5 then
									if self.b_verbose_load then
										self:print( "Use BLACK for EMISSIVE ")
									end
									material[ name ] = self.textures[ self.texture_nb + 1 ].bind

								end
							else
								material[ name ] = bind
							end
						else
							material[ name ] = bind
						end
					end
					self.mesh_material[ mesh_index ] = material
				else
					param.set( ref.mesh_index, mesh_index )
					aaa.obj.update( ref.bdd_fbx )

					-- ALBEDO
					local albedo_index = param.get( ref.texture_diffuse )
					material.albedo = self:load_texture_from_list( albedo_index )

					-- NORMAL
					local normal_index = param.get( ref.texture_normal )
					--self:print( "normal_index "..normal_index )
					if  normal_index == -1 then
						normal_index = param.get( ref.texture_bump )
				--		self:print( "bump index "..normal_index )
					end
					if normal_index == -1 then
						if self.b_verbose_load then
							self:print( "no normal use NO NORMAL ".. self.textures[ self.texture_nb ].bind )
						end
						material.normal = self.textures[ self.texture_nb ].bind
					else
						if self.textures[ normal_index ] and self.textures[ normal_index ].name == "" then
							if self.b_verbose_load then
								self:print( "Texture path is empty, use the next texture index" )
							end
							normal_index = normal_index + 1
						end
						material.normal = self:load_texture_from_list( normal_index )
					end
					-- ROUGHNESS
					local rough_index = param.get( ref.texture_shininess )
					material.roughness = self:load_texture_from_list( rough_index )
					if material.roughness == 0 then
						if self.b_verbose_load then
							self:print( "Use BLACK for ROUGHNESS" )
						end
						material.roughness = self.textures[ self.texture_nb + 1 ].bind
					end

					-- METAL
					local metal_index = param.get( ref.texture_specular )
					if  metal_index == -1 then
						metal_index = param.get( ref.texture_specular_factor )
					end
					material.metal = self:load_texture_from_list( metal_index )
					if material.metal == 0 then
						if self.b_verbose_load then
							self:print( "Use BLACK for METAL" )
						end
						material.metal = self.textures[ self.texture_nb + 1 ].bind
					end

					-- EMISSIVE
					local emissive_index = param.get( ref.texture_emissive )
					material.emissive = self:load_texture_from_list( emissive_index )
					if material.emissive == 0 then
						if self.b_verbose_load then
							self:print( "Use BLACK for EMISSIVE" )
						end
						material.emissive = self.textures[ self.texture_nb + 1 ].bind
					end

				end
				--self:print( "Adding material for mesh "..mesh_index )
				--table.print( material, "material", 2 )
				self.mesh_material[ mesh_index ] = material
			else
				self:print_error( "Files are probably missing for FBX : "..self.fbx_name )
			end
		end
		app:disable_img_scale_on_load()
		--table.print( self.mesh_material, "self.mesh_material", 3 )
		self:update_textures( 0, true )
		self.b_texs_loaded = true
	end
end

function meu:update_ui()
	local ui = self.ui
	local ref = self.ref
	ui.bu_info:set_text( param.get( ref.filename_fbx ) )
--	self:print( ref.bdd_fbx )
	ui.bu_mesh_nb:set_text( "Mesh Nb : "..self.mesh_nb )
	local mesh_name = ""
	local mesh_index = ""
	if self.b_draw_layer then
		mesh_name =  param.get( ref.mesh_name )
		mesh_index = self.mesh_fbx_index
	end

	ui.bu_mesh_name:set_text( mesh_name )

end

function meu:update_shader_offset( value )
	--todo cache this
	local m = self:get_meu_by_name_no_error( "MatteShader_1" )
	if m then
		--todo just update the add_shading_ui
	--	self:print( "Settings y offset for plane "..i.." : "..plane.y_offset)
		m:set_vertical_offset( value )
		local discard = self.b_uv_tex and 0 or 1
		discard = 0
		m:set_discard_uv( discard )
		--don't need draw because fn update_unform now
		--m:draw()
	else
		self:print( "Can't get MatteShader " )
	end
end

function meu:set_tex_unit( id, bind, b_update_ui )
	if bind then
		if b_update_ui then
			self:set_texture_bind_2d( id + 1, bind )
		end
		gol.set_tex_unit_2d_bind( id, bind )
	else
		gol.set_tex_unit_2d_bind( id, 0 )
	end
end

function meu:update_textures( mesh_index, b_update_ui )
	local fbx_matte = app:get_fbx_matte()
--	table.print( self.mesh_material[ mesh_index ], "self.mesh_material[ mesh_index ]" )
	if mesh_index < self.mesh_nb then
		local material = self.mesh_material[ mesh_index ]
		if material then
			b_update_ui = true
--			table.print( material, "material" )
			if self.b_uv_tex then
				gol.set_tex_unit_2d_bind( 0, 1 )
			else
				--if self.last_tex.aldedo ~= material.albedo then
					self:set_tex_unit( 0, material.albedo, b_update_ui )
				--	self.last_tex.albedo = material.albedo
				--end
			end
			--if self.last_tex.normal ~= material.normal then
				self:set_tex_unit( 1, material.normal, 		b_update_ui )
			--	self.last_tex.normal = material.normal
			--end
			--if self.last_tex.roughness ~= material.roughness then
				self:set_tex_unit( 2, material.roughness,	b_update_ui )
			--	self.last_tex.roughness = material.roughness
			--end
			--if self.last_tex.metal ~= material.metal then
				self:set_tex_unit( 3, material.metal,		b_update_ui )
			--	self.last_tex.metal = material.metal
			--end
				self:set_tex_unit( 4, material.emissive,		b_update_ui )
		end
	end
end

function meu:update_shader_color( mesh_index )
	--todo cache this
	local m = self:get_meu_by_name_no_error( "MatteShader_1" )
	if m and mesh_index >= 0 then

		m:set_use_color( self.b_mesh_color )
		local color_index = ( mesh_index + 1 + 27 ) % #BU_COLOR.COLORS + 1
		--self:print( "color "..color_index.. " : "..COLORS[color_index].name )
		m:set_mesh_color( BU_COLOR.COLORS[color_index] )
		--don't need draw because fn update_unform now
	else
		self:print( "Can't get MatteShader " )
	end
end

function meu:draw_attr()
	aaa.obj.update_then_draw( self.ref.layer_attr )	--	set the "attribute" no more shader here
end

function meu:draw_matte_elt( mesh_index, uv_offset, translation, b_update_tex_ui )
	if not mesh_index then
		return
	end
	param.set( self.ref.mesh_index, mesh_index )
	if translation then
		param.set( self.ref.translate_x, translation.x )
		param.set( self.ref.translate_y, translation.y )
		param.set( self.ref.translate_z, translation.z )
	else
		param.set( self.ref.translate_x, .0 )
		param.set( self.ref.translate_y, .0 )
		param.set( self.ref.translate_z, .0 )
	end
	self:update_shader_offset( uv_offset )
	self:update_shader_color( mesh_index )
	self:update_textures( mesh_index, b_update_tex_ui )
	aaa.obj.draw( self.	ref.bdd_fbx )
end

function meu:reload_texture()
	self.b_texs_loaded = false
end

function meu:draw_fbx_matte( matte )
	aaa.obj.update( self.ref.bdd_fbx )
	if self.mesh_index < 0 then
		self.mesh_index = 0
	elseif self.mesh_index >= self.mesh_nb then
		self.mesh_index = self.mesh_nb - 1
	end
	if self.min_layer_index < 0 then
		self.min_layer_index = 0
	end
	if self.max_layer_index >= self.mesh_nb then
		self.max_layer_index = self.mesh_nb - 1
	end

	if self.b_draw_layer then
		if self.b_draw_index then
			self.mesh_fbx_index = self.mesh_index
		--	self:print( "Mesh index "..self.mesh_index)
			--local layer = matte.layers[ mesh_index + 1 ]
			self:draw_matte_elt( self.mesh_index, nil, nil, true )
		else
			local layer_index = clamp( self.mesh_index, 0, matte.layer_count-1 )
			local layer = matte.layers[ layer_index + 1 ]
			--self:print( "hhh layer "..layer_index)
			if layer.mesh_name == "" then
				self:draw_matte_elt( layer.mesh_index, layer.uv_offset, layer.offset, true )
				self.mesh_fbx_index = layer.mesh_index
			else
				self.mesh_fbx_index = self.mesh_indexes[ layer.mesh_name ]
				self:draw_matte_elt( self.mesh_fbx_index, layer.uv_offset, layer.offset, true )
			end
		end
	else
		local layer_end = matte.layer_count
		local layer_start = 1
		if self.b_limit_layer then
			layer_start = self.min_layer_index + 1
			if layer_start > matte.layer_count then
				layer_start = matte.layer_count
			end
			layer_end = self.max_layer_index + 1
			if layer_end > matte.layer_count then
				layer_end = matte.layer_count
			end
			if layer_start < 1 then
				layer_start = 1
			end
		end
		local inc = 1
		if self.b_draw_reverse then
			local start = layer_start
			layer_start = layer_end
			layer_end = start
			inc = -1
		--	self:print( "Jere "..layer_start.." : "..layer_end )
		end
		for i = layer_start, layer_end, inc do
			--self:print( "layer index "..i )
			local layer = matte.layers[ i ]
			if layer.mesh_name == "" then
				self:draw_matte_elt( layer.mesh_index, layer.uv_offset, layer.offset, false )
			else
				--self:print( "drawing "..self.mesh_indexes[ layer.mesh_name ])
				self:draw_matte_elt( self.mesh_indexes[ layer.mesh_name ], layer.uv_offset, layer.offset, false )
			end
		end
	end
end

function meu:draw()
	local ref = self.ref
	--aaa.obj.set_focus_ui( bdd_fbx )
--	local layer = self:get_layer( 2 )
--	aaa.obj.set_focus_ui( layer )
--	self.b_texs_loaded = false

--self:print( "here "..self.fbx_name )
	local b_fbx_loaded = param.get( ref.fbx_loaded )
	if b_fbx_loaded == 0 then
		if self.b_load_tried == false then
			self.b_load_tried = true
			if not self.fbx_name then
				app:get_fbx_matte()
			end
			param.set( self.ref.bdd_fbx, "reload_trig", true )
			aaa.obj.update( ref.bdd_fbx )
			b_fbx_loaded = param.get( ref.fbx_loaded )
			if b_fbx_loaded == 0 then
				return
			end
		else
			return
		end
	end
	if self.b_texs_loaded == false then
		self:load_textures_from_fbx()
	end

	if self.b_mesh_loaded == false then
		self:get_mesh_list()
	end

	if self.b_draw_fbx then
		local fbx_matte = app:get_fbx_matte()
		--todo do a get_matte( name )
		local matte = fbx_matte[ self.fbx_name ]
		self:draw_layers_begin()
			self:draw_attr()	--	set the "attribute" no more shader here
			if matte then
			--	gol.set_wrap_2d_repeat()
			--	self:draw_layer( 1 )
			--	aaa.obj.update( self.ref.bdd_fbx )
				self:draw_fbx_matte( matte )
			else
			--	self:print( "No matte : hack Maa" )
				self:draw_layer( 2 )
			end
		self:draw_layers_end()
	end
end

