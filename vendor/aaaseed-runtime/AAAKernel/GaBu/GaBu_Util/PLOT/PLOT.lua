local function ROUND_DELTA( v )
	if v > 0 then
		return 	math.floor( v+.5 )
	end
	return -math.floor( -v+.5 )
end

--
--	PLOT
--
if CLASS.DECLARE( "PLOT" ) then
	PLOT.__len_inked = 0
end

PLOT.__verbose = 0
function PLOT:verbose( th, ... )	if self.__verbose >= th then	self:print(...)	end	end
function PLOT:verbose_0( ... )	self:verbose( 0, ... ) end
function PLOT:verbose_1( ... )	self:verbose( 1, ... ) end
function PLOT:verbose_2( ... )	self:verbose( 2, ... ) end
function PLOT:verbose_3( ... )	self:verbose( 3, ... ) end

function PLOT:set_simul( b )
	self.__b_simul = b
end

function PLOT:create( name )
	local self = PLOT:create_instance( name )
	self:init()
	return self
end

function PLOT:set_pos_xy( x,y )
	self.pen_ask_x = x
	self.pen_ask_y = y

	self.pen_hard_x = x
	self.pen_hard_y = y
	self.pen_soft_x = x
	self.pen_soft_y = y

	self.x_axisdraw = x
	self.y_axisdraw = y
end

function PLOT:reset_pos()
	self:set_pos_xy( 0,0 )
end

function PLOT:compute_dims()
	local mm_by_inch = 25.4
	self.sx_mm = self.sx_inch * mm_by_inch - 2 * self.x_border_mm
	self.sy_mm = self.sy_inch * mm_by_inch - 2 * self.y_border_mm
	-- *2 is a mistery
	self:set_viewport_cano_lbrt( 0, 0, 1, 1 )
	self.sx_step = math.floor( self.sx_mm * self.step_by_mm ) * 2
	self.sy_step = math.floor( self.sy_mm * self.step_by_mm ) * 2
	--local sx_step = 68802
	--local sy_step = 47508

	--self:print( self.sx_step.." by "..self.sy_step.." steps" )

	local SX = 8
	local hx = SX*.5
	local hy = hx * self.sy_step/self.sx_step

	self:set_viewport_aaa_lbrt( -hx, -hy, hx, hy )
	self:print_inverse( self.sx_step, self.sy_step )
	self:set_viewport_step_lbrt( 0, self.sy_step-1, self.sx_step-1, 0 )	--
end

function PLOT:set_border_xy_mm( x,y )
	self.x_border_mm = x
	self.y_border_mm = y
--	self:print( "PLOT:set_border_xy_mm()" )
	self:compute_dims()
end

function PLOT:init()
	self.sx_inch = 16.93
	self.sy_inch = 11.69
	self.step_by_mm = 80
	self.x_border_mm = 0
	self.y_border_mm = 0

	--self:print( "PLOT:init()" )
	self:compute_dims()	-- 1cm on evety side

	self.queue = QUEUE:create( self:get_name() )
--	self.queue_index	= 1
	self.range_begin	= 0
	self.range_end		= 1

	self.__b_pen_down_hard = false
	self:reset_pos()

	self.lines = {}
	self.b_plot = true
end
function PLOT:__compute_conversion( src, dst )
	local s,d = src, dst
	local t
	if s and d then
		t = {}
		t.xf = (d.r - d.l) / (s.r - s.l)
		t.xo = d.l - t.xf * s.l
		t.yf = (d.t - d.b) / (s.t - s.b)
		t.yo = d.b - t.yf * s.b
	else
		t = { xf=1, xo=0, yf=1, yo=0, check=42 }
	end
	return t
end
function PLOT:__compute_coor_conversion()
	local aaa  = self.viewport_aaa
	local step = self.viewport_step
	local cano = self.viewport_cano

	self.__step_to_aaa	= self:__compute_conversion( step, aaa )
	self.__aaa_to_step	= self:__compute_conversion( aaa, step )
	self.__cano_to_aaa	= self:__compute_conversion( cano, aaa )
	self.__aaa_to_cano	= self:__compute_conversion( aaa, cano )
	self.__cano_to_step	= self:__compute_conversion( cano, step )
	self.__step_to_cano	= self:__compute_conversion( step, cano )
end
function PLOT:__build_viewport_rect( l, b, r, t )
	local rect = { l=l, b=b, r=r, t=t }
	rect.sx = math.abs( r-l )
	rect.sy = math.abs( t-b )
	return rect
