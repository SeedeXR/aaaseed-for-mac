if CLASS.DECLARE( "SKEL" ) then
	SKEL.point_name_to_id = {
							hip				= 1,
							spine			= 2,
							neck			= 3,
							head			= 4,
							shoulder_left	= 5,
							elbow_left		= 6,
							wrist_left		= 7,
							hand_left		= 8,
							shoulder_right	= 9,
							elbow_right		= 10,
							wrist_right		= 11,
							hand_right		= 12,
							hip_left		= 13,
							knee_left		= 14,
							angle_left		= 15,
							foot_left		= 16,
							hip_right		= 17,
							knee_right		= 18,
							angle_right		= 19,
							foot_right		= 20,
							}
	SKEL.point_id_to_name = table.reverse( SKEL.point_name_to_id )
end

function SKEL:new_point( id )
	local pt = {}
	pt.id	= id
	pt.start	= V3.new()
	pt.cur		= V3.new()
	pt.prev		= V3.new()
	pt.speed	= V3.new()
	return pt
end

function SKEL:init( module )
	if self.gesture then return end

	self:print( "init()" )
	self.ui		= {}
	self.ref	= {}
	local ref = self.ref

	ref.module	= module
	ref.skel_layers			=	aaa.obj.get_branch_by_name_symbo( module, "skeleton" )


	--todo perhaps reinstantiate waiting mecanism because of window boot but this time inside the capture scripts
	aaa.obj.update_then_draw( ref.skel_layers )

	--SKELETON
	ref.skel_layer_1		=	aaa.obj.get_branch_by_name_symbo( ref.skel_layers, "skel_layer_1" )
		ref.skel_usebdd_1		=	param.get_ref(  ref.skel_layer_1, "use_bdd" )
		ref.skel_mocap_1		=	aaa.obj.get_branch_by_class(  ref.skel_layer_1, "bdd_mocap" )
			ref.skel_draw_number_1	=	param.get_ref(  ref.skel_mocap_1, "draw_number" )
	ref.skel_layer_2		=	aaa.obj.get_branch_by_name_symbo( ref.skel_layers, "skel_layer_2" )
		ref.skel_usebdd_2		=	param.get_ref(  ref.skel_layer_2, "use_bdd" )
		ref.skel_mocap_2		=	aaa.obj.get_branch_by_class(  ref.skel_layer_2, "bdd_mocap" )
			ref.skel_draw_number_2	=	param.get_ref(  ref.skel_mocap_2, "draw_number" )
	ref.skel_attr			=	aaa.obj.get( "m42_skel_attr" )
	ref.skel_attr_layer		=	aaa.obj.get( "m42_skel_attr_layer" )
	ref.skel_cam			=	aaa.obj.get( "m42_skel_cam" )

	--ANALYSE
	ref.kinect_anal			=	aaa.obj.get_branch_by_name_symbo( module, "kinect_anal_max" )
	aaa.obj.update_then_draw( ref.kinect_anal )

	--todo integrate in skel update
	self.anal = { ref = {} }
	local ref = self.anal.ref
	ref.bdd = aaa.obj.get( "kinect_process_anal" )
		ref.u = param.get_ref(	ref.bdd, "max_u" )
		ref.v = param.get_ref(	ref.bdd, "max_v" )
		ref.d = param.get_ref(	ref.bdd, "max_grey" )
		ref.skew = param.get_ref(	ref.bdd, "find_max_skew_y" )

	self.points = {}
	local pts = self.points
	for name, id in pairs(SKEL.point_name_to_id) do
		pts[name]	= self:new_point( id )
	end
	pts.shoulder		=	self:new_point( id )
	--pts.tra = pts.shoulder
	pts.shoulder.start		= 	V3.new()
	self.dist_hand_shoulder_left = 0
	self.dist_hand_shoulder_right = 0
	self.multitouch = {}
	self.point_to_update =	{}
	self.gesture = {}
	self.gesture.cur = {}
	self.gesture.last = {}
	self.gesture.info = {}
	self.gesture.delay_to_reallow = .5

	self.scope_3d			=	SCOPE_3D:create( "direct" )
	self.scope_3d_anal		=	SCOPE_3D:create( "anal" )
	self.scope_3d_anal_raw	=	SCOPE_3D:create( "anal_raw" )
	self.scope_3d_angle		=	SCOPE_3D:create( "angle" )

	ref = self.ref
	self.mocap_channel_1	=	param.get( ref.skel_mocap_1, "channel_id" )
	self.mocap_channel_2	=	param.get( ref.skel_mocap_2, "channel_id" )

	self.transfo = TRANSFO_THREE:create( self:get_name() )
	local transfo = self.transfo
	transfo:set_channel( self.mocap_channel_1 )
end

