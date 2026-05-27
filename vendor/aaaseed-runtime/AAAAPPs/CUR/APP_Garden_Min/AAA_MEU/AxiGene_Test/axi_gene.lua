function meu:get_noise3( x,y,z )
	--	v	= aaa.math.get_turbulence( x, y, z, freq, nb_harm )	--	by default freq = 1., nb_haram = 1
	--	v	= aaa.math.get_fractalsum( x, y, z, freq, nb_harm )	--	by default freq = 1., nb_haram = 1
	--was freq = 1 and nb_harm = 3
	return aaa.math.get_turbulence( x,y,z, 1, 3 ) - .5
end
function meu:get_noise3_pos( x,y,z, f,harm_nb )
	--	v	= aaa.math.get_turbulence( x, y, z, freq, nb_harm )	--	by default freq = 1., nb_haram = 1
	--	v	= aaa.math.get_fractalsum( x, y, z, freq, nb_harm )	--	by default freq = 1., nb_haram = 1
	--was freq = 1 and nb_harm = 3
	return self.intensity * aaa.math.get_turbulence_improved( x,y,z, f, harm_nb or 1 )
end
function meu:get_noise_cumul( x,y,z, f,harm_nb  )
	f = f or 1
	f = f * 1.1
--	x,y = x+362.17 , y+1300.2511
--	x,y = x+3652.2354 , y+120.3658
--	x,y = x+120.3658, y+3652.2354 
--	x,y = x,  y
	--harm_nb = 4 * (y+3.5)/4 + 1
	--harm_nb = 1
	
	local f2 = f *(1 - math.pow( x*x + y*y,.25 ) *.15 ) * 4.
	x,y = -y-17.2561313,x
	--y = y + math.floor(y*4)*100
	--x = x - math.floor(x*8)*.05

	return self:get_noise3_pos( x ,y, self:get_noise3_pos( x,y,z, f2,harm_nb ) * 5.7 , f2, harm_nb )
end
function meu:get_noise_jan_4( x,y,z, f,harm_nb  )
	f = f or 1
	f = f * 1.1 * .5

	--harm_nb = 4 * (y+3.5)/4 + 1
	harm_nb = 1
	
	local nor = math.pow( x*x + y*y, .5 )
	local gen = (.5 + clamp_01( nor / 2. ) * 4) * ( 1- math.abs(y/2) )
	y = y + math.floor(y*16)*.125
	x = x + math.floor(x*16)*.125

	local f2 = f *(.60 - math.pow( (x*x + y*y)/8,.125 ) * .435 ) * 16. * 4
	--local f2 = f
	--x,y = -y-10.2561313,x
	--y = y + math.floor(y*4)*100
	--x = x - math.floor(x*8)*.05
	--return val * f2
	local ph = self:get_noise3_pos( x ,y, self:get_noise3_pos( x,y,z, f2,1 ) * 1.7 , f2, 1 )
--	return (1/64) * ( math.max( math.sin( (y*20+.0) * 1 + ph*2.), math.sin( (y*8+.0) * 1 + ph*2.) )  )
	--return  (math.sin( (y*0+.0) * 1 + ph) + 1)  / 8
	return ph * gen
end
function meu:get_noise_dec_30( x,y,z, f,harm_nb  )
	f = f or 1
	f = f * 1.1
--	x,y = x+362.17 , y+1300.2511
--	x,y = x+3652.2354 , y+120.3658
--	x,y = x+120.3658, y+3652.2354 
--	x,y = x,  y
	--harm_nb = 4 * (y+3.5)/4 + 1
	--harm_nb = 1
	

	local f2 = f *(.55 - math.pow( (x*x + y*y)*2,.125 )  *.35 ) * 16.
	--local f2 = f
	--x,y = -y-10.2561313,x
	--y = y + math.floor(y*4)*100
	--x = x - math.floor(x*8)*.05
	--return val * f2
	return self:get_noise3_pos( x ,y, self:get_noise3_pos( x,y,z, f2,harm_nb ) * 1.7 , f2, harm_nb )

end
function meu:get_noise_feb( x,y,z, f,harm_nb1,harm_nb2  )
	f = f or 1
--	x,y = x+362.17 , y+1300.2511
--	x,y = x+3652.2354 , y+120.3658
--	x,y = x+120.3658, y+3652.2354 
--	x,y = x,  y
	--harm_nb = 4 * (y+3.5)/4 + 1
	--harm_nb = 1
	
	--local f2 = f *(1 - math.pow( x*x + y*y,.25 ) *.15 ) * 4.
	--x,y = -y-17.2561313,x
	--y = y + math.floor(y*1)*-.125*1
	--x = x + math.floor(x*1)*-.125*1
	local f = f * clamp( (3+y) / 8, 0, 8 )
	f = f * clamp( (3-y) / 8, 0, 8 )
	--f = math.pow( f, 4 )
	local f2 = math.pow( f, .7 )
	--*y * .75
	local v = self:get_noise3_pos( x,y, self:get_noise3_pos( x,y,z, f2,harm_nb2 ) * 5.7 , f, harm_nb1 )
	return v * ( 1 - math.pow( x*x + y*y, .5 ) / 5.5 )
	--	return v * ( 1 - clamp_01( (x) / 2.5 ) )
--	return v

end
function meu:get_xmas( x,y,z, f,harm_nb  )
	harm_nb = 1
	local  v = 0
	local val =  aaa.img.get_component_uv( 0, .4*x/2.42+.5, .4*y/2.42+.5,	5, false, true )
	--local val = aaa.img.get_component_xy( 0, x*200, y*200, 1 )
	--if val > 0 and val < 1 then self:print(val) end
	--v = val * .25

	f = f or 1
	f = f * 1.1
	harm_nb = harm_nb or 3
--	x,y = x+362.17 , y+1300.2511
--	x,y = x+3652.2354 , y+120.3658
--	x,y = x+120.3658, y+3652.2354 
--	x,y = x,  y
	x,y = -y-17.2561313,x
	v = self:get_noise3_pos( x,y, self:get_noise3_pos( x,y,z, f,4 ) * 50.7 , f, 4 )
	if val >.1 then
		v = .125 * .36
	end
	return v
end

local function mypow( x, pow )
	if x >= 0 then
		return math.pow( x, pow )
	end
	return - math.pow( -x, pow )
end

function meu:deform( x,y )
	return	x + self.intensity * ( aaa.math.get_turbulence( x, y, 0,	1, 3 )	- .25 ),
			y + self.intensity * ( aaa.math.get_turbulence( x, y, 3.14, 1, 3 )	- .25 )
end
function meu:deform_cumul( x,y )
	return	x + self.intensity * ( self:get_noise_cumul( x*2, y*2, 0,	1, 3 )	- .25 ),
			y + self.intensity * ( self:get_noise_cumul( x*2, y*2, 3.14, 1, 3 )	- .25 )
end
function meu:get_preset_nb() 	return 64	end

