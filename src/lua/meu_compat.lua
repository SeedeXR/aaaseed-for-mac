-- src/lua/meu_compat.lua
--
-- Phase 6 partial unblock : a tiny cross-platform compatibility shim for
-- the MEU (UI app) Lua side of AAASeed. AAASeed_Win-shipped MEUs reach
-- for the Windows-only `aaa.spout.send(name, tex)` to publish a render
-- target into another process via the Spout shared-texture protocol.
-- macOS has no Spout ; the moral equivalent is Syphon, exposed by the
-- Mac engine as `aaa.syphon.send(name, tex)`.
--
-- Rather than patch every MEU script call site (there are hundreds), we
-- install a thin one-way bridge here : whichever side the MEU script
-- happens to call, this shim forwards to the platform-native API. Pure
-- Lua, zero C-side cost, zero vendor edit.
--
-- The shim assumes a host-provided `aaa.platform_name()` returning one
-- of "mac" / "win" / "linux". On Mac that's wired from the engine's
-- AAA_OS_MACOS() branch ; on Win from AAA_OS_WINDOWS(). Tests inject a
-- stub `aaa.platform_name` so the shim is exercisable without the full
-- engine boot.
--
-- This file is loaded once at MEU init time (future GaBu integration).
-- Idempotent : repeated loads overwrite the wrappers with the same
-- closures.

local M = {}

--	Detect the host platform via the engine-provided helper. Returns
--	one of "mac" / "win" / "linux" / "unknown". Defensive against the
--	helper being absent (early-boot scripts may load before the C-side
--	`aaa` table is fully populated -- in that case we return "unknown"
--	rather than crashing).
function M.detect()
    if type(aaa) ~= "table" or type(aaa.platform_name) ~= "function" then
        return "unknown"
    end
    local name = aaa.platform_name()
    if type(name) ~= "string" or #name == 0 then
        return "unknown"
    end
    --	Normalise to the three known tokens. Accept a few common
    --	aliases the engine might emit (Darwin / macOS / Windows / ...).
    local lower = name:lower()
    if lower == "mac" or lower == "macos" or lower == "darwin" or lower == "osx" then
        return "mac"
    end
    if lower == "win" or lower == "windows" or lower:find("^win") then
        return "win"
    end
    if lower == "linux" then
        return "linux"
    end
    return lower
end

--	Install the spout <-> syphon bridge.
--	- On Mac : if MEU script calls `aaa.spout.send(name, tex)`, route
--	           it to `aaa.syphon.send(name, tex)`.
--	- On Win : reverse direction -- `aaa.syphon.send` routes to
--	           `aaa.spout.send`. (Symmetric, so a Mac-authored MEU runs
--	           unchanged on Win.)
--	The bridge is lazy : it only installs the missing-side table, so
--	if the engine provides BOTH (cross-platform build), the bridge
--	is a no-op.
function M.install()
    if type(aaa) ~= "table" then
        return false, "aaa table missing"
    end

    local platform = M.detect()

    if platform == "mac" then
        --	Provide a thin `aaa.spout` table that forwards to `aaa.syphon`.
        if type(aaa.syphon) == "table" and type(aaa.syphon.send) == "function" then
            aaa.spout = aaa.spout or {}
            aaa.spout.send = function(name, tex)
                return aaa.syphon.send(name, tex)
            end
        end
    elseif platform == "win" then
        --	Provide a thin `aaa.syphon` table that forwards to `aaa.spout`.
        if type(aaa.spout) == "table" and type(aaa.spout.send) == "function" then
            aaa.syphon = aaa.syphon or {}
            aaa.syphon.send = function(name, tex)
                return aaa.spout.send(name, tex)
            end
        end
    end
    --	Linux / unknown : nothing to bridge (no shared-texture API on
    --	this port yet). MEUs that call spout/syphon will get a normal
    --	Lua "attempt to index a nil value" if they try, which is the
    --	right diagnostic.

    return true
end

return M
