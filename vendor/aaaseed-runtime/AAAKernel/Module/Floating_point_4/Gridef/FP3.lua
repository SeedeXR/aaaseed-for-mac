--aaa.print( "FP3 is here" )
--fp3 = nil
if aaa.lua.global.declare_table( "fp3" ) then
	fp3.used = 0
	fp3.rnd = false
end

fp3.TIME_TO_ON = 15.
fp3.TIME_TO_OFF = 5.
fp3.COV_FACTOR = 5.

local function fp3_init_ref()
	fp3.ref = {}
	local ref = fp3.ref
	ref.anal = aaa.obj.get( "fp3_anal" )
	ref.cov = param.get_ref( ref.anal, "out_coverage" )

	ref.boid = aaa.obj.get( "fp3_boid" )
	ref.steer			= param.get_ref( ref.boid, "steering_influence" )
	ref.flock			= param.get_ref( ref.boid, "flocking_influence" )
	ref.repulse_draw	= param.get_ref( ref.boid, "repulse_draw_factor" )

--todo
--	ref.values = aaa.obj.get( "Module/Face/Gridef/fx.values" )
--	ref.val1 = param.get_ref( ref.values, "value_01" )
--	ref.val2 = param.get_ref( ref.values, "value_02" )
--	ref.val3 = param.get_ref( ref.values, "value_03" )
end

fp3_init_ref()

local function fp3_update()
	local ref = fp3.ref
	local cov = param.get( ref.cov ) * fp3.COV_FACTOR
	local dt = aaa.time.dt
	if cov > 1 then
		fp3.used = fp3.used + (cov*dt) / fp3.TIME_TO_ON
	elseif cov <= 0 then
		fp3.used = fp3.used - dt*2. / fp3.TIME_TO_OFF
	end
	if fp3.used > 2. then
		fp3.used = 2.
	elseif fp3.used < 0 then
		fp3.used = 0.
	end
	local inf = fp3.used - 1.
	--aaa.print( fp3.used )

	if inf < 0. then
		fp3.rnd = false
		return
	end

	if fp3.rnd == false then
		fp3.rnd = true
		param.set( ref.steer, 0 )
		param.set( ref.flock, 0 )
		param.set( ref.repulse_draw, 0 )
	end

--	local val1 = param.get( ref.val1 ) * inf
--	local val2 = param.get( ref.val2 ) * inf
--	local val3 = param.get( ref.val3 ) * inf

	--aaa.print( fp3.used )

	param.set( ref.steer, val1 )
	param.set( ref.flock, val2 )
	param.set( ref.repulse_draw, val3 )
end

fp3_update()