function meu:define_ui()
	local ui = self.ui
	local bu
	local ix = 1
	local iy = 1
	local SY = .9
	local SX = 8/3
	local DY = .2
	local SYM = SY * .8

	self:add_camera( {9,nil},16):set_preset_use( false )
	ui.bu_info			=	self:add_text_info(	{ix,iy,	8,SYM}, "NB" )

	iy = iy + SY
	bu = self:add_button(	{	ix,iy,	4, SY }, "What" )
 		bu:set_menu{ 	"Cercles", "", "Spiral", "Radial",
		 				"Heart", "Square", "Turb", "PbN",
						"Img", "Points", "", "NF", 
						"Spiral_V2", "", "", "",
						"Hilbert", "Hilbert_a", "Hilbert_b", ""
					}:set_nb_min_0( 4, 5 )
		bu:set_text_selector(true)
		ui.bu_what = bu
	self:add_trig_method(	{	ix+4,	iy,			4,SY},	"Gene",			self, "gene_sel"			):set_color_back("restart")		
	iy = iy + SY		

	bu = self:add_button(	{	ix,		iy,		SY,SY },	"Draw gene",	self, "b_draw_gene",		false )
	bu = self:add_button(	{	ix+4,	iy,		SY,SY },	"Always",		self, "b_gene_always",		false )
		bu:set_text_rect_ratio( 3 )
	iy = iy + SY + DY
	
	self:set_tab_key( "Main" )

	self:add_slider_two(	{ix,iy,		4,SY},	"nb_u", 		self, "nb_u_min", "nb_u",	16, 16, 1, 1024 )
		:set_value_type_integer( true ):set_color_back("u")
	self:add_slider_two(	{ix+4,iy,	4,SY},	"nb_v", 		self, "nb_v_min", "nb_v",	16, 16, 1, 1024 )
		:set_value_type_integer( true ):set_color_back("v")
	iy = iy + SY

	self:add_slider_two(	{ix,iy,		4,SY},	"Radius", 		self, "radius_beg", "radius",	0, 2, 0, 4 ):add_values_def( 1, 1.5, 3, 4 )
--	self:add_slider(		{ix+4,iy,	4,SY},	"Radius",		self, "radius", 			2, 0, 4 ):add_values_def( 1, 1.5, 3, 4 )
	self:add_slider(		{ix+4,iy,	4,SY},	"intensity",	self, "intensity", 			0.25, -1, 1 )
	iy = iy + SY + DY

	self:add_slider(		{ix,iy,		4,SY},	"Pass Nb",		self, "pass_nb", 			1, 1, 64 ):set_value_type_integer( true )
	self:add_slider(		{ix+4,iy,	4,SY},	"Pass Dist",	self, "pass_d", 			0, 0, 1000 )

	iy = iy + SY +DY
	self:add_slider_two(	{ix,iy,		8,SY},	"X", 			self, "th_min", "th_max",	0, 1, 0, 1 )
	iy = iy + SY
	bu = self:add_button(	{ix,iy,		SY,SY},	"Inv",			self, "b_inv",				false )
	self:add_slider(		{ix+4,iy,	4,SY},	"Gain", 		self, "gain",				0.5, 0, 1 )
	self:add_slider(		{ix+4,iy+1,	4,SY},	"Bias", 		self, "bias",				0.5, 0, 1 )
	iy = iy + SY*2 + DY
	bu = self:add_slider(	{ix,iy,		8,SY},	"Final Scale",		self, "final_scale",		1, 0 , 4 )
	bu = self:add_slider(	{ix,iy+SY,	4,SY},	"Final Offset X",	self, "final_offset_x",		0, -4, 4 )
		:set_text("X"):set_color_back("x")
	bu = self:add_slider(	{ix+4,iy+SY,4,SY},	"Final Offset Y",	self, "final_offset_y",		0, -4, 4 )
		:set_text("Y"):set_color_back("y")
	iy = iy + SY*2 + DY
	self:add_slider(	{ix,iy+SY,	4,SY},	"nb_try",		self, "nb_try", 			1, 1, 64 ):set_value_type_integer( true )


	ix = 9	
	iy = 3	
	SY = .7

	bu = self:add_button(	{ix,iy,		SY,SY},	"offset.before.active",				self, "b_before_active", 					false ):set_text_visible(false)
	bu = self:add_button(	{ix+1,iy,	SY,SY},	"offset.before.draw",				self, "b_before_draw",						false ):set_text_visible(false)
	iy = iy + SY*1

	bu = self:add_button(	{ix,iy,		SY,SY},	"offset.before_before.active",		self, "b_offset_before_before_active", 		false ):set_text_visible(false)
	bu = self:add_button(	{ix+1,iy,	SY,SY},	"offset.before_before.draw",		self, "b_offset_before_before_draw",		false ):set_text_visible(false)
	self:add_slider(		{ix+2,iy,	5,SY},	"offset.before_before.value", 		self, "offset_before_before_value",			0, -8, 8 ):set_text("offset")
		self:add_button(	{ix+7,iy,	SY,SY},	"offset.before_before.relative",	self, "b_offset_before_before_relative",	false ):set_text_visible(false)
	iy = iy + SY*1

	bu = self:add_button(	{ix,iy,		SY,SY},	"resample.before.active",			self, "b_resample_before", 				false ):set_text_visible(false)
	bu = self:add_button(	{ix+1,iy,	SY,SY},	"resample.before.draw",				self, "b_resample_before_draw",			false ):set_text_visible(false)
	self:add_slider(		{ix+2,iy,	3,SY},	"resample.before.nb", 				self, "resample_before_nb",				0, 0, 16 ):set_text("nb"):set_value_type_integer(true)
	self:add_slider(		{ix+5,iy,	3,SY},	"resample.before.interpolator", 	self, "resample_before_interpolator",	0.5, 0,1 ):set_text("where")
	iy = iy + SY*1

	bu = self:add_button(	{ix,iy,		SY,SY},	"offset.before_after.active",		self, "b_offset_before_after_active", 		false ):set_text_visible(false)
	bu = self:add_button(	{ix+1,iy,	SY,SY},	"offset.before_after.draw",			self, "b_offset_before_after_draw",			false ):set_text_visible(false)
	self:add_slider(		{ix+2,iy,	5,SY},	"offset.before_after.value", 		self, "offset_before_after_value",			0, -8, 8 ):set_text("offset")
		self:add_button(	{ix+7,iy,	SY,SY},	"offset.before_after.relative",		self, "b_offset_before_after_relative",		false ):set_text_visible(false)
	iy = iy + SY*1

	iy = iy + SY*.5

	bu = self:add_selector(	{ix,iy,	8,SY*1}, "Curve" )
	bu:set_nb( 5, 1 )
	bu:set_item_text( 2, "CatMull", "Bezier3", "Bezier4", "Try" )
	bu:set_text_draw( false )
	ui.bu_curve = bu
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	4,SY},	"Interpolator",						self, "interpolator", 					0, -4, 4 )
		bu:add_values_def( -.5, 1, 2, 3 )								
	bu = self:add_slider(	{ix+4,iy,	4,SY},	"Segment",							self, "segment_nb", 					4, 1, 64 )
		bu:set_value_type_integer( true )								
	bu = self:add_slider(	{ix,iy+SY,	4,SY},	"Asymetrisator",					self, "asymetrisator", 					0, -1, 1 )
		bu:add_values_def( .1, .5, .9 )			
	iy = iy + SY*2.5

	bu = self:add_button(	{ix,	iy,		SY,SY},	"offset.after.active",				self, "b_after_active", 				false ):set_text_visible(false)
	bu = self:add_button(	{ix+1,	iy,		SY,SY},	"offset.after.draw",				self, "b_after_draw",					false ):set_text_visible(false)
	iy = iy + SY*1

	bu = self:add_button(	{ix,	iy,		SY,SY},	"offset.after_before.active",		self, "b_offset_after_before_active", 	false ):set_text_visible(false)
	bu = self:add_button(	{ix+1,	iy,		SY,SY},	"offset.after_before.draw",			self, "b_offset_after_before_draw",		false ):set_text_visible(false)
	self:add_slider(		{ix+2,	iy,		5,SY},	"offset.after_before.value", 		self, "offset_after_before_value",		0, -8, 8 ):set_text("offset")
		self:add_button(	{ix+7,	iy,		SY,SY},	"offset.after_before.relative",		self, "b_offset_after_before_relative",	false ):set_text_visible(false)
	iy = iy + SY*1
	
	bu = self:add_button(	{ix,	iy,		SY,SY},	"resample.after.active",			self, "b_resample_after", 				false ):set_text_visible(false)
	bu = self:add_button(	{ix+1,	iy,		SY,SY},	"resample.after.draw",				self, "b_resample_after_draw",			false ):set_text_visible(false)
	self:add_slider(		{ix+2,	iy,		3,SY},	"resample.after.nb", 				self, "resample_after_nb",				0, 0, 16 ):set_text("nb"):set_value_type_integer(true)
	self:add_slider(		{ix+5,	iy,		3,SY},	"resample.after.interpolator",		self, "resample_after_interpolator",	0.5, 0,1 ):set_text("where")
	iy = iy + SY*1

	
	bu = self:add_button(	{ix,	iy,		SY,SY},	"offset.after_after.active",		self, "b_offset_after_after_active", 	false ):set_text_visible(false)
	bu = self:add_button(	{ix+1,	iy,		SY,SY},	"offset.after_after.draw",			self, "b_offset_after_after_draw",		false ):set_text_visible(false)
	self:add_slider(		{ix+2,	iy,		5,SY},	"offset.after_after.value", 		self, "offset_after_after_value",		0, -8, 8 ):set_text("offset")
		self:add_button(	{ix+7,	iy,		SY,SY},	"offset.after_after.relative",		self, "b_offset_after_after_relative",	false ):set_text_visible(false)
	iy = iy + SY*1

	bu = self:add_button(	{ix,	iy,		SY,SY},	"Line",								self, "b_draw_line",							false )
	bu = self:add_button(	{ix+4,	iy,		SY,SY},	"Point",							self, "b_draw_point",					false )
	bu = self:add_button(	{ix,	iy+SY,	SY,SY},	"Draw Order",						self, "b_draw_order",					false )
	bu = self:add_button(	{ix+5,	iy+SY,	SY,SY},	"Color",							self, "b_draw_colored",					false )
	iy = iy + SY*2

	self:set_tab_key( "Position" )
	ix = 9	
	iy = 3	

	local H = 3
	local HH = H*.5
	bu = self:add_slider_two({ix,	iy,		4,H},	"Pos"			)
		ui.bu_pos = bu
	bu = self:add_trig_method(	{ix+4,iy,	2,HH},	"1x1",			self, 	"set_center_11"		)
	bu = self:add_selector(		{ix+6,iy,	2,HH},	"2x2"			)
		bu:set_nb( 2, 2 )
	bu = self:add_selector(	{ix+4,iy+HH,	2,HH},	"3x3"			)
		bu:set_nb( 3, 3 )
	bu = self:add_selector(	{ix+6,iy+HH,	2,HH},	"4x4"			)
		bu:set_nb( 4, 4 )

	iy = iy + H
	bu = self:add_button(		{ix,iy,		SY,SY},	"Swap XY",		self, "b_swap_xy",		false )

	self:set_tab_key( "TEX" )
	ix,iy = 1,5
	local tex_nb = 4
	local SYT = 4
	self:add_bu_texture(	{ix,iy,		4,SYT}, "Tex_"..1,	1, true )

	bu = self:add_selector(		{ix+4,iy,	4,SY},	"Compo_2"	)
		bu:set_item_text( 1, "R", "G", "B", "A", "Gr" )
		--bu:set_target_lua( )
	self:add_bu_texture(	{ix+4,iy+SY,4,SYT},	"Tex_"..2,	2, true )

