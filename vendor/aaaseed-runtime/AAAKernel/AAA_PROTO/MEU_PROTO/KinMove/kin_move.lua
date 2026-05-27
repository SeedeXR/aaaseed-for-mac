function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "2d", "3d", "Camera", "texture", "unfinished", "depreciated" },
			help = "Depreciated use MEU KinMoveAuto, process kinect flux to project the 3d point cloud\n"..
					"used in some Old applications"
		}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local sha = self:get_shading()
	local ix,iy = 1,1
	local SX = 8/3

	ix, iy = 1, 1
		self:add_button(	{ix,	iy },					"Draw",					self,	"b_draw_grid",	 		true )

	ix = 4
	local DY = .4
	local SX = 1
	local DX = self:get_button_size_factor()
	local DXH = (1-DX)*.5
	local DYT = .12

	ix = ix + DXH

		bu = self:add_text_info(	{ix+.1,iy+.1,	4.5,.7}, "titi" )
		ui.bu_info = bu
	iy = iy + 1
	local SY = .5
	local function add_resizer( x,y, sx,sy, name, text, method, arg1, arg2 )
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
	iy = iy + SY
	add_resizer(	ix,			iy,		SX*2, SY,	"div_xy",	"/",	"mul_size", .5, .5	)
	add_resizer(	ix+SX*2,	iy,		SX*2, SY,	"mul_xy",	"*",	"mul_size", 2, 2	)
	iy = iy + SY + DY

	SY = .8
	ix = 1
	SX = 8/4

	--GA.__b_pixel_debug = true
	--table.print( self, "meu", 2 )
	bu = self:add_slider(		{ix,iy,		8,SY},		"Normal Dist Pixel",	self,	"pixel",				1, 0,16. )
		bu:add_values_def( 2,3,4,6,8,12 )
	iy = iy + SY
		bu = self:add_button(	{ix,iy 		},			"Clip Depth Active", 	self,	"b_clip_depth",			true )
			bu:set_text_draw( false )
		self:add_slider_two(	{ix+1,iy,	8-1,SY},	"Clip Depth", 			self,	"clip_min", "clip_max", 0,8, 0,8 )
	iy = iy + SY + DY

--BEGIN transfo
	SX = 8/4
	SY = 1
	local ST = 2
	local SR = 2
		--self:add_button(		{ix,iy,		SY,SY},		"Move", 				self.trs,	"b_move",				true )
		bu = self:add_selector(	{ix,iy,		4,SY*.8},	"Move"		):set_target_lua(	self,	"s_move"	):set_nb(3):set_item_text( 1, "NO", "Def", "Use" )
			bu:set_value(2)
			ui.bu_trs = bu
		bu = self:add_selector(	{ix+4,iy,	4,SY*.8},	"Move_id"	):set_target_lua(	self,	"move_id"	):set_nb(4):set_item_text_from_nb()
		--todo we should read (eventually write) when we change one of the two selector
		--bu:set_hook_on_value_change( function( bu, balue ) self:print( bu.." value_change to ".. balue:get_value() ) end )
			ui.bu_trs_id = bu
			
	self:begin_bu_group( "move" )
	iy = iy + SY*.8
		self:add_slider(		{ix,iy,		SR,SY},		"RotZ", 	self.trs,	"rot_z",				0, -1, 1. ):set_meter(false):set_color_back("z"):add_values_def(.5)
	iy = iy + SY
		self:add_slider(		{ix,iy,		ST,SY},		"CenX", 	self.trs,	"cen_x",				0, -10, 10. ):set_meter(false):set_color_back("x")
		self:add_slider(		{ix+ST,iy,	ST,SY},		"CenY", 	self.trs,	"cen_y",				0, -10, 10. ):set_meter(false):set_color_back("y")
		self:add_slider(		{ix+ST*2,iy,ST,SY},		"CenZ", 	self.trs,	"cen_z",				0, -10, 10. ):set_meter(false):set_color_back("z")


	iy = iy + SY
		self:add_slider(		{ix+SR,iy,		SR,SY},	"RotX", 	self.trs,	"rot_x",				0, -1, 1. ):set_meter(false):set_color_back("x")
		self:add_slider(		{ix+SR*2,iy,	SR,SY},	"RotY", 	self.trs,	"rot_y",				0, -1, 1. ):set_meter(false):set_color_back("y")
		--self:add_slider(		{ix+SX*2,iy,	SX,SY},	"RotZ", 	self,		"rot_z",				0, -1, 1. ):set_meter(false)
		self:add_button(		{ix+SR*3,iy,	SR,SY},	"Swap_XY", 	self.trs,	"b_swap_xy"				)--:set_text_rect_ratio(2)

	iy = iy + SY
		self:add_slider(		{ix,iy,			ST,SY},	"TraX", 	self.trs,	"tra_x",				0, -10, 10. ):set_meter(false):set_color_back("x")
		self:add_slider(		{ix+ST,iy,		ST,SY},	"TraY", 	self.trs,	"tra_y",				0, -10, 10. ):set_meter(false):set_color_back("y")
		self:add_slider(		{ix+ST*2,iy,	ST,SY},	"TraZ", 	self.trs,	"tra_z",				0, -10, 10. ):set_meter(false):set_color_back("z")
	iy = iy + SY
	SR = 6/3
		self:add_slider(		{ix,iy,			SR,SY},	"RotXY", 	self.trs,	"rot_xy",				0, -1, 1. ):set_meter(false):set_color_back("z"):add_values_def(.5)
		self:add_button(		{ix+SR,iy,		SR,SY},	"Flip_X", 	self.trs,	"b_flip_x"				):set_color_back("x")
		self:add_slider(		{ix+SR*2,iy,	SR,SY},	"ScaX", 	self.trs,	"sca_x",				1., .33, 3. ):set_meter(false):set_color_back("x")
		self:add_slider(		{ix+SR*3,iy,	SR,SY},	"ScaY", 	self.trs,	"sca_y",				1., .33, 3. ):set_meter(false):set_color_back("y")
	self:end_bu_group()
