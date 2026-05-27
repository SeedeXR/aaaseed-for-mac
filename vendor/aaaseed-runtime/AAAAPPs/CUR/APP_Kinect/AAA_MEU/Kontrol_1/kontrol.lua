

-- function meu.pong( ip, b_answer )
--     local meu_remote = "RemoteKinect_1"
-- 	if b_answer then
-- 		aaa.print( "Got ping from ip : "..ip )
-- 		ip = aaa.net.ip[1]
-- 		aaa.print( "Sending pong with our ip : "..ip )
-- 		aaa.net.lua_send( 8, 1, "app:get_meu_by_name( \""..meu_remote.."\" ).pong( \""..ip.."\" )" )
-- 	else
-- 		aaa.print( "Got pong from ip : "..ip )
-- 	end
-- end

-- function meu:ping()
--     local ip = aaa.net.ip[1]
--     local meu_remote = self:get_name()
--     self:print( "Sending ping to "..meu_remote.." with our ip : "..ip )
-- 	aaa.net.lua_send( 8, 1, "app:get_meu_by_name( \""..meu_remote.."\" ).pong( \""..ip.."\", \""..true.."\" ) " )
-- end



function meu:__send( str )
	local ip = aaa.net.ip[1]
    local meu_remote = self:get_name()
    self:print( "Sending to "..meu_remote.." with our ip : "..ip )
	self:print( str )
	aaa.net.lua_send( 0, 1, str )
end
function meu:set_rgb( b_on )
	self:__send( "app:get_meu_by_name( \"KinMoveAuto_1\" ):set_bu_value( \"SHADER Opt\", "..(b_on and 4 or 1).." )"  )
end
function meu:set_crop( val )
	self:__send( "app:get_meu_by_name( \"KinMoveAuto_1\" ):set_bu_value( \"Crop\", "..val.." )"  )
end

function meu:set_plan_local( b_on )
	self:get_meu_by_name( "KinMoveAuto_1" ):set_bu_value( "Plan", b_on )
end
function meu:set_move_local( val )
	self:get_meu_by_name( "KinMoveAuto_1" ):set_bu_value( "Move", val )
end
function meu:set_crop_local( val )
	self:get_meu_by_name( "KinMoveAuto_1" ):set_bu_value( "Crop", val )
end
function meu:set_edge_local( b_on )
	self:get_meu_by_name( "KinMoveAuto_1" ):set_bu_value( "Depth Size Max Active", b_on )
end
function meu:set_edge_depth_local( val )
	self:get_meu_by_name( "KinMoveAuto_1" ):set_bu_value( "Edge Depth Max", val )
end
function meu:set_rgb_local( b_on )
	self:get_meu_by_name( "KinMoveAuto_1" ):set_bu_value( "SHADER Opt", b_on and 4 or 1 )
	self:get_meu_by_name( "fbo_F1" ):set_bu_value( "channel", b_on and 4 or 1 )
	self:get_meu_by_name( "Kinect_2" ):set_bu_value( "RGBasked", b_on and 1 or 0 )
end
function meu:get_fps_local()
	self.fps = aaa.time.get_str_fps()
end
function meu:do_save_meu_local( )
	self:get_meu_by_name( self.meu_targets[self.id_meu_target] ):save()
end
function meu:do_save_all_local()
	MEU:save_all()
end
function meu:print_mus()
	GP.cur:get_mus_down():print_mu_rendered()
end
function meu:do_f2_local()
	ga:flip_ui_group_active( "top" )
	ga:flip_ui_group_active()
end

function meu:do_remote( method_name, ... )
	local vars = ...
	self:__send( "app:get_meu_by_name( \"Kontrol_1\" ):\""..method_name.."\"( \""..vars.."\" )" )
end

-- b_remote : network call if on
function meu:do_trig( b_remote, method_name, ... )
	local vars = ...
	self:print( "method : "..method_name..", b_remote : "..b_remote..", ... : "..vars )
	if b_remote then
		--local meu_remote = "Kontrol_1"
		self:do_remote( method_name, ... )
	else
		self[method_name]( self, ... )
	end
end

function meu:define_ui()
	local ix, iy = 1.5, 1.5
	local SX, SY = 2, 1
	local ui = self.ui
	local bu
--	table.print( bus_cur, self..".bus_cur" )
	print( bus_cur )
	self:print( bus_cur )

