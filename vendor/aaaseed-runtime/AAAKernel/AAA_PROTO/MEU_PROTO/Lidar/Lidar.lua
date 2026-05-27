function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "2d", "3D", "Core", "CoreGraphic", "Camera", "device", "texture", "input", "unfinished" },
			help = "read the image fluxs from a Lidar Intelsense l515 camera\n"..
					"pretty similar to Kinect"
		}
end

function meu:define_ui_proto( b_ui )
	if b_ui then
		--aaa.box_good( self..":define_ui_proto()" )
		self:add_button( {1, 1 }, "allow",	self, "b_allow", true )
		self:add_button( {1, 2 }, "On CPU",	self, "b_on_cpu", true )
	else
		self.b_allow	= true
		self.b_on_cpu	= true
	end
end

function meu:init()
	local ref = self.ref
	local function pgr_tab( t,	name, obj, pname )	t[name]	= param.get_ref( obj, pname and pname or name )	end
	local function pgr(			name, obj, pname )	pgr_tab( ref, name, obj, pname ) end

	self.__frame_count = 0

	ref.video	= self:get_obj_down( "tex_video" )
	local o = ref.video
		pgr( "depth_dst",				o,	"bind_dst"		)
		pgr( "depth_cpu",				o,	"image_on_cpu_wanted"	)
		pgr( "depth_gpu",				o,	"move_to_texture"		)
		pgr( "open",					o,	"capture_open"			)
		pgr( "run",						o,	"capture_run"			)
		pgr( "image_index",				o	)
--		pgr( "on_cpu",					o,	"image_on_cpu_wanted"	)
--		pgr( "move_to_texture",			o							)
--		pgr( "sx_depth",				o, 	"size_x"				)
--		pgr( "sy_depth",				o, 	"size_y"				)
		pgr( "capture_open",			o	)
		pgr( "capture_opened",			o	)
		pgr( "capture_running",			o	)
		pgr( "capture_device_name",		o	)
		pgr( "capture_crossbar_name",	o	)

	ref.lidar  = aaa.obj.get_down_by_class( ref.video, "realsense_ui" )
	local l = ref.lidar
		pgr( "rgb_asked", 				l)
		pgr( "rgb_dst", 			 	l,	"rgb_bind"  			)
		pgr( "rgb_cpu", 			 	l	)
		pgr( "rgb_gpu", 			 	l	)

		pgr( "ir_asked", 				l,	"ir_left_asked")
		pgr( "ir_dst", 					l,	"ir_left_bind" )
		pgr( "ir_cpu", 					l	)
		pgr( "ir_gpu", 					l	)


	ref.layer_lidar = self:get_layer( 1 )
		--unused now
	--ref.layer_img_process = self:get_layer( 2 )
	--aaa.box_good( "2 "..ref.layer_img_process )
	ref.layer_multitouch = self:get_layer( 3 )
		--aaa.box_good( "3 "..ref.layer_multitouch )
end

