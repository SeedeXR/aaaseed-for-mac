--Old stuff
--todo should be replace by new stuff below 
--	TRS
function MEU:add_trz( rect )
	local ref = self.ref
	local ui = self.ui
	if ref.trs then
		local ix,iy, sx,sy = self:unpack_rect_using_def( rect, {9,12.5, 8,2} )
		sy = sy / 2
		local bu
		local function add_param( ox,oy, sx,	bui_name, ref_param, str_color )
			bu = self:add_param(	{ix+ox,iy+oy,		sx,sy}, bui_name, ref_param, -6., 6. )
			bu:set_meter( false )
			bu:add_values_def( 0, 1, 2, 3, 4, -4, -3, -2, -1 )
			bu:set_color_back( str_color )
			return bu
		end
		--todo xyz typical use xyz we should
		local sx3 = sx / 3
		--ui.bu_tra_x = 
		add_param( 0,		0,	sx3,	"Tra X", ref.trs_tra_x, "x"	)
		--ui.bu_tra_y =
		add_param( sx3, 	0,	sx3,	"Tra Y", ref.trs_tra_y, "y"	)
		--ui.bu_tra_z =
		add_param( sx3*2, 	0,	sx3,	"Tra Z", ref.trs_tra_z, "z"	)
		bu = self:add_param(	{ix,iy+sy,	 sx,sy},	"Rot Z", ref.trs_rot_z, -1., 1. )
			bu:set_meter( false )
			bu:add_values_def( 0, .25, .5, 0.75, .125, .375, .625, .875 )
			bu:set_color_back( "z" )
	else
		aaa.box_error( self.." don't have a trs in ref" )
	end
end

function MEU:add_trzs( rect )
	local ref = self.ref
	if ref.trs then
		--todo better
		local ix,iy, sx,sy = self:unpack_rect_using_def( rect, {9,12.2, 3,2} )
		sy = sy / 3
		self:add_trz( { ix,iy, sx,sy*2 } )
		local bu = self:add_param(	{ix,iy+sy*2,	 sx,sy}, "Sca",  ref.trs_sca, .0, 5. )
			bu:add_values_def( .5, 1., 2. )
	else
		aaa.box_error( self.." don't have a trs in ref" )
	end
end

-- NEW Stuff (2024 Feb)
--
function MEU:__define_trs_one( rect, id )
	local ref = self.ref

	local function add( name, tab, max )
		local r = table.copy_simple( rect )
		local x,y,z = self:add_sliders_xyz(	r,	name,	tab, 		false,	max	)
		rect[2] = rect[2] + rect[4]
		return {x,y,z}
	end

	local tbu = self.__transfo.bu
	--table.print( rect, "rect", 1 )
	local tab = { name="transfo"..id } 
	self:begin_bu_group( tab.name )
		
		tab.tra = {}
		local t = add( "Tra",		tab.tra, 	5	)
		for _,bu in ipairs(t) do table.insert( tbu.tras, bu ) end
		
		tab.rot = {}
		t = add( "Rot",		tab.rot, 	1	)
		for _,bu in ipairs(t) do table.insert( tbu.rots, bu ) end

		tab.sca = {}
		local sx = rect[3] / 4 
		rect[3] = sx
		table.insert( tbu.scas, self:add_slider( rect,	"Sca_f", 	tab.sca, 	"f",	1,	0,8 ):set_color_back( "factor" )	)
		rect[1] = rect[1] + sx
		table.insert( tbu.scas, self:add_slider( rect,	"Sca_x", 	tab.sca, 	"x",	1,	0,8 ):set_color_back( "x" )			)
		rect[1] = rect[1] + sx
		table.insert( tbu.scas, self:add_slider( rect,	"Sca_y", 	tab.sca, 	"y",	1,	0,8 ):set_color_back( "y" )			)
		rect[1] = rect[1] + sx
		table.insert( tbu.scas, self:add_slider( rect,	"Sca_z", 	tab.sca, 	"z",	1,	0,8 ):set_color_back( "z" )			)
	
	self:end_bu_group()

	return tab, rect[2] + rect[4]
end

function MEU:__define_trs( rect, i_begin, i_end )
	local ref = self.ref	
	local t = {}
	local iy
	for i=i_begin,i_end do
		local r = table.copy_simple( rect )
		t[i], iy = self:__define_trs_one( r, i )
		--rect[2] = r[2] + .2
	end
	return t, iy + .2
end

function MEU:__update_ui_transfo()
	local transfo = self.__transfo 
	local tbu = transfo.bu
	local b = tbu.trs:get_value_as_bool()
	local a = tbu.tra:interpolate_alpha_bu( b )
	tbu.rot:set_alpha_bu( a )
	tbu.sca:set_alpha_bu( a )

	local t = tbu.tras
	a = t[1]:interpolate_alpha_bu( b and tbu.tra:get_value_as_bool() )
	for i=2,#t do t[i]:set_alpha_bu( a ) end
	local t = tbu.rots
	a = t[1]:interpolate_alpha_bu( b and tbu.rot:get_value_as_bool() )
	for i=2,#t do t[i]:set_alpha_bu( a ) end
	local t = tbu.scas
	a = t[1]:interpolate_alpha_bu( b and tbu.sca:get_value_as_bool() )
	for i=2,#t do t[i]:set_alpha_bu( a ) end
end

