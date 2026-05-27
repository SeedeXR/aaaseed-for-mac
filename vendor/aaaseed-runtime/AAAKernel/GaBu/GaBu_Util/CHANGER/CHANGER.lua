if CLASS.DECLARE( "CHANGER" ) then
	CHANGER:set_class_status_doc(	CLASS.STATUS.CORE,
									"CHANGERS deal with them" )
end

function CHANGER:create( name, ... )
	local self = CHANGER:create_instance( name )
	self:set( ... )
	return self
end
function CHANGER:set( time_offset, time_len, target, method, start, stop  )
	self.target			=	target
	self.method			=	method
	self.start			=	start
	self.stop			=	stop
	self.time_len		=	time_len
	self.factor			=	1. / time_len
	self.start_time		=	aaa.time.t + time_offset
end
--todo	deal with phase and also offset
function CHANGER:change()
	local	t = aaa.time.t
	if t <= self.start_time then
		return false
	end
	if not self.start then
		--	we assume a get_method as the set_method
		local get_method_name = "g"..self.method:sub(2)
		self:print( get_method_name )
		self.start = self.target[ get_method_name ]( self.target )
	end
	local	ph		=	(t - self.start_time) 	*	self.factor
	local	val
	local b_done
	if ph >= 1. then
		val = self.stop
		b_done = true
	else
		if self.gain then ph = aaa.math.gain( ph, self.gain ) end
		if self.bias then ph = aaa.math.bias( ph, self.bias ) end
		val = interpolate( self.start, self.stop, ph )
		b_done = false
	end
	--self:print( "ph "..ph )
	self.target[ self.method ]( self.target, val )
	return b_done
end
function CHANGER:set_gain_bias( gain, bias )
	self.gain = gain
	self.bias = bias
end

if CLASS.DECLARE( "TCHANGER" ) then
	TCHANGER:set_class_status_doc(	CLASS.STATUS.CORE,	"CHANGERS run them" )
end

function TCHANGER:create( name, ... )
	local self = TCHANGER:create_instance( name )
	self:set( ... )
	return self
end
function TCHANGER:set( time_offset, target, method, arg1, arg2, arg3, arg4 )
	self.target			=	target
	self.method			=	method
	self.arg1			=	arg1
	self.arg2			=	arg2
	self.arg3			=	arg3
	self.arg4			=	arg4
	self.start_time		=	aaa.time.t + ( time_offset or 0 )
end
--todo	deal with phase and also offset
function TCHANGER:change()
	local	t = aaa.time.t
	if t <= self.start_time then
		return false
	end
	self.target[ self.method ]( self.target, self.arg1, self.arg2, self.arg3, self.arg4 )
	return true
end

if CLASS.DECLARE( "CHANGERS" ) then
	CHANGERS:set_class_status_doc(	CLASS.STATUS.CORE,	"designed to create pool of CHANGER and TCHANGER, and update them" )
end

function CHANGERS:create( name )
	local self = CHANGERS:create_instance( name )
	self.__changer_id = 0
	self.b_no_delay = false
	return self
end
function CHANGERS:is_empty()
	return self.head == nil
end
function CHANGERS:set_no_delay( b )	self.b_no_delay = b end
function CHANGERS:add( time_offset, time_len, target, method, start, stop )
	if self.b_no_delay then
		target[ method ]( target, stop )
	else
		local id = self.__changer_id + 1
		self.__changer_id = id
		local ch = CHANGER:create( self:get_name()..id, time_offset, time_len, target, method, start, stop )
		ch.next = self.head
		self.head = ch
		return ch
	end
end
function CHANGERS:add_trig( time_offset, target, method, ... )
	if self.b_no_delay or time_offset==0 then
		target[ method ]( target, ... )
	else
		local id = self.__changer_id + 1
		self.__changer_id = id
		local ch = TCHANGER:create( self:get_name()..id, time_offset, target, method, ... )
		ch.next = self.head
		self.head = ch
		return ch
	end
end

function CHANGERS:change()
	local prev
	local cur = self.head
	while cur do
		local next = cur.next
		if cur:change() then
			-- remove cur
			if cur == self.head then
				self.head = next
			else
				prev.next = next
			end
			cur.next = nil
			-- DO NOT update prev
		else
			-- only advance prev if not removed
			prev = cur
		end
		cur = next
	end
end

