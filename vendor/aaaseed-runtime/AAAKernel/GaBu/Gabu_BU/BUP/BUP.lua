--
--	BU PLACER IS A BUP
--
if CLASS.DECLARE( "BUP" ) then
	BUP:set_class_status_doc(	CLASS.STATUS.GABU,
								"BUP stand for BU Position this is a helper for a BUS to provide automatic position for BU" )
end

BUP.doc.create = "( name, lbrt ) set the coordonate system we use as ouput, same as set_lbrt()"
function BUP:create( name, lbrt )
	local self = BUP:create_instance( name )
	self:set_col_nb()
	self:set_lbrt( lbrt )
	return self
end
function BUP:set_col_nb( col_nb )
	self.__col_nb = col_nb or 8
	return self
end
function BUP:get_col_nb()	return self.__col_nb	end

BUP.doc.set_lbrt = "( {left,bottom, right,top} )set the coordonnate system we use as ouput"
function BUP:set_lbrt( lbrt )
	--aaa.print_fn()
	local l = lbrt[1] or -4
	local b = lbrt[2] or -2.5
	local r = lbrt[3] or -l
	local t = lbrt[4] or -b
	self.lbrt = { l=l, b=b, r=r, t=t }
	self:set_size()
	return self
end

function BUP:compute_begin_x()	return self.lbrt.l	end

function BUP:compute_begin_y()	return self.lbrt.t - self.sy	end
function BUP:compute_end_y()	return self.lbrt.b	end

function BUP:start()
	self.x = self:compute_begin_x()
	self.y = self:compute_begin_y()
--	self:print( "start "..self.x.." "..self.y )
end

function BUP:set_size( sx,sy, mx,my )
	self.sx		= sx or self.sx or .2
	self.sy		= sy or self.sy or .2
	self.sxh	= self.sx * .5
	self.syh	= self.sy * .5

	self.mx		= mx or	self.sx * .1	-- .004	--from MEU
	self.my		= my or	self.sy * .1	-- .004	-- from MEU
	self.mxh	= self.mx * .5
	self.myh	= self.my * .5

	local r		= self.lbrt
	self.u_nb	= (r.r - r.l) / self.sx
	self.v_nb	= (r.t - r.b) / self.sy

	self:start()
	return self
end
function BUP:set_sxy_col( sx,sy, col_nb )
	self:set_size( sx, sy )
	self:set_col_nb( col_nb )
	return self
end


function BUP:move_x( nb )
	if nb~=0 then
		self.x = self.x + nb * self.sx
	end
end
function BUP:move_right( nb )	self:move_x(   nb or 1  )	end
function BUP:move_left( nb )	self:move_x( -(nb or 1) )	end

function BUP:move_prev_col( nb )
	nb = -(nb or 1) * self.__col_nb	--todo define col size
	self:move_x( nb )	--to deal with x getting outside
	self.y = self:compute_end_y()
end
function BUP:move_next_col( nb )
	nb = (nb or 1) *  self.__col_nb		--todo define col size
	self:move_x( nb )	--to deal with x getting outside
	self.y = self:compute_begin_y() -- - self.dy
end

function BUP:move_next( dy )
	dy = dy or 1
	local y = self.y - self.sy * dy
	if y <= self.lbrt.b then	self:move_next_col()
	else						self.y = y
	end
	--self:print( nb.." next "..self.y )
end
function BUP:move_prev( dy )
	dy = dy or 1
	local y = self.y + self.sy * dy
	--self:print( nb.." prev "..self.y )
	if y >= self.lbrt.t then	self:move_prev_col()
	else						self.y = y
	end
end
--to .125 should be defined
function BUP:move_y( dy )
	dy = dy or 1
	if dy and dy~=0 then
		if dy < 0 then	self:move_prev( -dy )
		else			self:move_next( dy )
		end
	end
end

function BUP:move_xy( dx,dy )
	self:move_x( dx )
	self:move_y( dy )
end

--function BUP:get_x( x )		return x or self.x		end
--function BUP:get_y( y )		return y or self.y		end
--function BUP:get_sx( sx )		return sx or self.sx	end
--function BUP:get_sy( sy )		return sy or self.sy	end

function BUP:set_x(	x )
	self.x = self:compute_begin_x() + x * self.sx
end
function BUP:set_y(	y )
	self.y = self:compute_begin_y() - (y -1) * self.sy
end
function BUP:set_xy( x,y )
	self:set_x( x or 1 )
	self:set_y( y or 1 )
end

function BUP:__compute_rect( rect )
	if rect[1] then	self:set_x(	rect[1] )				end
	if rect[2] then	self:set_y(	rect[2] + rect[4] - 1 ) end
	--x	= self:get_x( x )
	--y	= self:get_y( y )
	rect[1] = self.x + self.sxh*rect[3] + self.mxh
	rect[2] = self.y + self.syh*rect[4] - self.myh
	self:move_y( rect[4] )
--	self:print( x.." "..y )
	rect[3] = rect[3] * self.sx - self.mx
	rect[4] = rect[4] * self.sy - self.my
end

--todo move it to BU
--this call be use for all bu_classes
BUP.doc.__make_rect = "( rect, bu ) is intend to be called at object creation"
function BUP:__make_rect( rect, bu )
	if not bu then
		self:print_error( "BUP:__make_rect( rect, bu ) have no BU" )
	end

	rect = rect and table.copy_simple( rect ) or {}	-- we don't want to change the rect of the caller

	local bup = bus_cur:get_bup()
	local class = bu:get_class()
	if bup then
		if class == SELECTOR then
			rect[3] = rect[3] or (self.__col_nb or 1 )
			rect[4] = rect[4] or 1
		elseif class == SLIDER or class == SLIDER_TWO then
		 	rect[3] = rect[3] or (self.sx or 4)
		 	rect[4] = rect[4] or (self.sy or 1)
		else
			rect[3] = rect[3] or self.sx or 1
			rect[4] = rect[4] or self.sy or rect[3]
		end
		bup:__compute_rect( rect )
	end

	return rect
end
