BU.verbose_pos = 0

--
--	FLAG
--
function BU:set_inertia( b )		self.__b_inertia = b			end
function BU:is_inertia()			return self.__b_inertia			end

function BU:set_rot( b )			self.__b_rot = b				end
function BU:is_rot()				return self.__b_rot				end

function BU:set_scale( b )			self.__b_scale = b				end
function BU:is_scale()				return self.__b_scale			end

function BU:set_grid_x( x )			self.__grid_x = x				end
function BU:get_grid_x()			return self.__grid_x			end

function BU:set_grid_y( y )			self.__grid_y = y				end
function BU:get_grid_y()			return self.__grid_y			end

function BU:set_grid_xy( x,y )		self:set_grid_x( x )
									self:set_grid_y( y )			end

function BU:set_grid_angle( angle )	self.__grid_angle = angle		end
function BU:get_grid_angle()		return self.__grid_angle		end

local fx_verbose = 0
function BU:set_fx_on_click( b, ph, duration )
	if b then
		self.__fx_on_click = { b_on_click=true, ph_start = ph or 1, duration = duration or 1 }
	else
		self.__fx_on_click = nil
	end
	return self
end
function BU:begin_fx( ph, duration )
	if fx_verbose > 0 then self:print_debug( "begin_fx() asked" ) end
	if not (self:is_mobile() and aaa.keyboard.is_alt_only()) then	--todo avoid this condition but doing it at call level
		local fx = self.__fx_on_click
		if not fx then
			fx = { ph_start = ph or 1, duration = duration or 1 }
			self.__fx_on_click = fx
		end
		fx.time_factor = fx.ph_start / fx.duration
		fx.ph = fx.ph_start
		if fx_verbose > 0 then self:print_debug( "begin_fx() begin" ) end
		--self:print( "begin_fx( ph "..ph..", duration "..duration.." )" )	
	end
end
function BU:__update_fx()
	local fx = self.__fx_on_click
	--if fx then
	local ph = fx.ph
	--	if ph then
	--self:print( "ph = "..ph )
	--table.print( t )
	-- we use the min because when done after a locking dialog dt can be big 
	ph = ph - math.min( aaa.time.dt_real, .04 ) * fx.time_factor
	--self:print( "ph after = "..ph )
	if ph <= 0 then
		ph = nil
		if fx_verbose > 0 then self:print_debug( "begin_fx() end" ) end
	end
	fx.ph = ph
end

--
--  LIMIT
--
function BU:set_sxy_min( sx,sy )
	self.__sx_min = sx
	self.__sy_min = sy
end
function BU:get_sxy_min()
	return self.__sx_min, self.__sy_min
end

function BU:set_sxy_max( sx,sy )
	self.__sx_max = sx
	self.__sy_max = sy
end
function BU:get_sxy_max()
	return self.__sx_max, self.__sy_max
end

--
--	ALIGNMENT
--
--2023 July Maa commented implementation of alignment is buggy and incomplete
-- function BU:get_align_x()			return self.__align_x					end
-- function BU:set_align_x( align )		self.__align_x	= string.lower(align)	end
-- function BU:get_align_y()			return self.__align_y					end
-- function BU:set_align_y( align )		self.__align_y	= string.lower(align)	end

--
--	POSITION
--
function BU:__init_pos( rect )
	rect[1] = rect[1] or 0
	rect[2] = rect[2] or 0
	rect[3] = rect[3] or 1
	rect[4] = rect[4] or rect[3]
	rect[5] = rect[5] or 0

	--tododesk is this a hack ?

	self:set_page( bus_cur:get_page() )
	local x_offset = self.__page
	if x_offset~=0 then
		rect[1] = rect[1] + x_offset * bus_cur.__page_sx
	end

	--todo 2023 October removed if we need something like that make it well 
	-- if bus_cur.init_transfo then
	-- 	local v = bus_cur.init_transfo
	-- 	x = (x * v.sx) + v.ox
	-- 	y = (y * v.sy) + v.oy
	-- 	sx = sx and (sx * v.sx) or nil
	-- 	sy = sy and (sy * v.sy) or nil
	-- end

	local positions = { normal = {} } -- store the differents postions used (normal, minim ...) each having orig, reference and cur 
	self.__positions = positions
	positions.used = positions.normal
	positions.name_used = "normal"
	
	self:set_pos_orig( rect )
	self.__position_cur = positions.used.cur
	positions.memory = table.copy_deep( positions.normal.cur )
	--table.print( positions, "positions", 3 )
end

--todo : see if direct angle can be replace by get_angle
function BU:get_angle_direct()	return self.__position_cur.angle						end
function BU:get_angle()			return ga.b_rotate and self.__position_cur.angle or 0.	end

function BU:get_size_draw()
	local cur = self.__position_cur
	return cur.sx, cur.sy, cur.sz
end
function BU:get_size_draw_over()
	local cur = self.__position_cur
	return cur.sx_over, cur.sy_over
end

function BU:get_ratio_x()			return self.__position_cur.ratio_x		end
function BU:get_ratio_y()			return self.__position_cur.ratio_y		end
function BU:get_ratio_xy()			local cur = self.__position_cur 		return cur.ratio_x, cur.ratio_y	end

function BU:get_x()					return self.__position_cur.x			end
function BU:get_y()					return self.__position_cur.y			end
function BU:get_xy()				local cur = self.__position_cur 		return cur.x,cur.y								end

function BU:is_inside_lbrt( rect )	local cur = self.__position_cur			return inside_lbrt( rect, cur.x, cur.y )		end

function BU:get_pos_orig_x()		return self.__positions.used.orig.x		end
function BU:get_pos_orig_y()		return self.__positions.used.orig.y		end
function BU:get_pos_orig_xy()		local ori = self.__positions.used.orig	return ori.x,ori.y					end

function BU:get_sx()				return self.__position_cur.sx			end
function BU:get_sy()				return self.__position_cur.sy			end
function BU:get_sxy()				local cur = self.__position_cur 		return cur.sx,cur.sy				end
function BU:get_xy_sxy()			local cur = self.__position_cur			return cur.x,cur.y, cur.sx,cur.sy	end

function BU:get_lbrt()
	local cur = self.__position_cur
	local x,y, sxh,syh = cur.x,cur.y, cur.sx/2,cur.sy/2
	return { l=x-sxh,b=y-syh, r=x+sxh,t=y+syh }
