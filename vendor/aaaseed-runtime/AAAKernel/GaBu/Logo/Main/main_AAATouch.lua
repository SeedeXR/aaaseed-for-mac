-- this was made to have the AAATouch logo appear at start

if aaa.lua.global.declare( "main_AAATouch" ) then
	main_AAATouch = { ref={} }
	main_AAATouch.ref.layer = aaa.obj.get_by_name_symbo( "main_AAATouch" )
	main_AAATouch.ref.active = param.get_ref( main_AAATouch.ref.layer, "active" )
	main_AAATouch.ref.color = aaa.layer.get_color( main_AAATouch.ref.layer )
	main_AAATouch.ref.alpha = param.get_ref( main_AAATouch.ref.color, "global_alpha" )
	main_AAATouch.on = 0
end

local t = aaa.time.t_real
if 5 < t and t < 10 then
	main_AAATouch.on = 1
	param.set( main_AAATouch.ref.alpha, 2.5-t*.5 )
else
	main_AAATouch.on = 0
end
--aaa.print( main_AAATouch.on )
param.set( main_AAATouch.ref.active, main_AAATouch.on )




