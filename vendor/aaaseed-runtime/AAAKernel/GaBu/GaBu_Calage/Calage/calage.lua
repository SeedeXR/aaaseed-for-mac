CLASS.DECLARE( "CALAGE" )

function CALAGE:init_ref()
	if self.ref then return end

	self.ref = {}
	local ref = self.ref
	--

	ref.process_grid_out = aaa.obj.get( "GaBu/GaBu_Input/Deform/fx.layers_param" )
	--ref.process_grid_out		= aaa.obj.get_by_name_symbo( "gabu_calib_grid_map_uv" )
	ref.grid_adj				= aaa.obj.get_down( ref.process_grid_out, "gabu_calib_grid_uv_edit" )
	ref.grid_adj_reset				= param.get_ref( ref.grid_adj, "grid_reset_trig" )

	ref.grid_layers = aaa.obj.get( "GaBu/GaBu_Calage/Grid/fx.layers_param" )
	ref.grid_show				= aaa.obj.get_down( ref.grid_layers, "gabu_calib_grid_show" )
	ref.cursor_layers = aaa.obj.get( "GaBu/GaBu_Calage/Cursor/fx.layers_param" )
	ref.transfo					= aaa.obj.get_down( ref.cursor_layers, "gabu_calib_transfo" )

	ref.white_out_layers = aaa.obj.get( "GaBu/GaBu_Calage/WhiteOut/fx.layers_param" )
	--ref.white_out_layers		= aaa.obj.get( "gabu_calib_white_out_layers"	)

	ref.grid_layers_active 			= param.get_ref( ref.grid_layers,		"active"	)
	ref.cursor_layers_active		= param.get_ref( ref.cursor_layers,		"active"	)
	ref.white_out_layers_active		= param.get_ref( ref.white_out_layers,	"active"	)

	--	grid division
	ref.grid_show_u					= param.get_ref( ref.grid_show,	"nb_u" )
	ref.grid_show_v					= param.get_ref( ref.grid_show, "nb_v" )
	ref.grid_adj_u					= param.get_ref( ref.grid_adj,	"curve_nb_u" )
	ref.grid_adj_v					= param.get_ref( ref.grid_adj,	"curve_nb_v" )

	--	grid index
	ref.index_u						= param.get_ref( ref.grid_adj, "ui_index_u" )
	ref.index_v						= param.get_ref( ref.grid_adj, "ui_index_v" )
	--calage.ref.u = param.get_ref( ref.grid_adj, "ui_u" )
	--calage.ref.v = param.get_ref( ref.grid_adj, "ui_v" )

	ref.tra_x						= param.get_ref( ref.transfo, "translate_x" )
	ref.tra_y						= param.get_ref( ref.transfo, "translate_y" )

	ref.process_grid_out_active		= param.get_ref( ref.process_grid_out, "active" )
	ref.process_grid_out_model	= aaa.obj.get_branch_by_class_no_error( ref.process_grid_out, "model" )
	if ref.process_grid_out_model then
		ref.process_grid_out_model_u	= param.get_ref( ref.process_grid_out_model, "size_u" )
	else
		aaa.print( "GabuIn layers deform is absent: not loaded because OFF or no input camera defined" )
	end

	ref.fbo						= aaa.obj.get_branch_by_name_symbo( ref.process_grid_out, "GaBuIn_deform_fbo" )
	ref.fbo_size_x					= param.get_ref ( ref.fbo, "size_x" )
	ref.fbo_size_y					= param.get_ref ( ref.fbo, "size_y" )
end

function CALAGE:create( name )
	local self = CALAGE:create_instance(name)
	self.remanence = {}
	self:set_active( false )
	self:init_ref()
	return self
end

function CALAGE:set_grid_adj()
	local obj = self.ref.grid_adj
	for i = 1, self.nb_u do
		for j = 1, self.nb_v do
			local xy = self.xy[ i ][ j ]
			local x = xy[1] - 0.5
			local y = xy[2] - 0.5
		--	aaa.print( "set "..self.ref.grid.." "..i.." "..j.." "..x.." "..y )
			local ret = aaa.bdd.set_control_point( obj, i, j, x, y, 0 )
		end
	end
--	aaa.bdd_update( self.ref.grid )
end

function CALAGE:set_nb_uv( nb_u, nb_v )
	self:init_ref()

	self.nb_u = nb_u
	self.nb_v = nb_v

	local ref = self.ref
	param.set( ref.grid_show_u,		nb_u )
	param.set( ref.grid_show_v,		nb_v )

	param.set( ref.grid_adj_u, 		nb_u + 2 )
	param.set( ref.grid_adj_v, 		nb_v + 2 )
