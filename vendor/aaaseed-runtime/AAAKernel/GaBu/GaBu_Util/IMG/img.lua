--
--	IMG
--
if CLASS.DECLARE( "IMG" ) then
	IMG.__by_name = {}	--todo IMGS have to become a class and centralize this
	IMG:set_class_status_doc(	CLASS.STATUS.CORE,
								"encapsulate an image defined by a filename and a bind",
								"deal eventually with several files in different language",
								"IMG are asked using the IMGS class" )
end

function IMG:get_bind()	return self.bind		end
-- in the crop/proxy creation process, was Commented to provoque errors to correct them
function IMG:get_sx()	return self.sx			end
function IMG:get_sy()	return self.sy			end
function IMG:get_sxy()	return self.sx, self.sy	end

function IMG:__set_sxy( sx, sy )
	self.sx = sx
	self.sy = sy
	self.rx = sx/sy
	self.ry = sy/sx
end
function IMG:get_rx()	return self.rx	end
function IMG:get_ry()	return self.ry	end
--todo comment and explain crop better
-- crop_info is a table with field left, right, bottom, top, size_x, size_y }
function IMG:set_crop_from_def( crop_info )
	if not crop_info then
		self:print_error( "crop_info is nil" )
		return
	end
	local ci = crop_info
	local l,b, r,t, size_x,size_y = ci.left,ci.bottom, ci.right,ci.top, ci.size_x, ci.size_y
	local c = {}
	c.img_sx = r-l+1
	c.img_sy = t-b+1
	c.ratio_x = c.img_sx/size_x
	c.ratio_y = c.img_sy/size_y
	c.factor_size_l = l / size_x
	c.factor_size_b = b / size_y
	self.crop = c

	self:__set_sxy( size_x, size_y )
end


function IMG:draw_xyz_sxy( x,y,z, sx,sy )
	gol.bind_texture( self:get_bind() )
	local l,b,r,t
	l,b =  x-sx/2, y-sy/2
	local c = self.crop
	if c then
		l = l + c.factor_size_l * sx
		b = b + c.factor_size_b * sy
		sx = sx * c.ratio_x
		sy = sy * c.ratio_y
	end
	r,t = l+sx, b+sy
	aaa.draw_rect_uv_at_z( l,b, r,t, z )
end

function IMG:draw_xyz_sxy_rotz( x,y,z, sx,sy, rot_z )
	gol.bind_texture( self:get_bind() )
--	local l,b,r,t
--	l,b =  x-sx/2, y-sy/2
	local c = self.crop
	if c then
		self:print_error( "we have to implement this case" ) 
	end
	aaa.draw_rect_uv_axe_z( x,y,z, sx,sy, rot_z )
end


--todo we have an asymmetry here in the use of b_flip_x (not passed to other fns )
function IMG:draw_rects_xyz_sxy( x,y,z, sx,sy, b_flip_x )
	local c = self.crop
	if not sx then sx = sy*self.rx end
	if not sy then sy = sx*self.ry end
	local l,b,r,t
	if b_flip_x then
		sx = -sx
	end
	l,b = x-sx/2, y-sy/2
	aaa.draw_rect_line_at_z( l,b, l+sx,b+sy, z )
	if c then
		l = l + c.factor_size_l * sx
		b = b + c.factor_size_b * sy
		sx = sx * c.ratio_x
		sy = sy * c.ratio_y
	end
	r,t = l+sx, b+sy
	aaa.draw_rect_line_at_z( l,b, r,t, z )
end

function IMG:draw_sxy(					sx,sy )		self:draw_xyz_sxy(		0,0,0, sx,sy )				end
function IMG:draw_sx(					sx )		self:draw_sxy(			sx, math.abs(sx)*self.ry )	end
function IMG:draw_sy(					sy )		self:draw_sxy(			sy*self.rx, sy	)			end

function IMG:draw_xy_sxy(		x,y,	sx,sy )		self:draw_xyz_sxy( 		x,y,0, sx,sy )				end
function IMG:draw_lb_sxy(		l,b,	sx,sy )		self:draw_xyz_sxy(		l+sx/2,b+sy/2,0, sx,sy )	end
function IMG:draw_lt_sxy(		l,t, 	sx,sy )		self:draw_xyz_sxy(		l+sx/2,t-sy/2,0, sx,sy )	end

function IMG:draw_xy_sx( 		x,y,   sx )			self:draw_xy_sxy(		x,y,	sx, math.abs(sx)*self.ry	   ) end
function IMG:draw_xyz_sx( 		x,y,z, sx )			self:draw_xyz_sxy(		x,y,z,	sx, math.abs(sx)*self.ry	   ) end
function IMG:draw_xyz_sx_rotz(	x,y,z, sx, rotz )	self:draw_xyz_sxy_rotz(	x,y,z,	sx, math.abs(sx)*self.ry, rotz ) end
function IMG:draw_xy_sy( 		x,y,   sy )			self:draw_xy_sxy(		x,y,	sy*self.rx, sy	)	end
function IMG:draw_xyz_sy( 		x,y,z, sy, b_flip_x )
	local sx = sy*self.rx
	sx = b_flip_x and -sx or sx
	self:draw_xyz_sxy(	x,y,z,	sx, sy	)
