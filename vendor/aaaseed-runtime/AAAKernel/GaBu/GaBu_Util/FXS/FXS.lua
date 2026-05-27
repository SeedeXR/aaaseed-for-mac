--
--	FXS
--
CLASS.DECLARE( "FXS" )

function FXS:create( name )
	local self = FXS:create_instance( name )
	if self.verbose >= 1 then aaa.print_method() end

--todo should we use the down mecanism here
	self.__fx			= {}			--	each fx will be stored in this table
	self.__fx_by_name	= {}	--	each fx will be stored here too
	return self
end

function FXS:add_fx( fx )
	if self.verbose >= 1 then aaa.print_method() end
	if fx then
		self.__fx[#(self.__fx)+1] = fx
		local name = fx:get_name()
		local field = self.__fx_by_name[ name ]
		if not field then
			self.__fx_by_name[ name ] = fx
		else
			self:box_error( "in FXS:add_fx() already an FX with name : "..name )
		end
	end
	return fx
end
function FXS:add_obj( obj_name, fx_name, b_active )
	if self.verbose >= 1 then aaa.print_method() end
	local obj = aaa.obj.get_no_error( obj_name )
	if obj then
		local fx = FX:create( fx_name, obj, b_active )
		self:add_fx( fx )
		return fx
	else
		self:box_error( "Loading FX in FXS.\nCan't find object "..obj_name )
	end
end
function FXS:__make_fname( module, name )
	--self:box_debug( "Stop Now" )
	--local prefix = aaa.obj.get_dir( module )
	local prefix = aaa.file.get_dir_name( aaa.obj.get_name( module ) )
	local fname = prefix.."/fx_"..name.."/fx.layers_param"
	return fname
end
function FXS:add_one( module, fx_name, b_active )
	local fname = self:__make_fname( module, fx_name )
	return self:add_obj( fname, fx_name, b_active )
end
function FXS:add_list( module, list, b_active )
	for i = 1, #list do
		self:add_one( module, list[i], b_active )
	end
end

function FXS:get_by_name( name )	return self.__fx_by_name[ name ]	end

function FXS:add_ui()		array.apply_method( self.__fx, "add_ui" )		end
function FXS:add_window()	array.apply_method_index( self.__fx, "add_window" )	end

function FXS:sort_by_pos()
	table.sort( self.__fx, function(a,b) return a.__bu_win and b.__bu_win and a.__bu_win:get_y() > b.__bu_win:get_y() end )
end
function FXS:dump_active()
	self:print( "Start" )
	array.apply_fn( self.__fx, function(fx) if fx:is_active() then fx:print( fx.__bu_win:get_y() ) end end )
end
function FXS:update()
	self:sort_by_pos()
	array.apply_fn( self.__fx, function(fx) if fx:is_active() then fx:update() end end )
end
function FXS:draw()
	array.apply_fn( self.__fx, function(fx) if fx:is_active() then fx:draw() end end )
end
function FXS:updraw()
	if GA.b_spy then aaa.spy.push_range( self..":updraw()", 7 ) end
	self:sort_by_pos()
	--self:dump_active()
	array.apply_fn( self.__fx, function(fx) if fx:is_active() then fx:update() fx:draw() end end )
	if GA.b_spy then aaa.spy.pop_range() end
end