end



--todobb keep ratio
function BU:__clamp_sxy_min_max( sx,sy )
	sx = sx or self:get_sx()
	sy = sy or self:get_sy()

	--min
	local msx, msy = self:get_sxy_min()
	if sx < msx then
		if BU.verbose_pos > 0 then
			self:print_debug( "in BU:__clamp_sxy_min_max sx "..sx.." is too small, replacing by " .. msx )
		end
		sx = msx
	end
	if sy < msy then
		--aaa.debug.print_traceback()
		if BU.verbose_pos > 0 then
			self:print_debug( "in BU:__clamp_sxy_min_max sy "..sy.." is too small, replacing by " .. msy )
		end
		sy = msy
	end

	--max
	msx, msy = self:get_sxy_max()
	if msx and sx > msx then
		if BU.verbose_pos > 0 then 
			self:print_debug( "in BU:__clamp_sxy_min_max sx is too big, replacing by " .. msx )
		end
		sx = msx
	end
	if msy and sy > msy then
		if BU.verbose_pos > 0 then 
			self:print_debug( "in BU:__clamp_sxy_min_max sy is too big, replacing by " .. msy )
		end
		sy = msy
	end

	return sx, sy
end

function BU:get_lbrt_cur()
	local pos	= self.__position_cur
	local x,y	= pos.x, pos.y
	local sx,sy	= pos.sx*.5, pos.sy*.5
	return { l=x-sx, r=x+sx, b=y-sy, t=y+sy }
end
function BU:get_lb_sxy_cur()
	local pos	= self.__position_cur
	local sx,sy	= pos.sx, pos.sy
	return pos.x-sx/2,pos.y-sy/2, sx,sy
end

function BU:get_pos_orig_sx()		return self.__positions.used.orig.sx	end
function BU:get_pos_orig_sy()		return self.__positions.used.orig.sy	end
function BU:get_pos_orig_sxy()		local pos = self.__positions.used.orig
									return pos.sx, pos.sy	end

function BU:set_angle( a )			self.__position_cur.angle = a			end
--todo constraint in a BUS rect if useful
function BU:set_x( x )				self.__position_cur.x = x				end
function BU:set_y( y )				self.__position_cur.y = y				end
function BU:set_xy( x,y )			if BU.verbose_pos > 0 then aaa.print_fn() end
									local cur = self.__position_cur
									cur.x = x
									cur.y = y
end

--todo check and eventually regroup calls
function BU:set_xy_sxy( x,y, sx,sy  )
	local cur = self.__position_cur
	cur.x = x
	cur.y = y
	self:set_sxy( sx,sy )
	--self:set_xy( x,y )
end
--	now we set ratio_max to a big value to force dependency of the ratio on bind
function BU:set_ratio_max( ratio )	self.__ratio_max = ratio	end
function BU:get_ratio_max()			return self.__ratio_max		end
--	low level direct
function BU:set_sxy_direct( sx,sy )
	--aaa.print_fn()
	--if math.abs( self.cur.sx - sx ) > .1 then
	--	self:print( "sx changed" )
	--end

	local cur =  self.__position_cur
	cur.sx = sx
	cur.sy = sy
	if sy then
		cur.sz = math.min( sx,sy )
		cur.sx_over = 1. / sx
		cur.sy_over = 1. / sy
		cur.ratio_x = cur.sx * cur.sy_over
		cur.ratio_y = cur.sy * cur.sx_over
	else
		self:box_debug( "Solve this sy in nil" ) 
	end
end

function BU:fit_sxy_in( sx,sy, max_sx,max_sy, max_ratio )
	max_ratio = max_ratio or self:get_ratio_max()
	if sx and sx > 0 then
		local rx = sx / sy
		
		if max_ratio then
			if rx > max_ratio then
				rx = max_ratio
			else
				max_ratio = 1 / max_ratio
				if rx < max_ratio then
					rx = max_ratio
				end
			end
		end
		--self:print( "BU:fit_sxy_in() rx is "..rx )
		local r_ref = max_sx/max_sy
		--self:print( "BU:fit_sxy_in() r_ref is "..r_ref )
		if rx >= r_ref then
			max_sy =  max_sx / rx
		else
			max_sx = rx * max_sy
		end
	end
	return max_sx, max_sy
end

function BU:set_fxy_max_for_menu( fx, fy, max_sx )
	self.__menu_fx		= fx
	self.__menu_fy		= fy
	self.__menu_max_sx	= max_sx
end

function BU:set_sxy( sx,sy )
	--self:print( "BU:set_sxy() "..sx.." "..sy )

	--if math.abs( self.cur.sx - sx ) > .1 then
	--	self:print( "sx changed" )
	--end
	--aaa.print_fn()
	
	--aaa.debug.print_traceback()

	local nsx,nsy

	if self:is_mini() then
		nsx,nsy = self:__clamp_sxy_min_max( sx,sy )
	else
	--	self:print( "oyo" )
		local msx,msy = self:get_sxy_min()
	--	self:print( "min sxy ".. msx.." "..msy  )
		local bind = self:get_texture_bind_2d()
		if BU.verbose_pos > 0 then self:print( "bind "..bind ) end
		
		if bind then
			local psx,psy = aaa.img.get_size( bind )
			nsx,nsy = self:fit_sxy_in( psx,psy, sx,sy )
			if psx then
			--todo clamp the max size also
				if psx > psy then
					if nsy < msy then
						nsy = msy
						nsx = nsy * psx / psy
					end
				else
					if nsx < msx then
						nsx = msx
						nsy = nsx * psy / psx
					end
				end
			end
		else
			nsx,nsy = self:__clamp_sxy_min_max( sx,sy )
		end
	end

	if BU.verbose_pos > 0 then 
		if nsx ~= sx or nsy ~= sy then
			self:print( "pos sxy asked "..sx.." "..sy )
			self:print( "  adjusted to "..nsx.." "..nsy )	
		end
	end
	self:set_sxy_direct( nsx,nsy )
	return nsx,nsy
end

--used for DOMINO only, should be removed (Mâa 2025 Jan)
function BU:set_x_all( x )
	local pos = self.__positions.used
	pos.cur.x = x
	pos.orig.x = x
	pos.reference.x = x
end

