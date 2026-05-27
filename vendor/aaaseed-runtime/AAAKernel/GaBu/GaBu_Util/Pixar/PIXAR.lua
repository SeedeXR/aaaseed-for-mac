
--PIXAR
if CLASS.DECLARE( "PIXAR" ) then
	PIXAR:set_class_status_doc(	CLASS.STATUS.GABU,
								"draw text with pixel typo" )
	--aaa.box_good( "CLASS.DECLARE PIXAR"  )
	function PIXAR:define_font( dir )

		if self.dir == nil then self.dir = "Texture/Ascii_bitmap/" end
		self.fonts = {}
		local font
		font = { filename = "ascii.png" }
			font.b_vertical = true
			font.start_x = 67
			font.start_y = 351
			font.size_x = 20
			font.size_y = 20
			font.stride_x = 2
			font.stride_y = 2
			font.color_pix = 0.2
			font.pix_x = 8
			font.pix_y = 9
			self.fonts[1] = font

		font = { filename = "217px-ASCII_full.png" }
			font.b_vertical = false
			font.start_x = 3
			font.start_y = 163
			font.size_x = 13
			font.size_y = 27
			font.stride_x = 0.3
			font.stride_y = 0
			font.color_pix = 0.8
			font.pix_x = 25
			font.pix_y = 20
			self.fonts[2] = font

		--Fichier pas terrible
		font = { filename = "fig4-23.png" }
			font.b_vertical = true
			font.start_x = 152
			font.start_y = 619
			font.size_x = 30
			font.size_y = 29.5
			font.stride_x = 1.4
			font.stride_y = 8.8
			font.color_pix = 0.2
			font.pix_x = 7
			font.pix_y = 10
			self.fonts[3] = font

		font = { filename = "font5x7lcd.png" }
			font.b_vertical = false
			font.start_x = 31
			font.start_y = 349
			font.size_x = 15
			font.size_y = 21
			font.stride_x = 10
			font.stride_y = 4
			font.color_pix = 0.2
			font.pix_x = 15
			font.pix_y = 21
			self.fonts[4] = font

		font = { filename = "font5x7lcd.png" }
			font.b_vertical = false
			font.start_x = 31
			font.start_y = 349
			font.size_x = 15
			font.size_y = 21
			font.stride_x = 10
			font.stride_y = 4
			font.color_pix = 0.2
			font.pix_x = 5
			font.pix_y = 7
			self.fonts[5] = font

			--aaa.box_good( "fonts 1 to 5 " )
		--A REMPLACER PAR UN TABLEAU
		--self.init(39,false,26,349,23,22,2,3,0.2)
	end
	--aaa.box_good( "PIXAR:define_font()" )
	PIXAR:define_font()
	PIXAR.ref = {}
	local ref = PIXAR.ref
	ref.layers		=	aaa.layers.get_cur()
	ref.cam			=	aaa.obj.get_down( ref.layers, "pixar_cam" )
		ref.cam_y		= param.get_ref( ref.cam, "center_y" )
	ref.render		=	aaa.obj.get_down( ref.layers, "pixar_render" )
		ref.light		= param.get_ref( ref.render, "light" )
		ref.front_mode	= param.get_ref( ref.render, "front_mode" )
		--secu
		param.set( ref.render, "cull", 1 )
		param.set( ref.render, "clockwise", false )

	ref.color	= COLOR_REF:create(  "PIXAR", aaa.obj.get_by_class_and_name_symbo( "color", "pixar_color" ) )
end

function PIXAR:create( name, font_id )
	local self = PIXAR:create_instance( name )
	self:init( font_id )
	self:set_rgba( 0, 1, 0, 1 )
	self:set_function_draw_pixel( self.draw_pixel_square )
	return self
end

function PIXAR:set_rgba( r, g, b, a )
	if r then
		self.rgba = { r, g, b, a }
	else
		self.rgba = nil
	end
end