--todo pass the layers not the module (be more generic)
function SKEL:create( module, kinect_layers_ref )
	local self = SKEL:create_instance_no_name()
	if aaa.lua.global.get( "KINECT" ) then
		self.kinect = KINECT:create( kinect_layers_ref )
	else
		self:box_error( "no KINECT obj ready, clean this code please (maa July 2015)" )
	end
	self:init( module )
	return self
end

--
--	ANAL
--
function SKEL:update_anal_data()
	local anal = self.anal
	local ref = anal.ref
	local u = param.get( ref.u )	-- cano 0 1
	local v = param.get( ref.v )
	local d = param.get( ref.d )
	u = u - .5
	v = v - .5
	anal.u_raw = u
	anal.v_raw = v
	anal.d_raw = d
	--self:print( "u\t\t\t\t\t\t"..u )
	u = u - u * d * .15
	local c = m42.location.comp_y_center
	v = v - c
	v = v - v * d * m42.location.comp_y_factor
	v = v + c
	anal.u = u
	anal.v = v
	anal.d = d
end
function SKEL:get_anal_data()
	local anal = self.anal
	return anal.u, anal.v, anal.d
end
function SKEL:get_anal_data_raw()
	local anal = self.anal
	return anal.u_raw, anal.v_raw, anal.d_raw
end


function SKEL:get_point_cur( name )	return self.points[name].cur	end
--
--	Node to track
--
function SKEL:add_node_to_track( name, circle_pt_nb, size )
	self.point_to_update[name] = { cir=circle_pt_nb, size=size and size or 1. }
end
function SKEL:add_node_double_to_track( name, ... )
	self:add_node_to_track(	name.."_left",	...	)
	self:add_node_to_track(	name.."_right",	...	)
end
function SKEL:remove_node_to_track( name )	self.point_to_update[name] = nil	end
function SKEL:set_top_to_track()
	self:add_node_double_to_track(	"shoulder",	6	)
	self:add_node_double_to_track(	"elbow",	5,	.5	)
	self:add_node_double_to_track(	"wrist",	4,	.5	)
	self:add_node_double_to_track(	"hand",		3	)
	self:add_node_to_track(		"neck",		8,  .5 )
	self:add_node_to_track(		"head",		12	)
end

function SKEL:read_node( node )
	return { aaa.bdd.get_point_s( self.ref.mocap, node, 0 ) }
end
--[[
function skel:get_node_bary( dst, node_1, node_2 )
	local p1 = {}
	local p2 = {}
	self:get_node( p1, node_1 )
	self:get_node( p2, node_2 )
	V3.bary( dst, p1, p2 )
end
--]]

function SKEL:is_on()			return self.kinect:get_nb() > 0		end
function SKEL:get_angle()		return self.angle or 0			end
function SKEL:get_speed()		return self.speed or 0			end
function SKEL:get_speed_lat()	return self.speed_lat or 0			end


--todo	add a king of validity time
function SKEL:add_gesture( name )
	--aaa.print( "add_gesture "..name )
	local ges = self.gesture
	local info = ges.info[name]
	if not info or not info.when or aaa.time.t > info.when + ges.delay_to_reallow  then
		self.gesture.cur[name] = {}
		return true
	end
	return false

end
function SKEL:store_gesture( name )
	self.gesture_stored = name
	self:print( "store_gesture "..name )
end
function SKEL:set_gesture_used( name )
	local ges = self.gesture
	if not ges.info[name] then
		ges.info[name] = {}
	end
	ges.info[name].when = aaa.time.t
	self:print( "set_gesture_used "..name )
end
function SKEL:add_gesture_verbose( name )
	--self:print( "try to add_gesture "..name )
	if self:add_gesture( name ) then
		--self:print( "add_gesture "..name )
		aaa.mess.show( name )
	end
end

function SKEL:is_gesture( name )
	return self.gesture.cur[name] and true or false
end
function SKEL:find_gesture_arm_straight_one( post )
	local pts = self.points
	local shoulder	= pts["shoulder_"..post]
	local hand		= pts["hand_"..post]
	local d = V3.dist( shoulder.cur, hand.cur )
	self[ "dist_hand_shoulder_"..post ] = d
	local limit = (self.shoulder_size * 1.1 )
	--aaa.print( post.." "..d.." "..limit )
	if d > limit then
		--aaa.print( "arm_straight_"..post )
		self:add_gesture( "arm_straight_"..post )
	end
end
function SKEL:find_gesture_arm()
	self:find_gesture_arm_straight_one( "left" )
	self:find_gesture_arm_straight_one( "right" )
	if self:is_gesture( "arm_straight_left" ) and self:is_gesture( "arm_straight_right" ) then
		self:add_gesture( "arm_straight" )
	end
end
function SKEL:find_hand_align_one( what, post, i1, i2 )
	local pts = self.points
	local sh	= pts["shoulder_"..post]
	local prefix = "hand_"..post
	local hd	= pts[prefix]
	local d = V2.dist_from_xyxy( sh.cur[i1], sh.cur[i2], hd.cur[i1], hd.cur[i2] )
	local name = prefix.."_"..what
	--aaa.print( name.." "..d )
	if d < self.align_delta then
		self:add_gesture( name )
	end
