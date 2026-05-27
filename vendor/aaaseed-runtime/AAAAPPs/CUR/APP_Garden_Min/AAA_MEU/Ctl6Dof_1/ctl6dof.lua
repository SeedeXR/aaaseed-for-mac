--todo bad from beginning, no bdd ?

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local DY = .5
	local ix,iy = 1,1
	bu = self:add_button(		{ix,iy,		4,2}, 	"Control"	)--, ref.bdd, "camera_influence", false )

	ix = 9
	iy = 1

	bu = self:add_button(		{ix,iy,		4,2},	"Use",		self, "b_use", false )
	bu = self:add_trig_method(	{ix+4,iy,	4,2},	"Grab",		self, "grab_camera" )
	iy = iy + 2 + DY
	bu = self:add_selector(	{ix,iy,	8,2}, "CAM" )
		bu:set_nb( 8, 2 )
		bu:set_item_text_from_nb()
		bu:disable_mobile()
		bu:set_target_lua( self, "cam_id" )
--		bu:set_method_on_value_change( self, "update_cam_id", bu )
	iy = iy + 2

	bu = self:add_text_info(	{9,10,		8,2}, "Flying" )
		self.ui.bu_info_fly = bu
	bu = self:add_text_info(	{9,12,		8,2}, "Lock" )
		self.ui.bu_info_lock = bu

	bu = self:add_trig_method(	{1, 15}, "Focus", self, "set_focus_bdd" )
end
function meu:grab_camera()
	local gp = app:get_gp()
	local meu = gp:get_meu_ui()
	self.meu_targets[ self.cam_id ] = meu
--	self.cams[ self.cam_id ] = meu:get_camera_used()
end
function meu:set_focus_bdd( bu )
	aaa.obj.set_focus_ui( self.ref.bdd )
end
function meu:update_cam_id( bu )
	self.cam_id = bu:get_value()
end

function meu:init()
	local ref = self.ref
	ref.bdd = aaa.obj.get_no_error( aaa.dir.get_dir_tracker().."/default.sensor_6dof" )
	--aaa.box_error( "ref.bdd is "..ref.bdd )
	if ref.bdd then
		ref.use_target = param.get_ref( ref.bdd, "camera_influence_target" )
	end
	--self.cams = {}
	self.meu_targets = {}
end
--function meu:init_spe()


function meu:update()
	local ref = self.ref
	if not ref.use_target then return end
	param.set( ref.use_target, self.b_use )
	if self.b_use then
		local meu = self.meu_targets[ self.cam_id ]
		if meu then
			local cam = meu:get_camera_used()
			if cam then
				--local cam = self.cams[ self.cam_id ]
				aaa.obj.set_target( ref.bdd, 1, cam )
				self.__cam_controled = cam
			end

		end
	else
		self.__cam_controled = aaa.camera.get_ui()
	end
end

function meu:update_ui()
	local ui = self.ui
	local ref = self.ref

	local cam = self.__cam_controled
	if cam and ui.bu_info_fly then
		local b_fly = param.get_bool( cam, "flying" )
		ui.bu_info_fly:set_text_color( b_fly and "green" or "blue" )
		ui.bu_info_fly:set_text( b_fly and "Flying" or "STD" )
		local b_lock = param.get_bool( cam, "ui_lock" )
		ui.bu_info_lock:set_text_color( b_lock and "red" or "green" )
		ui.bu_info_lock:set_text( b_lock and "Locked" or "" )
	end
end