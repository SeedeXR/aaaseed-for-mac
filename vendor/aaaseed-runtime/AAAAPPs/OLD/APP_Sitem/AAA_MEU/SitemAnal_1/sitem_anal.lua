function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()
	self:add_monitor()
	self:add_bu_texture_target_unit()

	local ix,iy = 1,5
--	self:add_button(	{		ix, iy },			"Draw",				self, "b_draw",				false )
	bu = self:add_button(	{	ix, iy },			"Run",				self, "b_run",				false )
		bu:set_text_rect_ratio( 2 )
		bu:set_preset_use( false )

	iy = iy + 2
	bu = self:add_selector(	{ix,iy,	1,7},	"Chapter" )
		bu:set_nb_min_0( 1, 7 )
		bu:set_item_text( 1, "No" )
		bu:set_item_text_from_nb_minus_1( 2 )

	bu = self:add_selector(	{ix+1,iy,	7,2},	"Video" )
		bu:set_nb_min_0( 8, 2 )
		bu:set_item_text( 1, "No" )
		bu:set_item_text_from_nb_minus_1( 2 )
end

function meu:init()
	local ref = self.ref

	local	bdd	=	self:get_layer_bdd( 2 )
	ref.bdd	=	bdd
	ref.u		=	param.get_ref( bdd, "begin_u" )
	ref.v		=	param.get_ref( bdd, "begin_v" )
	ref.su		=	param.get_ref( bdd, "end_u" )
	ref.sv		=	param.get_ref( bdd, "end_v" )
	ref.out_cov	=	param.get_ref( bdd, "out_coverage" )
end

function meu:update()
	if sitem then
		sitem.meu_anal = self
	end
end
function meu:draw()
	local ref = self.ref

	self:draw_layers_begin()
	
		self:draw_layer( 1 )

		local SX = .11
		local SY = SX * 2
		SX = SX * .5
		SY = SY * .5
		local function get_square( u, v, id, pre )
			param.set( ref.u,	u*.125+ .5 - SX )
			param.set( ref.v,	v*.25 + .5 - SY )
			param.set( ref.su,	u*.125+ .5 + SX )
			param.set( ref.sv,	v*.25 + .5 + SY )
			--param.set( ref.su, *.125 * .8)
			--param.set( ref.sv, *.25  * .8)
			self:draw_layer( 2 )
			local cov = param.get( ref.out_cov )
			local b_cov = cov > 0
			if b_cov then
				aaa.draw_circle_axe_z( u, v, 0, SX*16 )
			end
			aaa.draw_str_xy( pre..id, u-.5, v+.25, .3, .3 )
			return b_cov
		end


		self.v ={}
		local id = 1
		if true then
			self.v[1]	= get_square(	-1.34,	.75,		1, "v" )
			self.v[2]	= get_square(	-0.45,	.5,			2, "v" )
			self.v[3]	= get_square(	-.6,	-.5,		3, "v" )
			self.v[4]	= get_square(	-2.,	-.7,		4, "v" )
			self.v[5]	= get_square(	-2.4,	.2,			5, "v" )
			self.v[6]	= get_square(	-1.5,	.0,			6, "v" )
			self.v[7]	= get_square(	1.44,	.75,		7, "v" )
			self.v[8]	= get_square(	2.15,	.0,			8, "v" )
			self.v[9]	= get_square(	1.95,	-.8,		9, "v" )
			self.v[10]	= get_square(	.5,		-.5,		10, "v" )
			self.v[11]	= get_square(	0.55,	.4,			11, "v" )
			self.v[12]	= get_square(	1.5,	-.1,		12, "v" )
			self.v[13]	= get_square(	-1.5,	1.5,		13, "v" )
			self.v[14]	= get_square(	1.7,	1.5,		14, "v" )
		else
			local nb_u = 4
			local nb_v = 4
			local DX = 1
			local DY = 1
			for iv=1,nb_u do
				local v = -DY * ( iv - 1 - (nb_v-1)/2 )
				for iu=1,nb_u do
					local u = DX * ( iu - 1 - (nb_u-1)/2 )
					self.v[id] = get_square( u, v, id, "v" )
					id = id + 1
				end
			end
		end

		self.c ={}
		local X =2.75
		local Y =1
		self.c[1] = get_square( -X,  Y, 1, "c" )
		self.c[2] = get_square(  X,  Y, 2, "c" )
		self.c[3] = get_square( -X, -Y, 3, "c" )
		self.c[4] = get_square(  X, -Y, 4, "c" )
		X = X + .25
		self.c[5] = get_square( -X,  0, 5, "c" )
		self.c[6] = get_square(  X,  0, 6, "c" )

	self:draw_layers_end()

	local function make_result( tab )
		local count = 0
		local id = 0
		for i=1,#tab do
			if tab[i] then
				id = i
				count =count +1
			end
		end
		return count==1 and id or 0
	end

	self.cha_id = make_result( self.c )
	self.vid_id = make_result( self.v )

	self:set_bu_value( "Chapter",	self.cha_id )
	self:set_bu_value( "Video",		self.vid_id )
end