-- FONCTION POUR REMPLIR LE TABLEAU ASCII
function PIXAR:set_font( font_id )
	font_id = font_id and font_id or 5
	local font = PIXAR.fonts[ font_id ]	-- 5 is default font
	if not font then
		self:box_error( "No font for font_id "..font_id )
		return
	end
	--pixar.bind_index = font.bind
	self.filename = font.filename
	local fname = aaa.dir.get_def().."/"..self.dir..font.filename
	font.bind = IMGS.get_bind( fname, false )
	if not font.bind then
		self:box_error( "can't find \""..fname.."\" for font_id "..font_id )
		self:box_error( "dir def is "..aaa.dir.get_def() )
		self:box_error( "dir kernel is "..aaa.dir.get_dir_kernel() )
		self:box_error( "dir start is "..aaa.dir.get_dir_start() )
		return
	end
	--Voir pour fixer l'emplacement des png !!!!
	local bind = font.bind
	local sx, sy = aaa.img.get_size( bind )
	self:print( bind.." -> "..sx..","..sy )
	--sy = sy + 1
	--sx = sx - 3
	self.b_vertical = font.b_vertical
	self.start_x = font.start_x
	self.start_y = font.start_y
	self.size_x = font.size_x
	self.size_y = font.size_y
	self.pix_x = font.pix_x
	self.pix_y = font.pix_y
	local step_x = self.size_x / font.pix_x
	local step_y = self.size_y / font.pix_y
	self.stride_x = font.stride_x
	self.stride_y = font.stride_y
	self.square_size_over_y = 1 / font.pix_y
	self.char = {}
	local start_x = self.start_x
	local start_y = self.start_y
	local size_x = self.size_x
	local size_y = self.size_y

	aaa.img.set_lua_cur( bind )
	for i = 32,127 do
		local mt = {}
		local nb_pix = 1
		--self:print("  i    "..i)
		for j = start_x, start_x+size_x, step_x do
			for k = start_y, start_y-size_y, -step_y do
				local r,g,b,a = aaa.img.get_color_xy( nil, j, k )
				--if r ~= 1 then
				--self:print( "r "..r.." "..bind.." -> "..j..","..k )
				if font.color_pix < .5 then
					if r <= font.color_pix then
					--if r ~= 0 then
						--self:print("r  "..r)
						local temp_x = j - start_x
						local temp_y = k - start_y
						local f = (size_y > size_x) and size_y or size_x
						f = 1. / f
						temp_x = temp_x * f
						temp_y = temp_y * f
						mt[nb_pix] = {x = temp_x, y = temp_y}
						nb_pix = nb_pix + 1
					end
				else
					if  (r >= font.color_pix) then
					--if r ~= 0 then
						--self:print("r  "..r)
						local temp_x
						local temp_y
						if size_y > size_x then
							temp_x = (j-start_x) / size_y
							temp_y = (k-start_y) / size_y
						else
							temp_x = (j-start_x) / size_x
							temp_y = (k-start_y) / size_x
						end
						mt[nb_pix] = { x = temp_x, y = temp_y }
						nb_pix = nb_pix + 1
					end
				end
			end
		end
		self.char[i] = mt
		if self.b_vertical then
			if i==47 or i==63 or i==79 or i==95 or i==111 then
				start_x = start_x + size_x + self.stride_x
				start_y = self.start_y
			else
				start_y = start_y - size_y - self.stride_y
			end
		else
			if i==47 or i==63 or i==79 or i==95 or i==111 then
				start_y = start_y - size_y - self.stride_y
				start_x = self.start_x
			else
				start_x = start_x + size_x + self.stride_x
			end
		end
	end
	--pixar.pix_draw(pixar.char[65])
	--local tab = pixar.string_tab("AAASeed\nGeode",-4,2,8,2)
	--pixar.string_draw(tab)
end

--	TERMINAL
function PIXAR:clear_terminal()
	self.terminal.tab = {}
end
function PIXAR:init_terminal()
	self.terminal = {}
	self:clear_terminal()
end
function PIXAR:print_terminal( x, y, str )
	if not str then
		self:print_error( "print_terminal() : received nil as str" )
		return
	end

	local term = self.terminal
	local nb_elt =  #term.tab
	local x_start = x
	for i=1, string.len(str) do
		local ascii = string.byte(str, i)
		if ascii == 10 then
			y = y + 1
			x = x_start
		else
			local index = nil
			local tab = term.tab
			for i=1,nb_elt do
				local elt = tab[i]
				if elt.x == x and elt.y == y then
					index = i
					break
				end
			end
			if not index then
				nb_elt = nb_elt + 1
				index = nb_elt
			end
			term.tab[index] = { ascii = ascii, x = x, y = y }
			x = x + 1
		end
	end
end
function PIXAR:set_pos_terminal( x, y )
	self.terminal.x = x
	self.terminal.y = y
end
--unesed for now
function PIXAR:set_scale_terminal( sx, sy, sz )
	self.terminal.sx = sx
	self.terminal.sy = sy
	self.terminal.sz = sz
end

function PIXAR:set_char_size( char_sx, char_sy, step_sx, step_sy )
	self.terminal.char_sx = char_sx
	self.terminal.char_sy = char_sy
	self.terminal.step_sx = step_sx
	self.terminal.step_sy = step_sy
	--self.terminal.ratio_x = 1/char_sx
	--self.terminal.ratio_y = 1/char_sy
end

function PIXAR:draw_pixel_square( id, x, y, sx, sy )
--	local z = math.sin( x_min ) + math.sin( y_min )
--	z = z
--	z = math.sin( z + aaa.time.t * 5. ) * .1
--	z = math.random() * .1
	--aaa.debug.print_traceback()
	x = x - sx *.5
	y = y - sy *.5
	aaa.draw_rect( x, y, x + sx, y + sy )
