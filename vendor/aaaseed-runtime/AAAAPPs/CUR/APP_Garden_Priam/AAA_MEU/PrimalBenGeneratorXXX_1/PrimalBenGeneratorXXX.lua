function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
    local param_set = param.set

	local ix,iy = 1,1
    local SY,DY = 1,.2
    -- This makes self.ramp a new instance of the Ramp class
    self.ramp = Ramp:new()
    -- All class FUNCTIONS must be called using :, NOT .
    self.ramp:set_ease_type("linear")
    --self.ramp:set_ease_type("inout_cubic") --Try uncommenting this and reloading to see the inout_cubic easing instead of linear

    -- This is one very direct way to use the Ramp class. We're hooking in the values of the sliders and booleans DIRECTLY into the variables in the Ramp class so that we can change everything on the fly, even while the ramp is in progress.
    -- The names here must directly correspond to the Ramp variable names defined in garden.lua. This can work very well so long as you're careful, as there's no error checking when you're manipulating the variables directly.
    bu = self:add_button({ix+4, iy + 3, SY, SY}, "Start", self.ramp, "start", false):set_text_visible(true)
    bu = self:add_button({ix+8, iy + 3, SY, SY}, "Loop Ramp", self.ramp, "loop", false):set_text_visible(true)
    bu = self:add_slider_two({ix+2, iy + 4, 12, SY}, "Ramp Range", self.ramp, "start_val", "end_val", 0, 1, 0, 1)
    bu = self:add_slider({ix+2, iy + 5, 12, SY}, "Ramp Time", self.ramp, "time", 5, 0, 10)
    bu = self:add_slider({ix+2, iy + 6, 12, SY}, "Ramp Out", self.ramp, "val", 0, self.ramp.start_val, self.ramp.end_val)

end

function meu:update()
    -- All class VARIABLES are accessed using .
    if self.ramp.start then
        self.ramp:ramp() -- This begins the ramp with whatever parameters it already has. We can do it this way because of how we've bound those variables to the sliders/booleans above.
    end
    if self.ramp.active then
        self.ramp:tick() -- We must call tick() every frame so that the ramp progresses.
    end
    
    -- Another way of doing this that's "safer" and less direct would be to bind the sliders/booleans to a separate table (self.ramp_info in previous examples), and then start the ramp using self.ramp:ramp_from_to(self.ramp_info.start, self.ramp_info.end, self.ramp_info.time) or similar.
    -- The benefit of direct binding, however, is that as you can see here in update() we only have some extremely basic logic and two function calls in order to execute the full life cycle of the ramp.
end