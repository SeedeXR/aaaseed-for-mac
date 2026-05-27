function meu:define_meu_infos( )
	return { 	author = "Mâa",
				date = "2019",
				version = 0,
				tags = { "Proprietary", "Vj", "art", "procedural", "2d","3d", "Draw" },
				help = "Render a texture as a serie of lines",
			}
end
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local ix,iy
	local SX, SY = 1, 1
	local DY = .2
	local DYB = .5

	ix,iy = 1,1

	self:add_camera()
	self:add_shading_ui( {1,1,	8,1} )
	iy = iy + 1 + DYB

	self:add_rendering( {ix,iy, nil,1.6} )
	iy = iy + 1.6 + DYB

	self:set_tab_key_def()
	local SYT = 3
	local SXL = 4
	for i=1,2 do
		self:add_bu_texture_target_unit( 	{ix,iy,			4,SYT}, "tex_height_"..i, i	)
		bu = self:add_slider(		{ix+4,iy,		4,SY},	"Displace "..i,				self,	"displace_"..i,				.25,	-1, 1		):set_color_back("w")
		bu = self:add_slider(		{ix+4,iy+SY,	2,SY},	"Displace Gamma "..i,		self,	"displace_gamma_"..i,		1,		.0001,	8	):set_color_back("w")
			bu:set_text( "Gamma "..i )
		bu = self:add_slider(		{ix+6,iy+SY,	2,SY},	"Displace V Offset "..i,	self,	"displace_v_offset_"..i,	0,		-1,	1		):set_color_back("v")
			bu:set_text( "V Offset "..i )
		iy = iy + SYT + DY
	end

	self:add_bu_texture_target_unit( 		{ix,iy,			4,4},	"tex_line", 3	)
	iy = iy + SYT + DY

	--bu = self:add_button( {ix, 8, 	3,	1 }, "b_speed", self, "b_speed"	):set_text_rect_ratio( 2 )
	--bu = self:add_slider(	{1,9,	8,1},	"Speed",	self, "speed",	.2,		-1,	1 )
	--bu:set_meter( false )

	ix, iy = 9, 2+DYB


	--iy = iy + 1
	local SX12 = 8 / 2
	local SX13 = 8 / 3
	local SX23 = SX13 * 2

	bu = self:add_slider(			{ix,iy,		SX12,SY},	"Size_u",		self,	"size_u",			1,		0,	16	):set_color_back("u")
		bu:set_meter( true )
	bu = self:add_slider(			{ix+SX12,iy,SX12,SY},	"Size_v",		self,	"size_v",			1,		0,	16	):set_color_back("v")
		bu:set_meter( true )
	iy = iy + SY

	-- bu = self:add_slider(	{ix,iy,	SX13,SY},	"Line_res",		nil,	nil,				64, 	2,	512	):set_color_back("x")
	-- 	bu:add_values_def( 128, 256 )
	-- 	bu:set_target_param( ref.nb_u )
	bu = self:add_slider(			{ix,iy,		2,SY},		"Line Before",	self,	"line_nb_before",	0,		0,	16	):set_color_back("v")
		bu:set_value_type_integer( true )
	bu = self:add_slider(			{ix+2,iy,	4,SY},		"Line Nb",		self,	"line_nb",			100,	0,	300	):set_color_back("v")
		bu:add_values_def( 14, 32, 42, 69, 100, 128, 150, 200, 250, 300 )
		bu:set_value_type_integer( true )
	bu = self:add_slider(			{ix+6,iy,	2,SY},		"Line After",	self,	"line_nb_after",	0,		0,	16	):set_color_back("v")
		bu:set_value_type_integer( true )
	iy = iy + SY
	iy = iy + DY

	bu = self:add_slider(			{ix,iy,			SX13,SY},	"Bottom",		self,	"bottom",			.25,	-1,	1	)
		bu:add_values_def( -.75, -.5, -.25, .5, .75 )
	bu = self:add_slider(			{ix+SX13,iy,	SX13,SY},	"Top",			self,	"top",				.25,	-1,	1	)
		bu:add_values_def( -.75, -.5, -.25, .5, .75 )
	bu = self:add_slider(			{ix+SX23,iy,	SX13,SY},	"Line_size",	self,	"line_size_y",		.1,		0,	1	)
	iy = iy + SY
	bu = self:add_slider(			{ix,iy,			SX13,SY},	"Angle",		self,	"angle",			.25,	0,	1	):set_color_back("u")
		bu:add_values_def( .1,.125,.2,.25,.3,.375,.4 )