function meu:set_accum( b )
	local ref = self.ref
	local bdd = ref.bdd_multitouch
	if bdd then
		if b and self.b_accum ~= b then
			param.set( ref.accum_trig, b )
		end
		param.set( ref.accum_max, b )
		self.b_accum = b
	end
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local ix = 1
	local iy = 1
	local SY = .8

	bu = self:add_text_info(	{ix,iy,			8,SY},		"name" )
		--bu:set_value_load_save( false )
		self.ui.bu_name = bu
	bu = self:add_text_info(	{ix,iy+1,		8,SY},		"State" )
		self.ui.bu_info = bu

	ix = 9
	bu = self:add_trig_fn(		{ix, iy,		2.5, 1},	"Tex_Video",	aaa.obj.set_focus_ui, ref.video	):set_color_back("focus")
	bu = self:add_trig_fn(		{ix+8/3, iy,	2.5, 1},	"Lidar",		aaa.obj.set_focus_ui, ref.lidar ):set_color_back("focus")
	ix = 1

	self:set_tab_key_def()
	iy = iy + 2
	self:add_button( 			{ix,   iy },				"Open", 		ref.open, nil,	false	)
	self:add_button( 			{ix+4, iy },				"Run",			ref.run, nil, 	false	)


	iy = iy + 3
	local SX = 2.5

	bu = self:add_slider(		{ix+SX,iy,		8-SX,SY},	"Dist", 		ref.lidar, "l500_min_distance_mm",		245, 0,8000 )

	iy = iy + SY
	iy = iy + SY

	local sy = 4

	--self:add_bu_texture_target_unit(	{ix,iy,		8,dy},	"Depth",	1  )
	--self:add_bu_texture_target_unit(	{ix,iy+dy,	8,dy},	"RGB",		2  )

	local function add_monitor_one( rect, name, b_flip_v )
		local bu = self:add_monitor( rect, name )
			bu:set_render_mode_alpha( 1 )
			bu:__delegate_get_set_bind_2d( nil ) --hack
			---if b_flip_v then bu:set_texture_flip_v( true ) end
			bu:set_texture_flip_v( false )
			return bu
	end
	local function add_mon( x,y, sx,sy, what, b_flip_v, ref_asked, ref_cpu, ref_gpu  )
		local what_low = string.lower( what )
		local bu
		local syt = 1.5
		if ref_asked then
			bu = self:add_button(	{x, y-syt,		1,1},	what.."asked",		ref_asked,nil,	true ):set_text_draw( false )
		end
		bu = add_monitor_one( {x,y, 8,sy},  what.." Dst", b_flip_v )
			ui[ "bu_"..what_low.."_mon" ] = bu
		bu = self:add_texture_selector( {x+1,y-syt, 5,syt}, what )
			ui[ "bu_"..what_low.."_dst" ] = bu
		local s = syt * .5
		if ref_cpu then
			bu = self:add_button(	{x+sx, y-s*2,	s,s},	what_low.." Cpu",	ref_cpu,nil,	false ):set_text( "Cpu")
				bu:set_text_rect_ratio(2)
		end
		if ref_gpu then
			bu = self:add_button(	{x+sx, y-s,		s,s},	what_low.." Gpu",	ref_gpu,nil,	false ):set_text( "Gpu" )
		end
		return bu
	end

	iy = 9.5
	self:add_button(				{11, iy},				"Control Next", 	self,"b_control_next",		false	)
	bu = self:add_selector(			{ix+3,iy,		5,SY},	"Flip"				)
		bu:set_nb_min_0( 5, 1 )
		--bu:set_target_lua( self, "__s_flip_dst" )
		local tab = { "No",	"B", "C", "F1",	"F2" }
		bu:set_item_text(	1,	unpack(tab) )
		bu:set_item_data( 1, 0 )
		for i=2,5 do
			bu:set_item_data( i, self:get_bind_by_name( tab[i] ) )
		end
		self.ui.depth_flip = bu

	local mon_sy = 8*9/16
	bu = add_monitor_one(			{1,10.5,		8,5.5},		"_Depth",	false )
		ui.bu_visu_depth = bu
	bu = add_monitor_one(			{9,10.5,		8,mon_sy},	"_RGB",		false )
		ui.bu_visu_rgb = bu

	self:set_tab_key( "DST" )
	ix,iy =  1, 4.5

	add_mon( 1, iy,				6, mon_sy, 	"Depth",		false	)
	add_mon( 9, iy,				6, mon_sy,	"RGB",			false,	ref.rgb_asked, ref.rgb_cpu, ref.rgb_gpu 	)
	add_mon( 1, iy+mon_sy+1.5,	6, mon_sy,	"IR",			false,	ref.ir_asked, ref.ir_cpu, ref.ir_gpu	)
	-- add_mon( 9, iy+mon_sy+1.5,	6, mon_sy,	"IR2",			false,	ref.ir_asked, ref.ir_cpu, ref.ir_gpu	)

	iy = 3.25
	iy = iy + 4
