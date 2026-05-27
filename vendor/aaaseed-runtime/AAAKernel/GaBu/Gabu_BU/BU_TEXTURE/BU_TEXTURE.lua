
if CLASS.DECLARE( "BU_TEXTURE", BUTTON,  {
	ref = {},
--	__b_preset_use = false,
	} ) then
		BU_TEXTURE:set_class_status_doc(	CLASS.STATUS.GABU, "Use for Texture choice" )
end

--
-- SET/GET
--
function BU_TEXTURE:set_preset_use( b_use )
	oo.getsuper(BU_TEXTURE).set_preset_use( self, b_use )
	for _,bu in pairs(self.__t_bu) do
		bu:set_preset_use( b_use )
	end
end

function BU_TEXTURE:set_send( b_send )
	oo.getsuper(BU_TEXTURE).set_send( self, b_send )
	for _,bu in pairs(self.__t_bu) do
		bu:set_send( b_send )
	end
end
--todotex
function BU_TEXTURE:set_value_load_save( b )
	oo.getsuper(BU_TEXTURE).set_value_load_save( self, b )
	--todo refine
	local function set_save( bu )
		bu:set_value_load_save( b )
		if bu.get_target_param then
			local param_ref = bu:get_target_param()
			if param_ref then
				param.set_save( param_ref, b )
			end
		end
		local sel = bu:get_selector()
		if sel then
			set_save( sel )
		end
	end
	for _,bu in pairs(self.__t_bu) do
		set_save( bu )
	end
	--	done in MEU but ???
	--		self.__bu_ui.b_build_preset_needed = true
end

--todo maa 2023 Feb removed it
--function BU_TEXTURE:get_texture_bind_2d()	end
--function BU_TEXTURE:set_texture_bind_2d( bind )	end

-- function BU_TEXTURE:update()
-- 	--if true then return end
-- 	--self:print( "update()" )

-- 	oo.getsuper(BU_TEXTURE).update( self )
-- end

-- function BU_TEXTURE:draw()
-- 	oo.getsuper(BU_TEXTURE).draw( self )
-- 	--aaa.debug.print_traceback()
-- end

--todo keep here there ?
--todo should we cache data ?
function BU_TEXTURE:__update_state()
	local state = self.__tex_state
	--todotex unclear why we have to do this because of loas or preset changes
	if not state.bank_2d_set_by_bu then
	 	local t_bu = self.__t_bu
	 	state.bank_2d_set_by_bu = t_bu.bank:get_value()
	end
	if not state.bind_2d_set_by_bu then
		local t_bu = self.__t_bu
		state.bind_2d_set_by_bu = t_bu.bind:get_value()
   	end
	if    state.bank_2d_computed_with ~= state.bank_2d_set_by_bu
	   or state.bind_2d_computed_with ~= state.bind_2d_set_by_bu then
	   state.bank_2d_computed_with = state.bank_2d_set_by_bu
	   state.bind_2d_computed_with = state.bind_2d_set_by_bu
	   state.bind_2d = aaa.img.make_bind_2d( state.bank_2d_set_by_bu, state.bind_2d_set_by_bu )
	end  
end
function BU_TEXTURE:get_bind_2d()
	self:__update_state()
	return self.__tex_state.bind_2d
end

function BU_TEXTURE:get_bank_bind_2d()
	self:__update_state()
	local state = self.__tex_state
	return state.bank_2d_set_by_bu, state.bind_2d_set_by_bu
end
--used for test
function BU_TEXTURE:set_bank_2d( bank )
	if bank then
		local t_bu = self.__t_bu
		--self:print( "bank asked is "..bank )
		t_bu.bank:set_value( bank )
		--self:print( "value is now "..t_bu.bank:get_value() )
	end
end
BU_TEXTURE.doc.set_bind_2d = "(bind) set the bind of the texture without trigerring undo/redo" 
function BU_TEXTURE:set_bind_2d( bind )
	if bind then
		local bank
		bank, bind = aaa.img.make_bank_bind_2d( bind )
		--self:print( "bank "..bank.." bind "..bind )
		local t_bu = self.__t_bu
		t_bu.bank:set_value( bank )
		t_bu.bind:set_value( bind )
	else
		aaa.print_fn(true)
		self:print_error( "bind is nil" )
	end
