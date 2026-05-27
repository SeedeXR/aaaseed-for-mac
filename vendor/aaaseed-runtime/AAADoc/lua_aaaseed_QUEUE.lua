-- AAASeed Lua API for the QUEUE class.
-- Defined in AAAKernel/GaBu/GaBu_Util/QUEUE/QUEUE.lua, loaded via GaBu_Util bind order.
--
-- All entries in this file are pure Lua (no C bindings).
--
-- QUEUE is a high-performance FIFO (First-In First-Out) container built on a circular buffer.
-- Designed for real-time graphics: O(1) push and pop, auto-growing capacity,
-- no nil-assignment on pop (preserves Lua array continuity to avoid GC pressure).
-- Memory compaction is explicit via cleanup_memory() so it never causes frame spikes.


-- # QUEUE

-- ## create

-- Constructor. Returns a new empty QUEUE. An optional initial_capacity pre-allocates
-- the internal buffer to avoid reallocations during the first pushes.

	queue = QUEUE:create( name [, initial_capacity] )

-- ## push / pop

-- push() appends a value at the tail. If the buffer is full, capacity is automatically
-- doubled (minimum 8). Returns true on success.
-- pop() removes and returns the head value. Returns nil if empty.
-- Both operations are O(1) amortized.

	b = queue:push( val )
	val = queue:pop()

-- ## get_front

-- Returns the element at the given 1-based index from the head without removing it.
-- Index 1 is the oldest element (the one pop() would return). Index defaults to 1
-- if omitted. Returns nil if out of range.

	val = queue:get_front( [index_one_based] )

-- ## get_first / get_last

-- Shortcuts: get_first() returns the oldest element, get_last() returns the newest.
-- Both are O(1) and non-destructive.

	val = queue:get_first()
	val = queue:get_last()

-- ## get_nb / get_capacity

-- get_nb() returns the current element count.
-- get_capacity() returns the total allocated buffer size.

	n = queue:get_nb()
	cap = queue:get_capacity()

-- ## is_empty / is_full

-- Status queries.

	b = queue:is_empty()
	b = queue:is_full()

-- ## clear

-- Resets the queue to empty in O(1) without deallocating memory. The buffer is
-- kept at its current capacity so subsequent pushes avoid reallocation.

	queue:clear()

-- ## reserve

-- Pre-allocates buffer capacity. Behavior by case:
-- capacity > current: grows, linearizes data in place.
-- capacity == current: no-op.
-- size <= capacity < current: calls cleanup_memory() to shrink.
-- capacity < size: prints an error (would lose data).
-- Array continuity is preserved in all cases.

	queue:reserve( capacity )

-- ## cleanup_memory

-- Explicit memory compaction. Linearizes the circular buffer and reduces logical
-- capacity when more than 50% is unused. O(n) but only called when the caller
-- decides it is safe (typically between frames, never inside a hot loop).
-- When the queue is empty, just resets pointers.

	queue:cleanup_memory()

-- ## insert

-- Inserts a value at the given 1-based position, shifting later elements.
-- If index > size, behaves like push(). This is O(n) because of the shift;
-- prefer push/pop for performance-critical paths.

	queue:insert( index_one_based, val )

-- ## erase

-- Full reset: deallocates the internal buffer and sets capacity to 0.
-- Use clear() instead if you plan to reuse the queue soon.

	queue:erase()
