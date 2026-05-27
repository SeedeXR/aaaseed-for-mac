

function AQUA:draw_camera( name )
	if name == "back" then
		--test added why ?
		if self.ref.camera then
			aaa.obj.update_then_draw( self.ref.camera.back.layers )
		end
	end
end

function AQUA:draw()
	if not self:is_active() then return end

	local tank = self.tank
	if not self.camera_hooked then
		self.camera_hooked = aaa.obj.get( "aqua_camera_hook" )
	end
	local cam = self.camera_hooked
	if cam then
		tank:place_camera_hooked( cam )
	end

	local ref = self.ref
-- FBO
	if self.b_fbo then
		self.fbo_and_out:draw_fbo()
	end
	--	gol.reset()
-- CAMERA
	self:draw_camera( "back" )
	--	gol.reset()
-- GRILLE
	if self.b_grid_draw then
		aaa.obj.update_then_draw( ref.grille_layers )
	end
	--	gol.reset()
-- RE CAM
	--test added why ?
	local ref_cam = ref.camera
	if ref_cam then
		if self.b_cam_display then
			aaa.obj.update_then_draw( ref_cam.display.layers )
		else
			aaa.obj.update_then_draw( ref_cam.edit.layers )
		end
	end
	--	gol.reset()
-- BACK		FXS
	if self.b_draw_back then
		self.fxs_back:updraw()
	end

	gol.reset()

-- OTHER	FXS
	if self.b_draw_other then
		self.fxs_other:updraw()
	end
	--	gol.reset()

	tank:draw()

	--	gol.reset()
-- FORE
	if self.b_draw_fore then
		self.fxs_fore:updraw()
	end
	--	gol.reset()
-- SCREEN
	--self:print( "test-> "..self.b_draw_screens )
	if self.b_draw_screens then
		gol.push_matrix()
			gol.translate( 0, 0, 5 )
			self:draw_screens( self.b_draw_screens_all )
		gol.pop_matrix()
	end
-- CLEANING
	gol.reset()
	--aaa.obj.update_then_draw(	ref.cleaning_layers )
-- FINAL OUTPUT
	if self.b_fbo and self.b_fbo_draw then
		--self:print( self.b_fbo_out )
		if self.b_fbo_out then
			self.fbo_and_out:draw_out()
			self:draw_4_screen( self.cam_wall_id )
		else
			self.fbo_and_out:draw_visu()
		end
	end
end



function AQUA:set_camera( id, dist_cam_glass, syh )
	if not id then return end
	--	dist is the dist to the glass
	local dist_cam_origin = dist_cam_glass + 5.
	--aaa.print( syh )
	local v = syh / dist_cam_glass
	v = v * v + 1
	v = 1. / math.sqrt( v )
	local angle_wall = math.acos( v ) * 2.
	param.set( self.ref.camera.wall.focal, math.deg( angle_wall ) )
	local cam
	if id then
		local rect
		if id == 0 then
			rect = self.wall_rect
		else
			--aaa.print( id )
			local pc = self.pcs:get_pc(id)
			pc:set_cam_tra_z( -dist_cam_origin )
			pc:set_cam_focal_from_rect( dist_cam_glass )
			pc:set_cam_frustum_from_rect()
			--	param.set( ref.frustum_x, syh / (rect.right * 4) )
			--[[
			local alp_right	= math.atan( rect.right / dist_cam_glass )
			local alp_left	= math.atan( rect.left / dist_cam_glass )
			local angle = alp_right - alp_left
			param.set( ref.focal, math.deg( angle ) )
			--]]
			--	pc_ref.frustum_x	= param.get_ref( cam, "frustum_offset_x" )
			--	pc_ref.frustum_y	= param.get_ref( cam, "frustum_offset_y" )
			local nb_pix_x = 	self:is_onsite() and 2048 or 1024 --and 2610 or 1024
			self.fbo_and_out:set_pixel_size( nb_pix_x, nb_pix_x * pc:get_rect_ratio_y() )
			rect = pc:get_rect()
		end
		local tank = self.tank
		tank.clip_left	=	tank:compute_clip( dist_cam_origin, rect.left,	1.	)
		tank.clip_right	=	tank:compute_clip( dist_cam_origin, rect.right,	-1.	)
	end
end

function AQUA:update()
	APP.update( self )

	if not self.b_kinect and not self:is_active() then return end

	if self:is_onsite() and self.b_slave_onsite and not aaa.is_edit() then
		if not self.fullscreen_minute or self.fullscreen_minute ~= aaa.time.minute then
			ga:force_fullscreen()
			self.fullscreen_minute = aaa.time.minute
		end
	end

	if not self.b_kinect then
		self:receive_osc()
	end

--[[
	local R = 16
	local W = 1 / 16.
	self.fbo_and_out:set_u_one(	1, -1 * W, 	7  * W	)
	self.fbo_and_out:set_u_one( 2,  4 * W,	12 * W	)
	self.fbo_and_out:set_u_one( 3,  9 * W,	17 * W	)
	local gamma = self.b_gamma and self.ui.bu_gamma:get_value() or 0.
	self.fbo_and_out:set_gamma_fuzzy( gamma )
--]]

	self.time = aaa.time.t

	local tank = self.tank
	tank:update()

