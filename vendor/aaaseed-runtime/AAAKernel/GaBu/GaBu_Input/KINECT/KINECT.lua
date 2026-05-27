
CLASS.DECLARE( "KINECT" )

function KINECT:init( kinect_layers_ref )
	self:print( "init()" )
	self.ui		= {}
	self.ref	= {}
	local ref = self.ref

	ref.kinect_layers	=	kinect_layers_ref	--aaa.obj.get_branch_by_name_symbo( module, "kinect" )

	--todo perhaps install waiting mecanism because of window boot but this time inside the capture scripts
	aaa.obj.update_then_draw( ref.kinect_layers )

	--KINECT
	ref.tex_video	=	aaa.obj.get_down_by_class( ref.kinect_layers, "tex_video" )
		ref.img_index	=	param.get_ref( ref.tex_video, "image_index"  )
		ref.open		=	param.get_ref( ref.tex_video, "capture_open"  )
		ref.run			=	param.get_ref( ref.tex_video, "capture_run"  )
		ref.device_name	=	param.get_ref( ref.tex_video, "capture_device_name"  )

		ref.kinect	=	aaa.obj.get_down_by_class( ref.tex_video, "kinect_ui" )
			ref.skel_selected_id	=	param.get_ref( ref.kinect, "skeleton_selected_id" )
			ref.skel_selected_x		=	param.get_ref( ref.kinect, "skeleton_selected_x" )
			ref.skel_selected_y		=	param.get_ref( ref.kinect, "skeleton_selected_y" )
			ref.skel_selected_z		=	param.get_ref( ref.kinect, "skeleton_selected_z" )
			ref.kinect_nb			=	param.get_ref( ref.kinect, "camera_nb_present" )
	--[[
	param.set( ref.kinect, "skeleton_select_min_x", -.6 )
	param.set( ref.kinect, "skeleton_select_max_x", .3 )
	param.set( ref.kinect, "skeleton_select_min_y", -1. )
	param.set( ref.kinect, "skeleton_select_max_y", 9.261 )
	param.set( ref.kinect, "skeleton_select_min_z", 2 )
	param.set( ref.kinect, "skeleton_select_max_z", 3.30921 )
	--]]
			ref.skel_nb		=	param.get_ref( ref.kinect, "skeleton_nb_tracked" )
			ref.body_nb		=	param.get_ref( ref.kinect, "body_seen_nb" )
			ref.head_u		=	param.get_ref( ref.kinect, "head_pos_u" )
			ref.head_v		=	param.get_ref( ref.kinect, "head_pos_v" )

	self.device_name = param.get( ref.device_name )
end

function KINECT:create( kinect_layers_ref )
	local self = KINECT:create_instance_no_name()
	self:init( kinect_layers_ref )
	return self
end

function KINECT:restart()
	param.set( self.ref.open, false )
	aaa.obj.update_then_draw( self.ref.kinect_layers )
	param.set( self.ref.open, true )
	param.set( self.ref.run, true )
	self:update_device_name()
end
function KINECT:get_nb()			return param.get( self.ref.skel_nb )			end
function KINECT:get_selected_id()	return param.get( self.ref.skel_selected_id )	end
function KINECT:get_img_index()		return param.get( self.ref.img_index )			end

function KINECT:get_head_pos()
	local ref = self.ref
	return param.get( ref.head_u ), param.get( ref.head_v )
end

function KINECT:update_device_name()		self.device_name = param.get( self.ref.device_name )	end
function KINECT:get_device_name()		return self.device_name									end

function KINECT:updraw()
	--self:print( self:get_device_name() )
	if self:is_active() then
		--self:print( "Kinect active" )
		aaa.obj.update_then_draw( self.ref.kinect_layers )
	end
end

function KINECT:define_ui_base( bus )
	bus:add_but_target_lua(		"Kinect On",		nil,	self, "__b_active", param.get( self.ref.kinect_nb ) > 0 )
	bus:add_but_trig_fn(		"Restart",			nil,	self.restart, self )
	bus:add_but_target_param(	"Seated",			nil,	param.get_ref( self.ref.kinect, "seated" ) )
end
function KINECT:define_ui_bis( bus )
	bus:add_but_trig_fn(		"Focus Capture",	nil,	aaa.obj.set_focus_ui, 	self.ref.tex_video )
	bus:add_but_trig_fn(		"Focus Kinect",		nil,	aaa.obj.set_focus_ui,	self.ref.kinect )
end
