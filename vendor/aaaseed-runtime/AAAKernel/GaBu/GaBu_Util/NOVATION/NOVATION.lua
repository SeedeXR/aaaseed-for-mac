--test

if CLASS.DECLARE( "NOVATION" ) then
	NOVATION:set_class_status_doc(	CLASS.STATUS.GABU,
									"Novation controller interface" )
end

--	how it is indexed
--	 x 1 2 3 4 5 6 7 8  9
--	y
--	0  o o o o o o o o
--	1  x x x x x x x x  offn
--	2  x x x x x x x x  o
--	3  x x x x x x x x  o
--	4  x x x x x x x x  o
--	5  x x x x x x x x  o
--	6  x x x x x x x x  o
--	7  x x x x x x x x  o
--	8  x x x x x x x x  o
--

-- state is the state of the led
-- sqaure is about what us touched 

function NOVATION:create_square()
	local data = {}
	for x=1,9 do
		data[x] = {}
		for y=0,8 do
			data[x][y] = { on=0, last=0 }
			if y~=0 then
				data[x][y].b_switch = true
			end
		end
	end
	return data
end


function NOVATION:init_group()
	self.group = {}
end
function NOVATION:define_group( id, xmin, ymin, xmax, ymax )
	if not self.group then  self:init_group() end
	self.group[id] = { xmin = xmin, ymin = ymin, xmax = xmax, ymax = ymax }
	self:__set_group( self.group[id], xmin, ymin )
end
function NOVATION:get_first_group( x, y )
	--aaa.print( "NOVATION:get_first_group( "..x..", "..y.." )" )

	for k, g in pairs(self.group) do
		--aaa.print( "g" )
		--table.print( g, "group" )
		if g.xmin<=x and x<=g.xmax and g.ymin<=y and y<=g.ymax then
			--aaa.print( "found group" )
			return g
		end
	end
	return nil
end
function NOVATION:erase_group( g )
	for x=g.xmin,g.xmax do
		for y=g.ymin,g.ymax do
			self:do_switch_low( self.square[x][y], x,y, 0 )
		end
	end
end
function NOVATION:__set_group( g, x,y )
	self:erase_group(g)
	g.value = (x+1-g.xmin) + (y-g.ymin)*(g.xmax-g.xmin+1)
	aaa.print( "group value is now "..g.value )
	self:do_switch_low( self.square[x][y], x,y, 1 ) --was self.state[x][y] but Claude protest
end
function NOVATION:get_group_value( id )
	return self.group[id].value
end

function NOVATION:create( obj_midi_name )
	aaa.print_method()
	local self = self or NOVATION:create_instance_no_name()
	self.state = {}
	self.square = self:create_square()
	self.group = {}
	for x=1,9 do
		self.state[x] = {}
		for y=0,8 do
			self.state[x][y] = { red=0, green=0, on=0, last=0 }
		end
	end
	self.midi_obj = aaa.obj.get_no_error( obj_midi_name )
	self:reset_pad()
	return self
end
function NOVATION:is_valid_row_col( x, y )
	if x<1 or 9<x then return false end
	if y<0 or 8<y then return false end
	return true
end
function NOVATION:__set_led_low( x, y, red, green )
	if not self.midi_obj then return end
	--self:print( x.." "..y.." -> "..red.." "..green )
	local v = green*16 + red
	if y > 0 then
		local note = (y-1) * 16 + x
		aaa.midi.send_note_on( self.midi_obj, 1, note, v )
	else
		aaa.midi.send_control_change( self.midi_obj, 1, 104 + x , v )
	end
end
function NOVATION:set_led( x,y, red, green )
	if not self:is_valid_row_col( x,y ) then return end
	if self.verbose >= 2 then aaa.print_method() end
	local elt = self.state[x][y]
	red = math.floor( clamp( (red or 0)*4., 0, 3 ) )
	green = math.floor( clamp( (green or 0)*4., 0, 3 ) )
	if elt.red == red and elt.green == green then return end
	elt.red = red
	elt.green = green
	self:__set_led_low( x,y, red, green )