end
function meu:init()
	self.__strips = LINE_STRIP_TABLE:create( self:get_name() )
end

function meu:set_center_11()
	local bu = self.ui.bu_pos
	self:print( "center" )
	--todo why get_value() is wrong here
	bu:set_value_cano( .5, 1 )
	bu:set_value_cano( .5, 2 )
end
function meu:add_new()			self.__strips:add_new()			end
function meu:add_pt_xyz( x,y )
	x = ( x + self.out_offset_x ) * self.final_scale + self.final_offset_x
	y = ( y + self.out_offset_y ) * self.final_scale + self.final_offset_y

	if self.b_swap_xy then		self.__strips:add_pt_xyz( y,x )
	else						self.__strips:add_pt_xyz( x,y )
	end
end
function meu:set_closed( b )	self.__strips:set_closed( b )			end
function meu:add_line_xy(  x1,y1, x2,y2 )
	self:add_new()
	self:add_pt_xyz( x1,y1 )
	self:add_pt_xyz( x2,y2 )
end

function meu:convert_to( curve, ... ) -- nb, interpolator, asymetrisator )
	if curve ~= "" then
		local strips = self:get_strips()
		local nb = self.segment_nb
		if		curve == "CatMull"	then	strips:convert_to_catmull_rom(	... )
		elseif	curve == "Bezier3"	then	strips:convert_to_bezier_by3(	... )
		elseif	curve == "Bezier4"	then	strips:convert_to_bezier(		... )
		elseif	curve == "Try"		then	strips:convert_to_try(			... )
		end
	end
end

function meu:do_passes( fn, cx,cy, r, ... )
	local nb = self.pass_nb
	local sc = self.pass_d / 1000
	--if nb>1 then sc = r * .25 *.25 / 16 end
	
	for i=1,nb do
		self.pass_id = i
		self:print( "do_passes() loop "..i )
		self.pass_inter = nb==1 and 1 or (i-1)/(nb-1)
		math.randomseed(i*1317+42)
		local a = (i-1)/nb * math.pi2
		self.out_offset_x = sc * math.cos(a)
		self.out_offset_y = sc * math.sin(a)
		fn( self, cx, cy, r, ... )	
	end
	--todo sort point by hilbert
end

-- generic generator
function meu:gene_sel()

	local ui = self.ui
	local sel = ui.bu_what:get_item_text()
	if not sel then return end
	
	sel = string.lower( sel )
	self:print( sel )
	local name = "gene_"..sel
	self:print( name )
	local fn = self[name]
	if not fn then
		self:box_debug( "Axi_gene.lua have no fn "..name.."()" )
		return
	end

	self:print( "gene_sel Begin" )

	local bu = self.ui.bu_pos
	--todo why get_value() is wrong here
	local cx,cy = bu:get_value_cano_clamped(1) - .5, bu:get_value_cano_clamped(2) - .5
	cx,cy = cx*8, cy*8 * 9 / 16
	
	math.randomseed( 42*42*42*44 )

	local strips = self:get_strips_new() --refine

	self.out_offset_x = 0
	self.out_offset_y = 0

	self:do_passes( fn, cx,cy, self.radius )

	if self.b_draw_gene then gol.color_cyan() strips:draw() end
	
	if self.b_before_active then
		if self.b_offset_before_before_active and self.offset_before_before_value~=0 then
			strips:offset_on_normal( self.offset_before_before_value, self.b_offset_before_before_relative )
			if self.b_before_draw and self.b_offset_before_before_draw then gol.color( .25, .75, 1 ) strips:draw() end
		end
		if self.b_resample_before then
			for i=1,self.resample_before_nb do	strips:convert_to_midpoint( self.resample_before_interpolator )	end
			if self.b_before_draw and self.b_resample_before_draw then gol.color( 0, .75, 1 ) strips:draw() end
		end
		if self.b_offset_before_after_active and self.offset_before_after_value~=0 then
			strips:offset_on_normal( self.offset_before_after_value, self.b_offset_before_after_relative  )
			if self.b_before_draw and self.b_offset_before_after_draw then gol.color( .5, .75, 1 ) strips:draw() end
		end
	end
	
	self:convert_to( ui.bu_curve:get_item_text(), self.segment_nb, self.interpolator, self.asymetrisator )

	--strips:decimate_point( 7, 160, 132, 0, 1 )
	--strips:decimate_point( 7, 128, 132, 0, 1 )
	--math.randomseed(42)
	--strips:decimate_point_gauss( nil, 4, 17, 1,19  )

	if self.b_after_active then
		if self.b_offset_after_before_active and self.offset_after_before_value~=0 then
			strips:offset_on_normal( self.offset_after_before_value, self.b_offset_after_before_relative )
			if self.b_after_draw and self.b_offset_after_before_draw then gol.color( .25, .5, 1 ) strips:draw() end
		end
		if self.b_resample_after then
			for i=1,self.resample_after_nb do	strips:convert_to_midpoint(self.resample_after_interpolator)	end
			if self.b_after_draw and self.b_resample_after_draw then gol.color( 0, .5, 1 ) strips:draw() end
		end
		if self.b_offset_after_after_active and self.offset_after_after_value~=0 then
			strips:offset_on_normal( self.offset_after_after_value, self.b_offset_after_after_relative )
			if self.b_after_draw and self.b_offset_after_after_draw then gol.color( .5, .5, 1 ) strips:draw() end
		end
	end

	--todo

	--strips:deform_on_normal( false, self, "get_noise_jan_4", .5, 12.5 )
