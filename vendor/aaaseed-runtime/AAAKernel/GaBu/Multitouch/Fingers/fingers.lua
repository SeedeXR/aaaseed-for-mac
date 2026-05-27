if not GABU then return end

if not GABU.finger then
	aaa.print( "fingers_init()" )
	GABU.finger = {}
	GABU.finger.ref = {}
	local ref = GABU.finger.ref
	ref.tra_x		= param.get_ref( "GaBu/Multitouch/Finger/fx.transfo", "translate_x" )
	ref.tra_y		= param.get_ref( "GaBu/Multitouch/Finger/fx.transfo", "translate_y" )
	ref.active		= param.get_ref( "GaBu/Multitouch/Finger/fx_b.layer", "active" )
	ref.text_id		= param.get_ref( "GaBu/Multitouch/Finger/fx_a.tex2d", "Text" )
	ref.nb			= param.get_ref( "GaBu/Multitouch/Fingers/fx_a.grid", "nb_u" )
	ref.layers		= aaa.obj.get( "gabu_finger" )
	GABU.finger.layers = aaa.layers.get_cur()
end

local SB
local SE
local nb_rayon
local step

local d = .005
local function draw_with_shadow( fn, x,y, b_magenta )
	gol.color_black( .8 )
	fn( x,y )
	if b_magenta then
		gol.color_magenta( .8 )
	else
		gol.color_cyan( .8 )
	end
	fn( x-d,y+d )
end
local function draw_str( x,y, str, b_magenta )
	draw_with_shadow( function(x,y) aaa.draw_str_xy(str, x,y, .1,.1) end, x,y, b_magenta )
end

function GABU.finger:draw_gene( x,y, id )
	local r = id==-1 and .15 or .06
	local sb = (id==-1 and 0 or SB) + r
	local se = (id==-1 and SE or SE) 

	local a,s,c
	for v = 0., .999, step do
		a = v * math.pi2
		s = math.sin( a )
		c = math.cos( a )
		gol.color_black( .8 )
		aaa.draw_line( x+s*sb, y+c*sb, x+s*se, y+c*se )
		if id==-1 then
			gol.color_magenta( 1 )
		else
			gol.color_cyan( .8 )
		end
		aaa.draw_line( x-d+s*sb, y+d+c*sb, x-d+s*se, y+d+c*se )
		--aaa.print( x.." "..y )
	end

	if id <= 0 then
		local b_mouse = id==-1
		if b_mouse and not GABU.__click_last then --just clicked
			GABU.__click_last = { x,y, id }
		end
		gol.set_line_width( 5 )
		--if id == -1					then	draw_str( x+r,		y, 		 "Click" )	end
		if aaa.keyboard.is_alt()	then	draw_str( x+r,		y-r-.05, "Alt",		b_mouse )	end
		if aaa.keyboard.is_shift()	then	draw_str( x-r-.2,	y+r-.05, "Shift",	b_mouse )	end
		if aaa.keyboard.is_ctrl()	then	draw_str( x-r-.2,	y-r-.05, "Ctrl", 	b_mouse )	end
		gol.set_line_width( 4 )
	end

	local t = GABU.__click_last
	if t then
		local cx,cy = t[1],t[2]
		draw_with_shadow( function(x,y) aaa.draw_circle_axe_z( x,y,0, 2*r,36 ) end, cx,cy,	true )
		local dx,dy = x-cx,y-cy
		draw_with_shadow( function(x,y) gol.draw_lines_2d( x,y, x+dx,y+dy ) end,    cx,cy,	true )
		draw_str( cx-r*.65,cy+r,	"Click", 	true )
	end
end

--local function draw_hori( x, y )	gol.draw_lines_2d(	x+SB, y, x+SE, y, 	x-SB, y, x-SE, y )	end
--local function draw_vert( x, y )	gol.draw_lines_2d(	x, y+SB, x, y+SE,	x, y-SB, x, y-SE )	end

function GABU.finger:draw_one( x,y, id )
	if GA.b_draw_finger_number then
		aaa.draw_str_xy( id, x+.1,y+.1, .1,.1 )
	end
	self:draw_gene( x,y, id )
end
function GABU.draw_mouse()
	gol.set_texture_dim( 0 )
	gol.color_cyan( .8 )
	gol.set_line_width( 4 )
	gol.set_line_smooth( true )

	local x = param.get( aaa.ref.mouse_x )
	local y = param.get( aaa.ref.mouse_y )
	x = x / param.get( aaa.ref.window_sx )
	y = y / param.get( aaa.ref.window_sy )

	local cam = ga.cam
	local ox,oy, sx,sy = cam:get_blobs_offset_scale()
		--self:print( sx.." "..sy )
	local l,b, r,t = cam:get_viewport()
	--ox = ox * (r-l)
	--oy = oy * (t-b)
	-- (r-l)*8., (t-b)*sy*8./sx

	--ox = ox + 1
	sx = sx / (r-l)
	ox = ox + (1-l-r)*.5
	sy = sy / (t-b)
	oy = oy + (t+b-1)*.5
	
	local f = 1
	GABU.finger:draw_one( (x+ox)*sx*f,-(y+oy)*sy*f, 0 )
end

function GABU.finger.update()
	local ph = aaa.time.t_real * math.pi * 4.
	if GA.b_draw_finger_line then
		SB = math.sin( ph ) * .01
		SE = 32.
		nb_rayon = 4
	else
		SB = math.sin( ph ) * .01
		SE = .15 + math.sin( ph ) * .01
		nb_rayon = 12
	end
	step = 1./ nb_rayon

	--ga.cam:update()
	local blobs = ga:get_blobs()
	local nb = blobs and blobs:get_blob_nb() or 0
	local ref = GABU.finger.ref
	--aaa.print( nb )
	--param.set( GABU.finger.ref.nb, nb )
	aaa.show( nb, "nb finger" )
	if nb==0 then
		GABU.__click_last = nil
		GABU.draw_mouse()
	else
		for i=1,nb do
			local blob = blobs.blob[i]
			--aaa.print( i.." "..blob.x.." "..blob.y )
			local x = blob.x
			local y = blob.y
			local id = blob.id

			if false then
				param.set( ref.tra_x,	x )
				param.set( ref.tra_y,	y )
				param.set( ref.text_id,	id )
				aaa.obj.update_then_draw( ref.layers )
			else
				gol.set_texture_dim( 0 )
				gol.color_cyan( .8 )
				gol.set_line_width( 4 )
				gol.set_line_smooth( true )
				--draw_hori( x, y )
				--draw_vert( x, y )
				GABU.finger:draw_one( x,y, id )
			end
		end
	end
end



GABU.finger.update()
aaa.layers.skip_rest( GABU.finger.layers )

