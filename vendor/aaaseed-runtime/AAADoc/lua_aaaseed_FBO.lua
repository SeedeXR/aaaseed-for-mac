-- AAASeed Lua API for the FBO class.
-- Defined in AAAKernel/GaBu/GaBu_Util/FBO/FBO.lua, loaded via GaBu_Util bind order.
--
-- All entries in this file are pure Lua (no C bindings).
--
-- FBO wraps the C++ c_fbo (Frame Buffer Object) for off-screen rendering. It manages
-- pixel size, channel format, up to 8 color attachments plus a depth attachment, and
-- exposes per-attachment control over bind, CPU readback, and mipmap generation.
-- Each FBO also owns a BDD_CLEAR_SCREEN for its background clear.


-- # FBO

-- ## create

-- Constructor. Wraps an existing C++ fbo object reference and its parent layers.
-- Builds internal parameter references for all attachments.

	fbo = FBO:create( name, fbo_obj_ref, layers )

-- ## set_focus

-- Sets the C++ UI focus to this FBO's object (for the property panel).

	fbo:set_focus()


-- # FBO : pixel size

-- ## set_pixel_size

-- Sets the requested FBO resolution. sx,sy can be integers or a PIXEL_SIZE
-- preset string (e.g. "HD", "512", "WQHD"), in which case PIXEL_SIZE:get_sxy()
-- is called to resolve the dimensions.

	fbo:set_pixel_size( sx,sy )
	fbo:set_pixel_size( "HD" )

-- ## get_pixel_size

-- Returns the actual FBO resolution (may differ from asked if hardware-limited).

	sx,sy = fbo:get_pixel_size()


-- # FBO : channel format

-- ## set_channel_nb / get_channel_nb

-- Sets or gets the number of color channels for the FBO (1 to 4).

	fbo:set_channel_nb( nb )
	nb = fbo:get_channel_nb()

-- ## set_channel_type / get_channel_type

-- Sets or gets the channel data type.
-- Common values: 0 = unsigned byte, 1 = unsigned short, 2 = half float, 3 = float.

	fbo:set_channel_type( ch_type )
	ch_type = fbo:get_channel_type()

-- ## set_channel_nb_type

-- Sets both channel count and type in one call.

	fbo:set_channel_nb_type( nb, ch_type )

-- ## get_channel_nb_type

-- Returns both channel count and type.

	nb, ch_type = fbo:get_channel_nb_type()

-- ## get_pixel_size_channel_type

-- Returns sx,sy, channel_nb, channel_type_str in one call.

	sx,sy, nb, ch_type_str = fbo:get_pixel_size_channel_type()


-- # FBO : status

-- ## is_valid

-- Returns true if the FBO is in a valid OpenGL state.

	b = fbo:is_valid()

-- ## is_attach

-- Returns true if the FBO's attachment configuration has been applied.

	b = fbo:is_attach()


-- # FBO : attachments

-- All attachment methods accept an id parameter:
--   1 to 8       : color attachment index
--   "depth"       : the depth attachment
-- Not all operations apply to depth (mipmap and channel_type are color-only).

-- ## set_attachment_active / get_attachment_active

-- Enables or disables an attachment.

	fbo:set_attachment_active( id, b )
	b = fbo:get_attachment_active( id )

-- ## set_attachment_bind / get_attachment_bind

-- Sets or gets the texture bind for an attachment. bind can be an integer
-- or a string name (resolved via TEXS:get_bind_by_name).

	fbo:set_attachment_bind( id, bind )
	bind = fbo:get_attachment_bind( id )

-- ## set_attachment_tex_on_cpu / is_attachment_tex_on_cpu

-- Enables CPU-side readback for an attachment's texture. When enabled, the
-- engine keeps a CPU copy of the rendered pixels for access via aaa.img.*.
-- b_pbo_use (optional) enables PBO-based async transfer for better performance.

	fbo:set_attachment_tex_on_cpu( id, b [, b_pbo_use] )
	b = fbo:is_attachment_tex_on_cpu( id )

-- ## set_attachment_mipmap_generate / is_attachment_mipmap_generate

-- Enables automatic mipmap generation for a color attachment after rendering.
-- Not applicable to the depth attachment.

	fbo:set_attachment_mipmap_generate( id, b )
	b = fbo:is_attachment_mipmap_generate( id )

-- ## set_attachment_channel_type

-- Overrides the channel format for a specific color attachment (not depth).
-- ch_nb and ch_type set the channel count and type. b_define_format enables
-- the per-attachment format override (otherwise the FBO-level format is used).

	fbo:set_attachment_channel_type( id, ch_nb, ch_type, b_define_format )


-- # FBO : depth attachment

-- ## set_attachment_depth_as_texture / get_attachment_depth_as_texture

-- Controls whether the depth buffer is stored as a texture (readable by shaders)
-- or as a renderbuffer (faster but not readable).

	fbo:set_attachment_depth_as_texture( b )
	b = fbo:get_attachment_depth_as_texture()


-- # FBO : clear screen

-- ## get_bdd_clear_screen

-- Returns the BDD_CLEAR_SCREEN object attached to this FBO. Use it to
-- configure the background clear color and mode.

	bdd_cs = fbo:get_bdd_clear_screen()


-- # FBO : rendering

-- ## update

-- Triggers aaa.obj.update on the underlying C++ FBO object.

	fbo:update()

-- ## draw

-- Triggers aaa.obj.draw on the underlying C++ FBO object.

	fbo:draw()
