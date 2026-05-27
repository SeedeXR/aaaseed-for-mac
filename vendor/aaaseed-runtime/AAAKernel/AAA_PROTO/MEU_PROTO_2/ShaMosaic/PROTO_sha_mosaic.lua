function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "art", "2d", "draw", "procedural", "texture", "vj" },
			help = 	{
						"pixelize a texture in a semi continous way,",
						" some mode are imitate a vision through a window made to hide what's behind.",
					}
			}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix = 1
	local iy = 1
	local SY = 1.
	local SYH = .4
	local SYT = 2.
	local DY = .2
	local SX3 = 8/3

	self:add_rendering()
	self:add_camera( nil, 16 )

	self:add_shading_ui({1,1})
	iy = iy + SY + DY

	local function add_but(	name, ox )
		ox = ox or 0
		self:add_button(	{ix+ox*SY,iy,	SY,SY},		name,	self,	"b_"..string.lower(name), 	false	)
		iy = iy + SY
	end
	
	-- add_but(	"Draw_box"	)
	local SXA = 1.5
 	local SXH = (8-SY-SXA)/2
	local DX = 4
	-- local function add_but_center(	name, ... )
	-- 	self:add_button({ix+SXH,iy,	SY,SY},		name,			self,	"b_"..name, 		false	)
	-- end
	local function add_group(	name,	b_two,	... )
		local pre = string.lower( name )
		self:add_button(				{ix,iy,			SXA,SY},	name,		self,	"b_"..pre.."_active", 		false	)
			:set_text_inside( true )
		ix = ix + SXA

		local bu,bv
		if b_two then
			bu = self:add_slider_two(	{ix,iy,			SXH,SY},	name.." U",	self, pre.."_u_min",	pre.."_u_max", 	... )
				:set_draw_step( "linear" ):set_draw_force_cr(false)
			bv = self:add_slider_two(	{ix+SXH+SY,iy,	SXH,SY},	name.." V",	self, pre.."_v_min",	pre.."_v_max", 	... )
				:set_draw_step( "linear" ):set_draw_force_cr(false)
		else
			bu = self:add_slider(		{ix,iy,			SXH,SY},	name.." U",	self, pre.."_u", 						... )
			bv = self:add_slider(		{ix+SXH+SY,iy,	SXH,SY},	name.." V",	self, pre.."_v", 						... )
		end
		bu:set_text( "U" ):add_values_def_integer():set_color_back( "u" )
		bv:set_text( "V" ):add_values_def_integer():set_color_back( "v" )
		ix = ix - SXA
			
		self:add_button(		{ix+SXA+SXH,iy,	SY,SY},		name.."_same",		self,	"b_"..pre, 		false	)
			:set_text_visible( false )
		iy = iy + SY
	end

	add_group( "Scale",		false,	16,		0,32	)
	iy = iy + DY

	bu = self:add_selector(		{ix,iy,		8,SY*1.5},		"How" )
		bu:set_item_text( 1, "No", "GB", "GB Sin", "Step GB", "Sin GB" )
		bu:set_nb( 5, 2 )
		bu:set_item_text( 7, "Sin", "Sin2", "SmoothStep" )
		--self:print( "before" )
		bu:set_target_param( sha:get_ref_frag_int(1) )	
	iy = iy + SY*1.5 + DY

	add_group( "Step",		true,	.5,1, 	0,1		)
	add_group( "Gain",		false,	.5, 	0,1		)
	add_group( "Bias",		false,	.5, 	0,1		)
	iy = iy + DY

	add_group( "Inf",		false,	1,		0,4   	)
	add_group( "Factor",	false,	1, 		0,4	)
	iy = iy + DY

	self:add_slider(			{ix+2,iy,	4,SY},	"Rotate",		sha:get_ref_frag_float(9),	nil, 	0, -2,2 )
		:set_color_back( "w" )
	iy = iy + SY + DY
	

	ix,iy = 9,3+DY
	self:add_size_uvf_video(	{ix,iy,		8,SY*2})
	iy = iy + SY*2 + DY

	self:add_transfo(			{ix,iy,		8,2.4} ) 
	iy = iy + 2.4 + DY
	
	local SYC = 5.5
	self:add_bu_texture_target_unit( {ix,iy, 8,SYC}, "Src" )
		:set_preset_use( false )
		--self:set_bu_texture_preset_use( 1, false )
	iy = iy + SYC + DY

	self:add_rgbf(				{ix,iy,		8,SY}	)
	iy = iy + SY + DY

end

function meu:init()
	local ref = self.ref
	ref.layer_attr = self:get_layer(1)
	-- ref.layer_back = self:get_layer(2)
	-- ref.layer_front = self:get_layer(3)
	-- ref.layer_flex = self:get_layer(4)
	self:add_shading( 2 )
end


function meu:update()
	local sha = self:get_shading()
	local scale = self.b_scale_active and self.scale_u or 1
	local inf = self.b_inf_active and self.inf_u or 1
	--aaa.show( self.inf_u, "inf_u" )
	sha:set_frag_vec4( 1,	inf,				self.b_inf		and self.inf_v		or inf,
							scale,				self.b_scale	and self.scale_v	or scale		)
	local step_min = self.b_step_active and self.step_u_min or 0
	local step_max = self.b_step_active and self.step_u_max or 1
	sha:set_frag_vec4( 2,	step_min,			self.b_step		and self.step_v_min	or step_min,
							step_max,			self.b_step		and self.step_v_max	or step_max	)
	local gain = self.b_gain_active and self.gain_u or .5
	local bias = self.b_bias_active and 1.-self.bias_u or .5
	sha:set_frag_vec4( 3,	gain,				self.b_gain      and (1-self.gain_v)  or gain,
							bias,				self.b_bias      and (1-self.bias_v)  or bias )
	local factor = self.b_factor_active and self.factor_u or 1
	sha:set_frag_vec4( 4,	factor,				self.b_factor  	 and self.factor_v    or factor	)
end


function meu:draw()
	local ref = self.ref
	self:draw_layers_begin()
		self:draw_layer(1)
		self:draw_layer(2)
	self:draw_layers_end()
end
