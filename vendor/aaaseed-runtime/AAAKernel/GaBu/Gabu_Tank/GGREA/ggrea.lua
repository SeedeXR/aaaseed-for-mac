--
--	GGREA
--		centralize the GREAs
--
if CLASS.DECLARE( "GGREA" ) then
end

function GGREA:erase_curve_rt()
	local greas = self.__greas
	for i=1,self.__grea_nb do
		greas[i]:erase_curve_rt()
	end
end
--todomona free
function GGREA:get_curve_rt_layer_and_bdd()
	--todo get_curve_rt_layer_and_bdd() clone an object so we recreate every time we RUN.lua (2023 Jan)
	local bdd = aaa.obj.clone( self.curve_rt_bdd_src )
	return self.curve_rt_layer_bdd, bdd
end

function GGREA:init()
	local tank = TANK.cur
	--todomona perhaps it is time to separate or not
	--	1/keep in CLASS but kjust have do_on_race
	--	2/ one by fish tank

	self.boid_attr_layers		=	tank:get_fish_layers( "Boid_Attr"		)
	--	todo not sure this is used anymore
	GGREA.boid_out_attr_layers	=	tank:get_fish_layers( "Boid_Out_Attr"	)


	if not GREA.curve_rt_layer_bdd then
		-- one bdd in a layer for each boid see get_curve_rt_layer_and_bdd()
		local layers = tank:get_fish_layers( "Curve_RealTime" )
			local attr_layer	= aaa.layers.get_layer( layers, 1 )
				GGREA.curve_rt_attr_layer = attr_layer
				aaa.obj.update_then_draw( attr_layer )	-- we do this to be sure to get the bdd

				GGREA.curve_rt_bdd_src 	= aaa.layer.get_bdd( attr_layer )
			GGREA.curve_rt_layer_bdd	= aaa.layers.get_layer( layers, 2 )
			GGREA.curve_rt_color_ref	= aaa.layer.create_color( GGREA.curve_rt_layer_bdd, "curve_rt" )
			--table.print( GGREA.curve_rt_color_ref, "GGREA.curve_rt_color_ref" )
	end

	self.crea_nb		= 0
	self.__grea_nb		= 0
	self.__greas		= {}
	self.__grea_by_name	= {}
	self.ref = {}

	local tank = TANK.cur
	local boid_attr_ref = tank:get_fish_layers( "Boid_Attr"		)
	self.ref.wall_interaction = aaa.obj.get_down_by_class_no_error(boid_attr_ref, "def_wall_interaction")

end
--todo complete
function GGREA:free()
	local greas = self.__greas
	for i=1,self.__grea_nb do
		greas[i]:free()
	end
	self.__grea_nb		= 0
	self.__greas		= {}
	self.__grea_by_name	= {}
end

function GGREA:get_grea_by_name( name )
	--aaa.print_fn()
	--table.print( self.__grea_by_name, "__grea_by_name", 1 )
	--table.print( self.__greas, "get_grea_by_id", 1 )
	return self.__grea_by_name[ string.lower(name) ]
end
function GGREA:get_grea_by_id( id )	return self.__greas[id]		end
function GGREA:get_grea_nb()		return self.__grea_nb		end

function GGREA:create( name )
	local self = GGREA:create_instance( name )
	self:init()
	return self
end

--todomona network aspect
--todo location
function GGREA:add_grea( name, race_name, nb, boid_id, location )
	if not app.check_grea_location or app:check_grea_location( name, location ) then
		local race = TANK.cur:get_race( race_name )
		if race then
			local grea_id = self.__grea_nb + 1
			local grea = GREA:create( name, grea_id, race, nb, self.crea_nb, boid_id )
			self.__greas[ grea_id ] = grea
			self.__grea_by_name[ string.lower(name) ] = grea
			self.__grea_nb = grea_id
			self.crea_nb = self.crea_nb + nb
			local aqua = TANK.cur.aqua
			if aqua and aqua:is_master() then
				aqua:send_osc( "aqua/add_grea", name, race_name, nb, boid_id )
			end
			return grea
		end
	end
	
	-- to avoid errors in run.lua script we just return an empty grea
	--	that we don't add in grea structure
	local grea = GGREA.__grea_empty
	if not grea then
		grea = GREA:create_empty( "empty" )
		GGREA.__grea_empty = grea
	end
	return grea
