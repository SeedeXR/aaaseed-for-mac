if CLASS.DECLARE( "SLIDER_MULTI_CURVE", SLIDER_MULTI ) then
		SLIDER_MULTI_CURVE:set_class_status_doc(	CLASS.STATUS.GABU,
									"like SLIDER_MULTI but specialized to describe a y = f(x) curve"  )
end

--used for debugging of MEU ColorCurve
-- function SLIDER_MULTI_CURVE:set_values( tab )
-- 	local name = self:get_name_lowercase()
-- 	if name=="curve_1" or name=="curve_2" then
-- 		table.print( tab, self.." SLIDER_MULTI_CURVE values table", 3 )
-- 		aaa.debug.print_traceback()
-- 	end
-- 	oo.getsuper(SLIDER_MULTI_CURVE).set_values( self, tab )
-- end

function SLIDER_MULTI_CURVE:draw_curve_after( )
--	self:print( "draw_curve_after" )
	gol.color_white()
	gol.set_line_width( 2 )
	local elt = self:get_elt( 1 )
	if elt then
		local x2,y2 = elt:get_xy()
		local x1,y1 = 0, y2
		aaa.draw_line( x1, y1, x2, y2 )
		for i=2,self:get_elt_nb() do
			x1,y1 = x2,y2
			elt = self:get_elt( i )
			x2,y2 = elt:get_xy()
			aaa.draw_line( x1,y1, x2,y2 )
		end
		x1,y1 = x2,y2
		x2 = 1.
		aaa.draw_line( x1, y1, x2, y2 )
	end
end

function SLIDER_MULTI_CURVE:get_curve_value( x_in )
	local i1 = 1
	local e1 = self:get_elt( i1 )
	local x1,y1 = 0,0
	if e1 then x1,y1 = e1:get_xy() end

	local i2 = self:get_elt_nb()
	local e2 = self:get_elt( i2 )
	local x2,y2 = 1,1
	if e2 then x2,y2 = e2:get_xy() end

	if x_in < x1 then x_in = x1 end
	if x_in > x2 then x_in = x2 end

	if e1 == e2 then
		return y1
	else
		repeat
			local i = math.floor( (i1 + i2) * .5 )
			if i == i1 then
				break
			end
			local e = self:get_elt( i )
			local x,y = e:get_xy()

			if x_in < x then
				--self:print( x_in.." < "..i.." "..x )
				e2 = e
				i2 = i
			else
				--self:print( x_in.."   "..i.." "..x )
				e1 = e
				i1 = i
			end
		until false

		x1,y1 = e1:get_xy()
		x2,y2 = e2:get_xy()

		return (x_in-x1) * (y1-y2) / (x1-x2) + y1, (x_in-x1) / (x2-x1), e1, e2
	end
end

local function __draw_elt( bu )
	gol.color_cyan( .4 )
	aaa.draw_mul_line(	0,0, .5 )
end


function SLIDER_MULTI_CURVE:create_elt( id, bus, rect )
	local bu = oo.getsuper(SLIDER_MULTI_CURVE).create_elt( self, id, bus, rect )
	bu.draw = __draw_elt
	return bu
end

function SLIDER_MULTI_CURVE:create(	name,	rect,	nb,	b_more )
	local self = SLIDER_MULTI_CURVE:create_instance( name, rect )
	self:__init_elt_nb( name, nb )

	self:set_mobile( false )
	self:set_ui_top(false)
	local rx = self:get_ratio_x()
	local e_sy = .2
	local e_sx = e_sy * self:get_ratio_y()
	self:set_elt_sxy( e_sx, e_sy )

	local bus_down = self:get_bus_down()
	bus_down:set_method( "draw_after", self, "draw_curve_after" )

	if b_more then
		local bus_more = self:define_more( name.."_down", rect[3],rect[4] )
		--bu:set_dplane(42)
	end
	return self
end

function SLIDER_MULTI_CURVE:update_elt_name()
	for i,elt in IPAIRS(self.__elts) do
		elt:set_text( tostring(i) )
	end
end

function SLIDER_MULTI_CURVE:update()
	oo.getsuper(SLIDER_MULTI_CURVE).update(self)
	self:sort_elt_by_x()

	--self:update_elt_name()
	--local elt_nb = self:get_elt_nb()
	-- local nb_last = self.__elt_nb_last or 0
	-- if nb_last ~= elt_nb then
	-- 	self:print( "change of elt nb : "..nb_last.." to "..elt_nb ) 
	-- 	local e_sx = .05
	-- 	local e_sy = e_sx * self:get_ratio_x()
	-- 	for i = 1, elt_nb do
	-- 		self:set_elt_sxy( e_sx, e_sy, i )
	-- 	end
	-- 	self.__elt_nb_last = elt_nb
	-- end
end

-- function SLIDER_MULTI_CURVE:add_elt( add_nb )
-- 	local nb_before = self:get_elt_nb()
-- 	oo.getsuper(SLIDER_MULTI_CURVE).add_elt( self, add_nb )
-- 	local nb = self.__elt_nb
-- 	local e_sx = .05
-- 	local e_sy = e_sx * self:get_ratio_x()
-- 	for i=nb_before+1,self:get_elt_nb() do
-- 		self:set_elt_sxy( e_sx, e_sy, i )
-- 	end
-- end

