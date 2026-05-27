--
--	BOID
--
if CLASS.DECLARE( "BOID" ) then
	BOID.forces = { "Box", "Target", "Repulse", "Flocking", "Steering", "Field", "Repulse_By_Other" }
	local ref = {}
	ref.obj = aaa.obj.get_from_top_by_class( "master_boid" )
	ref.draw_force		= param.get_ref( ref.obj, "draw_force" 		)
	ref.draw_net		= param.get_ref( ref.obj, "draw_net"		)
	ref.box_draw		= param.get_ref( ref.obj, "box_draw" 		)
	ref.box_draw_force	= param.get_ref( ref.obj, "box_draw_force" 	)
	BOID.master = { ref = ref }
	BOID:set_class_status_doc(	CLASS.STATUS.CORE,
								"helper to access to the C bdd_boid object. https://en.wikipedia.org/wiki/Boids" )
end	

local set_real		= param.set_real
local set_real_3	= param.set_real_3
local set_real_4	= param.set_real_4
local set_real_5	= param.set_real_5
local set_real_6	= param.set_real_6

function BOID:create( name, obj )
	local self = BOID:create_instance_with_obj( name, obj )
	self:build_all_ref()
	return self
end

function BOID:trig_restart()	set_real( self.ref.restart_trig, 1 )			end

function BOID:get_ref()
	return self.ref
end

function BOID:build_all_ref( )
	local ref = self.ref
	local obj = ref.obj
	local pgr			= param.get_ref
	local pgr_no_err	= param.get_ref_no_error

	local function make_ref( 	tab,	str, str_def )		tab[ str_def and str_def or str ]	=	pgr( obj, str ) end

	local t = {}

	ref.master = BOID.master.ref

	local tab = {
				"universe", "id", "restart_trig",
				"nb_allocated", "nb_alive_min", "nb_alive_max", "nb_current",
				"birth_nb_trig", "death_nb_trig",
				"dimension",
				"move",				--nomore "accel_early_limit",
				"draw_force",
				"acceleration_max", "acceleration_max_target_box", "acceleration_max_interaction",
				"speed_min",  "speed_max", "speed_vertical_ratio_max", "viscosity",
				"internal_radius",
				"visibility_use", "visibility_angle",
				"box_border_size",
				"mocap_feed", "deform_active",
				"curvature_limit", "curvature_turn_by_sec",
				}
	for _, str in pairs(tab) do make_ref( ref, str ) end

	local tab_xyz = { "birth_origin", "acceleration", "speed", "offset" }
	for _, str in pairs(tab_xyz) do
		local t = {}
		make_ref( t, str.."_x",	"x" )
		make_ref( t, str.."_y",	"y" )
		make_ref( t, str.."_z",	"z" )
		ref[str] = t
	end

	local tab_size_xyzf = { "birth" }
	for _, str in pairs(tab_size_xyzf) do
		local t = {}
		make_ref( t, str.."_size_x"			,"sx"	)
		make_ref( t, str.."_size_y"			,"sy"	)
		make_ref( t, str.."_size_z"			,"sz"	)
		make_ref( t, str.."_size_factor"	,"sf"	)
		ref[str] = t
	end

	ref.repulse_net_color = {}
	ref.repulse_net_color[1]	=	pgr( obj, "repulse_net_red"		)
	ref.repulse_net_color[2]	=	pgr( obj, "repulse_net_green"	)
	ref.repulse_net_color[3]	=	pgr( obj, "repulse_net_blue"	)
	ref.repulse_net_color[4]	=	pgr( obj, "repulse_net_alpha"	)

	ref.repulse_net_color_define = pgr( obj, "repulse_net_color_define"	)

	for _,force in PAIRS( BOID.forces ) do
		local t = {}
		ref[string.lower(force)] = t
		local function make_ref_force( src, dst )
			local ref = pgr_no_err( obj, force.."_"..src )
			--self:print( force.."_"..src.." "..ref)
			t[dst and dst or src] = ref
		end
		make_ref_force( "x"							)
		make_ref_force( "y"							)
		make_ref_force( "z"							)
		make_ref_force( "radius_external"			)
		make_ref_force( "radius_internal"			)
		make_ref_force( "normal"					)
		make_ref_force( "size_x"					,"sx" )
		make_ref_force( "size_y"					,"sy" )
		make_ref_force( "size_z"					,"sz" )
		make_ref_force( "size_factor"				,"sf" )
		make_ref_force( "sphere"					)
		make_ref_force( "border_size"				)
		make_ref_force( "active" 					)
		make_ref_force( "influence"					)
		make_ref_force( "type"						)
		make_ref_force( "type_x"					)
		make_ref_force( "type_y"					)
		make_ref_force( "type_z"					)
		make_ref_force( "exponent"					)
		make_ref_force( "distance"					)
		make_ref_force( "visibility_use"			)
		make_ref_force( "draw"						)
		make_ref_force( "draw_factor"				)
		make_ref_force( "red"						,"r" )
		make_ref_force( "green"						,"g" )
		make_ref_force( "blue"						,"b" )
		make_ref_force( "alpha"						,"a" )
		make_ref_force( "net_draw"					)
		make_ref_force( "net_ease"					)
		make_ref_force( "net_ease_in"				)
		make_ref_force( "net_ease_out" 				)


		make_ref_force( "mvt_scaling" 				)
		-- make_ref_force( "net_ease_out" 				)
		-- make_ref_force( "net_ease_out" 				)
		-- make_ref_force( "net_ease_out" 				)
	end

	local t = {}
	ref.repulse_by_other = t
	make_ref( t, "repulse_by_other",				"type" )
	make_ref( t, "repulse_by_other_influence",	"influence" )
	make_ref( t, "repulse_by_other_distance", 	"distance" )

	return ref
