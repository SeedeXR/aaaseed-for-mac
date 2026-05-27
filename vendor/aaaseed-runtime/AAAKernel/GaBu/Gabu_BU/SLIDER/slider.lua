if CLASS.DECLARE( "SLIDER", BUI, {
		__tex_back 			= nil,
		__tex_main 			= nil,
		__tex_cursor 		= nil,
		__tex_meter 		= nil,
		__meter_color		= nil,
		__b_tex_draw		= false,
		__b_dial 			= false,
		__dv 				= .8,
		__du 				= .1,

		__b_dialog_simple	= false,

		__separation_y		= -.25,

		__b_uif_outside		= false
	} ) then
	SLIDER:set_class_status_doc(	CLASS.STATUS.GABU,
									"a type of BUI for one float or int value, or the base class for sliders with more values"  )
	SLIDER.__color_text = {0,0,0,1}
	SLIDER.__text_obj	= TEXT:create( "SLIDER_TEXT_OBJ" )
	SLIDER.__text_obj:set_font_texture( 0 )
	SLIDER.__color_back = BU:get_color_back_named( "SLIDER" )
end

local DY = .5
function SLIDER:set_dial( b )
	self.__b_dial = b
	self:set_range_angle( .8 )
	self.val_last = .5

	local sx, sy	= self:get_sxy()
	local angle		= self:get_angle()

	if sx <= sy then
		self:set_angle( angle - .25 )
	end
	return self
end
function SLIDER:is_dial()		return self.__b_dial 	end

function SLIDER:associate_mu( mu )	self.__mu = mu end

function SLIDER:get_uif_zones()
	local zones = {} --oo.getsuper(BU_MEU).get_uif_zones(self)
	local mu = self.__mu
	if mu then -- this is the slider of a mu
		zones = mu:get_uif_zones_for_slider()
		mu:active_link()
		-- y=1.1 lifts Name/Values/Dump/Doc (and the Min/Max row) so they line up
		-- with the MEU UIF layout where the same buttons were also moved up.
		-- x=-1.225 aligns the left of Name with the left of Pick Ui
		-- (Pick Ui is at ui_x=-0.1 with width 2.25, so its left edge sits at
		--  -0.1 - 2.25/2 = -1.225 in DY units).
		self:add_uif_zones_base( zones, 3, -1.225,1.1, true )	-- true because focus replace Dialoh for mu slider
	else
		zones = {}
--		zones.Slide		= {	angle=1/4,			dangle=1/4,		b_text_no_when_selected=true	}
--		zones.Min		= {	angle=1/2-1/32,		dangle=3/16,	dist=2.5,	dy=-.06,		justify="right" }
--		zones.Max		= {	angle=0+1/32,		dangle=3/16,	dist=2.5,	dy=-.06,		justify="left"}
--		zones.Dialog	= {	angle=3/4,			dangle=1/8,		dist=.7		}
		--	zones.More3	= {	angle=3/4+1/8,		dangle=1/8,		dist=3			},
		-- type unused for now (2023 July)
		--	zones.But1				= { type="button",		angle=3/4,		dist=4,	rect={5,0},				},
		--	zones.TresGrosButtton	= {	type="button",		angle=3/4,		dist=4,	rect={-5,0,8},			},
		self:add_uif_zones_base( zones, 1 )
	end
	--table.print( zones, "zones", 2 )
	return zones
end

--common method with SLIDER_TWO
function SLIDER:__do_mouse_move_slider_base( x,y, id )
	--aaa.print_fn()
	--todo generalize and move up

	--self:print( "do_mouse_move( "..x..", "..y.." )" )
	oo.getsuper(SLIDER).do_mouse_move( self, x,y )

	local u
--	if not self:is_uif_running() then
	if not self:is_uif() then	
--		u = self:__get_uif_u()
--	else
		if self:is_dial() then
			x = self:get_val_from_xy( x,y )
		end

		if not self.slider_x_start then
			aaa.debug.print_traceback()
			self:print_error( "ERROR no self.slider_x_start, skip do_mouse_move( x, y )" )
			return
		end
		local du = x - self.slider_x_start
		if aaa.keyboard.is_ctrl() then
			du = du * .125
			if aaa.keyboard.is_alt() then
				du = du * .125
			end
		end
		local u
		if self:is_meter() then	u = self.slider_u_begin + du
		else					u = self.slider_u_begin + du / ( 1-self.__du )
		end
		self:__set_from_u( u, id )
	end
