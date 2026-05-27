CLASS.DECLARE( "MARK_LINE" )

function MARK_LINE:begin()
	self.id_mark = 0
end
function MARK_LINE:init()
	self.mark	= {}
	self.s 		= -1
	self:begin()
end

function MARK_LINE:create( name )
	local self = MARK_LINE:create_instance( name )
	self:init()
	return self
end

function MARK_LINE:find( s )
	local mark = self.mark
	local nb = #mark
	for id = 1, nb do
		local m = mark[ id ]
		if s <= m.s then
			return id
		end
	end
	return nb + 1
end

function MARK_LINE:add( s, val )
	local id = self:find( s )
	table.insert( self.mark, id, { s=s, val=val } )
end

function MARK_LINE:update( s )
	if s <.01 then
		self:begin()
	end
	local id = self:find( s )
	if id then
		if id > 1 then
			id = id -1
		end
		if self.id_mark ~= id then
			self.id_mark = id
			return self.mark[id].val
		end
	end
end

--todo
function MARK_LINE:dump()
end


CLASS.DECLARE( "MARKS_LINE" )

function MARKS_LINE:begin()
	for _,m in pairs( self.marks ) do
		m:begin()
	end
end
function MARKS_LINE:init()
	self.marks	= {}
end

function MARKS_LINE:create( name )
	local self = MARKS_LINE:create_instance( name )
	self:init()
	return self
end

function MARKS_LINE:add( key, s, val )
	local m = self.marks[ key ]
	if not m then
		m = MARK_LINE:create( key )
		self.marks[ key ] = m
	end
	m:add( s, val )
end

function MARKS_LINE:update( s, fn )
	for key,m in pairs( self.marks ) do
		local val = m:update( s, fn )
		if val ~= nil then
			fn( key, val )
		end
	end
end

--todo
function MARKS_LINE:dump()
end


