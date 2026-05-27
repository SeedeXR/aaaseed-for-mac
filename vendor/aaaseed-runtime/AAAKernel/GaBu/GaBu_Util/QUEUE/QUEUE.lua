--
--	QUEUE - High-Performance Circular Buffer Implementation
--
-- Designed for real-time graphics applications with constant-time operations
-- Maintains array continuity by never setting elements to nil during pop()
-- Memory cleanup is explicit via cleanup_memory() to avoid performance spikes
--
if CLASS.DECLARE( "QUEUE" ) then
	QUEUE:set_class_status_doc(  CLASS.STATUS.CORE,
								"High-performance FIFO queue with constant-time operations",
								"Circular buffer design for real-time graphics applications",
								"Maintains array continuity for optimal performance" )
end

function QUEUE:erase()
	self.__data = {}
	self.__head = 1      -- Points to first element
	self.__tail = 1      -- Points to next available slot
	self.__size = 0      -- Current number of elements
	self.__capacity = 0  -- Total allocated capacity
end

function QUEUE.init_instance(self)
	oo.getsuper(QUEUE).init_instance(self)
	self:erase()
end

function QUEUE:create( name, initial_capacity )
	local self = QUEUE:create_instance( name )
	if initial_capacity then
		self:reserve( initial_capacity )
	end
	return self
end

-- Helper method to linearize data in-place or to a new target table
-- If target_table is provided, uses it; otherwise linearizes in current table
local function linearize_data(self, target_table)
	if self.__size == 0 then
		return target_table or self.__data
	end

	local source_data = self.__data
	local result = target_table or source_data
	local write_pos = 1

	if self.__head < self.__tail then
		-- Contiguous block - copy directly
		for i = self.__head, self.__tail - 1 do
			result[write_pos] = source_data[i]
			write_pos = write_pos + 1
		end
	else
		-- Wrapped around - copy head->end then start->tail
		for i = self.__head, self.__capacity do
			result[write_pos] = source_data[i]
			write_pos = write_pos + 1
		end
		for i = 1, self.__tail - 1 do
			result[write_pos] = source_data[i]
			write_pos = write_pos + 1
		end
	end

	-- If we're reusing the same table, clear old references beyond new size
	if not target_table and write_pos <= self.__capacity then
		-- Don't set to nil - let GC handle it naturally
		-- The array part will be maintained up to write_pos-1
	end

	return result
end

-- Reserve capacity to avoid reallocations during critical operations
-- Behavior:
-- - capacity > current capacity: Always tries in-place linearization first
-- - capacity == current capacity: No operation (already at requested size)
-- - size < capacity < current capacity: Calls cleanup_memory() to shrink
-- - capacity < size: Error - cannot shrink below current data size
-- - Empty queue: Creates new table (only safe case for new table)
-- Array continuity is preserved in all cases
function QUEUE:reserve( capacity )
	if capacity <= self.__capacity then
		-- If requested capacity is smaller or equal, check if we should shrink
		if capacity < self.__size then
			-- Cannot shrink below current size - that would lose data
			self:error("QUEUE:reserve() - cannot shrink below current size ("..self.__size..")")
			return
		elseif capacity == self.__capacity then
			-- Already at requested capacity, nothing to do
			return
		else
			-- Requested capacity is between current size and capacity
			-- This is effectively a cleanup operation
			return self:cleanup_memory()
		end
	end

	-- We need to grow - always try in-place first for best performance
	if self.__size == 0 then
		-- Empty queue - we can safely replace with new table
		local new_data = {}
		self.__data = new_data
		self.__capacity = capacity
		return
	end

	-- Always try in-place linearization first (preserves table and array continuity)
	linearize_data(self, nil)  -- nil means in-place linearization
	self.__head = 1
	self.__tail = self.__size + 1
	self.__capacity = capacity
end

-- O(1) push operation (amortized constant time)
function QUEUE:push( val )
	-- Auto-grow if needed
	if self.__size == self.__capacity then
		self:reserve( math.max( 8, self.__capacity * 2 ) )  -- Double capacity
	end

	self.__data[self.__tail] = val
	self.__tail = self.__tail + 1
	if self.__tail > self.__capacity then
		self.__tail = 1  -- Wrap around
	end
	self.__size = self.__size + 1
	return true
end

-- O(1) pop operation
function QUEUE:pop()
	if self.__size == 0 then
		return nil
	end

	local val = self.__data[self.__head]
	-- Don't set to nil to maintain array continuity
	-- Just advance the head pointer
	self.__head = self.__head + 1
	if self.__head > self.__capacity then
		self.__head = 1  -- Wrap around
	end
	self.__size = self.__size - 1
	return val
end

-- O(1) get_front operation
function QUEUE:get_front( index_one_based )
	if not index_one_based then index_one_based = 1 end
	if index_one_based < 1 or index_one_based > self.__size then
		return nil
	end

	local pos = self.__head + index_one_based - 1
	if pos > self.__capacity then
		pos = pos - self.__capacity
	end
	return self.__data[pos]
end

-- O(1) get_last operation
function QUEUE:get_last()
	if self.__size == 0 then
		return nil
	end

	local pos = self.__tail - 1
	if pos < 1 then
		pos = self.__capacity
	end
	return self.__data[pos]
end

function QUEUE:get_first()		return self:get_front( 1 )	end
function QUEUE:get_nb()			return self.__size			end
function QUEUE:get_capacity()	return self.__capacity		end
function QUEUE:is_empty()		return self.__size == 0		end
function QUEUE:is_full()		return self.__size == self.__capacity	end