end
function PLOT:set_viewport_cano_lbrt( l, b, r, t )	self.viewport_cano	= self:__build_viewport_rect( l, b, r, t )	self:__compute_coor_conversion()	end
function PLOT:set_viewport_aaa_lbrt(  l, b, r, t )
	self.viewport_aaa	= self:__build_viewport_rect( l, b, r, t )
	self:__compute_coor_conversion()
	self.viewport_aaa_secu	= self:__build_viewport_rect( l, b*.99, r, t )
end
function PLOT:set_viewport_step_lbrt( l, b, r, t )	self.viewport_step	= self:__build_viewport_rect( l, b, r, t )	self:__compute_coor_conversion()	end

function PLOT:aaa_to_step( x,y )					local t = self.__aaa_to_step		return x*t.xf + t.xo, y*t.yf + t.yo		end
function PLOT:step_to_aaa( x,y )					local t = self.__step_to_aaa		return x*t.xf + t.xo, y*t.yf + t.yo		end
function PLOT:cano_to_aaa( x,y )					local t = self.__cano_to_aaa		return x*t.xf + t.xo, y*t.yf + t.yo		end
function PLOT:aaa_to_cano( x,y )					local t = self.__aaa_to_cano		return x*t.xf + t.xo, y*t.yf + t.yo		end
function PLOT:cano_to_step( x,y )					local t = self.__cano_to_step		return x*t.xf + t.xo, y*t.yf + t.yo		end
function PLOT:step_to_cano( x,y )					local t = self.__step_to_cano		return x*t.xf + t.xo, y*t.yf + t.yo		end

function PLOT:delta_aaa_to_step( dx, dy )			local t = self.__aaa_to_step		return dx*t.xf, dy*t.yf				end

function PLOT:is_inside_viewport( viewport, x,y )
	return inside( x, viewport.l, viewport.r ) and inside( y, viewport.b, viewport.t )
end
function PLOT:is_inside_step( x, y )
	return self:is_inside_viewport( self.viewport_step, x,y )
end
function PLOT:is_inside_aaa( x, y )
	return self:is_inside_viewport( self.viewport_aaa_secu, x,y )
end
function PLOT:set_offset( ox, oy )
	self.__ox_step = ox * 80
	self.__oy_step = oy * 80
	--self:print( self.__ox_step.." "..self.__oy_step )
end
function PLOT:__add_dxy( x, y, dx, dy )
	x = x + dx * 2
	y = y - dy * 2
	return x,y
end

function PLOT:compute_sxy_aaa( sx_in, sy_in )
	local rect = self.viewport_aaa
	local r_x_in = sx_in   / sy_in
	local r_x    = rect.sx / rect.sy
	if r_x_in > r_x then	return rect.sx,          rect.sx / r_x_in
	else					return rect.sy * r_x_in, rect.sy
	end
end
function PLOT:set_printing( b )	self.__b_printing = b		end
function PLOT:is_printing()		return self.__b_printing	end
function PLOT:clear()
--	self.queue_index = 1
	self.queue:erase()
	self:set_begin()
end
function PLOT:set_bdd( bdd )
	local ref = {}

	ref.bdd						= bdd
	ref.received_number_last	= param.get_ref( bdd, "received_number_last"			)
	ref.received_last			= param.get_ref( bdd, "received_last"					)
	ref.received_last_empty_not	= param.get_ref( bdd, "received_last_non_empty"			)
	ref.received				= param.get_ref( bdd, "received"						)
	ref.open_asked				= param.get_ref( bdd, "open_asked"						)
	ref.open_state				= param.get_ref( bdd, "open_state"						)
	ref.port_nb					= param.get_ref( bdd, "com_port_nb"						)
	ref.send_trig				= param.get_ref( bdd, "send_trig"						)
	ref.send					= param.get_ref( bdd, "send_string"						)
	ref.wait_answer				= param.get_ref( bdd, "wait_answer"						)
--	ref.wait_answer_ms			= param.get_ref( bdd, "wait_answer_timeout_millisec"	)
	self.ref = ref
end

--SEND
function PLOT:__update_low_low( str, len )
	local ref = self.ref

	if str then
		self:verbose_2( "AxiDraw sending : "..str.." -----------------> " )
		param.set( ref.send, str )
		--self:print( "__update_low_low : "..str )
		param.set( ref.send_trig, true )
		if len then
			self.__len_inked = self.__len_inked + len
		end
	end

	aaa.obj.update_then_draw( ref.bdd )

	local str_out = param.get( ref.received_last )
	if string.len(str_out) > 0 then
		self:verbose_2( "     Out      : "..str_out )
		return str_out
	end
