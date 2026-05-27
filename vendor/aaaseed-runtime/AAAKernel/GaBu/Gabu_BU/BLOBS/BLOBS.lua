if CLASS.DECLARE( "BLOB" ) then
	BLOB:set_class_status_doc(	CLASS.STATUS.CORE,
								"encapsulate one blob" )
end
function BLOB:copy_to( dst )
	if dst then
		dst.id = self.id
		dst.x,dst.y = self.x,self.y
	else
		--dst.toto = 2	--trig an error
		self:print_error( "try to copy a blob to a nil" )
	end
end

function BLOB:create( src )
	local self = BLOB:create_instance_no_name()	--todo
	if src then	src:copy_to( self )
	else		self:init()
	end
	return self
end

function BLOB:init()
	self.id = 0
	self.x,self.y = 0,0
	self.__b_consumed = nil	--mark that blob is free from interaction
--	blob.dx = .0
--	blob.dy = .0
end



if CLASS.DECLARE( "BLOBS", nil,  {
	blob_aaa_ui_max = 32,
	touch_ui_max = 32,

	blob_nb_max = 512,
	touch_nb_max = 32,

	blob_nb = 0,
			--	not used nb_by_interaction = 4,
	} ) then
	BLOBS:set_class_status_doc(	CLASS.STATUS.CORE,
								"deal with BLOBS, includung readung from the C bdd" )
end

function BLOBS:read_bdd_in_array_xy( bdd )
	local nb = aaa.bdd.get_blob_nb( bdd )
	local t = {}
	local get = aaa.bdd.get_blob_xy
	for id=1,nb do
		local x,y = get( bdd, id )
		t[id] = { x=x, y=y }
	end
	return t
end

function BLOBS:get_blob_nb()		return	self.blob_nb		end
function BLOBS:get_touch_nb()		return	self.touch_nb		end
function BLOBS:get_untouch_nb()		return 	self.untouch_nb		end
function BLOBS:get_touch_id( i )	return	self.touch[i]		end
function BLOBS:get_untouch_id( i )	return	self.untouch[i]		end

function BLOBS:reset_low()
	--self:print( "reset_low()" )
	for i = 1, self.blob_nb_max		do
		self.blob[i]:init()
	end
	for i = 1, self.touch_nb_max	do
		self.touch[i]	= 0
		self.untouch[i]	= 0
	end
	--self.blob_used = {}
end
function BLOBS:reset_min()
	--self:print( "reset_min()" )
	--self.blob_nb_last = self.blob_nb
	self.blob_nb = 0
	self.touch_nb = 0
	self.untouch_nb = 0

	self.bx = .0
	self.by = .0
end
function BLOBS:reset()
	--self:print( "reset()" )
	self:reset_low()
	self:reset_min()
end
function BLOBS:restart()
	self.blob_nb	= 0
	self.by_id		= {}

	self.touch_nb	= 0
	self.touch		= {}

	self.untouch_nb	= 0
	self.untouch	= {}

	self.bx = .0
	self.by = .0
end

function BLOBS:create( name )
	local self = BLOBS:create_instance( name )
	aaa.print_fn()

	self.touch_nb_max = math.min( BLOBS.touch_nb_max, BLOBS.touch_ui_max )


	self.blob		=	{}
	self.touch		=	{}
	self.untouch	=	{}
	self.__transfo	=	{}
	self.blobs_external			=	{}
	self.blobs_external.cur		=	{}
	self.blobs_external.last	=	{}

	for i = 1, self.blob_nb_max do
		self.blob[i] = BLOB:create()
	end
	self:reset()
	--self.blob_used = {}
	self:print( "BLOBS:create() Done" )
	return self
end
function BLOBS:add_used(blob)
--	self:print( "add_used( "..blob.." )" )
--	self.blob_used[blob.id] = blob
end

