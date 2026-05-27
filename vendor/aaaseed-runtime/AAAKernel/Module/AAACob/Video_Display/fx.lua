local function do()
	local col = aaa.layer.get_color()
	--aaa.print( aaa.obj.get_name(col) )
	param.set( col, "global_grey", math.sin(aaa.time.t) )
end
