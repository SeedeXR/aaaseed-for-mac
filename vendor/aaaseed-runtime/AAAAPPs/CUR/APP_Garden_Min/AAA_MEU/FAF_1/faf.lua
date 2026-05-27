
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

end

function meu:init_spe()
	local tmeu = {}
	self.tmeu = tmeu

	tmeu.dof		= self:get_meu_by_name( "Ctl6Dof_1" )
	tmeu.word		= self:get_meu_by_name( "BoidWord_1" )
	tmeu.shader		= self:get_meu_by_name( "Shader_1" )
	tmeu.gridsel	= self:get_mu_by_name( "GridSel_Particles" )
	tmeu.dof:set_ui_slot( 2 )
	tmeu.word:set_ui_slot( 4 )
	tmeu.shader:set_ui_slot( 3 )
	tmeu.gridsel:set_ui_slot( 1 )
end


function meu:update()
	local cnt = self.count_spe or 1
	if cnt < 20 then
		cnt = cnt + 1
		if cnt == 15 then
			self:init_spe()
		end
		self.count_spe = cnt
		return
	end
end

