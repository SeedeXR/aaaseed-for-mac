function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
    local param_set = param.set

	local ix,iy = 1,1
    local SY,DY = 1,.2
    local const_info = {}
    self.const = const_info
    local ramp_info = {}
    self.ramp = ramp_info
    local sine_info = {}
    self.sine = sine_info
    
    -- BU IN should provide boolean data, which sets self.active to determine whether this MEU is sending values to the output
    bu = self:add_text({ix + 5, iy, 8, SY}, "MEU IN"):set_value_load_save(true)
        ui.meu_in_info = bu
    self:add_trig_method({ix, iy, 4, SY}, "Edit MEU in", self, "edit_meu_in"):set_color_back( "load" )
    bu = self:add_text({ix + 5, iy + 1, 8, SY}, "BU IN" ):set_value_load_save(true)
        ui.bu_in_info = bu
    self:add_trig_method({ix, iy + 1, 4, SY}, "Edit BU in", self, "edit_bu_in"):set_color_back( "load" )

    bu = self:add_button({ix+4, iy + 3, SY, SY}, "Active", self, "active", true):set_text_visible(true)
    
    bu = self:add_text({ix + 5, iy + 8, 8, SY}, "MEU OUT" ):set_value_load_save(true)
        ui.meu_out_info = bu
    self:add_trig_method({ix, iy + 8, 4, SY}, "Edit MEU out", self, "edit_meu_out"):set_color_back( "load" )
    bu = self:add_text(	{ix + 5, iy + 9, 8, SY}, "BU OUT" ):set_value_load_save(true)
        ui.bu_out_info = bu
    self:add_trig_method({ix, iy + 9, 4, SY}, "Edit BU out", self, "edit_bu_out"):set_color_back( "load" )
    
    self:set_tab_key("Const")
    bu = self:add_slider({ix + 2, iy + 4, 12, SY}, "Const", const_info, "out", 0.5, 0, 1)

    self:set_tab_key("Ramp")
    bu = self:add_button({ix+8, iy + 3, SY, SY}, "Loop Ramp", ramp_info, "loop_ramp", true):set_text_visible(true)
    bu = self:add_slider_two({ix+2, iy + 4, 12, SY}, "Ramp Range", ramp_info, "min", "max", 0, 1, 0, 1)
    bu = self:add_slider({ix+2, iy + 5, 12, SY}, "Ramp Time", ramp_info, "time", 5, 0, 10)
    bu = self:add_slider({ix+2, iy + 6, 12, SY}, "Ramp Out", ramp_info, "out", 0, ramp_info.min, ramp_info.max)
    ramp_info.progress = 0
    
    self:set_tab_key("Sine")
    bu = self:add_button({ix+8, iy + 3, SY, SY}, "Loop Sine", sine_info, "loop_sine", true):set_text_visible(true)
    bu = self:add_slider({ix+2, iy + 4, 12, SY}, "Amplitude", sine_info, "amp", 1, 0, 10)
    bu = self:add_slider({ix+2, iy + 5, 12, SY}, "Period", sine_info, "period", 5, 0, 10)
    bu = self:add_slider({ix+2, iy + 6, 12, SY}, "Sine Out", sine_info, "out", 0, -sine_info.amp, sine_info.amp)
    sine_info.progress = 0

end

function meu:update()
    self:set_active_from_input()
    -- Only send values when active is true
    if self.active then
        local tab = self:get_tab_key()
        self:tick(tab)
    end
end

function meu:set_active_from_input()
    -- Get the text for the input MEU and BU
    --param.set(self.ref.text, self.ui.meu_in_info:get_text())
    local meu_in_string = self.ui.meu_in_info:get_text()
    local bu_in_string = self.ui.bu_in_info:get_text()
    
    -- Get the input MEU
    local meu_in = self:get_meu_by_name_no_error(meu_in_string)
    if meu_in then
        -- Get the value of the input BU in the input MEU
        local bu_in_val = meu_in:get_bu_value(bu_in_string)
        -- Set active based on the input value
        if bu_in_val then
            self.active = bu_in_val
        end
    end
end

function meu:send_value(val)
    local meu_out_string = self.ui.meu_out_info:get_text()
    local bu_out_string = self.ui.bu_out_info:get_text()
    local meu_out = self:get_meu_by_name_no_error(meu_out_string)
    if meu_out then
        -- Set the output BU value 
        meu_out:set_bu_value(bu_out_string, val)
    end
end

-- Generic tick function which encapsulates sending and incrementing values based on the currently selected tab
function meu:tick(tab)
    if tab == "const" then
        self:send_value(self.const.out)
    elseif tab == "ramp" then
        self:send_value(self.ramp.out)
        self:tick_ramp()
    elseif tab == "sine" then
        self:send_value(self.sine.out)
        self:tick_sine()
    end
end

-- Increments the state of the linear ramp
function meu:tick_ramp()
    local progress_step = aaa.time.dt/self.ramp.time
    self.ramp.progress = self.ramp.progress + progress_step
    -- Cap progress at 1
    if self.ramp.progress >= 1 then
        self.ramp.progress = 1
    end

    -- Set out value
    self.ramp.out = self.ramp.min + ((self.ramp.max - self.ramp.min) * self.ramp.progress)
    -- Reset if looping or stop if not
    if self.ramp.progress == 1 and self.ramp.loop_ramp then
        self.ramp.progress = 0
    elseif self.ramp.progress == 1 then
        self.active = false
        self.ramp.progress = 0
    end
end

-- Increments the state of the sinusoid
function meu:tick_sine()
    local progress_step = aaa.time.dt/self.sine.period
    self.sine.progress = self.sine.progress + progress_step

    -- Cap progress at 1
    if self.sine.progress >= 1 then
        self.sine.progress = 1
    end

    self.sine.out = self.sine.amp * math.sin(2*math.pi * self.sine.progress)
    -- Reset if looping or stop if not
    if self.sine.progress == 1 and self.sine.loop_sine then
        self.sine.progress = 0
    elseif self.sine.progress == 1 then
        self.active = false
        self.sine.progress = 0
    end
end


-- Each of the functions below is used with a button press to open up the text dialog for the desired UI element
function meu:edit_meu_in(arg1, arg2)
    self.ui.meu_in_info:edit_dialog_param()
end

function meu:edit_bu_in(arg1, arg2)
    self.ui.bu_in_info:edit_dialog_param()
end

function meu:edit_meu_out(arg1, arg2)
    self.ui.meu_out_info:edit_dialog_param()
end

function meu:edit_bu_out(arg1, arg2)
    self.ui.bu_out_info:edit_dialog_param()
end