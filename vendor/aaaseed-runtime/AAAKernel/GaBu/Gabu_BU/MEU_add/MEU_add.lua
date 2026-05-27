--
--	MEU ADD stuff
--


--	BU_GROUP handled at the BUS levelm infact
--
function MEU:begin_bu_group( name )					return self:get_bus():begin_bu_group( name )			end
function MEU:end_bu_group()							self:get_bus():end_bu_group() 							end
function MEU:get_bu_group( name )					return self:get_bus():get_bu_group( name )				end
function MEU:set_bu_group_active( name, b_active )	self:get_bus():set_bu_group_active( name, b_active )	end

function MEU:create_add_bu(	bu_class, name,	irect, ... )
	if self.verbose >= 2 then self:print( "create_add_bu() : "..bu_class ) end
--	name = self:__do_before_add_bu( name )
	local bu = bus_cur:create_add_bu( bu_class, name, self:make_rect_from_irect( irect ), ... )
--	self:__do_after_add_bu( bu )

	return bu
end

--	RECT handling position, helpers fns
--
--todo this margin stuff should be passed to the BUS somnehow
--     BU_COLORS, and BU_TEXTURE should be able to use it
MEU.__MARGIN	= .004
MEU.__HMARGIN	= MEU.__MARGIN * .5

MEU.doc.compute_rect_margin_no_from_irect = "( irect ) handle conversion from MEU coor [1,16] going down to BUS coor [-.5, .5]"
function MEU:compute_rect_margin_no_from_irect( irect )
	local angle
	local nx,ny, ix,iy
	if irect then
		irect[4] = irect[4] or irect[3]
		angle = irect[5]
		nx = irect[3] or MEU.__last_pos_nx or 1
		ny = irect[4] or MEU.__last_pos_ny or 1
		ix = irect[1] or MEU.__last_pos_ix
		iy = irect[2] or MEU.__last_pos_iy
	else
		nx = MEU.__last_pos_nx or 1
		ny = MEU.__last_pos_ny or 1
		ix = MEU.__last_pos_ix
		iy = MEU.__last_pos_iy
	end
	--aaa.print_fn()
	MEU.__last_pos_nx = nx
	MEU.__last_pos_ny = ny
	MEU.__last_pos_ix = ix
	MEU.__last_pos_iy = iy + ny

	local SX = 1/16
	local SY = 1/16

	local sx = SX * nx
	local sy = SY * ny

	if angle then
		local dx = sx*.5
		local dy = sy*.5
		angle = angle * math.pi2
		local c,s = math.cos(angle), math.sin(angle)
		return	{	-.5 + (ix -1)*SX + c*dx + s*dy,
					 .5 - iy*SY      + s*dx - c*dy,
					sx,sy, angle 	}
	else
		return 	{	-.5 + (ix -1)*SX + sx*.5,
					 .5 - iy*SY      - sy*.5,
					sx,sy
				}
	end
end

MEU.doc.make_rect_from_irect = { "( irect ) handle conversion from MEU coor [1,16] going down to BUS coor [-.5, .5],",
									" deal with margin, compact flag and angle in irect." }
function MEU:make_rect_from_irect( irect )
	--aaa.print_fn()
	local rect = self:compute_rect_margin_no_from_irect( irect )
	rect[1] = rect[1] + self.__HMARGIN
	rect[2] = rect[2] + self.__HMARGIN
	rect[3] = rect[3] - self.__MARGIN
	rect[4] = rect[4] - self.__MARGIN
	if irect then
		local v = irect.b_compact
		if v ~= nil then
			rect.b_compact = v --todonow
		end
		v = irect[5]
		if v then
			rect[5] = v
		end
	end
	return rect
end

--
--todo reuse everywhere and refine
function MEU:unpack_rect_using_def( rect, def )
	if not rect then return def[1],def[2], def[3],def[4] end
	local x  = rect[1] or def[1]
	local y  = rect[2] or def[2]
	local sx = rect[3] or def[3]
	local sy = rect[4] or def[4]
	return x,y, sx,sy
end
function MEU:get_rect_using_def( rect, def )
	local x,y, sx,sy = self:unpack_rect_using_def( rect, def )
	return {x,y, sx,sy}
end

--
--	ADD PRIMITIVES
--
MEU.doc.add_param = "( rect, bui_name, param_ref, min,max, val )"
function MEU:add_param(	rect, bui_name, param_ref, min,max, val )
	rect[3] = rect[3] or param.is_type_bool( param_ref ) and 1 or 8
	rect[4] = rect[4] or 1
	local bu
	if self.verbose >= 2 then self:print( "MEU:add_param() "..bui_name ) end
	if param_ref then
		if self.verbose >= 2 then self:print( "MEU:add_param() have a param ref" ) end
		if param.is_type_bool( param_ref ) then	bu = self:add_button(	rect, bui_name )	--, param_ref )
		else 									bu = self:add_slider(	rect, bui_name )	--, param_ref )
		end
		--param.set_comment( param_ref, bui_name )
		bu:set_target_param( param_ref, 1, bui_name )
		param.set_save( param_ref, false )	--hack we try : if the meu store the value we don't need to store it in the object
	else
		bu = self:add_slider(	rect, bui_name )
	end
	if val~=nil then
		bu:set_value( val )
	end
	if min and max then
		bu:set_min_max( min,max )
	end
	bu:set_color_back( bui_name )
	return bu
end

-- function MEU:add_param(			rect,	bui_name, param_ref, min, max )
-- 	rect[3] = rect[3] or param.is_type_bool( param_ref ) and 1 or 8
-- 	rect[4] = rect[4] or 1
-- 	return self:add_param_size(		rect,bui_name,	 param_ref,min, max )
-- end
-- function MEU:add_param_size_obj_name(	ix, iy,		nx, ny,		bui_name, obj_ref, param_name, min, max )
-- 	local param_ref = obj_ref and param.get_ref( obj_ref, param_name ) or nil
-- 	return self:add_param(	{ix,iy,	 nx,ny}, bui_name, param_ref, min, max )
-- end

MEU.doc.add_param_obj_name = "( rect, bui_name, obj_ref, param_name, min, max )"
function MEU:add_param_obj_name(	rect, bui_name, obj_ref, param_name, min, max )
	local param_ref = obj_ref and param.get_ref( obj_ref, param_name ) or nil
	return self:add_param( 			rect, bui_name, param_ref, min, max )
end

--	ADD	TEXT
--
MEU.doc.add_text = "( rect, name )"
function MEU:add_text(				rect, name )
	rect[3] = rect[3] or 8
	return self:create_add_bu(	 	BU_TEXT, name, rect )
end

MEU.doc.add_text_info = "( rect, name )"
function MEU:add_text_info(			rect, name )
	local b_size_half = not rect[4]
	local bu = self:create_add_bu(	BU_TEXT, name, rect )
	bu:make_text_info()
	if b_size_half then
		bu:set_text_factor( .5 )
	end
	return bu
end

--	ADD BUTTON
--
MEU.__button_size_factor = 1.
function MEU:get_button_size_factor()	return MEU.__button_size_factor		end

MEU.doc.add_button = "( rect, name, tab_or_ref, name_field, val )"
function MEU:add_button(			rect, name,	tab_or_ref, name_field, val )
	local bu = self:create_add_bu(	BUTTON, name, rect )
	if tab_or_ref or name_field then
		bu:set_target( tab_or_ref, name_field, val )
	else
		if val~=nil then
			bu:set_value( val )
		end
	end
	return bu