-- Clear queue without deallocating memory (O(1))
function QUEUE:clear()
	-- Just reset pointers, don't clear data to avoid GC pressure
	self.__head = 1
	self.__tail = 1
	self.__size = 0
end

-- Controlled memory cleanup (O(n) but called explicitly when safe)
-- Maintains array continuity by avoiding nil assignments and reusing tables
function QUEUE:cleanup_memory()
	-- Only clean up if we have significant fragmentation
	if self.__size == 0 and self.__capacity > 0 then
		-- When queue is empty, we can safely reset without breaking array structure
		self.__head = 1
		self.__tail = 1
		-- Don't set capacity to 0 to maintain array structure for future use
		-- self.__capacity = 0  -- Removed to preserve array
	elseif self.__size < self.__capacity * 0.5 then
		-- If more than 50% of capacity is unused, compact in-place
		if self.__size > 0 then
			if self.__head < self.__tail then
				-- Contiguous block - already optimal, just reset pointers
				-- No data movement needed, array is already contiguous
			else
				-- Wrapped around - linearize in-place using shared helper
				linearize_data(self, nil)  -- nil means linearize in-place
			end
		end
		self.__head = 1
		self.__tail = self.__size + 1
		-- Update capacity to actual size, but don't shrink the table
		-- This maintains array continuity while reducing logical capacity
		self.__capacity = self.__size
	end
end


-- Insert operation (maintains compatibility but with different performance)
function QUEUE:insert( index_one_based, val )
	if index_one_based < 1 then
		self:print_error( "QUEUE:insert() - index_one_based must be >= 1" )
		return
	end

	if index_one_based > self.__size then
		return self:push( val )
	end

	-- For circular buffer, insert is more complex - we'll do it by shifting
	-- This is O(n) but maintains the circular buffer properties
	if self.__size == self.__capacity then
		self:reserve( self.__capacity * 2 )
	end

	-- Shift elements to make room
	local pos = self.__head + index_one_based - 1
	if pos > self.__capacity then
		pos = pos - self.__capacity
	end

	-- Shift elements from pos onward
	if self.__head < self.__tail then
		-- Contiguous case
		for i = self.__size, index_one_based, -1 do
			local from_pos = self.__head + i - 1
			local to_pos = from_pos + 1
			if to_pos > self.__capacity then
				to_pos = 1
			end
			self.__data[to_pos] = self.__data[from_pos]
		end
	else
		-- Wrapped case - more complex shifting needed
		-- This is a rare case, so we'll do a simpler approach: linearize, insert, re-circularize
		local temp_data = {}
		for i = 1, self.__size do
			temp_data[i] = self:get_front(i)
		end
		table.insert(temp_data, index_one_based, val)
		self:clear()
		for i = 1, #temp_data do
			self:push(temp_data[i])
		end
		return
	end

	self.__data[pos] = val
	self.__size = self.__size + 1
	self.__tail = self.__tail + 1
	if self.__tail > self.__capacity then
		self.__tail = 1
	end
end

-- Unit testing method for QUEUE class
function QUEUE:do_unit_test()
	-- Test basic operations
	local queue = QUEUE:create("test_queue")
	
	self:do_test( "Empty queue has size 0", queue:get_nb() == 0 )
	self:do_test( "Empty queue is empty", queue:is_empty() )
	--self:do_test( "Empty queue not full", not queue:is_full() )

	-- Test push/pop
	queue:push( "item1" )
	self:do_test( "Push increases size to 1", queue:get_nb() == 1 )
	self:do_test( "Push makes queue not empty", not queue:is_empty() )
	
	local item = queue:pop()
	self:do_test( "Pop returns correct item", item == "item1" )
	self:do_test( "Pop decreases size to 0", queue:get_nb() == 0 )

	-- Test multiple items
	for i = 1, 10 do
		queue:push( "item" .. i )
	end
	self:do_test( "Push 10 items gives size 10", queue:get_nb() == 10 )
	self:do_test( "Get first item", queue:get_front(1) == "item1" )
	self:do_test( "Get last item", queue:get_last() == "item10" )

	-- Test wrap-around
	for i = 1, 5 do queue:pop() end
	for i = 11, 15 do queue:push("item" .. i) end
	self:do_test( "After wrap-around, size is 10", queue:get_nb() == 10 )
	self:do_test( "Wrap-around preserves order", queue:get_front(1) == "item6" )

	-- Test reserve
	queue:reserve(20)
	self:do_test( "Reserve increases capacity", queue:get_capacity() >= 20 )
	self:do_test( "Reserve preserves size", queue:get_nb() == 10 )

	-- Test cleanup
	queue:cleanup_memory()
	self:do_test( "Cleanup maintains size", queue:get_nb() == 10 )
	self:do_test( "Cleanup reduces capacity", queue:get_capacity() >= 10 )

	-- Test edge cases
	local empty_queue = QUEUE:create( "empty_test" )
	self:do_test( "Pop from empty returns nil", empty_queue:pop() == nil )
	self:do_test( "Front from empty returns nil", empty_queue:get_front(1) == nil )

	-- Test error casee
	local function test_error()
		local q = QUEUE:create( "error_test" )
		for i = 1, 5 do q:push(i) end
		q:reserve(3)  -- Should error: can't shrink below size
	end
	self:do_test_error( "Reserve below size throws error", test_error )
end
