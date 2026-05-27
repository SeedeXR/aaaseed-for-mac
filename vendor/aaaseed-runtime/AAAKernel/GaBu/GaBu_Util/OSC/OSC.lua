if CLASS.DECLARE( "OSC" ) then
	OSC:set_class_status_doc(	CLASS.STATUS.CORE,
									"central object to deal with OSC" )
end

function OSC:set_dst( index )
	if index ~= nil then
		self.dst = index
	end
end

function OSC:init( dst )
	self.count = 0
	self.b_verbose = false
	self.b_send = true
	self.b_received = true
	self.b_bundle = true
	self:set_dst( dst or 2 )
end

function OSC:create( name, dst )
	local self = OSC:create_instance( name )
	self:init( dst )
	return self
end

function OSC:send( what, ... )
	self.b_bundle = true
--	aaa.print_fn()
	local sent_count = self.count + 1
	self.count = sent_count
	local tag = "/"..what

	if self.b_verbose then
		if self.b_send then
			self:print( "send "      ..sent_count.." : "..tag..string:make_from_list( ... ) )
		else
			self:print( "don't send "..sent_count.." : "..tag..string:make_from_list( ... ) )
		end
	end

	if self.dst then
		if self.b_send then
			if self.b_bundle then
				aaa.net.osc_send( self.dst, tag, ... )
			else
				aaa.net.osc_send_no_bundle( self.dst, tag, ... )
			end
		end
	else
		self:print_debug( "destination index is nil so we can't send anything" )
	end
	--aaa.sleep( osc_delay )
end

function OSC:send_mess_test( text, ... )
--	aaa.print_fn()
	self:send( "AAASeed/mess/test", text, self.count+1, ... )
end

-- VALUE
--
function OSC:send_value( name, v )		self:send( "AAASeed/value", name, v ) end
function OSC:receive_value( m )
	local name = m:get_arg(1)
	local value = m:get_arg(2)
	self[name] = value
end

-- MIDI
--
function OSC:send_midi( ch, ctl, v )	self:send( "AAASeed/midi/control", ch, ctl, v ) end
function OSC:receive_midi( m )
	m:pop_tag()

	local tag_midi = m:get_tag()
	if tag_midi == "control" then
		--m:dump( "MIDI_CTL" )
		--self:print( m:get_arg(1), m:get_arg(2), m:get_arg(3) )
		aaa.midi.set_control( m:get_arg(1), m:get_arg(2), m:get_arg(3) )
	end
end

-- FRAME
--
function OSC:send_ndim_begin( ch_name, ch_frame_id, dim_max, total_point_nb )
	self.ch_name = ch_name
	self.ch_frame_id = ch_frame_id
	self:send( "AAASeed/ndim/begin", ch_name, ch_frame_id, dim_max, total_point_nb )
end
function OSC:send_ndim_end( ch_name, ch_frame_id )
	self:send( "AAASeed/ndim/end", ch_name or self.ch_name, ch_frame_id or self.ch_frame_id )
end

function OSC:send_ndim_group( ch_name, ch_frame_id, gr_name, gr_id, dim, tab )
--	local nb_max = math.floor( 180 / (dim + 1) )
	local nb = #tab / (dim + 1)	
	self:send( "AAASeed/ndim/group_data", ch_name or self.ch_name, ch_frame_id or self.ch_frame_id, gr_name, gr_id, dim, nb, unpack(tab) )
end
function OSC:erase_ndim()
	self.__ndim = nil
end

OSC.doc.receive_ndim = "( m ) take an OSC_MESS as a argument"
function OSC:receive_ndim( m )
	m:pop_tag()
	
	local tag = m:get_tag()
	local ch_name = m:pop_arg()
	local ch_frame_id = m:pop_arg()
	local ch = self:get_table_always( "__ndim", ch_name )
	local frame = ch[ch_frame_id]

	--aaa.print_fn()
	if frame == nil then
		frame = {}
		ch[ch_frame_id] = frame
		if ch[ch_frame_id-2] then
			ch[ch_frame_id-2] = nil
		end
	end

	if tag == "begin" then
	elseif tag == "end" then
		self.__stream_frame_done = frame
	elseif tag == "group_data" then
		local gr_name = m:pop_arg()
		local gr_id = m:pop_arg()
		
		local gr = frame[gr_name]
		if not gr then
			gr = {}
			frame[gr_name] = gr
		end
		--table.print( frame, "frame", 3 )

		local tab = gr[gr_id]
		if not tab then
			tab = {}
			gr[gr_id] = tab
		end

		local dim = m:pop_arg()
		local nb = m:pop_arg()
		local id_prev
		for i=1,nb do
			local id = m:pop_arg()
			if id then
				local t = {}
				for j=1,dim do
					local v = m:pop_arg()
					if v then
						t[j] = v
					else
						self:print_error( "OSC:receive_ndim() vector of id "..id.." got no value for component = "..j )
						t = nil
						break;
					end
				end
				if not t then
					break
				end
				tab[id] = t
				id_prev = id
			else
				self:print_error( "OSC:receive_ndim() got no vector id at loop iteration = "..i )
				if id_prev then
					self:print_error( "OSC:receive_ndim() for info previous id was "..id_prev )
				end
				break
			end
		end 	
	end
end

function OSC:get_ndim_done()
	return self.__stream_frame_done
end
