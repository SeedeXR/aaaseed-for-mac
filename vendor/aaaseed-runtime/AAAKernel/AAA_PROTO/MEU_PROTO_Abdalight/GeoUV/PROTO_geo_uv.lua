function meu:define_meu_infos( )
	return	{ author = "Mâa and Abdalight",
				name_long = "Geometry with UV",
				tags = { "3D", "core", "coregraphic", "geometry", "surface", "Draw", "VJ", "unfinished" },
				help = "Display a 3D procedural object (Sphere, Torus, Cone, Teapot ...)"
			}
end

function meu:define_ui()
	local ref	 = self.ref
	local ui 	 = self.ui
	local bu

	local ix, iy = 1,1
    local SX, SY = 4/3, 1
	local DY	 = .2

	local mapping_ref = self:get_layer_mapping(1)

	self:add_rendering()
	self:add_camera()


	bu = self:add_button(		{ix,iy,			4,SY},		"Geometry",				nil,  nil,               		    0       	)
		bu:set_menu{ "Sphere", "Torus", "Cone", "Grid", "Teapot" }:set_nb_min_0( 1, 5)
		bu:set_target_lua( self , "selec_geometry" )
		bu:set_text_selector( true )
	bu = self:add_selector(		{ix+4,iy,		4,SY},		"AXE"  	     )
		bu:set_item_text( 1, "X", "Y", "Z" )
		bu:set_target_lua( self , "selec_axe" )
		bu:set_text_draw( false )
		self.selec_axe = 3
	iy = iy + SY + DY

	bu = self:add_button(		{ix,iy,			3.5,SY},	"Draw Mode",			nil,  nil,               		   0       		)
 		bu:set_menu{ "Regular" , "Point", "Line_U"   }:set_nb_min_0( 1, 3)
        bu:set_target_lua( self, "draw_mode" )
        bu:set_text_selector( true )
	bu = self:add_rendering_primitive( 	{ix+4,iy,		3.5,SY} )
		ui.bu_rendering_primitive = bu
	iy = iy + SY + DY
	
	--todo at some point try to use MEU:add_sliders_suvwf() (search MEU:add_sliders_sxyzf() for examples)
	--bu = self:add_sliders_suvwf( {ix,iy,		8,SY},		"Size2",		self, true, 1, 4 )
	--iy = iy + SY
	bu = self:add_slider(		{ix,iy,			2,SY},		"Size",					self,"size",					  1,	0,4		)
		bu:add_values_def( .1,.3,.5,.8,1,1.5 )
		bu:set_color_back( "factor" )
	bu = self:add_slider(		{ix+2,iy,		2,SY},		"Size U",				self,"size_u",					  1,	0,4		)
		bu:set_color_back( "u" )
		bu:add_values_def( 1,2,3,4)
	bu = self:add_slider(		{ix+4,iy,		2,SY},		"Size V",				self,"size_v",					  1,	0,4		)
		bu:set_color_back( "v" )
		bu:add_values_def( 1,2,3,4)
	bu = self:add_slider(		{ix+6,iy,		2,SY},		"Size W",				self,"size_w",					  1,	0,4		)
		bu:set_color_back( "w" )
		bu:add_values_def( .1,.3,.5,.8,1,1.5 )	
	iy = iy + SY

	bu = self:add_trig_method(	{ix,iy,			2,SY},		"Reset",				self,"reset"									)
    	bu:set_text_visible( true )
	bu = self:add_slider(		{ix+2,iy,		3,SY},		"Torus_Cone Radius",	self,	"torus_cone_radius",	.25,	0,1		)
		ui.bu_torus_cone_radius = bu
	bu = self:add_slider(		{ix+5,iy,		3,SY},		"Turn",					self,	"turn",						0,	0,1		)
		ui.bu_turn = bu
	iy = iy + SY

	bu = self:add_slider(		{ix,iy,		 	2*SX,SY},	"Nb U",					self,"nb_u",					   32,	0,128	)
		bu:set_color_back( "u" )
		bu:set_value_type_integer( true )
		bu:add_values_def( 10,20,30,50,70,80,90 )
	bu = self:add_slider(		{ix+2*SX,iy, 	2*SX,SY},	"Center U",				self,"center_u",					0,	-1,1	)
		bu:set_color_back( "u" )
	bu = self:add_slider(		{ix+4*SX,iy, 	2*SX,SY},	"Range U",				self,"range_u",						1,	-1,1	)
		bu:set_color_back( "u" )	
	iy = iy + SY

	bu = self:add_slider(		{ix,iy,			2*SX,SY},	"Nb V", 				self,"nb_v",					   32,	0,128	)
		bu:set_color_back( "v" )
		bu:set_value_type_integer( true )
		bu:add_values_def( 10,20,30,50,70,80,90 )
	bu = self:add_slider(		{ix+2*SX,iy, 	2*SX,SY},	"Center V",				self,"center_v",					0,	-1,1	)
		bu:set_color_back( "v" )
	bu = self:add_slider(		{ix+4*SX,iy, 	2*SX,SY},	"Range V",				self,"range_v",						1,	-1,1	)
		bu:set_color_back( "v" )
	iy = iy + SY + DY


	local y1 = iy

	ix, iy = 9, 2 + DY

	
	self:add_rgbf(	 			{ix,iy,		     8,SY}	)
	iy = iy + SY + DY

	self:add_transfo(			{ix,iy,		     8,2.4}, 1 )
	iy = iy + 2.4 + DY

	ix,iy = 1,y1
	self:add_mapping_texture_mode( {ix,iy,		8,SY} )
	--	bu:set_target_param( param.get_ref( mapping_ref,	"tex_implicit"	) )
	iy = iy + SY

	bu = self:add_slider(		{ix,iy,   	  	2*SX,SY},	"Factor",		self,"factor",		1,	 0,5 )
		bu:set_target_param( param.get_ref( mapping_ref,	"tex_factor" )	)
		bu:set_color_back( "factor" )
	bu = self:add_slider(		{ix+2*SX,iy,	2*SX,SY},	"Factor u",		self,"factor_u",	1,	 0,5 )
		bu:set_target_param( param.get_ref( mapping_ref,	"tex_u" )		)
		bu:set_color_back( "u" )
	bu = self:add_slider(		{ix+4*SX,iy,	2*SX,SY},	"Factor v",		self,"factor_v",	1,	 0,5 )
		bu:set_target_param( param.get_ref( mapping_ref,	"tex_v" )		)
		bu:set_color_back( "v" )
	iy = iy + SY

	bu = self:add_slider(		{ix+2*SX,iy,	2*SX,SY},	"U Origin",		self,"u_ori",		0,	-5,5 )
		bu:set_target_param( param.get_ref(  mapping_ref,	"tex_u_ori"	) )
		bu:set_color_back( "u" )
	bu = self:add_slider(		{ix+4*SX,iy,	2*SX,SY},	"V Origin",		self,"v_ori",		0,	-5,5 )
		bu:set_target_param( param.get_ref(  mapping_ref,	"tex_v_ori" ) )
		bu:set_color_back( "v" )
	iy = iy + SY + DY

	ix,iy = 9,y1
	self:add_bu_texture_target_unit( {ix,iy,     8,6*SY}    )
	iy = iy + 6*SY + DY
