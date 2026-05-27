


-- PONG useful functions
local function array_shift(array, n)
	-- used for time integration as a "memory" of the previous states
	-- for each frame into an  indexed register: array[0=current_state], array[1=prev_frame_state],..
	for i=1, n do
		array[(n-i)+1] = array[(n-i)]
	end
	-- to be followed by instructions to update current state array[0]=current_value
 end

local function dyn_shift( struct, n )
 	array_shift( struct.x, n )
	array_shift( struct.y, n )
	array_shift( struct.dx, n )
	array_shift( struct.dy, n )
	array_shift( struct.ddx, n )
	array_shift( struct.ddy, n )
 end

local function saturation(x, x_min, x_max, sat_min, sat_max, sat_power)

	-- SMOOTH EXPONENTIAL SATURATION
	-- constraints any variable x ranging into [x_min x_max]
	-- to be "smoothly saturated" (fonction sigmoid c�d coud��)
	-- and ouput onto the range [0 1] or [sat_min sat_max]

	local x_mn = x_min or 0
	local x_mx = x_max or 1
	local x_mid = (x_max + x_min)/2 or 0.5
	local x_range = (x_max - x_min) --( math.max(xmin,xmax) - math.min(xmin,xmax) )

	local y_mn = sat_min or 0
	local y_mx = sat_max or 1

	local xpon = 2 * sat_power / x_range
	local xvar = xpon * (x - x_mid)
	local xsat01 = math.exp(xvar) / ( math.exp(xvar) + math.exp(-xvar) )
	local xsat =  (y_mx - y_mn) * xsat01

	-- print( "saturation: x_in=" .. x-x%0.01 .. "x_out=".. xsat-xsat%0.01)
	-- print( "t=" .. aaa.time.t-aaa.time.t%0.001 .. "\t dt=" .. aaa.time.dt-aaa.time.dt%0.001)

	return(xsat)

end

--[[ GET/SET X,Y
function BALL:get_xy()		return self.x, self.y	end
function BALL:set_xy( x, y )	self.x = x, self.y = y end
--]]