end

function CALAGE:init_pos()
	self.xy = {}
	for i = 1, self.nb_u do
		self.xy[ i ] = {}
		for j = 1, self.nb_v do
			self.xy[ i ][ j ] = { 0., 0. }
		end
	end
end

function CALAGE:init_for_start()
	self:init_ref()

	-- set object factor
	param.set( "GaBu/GaBu_Calage/Grid/fx_a.model", "size_u", ga.cam:get_ratio_x() )

	local ref = self.ref

	self.u = 1
	self.v = 1
	self.clock = os.clock()

	self:init_pos()

	param.set( ref.grid_layers_active, 1 )
	param.set( ref.cursor_layers_active, 1 )
	param.set( ref.process_grid_out_active, 1	 )
	param.set( ref.grid_adj_reset, 1 )
end

function CALAGE:start()
	self:init_for_start()
	self:set_active( true )
	if GABUIN and GABUIN.cv_reset_background then GABUIN.cv_reset_background() end
	self.x_last = -100000
	self.y_last = -100000
	aaa.mess.show( "Calage Multitouch Started" )
end

function CALAGE:stop()
	self:set_active( false )
	local ref = self.ref
	param.set( ref.grid_layers_active, 0 )
	param.set( ref.cursor_layers_active, 0 )
	param.set( ref.process_grid_out_active, 1 )
	aaa.mess.show( "Calage Multitouch Aborted" )
end



function CALAGE:flip()
	if self:is_active() then
		self:stop()
	else
		self:start()
	end
end

function CALAGE:dec_index()
	if self.u == 1 then
		if self.v == 1 then
			self:print( "can't go back, already at first calage point" )
		else
			self.u = self.nb_u
			self.v = self.v - 1
		end
	else
		self.u = self.u - 1
	end
end

function CALAGE:inc_index()
	if self.u >= self.nb_u then
		if self.v >= self.nb_v then
			return false
		end
		self.u = 1
		self.v = self.v + 1
	else
		self.u = self.u + 1
	end
	return true
end

function CALAGE:draw()
	local ref = self.ref
	if self:is_active() then
		aaa.obj.update_then_draw( ref.grid_layers		)
		aaa.obj.update_then_draw( ref.cursor_layers		)
	end
	if self.b_white_out then
		aaa.obj.update_then_draw( ref.white_out_layers	)
	end
end

function CALAGE:update()
--	self:print( self.u.." "..self.v )
	if not self:is_active() then
		return
	end

	local ref = self.ref
	--keep grid in the fbo ratio

	if not ref.fbo_size_x then
		self:print_error( "Probably ref no init" )
		return
	end

	local ratio = param.get( ref.fbo_size_x ) / param.get( ref.fbo_size_y )
	if not ref.process_grid_out_model_u then
		self:print( "no ref for process_grid_out_model_u, skipping CALAGE:update()" )
		return
	end
	param.set( ref.process_grid_out_model_u, ratio )

	if not self:is_active() then
		return
	end

	local ortho_y = 2
	local ortho_x = ortho_y * ga.cam:get_ratio_x()
	local x, y
	x = 2 * ortho_x * (self.u -1) / ( self.nb_u - 1 ) - ortho_x
	y = 2 * ortho_y * (self.v -1) / ( self.nb_v - 1 ) - ortho_y

	param.set( ref.tra_x, x )
	param.set( ref.tra_y, y )

	if os.clock() - self.clock > .7 then
		--todo	perhaps not the right blobs to use here
		local blobs		= ga:get_blobs()
		local blob_nb	= blobs:get_blob_nb()
		--aaa.print( "calage testing " )
		if blob_nb == 1 then
			--aaa.print( "calage one_touch " )
			if self.waiting then
				--aaa.print( "waiting "..os.clock()-self.waiting  )
				local x = blobs.blob[ 1 ].x_raw
				local y = blobs.blob[ 1 ].y_raw
				local xd = x - self.x_last
				local yd = y - self.y_last
				self.x_last = x
				self.y_last = y

				local d2 = xd*xd + yd*yd
				self:print( "d2 "..d2 )
				if d2 < 1. * aaa.time.dt * aaa.time.dt then
					self:print( "fixe " )
					if os.clock() - self.waiting > .7 then
						self.clock = os.clock()
						self.waiting = nil
						self.x_last = -100000
						self.y_last = -100000
						local xy = self.xy[ self.u ][ self.v ]
						xy[1] = ((x-.5)*ratio)+.5
						xy[2] = y
						if not self:inc_index() then
							-- calibration finished
							self:stop()
							self:set_grid_adj()
							--reset_background()
							aaa.mess.show( "Calage Multitouch Done" )
							GABUIN.cv_reset.nb_todo = 50
							--if GABUIN and GABUIN.cv_reset_background then GABUIN.cv_reset_background() end
						end
					end
					return
				end
			end
		end
		self.waiting = os.clock()
	end
