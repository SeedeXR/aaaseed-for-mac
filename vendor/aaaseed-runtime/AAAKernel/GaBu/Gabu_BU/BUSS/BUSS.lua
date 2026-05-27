--BUSS
--todo BUSS should be a BU containting several BUS
if CLASS.DECLARE( "BUSS" ) then
	--todo will have to be recommented
	BUSS:set_class_status_doc(	CLASS.STATUS.GABU,
								"When App are defined, under the GA control (see GA:add_app()),",
								"several BUSS are open one after another (regular/top/top_top)",
								"In fact BUSS regroup several Bus and maintain an order of prioritity for their update and/or draw",
								"bus are added to BUSS using the ga (e.g. ga:add_bus(bus) )" )
end

function BUSS:create( name )
	local self = BUSS:create_instance( name )
	self:set_active_direct( true )
	self.__bus_active = {}
	self.__bus_active_nb = 0
	return self
end

function BUSS:update_bus_active()
	-- local timer = TIMER:create( "Tmp" )
	-- timer:start()

		local nb = self:get_down_nb()
		local nb_out = 0
		if nb > 0 then
			local down = self:get_down_array()
			--if nb == 7 then
			--	table.print( down, nil, 2 )
			--end
			local tab = self.__bus_active
			for i = 1,nb do
				local bus = down[i]
				if bus:is_active() then
					nb_out = nb_out + 1
					tab[nb_out] = bus
				end
			end
		end
		self.__bus_active_nb = nb_out

	-- timer:store()
	-- aaa.show( nb.."/"..nb_out.." in "..timer:get_ms_str(), "BUSS|"..self:get_name() )
	--2024 july : 0.1ms for 1000 elts 
	if self:get_name() == "of_GA_top" then 
		aaa.show( nb.."/"..nb_out, "BUSS|"..self:get_name() )
	end
end

function BUSS:apply_bus_active( ... )
	array.apply_begin_end_fn( self.__bus_active, 1,self.__bus_active_nb, ... )
end
function BUSS:apply_bus( ... )
	array.apply_begin_end_fn( self:get_down_array(), 1, self:get_down_nb(), ... )
end

function BUSS:update()
	--todo do like bus
	if self.verbose >= 3 then aaa.print_method() end

	if GA.b_verbose_render then self:print_inverse( "\tupdate() begin" ) end
	local fn_table = self.__fn_table
	if fn_table then self:do_fn( "update_before" ) end

		self:update_bus_active()	
		self:apply_bus_active( BUS.update )

	if fn_table then self:do_fn( "update_after" ) end
	if GA.b_verbose_render then self:print_inverse( "\tupdate() end" ) end
end

function BUSS:draw()
	if not self:is_active() then
		return
	end

	if not BU.draw or not BU.update_video then return end
	if self.verbose >= 3 then aaa.print_method() end

	if GA.b_spy then aaa.spy.push_range( self.." draw()", 2 ) end
	if GA.b_verbose_render then self:print_inverse( "\tdraw() begin" ) end

		local fn_table = self.__fn_table
		if fn_table then self:do_fn( "draw_before" ) end

			self:apply_bus_active( BUS.draw, true )
		
		if fn_table then self:do_fn( "draw_after" ) end

	if GA.b_verbose_render then self:print_inverse( "\tdraw() end" ) end
	if GA.b_spy then aaa.spy.pop_range() end
end

function BUSS:deal_with_untouch( blobs )
	local nb = blobs:get_untouch_nb()
	if nb <= 0 then return false end
	if self.verbose >= 3 then self:print( "deal_with_untouch() there is "..nb.." untouch" ) end

	local untouch_used = 0
	for i_untouch = 1, nb do
		local blob_id = blobs:get_untouch_id(i_untouch)
		if self.verbose >= 2 then self:print( "analyse_touch of blob "..blob_id ) end
		if self:test_down_reverse( BUS.deal_with_untouch, blob_id ) then
			untouch_used = untouch_used + 1
		end
	end
	return (untouch_used==nb)
end

function BUSS:find_bu_by_pos( x,y )
	return self:test_down_reverse( BUS.find_bu_by_pos, x,y )
end
function BUSS:find_bu_active_by_pos( x,y )
	return self:test_down_reverse( BUS.find_bu_active_by_pos, x,y )
end

BUSS.doc.deal_with_touch = "( blobs ) return the number of touch used."
function BUSS:deal_with_touch( blobs )
	local nb = blobs:get_touch_nb()
	if nb <= 0 then return 0 end

	--self:print( "deal_with_touch() there is "..nb.." touch" )
	if self.verbose >= 3 then self:print( "deal_with_touch() there is "..nb.." touch" ) end

	local touch_used = 0
	for i_touch = 1,nb do
		local blob_id = blobs:get_touch_id( i_touch )
		if self.verbose >= 2 then self:print( "analyse_touch of blob "..blob_id ) end
		local blob = blobs:find_by_id( blob_id )
		if blob then
			if blob.__b_consumed then
				if self.verbose >= 2 then self:print( blob.." consumed by a previous BUSS" ) end
			else
				if self.verbose >= 3 then self:print( "analyse_touch of blob "..blob.id.." at "..blob.x..", "..blob.y ) end

				if self:test_down_reverse( BUS.deal_with_touch, blob ) then
					--self:print( "touched "..blob.id )
					if blob.id == -1 then	--this is a mouse
						aaa.mouse.cancel_move()
					end
					touch_used = touch_used + 1
				end
			end
		else
			if self.verbose > 1 then
				table.print( blobs, "BLOBS" )
				self:print_error( "BLOBS:deal_with_touch_bus() nb "..nb.." can't find blob_id "..blob_id )
			end
		end
	end
	return touch_used
end

function BUSS:find_bus_or_bu_to_drop( x,y )
	return self:test_down_reverse( BUS.find_bus_or_bu_to_drop, x,y )
end

function BUSS:reset_all_page()	self:apply_fn_down( BUS.reset_all_page )	end
function BUSS:save_bu_pos( saver )
	for _,bus in IPAIRS(self.__down) do
--		self:box_debug( " BUSS:save_bu_pos()\n"..saver )
		bus:save_bu_pos()
	end
end

function BUSS:find_bu_by_name_lowercase( bus_name, bu_name )
	--table.print( self.__down, "BUSS:find_bu_by_name_lowercase()", 1 )
	return self:test_down( BUS.find_bu_by_name_lowercase, bus_name, bu_name )
end

function BUSS:get_bus( id )
	return oo.getsuper(BUSS).get_down( self, id )
end

function BUSS:print_open_close( str )
	self:print_inverse( "\t\t\t\t\t\t\t"..str.."()" )
--	self:box_debug( str )
end
function BUSS:open()
	self.__b_open = true
	self:print_open_close( "open" )
end
function BUSS:close()
	self.__b_open = false
	self:print_open_close( "close" )
end
function BUSS:is_open()
	return self.__b_open
end
