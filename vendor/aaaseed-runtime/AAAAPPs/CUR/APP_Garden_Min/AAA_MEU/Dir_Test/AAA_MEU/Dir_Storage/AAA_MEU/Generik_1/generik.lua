function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ui = self.ui

	self:add_camera()

	local ix, iy = 1,1
	self.speed = 1.
	bu = self:add_slider(	{ix+1,iy,	7,1},	"speed",	self, "speed" )
	self:add_trig_restart(	{	ix,iy+1})

end

function meu:get_pixar()
	local pixar = self.pixar
	if pixar then
		pixar = PIXAR:create( "generik", 5 )
		pixar:set_light( false )
		pixar:print_terminal( 0, 1, self.text )
		self.pixar = pixar
	end
	return pixar
end

function meu:init()
	local ref = self.ref

---[[
	local str = nil
	local fname = app.media_dir_rel.."Xanae/generik.txt"
	if not aaa.file.is_exist( fname ) then return end

	local file = aaa.file.open( fname, "r" )
	if not file then return end

	for line in file:lines() do
		if str then
			str = str.."\n"..line
		else
			str = line
		end
	end
	file:close()
	self.text = str

end

function meu:restart()
	self.phase = -.03
end
function meu:draw()
	MEU.draw( self )

	if not self.text then return end

	local pixar = self:get_pixar()

	--pixar:set_font( 5 )
	pixar:set_light( false )

	self.phase =  ( self.phase or 0.) + self.speed * aaa.time.dt / 90.
	if self.phase > 1. then self.phase = -.03 end

	local nb_u = 18
	local nb_v = 11
	local osu = 1 / nb_u
	local osv = 1 / nb_v
	local id = 0
	local s = 1.8

	local function hack_draw_pixel( self, id, x, y, sx, sy )
		math.randomseed(id)
		local u = math.random(nb_u)
		local v = math.random(nb_v)
		u = (u-1)*osu
		v = (v-1)*osv
		local f = math.max( 1, y+.50 )
		gol.set_quad_uv( u, v, u+osu, v+osv )
		sx = sx * s * math.pow( f, .8 ) *.5
		sy = sx
		local ph = math.max(0,y-3)*1.
		y = y - (1 - math.sin(ph)) * 0
		x = x
		--aaa.draw_rect_uv( x-sx, y-sy, x+sx, y+sy )
		aaa.draw_rect_uv_at_z( x-sx, y-sy, x+sx, y+sy, (1 - math.cos(ph)) * 0 )
	end
	pixar:set_function_draw_pixel( hack_draw_pixel )

	pixar:set_pos_terminal( -5.5, -4. + self.phase * 140. )

	pixar:set_rgba( 1, 1, 1, 1 )
	pixar:set_light( false )

	pixar:prepare_draw( 2.4 )
	pixar:draw_text()
end