end

function BOID:set_param( param_str, value )
	param.set( self.ref[param_str], value )
end

function BOID:get_force( force, key )				return	param.get(	self.ref[force][key] )	end
function BOID:set_force( force, key, val )			param.set(	self.ref[force][key], val )		end

function BOID:set_curvature( b_limit, turn_by_sec )
	local ref = self.ref
	if b_limit~=nil		then param.set( ref.curvature_limit,		b_limit )		end
	if turn_by_sec~=nil	then param.set( ref.curvature_turn_by_sec,	turn_by_sec )	end
end
--
-- LIVING_BOX
--
function BOID:set_repulse_net_color_define( b )
	param.set( self.ref.repulse_net_color_define, b )
end
function BOID:set_repulse_net_color( r, g, b, a )
	local color = self.ref.repulse_net_color
	param.set( color[1], r )
	param.set( color[2], g )
	param.set( color[3], b )
	param.set( color[4], a )
end


--nonmore function BOID:set_accel_early_limit( b )	param.set( 	self.ref.accel_early_limit, b )		end


function BOID:set_repulse_active( b )				param.set( 	self.ref.repulse.active,		b )	end
function BOID:set_repulse_influence( v )			param.set( 	self.ref.repulse.influence,		v )	end
function BOID:set_repulse_distance( v )				param.set( 	self.ref.repulse.distance,		v )	end

function BOID:set_repulse_by_other_type( v )		param.set( 	self.ref.repulse_by_other.type,			v )	end
function BOID:set_repulse_by_other_influence( v )	param.set( 	self.ref.repulse_by_other.influence,	v )	end
function BOID:set_repulse_by_other_distance( v )	param.set( 	self.ref.repulse_by_other.distance,		v )	end

function BOID:set_flock_active( b )					param.set( 	self.ref.flocking.active,		b )	end
function BOID:set_flock_influence( v )				param.set( 	self.ref.flocking.influence,	v )	end
function BOID:set_flock_distance( v )				param.set( 	self.ref.flocking.distance,		v )	end

function BOID:set_steer_active( b )					param.set( 	self.ref.steering.active,		b )	end
function BOID:set_steer_influence( v )				param.set( 	self.ref.steering.influence,	v )	end
function BOID:set_steer_distance( v )				param.set( 	self.ref.steering.distance,		v )	end

--	BOX
--
function BOID:set_box_type( str )			param.set( 	self.ref.box.type, str )			end
function BOID:set_box_type_xyz( str_x, str_y, str_z )
	self:set_box_type( "No" )
	if str_x then	param.set( 	self.ref.box.type_x, str_x ) end
	if str_y then	param.set( 	self.ref.box.type_y, str_y ) end
	if str_z then	param.set( 	self.ref.box.type_z, str_z ) end
end

function BOID:set_box_influence( f, e )
	local box =  self.ref.box
	local	pset = param.set_real
	if f then	pset( 	box.influence, f )	end
	if e then	pset( 	box.exponent, e )	end
end
function BOID:set_box_border_size( s )		param.set( 	self.ref.box.border_size, s )		end

function BOID:set_box_xyz_save( b_save )
	local	box = self.ref.box
	local	pset = param.set_save
	pset(	box.x,	b_save	)
	pset(	box.y,	b_save	)
	pset(	box.z,	b_save	)
