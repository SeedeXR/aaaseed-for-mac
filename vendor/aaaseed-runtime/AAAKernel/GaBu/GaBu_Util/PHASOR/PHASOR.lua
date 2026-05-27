
--todo move to math qq chose
function math.randomf( min, max ) return  math.random() * (max - min) + min end
function math.randomf_pow( min, max, pow )
	local rnd = math.pow( math.random(), pow )
	return  rnd * (max - min) + min
end

if CLASS.DECLARE( "PHASOR" ) then
	PHASOR:set_class_status_doc(	CLASS.STATUS.CORE,
									"Base class for an object which after a wait generate a signal or phase",
									"instances seems to be also a PHASOR_A (2023 July Mâa)" )
end

function PHASOR:set_wait_range( min, max )
	self.wait_min = min
	self.wait_max = max
	self.wait = min
end
function PHASOR:set_attack_range( min, max )
	self.attack_min = min
	self.attack_max = max
	self.attack = min
end
function PHASOR:init( wait_min, wait_max, attack_min, attack_max )
	self:set_wait_range( wait_min, wait_max )
	self:set_attack_range( attack_min, attack_max )
	self.t = 0

end
function PHASOR:set_id_rnd_nb( nb )
	self.id_rnd_nb = nb
	self.id_rnd = math.random(nb)
end
function PHASOR:create( name, wait_min, wait_max, attack_min, attack_max )
	local self = PHASOR:create_instance( name )
	self:init( wait_min, wait_max, attack_min, attack_max )
	return self
end
function PHASOR:restart()
	self.wait  = math.randomf( self.wait_max  	,self.wait_min 	)
	self.attack = math.randomf( self.attack_max	,self.attack_min	)
	local nb = self.id_rnd_nb
	if nb then
		self.id_rnd = math.random(nb)
		--self:print( "id_rnd is "..self.id_rnd.." / "..nb )
	end
	self.t = 0
end
function PHASOR:inc_time( dt )
	return self:set_time(  self.t + dt )
end

if CLASS.DECLARE( "PHASOR_A", PHASOR ) then
	PHASOR_A:set_class_status_doc(	CLASS.STATUS.CORE,
									"PHASOR having just an Attack signal" )
end

function PHASOR_A:create( name, wait_min, wait_max, attack_min, attack_max )
	local self = PHASOR_A:create_instance( name )
	self:init( wait_min, wait_max, attack_min, attack_max )
	return self
end
function PHASOR_A:set_time( t )
	self.t = t
	if t <= self.wait then
		self.state = "wait"
		return 0, self.id_rnd
	end
	t = t - self.wait

	if t < self.attack then
		self.state = "attack"
		return t/self.attack, self.id_rnd
	end
	t = t - self.attack

	self.state = "done"
	self:restart()
	self.t = t
	return 0, self.id_rnd, true
end

if CLASS.DECLARE( "PHASOR_ASR", PHASOR ) then
	PHASOR_ASR:set_class_status_doc(	CLASS.STATUS.CORE,
									"PHASOR having an Attack / Sustain / Release signal" )
end

function PHASOR_ASR:set_sustain_range( min, max )
	self.sustain_min = min
	self.sustain_max = max
	self.sustain = min
end
function PHASOR_ASR:set_release_range( min, max )
	self.release_min = min
	self.release_max = max
	self.release = min
end
function PHASOR_ASR:init( wait_min,wait_max, a_min,a_max, s_min,s_max, r_min,r_max )
	PHASOR.init( self, wait_min,wait_max, a_min,a_max )
	self:set_sustain_range(	s_min,		s_max )
	self:set_release_range(	r_min,		r_max )
end
function PHASOR_ASR:create( name, wait_min,wait_max, a_min,a_max, s_min,s_max, d_min,d_max )
	local self = PHASOR_ASR:create_instance( name )
	self:init( wait_min,wait_max, a_min,a_max, s_min,s_max, d_min,d_max )
	return self
end

function PHASOR_ASR:restart()
	PHASOR.restart( self )
	self.sustain	= math.randomf( self.sustain_max	,self.sustain_min	)
	self.release	= math.randomf( self.release_max	,self.release_min	)
	self.t = 0
end
function PHASOR_ASR:set_time( t )
	self.t = t
	if t <= self.wait then
		self.state = "wait"
		return 0, self.id_rnd
	end
	t = t - self.wait

	if t < self.attack then
		self.state = "attack"
		return t/self.attack, self.id_rnd
	end
	t = t - self.attack

	if t < self.sustain then
		self.state = "sustain"
		return 1, self.id_rnd
	end
	t = t - self.sustain

	if t < self.release then
		self.state = "release"
		return 1-t/self.release, self.id_rnd
	end
	t = t - self.release

	self.state = "done"
	self:restart()
	self.t = t
	return 0, self.id_rnd, true
end
