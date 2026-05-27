
function meu:define_ui()
	local ref = self.ref
	local bu
	local par


	self:add_camera()

	local iy = 1
	local ix = 1
	local sy = 1
	local bu

	bu = self:add_button( {ix, iy, sy, sy },	"Draw", self:get_layer_ref_table(2).use_bdd )
		bu:set_min_max( 3, 2 )
	iy = iy + sy
	bu = self:add_button(	{	ix+1, iy, sy, sy },	"Draw_force", ref.b_draw_force )

	iy = iy + sy
	bu = self:add_trig(	{	ix, iy,	 sy, sy},		"Restart", ref.restart_trig )

	iy = iy + sy
	bu = self:add_button(	{	ix, iy, nil, sy },	"Field", ref.field_active )

	iy = iy + sy
	bu = self:add_button(	{	ix+1, iy, sy, sy },	"Doors", ref.dp_active )

	iy = iy + sy
	bu = self:add_button(	{	ix+1, iy, sy, sy },	"Floor", ref.floor_active )

	iy = iy + sy*2
	bu = self:add_button(	{	ix, iy, sy, sy },		"Run", self, "b_run" )
	iy = iy + sy
	bu = self:add_slider(	{ix+1,iy,	7,sy},	"Open Duration", self, "open_duration", 1, 0,4 )
	iy = iy + sy
	bu = self:add_slider(	{ix+1,iy,	7,sy},	"Close Duration", self, "close_duration", 1, 0,4 )
	iy = iy + sy
	bu = self:add_slider(	{ix+1,iy,	7,sy},	"Close Delay", self, "close_delay", 0, 0,32 )
	iy = iy + sy
	bu = self:add_button(	{	ix+1, iy, sy, sy },	"Open", self, "b_open", false, 0,1 )

	ix,iy = 9,3
	self.vol = {}
	for i=1,5 do
		bu = self:add_slider(	{ix+1,iy,	3,sy},	"Vol_"..i, self.vol, i, 0, 0,1 )
		iy = iy + sy
	end
end

function meu:init()
	local ref = self.ref

	ref.bdd					=	self:get_layer_bdd( 2 )
	ref.restart_trig		=	param.get_ref( ref.bdd, "restart_trig" )
	ref.field_active		=	param.get_ref( ref.bdd, "field_active" )
	ref.repulse_dist		=	param.get_ref( ref.bdd, "repulse_distance" )
	ref.b_steering			=	param.get_ref( ref.bdd, "steering_active" )
	ref.b_draw_force		=	param.get_ref( ref.bdd, "draw_force" )
	ref.repulse_alpha		=	param.get_ref( ref.bdd, "repulse_alpha" )

	local lay = self:get_layer(2)

	local dp = aaa.obj.get_down( lay, "Doors" )
	ref.def_dp = dp
	ref.dp_active = param.get_ref( dp, "active" )

	local floor = aaa.obj.get_down( lay, "Floor" )
	ref.def_floor = floor
	ref.floor_active = param.get_ref( floor, "active" )

	local color = self:get_obj_down( "color" )
	ref.alpha = param.get_ref( color, "global_alpha" )

	self.close_time = 0
	self.b_close = true
	self.opening_dir = nil
	self.opening = 0
end

function meu:get_meu_anal( post )
	local name = "AnalDP"
	if post then name = name.."_"..post end
	local m = self:get_meu_by_name_no_error( name )
	return m
end
function meu:get_meu_sound( post )
	local name = "Sound"
	if post then name = name.." "..post end
	local m = self:get_meu_by_name_no_error( name )
	return m
end
function meu:get_coverage( post )
	local m = self:get_meu_anal( post )
	local c, u, v
	if m then
		c = m:get_bu_value( "coverage" )
		u = m:get_bu_value( "Center u" )
		v = m:get_bu_value( "Center v" )
	else
		c = 0
	end
	return c, u, v
end
function meu:set_volume( post, vol )
	local m = self:get_meu_sound( post )
	if m then
		m:set_bu_value( "volume", .3 + vol * .7 )
		local video = m:get_video()
	--video:set_pan( 0 )
	end
end
function meu:restart_snd( post, vol )
	local m = self:get_meu_sound( post )
	if m then
		m:restart( )
	end
end
function meu:set_time_factor( post, val )
	local m = self:get_meu_sound( post )
	if m then
		local video = m:get_video()
		video:set_time_factor( val )
	end
