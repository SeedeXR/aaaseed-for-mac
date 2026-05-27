function meu:define_meu_infos( )
	return	{	author = "Mâa",
				help = "Preparatory work for Numa opening",
				tags = { "2d", "Art", "Draw", "Procedural", "unfinished" }
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()

--	self.b_draw_countdown = true
--	bu = self:add_button(	{1,	1 },	"draw_countdown",	self, "b_draw_countdown"			)
--		bu:set_value_load_save( false )

--	self.speed = 1
--		bu = self:add_slider(	{1,7,	8,1}, "speed", self, "speed" )
--		bu:set_min_max( 0, 4. )
end

function meu:draw_frame()
	local	sx	=	self.sx
	local	sy	=	self.sy
	gol.draw_lines_2d(	-sx*.5,	-sy,
						-sx*.5,  sy,
						 sx*.5, -sy,
						 sx*.5,  sy,
						-sx,	-sy*.5,
						 sx,	-sy*.5,
						-sx,	 sy*.5,
						 sx,	 sy*.5
					)
end
function meu:update_dot()
	local band = self.band
	local sx = band.sx * .5
	local sy = band.sy * .5
	local dot = self.dot
	dot.a = { x=-sx,  y=sy }
	dot.b = { x=sx,  y=sy }
	dot.c = { x=sx,  y=-sy }
	dot.d = { x=-sx,  y=-sy }

	local d = self.char_dy
	dot.ab = { x=dot.a.x,  y=dot.a.y-d }
	dot.bb = { x=dot.b.x,  y=dot.b.y-d }
	dot.cb = { x=dot.c.x,  y=dot.c.y+d }
	dot.db = { x=dot.d.x,  y=dot.d.y+d }
end

function meu:set_size_def()
	local band = self.band
	band.sy		=	1.
	band.sx		=	band.sy * 8/24
	band.dx		=	band.sx / 4
	self.char_dist	=	band.sy * 6/24
	self.char_dy	=	band.sy * 4/24 * math.sin( aaa.time.t * 10.)

	self.sx =	band.sx * 9 + band.dx * 5 + self.char_dist * 3
	self.sy =	band.sy

	self.translate_band_x	=	band.sx + band.dx
	self.translate_letter_x	=	self.char_dist - band.dx

	self:update_dot()
end
function meu:center_numa()		gol.translate( -self.sx*.5 + self.band.sx * .5 )	end
function meu:translate_letter()	gol.translate( self.translate_letter_x )			end
function meu:translate_band()	gol.translate( self.translate_band_x )				end

function meu:draw_generic( fn )
	gol.push_matrix()
		self:center_numa()
		local l = self.letter
		fn( self, l.n )
		self:translate_letter()
		fn( self, l.u )
		self:translate_letter()
		fn( self, l.m )
		self:translate_letter()
		fn( self, l.a )
	gol.pop_matrix()
end

function meu:draw_fan( f )
	local dot = self.dot
	gol.begin_triangle_fan()
	for i=1,#f do
		--aaa.print( f[i] )
		local d = dot[ f[i] ]
		gol.vertex3( d.x, d.y, d.z )
	end
	gol.do_end()
end
function meu:draw_letter( l )
	for i=1,#l do
		self:draw_fan( self.fan[ l[i] ] )
		self:translate_band()
	end
end
function meu:draw_fan_line( f )
	local dot = self.dot
	gol.begin_line_loop()
	for i=1,#f do
		--aaa.print( f[i] )
		local d = dot[ f[i] ]
		gol.vertex3( d.x, d.y, d.z )
	end
	gol.do_end()
end
function meu:draw_letter_line( l )
	for i=1,#l do
		self:draw_fan_line( self.fan[ l[i] ] )
		self:translate_band()
	end
end
function meu:draw_numa()		self:draw_generic( self.draw_letter )	end
function meu:draw_numa_line()	self:draw_generic( self.draw_letter_line )	end

function meu:draw_band( l )
	local	sy	=	self.sy
	local	band	=	self.band
	local 	sx		=	band.sx * .5
	for i=1,#l do
		gol.draw_lines_2d(	-sx, -sy,
							-sx,  sy,
							 sx, -sy,
							 sx,  sy
						)
		self:translate_band()
	end
end
function meu:draw_numa_band()	self:draw_generic( self.draw_band )	end

function meu:draw()
	MEU.draw( self )
	--gol.bind_texture( self.bind )
	--aaa.print( self.bind )
	self:set_size_def()

	--self:draw_frame()
	--self:draw_numa_band()
	self:draw_numa()
	--self:draw_numa_line()
end

function meu:init()
	self.band	=	{}
	self.dot	=	{}
	local fan = {}
	fan[1]	=	{ "c", "b", "a", "d" }
	fan[2]	=	{ "c", "b", "ab", "d" }
	fan[3]	=	{ "db", "c", "b", "a" }
	fan[4]	=	{ "cb", "b", "a", "d" }
	fan[5]	=	{ "c", "b", "ab", "db" }
	self.fan	=	fan
	local letter = {}
	letter.n =	{ 1, 2 }
	letter.u =	{ 3, 4 }
	letter.m =	{ 1, 2, 2 }
	letter.a =	{ 5, 1 }
	self.letter	=	letter
	self:set_size_def()
end

function meu:draw_icon()
	--gol.color_white()
	gol.push_matrix()
		gol.translate( -.35 )
		gol.scale( 1/16., .3 )
		--todo deal with proto / inst init
		if not self.band then
			self:init()
			self:set_size_def()
		end
		self:draw_numa()
	gol.pop_matrix()
end