local function affich7(n, x, y, size)

	local digit = ( n % 10 )

	local d_hei = 0.2 * size	-- height
	local d_wid = 0.1 * size	-- width
	local d_thi = 0.05* size	-- thickness

	--[[ Full 7 segments
	if digit == 8 then
		aaa.draw_rect( x-d_wid, 		y-d_hei, 		x+d_wid, 		y-d_hei + d_thi) 	-- H-1
		aaa.draw_rect( x-d_wid, 		y,		 		x+d_wid, 		y + d_thi)			-- H-0
		aaa.draw_rect( x-d_wid, 		y+d_hei, 		x+d_wid, 		y+d_hei - d_thi)	-- H+1
		aaa.draw_rect( x-d_wid, 		y-d_hei, 		x-d_wid+d_thi,	y)					-- V-1L
		aaa.draw_rect( x-d_wid, 		y, 				x-d_wid+d_thi,	y+d_hei)			-- V+1L
		aaa.draw_rect( x+d_wid-d_thi, 	y-d_hei, 		x+d_wid, 		y)					-- V-1R
		aaa.draw_rect( x+d_wid-d_thi, 	y, 				x+d_wid, 		y+d_hei) 			-- V+1R
	end--]]

	if 		digit == 0 then
		aaa.draw_rect(	x-d_wid, 		y-d_hei, 		x+d_wid , 		y-d_hei + d_thi) 	-- H-1
		aaa.draw_rect(	x-d_wid, 		y+d_hei, 		x+d_wid , 		y+d_hei - d_thi)	-- H+1
		aaa.draw_rect(	x-d_wid, 		y-d_hei, 		x-d_wid+d_thi,	y)					-- V-1L
		aaa.draw_rect(	x-d_wid, 		y, 				x-d_wid+d_thi,	y+d_hei)			-- V+1L
		aaa.draw_rect(	x+d_wid-d_thi,	y-d_hei, 		x+d_wid, 		y)					-- V-1R
		aaa.draw_rect(	x+d_wid-d_thi,	y, 				x+d_wid, 		y+d_hei) 			-- V+1R

	elseif 	digit == 1 then
		aaa.draw_rect(	x+d_wid-d_thi, 	y-d_hei, 		x+d_wid, 		y)					-- V-1R
		aaa.draw_rect(	x+d_wid-d_thi, 	y, 				x+d_wid, 		y+d_hei) 			-- V+1R

	elseif 	digit == 2 then
		aaa.draw_rect(	x-d_wid, 		y-d_hei, 		x+d_wid, 		y-d_hei + d_thi) 	-- H-1
		aaa.draw_rect(	x-d_wid, 		y		, 		x+d_wid, 		y + d_thi)			-- H-0
		aaa.draw_rect(	x-d_wid, 		y+d_hei, 		x+d_wid, 		y+d_hei - d_thi)	-- H+1
		aaa.draw_rect(	x-d_wid, 		y-d_hei, 		x-d_wid+d_thi,	y)					-- V-1L
		aaa.draw_rect(	x+d_wid-d_thi,	y, 				x+d_wid, 		y+d_hei) 			-- V+1R

	elseif 	digit == 3 then
		aaa.draw_rect(	x-d_wid, 		y-d_hei, 		x+d_wid, 		y-d_hei + d_thi) 	-- H-1
		aaa.draw_rect(	x-d_wid, 		y		, 		x+d_wid, 		y + d_thi)			-- H-0
		aaa.draw_rect(	x-d_wid, 		y+d_hei, 		x+d_wid, 		y+d_hei - d_thi)	-- H+1
		aaa.draw_rect(	x+d_wid-d_thi,	y-d_hei, 		x+d_wid, 		y)					-- V-1R
		aaa.draw_rect(	x+d_wid-d_thi,	y, 				x+d_wid, 		y+d_hei) 			-- V+1R

	elseif 	digit == 4 then
		aaa.draw_rect(	x-d_wid, 		y,		 		x+d_wid, 		y + d_thi)			-- H-0
		aaa.draw_rect(	x-d_wid, 		y, 				x-d_wid+d_thi,	y+d_hei)			-- V+1L
		aaa.draw_rect(	x+d_wid-d_thi,	y-d_hei, 		x+d_wid, 		y)					-- V-1R
		aaa.draw_rect(	x+d_wid-d_thi,	y, 				x+d_wid, 		y+d_hei)	 		-- V+1R

	elseif 	digit == 5 then
		aaa.draw_rect(	x-d_wid, 		y-d_hei, 		x+d_wid, 		y-d_hei + d_thi) 	-- H-1
		aaa.draw_rect(	x-d_wid, 		y, 				x+d_wid, 		y + d_thi)			-- H-0
		aaa.draw_rect(	x-d_wid, 		y+d_hei, 		x+d_wid,	 	y+d_hei - d_thi)	-- H+1
		aaa.draw_rect(	x-d_wid, 		y, 				x-d_wid+d_thi,	y+d_hei)			-- V+1L
		aaa.draw_rect(	x+d_wid-d_thi,	y-d_hei, 		x+d_wid, 		y)					-- V-1R

	elseif 	digit == 6 then
		aaa.draw_rect(	x-d_wid, 		y-d_hei, 		x+d_wid, 		y-d_hei + d_thi) 	-- H-1
		aaa.draw_rect(	x-d_wid, 		y,		 		x+d_wid, 		y + d_thi)			-- H-0
		aaa.draw_rect(	x-d_wid, 		y+d_hei, 		x+d_wid, 		y+d_hei - d_thi)	-- H+1
		aaa.draw_rect(	x-d_wid, 		y-d_hei, 		x-d_wid+d_thi,	y)					-- V-1L
		aaa.draw_rect(	x-d_wid, 		y, 				x-d_wid+d_thi,	y+d_hei)			-- V+1L
		aaa.draw_rect(	x+d_wid-d_thi,	y-d_hei, 		x+d_wid, 		y)					-- V-1R

	elseif 	digit == 7 then
		aaa.draw_rect(	x-d_wid, 		y+d_hei, 		x+d_wid, 		y+d_hei - d_thi)	-- H+1
		aaa.draw_rect(	x+d_wid-d_thi,	y-d_hei, 		x+d_wid, 		y)					-- V-1R
		aaa.draw_rect(	x+d_wid-d_thi,	y, 				x+d_wid, 		y+d_hei)	 		-- V+1R

	elseif 	digit == 8 then
		aaa.draw_rect(	x-d_wid, 		y-d_hei, 		x+d_wid, 		y-d_hei + d_thi) 	-- H-1
		aaa.draw_rect(	x-d_wid, 		y,		 		x+d_wid, 		y + d_thi)			-- H-0
		aaa.draw_rect(	x-d_wid, 		y+d_hei, 		x+d_wid, 		y+d_hei - d_thi)	-- H+1
		aaa.draw_rect(	x-d_wid, 		y-d_hei, 		x-d_wid+d_thi,	y)					-- V-1L
		aaa.draw_rect(	x-d_wid, 		y, 				x-d_wid+d_thi,	y+d_hei)			-- V+1L
		aaa.draw_rect(	x+d_wid-d_thi,	y-d_hei, 		x+d_wid, 		y)					-- V-1R
		aaa.draw_rect(	x+d_wid-d_thi,	y, 				x+d_wid, 		y+d_hei)	 		-- V+1R

	elseif 	digit == 9 then
		aaa.draw_rect(	x-d_wid, 		y-d_hei, 		x+d_wid, 		y-d_hei + d_thi) 	-- H-1
		aaa.draw_rect(	x-d_wid, 		y, 				x+d_wid, 		y + d_thi)			-- H-0
		aaa.draw_rect(	x-d_wid, 		y+d_hei, 		x+d_wid, 		y+d_hei - d_thi)	-- H+1
		aaa.draw_rect(	x-d_wid, 		y, 				x-d_wid+d_thi,	y+d_hei)			-- V+1L
		aaa.draw_rect(	x+d_wid-d_thi,	y-d_hei, 		x+d_wid, 		y)					-- V-1R
		aaa.draw_rect(	x+d_wid-d_thi,	y, 				x+d_wid, 		y+d_hei)	 		-- V+1R

	end

end --]]



-- *** PONG: CLASSES DEFINITIONS *** --

-- BALL Class
CLASS.DECLARE( "BALL" )

function BALL:create( i, pong )
	local self = BALL:create_instance( "ball_"..i )
	self.pong = pong -- reference parent
	self:init( i )
	return self
end

