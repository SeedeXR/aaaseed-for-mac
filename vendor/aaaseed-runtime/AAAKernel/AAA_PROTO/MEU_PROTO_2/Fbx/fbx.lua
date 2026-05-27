function meu:define_meu_infos( )
	return { author = "Mâa Franz mrvux Romain",
			tags = { "3D", "Art", "Draw", "Core", "CoreGraphic", "Geometry", "Procedural", },
			help = "Encapsulate use of c_bdd_fbx reading fbx animation including Bones influence\n"..
					"deal even with more that 4 bones influences on a point\n"..					
					"compatible with AAASeed Gbuffer rendering\n"..
					"used a lot for Monaco Aquarium Animation and fish exported from 3DMax\n"..
					"probably a little flaky as much as the Fbx format is weird and implementations can diverge"
			 }
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	--self:print( "INITIALIZE FBX")
	self:add_camera()
	self:add_rendering()

	local fbx = ref.bdd_fbx
	local obj = fbx:get_obj()

	iy = 2
--	bu = self:add_button(	{		ix, iy,			SY, SY },	"Draw Cube", 	self, "b_draw_cube", false )
--	iy = iy + SY

	ui.bu_name =
		self:add_text_info(	{ix,iy - 1,		8,SY},		"Name"		)
	ui.bu_info =
		self:add_text_info(	{ix,iy,			16,SY*.65},	"Info"		)

	self:set_tab_key( "Main" )
	iy = iy + SY
	self:add_button(		{ix,		iy,	SY,SY},	"Verbose", 			ref.verbose, nil,			true	)
	self:add_button(		{ix+4, 		iy,	SY,SY},	"Validate at load", ref.validate, nil, 			false	):set_text("Validate")
	iy = iy + SY
	local SX = (8)/3 
	self:add_trig_method(	{ix, 		iy,	SX,SY},	"FBX.Load", 		self, "load"				):set_text( "Load"		):set_color_back()
	self:add_trig_method(	{ix+SX,		iy,	SX,SY},	"FBX.ReLoad", 		self, "reload"				):set_text( "ReLoad"	):set_color_back()
	self:add_trig_method(	{ix+SX*2,	iy,	SX,SY},	"FBX.Focus",		fbx, "set_focus"			):set_text( "Focus"		):set_color_back()
	iy = iy + SY + DY

	self:add_button(		{ix,		iy,	SY,SY},	"SDK evaluator", 	    obj, "use_sdk_evaluator",	    true	):set_text("SDK")
	self:add_button(		{ix+4,		iy,	SY,SY},	"Geometric Transform", 	obj, "use_geometric_transform",	true	):set_text("GEO")
	iy = iy + SY
	self:add_button(		{ix,		iy,	SY,SY},	"Draw Mesh", 		obj,"draw_mesh",			true	):set_text( "Mesh"	)
	iy = iy + SY
	-- Draw one FBX layer
	ui.bu_mesh_nb =
		self:add_text_info(	{ix+4,iy-SY,	4,SY/2},"Mesh Nb" 	)
	ui.bu_mesh_name =
		self:add_text_info(	{ix+4,iy-SY*.5,	4,SY/2},"Mesh Name" )

	self:add_button(		{ix,		iy,	SY,SY},	"Draw by Index", 	self, "b_draw_index",		false )
	ui.bu_index_mesh =
		self:add_slider(	{ix+4,		iy,	4,SY},	"Index",			self, "mesh_index",			0, 0, 16 ):set_value_type_integer(true)
	iy = iy + SY
	self:add_button(		{ix,		iy,	SY,SY},	"Draw Debug", 		obj,"draw_debug",			false	):set_text( "Debug"	)
	self:add_button(		{ix+4,		iy,	SY,SY},	"Curve", 			obj,"draw_curve",			false	):set_text( "Curve"	)
	iy = iy + SY
	self:add_button(		{ix+1,		iy,	SY,SY},	"Position", 		obj,"draw_position",		false	)
	self:add_slider(		{ix+5,		iy,	3,SY}, 	"Position Size", 	obj,"draw_position_size",	1,		0, 8 )
	iy = iy + SY
	self:add_button(		{ix+1,		iy,	SY,SY},	"Bone", 			obj,"draw_bone",			false	)
	self:add_slider(		{ix+5,		iy,	3,SY}, 	"Bone Size", 		obj,"draw_bone_size",		1,		0, 8 )
	iy = iy + SY + DY

	--ui.bu_time = self:add_text_info(	{ix,iy,	16,SY * .65},	"Time"		)
	iy = self:define_time(	{ix,		iy,	1, SY}	)

	iy = iy + SY
	--bu = self:add_button(	{ix, 		iy,	SY, SY },	"Play", 			obj,"play",					true	)
	--bu = self:add_slider(	{ix+4,		iy,	4,SY}, 	"Speed", 			obj,"play_time_factor",		1,		0, 4 )

	ix,iy = 9,3

	local SXM = 12/5
	if true then
		for i=1,5 do
			self:add_monitor_id( {ix+8-SXM,iy+(i-1)*SXM, SXM,SXM}, BDD_FBX.tex_name[i].."Visu", i	)
		end
	else
		for i=1,4 do
			local x = i - 1
			local y = math.floor(x/2)
			x = x - y * 2
			self:add_monitor_id( {ix+x*4,iy+y*4, 4,4}, name[i].."Visu", i	)
		end
	end

	ui.bu_blend_shape_nb	= self:add_text_info(	{ix,iy,	8,SY*.5},	"Blend Shape Nb" )
	iy = iy + SY*.5
	local SX = 2
	local id = 1
	local fy = 1
	for l=1,8 do
		for i=1,1 do
			local x = ix + (i-1)*SX
			local pre = "blend_shape_"..id
			local name = pre.."_manual"
			bu = self:add_button(	{x, iy, SY*fy, SY*fy },	name, fbx:get_param_ref( name ), nil, false ):set_text(id)
				bu:set_text_x(-1)
			name = pre.."_factor"
			local ref_bf = fbx:get_param_ref( name )
			bu = self:add_slider(	{x+SY*fy,iy,	8-SXM-SY*fy,SY*fy}, 	name, ref_bf, nil, 0 ):set_text("factor "..id)
				param.set_save( ref_bf, false )
				bu:set_value_load_save( false )
			id = id + 1
		end
		iy = iy + SY*fy
	end

	self:set_tab_key( "TEX" )
	ix,iy = 9,3
	SY = 12/5
