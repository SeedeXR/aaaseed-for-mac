function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bdd = ref.bdd
	local bu
	local par

	local SY = 1
	local DY = .5
	local ix,iy = 1,1

	self:add_camera()

	local iys = iy

	self:set_tab_key_def()
	iy = iy - SY
	local S = .6
	--self:add_button(		{9,iy,		SY,SY}, "Tex Set", self, "b_tex_set", true )
	-- iy = iy + SY + DY
	-- self:add_size_uvf_video( {9,iy},		nil, true )
	-- iy = iy + 3
	-- self:add_bu_texture_target_unit( {9,iy, 8,7} ,	"Tex", 1 )
	-- iy = iy + 7

	local ix = 1
	iy=1
--	self:add_button(		{ix,iy,			SY,SY },	"Pixel_Exact",	self, "b_pixel_exact",	false	)
--	self:add_button(		{ix+SY, iy+SY,	SY,SY },	"Clamp",		self, "b_clamp",		false		)
	iy = iy + 2*SY

	iy = iys
	iy = iy + SY
	self:add_trig(			{ix,	iy,		4,SY},	"Grid Reset",	bdd, "grid_reset_trig" )
		:set_color_back( "reset")
	self:add_button(		{ix+4,	iy,		4,SY},	"Grid Draw",	bdd, "grid_draw", false )
	-- sx = 8/3
	iy = iy + SY*2 + DY
	local sx = 1
	if ref.deform then
		self:add_button(	{ix, 	iy,		SY,SY},	"Deform",			ref.deform			)
		self:add_button(	{ix+sx,	iy+SY,	SY,SY},	"UI draw",		ref.ui_draw			)
		self:add_button(	{ix+sx,	iy+SY*2,SY,SY},	"UI intercept (Key Q)", ref.ui_intercept	)
			:set_text_rect_ratio( 7 )
	end
	self:add_button(		{ix+4,	iy,		SY,SY},	"TopLine",		ref.rendering,		"top_line" )
	iy = iy + SY * 3 + DY
	self:add_rendering()
	iy = iy + SY * 2 + DY


	self.ui.bu_rgb = self:add_rgbf(	{ix,iy,	8,SY*2}	)

	ix,iy = 9,3
	self:add_slider(		{ix+2,	iy,		4,SY},	"Nb",	self, "nb_u",		32, 	0,1024	)
		:set_value_type_integer( true )
	iy = iy + SY

	self.elt = {}
	local nb = 9
	for i=1,nb do
		local elt = {}
		self.elt[i] = elt
		self:add_button(	{ix,	iy,		SY,SY},		i.."_active",	elt, "b_active",	true	)
			:set_text_visible( false )
		self:add_slider(	{ix+SY,	iy,		3-SY,SY},	i.."_u_start",	elt, "u_start",		i/nb, 	0,1	)
			:set_color_back( "u" )
		self:add_slider(	{ix+3,	iy,		2.5,SY},	i.."_nb_factor",	elt, "u_nb_factor",	1, 	0,2	)
		self:add_slider(	{ix+5.5,iy,		2.5,SY},	i.."_size_factor",	elt, "size_factor",	1, 	0,2	)
		iy = iy + SY
	end
	iy = iy + DY
end


function meu:init()
	local ref = self.ref
	local layer = self:get_layer(1)
	ref.b_active = param.get_ref( layer, "active" )
	local bdd = self:get_layer_bdd(1)
	--self:box_debug( "bdd is "..bdd )
	ref.bdd = bdd
	-- No error in case the layer use a regular bdd_grid (there is no center param in bdd_adjustable )
	ref.deform			= param.get_ref_no_error( bdd, "deform"				)
	ref.ui_draw			= param.get_ref_no_error( bdd, "ui_draw"			)
	ref.ui_intercept	= param.get_ref_no_error( bdd, "ui_intercept"		)

	if ref.ui_intercept then	--protect from layer have a grid instead of a grid_ajustable
		param.set_save( bdd, "ui_index_u", false )
		param.set_save( bdd, "ui_index_v", false )
	end
end

function meu:update()
	--aaa.print_fn()
	local ref = self.ref

	--self.ui.bu_type:set_value( 3 )
	if ref.ui_intercept then
		local b = param.get_bool( ref.ui_intercept )
		if self.__ui_intercept_last ~= b then
			self.__ui_intercept_last = b
			if b then	--cheat once activated because the ui one to interect alt q for example
				aaa.bdd.set_ui( ref.bdd )
			end
		end
	end

end

function meu:draw_dot( v, u_nb, size )
	size = size / u_nb
	local bdd = self.ref.bdd
	local du = 1. / (u_nb-1)
	local x,y,z
	for u=0,1.000001,du do
		x,y,z = aaa.bdd.get_point_from_uv( bdd, u,v )
		--self:print( u.." "..x..","..y )
		aaa.draw_disk_axe_z( x,y,0, size,24 )
	end
end
function meu:draw_dots()
	local ref = self.ref
	local bdd = ref.bdd
	local size = 12
	local nb = self.nb_u

	local elts = self.elt

	local v = 0.5
	self.b_sym = true
	for i=1,#elts do
		local elt = elts[i]
		nb = math.floor( nb * elt.u_nb_factor )
		size = size * elt.size_factor 
		v = v + elt.u_start
		if elt.b_active then
			if self.b_sym and i>1 then
				self:draw_dot( v,		nb, size )
				self:draw_dot( 1.-v,	nb, size )
			else
				self:draw_dot( v,		nb, size )
			end
		end
		--size  = size*.7
	end
end

function meu:update()
end

function meu:update()
	local ref = self.ref
	local bdd = ref.bdd
	self:draw_layers_begin()
		self:draw_layer(	1 )
		self:draw_dots()
--		self:draw_layer(	2 )
	self:draw_layers_end()
--	MEU.draw(self)
end
