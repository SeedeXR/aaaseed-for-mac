
function MEU:define_time( rect, time_max, b_save_time )
	b_save_time = b_save_time==nil and true or b_save_time
	local ix,iy, SX,SY = unpack(rect) 
	SX = SX * 8 / 3

	local bu
	local t = { time=0, time_max=time_max }
	local f = 1
	self:add_button(		{ix,		iy,		SY*f, SY*f},	"Play",			t, "b_play",		false )
	self:add_button(		{ix+2.5,	iy,		SY*f, SY*f},	"Loop",			t, "b_loop",		false )

	self:add_trig_method(	{ix+SX*2,	iy,		SX, SY},		"Restart", 		self, "restart_time" )
--		:set_color_back( "Restart" )
	t.bu_info =
		self:add_text_info(	{ix+3,		iy+SY*.35,	3,SY*.65},	"Status" )

	iy = iy + SY
	--todo the 60 (1 min) is a quick solution
	--todo chack that time will not overflow glsl float
	t.bu_slider_time =
		self:add_slider(	{ix,		iy,		4,	SY},		"Time"			)
		:set_min_max( 0, time_max or 60 )
		:set_target_lua( t, "time" )
		:set_value_load_save( false );
	t.bu_slider_speed =
		self:add_slider(	{ix+4,		iy,		4,	SY},		"Speed"			):set_min_max( 0, 1 ):set_target_lua( t, "speed" )
	iy = iy + SY
	self:add_button(		{ix,	iy+SY*.1,	SY*.8, SY*.8},	"Inverse",		t, "b_inverse", false ):set_text("-"):set_text_rect_ratio(1.5)
	bu = self:add_selector(	{ix+1.5,	iy,		6.5,SY},		"time_factor"	)
		bu:set_nb(7)
		bu:set_item_text( 1, "/16", "/4", "No", "x1", "x4", "x16", "x64" )
		bu:set_item_data( 1, 1/16, 1/4, 0, 1, 4, 16, 64 )
		bu:set_value(4)
		bu:set_text_visible( false )
		t.bu_factor = bu
	iy = iy + SY
	self.__time = t
	return iy
end
function MEU:restart_time()
	local t = self.__time
	if t then
		t.time = 0.0
	end
end
function MEU:time_play( b )
	local t = self.__time
	if t then
		t.b_play = b
	end
end
function MEU:set_time_max( val )
	local t = self.__time
	if t then
	--	self:print( "set_time_max "..val)
		t.time_max = val
		t.bu_slider_time:set_min_max( 0, val )
	--	self:print( "t.time_max "..t.time)
	end
end 
function MEU:set_time_speed( val )
	local t = self.__time
	if t then
		t.speed = val
		t.bu_slider_speed:set_min_max( 0, val )
	end
end
function MEU:get_time_speed( val )
	local t = self.__time
	if t then
		return t.speed
	end
	return 1.0
end

function MEU:set_time_looping( b )
	local t = self.__time
	if t then
		t.b_loop = b
	end
end

function MEU:is_time_looping()
	local t = self.__time
	if t then
		return t.b_loop
	end
	return false
end

function MEU:update_time_ui()
	local t = self.__time
	if t then
--		local time_str =  string.format( "Time : %.2f", self.time )
		t.bu_info:set_text( t.b_play and "Playing" or "Stopped" )
	end
end

function MEU:update_time()
	local t = self.__time
	if t then
		local lt = t.time
	--	self:print( "lt "..type( t.time ))
		if t.b_play then
			local f = t.bu_factor:get_item_data() * t.speed
		--	self:print( "f "..f)
		--	self:print( "t.speed "..t.speed)
			if t.b_inverse then f = -f end
			lt = lt + aaa.time.dt * f
			if t.time_max then
				if t.b_loop then
					lt = wrap( lt, 0, t.time_max )
				elseif lt > t.time_max then
					lt = t.time_max
				end
			end
		--	self:print( "update time "..lt)
			t.time = lt
			return lt
		end
	end
end