end
function SKEL:find_hand_align( what, i1, i2 )
	self:find_hand_align_one( what, "left", i1, i2 )
	self:find_hand_align_one( what, "right", i1, i2 )
	if self:is_gesture("hand_left_"..what) and self:is_gesture("hand_right_"..what) then
		self:add_gesture( "hand_"..what )
	end
end
function SKEL:find_hand_side()
	self:find_hand_align( "front",	1, 2 )
	self:find_hand_align( "side",	2, 3 )
	self:find_hand_align( "up",		1, 3 )
end
function SKEL:find_hand_shoulder_close()
	local pts = self.points
	local d = .15
	if V3.dist( pts.hand_left.cur, pts.shoulder_left.cur ) < d		then	self:add_gesture( "hand_shoulder_close_left" )	end
	if V3.dist( pts.hand_right.cur, pts.shoulder_right.cur ) < d	then	self:add_gesture( "hand_shoulder_close_right" )	end
	if self:is_gesture("hand_shoulder_close_left") and self:is_gesture("hand_shoulder_close_right") then
		self:add_gesture( "hand_shoulder_close" )
	end
end
function SKEL:find_hand_joined()
	local pts = self.points
	--aaa.mess.show( "ooto"..V3.dist( pts.hand_left.cur, pts.hand_right.cur ) )
	if V3.dist( pts.hand_left.cur, pts.hand_right.cur ) < .15 then
		self:add_gesture( "hand_joined" )
	end
end
function SKEL:find_final()
	local pts = self.points
	if self:is_gesture("arm_straight") then
		if self:is_gesture("hand_side") then
			self:add_gesture_verbose( "reset_pos" )
		elseif self:is_gesture("hand_up") then
			local b_l = pts.hand_left.cur[2] > pts.shoulder_left.cur[2]
			local b_r = pts.hand_right.cur[2] > pts.shoulder_right.cur[2]
			if b_l then
				if b_r then
					self:print( "out from hands up" )
					self:add_gesture_verbose( "out" )
				else
					self:add_gesture_verbose( "cross left up" )
				end
			else
				if b_r then
					self:add_gesture_verbose( "cross right" )
				else
					self:add_gesture_verbose( "relax" )
				end
			end
		elseif self:is_gesture("hand_front") then
			self:add_gesture_verbose( "front" )
		end
		if self:is_gesture("front") then
			self:add_gesture_verbose( "in" )
		end
	else
		if self:is_gesture("hand_joined") then
			self:print( "in from hand joined" )
			--self:add_gesture_verbose( "in" )
		elseif self:is_gesture("hand_shoulder_close_left") and self:is_gesture("hand_shoulder_close_right") then
			self:print( "out from hand on shoulder" )
			self:add_gesture_verbose( "out" )
		end
	end
end


function SKEL:is_tracking()		return self.kinect:is_active() and  self.selected_id ~= 0		end

function SKEL:find_gesture()
	--self:print( "find_gesture" )
	local pts = self.points
	self.align_delta = .15
	self.gesture.last = self.gesture.cur
	self.gesture.cur = {}
	if self.gesture_stored then
		self:print( "use gesture stored "..self.gesture_stored )
		self:add_gesture_verbose( self.gesture_stored )
		self.gesture_stored = nil
	end
	if self:is_tracking() then
		self.shoulder_size = V3.dist( pts.shoulder_left.cur, pts.shoulder_right.cur )
		self:find_gesture_arm()
		self:find_hand_side()
		self:find_hand_joined()
		self:find_hand_shoulder_close()
		self:find_final()
	end
end

function SKEL:set_pt_start( pt )		V3.cpy( pt.start, pt.cur )		end
function SKEL:reset_pos()
	local pts = self.points
	self:set_pt_start( pts.shoulder )
	self:set_pt_start( pts.shoulder_left )
	self:set_pt_start( pts.shoulder_right )

	local d1 = V3.dist( pts.shoulder_left.cur, pts.elbow_left.cur )
	local d2 = V3.dist( pts.shoulder_right.cur, pts.elbow_right.cur )
	local d = (d1 + d2) * .5

	self.elbow_h = pts.shoulder.cur[2] - d
end

function SKEL:update_pt( pt, p )
	V3.cpy( pt.prev, pt.cur )
	V3.cpy( pt.cur, p )
	V3.compute_speed( pt.speed, p, pt.prev, self.over_dt )
end

function SKEL:update_pts( targets_names )
	local pts = self.points
	targets_names = targets_names or self.point_to_update
	for name, t  in pairs(targets_names) do
		local pt = pts[name]
		local p = self:read_node( pt.id )
		self:update_pt( pt, p )
	end
end