--	bu = self:add_slider(	{ix+SX23,iy+SY,	SX13,SY},	"Angle delta",	self,	"angle_delta",		.25,	0,	1	):set_color_back("u")
--		bu:add_values_def( .1,.125,.2,.25,.3,.375,.4 )
	iy = iy + SY + DY
	self.ui.bu_back = self:add_rgbfa(	{ix,iy,		8,SY},		"Color Below", false )
	iy = iy + SY + DY
	SYT = 4
	self:add_bu_texture_target_unit( 		{ix,iy,			4,SYT},		"tex_color", 4	)
	self:begin_bu_group( "Heat" )
	bu = self:add_button(			{ix+4,iy,		2,SY},		"Vertical",self,	"b_heatmap_vertical", 	true )
	bu = self:add_slider(			{ix+4,iy+SY,	4,SY},		"Begin",	self,	"heatmap_begin", 	0,	-1,2 )
		bu:add_values_def( 1 )
	bu = self:add_slider(			{ix+4,iy+SY*2,	4,SY},		"End",		self,	"heatmap_end",  	1,	-1,2 )
		bu:add_values_def( 0 )
	bu = self:add_slider(			{ix+4,iy+SY*3,	3,SY},		"Gamma",	self,	"heatmap_gamma",	1,	1/8,8 )
	self:end_bu_group()

	iy = iy + SYT + DY
	self.ui.bu_rgb = self:add_rgbf(	{ix,iy,			8,SY } )

	self:set_tab_key( "More" )
	ix,iy = 1,5
	bu = self:add_sliders_uvw(		{ix,iy,			8,SY},		"Tra",		self.tra,		false,	0,	8	)
	bu = self:add_sliders_uvw(		{ix,iy+SY,		8,SY},		"Tra End",	self.tra_end,	false,	0,	8	)
	iy = iy + SY*2 + DY

	ix,iy = 9,3
	bu = self:add_sliders_xyz(		{ix,iy,			8,SY},		"Pos",	self.pos, 	false,	2.5	)
	bu = self:add_sliders_xyz(		{ix,iy+1,		8,SY},		"Rot",	self.rot, 	false,	1	)
	iy = iy + SY*2 + DY

end

function meu:init()
	local ref = self.ref

--	self.time = 0
	self.pos		= { x=0, y=0, z=0 }
	self.rot		= { x=0, y=0, z=0 }
	self.tra		= { u=0, v=0, w=0 }
	self.tra_end	= { u=0, v=0, w=0 }

	local bdd			=	self:get_layer_bdd(1)
	local layer			=	self:get_layer(1)
	local prg			=	param.get_ref
--	ref.nb_u			=	prg( bdd, "nb_u" )
	local model			=	aaa.obj.get_down_no_error( layer, "model" )
	ref.size_u			=	prg( model, "size_u" )
	ref.size_v			=	prg( model, "size_v" )

	local transfo		=	aaa.obj.get_down_by_class_no_error( layer, "transfo_trs" )

	local tra = {}
	tra.x, tra.y, tra.z	=	prg( transfo, "translate_x" ), prg( transfo, "translate_y" ), prg( transfo, "translate_z" )
	ref.tra = tra

	local rot = {}
	rot.x, rot.y, rot.z	=	prg( transfo, "rotate_x" ), prg( transfo, "rotate_y" ), prg( transfo, "rotate_z" )
	ref.rot = rot

	self.ph_fade		=	0
	
	local sha = self:add_shading( 1, "2054" )
	sha:set_save_frag_float( false,	1,6 )
	sha:set_save_frag_float( false,	8,13 )
	sha:set_save_frag_vec4(	 false,	2,2 )
end

