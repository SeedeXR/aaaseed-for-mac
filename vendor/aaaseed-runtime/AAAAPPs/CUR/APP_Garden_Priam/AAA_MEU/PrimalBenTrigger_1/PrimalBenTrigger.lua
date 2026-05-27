function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu

	local ix = 1
	local iy = 1
	local ix,iy = 1,1
    local SY,DY = 1,.2
    local range_info = {}
    self.range = range_info
    range_info.trigger = false
    
    bu = self:add_text({ix + 5, iy, 8, SY}, "MEU IN"):set_value_load_save(true)
        ui.meu_in_info = bu
    self:add_trig_method({ix, iy, 4, SY}, "Edit MEU in", self, "edit_meu_in"):set_color_back( "load" )
    bu = self:add_text(	{ix + 5, iy + 1, 8, SY}, "BU IN" ):set_value_load_save(true)
        ui.bu_in_info = bu
    self:add_trig_method({ix, iy + 1, 4, SY}, "Edit BU in", self, "edit_bu_in"):set_color_back( "load" )
    bu = self:add_slider_two({ix+2, iy + 2, 12, SY}, "range", range_info, "min", "max", 0.5, 1, 0, 1)
    bu = self:add_text(	{ix + 5, iy + 4, 8, SY}, "MEU OUT" ):set_value_load_save(true)
        ui.meu_out_info = bu
    self:add_trig_method({ix, iy + 4, 4, SY}, "Edit MEU out", self, "edit_meu_out"):set_color_back( "load" )
    bu = self:add_text(	{ix + 5, iy + 5, 8, SY}, "BU OUT" ):set_value_load_save(true)
        ui.bu_out_info = bu
    self:add_trig_method({ix, iy + 5, 4, SY}, "Edit BU out", self, "edit_bu_out"):set_color_back( "load" )
end

function meu:init()
	local ref = self.ref 
	local bdd =	self:get_layer_bdd(1)
end

function meu:update()
    self:set_output_from_input()
end

function meu:set_output_from_input()
    -- Get the text for the input and output MEUs and BUs
    local meu_in_string = self.ui.meu_in_info:get_text()
    local bu_in_string = self.ui.bu_in_info:get_text()
    local meu_out_string = self.ui.meu_out_info:get_text()
    local bu_out_string = self.ui.bu_out_info:get_text()
    -- Get the range info configured with the slider
    local r = self.range
    -- Get the input MEU
    local meu_in = self:get_meu_by_name_no_error(meu_in_string)
    local bu_in_val = nil
    if meu_in ~= nil and type(meu_in["get_bu_value"]) == "function" then
        -- Protect against invalid inputs
        if pcall(meu_in.get_bu_value, meu_in, bu_in_string) then 
            bu_in_val = meu_in:get_bu_value(bu_in_string)
        else
            self:update_trigger_state(0, r)
            self:get_bu_by_key("range"):set_text("Invalid input BU")
        end
        -- Check if we should send this value
        if bu_in_val ~= nil then
            local input_type = type(bu_in_val)
            -- Inform user if input is an unsupported type and exit
            if input_type ~= "boolean" and input_type ~= "number" then
                self:get_bu_by_key("range"):set_text("Invalid input type: " .. input_type)
                return
            end
            -- Convert boolean inputs to number, 1 for true and 0 for false
            if type(bu_in_val) == "boolean" then
                bu_in_val = bu_in_val and 1 or 0
            end
            -- Display the current input value
            self:get_bu_by_key("range"):set_text(bu_in_val)
            -- Note that this only sends a value to the output when the trigger state CHANGES
            if self:update_trigger_state(bu_in_val, r) then
                self:print("Updating trigger state: " .. r.trigger)
                -- Get the output MEU
                local meu_out = self:get_meu_by_name_no_error(meu_out_string)
                if meu_out then
                    -- Set the output BU value 
                    meu_out:set_bu_value(bu_out_string, r.trigger)
                end
            end
        end
    else
        self:update_trigger_state(0, r)
        self:get_bu_by_key("range"):set_text("Invalid input BU")
    end
end

function meu:update_trigger_state(val, r)
    local trigger_state = r.trigger
    -- Check if the value falls within bounds
    if (val >= r.min) and (val <= r.max) then
        r.trigger = true
    else
        r.trigger = false
    end
    return trigger_state ~= r.trigger
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