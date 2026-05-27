--todo transfer to aaa_math.lua when stable


--todo  in glsl include
--todo step_smooth too
function math.step_linear( val, a, b )
    --aaa.print_fn()
	if b < a then
		if val < b then			val = 1
		elseif a < val then		val = 0
		else					val =  ( val - a ) / ( b - a )
		end

	else
		if val < a then			val = 0
		elseif  b < val then	val = 1
		else					val =  ( val - a ) / ( b - a )
		end
	end
	return val
end

function math.step_interval_linear( val, a, b, c, d )
	if		val < a then	val = 0
	elseif  val < b then	val =  ( val - a ) / ( b - a )
	elseif	val < c then	val = 1
	elseif  val < d then	val =  ( val - d ) / ( c - d )
	else 					val = 0
	end
	return val
end

function math.step_interval_linear_table( val, tab )
	return math.step_interval_linear( val, unpack(tab) )
end
