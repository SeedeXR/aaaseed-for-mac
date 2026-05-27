
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	ui.cam = self:add_camera()

	local ix,iy = 1,1

--	bu = self:add_button(	{ix, iy }, "Take Control" )

end


function meu:draw_menu()
end

function meu:draw_pictos()
	local sh = 8
	local fx = 16 / sh * 2
	local fy = 9 / sh * 2
	local v = math.sin( math.pi * .25 )
	local r = .2


	local pos_hack 	=	roma.pos_hack
	local nb 		=	#roma.subs_all
	local S 		=	.5
	local RA 		=	.15

	gol.set_quad_uv()

	table.sort( pos_hack, function(a,b) return a[2] > b[2] end )

	local bictos = roma.bictos
	if not bictos then return end

	local nb_bictos = #bictos

--	for i=1,nb_bictos do
--		bictos[i].__lb_used = false
--	end

	local E 	= .007
	local D 	= .05
	local DD 	= D +.1
	gol.set_blend_add()
	for i=1,#pos_hack do
		local t 	= pos_hack[i]
		local id 	= (t.id-1)%nb + 1
		--aaa.print(id)
		local sub 		= roma.subs_all[id]
		local section	= sub.section

		local alpha = section.alpha
		gol.color_white(alpha)
		--gol.color( i==2 oe )
		local x = t[1]*fx
		local y = t[2]*fy

		local s = S
		--gol.set_blend_add_alpha_one()
		--aaa.draw_rect_uv( x, y, x + s, y + s*RA )
		gol.set_texture_dim( 0 )
		if true then

			aaa.draw_rect_uv(	x+D,	y-E,	x+DD, 	y+2*E 	)
			aaa.draw_rect_uv(	x-D,	y-E, 	x-DD, 	y+2*E 	)
			aaa.draw_rect_uv(	x-E,	y+D,	x+E, 	y+DD 		)
			aaa.draw_rect_uv(	x-E,	y-D,	x+E, 	y-DD 		)
		end

		local sx = s * (1 - RA)
		local sy = sx * 140/115

		--local ox = x/4 * 2	* RA - sx * .5
		local ox = RA
		local oy = RA

		--aaa.draw_line(	x,	y,	x+o*1.2, 	y+o*1.2 		)
		if false then
			gol.color_white(.3)
			local f = .2
			if x < x+ox then
				gol.draw_triangles_2d(	x,			y,
										x+ox+sx*f,	y+oy,
										x+ox,		y+oy+sy*f )
			elseif x+ox+sx < x then
				gol.draw_triangles_2d( 	x,			y,
										x+ox,		y+oy,
										x+ox+sy*f,	y+oy+sy*f )
			else
				gol.draw_triangles_2d(	x,			y,
										x+ox+sx*f,	y+oy,
										x+ox,		y+oy )
			end
		end

		x = x + ox
		y = y + oy

		--aaa.draw_disk_axe_z( x, y, 0, .4, 48 )
		--aaa.draw_line( x+r, y+r, x+1., y+1. )
		gol.set_texture_dim( 2 )

		gol.bind_texture( sub.picto:get_bind() )

		aaa.draw_rect_uv( x, y, x+sx, y+sy )

		if id <= nb_bictos then
			local bid = ((id-1)%nb_bictos)+1
			local bu = bictos[ bid ]
			bu:set_xy( x+sx*.5, y+sy*.5 )
			bu:set_sxy( sx, sy )
			bu:set_visible( roma.b_vdebug )
			bu.__lb_used = true
		end
	end

	for i=1,nb_bictos do
		local bu = bictos[i]
		if not bu.__lb_used then
			bu:set_y( 10 )
			bu.__lb_used = false
		end
	end

	gol.set_texture_dim( 0 )

end

function meu:draw_ui()
	local demo_roma = app.demo_roma

	gol.set_mask_alpha( false )

	local l = -4
	local SX = 8*(454/1920)
	local r = l + SX
	local Y = -.5
	local DY = -.33
	roma:set_color( .75 )
	aaa.draw_rect_uv( l, -4, r, 4  )

	local img
	gol.color_white()
	gol.set_texture_dim( 2 )

	local imgs = roma.imgs
	--	fixed title
	imgs.groupement:draw_xy_sx(	l, 1.6,	SX	)
	imgs.romanite:draw_xy_sx(	l, .5,	SX	)

	--	arrow
	local i_sel = 1
	local dt = aaa.time.dt
	aaa.show( dt, "dt" )
	img = imgs.arrow
	local sx = SX*.8
	local i_arrow = demo_roma.i_arrow or 0
	local i_speed = demo_roma.i_speed or 0
	local d = i_sel-1 - i_arrow
	i_speed = i_speed * math.pow( .01, aaa.time.dt ) + d*4
	i_arrow = i_arrow + i_speed*dt
	demo_roma.i_speed = i_speed
	demo_roma.i_arrow = i_arrow

	aaa.show( i_arrow )
	img:draw_xy_sx(		r-sx, Y+i_arrow*DY-sx*.408,	sx	)

	--	item
	gol.set_blend_add_color()
	local sections = demo_roma.sections
	for i=1,#sections do
		local section = sections[i]
		if i ~= i_sel then		gol.color( 163/255, 197/255, 222/255, 1. )
		else					gol.color_white()
		end
		section.img:draw_xy_sx(		l, Y+(i-1)*DY,	SX	)
	end

	gol.set_mask_alpha( true )
	--gol.set_blend_add()
end

function meu:draw()
	MEU.draw( self )

	if not aaa.lua.global.get( "roma" ) then return end

	self:draw_pictos()

	--	self:draw_ui()

end
	