end

--	TRIG
--
MEU.doc.add_trig = "( rect, name, tab_or_ref, name_field, val )"
function MEU:add_trig(				rect, name,	tab_or_ref, name_field, val )
	rect[3] = rect[3] or 3
	rect[4] = rect[4] or 1
	local bu = self:add_button(		rect, name,	tab_or_ref, name_field, val or false )
		:set_trig()
	return bu
end

MEU.doc.add_trig_method = "( rect,	name, obj, method_name,	... )"
function MEU:add_trig_method(		rect,	name, obj, method_name,	... )
	local bu = self:add_trig(		rect,	name )
		bu:set_method_on_click( obj, method_name, ... )
	return bu
end

MEU.doc.add_trig_fn = "( rect, name, ... )"
function MEU:add_trig_fn(			rect, name, ... )
	local bu = self:add_trig(		rect, name )
		bu:set_function_on_click( ... )
	return bu
end
-- function MEU:add_trig_size_method(	ix,iy,	nx,ny,		name, obj, method_name,	... )
-- 	local bu = self:add_trig(		{	ix,iy,	nx,ny},		name )
-- 		bu:set_method_on_click( obj, method_name, ... )
-- 	return bu
-- end
-- function MEU:add_trig_size_fn(		ix,iy,	nx,ny,		name, ... )
-- 	local bu = self:add_trig(	{		ix,iy,	nx,ny},		name )
-- 		bu:set_function_on_click( ... )
-- 	return bu
-- end

MEU.doc.add_trig_restart = "( rect,	... )"
function MEU:add_trig_restart(		rect,					... )
	return self:add_trig_method(	rect,	"Restart",	self, "restart", ...	)
end
-- function MEU:add_trig_size_restart(	ix,iy,	nx,ny,		... )
-- 	return self:add_trig_method(	{	ix,iy,	nx,ny},		"Restart",	self, "restart", ...	)
-- end

MEU.doc.add_window = "( irect, name )"
function MEU:add_window( 			irect, name )
	local rect = self:make_rect_from_irect( irect )
	--todo does it work always ? does ir generate conflics sometimes ?
	local bu = bus_cur:add_window( name, BUS:create( name.."_window" ),	rect )
	return bu
end


MEU.doc.add_window_ga = "( rect, name, col_nb, method_name )"
function MEU:add_window_ga( 		rect, name, col_nb, method_name )
	local bus = BUS:create( name )
	local sx = 2 * col_nb/4
	local r = rect[4]/rect[3]
	local sy = sx * r
	
	bus:set_transfo( sx,sy, sx*.5-.5,-sy*.5+.5 )
	bus:init_begin()
		self:add_text( {1,0, 8,1}, name ):set_ui_active( false )
		
		self[method_name]( self, col_nb )
	bus:init_end()

	local bu = BU:create_window_center( name, rect, bus )
		bu:set_color_back( {0,0,0, 1} )
	self.__bu_win = bu
	return bu
end


--	ADD SLIDER
--
MEU.doc.add_slider = "( rect, name, tab_or_ref, name_field, val, vmin,vmax )"
function MEU:add_slider(			rect, name, tab_or_ref, name_field, val, vmin,vmax )
	--aaa.print_method()
	if rect then
		rect[3] = rect[3] or 8
	end	
	local bu = self:create_add_bu( SLIDER, name, rect )
	--bu:set_meter( true )
	bu:set_show_value( true )

	--self:print( name.." ".. tab_or_ref .." "..name_field)
	--if tab_or_ref then self:print( "Before set_target "..param.get( tab_or_ref ) ) end
	if tab_or_ref or name_field then
		bu:set_target( tab_or_ref, name_field ) --la val )
	end
	--if tab_or_ref then	self:print( "Before set_target "..param.get( tab_or_ref ) ) end
	bu:set_min_max_value( vmin, vmax, val )
	
	if val~=nil then
		bu:set_def(val)
	end
	--if tab_or_ref then	self:print( "after set_min_max_value "..param.get( tab_or_ref ) )	end

	if vmin and vmax then
		if vmin==0 and vmax==1 then
			bu:add_values_def( .1, .2, .3, .4, .5, .6, .7, .8, .9 )
		end
	end
	return bu
end

MEU.doc.add_slider_two = "( rect, name,	tab_or_ref, name1_or_ref, name2_or_ref,	val1,val2,	vmin,vmax )"
function MEU:add_slider_two(		rect,		name,	tab_or_ref,name1_or_ref,name2_or_ref,	val1,val2,	vmin,vmax )
	local bu = self:create_add_bu(	SLIDER_TWO, name, rect )
	bu:set_slider_two_target(	tab_or_ref,name1_or_ref,name2_or_ref,	val1,val2,	vmin,vmax )
	return bu
end

MEU.doc.add_slider_multi = "( rect, name, nb, ... )"
function MEU:add_slider_multi(		rect, name, nb, ...)
	local bu = self:create_add_bu( SLIDER_MULTI, name, rect, nb, ... )
	bu:set_mobile( false )
	return bu
end

MEU.doc.add_slider_multi_curve = "( rect, name, nb, b_more, ... )"
function MEU:add_slider_multi_curve(rect,		name, nb, b_more, ... )
	local bu = self:create_add_bu( SLIDER_MULTI_CURVE, name, rect, nb, b_more, ... )
	return bu
end

MEU.doc.add_dial = "( rect, name, tab_or_ref, name_field, val, vmin, vmax )"
function MEU:add_dial(				rect,		name, tab_or_ref, name_field, val, vmin, vmax )
	local bu = self:add_slider(		rect,		name, tab_or_ref, name_field, val, vmin, vmax )
		bu:set_dial( true )
		bu:set_meter( true )
		bu:set_show_value( true )
	return bu
end

MEU.doc.add_slider_xy = "( rect, name, tab_or_ref, name1_or_ref, name2_or_ref, val1,val2, vmin1, vmax1, vmin2, vmax2 )"
function MEU:add_slider_xy(			rect,	  name, tab_or_ref, name1_or_ref, name2_or_ref, val1,val2, vmin1, vmax1, vmin2, vmax2 )
	local bu = self:create_add_bu( SLIDER_XY, name, rect )
	bu:set_slider_xy_target(	tab_or_ref, name1_or_ref, name2_or_ref, val1,val2, vmin1,vmax1, vmin2,vmax2 )
	return bu
end

--	ADD GROUP OF VALUE
--
MEU.doc.__add_slider_size_one = "( rect, color_key, name, tab_or_ref, text_short, val, min,max, values_def )"
function MEU:__add_slider_size_one(	rect,	  color_key, name, tab_or_ref, text_short, val, min,max, values_def )
	--aaa.print_fn()
	--self:print( "tab or ref : "..tab_or_ref )
	local bu
	-- we set min max here to avoid too many value_def set by default
	if param.is_ref_no_error( tab_or_ref ) then	--ref
		bu = self:add_slider(	rect,	name, tab_or_ref, nil,					val, min,max )
			:set_color_back( color_key )
	else	--tab
		bu = self:add_slider(	rect,	name, tab_or_ref, string.sub(name,-1),	val, min,max )
			:set_color_back( color_key )
		--self:print( "Titi "..string.sub(name,-1) )
	end

	if text_short then
		bu:set_text( text_short )
	end
	if values_def then
		bu:add_values_def( values_def  )
	end

	return bu
end

--to generalize even more do _add_compo_n and index table with ints