end

function GGREA:read_shader_params( )
	local grea = self.grea_cur
	if grea then
		local sha = grea:get_race():get_shading()
		if sha then	--todomona
			table.apply_fn( self.shader_list, function(t)
				t.bu:set_value( sha:get_val( t.sha_type, t.val_type, t.id ) )
			end )
		end
	end
end
function GGREA:write_shader_params( )
	local grea = self.grea_cur
	if grea then
		--local sha = grea:get_race():get_shading()
		--table.apply_fn( self.shader_list, function(t)
		--	sha:set_val( t.sha_type, t.val_type, t.id, t.bu:get_value() )
		--end )
		local race = grea:get_race()
		table.apply_fn( self.shader_list, function(t)
				race:set_shader( t.sha_type, t.val_type, t.id, t.bu:get_value() )
			end )
	end
end

function GGREA:update( b_master, b_update )
	--aaa.print_method()
	--self:print( "GGREA:update "..self.__grea_nb )
	if b_update then
		self:write_shader_params()
		--todo in ggrea ? this is monaco code
		self:send_humans_to_deformer()
	end

	if GA.b_spy then aaa.spy.push_range( "GGREA update", 8 ) end

		local greas = self.__greas
		for i=1,self.__grea_nb do
			greas[i]:update( b_master, b_update )
		end
		
	if GA.b_spy then aaa.spy.pop_range() end
end

function GGREA:draw( b_osc )
--	aaa.print_method()
	--self:print( "GGREA:draw "..self.gr__grea_nbea_nb )
	if GA.b_spy then aaa.spy.push_range( "GGREA:draw()", 7 ) end
		if b_osc then
			self:receive_osc()
		end
		local greas = self.__greas
		for i=1,self.__grea_nb do
			greas[i]:draw()
		end
	if GA.b_spy then aaa.spy.pop_range() end

end

function GGREA:set_all_grea()
	local b = self.bu_grea_active:get_value_as_bool()
	--self:print( self.bu_grea_active:get_value().." "..b )
	local greas = self.__greas
	for i=1,self.__grea_nb do
		greas[i].bu_active:set_value( b )
	end
end
function GGREA:flip_all_grea()
	local greas = self.__greas
	for i=1,self.__grea_nb do
		local bu = greas[i].bu_active
		bu:set_value( not bu:get_value_as_bool() )
	end
end

--todopostmona
function GGREA:change_grea()
	local bu = self.bu_grea_selector
	local name = bu:get_item_data()
	self:print( "change grea to "..name )
	self.grea_cur = self:get_grea_by_name( name )
	self:print( "change grea to "..name.." : "..self.grea_cur )
	self:edit_boid()
	self:read_shader_params()
end

function GGREA:focus_boid()				if self.grea_cur then self.grea_cur:focus_boid()	end		end
function GGREA:restart_boid()			if self.grea_cur then self.grea_cur:restart_boid()	end		end

function GGREA:do_on_race( bu, b_trig, method_race, ... )
	aaa.print_fn()
	if self.grea_cur then
		local race = self.grea_cur:get_race()
		if b_trig then
			race[ method_race ]( race, ... )
		else
			race[ method_race ]( race, bu:get_value(), ... )
		end
	end
end
function GGREA:set_grea_cur( grea )
	self.grea_cur = grea
end
function GGREA:edit_boid( grea )
	if GP.cur then	--todo move to app ?
		local meu = GP.cur:get_meu_by_name_no_error( "Boid_Editor" )
		if meu then
			grea = grea or self.grea_cur
			if grea then
				local boid, layers = grea:get_boid_and_layers()
				meu:set_boid_and_layers( boid, layers, grea )
				local meu_tank = TANK.cur.meu
				--todo do better and more generic using GP
				local ui_slot = meu_tank:get_ui_slot() or 4
				meu:set_ui_slot( ui_slot-1 )
			end
		else
			self:box_debug( "No MEU Boid_Editor" )
		end
	end
end

