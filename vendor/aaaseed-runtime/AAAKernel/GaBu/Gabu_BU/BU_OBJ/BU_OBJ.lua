if CLASS.DECLARE( "BU_OBJ", BU ) then
	BU_OBJ:set_class_status_doc(	CLASS.STATUS.GABU,
									"a BU associated with a c obj_ui, probably depreciated to a BU property" )
end

--todo check of it make sense to merge with BU:draw_obj() below
function BU_OBJ:draw()
--[[
	gol.push_matrix()
--todo	gol.push_attrib()
	for i=1, #self.__objs do
		local obj = self.__objs[i]
		--self:print( "update_then_draw )
		aaa.obj.update_then_draw( obj.ref )
	end
--todo	gol.pop_attrib()
	gol.pop_matrix()

	oo.getsuper(BU_OBJ).draw(self)
--]]
end

function BU_OBJ:add_obj( obj_or_name )
	local ref
	local name
	if type(obj_or_name) == "string" then
		ref = aaa.obj.get_by_name_symbo( obj_or_name )
		name = obj_or_name
	else
		ref = obj_or_name
	end
	if ref then
		table.insert( self.__objs, { name = name, ref = ref } )
	end
end

function BU_OBJ:create( name, rect, names )
	local self = BU_OBJ:create_instance( name, rect )
 
	self:set_ui_top_size( true )

	self.__objs = {}
	if names then
		for i=1, #names do
			self:add_obj( names[i] )
		end
	end

	return self
end

function BU_OBJ:create_from_list( name, names )
	return BU_OBJ:create( name, nil, names )
end



	--

function BU:attach_obj( ref_draw, ox,oy, sx,sy, ref_focus  )	-- or name

	local ref = self:check_ref( ref_draw )
	if not ref then
		self:box_error( "Can't attach "..ref_draw )
		return
	end
	if ref_focus then
		ref_focus = self:check_ref( ref_focus )
	end

	self.obj = { ref = { draw = ref_draw, focus = ref_focus } }
	if ref_focus then
		aaa.bdd.set_ui_intercept( ref_focus, false )
	end

--	TRANSLATE
	ox = ox or 0
	oy = oy or 0
	if ox~=0 or oy~=0 then
		self.obj.tra = { ox=ox, oy=oy }
	end

--	SCALE
	sx = sx or 1
	sy = sy or 1
	if sx~=1 or sy~=1 then
		self.obj.sca = { sx=sx,sy=sy, sx_over = 1./sx, sy_over = 1./sy }
	end
end

function BU:draw_obj()
	if GA.b_spy then aaa.spy.push_range( self..":draw_obj()", 4 ) end

		local obj = self.obj
		--if not obj then return end
		local ref = obj.ref

		local sca = obj.sca
		local tra = obj.tra
		if sca then	gol.scale(		sca.sx_over,	sca.sy_over )	end
		if tra then	gol.translate(	tra.ox,			tra.oy )		end

			aaa.obj.update_then_draw( ref.draw )

		if tra then	gol.translate(	-tra.ox,		-tra.oy )		end
		if sca then	gol.scale(		sca.sx,			sca.sy )		end

		local bdd = ref.focus
		local b_crosshair = false
		--aaa.print_fn( "tutu"..self.__crosshair_u )
		if bdd then
			if ga:is_ui_intercept( bdd ) and not aaa.keyboard.is_alt_only() then
				gol.color_green( .5 )
				gol.set_line_width( 2 )
				if self.__crosshair_u then
					param.set( bdd, "crosshair_u", self.__crosshair_u + .5 )
					param.set( bdd, "crosshair_v", self.__crosshair_v + .5 )
					b_crosshair = true
				end
			else
				gol.color_cyan( .5 )
				gol.set_line_width( 1 )
			end
		end
		self:gol_draw_frame_rect()
		param.set( bdd, "crosshair_draw", b_crosshair )

	if GA.b_spy then aaa.spy.pop_range() end
end