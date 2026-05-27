function meu:define_meu_infos( )
	return	{ author = "Mâa",
				name_long = "Object 3D",
				tags = { "3D", "core", "coregraphic", "geometry", "surface", "Draw", "VJ", "unfinished" },
				help = "Display a 3D object, loaded from a .obj file, deal with normal and UV."
			}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = 1
	local DY = .2

	local sha = self:get_shading()

	ui.cam = self:add_camera( nil, 16 )
	self:add_rendering()

	self:add_trig_method(	{ix,iy,		4,SY},	"Load Model",	self, "load_model"	):set_color_back( "load" )
	iy = iy + SY + DY

	self:add_bu_texture_target_unit(	{ix,iy,		8,6} )
	iy = iy + 6 + DY

	bu = self:add_selector(	{ix,iy,		8,SY},	"Type" )
		bu:set_nb_min_0( 4, 1 )
		bu:set_item_text( 2, "Toon", "BW", "Flag" )
		bu:set_target_param( sha:get_ref_frag_int( 1 ) )
	iy = iy + SY + DY

	self:add_rgbf(			{ix,iy,		8,SY*2}	)
	iy = iy + SY*2 + DY

	ui.float = {}
	for i=1,2 do
		bu = self:add_param({ix,iy,		8,SY},	"f"..i,		sha:get_ref_frag_float(i),	0,1	)
			bu:set_meter( false )
			ui.float[i] = bu
		iy = iy + SY
	end

	ix,iy = 9,2.8
	self:add_transfo( 		{ix,iy,		8,2.4} )
	iy = iy + 2.4 + DY
	self:add_shading_ui(	{ix,iy,		8,SY}	)
	iy = iy + SY + DY

	self:add_button( 		{ix,	iy,	4,SY}, 	"Paint",	self, "b_paint",	true )
	self:add_button( 		{ix+4,	iy,	4,SY}, 	"Add", 		self, "b_add",		false )
	iy = iy + SY + DY

	self:add_bu_texture_target_unit( {ix,	iy,	8,6},	"tex_paint", 2, false )
	iy = iy + 6 + DY

end

-- function meu:init()	
-- 	self:print( "is "..self.ref.bdd )
-- end

function meu:load_model()
	param.do_action_open( self:get_layer_bdd(1), "model_filename" )
end
function meu:update()
	local sha = self:get_shading()

	local sx,sy = self:get_texture_size( 2 )
	--we disable paint when no texture
	param.set( sha:get_ref_frag_int(3), (sx and self.b_paint) and (self.b_add and 1 or 2) or 0 )
	local a = self.ref.alpha
	if a then
		param.set( a, self:get_mu():get_value() )
	end
end
function meu:init()
	self:add_shading()
end

function meu:draw_icon()
	self:draw_icon_text( "3D" )
end