end


function SLIDER:do_mouse_move( x,y )
	self:__do_mouse_move_slider_base( x,y, 1 )
end

function SLIDER:__do_click_down_slider_base( x,y, id )
--common with SLIDER_TWO
	--self:print( "do_click_down( "..x..", "..y.." )" )
	oo.getsuper(SLIDER).do_click_down( self, x,y )

	--self:print( "y = "..y )
--	if not self:is_uif_running() then
	if not self:is_uif() then
		if not self:is_dial() then
		--	table.print( self, "slider clicked", 1 )
			local val = self:get_value_cano_clamped(id) 
			local du = self.__du
			local h = (1-du) * val + du*.5
			self.slider_x_start = x
			if y < self.__separation_y then
				--we set directly the new value from position instead of from movement
				if self:is_meter() or self.__str_slider_two_draw_step then
					self:set_value_cano( x + .5, id )
				else
					if inside( x+.5, h-du*.5, h+du*.5 ) then
						self:print( "inside button" )
					else
						self:print( "outside button" )
						self:set_value_cano( x / (1-du) + .5, id )
					end
				end
				val = self:get_value_cano_clamped(id)
			end
			self.slider_u_begin = val
		end
	end
--	self:do_mouse_move( x,y )
end

function SLIDER:do_click_down( x,y )
	-- this is a special case for the slider of the MU
	if self.__mu then
		self.__mu:active_link()
	end

	self:__do_click_down_slider_base( x,y, 1 )
end

function SLIDER:set_dialog_simple( b_simple )
	self.__b_dialog_simple = b_simple
end

function SLIDER:is_dialog_simple()
	return self.__b_dialog_simple
end

function SLIDER:do_uif_command( uif )
	aaa.print_fn()

	local b_used = false 
	local mu = self.__mu
	if mu then
		b_used = mu:do_uif_command( uif )
		if not b_used then
			local meu = mu:get_meu()
			b_used = meu:do_uif_command( uif )
		end
	end
	
	return GABU_OBJ.do_uif_command_with_super( self, b_used, uif, SLIDER )
end

function SLIDER:update()
	--self:disable_uif()	--debugging
	--aaa.print_method()
	oo.getsuper(SLIDER).update(self)
end

function SLIDER:get_val_from_xy( x,y )
	if not self:is_dial() then return end

	local angle = math.atan2( y, x ) * math.pi2_over
	if angle < -.25 then angle = angle + 1. end
	--self:print( angle )
	local val = self.ca * angle + self.cb

	if self.val_last then
		local d = val - self.val_last
		if d > .6 then
			val = val - 1.
		elseif d < -.6 then
			val = val + 1.
		end
	end
	self.val_last = val
	return val
--	return clamp_01( val )
end

function SLIDER:__get_lr_ref_from_val_cano( val )
	--if val == nil then self:print( "in __get_lr_ref_from_val_cano() val is nil"  ) end
	--val = clamp_01( val )
	if self:is_meter() then
		local ref = self:get_meter_ref_cano()
		if ref then
			if val < ref then
				return val-.5, ref-.5, ref-.5
			else
				return ref-.5, val-.5, ref-.5
			end
		end
		return -.5, -.5+val
	else
		local du = self.__du
		local l = ( 1. - du ) * val - .5
		return l, l + du
	end
end

function SLIDER:set_tex_draw( color_back, bind_back, bind_main, bind_meter, bind_cursor )
	self.__b_tex_draw 	= true
	self.__color_back 	= color_back
	self.__tex_back 	= bind_back
	self.__tex_main 	= bind_main
	self.__tex_meter 	= bind_meter
	self.__tex_cursor 	= bind_cursor

	self:define_cursor_tex_size()
end

function SLIDER:set_tex_back( bind_back )
	self.__b_tex_draw = true
	self.__tex_back = bind_back
end
function SLIDER:set_tex_main( bind_main )
	self.__b_tex_draw = true
	self.__tex_main = bind_main
end
function SLIDER:set_tex_meter( bind_meter )
	self.__b_tex_draw = true
	self.__tex_meter = bind_meter
end

