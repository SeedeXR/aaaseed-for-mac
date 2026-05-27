CLASS.DECLARE( "SEQ", nil,  {
--	__ease_in = 0,
--	__ease_in_time = 1,
--	__ease_out = 0,
--	__ease_out_time = 1,
	} )

function SEQ:set_duration( d )			self.__duration = d			return self	end
function SEQ:get_duration()				return self.__duration					end

function SEQ:get_phase()				return self.__phase						end
function SEQ:get_phase_left()			return 1. - self.__phase				end

function SEQ:__set_time( time, test_time )
	local dur = self.__duration or 1
	if test_time then
		dur = (test_time>0) and math.min( test_time, dur ) or dur
	end

	self.__time		= time
	self.__phase	= time / dur

	local ease_in = self:get_ease_in_time()
	if ease_in then
		self:set_ease_in( clamp_01( time / ease_in ) )
	end
end
function SEQ:get_time()					return self.__time						end
function SEQ:get_time_left()
	local dur = self:get_duration()
	if dur then
		return dur - self.__time
	else
		return 0
	end
end

function SEQ:set_level_id( level_id )	self.__level_id = level_id	return self	end
function SEQ:get_level_id()				return self.__level_id		end

function SEQ:set_seq_id( seq_id )		self.__seq_id = seq_id		return self	end
function SEQ:get_seq_id()				return self.__seq_id		end


--completed is a way to exit the seq using a condition
function SEQ:is_completed()				return self.__b_completed	end
function SEQ:set_completed( b )			self.__b_completed = b
										if self.verbose >= 1 then
											self:print( "completed now "..b )
										end
									end

function SEQ:get_title()				return self:get_name()	end

function SEQ:get_imgs()					return self.__imgs			end
function SEQ:set_imgs( imgs )			self.__imgs = imgs			return imgs			end

SEQ.doc.set_ease_in_time	= "( t ) set the time to use for ease in"
SEQ.doc.get_ease_in_time	= "() get the time used for ease in"
SEQ.doc.set_ease_in			= "( v ) set the ease in value (internal to seq usually)"
SEQ.doc.get_ease_in			= "() get the ease in value"
function SEQ:set_ease_in_time( t )		self.__ease_in_time = t		end
function SEQ:get_ease_in_time()			return self.__ease_in_time	end
function SEQ:set_ease_in( v )			self.__ease_in = v			end
function SEQ:get_ease_in()				return self.__ease_in		end

--function SEQ:set_ease_out_time( t )		self.__ease_out_time = t	end
--function SEQ:get_ease_out_time()		return self.__ease_out_time	end
--function SEQ:set_ease_out( v )			self.__ease_out = v			end
--function SEQ:get_ease_out()				return self.__ease_out		end


--	PLAYLIST
--
function SEQ:add_playlist( playlist, key, time )
	local t = self:get_table_always( "__playlists" )
	table.insert( t, { playlist=playlist, key=key, time_start=time } )
end
function SEQ:reset_playlist()
	table.apply_fn( self.__playlists, function(elt) elt.b_need_start=true elt.b_started=false end )
end
function SEQ:stop_playlist()
	table.apply_fn( self.__playlists, function(elt)
		if elt.b_started then
			elt.playlist:stop_track()
		end
	end )
end
function SEQ:update_playlist()
	local time = self:get_time()
	table.apply_fn( self.__playlists, function(elt)
		if elt.b_need_start then
			if time > elt.time_start then
				elt.playlist:begin_track( elt.key )
				elt.b_started=true
				elt.b_need_start=false
			end
		end
	end )
end

function SEQ:set_methods( target, method_update, method_active )
	if method_update then	self:set_method( "update",		target, method_update, self )	end
	if method_active then	self:set_method( "set_active",	target, method_active, self )	end	--todo nil for first arg (redo dofn)
	return self
end

--function SEQ:reset_playlist()
--	table.apply_fn( self.__playlists, function(elt) elt.b_need_start=true elt.b_started=false end )
--end

--	MU used
--
SEQ.doc.add_mu_used_by_name	= "(table) add a table of mu to switch on/off according to seq activation, a table of name is passed here"
function SEQ:add_mu_used_by_name( tab )
	local gp = app:get_gp()
	if gp then	-- b_ios
		local t = self:get_table_always( "__mu_used" )
		for i=1,#tab do
			table.insert( t, { mu = gp:get_mu_by_name( tab[i] ) } )
		end
	end
end


--UNHACK
function SEQ:__set_mu_used( b )	table.apply_fn( self.__mu_used, function(elt) elt.mu:set_value( b )	end )	end
function SEQ:disable_mu_used()	self:__set_mu_used( false )	end
function SEQ:enable_mu_used()	self:__set_mu_used( true )	end

function SEQ:__init_new( level_id, seq_id, comment, duration )
	self:set_level_id(	level_id 	)
	self:set_seq_id(	seq_id		)
	self:switch( false )
	self.__comment 	= comment
	self:__set_time( 0. )
	if duration then self:set_duration( duration ) end
	return self
end

function SEQ:create( title, level_id, seq_id, comment, duration )
	local self = SEQ:create_instance( title )
	self:print_inverse( "SEQ:create( "..level_id..", "..seq_id..", \""..title.."\" )" )
	return self:__init_new( level_id, seq_id, comment, duration )
end

function SEQ:switch( b_on, seq_other )
	if self.verbose >= 1 then
		--todo make more precise
		self:print( "switch( "..b_on.." )" )
	end
	--GABU_OBJ at least
	GABU_OBJ.set_active( self, b_on )

	if seq_other then seq_other:set_completed( b_on ) end
	self:set_completed( not b_on )
	if b_on then
		self:reset_playlist()
		local ease = self:get_ease_in_time()
		self:set_ease_in( ease and 0 or 1 )
		--self:set_ease_out( 0 )
	else
		self:stop_playlist()
		--self:set_ease_out( 1 )
	end
	self:__set_mu_used( b_on )

--	if self.fn_set_active then
--		self.fn_set_active( b_on )
--	end
	self:__set_time( 0 )

	self:do_fn( "set_active", b_on )
end

function SEQ:is_done( b_run, test_time )
	if b_run then
		if self.__duration then
			if self.__phase > 1 then
				return true
			end
		else
			if test_time>0 and self.__time>test_time then
				return true
			end
		end
	end
	if self:is_completed() then
		return true
	end
end

function SEQ:updraw( b_run, b_update, dt, test_time )
	SEQ.cur = self

	--aaa.print( "toto" )
	if b_update then
		if b_run then
			self:__set_time( self:get_time() + dt, test_time )
			self:update_playlist()		--todo avoid double updates
		end

		self:do_fn( "update" )
		if self.update then self:update() end
	end
end
