--
--	POINT_3D
--
--todo make it local or regroup with a better class
CLASS.DECLARE( "POINT_3D", nil, {__factor = .25} )

function POINT_3D:set_factor( f ) self.__factor = f	end
function POINT_3D:set( x, y, z )
	local	f	=	self.__factor
	local	omf	=	1-f
	if self.__x then
		self.__x = f * x + omf * self.__x
		self.__y = f * y + omf * self.__y
		self.__z = f * z + omf * self.__z
	else
		self.__x = x
		self.__y = y
		self.__z = z
	end
end
function POINT_3D:get()
	return self.__x, self.__y, self.__z
end


if CLASS.DECLARE( "TANK" ) then
	--aaa.box_debug( aaa.obj.get_name( aaa.get_caller() ) )
	--aaa.box_debug( aaa.obj.get_name( aaa.layers.get_cur() ) )
	--aaa.box_debug( aaa.obj.get_name( aaa.layer.get_cur() ) )
	--this is the default SoOuest version
	local layer = aaa.layers.get_layer( aaa.layers.get_cur(), 1 )
	--aaa.box_debug( aaa.obj.get_name( layer )
	--	this value is replaced by in TANK:init
	TANK.__ref_attrib_layer = layer
	TANK.b_machine_mono = true
end

-- app_owner is really for SoOuest : no Go, no fbx, master and slave for network...
function TANK:create( name, app_owner, meu, b_fbx )
	name = name or string.lower( app_owner:get_class_name() )
	local self = TANK:create_instance( name )

	self.ref = {}
	self.ui = {}
	self.b_draw_creatures = true
	self.aqua = app_owner
	self.meu = meu
	self.b_loaded = false
	self.b_fbx = b_fbx

	self.morph_value_ui = {}
	return self
end

function TANK:compute_clip( dist_cam_origin, x_on_glass, f )
	local a, b, c
	a = ( 1. - 5. / dist_cam_origin ) / x_on_glass
	b = 1. / dist_cam_origin
	c = - 1
	f = f / math.sqrt( a*a + b*b )
	if x_on_glass > 0 then
		f = -f
	end
	return { a=a*f, b=b*f, c=c*f }
end

function TANK:is_fbx()	return self.b_fbx	end

--todo optimize
function TANK:is_fish_clipped( x,y,z, d_clip )
	--todo improve  : avoid even calling
	if self.aqua then
		--aaa.print_fn()
		local clip = self.clip_left
		if not clip then return false end
		if (clip.a*x + clip.b*z + clip.c) > d_clip then return true end
		clip = self.clip_right
		return (clip.a*x + clip.b*z + clip.c) > d_clip
	else
		local b = self.__meu_tank_master:clip_fish( x,y,z, d_clip ) or false
		--self:print( "clip "..b )
		return b
	end
end

function TANK:init_creatures()
	self.__creas_nb = 0
	self.__creas = {}
end
--hack monaco
function TANK:init_races()
	TANK.__races = RACES.create( "races" )
	return self.__races
end
function TANK:init_fish_begin()
	self:init_races()
	self:load_races()

	self:init_creatures()

	--if self:is_master() then
	self.__ggrea = GGREA:create( self:get_name() )
	if self.aqua then
		self.aqua:send_osc( "/aqua/restart" )
	end
end
function TANK:update()
	TANK.cur = self

	local pass_info = ga:get_pass_info()
	--( pass_info, "pass_info", 2 )
	local b_pass_update = pass_info.pass_index==1
	self.b_pass_update = b_pass_update

	--todo make sur this is ok in all the cases
	app.__b_is_fish_boid = app.is_fish_boid and app:is_fish_boid()
	self.b_boid_update = self.b_boid_update_ui and b_pass_update and ( not app.is_fish_boid or app:is_fish_boid() )


	--self:print( "TANK:update() "..ga:get_pass_info().name.." "..b_pass_update )
	if b_pass_update then
		local caustic = self.caustic
		--self:print( "TANK:update() "..caustic )
		if caustic then
			caustic:set_phase( aaa.time.t * .3 )
			local bind = caustic:get_bind()
			--self:print( "caustic bind is "..bind )
			if bind then
				if app.set_caustic_bind then
					app:set_caustic_bind( bind )
				else
					gol.set_tex_unit_2d_bind( 8, bind )
				end
			end
		end

		--todomona do we need this every frame ?
		if self.__ref_def then
			aaa.obj.update_then_draw( self.__ref_def )
		end
		if self.__ref_run then
			aaa.obj.update_then_draw( self.__ref_run )
		end
		if self.update_after then
			self:update_after()
		end
	end


	--todo refine

--	param.set( self.ref.b_curve_intercept, self.b_edit_curve )
end

function TANK:draw()
	TANK.cur = self
	if not self.b_loaded then return end

	local b_fbx = self:is_fbx()
-- CURVE
	if self.b_pass_update then
		aaa.obj.update_then_draw( self.ref.curve_edit.layers )
	end
	--	gol.reset()

-- CREATURES
	--self:print( "self.b_draw_creatures "..self.b_draw_creatures )
	if self.b_draw_creatures then
		--self:print( "TANK:draw() draw_creatures "..ga:get_pass_info().name.." "..self.b_pass_update )

		CELT.begin_draw()
		--we don't erase to avoid allocations and so lost of time
		--todo	RACE.draw_on_grid()

		local b_update = self.b_pass_update
		if b_update then
--			if not b_fbx then	--hackq fbx was blocking for meu_fbx (2021 Nov) have to sort the whole fbx.meu thing 
				self.__races:update()
--			end
		end

		local b_draw = true
		local b_osc = false
		local aqua = self.aqua
		local ggrea = self:get_ggrea()
		self.b_curve_rt_update		= self.b_curve_rt_update_ui
		self.b_curve_rt_draw		= self.b_curve_rt_draw_ui
		self.b_curve_rt_point_draw	= self.b_curve_rt_point_draw_ui
		if aqua then
			ggrea:update( aqua:is_master(), b_update )
			b_draw = aqua:is_slave()
			b_osc = true
		else
			ggrea:update( true, b_update )
		end

		if not b_fbx then
			aaa.obj.update_then_draw( self.__ref_attrib_layer )
		end

		if b_draw and self.b_pass_update then
			self.__creas_nb = 0
			ggrea:draw( b_osc )
			aaa.show( self.__creas_nb, "creas_nb" )
			--aaa.show( #(self.__creas), "creas_table_nb" )
			-- set the remaining crea to nil, not even sure it is useful
			local creas = self.__creas
			for i = self.__creas_nb+1, #creas do
				creas[i] = nil
			end
		end

		-- draw fish and fish segment
		self:draw_creas_all()
	end
end

function TANK:reload_local()
	local races = self.__races
	if races then
		races:process_race_defs()
	end
end
function TANK:reload_shader()
	local races = self.__races
	if races then
		races:reload_shader()
	end
end
function TANK:set_fog()
	local v = self.ui.bu_fog:get_value()
	v = v>0.5
	aaa.print_fn( v )
	self.__races:set_fog( v )
end
function TANK:set_fog_current()
	self.__races:set_fog_current()
end

function TANK:race_define_ui( )
	local bus = BUS:begin_window( "RACE" )
	bus:set_bu_pos_load_save( false )
		self.__races:define_ui( bus, false )
	bus:end_window()
	return bus
end

function TANK:focus_curve_edit()
	local ref = self.ref
	local layer = ref.curve_edit.layer
	local curve = ref.curve_edit.bdd

	aaa.obj.set_focus_ui( layer )
--	aaa.obj.set_focus_ui( curve )
end

function TANK:define_ui_global( bus )
	local bu
	local aqua = self.aqua
 
	bu = bus:add_but_target_lua(		"MonoPC", 		nil,	self,	"b_machine_mono", 	not aqua ):set_text_rect_ratio( 4 )

		--bu:set_value_load_save( true )
	bu = bus:add_but_target_lua(		"Deferred",		nil,	self,	"b_deferred", 	 	aqua~=nil ):set_text_rect_ratio( 4 )
	bus:move_xy( 1, 0 )
		bu = bus:add_but_target_lua(	"Z Sort",		nil,	self,	"b_sort",			true	)
		bus:move_xy( -1, 0 )

	bu = bus:add_slider_target(			"Speed", 		nil,		self, "creature_speed" )
	bu = bus:add_but_target_lua(		"Caustic Play",	nil,	self,	"b_caustic_play",	true	)
		bu:set_text_rect_ratio( 8 )
	--todo this should be more simple
--		bu = bus:add_but_flip_method(	"Fog",			nil,	self,	"set_fog"			)
--			self.ui.bu_fog = bu

--		bu = bus:add_but_flip_method(	"hack Fog cur",	nil,	self,	"set_fog_current"	)

	bu = bus:add_trig( 					"Hook Camera", {nil,nil, 4,1}  ):set_method_on_click( self, "hook_camera" )
end


function TANK:define_ui_curve( bus )
	local bu
	local aqua = self.aqua

	--	CURVE
	--todoaqua make sure we init before
	local curve_edit = self.ref.curve_edit
	if curve_edit then
		bu = bus:add_but_target_param(	"Curve See",			nil,	curve_edit.active, 0 )
			bu:set_value_load_save( true )
			bu:set_text_rect_ratio( 4 )

--		bus:move_xy( 4, -1 )
--	this is manual for now
		--bu = bus:add_but_target_lua(	"Edit",					nil,	self,			"b_edit_curve", 		false )
--		bu = bus:add_but_target_param(	"Edit",					nil,	curve_edit.b_intercept, 			false )
				--todoaqua refine
--			bu:set_value_load_save( aqua and (not aqua.b_slave_onsite) or true )

--		bus:move_xy( -4, 0 )

		bu = bus:add_slider( "Dataset Id", {	nil, nil, 4, 1 } )
			bu:set_target_param( curve_edit.dataset_id )
			bu:set_min_max( 1, 128 )
			bu:set_show_value( true )
		bus:move_xy( 4, -1 )
		bu = bus:add_trig( "Focus Curve", {nil,nil, 4,1} ):set_text( "Edit Curve" ):set_color_back("focus")
			bu:set_method_on_click( self, "focus_curve_edit" )
		bus:move_xy( -4, 0 )

	end
end

function TANK:define_ui_updraw( bus )
	local bu
	local aqua = self.aqua
	local DX = 1
	local DY = .2

	bu = bus:add_but_target_lua(		"Boid Update",	nil,	self,			"b_boid_update_ui",			true	):set_text( "Boid"  )--:set_text_rect_ratio( 5 )
		bus:move_xy( DX, 0 )
		bu = bus:add_but_target_lua(	"Boid Draw",	nil,	self,			"b_boid_draw_ui",			false	):set_text( "Draw"  )--:set_text_rect_ratio( 5 )
			self.bu_boid_draw = bu
		bus:move_xy( -DX, DY )


	bu = bus:add_but_target_lua(		"Curve Update",	nil,	self,			"b_curve_rt_update_ui",		false	):set_text( "Curve"  )--:set_text_rect_ratio(4)
		bus:move_xy( DX, 0 )
		bu = bus:add_but_target_lua(	"Curve RT",		nil,	self,			"b_curve_rt_draw_ui",		false	):set_text( "Draw"  )--:set_text_rect_ratio(4)
		bus:move_xy( DX, 0 )
		bu = bus:add_but_target_lua(	"Point RT",		nil,	self,			"b_curve_rt_point_draw_ui",	false	):set_text( "Point"  )--:set_text_rect_ratio(4)
		bus:move_xy( -DX, 0 )
		bu = bus:add_trig( 				"Erase",		{nil,nil, 3,1} ):set_color_back("restart"):set_method_on_click(	self, "erase_curve_rt" )
		bus:move_xy( -DX, DY )

--		bu = bus:add_slider(			"Factor",		{nil,nil,	4,1}	):set_target_lua(		self, "curve_factor" ):set_min_max( .05, 1 ):set_meter( true )


	bu = bus:add_but_target_lua(		"Fish",			nil,	self,			"b_draw_fish",				true	)	--:set_text_rect_ratio( 2 )
		bus:move_xy( DX, 0 )
		bu = bus:add_but_target_lua(	"Fish Color",	nil,	self,			"b_draw_fish_color",		true	)	--:set_text_rect_ratio( 2 )
		bus:move_xy( -DX, 0 )
		self.bu_draw_fish = bu
	bu = bus:add_but_target_lua(		"Fish Symbo",	nil,	self,			"b_draw_fish_symbo",		false	)	--:set_text_rect_ratio( 6 )

		bus:move_xy( DX, 0 )
		bu = bus:add_slider(			"Null Size"		):set_target_lua(		self,			"null_size",		1.		):set_min_max( 0, 1 ):set_meter( true )
		bus:move_xy( -DX, 0 )

	bu = bus:add_but_target_lua(		"Fish Seg",		nil,	self,			"b_draw_fish_seg",		false	):set_text_rect_ratio( 4 )
		bus:move_xy( DX, 0 )
		bu = bus:add_slider(			"Line Size",	{nil,nil,	4,1}	):set_target_lua(		self,			"line_size",		1.		):set_min_max( 1, 16 ):set_meter( true )

		bu = bus:add_but_target_lua(	"Line",			nil,	self,			"b_draw_line",			true	)--:set_text_rect_ratio( 2 )
		bu = bus:add_but_target_lua(	"Point",		nil,	self,			"b_draw_point",			false	):set_text_rect_ratio( 2 )
		bu = bus:add_but_target_lua(	"Catmull",		nil,	self,			"b_draw_catmull_rom",	false	)--:set_text_rect_ratio( 3 )
		bu = bus:add_but_target_lua(	"Exp",			nil,	self,			"b_draw_exp",			false	)--:set_text_rect_ratio( 3 )
		bus:move_xy( -DX, 0 )

end
function TANK:erase_curve_rt()
	local ggrea = self:get_ggrea()
	if ggrea then ggrea:erase_curve_rt() end
end
function TANK:restart()
	self:print_inverse( "TANK:restart()" )
	self:erase_curve_rt()
end
function TANK:set_boid_draw( b )
	self.bu_boid_draw:set_value( b )
--	self.b_boid_draw_ui = b
end
function TANK:set_draw_fish( b )
	self:print( "set_draw_fish( "..b.." )" )
	self.bu_draw_fish:set_value( b )
--	self.b_draw_fish = b
end
function TANK:play_fish_sound( name, x )
	self.__meu_tank_master:play_fish_sound( name, x )
end

function TANK:define_ui_draw()
	local bus = BUS:begin_window( "Draw" )
	bus:set_bu_pos_load_save( false )

		self:define_ui_global( bus, nil )
		self:define_ui_updraw( bus )

	bus:end_window()
	return bus
end

function TANK:define_ui()
	TANK.cur = self
	local bus = BUS:create( "TANK" )
	bus:set_bu_pos_load_save( true )
	--self.ui.bus = bus
	bus:set_active( true )
	bus:init_begin_add_to_ga()

		local bu
		local Y = 2

		if self.meu then
			self.__races:define_ui( bus, self.meu )
		else
			bu = bus:add_window(	"Race",	self:race_define_ui(),		{ 0,Y,	2.5,2}	)
			bu = bus:add_window(	"Draw",	self:define_ui_draw(),		{-2,Y,	2.5,2}	)
			bu = bus:add_window(	"Fish",	self:define_ui_select(),	{ 1,Y,	2,  2}	)
		end

	bus:init_end()

	ga:register_ui_group( bus )

	self.ga_bus = bus

	return bus
end

--todo complete
function TANK:free()
	local bus = self.ga_bus
	if bus then
		ga:remove_ui_group( bus )
		ga:remove_bus( bus )
		--self:box_debug( "look" )
		self.ga_bus = nil
	end
	if self.__ggrea then
		self.__ggrea:free()
		self.__ggrea = nil
	end
	self.b_loaded = false
end
--
--	GLOBAL
--
function TANK:sort_in_z_direct( t, n )
	if GA.b_spy then aaa.spy.push_range( "ZSort", 2  ) end

		if true then
			table.sort( self.__creas, function (a,b) return a.z < b.z end )
		else
			local incs = {	1391376,
					463792, 198768, 86961, 33936,
					13776, 4592, 1968, 861, 336,
					112, 48, 21, 7, 3, 1 }
			--for _, h in ipairs(incs) do
			for ih = 1, #incs do
				local h = incs[ih]
				for i = h + 1, n do
					local v = t[i]
					for j = i - h, 1, -h do
						local testval = t[j]
						if v.z < testval.z then
							break
						end
						t[i] = testval
						i = j
					end
					t[i] = v
				end
			end
		end

	if GA.b_spy then aaa.spy.pop_range() end
	return t
end

function TANK:draw_creas( fn_name )
	--aaa.print_fn()
	if GA.b_spy then
		aaa.spy.push_range( fn_name, self.spy_col )
		self.spy_col = self.spy_col + 1
	end

		local creas = self.__creas
		local fn = CREATURE[fn_name]	--todoaqua should not be in RACE
		for i=1,self.__creas_nb do
			--self:print( "TANK:draw_creas()"..i )
			fn( creas[i] )
		end

	if GA.b_spy then
		aaa.spy.pop_range()
	end
end

function TANK:draw_creas_gol( fn_name, ... )
	--self:print( "draw_creas_gol( "..fn_name )
	if GA.b_spy then
		aaa.spy.push_range( fn_name, self.spy_col )
		self.spy_col = self.spy_col + 1
	end

		local creas = self.__creas
		local fn = CREATURE[fn_name]	--todoaqua should not be in RACE
		--self:print( "draw_creas_gol( "..fn_name.." "..self.__creas_nb )
		for i=1,self.__creas_nb do
			local crea = creas[i]
			--self:print( "TANK:draw_creas_gol()"..i )
			crea:set_color_gol()
			fn( crea, ... )
		end

	if GA.b_spy then
		aaa.spy.pop_range()
	end
end

function TANK:draw_creas_all()
	
	if not self.b_deferred then return end
--	aaa.print_fn()

	--self:print( "TANK:draw_creas_all "..self.__creas_nb )
	-- SORT in z (painter algo)
	if self.b_sort then
		self:sort_in_z_direct( self.__creas, self.__creas_nb )
	end

	self.spy_col = 1

	-- real rendering of the fishes
	if self.b_draw_fish	then
		--self:print( "TANK:draw_creas_all() draw_creatures "..self.b_draw_fish )
		if self.b_draw_fish_color then
			self:draw_creas_gol( "draw_deformed" )
		else
			self:draw_creas( "draw_deformed" )
		end
	end

	--aaa.print( "draw_creas_all b" )
	--todo reduce call nb here
	gol.reset()
	gol.set_default()
	gol.set_depth( true )

	--gol.disable_lighting()
	--gol.color_green()
	gol.set_line_width( self.line_size * .5 )
	if self.b_draw_fish_symbo	then	self:draw_creas_gol( self.b_machine_mono and "draw_symbo_mono" or "draw_symbo", self.null_size ) end

	gol.set_line_width( self.line_size )
	gol.set_point_size( self.line_size*2 )

	--gol.set_depth( false )
	if self.b_draw_fish_seg then
		--aaa.print( "draw_creas_all c" )
		if self.b_draw_line			then	self:draw_creas_gol(	"draw_deformed_line" 		)	end
		if self.b_draw_catmull_rom	then	self:draw_creas_gol(	"draw_deformed_catmull_rom"	)	end
		if self.b_draw_point		then	self:draw_creas_gol(	"draw_deformed_point"		)	end

		if self.b_draw_exp			then	self:draw_creas_gol(	"draw_exp"					)	end
	end
	--RACE.set_camera()
end

function TANK:get_ggrea()				return self.__ggrea								end
function TANK:get_grea_by_name( name )	return self.__ggrea:get_grea_by_name( name )	end
function TANK:add_grea( name, race_name, nb, boid_id, location )
	return self.__ggrea:add_grea( name, race_name, nb, boid_id, location )
end
function TANK:get_grea_nb()				return self.__ggrea:get_grea_nb()				end
function TANK:get_grea_by_id( id )		return self.__ggrea:get_grea_by_id( id )		end
--todoaqua
function TANK:get_race( race_name )		return self.__races:get( race_name )			end

function TANK:send_one_fish( name )
	--aaa.print_fn()
	local grea = self:get_grea_by_name( name )
	if grea then
		self:print( "TANK:send_one_fish(() found grea "..grea )
		grea:do_launcher( true )
	else
		self:print( "TANK:send_one_fish(() No grea found for "..name )
	end
end


function TANK:define_ui_select()
	local bus = BUS:begin_window( "Fish" )
		--bus:set_bu_pos_load_save( false )
		bus:add_wiz_fps( {2.5, 4., 1., .25} )
		self:get_ggrea():define_ui( bus )
		self:get_ggrea():define_ui_greas( bus )
	bus:end_window()
	return bus
end

function TANK:define_crea_candidate( crea )
	self.crea_hooked = crea
	self:print( "TANK:define_crea_candidate() "..crea )
	self.pt_hook_pos	= POINT_3D{}
	self.pt_hook_target = POINT_3D{}
end

function TANK:set_crea_hook_candidate()
	local ggrea = self:get_ggrea()
	if ggrea then
		local grea = ggrea:get_grea_by_id( math.random(ggrea:get_grea_nb()) )
		self:print( "hook_camera() "..grea )
		local crea = grea:get_crea_random()
		self:define_crea_candidate( crea )
		return crea
	end
end

--todo use CAMERA_REF (to be defined )
function TANK:place_camera_hooked( cam, crea )
	--todo use CAMERA

	crea = crea or self.crea_hooked
	if not crea then
		self:print_error( "No creature hooked set to attach the camera" )
		return
	end

	--self:print( cam )
	--local get_point = aaa.bdd.get_point_tangent_s
	--local curve		=	crea.curve
	--local id_curve	= crea.__curve_id
	--local s = crea.s * crea.len_curve_over
	--crea.s_inter * crea.seg_len_over
	--local s_off = crea.s_inter --* crea.seg_len_over
	--s = s + s_off * .2
	--local x,y,z = get_point( curve, id_curve, s-.1 )
	--s = s-.4
	--local x, y,	z,	tx,	ty,	tz	= aaa.bdd.get_point_tangent_s(	curve, id_curve, s	)
	--self:print( "toto ".. cam.." "..crea )

	--	a little raw on continuity
	--		but doing better require to go a full catmull-rom
	--		or add physics on top of this
	local inter = crea.s_inter * crea.seg_len_over
	--inter = 1
	--self:print( inter )
	local inter_m1 = 1-inter

	local pos = crea.pos
	--table.print( pos, "crea.pos" )
	--self:print(  pos[4].." "..pos[5].." "..pos[6] )
	local pt
	local x, y, z

	x = pos[4] * inter + pos[7] * inter_m1
	y = pos[5] * inter + pos[8] * inter_m1
	z = pos[6] * inter + pos[9] * inter_m1
	pt = self.pt_hook_pos
	pt:set( x, y, z )
	x,y,z = pt:get()
	param.set( cam, "tra_x", x )
	param.set( cam, "tra_y", y )
	param.set( cam, "tra_z", z )

	x = pos[1] * inter + pos[4] * inter_m1
	y = pos[2] * inter + pos[5] * inter_m1
	z = pos[3] * inter + pos[6] * inter_m1
	pt = self.pt_hook_target
	pt:set( x, y, z )
	x,y,z = pt:get()
	param.set( cam, "center_x", x )
	param.set( cam, "center_y", y )
	param.set( cam, "center_z", z )
end

function TANK:choice_for_hook( grea )
	aaa.print_method()
	self.grea_to_hook = grea
end

function TANK:set_module_fish_dir_name( dirname )
	--aaa.box_debug( "TANK:set_module_fish_dir_name to \n"..dirname )
	self.__module_fish_dir_name = dirname
	CELT:set_dir( self.__module_fish_dir_name.."fish_data/" )
end
function TANK:get_module_fish_dir_name()
	--self:box_look( self.__module_fish_dir_name )
	return self.__module_fish_dir_name
end

--todo make it even better
function TANK:get_fish_layers( name )
	return aaa.obj.get_by_name( self:get_module_fish_dir_name()..name.."/fx.layers_param" )
end
function TANK:get_boid_layers( id )	return self:get_fish_layers( "TBoid_"..id )	end
function TANK:get_def_layers()	return self.__ref_def	end
function TANK:get_run_layers()	return self.__ref_run	end

function TANK:init()
	TANK.cur = self

	local ref = self.ref

	if not self.aqua and not self.__ref_run then
		self.__ref_attrib_layer = self:get_fish_layers( "FishBaseAttr" )

		--aaa.box_debug( self:get_module_fish_dir_name() )
		if self.__force_def_file then

			self.__ref_def = aaa.obj.get_by_name( self.__force_def_file )
		else
			self.__ref_def = self:get_fish_layers( "DEF" )
		end
		--self:box_good( self.." read def" )

		self.__ref_run = self:get_fish_layers( "RUN" )
		--self:box_debug( self.__ref_def.." / "..self.__ref_run )
	--	Bind_01 = "Camera_Edit/fx" ;
	end

-- CURVE
	if not ref.curve_edit then
		local tab
		tab = {}
		-- one for all
		tab.layers	= self:get_fish_layers( "Curve_Edit" )
			tab.bdd = aaa.obj.get_down_by_class( tab.layers, "bdd_curve_3d" )
				tab.b_intercept	= param.get_ref( tab.bdd, "ui_intercept" )
				tab.dataset_id	= param.get_ref( tab.bdd, "dataset_id" )
				tab.layer = aaa.obj.get_root( tab.bdd )
					tab.active = param.get_ref( tab.layer, "active" )
		ref.curve_edit = tab
	end

-- CAUSTIC
	--todoaqua refine
	local b_caustic
	if self.aqua then
		MEDIA.set_dir_media( "Aqua/ACD" )
		b_caustic = true
	else
		app:set_media_dir()
		b_caustic = app.is_caustic and app:is_caustic()
	end

	if b_caustic then
		local name = aaa.pc.is_dev() and "Maa_256" or "Maa_1024"
		name = "caustic/"..name.."/caustic_"
		local img_seq 
		if app.use_compressed_texture and app:use_compressed_texture() then
			self:print( "Trying to load DDS caustics" )
			img_seq = IMG_SEQ:create( "caustic", name, "dds", 1, 60 )
			if not self.caustic then
				self:print( "Failed to load DDS caustics" )
			end
		end
		if not img_seq or img_seq:get_nb() == 0 then
			self:print( "Trying to load TGA caustics" )
			img_seq = IMG_SEQ:create( "caustic", name, "tga", 1, 60 )
		end
		self.caustic = IMG_SEQ_PLAYER:create( "caustic", img_seq )
		--self:box_debug( "Caustic "..self.caustic )
	end
	
	-- CREATURES
	self:init_fish_begin()

	self:init_fish()
	self.__races:set_fog( true )
	self.b_loaded = true
end