function BALL:init( i )

	self.id = i

	self.size = 0.05

	--maa more sense in dealing with pos = { x, y, z } and a pos array
	--	instead of having an array on each
	--luc >> linear arrays are kept in memory for dynamic calculation from previous states
	self.x = {}
	self.y = {}
	self.z = {}
	self.dx = {}
	self.dy = {}
	self.dz = {}
	self.ddx = {}
	self.ddy = {}
	self.ddz = {}

	local n_dynsteps = (self.pong.n_dynsteps or 3)
	for n = 0, n_dynsteps do
		self.x[n] = 0
		self.y[n] = 0
		self.z[n] = 0
		self.dx[n] = 0
		self.dy[n] = 0
		self.dz[n] = 0
		self.ddx[n] = 0
		self.ddy[n] = 0
		self.ddz[n] = 0
	end

	local v_min = 3
	local v_max = 8
	local  rand_veloc = math.random (v_min,v_max)
	self.init_vel = rand_veloc
	local rand_angle = math.random (0, math.pi2)
	local rand_angle_is_horvert = (rand_angle % math.pi/4)  == 0
	if rand_angle_is_horvert then rand_angle = math.pi/3 end -- avoid square angles mov.
	self.init_angle = rand_angle

	self.dx[0] = self.init_vel *  math.cos(self.init_angle)
	self.dy[0] = self.init_vel * math.sin(self.init_angle)

	self:print( "init done!" )

end

function BALL:update()
	local dt = aaa.time.dt

	-- update by time integration by & shift on frame index
	-- function dynShift (structure, n_integration) is assumed to be called before update:
	-- update IN: [0]=current, [1]=curentCopy, [2]=previous, [3]=prev/previous
	-- >>update OUT: [0]=current_new, [1]=currentOld, [2]=previous, [3]=prev/previous

	-- update ball velocity with accelaration
	local vx = self.dx[1] + (self.ddx[1]) * dt
	local vy = self.dy[1] + (self.ddy[1]) * dt

	-- update ball position with velocity
	local x = self.x[1] + vx * dt
	local y = self.y[1] + vy * dt

	-- interaction decor
	local wall_y_top = self.pong.decor.wall.top_y
	local wall_y_bot = self.pong.decor.wall.bot_y
	local wall_len = self.pong.decor.wall.length
	local delta_goal = 2
	local b_goal = false
	-- Goal 1
	if ( x >= wall_len/2 + delta_goal) then
		self.pong.score[1].c = self.pong.score[1].c + 1
		b_goal = true
	end
	-- Goal 2
	if ( x <= -wall_len/2 - delta_goal) then
		self.pong.score[2].c = self.pong.score[2].c + 1
		b_goal = true
	end
	-- Arriv�e fond de cours
	if b_goal then

		self:init(1)  -- ???
		--pong.ia[1]:init()
		--pong.ia[2]:init()
		-- PRINT ON INT. MONITOR WINDOW
		--self:print( "self:init(1)_called" )

		return
	end

	-- collision wall
	local delta_wall = (self.pong.decor.wall.width + self.size)
	if y  < wall_y_bot then
		vy = -vy
		y = wall_y_bot - ( y - wall_y_bot ) + delta_wall
	elseif y > wall_y_top then
		vy = -vy
		y = wall_y_top - ( y - wall_y_top ) - delta_wall
	end

	-- Collisions raquettes
	-- X/Y_Cross interval Method
	for i=1, self.pong.n_rackets do

		local racket = self.pong.racket[i]

		local deltax_ini = (self.x[2]-racket.x[2])
		local deltax_fin = (self.x[1]-racket.x[1])

		local delta_width = (racket.width+self.size)/20
		local isXCrossed = ((deltax_ini)*(deltax_fin) <= delta_width)


		--[[local deltay_ini = (self.y[2]-(racket.y[2]+racket.length/2))
		local deltay_fin = (self.y[1]-(racket.y[1]+racket.length/2))
		local isYCrossed = ((deltay_ini)*(deltay_fin) <= 0)
		--local interframe_Rdx = (racket[1].x[1]-racket[1].x[2])
		--local interframe_bdx = (self.x[1]-self.x[2])
		]]--

		local is_intoY = inside( self.y[1], racket.y[1] - racket.length, racket.y[1] + racket.length )

		local wasnot_just_Xreversed = ( self.dx[1]*self.dx[2] >= 0 )
		local Nframe=3
		local wasnot_soon_Xreversed = ( self.dx[1]*self.dx[1+Nframe] >= 0 )
		local Nframe=9
		local wasnot_late_Xreversed = ( self.dx[1]*self.dx[1+Nframe] >= 0 )
		local Xnotreversed = wasnot_just_Xreversed and wasnot_soon_Xreversed and wasnot_late_Xreversed

		local field_sign = 3 - (2*i)
		local is_righ_vx =(field_sign*self.dx[0]) < 0

		-- if isXCrossed and isYCrossed and wasnot_just_reversed then
		-- if  wasnot_just_Xreversed and isXCrossed and is_intoY then
		if  Xnotreversed and isXCrossed and is_intoY and is_righ_vx then

			vx = -vx -- ball reversed
			x = x + 2 * vx * dt -- +2*deltax_fin

			-- interactions between velocities:
			local racket_impulse_mode = true -- lift mode?
			local impulse_vx = 0
			local impulse_vy = 0
			local impulse2_vy = 0
			if 	(racket_impulse_mode) then
				impulse_vx = -(racket.x[1] - racket.x[0]) /dt
				impulse_vy = (racket.y[1] - racket.y[0]) /dt
				impulse2_vy = math.abs(racket.y[2] - racket.y[1])
			end
			vx = vx * (1 + 5 * impulse2_vy) -- smooth acceleration
			--vx = vx + impulse_vy -- ball impulsed
			--vy = vy + impulse_vy

			--[[ Monitor
			local var1= impulse_vx
			local var2=	impulse_vy
			print( "var1= " ..  var1-var1%0.001)
			print( "var2= " ..  var2-var2%0.001) ]]--

		end

	end

	-- ball's attributes update
	self.x[0] = x
	self.y[0] = y
	self.dx[0] = vx
	self.dy[0] = vy