function GGREA:define_ui( bus )
	local bu
	local ix
	local iy = 2

	local nb = self:get_grea_nb()
	--this is the main fish selector
	--local col_nb = (nb > GGREA.select_fish_line_max) and 2 or 1
	-- bu = bus:add_selector( "Fish current", {2, iy, 4*col_nb, nb} )
	-- 	--we do it one or two colon
	-- 	if col_nb == 2 then		bu:set_nb( 2, GGREA.select_fish_line_max )
	-- 	else					bu:set_nb( 1, nb )
	-- 	end
	-- 	bu:set_text_draw( false )
	-- 	bu:set_method_on_value_change(	self, "change_grea" )
	-- 	bu:set_method_on_click(			self, "edit_boid", bu )
	-- 	bu:set_method_on_click_double(	self, "click_double_selector", bu )
	-- 	self.bu_grea_selector = bu

	self.shader_list = {}
	local function add_to_shader_list( bu, sha_type, val_type, id )
		self.shader_list[#self.shader_list+1] = { bu=bu, sha_type=sha_type, val_type=val_type, id=id }
	end
	local function add_shader_ctrl( name, short, x, y, sx, sha_type, val_type, id, min, max )
		bu = bus:add_slider( name, {ix+x,iy+y, sx,1} ):set_min_max( 0, 2 )
		if short then bu:set_text( short ) end
		add_to_shader_list( bu, sha_type, val_type, id )
		if min and max then
			bu:set_min_max( min, max )
		end
		return bu
	end


	self.grea_cur = self.__greas[1]

	iy = iy + 0
	ix = 16
	bu = bus:add_trig( "Boid", {ix,iy, 3,1.5} ):set_color_back("focus")
		bu:set_method_on_click( self, "focus_boid" )
	bu = bus:add_trig( "Restart", {ix+3,iy, 3,1.5} ):set_color_back("restart")
		bu:set_method_on_click( self, "restart_boid" )
	iy = iy + 3

	local S = 1.5
	bu = bus:add_button(	"Deform",	{ix,iy-S*.5, S,S} ):set_text_rect_ratio( 2 )
		bu:set_value_type_bool( true )
		bu:set_target_lua( GGREA, "b_deform" )
		add_to_shader_list( bu, "vert", "int", 1 )
	iy = iy + 2

	bu = bus:add_trig( "Reload", {ix,iy-1, 5,2} )
	bu:set_method_on_click(	TANK.cur,	"reload_shader"	)
	local function add_grea( b_trig, name, short, x, y, sx, ... )
		x, y = ix+x, iy+y
		bu = b_trig and bus:add_trig( name, {x,y, sx,1} ) or bus:add_button( name, {x,y, sx,1} )
		if short then bu:set_text( short ) end
		bu:set_method_on_click( self, "do_on_race", bu, b_trig, ... )
		return bu
	end
	local function add_grea_trig	( name, short, x, y, sx, ... ) return add_grea( true, name, short, x, y, sx, ... ) end
	local function add_grea_button	( name, short, x, y, sx, ... ) return add_grea( false, name, short, x, y, sx, ... ) end

	add_grea_trig( 		"Shader",			nil,			1, 1, 5,	"focus_shading"		):set_color_back("focus")
	add_grea_trig( 		"Shader Reload",	"Reload One",	1, 2, 5,	"reload_shader"		)

	add_grea_trig(		"RACE Reload",		nil,			1, 4, 5,	"reload"			)
	add_grea_trig(		"RACE Save",		nil,			1, 5, 5,	"save_by_index"		):set_color_back("save")

	--local
	add_grea_trig(		"Layer A",			"A",			4, 7, 2,	"set_focus_layer", 1, 1	):set_color_back("focus")
	add_grea_trig(		"Layer B",			"B",			4, 8, 2,	"set_focus_layer", 1, 2	):set_color_back("focus")
	add_grea_trig(		"Layer C",			"C",			4, 9, 2,	"set_focus_layer", 1, 3	):set_color_back("focus")

	ix = 6
	local b_fbx = TANK.cur:is_fbx()
	if b_fbx then
	else
		local xl = 4 --xl and its use are a hack	
		bu = bus:add_selector(	"See Texture",	{xl+9, iy-1, 2, 11} ):set_nb_min_0( 1, 11 ):set_text_draw( false )
			bu:set_item_text( 1, "Render", "Light", "Diffu", "AO", "Mask", "Bis", "5", "6", "7", "8", "Caustic" )
			add_to_shader_list( bu, "frag", "int", 4 )

		iy = iy + 1
		add_shader_ctrl(	"Diffu",			nil,			xl, 0,	3,	"frag",	"float", 1	)
		add_shader_ctrl(	"AO",				nil,			xl, 1,	3,	"frag",	"float", 2	)
		add_shader_ctrl(	"Mul",				nil,			xl, 2,	3,	"frag",	"float", 3	)
		add_shader_ctrl(	"Dif Bis",			nil,			xl, 3,	3,	"frag",	"float", 5	)
		add_shader_ctrl(	"Caustic",			nil,			xl, 4,	3,	"frag",	"float", 4	)
		--add_shader_ctrl(	"Image",			nil,			xl, 5,	4,	"frag",	"float", 6	)
		--add_shader_ctrl(	"Position",			nil,			xl, 6,	4,	"frag",	"float", 8, -1, 1	)
	end
	--bu:set_method_on_click_double( self, "__focus_camera" )
end

function GGREA:define_ui_greas( bus )
	local bu

	bu = bus:add_but_flip_method(	"Set", nil,	self, "set_all_grea" )
		bu:set_text_rect_ratio( 1.5 )
		self.bu_grea_active = bu

	bus:move_xy( 2.5, -1 )
	bu = bus:add_trig( "Flip", {nil,nil, 2.5,1} )
		bu:set_method_on_click( self, "flip_all_grea" )
	bus:move_x( -2.5 )

	self.__fish_selector_nb = 0
	for i=1,self.__grea_nb do
		self.__greas[i]:add_ui( i, bus )
	end

	local function add_but_send( name )
		--self:box_debug( "add send "..name )
		local bu = bus:add_but_trig_method(		"Send "..name,	nil,	self,	"send_one_fish",	name	)
		bu:set_text_rect_ratio( 6 )
		return bu
	end

	bus:set_xy( 5, 15 )
	for i=1,self:get_grea_nb() do
		local grea = self:get_grea_by_id( i )
		if grea:is_boid_out() then
			add_but_send( grea:get_name() )
		end
	end
end

function GGREA:receive_osc()
	--todoaqua refine
	if not aqua.osc.b_received then return end
	if aqua.b_master_loop then return end

	if GA.b_spy then aaa.spy.push_range( "RECEIVE'CURVE", 7 ) end
		repeat
			local tab = aaa.net.osc_take_by_start( "/grea" )
			if not tab then break end

			--aaa.print( "received Osc " )
			--todoaqua refine
			local	b_master = aqua:is_master()
			local	i = 1
			while true do
				--aaa.print( i )
				local sub_tab = tab[i]
				if not sub_tab then break end

				--aaa.print( "received Osc "..i )
				local tags = sub_tab.tags
				local args = sub_tab.args

				--self:print( id )
				local id = args[1]
				if id then
					local grea = self:get_grea_by_id( id )
					if grea then
						local fn_name = tags[2]
						--self:print( "receive osc grea "..fn_name.."( "..id )
						if fn_name == "add_curve_rt_point" then
							local crea = grea.__pool[args[2]]
							if crea then
								grea:add_curve_rt_point( crea, args[3], args[4], args[5] )
							else
								self:print_error( "can't get crea for id " .. args[2] )
							end
						elseif	fn_name == "set_curve_dist_nb_direct" then
							if (not b_master) then	grea:set_curve_dist_nb_direct( args[2], args[3] )				end
						elseif	fn_name == "start_on_curve_direct" then
							if (not b_master) then	grea:start_on_curve_direct( args[2], args[3], args[4], args[5] )	end
						elseif	fn_name == "set_blow" then
							--self:print( "receive osc grea "..fn_name.."( "..id )
							if (not b_master) then	grea:set_blow( args[2] )										end
						end
					else
						self:print_error( "this osc commande can't get creature group "..id )
					end
				else
					self:print_error( "this osc commande need an id for the grea" )
					--todo dump osc here
				end
				i = i + 1
			end
		until false
	if GA.b_spy then aaa.spy.pop_range() end
end

--todo in ggrea ? this is monaco code
function GGREA:send_humans_to_deformer()
	local def = self.ref.wall_interaction
	if def then
		aaa.bdd.reset_walls( def )
		local humans = app:get_presence_SO()
		for wall, value in pairs(humans) do
			if value.coverage ~= 0 then
				-- self:print(wall..' '..value.x..' 0 '..value.z..' '..value.coverage)
				aaa.bdd.add_human( def, value.x, 1.5, value.z, 100 * value.coverage )
			end
		end
		aaa.bdd.end_humans( def )
	end
end