--	ui.bu_mon = {}
	for i=1,5 do
		self:add_bu_texture_target_unit( {9,iy, nil,SY}, BDD_FBX.tex_name[i], i, false	)
		--todo this means if we change it will not be save find a solution
		self:set_bu_texture_save( i, false )
		iy = iy + SY
	end


	self:set_tab_key( "Animations" )
	SY = .9
	ix = 1
	iy = 2
	iy = iy + SY
	bu = self:add_trig_method(	{	ix, iy,			SX, SY},	"FBX.InfoAnims",	self, "update_animations_info"	)
		bu:set_text( "Anims Info" )
	bu = self:add_trig_method(	{	ix + SX, iy,	SX, SY},	"FBX.SaveAnims",	self, "save_anims"	)
		bu:set_text( "Save Anims" ):set_color_back("save")
	iy = iy + SY
	ui.bu_animation = self:add_text_info(	{ix,iy,	16,SY * .5},	"Animation Nb" )
	iy = iy + SY *.5
	ui.bu_animation_time = self:add_text_info(	{ix,iy,	16,SY * .5},	"Animation Time :" )
	iy = iy + SY *.5
	--self:set_tab_key_def()
end

function meu:init_data()
	self.b_texs_loaded = false
	self.b_animations_loaded = false
	self.mesh_nb = 0
	self.animation_nb = 0
	self.b_static = false
	self.tex_seq_index = 1
	self.b_tex_seq = false
end

function meu:init()
	local ref = self.ref

	self.b_set_time_max = false
	self.b_verbose_load = false
	self.b_load_tried = false
	self.b_mesh_loaded = false
	self.b_animation = false
	self:init_data()

	local layer = self:get_layer( 1 )
	ref.layer_attr = layer

	layer = self:get_layer( 2 )
	ref.layer_fbx = layer

	param.set( layer, "use_multiple",		"none"	)
	param.set( layer, "shader_bdd_only",	1		)

	local bdd_fbx = BDD_FBX:create( "bdd_fbx_"..self:get_inst_key(), self:get_layer_bdd( 2 ) )
	ref.bdd_fbx = bdd_fbx

	bdd_fbx:set_time_update( true )

	bdd_fbx:set_param(		"play",					false )

	bdd_fbx:set_param_save( "play",					false );
	bdd_fbx:set_param_save( "time",					false );
	bdd_fbx:set_param_save( "use_sdk_evaluator",	false );
	bdd_fbx:set_param_save( "validate_at_load",		false );
	bdd_fbx:set_param_save( "verbose",				false );
end

--todo seems unused : remove ?
function meu:set_tex_seq_index( index )
	self.tex_seq_index = index
end

function meu:get_tex_seq_bind()
	return self.tex_seq_bind_begin, self.tex_seq_bind_end
end

function meu:get_bdd_fbx()		return self.ref.bdd_fbx		end
function meu:get_layer_fbx()	return self.ref.layer_fbx	end

function meu:set_focus_on_obj()
	local bdd = self.ref.bdd_fbx
	if bdd then	--no bdd in proto
		bdd:set_focus()
	end
end

function meu:set_time( value )
	self.time = value
	self.ref.bdd_fbx:set_time( value )
end
function meu:get_time()		return self.time									end

function meu:restart()
	self:restart_time()
	self:print( "Restart Time")
end

function meu:set_play( b )	param.set( self.ref.bdd_fbx.ref.obj, "play", b )	end


function meu:time_set_play( b )
	self:print( "Time set play "..b )
	self:time_play( b )
end

function meu:start()		self:set_play( true )								end
function meu:stop()			self:set_play( false )								end

--todomona data inited in init_data should be dealt with in BDD_FBX ?
function meu:load()			self:init_data()	self.ref.bdd_fbx:load()			end
function meu:reload()		self:init_data()	self.ref.bdd_fbx:reload()		end

function meu:update_animations_info()
	local ref = self.ref
	local bdd_fbx = ref.bdd_fbx
	self.animations = {}
--	self:print( "getting animations")
	if not bdd_fbx then return end
	self:print( "getting animations")
	-- we want to get the internal rotation for each anim for debug purpose
	bdd_fbx:set_param( "get_rotation", true )
	--param.set( ref.get_rotation, true )
	self:set_play( false )
	bdd_fbx:set_time_update( true )
	aaa.obj.update( ref.layer_fbx )

