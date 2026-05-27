
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local sha = self:get_shading()
	local ix,iy = 1,1
	local SX = 8/3

	ix, iy = 1, 1
		self:add_button(	{ix,	iy },		"Draw",			self,	"b_draw_grid",	 		true )
		self:add_button(	{ix+1,	iy+1 },		"Cone",			self,	"b_draw_cone",	 		true ):set_text_rect_ratio(2)

	ix = 5
	local DY = .4
	local SX = 1
	local DX = self:get_button_size_factor()
	local DXH = (1-DX)*.5
	local DYT = .12

	ix = ix + DXH
		bu = self:add_text_info(	{ix+.1,iy+.1,	4,.7}, "Info nb uv" ):set_text_align_x( "center" )
		ui.bu_info = bu
	iy = iy + 1
	local SY = .5
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
	iy = iy + SY
	add_resizer(	ix,			iy,		SX*2, SY,	"div_xy",	"/",	"mul_size", .5, .5	)
	add_resizer(	ix+SX*2,	iy,		SX*2, SY,	"mul_xy",	"*",	"mul_size", 2, 2	)
	iy = iy + SY + DY

	SY = .8
	ix = 1
	SX = 8/4

	--GA.__b_pixel_debug = true
	--table.print( self, "meu", 2 )
	bu = self:add_slider(		{ix,iy,		8,SY},		"Normal Dist Pixel",	self,	"pixel",				1, 0, 16. )
		bu:add_values_def( 2,3,4,6,8,12 )
	iy = iy + SY
		bu = self:add_button(	{ix,iy,		SY,SY},				"Clip Depth Active", self,		"b_clip_depth",			true )
			bu:set_text_draw( false )
		self:add_slider_two(	{ix+1,iy,	8-1,SY},	"Clip Depth", 		self,		"clip_min", "clip_max", 0, 8, 0, 8 )
	iy = iy + SY + DY

