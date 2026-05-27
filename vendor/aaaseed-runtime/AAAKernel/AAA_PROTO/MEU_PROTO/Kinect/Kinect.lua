function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "2d", "3D", "Core", "CoreGraphic", "Camera", "device", "texture", "input" },
			help = "read the image fluxs from a Kinect camera (v1, v2, and Azure)\n"..
					"in some case it can alsi read the skeleton by this need to be reworked"
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

	local tex_video = self:get_obj_down( "tex_video" )
	self.video = VIDEO:create( "VIDEO_"..self:get_name(), tex_video )
	-- complte ref build in VIDEO
		pgr( "depth_dst",				tex_video,	"bind_dst"				)
		pgr( "depth_cpu",				tex_video,	"image_on_cpu_wanted"	)
		pgr( "depth_gpu",				tex_video,	"move_to_texture"		)
--		pgr( "on_cpu",					tex_video,	"image_on_cpu_wanted"	)
--		pgr( "move_to_texture",			tex_video	)
--		pgr( "sx_depth",				tex_video, 	"size_x"				)
--		pgr( "sy_depth",				tex_video, 	"size_y"				)
		param.set( tex_video, "mipmap_generate", true )

	ref.kinect  = aaa.obj.get_down_by_class( tex_video, "kinect_ui" )
	local k = ref.kinect
		pgr( "version_asked",			k	)
		pgr( "skeleton_selected_id",	k 	)
		pgr( "is_rgb", 			 		k,	"rgb"					)
		pgr( "rgb_asked", 				k	)
		pgr( "rgb_dst", 			 	k,	"rgb_bind"  			)
		pgr( "rgb_cpu", 			 	k	)
		pgr( "rgb_gpu", 			 	k	)

		pgr( "rgb_aligned_asked", 		k	)
		pgr( "rgb_aligned_dst", 		k,	"rgb_aligned_bind"  	)
		pgr( "rgb_aligned_cpu", 		k	)
		pgr( "rgb_aligned_gpu", 		k	)

		pgr( "is_depth", 			 	k,	"depth"					)
		pgr( "depth_clip_side", 		k	)
		pgr( "depth_asked", 			k	)
		pgr( "depth_raw_min", 			k	)
		pgr( "depth_raw_max", 			k	)
		pgr( "depth_raw_clamp", 		k	)

		pgr( "is_skel",					k,	"skeleton"				)
		pgr( "skel_asked",				k,	"skeleton_asked"		)
		pgr( "skel_nb",					k,	"skeleton_nb_tracked"	)

		pgr( "depth_deform_dst",		k,	"depth_deform_bind"		)
		

	self.skel = {}
	for i=1,6 do
		self.skel[i] = { ref = {} }
		local r = self.skel[i].ref
		local pre = "skeleton_0"..i.."_"
		pgr_tab( r, "is_tracked",		k,	pre.."tracked"					)
		pgr_tab( r, "id",				k,	pre.."id"						)
		pgr_tab( r, "mocap_dst",		k,	pre.."mocap_target_dst"			)
		pgr_tab( r, "hand_left",		k,	pre.."hand_left"				)
		pgr_tab( r, "hand_left_conf",	k,	pre.."hand_left_confidence"		)
		pgr_tab( r, "hand_right",		k,	pre.."hand_right"				)
		pgr_tab( r, "hand_right_conf",	k,	pre.."hand_right_confidence"	)
		local pos = {}
		pgr_tab( pos, 1,				k,	pre.."x"	)
		pgr_tab( pos, 2,				k,	pre.."y"	)
		pgr_tab( pos, 3,				k,	pre.."z"	)
		pgr_tab( pos, 4,				k,	pre.."w"	)
		r.pos = pos
	end


	ref.layer_kinect = self:get_layer( 1 )
		--aaa.box_good( "kinect "..ref.layer_kinect )
	--unused now
	--ref.layer_img_process = self:get_layer( 2 )
	--aaa.box_good( "2 "..ref.layer_img_process )
	ref.layer_multitouch = self:get_layer( 3 )
		--aaa.box_good( "3 "..ref.layer_multitouch )
	ref.bdd_multitouch = self:get_layer_bdd( 3 )

	local bdd = ref.bdd_multitouch
	if bdd then
		ref.image_agitation_src	= param.get_ref( bdd, "image_src"					)
		ref.grey_average		= param.get_ref( bdd, "grey_average"				)
		ref.agitation 			= param.get_ref( bdd, "agitation_result"			)
		ref.accum_max 			= param.get_ref( bdd, "remanence_accumulate_max"	)
		ref.accum_trig 			= param.get_ref( bdd, "remanence_restart_trig"		)
	end
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

