boid_ref = boid_ref or aaa.obj.get_by_name_symbo( "dev_boid_create" )

function poid.hook_do_kill( boids_id, id )
	aaa.print( "kill "..boids_id.." "..id )
	x,y,z = poid.get_pos()
	--aaa.print( "  at "..x.." "..y.." "..z )
	x,y,z = poid.get_pos_to_draw()
	--aaa.print( "  drawn "..x.." "..y.." "..z )
	x,y,z = poid.get_speed()
	--aaa.print( "  speed "..x.." "..y.." "..z )
	if boids_id == 2 then
		x,y,z = poid.get_pos()

		poid.create( boid_ref, x, y, z );
		poid.create( boid_ref, x, y, z );
		poid.create( boid_ref, x, y, z );
		poid.create( boid_ref, x, y, z );
		poid.create( boid_ref, x, y, z );

		poid.create( boid_ref, x, y, z );
		poid.create( boid_ref, x, y, z );
		poid.create( boid_ref, x, y, z );
		poid.create( boid_ref, x, y, z );
		poid.create( boid_ref, x, y, z );
	end
end

function poid.hook_do_create( boids_id, id )
	--aaa.print( "create "..boids_id.." "..id )
	x,y,z = poid.get_pos()
	aaa.print( "  at "..x.." "..y.." "..z )
end

function poids.hook_is_repulse( ba, bb )
	if ba == 2 then
		if bb == 1 then
			ret = 1
		elseif bb == ba then
			ret = 1
		else
			ret = 0
		end
	else
		ret = 1.
	end
	qaa.print( "interact "..ba.." "..bb.." -> "..ret )
	return ret
end