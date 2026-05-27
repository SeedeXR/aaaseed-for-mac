function meu:define_meu_infos( )
	return	{ author = "Mâa",
				help = "Helper MEU used to detect floor or wall transformation when using Depth camera,\n"..
						"works in conjunction with KinMoveAuto.",
				tags = { "2d", "3d", "camera", "core", "coregraphic", "imageProcessing",  "procedural", "texture", "unfinished", "VJ" }
			}
end

local function draw_back_multi( bu )
	local draw_data = bu.draw_data 
	bu:set_back_bind( draw_data.bind_src )
	oo.getsuper(SLIDER_MULTI).draw(bu)
end

local function draw_fore_multi( bu )
--	gol.set_line_width( 1 )
--	gol.color_cyan( .5 )
	for i=1,bu:get_elt_nb() do
		local elt = bu:get_elt( i )
		local x,y = elt:get_xy()
		x,y = x-.5, y-.5
		gol.color_cyan( .5 )
		gol.set_line_width( 1	 )
		aaa.draw_plus_line(	x, y, 1/16 )

		local sx,sy = elt:get_sxy()
		sx,sy = sx*.5, sy*.5
		gol.set_line_width( 2 )
		gol.color_white( 1 )
		aaa.draw_rect_line( x-sx, y-sy, x+sx, y+sy )
	end
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	self.number = 5

	local ix = 1
	local iy = 1
	local dx = 4
	local sx = 12
	local sy = 1

	local what = "Src"
	local what_low = string.lower( what )
		bu = self:add_texture_selector(	{ix,iy,		5,1.5},			what )
		self.ui[ "bu_"..what_low ] = bu
	iy = iy + 1.5

	bu = self:add_slider_multi(		{1,iy+1,	sx, sx*424/512},	"dsimul", self.number )
		bu:set_elt_text_xy_f_ratio( -.5, .0, .4 )
		bu:set_select_on_click_double()
		bu.draw_back = draw_back_multi
		bu.draw_fore = draw_fore_multi
--		bu.draw = self.draw_multi
		bu.draw_data = self.draw_data
		self.bu_multi = bu
	sx = 4
	self:add_trig_method(			{ix,    	iy},				"Flip", 			self.bu_multi,	"flip_elts_active_all"	)
	self:add_trig_method(			{ix+dx,		iy},				"Set",				self.bu_multi,	"set_elts_active_all" 	):set_color_back( "on" )
	self:add_trig_method(			{ix+dx*2,	iy},				"Clear",			self.bu_multi,	"clear_elts_active_all"	):set_color_back( "off" )


	ix, iy = 13, 2
	self:add_button(				{ix,		iy },				"Sol",				self,	"b_sol",			true	)
	self:add_trig_method(			{ix,		iy+sy,		sx,sy},	"Pick Set", 		self,	"set_pick_mode"				)
	bu = self:add_trig_method(		{ix,		iy+sy*2,	sx,sy},	"Plan", 			self,	"do_auto_pick", "plan"		)
	bu = self:add_trig_method(		{ix,		iy+sy*3,	sx,sy},	"Position", 		self,	"pick_center"				)
--	bu = self:add_trig_method(		{ix,		iy+sy*4,	sx,sy},	"UV", 				self,	"do_auto_pick", "uv"		)
	self:add_button(				{ix,		iy+sy*5 },			"Draw",				self,	"b_draw_picked",	true	)

	iy = 10
	self:add_trig_method(			{ix,    	iy,			sx,sy},	"Dump",		 		self,	"dump"	)
	iy = iy + 1
	self:add_trig_method(			{ix,    	iy,		sx,		sy},"Send Plan", 		self,	"transfer_plan"		)
	self:add_trig_method(			{ix,    	iy+sy,	sx*.5,	sy},"Tra",				self,	"transfer_plan_translate"	)
	self:add_trig_method(			{ix+sx*.5,  iy+sy,	sx*.5,	sy},"Rot", 				self,	"transfer_plan_rotate"		)
	iy = iy + 2
end

-- function meu:get_meu_kin_move()
-- 	local m = self.__meu_kin_move
-- 	if m then return m end

