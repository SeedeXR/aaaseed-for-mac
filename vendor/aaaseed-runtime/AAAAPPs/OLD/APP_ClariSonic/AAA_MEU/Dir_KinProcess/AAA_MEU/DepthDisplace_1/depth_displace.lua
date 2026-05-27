function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local SY = .98
	local ix,iy = 1,1
	local SX = 8/3

	local sha =  self:get_shading()

	self:add_camera()
	self:add_rendering()

	ix, iy = 1, 1
	self:add_button(		{ix,iy},				"Draw",					self,	"b_draw_grid",	 		true )
	self:add_bu_texture_target_unit( {1,2}, "Src" )

	SX = 8/4
	iy = iy + SY*4.5
		self:add_slider(	{ix,iy,		8,SY},		"Dist Pixel",			self,	"pixel",				0, 0, 8. )
	iy = iy + SY*1
		bu = self:add_button( {ix,iy},				"Clip Depth Active", 	self,	"b_clip_depth",			true )
			bu:set_text_draw( false )
		self:add_slider_two({ix+1,iy,	8-1,SY},	"Clip Depth", 			self,	"clip_min", "clip_max", 0, 8, 0, 8 )
	iy = iy + SY*1.5
		self:add_button(	{ix,iy,			SX,SY},	"Move", 				self,	"b_move",				true )
		self:add_slider(	{ix+SX,iy,		SX,SY},	"RotX", 				self,	"rot_x",				0, -1, 1. ):set_meter(false)
		self:add_slider(	{ix+SX*2,iy,	SX,SY},	"RotY", 				self,	"rot_y",				0, -1, 1. ):set_meter(false)
		self:add_button(	{ix+SX*3,iy,	SX,SY},	"Flip_X", 				self,	"flip_x"				)

		--self:add_slider(	{ix+SX*2,iy,	SX,SY},	"RotZ", 				self,	"rot_z",				0, -1, 1. ):set_meter(false)

	SX = 8/3
	iy = iy + SY
		self:add_slider(	{ix,iy,			SX,SY},	"TraX", 				self,	"tra_x",				0, -10, 10. ):set_meter(false)
		self:add_slider(	{ix+SX,iy,		SX,SY},	"TraY", 				self,	"tra_y",				0, -10, 10. ):set_meter(false)
		self:add_slider(	{ix+SX*2,iy,	SX,SY},	"TraZ", 				self,	"tra_z",				0, -10, 10. ):set_meter(false)

	iy = iy + SY*1.2
		self:add_button( {		ix,			iy },	"Crop", 				self,	"b_crop",				false )
		self:add_slider_two(	{ix+4,iy,	4,SY},	"Z", 					self,	"z_min", "z_max",		-8, 8, -10, 10 )
	iy = iy + SY
		self:add_slider_two(	{ix,iy,		4,SY},	"X", 					self,	"x_min", "x_max",		-8, 8, -10, 10 )
		self:add_slider_two(	{ix+4,iy,	4,SY},	"Y", 					self,	"y_min", "y_max",		-8, 8, -10, 10 )


	iy = 2.2
	ix = 9

--	iy = iy + SY*2
		bu = self:add_selector(	{ix,iy,		8,SY*2},	"Test shader" )
			bu:set_text_draw( false )
			bu:set_nb_min_0( 4, 3 )
			bu:set_target_param( sha:get_ref_frag_int( 1 ) )
			bu:set_item_text( 1, "White", "TexD", "Grad", "UV", "NorD", "Nor01D", "Depth", "Mul", "Calage" )

	iy = iy + 2*SY
		self:add_button(		{ix,	iy },			"Shader",			sha.ref.active,		nil,	 true )
		bu = self:add_trig(		{ix+4,	iy },			"ReLoad",			sha.ref.reload_all,	nil,	false )
			ui.bu_reload = bu

	iy = iy + SY
		self:add_slider(		{ix,iy,		8,SY},		"Depth Size Max",	self, "edge_depth_size_max",	0.1, 0, 4. )
	iy = iy + SY
		self:add_slider(		{ix,iy,		8,SY},		"Depth Thres", 		sha:get_ref_geom_float(2), nil,	1, 0, 8.0)
	iy = iy + SY
		self:add_slider(		{ix,iy,		8,SY},		"Push",				sha:get_ref_geom_float(4), nil,	0, -.1, .1):set_meter(false)
	iy = iy + SY*1.5
		self:add_slider_two(	{ix,iy,		8,SY},		"Depth Range", 		self, "d_min", "d_max",					0, 8, 0, 8 )