function SLIDER:gol_color_meter_def() 			gol.color( .5,1,1,  self:get_alpha_bu_to_draw() )	end
function SLIDER:set_meter_color( r_or_table,g,b, a )
	if type(r_or_table) == "table" then	self.__meter_color = r_or_table
	else								self.__meter_color = { r_or_table,g,b, a or 1 }
	end
	return self
end
function SLIDER:set_meter_color_named( name )
	local col = self:get_color_named_no_error( name )
	if col then self:set_meter_color( col ) end
	return self
end

function SLIDER:get_meter_color() 				return self.__meter_color					end
function SLIDER:gol_meter_color( alpha )
	local color = self.__meter_color
	local f_alpha = self:get_alpha_bu_to_draw()
	if color then
		gol.color( color[1], color[2], color[3], (alpha and alpha or color[4]) * f_alpha )
	else
		alpha = alpha and (alpha * f_alpha) or f_alpha
		if self:is_ui_active() then
			gol.color( .5,1,1, alpha )
		else
			gol.color_yellow( alpha )
		end
	end
end

function SLIDER:set_tex_cursor( bind_cursor )
	self.__b_tex_draw = true
	self.__tex_cursor = bind_cursor
	--Bug if i do here. See with Maa
	--self:define_cursor_tex_size()
end


function SLIDER:define_cursor_tex_size()
	-- 1/8 par defaut si il n'y a pas
	local bind_cursor = self:get_tex_cursor()
	local sx_cursor = 0.125

	if bind_cursor then
		local ratio_y = self:get_ratio_y()
		sx_cursor = ratio_y
	end

	self.__sx_cursor = sx_cursor
end

function SLIDER:is_tex_draw() 			return self.__b_tex_draw and GA:is_tex_draw_custom()								end

--function SLIDER:get_tex_all()			return self.__tex_back, self.__tex_main, self.__tex_cursor, self.__tex_progress		end
function SLIDER:get_tex_back()			return self.__tex_back																end
function SLIDER:get_tex_main()			return self.__tex_main																end
function SLIDER:get_tex_cursor()		return self.__tex_cursor															end
function SLIDER:get_tex_meter()			return self.__tex_meter																end

--todo regroup with_step_linear

function SLIDER:set_draw_gamma( b )		self.__b_draw_gamma = b		return self		end
function SLIDER:is_draw_gamma()			return self.__b_draw_gamma	end

function SLIDER:set_draw_force_cr( b )	self.__draw_force_cr = b	return self		end
--
--	DRAW
--
function SLIDER:__draw_separation_ui_line( color )
	--gol.set_line_width( 1 )
	local alpha = self:get_alpha_bu_to_draw()
	if color then
		gol.color( color[1], color[2], color[3], color[4] * alpha )
	else
		gol.color_yellow( .4 * alpha )
	end
	--aaa.draw_rect( -.5, self.__separation_y+.03, .5, self.__separation_y	)	-- faster than line
	aaa.draw_line( -.5, self.__separation_y, .5 , self.__separation_y )
end

--todo3d
function SLIDER:draw_tex_back()
	gol.color_white()
	local tex_back = self:get_tex_back()
	if tex_back then
		aaa.draw_bind_rect( tex_back, -.5, -DY, .5, DY )
	else
		self:draw_back()
	end
end

function SLIDER:draw_tex_main()
	gol.color_white()
	local tex_main = self:get_tex_main()
	if tex_main then
		aaa.draw_bind_rect( tex_main, -.5, -DY, .5, DY )
	end
end

function SLIDER:draw_tex_meter( l, r )
	gol.set_texture_dim( 2 )
	local tex_meter = self:get_tex_meter()

	self:gol_meter_color()

	if tex_meter then
		gol.set_quad_uv(	0, 0,		r + .5, 1	)
		gol.bind_texture( tex_meter )
		aaa.draw_rect_uv( l, -DY, r, DY )
	else
		self:draw_meter( l, r )
	end
end
--todo3d unused
function SLIDER:draw_meter( l, r )
	gol.set_texture_dim( 0 )
	aaa.draw_rect( l, -DY, r, DY )
end