--	self.animation_nb = param.get( ref.animation_nb )
	self.animation_nb = bdd_fbx:get_param( "animation_nb" )
	self:print( "FBX "..self.fbx_name.." detecting "..self.animation_nb.." animations" )
	for i = 0, self.animation_nb - 1 do
		local animation = {}
		bdd_fbx:set_param( "animation_index", i )
		-- update bdd to get animation info
		aaa.obj.update( ref.layer_fbx )

		-- get starting and end time
		animation.start_time = bdd_fbx:get_param( "time_begin" )
		animation.end_time = bdd_fbx:get_param( "time_end" )

		animation.name = bdd_fbx:get_param( "animation_name" )
		self:print( "Get Animation : "..animation.name )
		animation.index = i
		-- get translation values for start time
		bdd_fbx:set_param( "time", animation.start_time )
		aaa.obj.update( ref.layer_fbx )

		animation.pos = {}
		animation.pos.x = bdd_fbx:get_param( "position_x" )
		animation.pos.y = bdd_fbx:get_param( "position_y" )
		animation.pos.z = bdd_fbx:get_param( "position_z" )
		local start_rot_x = bdd_fbx:get_param( "rotation_x" )
		local start_rot_y = bdd_fbx:get_param( "rotation_y" )
		local start_rot_z = bdd_fbx:get_param( "rotation_z" )

		-- get translation and rotation values for end time
		bdd_fbx:set_param( "time", animation.end_time )
		aaa.obj.update( ref.layer_fbx )
		animation.delta = {}
		animation.rot = {}
		animation.delta.x = bdd_fbx:get_param( "position_x" ) - animation.pos.x
		animation.delta.y = bdd_fbx:get_param( "position_y" ) - animation.pos.y
		animation.delta.z = bdd_fbx:get_param( "position_z" ) - animation.pos.z
		-- compute rotation between start and end time
		animation.rot.x = bdd_fbx:get_param( "rotation_x" ) - start_rot_x
		animation.rot.y = bdd_fbx:get_param( "rotation_y" ) - start_rot_y
		animation.rot.z = bdd_fbx:get_param( "rotation_z" ) - start_rot_z

		-- get a point every .25s to draw the position curve
		local dt = 0.25
		local count = 0
		animation.points = {}
		for t = animation.start_time + dt, animation.end_time, dt do
			local point = {}
			bdd_fbx:set_param( "time", t )
			aaa.obj.update( ref.layer_fbx )
			point.x = bdd_fbx:get_param( "position_x" ) - animation.pos.x
			point.y = bdd_fbx:get_param( "position_y" ) - animation.pos.y
			point.z = bdd_fbx:get_param( "position_z" ) - animation.pos.z
			count = count + 1
			animation.points[ count ] = point
		end
		animation.point_count = count
		--table.print( animation, animation.name )
		self.animations[ animation.name ] = animation
		self.b_animations_loaded = true
	end
	if self.animation_nb == 0 then
		self.b_animations_loaded = true
	end
	bdd_fbx:set_param( "get_rotation", false )

	--table.print( self.animations, "animations", 4 )
end


-- Save Anim info in a csv file for debug purpose
function meu:save_anims()
	self:update_animations_info()

	local dirname = self:build_dirname()
	local fpath = string.format( dirname..self.fbx_name..".csv" )
	local file = io.open( fpath, "w" )
	if file then
		file:write( "index" )		file:write( ", " )
		file:write( "name" )		file:write( ", " )
		file:write( "start_time" )	file:write( ", " )
		file:write( "end_time" )	file:write( ", " )
		file:write( "pos_x" )		file:write( ", " )
		file:write( "pos_y" )		file:write( ", " )
		file:write( "pos_z" )		file:write( ", " )
		file:write( "delta_x" )		file:write( ", " )
		file:write( "delta_y" )		file:write( ", " )
		file:write( "delta_z" )		file:write( ", " )
		file:write( "rot_x" )		file:write( ", " )
		file:write( "rot_y" )		file:write( ", " )
		file:write( "rot_z" )		file:write( ", " )

		file:write( "\n" )
		self:print( "self.animation_nb "..self.animation_nb)
	--	table.print( self.animations, "self.animations", 2)
--		for i = 0, self.animation_nb - 1 do
		for name in PAIRS( self.animations ) do
			local animation = self.animations[ name ]
			--local index = index
			self:print( "writing animation "..name )
			file:write( animation.index )		file:write( ", " )
			file:write( name )					file:write( ", " )
			file:write( animation.start_time )	file:write( ", " )
			file:write( animation.end_time )	file:write( ", " )
			file:write( animation.name )		file:write( ", " )
			file:write( animation.pos.x )		file:write( ", " )
			file:write( animation.pos.y )		file:write( ", " )
			file:write( animation.pos.z )		file:write( ", " )
			file:write( animation.delta.x )		file:write( ", " )
			file:write( animation.delta.y )		file:write( ", " )
			file:write( animation.delta.z )		file:write( ", " )

			file:write( animation.rot.x )		file:write( ", " )
			file:write( animation.rot.y )		file:write( ", " )
			file:write( animation.rot.z )		file:write( ", " )
			file:write( "\n" )
		end
		file:close()
	else
		self:print( "save_anims() Can't open file for saving" )
	end
end