end
BU_TEXTURE.doc.set_bind_2d_ui = "(bind) set the bind of the texture including the change in the undo/redo stack" 
function BU_TEXTURE:set_bind_2d_ui( bind_next )
	if bind_next then
		local bind_prev = self:get_bind_2d()
		self:set_bind_2d( bind_next )
		if bind_prev ~= bind_next then
			local undo = self:make_table_to_call_method( self, "set_bind_2d", bind_prev )
			local redo = self:make_table_to_call_method( self, "set_bind_2d", bind_next )
			ga:add_undo_redo( undo,redo )
		end
	else
		aaa.print_fn(true)
		self:print_error( "bind is nil" )
	end
end

function BU_TEXTURE:get_dim()
	--local state = self.__tex_state
	--return state.dim_set_by_bu
	local bu_dim = self.__t_bu.dim
	return bu_dim and bu_dim:get_value() or 2
end


function BU_TEXTURE:set_bank_line_next( nb_bank_line )
	self.__nb_bank_line_next = nb_bank_line
end
function BU_TEXTURE:get_bank_line()
	local nb
	if self.__nb_bank_line_next then
		nb = self.__nb_bank_line_next
		self.__nb_bank_line_next = nil
	else
		nb = 4
	end
	return nb
end

function BU_TEXTURE:load_texture()
	local bind = self:get_bind_2d()
	TEXS:load_texture( bind )
end

function BU_TEXTURE:__set_bank_from_selector( sel )
	--self:print( "BU_TEXTURE:__set_bank_from_selector" ) 
	--aaa.print_fn()
	self.__tex_state.bank_2d_set_by_bu = sel:get_value()
--	table.print( self.__tex_state, "state from bank" )
end
function BU_TEXTURE:__set_bind_from_selector( sel )
	--self:print( "BU_TEXTURE:__set_bind_from_selector" ) 
	--aaa.print_fn()
	self.__tex_state.bind_2d_set_by_bu = sel:get_value()
--	table.print( self.__tex_state, "state from bind" )
end

--[[
function BU_TEXTURE:set_target_lua( t_target )
	-- VERBOSE / DEBUG
	if self.__b_bu_texture_verbose then
		aaa.print_fn()
		if t_target then
			self:print( ":set_target_lua(): we have a target_lua_table" )
			table.print( t_target, "target_lua_table", 2 )
		else
			self:print( ":set_target_lua(): no target_lua_table" )
		end
		self:box_debug( "Verbose" )
	end

	local t_bu = self.__t_bu	
	if t_target then
		t_bu.bank:set_target_lua( t_target, "__bank_2d" )
		t_bu.bind:set_target_lua( t_target, "__bind_2d" )
		local dim = t_bu.dim
		if dim then
			dim:set_target_lua( t_target, "dim" ) --todonow
		end
	end
	--todo NOT NEEDED be here in case
	-- else
	-- 	if t_target then
	-- 		t_target.__dim = 2
	-- 	end

	return self
end
--]]

function BU_TEXTURE:set_target_layer( layer_or_tex_unit, mapping )
	-- VERBOSE / DEBUG
	if self.__b_bu_texture_verbose then
		aaa.print_fn()
		if layer_or_tex_unit then
			self:print( ":set_target_layer() layer_or_tex_unit is "..aaa.obj.get_filename(layer_or_tex_unit) )
		else
			self:print( ":set_target_layer() no layer_or_tex_unit" )
		end
		self:box_debug( "Verbose" )
	end

	local t_bu = self.__t_bu
	if layer_or_tex_unit then
		local bu = t_bu.bank
		local min,max = bu:get_min_max()	-- we have to do this or min max of bu will be imposed by the param
		bu:set_target_obj_param( layer_or_tex_unit, "bank_2d" )
		bu:set_min_max( min, max )

		bu = t_bu.bind
		min, max = bu:get_min_max()	-- we have to do this or min max of bu will be imposed by the param
		bu:set_target_obj_param( layer_or_tex_unit, "bind_2d" )
		bu:set_min_max( min, max )

		if mapping then
			bu = t_bu.dim
			if bu then
				bu:set_target_obj_param( mapping, "tex_use" )
			end
		end
	end
	--todo NOT NEEDED be here in case
	-- else
	-- 	if target_lua_table then
	-- 		target_lua_table.__dim = 2
	-- 	end

	return self
