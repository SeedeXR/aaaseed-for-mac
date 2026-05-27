-- AAASeed Lua API for the IMG and IMGS classes.
-- Defined in AAAKernel/GaBu/GaBu_Util/IMG/img.lua, loaded via GaBu_Util bind order.
--
-- All entries in this file are pure Lua (no C bindings).
--
-- IMG wraps a single GPU texture: a bind ID, pixel dimensions, aspect ratios,
-- optional crop region, and optional per-language texture variants.
-- IMGS is a singleton namespace (not instantiated) that manages bind allocation,
-- image loading, and IMG object creation. In normal usage you ask IMGS for an
-- image by filename and it returns an IMG, loading the file on first request.


-- # IMG

-- An IMG instance represents one loaded texture on the GPU.
-- It holds a bind (OpenGL texture unit), pixel dimensions (sx,sy),
-- precomputed aspect ratios (rx = sx/sy, ry = sy/sx), and an optional
-- crop definition that remaps UV coordinates for atlas-style textures.

-- ## create

-- Low-level constructor. Registers the IMG by filename in the global registry.
-- Calls assign_bind to read dimensions from the GPU.
-- Prefer IMGS.get_img() in application code.

	img = IMG:create( filename, bind )

-- ## create_from_filename

-- Extended constructor with multi-language support. If an APP is running and
-- has a LANG object, scans for translated versions of the image file and
-- stores their binds in self.versions. Falls back to create() otherwise.

	img = IMG:create_from_filename( filename, bind )

-- ## assign_bind

-- Replaces the texture bind and refreshes dimensions and aspect ratios
-- from the GPU via aaa.img.get_size(). Use this to re-target an IMG to
-- a different texture at runtime.

	img:assign_bind( bind )


-- # IMG : queries

-- ## get_bind

-- Returns the OpenGL texture bind ID.

	bind = img:get_bind()

-- ## get_sx / get_sy / get_sxy

-- Pixel dimensions. get_sxy returns both as two values.

	sx = img:get_sx()
	sy = img:get_sy()
	sx,sy = img:get_sxy()

-- ## get_rx / get_ry

-- Aspect ratios. rx = sx/sy (width relative to height),
-- ry = sy/sx (height relative to width).

	rx = img:get_rx()
	ry = img:get_ry()


-- # IMG : crop

-- ## set_crop_from_def

-- Configures a crop region for atlas-style rendering. crop_info is a table
-- with fields: left, right, bottom, top (pixel coordinates of the sub-region
-- inside the atlas), size_x, size_y (logical size of the full image).
-- After calling this, all draw methods automatically offset and scale UVs
-- to render only the cropped region, while reporting size_x/size_y as the
-- image dimensions.

	img:set_crop_from_def( crop_info )

-- crop_info example:
--   { left=100, bottom=50, right=355, top=305, size_x=512, size_y=512 }


-- # IMG : draw methods

-- All draw methods bind the texture and emit a textured quad.
-- Coordinates are in world/screen space. Sizes are in world units.
-- The image is centered on (x,y) unless an lb/lt variant is used.
-- Crop is applied automatically when set.

-- ## draw_sxy

-- Draw centered at origin with explicit width and height.

	img:draw_sxy( sx,sy )

-- ## draw_sx / draw_sy

-- Draw centered at origin, specifying one dimension. The other dimension
-- is computed from the aspect ratio to avoid distortion.

	img:draw_sx( sx )
	img:draw_sy( sy )

-- ## draw_xy_sxy

-- Draw centered at (x, y) with explicit size.

	img:draw_xy_sxy( x,y, sx,sy )

-- ## draw_xyz_sxy

-- Draw centered at (x, y, z) with explicit size. The z coordinate
-- sets the depth for the textured quad.

	img:draw_xyz_sxy( x,y,z, sx,sy )

-- ## draw_xyz_sxy_rotz

-- Draw centered at (x, y, z) with explicit size and a rotation around
-- the Z axis (radians). Crop is not supported with rotation (prints error).

	img:draw_xyz_sxy_rotz( x,y,z, sx,sy, rot_z )

-- ## draw_xy_sx / draw_xy_sy

-- Draw centered at (x, y), specifying one dimension (the other from aspect ratio).

	img:draw_xy_sx( x,y, sx )
	img:draw_xy_sy( x,y, sy )

-- ## draw_xyz_sx / draw_xyz_sy

-- Draw centered at (x, y, z), specifying one dimension.
-- draw_xyz_sy accepts an optional b_flip_x to mirror horizontally.

	img:draw_xyz_sx( x,y,z, sx )
	img:draw_xyz_sy( x,y,z, sy [, b_flip_x] )

-- ## draw_xyz_sx_rotz

-- Draw centered at (x, y, z) with width sx and Z-axis rotation.

	img:draw_xyz_sx_rotz( x,y,z, sx, rot_z )

-- ## draw_lb_sxy / draw_lb_sx

-- Draw with left-bottom corner at (l, b). The image extends rightward and upward.

	img:draw_lb_sxy( l,b, sx,sy )
	img:draw_lb_sx( l,b, sx )

-- ## draw_lt_sxy / draw_lt_sx

-- Draw with left-top corner at (l, t). The image extends rightward and downward.

	img:draw_lt_sxy( l,t, sx,sy )
	img:draw_lt_sx( l,t, sx )

