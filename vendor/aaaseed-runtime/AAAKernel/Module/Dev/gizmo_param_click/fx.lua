--todo
local ref
ref = aaa.ref.pref
	s_type	= param.get( ref, "move_type" )
	msx		= param.get_ref(	ref, "mouse_focus_start_x" )
	msy		= param.get_ref(	ref, "mouse_focus_start_y" )
	mx		= param.get_ref(	ref, "mouse_focus_x" )
	my		= param.get_ref(	ref, "mouse_focus_y" )

ref = aaa.obj.get_by_name( "Module/Dev/gizmo_param_click/fx_a.null" );
	osx	= param.get_ref( ref, "origin_x" )
	osy	= param.get_ref( ref, "origin_y" )
ref = aaa.obj.get_by_name( "Module/Dev/gizmo_param_click/fx_b.null" );
	ox	= param.get_ref( ref, "origin_x" )
	oy	= param.get_ref( ref, "origin_y" )

ref = aaa.obj.get_by_name( "Module/Dev/gizmo_param_click/fx_c.bdd_line" );
	lsx	= param.get_ref( ref, "origin_x" )
	lsy	= param.get_ref( ref, "origin_y" )
	lx	= param.get_ref( ref, "point_x" )
	ly	= param.get_ref( ref, "point_y" )

--aaa.print( s_type)
r_val = param.get_ref( "Module/Dev/gizmo_param_click/fx.values", "value_01" );
if s_type == 5 then
	param.set( r_val, 1)

	local x = param.get( msx)
	x = x * 3.2 - 1.6
	param.set( osx, x)
	param.set( lsx, x)

	local y = param.get( msy)
	y = y * -8. + 4.
	param.set( osy, y)
	param.set( lsy, y)

	x = param.get( mx)
	x = x * 3.2 - 1.6
	param.set( ox, x)
	param.set( lx, x)

	y = param.get( my)
	y = y * -8. + 4.
	param.set( oy, y)
	param.set( ly, y)
else
	param.set( r_val, 0)
end
