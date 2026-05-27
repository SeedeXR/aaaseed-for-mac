if not sercel then
	sercel = {}
	sercel.ref = {}
	local ref = sercel.ref
	ref.layer_a	= aaa.obj.get( "sercel_boxone_layer_a" )
	ref.cam		= aaa.obj.get( "sercel_boxone_cam" )
	ref.model	= aaa.obj.get( "sercel_boxone_model" )
	ref.size		= param.get_ref( ref.model, "size_factor" )
	ref.size_axe	= param.get_ref( ref.model, "size_axe" )
	ref.size_u		= param.get_ref( ref.model, "size_u" )
	ref.size_v		= param.get_ref( ref.model, "size_v" )

--aaa.print( "toto" )
end
local	bdd_src = aaa.obj.get( "sercel_terrain" )


local	pha_scr_u = 0
local	pha_scr_v = 0
local	pha_time_freq = 4
local	pha_space_freq = .4
function get_phase( ru, rv )
	local	cu = ru - pha_scr_u
	local	cv = rv - pha_scr_v
	local t = (1. + math.sin( math.sqrt( cu*cu + cv*cv ) * pha_space_freq - aaa.time.t * pha_time_freq) ) * .5
	return t
end
function get_anti_phase( ru, rv )
	local	cu = ru - pha_scr_u
	local	cv = rv - pha_scr_v
	local t = (1. + math.sin( math.sqrt( cu*cu + cv*cv ) * pha_space_freq - aaa.time.t * pha_time_freq + math.pi*.5 ) ) * .5
	return t
end

function sercel_fn( fn )
	--aaa.obj.update_then_draw( "sercel_boxone_cam" )
	local nb_u = 33
	local nb_v = 33
	local su = 8
	local sv = 8
	local step_u = su / (nb_u)
	local step_v = sv / (nb_v)
	local ou = su * .5 + step_u * .5
	local ov = sv * .5 + step_v * .5

	local i = 0
	for iu = 1,nb_u do
		local ru = iu * step_u - ou
		for iv = 1,nb_v do
			i = i + 1
			local	rv = -(iv * step_v - ov)
			local ph = get_phase( ru, rv )
			--ph = 1
			fn( ph, iu, iv, i, ru, rv, step_u, step_v, 1. )
		end
		i = i + 1
	end
	aaa.layers.skip_rest()
end

function draw_baton_simple( t, iu, iv, i, ru, rv, su, sv, sa )
	gol.push_matrix()
		z = 4
		gol.translate( ru, 0, rv )
		local	ref = sercel.ref
		param.set( ref.size, 1 )
		param.set( ref.size_axe,  t*sa*2 )
		local at = get_anti_phase( ru, rv )
		param.set( ref.size_u, at*su  )
		param.set( ref.size_v, at*sv  )
		aaa.obj.update_then_draw( ref.layer_a )
	gol.pop_matrix()
end

function draw_baton( t, iu, iv, i, ru, rv, su, sv, sa )
	local	x,y,z = aaa.bdd.get_point( bdd_src, i )
	gol.push_matrix()
		z = z*16
		gol.translate( ru, z*.5*t*.3, rv )
		local	ref = sercel.ref
		param.set( ref.size,  t*su )
		param.set( ref.size_axe,  z*sa )
		aaa.obj.update_then_draw( ref.layer_a )
	gol.pop_matrix()
end

function draw_plaque( t, iu, iv, i, ru, rv, su, sv, sa )
	local	x,y,z = aaa.bdd.get_point( bdd_src, i )
	z = -.4-z*16
	aaa.draw_box( t*su, sa*.05, t*sv,	ru, t*z*.5*t*.3, rv )
end

function draw_oscillo( t, iu, iv, i, ru, rv, su, sv, sa )
	gol.push_matrix()
		gol.translate( ru, rv, 0 )
		gol.scale( su*.45, sv*.4 )
		if iu==14 or iv==36 then
			gol.color_green		()
		else
			gol.color_white()
		end
		aaa.draw_rect_line( -1, -1, 1, 1 )
		gol.color_cyan()
		gol.begin_line_strip()
			f =  math.sin( t * 5 ) * .9
			--f = f * f
			local v
			for x=-1,.5,.1 do
				v = math.sin( x*2.5 - aaa.time.t * 40. )
				v = v * v
				v = v * v
				v = 2 * v  -1
				gol.vertex2( x, v*f )
			end
		gol.do_end()
		gol.color_white()
		aaa.draw_circle_axe_z( .5, v*f, 0, .2, 12 )
		if iu%10==0 or	 iv%10==0 then
			aaa.draw_str_maa_xy( iu.." "..iv, 1, 1, .3, .3 )
		end
	gol.pop_matrix()
end

sercel_fn( draw_baton_simple )