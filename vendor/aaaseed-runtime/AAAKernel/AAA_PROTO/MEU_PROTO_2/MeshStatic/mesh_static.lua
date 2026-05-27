function meu:define_meu_infos( )
	return { author = "Mâa Franz mrvux Romain Etienne",
			tags = { "3D", "Draw", "Core", "CoreGraphic", "Geometry", "depreciated", "unfinished" },
			help = "Depreciated use MEU FbxMatte\n"..
					"Encapsulate c_bdd_mesh_static reading fbx static geometry.\n"..
					"compatible with AAASeed Gbuffer rendering\n"..
					"Depreciated "
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

	self:add_camera()
	self:add_rendering()

	iy = 2
--	bu = self:add_button(	{		ix, iy,		SY, SY },	"Draw Cube", 	self, "b_draw_cube", false )
--	iy = iy + SY

	ui.bu_info = self:add_text_info(	{ix,iy,	16,SY},	"Info" )
	iy = iy + SY
	bu = self:add_button(	{		ix, iy,			SY, SY },	"Validate at load", 	self, "b_draw_fbx", true )
	iy = iy + SY
	local SX = (8)/3
	bu = self:add_trig_method(	{	ix, iy,				SX, SY},	"FBX.Load", 	self, "load_fbx" )
		bu:set_text( "Load" )
	bu = self:add_trig_method(	{	ix+SX, iy,			SX, SY},	"FBX.ReLoad", 	self, "reload_fbx" )
		bu:set_text( "ReLoad" )
	bu = self:add_trig_fn(	{		ix+SX*2, iy,		SX, SY},	"FBX.Focus",	aaa.obj.set_focus_ui, ref.bdd_fbx	)
		bu:set_text( "Focus" )

	iy = iy + SY


--	bu = self:add_button(	{		ix+1, iy,	SY,SY },	"Grid", 		ref.bdd_fbx, "grid_display" )
	iy = iy + SY + DY

	--self:set_tab_key( "TEX" )
	iy = 4
	SY = 2.5
	for i=1,4 do
		self:add_bu_texture_target_unit( {9,iy, nil,SY}, "Tex_"..i, i	)
		iy = iy + SY
	end
	--self:set_tab_key_def()
end

function meu:init()
	local ref = self.ref

	local bdd_fbx = self:get_layer_bdd( 2 )
	ref.bdd_fbx = bdd_fbx
	ref.filename_fbx = param.get_ref( bdd_fbx, "filename" )
end


function meu:load_fbx()
	param.do_action_open( self.ref.bdd_fbx, "filename" )
end
function meu:reload_fbx()
	param.set( self.ref.bdd_fbx, "reload_trig", true )
end

function meu:update_ui()
	local ui = self.ui
	local ref = self.ref
	ui.bu_info:set_text( param.get( ref.filename_fbx ) )
end

function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 )	--	set the "attribute" no more shader here
--		if self.b_draw_cube then
--			self:draw_layer( 3 )
--		end
		if self.b_draw_fbx then
			self:draw_layer( 2 )
		end

	self:draw_layers_end()
end