end
function IMG:draw_lb_sx( 		l,b,	sx )		self:draw_lb_sxy(	l, b,	sx, math.abs(sx)*self.ry	)	end
function IMG:draw_lt_sx(		l,t,	sx )		self:draw_lt_sxy(	l, t,	sx, math.abs(sx)*self.ry	)	end

function IMG:assign_bind( bind )
	self.bind = bind
	local sx,sy = aaa.img.get_size( bind )
	self:__set_sxy( sx or 1, sy or 1 )
end

--now that we crop we should eventually change this too
function IMG:switch_version( la )
	local ver = self.versions
	if not ver then return end
	local bind = ver[la]
	if bind then
		self:assign_bind( bind )
	end
end

function IMG:create( filename, bind )
	local self = IMG:create_instance( filename )
	self:assign_bind( bind )
	if filename then	IMG.__by_name[ filename ] = self
	else				self:print_error( "IMG:create() we need a name here" )
	end
	return self
end

function IMG:create_from_filename( filename, bind )
	local self = IMG:create( filename, bind )

	local lang = app and app:get_lang()
	if lang then
		local tab = lang:find_file_translation( filename )
		if tab then
			local ver = self.versions
			if not ver then
				self.versions = {}
				ver = self.versions
				ver[lang:get_def()] = self.bind
			end
			for la, fname in pairs(tab) do
				local bind = IMGS.get_bind( fname, true )
				if bind then
					ver[la] = bind
				end
			end
		end
	end
	return self
end


--
--	IMGS
--
--todo named texture should be here too
if CLASS.DECLARE( "IMGS" ) then
	IMGS:set_class_status_doc(	CLASS.STATUS.CORE,
								"IMGS is more a namespace than a class",
								"here are the entry points to get IMGs" )

	IMGS.__ref_image_bind = aaa.obj.get_by_class_and_name_symbo( "bind", "image_bind" );
end

--todo init/reset where to call it from
function IMGS.init()
	MEDIA.set_dir( "" )

	--IMG:box_debug( "bind inited to "..IMGS.__bind_free )
	IMGS.__bind_by_name = {}
	IMGS.__bind_by_format = {}

	-- bank 16-17 are there for texture dynamic allocation
	IMGS.BIND_FREE_START = 1024 
	-- westart dynamic allocation at 1024: bank 32
	-- in 2022 was VIDEOS:get_bind_max() infact 256 + 128
	--todo		load at start and allocate anywhere
	--			see with BUS

	IMGS.set_bind_free( IMGS.get_bind_free_start() )
end

-- deal with allocating bind
--
-- todo have a mecanism to free ids too
function IMGS.set_bind_free( bind )		IMGS.__bind_free = bind		return bind			end
function IMGS.get_bind_free()			return IMGS.__bind_free							end
function IMGS.inc_bind_free( nb )		
	local bind = IMGS.get_bind_free()
	IMGS.set_bind_free( IMGS.__bind_free + (nb or 1) )
	return bind
end

function IMGS.get_bind_free_start()		return IMGS.BIND_FREE_START					end	


function IMGS.set_force_keep_on_cpu( b )	IMGS.__b_force_keep_on_cpu = b		end
function IMGS.get_force_keep_on_cpu()		return IMGS.__b_force_keep_on_cpu	end

function IMGS.__get_bind( filename, b_error )
	--	FNAME.split_ext( filename )
	--	aaa.print( string.match( filename, "(.-)([^%.\\/]*)" ) )
	local bind = IMGS.__bind_by_name[filename]
	--aaa.print_debug( "--------------- trying to load "..filename )
	if bind then
		--aaa.print_debug( "--------------- found it in bind "..bind )
		return bind
	end

	-- if this image is not already loaded
	local name = MEDIA.make_fname( filename )
	local b = aaa.file.is_exist( name )
	if not b and b_error then
		aaa.print_error( "file don't exist : "..name )
	end
	return b
end

function IMGS.is_exist( filename )
	return IMGS.__get_bind( filename, true  ) ~= false
end
function IMGS.is_exist_error_no( filename )
	return IMGS.__get_bind( filename, false ) ~= false
end