MEU.doc.__add_compo_n = "( nb, rect, text, tab, b_xyz, val,	min,max, values_def, pre, names )"
function MEU:__add_compo_n(			nb, rect,	text, tab, b_xyz, val,	min,max, values_def, pre, names )
	--aaa.print_fn()

	local dx,dy
	if rect.b_vert then
		dx,dy = 0, rect[4]/nb
		rect[4] = dy
	else
		dx,dy = rect[3]/nb,	0
		rect[3] = dx
	end

	text = text.."_"
	local function add( i, compo )
		local name = text..compo
		local name_full = name
		local name_short = b_xyz and string.upper(compo)
		local b = param.is_ref_no_error( tab[compo] )
		if pre then	name_short,name_full = name_short or name_full, pre..name_full end
		local r = table.copy_simple(rect)
		r[1] = r[1] + dx*(i-1)
		r[2] = r[2] + dy*(i-1)
		--if norm==0 then
		--	self:box_debug( "norm should not be 0" )
		--end
		--self:print( "Toto "..tab )
		return self:__add_slider_size_one(	r,	compo,	name_full,	b and tab[compo] or tab,  name_short,	val,	min,max,	values_def	)
	end
	local bu = {}
	for i=1,nb do
		bu[i] = add(	i,		names[i]		)
	end
	return unpack(bu)
end

MEU.doc.add_sliders_xyz_min_max = "( rect, name, tab, b_xyz, val, min,max, values_def, pre )"
function MEU:add_sliders_xyz_min_max(		rect,	name, tab, b_xyz, val,	min,max,	values_def, pre )
	--aaa.print_fn()
	return self:__add_compo_n(	3,			rect,	name, tab, b_xyz, val,	min,max, 	values_def, pre, {"x","y","z"} )
end

MEU.doc.add_sliders_xyz = "( rect, name, tab, b_xyz, norm, values_def, pre )"
function MEU:add_sliders_xyz(				rect,	name, tab, b_xyz,		norm, 		values_def, pre )
	--aaa.print_fn()
	return self:add_sliders_xyz_min_max(	rect, 	name, tab, b_xyz, 0,	-norm,norm,	values_def, pre )
end

MEU.doc.add_sliders_xy_min_max = "( rect, name, tab, b_xyz, val, min,max, values_def, pre )"
function MEU:add_sliders_xy_min_max(		rect,	name, tab, b_xyz, val,	min,max,	values_def, pre )
	return self:__add_compo_n(  2,			rect,	name, tab, b_xyz, val,	min,max, 	values_def, pre, {"x","y"} )
end

MEU.doc.add_sliders_xy = "( rect, name, tab, b_xyz, norm, values_def, pre )"
function MEU:add_sliders_xy(				rect,	name, tab, b_xyz, 		norm,		values_def, pre )
	return self:add_sliders_xy_min_max( 	rect,	name, tab, b_xyz,	0,	-norm,norm,	values_def, pre )
end

MEU.doc.add_sliders_uvw_min_max = "( rect, name, tab, b_xyz, val, min,max, values_def, pre )"
function MEU:add_sliders_uvw_min_max(		rect,	name, tab, b_xyz, val,	min,max,	values_def, pre )
	return self:__add_compo_n(	3,			rect,	name, tab, b_xyz, val,	min,max, 	values_def, pre, {"u","v","w"} )
end

MEU.doc.add_sliders_uvw = "( rect, name, tab, b_xyz, val, norm, values_def, pre )"
function MEU:add_sliders_uvw(				rect,	name, tab, b_xyz, val,	norm, 		values_def, pre )
	return self:add_sliders_uvw_min_max(	rect, 	name, tab, b_xyz, val,	-norm,norm,	values_def, pre )
end

MEU.doc.add_sliders_uv_min_max = "( rect, name, tab, b_xyz, val, min,max, values_def, pre )"
function MEU:add_sliders_uv_min_max(		rect,	name, tab, b_xyz, val,	min,max,	values_def, pre )
	return self:__add_compo_n(  2,			rect,	name, tab, b_xyz, val,	min,max, 	values_def, pre, {"u","v"} )
end

MEU.doc.add_sliders_uv = "( rect, name, tab, b_xyz, val, norm, values_def, pre )"
function MEU:add_sliders_uv(				rect,	name, tab, b_xyz, val,	norm,		values_def, pre )
	return self:add_sliders_uv_min_max( 	rect,	name, tab, b_xyz, val,	-norm,norm,	values_def, pre )
end

--todo do this like previous fns using add_compo_n
MEU.doc.add_sliders_sxyzf = "( rect, name, tab, b_xyz, val, norm, values_def )"
function MEU:add_sliders_sxyzf(				rect,	name, tab, b_xyz, val, norm,		values_def  )
	local ix,iy, sx,sy = unpack( rect )

	local dx,dy
	if rect.b_vert then
		dx,dy = 0,sy/4
		sy = dy
	else
		dx,dy = sx/4,	0
		sx = dx
	end

	name = name.."_size_"
	self:__add_slider_size_one(	{ix,		iy+dy*3,	sx,sy},	"factor",	name.."sf",	tab.sf, b_xyz and "SFactor",	val, 0,norm,	values_def	)
	self:__add_slider_size_one(	{ix+dx,		iy,			sx,sy},	"x",		name.."sx",	tab.sx, b_xyz and "SX",			val, 0,norm,	values_def	)
	self:__add_slider_size_one(	{ix+dx*2,	iy+dy,		sx,sy},	"y",		name.."sy",	tab.sy, b_xyz and "SY",			val, 0,norm,	values_def	)
	self:__add_slider_size_one(	{ix+dx*3,	iy+dy*2,	sx,sy},	"z",		name.."sz",	tab.sz, b_xyz and "SZ",			val, 0,norm,	values_def	)
end
--todo centralize this and previous one in a generic functionm eventually deal with param order
MEU.doc.add_sliders_suvwf = "( rect, name, tab, b_xyz, val, norm, values_def )"
function MEU:add_sliders_suvwf(				rect,	name, tab, b_uvw, val, norm,		values_def  )
	local ix,iy, sx,sy = unpack( rect )

	local dx,dy
	if rect.b_vert then
		dx,dy = 0,sy/4
		sy = dy
	else
		dx,dy = sx/4,	0
		sx = dx
	end

	name = name.."_size_"
	self:__add_slider_size_one(	{ix,		iy+dy*3,	sx,sy},	"factor",	name.."sf",	tab.sf, b_uvw and "SFactor",	val, 0,norm,	values_def	)
	self:__add_slider_size_one(	{ix+dx,		iy,			sx,sy},	"u",		name.."su",	tab.su, b_uvw and "SU",			val, 0,norm,	values_def	)
	self:__add_slider_size_one(	{ix+dx*2,	iy+dy,		sx,sy},	"v",		name.."sv",	tab.sv, b_uvw and "SV",			val, 0,norm,	values_def	)
	self:__add_slider_size_one(	{ix+dx*3,	iy+dy*2,	sx,sy},	"w",		name.."sw",	tab.sw, b_uvw and "SW",			val, 0,norm,	values_def	)
end


--	ADD SELECTOR
--
-- deal too with
--todo add nb target ...
MEU.doc.add_selector = "( rect, name )"
function MEU:add_selector(			rect,	 name )
	local bu = self:create_add_bu( SELECTOR, name, rect )
	return bu
end

MEU.doc.add_preset = "( rect, name )"
function MEU:add_preset(			rect,  name )
	local bu = self:create_add_bu( PRESET, name or "Preset", rect )
	self.ui.bu_preset = bu
	--bu:set_draw_skip_for_speed( true )
	return bu
