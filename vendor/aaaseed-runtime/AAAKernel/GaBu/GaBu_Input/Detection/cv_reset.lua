if GABUIN.cv_reset == nil then
	GABUIN.cv_reset = { ref={} }
	GABUIN.cv_reset.ref.cv = GABUIN.get_cv_multitouch()
	GABUIN.cv_reset.ref.cv_reset = param.get_ref( GABUIN.cv_reset.ref.cv, "remanence_restart_trig" )
	GABUIN.cv_reset.ref.time = aaa.time.ref.time
	GABUIN.cv_reset.nb_todo = 10
end
function GABUIN.cv_reset_background()
	param.set( GABUIN.cv_reset.ref.cv_reset, 1 )
	aaa.mess.show( "Reset Multitouch Background" )
	local c = GABUIN.cv_reset.nb_todo
	if c > 0 then
		GABUIN.cv_reset.nb_todo = c - 1
	end
end
if GABUIN.cv_reset.nb_todo > 0 or aaa.time.t_real < 3. then
	GABUIN.cv_reset_background()
end