function SKEL:update()
	self.over_dt = 1. / ( ( aaa.time.dt~=0 ) and aaa.time.dt or 0.04 )
	local ref = self.ref

	--	Update kinect
	self.kinect:updraw()
	--	Update mocap
	param.set( ref.skel_usebdd_1, "update" )
	aaa.obj.update_then_draw( ref.skel_layer_1 )
	ref.mocap = ref.skel_mocap_1

	local id = self.kinect:get_selected_id()

	--	Deal with skeleton change
	if self:is_kinect_active() then
		if self.selected_id ~= id then
			local str
			if id == 0 then
				str = "NO Skeleton"
			else
				str = "change Skeleton"
				self.b_had_one = true
				self:reset_pos()
			end
			aaa.print( str )
			aaa.mess.show( str )
			self.selected_id = id
		end
	end

	local pts = self.points
	--	Update points if there is a skeleton
	if self:is_tracking() then
		self:update_pts()
		local p = {}
		V3.bary( p, pts.shoulder_left.cur, pts.shoulder_right.cur )
		self:update_pt( pts.shoulder, p )

		local head = self:get_point_cur( "head" )
		--todo clean this
		param.set( "m42_bearstech_cam_lookat", "tra_offset_x", head[1] )
		param.set( "m42_bearstech_cam_lookat", "tra_offset_y", -head[2]+1.75 )
		param.set( "m42_bearstech_cam_lookat", "tra_offset_z", -head[3]+1.5 )
		--param.set( "m42_bearstech", tra_offset_y  )
	--aaa.print( "doit" )
	end

	self:find_gesture()

	if self:is_gesture( "reset_pos" ) then
		self:reset_pos()
	end

	if m42 then
		aaa.obj.update_then_draw( ref.kinect_anal )
		self:update_anal_data()
	end

	--self:print( id )
	if not self:is_tracking() then return end


	--	get shoulder angle in vertical plane for m42
	local a = pts.shoulder_left.cur
	local b = pts.shoulder_right.cur
	self.angle = math.atan2( b[3] - a[3], b[1] - a[1] ) * math.pi_over
	self.angle = wrap_01( self.angle + .5 ) - .5
	--	get speed in vertical plane for m42
	local pt = pts.shoulder
	self.speed = pt.cur[3] - pt.start[3]
	--aaa.print( "speed "..self.speed )
	--local pt = pts.shoulder
	self.speed_lat = pt.cur[1] - pt.start[1]
	--aaa.print( "speed lat "..self.speed )

	--	this is done for the Garden App
	function write_midi( ch, ctl, a1, a2, a3, a4 )
		aaa.midi.set_control( ch, ctl, a1 )
		aaa.midi.set_control( ch, ctl+1, a2 )
		aaa.midi.set_control( ch, ctl+2, a3 )
		aaa.midi.set_control( ch, ctl+3, a4 )
	end

	function do_sh_hand( post, ctl )
		local shoulder	= pts[ "shoulder_"..post]
		local hand		= pts[ "hand_"..post	]
		local dif = {}
		V3.sub( dif, hand.cur, shoulder.cur )
		local d = V3.norm( dif )
		--write_midi( 5, ctl, dif[1], -dif[2], dif[3], clamp_01( (1.-d) - ((post==left) and .5 or .2 ) ) )
	end
	do_sh_hand( "left", 1 )
	do_sh_hand( "right", 5 )

	local pt = self.points.hand_right.cur
	local v = { pt[1], pt[2], pt[3] }
	self.scope_3d:add( v )
	---[[
	v[1], v[2], v[3] = self:get_anal_data_raw()
	self.scope_3d_anal_raw:add( v )
	m42.scope.anal_raw_x:add( v[1] )
	m42.scope.anal_raw_y:add( v[2] )
	m42.scope.anal_raw_z:add( v[3] )

	v[1], v[2], v[3] = self:get_anal_data()
	self.scope_3d_anal:add( v )

	m42.scope.anal_x:add( v[1] )
	m42.scope.anal_y:add( v[2] )
	m42.scope.anal_z:add( v[3] )
	--]]
	local elt = m42.scope.hand_right_x:get_last()
	if elt then
		v[1] = elt.v
		local elt = m42.scope.hand_right_y:get_last()
		v[2] = elt.v
		v[3] = math.cos( v[1] * math.pi2 ) * math.cos( v[2] * math.pi2 )
		self.scope_3d_angle:add( v )
	end

--[[
	local pt = self.points.shoulder_left
	local dif = V3.sub( pt.start, pt.cur )
	local d = V3.norm( dif )
	write_midi( 5, 1, dif[1], dif[2], dif[3], d )
--]]

end

function SKEL:get_tra_lat_rot()
	if self:is_tracking() then
		tra = - self:get_speed() * 8 or 0
		rot = - self:get_angle() * 6 or 0
		lat =  self:get_speed_lat() * 8
		return	tra, lat, rot
	end
	return 0, 0, 0
end

