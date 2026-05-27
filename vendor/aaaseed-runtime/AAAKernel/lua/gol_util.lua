aaa.show_file_begin( "gol_util" )

if not gol.ref then
	gol.ref = {}
--	aaa.print_inverse( "gol_util.lua" )
end

--
--	COLOR FULL
--
function gol.color_255( r,g,b, a )
	if r then r = r / 255 end
	if g then g = g / 255 end
	if b then b = b / 255 end
	if a then
		gol.color4( r,g,b, a/255 )
	else
		gol.color3( r,g,b )
	end
end

local tab = {}
tab.color_white = 	"Call gol.color4( 1,1,1, alpha ) if alpha is provided\n"..
					"else gol.color3( 1,1,1 )."
tab.color_black =	"Call gol.color4( 0,0,0, alpha ) if alpha is provided, else it uses "..
					"else gol.color3( 0,0,0 )."
tab.color_red =		"Call gol.color4( 1,0,0, alpha ) if alpha is provided, else it uses "..
					"else gol.color3( 1,0,0 )."
tab.color_green =	"Call gol.color4( 0,1,0, alpha ) if alpha is provided, else it uses "..
					"else gol.color3( 0,1 0 )."
tab.color_blue =	"Call gol.color4( 0,0,1, alpha ) if alpha is provided, else it uses "..
					"else gol.color3( 0,0,1 )."
tab.color_yellow =	"Call gol.color4( 1,1,0, alpha ) if alpha is provided, else it uses "..
					"else gol.color3( 1,1,0 )."
tab.color_cyan =	"Call gol.color4( 0,1,1, alpha ) if alpha is provided, else it uses "..
					"else gol.color3( 0,1,1 )."
tab.color_magenta = "Call gol.color4( 1,0,1, alpha ) if alpha is provided, else it uses "..
					"else gol.color3( 1,0,1 )."
tab.color_orange =	"Call gol.color4( 1,.5,0, alpha ) if alpha is provided, else it uses"..
					"else gol.color3( 1,.5,1 )."
tab.color_grey =	"Call gol.color4( grey,grey,grey, alpha ) if alpha is provided, else"..
					"else gol.color( grey )."
tab.color_255 =		"Call gol.color( r/255, g/255, b/255, a/255 )."
gol.doc = tab

aaa.show_file_end( "gol_util" )