end

function PLOT:__update_low( str, ms, b_qm, len )
	local str_out
	if b_qm and str then
--		str_out = self:__update_low_low( "QM" )
--		if not str_out then	return -42 end
--		if string.sub( str_out, 1, 2 ) ~= "QM" then return -42 end
--		if string.sub( str_out, 10, 10 ) ~= "0" then return -42 end

		str_out = self:__update_low_low( "QG" )
		if not str_out then	return -42 end
		local bits = string.sub( str_out, 1, 2 )
		bits = tonumber( "0x"..bits )
		if not bits then return -42 end
		--self:print( bits.."  "..(bits % 32)  )
		-- check if button pushed on axidraw
		if (bits % 64) >= 32  then
			self:set_printing( false )
			self:stop_motors()
		end
		--self:print( bits_low )
		if (bits % 2) ~= 0 then return -42 end
	end
	str_out = self:__update_low_low( str, len )
	if str_out then
		self:verbose_2( "received : "..str_out  )
		self:scan_coor( str_out )
	end
	return ms or 0, str_out
end

function PLOT:send_axidraw( str, ms, b_qm, len )
	if self.__b_simul then
		self:print( "send : "..str )
	else
		if string.sub( str, 1, 1 ) == "Q" then
			self:verbose_3( "send_axidraw: "..str.." -----------------> " )
		else
			self:verbose_1( "send_axidraw: "..str.." -----------------> " )
		end
		ms, str = self:__update_low( str, ms, b_qm, len )
		self:verbose_2( "send_axidraw: ------- received ---> "..str )
	end
	return ms, str
end

function PLOT:stop_motors()
	self:send_axidraw( "EM,0,0" )
end
function PLOT:send_wait( ms )
	ms = self:send_axidraw( "XM,"..ms..",0,0", ms, true )
	return ms
end
function PLOT:query_motor()
	local _, str = self:send_axidraw( "QM" )
	if str then
		self:verbose_2( "QM returned ___________________________________ "..str )
	end
end
function PLOT:query_button()
	local _, str = self:send_axidraw( "QB" )
	if str then
		self:verbose_2( "QB returned ___________________________________ "..str.." : "..string.sub( str, 1, 1 ) )
		if string.sub( str, 1, 1 ) == "1" then
			self:set_printing( false )
			self:stop_motors()
			self:print_error( "_____________________________ BUTTON PRESS _______________________________" )
			return true
		end
	end
	return false
end
function PLOT:query_general()
	local _, str = self:send_axidraw( "QG" )
	if str then
		self:verbose_2( "QG returned ___________________________________ "..str )
	end
end

function PLOT:query_step()	self:send_axidraw( "QS" )								end
function PLOT:clear_step()	self:send_axidraw( "CS" )								end
function PLOT:sync_step()
	self:query_step()
	self.__b_sync_step = true
end

function PLOT:compute_ms_from_dxy_step( dx, dy )
	local d = math.max( math.abs(dx), math.abs(dy) )
	--self:print( "d is "..d )
	local ms = d / 12.5	 --  25 cause trouble even 18
	self:verbose_1( self.__b_pen_down_hard.." pen -> ms "..ms )

	ms = ms * (self.__b_pen_down_hard and 6 or 2 ) -- 6 or 2
	ms = math.max( math.ceil(ms)+1, 3 )
	--self:print( self.__b_pen_down_hard.." ms "..ms )
	return ms
end

function PLOT:__send_dxy_step( dx, dy )
	local dx = ROUND_DELTA( dx )
	local dy = ROUND_DELTA( dy )
	if dx==0 and dy==0 then
		return -42
	else
		local ms
		if true then
			local da,db = dx-dy, dy+dx
			ms = self:compute_ms_from_dxy_step( da, db )
			local str = "SM,"..ms..","..da
			if db ~= 0 then str = str.. ","..db end
			ms = self:send_axidraw( str, ms, true, self.__b_pen_down_hard and math.sqrt( dx*dx + dy*dy ) or 0 )
		else
			--	ms = 200
			--local str = "XM,"..ms..","..idx
			--str = str.. ","..-idy
			--self:quey_step()
		end
		if ms >= 0 then
			self.pen_hard_x, self.pen_hard_y = self:__add_dxy( self.pen_hard_x, self.pen_hard_y, dx, dy )
			self:verbose_1( "pen will be at "..self.pen_hard_x.." , "..self.pen_hard_y )
		end
		return ms
	end
