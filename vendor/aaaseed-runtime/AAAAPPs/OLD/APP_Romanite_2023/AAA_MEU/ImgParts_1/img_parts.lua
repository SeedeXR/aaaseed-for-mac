

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = 1
	local SYT = 3.2
	local DY = .2

	local build = self.building
	local recit = self.recit

	local meu_proto = MEU_CTX.cur:get_proto_from_type( "ImgPartComp" )

	--self:set_tab_key_def()
	self:add_camera()
	self:add_rendering( 	{nil,nil,	8,2*SY}	)

	self:add_button(		{ix,iy,		3,SY},	"Time",				self, "b_time", 		true	) 
	self:add_slider(		{ix+3,iy,	4,SY},	"Time Factor",		self, "time_factor", 	1,	0,16	) 
	iy = iy + SY + DY
 
	self:add_button(		{ix,iy,		4,SY},	"Building", 		build, "b_draw", 	true	)
	self:add_button(		{ix+4,iy,	4,SY},	"Recit",			recit, "b_draw", 	true	)
	iy = iy + SY
	self:add_button(		{ix,iy,		4,SY},	"Building verbose", build, "b_verbose", false	):set_text( "Verbose" )
	self:add_button(		{ix+4,iy,	4,SY},	"Recit verbose",	recit, "b_verbose", false	):set_text( "Verbose" )
	iy = iy + SY
	self:add_button(		{ix,iy,		4,SY},	"Rect Draw",		self, "b_building_rect_draw", 	false	)
	self:add_trig_method(	{ix+4,iy,	4,SY},	"Restart Recits",	self, "restart_recit"	):set_color_back( "restart" )
	iy = iy + SY

	self:add_slider(		{ix,iy,		4,SY},	"Building particle by Unit", build, "nb_by_unit", 	140, 1,512	)
		:set_text( "by unit")
		:set_min_max_strict( true )
	self:add_slider(		{ix+4,iy,	4,SY},	"Recit particle by Unit",    recit, "nb_by_unit", 	140, 1,512	)
		:set_text( "by unit")
		:set_min_max_strict( true )
	iy = iy + SY
	ui.bu_total =
	self:add_text_info(		{ix+DY,iy,	8-DY*2,SY},	"Nb Total" )
		:set_text( "No" )
	iy = iy + SY + DY

	self:add_trig_method(	{ix,iy,		3,SY},	"Defs",		self, "edit_defs"	):set_color_back( "load" )
	ui.bu_info_bta =
	self:add_text_info(		{ix+3+DY,iy,5-DY*2,SY},"info building" )
		:set_text( "B+T=A" )
	iy = iy + SY
	self:add_trig_method(	{ix,iy,		2,SY},	"Random",	self, "random"		):set_color_back( "Reset"   )
	self:add_trig_method(	{ix+2,iy,	2,SY},	"Load",		self, "load_media"	):set_color_back( "load"    )
	self:add_trig_method(	{ix,iy+SY,	2,SY},	"Restart",	self, "restart_all"	):set_color_back( "restart" )
	self:add_trig_method(	{ix+2,iy+SY,2,SY},	"Reset",	self, "reset_all"	):set_color_back( "restart" )

	self:add_trig_method(	{ix+4,iy,	1,SY},	"-",		self, "inc_elt", -1	):set_color_back( "Reset"   )
		:set_text_inside( true )
	self:add_trig_method(	{ix+7,iy,	1,SY},	"+",		self, "inc_elt", 1	):set_color_back( "Reset"   )
		:set_text_inside( true )
	self:add_slider(		{ix+5,iy,	2,SY},	"Elt Nb",	self, "elt_nb_asked",	32,	0,50 )
		:set_min_max_strict( true )
		:set_value_type_integer( true )

	self:add_button(		{ix+4,iy+SY,2,SY},	"Renew",		self, "b_renew",	false	) --:set_color_back( "Reset"   )
	self:add_slider(		{ix+6,iy+SY,2,SY},	"Tempo",		self, "renew_tempo", 2, .5,4	)--:set_color_back( "Reset"   )

	iy = iy + SY*2 + DY

	self:add_button(		{ix,iy,		2,SY},	"One",	 	build, "b_one", 		false	)
	self:add_slider(		{ix+2,iy,	2,SY},	"Tested",	build, "b_one_index", 	1,	1,build.meu_nb	)
		:set_min_max_strict( true )
		:set_value_type_integer( true )	
	self:add_button(		{ix+4,iy,	2,SY},	"Center",	build, "b_center", 		false	)
	self:add_slider(		{ix+6,iy,	2,SY},	"Ph",		build, "ph", 			0,	0,1	)
--		:set_min_max_strict( true )
	iy = iy + SY
	self:add_button(		{ix,iy,		2,SY},	"Print",	build, "b_print", 		false	):set_color_back( "load" )
	self:add_trig_method(	{ix+2,iy,	2,SY},	"Focus",	self, "focus_building"	)
	self:add_slider(		{ix+4,iy,	2,SY},	"Rot",		build, "rot_y", 		0,	-45,45	)
	iy = iy + SY + DY

	self:set_tab_key( "Main" )
	self:add_button(		{ix,iy,		2,SY},	"Pos",		self, "b_pos_force", 	true	)