function SKEL:draw_line( a, b )		aaa.draw_line( a[1], a[2], a[3], b[1], b[2], b[3] )		end
function SKEL:draw_segment( a, b )	self:draw_line( a.cur, b.cur )							end

function SKEL:draw_gesture_straight()
	local pts = self.points
	if self:is_gesture( "arm_straight_left" ) then
		self:draw_segment( pts.shoulder_left,  pts.hand_left )
	end
	if self:is_gesture( "arm_straight_right" ) then
		self:draw_segment( pts.shoulder_right,  pts.hand_right )
	end
end

--
--	DRAW DRESS
--
function SKEL:init_dress()
	if self.dress then return end
end
function SKEL:draw_maa_segment( n1, n2, n3 )
	local tr = self.transfo
	gol.push_matrix()
		tr:set_start_node( n1 )
		tr:set_stop_node( n2 )
		if n3 then
			tr:set_third_node( n3 )
		end
		tr:update_then_draw()
		aaa.draw_box( .5, .1, 1. )
		--aaa.draw_teapot( 1., 10. )
	gol.pop_matrix()
end

function SKEL:draw_attr()
	aaa.obj.update_then_draw( self.ref.skel_attr_layer )
end
--
--	was before bodys
--[[
function SKEL:draw_maa()

	local tr = self.transfo
	--tr:set_focus_ui()

	tr:set_obj_active( true )
	tr:set_type( 3 )
	tr:set_axe( 2 )
	tr:set_scale( 1 )
	tr:set_inter( .5 )

	self:draw_maa_segment( 5, 6, 3 )
	self:draw_maa_segment( 6, 7 )

	self:draw_maa_segment( 9, 10, 3 )
	self:draw_maa_segment( 10, 11 )

	self:draw_maa_segment( 13, 14 )
	self:draw_maa_segment( 14, 15 )

	self:draw_maa_segment( 17, 18 )
	self:draw_maa_segment( 18, 19 )
end
function SKEL:draw_dress()
	self:draw_attr()
	self:draw_maa()
end
--]]

function SKEL:draw_before()
	local layers = self.ref.skel_attr
	aaa.obj.update_then_draw( layers )
--	aaa.obj.become_ui( cam )
end
--
--	DRAW SKEL
--
function SKEL:draw_low( b_visu )
	--self:draw_maa()
	--	this draw using the mocap object
	local ref = self.ref
	param.set( ref.skel_draw_number_1, b_visu and "Billboard" or "No" )
	param.set( ref.skel_usebdd_1, "draw" )
	aaa.obj.update_then_draw( ref.skel_layer_1 )
end

function SKEL:draw_unit()
	gol.color_white()
	gol.set_line_width( 1 )
	local HS = .5
	aaa.draw_line( -HS, 0, 0, HS, 0, 0 )
	aaa.draw_line( 0, -HS, 0, 0, HS, 0 )
end

function SKEL:draw_skel_only( b_visu, fn_draw_circle )
	local pts = self.points

	gol.color_white()
	gol.set_texture_dim( 0 )
		self:draw_low( true )
--[[
	gol.set_line_width( 2 )
		self:draw_line( pts.shoulder_left.cur, pts.shoulder_right.cur )
	gol.color_cyan()
		self:draw_line( pts.shoulder.cur, pts.shoulder.start )
--]]
--[[
	gol.color_cyan()
		self.scope_3d:draw( 0, 0, 0 )
	gol.color_green()
		self.scope_3d_anal:draw( 0, 0, 0 )
	gol.color_magenta()
		self.scope_3d_angle:draw( 0, 0, 0 )
--]]
	if fn_draw_circle then
		for name, t  in pairs(self.point_to_update) do
			local pt = pts[name]
			local p = self:read_node( pt.id )
			gol.color_green()
			if name == "hand_left" or name == "hand_right" then
				if self:is_gesture(name.."_front") or self:is_gesture(name.."_side") or self:is_gesture(name.."_up") then
					gol.color_red()
				else
					gol.color_green()
				end
			else
				gol.color_green()
			end
			fn_draw_circle( p[1], p[2], p[3], .1*t.size, t.cir  )
		end
	end

---[[
	gol.color_red( .6 )
	gol.set_line_width( 8 )
	self:draw_gesture_straight()
--]]
end

function SKEL:draw( b_visu )
	if not self.b_had_one then return end

	--self:print( self.b_had_one )
	gol.set_texture_dim( 0 )
	local angle = 0

	if not b_visu then
		gol.color_green()
		self:draw_skel_only( b_visu )	--, aaa.draw_circle_axe_z )
		return
	end