BU.src_no_verbose = {}
BU.src_no_verbose["BU:set_pos_orig"] = true
function BU:__verbose_pos( fn, src )
	if BU.verbose_pos > 0 then
		if not BU.src_no_verbose[src] then
			self:print( fn.."() call from "..src )
		end
	end
end

function BU:__cpy_pos_to_cur( src )
	local pos = self.__positions.used
	local cur = table.copy_simple_check( src )
	pos.cur = cur
	self.__position_cur = cur
	self:set_sxy_direct( cur.sx,cur.sy )
	return cur
end

function BU:cpy_orig_to_cur( signature )
	self:__verbose_pos( "BU:cpy_orig_to_cur", signature )
	local pos = self.__positions.used
	self:__cpy_pos_to_cur( pos.orig )
end
function BU:cpy_reference_to_cur( signature )
	self:__verbose_pos( "BU:cpy_reference_to_cur", signature )
	local pos = self.__positions.used
	self:__cpy_pos_to_cur( pos.reference )
end
function BU:cpy_orig_to_all( signature )
	self:__verbose_pos( "BU:cpy_orig_to_all", signature )
	local pos = self.__positions.used
	local cur = self:__cpy_pos_to_cur( pos.orig )
	pos.reference = table.copy_simple_check( cur )
end
function BU:cpy_cur_to_reference( signature )
	self:__verbose_pos( "BU:cpy_cur_to_reference", signature )
	local pos = self.__positions.used
	pos.reference = table.copy_simple_check( pos.cur )
end

BU.doc.cpy_cur_to_all = "() copy current position to all (reference and origin)"
function BU:cpy_cur_to_all( signature )
	self:__verbose_pos( "BU:cpy_cur_to_all", signature )
	local pos = self.__positions.used
	pos.reference	= table.copy_simple_check( pos.cur )
	pos.orig		= table.copy_simple_check( pos.cur )
end


function BU:set_pos_orig( rect )
	--this detect error when init are not done correctly instead of computation on string angle later
	-- if angle and type(angle) ~= "number" then
	-- 	error( "angle not a number but a "..type(angle) )
	-- end
	local orig = {}
	
	orig.x		= rect[1]
	orig.y		= rect[2]

	orig.sx		= rect[3] or orig.sx
	orig.sy		= rect[4] or orig.sy
	orig.sx,orig.sy = self:__clamp_sxy_min_max( orig.sx,orig.sy )
	orig.sz		= math.min( orig.sx, orig.sy )

	orig.angle	= rect[5] or ( orig.angle or 0 )
	--table.print( orig, "Orig", 2 )
	self.__positions.used.orig = orig

	self:cpy_orig_to_all( "BU:set_pos_orig" )
end

-- CONVERSIONS
--todo3d
BU.doc.convert_xy_bus_up_to_local = "( x,y ) convert from the bus up -> to the BU"
function BU:convert_xy_bus_up_to_local( x,y )	
	--self:print( "convert_xy_bus_up_to_local( "..x..", "..y.." )" )
	--table.print( self.__positions, "positions", 3 )
	local cur = self.__position_cur

	local bus_up = self:get_bus_up()
	x = x - cur.x - bus_up.__x_offset_exp
	y = y - cur.y - bus_up.__y_offset_exp
	--self:print( x.." "..y )
	x,y = math.rotate_ab_turn( x,y, -self:get_angle() )
	--self:print( x.." "..y )
	local sxo,syo = self:get_size_draw_over()
	x,y = x*sxo,y*syo
	return x,y
end
--todo3d
BU.doc.convert_xy_local_to_bus_up = "( x,y ) convert from the BU -> to the bus up"
function BU:convert_xy_local_to_bus_up( x,y ) 
	local cur = self.__position_cur

	local sx,sy = self:get_size_draw()
	x,y = x*sx, y*sy
	x,y = math.rotate_ab_turn( x,y, self:get_angle() )

	x = x + cur.x
	y = y + cur.y
	
	local bus_up = self:get_bus_up()
	if bus_up then
		x = x + bus_up.__x_offset_exp
		y = y + bus_up.__y_offset_exp
	else
		self:print_debug( "convert_xy_local_to_bus_up() no bus up" )
	end

	return x,y
end
--todo3d
--to do on list ?
BU.doc.convert_xy_local_to_top = "( x,y ) convert from the BU -> to the top bus (UI camera)"
function BU:convert_xy_local_to_top( x,y )
	x,y = self:convert_xy_local_to_bus_up( x,y )

	local bus_up = self:get_bus_up()
	if bus_up then
		x,y = bus_up:do_transfo_inverse( x,y )

		local bu_up = bus_up:get_bu_up()
		if bu_up then
			x,y = bu_up:convert_xy_local_to_top( x,y )
		end
	else
		self:print_error( "convert_xy_local_to_top() no bus up" )
	end
	return x,y
end
--todo3d
--move it to BUS
BU.doc.convert_xy_top_to_bus = "( bus, x,y ) convert from the top bus -> to the bus passed as argument"
function BU:convert_xy_top_to_bus( bus, x,y )
	if bus then
		local bu_up = bus:get_bu_up()
		if bu_up then
			x,y = bu_up:convert_xy_top_to_local( x,y )
			x,y = bus:do_transfo( x,y )
		end
	else
		self:print_error( "BU:convert_xy_top_to_bus() was providded a nil bus" )
	end
	return x,y
end
--todo3d
BU.doc.convert_xy_top_to_bus_up = "( x,y ) convert from the top bus -> the bus up"
function BU:convert_xy_top_to_bus_up( x,y )
	local bus = self:get_bus_up()
	return self:convert_xy_top_to_bus( bus, x,y )
end
--todo3d
BU.doc.convert_xy_top_to_bus_down = "( x,y ) convert from the top bus -> the bus down"
function BU:convert_xy_top_to_bus_down( x,y )
	local bus = self:get_bus_down()
	return self:convert_xy_top_to_bus( bus, x,y )
end
BU.doc.convert_xy_top_to_local = "( x,y ) convert from the top bus -> the BU"
function BU:convert_xy_top_to_local( x,y )
	x,y = self:convert_xy_top_to_bus_up( x,y )
	return self:convert_xy_bus_up_to_local( x,y )
end

function BU:convert_blob_from_top_to_bus_up( blob )
	blob.x_top = blob.x
	blob.y_top = blob.y
	blob.x, blob.y = self:convert_xy_top_to_bus_up( blob.x, blob.y )