end

function BU_TEXTURE:get_bind_2d_asked()
	local bind = self:get_bind_2d()
	--to refine bind interaction
	local state = self.__tex_state
	if state.bind_2d_asked_last ~= bind then
		state.bind_2d_asked_last	= bind
		return true, bind
	end
	return false
end

function BU_TEXTURE:get_bind_2d_dim_asked()
	local bind = self:get_bind_2d()
	local dim  = self:get_dim()
--	self:print( bind.." with dim "..dim )
	--to refine dim bind interaction
	local state = self.__tex_state
	if state.bind_2d_asked_last ~= bind or state.dim_2d_asked_last ~= dim then
		state.bind_2d_asked_last	= bind
		state.dim_2d_asked_last		= dim
		return true, bind, dim
	end
	return false
end

function BU_TEXTURE:__set_bind_from_ui( bind )	--todotex
	--aaa.print_fn()
	--when reading preset we don't want to change anything
	--todonow was there when in MEU we need a solution
	local meu = self.__texture_meu
	if meu and meu:is_preset_recalling() then
		return
	end
	--we just do it with interaction : don't function no contact yet on first click
	--if bu:is_contact() then
	self:print( "__set_bind_from_ui() bind is "..bind )
	if bind ~= "none" then
		self:set_bind_2d_ui( bind )
		return true
	end
end

function BU_TEXTURE:__set_main_from_selector( meu )	--todotex
	local bind = self:get_selector():get_item_data()
	self:__set_bind_from_ui( bind )
end


function BU_TEXTURE:add_uif_zones_base( zones, dist, x,y )
	return {}
end
	
function BU_TEXTURE:get_uif_zones()
	local d = 1
	local zones = BU_MONITOR.get_uif_zones( self, 0,-d )
	local tn = TEXS:get_tex_named()
	local x,y = -.5,d
	local SX,SY = 3,.8
	local DY = .08
	local groups = tn.__groups_sorted
	table.print( groups, "groups", 2 )
	--self:box_debug( "Oh" )
	local nb = 0
	for i,group in ipairs(groups) do
		if #(group.texs) > 0 then
			nb = nb + 1
		end
	end
	nb = nb + 1 
	local sub_sy = nb*SY + (nb-1)*DY	
	local xy_sxy = { x + SX/2 + sub_sy/2 + SY, y - SY/2 + sub_sy/2, sub_sy,sub_sy }
	local bank_index = self.__tex_state.bank_2d_set_by_bu
	local bank_group = tn:make_group( "Bank Cur", 200, 8,8	)
	tn:add_tex_bank( bank_group, bank_index*32,32 )
	zones["Bank  "..bank_index]	= {	type="button",		rect={x,y, SX,SY}, sub_def = { group=bank_group, xy_sxy=xy_sxy } 	}
	y = y + SY + DY
	for i,group in ipairs(groups) do
		if #(group.texs) > 0 then
			local name = group.name
			self:print( " UIF add tex named group "..name )
			zones[name]	= {	type="button",		rect={x,y, SX,SY}, sub_def = { group=group, xy_sxy=xy_sxy } 	}
			y = y + SY + DY
		end
	end
	zones.Tmp	= {	type="frame", rect=xy_sxy	}
	return zones
end