end

function CALAGE:set_grid( b_on, b_full )
	self:print( "CALAGE:set_grid( "..b_on.." )" )
	--todo what eo do here
	if eo.pc.b_img and self.b_white_out ~= b_on then
		self.b_white_out = b_on
		if b_on then
			-- memorize flatland draw flags
			if not self.remanence.last then self.remanence.last = {} end
			self.remanence.last.draw_focus			= param.get( aaa.ref.draw_focus )
			self.remanence.last.draw_info			= param.get( aaa.ref.draw_info )
			self.remanence.last.cursor_hide_in_render	= param.get( aaa.ref.pref, "cursor_hide_in_render" )
			self.remanence.last.b_simul				= GABU.is_simulation()
			aaa.print( self.remanence.last.b_simul	 )
			param.set( aaa.ref.draw_focus, 0 )
			param.set( aaa.ref.draw_info, 0 )
			param.set( aaa.ref.pref, "cursor_hide_in_render", 1 )
			GABU.set_simulation( false )
			param.set( "gabu_whiteout_render", "front_mode", b_full and "FILL" or "LINE" )
		elseif self.remanence.last then
			-- restore flatland
			param.set( aaa.ref.draw_focus, self.remanence.last.draw_focus )
			param.set( aaa.ref.draw_info, self.remanence.last.draw_info )
			param.set( aaa.ref.pref, "cursor_hide_in_render", self.remanence.last.cursor_hide_in_render )
			GABU.set_simulation( self.remanence.last.b_simul )
		end
	end
end

function CALAGE:remanence_set_active( b_on )
	if self.b_on ~= b_on then	--hack avoid double call when using local host
		self:print( "CALAGE:remanence_set_active( "..b_on.." )" )
		self.b_on = b_on
		--todo what eo do here
		if eo.pc.b_cam then
			local cv = GABUIN.get_cv_multitouch()
			if b_on then
				param.set( cv, "remanence_restart_trig",	1 )
				param.set( cv, "remanence_image_ref",   	0 )
			else
				self:remanence_save()
			end
			param.set( cv, "remanence_accumulate_max", b_on )
		end
		self:set_grid( b_on, true )
	else
		self:print( "repeat so skip CALAGE:remanence_set_active( "..b_on.." )" )
	end
end

function CALAGE:show_grid()	self:set_grid( true, false )	end
function CALAGE:hide_grid()	self:set_grid( false )			end

function CALAGE:remanence_save()		self:print( "remanence_save() have to be implemented" )			end
--hack
if CALAGE.xxtoto then
	CALAGE.xxtoto = xxtoto -1
	if CALAGE.xxtoto < 0 then
		CALAGE.xxtoto = nil
		param.set( GABUIN.get_cv_multitouch(), "remanence_image_ref",		1 )
	end
end

function CALAGE:remanence_start()	self:remanence_set_active( true ) end
function CALAGE:remanence_stop()	self:remanence_set_active( false ) end

function CALAGE:remanence_do_command( str, arg1 )
	local mess =  "CALAGE:remanence_do_command( \""..str.."\""
	if arg1 then
		mess = mess..", "..arg1
	end
	mess = mess.." )"
	aaa.print( "received : "..mess )
	if arg1 then mess = mess..", "..arg1 end

	if		str == "begin"		then self:remanence_start()
	elseif	str == "end"		then self:remanence_stop()

	elseif	str == "show_grid"	then self:show_grid()
	elseif	str == "hide_grid"	then self:hide_grid()
	elseif	str == "save"		then self:remanence_save()
	elseif	str == "set_min"	then
		local obj = GABUIN.get_cv_multitouch()
		param.set( obj, "remanence_diff_min", arg1 )
	else
		aaa.print_error( "dont know this command : "..mess )
	end
end

--todo
--if ga then
--	ga.calage:update()
--end

--ga.calage:remanence_do_command( "begin" )