--	strips:deform_on_normal( false, self, "get_noise_dec_30", .5, 12.5 )
--	strips:deform_on_normal( false, self, "get_xmas", .5, 2 )

	--self:convert_to( ui.bu_curve:get_item_text(), 4, 1, 4 )

	--self:convert_to( ui.bu_curve:get_item_text(), self.segment_nb, self.interpolator, self.asymetrisator )
	gol.color_white()

	self:print( "gene_sel End" )
end

function LINE_STRIP:deform_on_normal( src_pts, dst_pts, b_relative, tab, deform_method, freq, harm_nb )
	local fn = tab[deform_method] 

	freq = freq * .5 * 3.
	local src_index = self.__indexes
	local nb_pt = #src_index
	--todo deal with close open ...
	if nb_pt >= 2 then
		local dst_index = dst_pts:get_nb()
		local function add_pt( pt )
			dst_pts:add_pt_xy( pt[1], pt[2], pt[3] )
			dst_index = dst_index + 1
			self:add_index( dst_index )
		end
		self.__indexes = {}

		local fn_tg = V2.get_tangent_scaled
		local fn_normal = b_relative 	and	V2.get_normal_raw
										or	V2.get_normal_scaled

		local a
		local b = src_pts:get_pt( src_index[1] )
		for src_i = 1,nb_pt do
			a = b 
			b = src_pts:get_pt( src_index[src_i] )

			local nor = V2.norm(a)
			local f = nor / 4.5 --2.84
			--f = 1
			f = clamp_01( V2.norm(a)/2.5 )
			--f = (1 - math.cos( f * 1.8 * math.pi2 )) *.5
			--f = (1 - math.cos( f * 1.8 * math.pi2 )) *.5

			--f = 1 - wrap_01( f * 31.85) 
			local fac
			--fac = fn( tab, b[1], b[2], b[3], freq, harm_nb )
			--fac = aaa.math.get_turbulence( a[1], a[2], a[3], freq, 2 ) * 16. * .75
--			fac = fac * aaa.math.gain_bias( clamp_01( -(a[2]+2.5)/7. ), .6, .5 )
			fac = 0
			f = wrap_01( f * 2.05 * fac * 2.  )
			--f = interpolate( 0, wrap_01( f * 2.05 * fac * 2.  ), fac/3)
			--f = math.pow( f,1.5)
			--f = f * ( .5 + 3* nor/2.84 )
			--f = f * (1 -( nor - .41 ) / 2.01 )
			local off --= fn( tab, b[1], b[2], b[3], freq, harm_nb )
			off = f * .25 --* -1.05
			--off= off * clamp_01( (5-nor) / 5 )
		--	local tmp = clamp_01( 1. - math.max( math.abs(a[1]/3.9), math.abs(a[2]/2.6) ) )
		--	off = off * aaa.math.gain_bias( tmp, .5, .80 ) * 1.

			--off = interpolate ( off, 0, math.pow( clamp_01(nor*4/10), 3 ) )
			--off = clamp_01( off )
			local n = fn_normal( b,a, off )
			--n[1] = mypow( n[1], 3 );
			--n[2] = mypow( n[2], 1 );
			local pt = V2.get_add( b, n )
			add_pt( pt )
			--n = fn_tg( b,a, off )
			--aad-d_pt( V2.get_add( pt, n ) )			
		end
	end
end

function meu:add_cercle( cx,cy, r, nb_arc )
	nb_arc = nb_arc or math.max( 12, math.floor( r*2*math.pi*64 ) )
	--nb = 3
	--self:print( "will do cercle radius "..r.." with "..nb_arc.." arcs" )
	local function def( x,y )
		x = x + math.sin( x * 4 ) * .1
		local d = - math.sin( (x*x + y*y) * 3)
		y = y + d * .4
		return x,y
	end
	--local x,y = cx+r, cy
	--x,y = def( x,y )
	self:add_new()
	local da = math.pi2 / nb_arc
	local def_factor = self.intensity * math.pow( 1.-self.f_inter_y, .5 )
	for i=0,nb_arc-1 do 
		local a = da * i + math.pi/4 * ( math.pow( r, .125 ) * 4 + .0 )
		--local a = fa * i + math.pi/4 * (math.pow( r, .125 ) * 4 + .0)
		local c, s = math.cos(a), math.sin(a)
		local x = cx + c * r
		local y = cy + s * r
		local rbis = r + r * def_factor * self:get_noise_cumul( x,y,0 )
		x = cx + c * rbis
		y = cy + s * rbis
		--x,y = def( x,y )
		--self:print( "coor "..x.." , "..y )
		self:add_pt_xyz( x, y )
	end
	self:set_closed( true )
end

function meu:gene_cercles( cx,cy, r )
	local nb = self.nb_v
	
	for i=1,nb do
		local inter = (i-1)/(nb-1)
		self.f_inter_y = inter
		self.f_inter_y_1 = i/nb
		local rad = r * self.f_inter_y_1
		--local rad = math.pow(  i/nb, 8 )  * r
		local nb_arc = math.floor( interpolate( self.nb_u_min, self.nb_u, math.pow( inter, 1 ) ) )
		--local dx = d * self:get_noise3( rad,0,0 )
		--local dy = d * self:get_noise3( 0,rad,12 )
		--self:add_cercle( cx+dx, cy+dy, rad, nb_arc )
		self:add_cercle( cx,cy, rad, nb_arc )
	end
end