end
--todomaa
function MEU:set_preset( v )
	self.ui.bu_preset:set_value( v-1 )
	self:recall_preset( v )
end


--	COLOR
--
--todo move to COLOR_REF ???
MEU.doc.__make_color_ref = "( name, false_nil_ref_obj_or_color_ref, param_prefix ) helper fn to make a default ref when passed nil,\npass false to get a nil out"
function MEU:__make_color_ref( name, false_nil_ref_obj_or_color_ref, param_prefix )
	--self:print( "make_color_ref : ref is "..ref )
	if false_nil_ref_obj_or_color_ref==false then
		return nil
	end
	if false_nil_ref_obj_or_color_ref==nil then
		local color_ref = self.ref and self.ref.color
		if color_ref then
			return color_ref
		--else
		--	self:box_error( "color_ref is nil" )
		end
	end

	--self:print( "make_color_ref : build one now from "..ref )
--	self:print( "name_to_display "..name_to_display )

	--bu.color_ref = color_ref
	if type(false_nil_ref_obj_or_color_ref) == "table" then
		--ref:print( "we consider we have a color_ref, check more later" )
		--table.print( ref, "color_ref", 4 )
		return false_nil_ref_obj_or_color_ref
	elseif false_nil_ref_obj_or_color_ref == nil then
		return COLOR_REF:create( self:get_name().."_from_nil", nil, param_prefix )
	elseif aaa.obj.is_ref_no_error( false_nil_ref_obj_or_color_ref ) then
		local class_name = aaa.obj.get_class( false_nil_ref_obj_or_color_ref )
		--self:print( "make_color_ref : class_name is "..class_name )
		if class_name == "color" then
			local color_ref = COLOR_REF:create( self:get_name(), false_nil_ref_obj_or_color_ref )
			color_ref:set_convert( true )
			--table.print( color_ref, "color_ref", 4 )
			return color_ref
		else
			local color_ref = COLOR_REF:create( name, false_nil_ref_obj_or_color_ref, param_prefix )	--tocheck later
			--table.print( color_ref, "color_ref", 4 )
			return color_ref
			--self.__color_bdd = ref
		end
	else
		self:print_error( false_nil_ref_obj_or_color_ref.." should be a color ref, but is not." )
	end
end

--pass ref = false to avoid taking color of later marked as ref
function MEU:__add_colors( irect, name, b_alpha, b_grey, b_hsv, false_nil_ref_obj_or_color_ref, param_pre )
	irect = self:get_rect_using_def( irect, {1,8, 8,1} )
	local cref = self:__make_color_ref( name, false_nil_ref_obj_or_color_ref, param_pre )
--	local bu_color = self:__add_bu_color_size( ix, iy, nx, ny or nx, name, cref, b_alpha, b_grey )

	name = name or "color"
	local bu = self:create_add_bu( BU_COLOR, name, irect, cref, b_alpha, b_grey, b_hsv, self )
	return bu
end

--todo ref have to be set to false or we get in trouble
--todo		this behavior is dangerous and have to change
--todo under we deql with ref = nil now (april 2016)
--todo nx = nil ok but if we define it it is weird
--ix, iy, nx, ny,	pre, ref (COLOR_REF or ...)
local function make_comment( str, b_only )
	local comment = "( rect, name ) return a BU_COLOR with sliders "..str.."\n"
	if b_only then
		comment = comment.." this version (_only) have no HSV mode\n"
	end
	return comment
end

MEU.doc.add_rgb        = make_comment( "RGB",			  false )
MEU.doc.add_rgbf       = make_comment( "RGB and Factor",  false )
MEU.doc.add_rgba       = make_comment( "RGBA",            false )
MEU.doc.add_rgbfa      = make_comment( "RGBA and Factor", false )

MEU.doc.add_rgb_only   = make_comment( "RGB",             true )
MEU.doc.add_rgbf_only  = make_comment( "RGB and Factor",  true )
MEU.doc.add_rgba_only  = make_comment( "RGBA",            true )
MEU.doc.add_rgbfa_only = make_comment( "RGBA and Factor", true )

-- for these function double clicking on the left part of the BU flip HSV mode 
function MEU:add_rgb(		 irect, name, ... )	return self:__add_colors( irect, name, false, false, true,  ... ) end
function MEU:add_rgbf(		 irect, name, ... )	return self:__add_colors( irect, name, false, true,  true,  ... ) end
function MEU:add_rgba(		 irect, name, ... )	return self:__add_colors( irect, name, true,  false, true,  ... ) end
function MEU:add_rgbfa(		 irect, name, ... )	return self:__add_colors( irect, name, true,  true,  true,  ... ) end
--todo in hsv mode grey is on alpha
-- no HSV mode for these
function MEU:add_rgb_only(	 irect, name, ... )	return self:__add_colors( irect, name, false, false, false, ... ) end
function MEU:add_rgbf_only(  irect, name, ... )	return self:__add_colors( irect, name, false, true,  false, ... ) end
function MEU:add_rgba_only(	 irect, name, ... )	return self:__add_colors( irect, name, true,  false, false, ... ) end
function MEU:add_rgbfa_only( irect, name, ... )	return self:__add_colors( irect, name, true,  true,  false, ... ) end

MEU.doc.add_color_1 = "( rect )"
function MEU:add_color_1( rect )
	local col = {}
	rect[3] = rect[3] or 4
	rect[4] = rect[4] or 1
	col[1] = self:add_rgba(	rect, "color_1", false )
	return col
end

MEU.doc.add_color_2 = "( rect )"
function MEU:add_color_2( rect )
	rect[3] = rect[3] or 8
	rect[4] = rect[4] or 1
	local col = {}
	rect[3] = rect[3] / 2
	col[1] = self:add_rgba(	rect, "color_1", false )
	rect[1] = rect[1] + rect[3]
	col[2] = self:add_rgba(	rect, "color_2", false )
	return col
end

--	MODEL
--
MEU.doc.__add_size = "( rect, ref, name, min, max ) Helper fn"
function MEU:__add_size(				rect,	ref, name, min, max )
	local bu = self:add_param_obj_name(	rect, "s"..name,	ref,  "size_"..name, min, max )
	bu:set_color_back( name )
	return bu
end

--todo extend ? move ?
-- the fact we don't save param when MEU store the value make the ref not being loadded when all the value are default
-- so we have to fore existence
function MEU:__force_obj_ui_refname_needed( obj_ref )
	local ref = self.ref
	obj_ref = obj_ref or ref.model
	--table.print( ref, "refsa", 3 )
	-- --new version relly on current state not stae at init
	-- if not obj_ref then
	-- 	self:print( "try to get model from layer marked" )
	-- 	obj_ref = aaa.layer.get_model( ref.__layer_marked )
	-- end
	-- if obj_ref then
	-- 	local name_symbo_ref = param.get_ref_no_error( obj_ref, "name_symbo" )
	-- 	if name_symbo_ref then
	-- 		local name_symbo = param.get( name_symbo_ref )
	-- 		if name_symbo=="" then
	-- 			param.set( name_symbo_ref, "MEU_Needed" )
	-- 		end
	-- 	else
	-- 		self:print_error( "Can't force obj_ui futur existence using name_symbo param with object of class "..aaa.obj.get_class(obj_ref) )
	-- 	end
	-- else
	-- 	self:box_debug( "can't force obj_ui futur existence without a ref" )
	-- end
	return obj_ref
