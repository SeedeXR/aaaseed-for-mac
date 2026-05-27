
if APP.DECLARE( "GARDEN_PRIAM", APP_GP ) then
end

local L_APP = GARDEN_PRIAM

function L_APP:init_app( ... )

	self:set_init_monitor_pass( true )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end
	self:lock(false)
	return true
end

function L_APP:get_capture_suv_meter()		return 4.67,	3.		end
function L_APP:get_capture_nb_uv()			return 1,	1		end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

-- Is this the best way to declare a global "class"?
aaa.lua.global.declare_table( "Ramp" )

-- RAMP VARIABLES
Ramp.start_val = 0  -- Start value of the ramp
Ramp.val = 0        -- Current value of the ramp
Ramp.end_val = 1    -- End value of the ramp
Ramp.t = 0          -- Relative time value of the ramp, always linear between 0-1
Ramp.progress = 0   -- Progress value of the ramp, progresses from 0-1 based on the current easing function
Ramp.time = 5       -- Time to go from start_val to end_val for the ramp
Ramp.active = false -- Whether the ramp is currently active
Ramp.start = false  -- Whether the ramp is being started. This is true between the calling of ramp(), ramp_to(), or ramp_from_to() and the first tick()
Ramp.loop = false   -- Whether the ramp should loop or not
Ramp.ease_type = "linear"
Ramp.ease_types = {"linear", "inout_cubic", "out_cubic"}
Ramp.cb_list = {}

-- RAMP FUNCTIONS
function Ramp:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function Ramp:ramp()
    -- Start ramp with existing start, end, time, loop parameters
    self.progress = 0
    self.t = 0
    self.start = true
    self.active = true
end

function Ramp:ramp_to(target_val, time)
    -- Start ramp with new end and time parameters
    self.start_val = val -- Begin the ramp from where we are currently to prevent jumping around
    self.end_val = target_val
    self.progress = 0
    self.t = 0
    self.time = time
    self.start = true
    self.active = true
end

function Ramp:ramp_from_to(start_val, target_val, time)
    -- Start ramp with new start, end, and time parameters
    self.start_val = start_val
    self.end_val = target_val
    self.progress = 0
    self.t = 0
    self.time = time
    self.start = true
    self.active = true
end

function Ramp:add_cb(progress, cb)
    self.cb_list[progress] = {callback=cb, fired=false}
end

function Ramp:set_loop(loop)
    if (type(loop) ~= "boolean") then
        return
    end
    self.loop = loop
end

function Ramp:tick()
    self.start = false
    local t_step = aaa.time.dt/self.time
    self.t = self.t + t_step
    self:ease()
    -- Cap progress at 1
    if self.progress >= 1 then
        self.progress = 1
        self.t = 1
    end
    self.val = self.start_val + ((self.end_val - self.start_val) * self.progress)

    -- Reset if looping or stop if not
    if self.progress == 1 and self.loop then
        self.progress = 0
        self.t = 0
    elseif self.progress == 1 then
        self.active = false
        self.progress = 0
        self.t = 0
    end
    return self.val
end

function Ramp:set_ease_type(ease_type)
    if self:has_value(self.ease_types, ease_type) then
        self.ease_type = ease_type
    end
end

function Ramp:ease()
    if self.ease_type == "linear" then
        self:ease_linear()
    elseif self.ease_type == "inout_cubic" then
        self:ease_inout_cubic()
    elseif self.ease_type == "out_cubic" then
        self:ease_out_cubic()
    end
end

function Ramp:ease_linear()
    self.progress = self.t
end

function Ramp:ease_inout_cubic()
    local x = self.t
    self.progress = x < 0.5 and 4*x*x*x or 1 - math.pow(-2*x+2, 3)/2
end

function Ramp:ease_out_cubic()
    self.progress = 1 - math.pow(1-self.t, 3)
end

function Ramp:has_value(tab, val)
    for idx, value in ipairs(tab) do
        if value == val then
            return true
        end
    end
    return false
end
