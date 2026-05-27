if not aaa.grid then
	aaa.grid = { ref={} }
	local ref = aaa.grid.ref
	ref.obj = aaa.layers.get_cur()
end

function aaa.grid.draw()
	local layers = aaa.grid.ref.obj
	local S = 1/8
	local OX = S*.75
	local OY = S*1
	aaa.layers.draw_begin(layers)
		aaa.layers.draw_layer_all(layers)
		for i =1,4 do aaa.draw_str_xyz(	-i,	-i, 0, 0,  S,S  ) end
		--aaa.draw_str_xyz(	0,	-OX*.5, -OY*.5, 0,  S,S  )
		for i =1,4 do aaa.draw_str_xyz(	i,	i-OX, 0, 0,  S,S  ) end
		for i =1,4 do aaa.draw_str_xyz(	-i,	0, -i, 0,  S,S  ) end
		for i =1,4 do aaa.draw_str_xyz(	i,	0, i-OY, 0,  S,S  ) end
	aaa.layers.draw_end(layers)
end