--BEGIN transfo
	SX = 8/4
	SY = 1
	local ST,SR
	ST,SR = 3/3,4/3
		bu = self:add_button(	{ix,iy,		SY,SY},		"Plan",				self,		"b_plan",	 		true )
		self.ui.bu_plan = bu
	ix = ix + SY
	self:begin_bu_group( "plan" )
		self:add_slider(	{ix,iy,			ST,SY},		"Tx", 				self.plan,	"tx",			0, -10, 10. ):set_meter(false):set_color_back("x"):add_values_def(-3,-2,-1,0,1,2,3)
		self:add_slider(	{ix+ST,iy,		ST,SY},		"Ty", 				self.plan,	"ty",			0, -10, 10. ):set_meter(false):set_color_back("y"):add_values_def(-3,-2,-1,0,1,2,3)
		self:add_slider(	{ix+ST*2,iy,	ST,SY},		"Tz", 				self.plan,	"tz",			0, -10, 10. ):set_meter(false):set_color_back("z"):add_values_def(-3,-2,-1,0,1,2,3)
		ix = ix + ST *3
		self:add_slider(	{ix,iy,			SR,SY},		"Ry", 				self.plan,	"ry",			0, -1, 1. ):set_meter(false):set_color_back("y"):add_values_def(-.75,-.5,.25,0,.25,.5,.75)
		self:add_slider(	{ix+SR,iy,		SR,SY},		"Rx", 				self.plan,	"rx",			0, -1, 1. ):set_meter(false):set_color_back("x"):add_values_def(-.75,-.5,.25,0,.25,.5,.75)
		self:add_slider(	{ix+SR*2,iy,	SR,SY},		"Rz", 				self.plan,	"rz",			0, -1, 1. ):set_meter(false):set_color_back("z"):add_values_def(-.75,-.5,.25,0,.25,.5,.75)
		ix = ix - ST *3
	self:end_bu_group()
	ix = ix - SY

	iy = iy + SY + DY
		--self:add_button(		{ix,iy,		SY,	SY },	"Move", 			self.trs,	"b_move",				true )
		bu = self:add_selector(	{ix,iy,		3,SY*.8},	"Move"		):set_target_lua(	self,	"s_move"	):set_nb(3):set_item_text( 1, "NO", "Def", "Use" )
			bu:set_value(2)
			ui.bu_trs = bu
		bu = self:add_trig_method(	{ix+3,iy,	2,SY*.8},	"Clear",			self,		"clear_trs" 	):set_color_back( "restart" )
		--self:print( "before add_selector()" )
		bu = self:add_selector(	{ix+5,iy,	3,SY*.8},	"Move_id"	)
			--self:print( "before set_target_lua()" )
			bu:set_target_lua( self, "move_id" )
			--self:print( "before set_nb()" )
			bu:set_nb(4)
			--self:print( "before set_item_text_from_nb()" )
			bu:set_item_text_from_nb()
		--self:box_debug( "Move_id is "..self.move_id )
		--todo we should read (eventually write) when we change one of the two selector
		--bu:set_hook_on_value_change( function( bu, balue ) self:print( bu.." value_change to ".. balue:get_value() ) end )
			ui.bu_trs_id = bu

	self:begin_bu_group( "move" )
	ST,SR = 8/3,2
	iy = iy + SY * .8
		self:add_slider(	{ix,iy,			ST,SY},		"CenX", 			self.trs,	"cen_x",		0, -10, 10. ):set_meter(false):set_color_back("x")
		self:add_slider(	{ix+ST,iy,		ST,SY},		"CenY", 			self.trs,	"cen_y",		0, -10, 10. ):set_meter(false):set_color_back("y")
		self:add_slider(	{ix+ST*2,iy,	ST,SY},		"CenZ", 			self.trs,	"cen_z",		0, -10, 10. ):set_meter(false):set_color_back("z")

	iy = iy + SY
		self:add_slider(	{ix,iy,			SR,SY},		"RotZ", 			self.trs,	"rot_z",		0, -1, 1. ):set_meter(false):set_color_back("z"):add_values_def(-.75,-.5,-.25,0,.25,.5,.75)
		self:add_slider(	{ix+SR,iy,		SR,SY},		"RotX", 			self.trs,	"rot_x",		0, -1, 1. ):set_meter(false):set_color_back("x"):add_values_def(-.75,-.5,-.25,0,.25,.5,.75)
		self:add_slider(	{ix+SR*2,iy,	SR,SY},		"RotY", 			self.trs,	"rot_y",		0, -1, 1. ):set_meter(false):set_color_back("y"):add_values_def(-.75,-.5,-.25,0,.25,.5,.75)
		self:add_button(	{ix+SR*3,iy,	SR,SY},		"Swap_XY", 			self.trs,	"b_swap_xy",	false	)
		
	iy = iy + SY
		self:add_slider(	{ix,iy,			ST,SY},		"TraX", 			self.trs,	"tra_x",		0, -10, 10. ):set_meter(false):set_color_back("x")
		self:add_slider(	{ix+ST,iy,		ST,SY},		"TraY", 			self.trs,	"tra_y",		0, -10, 10. ):set_meter(false):set_color_back("y")
		self:add_slider(	{ix+ST*2,iy,	ST,SY},		"TraZ", 			self.trs,	"tra_z",		0, -10, 10. ):set_meter(false):set_color_back("z")
	iy = iy + SY
	SR = 6/3
		self:add_slider(	{ix,iy,			SR,SY},		"RotXY", 			self.trs,	"rot_xy",		0, -1, 1. ):set_meter(false):set_color_back("z"):add_values_def(.5):add_values_def(-.75,-.5,-.25,0,.25,.5,.75)
		self:add_button(	{ix+SR,iy,		SR,SY},		"Flip_X", 			self.trs,	"b_flip_x",		false	)
		self:add_slider(	{ix+SR*2,iy,	SR,SY},		"ScaX", 			self.trs,	"sca_x",		1., .33, 3. ):set_meter(false):set_color_back("x")
		self:add_slider(	{ix+SR*3,iy,	SR,SY},		"ScaY", 			self.trs,	"sca_y",		1., .33, 3. ):set_meter(false):set_color_back("y")
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
		self:add_slider_two(	{ix,iy,		SX,SY},		"X", 				self.crop,	"x_min", "x_max",		-8, 8, -10, 10 ):set_color_back("x"):add_values_def( 0, 1 )
		self:add_slider_two(	{ix+SX,iy,	SX,SY},		"Y", 				self.crop,	"y_min", "y_max",		-8, 8, -10, 10 ):set_color_back("y"):add_values_def( 0, 1 )
		self:add_slider_two(	{ix+SX*2,iy,SX,SY},		"Z", 				self.crop,	"z_min", "z_max",		-8, 8, -10, 10 ):set_color_back("z"):add_values_def( 0, 1 )
	iy = iy + SY
	self:end_bu_group()

	SY = .8
	iy = iy + DY
		self:add_button( 		{ix,iy,		SY,SY},		"Edge Size Max Active",	self, "b_edge_size_max",	true ):set_text_draw( false )
		self:add_slider(		{ix+1,iy,	3,SY},		"Edge Size Max",		self, "edge_size_max",	0.1, 0, 1.0 )
		self:add_button( 		{ix+4,iy,	SY,SY},		"Edge Ratio Max Active",self, "b_edge_ratio_max",	true ):set_text_draw( false )
		self:add_slider(		{ix+5,iy,	3,SY},		"Edge Ratio Max",		self, "edge_ratio_max",	4., 1., 16. )