function meu:gene_spiral( cx,cy, radius )
	local nb_loop = self.nb_v

	local function def( x,y )
		x = x + math.sin( x * 4 ) * .1
		local d = - math.sin( (x*x + y*y) * 3)
		y = y + d * .2
		return x,y
	end

	--self:add_new()
	self.b_out = true
	local gold = math.pi2 -- * 1.61803398875

	--loop on turn
	local nb_loop_over = 1. / nb_loop
	self:print( "nb_loop is "..nb_loop )
	for i=self.nb_v_min-1,(nb_loop-1)/3*1.8 do
		local inter_loop	= i / (nb_loop-1)
		local inter			= i * nb_loop_over

		local nb_arc = math.floor( interpolate( self.nb_u_min, self.nb_u+.9999, math.pow( inter_loop, 1. ) ) )
		--self:print( "arc nb is "..nb_arc )

		local x,y
		for j=0,nb_arc-1 do
			local turn = j / nb_arc	-- from 0 to (nb_arc-1) / nb_arc
			local l_inter = inter + turn * nb_loop_over

			local a = (math.pi2 * j) / nb_arc
			--+ math.pi2 * .25 --.1666666
			--local a = da * i + math.pi/4 * (math.pow( r, .125 ) * 4 + .0)	
			local c, s = math.cos(a), math.sin(a)

			local fr = 1
			--local fr = (c + 1) * .5
			--fr = turn

			-- fr = (turn * 1. ) - .0
			--	fr = aaa.math.gain_bias( fr, .5, .8 )
			--	fr = math.sin( math.pi2 * fr ) *.5 + .5
			--	fr = 1

			local oph = l_inter
			local orad = -.08 * 2 * (1-inter_loop) * fr * oph * radius
			oph = -32*(12+0/64) * oph * gold + math.pi2/4
			local ox = orad * math.cos( oph )
			local oy = orad * math.sin( oph )

			--l_inter = math.pow( l_inter, .5 )		
			--local v = wrap_01( turn * 3 )
			--v = (v<.5) and (2*v) or (2*(1-v))
			--v  = .4 + .3 * v
			--v  = .5 + math.sin( v * math.pi2 ) * .0
			--self:print( i..","..j.." "..v )
			--l_inter = aaa.math.gain_bias( l_inter, .5, v )				
			--l_inter = aaa.math.gain_bias( l_inter, .5 + v * .34, .5 - v * .05 )				
			--todo have a begin end
			--local r = radius * l_inter
			local r = radius * l_inter - orad

			--self:print( "r is "..r )
			--local def_factor = self.intensity * math.pow( 1.-l_inter, 1. )

			local a = (math.pi2 * j) / nb_arc
			--+ math.pi2 * .25 --.1666666
			--local a = da * i + math.pi/4 * (math.pow( r, .125 ) * 4 + .0)	
			local c, s = math.cos(a), math.sin(a)
			x = cx + ox + c * r
			y = cy + oy + s * r
			local dr = wrap_01( self:get_noise_feb( x, y, 0, 3.8, 2, 2 ) ) * 6 * math.pow( (1-l_inter), .25 )
			dr = dr * math.pow( clamp_01((2.-r)/2.), .5 )
			dr = wrap_01( dr )
			dr = aaa.math.gain_bias( dr, .5, .5 )  * 2 - 0.
			--dr =  math.max( dr , 0 )
			local sub_inter = 1. - r/2.6 * 1.11072
			sub_inter =  math.pow( sub_inter, 2. )
			local sub
			if false then
				sub = interpolate( 1, 16, sub_inter )
				sub = interpolate( sub, 256, math.pow( turn, 2.)  )
			else
				--local v = math.pow( turn, 1.)
				local v = wrap_01( turn * 5 )
				local pow = .5
				local min = 0
				local max = 128
				sub = interpolate( min, max, math.pow( v, pow)  )
					* interpolate( min, max, math.pow( 1.-v, pow)  ) / 2
				sub = math.max( sub , 1 )
				sub = 1.1
			end
			local rest_01 =  wrap_01( r * sub ) ---  .06  )
			local rest = rest_01 * 2 / sub
			r = r - rest
			dr = dr -- * sub_inter
			r = r  + dr * aaa.math.gain_bias( rest_01, .5, .5 ) * rest
			x = cx + ox + c * r 
			y = cy + oy + s * r
			--y = -
			--local rbis = r_cur + r_cur * def_factor * self:get_noise_cumul( x*.25,y*.25,0 )
			--x = cx + c * rbis
			--y = cy + s * rbis
			--x,y = def( x,y )
--			x,y = self:deform( x,y )
			--self:print( "coor "..x.." , "..y )
			local val = aaa.img.get_component_uv( 0, .55*x/2.42+.5, .55*y/2.42+.5,	5, true, true )
			if true then --val > .5 then
				if self.b_out then
					self:add_new()
					self.b_out = false
				end
				self:add_pt_xyz( y, x )
			else
				self.b_out = true
			end
		end
	end
end

local function linearstep( v_begin, v_end, t )	return clamp_01( (t-v_begin) / (v_end-v_begin) ) end

function meu:gene_spiral_v2( cx,cy, radius )
	local nb_loop = self.nb_v

	--self:add_new()
	self.b_out = true
	local gold = math.pi2 -- * 1.61803398875

	--loop on turn
	local nb_loop_over = 1. / nb_loop
	self:print( "nb_loop is "..nb_loop )
	for i=self.nb_v_min-1,(nb_loop-1) do
		local inter_loop	= i / (nb_loop-1)
		local inter			= i * nb_loop_over

		local nb_arc = math.floor( interpolate( self.nb_u_min, self.nb_u+.9999, math.pow( inter_loop, 1. ) ) )
		--self:print( "arc nb is "..nb_arc )

		local x,y
		for j=0,nb_arc-1 do
			local turn = j / nb_arc	-- from 0 to (nb_arc-1) / nb_arc
			local l_inter = inter + turn * nb_loop_over

			local a = (math.pi2 * j) / nb_arc
			--+ math.pi2 * .25 --.1666666
			--local a = da * i + math.pi/4 * (math.pow( r, .125 ) * 4 + .0)	
			local c, s = math.cos(a), math.sin(a)

			local fr = 1
			--local fr = (c + 1) * .5
			--fr = turn

			-- fr = (turn * 1. ) - .0
			--	fr = aaa.math.gain_bias( fr, .5, .8 )
			--	fr = math.sin( math.pi2 * fr ) *.5 + .5
			--	fr = 1

			local oph = l_inter
			local orad = .0 * (1-inter_loop) * fr * oph * radius
			oph = -32*(6+0/64) * oph * gold + math.pi2/4
			local ox = orad * math.cos( oph )
			local oy = orad * math.sin( oph )

			--l_inter = math.pow( l_inter, .5 )		
			--local v = wrap_01( turn * 3 )
			--v = (v<.5) and (2*v) or (2*(1-v))
			--v  = .4 + .3 * v
			--v  = .5 + math.sin( v * math.pi2 ) * .0
			--self:print( i..","..j.." "..v )
			--l_inter = aaa.math.gain_bias( l_inter, .5, v )				
			--l_inter = aaa.math.gain_bias( l_inter, .5 + v * .34, .5 - v * .05 )				
			--todo have a begin end
			--local r = radius * l_inter
			local r = radius * l_inter - orad

			--self:print( "r is "..r )
			--local def_factor = self.intensity * math.pow( 1.-l_inter, 1. )

			local a = (math.pi2 * j) / nb_arc
			--+ math.pi2 * .25 --.1666666
			--local a = da * i + math.pi/4 * (math.pow( r, .125 ) * 4 + .0)	
			local c, s = math.cos(a), math.sin(a)
			x = cx + ox + c * r
			y = cy + oy + s * r
			--local dr = wrap_01( self:get_noise_feb( x, y, 0, 3.8, 2, 2 ) ) * 6 * math.pow( (1-l_inter), .25 )
			--local dr = self:get_noise3_pos( x,y,self.pass_inter*.0485, 1, 1)

			local dr = self:get_noise3_pos( x,y,.5+self.pass_inter*.01, 2, 4 )  
			local gs = 1/5.5

			dr = dr * self.pass_inter
			
			--local gu,gv = aaa.img.get_gradient_uv(	67, x*gs+.5, y*gs+.5, 1, true, true )
			--dr = dr * clamp_01( 1- math.sqrt( gu*gu + gv*gv) * 100) * 2



			--dr = dr * math.pow(1 - inter_loop, .05 )
			--dr = dr * linearstep( 0, .5, l_inter )
			--dr = dr * linearstep( 1, .5, l_inter )
			dr = dr * math.pow( math.min( linearstep(  0, .5, l_inter ), linearstep( 1, .5, l_inter ) ), 1.5 )
			--self:print( inter_loop.." ->"..linearstep( .8, 1., inter_loop ) )

			-- use an image for birthday plots
			if false then
				local f = aaa.img.get_component_uv(	67, x*gs+.5, y*gs+.5, 1, true, true )
				dr = dr + f * 2
			end
			--dr = dr * math.pow( clamp_01((2.-r)/2.), .5 )

			-- dr = wrap_01( dr )
			-- dr = aaa.math.gain_bias( dr, .5, .5 )  * 2 - 0.
			-- --dr =  math.max( dr , 0 )
			-- local sub_inter = 1. - r/2.6 * 1.11072
			-- sub_inter =  math.pow( sub_inter, 2. )
			-- local sub
			-- if false then
			-- 	sub = interpolate( 1, 16, sub_inter )
			-- 	sub = interpolate( sub, 256, math.pow( turn, 2.)  )
			-- else
			-- 	--local v = math.pow( turn, 1.)
			-- 	local v = wrap_01( turn * 5 )
			-- 	local pow = .5
			-- 	local min = 0
			-- 	local max = 128
			-- 	sub = interpolate( min, max, math.pow( v, pow)  )
			-- 		* interpolate( min, max, math.pow( 1.-v, pow)  ) / 2
			-- 	sub = math.max( sub , 1 )
			-- 	sub = 1.1
			-- end
			-- local rest_01 =  wrap_01( r * sub ) ---  .06  )
			-- local rest = rest_01 * 2 / sub
			-- r = r - rest
			-- dr = dr -- * sub_inter
			-- r = r  + dr * aaa.math.gain_bias( rest_01, .5, .5 ) * rest
			r = r  + dr
			x = cx + ox + c * r 
			y = cy + oy + s * r

			

			--local f = 4*self.pass_inter
			--x = x + gu * f
			--y = y + gv * f
			--self:print( "coor "..x.." , "..y )
			
			if true then
				--local val = aaa.img.get_component_uv( 0, .55*x/2.42+.5, .55*y/2.42+.5,	5, true, true )
				 --val > .5 then
				if self.b_out then
					self:add_new()
					self.b_out = false
				end
				self:add_pt_xyz( y,-x )
			else
				self.b_out = true
			end
		end
	end