-- 	m = self:get_meu_by_name_no_error( "KinMoveAuto_1" )
-- 	if m then
-- 		self.__meu_kin_move = m
-- 	else
-- 		self:print_error( "Can't find ".."KinMoveAuto_1" )
-- 	end
-- 	return m
-- end
function meu:get_meu_kin_move()
	local m = self.__meu_prev
	--self:print( string.lower( "KinMoveAuto" ) )
	if not m:is_meu_type( "KinMoveAuto" ) then
		self:print_error( "Previous meu have to be a KinMoveAuto and is "..m:get_meu_type()  )
		return nil
	end
	return m
end

function meu:set_pick_mode()
	local kmove = self:get_meu_kin_move()
	if kmove then
		kmove:set_for_pick()
	end
end


function meu:transfer_plan()
	self:transfer_plan_translate()
	self:transfer_plan_rotate()
end
function meu:transfer_plan_rotate( )
	self:print( "do_rotate" )
	local kmove = self:get_meu_kin_move()
	if not kmove then
		self:print_error( "no kin_move_auto" )
		return
	end

	local thetass = self.thetass
	if thetass then
		kmove:set_plan_rot( thetass[1], thetass[2], thetass[3] )
--[[
		local transfo = aaa.obj.get_down_by_class_no_error( kmove.ref.__layer_marked, "transfo_trs" )
		if transfo then
			local kin_trs = {}
			local prg = param.get_ref
		-- 	kin_trs.active = prg( transfo, "active" )
		-- 	kin_trs.rotate = prg( transfo, "rotate" )
			kin_trs.rot = { prg( transfo, "rotate_x" ), prg( transfo, "rotate_y" ), prg( transfo, "rotate_z" ) }

			self:print( "do_rotate updated " )
		-- 	table.print( kin_trs, "kin_trs", 2 )


		-- --table.print( self.ref.kin_trs.rot, "kinmove ref rot" )

			local thetass = self.thetass
		-- 	if not kin_trs or not thetass then return end

			local rot = kin_trs.rot
			local prs = param.set
		-- 	prs( kin_trs.active, true )
		-- 	prs( kin_trs.rotate, true )
			prs( rot[1], thetass[1] )
			prs( rot[2], thetass[2] )
			prs( rot[3], thetass[3] )
		end
--]]
	else
		self:print_error( "No thetas computed" )
	end
end

function meu:transfer_plan_translate( )
	local kmove = self:get_meu_kin_move()

	if not kmove then
		self:print_error( "no kin_move_auto" )
		return
	end

	local center = self.center
	if center then
		kmove:set_plan_tra( -center[1], -center[2], -center[3] )
		-- local prg = param.get_ref
		-- local b_transfo2 = prg( kmove.ref.__layer_marked, "trs_2" )
		-- if b_transfo2 then
		-- 	local transfo2 = param.get_obj_attached( b_transfo2 )
		-- 	local kin_trs2 = {}
		-- 	kin_trs2.active = prg( transfo2, "active" )
		-- 	kin_trs2.translate = prg( transfo2, "translate" )
		-- 	kin_trs2.trs = {  prg( transfo2, "translate_x" ), prg( transfo2, "translate_y" ), prg( transfo2, "translate_z" ) }

		-- 	local center = self.center
		-- 	local decal = self.decal
		-- 	if not kin_trs2 or not center then return end
		-- 	local trs = kin_trs2.trs
		-- 	local prs = param.set
		-- 	prs( kin_trs2.active, true )
		-- 	prs( kin_trs2.translate, true )
		-- 	prs( trs[1], -center[1] + decal.x )
		-- 	prs( trs[2], -center[2] + decal.y )
		-- 	prs( trs[3], -center[3] + decal.z )
		-- end
	else
		self:print_error( "No center computed" )
	end
end

function meu:dump()
	self:print( "dump" )
	local thetass = self.thetass
	local center = self.center
	if thetass then	table.print( thetass, "thetass" ) end
	if center then	table.print( center, "center" ) end
end

function meu:init()
	self.draw_data = {}
	self.draw_data.meu = self
	self.draw_data.bind_src = 64
	self.points = {}
	if not self.ref then
		self.ref = {}
	end
	self.ref.kin_trs = {}
	self.decal = { x=0, y=0, z=0 }
