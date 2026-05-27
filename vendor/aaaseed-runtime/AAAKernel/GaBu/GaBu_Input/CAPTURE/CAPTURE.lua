-- 2023 Sep unused
if true then return end

--
--	CAPTURE watch out if capture is stable
--
CLASS.DECLARE( "CAPTURE" )


--todo add name for message infact add print
function CAPTURE:create( name, ref )
	local self = CAPTURE:create_instance_with_obj( name, ref )
	--todo remove this already done in GABU_OBJ
	local ref = self.ref
	if ref and ref.obj then
		local tv = ref.obj
		ref.capture = {}
		ref.capture.img_index	= param.get_ref( tv, "image_index" 		)
		ref.capture.open		= param.get_ref( tv, "capture_open"		)
		ref.capture.opened		= param.get_ref( tv, "capture_opened"	)
		ref.capture.run			= param.get_ref( tv, "capture_run"		)
		ref.capture.running		= param.get_ref( tv, "capture_running"	)
	end
	self.b_error_to_confirm = false
	self:set_check_period( 3 )
	return self
end
function CAPTURE:set_check_time( t, img_index )
	self.check_time = t
	self.check_time_next = t + self.check_period
	self.check_img_index = img_index
end
function CAPTURE:set_check_period( period )
	self.check_period = period
	self:set_check_time( aaa.time.t, param.get( self.ref.capture.img_index ) )
end
function CAPTURE:reset_check()
	self:set_check_period( self.check_period )
end

function CAPTURE:set_start_stop( b )
	local ref = self.ref.capture
	param.set( ref.open, b )
	param.set( ref.run, b )
end
function CAPTURE:set_start()
	self:print( "set_start()" )
	self:set_start_stop( true )
end
function CAPTURE:set_stop()
	self:print( "set_stop()" )
	local ref = self.ref
	self:set_start_stop( false )
end

function CAPTURE:check( t )
	local ref = self.ref.capture
	local b_run = param.get_bool( ref.running )
	if not b_run then
		aaa.print( "not running" )
		if param.get_bool( ref.open ) then
			aaa.print( "open" )
			if self.b_error_to_confirm then
				self:post_error( "did not manage to restart, trying again" )
			end
		else
			aaa.print( "not open" )
			self:set_start()
			self:reset_check()
			self.b_error_to_confirm = true
			return
		end
	end
	local index = param.get( ref.img_index )
	if index ~= self.check_img_index then
		if self.b_error_to_confirm then
			self:post_warning( "restart was sucessful" )
		end
		--so it is running fine
			self:set_check_time( t, index )
			self.b_error_to_confirm = false
		return
	end
	--there is a problem
	if self.b_error_to_confirm then
		self:post_error( "did not manage to restart, trying again" )
	else
		self:post_warning( "check failed, no image refreshed, trying to restart cam" )
	end
	self:set_stop()
	self:reset_check()
end


function CAPTURE:post_warning( str )
	self:print( "warning: "..str )
	if self.send_fn then
		self.send_fn( "warning", { self.." : "..str } )
	end
end

function CAPTURE:post_error( str )
	self:print( "ERROR: "..str )
	if self.send_fn then
		self.send_fn( "error", { self.." : "..str } )
	end
end

function CAPTURE:update()
	local t = aaa.time.t
	if inside( t, self.check_time, self.check_time_next ) then return end
	if t < self.check_time then
		-- time was probably reset, so we just reset our checking process
		self:reset_check()
		return
	end
	self:check( t )
end