end

function meu:init()
	local ref	= self.ref
	local layer	= self:get_layer( 1 )
	local pgr	= param.get_ref
	local gbbc	= aaa.obj.get_branch_by_class

	ref.model	= gbbc( layer,     "model" 		 )
	ref.axe		=  pgr( ref.model, "axe_object"  ) 
	ref.factor	=  pgr( ref.model, "size_factor" )
	ref.size_u	=  pgr( ref.model, "size_u" 	 )
	ref.size_v	=  pgr( ref.model, "size_v" 	 )
	ref.size_w	=  pgr( ref.model, "size_axe"    )

	local bdd_sphere_ref = 	gbbc( layer , "bdd_sphere"	)
	local bdd_torus_ref  = 	gbbc( layer , "bdd_torus"	)	
	local bdd_cone_ref	 = 	gbbc( layer , "bdd_cone"	)
	local bdd_grid_ref 	 = 	gbbc( layer , "bdd_grid"	)
	local bdd_teapot_ref = 	gbbc( layer , "bdd_torus"	) 				

	self.bdd_names	 = 	{ "sphere" , "torus" , "cone" , "grid" , "teapot" }
	self.axe_names	 = 	{ "X" , "Y" , "Z" }
	self.bdd_general = 	{ bdd_sphere_ref, bdd_torus_ref, bdd_cone_ref, bdd_grid_ref, bdd_teapot_ref }
	self.layer = layer
	self.last_bdd_use = nil