end
function NOVATION:is_on( x,y )
	if not self:is_valid_row_col( x,y ) then return end
	--aaa.print( "NOVATION:is_on( "..x..", "..y.." ) -> "..self.square[x][y].on )
	return self.square[x][y].on == 1
end
--todo add check and message : is_ valid adresse
function NOVATION:set_trig( x,y )
	if not self:is_valid_row_col( x,y ) then return end
	self.square[x][y].b_switch = false
end
function NOVATION:set_switch( x,y )
	if not self:is_valid_row_col( x,y ) then return end
	self.square[x][y].b_switch = true
end

--todo clean this
function NOVATION:set_elt_low( elt, x,y, val )
	elt.on = (val == 0) and 0 or 1
	self:set_led( x,y, elt.on, elt.on )
end
function NOVATION:set_elt_xy_low( x,y, val )
	local elt = self.square[x][y]
	self:set_elt_low( elt, x,y, val )
end
function NOVATION:do_switch_low( elt, x,y, val )
	self:set_elt_low( elt, x,y, val )
	local fn = self:get_fn_switch( x,y )
	if fn then
		fn( elt.on )
	end
	local target, method, a1, a2 = self:get_method_switch( x,y )
	if target then
		target[method]( target, elt.on, a1, a2 )
	end
end
function NOVATION:do_switch( elt, x,y, val )
	if self.verbose >= 1 then self:print( "switch novation "..x.." "..y.." to "..val ) end
	--deal with exclusive group
	local g = self:get_first_group( x,y )
	if g then
		if val > 0 then
			aaa.print( val )
			self:__set_group( g, x,y )
			return
		else
			if elt.on==1 then return end
			aaa.print( "g" )
		end
	end
	self:do_switch_low( elt, x,y, val )
end
function NOVATION:do_touch( x,y, b_on )
	local elt = self.square[x][y]

	if b_on~=0 then
		--aaa.print( x,y )
		if elt.b_switch == true then
			if elt.last==0 then
				elt.last = 1
				self:do_switch( elt, x,y,  1 - elt.on )
			end
		else
			if elt.last==0 then
				elt.last=1
				self:do_switch( elt, x,y, 1 )
			end
		end
	else
		if elt.b_switch == true then
			if elt.last==1 then
				if self.verbose >= 2 then self:print( "novation "..x.." "..y.." "..b_on ) end
			end
		else
			if elt.last==1 then
				self:do_switch( elt, x,y, 0  )
			end
		end
		elt.last = 0
	end
end
function NOVATION:update()
	for x=1,9 do
		for y=0,8 do
			if y > 0 then
				self:do_touch( x, y, aaa.midi.get_velocity( 1, (y-1)*16 + x ) )
			else
				self:do_touch( x, y, aaa.midi.get_control( 1, 105 + x - 1 ) )
			end
		end
	end
	return self
end

function NOVATION:reset_pad()
	for y=1,8 do
		for x=1,8 do
			self:set_led( x, y, 0, 0 )
			self:__set_led_low( x, y, 0, 0 )
		end
	end
	--aaa.midi.send_note_on( self.midi_obj, 1, note, red*16 + green )
end

function NOVATION:set_function_switch( x,y, fn )
	self.square[x][y].fn_switch = fn
	--self.square[x][y].fn_arg = arg
end
function NOVATION:get_fn_switch( x,y )
	return self.square[x][y].fn_switch
end
function NOVATION:set_method_switch( x,y, target, method, a1, a2 )
	self.square[x][y].method_switch = { target=target, method=method, a1=a1, a2=a2 }
end
function NOVATION:get_method_switch( x,y, a1, a2 )
	local t = self.square[x][y].method_switch
	if t then
		return t.target, t.method, t.a1, t.a2
	end