function BU_TEXTURE:define_uif_sub( sub_def )
	local uif = self:get_uif_data()

	local sub = uif.sub
	if sub and sub.group == sub_def.group then
		return
	end

	--aaa.print_fn()
	table.print( sub_def, "sub_def" )

	local group = sub_def.group
	local group_name = group.name
	self:print( "add group "..group_name)
	sub = { group = group, xy_sxy = sub_def.xy_sxy }

	local texs = group.texs
	local nb = #texs
	local nb_u,nb_v = group.nb_u,group.nb_v
	if nb_u then
		if not nb_v then
			nb_v = math.max( 8, math.ceil( nb / nb_u ) )
		end
	elseif nb_v then
		if not nb_u then
			nb_u = math.max( 4, math.ceil( nb / nb_v ) )
		end
	else
		if nb < 8 then
			nb_u,nb_v = 1,nb
		elseif nb == 9 then
			nb_u,nb_v = 3,6
		elseif nb <= 32 then
			nb_u,nb_v = 4,math.floor( (nb+3) / 4 )
		else
			nb_v = math.ceil(nb)
			nb_u = math.ceil( (nb+nb_v-1) / nb_v )
		end
	end

	local x,y, sx,sy = unpack(sub_def.xy_sxy)
	local margin = sx / 256
	x,y = x-sx*.5,y-sy*.5

	sx,sy = sx/nb_u,sy/nb_v
	x,y = x+sx*.5,y+sy*.5

	local oy = nb_v - math.max( 0, nb_v - math.ceil(nb/nb_u) ) - 1

	local zones = {}
	for i=1,nb do
		local tex = texs[i]  
		local iy = math.floor( (i-1)/ nb_u )
		local ix = i - iy * nb_u
		iy = -iy + oy
		zones[tex.name_short or tex.name] = {	type="button",	rect = {x+(ix-1)*sx,y+iy*sy, sx-margin,sy-margin},	bind_flat = tex.bind_flat }
	end
	sub.zones = zones

	uif.sub = sub
end

function BU_TEXTURE:do_uif_command( uif )
	aaa.print_fn()
	--local command = BU:get_uif_name_sel( uif )
	local b_used = false
	local bind_flat = uif.zone_sel.bind_flat
	if bind_flat then
		self:print( "BU_TEXTURE find bind "..bind_flat )
		self:__set_bind_from_ui( bind_flat )
		b_used = true
	else
		-- b_used = true
		-- if		command == "glue"	then	self.__b_glue = true
		-- elseif	command == "unglue"	then	self.__b_glue = false	
		-- else								b_used = false
		-- end
		local command = BU:get_uif_name_sel( uif )
		b_used = BU_MONITOR.do_command_for_bind( self, command, self:get_bind_2d()  )
	end
	return GABU_OBJ.do_uif_command_with_super( self, b_used, uif, BU_MONITOR )
end


--BU_TEXTURE.do_click_down = nil
function BU_TEXTURE:do_click_down( x,y )
	self:print( "BU_TEXTURE:do_click_down() at "..x..", "..y)
	self:set_bu_cur()

	ga:get_undo_redo():store_values_in_bu(self)

-- -- local bus = self:get_bus_up()
-- -- if bus then
-- --  	x = x + bus.__x_offset_exp
-- -- 	y = y + bus.__y_offset_exp
-- -- end
	self:begin_uif( x,y, "action_down" )
end


function BU_TEXTURE:create( name, rect,	id,	b_dim_or_x_skip )
	id = id or 1

	name = name or (id==1 and "TEX" or ("TEX_"..id))

	local x,y, sx,sy = unpack(rect)

	--table.print( rect, "BU_TEXTURE rect" )
	local b_compact = true
	if rect.b_compact==false then
		b_compact = false
	end 
	local SYB =	b_compact and math.min( .8, sy/5 ) or sy/4

	local b_dim
	if type(b_dim_or_x_skip)=="boolean" then
		b_dim = b_dim_or_x_skip
	else
		b_dim = false
	end
	local dx = sx/8
	local SX_DIM
	if type(b_dim_or_x_skip)=="number" then
		SX_DIM = b_dim_or_x_skip * dx
	else
		SX_DIM = b_dim and dx or 0
	end


	local MARGIN = dx/16
	local HMARGIN = MARGIN * .5
	local function add_margin_to_rect( r )
		return {r[1]+HMARGIN,r[2]+HMARGIN, r[3]-MARGIN,r[4]-MARGIN}
	end

	local l = x - sx * .5
	local t = y + sy * .5
	local Y_CENTER = t - SYB*.5
	local SX_SELF
	local rect_self
	local SX_BANK, SX_BIND
	local SX_MONITOR,SY_MONITOR	
	if b_compact then
		SX_BANK, SX_BIND		= dx*1.5, dx*1.5
		SX_SELF = sx - SX_BANK - SX_BIND - SX_DIM
		rect_self = {l+SX_DIM+SX_SELF/2,		Y_CENTER, SX_SELF,SYB}
		SX_MONITOR,SY_MONITOR	= sx, sy-SYB	
	else
		SX_DIM					= b_dim and sx/4 or 0
		SX_SELF					= sx/2 - SX_DIM
		SX_MONITOR,SY_MONITOR	= sx/2, sy
		local SX_REST			= sx - SX_MONITOR	
		SX_BANK, SX_BIND		= SX_REST/2, SX_REST/2
		rect_self = {l+sx/2+SX_DIM+SX_SELF/2,	Y_CENTER, SX_SELF,SYB}
	end

	local self = BU_TEXTURE:create_instance( name, add_margin_to_rect(rect_self) )
	self.__t_bu = {}
	self.__tex_state = {bu_texture_id=id}	--we store stuff here, id will not be used but can usefull for debug
	self.__t_bu_for_alpha = {}
	self.__texture_meu = meu	--meu is a global 