--	iy = iy + SY + DY

	self:end_bu_group()

	self:add_slider_two(	{ix+3,iy,	5,SY},	"Range",	self, "range_min", "range_max",	1,build.meu_nb, 1,build.meu_nb	)
		:set_min_max_strict( true )
		:set_value_type_integer( true )
	-- global factor for the angle random generation
	-- self:add_slider(		{ix+4,iy,	4,SY},	"Angle",	self, "rot_y",		22, 0,45	)
	-- 	:set_min_max_strict( true )
	iy = iy + SY

	ix,iy = 9,2.5
	ix,iy = meu_proto.define_physic( self, ix,iy, 1,3.2 )

	ix,iy = 9,2+DY

	local SY = .8
	self:set_tab_key( "Recit" )
	local slots = self.slots

	for i=1,3 do
		self:begin_bu_group( "Recit "..i )
		local slot = slots[i]

		self:add_selector(		{ix,iy,		8,SY},	"sel"	)
			:set_nb(15)
			:set_value(i)
			:set_target( slot, "s_sel" )
		iy = iy + SY
		self:add_slider(		{ix,iy,		4,SY},	"Recit "..i,	slot, "alpha",			0, 0,1 )
		self:add_trig_method(	{ix+4,iy,	4,SY},	"Focus",		self, "focus_recit",	i )
			:set_color_back( "focus" )
		iy = iy + SY
		self:add_button(		{ix,iy,		2,SY},	"Auto",			slot, "b_auto",			false )
		self:add_trig_method(	{ix+2,iy,	3,SY},	"Begin",		self, "begin_recit",	i ):set_color_back( "green" )
		self:add_trig_method(	{ix+5,iy,	3,SY},	"End",			self, "end_recit",		i ):set_color_back( "red" )		iy = iy + SY
		self:add_slider(		{ix,iy,		4,SY},	"Video "..i,	slot, "video_alpha",	0, 0,1 )
		slot.bu_info =
			self:add_text_info(	{ix+4,iy,	4,SY},	"Info "..i		)--:set_text_align_x( "center" )

		iy = iy + SY

		bu = self:add_slider(	{ix+2,iy, 	6,SY},	"Interpolate",	slot, "inter",			0, -1,2	)
		iy = iy + SY + DY

		self:end_bu_group()
	end
end
function meu:init()
	self.time = 0
	self.building = { defs={}, meu_nb=50 }
	self.recit =	{
						defs={}, meu_nb=15, time={}, video={}, used={}, free={},
						away_min=5, away_max=8,
						away_min_busy=9, away_max_busy=12
					}
	self.slots = { {}, {}, {} }
	self.recit_order = { 1,15,2,10,4, 11,13,5,3,9, 6,8,14,12,7 }
	self.recit_order_index = 1
	self.render_count = 0
end
-- EDIT
--
function meu:edit_text( name, fname )
	self:print( "Edit Nimes "..name.." file "..fname )
	aaa.os.open_editor( fname )
end
function meu:edit_defs()	self:edit_text( "definitions", app:get_dir_absolute().."Romanite/romanite.lua" )		end

function meu:get_recit_elt_by_slot( i )
	--aaa.print_fn()
	local s_sel = self.slots[i].s_sel
	--self:print_recit( "s_sel is "..s_sel )
	--table.print( self.recit, "self.recit", 2 )
	local elts = self.recit.elts
	return elts and elts[s_sel] or nil
end
function meu:focus_recit( i )
	self:get_recit_elt_by_slot(i).meu:set_ui_slot( 4 )
end
function meu:print_building( str )	if self.building.b_verbose	then self:print( "BUILDING : "..str )	end	end
function meu:print_recit( str, i )
	if self.recit.b_verbose	then
		if i then	self:print( "RECIT slot "..i.." : "..str )
		else		self:print( "RECIT : "..str )
		end
	end
end


-- VIDEO
--
function meu:get_recit_index_next()
	local id  = self.recit_order_index + 1
	if id > 15 then id = 1 end
	self.recit_order_index = id
	return self.recit_order[id]
end
function meu:change_recit( i )
	local slot = self.slots[i]
	slot.s_sel = self:get_recit_index_next()
	self:print_recit( "change to recit "..slot.s_sel, i  )

	local elt = self:get_recit_elt_by_slot(i)
	if elt then
		--table.print( elt, "elt", 1 )
		local def = self:get_def_from_elt(elt)
		--table.print( def, "def", 1 )
		slot.vname = def.vname
	end
end
function meu:__do_recit_video( i, time )
	--aaa.print_fn()
	local slot = self.slots[i]
	local mv = self.meu_video[i] 

	if time == nil then -- end_recit
		if self.recit.b_verbose then
			self:print( "RECIT :")
		end
		if slot.vname then	-- avoid wrong send at start
			app:send_osc( "romanite/video", slot.vname, i-2, "end" )
		end
		self.meu_pip[i]:set_mu_value(0)
		self:print_recit( "end", i )
		self:set_recit_state( i, "away" )
	else -- begin_recit
		local elt = self:get_recit_elt_by_slot(i)
		--table.print( elt, "elt", 1 )
		local def = self:get_def_from_elt(elt)
		--table.print( def, "def", 1 )
		slot.vname = def.vname
		--self:print( mv.." will set index to "..def.vid-1 )
		mv:set_bind_only_video( def.vid-1 )
		self.meu_pip[i]:set_mu_value(1)
		app:send_osc( "romanite/circle", slot.vname, i-2, 0.5 )
		self:print_recit( "begin", i )
		self:set_recit_state( i, "play" )
	end

	slot.b_video_playing = false
	mv:restart()
	mv:stop()
	slot.time = time
end