end

function BALL:draw()
	local size = self.size
	local x = self.x[0]
	local y = self.y[0]

	-- OpenGL sets:
	gol.set_texture_dim( 0 )
	gol.set_line_width( 1. )
	gol.color_green()

	aaa.draw_rect( x-size , y-size, x+size , y+size )
	--aaa.draw_str_xy( "ball", pong.ball.x - .1, pong.ball.y + .07, .1, .1 )
end


-- RACKET Class
CLASS.DECLARE( "RACKET" )

function RACKET:create( i, pong )
	local self = RACKET:create_instance( "racket_"..i )
	self.pong = pong -- reference parent
	self:init( i )
	return self
end

function RACKET:init( i )
	self.id = i

	self.width = 0.05/2
	self.length = 0.5

	self.x = {}
	self.y = {}
	self.z = {}
	self.dx = {}
	self.dy = {}
	self.dz = {}
	self.ddx = {}
	self.ddy = {}
	self.ddz = {}

	local n_dynsteps=  (self.pong.n_dynsteps or 3)
	for n=0, n_dynsteps do
	self.x[n] = 0
		self.y[n] = 0
		self.z[n] = 0
		self.dx[n] = 0
		self.dy[n] = 0
		self.dz[n] = 0
		self.ddx[n] = 0
		self.ddy[n] = 0
		self.ddz[n] = 0
	end

	self.x[0] = (((i==1) and -2) or 2)

	self.control= {}
	self.control.on_midi = false
	self.control.on_ia = true
	self.control.on_mouse = true
	self.control.on_kinect = false
	self.control.on_external = false

	-- External Midi Control:
	--self.y_bu_id 	= (((i==1) and 1) or 9)

	self.y_midicc   = (((i==1) and 1) or 9)
	self.x_midicc   = (((i==1) and 2) or 10)
	self.len_midicc = (((i==1) and 3) or 11)

	self:print( "init done!" )

end

--[[ old unused external control...
function RACKET:set_xyz( ext_x, ext_y, ext_z )
	-- direct routing from "external control"
	-- RQ: x, y, z MUST be comprised into [0 1]
	-- >> to be adapted with decor (cf satur into RAcket:draw)
	local racket = self
	racket.x[0] = ext_x
	racket.y[0] = ext_y
	racket.z[0] = ext_z
end

function RACKET:set_len(len)
	-- direct routing from "external control"
	self.length = len
end

function RACKET:set_control(control_string, boolean)

	if 		(control_string == "midi" ) then
			self.control.on_midi = boolean
	elseif 	(control_string == "ia" ) then
			self.control.on_ia = boolean
	elseif 	(control_string == "mouse" ) then
			self.control.on_mouse = boolean
	elseif 	(control_string == "kinect" ) then
			self.control.on_kinect = boolean
	end

end
--]]

