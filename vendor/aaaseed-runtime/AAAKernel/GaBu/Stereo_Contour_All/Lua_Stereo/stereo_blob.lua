if not stereo_blob then

	stereo_blob = {}
	stereo_blob.ref = {}
	local ref = stereo_blob.ref
	ref.blob_l		=	aaa.obj.get_by_name_symbo( "blob_l" )
	ref.blob_r		=	aaa.obj.get_by_name_symbo( "blob_r" )
	ref.blob_out	=	aaa.obj.get_by_name_symbo( "blob_out" )

	--TEXT
	ref.text_layer	=	aaa.obj.get_by_name_symbo( "stereo_text" )
		ref.text_active	=	param.get_ref( ref.text_layer, "active" )
	ref.text_bdd	= aaa.obj.get_by_name_symbo( "stereo_text_bdd" )
		ref.text		=	param.get_ref( ref.text_bdd, "text" )
	param.set( ref.text_active, 0 )

end


function stereo_blob:read_one( blob_ref, side )
	local	nb = aaa.bdd.get_point_nb( blob_ref )
	side.nb = nb
	side.pt = {}
	--aaa.print( "reading "..side.nb.." blobs" )
	for i = 1, nb do
		--collect the ids and the coors
		local id, x, y = aaa.get_id_and_point( blob_ref, i )
		side.pt[i] = { id=id, x=x, y=y }
	end
	table.sort( side.pt, function (a,b) return a.x < b.x end )
end

--[[	home
	--stereo_blob.y_min = 76
	--stereo_blob.y_max = 117.6

	--stereo_blob.y_src = { stereo_blob.y_min, stereo_blob.y_max }

	stereo_blob.y_min = 76
	stereo_blob.y_max = 117.6
	stereo_blob.y_src = { stereo_blob.y_min,
					83.4,
					92.2,
					104,
					stereo_blob.y_max }
--]]
	--arte
	stereo_blob.y_min = 45.4
	stereo_blob.y_max = 76.1
	stereo_blob.y_src = {	 stereo_blob.y_min,
					51,
					57.6,
					66.1,
					stereo_blob.y_max }
		stereo_blob.x_min = -.45
		stereo_blob.x_max = .58

--[[
	stereo_blob.y_min = 35.5
	stereo_blob.y_max = 61

stereo_blob.y_src = { stereo_blob.y_min, stereo_blob.y_max }

	stereo_blob.y_min = 58.5
	stereo_blob.y_max = 90.5
	stereo_blob.y_src = { stereo_blob.y_min,
					60.8,
					63,
					66.1,
					69.2,
					72.2,
					76.4,
					80.5,
					85.,
					stereo_blob.y_max }
--]]
--[[
	stereo_blob.y_min = 36.8
	stereo_blob.y_max = 56.6
	stereo_blob.y_src = { stereo_blob.y_min,
					37.5,
					40,
					41.5,
					43.2,
					45.1,
					49.5,
					51.3,
					53.9,
					stereo_blob.y_max }
--]]


function stereo_blob:compute_y_raw_to_screen( y )
	local	t = self.y_src
	local	nb = #t - 1

	y = math.max( y, self.y_min )
	y = math.min( y, self.y_max )

	local ind
	for i = 1, nb do
		if t[i] <= y and y <= t[i+1] then
			local inter = ( y - t[i] ) / ( t[i+1] - t[i] )
			y = ( (i-1) + inter ) / nb
			return y
		end
	end
end

function stereo_blob:read_all()

	local left = {}
	self:read_one( self.ref.blob_l, left )
	local right = {}
	self:read_one( self.ref.blob_r, right )

	if left.nb ~= right.nb then
		return
	end

	--aaa.print( "blob nb "..left.nb )
	local x,y
	local point = {}
	aaa.bdd.clear_blob( self.ref.blob_out )
	--if left.nb==1 then aaa.print( left.pt[1].x - right.pt[1].x ) end
	--if left.nb==1 then aaa.print( left.pt[1].x + right.pt[1].x ) end
	for i=1,left.nb do
		y = left.pt[i].x - right.pt[i].x
		--[[	PRINT Y
		param.set( self.ref.text_active, 1 )
		if left.nb==1 then
			aaa.print( "y = "..y*1000. )
			param.set( "GaBu/Contour_All/Lua_Stereo/fx_c.text", "text", y*1000. )
		end
		--]]
		y = self:compute_y_raw_to_screen( y*1000. ) -- + x *.1
		--y = (y-.5) / 4.5 * 4 + .5--home
		if left.nb==1 then aaa.print( "y = "..y ) end

		x = left.pt[i].x + right.pt[i].x
		--if left.nb==1 then aaa.print( "x = "..x ) end
		---[[	PRINT X
		param.set( self.ref.text_active, 1 )
		if left.nb==1 then
			aaa.print( "x = "..x*1000. )
			param.set( "GaBu/Contour_All/Lua_Stereo/fx_c.text", "text", x*1000. )
		end
		--]]
		x = (x - self.x_min) / (self.x_max - self.x_min)
		x = x - .5
		x = x - (x-.01) * y * .4	--	this the trapeze
		--x =  x - x * y * .3

		y = y - .5

		x,y = math.rotate_ab_turn( x, y, 0.01 )
--		y = y*1.2 -.2
--		x = x*1.2 + .4
		y = y+.5
		x = x+.5
		--x = x + 2./16.

--		y = (y) * 4.5
--		x = x * 8.
		point[i] = { x, y }
		aaa.bdd.add_blob( self.ref.blob_out, i, x, y, .1, .1 )
	end
end

stereo_blob:read_all()