end

function PLOT:add_move_xy_step( x, y )
	x = math.floor( x )
	y = math.floor( y )
	if self:is_inside_step( x,y ) then
		self.pen_soft_x, self.pen_soft_y = x,y
		self.queue:push(  { fn="do_go_xy_step", args={ x, y, false } }  )
	else
		self:print_error( "can't go to "..x..", "..y )
	end
end
function PLOT:add_move_dxy_step( dx, dy )
	self:add_move_xy_step( self.pen_soft_x+dx, self.pen_soft_y+dy )
end
function PLOT:add_move_dxy_aaa( dx, dy )
	dx,dy = self:delta_aaa_to_step( dx,dy )
	self:add_move_dxy_step( dx, dy )
end

function PLOT:add_move_xy_aaa( x,y )
	x,y = self:aaa_to_step( x,y )
	self:add_move_xy_step( x,y )
end

function PLOT:scan_coor( str )
	--local x,y = string.match( str, "%d+%d+" )
	local t = {}
	local i = 0
	for word in string.gmatch( str, "%d+" ) do
		i = i + 1
		t[i] = tonumber(word)
		self:verbose_3( "word is "..word )
	end
	if i == 2 then
		local x = t[1]
		local y = t[2]
		if x and y then
			x, y = x-y, x+y
			self:verbose_1( "got xy from axidraw ---------------------------- "..x.." , "..y )
			self.pen_axidraw_x, self.pen_axidraw_y = x,y
			if self.__b_sync_step then
				self.pen_hard_x	= x
				self.pen_hard_y	= y
				self.pen_soft_x	= x
				self.pen_soft_y	= y
				self.pen_ask_x	= x
				self.pen_ask_y	= y
				self.__b_sync_step = false
			end
		else
		end
	end
end


function PLOT:add_axidraw( command, ms )
	self.queue:push( { axidraw=command, ms=(ms or 0 ) } )
end
function PLOT:set_begin()
	self.__cur_id = 1
end
function PLOT:update_queue()
	local dt = 0
	local ms = 0
	local id = self.__cur_id or 1
	local queue = self.queue
	local nb = queue:get_nb() - 1
	repeat
		--todo keep queue
		--local id = self.queue_index
		--self.queue_index = id + 1
		local range = (id-1)/nb
		if range < self.range_begin then
			id = id+1
		elseif self.range_end < range then
			break
		else
			local t = queue:get_front( id )
			if not t then
				self:do_go_xy_step( self.pen_ask_x, self.pen_ask_y, true )
				self.pen_soft_x = self.pen_hard_x
				self.pen_soft_y = self.pen_hard_y
				break
			end

			if t.axidraw then
				self:send_axidraw( t.axidraw.."\r", t.ms )
				ms = t.ms or 0
			elseif t.fn then
				--table.print( t, "t", 2 )
				local args = t.args
				if args then	ms = self[t.fn]( self, args[1], args[2], args[3] )
				else			ms = self[t.fn]( self )
				end
			end

			if ms and ms >= 0 then
				id = id+1
				dt = dt + ms
				self:verbose_3( "dt _____________________________ "..dt )
				if ms >= 50 then
					break
				end
			else
				break
			end
		end
	until( dt > 50 )
	self.__cur_id = id
end

function PLOT:update()
	aaa.show( self.pen_hard_x, "pen_hard_x" )
	aaa.show( self.pen_hard_y, "pen_hard_y" )
	aaa.show( self.pen_soft_x, "pen_soft_x" )
	aaa.show( self.pen_soft_y, "pen_soft_y" )
	--self:query_button()
	if self:is_printing() then
		self:update_queue()
	end
	self:__update_low()
end
function PLOT:draw()
end

-----------------
--PEN
function PLOT:send_pen_up( ms )
	ms = self:send_axidraw( "SP,1,"..ms, ms, true )
	if ms >= 0 then
		self.__b_pen_down_hard = false
		--self:print( "pen up" )
	end
	return ms
end
function PLOT:send_pen_down( ms )
	ms = self:send_axidraw( "SP,0,"..ms, ms, true )
	if ms >= 0 then
		self.__b_pen_down_hard = true
		--self:print( "pen down" )
	end
	return ms
end

function PLOT:do_pen_up_before()
	local ms = 0
	if self.b_plot then
		ms = self:send_wait( 100 )
	else
	end
	return ms
