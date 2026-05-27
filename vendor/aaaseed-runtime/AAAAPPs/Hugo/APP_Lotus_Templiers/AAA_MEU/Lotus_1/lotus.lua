function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	self:add_bu_texture_target_unit()
	local ix,iy = 1,1

	self.petals = {}
	local SY = .8
	for i=1,12 do
		self:add_button( {9, 1+i*.8, SY, SY }, "Petal_"..i, self.petals, i, true )
	end
	self:add_slider(	{9,1,	8,1}, "Rot", self, "rot" )
end

function meu:init()
	local ref = self.ref
	local layers = self:get_layers()
	local obj = aaa.obj.get_down( layers, "lotus_trs1" )
	ref.rot = param.get_ref( obj, "rotate_y" )
	local obj = aaa.obj.get_down( layers, "lotus_trs2" )
	ref.trs_y = param.get_ref( obj, "translate_y" )
	ref.trs_z = param.get_ref( obj, "translate_z" )
	ref.rot2 = param.get_ref( obj, "rotate_x" )
end

function meu:draw()
	MEU.draw(self)
	app.meu_petal:draw_flower( self.rot, self.petals )
end