end

MEU.doc.__add_size_begin = "( rect, ref )"
function MEU:__add_size_begin(		rect,	ref )
	rect = self:get_rect_using_def( rect, {9,4, 8,1} )
	-- the fact we don't save param when MEU store the value make the ref not being loaded when all the value are default
	--  so we have to fore existence
	ref = self:__force_obj_ui_refname_needed( ref )
	return rect, ref
end

MEU.doc.add_size_f = "( rect, ref )"
function MEU:add_size_f(			   rect,	ref )
	rect, ref = self:__add_size_begin( rect,	ref )
	if ref then
		local bu = self:__add_size( rect, ref, "factor",	0, 16. ):add_values_def( 1, 2, 4, 4.5, 8, .125, .25, .5 )
		return bu
	end
end

--todo change it to "size" or "aaa_size" or "__size" with retro compatibility 
local bu_group_size_name = "grid"

MEU.doc.add_size_uvf = "( rect, ref )"
function MEU:add_size_uvf(			   rect,	ref )
	rect, ref = self:__add_size_begin( rect,	ref )
	if ref then
		self:begin_bu_group( bu_group_size_name )
			local t = {}
			rect[3] = rect[3]/3
			t.sf =	self:add_size_f(	rect, ref )
			rect[1] = rect[1] + rect[3]
			t.su =	self:__add_size(	rect, ref,  "u", 0, 10 ):add_values_def( .5, 1, 2, 3, 4, 4.5, 5, 6, 7, 8, 9 )
			rect[1] = rect[1] + rect[3]
			t.sv =	self:__add_size(	rect, ref,  "v", 0, 10 ):add_values_def( .5, 1, 2, 3, 4, 4.5, 5, 6, 7, 8, 9 )
			self.ui.__t_bu_size_uv = t --todo make a BU_SIZE_UV
		self:end_bu_group()
		return t
	end
end

MEU.doc.add_size_uva = "( rect, ref )"
function MEU:add_size_uva(			   rect, ref )
	rect, ref = self:__add_size_begin( rect, ref )
	if ref then
		self:begin_bu_group( bu_group_size_name )
		local t = {}
		rect[3] = rect[3]/3
		t.su	=	self:__add_size( rect, ref, "u",		0, 4. )
		rect[1] = rect[1] + rect[3]
		t.sv	=	self:__add_size( rect, ref, "v",		0, 4. )
		rect[1] = rect[1] + rect[3]
		t.s_axe	=	self:__add_size( rect, ref, "axe",		0, 4. )
		self.ui.__t_bu_size_uv = t --todo make a BU_SIZE_UV
		self:end_bu_group()
	end
end

MEU.doc.add_size_uvaf = "( rect, ref )"
function MEU:add_size_uvaf(			   rect, ref )
	rect, ref = self:__add_size_begin( rect, ref )
	if ref then
		self:begin_bu_group( bu_group_size_name )
		local t = {}
		local ui = self.ui
		rect[3] = rect[3] / 4
		t.su	=	self:__add_size( rect, ref, "u",		0, 4. )
		rect[1] = rect[1] + rect[3]
		t.sv	=	self:__add_size( rect, ref, "v",		0, 4. )
		rect[1] = rect[1] + rect[3]
		t.s_axe	=	self:__add_size( rect, ref, "axe",		0, 4. )
		rect[1] = rect[1] + rect[3]
		t.sf	=	self:add_size_f( rect, ref )
		self.ui.__t_bu_size_uv = t --todo make a BU_SIZE_UV
		self:end_bu_group()
	end
end

function MEU:__set_format_screen()
 	local id = self.ui.bu_screen_id:get_value()
	local sx,sy = aaa.screen.get_sxy(id)
	if sx~=0 and sy~=0 then
		self:__set_ratio_x( sx/sy )
	else
		self:print_error( "No screen size" )
	end
end
function MEU:__set_format_fbo()
	local fbo = self:get_fbo()
	if fbo then
		local sx,sy = fbo:get_pixel_size()
		self:__set_ratio_x( sx/sy )
	else
		self:print_error( "No Fbo" )
	end
end
function MEU:__swap_format()
	local tb = self.ui.__t_bu_size_uv
	local su,sv = tb.su:get_value(), tb.sv:get_value()
	--todo aglomerate the two for undo/redo
	tb.su:set_value_ui( sv )
	tb.sv:set_value_ui( su )
end

function MEU:__set_ratio_x( ratio_x )
--	aaa.print_method()
	local tb = self.ui.__t_bu_size_uv
	if tb.ratio_sel:get_value()==1 then
		local sy = 	tb.sv:get_value()
		ratio_x = ratio_x or self:get_texture_ratio_x()
	--self:print( "\t"..ratio_x.." "..sy )
		tb.su:set_value_ui( ratio_x * sy )
		--tb.sv:set_value( sy )
	else
		local sx = tb.su:get_value()
		local ratio_y = ratio_x and (1./ratio_x) or self:get_texture_ratio_y()
	--self:print( "\t"..ratio_x.." "..sy )
		--tb.su:set_value( sx )
		tb.sv:set_value_ui( ratio_y * sx )
	end
end

MEU.doc.add_size_uvf_video = "( rect, ref, b_screen )"
function MEU:add_size_uvf_video( rect, ref, b_screen )
--	rect[2] = rect[2] or 7
--	rect[4] = rect[4] or 2
	rect, ref = self:__add_size_begin( rect, ref )
	local ix,iy, sx,sy = unpack(rect)
	if not ref then return end

	local bu
	local SYB = 1.5
	local SY = sy / (2+SYB)

	--todo very good example of problem to solve for a clean name comvention

	local SBX = sx/5
--	aaa.print_fn()
	self:begin_bu_group( bu_group_size_name )
	local bu_sel_ratio = self:add_selector(	{ix,iy+SY*.5,	SBX,SY},	"Ratio"	)
		:set_nb( 2,1 )
--		:set_item_text( 1, "U", "V" )
		:set_item_text( 1,  "-", "|" )

	sx = sx - SBX
	ix = ix + SBX

	local SX = sx/4

	bu = self:add_trig_method(	{ix,   		iy,		SX,SY},	"1",		self, "__set_ratio_x", 1	)
	bu = self:add_trig_method(	{ix+SX,		iy,		SX,SY},	"from_tex",	self, "__set_ratio_x"		)
		bu:set_text( "Tex" )	--avoid duplicate with tex default
	bu = self:add_trig_method(	{ix+SX*2,   iy,		SX,SY},	"16/9",		self, "__set_ratio_x", 16/9	)
	bu = self:add_trig_method(	{ix+SX*3, 	iy,		SX,SY},	"16/10",	self, "__set_ratio_x", 16/10	)
	iy = iy + SY
	
	bu = self:add_trig_method(	{ix,   		iy,		SX,SY},	"Swap",		self, "__swap_format"	)
	if b_screen then
		bu = self:add_trig_method(	{ix+SX,	iy,		SX,SY},	"Screen",	self, "__set_format_screen" )
	else
		bu = self:add_trig_method(	{ix+SX,	iy,		SX,SY},	"Fbo",		self, "__set_format_fbo"	)
	end
	bu = self:add_trig_method(	{ix+SX*2, 	iy,		SX,SY},	"4/3",		self, "__set_ratio_x", 4/3	)
	bu = self:add_trig_method(	{ix+SX*3, 	iy,		SX,SY},	"5/4",		self, "__set_ratio_x", 5/4	)
	self:end_bu_group()

	iy = iy + SY
	SY = SY * SYB
	local t = self:add_size_uvf( {ix,iy, sx,SY}, ref )
	t.ratio_sel = bu_sel_ratio
