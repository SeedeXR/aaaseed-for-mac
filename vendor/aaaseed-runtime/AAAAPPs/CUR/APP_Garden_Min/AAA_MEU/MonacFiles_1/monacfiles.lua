
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix = 4
	local iy = 2

--	self:add_camera()
--	self:add_button(	{	ix,	iy },		"Logo", 		self, "b_logo", true	)
	bu = self:add_slider(	{ix,iy,	2,1}, "Colons", self, "colons", 1, 1, 40 )
		bu:set_value_type_integer( true )
	ix = ix - 2
	bu = self:add_slider(	{ix,iy,	2,1}, "Lines", self, "lines", 1, 1, 100 )
		bu:set_value_type_integer( true )
	ix, iy = ix -1, iy + 1
	--self:add_trig(	{ix, iy }, "Print", self, "print_cell" )
	self:add_trig_method(	{ix,	iy}, 	"Print", 	self, "print_cell" )

end

function meu:init()
	local ref = self.ref
	local data = {}
	ref.data		=	data
		data.layer				= self:get_layer(1)
		data.bdd				= self:get_layer_bdd(1)
		data.data_new			=	param.get_ref( data.bdd, "data_new" )
		data.data_load_count	=	param.get_ref( data.bdd, "data_load_count" )
		data.y_max				=	param.get_ref( data.bdd, "y_max" )

end

function meu:print_cell()
	local keys = self.__keys
	local data = self.__data
	local line = self.lines
	local colon = self.colons

--	local value_at = aaa.obj_get_data( dref.bdd, colon, line )
	if data[line] then
		-- Also check what happens for tab[nil] (in case keys[colon] is nil)
		self:print( "data[col="..colon.."][line="..line.."] : "..data[ line ][ keys[ colon ] ] )
	else
		self:print( "line is out of bond" )
	end
end

function meu:update(  )
	--self:set_text( "test" )
	self:update_datagrid()
end

function meu:update_datagrid()
	local ref = self.ref
	local dref = ref.data

	aaa.obj.update_then_draw( dref.layer )

	local data_load_count = param.get( dref.data_load_count )
	if data_load_count ~= self.__id_file then
		self.__id_file = data_load_count
		local bdd = dref.bdd
		--self:box_debug( "new Data" )
		local keys = {}
		local nb = 1
		for i=1,4096 do -- is it limit of excel ? seem arbitrary
			local v = aaa.obj_get_data( bdd, i, 1 ) -- couldn't we get x_max ?
			if v == nil then
				break
			else
				keys[#keys+1] = v
			end
		end
		table.print( keys, "datagrid keys" )
		self.__keys = keys
		local y_max = param.get( dref.y_max )
		local x_max = #keys
		local tab = {}
		for l=2,y_max do
			local t = {}
			for i=1,x_max do
				t[keys[i]] = aaa.obj_get_data( bdd, i, l )
			end
			tab[l-1] = t
		end
		self.__data = tab
		table.print( tab, "datagrid data", 2 )
		param.set( dref.data_new, false )
	end


end