--END transfo


	local SX = 8/3
	iy = iy + SY + DY
		bu = self:add_selector(	{ix,iy,		4,SY*.8},	"Crop"		):set_target_lua(	self,	"s_crop"	):set_nb(3):set_item_text( 1, "NO", "Def", "Use" )
			bu:set_value(2)
			ui.bu_crop = bu
		bu = self:add_selector(	{ix+4,iy,	4,SY*.8},	"Crop_id"	):set_target_lua(	self,	"crop_id"	):set_nb(4):set_item_text_from_nb()
			ui.bu_crop_id = bu

	self:begin_bu_group( "crop" )
	iy = iy + SY*.8
		self:add_slider_two(	{ix,iy,		SX,SY},		"X", 		self.crop,	"x_min", "x_max",		-8, 8, -10, 10 ):set_color_back("x"):add_values_def( 0, 1 )
		self:add_slider_two(	{ix+SX,iy,	SX,SY},		"Y", 		self.crop,	"y_min", "y_max",		-8, 8, -10, 10 ):set_color_back("y"):add_values_def( 0, 1 )
		self:add_slider_two(	{ix+SX*2,iy,SX,SY},		"Z", 		self.crop,	"z_min", "z_max",		-8, 8, -10, 10 ):set_color_back("z"):add_values_def( 0, 1 )
	iy = iy + SY
	self:end_bu_group()

	SY = .8
	iy = iy + DY
		self:add_slider(	{ix+1,iy,		7,SY},		"Edge Depth Max",		self, "edge_depth_size_max",	0.1, 0, 1.0 )
		self:add_button( 	{ix,iy,			SY,SY},		"Depth Size Max Active",self, "b_edge_depth_size_max",	true ):set_text_draw( false )
--	iy = iy + SY
--		self:add_slider(	{ix,iy,	8,SY},		"Depth Thres", 			sha:get_ref_geom_float(2), nil,	1, 0, 8.0)
	iy = iy + SY
		self:add_slider(	{ix+1,iy,		7,SY},		"Push",					self, "push",	0, -.1, .1):set_meter(false)
		self:add_button( 	{ix,iy,			SY,SY},		"Push Active", 			self, "b_push",	true ):set_text_draw( false )


	ix = 9
	iy = 1
	self:add_camera()
	iy = iy + 1 + DY
	self:add_rendering( {ix,iy} )
	iy = iy + 2 + DY


	SY = .8
	
		self:add_shading_ui(	{ix,iy,		8,SY} )
	iy = iy + SY

		bu = self:add_selector(	{ix,iy,		8,SY*3},	"Test shader" )
			bu:set_text_draw( false )
			bu:set_nb_min_0( 4, 3 )
			bu:set_target_param( sha:get_ref_frag_int( 1 ) )
			bu:set_item_text( 1, "White", "TexD", "Grad", "UV", "NorD", "Nor01D", "Depth", "Mul", "Z to RGB", "Y", "Calage" )
	iy = iy + 3*SY

		self:add_slider(		{ix+1,iy,	3,SY},		"Calage", 				self, "calage",		0, 0, 1. ):set_meter(false)
		bu = self:add_button(	{ix,iy,		SY,SY},		"Calage Active", 		self, "b_calage",	true )
			bu:set_text_draw( false )
	iy = iy + SY*1.5

	self:begin_bu_group( "out" )
		self:add_range_rgb_xyz( {ix,iy,	8,SY*3} )
	iy = iy + SY*3 + DY
	self:end_bu_group()

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

	self.trs = {}
	self.crop = {}

	local p = self:get_proto()
	if not p.__crop then
		p.__trs = {}
		p.__crop = {}
	end