--	aaa.print( b_visu )
	gol.push_attrib()
		gol.push_matrix()

			local pts = self.points

			gol.translate( 4, 2 )
			gol.set_line_width( 2. )
			gol.color_green()
			aaa.draw_str_xy( "Top", 1.2, 1.6, .40, .40 )
			gol.color_blue()
			aaa.draw_str_xy( "Front", .90, -1.9, .40, .40 )

			gol.scale( -2, 2, 2 )

			--self:draw_unit()
			--	FRONT
			local offset_y = -pts.shoulder.cur[2] - .5
			gol.translate( 0, offset_y, 0 )
			gol.rotate_x( angle )
				self:draw_skel_only( b_visu, aaa.draw_circle_axe_z )
			gol.rotate_x( -angle )
			gol.translate( 0, -offset_y, 0 )

			--	TOP
			local offset_z = -pts.shoulder.cur[3] - .5
			--self:draw_unit()
			gol.rotate_x( .25 )
			gol.translate( 0, 0, offset_z )
				self:draw_skel_only( b_visu, aaa.draw_circle_axe_y )
			gol.translate( 0, 0, -offset_z )
			gol.rotate_x( -.25 )

			local offset_x = -pts.shoulder.cur[1] - .5
			--self:draw_unit()
			gol.rotate_y( .25 )
			gol.translate( offset_x, offset_y, offset_z-1 )
				self:draw_skel_only( b_visu, aaa.draw_circle_axe_x )
			gol.translate( -offset_x, -offset_y, -offset_z )
			gol.rotate_y( -.25 )

		gol.pop_matrix()
	gol.pop_attrib()
end

function SKEL:is_kinect_active()		return self.kinect:is_active()		end

function SKEL:update_touch_dist_sh( blobs, name, i )
	local len_max = .50
	local len_min = .30
	local pts = self.points
	local sh = pts.shoulder
	local hand = pts["hand_"..name]
	local f = 6.
	local x = -(hand.cur[1] - sh.cur[1])
	local y = (hand.cur[2] - sh.cur[2])
	local z = (hand.cur[3] - sh.cur[3])
	local d = math.sqrt( x*x + z*z )
	--x = x + sh.start[1]
	x = x * f
	y = y * f + .5
	local s = clamp_01( (d - len_min) / (len_max - len_min) )
	self.multitouch[i] = { x=x, y=y, id=i, s=s }
	if s > 0 then
		if s >=1. then
			blobs[i] = { x=x, y=y, id=i }
		end
	end
end

function SKEL:update_touch( blobs, name, i )
	if not self.elbow_h then return end
	local len_max = .20
	local len_min = .10
	local pts = self.points
	local sh = pts.shoulder
	local hand	= pts["hand_"..name]
	local elbow	= pts["elbow_"..name]
	local f = 6
	local x =	-	hand.cur[1]
	local y =		hand.cur[2]	- self.elbow_h
	--if i==2 then aaa.mess.show( y ) end
	local z =		(	elbow.cur[3]	- sh.cur[3]		)
	local d = -z
	--self:print( i.."\t"..d )
	--x = x + sh.start[1]
	x = x * f
	y = y * f
	local s = clamp_01( (d - len_min) / (len_max - len_min) )
	self.multitouch[i] = { x=x, y=y, id=i, s=s }
	if s > 0 then
		if s >=1. then
			blobs[i] = { x=x, y=y, id=i }
		end
	end
end

function SKEL:set_clicked( b )	self.b_clicked = b	end
function SKEL:is_clicked()		return self.b_clicked 	end

function SKEL:update_touch_2_hands_click( blobs )
--[[
	if not self.elbow_h then return end
	local len_min = .10
	local len_max = .20
	local pts = self.points
	local shou			= pts.shoulder.cur
	local shou_pos		= pts.shoulder_right.cur
	local shou_click	= pts.shoulder_left.cur
	local hand_pos		= pts.hand_right.cur
	local hand_click	= pts.hand_left.cur
	local elbow_click	= pts.elbow_left.cur
	local elbow			= pts.elbow_right.cur
	local d = V3.dist( hand_pos, shou_pos )
	if d < .2 then
		self:set_clicked( false )
		return
	end
--]]
--[[
	--if i==2 then aaa.mess.show( y ) end
	if self.b_clicked then
--		d = -	v3.(	hand_click[3]	-	self.click_z_ref	) +.10
	else
		if hand_click[2] > (elbow_click[2]) then
			d =	-	(	hand_click[3]	-	shou_click[3]			) -.30
			self:print( d )
		else
			return
		end
	end
--]]

	local x,y,z
	local f

	local loc = m42.location
	local elt = self.scope_3d_anal:get_last()
	if not elt then return end -- protect when no kinect

	x = elt.v[1]
	y = elt.v[2]
	z = elt.v[3]
	f = 10
	x = x * loc.xy_scale * 1.33
	y = y * loc.xy_scale + loc.y_offset

	local s = z - loc.far
	if s <= 0 then
		self:set_clicked( false )
		s = 0
	else
		if self:is_clicked() then
			s = s / (loc.off - loc.far)
			if s<1. then
				self:set_clicked( false )
			else
				blobs[1] = { x=x, y=y, id=1 }
			end
		else
			s = s / (loc.click - loc.far)
			if s>=1. then
				self:set_clicked( true )
				blobs[1] = { x=x, y=y, id=1 }
			end
		end
	end
	self.multitouch[1] = { x=x, y=y, id=1, s=s }
