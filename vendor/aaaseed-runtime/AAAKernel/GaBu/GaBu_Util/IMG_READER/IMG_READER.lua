if CLASS.DECLARE( "IMG_READER_POOL" ) then
	--IMG_READER.bind_free = {}
	IMG_READER_POOL:set_class_status_doc(	CLASS.STATUS.CORE,
											"Base class for the 2d and 3d version, handle several IMG_READER" )
end

function IMG_READER_POOL:init_buffers( start, nb )
	self.info_by_name =	{}
	self.info_by_age  =	{}
	self.bind_start		=	start
	self.bind_nb		=	nb
	for i=1,nb do
		self.info_by_age[i] = { bind = start + i - 1, b_ready = true, ref = 0 }
	end
end
function IMG_READER_POOL:reset_buffers()
	for i=1,self.bind_nb do
		self.info_by_age[i].ref = 0
		self.info_by_age[i].b_ready = true
	end
end
function IMG_READER_POOL:inc_ref( info )
	info.ref = info.ref + 1
	if info.ref > self.bind_nb then
		--info.ref = self.bind_nb
		table.print( info, "IMG_READER_POOL ERROR ref nb to high", 2 )
	end
end
function IMG_READER_POOL:dec_ref( info )
	info.ref = info.ref - 1
	if info.ref < 0 then
		--info.ref = 0
		table.print( info, "IMG_READER_POOL ERROR ref nb to low", 2 )
	end
end

function IMG_READER_POOL:update_info( info )
	if not info.b_ready then
		--self:print( "update bind "..info.bind )
		info.b_ready = self:is_info_ready( info )
		--aaa.img.move_to_gpu( info.bind )
	end
end

function IMG_READER_POOL:get_info( root, name )
	--self:print( "get_info( \""..name.."\" )" )
	local info = self.info_by_name[ name ]
	if not info then
		local found = nil
		local by_age = self.info_by_age
		for i=1,#(self.info_by_age) do
			if by_age[i].ref == 0 then
				info = by_age[i]
				found = i
				break
			end
		end
		if found then	--info.ref == 0
			--table.print( IMG_READER.info_by_age, "TOTO", 3 )
			--remove from tables
			table.remove( by_age, found )
			if info.name then
				self.info_by_name[ info.name ] = nil
			end
			--set info for this file
			info.name = name
			info.b_ready = false
			--start reading now
			local name_full = root..info.name
			info.name_full = root..info.name
			--self:print( "full_mane is "..name_full )
			--todo check return code
			self:start_info( info )
			info.ask_time = aaa.time.t
			--put new one in tables
			table.insert( by_age, info )
			self.info_by_name[ info.name ] = info
		else
			self:print( "No free IMG_reader" )
			table.print( self.info_by_age, "TOTO", 3 )
		end
	end
	--if info then
	--	self:update_info(info)
	--end
	return info
end

if CLASS.DECLARE( "IMG_READER_POOL_2D", IMG_READER_POOL ) then
	IMG_READER_POOL_2D:set_class_status_doc(	CLASS.STATUS.CORE,
												"the 2d version of an IMG_READER_POOL" )
end
function IMG_READER_POOL_2D:create( name, start, nb )
	local self = IMG_READER_POOL_2D:create_instance( name  )
	self:init_buffers( start, nb )
	return self
end
function IMG_READER_POOL_2D:is_info_ready( info )	return aaa.img.is_ready( info.bind )	end
function IMG_READER_POOL_2D:start_info( info )		aaa.img.read( info.bind, info.name_full, false, true, true )	end

if CLASS.DECLARE( "IMG_READER_POOL_3D", IMG_READER_POOL ) then
	IMG_READER_POOL_3D:set_class_status_doc(	CLASS.STATUS.CORE,
												"the 3d version of an IMG_READER_POOL" )
end
function IMG_READER_POOL_3D:create( name, start, nb )
	local self = IMG_READER_POOL_3D:create_instance( name  )
	self:init_buffers( start, nb )
	return self
end
function IMG_READER_POOL_3D:is_info_ready( info )	return aaa.img3d.is_ready( info.bind )	end
function IMG_READER_POOL_3D:start_info( info )
	--self:print( info.name_full )
	aaa.img3d.read( info.bind, info.name_full, false, true, true )