function meu:begin_recit( i )	self:__do_recit_video( i, 0 )	end
function meu:end_recit( i )		self:__do_recit_video( i, nil )	end

function meu:set_recit_away_min_max( min, max, min_busy, max_busy )
	local recit = self.recit
	recit.away_min = min
	recit.away_max = max
	recit.away_min_busy = min_busy
	recit.away_max_busy = max_busy	
end

function meu:set_recit_state( slot_index, what )
	local recit = self.recit
	local slot = self.slots[slot_index]
	slot.state = what
	if what=="away" then
		local min,max
		local meu_exp = self.meu_exploitation
		local accum = meu_exp and meu_exp:get_circle_accum(slot_index) or 0
		if accum < .2 then	min,max = recit.away_min,      recit.away_max
		else				min,max = recit.away_min_busy, recit.away_max_busy
		end
		math.randomseed( slot_index * 1243 + self.time )
		slot.tempo = math.random_real( min, max )
		--self:print( "set away for "..slot.tempo.." from ["..min..","..max.."]" )
		self:change_recit(slot_index)
		
		slot.phase = 0
	elseif what=="appear" or what=="vanish" then
		slot.tempo = 1
		--self:box_debug( "Before "..slot.vname )
		app:send_osc( "romanite/recit", slot.vname, slot_index-2, what=="appear" and 1 or 0 )
		--self:box_debug( "After "..slot.vname )
	elseif what=="interact" then
		slot.tempo = 12
		self.meu_exploitation:set_circle_accum( slot_index, 0 )
	elseif what=="play" then
		slot.phase = 0
		slot.tempo = 100.
	end
end 