end

function meu:read_img( bind, x,y, sx, sy )
	local img_x, img_y = aaa.img.get_size( bind )
	if img_x then
	-- size x and y in texture coordinate ( divided by 2 because we are using it centered over x an y )
		local tsx = math.floor(sx * img_x * .5)
		local tsy = math.floor(sy * img_y * .5)
	-- coord x and y in texture coordinate
		local tx = math.floor(x * img_x)
		local ty = math.floor(y * img_y)

	-- sample pixels in elt rect over image and calc mean in rgb ( storing x, y, z )
		local mr, mg, mb = 0, 0, 0
		local samples = 0 -- total number of samples
		for i=tx-tsx, tx+tsx do
			if i > 0 and i < img_x then
				for j=ty-tsy, ty+tsy do
					if j > 0 and j < img_y then
						local r,g,b,a = aaa.img.get_color_xy(	bind, i, j )
						if a ~= .0 then
							mr, mg, mb = mr + r, mg + g, mb + b
							samples = samples + 1
						end
					end
				end
			end
		end
		if samples > 0 then
			mr, mg, mb = mr / samples, mg / samples, mb / samples
			self:print( "point is : "..mr.." "..mg.." "..mb )
			return mr, mg, mb
		end
		self:print_error( "No sample valid" )
	else
		self:print_error( "No image to read from" )
	end
end
	-- Caculates a 3d point coord from area picked in kin image with active elements of bu_multi
