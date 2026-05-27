
local cube = aaa.obj.get("bouzeb_cube")
local teapot = aaa.obj.get("bouzeb_teapot")
local t = param.get( aaa.ref.app, "time")
local refcubex = param.get_ref(cube,"center_x")
local refcubey = param.get_ref(cube,"center_y")
local refcubez = param.get_ref(cube,"center_z")
param.set( refcubex, math.sin(t) )
local x = param.get(refcubex)

local refteapotx = param.get_ref(teapot, "center_y")
param.set(refteapotx, x)

local pasZ = 0
if aaa.lua.global.declare_table( "bouze" ) then
	bouze.layers = aaa.layers.get_cur()
	local lays = bouze.layers
	bouze.layer_a = aaa.layers.get_layer( lays, 1 )
	bouze.layer_b = aaa.layers.get_layer( lays, 2 )
	bouze.layer_c = aaa.layers.get_layer( lays, 3 )
	bouze.layer_d = aaa.layers.get_layer( lays, 4 )
end

local toucheAct = param.get( aaa.ref.pref, "keyboard_special_out" )
if not(toucheAct == 0) then
	if(toucheAct == 103) then
		pasZ = pasZ - 1
	elseif(toucheAct == 101) then
		pasZ = pasZ + 1
	end
end

aaa.layers.draw_begin()	--	should be match always by a draw_after
	aaa.obj.update_then_draw( bouze.layer_a )
	for x=-2.3, 2.2,.5 do
		for y= -2.3, 2.3, .5 do
			for z= -3, pasZ,.5 do
			param.set(refcubex, x)
			param.set(refcubey, y)
			param.set(refcubez, z)
			aaa.obj.update_then_draw( bouze.layer_b )
			end
		end
	end

	--aaa.draw_cube( 0, 0, 0, 2)
	--aaa.obj.update_then_draw( bouze.layer_c )
	--aaa.obj.update_then_draw( bouze.layer_d )
aaa.layers.draw_end()
aaa.layers.skip_rest()


