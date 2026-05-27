--	STACK
--
if CLASS.DECLARE( "STACK" ) then
	STACK.__depth_min	= 0
	STACK:set_class_status_doc(  CLASS.STATUS.CORE, "implement a LIFO" )
end

function STACK:erase()
	self.__last	= 0
	self.__data	= {}
end

function STACK:set_size_min( new_min )
	self.__depth_min	= new_min
end
function STACK.init_instance(self)
	oo.getsuper(STACK).init_instance(self)
	self:erase()
end
function STACK:create( name )
	return STACK:create_instance( name )
end
function STACK:get_top()
	local last = self.__last
	if last <= 0 then
		self:print( "data is empty" )
		return nil
	end
	return self.__data[ last ]
end

function STACK:set_top( val )
	local last = self.__last
	if last <= 0 then
		self:print( "data is empty" )
		self:push( val )
		return
	end
	self.__data[ last ] = val
end
function STACK:push( val )
	local last = self.__last + 1
	self.__last = last
	self.__data[ last ] = val
	self.__store = nil
end
function STACK:pop()
	local last = self.__last
	if last <= self.__depth_min then
		self:print_error( "stack is fuck up. pop() failed because it wasn't match by a push()" )
		return self.__store
	end
	
	local val = self.__data[last]
    if last == 1 then
        self.__store = val                        -- cache bottom of stack
    end
    self.__data[last] = nil                        -- remove top
    self.__last = last - 1
	return val
end
function STACK:get_size()
	return self.__last
end
function STACK:get_from_bottom( index )
	--todo protect and use neg inddex
	return self.__data[index]
end
function STACK:print_fn( fn_str )
	self:print( fn_str.."() -> "..self:get_size() )
	--aaa.debug.print_traceback()
end
function STACK:get_table_and_nb()
	return self.__data, self.__last
end

function STACK:do_unit_test()
    local stack = STACK:create( "test_stack" )
    -- Basic tests
    self:do_test( "Empty stack size is 0", stack:get_size() == 0 )
    self:do_test( "Empty stack top is nil", stack:get_top() == nil )

    -- Push/pop tests
    stack:push( "item1" )
    self:do_test( "Push increases size", stack:get_size() == 1 )
    self:do_test( "Top returns pushed item", stack:get_top() == "item1" )

    self:do_test( "Pop returns correct item", stack:pop() == "item1" )
    self:do_test( "Pop decreases size", stack:get_size() == 0 )

    -- Minimum size test
    stack:set_size_min( 2 )
    for i = 1, 5 do stack:push( "item" .. i ) end
    for i = 1, 3 do stack:pop( ) end
    self:do_test( "Pop stops at minimum size", stack:get_size() == 2 )

    -- Edge cases
    local empty = STACK:create( "empty_test" )
    self:do_test( "Pop from empty returns nil", empty:pop() == nil )

	local function test_error()
       -- empty:set_top( "new" )
       -- assert( empty:get_size() == 1 and empty:get_top() == "new" )
	end

    self:do_test_error( "Set top on empty becomes push", test_error )
end