-- MAIN BU (Shortcut to texture), in fact self

	self.__b_uif_outside		= false
	TEXS:attach_src_menu( self )
	-- we don't want undo on this one
	self.__b_undo_redo_skip = true
	self:get_selector().__b_undo_redo_skip = true
	self:get_selector():set_method_on_value_change( self, "__set_main_from_selector" )	--todotex



	self:set_value_load_save( false )	-- bank and bind save their value
	self:set_method_on_click_double( self, "load_texture" )


	local nb_bank_line = BU_TEXTURE:get_bank_line()	
	local sel_bank, sel_bind, sel_dim
	local rect_monitor
	local rect_dim
	local nb_bank_col
	local text_bank = name.."_bank"
	local text_bind = name.."_bind"
	local text_dim = name.."_dim"

	--self:print( b_compact )
	
	if b_compact then
		local x_new	= l + SX_DIM + SX_SELF

		local function add_button_menu( name, rect, b_slider )
			local bu
			--b_slider = false
			--  create a slider or a button
			if b_slider then 
				bu = bus_cur:add_slider( name, add_margin_to_rect(rect) )
				bu:set_value_type_integer( true )
				bu:set_text_draw( false )
				--bu:set_text_visible( true )
				--bu:set_text( "A" )
				bu:set_show_value( true )
			else
				bu = bus_cur:add_button( name, add_margin_to_rect(rect) )
				bu:set_text_inside( true )
			end

			--bu:set_min_max( 0,31 )
			local sel = SELECTOR:create( name )
			bu:set_menu( sel )
			bu:set_text_selector( true )
			bu:set_method_on_click_double( self, "load_texture" )
			--if b_slider then
				--bu:set_min_max( 1, 32 )
			--end
			return sel, bu
		end
		local bu
	-- BANK
		nb_bank_col = 16
		sel_bank, bu =		add_button_menu(	text_bank,	{x_new+SX_BANK/2,         	Y_CENTER,	SX_BANK,	SYB},	false	)
		self.__t_bu_for_alpha.bank = bu
	--BIND
		sel_bind, bu =		add_button_menu(	text_bind,	{x_new+SX_BANK+SX_BIND/2,	Y_CENTER,	SX_BIND,	SYB},	true	)
		self.__t_bu_for_alpha.bind = bu
	-- DIM
		if b_dim then
			sel_dim, bu =	add_button_menu(	text_dim,	{l+SX_DIM/2,				Y_CENTER,	SX_DIM,		SYB}	)
			self.__t_bu_for_alpha.dim = bu
		end
	-- MONITOR
		rect_monitor = {x, t-SYB-SY_MONITOR/2,	SX_MONITOR, SY_MONITOR}
	else
		local x_new		= l + SX_MONITOR
		local Y_B		= t - SYB - SY_MONITOR/2

		local function add_selector( name, rect )
			return bus_cur:add_selector( name, add_margin_to_rect(rect) )
		end
	-- BANK
		nb_bank_col = 16
		sel_bank =			add_selector(		text_bank,	{x_new+SX_BANK/2,			Y_B,		SX_BANK,	SY_MONITOR} )			
	-- BIND
		sel_bind =			add_selector(		text_bind,	{x_new+SX_BANK+SX_BIND/2,	Y_B,		SX_BIND,	SY_MONITOR}	)		
		for i=1,4 do
			sel_bind:set_item_text( (i-1)*8+1,	(i-1)*8+0	)
		end
	-- DIM
		if b_dim then
			sel_dim =		add_selector(		text_dim,	{x_new+SX_DIM/2, 			Y_CENTER,	SX_DIM,		SYB}		)
		end
	-- MONITOR
		rect_monitor = {l+SX_MONITOR/2, y,	SX_MONITOR, SY_MONITOR}	
	end

	local function configure_selector( sel, nbx,nby )
		sel:set_text_draw( false )
		sel:set_nb_min_0( nbx,nby )
		sel:set_method_on_click_double( self, "load_texture" )	-- MEU_add_tex pass an id
	end