function RACKET:update()
	local pong = PONG.cur -- reference parent

	local wall = self.pong.decor.wall
	local xmax = (wall.length)/2
	local len_min = 0.01
	local len_max = 0.3 * (wall.top_y - wall.bot_y)
	local len = self.length

	-- default values -- TO BE CHANGED?/ Old init for midi ctrl..
	local y_val01 = 0
	local x_val01 = ( (self.id) / 1.5 ) - 0.5

	-- MIDI controls  Values --
	-- if (interface.player[self.id].is_on_midi) then
	 if (self.control.on_midi) then
		-- midi CC1 & CC9 control raquette Y posit (method by bu:get_value)
		--local bu_val = interface.controler_box.bu_r[self.y_bu_id]:get_value(1)
		--y_midi = bu_val
		local midi_val = aaa.midi.get_control( 1, self.y_midicc ) -- between [0 1]
		y_val01 = midi_val
		-- midi CC2 & CC10 control raquette X posit (method by aaa.midi.get)
		local midi_val = aaa.midi.get_control( 1, self.x_midicc ) -- between [0 1]
		x_val01 = midi_val
		-- midi CC3 & CC11 control raquette lengt on R[1] & R[2]
		local midi_val = aaa.midi.get_control( 1, self.len_midicc ) -- between [0 1]
		len = midi_val * len_max + len_min

		-- MIDI Listenig Pause
		-- wait = 10
		-- sample_period = 1 -- sample every n sec.
		-- sample_current_time = aaa.time.t  -- temps en seconde
		-- sample_current_val = y_midi
		-- sample_hold_time = 0 --!! has to be Global to be kept non_init?
		-- sample_hold_val = 0.5 --!! has to be Global to be kept non_init?
		-- if ( sample_current_time > sample_hold_time + sample_period ) then
		--	sample_hold_time = sample_current_time
		--	sample_hold_val = sample_current_val
		-- end
		-- midi_moved = (sample_hold_val ~= sample_current_val)
		-- if midi_moved then
		--	y_val01 = sample_current_val
		--  end
	end

	-- IA controls Values
	if (self.control.on_ia) then
	--if (interface.player[self.id].is_on_ia) then
			y_val01 = self.pong.ia[self.id].sine_jig.value01
		--x_val01 = (self.pong.ia[self.id].sine_jig.value01)/4
	end

	-- Mouse controls Values
	if (self.control.on_mouse) then
	--if (interface.player[self.id].is_on_mouse) then
	
	 	local click_on_mouse
		local click01 = param.get(param.get_ref( aaa.ref.pref, "mouse_button_left" ) )
		if click01== 0 then click_on_mouse = false end
		if click01== 1 then click_on_mouse = true end

		local screen_w = param.get(param.get_ref( aaa.ref.pref, "camera_format_custom_x" ) )
		local screen_h = param.get(param.get_ref( aaa.ref.pref, "camera_format_custom_y" ) )
		local mouse_posx = param.get( aaa.ref.mouse_x )
		local mouse_posy = param.get( aaa.ref.mouse_y )
		local mouse_offsety = -125
		mouse_posy = mouse_posy + mouse_offsety
		local mouse_inside_w
		if (self.id == 1) and (mouse_posx < screen_w/2) then mouse_inside_w = true end
		if (self.id == 1) and (mouse_posx > screen_w/2) then mouse_inside_w = false end
		if (self.id == 2) and (mouse_posx < screen_w/2) then mouse_inside_w = false end
		if (self.id == 2) and (mouse_posx > screen_w/2) then mouse_inside_w = true end
		local mouse_inside_h = ( mouse_posy < screen_h )
		local mouse_is_infield = (mouse_inside_h and mouse_inside_w)

		local mouse_is_receivable = click_on_mouse and mouse_is_infield
		if  mouse_is_receivable then
			if (self.id == 1) then x_val01 =  2 * (mouse_posx / screen_w) end
			if (self.id == 2) then x_val01 =  2 * ((mouse_posx/(screen_w))-0.5) end
			local y_offset= 0-- 0.05
			y_val01 =  1 -(mouse_posy / screen_h) + y_offset
			--print("self:update: y_val01 ="..x_val01)
		end

	end

--[[ Kinect direct control: to be conbstraint with decor!)
	if (self.control.on_kinect) then
		kinect_x = 0 -- not implemented yet
		kinect_y = 0
		kinect_z = 0
		self.set_xyz( external_x, external_y, external_z )
	end
]]--

--[[ EXTERNAL control: to be conbstraint with decor!)
	if (self.control.on_external) then
	-- if (interface.player[self.id].is_on_external) then
		external_x = 0 -- not implemented this way
		external_y = 0
		external_z = 0
		self.set_xyz( external_x, external_y, external_z )
	end
]]--

	-- Function's External control
	local tab = pong.external_control
	-- local is_uv_receivable = (not(tab.u==nil)) and (not(tab.v==nil))
	local b_uv_receivable = tab and (not(tab.u==nil)) and (not(tab.v==nil))
	-- this tab is created externaly
	-- by the function PONG:m42_set_pos( u, v, d ) [0,1]

	--[[ for Racket[1] only!
	if is_uv_receivable and (self.id == 1) then
		x_val01 = tab.u
		y_val01 = tab.v
		-->> then destroy table elements when used?:
		tab.u = nil
		tab.v = nil
	end ]]--


	--self:print( self.id )
	if b_uv_receivable  then
		local u, v  = tab.u, tab.v
		v = v * 4
	-- for Racket 1 & 2
	---[[
		if (u < 0) and (self.id == 1)	then
			x_val01 = clamp( 1 + u * 3, 0, .8 )
		end
	--]]

	---[[
		self:print( "toto" )
		if (u > 0) and (self.id == 2)	then
			x_val01 = clamp( u * 3, 0, .8 )
		end
	--]]
			--[[toDo ds space.lua?:
			--if d and (d > 0) then	--todo here ?
			--	self.pong:m42_set_pos()
			--else
			self.pong:m42_set_pos( u, v, d )
			--end ]]--

		--	Close to NET LIMITs?:
		--[[
		if (tab.u > 0.5) and (self.id == 1)	then
			x_val01 = 0.95
		end
		if (tab.u < 0.5) and (self.id == 2)	then
			x_val01 = 0.05
		end ]]--

		y_val01 = v + .5

		-- val. check:
		if false then
				self:print("val u,v "..tab.u..", "..tab.u)
				self:print("val01 x,y "..x_val01..", "..y_val01)
			if (tab.d) then
				aaa.print("tab.d="..tab.d)
			end
		end

		-->> then destroy table elements when used:
	end
	-- *** x_val01 & y_val01 adapted values ***

	-- Y Saturation in walls
	local delta_y = self.length
	local extent_y = (wall.top_y - wall.bot_y) - 2 * delta_y
	local y = (y_val01-0.5)* extent_y
	--local y = y_val01

	-- X Saturation in decor length
	local x
	if self.id == 1 then x = (x_val01 * xmax) - xmax
	elseif 	self.id == 2 then x = (x_val01 * xmax)
	end


	--atributes update:
	local offset = -0.3 --probelm with saturation??
	self.y[0] = y + offset
	self.x[0] = x
	--self.length = len

end

function RACKET:draw()
	local racket = self

	local width = racket.width
	local length = racket.length
	local rx = racket.x[0]
	local ry = racket.y[0]
	local rz = racket.z[0]

	gol.set_texture_dim( 0 )
	gol.set_line_width( 1. )
	gol.color_white()

	aaa.draw_rect( rx - width, ry - length, rx + width, ry + length )
