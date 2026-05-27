
aaa.show_file_begin( "aaa_viewport" )

if not aaa.viewport then
	local pgr = param.get_ref
	local pref = aaa.ref.pref
	aaa.viewport = { render={}, focus={}, info={}, curve={} }
	for key, v in pairs(aaa.viewport) do
		local r = {}
		r.obj = param.get_obj_attached( pref, "viewport_"..key.."->" )
			r.x		= pgr( r.obj, "x" )
			r.y		= pgr( r.obj, "y" )
			r.sx	= pgr( r.obj, "size_x" )
			r.sy	= pgr( r.obj, "size_y" )
		aaa.viewport[key].ref = r
	end
end

function aaa.viewport.get_ref_by_name( name )
	name = name or "render"
	local t = aaa.viewport[name]
	if t==nil then
		aaa.print( "No viewport named "..name )
		return nil
	end
	return t.ref
end

function aaa.viewport.get( name )
	local r = aaa.viewport.get_ref_by_name( name )
	if r==nil then return end
	local pg 	= param.get
	return	pg(r.x), pg(r.y), pg(r.sx), pg(r.sy)
end

function aaa.viewport.get_ratio_x( name )
	local r = aaa.viewport.get_ref_by_name( name )
	if r==nil then return end
	local pg = param.get
	return	pg(r.sx)/pg(r.sy)
end

function aaa.viewport.get_ratio_y( name )
	local r = aaa.viewport.get_ref_by_name( name )
	if r==nil then return end
	local pg = param.get
	return	pg(r.sy)/pg(r.sx)
end


aaa.show_file_end( "aaa_viewport" )
