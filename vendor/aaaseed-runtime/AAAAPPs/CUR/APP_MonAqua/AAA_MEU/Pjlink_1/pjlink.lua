
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	iy = 3
--	bu = self:add_button(	{ix, iy,		SY, SY },	"Active", 	ref.active, nil, true )
--	iy = iy + SY


	--local i = 1;

end

function meu:init()
	local ref = self.ref

	self.time = aaa.time.t
	--todomonaq use BDD_FBX as for non matte stuff
--	local yocto = self:get_layer_bdd( 1 )



end

function meu:send_command()

end

function meu:update()
	local ui = self.ui
	local ref = self.ref
	--self.last_input = -1

--	aaa.net.lua_send( 0, 0, "if app then app:set_screens_white( "..( false and "true" or "false").." ) end" )

end

function meu:draw()
 	self:draw_layers_begin()
 		self:draw_layer( 1 )
	self:draw_layers_end()
end