function meu:restart_recit()
	local recit = self.recit
	recit.used = {}
	recit.free = {}
	-- for i,elt in IPAIRS(self.recit) then
	-- 	table.insert( recit.free, elt )
	-- end
	-- table.print( recit.free, "recit.free", 1 
	for i=1,3 do
		self:set_recit_state( i, "away" )
	end
end
function meu:assign_recit_to_slot( i )
	local recit = self.recit
	local i = math.random(3)
	local elt = table.remove( table.free, i )
	table.insert( table.used, elt )
	slot.recit = elt
end
function meu:release_recit_from_slot( i )
	local recit = self.recit
	table.insert( table.free, slot.recit )
	slot.recit = nil
end

-- INIT MEUS (the MEUs used by buildings or recit)
--
function meu:__init_meus( pre, nb )
	--aaa.print_fn()
	local meus = {}
	for i=1,nb do
		local name = pre..i
		local dst = self:get_meu_by_name( name )
		self:print( name.." -> meu "..dst )
		meus[i] = dst
		--dst:set_bu_value( "how", 3 )
		dst:set_bu_value( "decomposition.auto", false )
		dst:set_bu_value( "render.back",		0 )
		dst:set_bu_value( "render.cull", 		1 )
		dst:set_bu_value( "render.front", 		0 )
		dst:set_bu_value( "render.depth", 		1 )
		dst:set_bu_value( "noise",				0 )
		param.set( dst.ref.point_instance_nb, 2 )
	end
	--table.print( meus, "after __init_meus()", 1 )
	return meus
end
function meu:init_meus_begin()
	self.building_meus = self:__init_meus( "ImgPartComp_",  self.building.meu_nb )
	self.recit_meus    = self:__init_meus( "ImgPartComp_R", self.recit.meu_nb    )
	-- we use one shader for building and recit, could be 2 at some point (building and recit)
	self.meu_sha = self.building_meus[1]

	local post = { "A", "B", "C" }
	self.meu_video = {}
	self.meu_pip = {}
	self.meu_timer_frag =  app:get_meu_by_name( "TimerFrag_1" )

	for i=1,3 do
		local slot = self.slots[i]		
		self.meu_video[i]      = app:get_meu_by_name( "Video_"..post[i] )
		self.meu_pip[i]        = app:get_meu_by_name( "PIP_"..i )
		self:end_recit(i)
		local mv = self.meu_video[i] 
		mv:restart()
		mv:start()
	end

	self.meu_exploitation =  self:get_meu_by_name( "exploitation_1" )
end
function meu:init_meus_end()
	for i=1,3 do
		local mv = self.meu_video[i] 
		mv:restart()
		mv:stop()
	end
end
function meu:make_elts( defs, b_building )
	local elts = {}
	local count = 0
	local function add( def_index, name )
		count = count + 1
		local elt = { def_index=def_index, name=name, meu_index=count, b_building=b_building }
		elt.rndseed = 4212487 * count
		table.insert( elts, elt )
	end
	--table.print( defs, "make_elts() "..(b_building and "building" or "recit"), 2 )
	for i,def in ipairs(defs) do
		local nb = def.inst_nb
		if nb then	for j = 1,nb do	add( i, def.name..j ) end
		else						add( i, def.name )
		end
	end
	--table.print( elts, b_building and "building" or "recit", 2 )
	return elts
end

function meu:__load_media( elts, meus, bind_depth_def )
	aaa.print_fn()
	--table.print( meus, "ImgPart:__load_media() meus" )
	for i,elt in ipairs(elts) do
		local def = self:get_def_from_elt( elt )
		self:print( elt.meu_index )	

		elt.meu = meus[elt.meu_index]
		local mu = elt.meu:get_mu()
		mu:set_text( elt.name )

		local fname = def.fname
		local b_table = type(fname)=="table"
		local path = b_table and fname[1] or fname
		local name = elt.name
		self:print( "Dealing with "..name )

		local function get_binds( path, b_first )
			local path_rgb = path..".tga"
			local bind = IMGS.get_bind( path_rgb )
			self:print( path_rgb.." bind is "..bind )
			if b_first then
				elt.psx,elt.psy = aaa.img.get_size( bind )
			end
			local path_d = path.."_D.tga"
			local bind_depth = IMGS.get_bind( path_d )
			bind_depth = bind_depth or bind_depth_def
			return bind, bind_depth
		end

		elt.bind, elt.bind_depth = get_binds( path, true )
		if b_table then	elt.bind_bis, elt.bind_depth_bis = get_binds( fname[2], false )
		else			elt.bind_bis, elt.bind_depth_bis = nil, nil
		end
	end
end

function meu:load_media()
	local build = self.building
	app:make_building_defs( build )
	build.elts = self:make_elts( build.defs, true )
	MEDIA.set_dir_media( "Nimes_2023/Nimes_Analogue" )
	local bind_depth_def = IMGS.get_bind( "ARENE_D.tga" )
	self:__load_media( build.elts, self.building_meus, bind_depth_def )

	local recit = self.recit
	app:make_recit_defs( recit )
	recit.elts = self:make_elts( recit.defs, false )
--	table.print( self.recit.elts, "self.recit.elts", 2 )
--	self:box_debug( "check 1" )
	MEDIA.set_dir_media( "Nimes_2023/Recits_3" )
	self:__load_media( recit.elts, self.recit_meus, bind_depth_def )

	self.recit_mask = IMG_SEQ:create( "recit_mask", "Mask/MASQUE_", "tga", 0,848, 1 )

	self.recit_timer = IMG_SEQ:create( "recit_timer", "ROND_ANIM/ROND_ANIM_", "tga", 0,299, 1 )

	self:random_all()
	self:restart_recit()

	--table.print( self.recit.elts, "self.recit.elts", 2 )
	--self:box_debug( "check 2" )
end

-- because we can redefine defs on the fly
function meu:get_def_from_elt( elt )
	if elt then
		local defs = elt.b_building and self.building.defs or self.recit.defs
		return defs[elt.def_index]
	else
		aaa.debug.print_traceback( "nil elt in get_def_from_elt()" )
		aaa.debug.debug()
	end
end

--local x_offset = { -.06, 0, -.08}
local x_offset = { 0, 0, 0 }
function meu:draw_loading( x,y,z, displace, rot_y, slot, slot_index, phase )

	if phase <= 0 then
		return
	end
	
	self.meu_timer_frag:set_bu_value( "phase", phase )

	local ph_circle = (slot.ph_circle or 0) + self.dt * 60/300
	slot.ph_circle = ph_circle
	local bind = self.recit_timer:get_bind_in_cycle( ph_circle )
	self.meu_timer_frag:set_texture_bind_2d( 1, bind )

	gol.push_matrix()
		gol.translate( x+x_offset[slot_index], y, z+displace * .75 )
		gol.rotate_y( rot_y )
		gol.scale( .60 )

		--elf.meu_timer_frag:render( (slot.state == "vanish") and slot.ph_appear or 1	 )
		self.meu_timer_frag:render( slot.alpha )
	gol.pop_matrix()

end
--todo change less stuff when ready
function meu:draw_elt( elt, b_center, slot_index, ph, alpha )
	--aaa.print_fn()
	if not elt then
		aaa.debug.print_traceback( "nil elt in draw_elt()" )
--		aaa.debug.debug()
		return
	end

	-- in case 
	if not elt.x then
		self:random_elt( elt )
	end

	local what = elt.b_building and self.building or self.recit
	local def = self:get_def_from_elt( elt )
	local meu_dst = elt.meu
--	self:print( elt.name.." : "..meu_dst )
--	self:print( elt.bind )

-- get size
	local sy = elt.b_building and elt.sy or def.sy
--	aaa.show( sy, "sy" )
	local sx = (elt.b_building and elt and elt.sx) or sy
--	aaa.show( sx, "sx" )

	local x,y,z
	if elt.b_building then
		if b_center then
			x,z = 0,0
		else
			x,z = elt.x,elt.z
		end
		y = sy / 2
	else
		x = (slot_index-2) * 1.95
		y = 1.26
		z = 3.

		--x = x + def.ox * (slot_index-2)
		--aaa.show( def.ox, "ox"..slot_index )
		--aaa.show( x, "o"..slot_index )
		x = x + (slot_index-2) * def.ox
		y = y + sy * .0 + (def.oy or 0 )
	end
-- compute particle_nb and accumulate
	local z_max = -15
	local z_min = -2
	local inter = clamp_01( (z-z_min)/(z_max-z_min) ) 
	inter = math.pow( inter, .8 )
	local nb_by_unit = def.nb_by_unit or ( what.nb_by_unit * interpolate( 1, 1/4, inter ) )

	--self:print( "nb_by_unit "..nb_by_unit )
	local nb_u = math.floor( sx * nb_by_unit )
	local nb_v = math.floor( sy * nb_by_unit )
	local nb = nb_u*nb_v
	self.particle_nb = self.particle_nb + nb
	
--	param.set( meu_dst.ref.point_nb, nb )			
	meu_dst:set_bu_value( "nb u", nb_u )
	meu_dst:set_bu_value( "nb v", nb_v )
	
	--local inter = math.fn_linear( ph,1, 0,0, .5,0, .75,.7, 1,0 )
	--local inter = elt.ph_out or ph

--	meu_dst:set_bu_value( "-", -1 )
--	meu_dst:set_bu_value( "interpolate", 0 )
	meu_dst:set_bu_value( "size_x", sx )
	meu_dst:set_bu_value( "size_y", sy )
	meu_dst:set_bu_value( "size_z", 1 )

	meu_dst:cpy_physic_from( self, elt.b_building )
-- DRAW BUILDING
	if elt.b_building then

--		aaa.show( elt.ph_in, "elt.ph_in" )
--		aaa.show( elt.ph_out, "elt.ph_out" )
		local dr = .2
		if not ph then
			--meu_dst:set_bu_value( "how",	self.phy.s_how	)
			ph = elt.ph_out
			if ph then
				--self:print( "Out" )
				meu_dst:set_bu_value( "how", 3	)
				dr = .05 + ph * .4
				ph = ph * 1.5
			else
				ph = elt.ph_in
				if ph then
					--self:print( "IN" )
					--aaa.show( elt.ph_in, "elt.ph_in" )
					meu_dst:set_bu_value( "how", 2	)
					ph = (1. - math.sin( (1-ph) * math.pi*.5 )) * 2
					--aaa.show( ph, "ph deduiced" )
				end
				
			end
		end

		meu_dst:set_bu_value( "decomposition.interpolate", ph or 0 )
		--if not ph then
			meu_dst:set_bu_value( "decomposition.range", dr )
			meu_dst:set_bu_value( "target.interpolate", elt.ph_out and elt.ph_out * 2 or 0  )
		--end

		if def.displace_gamma	then
			meu_dst:set_bu_value( "displace gamma", def.displace_gamma )
		end
		if def.displace 		then
			meu_dst:set_bu_value( "displace",		 def.displace		)
		end

		local rot_y
		if b_center then
			rot_y =  what.rot_y / 360
		else
			rot_y = elt.rot_y
		end

		if self.b_pos_force then
			--local nb =  what.meu_nb
			meu_dst:set_bu_value( "pos_x", x )
			meu_dst:set_bu_value( "pos_y", y )
			meu_dst:set_bu_value( "pos_z", z )
	--		meu_dst:set_bu_value( "pos_z", i <= 16 and -1 or 0 )
			meu_dst:set_bu_value( "rot_y", rot_y )

			meu_dst:set_bu_value( "target_x", elt.target_x )
			meu_dst:set_bu_value( "target_y", elt.target_y * 0 ) 
			meu_dst:set_bu_value( "target_z", elt.target_z )
		end

	--	meu_dst:set_bu_value( "displace gamma", .717 )
		if self.b_building_rect_draw then
			local r = elt.rect
			if r then
				if false then
					x = meu_dst:get_bu_value( "pos_x" )
					z = meu_dst:get_bu_value( "pos_z" )
					rot_y = meu_dst:get_bu_value( "rot_y" )
				else
					x,z,rot_y = r[1],r[2],r[5]
				end
				local id = gol.get_program_used()
				gol.use_program()
				--gol.push_attrib()
				gol.set_texture_dim(0)

				gol.color_white()
					aaa.draw_rect_line_axe_y( x,0,z,  r[4],r[3], -rot_y )
				-- r = elt.rect_a
				-- gol.color_white(.5)
				-- aaa.draw_rect_line_axe_y( r[1],0,r[2],  r[4],r[3], r[5] )
				--gol.pop_attrib()
				gol.use_program( id )
			end
		end

	else
-- DRAW RECIT
		local slot = self.slots[slot_index]
		
		--if slot_index  == 1 then
		--	table.print( def )
		--	table.print( self.recit_def[1] )
		--end
		--aaa.show( def.sy, "def.sy" )
		meu_dst:set_bu_value( "how", 4 )

		local r = -.1
		local dr = math.sin( self.time * 1 + slot_index * 2 ) * .03
		local rot_y = ((slot_index-2)*r + dr * (def.dr or 1)) * .5

		local b_dis = meu_dst:get_bu_value( "displace active" )
		local displace = b_dis and meu_dst:get_bu_value("displace") or 0

		meu_dst:set_bu_value( "rot_y", rot_y )

		meu_dst:set_bu_value( "pos_x", x )
		meu_dst:set_bu_value( "pos_y", y )
		meu_dst:set_bu_value( "pos_z", z )

		self:draw_loading( x,y,z, displace, rot_y, slot, slot_index, slot.inter )

		self:draw_layer(1)
		local ph_fumee = (slot.ph_fumee or 0) + self.dt * 30/850
		slot.ph_fumee = ph_fumee

		local bind = self.recit_mask:get_bind_in_cycle( ph_fumee )
		gol.set_tex_unit_cur( 3 )
		gol.bind_texture( bind )
		gol.set_tex_unit_cur( 0 )
		--gol.set_tex_unit_2d_bind( 0, bind )
		gol.bind_texture( bind )

		gol.set_quad_uv()
		--gol.set_texture_dim(2)
		gol.color( 1,1,1, alpha )
		--local xf = x * 1.06

	--	gol.color_white()
		aaa.obj.update_then_draw( self.layer_shading )

		local s = .2
		local r = sy/sx
		self.part_sha:set_comp_vec4( 4, r * s, s )

		meu_dst:set_bu_value( "decomposition.interpolate", slot.inter )
		--meu_dst:set_bu_value( "target.interpolate",  slot.inter + slot.inter_bis )
	-- todo optimize by doing at start and not every frame
		--meu_dst:set_bu_value( "displace gamma", 1 )

		meu_dst:set_bu_value( "recit appear", slot.ph_appear )
		meu_dst:set_bu_value( "target_x", slot.size_recit )
	--	meu_dst:set_bu_value( "target_y", .125 )
	--	meu_dst:set_bu_value( "target_z", 0 )
	end

	--meu_dst:set_mu_value( 1 )
--was
--[[
	if inside( ph, .74, .75 ) and not elt.time_protected then
		--todo
		self:random_elt( elt )
		elt.time_protected = 1.
		--self:print( "reset "..i )				
	else
		local t = elt.time_protected
		if t then
			t = t - self.dt
			if t < 0. then t = nil end
			elt.time_protected = t
		end
	end
--]]	

	local bind, bind_depth
	bind = elt.bind
	bind_depth = elt.bind_depth

	--todo only when changed 
	meu_dst:set_texture_bind_2d( 1, bind )
	meu_dst:set_texture_bind_2d( 2, bind_depth )
	meu_dst:render_min( self.part_sha, true )

	if elt.bind_bis then
		--self:print( elt.name.." -> "..elt.bind_bis.." "..elt.bind_depth_bis )
		meu_dst:set_texture_bind_2d( 1, elt.bind_bis )
		meu_dst:set_texture_bind_2d( 2, elt.bind_depth_bis )
		meu_dst:render_min( self.part_sha, false )

		meu_dst:set_texture_bind_2d( 1, bind )
		meu_dst:set_texture_bind_2d( 2, bind_depth )
	end

end

function meu:get_elt_debug()
	local build = self.building
	local index = build.b_one_index
	local elt = build.elts[index]
	return elt, index
end
function meu:focus_building()
	local elt = self:get_elt_debug()
	elt.meu:set_ui_slot( 4 )
end

function meu:draw_recit()
	local recit = self.recit
	if not recit.b_draw then return end

	app:make_recit_defs( recit )

	local meu_exp = self.meu_exploitation
	local dt = self.dt
	for i=1,3 do
		local slot = self.slots[i]
		local state = slot.state
		slot.size_recit = 1

		local accum = meu_exp:get_circle_accum(i)
		slot.accum = accum
		if state == "interact" and slot.b_auto then
			if accum > 0 then
				slot.change_time = 0
				if accum >= 1 then
					self:begin_recit(i)
					state = slot.state
				end
			end
		end
		
		if state~="play" then
			slot.inter = 0
			slot.inter_bis = 0
			local t = slot.change_time or 0
			if slot.b_auto then
				t = t + dt
			end
			if		state=="away"		then 	slot.alpha = 0
			elseif	state=="appear" 	then	slot.alpha = 1
												slot.ph_appear = (t or 0) / slot.tempo
			elseif	state=="interact"	then	slot.alpha = 1
												slot.inter = accum
			elseif	state=="vanish"		then	slot.alpha = 1
												slot.ph_appear = 1 - (t or 0) / slot.tempo
			end
			if t >= slot.tempo then
				local next
				if		state=="away"		then 	next = "appear"
				elseif	state=="appear" 	then	next = "interact"
				elseif	state=="interact"	then
					-- if there is a little bit of somebody we keep presenting the object recit
					if accum <= 0 then
						next = "vanish"
					end
				elseif	state=="vanish"		then	next = "away"
				end
				if next then
					self:set_recit_state( i, next )
					t = nil
				end
			end
			slot.change_time = t
		else
			local t = slot.time
			if t then
				local dur = .6		
				t = t + dt
				slot.size_recit = clamp_01( 1 - t/dur )
				if t > .5 then
					slot.alpha = clamp_01( slot.alpha - dt )
				end
				--slot.alpha = 1
				--self:print( i .. " " ..t )
				if t > .01 then
					local mv = self.meu_video[i]
					if not slot.b_video_playing then			
						mv:start()
						slot.b_video_playing = true
						--table.print( def, "def", 1 )
						app:send_osc( "romanite/video", slot.vname, i-2, "begin" )			
					else			
						local duration = mv:get_duration()
						local time = mv:get_time()
						if time >= duration then
							self:end_recit(i)
						end
					end	
				end
				slot.time = t
				slot.inter_bis = clamp_01( t*.2 )
			end


		end
		
		local state = slot.state
		if state~="away" then
			--table.print( t, "Recit "..i )
			if slot.alpha > 0 then
				--aaa.show( t.s_sel, "sel "..i )
				self:draw_elt( self:get_recit_elt_by_slot(i), false, i, 0, slot.alpha )
			end
		end

		--self.meu_pip[i]:set_mu_value( slot.video_alpha )
	end
end

function meu:draw_building()
	local build = self.building
	if not build.b_draw then return end

	app:make_building_defs( build )

	aaa.obj.update_then_draw( self.layer_shading )
	if build.b_one then
-- Draw only one building
		local elt, index = self:get_elt_debug()
		--self:print( self.building_tested.." "..elt )
		if build.b_print then
			table.print( elt, "elt_building["..index.."]", 1 )
		end
		self:draw_elt( elt, build.b_center, nil, build.ph )
	else
-- Draw all buildings
		self:update_building()
		local elts = build.used
		--local nb_i = self.range_max - self.range_min + 1
		table.sort( elts, function(a,b) return a.z < b.z end )
		if build.b_print then
			table.print( elts, "build.used", 2 )
		end
		for i,elt in ipairs( elts ) do
	--	for i,elt in pairs_sorted( elts, function(a,b) return a.z < b.z end ) do
			local index = elt.meu_index
			if index and inside( index, self.range_min, self.range_max ) then
				--local fi = (nb_i <= 1) and 0 or (index-1)/nb_i
				--local ph = fi + self.time / 30
				--local inter = triangle_01( ph )
				--ph = wrap_01(ph)
				self:draw_elt( elt, false )
			else
				--meu_dst:set_mu_value( 0 )
				--mu:set_render_skip_next( true )
			end
		end
	end
end

function meu:draw_building_and_recit()
	if not self.recit_meus then
		self:print( "No recit_meus" )
		return
	end
	local recit = self.recit
	if not recit.elts then
		self:load_media()
	end

	self.particle_nb = 0
	self.field_bind = self:get_texture_bind_2d( 2 )

	self.part_sha = self.meu_sha:get_shading()
	self.layer_shading = self.meu_sha.ref.layer_shading

	self:draw_building()
	self:draw_recit()
end


-- RESET
--
function meu:reset_elt( elt )		elt.meu:reset_particle()	end
function meu:reset_elts( elts )		for _,elt in pairs( elts ) do self:reset_elt( elt ) end		end
function meu:reset_all()
	self:reset_elts( self.building.elts )
	self:reset_elts( self.recit.elts ) 
end

-- RANDOM
--
function meu:get_rnd(nb)
	math.randomseed( self.time * 156847.445477 + nb * 16515 + math.random( 48643518 ))
	return  math.random( nb )
end

function meu:random_elt( elt )
	--self:print( "call random_elt for "..elt.name )
	if not elt then return false end

	self:reinit_elt( elt )

	local cam_d = 11.28	--todo get the real one
	local x_max_at_0 = 4.5

	math.randomseed( elt.rndseed )
	elt.rndseed = elt.rndseed + 1647
	local def = self:get_def_from_elt( elt )
	local meu_dst = elt.meu
	local sz
	if meu_dst:get_bu_value( "displace active" ) then
		sz = def.displace or meu_dst:get_bu_value( "displace" )
	else
		sz = .02
	end

	meu_dst:set_bu_value( "field.type", def.field_type or (def.b_tree and 0 or 1) ) 

	local try_count = 0
	local x,z, sx,sy, rot_y
	local rect

	local b_location_ok
	repeat 
		sy = def.sy
		if type(sy)=="table" then
			sy = interpolate( sy[1],sy[2], math.random() )
		end

		sx = sy * ((elt and elt.psx and elt.psy ) and elt.psx/elt.psy or 1)

		rot_y = (math.random()-.5) * (def.rot_y or 45) / 180

		z = def.z_range
		if z then
			local inter = math.random()
			z = interpolate( z[1],z[2], inter )
		else 
			z = 1. -- + math.random() * 5.
		end
		z = -z
		
		local f = (def.x_max or x_max_at_0) * (cam_d-z)/cam_d + sx * math.cos( rot_y * math.pi2 ) * .4
		x = (math.random()-.5) * 2 * f

		local cos,sin = math.cos(rot_y * math.pi2), math.sin(rot_y * math.pi2)
	--	elt.rect_a = { x,z, sx,sz, rot_y }

		-- change sign of rot because SAT was done for xy plane, and doing xz (and not zx) implie to reverse rotation (done also in)
		rect = { x + sin * sz/2, z + cos * sz/2, sx, sz, -rot_y }
		rect.rad = math.sqrt( (sx*sx + sz*sz) * .25 )

		b_location_ok = true
		local a = rect
		for _,elt_b in IPAIRS(self.building.used) do
			if elt_b ~= elt then
				local b = elt_b.rect
				local l2 = a.rad + b.rad
				l2 = l2 * l2
				local coor = b[1] - a[1]
				local d2 = coor * coor
				if d2 <= l2 then
					coor = b[2] - a[2]
					d2 = d2 + coor * coor
					if d2 <= l2 then
						if aaa.rect.is_intersect_SAT( a,b ) then
							b_location_ok = false
							break
						end
					end
				end
			end
		end
	
		try_count = try_count + 1
		-- if try_count > 10 then
		-- 	self:print( "try_count to BIG : "..try_count )
		-- 	break
		-- end

	until b_location_ok

	if try_count > 64 then
		aaa.print( def.name.." : "..try_count.." loops to find building location" )
	end
	
	--table.print( elt, "elt"  )
	elt.rect = rect
	elt.x,elt.z, elt.sx,elt.sy,	elt.rot_y = x,z, sx,sy, rot_y

	elt.target_x = (math.random()-.5)*10
	elt.target_z = (math.random()-.5)*10
	elt.target_y = math.random()*2	--used as scale

	return true
end
-- function meu:random_elts( elts )
-- 	for _,elt in pairs( elts ) do
-- 		self:random_elt(elt)
-- 	end
-- end
function meu:reinit_elt( elt )
	elt.ph_out=nil
	elt.ph_in=nil
end
function meu:init_elt_all()
	aaa.print_fn()
	local build = self.building
	-- the pool of free buildings
	build.free = {}
	for i,elt in ipairs(build.elts) do
		self:reinit_elt( elt )
		build.free[i] = elt
	end
	-- the pool of used buildings
	build.used = {}

	self.elt_nb = 0
--	self.elt_active_nb = 0
--	self.elt_active_nb = self.elt_active_nb + 1
	self.building_nb = 0
	self.tree_nb = 0
end
function meu:add_building()
	local build = self.building
	local nb_free = #build.free
	if nb_free > 0 then
		local index = self:get_rnd( nb_free )
		local elt = table.remove( build.free, index )
		if self:random_elt( elt ) then			
			self:reset_elt( elt )
			table.insert( build.used, elt )
			elt.ph_in = 1
			self.elt_nb = self.elt_nb + 1		
			self:update_elt_count( elt, 1 )
			return true
		else	-- put it back in this case
			table.insert( build.free, elt )
		end	
	end
	return false
end
function meu:remove_building( elt )
	local build = self.building
	local nb_used = #build.used
	if nb_used > 0 then
		array.remove_by_val( build.used, elt )
		self:reinit_elt( elt )
		--local elt = table.remove( build.used, index )
		table.insert( build.free, elt )
		self:update_elt_count( elt, -1 )
		return true
	end
	return false
end
function meu:ask_remove_building()
	if self.elt_nb > 0 then
		local build = self.building
		local nb_used = #build.used
		local elt
		-- find elt to remove
		local try = 0
		repeat
			local index = self:get_rnd( nb_used )
			--self:print( "tried "..index.." on "..nb_used.." used" )
			elt = build.used[index]
			--self:print( "ph_out "..elt.ph_out.." ph_in "..elt.ph_in )
			try = try + 1
			if try > 400 then
				self:print_error( "don't manage to find an elt to remove" )
				self:print_error( "so don't remove this frame" )
				return false
			end
		until elt.ph_out==nil and elt.ph_in==nil
		elt.ph_out = 0
		self.elt_nb = self.elt_nb - 1
		return true
	end
	return false
end
function meu:get_elt_sumup()
	local build = self.building
	local str = self.building_nb.."+"..self.tree_nb.." = "..self.elt_nb
	if build.used then
		str = str .. "/ ".. #build.used
	end
	return str
end
function meu:update_elt_count( elt, inc )
	local build = self.building
	--self.elt_nb = #build.used
	local def = self:get_def_from_elt( elt )
	if def.b_tree then	self.tree_nb 		= self.tree_nb + inc
	else				self.building_nb 	= self.building_nb + inc
	end
	--self:print( self:get_elt_sumup() )
end

function meu:adjust_elt_nb()
	while self.elt_nb_asked ~= self.elt_nb do
		local ret
		if self.elt_nb_asked <= self.elt_nb	then	ret = self:ask_remove_building()
		else										ret = self:add_building()
		end
		if not ret then break end
	end
end

function meu:update_building()
	--aaa.print_fn()
	if self.b_time then
		local dt = self.dt
		
		if self.b_renew then
			local t = (self.renew_time or 0) + dt 
			if t >= self.renew_tempo then
				t = nil
				self.b_tempo_add = not self.b_tempo_add
				local inc = self.b_tempo_add and 1 or -1
				self:inc_elt( inc )
			end
			self.renew_time = t
		end

		local build = self.building
		for i,elt in IPAIRS(build.used) do
			local ph = elt.ph_out
			if ph then
				ph = ph + dt / 6
				--aaa.show( ph, "ph_out" )
				local def = self:get_def_from_elt( elt )
				app:send_osc( "romanite/building", def.name, elt.x/7, 1-clamp_01(ph), -elt.z, elt.target_x/7, 0 )
				if ph >= 1 then
					ph = nil
					self:remove_building( elt )
				end
				elt.ph_out = ph
			else
				ph = elt.ph_in
				if ph then
					ph = ph - dt /3
					local def = self:get_def_from_elt( elt )
					app:send_osc( "romanite/building", def.name, elt.x/7, 1-clamp_01(ph), -elt.z, elt.x/7, 1 )
					if ph <= 0 then
						ph = nil
					end
					elt.ph_in = ph
				end
			end
		end
	end
	self:adjust_elt_nb()
end
function meu:inc_elt( inc )
	self.elt_nb_asked = clamp( self.elt_nb_asked + inc, 0, self.building.meu_nb )
end
function meu:random_all()
	self:init_elt_all()
	self:adjust_elt_nb()
end
function meu:random()
	local build = self.building
	if build.b_one then
		local elt, index = self:get_elt_debug()
		self:random_elt(elt)
	else
		self:random_all()
	end
end

-- RESTART
--
function meu:restart_elts( elts )
	for _,elt in pairs( elts ) do
		elt.meu:restart()
	end
end
function meu:restart_all()
	self:restart_elts( self.building.elts )
end



-- UPDATE AND DRAW
--

function meu:update_ui()
	local ui = self.ui
	if ui.bu_total then
		ui.bu_total:set_text( "Nb Particle: "..self.particle_nb )
	end
	if ui.bu_info_bta then		
	 	ui.bu_info_bta:set_text( self:get_elt_sumup() )
	end
	for i=1,3 do
		local slot = self.slots[i]
		slot.bu_info:set_text( slot.state )
	end
end

function meu:update()
	self.dt = self.b_time and (aaa.time.dt * self.time_factor) or 0
	self.time = self.time + self.dt
end

function meu:draw()
	self:draw_layers_begin()

		self:draw_layer(1)
		local render_count = self.render_count + 1
		self.render_count = render_count
		--self:box_debug( "render_count is "..render_count )
		if render_count <= 8 then
			--self:box_debug( render_count )
			-- first we init meu and start video
			--  then we stop videos
			--  then we consider init is done
			if render_count == 3 then
				self:init_meus_begin()
			elseif render_count == 8	then	
				self:init_meus_end()
			end
		else		
			self:draw_building_and_recit()
		end

	self:draw_layers_end()
end
