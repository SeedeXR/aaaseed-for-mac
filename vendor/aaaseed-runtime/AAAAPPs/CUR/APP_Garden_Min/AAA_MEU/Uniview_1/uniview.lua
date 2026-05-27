function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ui = self.ui

	self:add_camera()

	local ix, iy = 1,1
	self.speed = 1.
	bu = self:add_slider(	{ix+1,iy,	7,1}, "speed", self, "speed" )
	self:add_trig_restart(	{ix,iy+1})

end

function meu:init()
end

function meu:restart()
	self.phase = -.03
end


function meu:draw()
	MEU.draw( self )
end