end

function NOVATION:TEST_COLOR( vx, vy )
	for y=1,8 do
		for x=1,8 do
			self:set_led( x, y, vx or ((x-1)/8), vy or ((y-1)/8) )
		end
	end
end

local b_need_clean
function NOVATION.TEST()
	local t = aaa.time.t * 10

	local function clean()
		for x=1,8 do
			for y=1,8 do
				if not novation:is_on( x, y ) then
					novation:set_led( x, y, 0, 0 )
				end
			end
		end
		b_need_clean = false
	end
	local function loop_line( y, red, green, speed )
		local speed = speed or (y/27)
		--aaa.print( speed )
		local t = math.floor( math.fmod( t * speed, 9 ) ) + 1
		local nb = y<=0 and 8 or 9
		--print( nb )
		for x=1,8 do
			if not novation:is_on( x, y ) then
				if x == t then
					novation:set_led( x, y, red, green )
				else
					novation:set_led( x, y, 0, 0 )
				end
			end
		end
		b_need_clean = true
	end
	local function chenillard()
--		aaa.print_fn()
		loop_line( 1, 1, 0 )
		loop_line( 2, 1, 1/3 )
		loop_line( 3, 1, 2/3 )
		loop_line( 4, 1, 1 )
		loop_line( 5, 2/3, 1 )
		loop_line( 6, 1/3, 1 )
		loop_line( 7, 0, 1 )
		loop_line( 8, 0, 1 )
	end

	local tab = {	{0,0}, {0,0}, {1,0}, {2,0}, {3,0}, {3,0}, {2,0}, {1,0},
					{0,0}, {0,0}, {1,1}, {2,2}, {3,3}, {3,3}, {2,2}, {1,1},
					{0,0}, {0,0}, {0,1}, {0,2}, {0,3}, {0,3}, {0,2}, {0,1},
					{0,0}, {0,0}, {1,1}, {2,2}, {3,3}, {3,3}, {2,2}, {1,1},
				}
	local function all_color( speed )
		--aaa.print( speed )
		local t = t * speed
		for ix=1,4 do
			local x = 4.5 - ix
			x = x * x
			for iy=1,4 do
				local y = 4.5 - iy
				local id = 32 - math.floor( math.fmod( t - math.sqrt(x+y*y) * 2, 32 ) )
				local col = tab[ id ]
				local r,g = col[1]*.3333, col[2]*.3333
				if not novation:is_on( ix, iy ) then novation:set_led( ix, iy, r, g ) end
				ix = 9 -ix
				if not novation:is_on( ix, iy ) then novation:set_led( ix, iy, r, g ) end
				iy = 9 -iy
				if not novation:is_on( ix, iy ) then novation:set_led( ix, iy, r, g ) end
				ix = 9 -ix
				if not novation:is_on( ix, iy ) then novation:set_led( ix, iy, r, g ) end
			end
		end
		b_need_clean = true
	end

	novation:update()
	if novation:is_on( 9, 8 ) then
		--chenillard()
		all_color(.5)
	else
		if b_need_clean then
			clean()
		end
	end

	if novation:is_on( 6, 0 ) then
		novation:TEST_COLOR( aaa.time.t % 1. , (aaa.time.t * .25) % 1. )
	end
	loop_line( 0, 3, 0, .5 )
	--novation:set_switch( 7, 0 )
	--
	--local t = aaa.midi.get_velocity( 1, 0 )
	--aa.print(t)
	--novation:set_led( 1, 1, 0, 3 )
end

if aaa.lua.global.declare_table( "novation" ) then
	novation = NOVATION:create( aaa.dir.get_dir_tracker().."/default_a.midipref" )
end

local function test_change_color( on )
	aaa.print( "test_change_color "..on )
end

NOVATION.verbose = 1
novation:set_function_switch( 8, 0, test_change_color )
