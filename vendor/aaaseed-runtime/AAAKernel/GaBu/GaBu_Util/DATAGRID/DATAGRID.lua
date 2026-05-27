if CLASS.DECLARE( "DATAGRID" ) then
	DATAGRID:set_class_status_doc(	CLASS.STATUS.CORE,
									"encapsulate a C datagrid object" )
end

function DATAGRID:init()
	self.ref = {}
end
function DATAGRID:assign( bdd )
	local ref = self.ref
	ref.bdd = bdd
	ref.data_new	=	param.get_ref( bdd, "data_new"		)
	ref.y_max		=	param.get_ref( bdd, "y_max"			)
	ref.ref_local	=	param.get_ref( bdd, "local"			)
	ref.data_fmame	=	param.get_ref( bdd, "data_filename"	)
end
function DATAGRID:create( name, bdd )
	local self = DATAGRID:create_instance_with_obj( name, bdd )
	self:init()
	if bdd then self:assign( bdd ) end
	return self
end
function DATAGRID:read( b_verbose )
	local ref = self.ref
	local bdd = ref.bdd
	aaa.obj.update( bdd )

	if not param.get_bool( ref.data_new ) then
		return false
	end

	--self:box_debug( "new Data" )
	local keys = {}
	local nb = 1
	for i=1,4096 do
		local v = aaa.obj_get_data( bdd, i, 1 )
		if v == nil then	break
		else				keys[#keys+1] = v
		end
	end
	if b_verbose then table.print( keys, "datagrid keys" ) end

	local y_max = param.get( ref.y_max )
	local x_max = #keys
	local data = {}
	for l=2,y_max do
		local t = {}
		for i=1,x_max do
			t[keys[i]] = aaa.obj_get_data( bdd, i, l )
		end
		data[l-1] = t
	end
	self.__data = data
	if b_verbose then table.print( data, "datagrid data", 2 ) end

	param.set( ref.data_new, false )
	return true
end

function DATAGRID:set_file( fname, b_local )
	local ref = self.ref
	param.set( ref.data_fmame, fname )
	if b_local~=nil then
		param.set( ref.ref_local, b_local )
	end
	self:read()
end

function DATAGRID:get_data()
	return self.__data
end
function DATAGRID:get_field_max( key )
	local data = self:get_data()
	local max = -math.huge
	for i=1,#data do
		local v = data[i][key]
		if v and v > max then max = v end
	end
	return (max~=-math.huge) and max
end
function DATAGRID:get_field_min( key )
	local data = self:get_data()
	local min = math.huge
	for i=1,#data do
		local v = data[i][key]
		if v and v < min then min = v end
	end
	return (min~=math.huge) and min
end
function DATAGRID:get_field_all_value( key )
	local data = self:get_data()
	local tab = {}
	for i=1,#data do
		local v = data[i][key]
		if v then
			tab[#tab+1] = v
		end
	end
	return (#tab>0) and tab or nil
end
function DATAGRID:get_id_prev_next_inter( key, val )
--	aaa.print_method()
--	aaa.print_fn()

	--table.print( self.__data, "data", 2 )
	local data = self.__data
	local id = self.__id_opt_next_call or 1
	local prev = data[id]
	if not( prev and prev[key] and prev[key]-.000001<=val ) then
		id = 1
		prev = data[id]
	end
	--self:print( id )
	local next = data[id+1]
	local inter

	while true do
		--self:print( "get_id_prev_next_inter() loop : id is "..id )
		if not prev[key]	then break end
		if not next 		then break end
		if not next[key]	then break end
		if prev[key]>=next[key] then break end
		--self:print( id.." test  "..prev[key].." < "..val.." < "..next[key] )
		if prev[key]-.000001<= val and val<next[key]-.000001 then
			self.__id_opt_next_call = id
			return id, prev, next, (val - prev[key]) / (next[key] - prev[key])
		end
		prev = next
		id = id + 1
		next = data[id+1]
	end
	self.__id_opt_next_call = id-1
	return id, prev, next, 0
end
function DATAGRID:inc_id_prev( id, inc )
	local new_id
	if inc < 0 then
		if id > 1 					then	new_id = id-1	end
	elseif inc > 0 then
		if id < #(self.__data)-1	then	new_id = id+1	end
	end
	return new_id
end
function DATAGRID:get_field_at_id( key, id )
	return self:get_data()[id][key]
end
function DATAGRID:get_field( key )
	local val_p = self.prev[key]
	if type(val_p)=="number" then
		local val_n = self.next and self.next[key]
		--self:print( key.." "..val_p.." / "..val_n.."   with inter "..self.inter )
		if val_n then
			return interpolate( val_p, val_n, self.inter )
		end
	end
	return val_p
end
function DATAGRID:get_field_as_bool( key )
	local val = self:get_field( key )
	return val and val > .5
end
function DATAGRID:set_position_by_key_value( key, val )
	self.id, self.prev, self.next, self.inter = self:get_id_prev_next_inter( key, val )
	if self.id_last ~= self.id then
		self.id_last = self.id
		return true
	end
	return false
end
function DATAGRID:set_position_by_time( val )
	return self:set_position_by_key_value( "time", val )
end