function meu:enable_animation()
	self.b_animation = true
end
function meu:disable_animation()
	self.b_animation = false
end

function meu:set_animation( name, tra_x, tra_y, tra_z, rot_y )
	local ref = self.ref
	local bdd_fbx = ref.bdd_fbx
	if not self.animation_nb then return end
	if self.animations[ name ] then
		local animation = self.animations[ name ]
		bdd_fbx:set_param( "animation_index", animation.index )
		bdd_fbx:set_param( "recenter", true )
		bdd_fbx:set_param( "recenter_position_x", animation.pos.x )
		bdd_fbx:set_param( "recenter_position_y", animation.pos.y )
		bdd_fbx:set_param( "recenter_position_z", animation.pos.z )
		bdd_fbx:set_param( "transform_active", true )
		self:set_time( animation.start_time )
		aaa.obj.update( ref.layer_fbx )
	end
end

function meu:set_animation_transform( tra_x, tra_y, tra_z, rot_y )
	local ref = self.ref
	local bdd_fbx = ref.bdd_fbx
	bdd_fbx:set_param( "translate_x", tra_x )
	bdd_fbx:set_param( "translate_y", tra_y )
	bdd_fbx:set_param( "translate_z", tra_z )
	bdd_fbx:set_param( "rotate_y", rot_y )
	aaa.obj.update( ref.layer_fbx )
end

-- Get position and rotation delta between the start and the end of the animation
function meu:get_position_rotation( name )
	if self.animation_nb then
		if self.animations[ name ] then
			local animation = self.animations[ name ]
			return animation.delta.x, animation.delta.y, animation.delta.z, animation.rot.x
		end
	end
	return .0, .0, .0, .0
end

function meu:assign_fbx( name, preload )
	self:print( "Setting fbx to "..name )
	local ref = self.ref
	self.fbx_name = name
	local fbx_data = app.fbx_data[ name ]
	if fbx_data then
		local path_b = app:get_anim_path_rel()..fbx_data.path.."/B_"..fbx_data.fbx_fname..".FBX"
		local path = ""
		if aaa.file.is_exist( path_b ) then
			path = path_b
		else
			path = app:get_anim_path_rel()..fbx_data.path.."/"..fbx_data.fbx_fname..".FBX"
		end
		self:print( "Setting FBX path to "..path )
		local bdd_fbx = ref.bdd_fbx
		bdd_fbx:set_filename_and_nosave( path )
		if preload then
			self:print( "FBX "..name.." : preloading textures" )
			self:load_textures()
		end
	end
end


function meu:assign_fbx_from_def( def )

	--self:box_debug( "assign_fbx_from_def" )

	local ref = self.ref
	local bdd_fbx = ref.bdd_fbx

	self:print( "Setting fbx to "..def.model )
	local name = app:get_anim_path_rel()..def.model_path.."/"
	local name_b = name.."B_"..def.model..".FBX"
	if aaa.file.is_exist( name_b ) then
		name = name_b
	else
		name = name..def.model..".FBX"
	end
	self:print_inverse( "set model to "..name )
	bdd_fbx:set_filename_and_nosave( name )
	aaa.obj.update( bdd_fbx:get_obj() )
	self.fbx_name = def.model

	self:get_mesh_list()
	self:print( "FBX "..name.." : loading textures" )
	self:load_textures()

	self:draw()

	return true
end

function meu:get_mesh_list()
	local ref = self.ref
	local ui = self.ui
	local bdd_fbx = ref.bdd_fbx

	self.mesh_nb = bdd_fbx:get_param( "mesh_nb" )
	self.meshes = {}
	self:print( "FBX Matte detecting "..self.mesh_nb.." meshes" )
	local Xmin = 1000000.
	local Ymin = 1000000.
	local Zmin = 1000000.
	local Xmax = -1000000.
	local Ymax = -1000000.
	local Zmax = -1000000.

	for i = 0, self.mesh_nb - 1 do
		bdd_fbx:set_param( "mesh_index", i )
		aaa.obj.update( ref.layer_fbx )
	--	param.set( ref.mesh_index, i )
	--	aaa.obj.update( ref.bdd_fbx )
		local mesh = {}
		mesh.name = bdd_fbx:get_param( "mesh_name" )
		local bbox = {}

		local x_min, y_min, z_min = bdd_fbx:get_bbox_min()
		local x_max, y_max, z_max = bdd_fbx:get_bbox_max()
		local center = {}
		center.x = ( x_max + x_min )
		center.y = ( y_max + y_min )
		center.z = ( z_max + z_min )
		bbox.center = center
		local size = {}
		size.x = ( x_max - x_min ) * .5
		size.y = ( y_max - y_min ) * .5
		size.z = ( z_max - z_min ) * .5
		bbox.size = size
		mesh.bbox = bbox
		if x_min < Xmin then Xmin = x_min end
		if y_min < Ymin then Ymin = y_min end
		if z_min < Zmin then Zmin = z_min end
		if x_max > Xmax then Xmax = x_max end
		if y_max > Ymax then Ymax = y_max end
		if z_max > Zmax then Zmax = z_max end

		self:print( "Mesh "..(i+1).." :"..mesh.name )
		self.meshes[ i + 1 ] = mesh
	end
	local bbox = {}
	local center = {}
	center.x = ( Xmax + Xmin ) * .5
	center.y = ( Ymax + Ymin ) * .5
	center.z = ( Zmax + Zmin ) * .5
	bbox.center = center
	local size = {}
	size.x = ( Xmax - Xmin )
	size.y = ( Ymax - Ymin )
	size.z = ( Zmax - Zmin )
	bbox.size = size
	self.mesh_bbox = bbox
	--table.print( self.meshes, "Fbx Meshes", 5 )
	--table.print( self.mesh_bbox, "Fbx BBox", 3 )
	self.b_mesh_loaded = true
	--table.print( self.mesh_indexes, "self.mesh_indexes", 3 )
