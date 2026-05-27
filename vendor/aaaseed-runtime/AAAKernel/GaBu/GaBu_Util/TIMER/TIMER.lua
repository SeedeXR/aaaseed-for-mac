--
--	TIMER
--

if CLASS.DECLARE( "TIMER", nil ) then
	TIMER:set_class_status_doc(	CLASS.STATUS.CORE,
								"TIMER measure time from a begin call"  )
end

function TIMER.init_instance(self)
	oo.getsuper(TIMER).init_instance( self )
	return self
end
function TIMER:create( name )
	local self = TIMER:create_instance( name )
	self.last = 0
	return self
end

function TIMER:start()
	self.time_begin = aaa.time.get_time_now()
end
function TIMER:get_now()
	return aaa.time.get_time_now() - self.time_begin
end
function TIMER:store()
	self.last = aaa.time.get_time_now() - self.time_begin
end
function TIMER:get_ms_str()
	local nb = math.floor( self.last * 1000000 ) / 1000
	--nb = 1
	local str = string.format( "%3.3f", nb )	--todo make it better 3 significant digit
	return str .. "ms"
end
local function make_string( nb )
	local str
	if nb > 99		then	return tostring(nb)
	elseif nb > 9	then	return "  "..nb
	else					return "    "..nb
	end
end
local function make_string_3( nb )
	local str
	if nb > 99		then	return tostring(nb)
	elseif nb > 9	then	return "0"..nb
	else					return "00"..nb
	end
end
function TIMER:get_micro_sec_str()
	local str
	local nb = math.floor( self.last * 1000000 )
	if nb < 1000 then
		--str = "   "..make_string( nb )
		str = tostring(nb)
		return str  .." μs"
	else
		local top =  math.floor( nb*.001 )
		str = tostring(top) .. " " .. make_string_3( nb - top * 1000 )
		return str  .." μs"
	end
end	

