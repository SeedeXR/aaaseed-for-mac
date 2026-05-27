function meu:define_ui()
    self:add_camera()
end

function meu:init()
    self.ref.bdd_part = self:get_layer_bdd(1)
end

function meu:get_bdd_part()
    return self.ref.bdd_part
end