--[[
	if s >=1. then
		--self:print( "BLOB BLOB BLOB BLOB" )
		blobs[1] = { x=x, y=y, id=1 }
		if not self.b_clicked then
			self.click_ref = {}
			V3.cpy( self.click_ref, hand_click )
			self:set_clicked( true )
		end
	else
		self:set_clicked( false )
	end
--]]
end
function SKEL:update_multitouch()
	local new_blobs = {}
	if self.b_multitouch then
		self.multitouch = {}
		--self:update_touch( new_blobs, "left", 1 )
		--self:update_touch( new_blobs, "right", 2 )
		self:update_touch_2_hands_click( new_blobs )
	end
	local ga_blobs = ga:get_blobs()
	if ga_blobs then
		ga_blobs:register_blobs_external( new_blobs )
	end
end

function SKEL:draw_touch_cross( x, y, s )
	if s < 0 then
		gol.color_cyan()
	else
		if s > 1 then
			gol.color_green()
		else
			gol.color_red()
		end
	end

	gol.set_line_width( 4 )
	local SX = 4.
	local SY = 2.25
	local D = .4
	aaa.draw_line(	-SX-D, y,	x-D, y		)
	aaa.draw_line(	x+D, y,		SX+D, y		)
	aaa.draw_line(	x, -SY-D,	x, y-D		)
	aaa.draw_line(	x, y+D,	x,	SY+D		)
end

function SKEL:draw_touch( name, i )
	local t = self.multitouch[i]
	if not t then return end

	local s = t.s

	local x, y = t.x, t.y
	self:draw_touch_cross( x, y, s )

	gol.color_green( s*.5 )
	aaa.draw_disk_axe_z( x, y, 0, clamp_01(1-s*1.2), 48 )
	gol.color_red( .25 + s*.5 )
	if s >= 1. then
	else
		aaa.draw_circle_axe_z( x, y, 0, .05, 12 )
	end
end

function SKEL:set_multitouch( b )
	if self.b_multitouch ~= b then
		self.b_multitouch = b
		self:set_clicked( false )
	end
end
function SKEL:draw_multitouch()
	--if not self.b_multitouch then return end
	gol.set_texture_dim( 0 )
	gol.set_depth( false )
	self:	( "left", 1 )
	self:draw_touch( "right", 2 )
end
function SKEL:draw_on_top()
	self:draw_multitouch()
end
function SKEL:draw_ui_back()
	self:draw( true )

	local function conv( f ) return math.floor(f*100.)*.01 end
	local angle = self:get_angle()
	local speed = self:get_speed()
	if m42 and self.ui.bu_move then
		local str = "Move "..conv( angle ).." / "..conv( speed )
		self.ui.bu_move:set_text( str )
	end
end

function SKEL:add_flag( name, text, b_on, fn )
	local bu = bus_cur:add_button( text )

	if fn then
		bu:set_value( b_on )
		bu:set_function_on_click( fn, bu )
	else
		self[name] = b_on
		bu:set_target_lua( flag_name )
	end
	return bu
end

local function draw_plus( u, v, s )
	s =  s * .5
	aaa.draw_line(	u-s, v,	u+s, v		)
	aaa.draw_line(	u, v-s,	u, v+s		)
end
function SKEL:draw_on_tex_depth()
	--gol.push_attrib()
		gol.set_texture_dim( 0 )
		--gol.set_default()
		--gol.set_depth( false )
		gol.color_yellow()
		gol.set_line_width( 2 )
		--aaa.draw_rect_line_size( .5	)
		--aaa.print( "tto" )
	--gol.pop_attrib()

	if m42 then
		local function draw_visu( u, v, d )
			if not u then return end
			--self:print( d )
			local loc = m42.location
			local df = d - loc.far
			local sc = .2
			if df <= 0 then
				draw_plus( u, v, sc )
			else
				df = df / (loc.click - loc.far)
				sc = sc * 2
				if df<1. then
					aaa.draw_circle_axe_z( u, v, 0, df*sc, 12 )
				else
					aaa.draw_circle_axe_z( u, v, 0, df*sc, 12 )
					draw_plus( u, v, sc )
				end
			end
		end

		gol.color_cyan()
		local u, v, d = self:get_anal_data_raw()
		draw_visu( u, v, d )

		gol.color_red()
		local u, v, d = self:get_anal_data()
		draw_visu( u, v, d )
	end

	local angle = self.kinect:get_img_index() * .03
	gol.set_line_width( 3. )
	gol.color_green()
	aaa.draw_circle_arc_axe_z( .5, .5, 0, .2, angle, .66666, 12 )
end

