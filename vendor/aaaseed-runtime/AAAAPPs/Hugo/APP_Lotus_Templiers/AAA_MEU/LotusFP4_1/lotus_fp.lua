function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()
end

function meu:init()
	local ref = self.ref
	ref.boid = self:get_layer_bdd( 2 )
	ref.restart_trig = param.get_ref( ref.boid, "restart_trig" )
	ref.speed_min = param.get_ref( ref.boid, "speed_min" )
	ref.viscosity = param.get_ref( ref.boid, "viscosity" )
end
function meu:restart()
	local ref = self.ref
	param.set( ref.restart_trig, true )
end
function meu:update()
	local ref = self.ref

	self.meu_kinect		= self.meu_kinect or self:get_meu_by_name( "Kinect_2" )
	if self.meu_kinect then
		local t = aaa.time.t
		local g,a = self.meu_kinect:get_grey_and_agitation()
		local agi = self.agitation or 0
		a = clamp( (a-1) , -1, 1 )
		agi = agi + a  * aaa.time.dt * .25
		agi = clamp( agi, 0, 1 )
		self.agitation = agi
		aaa.show( agi, "agi" )
		local s = .125 * ( math.sin( t * .0456415 ) + 1. )
		--local s = agi * .5 + 1/32. * ( math.sin( t * .0456415) + 1. )
		param.set( ref.speed_min, s )

		local accum = self.accum or 0
		if agi >= .5 then
			accum = accum + aaa.time.dt * agi
			if accum > 12 then
				accum = 0
				--self:restart()
			end
		else
			accum = clamp( accum - aaa.time.dt, 0, 1 )
		end
		aaa.show( accum, "accum" )
		self.accum = accum
	end
end