-- BANK
	self.__t_bu.bank = sel_bank
		configure_selector(			sel_bank, nb_bank_col,nb_bank_line )
		TEXS:adjust_bank_selector(	sel_bank, nb_bank_col,nb_bank_line )
		--sel_bank:set_min_max( 0, nb_bank_col*nb_bank_line-1 )
		--sel_bank:set_target_lua( self.__tex_state, "bank_2d_set_by_bu" )
		sel_bank:set_method_on_value_change( self, "__set_bank_from_selector", sel_bank )
		sel_bank:set_min_max( 0, 2047 )
-- BIND		
	self.__t_bu.bind = sel_bind
		configure_selector(			sel_bind, 8,4 )
		TEXS:adjust_bind_selector(	sel_bind, 8,4 )
		--sel_bind:set_min_max( 0, 8*4-1 )
		--sel_bind:set_target_lua( self.__tex_state, "bind_2d_set_by_bu" )
		sel_bind:set_method_on_value_change( self, "__set_bind_from_selector", sel_bind )
-- DIM
	if sel_dim then
		self.__t_bu.dim = sel_dim
		sel_dim:set_text_draw( false )
		sel_dim:set_item_text(	1,	"no", "1D",	"2D", "3D"	)
		sel_dim:set_nb_min_0( 4 )
--		sel_dim:set_target_lua( self.__tex_state, "dim_set_by_bu" )
		--todo NOT NEEDED be here in case
		-- else
		-- 	if target_lua_table then
		-- 		target_lua_table.__dim = 2
		-- 	end
	end
-- MONITOR
	if rect_monitor then
		local bu = bus_cur:add_monitor( name.."_monitor", add_margin_to_rect(rect_monitor), nil, true )
			self.__t_bu.monitor = bu
			bu:set_ui_active( true )
			bu:set_text( name )
			--make sure monitor get his bind_2d from BI_TEXTURE owning him
			bu:__delegate_get_set_bind_2d( self, "get_bind_2d", "set_bind_2d" )
			self.__t_bu_for_alpha.monitor = bu
	end

	-- if self.__b_bu_texture_verbose then
	-- 	self:print( "we exit MEU:__add_bu_texture() now" )
	-- 	table.print( tex, "tex", 1 )
	-- end

	--	self:print( sel_bank:get_min_max() )
	--	self:print( sel_bind:get_min_max() )

	return self
end

-- function BU_TEXTURE:get_pixel_size( id )
-- 	local bind = self:get_texture_bind_2d( id )
-- 	if bind then return aaa.img.get_size( bind ) end
-- end

--generalize BU with sub BUs
function BU_TEXTURE:set_alpha_bu( a )
	--aaa.print_fn()
	oo.getsuper(BU_TEXTURE).set_alpha_bu( self, a )
	
	local t = self.__t_bu_for_alpha
	t.bind:set_alpha_bu( a )
	t.bank:set_alpha_bu( a )
	local bu = t.dim
	if bu then	bu:set_alpha_bu( a ) end
	bu = t.monitor
	if bu then	bu:set_alpha_bu( a ) end
	return self
end