function SLIDER:draw_tex_cursor( l,r )
	gol.set_texture_dim( 2 )
	gol.color_white()
	local tex_cursor = self:get_tex_cursor()

	if tex_cursor then
		self:define_cursor_tex_size()
		local sx_cursor = self.__sx_cursor * .5
		gol.set_quad_uv()
		gol.bind_texture( tex_cursor )

		local du = sx_cursor

		local function draw_circle_cursor( u )
			local p = du / 2 + u * ( 1 - du * 2 )
			-- cursor
			aaa.draw_rect_uv( p - du, -DY, p + du, DY )
		end

		if self.__str_slider_two_draw_step then
			local u = r
			draw_circle_cursor(u)

			local u = l - sx_cursor
			draw_circle_cursor(u)
		else
			if self:is_meter() then
				l = r - sx_cursor
			end

			local u = (r + l) / 2
			draw_circle_cursor(u)
		end
	end
end
--position du slider = du / 2 + u ( 1 – du ) ou du c'est la taille du curseur et c'est sa position dans le slider

function SLIDER:draw()
	--if true then return end
	if self:is_dial() then
		self:__draw_dial()
	else
		self:__draw_slider()
	end
end

function SLIDER:__draw_dial()
	--if true then return end
	local u, du
	local v, dv
	if self:is_meter() then
		du = self:get_value_cano_clamped() * self.ra
		u = .25 + (self.ra - du) * .5
		dv = self.__dv
		v = dv * .5 - .5
	else
		du = .03 -- self.__du
		u = (.5-self:get_value_cano_clamped()) * (self.ra-du) + .25
		dv = .9
		v = self.__dv - dv * .5 - .5
	end

	gol.color_cyan( 0.5 )
	local na = self.na
	local angle = 0.75 - na * .5
	aaa.draw_disk_arc_axe_z(	0, 0, 1, 1, angle, -du, 36 )
	gol.color_black()
	aaa.draw_disk_arc_axe_z( 	0, 0, 1, .2, 0, 1, 36 )

	if self:is_text_draw() then
		gol.color_white()
		local str = self:get_text()
		local val
		if self:is_show_value() then
			val = self:format_value( self:get_value() )
		end
		local b = -.45
		local t = -.2
		if val then
			self:draw_text_lrbt( val, 0, .4, b, t, "right" )
		end
		self:draw_text_lrbt( str, -.4, 0, b, t, "left" )
	end
end

function SLIDER:__draw_meter( l,r, DY )
	if self.__str_slider_two_draw_step then
		--todo make it better and faster
		--deal with inversion too	local f = .4
		local f = 0.33
		self:gol_color_back()
		self:draw_rect_lbrt( l, -DY*f, r, DY*f )
		self:gol_meter_color()
		--top
		self:draw_rect_lbrt( l, DY*f, r, DY )
		--middle
		self:draw_rect_lbrt( l+.02, -DY*f, r-.02, DY*f )
		--bottom
		self:draw_rect_lbrt( l, -DY, r, -DY*f )
	else
		self:draw_rect_lbrt( l, -DY, r, DY )
	end
end

function SLIDER:gol_color_back()
	if self:is_ui_active() then
		BU.gol_color_back( self )
	else
		gol.color( .3,.3,0, self:get_alpha_bu_to_draw() )
	end
end

local SX = .45
local SY = .4
local rect_text = { b=-SY, t=SY, l=-SX, r=SX }
local RATIO_X_ONE_LINE_THRESHOLD = 2.5
function SLIDER:__draw_slider()

	if GA.b_spy then aaa.spy.push_range( self..".__draw_slider()", 4 ) end
	--if true then return end
	local l,r, meter_ref

	local b_slider_two = self.__str_slider_two_draw_step
	local val_cano_1 = self:get_value_cano_clamped(1) or 0
	local val_cano_2
	if b_slider_two then
		val_cano_2 = self:get_value_cano_clamped(2)
		--self:print( "vals cano "..val_cano_1.." "..val_cano_2 )
		--todo SLIDER:__get_lr_ref_from_val_cano(val) only takes one arg, val_cano_2 is silently dropped here (b_slider_two path).
		l,r = self:__get_lr_ref_from_val_cano( val_cano_1, val_cano_2 )
		--self:print( "lr "..l.." "..r )
		--r = r - 1.
	else
		l,r, meter_ref = self:__get_lr_ref_from_val_cano( val_cano_1 )
	end

	if GA.b_spy then aaa.spy.push_range( "SLIDER GEO", 3 ) end
	--	prepare text position scale ...
	--[[
	local dxt, dyt
	local dxtb, dytb
	local FACTOR_X = 1.15
	local SY = .8
	if str then
		local x = 0
		if strb then -- 2 lines
			dyt = SY * .5
			dxt = dyt * ry
			dxtb, dytb = dxt, dyt

			--local l = rx*4.8
			local l = rx * FACTOR_X * 2
			local x = aaa.get_str_translate( str )
			--self:print( str.." -> "..x )
			if x > l then
				dxt = dxt * l / x
				--dyt = dyt * l / x
			end
			local x = aaa.get_str_translate( strb )
			if x > l then
				dxtb =  dxtb * l / x
				--dytb =  dytb * l / x
			end
		else -- 1 line
			dyt = SY
			dxt = dyt * ry
			--local l = rx*2
			local l = rx * FACTOR_X
			local x = aaa.get_str_translate( str )
			if x > l then
				dxt = dxt * l / x
				--dyt = dyt * l / x
			end
		end
	end
	--]]

	--if true then return end

	-- draw background
