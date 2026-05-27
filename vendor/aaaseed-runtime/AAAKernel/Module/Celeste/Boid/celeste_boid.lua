if false then
	return
end


if aaa.lua.global.declare( "celeste" ) then
	celeste = {}
end

if not celeste.boid then

	celeste.boid = {}
	celeste.boid.ref = {}

	local boid = celeste.boid
	local ref = boid.ref

	ref.layer_att		= aaa.obj.get( "celeste.boid.layer_att" )
	ref.layer			= aaa.obj.get( "celeste.boid.layer" )
	ref.bdd				= aaa.obj.get( "celeste.boid.bdd" )
	ref.bdd_nb_min 			= param.get_ref( ref.bdd, "nb_alive_min" )
	ref.bdd_nb_max 			= param.get_ref( ref.bdd, "nb_alive_max" )
	ref.bdd_repulse			= param.get_ref( ref.bdd, "repulse_distance" )

		--todo check this
	local pre = app.media_dir_rel.."Celeste/"
	aaa.img.read( 20, pre.."facade.tga"				)
	aaa.img.read( 21, pre.."facade_grey_dark.tga"	)
	aaa.img.read( 22, pre.."facade_NB_line_ref.tga"	)
	aaa.img.read( 23, pre.."facade_line_fine.tga"	)

	aaa.img.read( 24, pre.."facade_edge.tga"		)
	aaa.img.read( 25, pre.."facade_edge_2.tga"		)
	aaa.img.read( 26, pre.."facade_neon.tga"		)
	aaa.img.read( 27, pre.."facade_false.tga"		)


	aaa.img.read( 28, pre.."facade_balcon_field.tga"		)
	aaa.img.read( 29, pre.."facade_field.tga"				)
	aaa.img.read( 30, pre.."facade_NB_line_ref_blur.tga"	)
	aaa.img.read( 31, pre.."facade_balcon_mask.tga"			)
end

local NB	 = 96
local RADIUS = .15
local ALPHA		=	1.
local ref = celeste.			boid.ref
param.set( ref.bdd_nb_min, NB )
param.set( ref.bdd_nb_max, NB )
param.set( ref.bdd_repulse, RADIUS*2. )

function celeste.boid.restart()
	local boid = celeste.boid
	local ref = boid.ref

	boid.color = {}
end

if not celeste.boid.color then
	celeste.boid.restart()
end

local 	BALCON_SX	= .95
local 	BALCON_SXH	= BALCON_SX * .5

local	BALCON_XL	= -1.175
local	BALCON_XLL	= BALCON_XL - BALCON_SXH
local	BALCON_XLR	= BALCON_XL + BALCON_SXH

local	BALCON_XR	= 1.225
local	BALCON_XRL	= BALCON_XR - BALCON_SXH
local	BALCON_XRR	= BALCON_XR + BALCON_SXH

local	BALCON_XC	= ( BALCON_XL + BALCON_XR ) / 2.
local	BALCON_XCL	= BALCON_XC - BALCON_SXH
local	BALCON_XCR	= BALCON_XC + BALCON_SXH

local	BALCON_YT	= -.8
local	BALCON_YB	= -1.

function celeste.boid.draw( b_update )

	local SPEED_Z = .4
	--param.set( self.ref.phase, self.phase )
	local boid = celeste.boid
	local ref = boid.ref
	local tcolor = boid.color
	local bdd = ref.bdd

	aaa.obj.update_then_draw( ref.layer_att )
	aaa.obj.update_then_draw( ref.layer )

	--gol.set_texture_dim( 0 )
	local nb = aaa.bdd.get_point_nb( bdd )
	local r = RADIUS
	--aaa.print( nb )
	local get_point = aaa.bdd.get_point_and_id
	for i=1,nb do
		local x, y, z, id = get_point( bdd, i )
--[[	if x<r and y<r and -r<x and -r<y then
			local dum
			dum, id = poid.split_id( id )
		end
--]]
		--aaa.draw_circle_axe_z( x, y, z,	r, 12 )
		local color = tcolor[id]
		if inside( y, BALCON_YB, BALCON_YT ) then
			if inside( x, BALCON_XLL, BALCON_XLR ) then
				if not color then
					tcolor[id] = 1
					color = 1
				end
			elseif inside( x, BALCON_XRL, BALCON_XRR ) then
				if not color then
					tcolor[id] = 2
					color = 2
				end
			elseif inside( x, BALCON_XCL, BALCON_XCR ) then
				tcolor[id] = nil
				color = nil
			end
		end

		if not color then
			gol.color_white( ALPHA )
		elseif color == 1 then
			gol.color_magenta( ALPHA )
		else
			gol.color_cyan( ALPHA )
		end

		aaa.draw_rect_uv( x-r, y-r, x+r, y+r )
		--	aaa.empty()
	end
end

function celeste.do_key_custom()
	local b_key_not_use = false

	local key = aaa.keyboard.get_ascii_down()
	if key ~= 0 then
		aaa.print( "ascii_key "..key.." in "..aaa.script.get_name( ) )
		if key==65 or key==97 then --a,A
		elseif key==77 or key==109 then --m,M
		elseif key==82 or key==114 then --r,R
			celeste.boid.restart()
		elseif key==3 then	-- CTRL c
		elseif key==4 then	-- CTRL d
		elseif key==32 then	-- space
		elseif key==83 or key==115 then	-- s or S
		else
			b_key_not_use = true
		end
	end

	key = aaa.keyboard.get_special()
	if key ~= 0 then
		aaa.print( "special_key "..key.." in "..aaa.script.get_name( ) )
		if key == 100 then			-- >-
		elseif key == 102 then		-- ->
		elseif key == 106 then		-- Home/Debut
		elseif key==1 then	--F1
		elseif key==2 then	--F2
		elseif key==5 then	--F5
		elseif key==6 then	--F6
		elseif key==8 then	--F8
		elseif key==9 then	--F9
		elseif key==12 then	--F12
		else
			b_key_not_use = true
		end
	end
	if b_key_not_use then
		if GABU.do_key_custom then
			GABU.do_key_custom( key )
		end
	end
end

celeste.do_key_custom()
celeste.boid.draw( )

aaa.layers.skip_rest()