function MEU:add_transfo( rect, nb )
	nb = nb or 1
	local rect_def =  { 9,2.2,	8,3.2 }
	if not rect then
		rect = rect_def
	else
		rect[1] = rect[1] or rect_def[1]
		rect[2] = rect[2] or rect_def[2]
		rect[3] = rect[3] or rect_def[3]
		rect[4] = rect[4] or rect_def[4]
	end
	local ix,iy = rect[1],rect[2]
	local SY = rect[4]/3
	local DY = .1

	local ref = self.ref
	local bu

	local layers = self:get_layers()
	ref.layers_trs_use = param.get_ref( layers, "transfo" )
	local trs_ref = aaa.obj.get_branch_by_class_no_error( layers, "transfo_trs" )
	if not trs_ref then
		-- force a minmal ipdate to tranfo object will defined
		local prev = param.get_bool(ref.layers_trs_use)
		param.set( ref.layers_trs_use, true )
		self:draw_layers_begin()
		self:draw_layers_end()
		param.set( ref.layers_trs_use, prev )
		trs_ref = aaa.obj.get_branch_by_class_no_error( layers, "transfo_trs" )
	end
	--aaa.box_debug( "transfo is "..trs_ref )
	ref.trs = TRANSFO_TRS:create( "trs for Meu", trs_ref )
	ref.trs:set_active( true )
--	local trs = aaa.obj.get_branch_by_class( layers )

	local transfo = { bu = { tras = {}, rots = {}, scas = {} } }
	self.__transfo = transfo
	local tbu = transfo.bu

	local SX = 1.2
	local SXT = 2.
	local rect_selector
	self:begin_bu_group( "transfo" )

		tbu.trs = self:add_button( {ix,iy, 	SX,SY}, "Transfo", ref.layers_trs_use, nil,	false )
			:set_text( "TRS" )
			:set_text_inside( true )

		transfo.nb = nb
		transfo.trs = ref.trs
		if nb > 1 then
			rect_selector = {ix+2,iy,	6,SY}
			bu = self:add_selector(	rect_selector,	"Transfo Id"	)
				bu:set_target_lua( transfo, "id_sel" )
				bu:set_nb( transfo.nb )
				bu:set_item_text_from_nb()
				bu:set_method_on_value_change( self, "__change_transfo_id" )
			iy = iy + SY
			self:add_button( {ix+1,iy,	3,SY},	"Interpolate",		transfo, "b_interpolate",	false	)
			self:add_slider( {ix+4,iy,	4,SY},	"Inter", 			transfo, "interpolate", 	1,		0,1 )
			iy = iy + SY
		end
		
		tbu.tra = self:add_button( {ix+SX,iy,		SXT-SX,SY},	"Tra",		trs_ref,"translate",		false	):set_text_inside( true )
		tbu.rot = self:add_button( {ix+SX,iy+SY,	SXT-SX,SY},	"Rot",		trs_ref,"rotate",			false	):set_text_inside( true )
		tbu.sca = self:add_button( {ix+SX,iy+SY*2,	SXT-SX,SY},	"Sca",		trs_ref,"scale",			false	):set_text_inside( true )
		

	self:end_bu_group()

	-- this where the sliders are defined
	transfo.transfos, iy = self:__define_trs( {ix+SXT,iy, 8-SXT,SY}, 1,transfo.nb )

	transfo.bu = tbu

	if nb > 1 then
		self:__change_transfo_id()
	end

	self:register_update_ui( "__update_ui_transfo" )
end

function MEU:__change_transfo_id()
	local transfo = self.__transfo
	if transfo then
		local id_sel = transfo.id_sel
		for i=1,transfo.nb do
			local tr = transfo.transfos[i]
			self:set_bu_group_active( tr.name, i==id_sel )
		end
	end
end

function MEU:add_transfo_tab( rect, nb )
	self:set_tab_key( "TRS" )
	self:add_transfo( rect, nb )
end

local function interpolate_xyz( t1, t2, v )
	return	{
				x = interpolate( t1.x, t2.x, v ),
				y = interpolate( t1.y, t2.y, v ),
				z = interpolate( t1.z, t2.z, v )
			}
end
local function interpolate_transfo( trs_1, trs_2, v )
	
	local t = {
		tra = interpolate_xyz( trs_1.tra, trs_2.tra, v ),
		rot = interpolate_xyz( trs_1.rot, trs_2.rot, v ),
		sca = interpolate_xyz( trs_1.sca, trs_2.sca, v )
	}
	t.sca.f =  interpolate( trs_1.sca.f, trs_2.sca.f, v )
	return t
end

function MEU:__update_transfo( transfo )
	local ref = self.ref

	local inter
	local n
	local b_several = transfo.nb > 1
	local transfo_1 = transfo.transfos[1]
	if b_several then
		if transfo.b_interpolate then
			inter = transfo.interpolate
			n = interpolate_transfo( transfo.transfos[1], transfo.transfos[2], inter )
		else
			n = transfo.transfos[transfo.id_sel]
		end
	else
		n = transfo_1
	end
	
	
	local dst = transfo.trs
	local t = n.tra
	--tdo last argumnent commented should have a strategy settled
	dst:set_translate( t.x,t.y,t.z )	-- transfo_1.tra.b )
	t = n.rot
	dst:set_rotate(    t.x,t.y,t.z )	 --t.b )
	t = n.sca
	dst:set_scale(     t.x*t.f,t.y*t.f,t.z*t.f)	 --t.b )
end