---[[
	local val_1 = self:get_value(1)
	if self:is_tex_draw() then
		self:draw_tex_back()
		-- draw meter if needed
		if self:is_meter() or b_slider_two then
			self:draw_tex_meter( l,r )
		end
		-- draw main
		self:draw_tex_main()
		-- draw cursor
		self:draw_tex_cursor( l,r )
	else --self:is_tex_draw()
		local b_draw_back = false	--todo 2022 July Maa detected than we already draw the back so this hack to avoid doing it twice 
	--	draw background and knob
		local str_step = self.__str_slider_two_draw_step
		if str_step and (str_step ~= "range") then	--todo processed below
			self:gol_meter_color()
			--self:print( l.." "..r )
			if str_step == "linear_inverse" then	--to this not symetric so should it have another name
				if l==r then
					if l==-.5 then
					elseif l==.5 then
						self:draw_rect_lbrt( -.5,-.5, .5,.5 )
					else
						--self:print( "In "..l.." "..r )
						self:draw_rect_lbrt( -.5,-.5, r,.5 )
					end
				else
					gol.draw_triangle_strip_2d( -.5,-.5,	-.5,.5,		r,-.5,  l,.5 )
				end
			elseif str_step == "linear" then
				--todo if self:is_draw_gamma() then
				if l < r then
					gol.draw_triangle_strip_2d( l,-.5,	r,.5,	.5,-.5,	.5,.5 )
				elseif l > r then
					gol.draw_triangle_strip_2d( -.5,-.5, -.5,.5, l,-.5, r,.5 )
				else
					--self:print( "  "..self:get_value(1).." "..self:get_value(2) )
					--self:print( l.." "..r )
					if l==-.5 then
						if val_cano_1 <= val_cano_2 then
							self:draw_rect_lbrt( -.5,-.5, .5,.5 )
						end
					elseif l==.5 then
						if val_cano_1 > val_cano_2 then
							self:draw_rect_lbrt( -.5,-.5, .5,.5 )
						end
					else
						--self:print( "In "..l.." "..r )
						self:draw_rect_lbrt( l, -.5, .5, .5 )
					end
				end
			elseif str_step == "step_two" then
				local h = .3
				local fn = l < r and gol.draw_triangle_strip_2d or gol.draw_triangles_2d
				fn( -.5,-.5,	l,h,	 l,-.5,		r,h,  r,-.5,	.5,-.5	)
			else
				self:print_error( "SLIDER:__draw_slider() can have self.__str_draw_step as "..type(str_step).." "..str_step )
			end
		elseif self:is_draw_gamma() then
			--self:print( "draw_gamma "..g )
			self:gol_meter_color()
			local x,y = -.5,-.5
			local nb = 16
			for v=1/nb,1.001,1/nb do
				local vn = math.pow(v,1/val_1)
				local xn,yn = vn-.5, math.pow(vn,val_1)-.5
				--gol.draw_lines_2d( x,y, xn,yn )
				gol.draw_triangle_fan_2d( x,-.5, x,y, xn,yn, xn,-.5 )
				x,y = xn,yn
			end
		elseif r <= -.5 and not b_slider_two then
			if b_draw_back then
				self:gol_draw_rect_back()
			end
		elseif r < l then
			self:gol_meter_color()
			self:__draw_meter( -.5, r, DY )
			self:__draw_meter( l, .5, DY )
			-- back
			self:gol_color_back()

			if b_draw_back then
				self:draw_rect_lbrt( r, -DY, l, DY )
			end
		else
			self:gol_meter_color()
			--DY = DY + math.sin(aaa.time.t)	--for demo
			if .5 <= r then
				local F = 100
				if l <= -.5 then
					--	cursor / meter
					self:__draw_meter( -.5, .5, DY )
				else
					--	cursor / meter
					self:__draw_meter( l, r, DY )
					--Franz 	aaa.draw_rect( .49, -DY, .51, DY )
					--BACK
					--	before
					if b_draw_back then
						self:gol_color_back()
						self:draw_rect_lbrt( -.5, -DY, l, DY )
					end
				end
			else
				--	cursor / meter
				self:__draw_meter( l, r, DY )
				--BACK
				--	after
				if b_draw_back then
					self:gol_color_back()
					if -.5<l then
						self:draw_rect_lbrt( -.5, -DY, l, DY )
					end
					if r<.5 then
						self:draw_rect_lbrt( r, -DY, .5, DY )
					end
				end
				--	before
--				if not self:is_meter() or b_slider_two then
--					self:draw_rect_lbrt( -.5, -DY, l, DY )
--				end
			end
		end
		--meter_ref = 0
		if meter_ref then
			gol.color_white( .8 )
			local dy = DY * 1.1
			local dx = dy * self:get_ratio_y() * .5
			gol.draw_triangles_2d( meter_ref,.1, meter_ref+dx,dy, meter_ref-dx,dy )
			--gol.draw_triangles_2d( meter_ref,-.1, meter_ref-dx,-dy, meter_ref+dx,-dy )
		end
	end	--else self:is_tex_draw()

--]]
	--	draw previous position
	local val_begin = self.__uif_val_begin 
	if val_begin then
		l, r = self:__get_lr_ref_from_val_cano( val_begin.val_cano )
		self:gol_meter_color()

		gol.draw_lines_2d(	r, .5,	r, .9,		r, -.5, r, -.9	)
		if not self:is_meter() and not b_slider_two then
			gol.draw_lines_2d(	l, .5,	l, .9,		l, -.5, l, -.9	)
		end
		gol.color_black()
		aaa.draw_line(	r, -.5,	r, .5	)
		if not self:is_meter() and not b_slider_two then
			aaa.draw_line(	l, -.5,	l, .5	)
		end
	end

	--	draw separation betwwen relative and absolute UI
	if not self:is_uif_enabled() and self:is_ui_active() then
		local color = self.meter_color
		self:__draw_separation_ui_line( color )
	end