-- ## draw_rects_xyz_sxy

-- Debug helper: draws the bounding rectangle outline of the image and, if cropped,
-- the crop sub-rectangle outline as well. Optional b_flip_x mirrors horizontally.
-- If sx or sy is nil, the missing dimension is computed from the aspect ratio.

	img:draw_rects_xyz_sxy( x,y,z, sx,sy [, b_flip_x] )


-- # IMG : multi-language

-- ## switch_version

-- Switches this IMG to the texture variant for the given language code.
-- Only effective if create_from_filename found translated versions.
-- Calls assign_bind internally, so dimensions and ratios are refreshed.

	img:switch_version( lang_code )


-- # IMGS

-- IMGS is a singleton namespace (declared as a class but never instantiated).
-- It manages dynamic texture bind allocation and image loading.
-- All functions are called with dot syntax: IMGS.get_bind(...).

-- ## init

-- Initializes the bind allocator and clears the image registry.
-- Called once at startup. Sets the free-bind counter to BIND_FREE_START (1024).

	IMGS.init()

-- ## Bind allocation

-- Dynamic bind IDs start at BIND_FREE_START (1024). Each loaded image
-- consumes one bind. The allocator is a simple incrementing counter.
-- There is currently no mechanism to free individual binds.

	IMGS.BIND_FREE_START                           -- 1024
	bind = IMGS.get_bind_free()                    -- current free bind
	IMGS.set_bind_free( bind )                     -- set free bind counter
	bind = IMGS.inc_bind_free( [nb] )              -- consume nb binds (default 1), returns first allocated


-- # IMGS : loading images

-- ## get_bind

-- Main entry point for texture loading. Returns the bind if already loaded.
-- Otherwise reads the file via MEDIA path resolution, uploads to GPU
-- (unless b_async or b_move_to_gpu is false), registers the bind, and
-- returns it. Returns nil on failure (prints error).
--
-- b_move_to_gpu: trigger GPU upload after load (default true).
-- b_async: request asynchronous load, skips GPU upload (default false).
-- b_premultiply: multiply RGB by alpha on load (default false).

	bind = IMGS.get_bind( filename [, b_move_to_gpu [, b_async [, b_premultiply]]] )

-- ## get_bind_always

-- Same as get_bind but always returns a bind ID even if the file cannot be
-- loaded. Useful when you need a valid bind slot regardless of load success.

	bind = IMGS.get_bind_always( filename [, b_move_to_gpu [, b_async [, b_premultiply]]] )

-- ## get_bind_sxy

-- Loads the image (or retrieves from cache) and returns bind, sx,sy.
-- Returns nil if the file cannot be loaded.

	bind, sx,sy = IMGS.get_bind_sxy( filename [, b_async] )

-- ## assign_bind

-- Reserves a bind for a filename without actually loading the file.
-- The file must exist on disk (checked via MEDIA path). The bind is
-- registered in the cache so subsequent get_bind calls return it.
-- Useful for deferred or manual loading workflows.

	bind = IMGS.assign_bind( filename )

-- ## is_exist

-- Returns true if the filename is already loaded or the file exists on disk.

	b = IMGS.is_exist( filename )


-- # IMGS : getting IMG objects

-- ## get_img

-- High-level entry point. Returns an IMG object for the given filename.
-- Checks the IMG registry first (by name without extension, then with extension).
-- If not found, loads from disk with proxy support: if a .aaa_img sidecar file
-- exists, it is executed to get crop and redirect info (proxy version 0).
-- The returned IMG has crop applied if the proxy defines one.
-- b_premultiply multiplies RGB by alpha during loading.

	img = IMGS.get_img( filename [, b_premultiply] )

-- ## get_img_premultiply

-- Shortcut for IMGS.get_img( filename, true ).

	img = IMGS.get_img_premultiply( filename )

-- ## get_img_from_bind

-- Wraps an existing GPU bind into an IMG object. If an IMG for this bind
-- already exists, returns it. Otherwise creates a new one named "IMG_bind_N".

	img = IMGS.get_img_from_bind( bind )

-- ## get_img_new

-- Allocates a fresh bind and wraps it in a new IMG. The bind has no image
-- data yet: use aaa.img.* to fill it, or assign it as an FBO attachment.

	img = IMGS.get_img_new()

-- ## get_img_using_ext

-- Tries loading filename with each extension in the ext array until one succeeds.
-- Returns the first IMG found or nil.

	img = IMGS.get_img_using_ext( filename, ext_array )

-- ## get_img_with_error_dialog

-- Same as get_img but shows an error dialog box if the image cannot be found.

	img = IMGS.get_img_with_error_dialog( filename )


-- # IMGS : multi-language

-- ## switch_version

-- Switches all registered IMG objects to the texture variant for the given
-- language code. Iterates the global IMG.__by_name registry.

	IMGS.switch_version( lang_code )


-- # IMGS : CPU memory control

-- ## set_force_keep_on_cpu

-- When set to true, all subsequent image loads keep a CPU-side copy of the
-- pixel data in addition to the GPU texture. Useful for readback, CPU-side
-- processing, or saving. Default is nil (engine default behavior).

	IMGS.set_force_keep_on_cpu( b )