end

function meu:update_ui()
	local ui = self.ui

	local sx, sy = self:get_grid_size()
	ui.bu_info:set_text( sx.." x "..sy )

	self:set_bu_group_active( "move",	self.s_move == 2 )
	ui.bu_trs:set_color_back( self.s_move == 1 and "bui_pale" or "BUI" )
	ui.bu_trs_id:set_active( self.s_move > 1 )
	self:set_bu_group_active( "crop",	self.s_crop == 2 )
	ui.bu_crop:set_color_back( self.s_crop == 1 and "bui_pale" or "BUI" )
	ui.bu_crop_id:set_active( self.s_crop > 1 )
end

function meu:update()
	local ref		= self.ref
	local sha		= self:get_shading()
	local cam_kin	= ref.cam_kinect
	local ui		= self.ui

	local nbx, nby = self:get_grid_size()
	local nb = math.min( nbx, nby )

	if not self.__b_cam7_updated_done then
		--self:set_bu_value( "cam", 7 )
		--if not self.__b_cam7_updated then
		--end
	end

	if self.b_clip_depth then	sha:set_vert_float_1_3( self.pixel * .5, self.clip_min, self.clip_max )
	else						sha:set_vert_float_1_3( self.pixel * .5, .1,			1000 )
	end

	local x, y, z = cam_kin:get_tra_xyz()
	sha:set_geom_vec4( 1, x, y, z, 0 )
	x, y, z = cam_kin:get_cen_xyz()
	sha:set_geom_vec4( 2, x, y, z, 0 )


	sha:set_geom_float(	1, self.b_edge_depth_size_max and 100.*(self.edge_depth_size_max / nb) or 100. )

--BEGIN trs
	--self:print( math.max( self:get_grid_size() ) )
	local trs
	if self.s_move > 1 then
		local pro = self:get_proto()
		if self.s_move == 2 then	--this MEU define
			pro.__trs[self.move_id] = self.trs
		end
		trs = pro.__trs[self.move_id]
	end
	if trs then
		sha:set_vert_int( 1, trs.b_swap_xy )
		sha:set_geom_int( 1, trs.b_flip_x )

		local angle_z, angle_x
		angle_z = trs.rot_z * math.pi2
		angle_x = trs.rot_x * math.pi2
		sha:set_vert_vec4( 1, math.cos(angle_z), math.sin(angle_z), math.cos(angle_x), math.sin(angle_x) )

		local angle, cos, sin
		angle = trs.rot_y * math.pi2
		cos, sin = math.cos(angle), math.sin(angle)
		sha:set_vert_vec4( 2, cos, cos, -sin, sin )

		--Translation
		sha:set_vert_vec4( 3, trs.tra_x, trs.tra_y, trs.tra_z, 0 )

		angle = trs.rot_xy * math.pi2
		cos, sin = math.cos(angle), math.sin(angle)
		sha:set_vert_vec4( 4, cos, cos, -sin, sin )

		sha:set_vert_vec4( 5, trs.sca_x, trs.sca_y, 0, 0 )
		sha:set_vert_vec4( 6, trs.cen_x, trs.cen_y, trs.cen_z, 0 )
	else
		sha:set_vert_int( 1, 0 )
		sha:set_geom_int( 1, 0 )

		sha:set_vert_vec4( 1, 1, 0, 1, 0 )
		sha:set_vert_vec4( 2, 1, 1, 0, 0 )
		sha:set_vert_vec4( 3, 0, 0, 0, 0 )
		sha:set_vert_vec4( 4, 1, 1, 0, 0 )
		sha:set_vert_vec4( 5, 1, 1, 0, 0 )
		sha:set_vert_vec4( 6, 0, 0, 0, 0 )
	end
--END trs

	sha:set_geom_float( 4, self.b_push and self.push or 0 )

--BEGIN crop
	local crop
	if self.s_crop > 1 then
		local pro = self:get_proto()
		if self.s_crop == 2 then	--this MEU define
			pro.__crop[self.crop_id] = self.crop
		end
		crop = pro.__crop[self.crop_id]
	end
	if crop then
		sha:set_vert_vec4(	7,	crop.x_min,	crop.y_min,	crop.z_min	)
		sha:set_vert_vec4(	8,	crop.x_max,	crop.y_max,	crop.z_max	)
	else
		sha:set_vert_vec4(	7,	-10000,		-10000,		-10000		)
		sha:set_vert_vec4(	8,	 10000,		 10000,		 10000		)
	end
--END crop

	self:update_range_rgb_xyz( sha )

	sha:set_frag_float_1( self.b_calage and self.calage or 0  )
end

function meu:draw()
	--local cam = self:get_camera( 7 )
	--aaa.obj.update_then_draw( cam )

	if self.b_draw_grid then
		MEU.draw( self )
	else
		--self:print( "skip draw" )
	end
end

