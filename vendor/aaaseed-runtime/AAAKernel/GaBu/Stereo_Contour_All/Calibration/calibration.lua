function calibration_init()
	if not calibration then
		calibration = {}
		calibration.ref = {}
		local ref = calibration.ref
		ref.group = aaa.get_group_cur()
		ref.bdd_grid = aaa.obj.get_by_name_symbo( "calibration_bdd_grid" )
		ref.nb_u = param.get_ref( ref.bdd_grid, "nb_u" )
		ref.nb_v = param.get_ref( ref.bdd_grid, "nb_v" )
	end
end

function calibration_update()
	calibration = nil
	calibration_init()

	calibration.nb_u = 5
	calibration.nb_v = 5
	param.set( calibration.ref.nb_u, calibration.nb_u )
	param.set( calibration.ref.nb_v, calibration.nb_v )
end

function calibration_do_action( command )
	if command == "start" then
		calibration.x = 1
		calibration.y = 1
	elseif command == "stop" then

	elseif command == "right" then
		local x = calibration.x + 1
		if x > calibration.nb_u then x = 1 end
		calibration.x = x
	elseif command == "left" then
		local x = calibration.x - 1
		if x < 1 then x = calibration.nb_u end
		calibration.x = x
	elseif command == "up" then
		local y = calibration.y + 1
		if y > calibration.nb_v then y = 1 end
		calibration.y = y
	elseif command == "down" then
		local y = calibration.y - 1
		if y < 1 then y = calibration.nb_v end
		calibration.y = y
	elseif command == "next" then
		local x = calibration.x + 1
		if x > calibration.nb_u then
			x = 1
			calibration_do_action( "up" )
		end
		calibration.x = x
	elseif command == "prev" then
		local x = calibration.x - 1
		if x < 1 then
			x = calibration.nb_u
			calibration_do_action( "down" )
		end
		calibration.x = x
	end
end

function calibration_do_key()
end

calibration_update()