end

function meu:get_bbox()
	return self.mesh_bbox
end

function meu:basename(path)
	local path_find = path:find("\\[^\\]*$")
	if path_find == nil then
		return path
	else
		return path:sub(path_find + 1)
	end
end

function meu:load_textures()
	local fbx_data = app:get_fbx_data()
	local data = fbx_data and fbx_data[ self.fbx_name ]
	self.textures = {}
	self:print( "fbx load_textures() : "..self.fbx_name)
	if data then
		if app:is_load_fbx_textures() then
			local b_load_from_tex_name = data.tex_name and data.tex_name ~= ""
			if b_load_from_tex_name then
				self:load_textures_from_name()
			else
				self:load_textures_from_fbx()
			end
		else
			self.b_texs_loaded = true
		end
	else
		self:print_error( "can't get data for "..self.fbx_name)
	end
end

function meu:load_seq_textures( data )
	MEDIA.set_dir_media( app:get_anim_path()..data.path )
	local name_dir = data.fbx_fname.."_"
	local albedo = {}
	if app:use_compressed_texture() then
		self:print( "Trying to load DDS FBX textures" )
		albedo = IMG_SEQ:create( self.fbx_name, name_dir, "dds", 0, 249 )
		if not albedo then
			self:print( "Failed to load FBX seq textures" )
		end
	end
	if not albedo or albedo.nb == 0 then
		self:print( "Trying to load TGA FBX seq textures" )
		albedo = IMG_SEQ:create( self.fbx_name, name_dir, "tga", 0, 249 )
	end
	if albedo and albedo.nb > 0 then
		self.tex_seq_bind_begin = albedo:get_bind( 1 )
		self.tex_seq_bind_end = albedo:get_bind( albedo:get_nb() )
	else
		self.tex_seq_bind_begin = nil
		self.tex_seq_bind_end = nil
	end
--	table.print( albedo, "load_seq_textures", 4 )
	return albedo
end

function meu:load_texture_low( path, premultiply )
	local bind_tex = 0
	local img
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
	--	if self.b_verbose_load then
			self:print( "Can't get texture from list "..index )
		--end
	end
	return bind_tex
end

function meu:get_texture_list()
	local ref = self.ref
	local bdd_fbx = ref.bdd_fbx
	self.texture_nb = bdd_fbx:get_param( "texture_nb" )
	if self.b_verbose_load then
		self:print( "FBX "..self.fbx_name.." : detecting "..self.texture_nb.." textures" )
	end
	for i = 0, self.texture_nb - 1 do
		local texture = {}
		bdd_fbx:set_param( "texture_index", i )
		aaa.obj.update( ref.layer_fbx )
		local path = bdd_fbx:get_param( "texture_path" )
		if path then
			texture.name = self:basename( path )
		else
			texture.name = 0
		end
		--self:print( "Texture Filename "..texture.name )
		texture.bind = 0
		self.textures[ i ] = texture
	end
	bdd_fbx:set_param_save( "texture_index", false )
	--table.print( self.textures, "self.textures", 3 )
end

-- load textures from fbx : extract materials from fbx
function meu:load_textures_from_fbx()
	local ref = self.ref
	local bdd_fbx = self.ref.bdd_fbx
--	self.textures = {}
	self:print( "load_textures_from_fbx" )
	local fbx_data = app:get_fbx_data()
	local data = fbx_data and fbx_data[ self.fbx_name ]
	if data then
		-- disable time_update, we only want to get Texture and Material info
		local b_time_update = bdd_fbx:is_time_update()
		bdd_fbx:set_time_update( false )

		aaa.obj.update( ref.layer_fbx )
		self:get_texture_list()

		aaa.img.set_free_when_on_board( true )

		-- get No Normal texture, in case there's no normal map provided for a mesh