end

if CLASS.DECLARE( "IMG_READER" ) then
	--IMG_READER.bind_free = {}
	IMG_READER:set_class_status_doc(	CLASS.STATUS.CORE,
										"Use by the tree installation to handle asyncronous read of image (2d and 3d) in a serie of file",
										"need a pool (IMG_READER_POOL) to work on",
										"need to be generalized (2023 July Mâa)" )
end

function IMG_READER:make_info_fname( frame )
	--self:print( "  frame : "..frame )
	return string.format( "tree_%05d.tga", frame )
end
function IMG_READER:make_info_name( dir, frame )
	--self:print( "dir is : "..dir )
	return string.format( dir..self:make_info_fname(frame) )
end

function IMG_READER:create( name, pool )
	local self = IMG_READER:create_instance( name )
	self:attach_pool( pool )
	return self
end
function IMG_READER:attach_pool( pool )
	self.pool = pool
end

function IMG_READER:init_dir( seq )
	self.root 			= seq.dir
	self.dir 			= seq.name
	self.frame_nb 		= seq.frame_nb
	self.seq			= seq
	self:release_future_info()
--	self.info = nil
--	self.future_info = nil
--	self.pool:reset_buffers()
end

function IMG_READER:init_day_offset_frame_by_day( day_nb, frame_offset, day_frame_nb )
--	self:print_inverse( day_nb.." days with offset "..frame_offset.." frame_nb "..day_frame_nb )
	if not day_nb then error( "day_nb is nil" ) end
	self.day_nb 			= day_nb
	self.frame_offset 		= frame_offset
	self.day_frame_nb 		= day_frame_nb
end

function IMG_READER:get_frame()		return self.__frame		end
function IMG_READER:get_frame_nb()	return self.frame_nb	end

function IMG_READER:set_frame( frame )
	if not self.future_info then
		frame = math.floor( frame )
		frame = frame % self.frame_nb + 1
		if self.__frame ~= frame then
			local future_info = self.pool:get_info( self.root, self:make_info_name( self.dir, frame ) )
			if future_info then
				self.__frame = frame
				self.pool:inc_ref( future_info )
				self.future_info = future_info
				--self:print( future_info.bind.." -- "..future_info.ref )
			end
		end
	end
	return self.__frame
end

function IMG_READER:set_frame_using_day( day )
	day = math.fmod( day, self.day_nb )
	local frame = day * self.day_frame_nb + self.frame_offset
	return self:set_frame( frame )
end

function IMG_READER:release_future_info()
	local future_info = self.future_info
	if future_info then
		if self.info then
			self.pool:dec_ref( self.info )
			--self:print( self.info.bind.." -- "..self.info.ref )
		end
		self.info = future_info
		self.future_info = nil
		self.__time_mod = nil
	else
		--self:print( "No future info to release ????" )
	end
end

local verbose_mess_by_sec = 4.
local verbose_mess_by_sec_over = 1. / verbose_mess_by_sec

function IMG_READER:get_bind()
	local future_info = self.future_info
	--self:print( "future info "..future_info )
	if future_info then
		--self:print( "update future info" )
		self.pool:update_info( future_info )
		if future_info.b_ready then
			--IMG_READER.__bind_done = IMG_READER.__bind_done + 1
			self:release_future_info()
			--self:get_info( self.dir, self.frame+1 )
			--self:get_info( self.dir, self.frame+2 )
 		else
			--self:print( "not ready" )
			local wait_time = aaa.time.t - future_info.ask_time
			if wait_time > verbose_mess_by_sec_over  then
				if wait_time > 4.  then
					self:print_error( "resetting locked reader "..future_info.name  )
					self:release_future_info()
				else
					local mod = math.floor( wait_time * verbose_mess_by_sec )
					if self.__time_mod ~= mod then
						self.__time_mod = mod
						self:print( "busy reader for "..(mod*verbose_mess_by_sec_over).."sec "..future_info.name  )
					end
				end
			end
		end
	else
		--self:print( "No future info" )
	end

	return self.info and self.info.bind
end

