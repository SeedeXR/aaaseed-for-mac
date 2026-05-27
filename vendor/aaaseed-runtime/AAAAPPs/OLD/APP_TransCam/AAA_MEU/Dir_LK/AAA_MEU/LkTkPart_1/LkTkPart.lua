function meu:draw()
	if aaa.midi.get_control( 3, self.__inst_number ) > 0. then
		MEU.draw( self )
	end
end
function meu:init()
	self.__inst_number = tonumber(self:get_inst_key())
end