end

function BU:set_dist_circle( b )	self.__b_dist_circle = b	end

--todo3d
function BU:is_inside_xy_local( x,y )	--todo could be faster
	--aaa.print_fn()
	--if true then return false end
	--self:print( "inside( "..x..", "..y.." )" )

	--self:print( "converted "..x..", "..y.." )" )
 	--self:print( "is_inside x y : "..x..", "..y )
	if -.5<=x and x<=.5 and -.5<=y and y<=.5 then
		--self:print( "Test 1" )
		if self.__b_dist_circle then
			return x*x + y*y <= .25
		else
			return true
		end
	end
	return false
end

function BU:is_inside( x,y )	--todo could be faster
	--aaa.print_fn()
	--if true then return false end
	x,y = self:convert_xy_bus_up_to_local( x,y )
	return self:is_inside_xy_local( x,y )
end

--
--	CONSTRAINT SIZE
--
function BU:constraint_scale( sx,sy )
	--if true then return sx, sy end
	--self.__scale_max = 8.
	--self:print( "SCALE : Min = "..self.__scale_min.." Max = "..self.__scale_max )
	local pos = self.__positions.used
	local ratio_y = pos.reference.sy / pos.reference.sx	--todo understand if we can have a division by zero

	if sx < self.__scale_min * pos.orig.sx then
		if BU.verbose_pos > 0 then self:print_debug( "size x too small sx "..sx..", orig "..pos.orig.sx ) end
		sx = self.__scale_min * pos.orig.sx
		sy = sx * ratio_y
	end
	if sy < self.__scale_min * pos.orig.sy then
		if BU.verbose_pos > 0 then self:print_debug( "size y too small sy "..sy..", orig "..pos.orig.sy ) end
		sy = self.__scale_min * pos.orig.sy
		sx = sy / ratio_y
	end
	if sx > self.__scale_max * pos.orig.sx then
		if BU.verbose_pos > 0 then self:print( "size x too big" ) end
		sx = self.__scale_max * pos.orig.sx
		sy = sx * ratio_y
	end
	--self:print( sy / pos.orig.sy )
	if sy > self.__scale_max * pos.orig.sy then
		if BU.verbose_pos > 0 then self:print( "size y too big" ) end
		sy = self.__scale_max * pos.orig.sy
		sx = sy / ratio_y
	end
	return sx,sy
end

--
--	CONSTRAINT POS
--
function BU:__constraint_point_inside( x,y )
	local bus = self:get_bus_up()
	x,y = bus:do_transfo_inverse( x,y )
	x = clamp( x, -.5,.5 )
	y = clamp( y, -.5,.5 )
	return bus:do_transfo( x,y )
end
function BU:__constraint_inside_at_edge( x,y, sx,sy )
	if not sx then
		sx,sy = self:get_sxy()
	end
	local sxh,syh = sx*.5,sy*.5
	--we don't use rotation here. this will have to be added
	x,y	= self:__constraint_point_inside( x-sxh, y-syh	)	--	constraint	left	bottom
	x,y	= self:__constraint_point_inside( x+sx,	 y+sy	)	--	then		right	top
	return	x-sxh, y-syh								--	add go back to center
end

local function average_spe( v, new_v )
	if not v then v=0 end
	return v*.67 + new_v*.33
end

function BU:__set_rect_inertia( rect, b_store_speed )
	local x,y, sx,sy, angle = unpack(rect)
	local cur = self.__position_cur

	-- x,y
	local grid
	grid = self.__grid_x
	if grid then
		if sx then sx = math.floor( sx / grid + .5 ) * grid end
		local lsx = sx or cur.sx
		x =	math.floor( (x - lsx * .5) / grid + .5 ) * grid + lsx * .5
	end
	grid = self.__grid_y
	if grid then
		if sy then sy =	math.floor( sy / grid + .5 ) * grid end
		local lsy = sy or cur.sy
		y =	math.floor( (y - lsy * .5) / grid + .5 ) * grid + lsy * .5
	end

	-- angle
	if angle then
		local step = self.__grid_angle
		if step then
			angle =	math.floor( angle / step + .5 )	* step
		end
	else
		angle = 0
	end

	local bus = self:get_bus_up()
	if bus then x,y = x + bus.__x_offset_exp, y + bus.__y_offset_exp end
	if self.fn_set_pos_before then
		x,y, sx,sy, angle = self:fn_set_pos_before( x,y, sx,sy, angle )
	else
		local bu_up = bus and bus:get_bu_up()
		if bu_up then
			--self:print( "sxy "..sx.." "..sy )
			x,y	= bus:constraint_inside( self, x,y, sx,sy )
		else
			x = clamp( x, -ga.half_size_x, ga.half_size_x )
			y = clamp( y, -ga.half_size_y, ga.half_size_y )
		end
	end
	if bus then x,y = x - bus.__x_offset_exp, y - bus.__y_offset_exp end

	--aaa.debug.print_traceback()
	--aaa.print_fn()
	--memorise the speed to have inertia
	--should always pass x,y or (x,y, amd sx,sy,angle)
	local dt = aaa.time.dt_real
	if b_store_speed then
		if dt and dt~=0 then
			local dt_over = 1 / dt
	
			self.__inertia_trs_time_last = aaa.time.t_real

			local pos = self.__positions
			pos.vx	= average_spe( pos.vx, (x - cur.x) * dt_over )
			pos.vy	= average_spe( pos.vy, (y - cur.y) * dt_over )

			--aaa.mess.show( "__set_rect_inertia() ->\nvx = "..pos.vx.."\nvy = "..pos.vy )

			if sx then
				self.__inertia_sca_time_last = aaa.time.t_real
				pos.vsx = average_spe( pos.vsx, (sx - cur.sx) * dt_over )
				pos.vsy = average_spe( pos.vsy, (sy - cur.sy) * dt_over )
				pos.vangle = average_spe( pos.vangle, (angle - cur.angle) * dt_over )
			end
		end
	-- else
	-- 	pos.vx	= average_spe( pos.vx, 0 )
	-- 	pos.vy	= average_spe( pos.vy, 0 )
	-- 	pos.vsx = average_spe( pos.vsx, 0 )
	-- 	pos.vsy = average_spe( pos.vsy, 0 )
	-- 	pos.vangle = average_spe( pos.vangle, 0 )
	end

	--really do the change
	self:set_xy( x,y )
	if sx then
		self:set_sxy( sx,sy )
		self:set_angle( angle )
	end