--	self:add_trig_method(	{ix,  	iy,			SX, SY}, "Toto", self, "toto" )
--	iy = iy + 1
	--self:add_trig_method(	{ix,	iy,			SX, SY}, "RGB", self, "set_rgb", true )
	--self:add_trig_method(	{ix+SX,	iy,			SX, SY}, "White", self, "set_rgb", false )
	--iy = iy + 1
	--self:add_trig_method(	{ix,	iy,			SX, SY}, "Crop No", self, "set_crop", 1 )
	--self:add_trig_method(	{ix+SX,	iy,			SX, SY}, "Crop", self, "set_crop", 2 )
	ui.bu_fps = self:add_text_info(	{1.5,1,	8,1}, "Fps : " )
	iy = iy + 1
	self:add_trig_method(	{ix,	iy,			SX,	SY},	"Plan No",	self,	"do_trig", false, "set_plan_local", false )
	self:add_trig_method(	{ix+SX,	iy,			SX,	SY},	"Plan",		self,	"do_trig", false, "set_plan_local", true )
	iy = iy + 1
	self:add_trig_method(	{ix,	iy,			SX,	SY},	"Move No",	self,	"do_trig", false, "set_move_local", 1 )
	self:add_trig_method(	{ix+SX,	iy,			SX,	SY},	"Move",		self,	"do_trig", false, "set_move_local", 2 )
	iy = iy + 1
	self:add_trig_method(	{ix,	iy,			SX,	SY},	"Crop No",	self,	"do_trig", false, "set_crop_local", 1 )
	self:add_trig_method(	{ix+SX,	iy,			SX,	SY},	"Crop",		self,	"do_trig", false, "set_crop_local", 2 )
	iy = iy + 1
	self:add_trig_method(	{ix,	iy,			SX, SY},	"RGB",		self,	"do_trig", false, "set_rgb_local", true  )
	self:add_trig_method(	{ix+SX,	iy,			SX, SY},	"White",	self,	"do_trig", false, "set_rgb_local", false  )
	iy = iy + 1
	self:add_trig_method(	{ix,	iy,			SX,	SY},	"Do Save",		self,	"do_trig", false, "do_save_meu_local" ):set_color_back( "save" )
	self:add_trig_method(	{ix+SX,	iy,			SX,	SY},	"Print_mus",	self,	"do_trig", false, "print_mus" )
	iy = iy + 1.5
	bu = self:add_selector(	{ix,iy,				5,2},		"Target MEU" )
		bu:set_nb( 3, 2 )
		bu:set_item_text( 1, "Kinect_2", "Ref_Fbo_F1", "KinMoveAuto_1", "DepthPick_1", "ImgSend_KShaded" )
		bu:set_target_lua( self, "id_meu_target" )
	iy = iy + 2
	self:add_trig_method(	{ix,	iy,			SX,	SY},	"Do Save All",	self,	"do_trig", false, "do_save_all_local" ):set_color_back( "save" )
	iy = iy + 1
	self:add_trig_method(	{ix,	iy,			SX,	SY},	"Do F2",		self,	"do_trig", false, "do_f2_local" )

	ix, iy = 7, 2.5
	self:add_trig_method(	{ix,	iy,			SX,	SY},	"Edge No",		self,	"do_trig", false, "set_edge_local", false )
	self:add_trig_method(	{ix+SX,	iy,			SX,	SY},	"Edge",			self,	"do_trig", false, "set_edge_local", true )
	ix = ix + SX * 2
	ui.bu_send_edge = self:add_trig_method(	{ix,iy,	SX,SY},	"Send_edge",	self,	"do_trig", false, "set_edge_depth_local", self.edge_depth )
	self:add_slider(		{ix+SX,	iy,			4,SY},		"Edge_depth", 	self,	"edge_depth",	0.5, 0., 1. )


end

function meu:init()
	--self.ip = aaa.net.ip[1]
	self.edge_depth = .5
	self.fps = 0
	self.meu_targets = { "Kinect_2", "Ref_Fbo_F1", "KinMoveAuto_1", "DepthPick_1", "ImgSend_KShaded" }
end

function meu:update_ui()
	local ui = self.ui
	local SX, SY = 2, 1
	local ix, iy = 9, 1.5
--	ui.bu_send_edge = self:add_trig_method(	{ix,		iy,			SX,	SY},	"Send_edge",		self,	"do_trig", false, "set_edge_depth_local", self.edge_depth )
	self:get_fps_local()
	ui.bu_fps:set_text( "Fps : "..self.fps )
end

function meu:update()
end