end
function BOID:get_box_xyz()
	local	box = self.ref.box
	local	pget = param.get
	return pget( box.x ), pget(	box.y ), pget( box.z )
end
function BOID:set_box_xyz( x, y, z )
	local	box = self.ref.box
	local	pset = param.set_real
	pset(	box.x,	x	)
	pset(	box.y,	y	)
	pset(	box.z,	z	)
end

function BOID:get_box_x()				return param.get(	self.ref.box.x )			end
function BOID:get_box_y()				return param.get(	self.ref.box.y )			end
function BOID:get_box_z()				return param.get(	self.ref.box.z )			end

function BOID:set_box_x( x )			param.set_real(	self.ref.box.x,	x )					end
function BOID:set_box_y( y )			param.set_real(	self.ref.box.y,	y )					end
function BOID:set_box_z( z )			param.set_real(	self.ref.box.z,	z )					end

function BOID:get_box_x_min()
	local box = self.ref.box
	return param.get( box.x ) - param.get( box.sx ) * .5
end
function BOID:get_box_y_min()
	local box = self.ref.box
	return param.get( box.y ) - param.get( box.sy ) * .5
end
function BOID:get_box_z_min()
	local box = self.ref.box
	return param.get( box.z ) - param.get( box.sz ) * .5
end

function BOID:set_box_x_min( min )
	local box =  self.ref.box
	local max = param.get( box.x ) + param.get( box.sx ) * .5
	param.set_real(	box.x,	(max + min) * .5 )
	param.set_real(	box.sx,	max - min )
end
function BOID:set_box_y_min( min )
	local box =  self.ref.box
	local max = param.get( box.y ) + param.get( box.sy ) * .5
	param.set_real(	box.y,	(max + min) * .5 )
	param.set_real(	box.sy,	max - min )
end
function BOID:set_box_z_min( min )
	local box =  self.ref.box
	local max = param.get( box.z ) + param.get( box.sz ) * .5
	param.set_real(	box.z,	(max + min) * .5 )
	param.set_real(	box.sz,	max - min )
end

function BOID:get_box_x_max()
	local box =  self.ref.box
	return param.get( box.x ) + param.get( box.sx ) * .5
end
function BOID:get_box_y_max()
	local box =  self.ref.box
	return param.get( box.y ) + param.get( box.sy ) * .5
end
function BOID:get_box_z_max()
	local box =  self.ref.box
	return param.get( box.z ) + param.get( box.sz ) * .5
end

function BOID:set_box_x_max( max )
	local box =  self.ref.box
	local min = param.get( box.x ) - param.get( box.sx ) * .5
	param.set_real(	box.x,	(max + min) * .5 )
	param.set_real(	box.sx,	max - min )
end
function BOID:set_box_y_max( max )
	local box =  self.ref.box
	local min = param.get( box.y ) - param.get( box.sy ) * .5
	param.set_real(	box.y,	(max + min) * .5 )
	param.set_real(	box.sy,	max - min )
end
function BOID:set_box_z_max( max )
	local box =  self.ref.box
	local min = param.get( box.z ) - param.get( box.sz ) * .5
	param.set_real(	box.z,	(max + min) * .5 )
	param.set_real(	box.sz,	max - min )
end

function BOID:get_box_x_min_max()
	local box =  self.ref.box
	local c = param.get( box.x )
	local sh = param.get( box.sx ) * .5
	return c-sh, c+sh
end
function BOID:get_box_y_min_max()
	local box =  self.ref.box
	local c = param.get( box.y )
	local sh = param.get( box.sy ) * .5
	return c-sh, c+sh
end
function BOID:get_box_z_min_max()
	local box =  self.ref.box
	local c = param.get( box.z )
	local sh = param.get( box.sz ) * .5
	return c-sh, c+sh
end

function BOID:set_box_x_min_max( min, max )
	local box =  self.ref.box
	min, max = min_max( min, max )
	param.set_real(	box.x,	(max + min) * .5 )
	param.set_real(	box.sx,	max - min )
end
function BOID:set_box_y_min_max( min, max )
	local box =  self.ref.box
	min, max = min_max( min, max )
	param.set_real(	box.y,	(max + min) * .5 )
	param.set_real(	box.sy,	max - min )
end
function BOID:set_box_z_min_max( min, max )
	local box =  self.ref.box
	min, max = min_max( min, max )
	param.set_real(	box.z,	(max + min) * .5 )
	param.set_real(	box.sz,	max - min )
end

