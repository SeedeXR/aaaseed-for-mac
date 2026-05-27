function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()
end
function meu:restart()
	param.set( self.ref.restart_trig, true )
end
function meu:init()
	local ref = self.ref
	ref.boid = self:get_layer_bdd( 2 )
	ref.restart_trig = param.get_ref( ref.boid, "restart_trig" )
	ref.speed_min = param.get_ref( ref.boid, "speed_min" )
	ref.viscosity = param.get_ref( ref.boid, "viscosity" )
end

function meu:update()
	local ref = self.ref
	local t = aaa.time.t
	local s = math.sin( t * .0456415) * .5 + .5
	param.set( ref.speed_min, s * .5 )
	--local r = self.r or 0
	--r = r + aaa.time.dt
	--if r > 180 then
	--	r = 0
	--	self:restart()
	--end
	--self.r = r
end