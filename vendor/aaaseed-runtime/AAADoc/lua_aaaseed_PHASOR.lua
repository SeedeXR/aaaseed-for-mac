-- AAASeed Lua API for the PHASOR, PHASOR_A, and PHASOR_ASR classes.
-- Defined in AAAKernel/GaBu/GaBu_Util/PHASOR/PHASOR.lua, loaded via GaBu_Util bind order.
--
-- All entries in this file are pure Lua (no C bindings).
--
-- PHASORs are envelope generators with randomized timing. Each cycle goes through
-- a wait period then one or more active phases (attack, sustain, release), producing
-- a phase value in [0, 1]. When a cycle completes, durations are re-randomized from
-- their configured ranges and the cycle restarts automatically. An optional random ID
-- is picked at each restart, useful for selecting textures or behaviors per cycle.
--
-- The file also defines two global math helpers used internally:
--   math.randomf( min, max )             -- uniform random float in [min, max]
--   math.randomf_pow( min, max, pow )    -- power-biased random float


-- # PHASOR (base class)

-- Base class for all phasor types. Manages wait/attack ranges, time tracking,
-- and random ID generation. Not usually instantiated directly (use PHASOR_A
-- or PHASOR_ASR), but can be.

-- ## create

	ph = PHASOR:create( name, wait_min, wait_max, attack_min, attack_max )

-- ## set_wait_range / set_attack_range

-- Configures the randomization ranges (seconds) for wait and attack durations.
-- On restart, a random value is picked uniformly within [min, max].

	ph:set_wait_range( min, max )
	ph:set_attack_range( min, max )

-- ## set_id_rnd_nb

-- Sets the size of the random ID pool. On each restart, id_rnd is set to a
-- random integer in [1, nb]. Access it from the return values of set_time /
-- inc_time. Pass nil or do not call to disable random IDs.

	ph:set_id_rnd_nb( nb )

-- ## restart

-- Manually restarts the cycle: re-randomizes all durations and the random ID,
-- resets time to 0.

	ph:restart()

-- ## inc_time

-- Advances time by dt seconds and returns the result of set_time.
-- Typical call: phase, id_rnd [, b_done] = ph:inc_time( aaa.time.dt )

	phase, id_rnd [, b_done] = ph:inc_time( dt )


-- # PHASOR_A

-- Attack-only phasor (inherits from PHASOR). Each cycle is: wait, then attack.
-- During wait, phase is 0. During attack, phase ramps from 0 to 1 linearly.
-- When attack completes, the cycle auto-restarts with new random durations.

-- ## create

	ph = PHASOR_A:create( name, wait_min, wait_max, attack_min, attack_max )

-- ## set_time

-- Sets the absolute time and returns the current state.
-- Returns: phase (float), id_rnd (int or nil), b_done (true on cycle boundary).
--
-- Phase values by state:
--   "wait"   : phase = 0
--   "attack" : phase in (0, 1), linearly increasing
--   "done"   : phase = 0, b_done = true (cycle just restarted)

	phase, id_rnd [, b_done] = ph:set_time( t )


-- # PHASOR_ASR

-- Attack-Sustain-Release phasor (inherits from PHASOR). Each cycle is:
-- wait, attack, sustain, release. The phase ramps up during attack, holds at 1
-- during sustain, then ramps back down to 0 during release.

-- ## create

-- All eight range values are required: wait min/max, attack min/max,
-- sustain min/max, release min/max.

	ph = PHASOR_ASR:create( name, w_min,w_max, a_min,a_max, s_min,s_max, r_min,r_max )

-- ## set_sustain_range / set_release_range

-- Configures the sustain and release randomization ranges.

	ph:set_sustain_range( min, max )
	ph:set_release_range( min, max )

-- ## restart

-- Re-randomizes all four durations (wait, attack, sustain, release) and
-- the random ID. Resets time to 0.

	ph:restart()

-- ## set_time

-- Sets the absolute time and returns the current state.
-- Returns: phase (float), id_rnd (int or nil), b_done (true on cycle boundary).
--
-- Phase values by state:
--   "wait"    : phase = 0
--   "attack"  : phase in (0, 1), linearly increasing
--   "sustain" : phase = 1
--   "release" : phase in (1, 0), linearly decreasing
--   "done"    : phase = 0, b_done = true (cycle just restarted)

	phase, id_rnd [, b_done] = ph:set_time( t )


-- # math helpers

-- Two global functions added to the math table by this file.

-- ## math.randomf

-- Returns a uniform random float in [min, max].

	val = math.randomf( min, max )

-- ## math.randomf_pow

-- Returns a power-biased random float. The raw random [0,1] is raised to pow
-- before mapping to [min, max]. pow > 1 biases toward min, pow < 1 toward max.

	val = math.randomf_pow( min, max, pow )