end



--	MULTIPLE
--
MEU.doc.add_multiple_render = "( rect, name )"
function MEU:add_multiple_render( rect, name )
	local ref =	self.ref
	local obj = ref.multiple

	rect[3] = rect[3] or 8
	rect[4] = rect[4] or 1
	name = name or "Multiple"
	local bu = self:add_selector(	rect, name )
		bu:set_nb( 3 )
		bu:set_item_text( 1, "Regular", "Multiple", "Both" )
		local par = param.get_ref( ref.multiple, "render" )
		bu:set_target_param( par )
		bu:set_text_draw( false )
 		--bu:set_target_lua( par )
 	return bu
end

--nb_line is 1 or 2

MEU.doc.add_multiple_size = "( rect, nb_line )"
function MEU:add_multiple_size( rect, nb_line )
	local ref =	self.ref
	local obj = ref.multiple

	--aaa.print_fn()
	--table.print( rect, "rect" )
	local ix,iy, sx,sy = unpack(rect)
	--aaa.print_fn()

	sx = (sx or 8) * (nb_line==2 and .5 or .25)
	sy = (sy or 2)
	--name = name or "Multiple"
	local oy = nb_line==2 and sy or 0
	local mx = nb_line==2 and {0,1,0,1} or {0,1,2,3}
	--local par = param.get_ref( ref.multiple, "size_u" )
	--self:box_debug( "param max is "..param.get_max(par).."\n infini is "..param.infini )

	local bu
	bu = self:add_slider(	{ix+sx*mx[1],iy,	sx,sy}, "Mul_u",	ref.multiple, "size_u"		):set_color_back( "u" )
	bu = self:add_slider(	{ix+sx*mx[2],iy,	sx,sy}, "Mul_v",	ref.multiple, "size_v"		):set_color_back( "v" )
	bu = self:add_slider(	{ix+sx*mx[3],iy+oy,	sx,sy}, "Mul_axe",	ref.multiple, "size_axe"	):set_color_back( "axe" )
	bu = self:add_slider(	{ix+sx*mx[4],iy+oy,	sx,sy}, "Mul_size",	ref.multiple, "size_factor" ):set_color_back( "factor" )
 	--bu:set_target_lua( par )
end

--togo gd a BU_SIZE
MEU.doc.add_multiple_uvf = "( rect, ref )"
function MEU:add_multiple_uvf( rect,	ref )
	ref = ref or self.ref.multiple
	if ref then
		rect = self:get_rect_using_def( rect, {9,5, 8,1} )
		local  rect, ref = self:__add_size_begin( rect,	ref )
		local  ix,iy, sx,sy = unpack(rect)
		local sxh = sx*.5
		self:__add_size( { ix, 		iy,		sxh,sy},	ref, "u",		0, 4. ):add_values_def_integer_half()
		self:__add_size( { ix+sxh,	iy,		sxh,sy},	ref, "v",		0, 4. ):add_values_def_integer_half()
		self:__add_size( { ix,		iy+1,	sx,sy},		ref, "factor",	0, 4. ):add_values_def_integer_half()
	end
end

--	MAPPING
--
function MEU:__get_mapping( ref )
	if not ref then
		ref = self.ref.mapping
		if not ref then
			--self:print_error( "get_mapping() no ref" )	--find fn name
		end
	end
	return ref
end

MEU.doc.add_blending = "( rect, ref )"
function MEU:add_blending( rect, ref )
	rect = self:get_rect_using_def( rect, {1,4.6, 2,1} )
	ref = self:__get_mapping( ref )
	if not ref then
		self:print_error( "MEU:add_blending() no ref" )
		return
	end

	local bu = self:add_button( rect, "mapping_blend", nil, nil, 3 )
 		bu:set_menu(
 			{
 			"Min", "Max", "Add", "Sub", "RSub",
 			"Mul", "Screen", "Overlay",
			"Darken", "Lighten", "ColDodge", "ColBurn", "HardLight", "SoftLight",
			"Diff", "Exclusion",
			"HSL_Hue", "HSL_Sat", "HSL_Col", "HSL_Luma"
			},
			"mapping_blend_menu" )
 		bu:get_selector():set_nb_min_0( 5, 4 )
		bu:set_text_selector(true)
		local par = param.get_ref( ref, "blend_equation" )
		bu:set_target_param( par )
		bu:set_draw_by_value( 2, "notice" )
 	return bu
end

MEU.doc.add_mapping_by_side_only = "( rect, ref )"
function MEU:add_mapping_by_side_only( rect, ref )
	ref = self:__get_mapping( ref )
	if not ref then
		self:print_error( "MEU:add_mapping_by_side_only() no ref" )
		return
	end

	local x,y, sx,sy = self:unpack_rect_using_def( rect, {1,4.6, 8,1} )
	
	param.set( ref, "tex_by_side", true )
	local bu
	local b_vert = sx<=4 
	if b_vert then
		sy = sy/2
	else
		sx = sx / 2
	end
	local r = {x,y,		sx,sy}
	bu = self:add_slider_two(	r,	"U Min Max",	ref, "tex_left", 	"tex_right",	0,1, 0,1 ):set_color_back("u")
	if b_vert then	r[2] = y + sy
	else			r[1] = x + sx
	end

	bu = self:add_slider_two(	r,	"V Min Max",	ref, "tex_bottom", 	"tex_top",		0,1, 0,1 ):set_color_back("v")
end

MEU.doc.add_mapping_by_side = "( rect, ref )"
function MEU:add_mapping_by_side( rect, ref )
	rect = self:get_rect_using_def( rect, {1,4.6, 8,1} )
	self:add_mapping_by_side_only( rect, ref )

	local r =  { rect[1], rect[2]+rect[4], rect[3], rect[4] }
	if r[3]<=4 then
		r[4] = r[4] / 2
		r[3] = r[3] / 2
	else
		r[3] = r[3] / 4
	end
	self:add_blending( r, ref )
end

MEU.doc.add_mapping = "( rect, ref )"
function MEU:add_mapping( rect, ref )
	ref = self:__get_mapping( ref )
	if not ref then
		self:print_error( "MEU:add_mapping() no ref" )
		return
	end

	local ix,iy, sx,sy = self:unpack_rect_using_def( rect, {1,5, 8,2} )
	sy = sy/2

	local sxo = sx/8*3
	local sxs = sx/8*3
	local st = 8 - sxo - sxs - sx/8
	local bu
	bu = self:add_param_obj_name(	{ix+st,		iy,	sxo,sy},	"origin_u",	ref, "tex_u_ori",	0,4 ):add_values_def_integer_half():set_color_back( "u" )
			:set_text( "Origin" ) 
	bu = self:add_param_obj_name(	{ix+st+sxo,	iy,	sxs,sy},	"U",		ref, "tex_u", 		0,5 ):add_values_def_integer_half()
	iy = iy + sy
	bu = self:add_param_obj_name(	{ix+st,		iy,	sxo,sy},	"origin_v",	ref, "tex_v_ori",	0,4 ):add_values_def_integer_half():set_color_back( "v" )
			:set_text( "Origin" ) 
	bu = self:add_param_obj_name(	{ix+st+sxo,	iy,	sxs,sy},	"V",		ref, "tex_v",		0,5 ):add_values_def_integer_half()
end