end

function meu:gene_heart( cx,cy, rad )
	local nb = self.nb_v	

	local function def( x,y )
		x = x + math.sin( x * 4 ) * .1
		local d = - math.sin( (x*x + y*y) * 3)
		y = y + d * .2
		return x,y
	end

	self:add_new()
	for i=1,nb do
		local inter = (i-1)/(nb-1)
		self.f_inter_y = inter
		self.f_inter_y_1 = i/(nb-1)
		local r = rad * (i-1)/nb
		local nb_arc = math.floor( interpolate( self.nb_u_min, self.nb_u, math.pow( inter, .8 ) ) )
		--local x,y = cx+r, cy
		--x,y = def( x,y )

		local da = math.pi2 / nb_arc
		--todo continuous
		local def_factor = self.intensity * math.pow( 1.-self.f_inter_y, .8 )
		self:print( def_factor )
		local dr = (rad / nb) / nb_arc
		local x,y
		for j=0,nb_arc-1 do 
			local v = j/nb_arc
			local po = .65
			if v < .5 then
				v = math.pow( v*2, po) * .5
			else
				v = 1. - math.pow( 2 - v*2, po ) * .5 
			end
			v = v*2
			-- self:print( "v is "..v )
			--  local vh = (v<1) and v or v-1
			-- self:print( "vh is "..vh )
			-- vh = (vh - .5)*2
			-- self:print( "  is "..vh )
			-- vh = (vh<0) and -math.pow(-vh,1.3) or math.pow(vh,1.3)
			-- self:print( "  is "..vh )
			-- vh = vh*.5 + .5
			-- v = (v<1) and vh or vh+1

			

			--local a = -da * j
			local a = math.pi * v
			--local a = da * i + math.pi/4 * (math.pow( r, .125 ) * 4 + .0)
			local b_valid
			local r_cur = r + dr * j
			local c, s = math.cos(a), math.sin(a)
			--s = s<0 and -s*s or s*s
			s = s * s *s
			b_valid = math.abs(s)*r_cur > .002
			if b_valid then
				c = 13 * c - 5 * math.cos(2*a) - 2 * math.cos(3*a) - math.cos(4*a)
				c = c / 16
				c,s = s,c
				x = cx + c * r_cur
				y = cy + s * r_cur
				local rbis = r_cur + r_cur * def_factor * self:get_noise_cumul( x,y,0 )
				x = cx + c * rbis
				y = cy + s * rbis
				--x,y = def( x,y )
				--self:print( "coor "..x.." , "..y )
				self:add_pt_xyz( x, y )
			end
		end
	end
end
function meu:gene_square( cx,cy, rad )
	local nb = self.nb_v	

	local function def( x,y )
		x = x + math.sin( x * 4 ) * .1
		local d = - math.sin( (x*x + y*y) * 3)
		y = y + d * .2
		return x,y
	end

	self:add_new()
	for i=1,nb do
		local inter = (i-1)/(nb-1)
		self.f_inter_y = inter
		self.f_inter_y_1 = i/(nb-1)
		local r = rad * (i-1)/nb
		local nb_arc = math.floor( interpolate( self.nb_u_min, self.nb_u, math.pow( inter, .8 ) ) )
		--local x,y = cx+r, cy
		--x,y = def( x,y )
		

		local da = math.pi2 / nb_arc
		local dr = (rad / nb) / nb_arc
		--todo continuous
		local def_factor = self.intensity * math.pow( self.f_inter_y, .5 )
		self:print( def_factor )
		local dr = (rad / nb) / nb_arc
		local x,y
		for j=0,nb_arc-1 do
			local r_cur = r + dr * j 
			local v = j/nb_arc
			local xl,yl = math.fn_linear( v*4, 2,	0,1,-1,		1,1,1,	2,-1,1,		3,-1,-1,	4,1,-1, 4,1,-1 )
			x = cx + xl * r_cur
			y = cy + yl * r_cur
			local rbis = r_cur + r_cur * def_factor * self:get_noise_cumul( x,y,self.pass_inter*.004 )
			x = cx + xl * rbis
			y = cy + yl * rbis
				--x,y = def( x,y )
				--self:print( "coor "..x.." , "..y )
			self:add_pt_xyz( x, y )
		end
	end
end
function meu:gene_radial( cx,cy, r )		
	--local d = self.intensity
	local nb = self.nb_v
	
	for iy=1,nb do
		self.f_inter_y = (iy-1)/(nb-1)
		self.f_inter_y_1 = iy/nb
		local rad = r * self.f_inter_y_1
		--local rad = math.pow(  i/nb, 8 )  * r
		local nb_arc = math.floor( interpolate( self.nb_u_min, self.nb_u, math.pow( self.f_inter_y, 2 ) ) )
		--local dx = d * self:get_noise3( rad,0,0 )
		--local dy = d * self:get_noise3( 0,rad,12 )
		--self:add_cercle( cx+dx, cy+dy, rad, nb_arc )
		self:add_cercle( cx, cy, rad, nb_arc )
	end