--[[
	if not self.net_started and aaa.time.t > (self.time_aaaseed_start + 16.) then
		aaa.net.set_active( true )
		self.net_started = true
	end
--]]

-- CAMERA

	if self.b_kinect then
		for i=1,self.kinect_nb do
			local kinect = self.kinect[i]
			--self:print( "kinect updraw()" )
			kinect:updraw()
			local ref = kinect.ref
			local average = param.get( ref.average )
			local agitation = param.get( ref.agitation )
			self:send_osc( "aqua/kinect_agitation", i, average, agitation )
			--self:print( "kinect_agitation "..i.."  "..average.." "..agitation )
		end
	else
		local d = - param.get( self.ref.camera.wall.tra_z )
		local ref = self.ref.proj_cone
		param.set( ref.src_z, d )
		param.set( ref.dst_z, -5. )
		param.set( ref.sx, (-5-d) * 2. * self.wall_rect.right	/ (5-d) )
		param.set( ref.sy, (-5-d) * 2. * self.wall_rect.top		/ (5-d) )

		d = d - 5.	--	we placed the wall 5m from the center of the virtual center
		--aaa.print( self.cam_wall_id )
		self:set_camera( self.cam_wall_id, d, self.wall_rect.right )

		for i=1,self.kinect_nb do
			if i==2 then
				local b = self["b_k"..i]
				if b ~= self.kinect_last[i] then
					if b then
						self:print( "try send shark on "..i )
						local grea = tank:get_grea_by_name( "req" )
						if grea and grea:can_launch_req() then
							self:print( "send shark on "..i )
							grea:do_launcher( true )
						end
					end
					self.kinect_last[i] = b
				end
			end

			if i==1 and self.grea_kid then
				--aaa.print( "toto "..i )
				local dt = aaa.time.dt
				if self:is_master() then
					local grea = self.grea_kid[i]
					local blow = grea.bu_blow:get_value()
					local b_kinect = grea.bu_kinect:get_value_as_bool()
					if b_kinect then
						local more = grea.agitation_ui * .05
						if grea.agitation then
							more = more + grea.agitation * .04
						end
						if more > 0. then
							blow = blow + dt * more
						else
							blow = blow - dt * .4
						end
					else
						blow = blow - dt * 1.5
					end

					if blow > 3 then
						blow = 0
					end
					if b_kinect then
						blow = math.max( .6, blow )
					else
						blow = math.max( .1, blow )
					end
					grea.bu_blow:set_value( blow )
					grea:set_blow( blow )
					--self:print( blow )
				end
			--[[
			else
				local boid = GREA.boids[i]
				boid:set_target_active( b )
				boid:set_box_y( b and 0. or -3. )
			--]]
			end

		end
	end

end
function AQUA:receive_kinect( id, state, x, y, z )
	--aaa.print_method()
	--	Nov 2012 we go from 3 kinect to 2, deactivating kinect 2 : kinect 3 become 2
	if self.kinect_nb == 2 then
		if id == 2 then
			return
		elseif id == 3 then
			id = 2
		end
	end
	if inside( id, 1, self.kinect_nb ) then
		local bu = self.ui.kinect[id]
		bu:set_value( state )
	end
end
function AQUA:receive_kinect_agitation( id, average, agitation )
	--aaa.print_method()
	if inside( id, 1, self.kinect_nb ) then
		local bu = self.ui.kinect[id]
		local b_on = average > 12.
		if b_on then
		--	self:print( "receive kinect_agitation "..id.." "..average.." "..agitation )
		end
		bu:set_value( b_on )
		if self.grea_kid then
			self.grea_kid[id].agitation = (agitation<2.5) and 0 or (agitation)
		end
	end
end

function AQUA:receive_osc()

	if not self.osc.b_received then return end

	--if GA.b_spy then aaa.spy.push_range( "RECEIVE AQUA", 1 ) end

	repeat
		local tab = aaa.net.osc_take_by_start( "/aqua" )
		--aaa.print( "toto"..tab )
		if not tab then break end
		--aaa.print( "received Osc " )
		local	b_master = self:is_master()
		local	i = 1
		while true do
			--aaa.print( i )
			local sub_tab = tab[i]
			if not sub_tab then break end

			--self:print_osc_message( sub_tab )
			--aaa.print( "received Osc "..i )

			local tags = sub_tab.tags
			local args = sub_tab.args

			local switch = tags[2]
			if switch == "kinect" then
				if b_master then
					self:receive_kinect( args[1], args[2], args[3], args[4], args[5] )
				end
			elseif switch == "kinect_agitation" then
				if b_master then
					self:receive_kinect_agitation( args[1], args[2], args[3] )
				end
			else
				self:print_error( "this osc commande in unknown in aqua : "..switch )
			end
			i = i + 1
		end
	until false

	--if GA.b_spy then aaa.spy.pop_range() end
end
function aaa.send_slave( ... )
	--todo what eo do here
	aaa.net.osc_send( 1, "/eo/"..what, ... )
end

if IS_BUSS_OPEN() then
	--tododesk app name loke aqua should be able to destroy themself when freed
	APP.CREATE_INST( AQUA )
end