MEU.doc.add_mapping_and_blending = "( rect, ref )"
function MEU:add_mapping_and_blending( rect, ref )
	local ix,iy, sx,sy = self:unpack_rect_using_def( rect, {1,5, 8,3} )
	sy = sy/3
	self:add_mapping(	{ix,iy, sx,sy*2},  ref	)

	iy = iy + sy*2
	self:add_blending(	{ix,iy, sx/4,sy }, ref	)
end

MEU.doc.add_mapping_hexa = "( rect, ref )"
function MEU:add_mapping_hexa( rect, ref )
	ref = self:__get_mapping( ref )
	if not ref then
		self:print_error( "MEU:add_mapping_hexa() no ref" )
		return
	end

	local ix,iy, sx,sy = self:unpack_rect_using_def( rect, {1,14, 8,2} )
	sy = sy/3
	local sx3 = sx/3
	local bu
	bu = self:add_param_obj_name(	{ix,iy,			sy,sy},		"Hexa",		ref, "tex_hexa"		 )
		bu:set_text_rect_ratio(3)
	local ref_render = self.ref.rendering
	self:add_param_obj_name(		{ix+3,iy,		sy,sy}, 	"Top_line",	ref_render, "top_line" )
	iy = iy + sy
	bu = self:add_param_obj_name(	{ix,iy,			sx/2,sy},	"du",		ref, "tex_hexa_du",		0, 2	):set_color_back("u")
	bu = self:add_param_obj_name(	{ix+4,iy,		sx/2,sy},	"dv",		ref, "tex_hexa_dv",		0, 2	):set_color_back("v")
	iy = iy + sy
	bu = self:add_param_obj_name(	{ix,iy,			sx3,sy},	"offset_2",	ref, "tex_offset_2",	0, 5 	)
	bu = self:add_param_obj_name(	{ix+sx3,iy,		sx3,sy},	"offset_4",	ref, "tex_offset_4",	0, 5	)
	bu = self:add_param_obj_name(	{ix+sx3*2,iy,	sx3,sy},	"offset_6",	ref, "tex_offset_6",	0, 5	)
end

MEU.doc.add_mapping_texture_mode = "( rect, ref )"
function MEU:add_mapping_texture_mode( rect, ref )
	ref = self:__get_mapping( ref )
	local sx = rect[3]
	rect[3] =  sx/8* 3
	local bu = self:add_button(     rect,	"Mapping.Tex_Implicit",  ref,		"tex_implicit",    true				)
		:set_text( "Implicit" )
	self.ui.bu_tex_imp_use = bu

	rect[1] = rect[1] + sx/3
	bu = self:add_button( rect, "Mapping.Tex_Mode" )
		bu:set_menu( {	"Object", "Eye", "Sphere", "Reflexion",
						"Normal", "Camera"  } )
		bu:get_selector():set_nb_min_0( 2, 3 )
		bu:set_text_selector( true )		
		bu:set_target_param( param.get_ref( ref, "tex_mode" ) )
	self.ui.bu_tex_mode = bu

	self:register_update_ui( "__update_ui_tex_mode" )
end

function MEU:__update_ui_tex_mode()
	local ui = self.ui
	local val = ui.bu_tex_imp_use:get_value()
	local alpha_bu = ( val ) and 1 or 0.2
	ui.bu_tex_mode:interpolate_alpha_bu( alpha_bu )
end
--	GEO	
--
MEU.doc.add_nb_uv = "( rect, ref )"
function MEU:add_nb_uv( rect, ref )
	if not ref then
		ref = self:get_obj_down_by_class_no_error( "bdd_grid" )
		if not ref then
			self:box_error( "Can't find branch grid" )
			return
		end
	end

	local ix,iy, sx,sy = self:unpack_rect_using_def( rect, { 1,12, 8,1} )
	local bu
	sx = sx / 2
	bu = self:add_param_obj_name(	{ix,iy,			sx,sy}, 	"nb_u",	ref, "nb_u",  2, 200 ):set_text( "Nb U")
		bu:set_color_back("u")
		bu:add_values_def( 16, 32, 64, 128, 256 )
	bu = self:add_param_obj_name(	{ix+sx,iy,		sx,sy}, 	"nb_v",	ref, "nb_v",  2, 200 ):set_text( "Nb V")
		bu:set_color_back("v")
		bu:add_values_def( 16, 32, 64, 128, 256 )
end


--	RENDER
--
--todo find a way to make names like render.back to avoid potential conflicts
MEU.doc.add_rendering = "( rect, opts )"
function MEU:add_rendering( rect, opts ) -- opts is a table ref, pre
--	aaa.print_fn()

	local ref_rendering
	local pre
	local b_nor = true
	if opts then
		ref_rendering = opts.ref
		pre = opts.pre
	--	b_nor = opts.b_nor
	end
	if not ref_rendering then
		ref_rendering = self.ref.rendering
		if not ref_rendering then
			self:print_error( "have no ref.rendering : returning from add_rendering()" )
			return
		end
	end

	self:begin_bu_group( "render" )
	-- if not pre then
	-- 	pre = "render"
	-- end


	local ix,iy, sx,sy
	if rect then ix,iy, sx,sy = unpack(rect) end
	sy = sy or 2
	ix = ix or 1
	iy = iy or (16-sy)
	sx = sx or 8
	sy = sy * .5
	--self:print( "sx/sy "..sx.."/"..sy )
	local sxt = sx

	local function mn( name )	--make_name
		-- if pre then
		-- 	name = pre.."."..name
		-- end
		return name
	end
	--local d = .7
	local dy = .1
	local bu
	bu = self:add_button(	{ix,iy,		sy+.001,sy}, mn("Light"),	ref_rendering, "light", false )
		:set_text("Light")
		--bu:set_text_draw( true )
		--bu:set_text_xyf( -1.2,1, .5 )
		bu:set_text_xy_fxy( 0,.2, 1,.6 )
	bu = self:add_button(	{ix,iy+sy,	sy+.001,sy}, mn("Depth"),	ref_rendering, "depth", false )
		:set_text("Depth")
		--bu:set_text_draw( true )
		--bu:set_text_xyf( -1.2, -.4, .5 )
		bu:set_text_xy_fxy( 0,.2, 1,.6 )
	sx = sx - sy
	ix = ix + sy

	bu = self:add_selector(	{ix,iy,	sy*1.2,sy*2},	mn("Cull") )
		bu:set_nb_min_0( 1, 3 )
		bu:set_item_text( 1, "Back", nil, "Front" )
		bu:set_target_obj_param( ref_rendering, "cull" )
		bu:set_text_draw( false )
	sx = sx - sy*1.2 - dy
	ix = ix + sy*1.2