function BOID:set_box_min_max_xyz( min_x, max_x, min_y, max_y, min_z, max_z )
	local box =  self.ref.box
	local	pset = param.set_real

	min_x, max_x = min_max( min_x, max_x )
	min_y, max_y = min_max( min_y, max_y )
	min_z, max_z = min_max( min_z, max_z )

	pset(	box.x,	(max_x + min_x) * .5 )
	pset(	box.sx,	max_x - min_x )
	pset(	box.y,	(max_y + min_y) * .5 )
	pset(	box.sy,	max_y - min_y )
	pset(	box.z,	(max_z + min_z) * .5 )
	pset(	box.sz,	max_z - min_z )
	pset(	box.sf,	1	)
end
function BOID:set_birth_pos_size_xyz_save( b_save )
	local ref =  self.ref
	local birth_origin	=  ref.birth_origin
	local birth_size	=  ref.birth
	local	pset = param.set_save

	pset(	birth_origin.x,	b_save )
	pset(	birth_size.sx,	b_save )
	pset(	birth_origin.y,	b_save )
	pset(	birth_size.sy,	b_save )
	pset(	birth_origin.z,	b_save )
	pset(	birth_size.sz,	b_save )
	pset(	birth_size.sf,	b_save)
end
function BOID:set_birth_pos_size_xyz( x,y,z, sx,sy,sz )
	local ref =  self.ref
	local birth_origin	=  ref.birth_origin
	local birth_size	=  ref.birth
	local	pset = param.set_real

	pset(	birth_origin.x,	x )
	pset(	birth_size.sx,	sx )
	pset(	birth_origin.y,	y )
	pset(	birth_size.sy,	sy )
	pset(	birth_origin.z,	z )
	pset(	birth_size.sz,	sz )
	pset(	birth_size.sf,	1	)
end
function BOID:set_birth_min_max_xyz( min_x, max_x, min_y, max_y, min_z, max_z )
	local ref =  self.ref
	local birth_origin	=  ref.birth_origin
	local birth_size	=  ref.birth
	local	pset = param.set_real

	min_x, max_x = min_max( min_x, max_x )
	min_y, max_y = min_max( min_y, max_y )
	min_z, max_z = min_max( min_z, max_z )

	pset(	birth_origin.x,	(max_x + min_x) * .5 )
	pset(	birth_size.sx,	max_x - min_x )
	pset(	birth_origin.y,	(max_y + min_y) * .5 )
	pset(	birth_size.sy,	max_y - min_y )
	pset(	birth_origin.z,	(max_z + min_z) * .5 )
	pset(	birth_size.sz,	max_z - min_z )
	pset(	birth_size.sf,	1	)
end

function BOID:get_box_sx()				return param.get(	self.ref.box.sx )			end
function BOID:get_box_sy()				return param.get(	self.ref.box.sy )			end
function BOID:get_box_sz()				return param.get(	self.ref.box.sz )			end
function BOID:get_box_sf()				return param.get(	self.ref.box.sf )			end

function BOID:set_box_sx( sx )			param.set_real(	self.ref.box.sx,	sx )			end
function BOID:set_box_sy( sy )			param.set_real(	self.ref.box.sy,	sy )			end
function BOID:set_box_sz( sz )			param.set_real(	self.ref.box.sz,	sz )			end
function BOID:set_box_sf( sf )			param.set_real(	self.ref.box.sf,	sf )			end

function BOID:get_box_sxyz()
	local box = self.ref.box
	local pget = param.get
	local f = pget( box.sf )
	return pget(box.sx)*f, pget(box.sy)*f, pget(box.sz)*f
end
function BOID:get_box_sxyzf()
	local box = self.ref.box
	local	pget = param.get
	return pget( box.sx ), pget( box.sy ), pget( box.sz ),  pget( box.sf )
end

function BOID:set_box_sxyz( sx, sy, sz )
	local box = self.ref.box
	local	pset = param.set_real
	pset(	box.sx,	sx	)
	pset(	box.sy,	sy	)
	pset(	box.sz,	sz	)
end

function BOID:set_box_sxyzf( sx, sy, sz, sf )
	local box = self.ref.box
	local	pset = param.set_real
	pset(	box.sx,	sx	)
	pset(	box.sy,	sy	)
	pset(	box.sz,	sz	)
	pset(	box.sf,	sf	)
end

function BOID:get_box_as_sphere()
	local box = self.ref.box
	local pget = param.get
	return pget( box.sphere )
end

function BOID:set_box_as_sphere( b )
	local box = self.ref.box
	param.set(box.sphere, b and 1 or 0)
end

--	TARGET
--
function BOID:set_target_influence( v, b_save )	param.set_and_save( self.ref.target.influence, v, b_save )	end

