
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	ui.cam = self:add_camera()

	local ix,iy = 1,1

	bu = self:add_slider(	{ix,iy,	8,1}, "Presence" )

end

function meu:init()
	local ref = self.ref
	self.id_trak = (self:get_inst_key() == "1") and 1 or 2

	ref.bdd_anal = self:get_layer_bdd( 1 )
	ref.coverage = param.get_ref( ref.bdd_anal, "out_coverage" )
end
function meu:update_ui()
	--self:print( "   "..self.presence )
	self:set_bu_value( "presence", self.presence )
end
function meu:update()
	if not app.b_simul then
		local ref = self.ref
		local pget = param.get
		local presence = pget( ref.coverage )
		--self:print( presence )
		self.presence = presence
		app:get_zone( self.id_trak ).presence_raw = presence
	end
end

