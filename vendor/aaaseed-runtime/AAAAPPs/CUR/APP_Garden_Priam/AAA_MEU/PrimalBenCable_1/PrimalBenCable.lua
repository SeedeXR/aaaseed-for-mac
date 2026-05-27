function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
    local param_set = param.set

	local ix = 1
	local iy = 1
	local ix,iy = 1,1
    local SY,DY = 1,.2
    local range_info = {}

    
    self.range = range_info
    bu = self:add_text({ix + 5, iy, 8, SY}, "MEU IN"):set_value_load_save(true)
        ui.meu_in_info = bu
    self:add_trig_method({ix, iy, 4, SY}, "Edit MEU in", self, "edit_meu_in"):set_color_back( "load" )
    bu = self:add_text(	{ix + 5, iy + 1, 8, SY}, "BU IN" ):set_value_load_save(true)
        ui.bu_in_info = bu
    self:add_trig_method({ix, iy + 1, 4, SY}, "Edit BU in", self, "edit_bu_in"):set_color_back( "load" )
    bu = self:add_slider_two({ix+2, iy + 2, 12, SY}, "range", range_info, "min", "max", 0, 1, 0, 1)
    bu = self:add_button({ix+4, iy+3, SY, SY}, "Bound", range_info, "bounded", false):set_text_visible(true)
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
    if meu_in then
        -- Get the value of the input BU in the input MEU
        local bu_in_val = meu_in:get_bu_value(bu_in_string)
        -- Check if we should send this value
        if bu_in_val then
            -- Display the current input value
            self:get_bu_by_key("range"):set_text(bu_in_val)
            if self:should_send_value(bu_in_val, r) then
                -- Get the output MEU
                local meu_out = self:get_meu_by_name_no_error(meu_out_string)
                if meu_out then
                    -- Set the output BU value 
                    meu_out:set_bu_value(bu_out_string, bu_in_val)
                end
            end
        end
    end
end

function meu:should_send_value(val, r)
    -- If the range is bounded
    if r.bounded then
        -- Check if the value falls within bounds
        if (val >= r.min) and (val <= r.max) then
            -- Send if so
            return true
        else
            -- Don't send if not
            return false
        end
    end
    -- Otherwise, just send
    return true
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