end

--
--	INERTIA
--
function BU:reset_inertia()
	self.__inertia_trs_time_last = nil
	self.__inertia_sca_time_last = nil
end

function BU:do_inertia_trs()
	--if true then return end
	if self.__inertia_trs_time_last then
		local ft = ga:compute_stop_time_factor( self.__inertia_trs_time_last )
		--self:show( ft, "ft" )
		if ft <= 0 then
			self.__inertia_trs_time_last = nil -- stop early next update_loop
		else
			local bounce = false
			local bus = self:get_bus_up()
			local bu_up = bus:get_bu_up()
			local cur = self.__position_cur
			local pos = self.__positions
			local x,y
			if pos.vx then
				x = cur.x + bus.__x_offset_exp
				x = x + pos.vx * ft
			end
			if pos.vy then
				y = cur.y + bus.__y_offset_exp
				y = y + pos.vy * ft * ga.stop_time_factor_y
			end
			local l,b, r,t
			if bu_up then
				local sx,sy = self:get_sxy()
				--we don't use rotation here. this will have to be added
				l,b = bus:do_transfo_inverse( x-sx*.5, y-sy*.5 )
				r,t = bus:do_transfo_inverse( x+sx*.5, y+sy*.5 )
			end
			if pos.vx then
				--self:print( ga.half_size_x.." "..ga.half_size_y )
				if bu_up then
					if l < -.5 or .5 < r then
						bounce = true
					end
				else
					if ga.half_size_x and outside( x, -ga.half_size_x, ga.half_size_x ) then
						bounce = true
					end
				end
				if bounce then
					if self.verbose >= 1 then self:print( "bounce in x" ) end
					x = x - pos.vx * ft
					pos.vx = -pos.vx * BU.__side_bounce_factor
					--self:print( BU.__side_bounce_factor )
					x = x + pos.vx * ft
				end
				cur.x = x - bus.__x_offset_exp
			end
			bounce = false
			if pos.vy	then
				if bu_up then
					if b < -.5 or .5 < t then
						bounce = true
					end
				else
					--aaa.print( "ga.y "..ga.half_size_y )
					if ga.half_size_y and outside( y, -ga.half_size_y, ga.half_size_y ) then
						bounce = true
					end
				end
				if bounce then
					if self.verbose >= 1 then self:print( "bounce in y" ) end
					y = y - pos.vy * ft
					pos.vy = -pos.vy * BU.__side_bounce_factor
					y = y + pos.vy * ft
				end
				cur.y = y - bus.__y_offset_exp
			end
		end
	end
end

function BU:do_inertia_sca()
	if self.__inertia_sca_time_last then
		local ft = ga:compute_stop_time_factor( self.__inertia_sca_time_last )
		if ft <= 0 then
			self.__inertia_sca_time_last = nil -- stop early next update_loop
		else
			local cur = self.__position_cur
			local pos = self.__positions
			local sx,sy = self:constraint_scale( cur.sx + pos.vsx * ft, cur.sy + pos.vsy * ft )
			self:set_sxy( sx,sy )
			cur.angle = cur.angle + pos.vangle * ft
		end
	end
end

--
--	MOVE
--
function BU:__move_dxy( dx,dy, b_constraint_asked, b_store_speed, force_move )
	if dx==0 and dy==0 then
		return
	end

	--aaa.debug.print_traceback()
	--aaa.print_fn()

	local reference = self.__positions.used.reference
	local n_x,n_y	= reference.x,reference.y
	local n_sx,n_sy	= reference.sx, reference.sy
	local n_angle

	b_constraint_asked = b_constraint_asked or self:is_size_ratio_fix()
	b_constraint_asked = b_constraint_asked and not self:is_mini()
	local ratio = b_constraint_asked and reference.sx / reference.sy or nil
	local ldx, ldy = math.rotate_ab_turn( dx, dy, -self:get_angle() )
	--todo more need to be donne to deal with rotation and scaling
	local cx, cy
	local typ = force_move or self.__ui_interaction_type
	local b_scale_change = false
	if		typ=="scale_left" 	or typ=="scale_left_down"	or typ=="scale_left_up" then
		n_sx = n_sx - ldx
		b_scale_change = true
	elseif	typ=="scale_right"	or typ=="scale_right_down"	or typ=="scale_right_up" then
		n_sx = n_sx + ldx
		b_scale_change = true
	end

	if		typ=="scale_up" 	or typ=="scale_left_up"		or typ=="scale_right_up" then
		n_sy = n_sy + ldy
		b_scale_change = true
	elseif	typ=="scale_down"	or typ=="scale_left_down"	or typ=="scale_right_down" then
		n_sy = n_sy - ldy
		b_scale_change = true
	end

	if b_scale_change and ratio then
		if		typ=="scale_up" 	or typ=="scale_down"	then	n_sx = n_sy * ratio
		elseif	typ=="scale_left"	or typ=="scale_right"	then	n_sy = n_sx / ratio
		else
			if ldy > ldx then	n_sx = n_sy * ratio
			else				n_sy = n_sx / ratio
			end
		end
	end
	--self:print( "BU:__move_dxy() phase 1 xy "..n_x.." "..n_y.." sxy "..n_sx.." "..n_sy )
	if n_sx < 0 then	n_sx = 0
	end
	if n_sy < 0 then	n_sy = 0
	end

	if		typ=="scale_left" 	or typ=="scale_left_down"	or typ=="scale_left_up" then
		cx = reference.sx *.5 - n_sx *.5
	elseif	typ=="scale_right"	or typ=="scale_right_down"	or typ=="scale_right_up" then
		cx = - reference.sx *.5 + n_sx *.5
	elseif	typ=="move" then
		cx = ldx
	else
		cx = 0
	end

	if		typ=="scale_up" 	or typ=="scale_left_up"		or typ=="scale_right_up" then
		cy = - reference.sy *.5 + n_sy *.5
	elseif	typ=="scale_down"	or typ=="scale_left_down"	or typ=="scale_right_down" then
		cy = reference.sy *.5 - n_sy *.5
	elseif	typ=="move" then
		cy = ldy
	else
		cy = 0
	end

	ldx, ldy = math.rotate_ab_turn( cx,cy, self:get_angle() )

	n_x = reference.x + ldx
	n_y = reference.y + ldy

	if self:is_page_trs() then
		--self:print( "page trs update" )
		local d = dx * ga:get_page_trs_factor()
		local bus = self:get_bus_up()
		bus.__x_offset_exp = bus.__x_offset_start + d
		return
	end

	if b_constraint_asked then
		if typ == "move" then
			-- local step = 1/16--todoq shop be define somewhere and fit size constraint too
			-- n_x = n_x - n_sx * .5
			-- n_x = n_x - step * .5
			-- n_x = n_x - math.fmod( n_x, step )
			-- n_x = n_x + n_sx * .5

			-- n_y = n_y - n_sy * .5
			-- n_y = n_y - step * .5
			-- n_y = n_y - math.fmod( n_y, step )
			-- n_y = n_y + n_sy * .5

		else
			if math.abs(dx) > math.abs(dy) then	dy = 0
			else								dx = 0
			end
		end
	end

	n_angle = self.__position_cur.angle

	local rect
	if b_scale_change then
		--self:print( "BU:__move_dxy() xy "..n_x.." "..n_y.." sxy "..n_sx.." "..n_sy )
		rect = {n_x,n_y, n_sx,n_sy, n_angle}
	else
		--self:print( "BU:__move_dxy() xy "..n_x.." "..n_y )
		rect = {n_x,n_y, nil,nil, n_angle}
	end
	self:__set_rect_inertia( rect, b_store_speed )
	
	if ga.__b_inertia and self:is_inertia() then
		self:do_inertia_sca()
	end