function meu:set_displace( d1, d2 )
	if d1 then self.displace_1 = d1 end
	if d2 then self.displace_2 = d2 end
end

function meu:do_transition_v1( ph )
	local a_below	= math.fn_linear( ph, 1,	-1,0,	.2,0,	.8,.0,	1,1,	2,1 )
	local a_glo		= math.fn_linear( ph, 1,	-1,0,	0,0,	.2,1,	2,1 )
	self:set_bu_value( "color below_alpha", a_below )
	self:set_mu_value( a_glo )
end
function meu:do_transition_v2( ph )
	local d1,d2		= math.fn_linear( (ph-.25)/.75, 2,	-1,0,0,	0,0,0,	.5,0,1,	1,1,1,	2,1,1 )
	self:set_displace( d1, d2*.125 )
	local a_below	= math.fn_linear( ph, 1,	-1,0,	0,0,	.25,1,	1,1 )
--	local a_glo		= math.fn_linear( ph, 1,	-1,0,	0,0,	.2,1,	2,1 )
	self:set_bu_value( "color below_alpha", a_below )
--	self:set_mu_value( a_glo )
end
function meu:do_transition_lr( ph, b_lr )
	local sha = self:get_shading()
	local l = sha:get_frag_vec4_y(2)
	local v
	if b_lr then	v = -l + (1-ph) * ( 1+l )
	else			v = 0 + ph * ( 1-l )
	end
--	self:print( "transition "..v )
	sha:set_frag_vec4_x( 2, v )
end
function meu:do_transition_bu( ph )
	self.displace_2 = clamp_01( ph*2 - 1 ) * .125
	ph = ph*4 / 3
	self.line_nb_after = math.floor( -125 * math.pow(clamp_01(1.-ph), 1. ) ) + 1
end
function meu:do_transition(ph)
	self:do_transition_bu( ph )
	self.ph_fade = ph
end

function meu:draw_2054( center_y )
	local sha = self:get_shading()
	--	sha:set_frag_float( 9, .25 )

	local angle	= self.angle
	local nb	= self.line_nb
	local dv	= 1. / (nb-1.)

--	local t = wrap_01( aaa.time.t / 2.)
--	local tr = self.tra
--	local tre = self.tra_end

--	gol.rotate_x( -.25 )
	local ib = nb-1+self.line_nb_after
	local ie = -self.line_nb_before
	local nb = ib-ie

	local su = self.size_u / 2
	local sv = self.size_v_out / 2

	--local inter = 0
	for i=ib,ie,-1 do	
		--local ph2 = clamp_01( (1-inter) * 1. - t + .25 )
		--ph2 = aaa.math.gain_bias( ph2, .5, .3 )	
		local v = i * dv
		gol.push_matrix()	

			sha:set_frag_float_8( v )
--			sha:set_frag_float( 9, ph*nb - t*nb)

			if true then
				gol.translate_y( ( v -.5 ) * self.size_v )
				gol.rotate_x( angle )	--	- self.angle_delta * (ib-i) / nb )
				--gol.translate_y( center_y )
				--gol.translate( tr.u + ph2 * tre.u, tr.v + ph2 * tre.v, tr.w + ph2 * tre.w )

				--everything is ready but we need to update uniform change
				gol.update_uniform_fragment_float()
				--aaa.draw_rect_uv_at_z( -su,-sv, su,sv, 0 )
				aaa.draw_rect_uv( -su,-sv+center_y, su,sv+center_y )
			else
				gol.translate_y( ( v -.5 ) * self.size_v )
				gol.rotate_x( angle )	--	- self.angle_delta * (ib-i) / nb )
				gol.translate_y( center_y )
				--gol.translate( tr.u + ph2 * tre.u, tr.v + ph2 * tre.v, tr.w + ph2 * tre.w )

				self:draw_layer( 2 )
			end

		gol.pop_matrix ()
		--gol.translate_y( decal )
		--inter = inter + 1. / (nb-1)
	end

end

function meu:draw_2054_min( center_y )
	local sha = self:get_shading()

	local angle	= self.angle
	local nb	= self.line_nb
	local dv	= 1. / (nb-1.)

	local ib = nb-1+self.line_nb_after
	local ie = -self.line_nb_before