---[[
	local sxs = sx*3/8
	bu = self:add_selector(	{ix,iy,		sxs,sy},	mn("Back") )	--	no text : not save in preset
		bu:set_nb_min_0( 3 )
		bu:set_item_text( 1, "Fill", "Line", "Point" )
		bu:set_target_obj_param(  ref_rendering, "back_mode" )
		bu:set_text_draw( false )
	bu = self:add_selector(	{ix,iy+sy,	sxs,sy},	mn("Front") )	--	no text : not save in preset
		bu:set_nb_min_0( 3 )
		bu:set_item_text( 1, "Fill", "Line", "Point" )
		bu:set_target_obj_param( ref_rendering, "front_mode" )
		bu:set_text_draw( false )
	sx = sx - sxs - dy
	ix = ix + sxs

	local sx_left
	if b_nor then
		sx,sx_left = sx*2/3, sx/3
	end
	bu = self:add_slider(		{ix,iy,			sx,sy},			mn("Line"),		ref_rendering, "line_size",		1,	0, 32 )
		bu:add_values_def( .5, 1, 2, 4, 8, 16 )
	bu = self:add_slider(		{ix,iy+sy,		sx,sy},			mn("Point"),	ref_rendering, "point_size",	1,	0, 32 )
		bu:add_values_def( .5, 1, 2, 4, 8, 16 )

	if b_nor then
		bu = self:add_button(	{ix+sx,iy,		sx_left,sy},	mn("Normal Active"),	ref_rendering, "normal", false ):set_text( "Nor" )
		--	bu:set_text_draw( false )
		bu = self:add_slider(	{ix+sx,iy+sy,	sx_left,sy},	mn("Normal"),			ref_rendering, "normal_point",	1, 0,1 )
			bu:set_text( " " ) --todo do better to display just value
			--bu:add_values_def( 1, 2, 4, 8, 16, 32, 64 )
			param.set_save( ref_rendering, "normal_point", false )
		iy = iy + sy
	end

	self:end_bu_group()
--]]
end

--todo check if it good (rarely used too)
MEU.doc.add_rendering_size = "( rect, ref_rendering )"
function MEU:add_rendering_size( rect, ref_rendering )
	local ix,iy, sx,sy = self:unpack_rect_using_def( rect, {1,9, 4,.8} )
	local ref = self.ref
	if not ref.rendering then
		ref_rendering = self.ref.rendering
		if not ref_rendering then
			self:box_error( "have no ref.rendering : returning from add_rendering_size()" )
			return
		end
	end
	local bu
	bu = self:add_param_obj_name(	{ix,iy,			sx,sy},			"Line",		ref_rendering, "line_size",		0, 16 )
		bu:add_values_def( 1, 2, 4, 8 )
	bu = self:add_param_obj_name(	{ix,iy+sy,		sx,sy},			"Point",	ref_rendering, "point_size",	0, 256 )
		bu:add_values_def( .5, 1, 2, 4, 8, 16, 32, 64, 128, 256 )
--]]
end

function MEU:add_rendering_primitive( rect, name, rendering_ref )
	rendering_ref = rendering_ref or self.ref.rendering
	local bu = self:add_button( rect, name or "Primitive" )
		bu:set_menu( {	"Points", "Lines", "Line Loop", "Line Strip",
						"Triangles", "Triangle Strip", "Triangle Fan",
						"Quad", "Quad Strip", "Polygon"  } )
		bu:get_selector():set_nb_min_0( 2, 5 )
		bu:set_text_selector( true )		
		bu:set_target_param( param.get_ref( rendering_ref, "primitive" ) )
	return bu
end


--	Incrust
--
MEU.doc.add_incrust = "( rect, o )"
function MEU:add_incrust( rect, o )
	local ix,iy,sx,sy = unpack(rect)
	sx = (sx or 8) / 8
	sy = sy / 3
	local bu
	bu = self:add_param_obj_name(	{ix,iy,			sx*3,sy},	"Incrust",	o, "incrust_luma" 		)
	--	bu:set_text_rect_ratio( 3 )
	self:add_param_obj_name(		{ix+sx*3,iy,	sx*2,sy},	"Inv",		o, "incrust_inverse" 	)
	iy = iy + sy
	self:add_slider_two(			{ix,iy,			sx*8,sy},	"Levels",	o, "incrust_min", "incrust_max", 0, 1, 0, 1 )
		:set_draw_step( "linear" )
	iy = iy + sy
	self:add_param_obj_name(		{ix,iy,			sx*4,sy},	"Gain",		o, "incrust_gain",	0,1 )
	self:add_param_obj_name(		{ix+sx*4,iy,	sx*4,sy},	"Bias",		o, "incrust_bias",	0,1 )
	return iy + sy
end

--todo move to MEU file and generalize
--bu_seq need to be more protected
function MEU:check_need_define_seq_ui( seqs )
	if seqs.__b_hack_define_ui then
		--self:define_ui()
		if self.bu_seq then
			self:get_bus():remove_bu(self.bu_seq)
			self.bu_seq = nil
		end
		self.__b_need_seqs_define_ui = true
		seqs.__b_hack_define_ui = nil
	elseif self.__b_need_seqs_define_ui then
		self:define_seq_ui(self:get_bus())
		self.__b_need_seqs_define_ui = nil
	end
	return seqs
end

MEU.doc.add_blur_selector = "( rect, name, table, field_name, val )"
function MEU:add_blur_selector( rect, name, table, field_name, val )
	local bu = self:add_selector( rect, name )
		bu:set_nb( 4, 1 )
		bu:set_target_lua( table, field_name, val )
		bu:set_item_text( 1, "No", "Blur", "Gauss", "Median" )
		bu:set_draw_by_value( 1, "false" )
	return bu
end

MEU.doc.add_range_rgb_xyz = "( rect )"
function MEU:add_range_rgb_xyz( rect )
	rect[4] = rect[4]/3
	local X,SX = rect[1], rect[3]
	local t = {}
	local function add( color, color_back )
		local cl = color:lower()
		t[cl] = {}
		local str = "Range "..color
		local SXB = 1.5
		rect[1],rect[3] = X,SXB
		self:add_button(		rect,	str.." Inv",	t[cl],	"b_inv", false )
			:set_text( "Inv" ) 
		rect[1],rect[3] = X+SXB,SX-SXB
		self:add_slider_two(	rect,	str, 			t[cl],	"min",	"max",		0,8,	-8,8 )
			:add_values_def(-4,-1,0,1,4):set_color_back( color_back )
		rect[2] = rect[2] + rect[4]
	end
	add( "Red",		"x")
	add( "Green",	"y" )
	add( "Blue",	"z" )
	self.__range_rbg_xyz = t
end

function MEU:update_range_rgb_xyz( sha )
	local t = self.__range_rbg_xyz
	local function update_color( t )
		local min,max
		if t.b_inv then	min,max = t.max,t.min
		else			min,max = t.min,t.max
		end
		t.offset = -min
		t.factor =  1. / ( max	- min )
	end
	update_color( t.red )
	update_color( t.green )
	update_color( t.blue )
	sha:set_frag_float_2_7( t.red.offset,t.red.factor,	t.green.offset,t.green.factor,	 t.blue.offset,t.blue.factor	  )
end

MEU.doc.add_midi_button = "( irect, name, nbu,nbv, b_text_copy )"
function MEU:add_midi_button( irect, name, nbu,nbv, b_control )
	return bus_cur:add_midi_button( name, self:make_rect_from_irect(irect), nbu,nbv, b_control )
end
	

MEU.doc.add_midi_button_controller	= "( rect, name )"
MEU.doc.add_midi_button_channel 	= "( rect, name )"
function MEU:add_midi_button_controller(	rect, name )	return self:add_midi_button( rect, name, 16,8, true )	end
function MEU:add_midi_button_channel(		rect, name )	return self:add_midi_button( rect, name, 2,8 )	end


MEU.doc.add_bu_sequence = "( bus, seqs )"
function MEU:add_bu_sequence( bus, seqs )
	--aaa.box_warning( "define_seq_ui" )
	local bus_seq = seqs:define_bus_seqs()
	local s = 1.8
	local bu = bus:add_window( "Seq", bus_seq,	{-.25, -.025, .25 * s, .5 * s} )
	bus_seq:set_transfo( 1,2, .1,-s/2 )
	return bu
end