end

--
--	Position interpolation
--

function BU:is_transfo_inter()			return self.__positions.inter~=nil		end
function BU:get_transfo_inter()			return self.__positions.inter 			end
function BU:get_transfo_inter_smooth()	return self.__positions.inter_smooth	end
function BU:get_transfo_inter_text()
	local positions = self.__positions
	local inter = positions.inter_smooth
	local direction = positions.inter_text_direction
	if inter then
		if		direction == 0 then	return nil
		elseif	direction == 1 then	return 1 - inter
		else						return inter
		end
	end
end
function BU:compute_transfo_inter_smooth( inter )
	if false then
		inter = (inter - .5) * 2.
		if inter < 0 then
			inter = - math.pow( -inter, .8 )
		else
			inter =  math.pow( inter, .8 )
		end
		inter = (inter * .5) + .5
	else
		inter = (1 - math.cos( inter * math.pi ) ) * .5
	end
	return inter
end

function BU:__set_transfo_inter( inter, delta, delta_text )
	local positions = self.__positions
	positions.inter = inter
	if delta then
		positions.inter_time_factor = delta
		positions.inter_text_direction = delta_text
	end
	if inter then
		inter = self:compute_transfo_inter_smooth( inter )
		positions.inter_smooth = inter
	else
		positions.inter_smooth = nil
	end
end

function BU:__get_transfo_low()
	-- if self:get_name_lowercase() == "preset" then
	-- 	self:print_debug( "__get_transfo_low" )
	-- end
	local positions = self.__positions
	--todo is the right place for this ?
	local inter = positions.inter

	local tx,ty	--translate
	local sx,sy	--scale
	local rz	--rotation

	if inter then
		--aaa.print_inverse( inter )
		local pos_0 = positions.inter_0
		local pos_1 = positions.inter_1
		if pos_0 and pos_1 then

			--aaa.print( inter )
			inter = self:get_transfo_inter_smooth()
			local bus = self:get_bus_up()
			tx = interpolate( pos_0.x, pos_1.x, inter )
			ty = interpolate( pos_0.y, pos_1.y, inter )
			tx = tx + bus.__x_offset_exp	
			ty = ty + bus.__y_offset_exp
			rz = interpolate( pos_0.angle, pos_1.angle, inter )
			sx = interpolate( pos_0.sx, pos_1.sx, inter )
			sy = interpolate( pos_0.sy, pos_1.sy, inter )
			self.__sy_rt = sy	--	for scrub interpolation	

			return 	tx,ty,	sx,sy,	rz
		end
	end
	-- no interpolation
	self.__sy_rt = nil	--	for scrub interpolation

	-- if not self.__position_cur then
	-- 	self:print( "self.pos problem" )
	-- end
	--table.print( self, "BU:__get_transfo_low()", 3 )
	local cur = self.__position_cur
	local bus = self:get_bus_up()

	-- self:print( "bug"..ga.ortho_x_offset.." ".. bus.__x_offset_exp )
	-- table.print( bus, "bus", 1 )
	-- if  bus.__x_offset_exp == nil then
	-- 	aaa.debug.debug()
	-- end
	local x = cur.x + bus.__x_offset_exp
	local y = cur.y + bus.__y_offset_exp

	--2023 July Maa commented implementation of alignment is buggy and incomplete
	-- local align_x = self.__align_x
	-- if align_x == "left" then		x = x + cur.sx * .5
	-- elseif align_x == "right" then	x = x - cur.sx * .5
	-- end

	-- align_y = self.__align_y
	-- if align_y == "bottom" then	y = y + cur.sy * .5
	-- elseif align_y == "top" then	y = y - cur.sy * .5
	-- end

	local sx,sy = cur.sx,cur.sy
	local t = self.__fx_on_click
	if t and t.ph and self:get_name_lowercase() == "preset" then
		table.print( t, "_fx_on_click" ) 
	end
	if t then
		local ph = t.ph
		if ph then
			sx = sx * ( 1 + ph )
			sy = sy * ( 1 + ph )
		end
	end
	return x,y,	sx,sy,	ga.b_rotate and cur.angle or 0
end
function BU:pop_transfo()
	gol.pop_matrix()
