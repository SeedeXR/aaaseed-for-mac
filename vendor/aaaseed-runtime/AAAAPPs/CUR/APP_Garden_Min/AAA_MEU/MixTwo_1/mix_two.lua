--	MixTwo.lua

function meu:define_meu_infos( )
	return { 	author = "Mâa ",
				tags = { "2d", "texture", "unfinished", "experimental", "tutorial" },
				help =	{	"Example of MEU mixing two texture using a fragment shader."
						}
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()

	local names = { "A", "B" }
	self:add_bu_texture_target_unit(		{1}, 			names[1]	)
	self:add_bu_texture_target_unit(		{1}, 			names[2],	2	)


	local SX = 7
	local SY = SX/1.77
	local X= 1 + (8 - SX) * .5
	local Y = 6
	local pos = {	{ X, Y }, { X, Y+SY } }
	for i=1,2 do
		--local bind = self:get_texture_bind_2d( i )
		--aaa.print_inverse( "________________________________________________"..bind )
		local p = pos[i]
		--todo	deal with size and variation of size with video
		bu = self:add_monitor_id( {p[1],p[2]+2, SX,SY}, names[i].."_bis", i )	--todo quick remove last arg i
			--bu:set_draw_info( true )
			bu:set_text( names[i], .2 )
			bu:set_text_xy( .0, 1. )
			--bu:set_text_draw( true )
			bu:set_text_color( "cyan" )

			--bu:set_border( true )
			--bu:set_border_line( true )
	end
	local SY = 1
	local ix = 9
	local iy = 9
	bu = self:add_button(	{ix, iy,	2,SY},				"Inv",		self, "b_inv", false )
	bu = self:add_slider(	{ix+1,iy+1,	7,SY},	"Min",		self, "min", 0,	0, 1		)
	bu = self:add_slider(	{ix+1,iy+2,	7,SY},	"Max",		self, "max", 1,	0, 1		)
end

function meu:init()
	self:add_shading()
end

function meu:update()
	local sha = self:get_shading()
	if self.b_inv then
		sha:set_frag_float_1_2( self.max, self.min )
	else
		sha:set_frag_float_1_2( self.min, self.max )
	end
end
