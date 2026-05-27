aaa.update_util()

local lb_draw_text = true

local function draw_tex_for_flatland( bind, x, y, dx, dy, b_text, tsx, tsy )
	if aaa.img.get_size( bind ) then
		gol.set_texture_dim( 2 )
		gol.color_white()
		aaa.draw_bind_rect( bind, x, y, x+dx*.99, y+dy*.99 )
	else
		gol.set_texture_dim( 0 )
		gol.color( 1, .5, 0, 1 )
		aaa.draw_mul_line( x+dx*.5, y+dy*.5, dx*.1, dy*.1  )
	end

	if b_text and lb_draw_text then
		gol.set_texture_dim( 0 )
		gol.color( 0, 1, .5 ,1 )
		gol.push_matrix()
			gol.translate( x, y + dy*.7 )
			gol.scale( tsx or 1, tsy or 1 )
			aaa.draw_str_maa( " "..bind )
		gol.pop_matrix()
	end
end
local function begin_draw_in_flatland()
	--gol.reset()
	gol.set_default()
	gol.set_quad_uv()
	--	gol.push_attrib()
	gol.set_texture_dim( 2 )

	--aaa.print( sx.." "..sy )
	gol.color_white()
end
local function end_draw_in_flatland()
	--	gol.bind_texture( -1 )
	gol.set_texture_dim( 0 )
	--	gol.pop_attrib()
end

function aaa.flatland.hook_draw_info( sx, sy )
	--if 1 then return end
	--	aaa.print( "aaa.flatland.hook_draw_info( "..sx.." ,"..sy.." )" )
	local function draw_tex( bind, x )
		local y = .25
		local dy = 1 - y;
		local dx = dy * sy / sx * 1.33;
		local ddx = dx  * .05
		local x = 1. - (dx+ddx)*x


		draw_tex_for_flatland( bind, x,y, dx,dy, true, .006, .1	 )
	end

	begin_draw_in_flatland()

		--
		--	this the list of texture to draw in info
		--
		local l = { 30, 64, 67, 71 }	--	Version no men's land

		if aaa.lua.global.get( "APP" ) then
			if app and app.flatland_tex then
				--aaa.print( "GOT LIST" )
				l = app.flatland_tex
			end
		end

		local layer_ui = aaa.layer.get_ui()
		if layer_ui then
			l[1] = param.get( layer_ui, "bind_2d_out" )
		end
		local n = #l
		for i = 1, n do
			draw_tex( l[i], n-i+1 )
		end

	end_draw_in_flatland()
end



function aaa.flatland.hook_draw_focus( sx, sy )
	--if true then return end
	--aaa.print( "aaa.flatland.hook_draw_focus( "..sx.." ,"..sy.." )" )
	begin_draw_in_flatland()

	local function draw_tex( bind, ix, iy, b_text )
		local f = 3.5
		local dx	= 4 * f;
		local ddx	= dx * .0;
		local dy	= 1.3 * f;
		--aaa.print( dx.." "..dy )
		--x = .5*sx - dx*x
		local x = sx - ( dx + ddx ) * ix
		local y = sy - ( iy + 1 ) * dy
		draw_tex_for_flatland( bind, x, y, dx, dy, b_text )
	end

	local function draw_bank( bank, oix, oiy )
		local col = 1
		local row = 12
		local bind = bank * aaa.img.__BIND_BY_BANK_2D
		for r=1,row do
			for c=1,col do
				draw_tex( bind, col-c+1+oix-1, oiy+r-2, c==1 )
				bind = bind + 1
			end
		end
	end

	draw_bank( 3, 2, 2 )
	draw_bank( 2, 1, 2	 )
	--	aaa.draw_rect( sx/4,	0, 100, 	sy/4	)

	end_draw_in_flatland()
end