--	local nb = ib-ie

	local su = self.size_u / 2
	local sv = self.size_v_out / 2

	for i=ib,ie,-1 do	
		local v = i * dv
		gol.push_matrix()
			sha:set_frag_float_8( v )	-- v texture coordonate (which line is drawn)
			if true then
				gol.translate_y( ( v -.5 ) * self.size_v )
				gol.rotate_x( angle )
				gol.update_uniform_fragment_float()
				aaa.draw_rect_uv( -su,-sv+center_y, su,sv+center_y )
			else
				gol.translate_y( ( v -.5 ) * self.size_v )
				gol.rotate_x( angle )
				gol.translate_y( center_y )
				self:draw_layer( 2 )
			end
		gol.pop_matrix ()
	end
end

function meu:draw_2054_fade_mono_ub( center_y )
	local sha = self:get_shading()

	local angle	= self.angle
	local nb	= self.line_nb
	local dv	= 1. / (nb-1.)

	local ib = nb-1+self.line_nb_after
	local ie = -self.line_nb_before
--	local nb = ib-ie

	local su = self.size_u / 2
	local sv = self.size_v_out / 2

	for i=ib,ie,-1 do	
		local v = i * dv
		gol.push_matrix()
			sha:set_frag_float_8( v )	-- v texture coordonate (which line is drawn)
			if true then
				gol.translate_y( ( v -.5 ) * self.size_v )
				gol.rotate_x( angle )
				--sha:set_frag_float_2( .125 * clamp_01(self.ph_fade*4 + i/125) )
				local f =  (112-i+self.line_nb_after) / 75 - .04
				f = f + clamp_01( (self.ph_fade-.75) * 4 )
				f = math.pow( f, .5 )
				sha:set_frag_float_2( .125 * clamp_01( f ) )
--				sha:set_frag_float_2( .125 )
				gol.update_uniform_fragment_float()
				if i<=(ib+1) then
					if i==ib then
						gol.color_white( .55 )
					-- elseif i==ib-1 then
					-- 	gol.color_white( .5 )
					else
						gol.color_white()
					end
				end
				--gol.color_white()
				aaa.draw_rect_uv( -su,-sv+center_y, su,sv+center_y )
			else
				gol.translate_y( ( v -.5 ) * self.size_v )
				gol.rotate_x( angle )
				gol.translate_y( center_y )
				self:draw_layer( 2 )
			end
		gol.pop_matrix ()
	end
end
function meu:draw()
	local sha = self:get_shading()

	self:draw_layers_begin()

		local r,g,b,a = self.ui.bu_back:get_rgba()
		sha:set_frag_vec4( 1, r,g,b, a  ) -- problem : all slider change all values ..?!

		local bot, top		=	self.bottom, self.top
		self.size_v_out		=	top - bot
		local center_y		=	self.size_v_out*.5 + bot
		param.set( self.ref.size_u, self.size_u )
		param.set( self.ref.size_v, self.size_v_out )

		local f1 = self.line_size_y / self.size_v_out
		local f2 = -bot / self.size_v_out

		sha:set_vert_int_1( 0 )

		sha:set_frag_int_1( self.b_heatmap_vertical and 1 or 0 )

		sha:set_frag_float_1_6(		self.displace_1,			self.displace_2,
									self.displace_gamma_1,		self.displace_gamma_2,
									self.displace_v_offset_1,	self.displace_v_offset_2
									)
		--6 is unused fade
		sha:set_frag_float_9_13( 	f1,f2,
									self.heatmap_begin, self.heatmap_end, self.heatmap_gamma
									)
		self:draw_layer( 1 )

		gol.push_matrix()

			local pos = self.pos
			gol.translate( pos.x, pos.y, pos.z )

			local rot = self.rot
			gol.rotate_z( rot.z )
			gol.rotate_y( rot.y )
			gol.rotate_x( rot.x )

			gol.set_quad_uv()
			self:draw_2054_fade_mono_ub( center_y )

		gol.pop_matrix ()

	self:draw_layers_end()
end

function meu:get_preset_nb()	return 24	end