end

-- SCORE Class
CLASS.DECLARE( "SCORE" )

function SCORE:create( i, pong )
	local self = SCORE:create_instance( "score_"..i )
	self.pong = pong -- reference parent
	self:init( i )
	return self
end

function SCORE:init( i )

	self.id 	= i

	self.maxval = 15
	self.size 	= 1
	self.c 		= 0 	-- count!
	self.interdigit = 0.3 * self.size

	self.party  = 0


	if 		i == 1 then self.x =-1
	elseif 	i == 2 then self.x = 1
	else 				self.x = 0 -- default
	end

	self.y 		= 1.5

	self:print( "init done!" )

	end

function SCORE:update()
	--todo beurk use self not score
	local score = self
-- RESTART ON 15
	if (score.c > score.maxval ) then
		--todo beurk why these are in PONG
		self.party = score.party + 1
		for i=1, self.pong.n_scores do self.pong.score[i].c = 0 end
	end

	local move = 0.

end

function SCORE:draw()

	local score = self

	local x = score.x
	local y = score.y
	local count = score.c
	local size = score.size
	local interdigit = score.interdigit

	gol.set_texture_dim( 0 )
	gol.set_line_width( 1. )
	gol.color_blue()

	if (count <=9) then
		affich7(count, x, y, size)
	elseif (count >=10) then
		affich7(count, x, y, size)
		local decade = (count-(count%10))/10
		affich7(decade, x - interdigit, y, size)
	end

	local affiche_party = true
	if (affiche_party) then
		gol.color_red()
		affich7(self.party, (x/3)*10, y+0.3, size/2)
	end

end


-- DECOR Class
CLASS.DECLARE( "DECOR" )

function DECOR:create( i )
	local self = DECOR:create_instance( "decor_"..i )
	self:init( i )
	return self
end

function DECOR:init( i )

	self.id = i

	--maa title it is
	self.tittle = "PONG-2012"
	--self.x = 0.
	--self.y = 2.1
	--self.length = 0.5
	--self.width = 0.3

	self.wall = {}
	self.wall.top_y = 2
	self.wall.bot_y = -2
	self.wall.length = 8
	self.wall.width = 0.04

	self.net = {}
	self.net.width = 0.02
	self.net.length = (self.wall.top_y)-(self.wall.bot_y)
	self.net.n_steps = 40
	self.net.period = (self.net.length)/(self.net.n_steps)

	self:print( "init done!" )

end

--[[function DECOR:update()

local decor = self
local move = 0.

end ]]

function DECOR:draw()

	local decor = self
	-- Title
	local tittle =  decor.tittle
	local x = decor.x
	local y = decor.y
	local len =  decor.length
	local wid = decor.width

	--aaa.draw_str_xy("+", 0, 0, .1, .1 ) -- (0,0) Xcross
	--aaa.draw_str_xy(tittle, x-len/2, y-wid/2, len, wid )

	-- Wall
	local top_y = decor.wall.top_y
	local bot_y = decor.wall.bot_y
	local wall_length = decor.wall.length
	local wall_width = decor.wall.width

	gol.set_texture_dim( 0 )
	gol.set_line_width( 1. )
	gol.color_white()

	aaa.draw_rect( 0 - wall_length/2, top_y, 0 + wall_length/2 , top_y + wall_width )
	aaa.draw_rect( 0 - wall_length/2, bot_y, 0 + wall_length/2 , bot_y + wall_width )

	-- Net
	local n_steps = decor.net.n_steps
	local n_len =	decor.net.length
	local n_wid = 	decor.net.width

	for i = 1,(n_steps/2)-1 do
	local ny = (i)* 2 * (n_len/n_steps)  - (n_len/2)
	aaa.draw_rect( -n_wid, ny-(n_len/(2*n_steps)), n_wid, ny+(n_len/(2*n_steps)) )
	end

end


-- IA Class
CLASS.DECLARE( "IA" )

function IA:create( i, pong )
	local self = IA:create_instance( "ia_"..i )
	self.pong = pong -- reference parent
	self:init( i )
	return self
end

function IA:init( i )
	self.id = i

	self.sine_jig = {}
	self.sine_jig.harmonic = {}
	self.sine_jig.harmonic.n_harm = 10
	self.sine_jig.harmonic.n_excite = 1

	-- harmonics init
	for i=0, self.sine_jig.harmonic.n_harm do
		self.sine_jig.harmonic[i]={}
		self.sine_jig.harmonic[i].amp = 0
		self.sine_jig.harmonic[i].freq = 0
		self.sine_jig.harmonic[i].phase = math.random(0, math.pi2 )
	end

		-- manual harmonics values
		self.sine_jig.harmonic[1].amp = 1
		self.sine_jig.harmonic[1].freq = 5
		self.sine_jig.harmonic[2].amp = 1
		self.sine_jig.harmonic[2].freq = 10
		self.sine_jig.harmonic[3].amp = 1
		self.sine_jig.harmonic[3].freq = 15
		self.sine_jig.harmonic[4].amp = 1
		self.sine_jig.harmonic[4].freq = 20
		self.sine_jig.harmonic[5].amp = 1
		self.sine_jig.harmonic[5].freq = 25

		-- random harmonics values
		local freq_step = 5
	for i=1, self.sine_jig.harmonic.n_harm do
		self.sine_jig.harmonic[i].amp = 1 -- math.random(0.5 , 2)
		self.sine_jig.harmonic[i].freq = math.random(i * freq_step - freq_step/2 , i * freq_step + freq_step/2)
		self.sine_jig.harmonic[i].phase = math.random(0 , math.pi2 )
	end

	-- 1/N harmonics values
	--[[for i=1, self.sine_jig.harmonic.n_harm do
		self.sine_jig.harmonic[i].amp = 1/i
		self.sine_jig.harmonic[i].freq = 1*i
		self.sine_jig.harmonic[i].phase = 0*i
	end]]

	self.sine_jig.value01 = 0

	self:print( "init done!" )