function SKEL.bu_draw_on_tex_depth( bu )
	BU.draw( bu )
	bu.skel:draw_on_tex_depth()
end

function SKEL:draw_on_tex_rgb()
	--gol.push_attrib()
		gol.set_texture_dim( 0 )
		--gol.set_default()
		--gol.set_depth( false )
		gol.color_yellow()
		gol.set_line_width( 2 )
		--aaa.draw_rect_line_size( .5)
		--aaa.print( "tto" )
	--gol.pop_attrib()

	local x,y = self.kinect:get_head_pos()
	gol.set_line_width( 2. )
	gol.color_green()
	draw_plus( x-.5, y-.5, .5 )
end

function SKEL.bu_draw_on_tex_rgb( bu )
	BU.draw( bu )
	bu.skel:draw_on_tex_rgb()
end
function SKEL:define_ui()

	local	function add_slider( text )
		local bu = bus_cur:add_slider( text )
		return bu
	end
--	local	function add_button( text )
--		local bu = bus_cur:add_button( text )
--		return bu
--	end

	local bus = BUS:begin_window( "M42_Kinect" )

		if m42 then
			self.ui.bu_move =  m42.ui.add_text( bus, "Move",	2, 3.6 )
		end

		local bu

		bus:move_prev()
		self.kinect:define_ui_base( bus )

		bus:move_next(9)
		self.kinect:define_ui_bis( bus )
		bus:add_but_trig_fn(	"Focus Anal",	nil,	aaa.obj.set_focus_ui,	self.anal.ref.bdd )

		bus:add_but_trig_fn( "Reset Position",	nil,	self.reset_pos, self )
		bu = self:add_flag( "b_fix",			"Fix",				false	)

		bu = bus:add_monitor(	"Depth",	{0,1.5, 2,1.5},  30 )
		bu.draw = SKEL.bu_draw_on_tex_depth	--todo cracra to see generalize
		bu.skel = self

		bu = bus:add_monitor( 	"RGB",		{2,1.5, 2,1.5},  44 )
		bu.draw = SKEL.bu_draw_on_tex_rgb	--todo cracra to see generalize
		bu.skel = self

		local kinect = self.kinect.ref.kinect
		local ref_motor_pos	= param.get_ref(	kinect, "motor_position" )
		local ref_rot		= param.get_ref(	kinect, "skeleton_rot_x" )

		if m42 then
			local loc = m42.location
		--	if m42 then
				--local loc = m42.location
				if loc.motor_pos then		param.set( ref_motor_pos, loc.motor_pos )	end
				if loc.kinect_rot then		param.set( ref_rot, loc.kinect_rot )			end
				if loc.min_x then			param.set( kinect, "skeleton_select_min_x", loc.min_x )	end
				if loc.max_x then			param.set( kinect, "skeleton_select_max_x", loc.max_x )	end
				if loc.min_y then			param.set( kinect, "skeleton_select_min_y", loc.min_y )	end
				if loc.min_z then			param.set( kinect, "skeleton_select_min_z", loc.min_z )	end
				if loc.skew then			param.set( self.anal.ref.skew, loc.skew )	end
		--	end
			--	MOTOR
		--	bu = add_button( "Lock Motor" )
		--	bu:set_value( true )

			bus:move_next_col(3)

			bu = add_slider( "Motor" )
			--todom42 104 then move out, deal with skew in anal
			bu:set_target_param( ref_motor_pos )

			--	ROTATION
			bu = add_slider( "Skel Rot X" )
			bu:set_target_param( ref_rot )
			bu:set_min_max( -1, 1 )

			bu = add_slider( "Skew" )
			bu:set_target_param( self.anal.ref.skew )
			bu:set_min_max( -1, 1 )

			bus:move_next()
			bu = add_slider( "Comp y center" )
			bu:set_min_max( -2, 2 )
			bu:set_target_lua(  loc, "comp_y_center" )

			bu = add_slider( "Comp y factor" )
			bu:set_min_max( -1, 1 )
			bu:set_target_lua(  loc, "comp_y_factor" )

			bus:move_next()
			bu = add_slider( "Click" )		bu:set_min_max( -.5, 1.5 )	bu:set_target_lua(  loc, "click" )
			bu = add_slider( "Off" )		bu:set_min_max( -.5, 1.5 )	bu:set_target_lua(  loc, "off" )
			bu = add_slider( "Far" )		bu:set_min_max( -.5, 1.5 )	bu:set_target_lua(  loc, "far" )

			bus:move_next()
			bu = add_slider( "Y	offset" )	bu:set_min_max( -.2, 2. )	bu:set_target_lua(  loc, "y_offset" )
			bu = add_slider( "XY Scale" )	bu:set_min_max( 4, 16 )		bu:set_target_lua(  loc, "xy_scale" )
		end

	bus:end_window()
	bus:set_method( "draw_after",	self,	"draw_ui_back"	)

	return bus
end