--resizer
--	local DX = self:get_button_size_factor()
	local SX = 1
	local DX = self:get_button_size_factor()
	local DXH = (1-DX)*.5
	local DYT = .12

	ix = ix + DXH
	iy = iy + 1
		bu = self:add_text_info(	{ix+.1,iy+.1,	8,.7}, "titi" )
		ui.bu_info = bu

	iy = iy + 1
	local function add_resizer( x, y, sx, sy, name, text, method, arg1, arg2 )
		bu = self:add_trig_method(	{x,y,	 sx, sy},	name,	self, method, arg1, arg2 )
			bu:set_preset_use( false )
			bu:set_text( text )
			bu:set_text_xy( 0, DYT )
	end
	add_resizer(	ix,			iy,		SX, SY,		"div_x",	"/",	"mul_size", .5, 1	)
	add_resizer(	ix+DX,		iy,		SX, SY,		"mul_x",	"*",	"mul_size", 2, 1	)
	add_resizer(	ix+2,		iy,		SX, SY,		"div_y",	"/",	"mul_size", 1, .5	)
	add_resizer(	ix+2+DX,	iy,		SX, SY,		"mul_y",	"*",	"mul_size", 1, 2	)

	ix = ix - DXH
	iy = iy + 1
	add_resizer(	ix,			iy,		SX*2, SY,	"div_xy",	"/",	"mul_size", .5, .5	)
	add_resizer(	ix+SX*2,	iy,		SX*2, SY,	"mul_xy",	"*",	"mul_size", 2, 2	)

--[[
	iy = iy + SY
	bu = self:add_selector(	{ix,iy,	4,SY},		"Map" )
		bu:set_nb_min_0( 2, 1 )
		bu:set_target_param( param.get_ref( sha, "fu_int_01" ) )
		bu:set_item_text( 1, "Square" )
--	self:add_slider(	{ix+4,iy,	4,SY},		"Hardness", sha, "fu_float_01", 1, .5, 5. )
	self:add_slider_two(	{ix+4,iy,	4,SY},		"Hardness", sha, "fu_float_01", "fu_float_02", 0, 1, 0, 1. )
--]]
end

function meu:get_grid_size()
	local ref = self.ref
	return param.get( ref.nb_u ), param.get( ref.nb_v )
end
function meu:mul_size( fx, fy )
	local sx, sy = self:get_grid_size()
	local ref = self.ref
	param.set( ref.nb_u,  sx*fx )
	param.set( ref.nb_v,  sy*fy )
end

function meu:init()
	local ref = self.ref

	self:add_shading()
	ref.cam_kinect	= AAACAM:create( "kinect projector", self:get_camera( 7 ) )

	local bdd = self:get_layer_bdd(1)
	ref.nb_u = param.get_ref( bdd, "nb_u" )
	ref.nb_v = param.get_ref( bdd, "nb_v" )
end

function meu:update_ui()
	local sx, sy = self:get_grid_size()
	self.ui.bu_info:set_text( sx.." x "..sy )
end

function meu:update()
	local ref		= self.ref
	local sha 		= self:get_shading()
	local cam_kin	= ref.cam_kinect
	local ui		= self.ui

	if not self.__b_cam7_updated_done then
		--self:set_bu_value( "cam", 7 )
		--if not self.__b_cam7_updated then
		--end
	end

	if self.b_clip_depth then
		sha:set_vert_float_1_3( self.pixel, -self.clip_min, -self.clip_max )
	else
		sha:set_vert_float_1_3( self.pixel, 1000, -1000 )
	end

	local x, y, z = cam_kin:get_tra_xyz()
	sha:set_geom_vec4( 1, x, y, z, 0 )
	x, y, z = cam_kin:get_cen_xyz()
	sha:set_geom_vec4( 2, x, y, z, 0 )
	ui.bu_reload:set_text_color_problem_info( sha:is_valid() )

	sha:set_geom_int(	1, self.b_move and self.flip_x or 0 )
	sha:set_geom_float(	1, self.edge_depth_size_max * 256 / math.max( self:get_grid_size() ) )
	--self:print( math.max( self:get_grid_size() ) )

	if self.b_move then
		local angle, cos, sin

		angle = self.rot_x * math.pi2
		cos, sin = math.cos(angle), math.sin(angle)
		sha:set_vert_vec4( 2, cos, cos, -sin, sin )

		angle = self.rot_y * math.pi2
		cos, sin = math.cos(angle), math.sin(angle)
		sha:set_vert_vec4( 3, cos, cos, -sin, sin )

		sha:set_vert_vec4( 1, self.tra_x, self.tra_y, self.tra_z, 0 )
	else
		sha:set_vert_vec4( 2, 1, 1, 0, 0 )
		sha:set_vert_vec4( 3, 1, 1, 0, 0 )
		sha:set_vert_vec4( 1, 0, 0, 0, 0 )
	end

	if self.b_crop then
		sha:set_vert_vec4( 4, self.x_min, self.y_min, self.z_min )
		sha:set_vert_vec4( 5, self.x_max, self.y_max, self.z_max )
	else
		sha:set_vert_vec4( 4, -100, -100, -100 )
		sha:set_vert_vec4( 5,  100,  100,  100 )
	end
	local f = 1./(self.d_min - self.d_max);
	sha:set_frag_float_1_2( f, -self.d_max * f )
end

function meu:draw()
	--local cam = self:get_camera( 7 )
	--aaa.obj.update_then_draw( cam )

	if self.b_draw_grid then
		self:draw_layers_begin()
			self:draw_layers()
    	self:draw_layers_end()
	else
		--self:print( "skip draw" )
	end
end