end

function IA:update()

	--local ia = self
	local t = aaa.time.t
	local dt = aaa.time.dt

	-- modif behavior with comparative score
	local opposite_player_id
	if 	self.id == 1 then
			opposite_player_id = 2
	elseif 	self.id == 2 then
			opposite_player_id = 1
	end
	local excite_bydiff = ( (self.pong.score[opposite_player_id].c <= self.pong.score[self.id].c) and 1 ) or (self.pong.score[opposite_player_id].c - self.pong.score[self.id].c)
	local excite = math.min (excite_bydiff, self.sine_jig.harmonic.n_harm )

	local opp_tracker = self.pong.racket[opposite_player_id].y[1]
	--normalize with?
	--local delta_y = racket.length
	--local extent_y = (wall.top_y-wall.bot_y) - 2 * delta_y
	--local y = (val_01-0.5)* extent_y
	local ball_y = self.pong.ball[1].y[1]
	local tracker01 = (ball_y + opp_tracker/2)
	local tracker02 = (ball_y + opp_tracker/5)

	-- IA get here a multi_sin(t) movmt between [0 1]
	 -->> to be converted into 3D spherical harmonics!
	local amps_sum = 0
	local harmonics_sum = 0
	local phase_drift = t
	for i=1, excite do
		local amp = self.sine_jig.harmonic[i].amp
		local freq = self.sine_jig.harmonic[i].freq
		local phase = self.sine_jig.harmonic[i].phase + phase_drift
		--print("ia:update: amp ="..amp)
		amps_sum = amps_sum + amp
		harmonics_sum = harmonics_sum + amp * math.cos( (freq*t) + phase )
	end
	--print("ia:update: amps_sum ="..amps_sum)
	--print("ia:update: harmonics_sum ="..harmonics_sum)
	local ia_jig01 = ((amps_sum==0) and 0 ) or ((harmonics_sum /(2*(amps_sum))) + 0.5)

	-- IA follows tracker with a saturated movement between [0 & 1]
	local keep_center_idx = 1/excite -- 1 -- 0.1
	local in_min = 0  - keep_center_idx
	local in_max = 1  + keep_center_idx
	local out_min = 0
	local out_max = 1
	local sat_power = 0.5
	local ia_mov = (ia_jig01 + tracker01)
	--print("ia:update: ia_jig01 ="..ia_jig01)
	--print("ia:update: tracker01 ="..tracker01)
	local ia_jig_sat = saturation(ia_mov,in_min, in_max, out_min, out_max, sat_power)

	-- update atributes:
	local border_like_factor = 1 + (ball_y/40) +0.05
	self.sine_jig.value01 = ((ia_jig_sat) * border_like_factor) or 0
	self.sine_jig.harmonic.n_excite = excite

end

--[[function IA:draw()

local ia = self
local move = 0.

end]]


--1/ Create PONG Class
CLASS.DECLARE( "PONG", nil, {a = 2} )

function PONG:create( name )
	local self = PONG:create_instance( name )
	self:init()
	return self
end


--2/ INIT Pong ELEMENTS
function PONG:init()

	self.n_dynsteps = 10 -- number of steps for time(frames) integration

	self.n_balls = 1
	self.n_rackets = 2

	self.n_scores = self.n_rackets
	self.n_decors = 1
	self.n_interfaces = 1

	self:create_balls()		--	self.create_balls( self )
	--maa ce serait plus sense de faire create_rackets ( nb ) et ainsi de virer les lignes au dessus
	--	aussi si struct rackets il y a c'est a elle de porter le nom de racket
	self:create_rackets()
	self:create_scores()
	self:create_decors()

	self:create_ia()

--todo
	-- moved in pong_app.lua:
	-- create_interface()

end

--3/ Pong OBJECTs Instanciation (Call4 constructors)
function PONG:create_balls()
	self.ball = {}
	local ball = self.ball
	for i=1,self.n_balls do
		ball[i] = BALL:create( i, self )
	end
end

function PONG:create_rackets()
	self.racket = {}
	local racket = self.racket
	for i=1,self.n_rackets do
		racket[i] = RACKET:create( i, self )
	end
end

function PONG:create_scores()
	self.score = {}
	local score = self.score
	for i=1,self.n_scores do
		score[i] = SCORE:create( i, self )
	end
end

function PONG:create_decors()
	self.decor = DECOR:create( 1, self )

	--case many decors
	--[[self.decor = {}
	local decor = self.decor
	for i=1,self.n_decors do
		decor[i] = DECOR:create( i )
	end]]
end

function PONG:create_ia()
	self.ia = {}
	local ia = self.ia
	for i=1,self.n_rackets do
		ia[i] = IA:create( i, self )
	end
end

