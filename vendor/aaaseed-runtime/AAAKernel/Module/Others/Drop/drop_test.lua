-- drop of water done for water interactive table

if not drop then
	drop = {}
	drop.ref = {}
end

	local ref = drop.ref
	drop.ref.sphere			= aaa.obj.get( "drop.sphere" )
	drop.ref.x					= param.get_ref( drop.ref.sphere, "center_x" )
	drop.ref.y					= param.get_ref( drop.ref.sphere, "center_y" )
	drop.ref.layer_att		= aaa.obj.get( "drop.layer_att" )
	drop.ref.layer			= aaa.obj.get( "drop.layer" )
	drop.ref.model			= aaa.obj.get( "drop.model" )
	drop.ref.size				= param.get_ref( drop.ref.model, "size_factor" )
	drop.ref.turb1			= aaa.obj.get( "drop.turb1" )
	drop.ref.turb1_phase		= param.get_ref( drop.ref.turb1, "phase" )
	drop.ref.turb2			= aaa.obj.get( "drop.turb2" )
	drop.ref.turb2_phase		= param.get_ref( drop.ref.turb2, "phase" )

function drop.draw( x, y, size, phase )
	param.set( drop.ref.turb1_phase,		phase * 1.02565487 + .598745		)
	param.set( drop.ref.turb2_phase,		-phase * 0.9465475487 - .4446462139	)
	param.set( drop.ref.x,					x )
	param.set( drop.ref.y,					y )
	param.set( drop.ref.size,				size )
	aaa.obj.update_then_draw( drop.ref.layer )
end

function drop.test()
	math.randomseed( 9876 )
	aaa.obj.update_then_draw( drop.ref.layer_att )
	local rnd = math.random_centered
	for i=1,200 do
		drop.draw( rnd( 7.5 ), rnd( 4 ), .3, rnd( 1 )*aaa.time.t )
	end
end

drop.test()