--	if self.fn_transfo_after then self:fn_transfo_after() end
end
function BU:push_transfo()
	if GA.b_spy then aaa.spy.push_range( "transfo()", 5 ) end

		if self.fn_transfo_before then self:fn_transfo_before() end	--use for chanel / BBlossom

		local positions = self.__positions
		--todo is the right place for this ?
		local inter = positions.inter
		--inter = false
		if inter then
			--self:print( "update() inter" )
			inter = inter + positions.inter_time_factor * ga.__transfo_inter_time_factor * aaa.time.dt_real
			if inside( inter, 0, 1 ) then
				self:__set_transfo_inter( inter )
			else
				self:__set_transfo_inter( nil )
			end
		end

		local tx,ty,	sx,sy,	rz = self:__get_transfo_low()
	
		if ga:is_3d() then
			local sz = math.min(sx,sy)
			if rz == 0 then	gol.push_translate_scale_3d(			tx,ty,0,		sx,sy,sz )
			else			gol.push_translate_rotate_z_scale_3d(	tx,ty,0,	rz,	sx,sy,sz )
			end
		else
			if rz == 0 then	gol.push_translate_scale_2d(			tx,ty,			sx,sy )
			else			gol.push_translate_rotate_z_scale_2d(	tx,ty,		rz,	sx,sy )
			end
		end

	if GA.b_spy then aaa.spy.pop_range() end
end

--
--	MINI / NORMAL / FULL PAGE
--
function BU:get_window_state()
	return self.__positions.name_used
end
function BU:is_mini()		return self.__positions.name_used == "mini"	end
function BU:is_full_page()	return self.__positions.name_used == "full"	end

BU.doc.__set_window_state = 	"( asked ) asked can be \"mini\" \"normal\" \"full\" \"next\" \"prev\" \"flip_full\" \"flip_mini\"\n"..
								"see set_window_state() call this but store undo redo"

function BU:__set_window_state( asked, b_direct )
	--aaa.debug.print_traceback()
	
	-- hack
	--if self.__b_lock and not b_direct then
	if self.__b_lock then
		return
	end
	
	local used = self:get_window_state()
--	self:print( "BU:__set_window_state( \""..asked.."\" ) from "..used  )
	
	if		asked == "next" then
		if		used=="full" 	then	asked = "mini" 
		elseif	used=="mini"	then	asked = "normal" 
		else							asked = "full"
		end
	elseif	asked == "prev" then
		if		used=="full" 	then	asked = "normal" 
		elseif	used=="mini"	then	asked = "full" 
		else							asked = "mini"
		end
--	elseif	asked == "last"
	elseif	asked == "flip_full" then
		if		used=="full"  	then	asked = "normal" 
		elseif	used=="normal"  then	asked = "full"
		else							asked = "normal"
		end
	elseif	asked == "flip_mini" then
		if		used=="mini" 	then	asked = "normal" 
		else							asked = "mini"
		end
	end

	if used == asked then
		return
	end
	if asked ~= "mini" and asked ~= "normal" and asked ~= "full" then
		self:print_error( "state asked "..asked.." is not handled" )
		return
	end
	local bus
	if asked == "full" then
		bus = bus_cur or self:get_bus_up()
		if not bus then
			self:print_debug( "state asked "..asked.." but no bus" )
			return
		end
	end
		
	local positions = self.__positions

	if used == "full" then
		--if not b_direct then
		positions.normal.cur = table.copy_simple( positions.memory )
		positions.memory = nil
	end

	local pos_src = table.copy_simple( self.__position_cur )
	positions.inter_0 = pos_src

	if asked == "mini" then	--mini
		local b_new = not positions.mini
		if b_new then
			positions.mini = {}
		end
		positions.used = positions.mini
		self.__position_cur = positions.used.cur
		positions.name_used = asked
		if b_new then
			self:init_pos_mini()
		end

	elseif asked == "normal" then
		positions.used = positions.normal
		self.__position_cur = positions.used.cur
		positions.name_used = asked

		--self:__set_rect_inertia(	{mem.x - bus.__x_offset_target, mem.y - bus.__y_offset_target,	mem.sx,	mem.sy}	)
		--self.__pos_memory = nil

	elseif asked == "full" then
		positions.memory = table.copy_deep( positions.normal.cur )
		positions.used = positions.normal
		self.__position_cur = positions.used.cur
		positions.name_used = asked

		--table.print( bus, "Bus" )

		--todo why the false case don't consider bus_cur ? (Maa 2023 Jan)
		local ratio_x_cam = bus.__cam:get_ratio_x()

		local sx = bus.__page_sx
		local sy = sx / ratio_x_cam

		--self:print( "BU:__set_window_state() full "..sx.." "..sy )

		local ratio_x = self:get_ratio_x()
		if ratio_x < ratio_x_cam then	-- we adjust on heigth
			sx = sy * ratio_x
		else
			sy = sx / ratio_x
		end

		self:__set_rect_inertia( {-bus.__x_offset_target,-bus.__y_offset_target, sx,sy} )
	end

	self:__set_transfo_inter( 0, 1, asked=="mini" and -1 or (used=="mini" and 1 or 0) )
	positions.inter_1 = table.copy_simple( self.__position_cur )
	self:reset_inertia()

	return asked
end

BU.doc.set_window_state = 	"( asked ) asked can be \"mini\" \"normal\" \"full\" \"next\" \"prev\" \"flip_full\" \"flip_mini\"\n"..
								"__set_window_state() really do the job but this one store undo redo"
							
function BU:set_window_state( asked, b_direct )	--todo b_direct is not usedshould be removed
	local used = self:get_window_state()
	local next = self:__set_window_state( asked, b_direct )
	if used ~= next then
		local undo = self:make_table_to_call_method( self, "__set_window_state", used, b_direct	)
		local redo = self:make_table_to_call_method( self, "__set_window_state", next, b_direct	)
		ga:add_undo_redo( undo,redo )
	end
end

--todo these fn don;t go thru undo redo : should we do better
--function BU:set_mini( b )	self:__set_window_state( b and "mini" or "normal" )	end
--function BU:flip_mini()		self:set_window_state( "flip_mini" )	end

--todo regroup with set_mini : BU shoul have full min and x positions
-- this size argument was used long time ago should be dealt with in anothe way (inactive for now)
-- function BU:flip_full_page( size )
-- 	self:set_window_state( "flip_full" )
-- end

function BU:do_flip_full_page_and_mini( x,y )
--	aaa.print_fn()
--	aaa.debug.print_traceback()
	if not y then
--		self:print( "no y get it " )
		x,y = self:get_click_xy()
	end
	--self:print( "BU:do_flip_full_page_and_mini() y : "..y )

	if y >= BU.__UI_TOP_UP and not self:is_mini() then
		self:set_window_state( "mini" )
	else
		self:set_window_state( "flip_full" )
	end
end

