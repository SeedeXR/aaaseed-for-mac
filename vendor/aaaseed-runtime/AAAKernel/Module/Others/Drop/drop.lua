if aaa.lua.global.declare_table( "drop" ) then
	-- drop of water done for water interactive table
	drop.ref = {}

	local ref = drop.ref
	ref.sphere			= aaa.obj.get( "drop.sphere" )
	ref.x					= param.get_ref( ref.sphere, "center_x" )
	ref.y					= param.get_ref( ref.sphere, "center_y" )
	ref.layer_att		= aaa.obj.get( "drop.layer_att" )
	ref.layer			= aaa.obj.get( "drop.layer" )
	ref.bind_reflecion		= param.get_ref( ref.layer, "bind_2d" )
	ref.tu_back			= aaa.obj.get( "drop.tu.background" )
	ref.bank_back			= param.get_ref( ref.tu_back, "bank_2d" )
	ref.bind_back			= param.get_ref( ref.tu_back, "bind_2d" )
	ref.model			= aaa.obj.get( "drop.model" )
	ref.size				= param.get_ref( ref.model, "size_factor" )
	ref.turb1			= aaa.obj.get( "drop.turb1" )
	ref.turb1_phase			= param.get_ref( ref.turb1, "phase" )
	ref.turb1_period		= param.get_ref( ref.turb1, "space_period" )
	ref.turb1_factor		= param.get_ref( ref.turb1, "strenght" )
	ref.turb2			= aaa.obj.get( "drop.turb2" )
	ref.turb2_phase			= param.get_ref( ref.turb2, "phase" )
	ref.turb2_period		= param.get_ref( ref.turb2, "space_period" )
	ref.turb2_factor		= param.get_ref( ref.turb2, "strenght" )
	ref.transfo			= aaa.obj.get( "drop.transfo" )
	ref.transfo_x			= param.get_ref( ref.transfo, "translate_x" )
	ref.transfo_y			= param.get_ref( ref.transfo, "translate_y" )
	ref.shader			= aaa.obj.get( "drop.shader" )
	ref.tex_factor_x		= param.get_ref( ref.shader, "fu_float_01" )
	ref.tex_factor_y		= param.get_ref( ref.shader, "fu_float_02" )
	ref.tex_factor_normal	= param.get_ref( ref.shader, "fu_float_03" )
end


function drop.set_binds( background, reflection )
	local ref = drop.ref
	param.set( ref.bind_reflecion, reflection )
	local ba, bi = aaa.img.make_bank_bind_2d( background )
	param.set( ref.bank_back, ba )
	param.set( ref.bind_back, bi )
	drop.bind = background;
end

function drop.draw_before()
	aaa.obj.update_then_draw( drop.ref.layer_att )
end

function drop.draw( x, y, size, phase )
	local ref = drop.ref

	local size_to_turb = .1
	param.set( ref.turb1_phase,		phase * 1.02565487 + .598745		)
	param.set( ref.turb1_factor,	size * size_to_turb	)
	param.set( ref.turb2_phase,		-phase * 0.9465475487 - .4446462139	)
	param.set( ref.turb2_factor,	size * size_to_turb	)
	param.set( ref.x,				0		)
	param.set( ref.y,				0		)
	param.set( ref.transfo_x,		x		)
	param.set( ref.transfo_y,		y		)
	param.set( ref.size,			size	)

	local sx,sy = aaa.img.get_size( drop.bind )
	--local sx,sy = 256, 256
	if sx and sy then
		if sx<sy then
			sy = sx/sy
			sx = 1
		else
			sx = sy/sx
			sy = 1
		end
		param.set( ref.tex_factor_x, 		.48 * sx/size )
		param.set( ref.tex_factor_y, 		.48 * sy/size )
		param.set( ref.tex_factor_normal, 	-.08 )
		aaa.obj.update_then_draw( ref.layer )
	end
end

function drop.test()
	math.randomseed( 9876 )
	local rnd = math.random_centered
	for i=1,100 do
		drop.set_binds( 5 + i % 28 , 29 )
		drop.draw_before()
		drop.draw( rnd( 7.5 ), rnd( 4 ), .3 + rnd( .4 ), rnd( 1 )*aaa.time.t )
	end
end

drop.test()

aaa.layers.skip_rest()