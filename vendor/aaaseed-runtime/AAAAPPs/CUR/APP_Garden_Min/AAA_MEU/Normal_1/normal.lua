function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	--ui.bu_col = self:add_rgbf(	{1,8.2 } )

	self:add_bu_texture_target_unit()

	self:add_camera()
	self:add_shading_ui()

	bu = self:add_slider(	{9,10,	8,1},	"Speed" , self, "speed", 1, -1, 1 )
		bu:set_meter( false )
end

function meu:init()
	self.time = 0
--	self:print( "je passe ds l'init" )
	self:add_shading( 1, "Normal" )
end

function meu:update()
	local t = self.time
	t = t + aaa.time.dt * math.pow( self.speed*4, 3 )
	self.time = t
	local sha = self:get_shading()
	sha:set_frag_float_4( t )

	--self:print( self.ui.bu_col:get_rgba() )
end
