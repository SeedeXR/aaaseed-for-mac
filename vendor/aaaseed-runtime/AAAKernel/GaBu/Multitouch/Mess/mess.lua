local mess = aaa.mess
if not mess.update then
	function mess.show_low( str, size, duration )
		mess.size = size or 1.
		mess.str = str
		mess.time_left = duration or 2.
		aaa.print( "aaa.mess.show : "..str )
	end 

	function mess.update()
		local t = mess.time_left
		if t then
			if t > 0. then
				local t = t - math.min( aaa.time.dt_real, .04 )
				t = math.max( t, 0 )	--avoid error when using power
				mess.time_left = t
				t = t * .2
				t = math.min( t, 1 )
				t = math.pow( t, .5 )
				--aaa.print( "----------------------------------------------- t is "..t )
				gol.reset()
				gol.color_magenta(t)
				local s = t*mess.size*.5
				BU:draw_text_nice_shadow( mess.str,			-2,1,0, s,s, "left", "left" )
			end
		end
	end
end

mess.update()