end
function PIXAR:set_function_draw_pixel( fn )
	self.__draw_pixel = fn
end

function PIXAR:draw_char( ascii, x_in, y_in )

	local tab = self.char[ascii]
	if not tab then return end
	local length = #tab
	if length < 1 then return end

	local term = self.terminal
	local sx = term.char_sx
	local sy = term.char_sy
	local fn = self.__draw_pixel

	--local D = .125
	--aaa.draw_rect_line( startx-D, starty-D, startx+D, starty+D )
	--aaa.draw_rect( startx-D, starty-D, startx+D, starty+D )
	--aaa.draw_null( startx, starty, 0, .125 )
	--aaa.draw_str_maa( string.char(ascii) )
	--aaa.draw_str_maa( "toto" )

	local csx = self.step_x
	local csy = self.step_y
	local id = self.id_char
	for j = 1,length do
		local t = tab[j]
		local y = (t.y * sy) + y_in
		if -5<y and y<5 then
			local x = (t.x * sx) + x_in
			--aaa.draw_disk_axe_z( x, y, 0, self.step_x*1. )
			fn( self, id, x, y, csx, csy, t.x, 1.+t.y )
			--self:draw_pixel_square( id, x, y, .035, .05 )
			--aaa.draw_null_2d( x, y, 0, .1, 2 )
			id = id + 357
		end
	end

end
function PIXAR:set_line_by_unit( line_by_unit )
	if line_by_unit == nil then
		line_by_unit = 1
	end
	local factor = 1/  line_by_unit
	--todo
	self:set_char_size( 0.5*factor, 0.7*factor, 0.*factor, 0.2*factor )
end
--todo deal with texture and/or light and color
function PIXAR:draw_text()
	local term = self.terminal
	local tab = term.tab
	if #tab < 1 then return end

	self.step_x = self.square_size_over_y * term.char_sx
	self.step_y = self.square_size_over_y * term.char_sy

	for i=1, #tab do
		self.id_char = i
		local elt = tab[i]
		local x = term.x + ( elt.x * ( term.char_sx + term.step_sx ) )
		local y = term.y - ( elt.y * ( term.char_sy + term.step_sy ) )
			--self:print( "zuzu "..i.." - "..elt.x..","..elt.y.." -> "..x..","..y.."  "..string.char(elt.ascii) )
		if -5<y and y<5 then
			self:draw_char( elt.ascii,	x, y )
		end
	end

end

function PIXAR:prepare_draw( line_by_unit )
	self:set_line_by_unit( line_by_unit )
	--self:print( "draw()" )

	--todo use refs
	local ref = PIXAR.ref
	--HACK was there in EO
	--param.set( ref.cam_y, 		self.cam_y )

	param.set( ref.light,		self.b_light and 1 or 0 )
	param.set( ref.front_mode,	self.b_line	 and 1 or 0 )

	if self.rgba then
		ref.color:set_rgba_t( self.rgba )
	end

end

function PIXAR:draw( line_by_unit )
	self:prepare_draw( line_by_unit )

	local term = self.terminal
	local tab = term.tab
	if #tab < 1 then return end

--	gol.push_attrib()
	--gol.reset()
	--gol.set_default()
	--param.set( ref.front_mode, 0 )

	local layers = PIXAR.ref.layers
	--aaa.obj.update_then_draw( self.ref.cam )

	--HACK was there in EO
	aaa.layers.draw_begin( layers )
	aaa.layers.draw_layer_all( layers )

	--gol.scale( .5 )
	--aaa.draw_axe_and_plane( 4 )	gol.color( unpack(self.rgba) )
	self:draw_text()


	aaa.layers.draw_end( layers )
	aaa.layers.skip_rest( layers )
--	gol.pop_attrib()
end

function PIXAR:set_y_cam( y )		self.y_cam = y		end
function PIXAR:set_light( b_on )	self.b_light = b_on	end
function PIXAR:set_line( b_on )		self.b_line = b_on	end

function PIXAR:init( font_id )
	self:init_terminal()
	self:set_font( font_id )
	self:set_pos_terminal( -7, 4 )
--	local FACTOR = ( 8 / 16 ) / .8
--	self:set_char_size( FACTOR, 1.4*FACTOR, 0.4*FACTOR, 0.2*FACTOR )
	self.b_init = true
	self.y_cam = 0
	self.b_light = false
end

function PIXAR:test()
	--terminal_set_pos(-7, 4 )
	--terminal_set_char_size( 0.5, 0.7, 0.1, 0.2 )
	--terminal_print( 2, 5, "jkhjzkhaj\nssss")
	self:clear_terminal()
	for i = 1, 16 do
		self:print_terminal( 0, i, "ligne "..i )
	end
	--	terminal_print( 5, 3, "blabl\nssss")
	--terminal_clear()
end

--pixar_init()
--pixar_test()
--pixar:draw()