function meu:reset_kinect( b_all )
	local str = "\"C:\\Program Files\\Azure Kinect SDK v1.4.1\\tools\\AzureKinectFirmwareTool.exe\""
	local args = b_all and "-Reset" or ( "-r "..param.get( self.video.ref.capture_device_id_asked ) )
	aaa.os.execute_shell( str, args )
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local ix,iy
	local SY,DY = 1,.2 

	local mon_syl = 1.92 + 8*9/16
	local mon_syr = 1.4 + 8*9/16
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
		local syt = .8
		--y = y + ((x==1) and .8 or -.2)
		if ref_asked then
			bu = self:add_button(	{x, y,		syt,syt},	what.."_asked",		ref_asked,nil,	true ):set_text_draw( false )
				bu:set_method_on_value_change( self, "restart_kinect", bu )
		end
		bu = self:add_texture_selector( {x+syt,y, 	1.5,syt}, what )
			ui[ "bu_"..what_low.."_dst" ] = bu
		if ref_cpu then
			bu = self:add_button(	{x+8-3, y,		1.5,syt},	what_low.." Cpu",	ref_cpu,nil,	false ):set_text( "Cpu")
				bu:set_text_rect_ratio(2)
		end
		if ref_gpu then
			bu = self:add_button(	{x+8-1.5, y,	1.5,syt},	what_low.." Gpu",	ref_gpu,nil,	false ):set_text( "Gpu" )
		end
		y = y + syt
		bu = add_monitor_one( { x,y, 8,sy-syt },  what.." Dst", b_flip_v )
			ui[ "bu_"..what_low.."_mon" ] = bu
		return bu
	end

	ix,iy = 1,1

	local vref = self.video.ref
	bu = self:add_trig_fn(		{ix,iy,			2,SY},	"Enum" ):set_color_back( "action" )
		bu:set_target_param( vref.capture_enum_trig )
	ix =  ix + 2

	bu = self:add_button(		{ix,iy,			2,SY},   "Kinect Choice", 	nil, nil,		0 )
		bu:set_menu{ "Kinect_1", "Kinect_v2", "Azure" }:set_nb_min_0( 1, 3 )
		--bu:set_target_param( param.get_ref( ref.kinect, "version_asked" ) )
		bu:set_target_lua( self, "kinect_model" )
		bu:set_method_on_value_change( self, "restart_kinect", bu )
		bu:set_text_selector( true )
		ui.bu_kinect_version = bu
	bu = self:add_button(		{ix+2,iy,		2,SY},   "Depth Mode",		nil, nil,    	0 )
		bu:set_menu{ "Narrow_2x2",  "Narrow", "Wide_2x2", "Wide", "Passive_IR_Only" }:set_nb_min_0( 1, 5 )
		bu:set_target_param( param.get_ref( ref.kinect, "depth_start_k4_asked" ) )
		bu:set_method_on_value_change( self, "restart_kinect", bu )
		bu:set_text_selector( true )
		ui.bu_depth_mode = bu
	bu = self:add_button(		{ix+2,iy+1,		2,SY},   "RGB Mode", 		nil, nil,    	0 )
		bu:set_menu{ "HD 720p", "HD 1080p", "HD 1440p+", "2048 x 1536", "4K 2160p", "4096 x 3072" }:set_nb_min_0( 1, 6 )
		bu:set_target_param( param.get_ref( ref.kinect, "rgb_start_k4_asked" ) )
		bu:set_method_on_value_change( self, "restart_kinect", bu )
		bu:set_text_selector( true )
	ix =  ix + 4
	self:add_button(			{ix,  iy, 		2,SY},	"Open", 			vref.capture_open,nil, 	false	)
	bu = self:add_text_info(	{ix+2,iy,		2,.8},	"name" )
		--bu:set_value_load_save( false )
		ui.bu_name = bu
	iy = iy + SY

	self:add_button(			{ix,  iy,		2.,SY},	"Run",				vref.capture_run,nil, 	false	)
	bu = self:add_text_info(	{ix+2,iy,		5,.8},	"State" )
		ui.bu_info = bu

	ix,iy = 9,1
	bu = self:add_trig_fn(		{ix+2,iy,		3,SY},	"Kinect",			aaa.obj.set_focus_ui, ref.kinect		):set_color_back("focus")
	bu = self:add_trig_fn(		{ix+5,iy,		3,SY},	"Tex_Video",		aaa.obj.set_focus_ui, vref.tex_video	):set_color_back("focus")

	local iy2 = 3 + DY
	ix,iy = 1,iy2
	add_mon( 1, iy,			6, mon_syl, 	"Depth",		false, 	ref.depth_asked,		ref.depth_cpu,			ref.depth_gpu		)
	add_mon( 9, iy,			6, mon_syr,		"RGB",			false,	ref.rgb_asked,			ref.rgb_cpu,			ref.rgb_gpu			)
	iy2 = iy2 + mon_syl
	iy = iy2 + DY
	self:set_tab_key_def()

	local SY = .8
	local SX = 2.5
	self:add_param(				{ix,iy,		2,SY},	"Clamp",			ref.depth_raw_clamp 	)
	iy = iy + SY
	self:add_param(				{ix,iy,		2,SY},	"Clip",				ref.depth_clip_side 	)
	bu = self:add_slider_two(	{ix+SX,iy,	8-SX,SY},	"Dist", 			ref.kinect, "depth_raw_min", "depth_raw_max",	-1,.89,	-1,.89 )
	iy = iy + SY
	self:add_param_obj_name(	{ix+4,iy,	 	4,SY}, 	"Skew_Vert",		ref.kinect, "depth_skew_vert",	-8,8		)
	iy = iy + SY + DY

	local SX3 = 8/3
	self:add_button(			{ix,		iy,		2,SY},		"Skel",				ref.skel_asked,	nil,	false	)
	self:add_button(			{ix+SX3,	iy,		SX3,SY},	"Gesture",			self,	"b_gesture",	false	)
	self:add_button(			{ix+SX3*2, 	iy, 	SX3,SY},	"Agitation",		self,	"b_agitation",	false	)
	iy = iy + SY + DY

	SY = 1
	self:add_button( 			{ix,iy,		4,SY},	"Control Next", 		self,		"b_control_next", 			false	)
	bu = self:add_text_info(	{ix+4,iy,	4,SY},	"Control Info" )
		bu:set_value_load_save( false )
		ui.bu_control_info = bu
	iy = iy + SY + DY

	bu = self:add_trig_method(	{ix,iy,		4,SY},	"Azure Reset",		self, "reset_kinect",	false	):set_color_back("restart")
	bu = self:add_trig_method(	{ix+4,iy,	1,SY},	"Azure Reset ALL",	self, "reset_kinect",	true	):set_color_back("restart")
			:set_text("All"):set_text_inside(true)
	iy = iy + SY + DY


	--self:add_bu_texture_target_unit(	{1, iy, 8, dy},		"Depth",	1  )
	--self:add_bu_texture_target_unit(	{1, iy+dy, 8, dy},	"RGB",		2  

	--todo we should redo better
	--[[	
	bu = self:add_selector(	{ix+3,iy,	5,SY},		"Flip" )
		bu:set_nb_min_0( 5, 1 )
		--bu:set_target_lua( self, "__s_flip_dst" )
		local tab = { "No",	"B",	"C",	"F1",	"F2" }
		bu:set_item_text(	1,	unpack(tab) )
		bu:set_item_data( 1, 0 )
		for i=2,5 do
			bu:set_item_data( i,	self:get_bind_by_name( tab[i] ) )
		end
		ui.depth_flip = bu
	--]]

	-- 
	-- bu = add_monitor_one( {1,10.5, 8,5.5}, 		"_Depth", false )
	-- 	ui.bu_visu_depth = bu
	-- bu = add_monitor_one( {9,10.5, 8,mon_sy},	"_RGB", false )
	-- 	ui.bu_visu_rgb = bu

	self:set_tab_key( "DST" )
	ix,iy =  1, 4.5

	add_mon( 1, iy2,					6, mon_syl, "Deform",		false,	nil,					nil,					nil					)
	add_mon( 9, iy2+mon_syr-mon_syl,	6, mon_syr,	"RGB_Aligned",	false,	ref.rgb_aligned_asked,	ref.rgb_aligned_cpu,	ref.rgb_aligned_gpu	)


--	ix = 9
--	iy = 1

--	iy = 3.25
--	iy = iy + 4
--	self:add_button( {ix, iy }, "On Cpu", 	self, "b_on_cpu", false	)

	--iy = iy + 4.5
	--self:add_param_obj_name(	{ix,iy},	"sound",	r, "sound_active",	0,1 )
	--self:add_param_obj_name(	{ix,iy+1},	"volume",	r, "volume",		0,1 )

	--local bind = param.get( r, "bind_dst" )
end

function meu:get_kinect()
	return self.ref.kinect
end
function meu:is_skeleton()
	local id = param.get( self.ref.skeleton_selected_id )
	return id~=0
end

function meu:get_skel_id_pos_dst( id )
	local skel = self.skel[id]
	local pget = param.get
	local r = skel.ref
	local b = pget( r.is_tracked )
	if b==0 then return end

	local p = r.pos
	--todo use some low level get_xyz ot add it
	return pget(r.id), { pget(p[1]), pget(p[2]), pget(p[3]), pget(p[4]) }, pget(r.mocap_dst)

end
function meu:is_opened()			return self.video:is_capture_opened()	end
function meu:is_running()			return self.video:is_capture_running()	end

function meu:get_depth_bind()		return self.ui.bu_depth_dst      :get_bind_2d()		end
function meu:get_rgb_bind()			return self.ui.bu_rgb_dst        :get_bind_2d()		end
function meu:get_deform_bind()		return self.ui.bu_deform_dst     :get_bind_2d()		end
function meu:get_rgb_aligned_bind()	return self.ui.bu_rgb_aligned_dst:get_bind_2d()		end

function meu:draw_icon()
	if not self:is_proto() then
		self:draw_icon_texture( self:get_depth_bind() )
		self:draw_icon_frame( self.__frame_index_last )
		gol.push_translate_scale_2d( -.35, 0, .3, 1.2 )
			aaa.draw_rect_line(-0.3, -0.2, 0.3, 0.2)
			aaa.draw_circle_axe_z( -.15, 0, 0, 0.1, 8 )
			aaa.draw_circle_axe_z(  .15, 0, 0, 0.1, 8 )
		gol.pop_matrix()
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

	local bu = ui.bu_name
	if bu then
		bu:set_text_color( str )
		bu:set_text( string.gsub( pg( self.video.ref.capture_device_name ), "Kinect ", "" ) )
	end

	bu = ui.bu_info
	if bu then
		bu:set_text_color( str )
		if param.get_bool( ref.is_depth )	then	str = str.." Depth"		end
		if param.get_bool( ref.is_rgb ) 	then	str = str.." RGB"		end
		if param.get_bool( ref.is_skel ) 	then	str = str.." Skel "..param.get( ref.skel_nb )	end
		bu:set_text( str )
	end

	local bind
	bind = self:get_depth_bind()
	ui.bu_depth_mon	:set_texture_bind_2d( bind	)
--	ui.bu_visu_depth:set_texture_bind_2d( bind	)
	bind = self:get_rgb_bind()
	ui.bu_rgb_mon	:set_texture_bind_2d( bind )
--	ui.bu_visu_rgb	:set_texture_bind_2d( bind	)
	if ui.bu_deform_mon then		ui.bu_deform_mon:set_texture_bind_2d(		self:get_deform_bind()	)		end
	if ui.bu_rgb_aligned_mon then	ui.bu_rgb_aligned_mon:set_texture_bind_2d(	self:get_rgb_aligned_bind()	)	end

	bu = ui.bu_control_info
	if self.b_control_next then
		if self.__mu_dir_kinect_process then
			--if not self.__b_frame_new then
				bu:set_text( self.__b_frame_new and "Dir Ok" or "Dir Skip" ) 
			--end
		end
	else
		bu:set_text( "No" ) 
	end
end

function meu:get_lasso_nb()
	local le, ri =  0, 0
	local str
	for i = 1,6 do
		local s = self.skel[i]
		local r = s.ref
		if param.get_bool( r.is_tracked ) then
			str = param.get_str( r.hand_left )
			--aaa.debug.show( str, "le" )
			if str == "Lasso" then
				le = le + 1
			end
			str = param.get_str( r.hand_right )
			--aaa.debug.show( str, "r" )
			if str == "Lasso" then
				ri = ri + 1
			end
		end
	end

	return le, ri
end

function meu:set_far( v )			self:set_bu_value( "far", v ) 		end
function meu:set_close( v )			self:set_bu_value( "close", v ) 	end
function meu:set_skew_vert( v )		self:set_bu_value( "skew_vert", v ) end
function meu:set_gesture( b )		self:set_bu_value( "gesture", b )	end

function meu:is_frame_new()			return self.__b_frame_new	end
function meu:get_frame_count()		return self.__frame_count	end
function meu:get_depth_bind_used()	return self.__depth_bind	end

function meu:get_grey_and_agitation()
	local ref = self.ref
	local bdd = ref.bdd_multitouch
	if bdd then
		return param.get( ref.grey_average ), param.get( ref.agitation )
	else
		return 0,0
	end
end

-- 1->v1, 2->v2, 3->azure
function meu:get_kinect_model()
	return self.kinect_model + 1
end
--todo make surer the cache is always invalidated and reread correctly when camera are changed
function meu:get_kinect_rgb_camera_info()
	local t = self.rgb_camera_info
	if (not t) and self:is_running() then
		local k = self.ref.kinect
		local pg = param.get
		local cx = pg( k, "rgb_camera_c_x" )
		if cx~= 0 then
			local rot = {}	
			for i=1,9 do
				rot[i] = pg( k, "rgb_camera_rotation_"..i )
			end
			local tra = {}
			for i=1,3 do
				tra[i] = pg( k, "rgb_camera_translation_"..i )/1000	
			end
			t = { rot=rot, tra=tra }
			t.intrinsics = { cx, pg( k, "rgb_camera_c_y" ), pg( k, "rgb_camera_f_x" ), pg( k, "rgb_camera_f_y" ) }
			self.rgb_camera_info = t
		end
	end	
	return t
end

function meu:restart_kinect()
	self.video:restart_capture()
	self.rgb_camera_info = nil
end

function meu:update()
	if self.verbose >= 2 then aaa.print_method() end

	if not self:is_running() then
		self.rgb_camera_info = nil
	end

	local ref = self.ref
	param.set( ref.version_asked, self.kinect_model + 1 )
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

	if self.b_gesture then
		local le, ri = self:get_lasso_nb()
		aaa.debug.show( le.." "..ri, "le ri" )

		local function update_hand( hand, nb, delay )
	 		hand.b_trig = false
	 		local up = hand.up or 0
			if hand.b_lock then
				if nb==0 then
					up = up - aaa.time.dt
				end
			else
				up = up + ( nb==0 and -2 or nb ) * aaa.time.dt
			end
			--self:print( up )
	 		if up < 0 then
	 			up = 0
	 			hand.b_lock = false
	 		elseif up > delay then
	 			if not hand.b_lock then
	 				hand.b_trig = true
	 				hand.b_lock = true
	 			end
	 		else
	  			hand.b_trig = false
	 		end
	 		hand.up = up
	 	end

	 	local hand_le = self.hand_le or {}
	 	update_hand( hand_le, le, .25 )
	 	self.hand_le = hand_le
	 	if hand_le.b_trig then aaa.print( "valid left Lasso" ) end

	 	local hand_ri = self.hand_ri or {}
	 	update_hand( hand_ri, ri, .25 )
	 	self.hand_ri = hand_ri
	 	if hand_ri.b_trig then aaa.print( "valid right Lasso" ) end

	 	aaa.debug.show( hand_ri.up, "up" )
	 else
	 	self.hand_le = {}
	 	self.hand_ri = {}
	 end
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
			--todo redo better
			--[[
			local s_flip = ui.depth_flip:get_item_data()
			--self:print( "s_flip "..s_flip )
			if s_flip~=0 and ( frame_count % 2 == 1 ) then
				-- last frame was the usual dst, next frame should be flipped
				depth_bind_next = s_flip
			else
				depth_bind_next = self:get_depth_bind()
			end
			]]--
			depth_bind_next = self:get_depth_bind()
		else
			depth_bind_next =  self.__depth_bind_next or self:get_depth_bind()
		end
		self.__depth_bind_next = depth_bind_next
		--self:print( "dst -> "..self.__depth_bind  )
		param.set(	ref.depth_dst,			depth_bind_next				)
		param.set(	ref.rgb_dst,			self:get_rgb_bind()			)
		param.set(	ref.depth_deform_dst,	self:get_deform_bind()		)
		param.set(	ref.rgb_aligned_dst,	self:get_rgb_aligned_bind()	)
		-- update kinect
		if GA.b_spy then aaa.spy.push_range( "kinect", 2 ) end
			--self:print( ref.layer_kinect )
			aaa.obj.update_then_draw( ref.layer_kinect )
		if GA.b_spy then aaa.spy.pop_range() end

		local meu_begin
		local mu_dir
		if self.b_control_next then
			meu_begin = self.__meu_begin_kinect_process
			mu_dir = self.__mu_dir_kinect_process
			--self:print( "self.__mu_dir_kinect_process is "..mu_dir )
			if not meu_begin and not mu_dir then	
				meu_begin = self:get_meu_by_name_no_error( "MuBegin_Kinect" )
				if meu_begin then
					self.__meu_begin_kinect_process = meu_begin
					self:print( "found mu_begin to control "..meu_begin  )
				else
					if not mu_dir then
						local name = self:get_inst_key()
						mu_dir = self:get_mu_by_name_no_error( "Dir_Kinect"..name )
						if not mu_dir then
							mu_dir = self:get_mu_by_name_no_error( "Dir_Kin"..name )
						end
						-- if not mu_dir then
						-- 	mu_dirm = self:get_mu_by_name_no_error( "KIN"..name )
						-- end
						if mu_dir then
							self.__mu_dir_kinect_process = mu_dir
							self:print( "found dir mu to control "..mu_dir  )
						else
							self:print_error( "no Dir or MuBegin to control" )
						end
					end
				end
			end
		end

		--do we got a new frame
		local frame_index = self.video:get_image_index()
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

			--we do agitation
			if self.b_agitation and ref.layer_multitouch then
				if GA.b_spy then aaa.spy.push_range( "layer_multitouch", 2 ) end
					--todo deal with forcing on cpu for this
					--todo lotus special case we comment
					--param.set(	ref.image_agitation_src,	self.__depth_bind	)
					aaa.obj.update_then_draw( ref.layer_multitouch )
				if GA.b_spy then aaa.spy.pop_range() end
			end

			if meu_begin then
				meu_begin:set_mode_regular()
			end
		else
			--self:print( "No frame "..frame_index )
			--we did not get a new image so we use the old one
			if self.b_control_next then
				if meu_begin then
					meu_begin:set_mode_skip()
				elseif mu_dir then
					mu_dir:set_render_skip_next( true )
				end
			end
		end

	self:draw_layers_end()
	--self:set_bu_value( "restart", false )
end

function OSC_MESS:do_kinect( id )
	self:pop_tag()
	self:dump( "kinect 1" )

	local kinect = app:get_meu_by_name_no_error( "Kinect_2" )
	if not kinect then return end

	local tag = self:get_tag()
	if tag == "gesture" then
			self:pop_tag()
			local what = self:get_tag()
		if 	what == "enable"	then
			aaa.print( "kinect 1 gesture enable "..self:get_arg() )
			kinect:set_gesture( self:get_arg()~=0 )
		end
	elseif tag == "close" then
		aaa.print( "kinect 1 close "..self:get_arg() )
		kinect:set_close( self:get_arg() )
	elseif tag == "far" then
		aaa.print( "kinect 1 far "..self:get_arg() )
		kinect:set_far( self:get_arg() )
	elseif tag == "skew" then
		self:pop_tag()
		local what = self:get_tag()
		if 	what == "vertical"	then
			aaa.print( "kinect 1 skew vertical "..self:get_arg() )
			kinect:set_skew_vert( self:get_arg() )
		end
	end
end