--		self:print( "app.texs_no_normal_path"..app.texs_no_normal_path )
		MEDIA.set_dir_media( app.texs_no_normal_path )
		local texture = {}
		texture.name = "NO_NORMAL"
		texture.bind = self:load_texture_low( "MAP_NO_NORMAL.tga", true )
		self.textures[ self.texture_nb ] = texture
		local texture_black = {}
		texture_black.name = "BLACK"
		texture_black.bind = self:load_texture_low( "BLACK.tga", true )
		self.textures[ self.texture_nb + 1 ] = texture_black

		app:enable_img_scale_on_load()
		MEDIA.set_dir_media( app:get_anim_path()..data.path )

	--	table.print( self.textures, "self.textures", 3 )
		self.mesh_nb = bdd_fbx:get_param( "mesh_nb" )
		self.mesh_material = {}
		self:print( "Mesh count : "..self.mesh_nb)
		for i = 0, self.mesh_nb - 1 do
			local material = {}
			material.albedo		= 0
			material.normal		= 0
			material.metal		= 0
			material.roughness	= 0
			material.emissive	= 0
			material.b_img_seq = false
			bdd_fbx:set_param( "mesh_index", i )
			aaa.obj.update( ref.layer_fbx )
			self.b_tex_seq = false
			-- ALBEDO
			if data.b_texs_seq and data.b_texs_seq == true then
				self.b_tex_seq = true
				material.b_img_seq = true
				material.albedo = self:load_seq_textures( data )
			else
				local albedo_index = bdd_fbx:get_param( "diffuse_texture_index" )
				material.albedo = self:load_texture_from_list( albedo_index )
				if material.albedo == 0 then
					if self.b_verbose_load then
						self:print( "Use BLACK for ALBEDO" )
					end
					material.albedo = self.textures[ self.texture_nb + 1 ].bind
				end
			end
			-- NORMAL
			local normal_index = bdd_fbx:get_param( "normal_texture_index" )
			if  normal_index == -1 then
				normal_index = bdd_fbx:get_param( "bump_texture_index" )
			end
			if normal_index == -1 then
				if self.b_verbose_load then
					self:print( "no normal use NO NORMAL ".. self.textures[ self.texture_nb ].bind )
				end
				material.normal = self.textures[ self.texture_nb ].bind
			elseif self.textures[ normal_index ] then
				if self.textures[ normal_index ].name == "" then
					if self.b_verbose_load then
						self:print( "Texture path is empty, use the next texture index" )
					end
					normal_index = normal_index + 1
				end
				material.normal = self:load_texture_from_list( normal_index )
			end
			-- ROUGHNESS
			local rough_index = bdd_fbx:get_param( "shininess_texture_index" )
			material.roughness = self:load_texture_from_list( rough_index )
			if material.roughness == 0 then
				if self.b_verbose_load then
					self:print( "Use BLACK for ROUGHNESS" )
				end
				material.roughness = self.textures[ self.texture_nb + 1 ].bind
			end

			-- METAL
			local metal_index = bdd_fbx:get_param( "specular_texture_index" )
			if  metal_index == -1 then
				metal_index = bdd_fbx:get_param( "specular_factor_texture_index" )
			end
			material.metal = self:load_texture_from_list( metal_index )
			if material.metal == 0 then
				if self.b_verbose_load then
					self:print( "Use BLACK for METAL" )
				end
				material.metal = self.textures[ self.texture_nb + 1 ].bind
			end

			-- EMISSIVE
			local emissive_index = bdd_fbx:get_param( "emissive_texture_index" )
			material.emissive = self:load_texture_from_list( emissive_index )
			if material.emissive == 0 then
				if self.b_verbose_load then
					self:print( "Use BLACK for EMISSIVE" )
				end
				material.emissive = self.textures[ self.texture_nb + 1 ].bind
			end

			self.mesh_material[ i ] = material
		end
		app:disable_img_scale_on_load()
		self.b_texs_loaded = true

		if self.b_verbose_load then
			table.print( self.mesh_material, "self.mesh_material", 3 )
		end
		self:update_textures( 0, true )
		-- restore time_update
		bdd_fbx:set_time_update( b_time_update )
	else
		self:print( "can't get data for FBX "..self.fbx_name )
	end
end

-- Load textures using tex_name in fbx_data
function meu:load_textures_from_name()
	local ref = self.ref
	local fbx_data = app:get_fbx_data()
	local data = fbx_data and fbx_data[ self.fbx_name ]
	if data then
		self.mesh_material = {}
		if self.b_verbose_load then
			self:print( "FBX "..self.fbx_name.." : loading textures" )
		end
		aaa.obj.update( ref.layer_fbx )
		self:get_texture_list()
		self.mesh_nb = self.ref.bdd_fbx:get_param( "mesh_nb" )

		aaa.img.set_free_when_on_board( true )

		-- get No Normal texture, in case there's no normal map provided for a mesh
	--	self:print( "app.texs_no_normal_path"..app.texs_no_normal_path )
		MEDIA.set_dir_media( app.texs_no_normal_path )
		local texture = {}
		texture.name = "NO_NORMAL"
		texture.bind = self:load_texture_low( "MAP_NO_NORMAL.tga", true )
		self.textures[ self.texture_nb ] = texture
		local texture_black = {}
		texture_black.name = "BLACK"
		texture_black.bind = self:load_texture_low( "BLACK.tga", true )
		self.textures[ self.texture_nb + 1 ] = texture_black


		app:enable_img_scale_on_load()
			self:print( app:get_anim_path() )
			self:print( data.path )
			MEDIA.set_dir_media( app:get_anim_path()..data.path )
			local material = {}
			for i=1,5 do
				--table.print( data, "fbx_data "..i, 3 )
				local fname = (data.tex_name or data.fbx_fname).."_"..BDD_FBX.tex_name_for_file[i]..".TGA"
				self:print( "fname "..fname..", "..data.fbx_fname )
				local name = BDD_FBX.tex_name[i]:lower()
				local img = {}
				if app:use_compressed_texture() then
--					self:print( "getting DDS texture")
					local fname_dds = (data.tex_name or data.fbx_fname).."_"..BDD_FBX.tex_name_for_file[i]..".DDS"
					img = IMGS.get_img( fname_dds )
				end
				if img == nil or next(img) == nil then