--	self:add_button( {ix, iy }, "On Cpu", 	self, "b_on_cpu", false	)

	--iy = iy + 4.5
	--self:add_param_obj_name(	{ix,iy},	"sound",	r, "sound_active",	0,1 )
	--self:add_param_obj_name(	{ix,iy+1},	"volume",	r, "volume",		0,1 )

	--local bind = param.get( r, "bind_dst" 
end

function meu:get_lidar()
	return self.ref.lidar
end

function meu:is_opened()		return param.get_bool( self.ref.capture_opened )	end
function meu:is_running()		return param.get_bool( self.ref.capture_running )	end

function meu:get_depth_bind()	return self.ui.bu_depth_dst:get_bind_2d()			end
function meu:get_rgb_bind()		return self.ui.bu_rgb_dst  :get_bind_2d()			end
function meu:get_ir_bind()		return self.ui.bu_ir_dst   :get_bind_2d()			end
-- function meu:get_deform_bind()				return self.ui.bu_deform_dst:get_bind_2d()		end
-- function meu:get_rgb_aligned_bind()			return self.ui.bu_rgb_aligned_dst:get_bind_2d()	end

function meu:draw_icon()
	if not self:is_proto() then
		self:draw_icon_texture( self:get_depth_bind() )
		self:draw_icon_frame( self.__frame_index_last )
	end
end

function meu:update_ui()
	local	pg	= param.get
	local 	ref	= self.ref
	local	ui	= self.ui
--	local	sx = pg( ref.sx )
--	local	sy = pg( ref.sy )
--	aaa.debug.show( sx, "sx" )

	local str
	if		self:is_running()	then	str = "Running"
	elseif	self:is_opened()	then	str = "Opened"
	else								str = "Closed"
	end

	if ui.bu_name then
		ui.bu_name:set_text_color( str )
		ui.bu_name:set_text( pg( ref.capture_device_name ) )
	end

	-- if ui.bu_info then
	-- 	ui.bu_info:set_text_color( col )
	-- 	if param.get_bool( ref.is_depth )	then		str = str.." Depth"		end
	-- 	if param.get_bool( ref.is_rgb ) 	then		str = str.." RGB"		end
	-- 	ui.bu_info:set_text( str )
	-- end

	local bind
	bind = self:get_depth_bind()
	ui.bu_depth_mon :set_texture_bind_2d( bind	)
	ui.bu_visu_depth:set_texture_bind_2d( bind	)
	bind = self:get_rgb_bind()
	ui.bu_rgb_mon  :set_texture_bind_2d( bind )
	ui.bu_visu_rgb :set_texture_bind_2d( bind	)
	bind = self:get_ir_bind()
	ui.bu_ir_mon   :set_texture_bind_2d( bind )
	-- if ui.bu_deform_mon then		 	ui.bu_deform_mon:set_texture_bind_2d(		self:get_deform_bind()	)		end
	-- if ui.bu_rgb_aligned_mon then	ui.bu_rgb_aligned_mon:set_texture_bind_2d(	self:get_rgb_aligned_bind()	)	end
end

function meu:set_far( v )			self:set_bu_value( "far", v ) 		end
function meu:set_close( v )			self:set_bu_value( "close", v ) 	end
function meu:set_skew_vert( v )		self:set_bu_value( "skew_vert", v )	end

function meu:is_frame_new()			return self.__b_frame_new			end
function meu:get_frame_count()		return self.__frame_count			end
function meu:get_depth_bind_used()	return self.__depth_bind			end

function meu:update()
	if self.verbose >= 2 then aaa.print_method() end

	local ref = self.ref

	--self:print( "update() "..self:get_proto().." "..self:get_proto().b_allow.." "..self:get_alpha() )
--[[
	local b
	b = (self:get_alpha()>0.) and self.b_play and self:get_proto().b_allow
	if param.get_bool( ref.video_playing ) ~= b then
		param.set( ref.video_play, b )
	end
--]]

--local b = self.b_on_cpu and self:get_proto().b_on_cpu
--	param.set( ref.on_cpu,			b 		)
--	param.set( ref.move_to_texture, not b	)		--	false is slower when on cpu is on
end

--todo this is a quick hack
function meu:draw()
	local ref = self.ref
	local ui = self.ui

	self:draw_layers_begin()

		-- if last frame produce a frame
		--	we should be ready for a new and eventually flip buffer
		local depth_bind_next
		if self.__b_frame_new then
			self.__b_frame_new = false
			local frame_count = self.__frame_count + 1
			self.__frame_count = frame_count
			local s_flip = self.ui.depth_flip:get_item_data()
			--self:print( "s_flip "..s_flip )
			if s_flip~=0 and ( frame_count % 2 == 1 ) then
				-- last frame was the usual dst, next frame should be flipped
				depth_bind_next = s_flip
			else
				depth_bind_next = self:get_depth_bind()
			end
		else
			depth_bind_next =  self.__depth_bind_next or self:get_depth_bind()
		end
		self.__depth_bind_next = depth_bind_next
		--self:print( "dst -> "..self.__depth_bind  )
		param.set(	ref.depth_dst, depth_bind_next	)
		param.set(	ref.rgb_dst, self:get_rgb_bind()	)
		-- param.set(	ref.depth_deform_dst, self:get_deform_bind()	)
		-- param.set(	ref.rgb_aligned_dst, self:get_rgb_aligned_bind()	)
		if GA.b_spy then aaa.spy.push_range( "lidar", 2 ) end

		-- update lidar
		aaa.obj.update_then_draw( ref.layer_lidar )
		if GA.b_spy then aaa.spy.pop_range() end

		-- todo: restore control next.
		-- local meu_begin
		-- if self.b_control_next then
		-- 	meu_begin = self.__meu_begin_kinect_process
		-- 	if not meu_begin then
		-- 		meu_begin = self:get_meu_by_name_no_error( "MuBegin_Kinect" )
		-- 		if meu_begin then
		-- 			self.__meu_begin_kinect_process = meu_begin
		-- 			self:print( "found mu_begin to control "..meu_begin  )
		-- 		else
		-- 			local m = self.__mu_dir_kinect_process
		-- 			if not m then
		-- 				m = self:get_mu_by_name_no_error( "Dir_Kinect" )
		-- 				if m then
		-- 					self.__mu_dir_kinect_process = m
		-- 					self:print( "found dir mu to control "..m  )
		-- 				else
		-- 					self:print_error( "no Dir or MuBegin to control" )
		-- 				end
		-- 			end
		-- 		end
		-- 	end
		-- end

		--do we got a new frame
		local frame_index = param.get( ref.image_index )
		if self.__frame_index_last ~= frame_index then
			self.__frame_index_last = frame_index
			self.__b_frame_new = true
			self.__depth_bind = self.__depth_bind_next

			--unused now
			-- --update layer 2 of it exist infact
			-- if ref.layer_img_process and ref.layer_multitouch then
			-- 	if GA.b_spy then aaa.spy.push_range( "img_process", 2 ) end
			-- 		--self:print( " roro"..ref.layer_img_process )
			-- 		aaa.obj.update_then_draw( ref.layer_img_process )
			-- 	if GA.b_spy then aaa.spy.pop_range() end
			-- end

			-- if meu_begin then
			-- 	meu_begin:set_mode_regular()
			-- end
		else
			--self:print( "No frame "..frame_index )
			--we did not get a new image so we use the old one
			if self.b_control_next then
				-- if meu_begin then
				-- 	meu_begin:set_mode_skip()
				-- else
				-- 	local m = self.__mu_dir_kinect_process
				-- 	if m then
				-- 		m:set_render_skip_next( true )
				-- 	end
				-- end
			end
		end

	self:draw_layers_end()
	--self:set_bu_value( "restart", false )
end
