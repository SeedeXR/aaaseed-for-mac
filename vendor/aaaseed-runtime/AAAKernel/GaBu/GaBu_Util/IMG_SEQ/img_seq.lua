
--
--	IMG_SEQ
--
if CLASS.DECLARE( "IMG_SEQ", nil ) then
	IMG_SEQ:set_class_status_doc(	CLASS.STATUS.CORE,
									"deal with a sequence of image" )
    IMG_SEQ.b_use_img = true    --todo do we still need simplier bind mode
								-- have a fn to switch mode
end

function IMG_SEQ:print_result( pre, nb, first, last )
	if nb > 0 then
		local str = nb.." " 
		if first then
			str = str.."["..first..","..last.."] "
		end
		if pre then
			str = str.." images for "..pre
		end
		self:print( str )
	else
		self:print_error( "Failed: no image for sequence "..pre )
	end
end

function IMG_SEQ:__init_seq()
    if self.b_use_img then
        self.imgs = {}
    else
        self.binds = {}
    end
    self.nb = 0
end


function IMG_SEQ:__load_low( b_load_really, filename, ext,  first, last, step )

	self.__b_loaded = false
	if filename then
		self.filename = filename
		self.ext = ext
	else
		filename = self.filename
		ext = self.ext
	end

	if not filename then
		self:print_error( "Need a filename to try to explore or load sequence" )
		return
	end

	local full_name = MEDIA.make_fname( filename )
	ext = "."..ext
	local function get_format( number )
		--self:print( "fullname is "..full_name )
		return aaa.file.get_format_exist_name_number( full_name, number, ext )
	end

    -- we start  at an asked first or we try 0 then 1
	first = first or 0
	local format = get_format( first )
	if not format and first==0 then
		first = 1
		format = get_format( first )
	end

	local i_valid_last
	local id = 0
    local str_action = b_load_really and "reading" or "parsing"
	if format then
        self:__init_seq()

		last = last or 1000000
		step = step or 1
        
		self:print_debug( "Begin "..str_action.." \""..filename.."\" from "..first.." to "..last )

		for i = first,last, step do
			local fname = aaa.file.make_name_number( filename, i, ext, format )
			local b_exist = IMGS.is_exist_error_no( fname )
			if b_load_really then
				--if i%64 == 0 then self:print( i ) end
				if b_exist then
                    if self.b_use_img then
                        local img = IMGS.get_img( fname )
                        if img then
                            i_valid_last = i
                            self.imgs[id+1] = img
                        else
                            self:print_error( "can't IMGS.get_img( "..fname.." )" )
                        end
                   else
                        local bind = IMGS.get_bind( fname )
                        if bind then
                            i_valid_last = i
                            self.binds[id+1] = bind
                        else
                            self:print_error( "can't IMGS.get_bind( "..fname.." )" )
                        end
                    end
				else
					self.first = first
					break
				end
			else
				if b_exist then
					i_valid_last = i
				else
					break
				end
			end
			id = id + 1
		end

		--self.first = first
		--self.last = i_valid_last
		--self.i_step = step

        self.nb = id
		self.__b_loaded = b_load_really
	else
		self:print_error( str_action.." : can't find first file for "..filename..first..ext )
	end
	
	self:print_result( full_name, self.nb, first, i_valid_last )
end


function IMG_SEQ:is_loaded()
	return self.__b_loaded
end
function IMG_SEQ:dump( pre )
	pre = pre or ""
	self:print( pre.."have "..self.nb.." frames "..(self.__b_loaded and "" or "not ").."loaded" )
end

function IMG_SEQ:load( ... )
	return self:__load_low( true, ... )
end
function IMG_SEQ:explore( ... )
	return self:__load_low( false, ... )
end

function IMG_SEQ:load_from_array( dir, files )
	self:__init_seq()

	local id = 0
	for _, fname in IPAIRS(files) do
        local img_or_bind
		local name_full = dir and dir..fname or fname
        if self.b_use_img then
            img_or_bind = IMGS.get_img( name_full )
            if img_or_bind then
                id = id + 1
                self.imgs[id] = img_or_bind
            end
        else
            img_or_bind = IMGS.get_bind( name_full )
            if img_or_bind then
                id = id + 1
                self.binds[id] = img_or_bind
            end
        end
        if not img_or_bind then
            self:print_error( "could not load "..name_full )
        end
	end

	self.nb = id
	self:print_result( nil, self.nb )