IMGS.doc.get_bind = "( filename, b_move_to_gpu, b_async, b_premultiply )\n"..
						"return the bind of the image is loaded or already exist\n"..
						" filename of the file to be loaded\n"..
						" b_move_to_gpu trigger a copy from cpu to gpu once loaded,\n"..
						"  if nil it is considered as true\n"..	
						" b_async true means an asynchronous load is requested in ths case the copy to the gpu is ignored.\n"..
						"  if nil it is considered as false.\n"..
						" b_premultiply make the r,g,b channels to be multiply by alpha \n"..
						"  if nil it is considered as false."
function IMGS.get_bind( filename, b_move_to_gpu, b_async, b_premultiply )

-- already in a bind ?
	local bind = IMGS.__bind_by_name[filename]
	if bind then
		--aaa.print_debug( "--------------- found it in bind "..bind )
		return bind
	end

	if IMGS.verbose >= 1 then
		aaa.print_debug( "--------------- trying to load "..filename )
	end

-- image not already loaded
	bind = IMGS.get_bind_free()
	local name = MEDIA.make_fname( filename )

	local b = IMGS.__b_force_keep_on_cpu
	if b ~= nil then
		aaa.img.set_cpu_keep( bind, b )
	end

	--aaa.print_inverse( "aaa.img.read( "..bind..", "..name.." )" )
	if aaa.img.read_no_error( bind, name, false, b_async, false, b_premultiply ) then
		if not b_async then
			if b_move_to_gpu == nil or b_move_to_gpu then -- move it to gpu now to avoid stops later
				--aaa.print_inverse( "start gpu transfer" )
				aaa.img.move_to_gpu( bind )
				--aaa.print_inverse( "ended" )
			end
		end
		if IMGS.verbose >= 1 then
			aaa.print_debug( "--------------- Bind "..bind.." Loaded "..filename )
		end
		IMGS.__bind_by_name[filename] = bind
		param.set_save( IMGS.__ref_image_bind, "bind_"..bind, false )
		IMGS.inc_bind_free(1)
		return bind
	else
		aaa.print_error( "can't read "..filename )
		aaa.print_error( " using name "..name )
		local b = aaa.file.is_exist( name )
		if not b then
			aaa.print_error( "file don't exist : "..name )
		end
	end

	return nil
end

function IMGS.assign_bind( filename )
-- already in a bind ?
	local bind = IMGS.__bind_by_name[filename]
	if bind then
		--aaa.print_debug( "--------------- found it in bind "..bind )
		return bind
	end

	if IMGS.verbose >= 1 then
		aaa.print_debug( "--------------- trying to loassign a bind "..filename )
	end

-- image not already loaded
	bind = IMGS.get_bind_free()
	local name = MEDIA.make_fname( filename )

	local b = IMGS.__b_force_keep_on_cpu
	if b ~= nil then
		aaa.img.set_cpu_keep( bind, b )
	end

	--aaa.print_inverse( "aaa.img.read( "..bind..", "..name.." )" )
	-- if aaa.img.read_no_error( bind, name, false, b_async, false, b_premultiply ) then
	-- 	if not b_async then
	-- 		if b_move_to_gpu == nil or b_move_to_gpu then -- move it to gpu now to avoid stops later
	-- 			--aaa.print_inverse( "start gpu transfer" )
	-- 			aaa.img.move_to_gpu( bind )
	-- 			--aaa.print_inverse( "ended" )
	-- 		end
	-- 	end
	-- 	if IMGS.verbose >= 1 then
	-- 		aaa.print_debug( "--------------- Bind "..bind.." Loaded "..filename )
	-- 	end
	if aaa.file.is_exist( name ) then
		IMGS.__bind_by_name[filename] = bind
		local r = param.get_ref( IMGS.__ref_image_bind, "bind_"..bind )
		param.set( r, false )
		param.set_save( r, false )
		IMGS.inc_bind_free(1)
		return bind
	end
	
end

IMGS.doc.get_bind_always = "( filename, b_move_to_gpu, b_async, b_premultiply )\n"..
							" same as get_bind() but always return a bind even is no file is loaded"
function IMGS.get_bind_always( filename, b_move_to_gpu, b_async, b_premultiply )
	local bind = IMGS.get_bind( filename, b_move_to_gpu, b_async, b_premultiply )

	if not bind then
		bind = IMGS.inc_bind_free(1)
		local b = IMGS.__b_force_keep_on_cpu
		if b then
			aaa.img.set_cpu_keep( bind, b )
		end
	end 

	return bind
end

IMGS.doc.get_bind_sxy = "( filename, b_move_to_gpu, b_async, b_premultiply )\n"..
						" same as get_bind() but always return a bind, size_x, size_y if the file is loaded or exists"
function IMGS.get_bind_sxy( filename, b_async )
	local bind = IMGS.get_bind( filename, false, b_async )
	if bind then
		local sx,sy = aaa.img.get_size( bind )
		return bind, sx,sy
	end
	return nil
end

