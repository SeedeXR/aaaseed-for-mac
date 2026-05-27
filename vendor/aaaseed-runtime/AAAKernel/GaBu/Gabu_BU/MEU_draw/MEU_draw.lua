--
--	ICON
--
--todo MEU:set_icon_color_back()
--hack for the MU
local function make_rect_fitting_sxy( ssx,ssy, rsx,rsy )
	local f = 4	--we need "a source ratio"
	local sratio_x = ssx/ssy
	local rratio_x = rsx*f/rsy
	local dsx,dsy
	if sratio_x > rratio_x then
		dsx=rsx
		dsy=rsx*f/sratio_x
	else
		dsy=rsy
		dsx=dsy*sratio_x/f
	end
	return dsx, dsy
end
function MEU:set_icon_bind( bind )
	self.__icon_bind = bind
end
function MEU:draw_icon_texture( bind, size )
	if MEU.__b_draw_icon_pure then
		return
	end
	--if true then return end
	--self:print( bind )
	if not self:is_proto_and_isolated() then
		--self:print( bind )
		bind = bind or self.__icon_bind or self:get_texture_bind_2d( 1 )
	end

	local lbrt
	local alpha = self:get_mu():get_value() <=0 and .5 or 1
	local l,r = -.5,-.2
	local sx,sy = r-l,1
	if bind then
		if bind >= 0 then
			
			local psx,psy = aaa.img.get_size( bind )
			if psx then
				gol.color_white( alpha )
				sx,sy = make_rect_fitting_sxy( psx,psy, sx,sy )
			else
				gol.color{.5,.25,.25,.5} 
			end

			size = size and (size*.5 ) or .5
			sx,sy = sx*size,sy*size
			local lbrt = { r-sx*2,-sy, r,sy}
			if psx then
				aaa.draw_bind_rect( bind,	lbrt[1],lbrt[2],	lbrt[3],lbrt[4]	)
				--gol.set_texture_dim( 0 )
			else
				aaa.draw_rect( lbrt[1],lbrt[2],	lbrt[3],lbrt[4] )
				gol.color_blue( alpha )
				aaa.draw_line( lbrt[1]+.05,-.5, lbrt[3]-.05,lbrt[4] )	-- first priam contrib to the design	
			end
			return lbrt
		else
			aaa.debug.print_traceback()
			self:print( "in draw_icon_texture() bind is "..bind )
		end	
	else
		--self:print( "in draw_icon_texture() no bind" )
	end
end
function MEU:draw_icon_text( text, fx )
	gol.set_line_width( BU.__draw_text_line_width )
	aaa.draw_str_xy( text, -.48,-.3, .2*(fx or 1),.8 )
end

function MEU:draw_icon_base()	self:draw_icon_texture()	end
function MEU:draw_icon_opencv()	self:draw_icon_text( "CV" )	end
function MEU:draw_icon_camera()
	local L = -.47
	local R = L+.18
	local Y = -.15
	local TO = Y+.25
	aaa.draw_rect_line( L,Y-.25, R,TO )

	gol.draw_line_strip_2d(	R, 			Y - .05,
							R + .05,	Y -.2,
							R + .05,	Y + .2,
							R,			Y + .05
						)
	local RX = .05
	local RY = RX * 4 
	aaa.draw_ellipse_line_axe_z( L+RX*.6,TO+RY*.9,0, RX*2,RY*2 )
	aaa.draw_ellipse_line_axe_z( R-RX*.6,TO+RY*.9,0, RX*2,RY*2 )
end
function MEU:draw_icon()			self:draw_icon_texture()		end
function MEU:get_icon_lbrt()		return -.5,-.5, -.2,.5			end
function MEU:get_icon_ratio_x()
	local l,b, r,t = self:get_icon_lbrt()
	l,b = self:convert_xy_local_to_top( l,b )
	r,t = self:convert_xy_local_to_top( r,t )
	return (r-l) / (t-b)
end

function MEU:draw_icon_phase( phase )
	phase = wrap_01( phase or aaa.time.t )

	local l,b, r,t = self:get_icon_lbrt()
	gol.set_line_width( BU.__draw_text_line_width * 2 )
	gol.color_green()
	if phase < .5 then
		if     phase < .166 then	aaa.draw_line(  r,t, 		r,b )
		elseif phase < .333 then	aaa.draw_line(  r,b, 		(l+r)/2,b )
		else						aaa.draw_line(  (l+r)/2,b,	l,b )
		end
	else
		if     phase < .666 then	aaa.draw_line(  l,t, 		l,b )
		elseif phase < .833 then	aaa.draw_line(  l,t, 		(l+r)/2,t )
		else						aaa.draw_line(  (l+r)/2,t,	r,t )
		end
	end
end
function MEU:draw_icon_frame( frame_index )
	self:draw_icon_phase( ((frame_index or 0)+.5) / 6 )
end
function MEU:draw_icon_mark_ref()
	gol.set_texture_dim(0)
	gol.color_yellow()
	gol.set_line_width(1.5)
	local x,y = -.44, -.25 -- -.435,-.2
	gol.draw_lines_2d(
		-.5,y,	x,y,
		x,y, 	x,-.5,
		x,y,	-.53,-.625 )
end
function MEU:draw_icon_mark_proto()
	gol.set_texture_dim(0)
	gol.color_yellow()
	gol.set_line_width(1.5)
	aaa.draw_str_maa_xy(  'P' , -.5,-.9,	.14, .7 )
end

function MEU:set_icon_color()
	if MEU.__b_draw_icon_pure then
		gol.color_white()
	else
		local mu = self:get_mu()
		if mu then
			mu:set_icon_color()
		end
	end
end
function MEU:set_color_then_draw_icon()
	self:set_icon_color()
	self:draw_icon()
	gol.color_white()
end