--	iy = iy + SY
--		self:add_slider(	{ix,iy,	8,SY},		"Depth Thres", 			sha:get_ref_geom_float(2), nil,	1, 0, 8.0)
	iy = iy + SY
		self:add_slider(		{ix+1,iy,	7,SY},		"Push",					self, "push",	0, -.1, .1):set_meter(false)
		self:add_button(		{ix,iy,		SY,SY},		"Push Active", 			self, "b_push",	true ):set_text_draw( false )


	ix = 9
	iy = 1
	self:add_camera()
	iy = iy + 1 + DY
	self:add_rendering( {ix,iy} )
	iy = iy + 1.6 + DY*2


	SY = .8
		self:add_shading_ui(	{ix,iy,		8,SY} )
	iy = iy + SY
	
		bu = self:add_button(	{ix+1,iy,	SY,SY},		"ModelView", 	self,	"b_modelview",			true ):set_text_rect_ratio( 7 )
		ui.bu_modelview = bu
	iy = iy + SY + DY

		bu = self:add_selector(	{ix,iy,		8,SY*3},	"Shader Main" )
			bu:set_text_draw( false )
			bu:set_nb( 5, 3 )
			bu:set_target_param( sha:get_ref_frag_int( 1 ) )
			bu:set_item_text( 1, "White", "TEX", "Grad", "UV", "Calage", "Nor01D", "NorD", "Depth", "Mul", "XYZ World", "XYZ", "X", "Y",  "Z", "XYZ Kinect" )
		ui.bu_sel_shader_main = bu
	iy = iy + 3*SY

		bu = self:add_selector(	{ix,iy,		4,SY*2},	"SHADER Opt" )
			bu:set_text_draw( false )
			bu:set_nb( 3, 2 )
			bu:set_target_param( sha:get_ref_frag_int( 2 ) )
			bu:set_item_text( 2, "Nor 01", "Nor", "RGB", "Nor 01", "Nor" )
		ui.bu_sel_shader_opt = bu

		bu = self:add_button(	{ix+4,iy,	SY,SY},		"Combine outs",		self,	"b_combine",	true )
	iy = iy + 2*SY

		self:add_slider(		{ix+1,iy,	5,SY},		"Width cm", 		self, "plan_width",		0, 1, 30 ):add_values_def( 1,2,3,4,5,10,25 )
		bu = self:add_button(	{ix,iy,		SY,SY},		"Width Active", 	self, "b_plan_width",	true )
			bu:set_text_draw( false )
			ui.bu_plan_width_active = bu
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

function meu:push_bu( name, val )
	local tab = self.__pushed
	local bu =  self.ui[name]
	table.insert( tab, { bu=bu, val=bu:get_value() } )
	bu:set_value( val )
end
function meu:pop_bu_all()
	for _, elt in IPAIRS(self.__pushed) do
		elt.bu:set_value(elt.val)
	end
end
function meu:set_for_pick()
	self:print( "set_for_pick" )

	local ui = self.ui
	self.__pushed = {}

	self:push_bu( "bu_plan", false )
	self:push_bu( "bu_trs", 0 )
	self:push_bu( "bu_crop", 0 )
	self:push_bu( "bu_cam_show", false )
	self:push_bu( "bu_cam_sel", 0 )
	self:push_bu( "bu_plan_width_active", false )
	ui.bu_modelview:set_value( false )
end
function meu:push_for_pick()
	self:print( "push_for_pick" )
 	local ui = self.ui
	self:push_bu( "bu_sel_shader_main", 15 )
	self:push_bu( "bu_sel_shader_opt", 1 )
end
function meu:pop_for_pick()
	self:print( "pop_for_pick" )
	local ui = self.ui
	ui.bu_modelview:set_value( true )
	self:pop_bu_all()
end

function meu:set_plan_rot( x, y, z )
	local plan = self.plan
	plan.rx = x
	plan.ry = y
	plan.rz = z
end
function meu:set_plan_tra( x, y, z )
	local plan = self.plan
	plan.tx = x
	plan.ty = y
	plan.tz = z
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
function meu:clear_trs()
	local trs = self.trs
	trs.cen_x, trs.cen_y, trs.cen_z = 0, 0, 0
	trs.rot_x, trs.rot_y, trs.rot_z = 0, 0, 0
	trs.b_swap_xy = false
	trs.tra_x, trs.tra_y, trs.tra_z = 0, 0, 0
	trs.rot_xy  = 0
	trs.b_flip_x = false
	trs.sca_x, trs.sca_y = 1,1