end
function PLOT:do_pen_up()
	local ms = 0
	if self.b_plot then
		ms = self:send_pen_up( 200 )
	else
	end
	if ms >= 0 then
		self.pen_ask_x = self.pen_hard_x
		self.pen_ask_y = self.pen_hard_y
	end
	return ms
end
function PLOT:do_pen_up_after()
	local ms = 0
	if self.b_plot then
		ms = self:send_wait( 400 )
	else
	end
	return ms
end

function PLOT:do_pen_down_before()
	return self:do_go_xy_step( self.pen_ask_x, self.pen_ask_y, true )
end

function PLOT:do_pen_down()
	local ms = 0
	if self.b_plot then
		ms = self:send_pen_down( 150 )
	else
	end
	return ms
end
function PLOT:do_pen_down_after()
	local ms = 0
	if self.b_plot then
		ms = self:send_wait( 200 )
	else
	end
	return ms
end
function PLOT:add_pen_up()
	local queue = self.queue
	queue:push( { fn = "do_pen_up_before"	}	)
	queue:push( { fn = "do_pen_up" 			}	)
	queue:push( { fn = "do_pen_up_after" 	}	)
end
function PLOT:add_pen_down()
	local queue = self.queue
	queue:push( { fn = "do_pen_down_before"	}	)
	queue:push( { fn = "do_pen_down"		}	)
	queue:push( { fn = "do_pen_down_after" 	}	)
end

function PLOT:do_pause()
	self:print( "PAUSE asked : refill ?" )
	self:set_printing( false )
	return 5000
end
function PLOT:insert_pause( where )
	local queue = self.queue
	local nb = queue:get_nb()
	for i = math.floor( nb*where ),nb do
		local t = queue:get_front( i )
		if t and t.fn and t.fn == "do_pen_up_after" then
			queue:insert( i, { fn = "do_pause" 	}	)
			return
		end
	end
end
function PLOT:insert_pause_nb( nb )
	for i=1,nb do self:insert_pause( i/(nb+1) ) end
end

function PLOT:query_pen()
	self:add_axidraw( "QP" )
end
function PLOT:do_set_pen_pos_min_max()
	local experimental_max = .4
	self:send_axidraw( "SC,5,"..math.floor( 1+self.pen_min*65534*experimental_max) )
	self:send_axidraw( "SC,4,"..math.floor( 1+self.pen_max*65534*experimental_max) )
end

function PLOT:get_hard_xy_step()	return self.pen_hard_x, self.pen_hard_y							end
function PLOT:get_xy_step()			return self.pen_soft_x, self.pen_soft_y							end
function PLOT:get_xy_aaa()			return self:step_to_aaa( self.pen_hard_x, self.pen_hard_y )		end

function PLOT:do_go_xy_step( x,y, b_now )
	local ms = 0
	x = x + self.__ox_step
	y = y + self.__oy_step
	x = math.floor( x/2 ) * 2
	y = math.floor( y/2 ) * 2
	if self:is_inside_step( x,y ) then
		if self.__b_skip_next then
			self:do_pen_up()
			self.__b_skip_next = false
			ms = 1000
		elseif x~=self.pen_hard_x or y~=self.pen_hard_y then
			b_now = (b_now==nil) and true or b_now
			if b_now or self.__b_pen_down_hard then
				if self.b_plot then
					local dx =  (x - self.pen_hard_x) * .5
					local dy = -(y - self.pen_hard_y) * .5
					if (math.abs(dx) + math.abs(dy)) > 7 then
						--self:print( dx .. " step " .. dy )
						ms = self:__send_dxy_step( dx, dy )
					end
				else
					self:add_line_point( x, y )
				end
				if ms > 0 then
					--self:print( x .. " pos " .. y )
					self.pen_ask_x = x
					self.pen_ask_y = y
				end
			else
				self.pen_ask_x = x
				self.pen_ask_y = y
			end
		end
	else
		--hack dec 2020 Maa ?

		self:print_error( "skip invalid step position "..x..", "..y )
		self.__b_skip_next = true
	end
	return ms
end
--function PLOT:do_go_xy_step_args( args )
--	return self:do_go_xy_step(  args.x, args.y, args.b_now )
--end
function PLOT:do_go_xy_aaa( x,y, b_now )
	--self:print( x .. " aaa " .. y )
	x,y = self:aaa_to_step( x,y )
	return self:do_go_xy_step( x,y, b_now )
end
function PLOT:do_go_xy_cano( x,y )
	--self:print( x .. " cano " .. y )
	x,y = self:cano_to_step( x,y )
	--self:print( x .. " step " .. y )
	return self:do_go_xy_step( x,y )
end