end
-- function meu:set_pan( post, val )
-- 	local m = self:get_meu_sound( post )
-- 	if m then
-- 		local video = m:get_video()
-- 		video:set_pan( val )
-- 	end
-- end
local vol_max = { .817, .691, .658, .784, .62 }
function meu:set_vol_activity( vol )
	if false then
		for i=1,5 do
			self:set_volume( string.char(48+i), self.vol[i] )
		end
	else
		for i=1,5 do
			local v = 0
			if self.main_snd_id == i then
				v = vol
			end
			self:set_volume( string.char(48+i), v * vol_max[i] )
		end
	end
end
function meu:do_open( )
	self.close_time = 0
	self:restart_snd( "B" )
	self:set_volume( "B", .65 )
	self.opening = 0
	self.opening_dir = .5
	self.b_close = false
	self.b_open = false
	self.main_snd_id = math.random(5)
end
function meu:do_close()
	self:restart_snd( "C" )
	self:set_volume( "C", .65	 )
	self.opening = 1
	self.opening_dir = -.33
	self.b_close = false
	self.b_open = false
end
function meu:do_buzz()
	local delay = self.delay or .1
	if math.random() < delay then
		local ref = self.ref
		local si = math.random()*.8
		self:set_volume( "A", si )
		param.set( ref.alpha, .2 + si  )
		param.set( ref.repulse_alpha,  si *.25 )
		local r = math.pow( math.random(), .2 )
	end
end
function meu:check_for_fullscreen()
	local ref = self.ref
	local screen_nb = aaa.screen.get_nb()
	if screen_nb ~= self.screen_nb then
		self:print( "screen nb changed to "..screen_nb )
		self.screen_nb = screen_nb
		if screen_nb == 2 then
			self:print( "Asking to Trigger FullScreen soon" )
			self.__fullscreen_time = 0.
		end
	end
	local d = self.__fullscreen_time
	if d then
		d = d + aaa.time.dt
		if d > 4. then
			self:print( "Trigger FullScreen NOW" )
			ga:force_fullscreen()
			self.__fullscreen_time = nil
		else
			self.__fullscreen_time = d
		end
	end
end
function meu:update()

	self:check_for_fullscreen()

	local ref = self.ref

	local u,v
	if self.b_run then
		aaa.show( self.b_close, "b_close" )
		aaa.show( self.b_open, "b_open" )
		aaa.show( self.opening, "opening" )
		if self.b_open then
			local cl
			cl,u,v = self:get_coverage( "FullL" )
			local cr = self:get_coverage( "FullR" )
			--aaa.show( c, "Coverage" )
			if cl and cr and cl < .0625 and cr < .0625 then
				self.close_time = self.close_time + aaa.time.dt
				if self.close_time >= self.close_delay then
					self:do_close()
				end
			else
				self.close_time = math.max( self.close_time - aaa.time.dt, 0. )
			end
			param.set( ref.repulse_alpha,  .75  )
		end
		if self.b_close then
			local c
			c = self:get_coverage( "DoorL" )
			aaa.show( c, "Cov_L" )
			if c and c > .025 then
				self:do_open( -1 )
			end
			c = self:get_coverage( "DoorR" )
			aaa.show( c, "Cov_R" )
			if c and c > .025 then
				self:do_open( 1 )
			end
			self:do_buzz()
		else
			param.set( ref.alpha, 1.  )
			self:set_volume( "A", 0 )
		end
	end

	local open = self.opening or 0
	if self.opening_dir then
		local dt = self.opening_dir * aaa.time.dt
		open = open + dt
		if open < 0 then
			self.b_close = true
			self.opening_dir = nil
		elseif 1 < open then
			self.b_open = true
			self.opening_dir = nil
		end
		self.opening = clamp_01( open )
	end

	--self:set_volume( "1", open * .8 )
	self:set_vol_activity( open )
	local b_doors = param.get_bool( ref.dp_active )
	local rep = .042
	local b_ster = true
	if self.b_close or (self.opening_dir and (self.opening_dir < 0) ) then
		rep = .002
		b_ster = false
	end
	param.set( ref.repulse_dist, rep )
	param.set( ref.b_steering,  b_ster )
	param.set( ref.dp_active, not b_ster )

	self.u = self.u or 1
	u = u or 1
--	u = interpolate( self.u, u, .1 )
--	self.u = u
--	self:set_time_factor( "A", u )
--	local sp = math.abs( u - self.u ) * 30
--	self:set_pan( "A", 0 )
--	self:set_volume( "A", sp)
--	self.u = u
end
--[[
function poid.hook_do_contact( a, b )
	--aaa.print( a.." - "..b )
	return 1.
end
--]]
--poid.hook_do_contact = nil