end

function IMG_SEQ:load_from_dir( dir, pat )
	--aaa.print_fn()
	local files = aaa.dir.get_files( dir, pat )
	table.print( files, "in dir "..dir )
    return self:load_from_array( dir, files )
end
function IMG_SEQ:create_new( name )
	local self = IMG_SEQ:create_instance( name )
    self.b_use_img =  IMG_SEQ.b_use_img
	self:__init_seq()
	return self
end

--todo don't recreate when we already have it as we do for IMG
function IMG_SEQ:create( name, filename, ext, first, last, step )
	local self = self:create_new( name )
	self:load( filename, ext, first, last, step )
	return self
end

function IMG_SEQ:create_from_array( name, tab )
	local self = self:create_new( name )
	--self:box_debug( "Mâa in 20213 Feb notice next line need a dir as first arg\nSolve this.")
	self:load_from_array( nil, tab )
	return self
end
function IMG_SEQ:create_from_dir( name, dir, pat )
	local self = self:create_new( name )
	self:load_from_dir( dir, pat )
	return self
end



function IMG_SEQ:get_nb()			return self.nb			end
function IMG_SEQ:get_bind( id )
    if self.b_use_img then
        local img = self.imgs[id]
        return img and img:get_bind()
    end
    return self.binds[id]
end
function IMG_SEQ:get_img( id )
    if self.b_use_img then
        local img = self.imgs[id]
        return img
    else
        self:print_error( "init with b_use_img at false" )
    end
end
function IMG_SEQ:get_id_rnd( seed )
	if seed then
		math.randomseed( seed*133 + seed*37 + seed )
	end
    return math.random( 1, self.nb )
end
function IMG_SEQ:get_bind_rnd( seed )	return self:get_bind( self:get_id_rnd(seed) )	end
function IMG_SEQ:get_img_rnd( seed )	return self:get_img( self:get_id_rnd(seed) )	end
function IMG_SEQ:get_index_in_cycle( v )
	local r
	--self:print( v )
	v,r = math.modf(v)
	v = math.floor( r*self.nb )
	--self:print( v.." / "..self.nb )
	--table.print( self.bind )
	return v
end
function IMG_SEQ:get_index_in_cycle_triangle( v )
	local r
	v,r = math.modf(v*.5)
	if r > .5 then r = 1. - r end
	v = math.floor( r*2*self.nb )
	return v
end

function IMG_SEQ:get_img_in_cycle( v )
	v = self:get_index_in_cycle(v)
	return self:get_img(v+1)
end
function IMG_SEQ:get_bind_in_cycle( v )
	v = self:get_index_in_cycle(v)
	return self:get_bind(v+1)
end
function IMG_SEQ:get_bind_in_cycle_triangle( v )
	v = self:get_index_in_cycle_triangle(v)
	return self:get_bind(v+1)
end

if CLASS.DECLARE( "IMG_SEQ_PLAYER", nil, { __b_loop=true } ) then
	IMG_SEQ_PLAYER:set_class_status_doc(	CLASS.STATUS.CORE,
											"use an IMG_SEQ but this class handle playing it too" )
    IMG_SEQ_PLAYER.__fps = 25
end

function IMG_SEQ_PLAYER:create( name, img_seq )
    self = IMG_SEQ_PLAYER:create_instance( name )
	self.img_seq = img_seq
    self:__set_index_float( 0 )
	return self
end
function IMG_SEQ_PLAYER:is_loaded()             return self.img_seq:is_loaded()				end
function IMG_SEQ_PLAYER:dump( pre )             return self.img_seq:dump(pre)				end
function IMG_SEQ_PLAYER:load(...)               return self.img_seq:load(...)				end
function IMG_SEQ_PLAYER:explore(...)            return self.img_seq:explore(...)			end
function IMG_SEQ_PLAYER:get_img_in_cycle(ph)    return self.img_seq:get_img_in_cycle(ph)	end

--todo implemet different mode bounce/triangle/clamp
function IMG_SEQ_PLAYER:set_loop( b_loop )	    self.__b_loop = b_loop 		                end
function IMG_SEQ_PLAYER:is_loop()	    		return self.__b_loop 		                end
function IMG_SEQ_PLAYER:get_nb()			    return self.img_seq:get_nb()                end

