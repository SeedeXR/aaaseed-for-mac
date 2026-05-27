-- AAASeed Lua API for the STACK class.
-- Defined in AAAKernel/GaBu/GaBu_Util/STACK/STACK.lua, loaded via GaBu_Util bind order.
--
-- All entries in this file are pure Lua (no C bindings).
--
-- STACK is a LIFO (Last-In First-Out) container. It stores arbitrary Lua values
-- and provides O(1) push, pop, and peek operations. Used by BUS and APP for
-- push/pop context management.


-- # STACK

-- ## create

-- Constructor. Returns a new empty STACK instance.

	stack = STACK:create( name )

-- ## push / pop

-- push() adds a value on top of the stack.
-- pop() removes and returns the top value. If the stack has reached its minimum
-- depth (see set_size_min), pop prints an error and returns the cached bottom
-- value instead.

	stack:push( val )
	val = stack:pop()

-- ## get_top / set_top

-- get_top() returns the top value without removing it (peek). Returns nil if empty.
-- set_top() replaces the top value in place. If the stack is empty, it performs
-- a push instead.

	val = stack:get_top()
	stack:set_top( val )

-- ## get_size

-- Returns the current number of elements.

	n = stack:get_size()

-- ## get_from_bottom

-- Returns the element at the given 1-based index counted from the bottom.
-- Index 1 is the first pushed element.

	val = stack:get_from_bottom( index )

-- ## erase

-- Removes all elements and resets the stack to empty.

	stack:erase()

-- ## set_size_min

-- Sets a minimum depth. pop() will refuse to go below this depth and will print
-- an error. Useful to protect a base context that should never be popped.
-- Default is 0 (no minimum).

	stack:set_size_min( n )

-- ## get_table_and_nb

-- Returns the raw internal data table and the current element count.
-- Intended for iteration when performance matters (avoids per-element method calls).

	data, nb = stack:get_table_and_nb()
