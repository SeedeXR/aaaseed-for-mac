function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local ix,iy = 1,1
	local SY,DY = 1,.2
	local bu

	self:add_rendering()    --todo why we get remdering from layer_c ???
	self:add_camera()

 --   bu = self:add_slider_multi(	{ix,2,	12,12}, "Rects", 8 )
--    bu:set_select_on_click_double()
--		bu.draw = self.draw_multi
--   bu.draw_back = draw_multi
--    bu.__multi_meu_caller = self

	bu = self:add_button(	{ix,iy,	4   ,SY}, "Test full", self, "b_test_full",   false  )
	iy = iy + SY + DY

	bu = self:add_slider(	{ix,iy,	8,SY}, "Seed", self, "seed",   16, 1,256*256*256  )
		:set_value_type_integer(true)
	iy = iy + SY + DY

	bu = self:add_slider(	{ix,iy,	8,SY}, "NB", self, "rect_nb",   16, 1,256*8  )
		:set_value_type_integer(true)
		:set_min_max_strict(true)
	iy = iy + SY
	
	bu = self:add_slider(	{ix,iy,	8,SY}, "Size", self, "rect_size",   1, 0,8  )
	iy = iy + SY

end

function meu:generate_rects()
	if not self.rects
		or self.rect_nb~=#self.rects
		or self.seed_used~=self.seed
		or self.rect_size_used~=self.rect_size
		then
		local rects = {}    
		math.randomseed( self.seed )
		for i=1,self.rect_nb do
			local r = {
				(math.random()-.5)*8,
				(math.random()-.5)*4.5,
				math.random()*self.rect_size,
				math.random()*self.rect_size,
				math.random()       
			}
			local sh = r[3]*.5
			local r2 = sh*sh
			sh = r[4]*.5
			r2 = r2 + sh*sh
			r.rad = math.sqrt(r2)
			rects[i] = r
		end
		self.rects = rects
		self.seed_used = self.seed
		self.rect_size_used = self.rect_size
	end
end

function meu:test_rects()
	local rects = self.rects
	for i=1,#rects do
		rects[i].b_contact = false
	end
	for i=2,#rects do
		local a = rects[i]
		for j=1,i-1 do
			--self:print( i.."."..j )
			local b = rects[j]
			local l2 = a.rad + b.rad
			l2 = l2 * l2
			local coor = b[1] - a[1]
			local d2 = coor * coor
			if d2 <= l2 then
				coor = b[2] - a[2]
				d2 = d2 + coor * coor
				if d2 <= l2 then
					a.b_circle = true
					b.b_circle = true                   
					local b_full = aaa.rect.is_intersect_SAT( a,b )
					if self.b_test_full then
						local b_half = aaa.rect.is_intersect_SAT_half( a,b ) and aaa.rect.is_intersect_SAT_half( b,a )
						if b_half ~= b_full then
							self:print( "different result for "..i.." "..j )
						end
					end
					if b_full then
						a.b_contact = true
						b.b_contact = true
					end
				end
			end
		end
	end
end

function meu:draw_rects()
	local rects = self.rects
	for i=1,#rects do
		local r = rects[i]
		--r[5] = .125 * .5
		gol.push_matrix()
			gol.translate( r[1],r[2] )
			gol.rotate_z( r[5] )
			if r.b_contact then
				gol.color_red( .5 )
			else
				if r.b_circle then
					gol.color_green( .75 )
				else
					gol.color_green( .5 )
				end
			end
			aaa.draw_rect_size( r[3],r[4] )
			gol.color_white()
			aaa.draw_rect_line_size( r[3],r[4] )
			aaa.draw_line( 0,0,   r[3],0 )
		gol.pop_matrix()
	end
	gol.set_texture_dim(2)
	gol.bind_texture(1)
	gol.color_white()
	for i=1,#rects do
		local r = rects[i]

		-- if r.b_contact then
		--     gol.color_red( .5 )
		-- else
		--     if r.b_circle then
		--         gol.color_green( .75 )
		--     else
		--         gol.color_green( .5 )
		--     end
		-- end
		aaa.draw_rect_uv_axe_z( r[1],r[2],1,  r[3],r[4],    r[5] )
	end
end

function meu:update()
	self:generate_rects()
	self:test_rects()

	-- local str = "local a = math.sin(aaa.time.t); local b = math.cos(aaa.time.t); return a*a+b*b"
	-- local str = "return 1.5 + math.cos(.5)"
	-- local str = "local function res(a) return 1.5 + math.cos(a) end return res(1.2)"
	-- local f = loadstring( str )
	-- if f then
	-- 	self:print( "result is "..f() )
	-- else
	-- 	self:print( "failed to loadstring() text from \""..str.."\" )" )
	-- end
end

function meu:draw()
	self:draw_layers_begin()
		--  self:print( aaa.time.t )
		self:draw_layer( 1 )
		self:draw_rects()
	self:draw_layers_end()
end
