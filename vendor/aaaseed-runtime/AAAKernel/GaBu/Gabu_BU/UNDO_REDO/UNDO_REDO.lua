--todo limit the undo/redo table to a certain size 
if CLASS.DECLARE( "UNDO_REDO", nil,  {
	} ) then
		UNDO_REDO:set_class_status_doc(	CLASS.STATUS.GABU,
									"UNDO_REDO centralize undo_redo functionalities and data"
								)
end

function UNDO_REDO:reset()
	self.table = {}
	self.index = 0
	self.index_max = 0
end

function UNDO_REDO:create()
	local self = UNDO_REDO:create_instance_no_name()
	self:reset()
	return self
end

function UNDO_REDO:get_verbose()	return self.verbose		end

function UNDO_REDO:store_position_in_bu( bu )
	if self.verbose > 0 then
		aaa.print_fn()
	end

	local storage = bu:get_table_always("__undo_redo_storage")
	--table.print( storage, "storage before", 2 )	
	local x,y = bu:get_xy()
	local sx,sy = bu:get_sxy()
	storage.pos_prev = {x,y, sx,sy}
	--table.print( storage, "storage after", 2 )

	if self.verbose > 1 then
		table.print( bu.__undo_redo_storage, "undo_redo_storage for "..bu, 2 )
	end
end

function UNDO_REDO:store_values_in_bu( bu )
	if self.verbose > 0 then
		aaa.print_fn()
	end

	if bu.is_trig and bu:is_trig() then
		if self.verbose > 0 then
			self:print( bu.." is a trigger bu so we don't store" )
		end
		return
	end
	
	--local storage = {}
	--bu.__undo_redo_storage = storage
	--bu:get_table_always("__undo_redo_storage")

	local storage = bu:get_table_always("__undo_redo_storage")

	if bu.get_id_interacting then
		local id = bu:get_id_interacting()
		storage.value_prev = bu:get_value(id)
		storage.id = id
	end
	if bu.get_values then
		storage.values_prev = table.copy_simple(bu:get_values())
	end

	if self.verbose > 1 then
		table.print( bu.__undo_redo_storage, "undo_redo_storage for "..bu, 2 )
	end
end

function UNDO_REDO:make_str_one( t )
	local str = t.obj.." "..t.method_name.."( "
	for i=1,t.arg_nb do
		str = str..t.args[i]..((i==t.arg_nb) and " )" or ", ")
	end
	return str
end
function UNDO_REDO:print_stored( elt )
	self:print_debug( "we stored : " )
	self:print_debug( "\tundo : "..self:make_str_one(elt.undo) )
	self:print_debug( "\tredo : "..self:make_str_one(elt.redo) )
end

function UNDO_REDO:__print_elt( key, elt )
	local str = "elt["..key.."]:"
	self:print( str )
	table.print( elt, str, 1 )
	table.print( elt.args, "elt["..key.."].args:", 2 )
end
function UNDO_REDO:add_elt( elt )
	local verbose = self.verbose
	if verbose > 0 then aaa.print_fn() end
	if elt then
		local ind = self.index + 1
		self.table[ind] = elt
		if verbose > 1 then
			self:print_stored( elt )
		end
		if verbose > 2 then
			--table.print( elt, "elt "..ind, 2 )
			for k,e in PAIRS(elt) do
				self:__print_elt( k, e )
			end
		end 
		self.index = ind
		self.index_max = ind
	end
end
function UNDO_REDO:add_undo_redo( undo, redo )
	local verbose = self.verbose
	if verbose > 0 then
		self:print( "UNDO_REDO:add_undo_redo()" )
		table.print( undo, "undo", 1 )
		table.print( redo, "redo", 1 )
	end
	self:add_elt( { undo = undo, redo = redo } )
end

function UNDO_REDO:add_pos_to_undo_history( bu )
	if not bu then
		self:print_debug( "UNDO_REDO:add_pos_to_undo_history() no bu" )
		return 
	end

	local verbose = self.verbose
	--aaa.print_fn()
	--aaa.debug.print_traceback()
	local storage = bu.__undo_redo_storage
	if verbose > 1 then
		table.print( storage, "undo_redo_storage", 2 )
	end
	
	local prev = storage.pos_prev
	local x,y = bu:get_xy()
	local sx,sy = bu:get_sxy()
	local undo, redo
	if x~=prev[1] or y~=prev[2] then
		if sx~=prev[3] or sy~=prev[4] then
			undo = self:make_table_to_call_method( bu, "set_xy_sxy",	prev[1],prev[2],	prev[3],prev[4]	)
	  		redo = self:make_table_to_call_method( bu, "set_xy_sxy",	x,y,				sx,sy			)
		else
			undo = self:make_table_to_call_method( bu, "set_xy",		prev[1],prev[2]		)
			redo = self:make_table_to_call_method( bu, "set_xy",		x,y					)
		end
	else
		if sx~=prev[3] or sy~=prev[4] then
			undo = self:make_table_to_call_method( bu, "set_sxy",		prev[3],prev[4]		)
			redo = self:make_table_to_call_method( bu, "set_sxy",		sx,sy				)
		end
	end
	if undo then
		self:add_undo_redo( undo, redo )
	end
	bu.__undo_redo_storage = nil
end

function UNDO_REDO:undo()
	if self.verbose > 0 then aaa.print_fn() end

	local ind = self.index
	if 0 < ind then
		local t = self.table[ind].undo
		if self.verbose > 0 then
			self:print( "HISTORY "..ind.." : undo" )
			if self.verbose > 1 then
				self:print_debug( "UNDO : "..self:make_str_one(t) )
			end
			if self.verbose > 2 then
				table.print( t, "undo", 1 )
			end
		end
		self:do_table( t )
		self.index = ind - 1
	end
	if self.verbose > 2 then
		table.print( self.table, "history", 4 )
	end
end

function UNDO_REDO:redo()
	if self.verbose > 0 then aaa.print_fn() end

	local ind = self.index + 1
	if ind <= self.index_max then
		local t = self.table[ind].redo
		if self.verbose > 0 then
			self:print( "HISTORY "..ind.." : redo" )
			if self.verbose > 1 then
				self:print( "REDO : "..self:make_str_one(t) )
			end
			if self.verbose > 2 then
				table.print( t, "redo", 1 )
			end
		end
		self:do_table( t )
		self.index = ind
	end
	if self.verbose > 2 then
		table.print( self.table, "history", 4 )
	end
end