function PONG:m42_set_pos( u, v, d )
		-- push external positions values (kinect)
		-- to be used by racket:update
		--(RQ: nil values if no external control sent!)
		self.external_control = {}
		self.external_control.u = u
		self.external_control.v = v
		self.external_control.d = d

		-->> u, v, values in [0, 1]
		-- for [Left, Right], [Bottom, Top], [Rear, Front]

end


-- OBJECTs UPDATE & DRAW
function PONG:update_and_draw()

	PONG.cur = self -- current refence, tobe used by child struct as a link to this parent

	for i=1,self.n_balls do

		dyn_shift( self.ball[i], self.n_dynsteps )
		self.ball[i]:update()
		self.ball[i]:draw()

		--[[ Monitor:
		local monitor_val = self.ball[i]
		local monitor_var = 'ball'
		-- PRINT ON EXT. MONITOR WINDOW
		print( "Followinw frame: " )
		for j=0,self .n_dynsteps do
		print( monitor_var.."["..i.."].dx["..j.."]="..monitor_val.dx[j] )
		end
		]]

	end

	for i=1, self.n_rackets do

		dyn_shift(self.racket[i], self.n_dynsteps)

		self.racket[i]:update()
		self.score[i]:update()
		self.ia[i]:update()

		self.racket[i]:draw()
		self.score[i]:draw()
	end

	--self.decor:update()
	self.decor:draw()

	--self:draw_monitor()
	self.external_control = nil
end

function PONG:draw_monitor()

	local char_size = 0.08
	local startx = -3.8
	local deltax = 0.7
	local starty = 1.7
	local deltay = - 0.15

	-- MONITORED VALUES

	--[[ --ball velocity
	local val= self.ball[1].dx[0]
	aaa.draw_str_xy( "ball.dx[0]= "..(val-val%0.1), startx, starty, char_size, char_size )
	local val= self.ball[1].dy[0]
	aaa.draw_str_xy( "ball.dy[0]= "..(val-val%0.1), startx, starty+deltay, char_size, char_size )

	local val= self.ball[1].dx[1]
	aaa.draw_str_xy( "ball.dx[1]= "..(val-val%0.1), startx+deltax, starty, char_size, char_size )
	local val= self.ball[1].dy[1]
	aaa.draw_str_xy( "ball.dy[1]= "..(val-val%0.1), startx+deltax, starty+deltay, char_size, char_size )

	local val= self.ball[1].dx[2]
	aaa.draw_str_xy( "ball.dx[2]= "..(val-val%0.1), startx+2*deltax, starty, char_size, char_size )
	local val= self.ball[1].dy[2]
	aaa.draw_str_xy( "ball.dy[2]= "..(val-val%0.1), startx+2*deltax, starty+deltay, char_size, char_size )
	]]

	--local val= self.racket[2].y[1]
	--aaa.draw_str_xy( "racket[2].y[1] = "..(val-val%0.001), startx, starty +2*deltay, char_size, char_si

	local val= self.ball[1].dx[0]
	aaa.draw_str_xy( "ball.dx[0]= "..(val-val%0.1), startx, starty, char_size, char_size )
	local val= self.ball[1].dy[0]
	aaa.draw_str_xy( "ball.dy[0]= "..(val-val%0.1), startx, starty+deltay, char_size, char_size )

	-- IA: move
	local val= self.ia[1].sine_jig.value01
	aaa.draw_str_xy( "IA.1 value01= "..(val-val%0.001), startx,starty+ 2* deltay, char_size, char_size )

	-- IA: excite level
	local val = self.ia[1].sine_jig.harmonic.n_excite
	aaa.draw_str_xy( "IA.1 Excite Level = "..(val), startx, starty + 3 * deltay, char_size, char_size )

	local Ystep=3
	local nexcite = val
		for i=1, nexcite do
		local amp = self.ia[1].sine_jig.harmonic[i].amp
		local freq = self.ia[1].sine_jig.harmonic[i].freq
		local phase = self.ia[1].sine_jig.harmonic[i].phase

	aaa.draw_str_xy( "H.level["..i.."] Amp="..(amp), startx, starty + (i+Ystep) * deltay, char_size, char_size )
	--aaa.draw_str_xy( "Amp="..(amp), startx, starty + (i+Ystep+1) * deltay, char_size, char_size )
	aaa.draw_str_xy( "Freq="..(freq), startx + 1 * deltax, starty + (i+Ystep) * deltay, char_size, char_size )
	aaa.draw_str_xy( "Phase="..(phase), startx + 1.5 *deltax, starty + (i+Ystep) * deltay, char_size, char_size )

		--print("ia:update: amp ="..amp)
		--amps_sum = amps_sum + amp
		--harmonics_sum = harmonics_sum + amp * math.cos( (freq*t) + phase )
	end
end

CLASS.DECLARE( "SPACE_PONG", SPACE )

function SPACE_PONG:create( name )
	local self = SPACE_PONG:create_instance( name )
	self.pong = PONG:create( "m42" )
	return self
end

function SPACE_PONG:draw()
	local u, v, d = m42.skel:get_anal_data()
	if d<.6 then	--todo here ?
		self.pong:m42_set_pos()
	else
		self.pong:m42_set_pos( u, v, d )
	end
	gol.push_matrix()
		gol.translate( 0, 1.85 )
		gol.rotate_x(.5)
		gol.scale(.8)
		self.pong:update_and_draw()
	gol.pop_matrix()
end


