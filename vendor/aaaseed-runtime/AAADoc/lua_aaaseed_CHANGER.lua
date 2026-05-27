-- AAASeed Lua API for the CHANGER, TCHANGER, and CHANGERS classes.
-- Defined in AAAKernel/GaBu/GaBu_Util/CHANGER/CHANGER.lua, loaded via GaBu_Util bind order.
--
-- All entries in this file are pure Lua (no C bindings).
--
-- These three classes provide time-based value interpolation and deferred method triggering.
-- CHANGER interpolates a value from start to stop over a duration, calling a method on a
-- target object each frame. TCHANGER fires a single method call after a time delay.
-- CHANGERS is a pool that manages a collection of both, auto-removing them when completed.


-- # CHANGER

-- Interpolates a value from start to stop over time_len seconds, calling
-- target:method(val) each frame. The interpolation begins after time_offset seconds
-- from the moment of creation. Uses aaa.math.gain/bias for optional easing.

-- ## create

-- Constructor. All parameters can also be set later via set().

	ch = CHANGER:create( name, time_offset, time_len, target, method, start, stop )

-- ## set

-- Reconfigures all parameters. time_offset is relative to the current aaa.time.t.
-- start can be nil: in that case, change() will auto-read the current value by calling
-- target:get_METHOD() (replacing the leading "s" of the set method with "g").

	ch:set( time_offset, time_len, target, method, start, stop )

-- ## change

-- Call once per frame (typically from MEU:update). Computes the interpolation phase,
-- applies gain/bias easing if set, interpolates between start and stop, and calls
-- target:method(val). Returns true when the interpolation is finished (phase >= 1),
-- false otherwise. Before time_offset has elapsed, returns false without calling the method.

	b_done = ch:change()

-- ## set_gain_bias

-- Applies easing to the linear phase before interpolation.
-- gain reshapes the curve (0.5 = linear, < 0.5 = ease-out, > 0.5 = ease-in).
-- bias shifts the midpoint (0.5 = linear, < 0.5 = early, > 0.5 = late).
-- Uses aaa.math.gain() and aaa.math.bias() internally.

	ch:set_gain_bias( gain, bias )


-- # TCHANGER

-- Trigger changer: fires a single method call after a time delay. Unlike CHANGER,
-- it does not interpolate. Once the delay has elapsed, it calls
-- target:method(arg1, arg2, arg3, arg4) exactly once and reports done.

-- ## create

	tch = TCHANGER:create( name, time_offset, target, method, arg1 [, arg2 [, arg3 [, arg4]]] )

-- ## set

-- Reconfigures all parameters. time_offset is relative to current aaa.time.t.
-- Passing nil for time_offset defaults to 0 (immediate).

	tch:set( time_offset, target, method, arg1 [, arg2 [, arg3 [, arg4]]] )

-- ## change

-- Call once per frame. Returns true once the trigger has fired, false while waiting.

	b_done = tch:change()


-- # CHANGERS

-- Pool manager for CHANGER and TCHANGER instances. Maintains a singly-linked list,
-- calls change() on each element every frame, and auto-removes completed ones.
-- Typical usage: create one CHANGERS per MEU or per animation context, call
-- changers:change() in update(), and add() individual animations as needed.

-- ## create

	changers = CHANGERS:create( name )

-- ## add

-- Creates a CHANGER and inserts it into the pool. Parameters are the same as
-- CHANGER:create. Returns the CHANGER instance (useful for set_gain_bias).
-- If no_delay mode is active, immediately calls target:method(stop) and returns nil.

	ch = changers:add( time_offset, time_len, target, method, start, stop )

-- ## add_trig

-- Creates a TCHANGER and inserts it into the pool. Extra arguments are passed
-- through to the method. If no_delay mode is active or time_offset is 0,
-- immediately calls target:method(...) and returns nil.

	tch = changers:add_trig( time_offset, target, method, ... )

-- ## change

-- Call once per frame. Iterates through all active changers, calls change() on each,
-- and removes those that are done. Safe to call add() from within a target method
-- triggered by change() (new items go to list head, won't be visited this frame).

	changers:change()

-- ## is_empty

-- Returns true when the pool has no active changers left.

	b = changers:is_empty()

-- ## set_no_delay

-- When set to true, add() and add_trig() skip the animation entirely and
-- immediately apply the final value. Useful for instant UI transitions
-- (e.g. when loading a preset where animated fade-in would be distracting).

	changers:set_no_delay( b )
