function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
--	local par

	ui.cam = self:add_camera()

	local ix,iy = 1,2

--	local layer = self:get_layer(1)
--	bu = self:add_button(	{ix, iy }, "Top", aaa.layer.get_rendering( layer ), "top_line", true )
	bu = self:add_slider(	{ix,iy+1,	8,2},	"Phase",	nil, nil, 0, 0, 1 )
end

function meu:init()
	local ref = self.ref
	local b_img = aaa.img.read( 0, app:make_name_image( "ligne_couleurs.png" ), false )
	if b_img then
		local sx,sy = aaa.img.get_size( 0 )
		--aaa.box_good( "palette is there "..sx.." x "..sy )
		local col = {}
		for i=1,sy/10 do
			local r,g,b,a = aaa.img.get_color_xy( 0, sx/2, sx/2+sx*(i-1) )
			--self:print( r.." ".. g.." ".. b.." ".. a )
			col[i] = { r, g, b, a }
		end
		self.color = col
	end
end

function meu:update()
	local ref = self.ref
	--local id = (self:get_inst_key() == "1") and 1 or 2
	--self:set_bu_value( "inter", math.sin( aaa.time.t * .2 + id * math.pi ) )
	local ph = self:get_phase()
--	if ph <= 0 then
--		param.set( ref.restart, true )S
--	end
--	if inside( ph, 0.01, .45 ) then
--		param.set( ref.nb_by_sec, 1024*4 )
--	else
--		param.set( ref.nb_by_sec, 0 )
--	end
--	local d = .5
--	param.set( ref.pos, (.5-ph) * (8+d*2) - d )
--	self:init()
end

function meu:draw()
	local ref = self.ref
	--local id = (self:get_inst_key() == "1") and 1 or 2
	--self:set_bu_value( "inter", math.sin( aaa.time.t * .2 + id * math.pi ) )
	local ph = self:get_phase()
	--ph = 0.35
	local x = (.5-ph) * (8)
--	if ph <= 0 then
--		param.set( ref.restart, true )
--	end

--	local d = .5
--	param.set( ref.pos, (.5-ph) * (8+d*2) - d )
	local alpha
	if inside( ph, 0.0, .48 ) then
		alpha = 1
	else
		alpha = (ph-.5)/(.45-.5)
	end

	MEU.draw(self)

	local nb = 92
	local H = 9
	local s = H/nb
	local sh = s * .5
	local col = self.color
	local round = math.floor(x/s)
	math.randomseed( round )
	--x = round*s
	x = x - .15
	local y = -H*.5
	local col_nb = #col
	for i=1,nb do
		local c = col[math.random(col_nb)]
		gol.color( c[1], c[2], c[3], alpha *0.85 )
		aaa.draw_rect( x, y, x+s, y+s )
		y = y + s
	end
--	self:init()
end


function meu:set_phase( phase )		self:set_bu_value( "phase", phase )	end
function meu:get_phase()			return self:get_bu_value( "phase" )	end