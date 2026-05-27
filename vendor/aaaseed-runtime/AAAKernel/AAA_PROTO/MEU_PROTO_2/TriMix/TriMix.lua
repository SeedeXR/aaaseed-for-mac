
function meu:define_meu_infos( )
	return { 	author = "Mâa from a request by Luc Foubert",
				tags = { "2d", "Vj", "coregraphic", "imageprocessing", "texture" },
				help =	{	"Mix 2 textures A and B using a third one M (the Mask) as a pixel by pixel switch.",
						}
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local SY = 1
	local DY = .2
	local ix,iy = 1,1

	self:add_camera()

	self:add_shading_ui( 	{ix,iy,	8,1} )
	iy = iy + SY + DY

 
	bu = self:add_selector(	{ix,iy,	8,SY}, "Mode" )
	bu:set_item_text( 1, "A", "B", "M", "MA", "MB", "MAB" )
	bu:set_target_lua( self, "mode" )
	iy = iy + SY + DY

	local Y = 2.5
	local SX,SYM = 8,5.5*SY

	local YM = 14.7 - SYM - DY
	local names = { "MASK", "A", "B", "TEST" }
	self:add_bu_texture_target_unit( {1,16-SYM*2-SY,		SX,SYM}, names[2], 2	)
	self:add_bu_texture_target_unit( {1,16-SYM,			SX,SYM}, names[3], 3	)
	self:add_bu_texture_target_unit( {17-SX,	YM,			SX,SYM}, names[1], 1	)

	for i=1,3 do		
		self:set_bu_texture_preset_use( i, false )
	end

	local names = { A=2, B=3, M=1 }
	local iy = 16-SYM-SY
	self:add_trig_method( {1,iy,	2,SY},	"A-B", self, "swap_texture_bind", 2, 3 )
	self:add_trig_method( {9,YM-SY,	2,SY},	"A-M", self, "swap_texture_bind", 2, 1 )
	self:add_trig_method( {1+6,iy,	2,SY},	"B-M", self, "swap_texture_bind", 3, 1 )

--todo go back to this
--	self:add_bu_texture_target_unit(	{1, 13.6, 8, 3.2},	names[4], 4	)

	SY = 1
	local ix = 9
	local iy = Y

	bu = self:add_button(	{ix,iy,		3,1},	"Inverse",	self, "b_mask_inv", false )
	bu = self:add_button(	{ix+4,iy,	3,1},	"Alpha",	self, "", false )
	iy = iy + SY
	bu = self:add_slider(	{ix+1,iy,	7,1},	"Min",		self, "min",	0,	0,1	)
	iy = iy + SY
	bu = self:add_slider(	{ix+1,iy,	7,1},	"Max",		self, "max",	1,	0,1	)
	iy = iy + SY + DY
end

function meu:init()
	self:add_shading()
end

function meu:update()
	local sha = self:get_shading()
	sha:set_frag_int_1_2( self.mode, self.b_mask_alpha and 1 or 0 )
	if self.b_mask_inv then
		sha:set_frag_float_1_2( self.max, self.min )
	else
		sha:set_frag_float_1_2( self.min, self.max )
	end
	sha:set_frag_vec4_w( 1, self:get_alpha() )
end