function IMGS.get_bind_sxy_with_error_dialog( filename )
	local bind, sx, sy = IMGS.get_bind_sxy( filename )
	if not bind then
		aaa.box_error( "Can't load image :\n\t"..filename )
	end
	return bind, sx,sy
end


-- more complex approch filename <-> img
--
function IMGS.__get_img_by_name( name )	return IMG.__by_name[name]	end

function IMGS.get_img( filename_in, b_premultiply )
	local fname_in, ext = FNAME.split_ext( filename_in )

	--test if we have it thru a proxy
	if IMGS.verbose >= 1 then
		aaa.print( "IMGS.get_img() asked "..fname_in )
	end
	local img = IMGS.__get_img_by_name( fname_in )
	if img then
		if IMGS.verbose >= 1 then
			aaa.print( "IMGS: found "..fname_in )
		end
		return img
	end
	--test if we have it with the extension
	if IMGS.verbose >= 1 then
		aaa.print( "IMGS.get_img() asked "..filename_in )
	end
	local img = IMGS.__get_img_by_name( filename_in )
	if img then
		if IMGS.verbose >= 1 then
			aaa.print( "IMGS: found "..filename_in )
		end
		return img
	end

	--local bind = IMGS.__bind_by_name[filename]
	--if bind then
	--	--aaa.print_debug( "--------------- found it in bind "..bind )
	--	return bind
	--end

	if IMGS.verbose >= 1 then
		aaa.print_debug( "--------------- trying to load "..fname_in )
	end
	local full_name = MEDIA.make_fname( fname_in )
	local full_name_proxy = full_name..".aaa_img"
	local def
	local fname_to_read
	if aaa.file.is_exist( full_name_proxy ) then
		if IMGS.verbose >= 1 then
			aaa.print( "found a proxy\n"..full_name_proxy )
		end
		def = aaa.dofile( full_name_proxy )
		if IMGS.verbose >= 1 then
			table.print( def, "def" )
		end
		if def.version == 0 then
			local dir_name = aaa.file.get_dir_name( fname_in )
			if IMGS.verbose >= 1 then
				aaa.print( "dir_name "..dir_name )
			end
			fname_to_read = def.img_filename
			if dir_name and dir_name~="" then
				fname_to_read = dir_name.."/"..fname_to_read
			end
			if IMGS.verbose >= 1 then
				aaa.print( "proxy file name "..fname_to_read )
			end
		else
			aaa.print_error( "aaa_img is not version 0" )
			return
		end
	else
		fname_to_read = filename_in
	end
	full_name = MEDIA.make_fname( fname_to_read )	

	local bind = IMGS.get_bind_free()
	local b = IMGS.__b_force_keep_on_cpu
	if b~=nil then
		aaa.img.set_cpu_keep( bind, b )
	end

	--aaa.print_inverse( "aaa.img.read( "..bind..", "..full_name.." )" )
	if aaa.img.read_no_error( bind, full_name, false, false, false, b_premultiply ) then
		--aaa.print_inverse( "start gpu transfer" )
		aaa.img.move_to_gpu( bind )
		--aaa.print_inverse( "ended" )
		if IMGS.verbose >= 1 then
			aaa.print_debug( "--------------- Bind "..bind.." Loaded "..fname_in )
		end
		IMGS.__bind_by_name[fname_in] = bind
		param.set_save( IMGS.__ref_image_bind, "bind_"..bind, false )
		IMGS.inc_bind_free(1)

		if IMGS.verbose >= 1 then
			aaa.print( "IMGS: create at bind "..bind.." "..fname_in )
		end
		local img = IMG:create_from_filename( fname_in, bind ) --todo refine the name ???
		if def then
			img:set_crop_from_def( def.crop )
		end
		return img
	else
		aaa.print_error( "can't read "..full_name )		
	end

end

function IMGS.get_img_premultiply( filename )
	return IMGS.get_img( filename, true )
end



function IMGS.get_img_with_error_dialog( name )
	local img = IMGS.get_img( name )
	if not img then
		aaa.box_error( "could not find image named\n\""..name.."\"" )
	end
	return img
end
function IMGS.get_img_using_ext( filename, ext )
	local img
	for i=1,#ext do
		aaa.print( "try "..filename.."."..ext[i] )
		img = IMGS.get_img( filename.."."..ext[i] )
		if img then
			return img
		end
	end
	--aaa.box_error( "don't find "..filename )
end

function IMGS.get_img_from_bind( bind )
	local name = "IMG_bind_"..bind
	local img = IMGS.__get_img_by_name( name )
	if img then return img end

	return IMG:create( name, bind )
end

function IMGS.get_img_new()
	return IMGS.get_img_from_bind( IMGS.inc_bind_free(1) )
end

function IMGS.switch_version( la )
	for _, img in PAIRS( IMG.__by_name ) do
		img:switch_version( la )
	end
end