--
--	MINI
--
function BU:set_pos_mini( rect, b_mini )
	local positions = self.__positions
	if not rect then
		-- when info is incomplete we use the current normal position as base
		local pos = positions.normal.cur
		rect = {pos.x,pos.y, pos.sx*.1,pos.sy*.1}
	end

--	sx,sy = self:__clamp_sxy_min_max( sx,sy )
	if b_mini==nil then
		b_mini = self:is_mini()
	end

	local x_offset = self.__page
	if x_offset~=0 then
		local bus = bus_cur and bus_cur or self:get_bus_up()
		x_offset = x_offset * bus.__page_sx
	end

	rect[1] = rect[1] + x_offset
	local win_state = self:get_window_state()
	if win_state~="mini" then
		self:__set_window_state( "mini" )
	end

		self:set_pos_orig( rect )

	if not b_mini and win_state~="mini" then
		self:__set_window_state( win_state )
	end
	return self
end

function BU:__set_size_orig( sx,sy )
	local orig = self.__positions.used.orig
	orig.sx	= sx
	orig.sy	= sy
	orig.sz	= math.min( orig.sx, orig.sy )
	self:cpy_orig_to_all( "BU:__set_size_orig" )
end

function BU:set_size_orig_for_mini( sx,sy )
	local win_state_after = self:get_window_state()
	self:__set_window_state( "mini" )
	self:__set_size_orig( sx,sy )
	self:__set_window_state( win_state_after )
	return self
	--self:set_mini_click_double( true )
end

function BU:init_pos_mini()
	self:set_pos_mini()
end
function BU:init_pos_mini_if_not()
	if not self.__positions.mini then
		self:init_pos_mini()
		--self:set_mini_click_double( true )
	end
end
function BU:has_mini()		return self.__positions.mini~=nil			end
--todo	refine for using other fn ans still do mini
function BU:set_mini_click_double( b )
	if b then
		self:set_method_on_click_double( self, "set_window_state", "flip_mini" )
	else
		self:set_method_on_click_double( nil )
	end
end

--todopos window in full will be trouble
function BU:get_pos_normal()
	local p = self.__positions
	p = self:is_full_page() and p.memory or p.normal.cur
	if p then
		return p.x,p.y, p.sx,p.sy, p.angle
	else
		return nil
	end
end
function BU:get_pos_mini()
	local pos = self.__positions.mini
	if pos then
		pos = pos.cur
		if pos then
			return pos.x,pos.y, pos.sx,pos.sy, pos.angle
		end
	end
end


--todo move it in his own file
--
--	WINDOW
--
--		by default the coordonates system has is origin at bottom left and size is sx*2 by sy*2
function BU:set_window()
	self.__positions.b_window = true
	self:set_method_on_click_double( self, "do_flip_full_page_and_mini" )
end

BU.doc.transform_in_window_no_mini = "( bus_down ) like transform_in_window() but we no minimize option"
function BU:transform_in_window_no_mini( bus_down )
	self:add_bus_down( bus_down )
--	bus_down:add_button( nil, {0,0, .05,.05} )	--	visualize the origin
	--2025 July replaced true by false
	self:set_mobile( false )
	--self:set_text_xy( -.25, .46 )
	self:set_ui_top_size( true )
	self:set_border_line( true )
	local g = .125
	self:set_color_back( { g,g,g, .8 } )
	--bu:set_pos_load_save( false )
	return self
end

BU.doc.transform_in_window = "( bus_down ) set down the BUS and change the BU in window\n"..
										"making it mobile with resize ui too"
function BU:transform_in_window( bus_down )
	if bus_down then
		self:transform_in_window_no_mini( bus_down )
	--	if b_mini==nil or b_mini then
		self:init_pos_mini_if_not()
	--	end
		self:set_window()
	end
	return self
end
function BU:create_window_center( name, rect, bus_down )
	local self = BU:create( name, rect )
	self:set_text_factor( .05 )
	self:transform_in_window( bus_down )
	return self
end


local tab = BU:get_doc()
	--todo3d
	--tab.convert_xy_bus_up_to_local = ""	-- coor ds le BUS -> coor ds le BU
	--todo3d
	--tab.convert_xy_local_to_bus_up = ""	-- coor ds le BU -> coor ds le BUS
	--todo3d
	--tab.convert_xy_local_to_top = ""
	--todo3d
	--tab.convert_xy_top_to_bus = ""
	--todo3d
	--tab.convert_xy_top_to_local = ""
	--tab.convert_blob_from_top_to_bus = ""
	--todo3d
	--	the coor are just from the bus
	--tab.is_inside = ""	--todo could be faster
		--for text button outside
				--todo we compute the rect when we draw
				--	we should centralize in a unique fn
				--	and also cache it
					-- .75 is the beginning of the text at the right of button .5 is the size of the button
					-- Left and Right position
					-- Left and Right position is text not button

					 -- Left position is button and Right position is text
					-- Bottom position
					-- Top position

	--tab.__constraint_point_inside = ""
	--tab.__constraint_inside_at_edge = ""
		--we don't use rotation here. this will have to be added
	--	constraint	left	bottom
	--	then		right	top
	--	add go back to center
--	local function average_spe
	--tab.__set_rect_inertia = ""
		--memorise the speed to have inertia
		--should always pass x,y or (x,y, amd sx,sy,angle)
			--hack there is an asymetric between x and y : this fix it but why ?
				--hack there is an asymetric between x and y : this fix it but why ?
				--	but then it alter the artio of image
		--really do the change

	--
	--	INERTIA
	--
	--tab.reset_inertia = ""
	--tab.do_inertia_trs = ""
					--we don't use rotation here. this will have to be added
	-- stop early next update_loop

	--tab.do_inertia_sca = ""
	 -- stop early next update_loop

	--
	--	MOVE
	--
	--tab.__move_dxy = ""
		--hack dior for uv_edit
		--todo more need to be donne to deal with rotation and scaling
	--
	--	Position interpolation
	--
	-- tab.is_transfo_inter = ""
	-- tab.get_transfo_inter = ""
	-- tab.get_transfo_inter_smooth = ""
	-- tab.__set_transfo_inter = ""
	-- tab.pop_transfo = ""
	-- tab.push_transfo = ""


	--
	--	MINI
	--
	-- tab.set_pos_mini = ""

	--todo	refine for using other fn ans still do mini
	tab.set_mini_click_double = ""