end

function meu:gene_nf( cx,cy, rad )
	local nb = self.nb_v * self.nb_u	

	local function def( x,y )
		x = x + math.sin( x * 4 ) * .1
		local d = - math.sin( (x*x + y*y) * 3)
		y = y + d * .2
		return x,y
	end

	local s = rad / 100.
	self:add_new()
	self:add_pt_xyz( cx, cy )
	for i=1,nb do
		local rnd = math.random(8)
		--self:print( rnd )
		if		rnd == 1 then cx	= cx+s
		elseif	rnd == 2 then cx,cy = cx+s,	cy+s
		elseif	rnd == 3 then 	 cy	=		cy+s
		elseif	rnd == 4 then cx,cy = cx-s,	cy+s
		elseif	rnd == 5 then cx	= cx-s
		elseif	rnd == 6 then cx,cy = cx-s,	cy-s
		elseif	rnd == 7 then    cy	=		cy-s
		elseif	rnd == 8 then cx,cy = cx+s, cy-s
		end
		self:add_pt_xyz( cx, cy )
	end
end

function meu:gene_turb( cx,cy, r )
	r = r * 2
	local nb_v = self.nb_v
	for iy=self.nb_v_min,self.nb_v do
		self.f_inter_y = (iy-self.nb_v_min)/(self.nb_v-self.nb_v_min)
		local nb_u = math.floor( interpolate( self.nb_u_min, self.nb_u, math.pow( self.f_inter_y, .5 ) ) )
		local v = self.f_inter_y - .5

		local b_first = true
		self:add_new()
		for u=-.5,.5,1/nb_u do
			local nu,nv = self:deform_cumul( u,v )
			local x = cx + r * nu -- * 2 
			local y = cy + r * nv
			self:add_pt_xyz( x, y )
		end
	end
end

function meu:gene_pbn( cx,cy, r )
	local img = 19
	local img_blur = 64

	local sx,sy = aaa.img.get_size(	img_blur )	
	if not (sx and sy) then return end
	
	sx,sy = aaa.img.get_size( img )	
	if not (sx and sy) then return end

	local border = { l=.00, b=.00 }
	border.r = .01	--border.l
	border.t = border.b

	local su = 1 - border.l - border.r
	local sv = 1 - border.t - border.b
	local u_center = (border.l + 1 - border.r) * .5 
	local v_center = (border.b + 1 - border.t) * .5

	local SX,SY
	SX = 8 * r
	SY = SX * sy / sx
	----------------
	local FXY = .9	
	----------------
	SX = SX * FXY
	SY = SY * FXY
	local f_u_to_x = SX / su
	local f_v_to_y = SY / sv

	local DS = SY * self.intensity * 4

	local th_min = self.th_min / 4
	local th_min_sq = th_min * th_min
	local th = self.th_max / 4
	local th_sq = th * th

	local nb_u		= self.nb_u
	local nb_v		= self.nb_v
	local nb		= self.nb_try

	local du = su / nb_u
	local dv = sv / nb_v	

	local b_down = false	-- for vertical zig zag
	
	for ix = 0, nb_u-1 do
		local u_begin = border.l + ix*du

		local iy_begin,iy_end, iy_step
		if b_down then  iy_begin,iy_end, iy_step = 0,nb_v-1, 1
		else			iy_begin,iy_end, iy_step = nb_v-1,0, -1
		end
		b_down = not b_down

		for iy = iy_begin,iy_end, iy_step do
			local v_begin = border.b + iy*dv
			for i=1,nb do
				local u = u_begin + du * math.random()
				local v = v_begin + dv * math.random()
				--u = wrap_01( u )
				--v = wrap_01( v )
				local gr = aaa.img.get_component_uv( img_blur, u, v, 1, true, true )
				--self:print( "g "..g )
				if math.random() > gr then
					--local ds = DS * g
					local gx,gy = aaa.img.get_gradient_uv(	img, u, v, 1, true, true )
					--local gr = 1 - aaa.img.get_component_uv( i_img, u, v, 5, true, true )
					--gx = gx * gr
					--gy = gy * gr
					local d = math.get_length_squared( gx,gy )
					--if inside(gx,-th,th) and inside(gy,-th,th) and (outside(gx,-th_min,th_min) or outside(gy,-th_min,th_min)) then
					if inside( d, th_min_sq, th_sq ) then
						local x = cx + (u - u_center) * f_u_to_x
						local y = cy + (v - v_center) * f_v_to_y

						gr = 1 -- gr
						gr = math.max( .1, gr )
						d = math.sqrt( d )
						gx,gy = aaa.img.get_gradient_uv(	img_blur, u, v, 1, true, true )
						local db = math.get_length( gx, gy )
						d = DS * gr * d / db
						gx,gy = -gy*d, gx*d	--  the flip and - to rotate

						self:add_line_xy(	x+gx, y+gy,
											x-gx, y-gy		)
						--self:print( "nb ", nb )
					end
				end
			end
		end
	end
	--self:get_strips():sort_hilbert()
	--self:get_strips():decimate_gauss_section( 84, 64, 512, 32, 512 )
end

function meu:gene_img( cx,cy, r )
	local img = 20
	
	local sx,sy = aaa.img.get_size( img )	
	if not (sx and sy) then return end

	local border = { l=.0, b=.0 }
	border.r = .0	--border.l
	border.t = .0

	local su = 1 - border.l - border.r
	local sv = 1 - border.t - border.b
--	local u_center = (border.l + 1 - border.r) * .5 
--	local v_center = (border.b + 1 - border.t) * .5

	local SX,SY
	SX = 8 * r
	SY = SX * (sy*sv) / (sx*su)
	----------------
	local FXY = .9	
	----------------
	SX = SX * FXY
	SY = SY * FXY
	local f_u_to_x = SX / su
	local f_v_to_y = SY / sv

	local pow = 1 / self.intensity

	local th_min	= self.th_min
	local th_max	= self.th_max
	local d			= .001

	local nb		= self.nb_u * self.nb_v
	local nb_found	= 0

	while nb_found <= nb do
		--local u_begin = border.l + ix*du
		--local v_begin = border.b + iy*dv
		local u = math.random()
		local ut = border.l + u * su
		local v = math.random()
		local vt = border.b + v * sv

		local gr = aaa.img.get_component_uv( img, ut, vt, 5, true, true )
		--gr = 1-math.pow( 1-gr, pow )
		gr = aaa.math.gain_bias_clamped( gr, self.gain, self.bias )
		if inside( gr, th_min, th_max ) then
			local rnd = math.random()
			if rnd>=gr then
				nb_found = nb_found + 1
				local x = cx + (u - .5) * f_u_to_x
				local y = cy + (v - .5) * f_v_to_y
				self:add_line_xy(	{x+d,y+d,	x-d,y-d		})
			end
		end
	end

	self:get_strips():sort_hilbert()
	--self:get_strips():decimate_random_section( 84, 1, 5, 1, 5 )
end