function BLOBS:assign_bdd_blob( bdd_blob )
	self.ref = {}
	self:print( "BLOBS:assign_bdd_blob( "..bdd_blob.." )" )
	local ref = self.ref

	--todo frame_counter

	ref.obj = bdd_blob

	if aaa.obj.get_class( ref.obj ) == "bdd_blob_tracking" then
		--because touch untouch are still read thru the obj's param
		ref.touch_nb	= param.get_ref( bdd_blob, "touch_event_nb" )
		ref.untouch_nb	= param.get_ref( bdd_blob, "untouch_event_nb" )
		ref.touch = {}
		ref.untouch = {}
		for i = 1, self.touch_nb_max do
			local num_str = string.format( "%02d", i )
			ref.touch[i]	= param.get_ref( ref.obj, "touch_id_"..num_str )
			ref.untouch[i]	= param.get_ref( ref.obj, "untouch_id_"..num_str )
		end
	--[[
	--	used for the old read method
		ref.blob_nb = param.get_ref( bdd_blob, "blob_nb" )
		ref.blob = {}
		for i = 1, self.blob_aaa_ui_max do
			local num_str = string.format( "%02d", i )
			ref.blob[i] = {}
			local b = ref.blob[i]
			local name = "blob_"..num_str
			b.id =  param.get_ref( ref.obj, name.."_id_tracking" )
			b.x =  param.get_ref( ref.obj, name.."_x_filtered" )
			b.y =  param.get_ref( ref.obj, name.."_y_filtered" )

			b.ghost =  param.get_ref( ref.obj, name.."_ghost" )
		end
	--]]
	end
	self:print( "BLOBS:assign_bdd_blob() Done" )
	return self
end

--[[
--	this version was reading from the obj parameter and so limited by their numbers
function BLOBS:read_blob_old()
	local gref = self.ref
	local tmp = param.get( gref.blob_nb )
	if tmp ~= self.blob_nb then
--		self:print( "blob_nb changed to "..tmp )
		self.blob_nb = tmp
	end
	local bx = 0
	local by = 0
	self.bx = bx
	self.by = by
	if self.blob_nb > 0 then
		self.blob_nb = math.min( self.blob_nb, self.blob_nb_max )
		--self:print( "blobs.blob_nb "..self.blob_nb )
		--local frame = param.get( gref.frame_counter )
		local blob
		local ref
		local nb = self.blob_nb
		self:verbose_2( "blob_nb "..nb )
		for i = 1, nb do
			ref = gref.blob[i]
			blob = self.blob[i]
			blob.id		= param.get( ref.id )
			if blob.id == 0 then self:print_error( "got a blob.id of 0" ) end
			local x		= param.get( ref.x )
			local y		= param.get( ref.y )
			blob.x_raw = x
			blob.y_raw = y
			bx = bx + x-.5
			by = by + y-.5
			-- transform x y according to camera
			blob.x = ( x - 0.5 ) * ga.ortho_sx * 2.
			blob.y = ( y - 0.5 ) * ga.ortho_sy * 2.
			--self:print( "x "..blob.x.." y "..blob.Y )
			--blob.frame	= frame
			blob.b_ghost	= param.get( ref.ghost )
		end
		self.bx = (bx / nb) + .5
		eo.send_img( "aaa.set( \"tube_lr\","..self.bx..")" )
		self.by = (by / nb) + .5
		eo.send_img( "aaa.set( \"tube_du\","..self.by..")" )
	end
end
--]]

function BLOBS:__convert_mouse_coor( x,y, sx,sy )
	if ga.cam then	--this is the mouse we need to use the viewport
		--self:print( x.." "..y )
		local osx, osy = ga.cam:get_ortho_max_size()
		--self:print( sx.." "..sy )
		x = (x-.5) * osx / sx + .5
		y = (y-.5) * osy / sy + .5
		--self:print( "     "..x.." "..y )
	end
	return x,y