--TEXT
---[[
	if GA.b_spy then aaa.spy.pop_range() end	-- end GEO
---[[
	--todo rect will be precompted for the class and just passed here
	local str1, str2
	if self:is_text_draw() then
		str1 = self:get_text()
		if str1 == "" then
			str1 = nil
		end
	end
	if self:is_show_value() then		
		if self:is_value_to_draw( val_1 ) then
			if str1 then
				str2 = self:format_value( val_1 )
			else
				str1 = self:format_value( val_1 )
			end
		end
	end
					
	local b_force_cr = self.__draw_force_cr
	if b_force_cr==nil then
		local rx = self:get_ratio_x()
		if b_slider_two then
			b_force_cr = rx < RATIO_X_ONE_LINE_THRESHOLD --One or two lines depending on x ratio
		else
			b_force_cr = rx < RATIO_X_ONE_LINE_THRESHOLD --One or two lines depending on x ratio
		end
	end

	if str2 then
		if not b_force_cr then
			str1 = str1 .. " " .. str2
			str2 = nil
		end
	end

	if str1 then
		if GA.b_spy then aaa.spy.push_range( "Text", 5 ) end

			local SX = .45
			local SY = .4
			local rect = {}
			self:draw_text_color_sel()
			if str2 then
				rect.l = -SX
				rect.r = SX
				if str1 then
					rect.b = .25 - SY * .5
					rect.t = .25 + SY * .5
					self:draw_text_in_rect( str1, rect )
				end
				rect.b = -.25 - SY * .5
				rect.t = -.25 + SY * .5
				self:draw_text_in_rect( str2, rect )
			else
				rect.b = -SY
				if b_slider_two then
					--if val_r < val then
					--	val_r, val = val, val_r
					--end
					local str_v1 = self:format_value( val_1 )
					local str_v2 = self:format_value( self:get_value(2) )
					rect.l = -SX*.5
					rect.r = SX*.5
					rect.t = SY
					local f
					if b_force_cr then
						rect.b = 0
						self:draw_text_in_rect( str1, rect, "center" )
						rect.t = 0
						rect.b = -SY
						f = .1
					else
						rect.b = -SY
						self:draw_text_in_rect( str1, rect, "center" )
						f = .5
						rect.r = -SX*f
						rect.t = 0.3	--this set the size of numbers
					end
					rect.l = -SX
					rect.r = -SX*f
					self:draw_text_in_rect( str_v1, rect )
					rect.l = SX*f
					rect.r = SX
					self:draw_text_in_rect( str_v2, rect, "right" )
				else--if str then
					rect.l = -SX
					rect.r = SX
					rect.t = SY
					self:draw_text_in_rect( str1, rect, "center" )--  -.45, -.35, dxt, dyt )
				end
			end

		if GA.b_spy then aaa.spy.pop_range() end
	end
--]]

	if GA.b_spy then aaa.spy.pop_range() end
end

function SLIDER:__add_value_def_from_min_max( min,max, value )
	if min and max and min < 0 and 0 < max then
		if max == -min then
			--self:print( "__set_min_max_low() with mon = -max" )
			local val = 1
			while val < max do
				self:add_values_def( -val )
				self:add_values_def( val )
				val = val * 2
			end
			self:add_values_def( 0 )
		end
	elseif min==0 or max==0 then

		if min == -1 then
			self:add_values_def( -.5 )
		end
		if max == 1 then
			self:add_values_def( .5 )
		end
	end
	min, max = self:get_min_max()
	if value ~= min and value ~= max and min and max then
		if not value then value = ( min + max ) * .5 end
		self:add_values_def( value )
	end
end

function SLIDER:__set_min_max_low( min,max, value )
	if min and max and min < 0 and 0 < max then
		self:set_meter( false )
	elseif min==0 or max==0 then
		self:set_meter( true )
	end
	self:__add_value_def_from_min_max( min,max, value )
end

function SLIDER:set_min_max( min,max )
	oo.getsuper(SLIDER).set_min_max( self, min,max )
	self:__set_min_max_low( min,max )
	return self
end
function SLIDER:set_min_max_value( min,max, value )
	oo.getsuper(SLIDER).set_min_max_value( self, min,max, value )
	self:__set_min_max_low( min,max, value )
	return self
end

function SLIDER:set_value( val, ... )
	if type(val)=="boolean" then
		val = val and 1 or 0	-- make sure slider BALUE don't become bool
								--todo should we deal there at BALUE level instead
	end
	oo.getsuper(SLIDER).set_value( self, val, ... )
end
--function SLIDER:set_value_cano(...)
-- 	aaa.debug.print_traceback()
-- 	oo.getsuper(SLIDER).set_value_cano( self, ... )
--end

function SLIDER:do_click_double( x,y )
	--self:box_debug( "SLIDER:do_click_double()" )
	--aaa.print_fn()
	if self:edit_dialog_param() then
		self:end_uif( true, self..":do_click_double()")
		return false
	end
	return true
end

function SLIDER:set_range_angle( ra )
	if not self:is_dial() then return end
	self.ra = ra
	self.na = 1 - self.ra
	self.ca = 1. / ra
	self.cb = ( - ra * .5 - .25 ) / ra
end

function SLIDER:do_key( key )
	self:print_do_key( "SLIDER", key )

	local b_key_used = false

	if not aaa.keyboard.is_alt_only() then
		local c = string.char(key)
		local id = self:get_id_interacting()
		local val = self:get_value( id )
		local val_new
		if		c == "+"	then	val_new = self:is_value_type_integer() and (val + 1) or (val * 1.1)
		elseif	c == "-"	then	val_new = self:is_value_type_integer() and (val - 1) or (val / 1.1)
		elseif	c == "*"	then	val_new = val * 2
		elseif	c == "/"	then	val_new = val / 2
		elseif	c == "."	then	val_new = math.floor(val)
		else						b_key_used = false
		end
		if val_new then
			self:set_value_ui( val_new,	id )
			b_key_used = true
		end
	end

	return b_key_used or oo.getsuper(SLIDER).do_key( self, key )
end

function SLIDER:do_key_special( key )
	self:print_do_key_special( "SLIDER", key )

	local b_key_used = false
--	if			key == 272 then				-- ARROW_UP
--	elseif		key == 273 then				-- ARROW_DOWN
	if false then
	elseif not aaa.keyboard.is_alt() then
		local id = self:get_id_interacting()
		local val = self:get_value( id )
		local val_new
		if	key == 267	then	-- CTRL /
			val_new = val / 1.5
		elseif	key == 268	then -- CTRL *
			val_new = val * 1.5
		elseif	key == 269	then
			val_new = self:is_value_type_integer() and (val + 1) or (val * 1.025)
		elseif	key == 270	then
			val_new = self:is_value_type_integer() and (val - 1) or (val / 1.025)
		end
		if val_new then
			self:set_value_ui( val_new,	id )
			b_key_used = true
		end
	end

	return b_key_used or oo.getsuper(SLIDER).do_key_special( self, key )
end

function SLIDER:create( name, rect )
	if rect then
		-- if slider is too narrow that mean it will be vertical
		local r3,r4 = rect[3],rect[4]
		if r3 and r4 and (r4 >= r3) then
			rect[3],rect[4] = r4,r3
	   		rect[5] = (rect[5] or 0) + .25
		end
   end
   
	--todo use the GABU creation strategy
	local self = SLIDER:create_instance( name, rect )
	self:set_method_on_click_double( self, "do_click_double" )
	return self
end

--	MIDI HELPER FNS
--
--todo propagate list return strategy
function SLIDER.create_midi_hori( x_min, sx, y_min, sy, nb, ch, ctl )
	local	tab = {}
	local	f = .8
	local	x = x_min + sx * .5
	local	dy = sy / ( nb - 1 + f )
	local	y = y_min + sy - dy * .5 * f
	local	bu
	for i=1,nb do
		bu = bus_cur:add_slider( "Slider MIDI hori "..(i-1), {	x, y, sx, dy * f } )
		if ch then
			bu:set_midi( ch, ctl + i - 1, true )
			bu:set_text_draw( i==1 or i==5 )
			bu:set_pos_load_save( false )
		end
		tab[i] = bu
		y = y - dy
	end
	return tab
end
function SLIDER.create_midi_vert( x_min, sx, y_min, sy, nb, ch, ctl )
	local	tab = {}
	local	f = .8
	local	y = y_min + sy * .5
	local	dx = sx / ( nb - 1 + f )
	local	x = x_min + dx * .5 * f
	local	bu
	for i=1,nb do
		--aaa.print( "bordel "..x.." "..y )
		bu = bus_cur:add_slider( "Slider MIDI vert "..(i-1), {	x, y, sy, dx * f, .25 } )
		bu:set_meter( true )
		if ch then
			bu:set_midi( ch, ctl + i - 1, true )
			bu:set_text_draw( i==1 or i==5 )
			bu:set_pos_load_save( false )
		end
		tab[i] = bu
		x = x + dx
	end
	return tab
end



	local tab = SLIDER:get_doc()
	tab.set_tex_cursor = ""
		--Bug if i do here. See with Maa
		--self:define_cursor_tex_size()
	tab.define_cursor_tex_size = ""
		-- 1/8 par defaut si il n'y a pas
	tab.draw_tex_cursor = ""
	--		local function draw_circle_cursor( u )
				-- cursor
	--position du slider = du / 2 + u ( 1 – du ) ou du c'est la taille du curseur et c'est sa position dans le slider
	tab.__draw_meter = ""
			--todo make it better and faster
			--deal with inversion too	local f = .4
	tab.__draw_slider = ""
		--	prepare one or two string with their
				--	One or two lines depending on x
		--	prepare text position scale ...
		-- draw background
			-- draw meter if needed
			-- draw main
			-- draw cursor
		--	draw background and knob
				-- back
				--DY = DY + math.sin(aaa.time.t)	--for demo
						--	cursor / meter
						--	cursor / meter
						--Franz 	aaa.draw_rect( .49, -DY, .51, DY )
						--BACK
						--	before
					--	cursor / meter
					--BACK
					--	after
					--	before
		--	draw previous position
		--	draw separation betwwen relative and absolute UI
		--todo rect will be precompted for the class and just passed here"
