function meu:define_meu_infos()
    return { author = "Abdalight", date = "2025",
               tags = { "Tutorial" },
               help = "Example of controlling a BU in another MEU"
    }
end

function meu:define_ui()
    local ui = self.ui
    local ix, iy = 1, 1
    local SY, DY = 1, 0.2
    local bu

    local function add_text( rect, name )
		local bu =  self:add_text( rect, name )
		bu:set_value_load_save(true)
		return bu
	end

    bu = add_text(		  {ix,iy,   8,SY},  "Meu"   )
		bu:set_text( "MEU name" )
		bu:set_preset_use( true )
		ui.meu_name = bu
    iy = iy + SY + DY

	bu = add_text(		  {ix,iy,   8,SY},  "Bu"    )
		bu:set_text( "Bu name" )
		bu:set_preset_use( true )
		ui.bu_name = bu
    iy = iy + SY + DY

    bu = self:add_slider( {ix,iy,   8,SY,b_compact = true},  "Control Value",    self,   "control_value",        1,     -20,20   )
        ui.bu_control_value = bu
    iy = iy + SY + DY
end

function meu:init()
    self.control_value = 1
    self.last_MEU_name = "MEU name"
    self.last_Bu_name  = "Bu name"
    self.target_meu    = nil
    self.target_bu     = nil
end

function meu:update()
    local ui = self.ui
    local target_meu_name = ui.meu_name:get_text()
    local target_bu_name = ui.bu_name:get_text()

    self:update_target_bu_with_name_no_cashed( target_meu_name, target_bu_name )
    --self:update_target_bu()
end



function meu:update_target_bu_with_name_no_cashed( target_meu_name, target_bu_name )
    local target_meu = self.target_meu
    local target_bu  = self.target_bu

    if target_meu_name ~= self.last_MEU_name then
        self.target_meu    = self:get_meu_by_name_no_error( target_meu_name )
        self.last_MEU_name = target_meu_name
    end

    if target_bu_name ~= self.last_Bu_name then
        self.target_bu     = target_meu:get_bu_by_key( target_bu_name )
        self.last_Bu_name  = target_bu_name
    end

    if target_meu then
        if target_bu then 
            target_bu:set_value( self.control_value )
        end
    end
end


function meu:update_target_bu()
    local ui = self.ui
    local target_meu = self:get_meu_by_name_no_error( ui.meu_name:get_text() )
    if target_meu then
        local target_bu = target_meu:get_bu_by_key( ui.bu_name:get_text() )
        if target_bu then
            local success = target_bu:set_value( self.control_value )
            if not success then 
                self:print("Failed to update BU value. Check BU key or MEU.")
            end  
        end
    end
end


function meu:update_target_bu_with_alpha()
    local ui = self.ui
    local target_meu = self:get_meu_by_name_no_error( ui.meu_name:get_text() )
    local b_alpha = false
    if target_meu then
        local target_bu_name = ui.bu_name:get_text()
        if target_bu_name then
            local success = target_meu:set_bu_value( target_bu_name, self.control_value )
            if success then
                b_alpha = true
            else
                self:print("Failed to update BU value. Check BU key or MEU.")
            end
        end
    end
    ui.bu_control_value:interpolate_alpha_bu( b_alpha )
end