function meu:pick_points_in_img()
	self:print( "pick_points_in_img()" )
	local bu = self.bu_multi
	local nb = bu:get_elt_nb()
	if nb < 3 then
		self:print_error( "we have only"..nb.." eltements active, we need 3 at least" )
		return
	end
	local points = {}
	local bind = self.draw_data.bind_src
	for i=1,3 do
		local elt = bu:get_elt( i )
		if elt:get_value() then
			local x,y        = elt:get_xy()
			local sx,sy      = elt:get_sxy()
			local mr, mg, mb = self:read_img( bind, x,y, sx,sy )
			if mr then
				table.insert( points, { mr, mg, -mb } )
				self:print( "elt "..i.." points "..#points.." : "..mr.." "..mg.." "..mb )
			end
		end
	end
	self.points = points
	nb = #points
	if nb < 3 then
		self:print_error( "we have only "..nb.." valid points, we need 3 at least" )
		return
	else
		self:print( "we have "..nb.." valid points" )
	end

	local vectors = {}
	local center = {}
	local tmp
	vectors[1] =  V3.get_sub( points[2], points[1] )
	vectors[2] =  V3.get_sub( points[3], points[2] )
	vectors[3] =  V3.get_sub( points[1], points[3] )
	center = points[1]
	-- for ip, p in IPAIRS(points) do
	-- 	for jp, p2 in IPAIRS(points) do
	-- 		if ip == 1 and jp > 1 then
	-- 			vectors[jp-1] = V3.get_sub( p2, p )
	-- 			if jp == 2 then
	-- 				tmp = p2
	-- 			else
	-- 				V3.bary3( center, p, p2, tmp )
	-- 			end
	-- 		end
	-- 	end
	-- end


	-- See next link for what follows, considering we want to align camera plane with picked plane
	-- https://stackoverflow.com/questions/9423621/3d-rotations-of-a-plane
	if vectors[1] ~= nil and vectors[2] ~= nil then
		self.center = center
		local plane_nor = V3.get_cross( vectors[1], vectors[2] )
		--table.print( plane_nor, "plane_nor" )
		local dot = V3.dot( plane_nor, { 0, 0, -1 } )
		if dot > 0 then
			plane_nor = V3.get_cross( vectors[2], vectors[1] )
		end
		V3.normalize( plane_nor )
		V3.normalize( vectors[1] )
		--local ax, ay, az = self:get_angles_from_quat( plane_nor )
		--self:print( "angles en tours, x : "..ax..", y : "..ay..", z : "..az )
		self.thetass = self:get_angles_from_plane_axis( plane_nor, vectors[1], center )
	end
end

function meu:pick_center()
	self:print( "pick_center()" )
	local bu = self.bu_multi
	local nb = bu:get_elt_nb()
	if nb < 4 then
		self:print_error( "we have only"..nb.." eltements active, we the number 4 here" )
		return
	end
	--local points = {}
	local elt      = bu:get_elt( 4 )
	local x,y      = elt:get_xy()
	local sx,sy    = elt:get_sxy()
	local px py,pz = self:read_img( self.draw_data.bind_src, x,y, sx,sy )
	if px then
		self:print( "got a point for center xz" )
		local kmove = self:get_meu_kin_move()
		if kmove then
			local x
			kmove:set_bu_value( "CenX", -px )
			--kmove:set_bu_value( "CenZ", pz )
--			self.s_pick_points_in_img = 40
--			self.b_draw_picked_pushed = self.b_draw_picked
--			self.b_draw_picked = false
		end
	else
		self:print_error( "No point for center xz" )
	end
end

function meu:do_auto_pick( what )	-- what will be "plan", "Position", "uv"
	local kmove = self:get_meu_kin_move()
	if kmove then
		kmove:push_for_pick()
		self.s_pick_points_in_img = 40
		self.b_draw_picked_pushed = self.b_draw_picked
		self.b_draw_picked = false
	end
	local mfbo = self:get_meu_fbo_used()
	if mfbo then
		mfbo:push_pixel_channel_type( 4, 3 )	--deal with symbolic in param
		mfbo:get_fbo():set_attachment_tex_on_cpu( 1, true, true )	--todomona was push_tex...
	end
end
function meu:end_pick_point()
	local kmove = self:get_meu_kin_move()
	self:transfer_plan()
	if kmove then
		kmove:pop_for_pick()
	end
	local mfbo = self:get_meu_fbo_used()
	if mfbo then
		mfbo:pop_pixel_channel_type()
		mfbo:get_fbo():set_attachment_tex_on_cpu( 1, false )		--todomona was pop_tex_on_cpu
	end
	self.s_pick_points_in_img = nil
	self.b_draw_picked = self.b_draw_picked_pushed
end
function meu:update_pick_point()
	if self.s_pick_points_in_img then
		self.s_pick_points_in_img = self.s_pick_points_in_img -1
		if self.s_pick_points_in_img < 0 then
			self:pick_points_in_img()
			self:end_pick_point()
		end
	end
end
function meu:update_ui()
	self.bu_multi.draw_fore = draw_fore_multi
	self.bu_multi.draw_back = draw_back_multi
end
function meu:update()
	self.draw_data.bind_src = self.ui["bu_src"]:get_bind_2d()
--self:print( self.ui["bu_src"]:get_bind_2d() )
	local nb = self.bu_multi:get_elt_nb()
	if nb ~= self.number then
		self.bu_multi:set_elt_nb( self.number )
	end
	self:update_pick_point()
end

-- Rotation order : Z, X, Y
function meu:get_angles_from_plane_axis( plane_nor, plane_v1, center )
	local plane_v2 = V3.get_cross( plane_nor, plane_v1 )
	if self.b_sol then
		plane_nor, plane_v1, plane_v2 = plane_v1, plane_v2, plane_nor
	end
	--self:draw_axis( center, plane_v1, plane_v2, plane_nor, 0.5 )

	local r00, r01, r02, r11, r20, r21, r22
	if 1 then -- vecteurs ligne
		r00 = plane_v1[1]
		r01 = plane_v1[2]
		r02 = plane_v1[3]
		r11 = plane_v2[2]
		r20 = plane_nor[1]
		r21 = plane_nor[2]
		r22 = plane_nor[3]
	else -- vecteurs colonne
		r00 = plane_v1[1]
		r01 = plane_v2[1]
		r02 = plane_nor[1]
		r11 = plane_v2[2]
		r20 = plane_v1[3]
		r21 = plane_v2[3]
		r22 = plane_nor[3]
	end
	local thetaX, thetaZ, thetaY

	if r21 < 1 then
		if r21 > -1 then
			thetaX = math.asin( r21 )
			thetaZ = math.atan2(-r01 , r11 )
			thetaY = math.atan2(-r20 , r22 )
		else -- r21 = −1
		-- Not a unique solution : thetaY - thetaZ = atan2( r02 , r00 )
			thetaX = -math.pi / 2
			thetaZ = -math.atan2( r02, r00 )
			thetaY = 0
		end
	else --  r21 = +1
		-- Not a unique solution : thetaY + thetaZ = atan2( r02, r00 )
		thetaX = math.pi / 2
		thetaZ = math.atan2( r02, r00 )
		thetaY = 0
	end

	local f = 1./(2.*math.pi)
	thetaX, thetaY, thetaZ = thetaX*f, thetaY*f, thetaZ*f

	--gol.push_matrix()
	--	--gol.translate( center[1], center[2], center[3] )
	--	gol.rotate_z( thetaZ )
	--	gol.rotate_x( thetaX )
	--	gol.rotate_y( thetaY )
	--	self:draw_axis( nil, plane_v1, plane_v2, plane_nor, 1 )
	--gol.pop_matrix ()
	return { thetaX, thetaY, thetaZ }
end

-- DONE
-- check coordinates scaling in kinect with draw in f1 of sampled points
-- use cross product of 2 sampled vectors from those 3 points to determine plane
-- use plane to deduce camera look at plane ( may be by use of quaternions interpolation )

-- TODO :
-- Edit shader to enable using kinmove manual with auto
-- Clean : (1)replace var bipede by something meaningful, (2)limit number of elts to 3 ( we only pick in a triangle ),
--			(3) simplify PAIRS loop, (4) separate update and draw mixed stuff, (5) select useful debug, make it some interface
-- merge DepthPick interface in KinMoveAuto ( tab )
-- add interface and comments for camera settings that influence sampling
-- try to determine plane from anoter algorith than mean of sampled points : RANSAC, PCA
-- see slider multi init problem : if there is a preset it will load elt without concern of bu multi number elt param in add_slider_multi

function meu:draw_vectors()
	if not self.b_draw_picked then return end

	local points = self.points
	gol.set_line_width( 2 )
	gol.color_green()
 -- don't draw over image when picking points...
	for ip, p in IPAIRS(points) do
		aaa.draw_str_xyz( tostring(ip), p[1], p[2], p[3], .5, .5 )
		aaa.draw_null( p[1], p[2], p[3] )
		for jp, p2 in IPAIRS(points) do
			gol.draw_lines_3d( p[1], p[2], p[3], p2[1], p2[2], p2[3] )
		end
	end

	--if ip == 1 and jp > 1 then
	--	self.vectors[jp-1] = V3.get_sub( p2, p )
	--	if jp == 2 then
	--		tmp = p2
	--end

	--for ip, p in PAIRS(points) do
	--	aaa.draw_str_xyz( tostring(ip), p[1], p[2], p[3], .5, .5 )
	--	aaa.draw_null( p[1], p[2], p[3] )
	--	local j = 1
	--	for _, p2 in PAIRS(points) do
	--		if i == 1 and j > 1 then
	--			vectors[j-1] = V3.get_sub( p2, p )
	--			if j == 2 then
	--				tmp = p2
	--			else
	--				V3.bary3( center, p, p2, tmp )
	--			end
	--		end
	--		gol.draw_lines_3d( p[1], p[2], p[3], p2[1], p2[2], p2[3] )
	--		j = j + 1
	--	end
	--	i = i + 1
	--end
end

function meu:draw()
	self:set_ui_slot_at_start( 4 )

	self:draw_layers_begin()
		self:draw_layer( 1 )
		--self:my_draw()
		self:draw_vectors()
	self:draw_layers_end()
end

--
-- Utilities in case of need
--

-- Used to check if basis from picked triagle correspond to the plane we seek
function meu:draw_axis( pos, vx, vy, vz, gr )
	if pos then
		gol.push_matrix()
		gol.translate( pos[1], pos[2], pos[3] )
	end
			gol.color( gr, 0, 0 )
			gol.draw_lines_3d( 0, 0, 0, vx[1], vx[2], vx[3] )
			gol.color( 0, gr, 0 )
			gol.draw_lines_3d( 0, 0, 0, vy[1], vy[2], vy[3] )
			gol.color( 0, 0, gr )
			gol.draw_lines_3d( 0, 0, 0, vz[1], vz[2], vz[3] )
	if pos then
		gol.pop_matrix()
	end
end

-- Used to check if kin image picked zone is all right by drawing it
function meu:my_draw()
	local bu = self.bu_multi

	-- here we get camera size, lock the focus to horizontal and image size to assure we draw in the good proportion
	local cam = aaa.camera.get_cur()
	local ortho_size_ref = param.get_ref( cam , "ortho_size" )
	local ortho_size = param.get( ortho_size_ref )
	local focal_horiz_ref = param.get_ref( cam, "focal_horizontal")
	param.set( focal_horiz_ref, true )
	local bind = self.draw_data.bind_src
	local img_x, img_y = aaa.img.get_size( bind )

	self.fx,self.fy = ortho_size, ortho_size * ( img_y / img_x )--12/8*8,11/7*7
	local fx,fy = self.fx,self.fy
	local nb = bu:get_elt_nb()
	for i=1,nb do
		local elt = bu:get_elt( i )
		if elt:is_contact() or elt:get_value() then
			local x, y = elt:get_xy()
			local u, v = x, y
			x = (x-.5) *fx
			y = (y-.5) *fy
			local sx, sy = elt:get_sxy()
			local sw, sh = sx * .5, sy * .5
			sx = sx * .5 * fx
			sy = sy * .5 * fy
			--self:print( "sx : "..sx..", sy : "..sy )
			local function draw( x, y )
				gol.bind_texture( bind )
				gol.set_quad_uv(  u-sw, v-sh,  u+sw, v+sh )
				aaa.draw_rect_uv(	x-sx, y-sy, x+sx, y+sy )
				--aaa.draw_bind_rect( 96, x-sx, y-sy, x+sx, y+sy )
			end
			draw( x,y )
		end
	end
end

-- Rotation order : Y, Z, X
function meu:get_angles_from_quat( plane_nor )
	local z_axis = { 0, 0, -1 }
	local c = V3.dot( z_axis, plane_nor ) --/ ( 1 * V3.norm(plane_nor) ) -- c : costheta, and we know that norm of z axis is 1

	local axis = V3.get_cross( z_axis, plane_nor )
	--table.print( axis, "axis")
	V3.normalize( axis )
	table.print( axis, "axis normalized" )

	-- assumes axis is already normalised
	local function quat_from_axis_angle( x, y, z, angle )
		local s = math.sin( angle / 2 )
		local w = 0
		x = x * s;
		y = y * s;
		z = z * s;
		w = math.cos( angle / 2 );
		return QUATERNION:create( w, x, y, z )
	end
	local quat = quat_from_axis_angle( axis[1], axis[2], axis[3], math.acos( c ) )
	local angles = quat:get_euler()
	table.print( angles, "euler angles" )
	local f = 1./(2.*math.pi)
	return angles[1]*f, angles[3]*f, angles[2]*f
end

local function rotX( a )
	local c = math.cos( a )
	local s = math.sin( a )
	return { {1, 0, 0}, {0, c, -s}, {0, s, c} }
end
local function rotY( a )
	local c = math.cos( a )
	local s = math.sin( a )
	return { {c, 0, s}, {0, 1, 0}, {-s, 0, c} }
end
local function rotZ( a )
	local c = math.cos( a )
	local s = math.sin( a )
	return { { c, s, 0 }, {-s, c, 0}, {0, 0, 1.0} }
end
local function point_dot( pt, vec )
	return pt * vec[1] + pt * vec[2] + pt * vec[3]
end
local function vec_rot( vec, thetas )
	local mat_rot = rotX( thetaX )
	center[1] = point_dot( center[1], mat_rot[1] )
	center[2] = point_dot( center[2], mat_rot[2] )
	center[3] = point_dot( center[3], mat_rot[3] )
	mat_rot = rotY( thetaY )
	center[1] = point_dot( center[1], mat_rot[1] )
	center[2] = point_dot( center[2], mat_rot[2] )
	center[3] = point_dot( center[3], mat_rot[3] )
	mat_rot = rotZ( thetaZ )
	center[1] = point_dot( center[1], mat_rot[1] )
	center[2] = point_dot( center[2], mat_rot[2] )
	center[3] = point_dot( center[3], mat_rot[3] )
end