--					self:print( "getting TGA texture")
					img = IMGS.get_img( fname )
				end
				if img then
					local bind = img:get_bind()
					if bind == 0 then
						if i == 1 then
							if self.b_verbose_load then
								self:print( "Use BLACK for ALBEDO" )
							end
							material[ name ] = self.textures[ self.texture_nb + 1 ].bind
						elseif i == 2 then
							if self.b_verbose_load then
								self:print( "Use NO normal map for NORMAL" )
							end
							material[ name ] = self.textures[ self.texture_nb ].bind
						elseif i == 3 then
							if self.b_verbose_load then
								self:print( "Use BLACK for METAL" )
							end
							material[ name ] = self.textures[ self.texture_nb + 1 ].bind
						elseif i == 4 then
							if self.b_verbose_load then
								self:print( "Use BLACK for ROUGHNESS" )
							end
							material[ name ] = self.textures[ self.texture_nb + 1 ].bind
						elseif i == 5 then
							if self.b_verbose_load then
								self:print( "Use BLACK for EMISSIVE" )
							end
							material[ name ] = self.textures[ self.texture_nb + 1 ].bind
						end
					else
						material[ name ] = bind
					end
				--	material[ name ] = bind
					self:set_texture_bind_2d( i, bind )
					if self.b_verbose_load then
						self:print( "FBX "..self.fbx_name.." : "..fname.." at bind "..bind )
					end
				else
					if i == 2 then
						if self.b_verbose_load then
							self:print( "Use NO normal map for NORMAL" )
						end
						material[ name ] = self.textures[ self.texture_nb ].bind
					elseif i == 3 then
						if self.b_verbose_load then
							self:print( "Use BLACK for METAL" )
						end
						material[ name ] = self.textures[ self.texture_nb + 1 ].bind
					elseif i == 4 then
						if self.b_verbose_load then
							self:print( "Use BLACK for ROUGHNESS" )
						end
						material[ name ] = self.textures[ self.texture_nb + 1 ].bind
					elseif i == 5 then
						if self.b_verbose_load then
							self:print( "Use BLACK for EMISSIVE" )
						end
						material[ name ] = self.textures[ self.texture_nb + 1 ].bind
					end
					self:print_error( "Could not load img :  "..fname.." i "..5 )
				end
			end
			self.mesh_material[ 0 ] = material
		app:disable_img_scale_on_load()
		self.b_texs_loaded = true
		self:update_textures( 0, true )
	else
		self:print( "FBX "..self.fbx_name.." is not defined during load_textures()" )
	end
end

function meu:set_test_pattern_from_bu(	bu )
	self:print( "Clicking "..bu:get_value() )
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

function meu:update_ui()
	local ui = self.ui
	local ref = self.ref

	ui.bu_info:set_text( ref.bdd_fbx:get_filename() )
	ui.bu_name:set_text( self.fbx_name )

	local b_loaded = self.ref.bdd_fbx:is_fbx_loaded()
	ui.bu_name:set_text_color_problem_info( b_loaded )
	ui.bu_info:set_text_color_problem_info( b_loaded )

	ui.bu_index_mesh:set_min_max(0, self.mesh_nb - 1, 0)

	self:update_time_ui()

	local bdd_fbx = ref.bdd_fbx
	-- local time_str =  string.format( "Time : %.2f", self.time )
	-- ui.bu_time:set_text( time_str )
	ui.bu_mesh_nb:set_text( "Mesh Nb : "..bdd_fbx:get_param( "mesh_nb" ) )
	local mesh_name = ""
	if self.b_draw_index then
		mesh_name =  bdd_fbx:get_param( "mesh_name" )
	end
	ui.bu_mesh_name		:set_text( mesh_name )
	ui.bu_animation		:set_text( "Animation Nb : "..self.animation_nb )

	if self.__time then
		ui.bu_animation_time:set_text( "Animation Time : "..string.format( "%.2f", self.__time.time ).."/"..math.floor( self.__time.time * 60. ) )
	else
		ui.bu_animation_time:set_text( "Animation Time : " )
	end

	ui.bu_blend_shape_nb:set_text( "Blend Shape "..bdd_fbx:get_param("blend_shape_index").."/"..bdd_fbx:get_param("blend_shape_nb")
												.."-> "..bdd_fbx:get_param("blend_shape_channel_nb")  )
end

function meu:set_max_time()
	if self.ref.bdd_fbx:is_fbx_loaded() then
		self:set_time_max( self.ref.bdd_fbx:get_param( "time_end" ) )
	else
		self.b_set_time_max = true
	end
end

function meu:get_max_time()
	if self.ref.bdd_fbx:is_fbx_loaded() then
		return self.ref.bdd_fbx:get_param( "time_end" )
	end
	return 0.0
end

function meu:set_blend_shape( id, b_on, val )
	local ref = self.ref
	local pre = "blend_shape_"..id
	local name = pre.."_manual"
	ref.bdd_fbx:set_param( name, b_on )
	if val then
		name = pre.."_factor"
		ref.bdd_fbx:set_param( name, val )
	end
end

function meu:set_draw_by_index( b_on, index )
	self.b_draw_index = b_on
	if index then
		self.mesh_index = index
	end
end

function meu:update()
	local ref = self.ref
	if self.mesh_nb == 0 then
		self.mesh_nb = self.ref.bdd_fbx:get_param( "mesh_nb" )
	end
	self:set_time( self:update_time() )
	--self.time = ref.bdd_fbx:get_param( "time" )