end

function meu:reset()
	self.size_u, 	 self.size_v, 	self.size_w	= 1, 1, 1
	self.u_ori,		 self.v_ori,	self.w_ori	= 0, 0, 0
	self.size,		 self.factor				= 1, 1
	self.range_u, 	 self.range_v 				= 1, 1
	self.center_u, 	 self.center_v 				= 0, 0
	self.nb_u, 		 self.nb_v					= 32, 32
	self.factor_u,	 self.factor_v 				= 1, 1
	self.torus_cone_radius						= .25
end

function meu:update_ui()
	local ui = self.ui
	local ref = self.ref
	local selec_geom = self.selec_geometry

	local alpha_bu_torus = ( ref.radius_int or ref.radius_top) and true or false
	local alpha_bu_turn  = ( selec_geom ~= 5 ) and true or false

	ui.bu_torus_cone_radius:interpolate_alpha_bu( alpha_bu_torus )
	ui.bu_turn:interpolate_alpha_bu( alpha_bu_turn )
	ui.bu_rendering_primitive:interpolate_alpha_bu( self.draw_mode == 0 )
end


function meu:update()
	local ref   = self.ref
	local pgr   = param.get_ref
	local pgrne = param.get_ref_no_error
	local ps    = param.set

	local selec_geom = self.selec_geometry + 1
	local bdd_use	 = self.bdd_general[selec_geom]


	if bdd_use ~= self.last_bdd_use then
		ref.range_u    = pgr(   bdd_use, "u_range"    )
		ref.range_v    = pgr(   bdd_use, "v_range"    )
		ref.nb_u       = pgr(   bdd_use, "nb_u" 	  )
		ref.nb_v       = pgr(   bdd_use, "nb_v" 	  )
		ref.center_u   = pgr(   bdd_use, "u_center"   )
		ref.center_v   = pgr(   bdd_use, "v_center"   )
		ref.radius_int = pgrne( bdd_use, "radius_int" )
		ref.radius_top = pgrne( bdd_use, "radius_top" )
		ref.turn       = pgrne( bdd_use, "turn"       )
		ref.draw_mode  = pgr(   bdd_use, "draw_mode"  )
		self.last_bdd_use = bdd_use
	end

	local bdd_ref = pgr( self.layer , "bdd" )
	ps( bdd_ref       , self.bdd_names[selec_geom]     )
	ps( ref.axe       , self.axe_names[self.selec_axe] )
	ps( ref.factor    , self.size      )
	ps( ref.size_u    , self.size_u    )
	ps( ref.size_v    , self.size_v    )
	ps( ref.size_w    , self.size_w    )
	ps( ref.range_u   , self.range_u   )
	ps( ref.range_v   , self.range_v   )
	ps( ref.nb_u      , self.nb_u      )
	ps( ref.nb_v      , self.nb_v      )
	ps( ref.center_u  , self.center_u  )
	ps( ref.center_v  , self.center_v  )
	ps( ref.draw_mode , self.draw_mode  )
	if ref.radius_int then ps( ref.radius_int , self.torus_cone_radius ) end
	if ref.radius_top then ps( ref.radius_top , self.torus_cone_radius ) end
	if ref.turn       then ps( ref.turn       , self.turn              ) end
end