end
--	done for ui fine stuff
--		hack from read_blob()
--		we should extend for multiscreen
function BLOBS:convert_coor_to_blob( x,y, b_mouse )
	local tr = self.__transfo
	if not tr.dx then
		return
	end
	if ga and ga.cam then
		local l,b, r,t = ga.cam:get_viewport()
		--self:print( l.." "..b.." ------------- "..r.." "..t )
		--	take in account the viewport
		x = (x-l) / (r-l)
		y = (y-b) / (t-b)

		if b_mouse then	--this is the mouse we need to use the viewport
			x,y = self:__convert_mouse_coor( x,y, tr.sx,tr.sy )
		end
	end
	--here we are in [0,1] with origin at top bottom
	if ga.b_screen_rotate then
		x = 1 - x
		y = 1 - y
	end

	-- transform x y
	x = ( x + tr.dx ) * tr.sx
	y = ( y + tr.dy ) * tr.sy

	--self:print( x.." "..y )
	return x,y
end

--	barycenter is computed with same coordantes as the blob
function BLOBS:read_blob( b_mouse, b_mouse_no, dx,dy, sx,sy )
	--self:print( "read_blob "..b_mouse.." "..b_mouse_no )
	local obj	 = self:get_obj()
	local tmp_nb = aaa.bdd.get_point_nb( obj )
	--self:print( "tmp_nb "..tmp_nb )

	--we compute barycenter
	local bx = 0
	local by = 0
	local nb = 0
	if tmp_nb > 0 then
		---[[
		if not sy then	-- if no parameter passed don't transform coordonates
			dx,dy = 0,0
			sx,sy = 1,1
		end
		--]]

		local id, x, y
		--todo more refined in loop
		tmp_nb = math.min( tmp_nb, self.blob_nb_max )
		--local frame = param.get( gref.frame_counter )

		local l,b, r,t = ga.cam:get_viewport()
		--self:print( l.." "..b.." ------------- "..r.." "..t )
		--
		for i = 1, tmp_nb do
			id, x,y = aaa.bdd.get_id_and_point( obj, i )
			--self:print( blob.id )
			--	take in account the viewport
			x = (x-l) / (r-l)
			y = (y-b) / (t-b)

			local b_sel = false
			if id == -1 then
				if b_mouse then
					--self:print( "mouse", x,y )
					b_sel = true
					x,y = self:__convert_mouse_coor( x,y, sx,sy )
				end
			else
				if b_mouse_no then
					b_sel = true
				end
			end

			if b_sel then
				nb = nb + 1
				local blob = self.blob[nb]
				blob.id = id
				--self:print( "read_blob "..id )
				--here we are in [0,1] with origin at top bottom
				--self:print( "blob_id "..blob.id.." x "..x.." y "..y )
				--if blob.id == 0 then self:print_error( "got a blob.id of 0" ) end
				if ga.b_screen_rotate then
					x = 1 - x
					y = 1 - y
				end
				--todo x_raw used for calage should be refined (which transfo it go thru like mouse coor in C
				blob.x_raw = x
				blob.y_raw = y
				-- transform x y
				x = ( x + dx ) * sx
				y = ( y + dy ) * sy
				--self:print( x.." "..y )
				blob.x = x
				blob.y = y
				if blob.id >= 0 then
--hackurg			blob.b_ghost = aaa.bdd.get_ghost( obj, i )
				else
					blob.b_ghost = false	--	added in case but i don't know (maa)
				end
				blob.__b_consumed = nil	--mark that blob is free from interaction
				bx = bx + x
				by = by + y
				--self:print( "blob_id "..blob.id.." x "..blob.x.." y "..blob.y )
			end
		end
		if nb > 0 then
			bx = bx / nb
			by = by / nb
		end
	end
	self.bx = bx
	self.by = by

	if self.verbose >= 2 then self:print( "blob_nb "..nb ) end
	if nb ~= self.blob_nb then
	--	self:print( "blob_nb changed to "..nb.." from "..self.blob_nb )
		self.blob_nb = nb
	end

	--aaa.show( self.blob_nb, "read_blob() nb is" )
	--eo.send_img( "aaa.set( \"tube_lr\","..bx..")" )
	--eo.send_img( "aaa.set( \"tube_du\","..by..")" )
end

function BLOBS:register_blobs_external( blobs )
	self.blobs_external.cur = blobs
end
function BLOBS:add_blobs_external()

	local b		= self.blobs_external.cur
	local bl	= self.blobs_external.last
	local nb_new = math.max( #b, #bl )
	if nb_new == 0 then return end

	local nb	= self.blob_nb
	local t_nb	= self.touch_nb
	local u_nb	= self.untouch_nb
	for i=1,nb_new do
		if b[i] then
			nb = nb + 1
			local blob = self.blob[nb]
			blob.x = b[i].x
			blob.y = b[i].y
			blob.id = b[i].id
			blob.__b_consumed = nil
			if not bl[i] then
				aaa.mess.show( "Add touch "..b[i].x.." "..b[i].y )
				t_nb = t_nb + 1
				self.touch[t_nb] = i
			end
		else
			if bl[i] then
				aaa.mess.show( "Add Untouch " )
				u_nb = u_nb + 1
				self.untouch[u_nb] = i
			end
		end
	end
	self.blob_nb	= nb
	self.touch_nb	= t_nb
	self.untouch_nb	= u_nb
	self.blobs_external.last = b
	self.blobs_external.cur = {}
end

function BLOBS:read_touch_untouch( b_mouse, b_mouse_no )
	local ref = self.ref
	local nb

	--	touch untouch are still read thru the obj's param unlike the blob now
	if ref.touch_nb then
		local tmp = param.get( ref.touch_nb )
		tmp = math.min( tmp, self.touch_nb_max )
		nb = 0
		if tmp > 0 then
			if self.verbose >= 2 then self:print( "touch_nb "..tmp ) end
			for i = 1, tmp do
				local id = param.get( ref.touch[i] )
				if (id ~= -1 and b_mouse_no) or (id == -1 and b_mouse ) then
					--self:print( "touch : "..i.." "..id )
					nb = nb + 1
					self.touch[nb] = id
				end
			end
		end
	else
	--todo	 why still there ?
	--todo		if we keep mouse / mouse_no to deal with
		nb = math.min( self.blob_nb, self.touch_nb_max )
		if nb > 0 then
			if self.verbose >= 2 then self:print( "touch_nb "..nb ) end
			for i = 1, nb do
				self.touch[i] = i
			end
		end
	end
	self.touch_nb = nb

	--self:print( table.show( touch, "touch" ) )
	nb = 0
	if ref.untouch_nb then
		local tmp = param.get( ref.untouch_nb )
		tmp = math.min( tmp, self.touch_nb_max )
		if tmp > 0 then
			if self.verbose >= 2 then self:print( "untouch_nb "..nb ) end
			for i = 1, tmp do
				local id = param.get( ref.untouch[i] )
				if (id ~= -1 and b_mouse_no) or (id == -1 and b_mouse ) then
					--self:print( "untouch : "..i.." "..id )
					nb = nb + 1
					self.untouch[nb] = id
					if self.verbose >= 2 then self:print( "untouch "..i.." id "..self.untouch[i] ) end
				end
			end
		end
	end
	self.untouch_nb = nb
end

function BLOBS:read( b_mouse, b_mouse_no, dx,dy, sx,sy )
	--self:reset_min()
	--	store
	local tr = self.__transfo
	tr.dx = dx
	tr.dy = dy
	tr.sx = sx
	tr.sy = sy
	self:read_blob( b_mouse, b_mouse_no, dx,dy, sx,sy )
	self:read_touch_untouch( b_mouse, b_mouse_no )
	self:add_blobs_external()
end

function BLOBS:force( nb, x,y )
	self:reset_min()
	self.blob_nb = 1
	local blob = self.blob[1]
	blob.x_raw = x
	blob.y_raw = y
	blob.x = x
	blob.y = y
	blob.id = 1
	self.bx = x
	self.by = y
end

-- generic fn to send blob using a fn fonction
function BLOBS:send_with_fn( fn )
	--	touch act like a begin so we even send it when no touch
	local tab
	--if self.touch_nb > 0 then
		tab = { self.touch_nb }
		for i = 1, self.touch_nb do
			table.insert( tab, self.touch[i] )
		end
		fn( "touch", tab )
	--end

	local blob_nb = self:get_blob_nb()
	if blob_nb>0 then
		tab = { blob_nb }
		local blob
		for i = 1,blob_nb do
			blob = self.blob[i]
			--if blob.id~= 0 then
			--	blob.b_ghost
			--end
			table.insert( tab, blob.id )
			table.insert( tab, blob.x_raw )
			table.insert( tab, blob.y_raw )
			table.insert( tab, 1.42 )
			--fn( "blob", { blob.id, blob.x_raw, blob.y_raw, 1.2 } )
		end
		fn( "blob", tab )
	end

	--	touch act like an end so we even send it when no touch
	--if self.untouch_nb > 0 then
		tab = { self.untouch_nb }
		for i = 1, self.untouch_nb do
			table.insert( tab, self.untouch[i] )
		end
		fn( "untouch", tab )
	--end
end

function BLOBS:find_by_id( blob_id )
	return array.find_elt_by_id( self.blob, self:get_blob_nb(), blob_id )
end

function BLOBS:is_reset_pos( nb )
	local a = 0
	local b = 0
	local blob
	for i = 1, self:get_blob_nb() do
		blob = self.blob[i]
		if blob.x < .33 and blob.y < .33 then
			a = a + 1
		elseif blob.x > .66 and blob.y > .66 then
			b = b + 1
		end
	end
	return a==b and a==nb/2
end

--function BLOBS:count_in_rect( x, y, sx, sy )
function BLOBS:count_in_rect( rect )
-- center position (x,y) and size x and y
	if self.verbose >= 3 then aaa.print_method() end
	local blob
	local count = 0
	local sx_2 = rect.sx * .5
	local sy_2 = rect.sy * .5
	for i = 1, self:get_blob_nb() do
		blob = self.blob[i]
		if ( math.abs( blob.x - rect.x ) < sx_2 ) and  ( math.abs(blob.y - rect.y ) < sy_2 ) then
			count = count + 1
		end
	end
	return count
end

-- count number of blob for each element of rect table
function BLOBS:count_rect( rect_table )
-- table must be rect = { x, y, sx, sy, nb }
	if self.verbose >= 2 then aaa.print_method() end
	for i=1, #rect_table do
		rect_table[i].nb = self:count_in_rect( rect_table[i] )
	end
end

-- count number of blob for each element without using the rect_table rectangle imformation
--	here we consider the rectangles the same size and adjacent so we can compute directly the index
-- todo add the vertical version
function BLOBS:count_rect_hori_regular( x_min, y_min, x_max, y_max, tab )
	if self.verbose >= 3 then aaa.print_method() end
	local nb = #tab
	for i=1,nb do
		tab[i].nb = 0
	end

	local factor = 1 / ( x_max - x_min )
	for i = 1, self:get_blob_nb() do
		local blob = self.blob[i]
		if inside( blob.y, y_min, y_max ) and inside( blob.x, x_min, x_max ) then
			local x = ( blob.x - x_min ) * factor
			local i = math.min( math.floor( x*nb ) + 1, nb )
			--self:print( i )
			tab[i].nb = tab[i].nb + 1
		end
	end
end

-- count number of blob for each piece of disk without using the rect_table rectangle imformation
--	here we consider the sect of disk the same size and adjacent so we can compute directly the index
--	todo refine with a circular distance to the center of the disk section at the end
function BLOBS:count_disk_section_regular( cx, cy, r_min, r_max, tab )
	if self.verbose >= 3 then self:print( "count_disk_section_regular()" ) end

	local nb = #tab
	for i=1,nb do
		tab[i].nb = 0
	end

	local r2_min = r_min * r_min
	local r2_max = r_max * r_max
	for i = 1, self:get_blob_nb() do
		local blob = self.blob[i]
		local x = blob.x - cx
		local y = blob.y - cy
		local r2 = math.get_length_squared( x,y )
		--self:print( r2.." "..r2_min.." "..r2_max )
		if inside( r2, r2_min, r2_max ) then --and r2 ~= 0 then
			local angle = math.get_angle_01( x,y )
			--self:print( angle )
			local i = math.min( math.floor( angle*nb ) + 1, nb )
			--self:print( i.."/"..nb )
			tab[i].nb = tab[i].nb + 1
		end
	end
end


-- compute barycenter for points situated within a circle with radius centered at position cx, cy
function BLOBS:barycenter( cx,cy, radius )
	if self.verbose >= 3 then aaa.print_method() end
	local count = 0
	local sum_x = 0
	local sum_y = 0
	local center = { x=cx, y=cy }
	for i = 1, self:get_blob_nb() do
		local blob = self.blob[i]
		if math.dist_v2r( center, blob ) <= radius then
			sum_x = sum_x + blob.x
			sum_y = sum_y + blob.y
			count = count + 1
		end
	end
	-- no point return nil
	if count == 0 then return nil end
	return sum_x / count, sum_y / count
end

-- compute barycenter for points situated within a circle with radius centered at position cx, cy
function BLOBS:barycenter_dist( cx, cy, radius_min, radius_max )
	if self.verbose >= 3 then aaa.print_method() end
	local count = 0
	local sum_dist = .0
	local center = { x=cx, y=cy }
	for i = 1, self:get_blob_nb() do
		local dist = math.dist_v2r( center, self.blob[i] )
		if inside( dist, radius_min, radius_max ) then
			sum_dist = sum_dist + dist
			count = count + 1
		end
	end
	-- no point return nil
	if count == 0 then return nil end
	return sum_dist / count
end

--fx.fy are for uv sizing
function BLOBS.draw_blobs_with_colors( blobs, obj_get_rgb, ox,oy, fx,fy, margin, offset_top )
	if not blobs then
		aaa.print_error( "No blobs" )
		return
	end

	margin = margin or 0

	for id=1,blobs.blob_nb do
		local blob = blobs[id]	-- could be blobs.blob[id] in some cases perhaps
		--table.print( b )
		local x,y = blob.x_raw, blob.y_raw
		local sxh,syh = blob.sx_raw/2+margin, blob.sy_raw/2+margin
		local l,b,r,t = x-sxh, y-syh, x+sxh, y+syh

		if obj_get_rgb then
	 		local r,g,b, a = obj_get_rgb:get_blob_rgba( blob )
	 		gol.color4( r,g,b, a )
		else
			gol.color4( 1,1,1, 1 )
		end

		if false then
			gol.set_texture_dim( 0 )
			aaa.draw_null( x,y, 0, .3 )
			gol.set_texture_dim( 2 )
		end

		--gol.set_mask_color( false, true, false, false )
		--	aaa.draw_bind_rect( 101, x-r, y-r, x+r, y+r )

		gol.set_quad_uv( l/fx+.5, b/fy+.5, r/fx+.5, t/fy+.5 )
				--aaa.draw_bind_rect( bind, x-b.dx, y-b.dy, x+b.dx, y+b.dy )
		--fragment hack
		if offset_top then
			t = t + offset_top
		end
		aaa.draw_rect_uv( ox+l, oy+b, ox+r, oy+t )
		--gol.set_mask_color( true, true, true, true )
	end

	-- gol.set_texture_dim(0)
	-- for i=1,blobs.blob_nb do
	-- 	local b = blobs[i]
	-- 	--table.print( b )
	-- 	local x,y = b.x_raw, b.y_raw
	-- 	local sx,sy = b.sx_raw/2, b.sy_raw/2
	-- 	aaa.draw_rect_line( x-sx,y-sy, x+sx, y+sy )
	-- end
end

function BLOBS.find_closer_blob( blobs, x_search )
	if not blobs then
		aaa.print_error( "No blobs" )
		return
	end

	local found
	local found_dist = 10000000
	local found_second
	local found_dist_second

	for id=1,blobs.blob_nb do
		local blob = blobs[id]
		--table.print( b )
		local x,y = blob.x_raw, blob.y_raw
		--local sxh,syh = blob.sx_raw/2, blob.sy_raw/2
		--local l,b,r,t = x-sxh, y-syh, x+sxh, y+syh

		local dist = math.abs( x-x_search )
		if dist < found_dist then
			found_second = found
			found_dist_second = found_dist
			found = blob
			found_dist = dist
		elseif found_dist_second and dist < found_dist_second then
			found_second = blob
			found_dist_second = dist
		end
	end

	return found, found_second
end