function IMG_SEQ_PLAYER:set_fps( fps )              self.__fps = fps                                end
function IMG_SEQ_PLAYER:set_duration( duration )    self.__fps = self.img_seq:get_nb() / duration   end
function IMG_SEQ_PLAYER:restart()
    self.__index_float = 0.
	self.__loop_nb_done = 0
end

-- this is the internal mecanism to store where is the reading head
-- floor of index should be from 0 frame_nb-1
function IMG_SEQ_PLAYER:__set_index_float( v )	--this is a float
	if v >= 0 then
		local nb = self.img_seq:get_nb()
		if v >= nb then
			-- if self.b_pingpong then
			-- 	if self.b_reverse then
			-- 		self.b_reverse = false
			-- 		v = 0
			-- 		self.__loop_nb_done = (self.__loop_nb_done or 0) + 1
			-- 	else
			-- 		self.b_reverse = true
			-- 		v = 0
			-- 	end				
			-- else
			if self.__b_loop then
				v = math.fmod( v, nb )
				self.__loop_nb_done = (self.__loop_nb_done or 0) + 1
			else
				v = nb-.0001    --todo refine
				self.__loop_nb_done = 1
			end
		end
	else
--		self:print( "RESTART" )
		v = 0
	end
	self.__index_float = v
end
function IMG_SEQ_PLAYER:set_reverse( b_reverse )
	if (self.b_reverse == true) ~= b_reverse then
		local nb = self.img_seq:get_nb()
		self.__index_float = clamp( nb - self.__index_float, 0, nb-.0001 )	--todo refine
		self.b_reverse = b_reverse
	end
end
function IMG_SEQ_PLAYER:is_reverse()	return self.b_reverse	end

function IMG_SEQ_PLAYER:__inc_index_float( inc )	self:__set_index_float( self.__index_float + inc )    end
function IMG_SEQ_PLAYER:__get_index_float()       	return self.__index_float                               end

-- this is the interface to the outside world
-- phase is 0 to 1
function IMG_SEQ_PLAYER:set_phase( phase )      self:__set_index_float( self.img_seq:get_nb() * phase )   end
function IMG_SEQ_PLAYER:inc_phase( inc )        self:__inc_index_float( self.img_seq:get_nb() * inc )     end
function IMG_SEQ_PLAYER:get_phase()             return self:__get_index_float() / self.img_seq:get_nb()   end
-- use the fps
function IMG_SEQ_PLAYER:set_time( time )	    self:__set_index_float( self.__fps * time )               end
function IMG_SEQ_PLAYER:inc_time( inc )	        self:__inc_index_float( self.__fps * inc )                end
function IMG_SEQ_PLAYER:get_time()              return self:__get_index_float() / self.__fps              end

function IMG_SEQ_PLAYER:__get_id( id )
	local nb = self.img_seq:get_nb()
	if id then
		id = clamp( id, 1, nb )
		if self.b_reverse then
			id = nb + 1 - id
		end
	else
		local index_float = self.__index_float
		if self.b_reverse then	
			index_float = clamp( nb - index_float, 0, nb-.0001 )	--todo refine	
		end
		id = math.floor(index_float) + 1
	end
	return id
end

function IMG_SEQ_PLAYER:get_bind( id )          return self.img_seq:get_bind( self:__get_id(id) )	end
function IMG_SEQ_PLAYER:get_img( id )           return self.img_seq:get_img(  self:__get_id(id) )	end

-- function IMG_SEQ_PLAYER:get_img_in_cycle( v )
-- 	v = self.img_seq:get_index_in_cycle(v)
-- 	self.__index = v
-- 	return self:get_img(v+1)
-- end
-- function IMG_SEQ_PLAYER:get_bind_in_cycle( v )
-- 	v = self.img_seq:get_index_in_cycle(v)
-- 	self.__index = v
-- 	return self:get_bind(v+1)
-- end
-- function IMG_SEQ_PLAYER:get_bind_in_cycle_triangle( v )
-- 	v = self.img_seq:get_index_in_cycle_triangle(v)
-- 	self.__index = v
-- 	return self:get_bind(v+1)
-- end
