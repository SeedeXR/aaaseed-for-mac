function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ui = self.ui

	self:add_camera()
	self:add_blending( {1,5} )

	local ix, iy = 1,1
	self.speed = 1.
	bu = self:add_slider(	{ix+1,iy,	7,1},	"speed",	self, "speed" )
	self:add_trig_restart(	{ix,iy+1}		)

	iy = 4
	bu = self:add_slider(	{ix,iy,	8,1},	"Dot size",	self, "dot_size" )

end

function meu:get_pixar()
	local pixar = self.pixar
	if not pixar then
		pixar = PIXAR:create( "heavent", 5 )
		self.pixar = pixar
	end
	return pixar
end

function meu:get_lines()
	if not self.lines then
		local fname = app.media_dir_rel.."Hugo/heavent.txt"
		if aaa.file.is_exist( fname ) then
			local file = aaa.file.open( fname, "r" )
			if file then
				self.lines = {}
				for line in file:lines() do
					table.insert( self.lines, line )
				end
				file:close()
			end
		end
	end
	return self.lines
end

function meu:restart()
	self.phase = 0
end

function meu:draw()
	MEU.draw( self )

	local lines = self:get_lines()
	if not self.lines then return end

	local pixar = self:get_pixar()


	local phase = self.phase
	if not phase or phase <= 0 then
		local lines = self:get_lines()
		self:print( "lines are "..#lines )
		math.randomseed( math.floor(aaa.time.t * 100000) ) --we had a bug
		local id
		repeat
			id = math.random(#lines)
		until self.id_last ~= id
		self.id_last = id

		--id = 4
		--self:print( "id is "..id )
		self.line = lines[id]
		self:print( "line is now "..self.line )
		self.line_len = string.len( self.line )
		pixar:clear_terminal()
		pixar:print_terminal( 0, -.45, self.line )
	end

	local t = aaa.time.dt
	self.phase = (phase or 0.) + self.speed * 10. * t

	--pixar:set_font( 5 )
	pixar:set_light( false )

--	if self.phase > 1. then self.phase = -.03 end

	local nb_u = 18
	local nb_v = 11
	local osu = 1 / nb_u
	local osv = 1 / nb_v
	local id = 0
	local s = 1.8

	gol.set_quad_uv( 0, 0, 1, 1 )

	local dot_size = self.dot_size
	local function hack_draw_pixel( self, id, x, y, sx, sy )
		--[[
		math.randomseed(id)
		local u = math.random(nb_u)
		local v = math.random(nb_v)
		u = (u-1)*osu
		v = (v-1)*osv
		gol.set_quad_uv( u, v, u+osu, v+osv )
		--]]
		local f = math.max( 1, y+.50 )
		sx = sx * dot_size--* s * (1 - math.pow( f, .8 )) *.5
		sy = sx --* 1.4
		x = x * .8
		local ph = x
		local inf = clamp_01( 1. - math.abs( x *.25) )
		--y = y - inf * math.sin(x*4) * .1
		local yd = -.3 --(y >0) and .4 or -.4
		y = interpolate( y, yd, inf*inf )
		x = x --+ (1-inf) * (x - inf * math.sin(x*2.+ph*5) * .1)
		--sx, sy = 1, 1
		--aaa.draw_rect_uv( x-sx, y-sy, x+sx, y+sy )
		aaa.draw_rect_uv( x-sx, y-sy, x+sx, y+sy )
		--aaa.draw_rect_uv_at_z( x-sx, y-sy, x+sx, y+sy, (1 - math.cos(ph)) * 0 )
	end
	pixar:set_function_draw_pixel( hack_draw_pixel )

	local size = .9
	local sx = .5/ size
	local s = sx * self.line_len
	local hsx = 19/.8
	local x = hsx - self.phase
	pixar:set_pos_terminal( x , 0 )
	if x + s < -hsx then
		self.phase = 0
	end

	pixar:set_rgba( 1, 1, 1, 1 )
	pixar:set_light( false )

	pixar:prepare_draw( size )
	pixar:draw_text()
end