end
function meu:init()
	local ref = self.ref

	self:add_shading()
	ref.cam_kinect	= AAACAM:create( "kinect projector", self:get_camera( 7 ) )

	local layer = self:get_layer(1)

	for i=0,1 do
		local mapping = aaa.layer.get_tex_unit_mapping( layer, i )
		--self:box_debug( i.." mapping is "..mapping )
		param.set( mapping, "tex_2d_minification", 0 )
		param.set( mapping, "tex_2d_magnification", 0 )
	end

	local bdd = self:get_layer_bdd(1)
	ref.nb_u = param.get_ref( bdd, "nb_u" )
	ref.nb_v = param.get_ref( bdd, "nb_v" )

	self.trs = {}
	self.crop = {}
	self.plan = {}

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

	self:set_bu_group_active( "plan", self.b_plan )
	ui.bu_plan:set_text_visible(not self.b_plan)
	self:set_bu_group_active( "move", self.s_move == 2 )
	ui.bu_trs:set_color_back( self.s_move == 1 and "bui_pale" or "BUI" )
	ui.bu_trs_id:set_active( self.s_move > 1 )
	self:set_bu_group_active( "crop", self.s_crop == 2 )
	ui.bu_crop:set_color_back( self.s_crop == 1 and "bui_pale" or "BUI" )
	ui.bu_crop_id:set_active( self.s_crop > 1 )

	aaa.show( self.trs.b_flip_x, "b_flip_x"  )
	aaa.show( self:get_bu_value(		"move.flip_x"), "value" )
	aaa.show( self:get_bu_value_as_bool("move.flip_x"), "value as bool" )
	--self.show( 0, "b_flip_x" )
	--self:print( "b_flip_x is "..self.trs.b_flip_x )
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

	sha:set_geom_int( 2, self.b_geom and 1 or 0 )

	local v
	if self.b_edge_size_max then
		v = self.edge_size_max / nb;
		v = v*v*1000
	else
		v = 0
	end
	sha:set_geom_float(	1, v )
	sha:set_geom_float(	2, self.b_edge_ratio_max and self.edge_ratio_max or 0. )

--BEGIN trs
	--self:print( math.max( self:get_grid_size() ) )
	local trs
	--self.move_id = self.move_id or 0
	--if true then return end	--hacknow

	if self.s_move > 1 then
		local pro = self:get_proto()
		if self.s_move == 2 then	--this MEU define
			--self:print( "self.move_id : "..self.move_id )
			--table.print( pro.__trs, "pro.__trs", 3 )
			pro.__trs[self.move_id] = self.trs
		end
		trs = pro.__trs[self.move_id]
	end
	if trs then
		sha:set_vert_int( 1, trs.b_swap_xy and 1 or 0 )
		sha:set_geom_int( 1, trs.b_flip_x and 1 or 0 )

		sha:set_vert_vec4_cos_sin( 1, trs.rot_z, trs.rot_x )

		sha:set_vert_vec4( 3, trs.tra_x, trs.tra_y, trs.tra_z, 0 )

		sha:set_vert_vec4_cos_sin( 4, trs.rot_y, trs.rot_xy )

		sha:set_vert_vec4( 5, trs.sca_x, trs.sca_y, 0, 0 )
		sha:set_vert_vec4( 6, trs.cen_x, trs.cen_y, trs.cen_z, 0 )
	else
		sha:set_vert_int( 1, 0 )
		sha:set_geom_int( 1, 0 )

		sha:set_vert_vec4_cos_sin( 1, 0, 0 )

		sha:set_vert_vec4( 3, 0, 0, 0, 0 )

		sha:set_vert_vec4_cos_sin( 4, 0, 0 )

		sha:set_vert_vec4( 5, 1, 1, 0, 0 )
		sha:set_vert_vec4( 6, 0, 0, 0, 0 )
	end

	if self.b_plan then
		local plan = self.plan
		sha:set_vert_vec4( 9, plan.tx, plan.ty, plan.tz )
		sha:set_vert_vec4_cos_sin( 10, plan.rx, plan.ry )
		sha:set_vert_vec4_cos_sin( 11, plan.rz )
	else
		sha:set_vert_vec4( 9, 0,0,0,0 )
		sha:set_vert_vec4_cos_sin( 10, 0, 0 )
		sha:set_vert_vec4_cos_sin( 11, 0 )
	end
--END trs
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
	sha:set_geom_float( 4, self.b_push and self.push or 0 )

	sha:set_frag_int( 3, self.b_combine and 1 or 0 )

	self:update_range_rgb_xyz( sha )

	sha:set_frag_float_1( self.b_plan_width and (self.plan_width*.005) or 0  )
end

function meu:draw()
	--local cam = self:get_camera( 7 )
	--aaa.obj.update_then_draw( cam )

	if self.b_draw_grid then
		self:draw_layers_begin()
			self:draw_layer(1)
			if self.b_draw_cone then
				self:draw_layer(7)
			end
		self:draw_layers_end()
	else
		--self:print( "skip draw" )
	end
end

