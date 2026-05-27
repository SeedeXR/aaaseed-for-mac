local function doit()
	aaa.lua.global.declare( "n" )
	n = n or 0
	n = n + 1
	--	aaa.print( "Hello World "..n.." "..aaa.time.t )

	gol.color_white()
	--gol.begin_points()
	local d = .05
	for u=-4,4,d do
		for v=-4,4,d do
			--gol.vertex( u, math.sin( u*2+aaa.time.t) * math.sin(v*2+aaa.time.t) * (32-u*u-v*v)*.2, v )
			local dist = u*u + v*v
			local y = 16 - dist
			if y > 0 then
				y = y * .025 * ( 1. + math.sin( math.sqrt(dist)*4 - aaa.time.t * 3) ) -- * math.sin(v*2+aaa.time.t)
				--y = y * .1 * math.sin( u*2+aaa.time.t) * math.sin(v*2+aaa.time.t)
				if y<0 then
					gol.color_black()
				else
					gol.color( y )
				end
				--gol.vertex( u, y, v )
				aaa.draw_box(	d, y, d, u, y*.5, v )
			end
		end
	end
	--gol.do_end()

	gol.color_white()

	aaa.draw_str_xy( "Hello World\n"..n.."\n"..aaa.time.t, 0, 3 )
end

local layers = aaa.layers.get_cur()
--aaa.obj.update_then_draw( layers )
aaa.layers.draw_begin( layers )	--	should be match always by a draw_after
	--gol.rotate_y( aaa.time.t * .1 )
	aaa.layers.draw_layer_all( layers )	--draw all layers in layers
aaa.layers.draw_end( layers )
aaa.layers.skip_rest()

--aaa.obj.update_then_draw( "tp1_cam" )
doit()
local ref = aaa.obj.get_by_name( "tp1_model" )
--param.set( "Module/Jean/TpLua1/fx_a.model", "size_factor", math.sin(aaa.time.t) )
--param.set( "tp1_model", "size_factor", math.sin(aaa.time.t) )
param.set( ref, "size_factor", math.sin(aaa.time.t) )
param.set( ref, "size_u", math.sin(aaa.time.t*15.8)*.5+.5 )
param.set( ref, "size_v", math.sin(aaa.time.t*30.2)*.5+.5 )

