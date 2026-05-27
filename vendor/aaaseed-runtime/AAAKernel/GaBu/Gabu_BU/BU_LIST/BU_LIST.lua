if CLASS.DECLARE( "BU_LIST", BU, {} ) then
	BU_LIST:set_class_status_doc(	CLASS.STATUS.GABU,
									"display a list with scroll" )
end

function BU_LIST:define_ui()
	local bus = BUS:create( self:get_name().."_list" )
	bus:init_begin()

--		self:adjust_nb( bus, 16 )

		-- bu = bus:add_text( "Info", {0,3.4, 7,.6} )
		-- 	bu:set_ui_active( false )
		-- 	bu:set_text_nice()
		-- 	self.bu_info = bu

	bus:init_end()
	return bus
end

function BU_LIST:create( name, rect )
	local self = BU_LIST:create_instance( name, rect )

	self.__items = {}
	self.__item_nb = 0
	self.__items_nb_allocated = 0

	local bus = self:define_ui()
	bus.__b_hack_maa = true
	self:add_bus_down( bus )

	self.__y_min = 0
	self.__y_max = 0
	self:set_offset_xy( 0,0 )

	return self
end


function BU_LIST:adjust_nb( nb_asked )
	--aaa.print_fn()
	local items = self.__items
	local bus = self:get_bus_down()
	--self:print( "bus get "..self:get_bus_down() )

	local nb_alloc	= self.__items_nb_allocated
	if nb_alloc < nb_asked then
		for i=nb_alloc+1, nb_asked do
			local bu = BUTTON:create( "item_"..i, {0,0, .2,.1} )
			bu:set_trig()
			bu:set_text_nice()
			table.insert( items, bu )
			self:__set_item_pos( i )
			--bu:set_dplane( 42 )
		end
		nb_alloc = nb_asked
		self.__items_nb_allocated = nb_asked
	end

	-- local nb	= self.__items_nb
	-- for i=1,nb_asked			do	items[i]:set_active( true )		end
	-- for i=nb_asked+1,nb_alloc	do	items[i]:set_active( false )	end

	if self.__items_nb ~= nb_asked then
		self.__items_nb = nb_asked
	end
	self:set_offset_xy( 0,0 )
	self:update_visibility( true )
	--self.slider_pos_last = 0

end

function BU_LIST:get_bu(i)
	return self.__items[i]
end

function BU_LIST:__set_item_pos( id )
	local bu = self.__items[id]
	id = id - 1
	local ix = id % self.__nb_x
	local iy = (id-ix) / self.__nb_x
	bu:set_xy_sxy(  self.__left+ix*self.__dx,self.__top-iy*self.__dy, self.__sx,self.__sy )
end
function BU_LIST:set_nb_xy( nb_x, nb_y )
	local MARGIN = .01
	local items = self.__items
	local nb = self.__items_nb_allocated
	local sx = (1-(math.floor(nb_x)+1)*MARGIN) / nb_x
	self.__sx = sx
	local sy = (1-(math.floor(nb_y)+1)*MARGIN) / nb_y
	self.__sy = sy
	self.__left = -1/2 + MARGIN + sx/2
	local y = 1/2 - sy/2
	self.__top = y - MARGIN
	self.__dx, self.__dy = sx + MARGIN, sy + MARGIN
	self.__y_max = y
	self.__y_min = -y
	self.__nb_x = nb_x
	self.__nb_y = nb_y

	for i=1,nb do
		self:__set_item_pos( i )
		-- local bu = items[i]
		-- bu:set_xy_sxy(  x,y, sx,sy )
		-- x = x + dx
		-- if x > r then
		-- 	x = l
		-- 	y = y - dy
		-- end
	end

--	self:update_visibility( true )
end

function BU_LIST:update_visibility( b_force )
	-- if b_force then
	-- 	aaa.print_fn()
	-- end
	local bus = self:get_bus_down()
	local nb = self.__items_nb
	if nb==0 then
		bus:remove_bu_all()
		return
	end

	local _, y_offset_cur    = self:get_offset_xy_cur()
	local _, y_offset_target = self:get_offset_xy_target()
	if not b_force then --then we check if inertia still require update
	 	local dif = y_offset_target - y_offset_cur
	 	b_force = math.abs(dif) > .001
		-- if b_force then
		-- 	self:print( "delta is "..dif.." "..b_force )
		-- end
		if not b_force then
		 	return
		end
	end
	--self.__y_offset_last = y_offset

	local items = self.__items
	local ox,oy = self:get_offset_xy_cur()
	local bus = self:get_bus_down()
	oy = oy * 2.  --todo why 2 we got a problem   : we composante to an offset done the wrong way

	local margin_for_ui = .0
	local t = self.__y_max - oy + margin_for_ui
	local b = self.__y_min - oy - margin_for_ui
	local y_beg = items[1]:get_y()
	local y_end = items[nb]:get_y()
	if b_force or y_beg >= t or y_end <= b then
		--self:print( "Do it with nb "..nb )
		bus:remove_bu_all()
		for i=1,nb do
			local bu = items[i]
			local y = bu:get_y()
			--self:print( i.." - b y t "..b.." "..y.." "..t )
			if b <= y and y <= t then
				bus:add_bu(bu)
			end
		end
	end
end

function BU_LIST:get_offset_xy_cur()
	local bus = self:get_bus_down()
	return bus.__x_offset_exp, bus.__y_offset_exp
end
function BU_LIST:get_offset_xy_target()
	local bus = self:get_bus_down()
	return bus.__x_offset_target, bus.__y_offset_target
end
function BU_LIST:set_offset_xy( x,y )
	local bus = self:get_bus_down()
	if x then bus.__x_offset_target = x end 	
	if y then bus.__y_offset_target = y end
end

function BU_LIST:scroll_v( inc )
	--aaa.print_fn()

	local nb = self.__items_nb
	if nb==0 then return end

	local items = self.__items
	local ox,oy = self:get_offset_xy_target()
	local roy = oy * 2.  --todo why 2 we got a problem   : we composante to an offset done the wrong way

	local t = self.__y_max - roy
	local y_beg = items[1]:get_y()
	--self:print( "y_beg is "..y_beg )
	if y_beg < t and (inc < 0) then
	 	return
	end 

	local b = self.__y_min - roy
	local y_end = items[nb]:get_y()
	if y_end > b and (inc > 0) then
	 	return
	end

	local d = inc * .0005 * nb
	self:set_offset_xy( nil, oy + d ) 
	

	--local slider_pos_last = self.slider_pos_last
	
--	--local slider_pos = self.slider_posself.slider_pos_las
	

-- 	local t = self.__y_max
-- 	local b = self.__y_min
-- 	if y_beg > t or y_end < b then
-- 		local dy = self.__dy 
-- 		for i=1,nb do
-- 			local bu = items[i]
-- 			local y = bu:get_y()
-- 			bu:set_y( y + d * dy )
-- 		end
-- 		--todo Search why the following code is not equivalent to slider_pos_last = slider_pos
-- --		self.slider_pos_last = self.slider_pos_last
-- --		local slider_pos = slider_pos_last + d
-- 	end

	self:update_visibility()
end

function BU_LIST:draw_back()
	self:set_color_back( {0,0,0,.6 } )
	oo.getsuper(BU_LIST).draw_back(self)
end

--todo update visibility should be in an update not a draw ?
function BU_LIST:draw()
	self:update_visibility()
	oo.getsuper(BU_LIST).draw(self)
end