--	if self.time > 103.333 then
--	end
end

function meu:update_textures( mesh_index, b_update_tex_ui )
	if mesh_index < self.mesh_nb then
		local material = self.mesh_material[ mesh_index ]
		if material then
			if material.b_img_seq and material.b_img_seq == true then
				-- self.tex_seq_index = self.tex_seq_index + 1
				-- if self.tex_seq_index > 250 then
				-- 	self.tex_seq_index = 1
				-- end
				local bind = material.albedo:get_bind( self.tex_seq_index )
				self:set_tex_unit( 0, bind, 		b_update_tex_ui )
			else
				self:set_tex_unit( 0, material.albedo, 		b_update_tex_ui )
			end
			self:set_tex_unit( 1, material.normal, 		b_update_tex_ui )
			self:set_tex_unit( 2, material.roughness,	b_update_tex_ui )
			self:set_tex_unit( 3, material.metal,		b_update_tex_ui )
			self:set_tex_unit( 4, material.emissive,	b_update_tex_ui )
		end
	end
end

function meu:draw_attr()
	aaa.obj.update_then_draw( self.ref.layer_attr )	--	set the "attribute" no more shader here
	--hack this work for only one mesh (ok for the monaco fish)
	gol.set_tex_unit_2d_bind( 4, self:get_texture_bind_2d(5) )
end

function meu:draw_fbx_min( )
	local ref = self.ref
	local bdd_fbx = self.ref.bdd_fbx
	--self:draw_layer(2)
	if self.mesh_nb > 1 then
		bdd_fbx:set_param( "draw_mesh_by_index", true )
		if self.b_draw_index then
			bdd_fbx:set_param( "mesh_index", self.mesh_index )
			-- set texture
			self:update_textures( self.mesh_index, true )
			aaa.obj.update_then_draw( self.ref.layer_fbx )
		else
			local b_time_update = bdd_fbx:is_time_update()
		--	local i = 2
			for i = 0, self.mesh_nb - 1 do
			--	self:print( "drawing mesh "..i)
				bdd_fbx:set_param( "mesh_index", i )
				-- set texture
				self:update_textures( i, false )
				aaa.obj.update_then_draw( self.ref.layer_fbx )
				-- don't update time for the next mesh
				--bdd_fbx:set_time_update( false )
			end
			-- restore time update status
			bdd_fbx:is_time_update( b_time_update )
		end
	else
		if self.b_tex_seq == true then
		--	self:print( "update tex seq")
			self:update_textures( 0, true )
		end
		bdd_fbx:set_param( "draw_mesh_by_index", false )
		aaa.obj.update_then_draw( self.ref.layer_fbx )
	end
end

function meu:draw()
	local ui = self.ui
	local ref = self.ref
	--self.b_verbose_load = true
	if not self.fbx_name then
		app:get_fbx_data()
		self:print_error( "FBX : name not assigned")
		return
	end

-- 	self:print( "Drawing "..self.fbx_name)
	if ref.bdd_fbx:is_fbx_loaded() == false then
		if self.b_load_tried == false then
			self.b_load_tried = true
			ref.bdd_fbx:reload()
			aaa.obj.update( ref.bdd_fbx:get_obj() )
			if ref.bdd_fbx:is_fbx_loaded() == false then
				self:print_error( "FBX : Can't load "..self.fbx_name )
				return
			end
			--	self:set_time_max( ref.bdd_fbx:get_param( "time_end" ) )
		else
			return
		end
	end
	if self.ref.bdd_fbx:is_fbx_loaded() then
		--	self.b_texs_loaded = false
		if not self.b_mesh_loaded then
			self:get_mesh_list()
		end
		--self:print( "must load textures")
	--	self.b_texs_loaded = false
		if not self.b_texs_loaded then
			--self:print( "must load textures")
			self:load_textures()
		end
		if not self.b_animations_loaded then
			self:update_animations_info()
		end
	end
	--self:print( "gere")
	--self.b_set_time_max = true
	if self.b_set_time_max == true then
		self:set_max_time()
		self.b_set_time_max = false
	end
	if ui.bu_cam_use:get_value() > 0 then
		local cam = self:get_camera_sel()
		if cam then
			if self.b_animation then
				local pos_x, pos_y, pos_z = ref.bdd_fbx:get_position()
				local tra_x, tra_y, tra_z = ref.bdd_fbx:get_translation()
				local center_pos_x, center_pos_y, center_pos_z = ref.bdd_fbx:get_recenter_position()
				local rot_x, rot_y, rot_z = ref.bdd_fbx:get_rotation()
				local z, x = math.rotate_ab_turn( pos_z - center_pos_z, pos_x - center_pos_x, rot_y )
				param.set( cam, "center_x", x + tra_x )
				param.set( cam, "center_y", pos_y + tra_y - center_pos_y )
				param.set( cam, "center_z", z + tra_z )
			else
				local x,y,z = ref.bdd_fbx:get_position()
				param.set( cam, "center_x", x )
				param.set( cam, "center_y", y )
				param.set( cam, "center_z", z )
			end
		end
	end


	--self:print( "draw")
	self:draw_layers_begin()
		self:draw_attr()	--	set the "attribute" no more shader here
		--		if self.b_draw_cube then
		--			self:draw_layer( 3 )
		--		end
		--if self.b_draw_fbx then
		self:draw_fbx_min()

		--end
	self:draw_layers_end()
end
