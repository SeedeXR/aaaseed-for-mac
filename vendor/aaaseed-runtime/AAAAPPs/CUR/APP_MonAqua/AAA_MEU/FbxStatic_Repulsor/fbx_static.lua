
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

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	self:add_camera()
	self:add_rendering()
	
	local fbx = ref.bdd_fbx

	iy = 2
--	bu = self:add_button(	{		ix, iy,		SY, SY },	"Draw Cube", 	self, "b_draw_cube", false )
--	iy = iy + SY

	ui.bu_info = self:add_text_info(	{ix,iy,	16,SY},	"Info" )

	self:set_tab_key( "Main" )
	iy = iy + SY
	bu = self:add_button(	{		ix, iy,			SY, SY },	"Draw FBX", 		self, "b_draw_fbx",	true )
	iy = iy + SY
	bu = self:add_button(	{		ix, iy,			SY, SY },	"verbose", 			ref.verbose, nil,	true )
	iy = iy + SY
	bu = self:add_button(	{		ix, iy,			SY, SY },	"Validate at load", ref.validate, nil,	true )
	iy = iy + SY

	local SX = (8)/3
	bu = self:add_trig_method(	{	ix, iy,				SX, SY},	"FBX.Load", 		self, "load_fbx" )
		bu:set_text( "Load" )
	bu = self:add_trig_method(	{	ix+SX, iy,			SX, SY},	"FBX.ReLoad", 		self, "reload_fbx" )
		bu:set_text( "ReLoad" )
	bu = self:add_trig_fn(	{		ix+SX*2, iy,		SX, SY},	"FBX.Focus",		aaa.obj.set_focus_ui, ref.bdd_fbx	)
		bu:set_text( "Focus" )
	bu = self:add_trig_method(	{	ix+SX*2, iy - SY,	SX, SY},	"FBX.ReLoadTex", 	self, "reload_texture" )
		bu:set_text( "ReLoad Texs" ):set_color_back("reload")

--	iy = iy + SY
--	bu = self:add_button(	{		ix, iy,			SY, SY },	"SDK evaluator", 	ref.use_sdk, nil, true )

	iy = iy + SY
	bu = self:add_button(	{		ix, iy,				SY,SY},	"Reverse Layer Order", 	self, "b_draw_reverse",	false )

	iy = iy + SY
	-- Draw one FBX layer
	bu = self:add_button(	{		ix, iy,			SY, SY },	"Draw by Layer", 	self, "b_draw_layer",	false )
	ui.bu_layer_index = self:add_slider(	{ix+4,iy,	4,SY},	"Index",			self, "mesh_index",		0, 0, 16 ):set_value_type_integer(true)
	iy = iy + SY
	bu = self:add_button(	{		ix, iy,			SY, SY },	"Draw by Index", 	self, "b_draw_index",	false )
	iy = iy + SY

	-- Draw a range of layers
	bu = self:add_button(	{		ix, iy,			SY,SY},		"Limit Layer Index", 	self, "b_limit_layer",	false )
	ui.bu_layer_min = self:add_slider(	{ix+4,iy,	4,SY},		"Min Layer Index",		self, "min_layer_index",		0, 0, 30 ):set_value_type_integer(true)
	iy = iy + SY
	bu = self:add_slider(	{ix+4,iy,	4,SY},	"Max Layer Index",		self, "max_layer_index",		0, 0, 30 ):set_value_type_integer(true)
		bu:set_value_load_save( false )
	 	ui.bu_layer_max = bu

	iy = iy + SY

	-- Use UV texture as albedo for debugging purpose
	bu = self:add_button(	{		ix, iy,			SY, SY },	"Use UV Tex", 		self, "b_uv_tex",		false )
	iy = iy + SY
	-- Use colors for debugging purpose
	bu = self:add_button(	{		ix, iy,			SY, SY },	"Use Color", 		self, "b_mesh_color",		false )
	iy = iy + SY

	-- Draw one FBX layer
	ui.bu_mesh_nb = self:add_text_info(	{ix,iy,	16,SY * .5},	"Mesh Nb" )
	ui.bu_mesh_name = self:add_text_info(	{ix,iy + SY * .5,	16,SY * .5},	"Mesh Name" )

--	bu = self:add_button(	{		ix+1, iy,		SY,SY },	"Grid", 			ref.bdd_fbx, "grid_display" )
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
		self:add_bu_texture_target_unit( {9,iy, nil,SY}, BDD_FBX.tex_name[i], i, false	)
		--todo this means if we change it will not be save find a solution
		--self:set_bu_texture_save( i, false )
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

function meu:draw_attr()
	aaa.obj.update_then_draw( self.ref.layer_attr )	--	set the "attribute" no more shader here
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
			-- if not self.fbx_name then
			-- 	app:get_fbx_matte()
			-- end
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
	if self.b_draw_fbx then
			self:draw_layers_begin()
				self:draw_attr()	--	set the "attribute" no more shader here
				self:draw_layer( 2 )
			self:draw_layers_end()
	end
end

