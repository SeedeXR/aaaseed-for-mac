--todo clean (remove as global) or delete
function second_screen_init()
	if second_screen then return end
	second_screen = {}
	second_screen.time_in = {}
	second_screen.bind = 0
	second_screen.ref = {}
	local ref = second_screen.ref
	ref.bind = param.get_ref( "GaBu/Second_screen/Image/fx_a.layer", "bind_2d_offset" )
	ref.active = param.get_ref( "GaBu/Second_screen/Image/fx_a.layer", "active" )
	local obj = aaa.obj.get_from_top_by_class( "multi_screen" )
	ref.screen_nb = param.get_ref( obj, "x_nb" )
end
function second_screen_set( bind )
	second_screen.bind = bind
end
function second_screen_update()
	second_screen_init()
	local ref = second_screen.ref
	param.set( ref.bind, second_screen.bind )
	local nb = param.get( ref.screen_nb )
	param.set( ref.active, nb-1 )
end
--second_screen_update()