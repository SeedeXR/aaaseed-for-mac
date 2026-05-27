function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
	local par

	local ix,iy = 1,1
	local DY = .2

	local bu = self:add_bu_texture_target_unit( {ix,iy, 8,6} )
	self:add_blending( {ix,iy+6} )

	self:add_rendering()

	self:add_nb_uv( {1, 12.5}, self:get_layer_bdd(1) )

	ix,iy = 1, 2 + DY
	self:add_camera()
	self:add_transfo( {9,iy, 8,2.4}, 1 )
	iy = iy + 2.4 + DY
end

function meu:init()
end

function meu:update()
--	local bu_tex = self:get_bu_texture()
--	table.print( bu_tex.__tex_state, "__tex_state" )
	if false then
		--bu_tex:set_bank_2d(0)
		self:print( "value is "..bu_tex.__t_bu.bank:get_value() )
		table.print( bu_tex.__tex_state, "__tex_state" )
		self:print( bu_tex.__t_bu.bind:get_min_max() )
	end
--	self:print( bu_tex.__t_bu.bank:get_min_max() )
--	self:print( bu_tex.__t_bu.bind:get_min_max() )
end

-- function meu:draw()
-- 	--gol.translate_y(-1)
-- 	MEU.draw( self )
-- end