function BOID:get_target_active()				return param.get(	self.ref.target.active )			end
function BOID:set_target_active( b, b_save )
	--self:print( "set_target_active()"..b..", "..b_save )
	param.set_and_save( self.ref.target.active, b, b_save )
end

function BOID:get_target_xyz()
	local	target = self.ref.target
	local	pget = param.get
	return pget( target.x ), pget(	target.y ), pget( target.z )
end
function BOID:set_target_xyz( x, y, z )
	local	target = self.ref.target
	local	pset = param.set_real
	pset(	target.x,	x	)
	pset(	target.y,	y	)
	pset(	target.z,	z	)
end
function BOID:set_target_xyz_save( b_save )
	local	target = self.ref.target
	local	pset = param.set_save
	pset(	target.x,	b_save	)
	pset(	target.y,	b_save	)
	pset(	target.z,	b_save	)
end

function BOID:get_target_sxyz()
	local	target = self.ref.target
	local	pget = param.get
	return pget( target.sx ), pget(	target.sy ), pget( target.sz )
end

function BOID:get_target_x()			return param.get(	self.ref.target.x )			end
function BOID:get_target_y()			return param.get(	self.ref.target.y )			end
function BOID:get_target_z()			return param.get(	self.ref.target.z )			end

function BOID:set_target_x( x )			param.set_real(	self.ref.target.x,	x )			end
function BOID:set_target_y( y )			param.set_real(	self.ref.target.y,	y )			end
function BOID:set_target_z( z )			param.set_real(	self.ref.target.z,	z )			end

function BOID:set_target_radius( r_ext, r_int, b_save )
	local	target = self.ref.target
	if r_ext then param.set(	target.radius_external,	r_ext ) end
	if r_int then param.set(	target.radius_internal,	r_int ) end
	if b_save ~= nil then
		param.set_save( target.radius_external, b_save )
		param.set_save( target.radius_internal, b_save )
	end
end
function BOID:set_target_radius_internal( r )
	if r then
		local ref = self.ref.target.radius_internal
		param.set_real(	ref, r )
	end
end
function BOID:get_target_radius()				return param.get(	self.ref.target.radius_external )			end
function BOID:get_target_radius_internal()		return param.get(	self.ref.target.radius_internal )			end

function BOID:set_target_normal( n )
	local	target = self.ref.target
	local	pset = param.set_real
	pset(	target.normal,	n	)
end

function BOID:set_target_rotate( b )
	self.ref.target.rotate = b
	-- self:print("a: "..self.ref.target)
	-- local	target = self.ref.target
	-- param.set( 	self.ref.target.rotate,	b )
end

function BOID:get_target_rotate()	return self.ref.target.rotate			end

--
function BOID:get_nb_allocated()
	return 	param.get( self.ref.nb_allocated )
end
function BOID:get_nb_current()
	return 	param.get( self.ref.nb_current )
end
function BOID:set_nb( nb )
	local ref = self.ref
	local pset = param.set
	pset(	ref.nb_allocated,	nb	)
	pset(	ref.nb_alive_min,	nb	)
	pset(	ref.nb_alive_max,	nb	)
	--aaa.obj.update_then_draw( ref.obj )
end
function BOID:set_nb_no_save()
	local ref = self.ref
	local pset = param.set_save
	pset(	ref.nb_allocated,	false	)
	pset(	ref.nb_alive_min,	false	)
	pset(	ref.nb_alive_max,	false	)
	--aaa.obj.update_then_draw( ref.obj )
end

function BOID:set_viscosity( v )		param.set( self.ref.viscosity,			v	)	end

function BOID:set_speed_xyz( x, y, z )
	local speed = self.ref.speed
	local	pset = param.set_real
	pset(	speed.x,	x	)
	pset(	speed.y,	y	)
	pset(	speed.z,	z	)
end

function BOID:set_id( id )				param.set( self.ref.id,					id	)	end
function BOID:set_deform_active( b )	param.set( self.ref.deform_active,		b	)	end
function BOID:set_field_active( b )		param.set( self.ref.field.active,		b	)	end
function BOID:set_field_influence( v )	param.set( self.ref.field.influence,	v	)	end

function BOID:set_speed_min_max( speed_min, speed_max )
	--self:print( "speed_min / speed_max is "..speed_min.."/"..speed_max )
	local ref = self.ref
	local pset = param.set_real
	pset(	ref.speed_min,	speed_min	)
	pset(	ref.speed_max,	speed_max	)
end
function BOID:set_mocap_feed( b )			param.set( 	self.ref.mocap_feed, b )			end

