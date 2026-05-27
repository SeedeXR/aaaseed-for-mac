-- AAASeed Lua API for the TEXT class.
-- Defined in AAAKernel/GaBu/GaBu_Util/TEXT/TEXT.lua, loaded via GaBu_Util bind order.
--
-- All entries in this file are pure Lua (no C bindings).
--
-- TEXT renders text strings using the C++ bdd_text object. It supports multiple
-- font backends: MAA line, GLUT line, textured, buffer, outline, polygon,
-- extruded, and SDF (Signed Distance Field). A single shared C++ layer
-- (gabu_text_layer) is reused across all TEXT instances via static references.
-- TEXT drives the bdd_text parameters (font, size, alignment, etc.) and triggers
-- update_then_draw on the layer to produce the rendered output.


-- # TEXT

-- ## Font type constants

-- The following constants select the rendering backend (zero-based, matching the
-- C++ param). They are local to the file but referenced by set_font_texture,
-- set_font_sdf, and set_font_type.
--
--   0: MAA_LINE       line font (Maa's custom)
--   1: GLUT_LINE      line font (GLUT stroke)
--   2: FONT_TEXTURE   textured quads (fast, commonly used)
--   3: FONT_BUFFER    buffer-based
--   4: FONT_OUTLINE   outline curves
--   5: FONT_POLYGON   filled polygons
--   6: FONT_EXTRUDE   extruded 3D glyphs
--   9: FONT_SDF       Signed Distance Field (quality + scaling)


-- ## create

-- Constructor. Returns a TEXT instance with default settings:
-- font_index=0, alignment="left", alignment_vert="line_first",
-- justification="left", interligne=1.0, line_length=320,
-- face_dpi=72, face_size=96, draw_bbox=false.

	txt = TEXT:create( name )

-- ## set_font_texture

-- Switches to the FONT_TEXTURE backend and sets the font index.
-- Textured fonts are the fastest path for UI text.

	txt:set_font_texture( font_index )

-- ## set_font_sdf

-- Switches to the FONT_SDF backend and sets the font index.
-- SDF fonts scale cleanly at any size and support outline/shadow effects.

	txt:set_font_sdf( font_index )

-- ## set_font_type

-- Switches the font backend by name string. Accepted values (case-insensitive):
-- "maa_line", "glut_line", "font_texture", "font_buffer", "font_outline",
-- "font_polygon", "font_extrude", "font_sdf". Also accepts "font_nice" as
-- an alias for "font_outline".

	txt:set_font_type( type_str )

-- ## get_font_type

-- Returns the current font backend name as an uppercase string.

	str = txt:get_font_type()


-- # TEXT : font configuration

-- ## set_font / get_font

-- Sets or gets the font index (0 to 64). The index selects among the fonts
-- loaded by the engine. Which font corresponds to which index depends on
-- the font files installed in AAAKernel/Fonts/.

	txt:set_font( index )
	index = txt:get_font()

-- ## set_face_size / get_face_size

-- Sets or gets the font face size in points. Default is 96.

	txt:set_face_size( val )
	val = txt:get_face_size()

-- ## set_face_dpi / get_face_dpi

-- Sets or gets the font DPI. Default is 72.

	txt:set_face_dpi( val )
	val = txt:get_face_dpi()


-- # TEXT : layout

-- ## set_justification / get_justification

-- Text justification within the text block.
-- Values: "left" (default), "center", "right", "justify".

	txt:set_justification( val )
	val = txt:get_justification()

-- ## set_alignment / get_alignment

-- Horizontal alignment of the text origin point.
-- Values: "left" (default), "center", "right", "justify".

	txt:set_alignment( val )
	val = txt:get_alignment()

-- ## set_alignment_vert / get_alignment_vert

-- Vertical alignment of the text origin point.
-- Values: "line_first" (default), "bottom", "middle", "top".

	txt:set_alignment_vert( val )
	val = txt:get_alignment_vert()

-- ## set_interligne / get_interligne

-- Line spacing multiplier. 1.0 = single spacing. Default is 1.0.

	txt:set_interligne( val )
	val = txt:get_interligne()

-- ## set_line_length / get_line_length

-- Maximum line length for word wrapping (in font units). Default is 320.

	txt:set_line_length( val )
	val = txt:get_line_length()

-- ## set_draw_box / get_draw_box

-- Toggles rendering of the text bounding box outline (debug aid).

	txt:set_draw_box( b )
	b = txt:get_draw_box()


-- # TEXT : color

-- ## set_color

-- Sets the text color (RGBA). Used by both textured and SDF font paths.

	txt:set_color( r,g,b, a )

-- ## set_color_sdf

-- Alias for set_color. Historically specific to SDF, now identical.

	txt:set_color_sdf( r,g,b, a )


-- # TEXT : metrics

-- ## get_width

-- Returns the bounding box width of the last rendered text.
-- Read after a draw call for accurate measurement.

	w = txt:get_width()

-- ## get_height

-- Returns the bounding box height of the last rendered text.

	h = txt:get_height()


-- # TEXT : drawing

-- ## draw

-- Renders a text string. id is a buffer identifier (used by the BU widget system
-- to manage multiple concurrent text draws). x, y are the origin in world units.
-- sx, sy are the scale factors.

	txt:draw( id, text, x,y, sx,sy )

-- ## set_next_shadow

-- Enables shadow rendering for the next draw call only. The shadow is drawn
-- as a dark offset copy behind the main text. Resets automatically after one draw.

	txt:set_next_shadow()

-- ## set_next_fit

-- Enables fit-to-box for the next draw call only. su, sv define the maximum
-- bounding box: the text is scaled down if it would exceed these dimensions.
-- Resets automatically after one draw.

	txt:set_next_fit( su, sv )

-- ## update_bdd_param

-- Pushes all cached TEXT settings (font, alignment, size, etc.) to the C++
-- bdd_text parameters. Normally called internally before draw, but can be
-- called manually when driving the bdd_text from outside TEXT.

	txt:update_bdd_param()