function meu:gene_points( cx,cy, r )
	--local bdd = aaa.obj.get_by_name( "tooth_boid" )
	--local bdd = aaa.layer.get_bdd_prev()
	local bdd = self.__bdd_prev
	if not bdd then return end

	local nb = aaa.bdd.get_point_nb(bdd)
	--self:print( "nb_points : "..nb )

	local d = .012
	if false then
		local	get_point = aaa.bdd.get_point_by_id
		for i=1,nb do
			local x,y,z = get_point( bdd, i )
			if inside( x, 3, 5 )  and inside( y, -4, 4 ) then
				self:print( i.." -> "..x..", "..y )
				x,y = cx + x*r, cy + y*r
				self:add_cercle( x, y, d, 12 )
			end
		end
	else
		local pts = aaa.bdd.get_points(bdd)
		for i=1,nb do-- in pairs(boid_pts) do
			local pt = pts[i]
			--if pt[1] > 3. then
			--	self:print( i.." -> "..pt[1]..", "..pt[2] )
				local x,y = cx+pt[1]*r, cy+pt[2]*r
				self:add_cercle( x, y, d, 12 )
			--end
		end
	end

	self:get_strips():sort_hilbert()
	--self:get_strips():decimate_random_section( 84, 64, 256, 1, 128 )
end

function meu:get_fn_uv( img )
	local fn_uv = function( u,v ) return u,v end
	if img then
		local sx,sy = aaa.img.get_size( img )
		if sx then
			local d = -self.intensity * 2.
			fn_uv = function( u,v )
				local gu,gv = aaa.img.get_gradient_uv(	img, u*.5+.5, v*.5+.5, 4, true, true )
				u = u + d * gu
				v = v + d * gv
				return u,v
			end
		end
	else
		fn_uv = function( u,v ) return self:deform_cumul( u,v ) end
--		fn_uv = function( u,v ) return self:deform( u,v ) end
	end
	return fn_uv
end

function meu:gene_hilbert( cx,cy, r )
	r = r * 2

	local fn_uv = self:get_fn_uv( 25 ) --64
	local su = 2

	local nb_u = self.nb_u
	local nb_v = self.nb_v
	local du =  1/nb_u
	local dv =  1/nb_v
	for v=-.5+dv*.5,.5,dv do
		local b_first = true
		self:add_new()
		
		--for u=-.5+du*.5,.5,du do
		for u=-su*.5+du*.5,su*.5,du do
			local nu,nv = fn_uv( u,v )
			local x = cx + r * nu
			local y = cy + r * nv
			--self:set_closed( true )
			self:add_pt_xyz( x+du*.25, y+dv*.25 )
			--self:add_pt_xyz( x+du*.25, y-dv*.25 )
			--self:add_pt_xyz( x-du*.25, y-dv*.25 )
			--self:add_pt_xyz( x-du*.25, y+dv*.25 )
		end
	end

	self:get_strips():sort_hilbert()

	--self:get_strips():decimate_random_one_by_one( 84, .0001 )
	--self:get_strips():decimate_random_section( 84, 32, 64*8, 32, 64*8 )
	--pass1 self:get_strips():decimate_gauss_section( 84, 32, 512, 32, 512 )
	--pass2 self:get_strips():decimate_gauss_section( 84, 19, 256, 16, 256 )
	--self:get_strips():decimate_random_section( 84, 1, 62, 1, 64 )
end

function meu:gene_hilbert_a( cx,cy, r )
	r = r * 2
	local img_density
	if self:get_texture_bind_2d( 2 ) == 2 then
		img_density = self:get_texture_bind_2d( 2 ) --+ self.pass_id - 1
	end

	local fn_uv = self:get_fn_uv( nil )

	local nb = self.nb_u
	local f = 1 / (nb-1)
	local b_use_point = true
	self:add_new()
	for i=0,nb*nb-1 do
		local x,y	= aaa.math.convert_hilbert_d_to_xy( i, nb )
		local u,v =  f*x, f*y
		
		if img_density then
			b_use_point = false
			--self:print( u.." - "..v )
			--igor
			local gr = aaa.img.get_component_uv( img_density, u*1, v*1, 4, true, true )	--JC
			--local gr = aaa.img.get_component_uv( img_density, u*1, v*1.55-.3, 5, true, true )	--JC
			--local gr = aaa.img.get_component_uv( img_density, u*.75+.15, v*1.4-.25, 5, true, true )
			--self:print( "gr "..gr )
			if self.b_inv then
				gr = 1 - gr
			end
			gr = aaa.math.gain_bias_clamped( gr, self.gain, self.bias )
			if inside( gr, self.th_min, self.th_max ) then
				local rnd = math.random()
				if rnd<=gr+.001 then
					b_use_point = true
				end
			end
		else
			local rnd = math.random()
			if rnd<=self.bias then
				b_use_point = true
			else
				b_use_point = false
			end
		end

		if b_use_point then
			x,y = fn_uv(  u,v )
			x = cx + r * (x-.5)
			y = cy + r * (y-.5)
			if self.b_swap_xy or inside( y, -2.6, 2.6 ) then --and inside( x, -2.6, 2.6 ) 
				--x,y = MEU.__plot:aaa_to_step( x,y )
				--x,y = MEU.__plot:step_to_aaa( x,y )
				self:add_pt_xyz( x, y )
			end
		end
	end
end

function meu:gene_hilbert_b( cx,cy, r )
	r = r * 2
	local fn_uv = self:get_fn_uv( nil )

	local nb = self.nb_u
	local f = 1 / (nb-1)
	local b_use_point = true
	self:add_new()
	local inter_factor = 1. / (nb*nb-1) 
	for i=0,nb*nb-1 do
		local x,y	= aaa.math.convert_hilbert_d_to_xy( i, nb )
		local u,v =  f*x, f*y
		

		local rnd = math.random()

		--b_use_point = aaa.math.gain_bias( rnd, self.gain, self.bias ) <= i * inter_factor
		local uc,vc = u - .5, v-.5
		b_use_point = aaa.math.gain_bias( rnd, self.gain, self.bias ) <= math.sqrt( uc*uc + vc*vc )

		if b_use_point then
			x,y = fn_uv(  u,v )
			x = cx + r * (x-.5)
			y = cy + r * (y-.5)
			if self.b_swap_xy or inside( y, -2.6, 2.6 ) then --and inside( x, -2.6, 2.6 ) 
				--x,y = MEU.__plot:aaa_to_step( x,y )
				--x,y = MEU.__plot:step_to_aaa( x,y )
				self:add_pt_xyz( x, y )
			end
		end
	end
end


function meu:get_strips()		return self.__strips									end
function meu:get_strips_new()	return self:get_strips():init_with_empty_point_list()	end

function meu:update_ui()
	local ui = self.ui
	--table.print( self.strips.__strips[4], "see", 3 )
	local line_strip_table =  self:get_strips()
	if line_strip_table and line_strip_table.__strips and line_strip_table:get_point_list() then
		ui.bu_info:set_text( #line_strip_table.__strips.." strips: " .. aaa.format.int_to_char_with_space(line_strip_table:get_point_list():get_size()).." pts" )
	end
--	ui.bu_x:set_text( plot.pen_axidraw_x )
--	ui.bu_y:set_text( plot.pen_axidraw_y )
--	ui.bu_queue:set_text( plot.queue:get_nb() )
end


function meu:update()
	self.__bdd_prev = aaa.bdd.get_cur()	
end

function meu:draw()
	self:draw_layers_begin()

		self:draw_layer(1)
		
		local strips = self:get_strips()

		if self.b_gene_always then
			self:gene_sel()
		end
		if self.b_draw_line then
			strips:draw()
		end
		if self.b_draw_point then
			if self.b_draw_colored then	strips:draw_point_colored()
			else						strips:draw_point()	
			end
		end

		if self.b_draw_order then
			gol.color_red()
			strips:draw_order()
		end

		self:draw_layer(2)

	self:draw_layers_begin()
end