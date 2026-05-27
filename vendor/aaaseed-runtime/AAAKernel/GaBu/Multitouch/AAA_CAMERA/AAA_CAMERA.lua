CLASS.DECLARE( "AAACAM" )

function AAACAM:create( name, obj_cam )
	AAACAM:set_class_status_doc(	CLASS.STATUS.CORE,
									"Encapsulate accesses to the C camera object" )

	local self = AAACAM:create_instance_with_obj( name, obj_cam )
	self.id = name	--	check if we need this

	local ref = self.ref
	if ref.obj then	--to deal with init process
		--todo add param.get_ref_xy, param.get_ref_xyz
		ref.perspective	=	param.get_ref( ref.obj, "perspective" )
		ref.ortho_size	=	param.get_ref( ref.obj, "ortho_size" )
		ref.ortho_sx	=	param.get_ref( ref.obj, "ortho_size_x" )
		ref.ortho_sy	=	param.get_ref( ref.obj, "ortho_size_y" )

		ref.b_viewport	=	param.get_ref( ref.obj, "sub_viewport" )
		ref.viewport_l	=	param.get_ref( ref.obj, "sub_viewport_left" )
		ref.viewport_r	=	param.get_ref( ref.obj, "sub_viewport_right" )
		ref.viewport_t	=	param.get_ref( ref.obj, "sub_viewport_top" )
		ref.viewport_b	=	param.get_ref( ref.obj, "sub_viewport_bottom" )

		ref.ui_lock		=	param.get_ref( ref.obj, "ui_lock" )
		ref.draw_axe	=	param.get_ref( ref.obj, "draw_axe" )

		ref.rot_x		=	param.get_ref( ref.obj, "rot_x" )
		ref.rot_y		=	param.get_ref( ref.obj, "rot_y" )
		ref.rot_z		=	param.get_ref( ref.obj, "rot_z" )

		ref.cen_x		=	param.get_ref( ref.obj, "center_x" )
		ref.cen_y		=	param.get_ref( ref.obj, "center_y" )
		ref.cen_z		=	param.get_ref( ref.obj, "center_z" )
		ref.tra_x		=	param.get_ref( ref.obj, "tra_x" )
		ref.tra_y		=	param.get_ref( ref.obj, "tra_y" )
		ref.tra_z		=	param.get_ref( ref.obj, "tra_z" )

		ref.focal		=	param.get_ref( ref.obj, "focal" )
		ref.frustum_x	=	param.get_ref( ref.obj, "frustum_offset_x" )
		ref.frustum_y	=	param.get_ref( ref.obj, "frustum_offset_y" )
	else
		self:box_error( "Can't found the AAASeed camera with this name" )
	end
	self:update()
	return self
end

function AAACAM:set_ui()
	aaa.camera.set_ui( self.ref.obj )
end

function AAACAM:set_perspective( b )		param.set( self.ref.perspective, b )	end
function AAACAM:get_perspective()			param.get_bool( self.ref.perspective )	end

function AAACAM:set_ui_lock( b )			param.set( self.ref.ui_lock, b )	end
function AAACAM:set_axe_draw( b )			param.set( self.ref.draw_axe, b )	end

function AAACAM:set_ortho_size( s )			param.set( self.ref.ortho_size, s )		end
function AAACAM:get_ortho_size()			return param.get( self.ref.ortho_size )	end
function AAACAM:get_ortho_max_size()		return self.ortho_sx, self.ortho_sy		end
function AAACAM:get_ratio_x()				return self.ratio_sx					end

function AAACAM:get_viewport()
	local ref = self.ref
	local pg = param.get
	if pg( ref.b_viewport ) > .5 then
		return pg( ref.viewport_l ),pg( ref.viewport_b ), pg( ref.viewport_r ),pg( ref.viewport_t )
	else
		return 0,0, 1,1
	end
end

function AAACAM.set_viewport_from_ref( cam_ref, b_on, l,b, r,t )
	if cam_ref==0 then
		aaa.debug.print_traceback( "cam_ref is 0" )
		aaa.box_error( "AAACAM.set_viewport_from_ref() cam_ref is 0, GP rendering off probably (F3)" )
	else
		local ps = param.set
		ps( cam_ref, "sub_viewport", b_on )
		if l and l < r and b < t then --this is gl convention
			aaa.print( "viewport "..l.." "..b.." "..r.." "..t )
			ps(	cam_ref,	"sub_viewport_left",	l	)
			ps(	cam_ref,	"sub_viewport_bottom",	b	)
			ps(	cam_ref,	"sub_viewport_right",	r	)
			ps(	cam_ref,	"sub_viewport_top",		t	)
		end
	end
end
function AAACAM:set_viewport( b_on, l,b, r,t )
	local ref = self.ref
	local ps = param.set
	ps( ref.b_viewport, b_on )
	if b_on then
		ps( ref.viewport_l, l )
		ps( ref.viewport_b, b )
		ps( ref.viewport_r, r )
		ps( ref.viewport_t, t )
	end
end

function AAACAM:set_frustum( fx, fy )
	local ref = self.ref
	local ps = param.set
	ps( ref.frustum_x, fx )
	ps( ref.frustum_y, fy )
end
function AAACAM:set_tra( x,y,z )
	local ref = self.ref
	local ps = param.set
	if x then ps( ref.tra_x, x ) end
	if y then ps( ref.tra_y, y ) end
	if z then ps( ref.tra_z, z ) end
end
function AAACAM:set_rot( x,y,z )
	local ref = self.ref
	local ps = param.set
	if x then ps( ref.rot_x, x ) end
	if y then ps( ref.rot_y, y ) end
	if z then ps( ref.rot_z, z ) end
end
function AAACAM:get_tra_xyz( z )
	local ref = self.ref
	return param.get( ref.tra_x ), param.get( ref.tra_y ), param.get( ref.tra_z )
end

function AAACAM:set_tra_z( z )	param.set( self.ref.tra_z, z )		end
function AAACAM:set_focal( f )	param.set( self.ref.focal, f )		end

function AAACAM:get_cen_xyz( z )
	local ref = self.ref
	local pg = param.get
	return pg( ref.cen_x ), pg( ref.cen_y ), pg( ref.cen_z )
end

function AAACAM:get_blobs_offset_scale()
	local sx,sy = self:get_ortho_max_size()
	--self:print( sx.." "..sy )
	--local l, b, r, t = self:get_viewport()
	--return -(r-l)*.5, -(t-b)*.5, (r-l)*8., (t-b)*sy*8./sx
	return -.5,-.5, sx,sy
end

function AAACAM:update()
	local ref = self.ref
	if ref.obj then
		aaa.obj.update_then_draw( ref.obj )
		self.ortho_sx = param.get( ref.ortho_sx )
		self.ortho_sy = param.get( ref.ortho_sy )
		self.ratio_sx = self.ortho_sx / self.ortho_sy
	end
end

