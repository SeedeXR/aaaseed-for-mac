function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
	local par

	ref.curve = {}
	local function add_curve( id, irect, name )
		local layer_ref = self:get_layer( 13+id )
		if layer_ref then
			local rect = self:make_rect_from_irect( irect )
			bu = bus_cur:add_bu( BU:create( name, rect ) )
			--change_bu( bu )
			ref.curve[id] = aaa.obj.get_down_by_class( layer_ref, "bdd_curve_edit" )
			bu:attach_obj( layer_ref, -.5,-.5, 1,1, ref.curve[id] )
			--draw_central don't exist anymore
			--check these
			--bu.draw_central = function(self) end
			bu:set_ui_top_size( true )
		end
	end

	local ix = 1
	local iy = 1.4
	ui.bu_dst = self:add_texture_selector( {ix,iy, 8,1.6}, "dst" )

	local ny = 4
	local iy1 = 4
	local iy2 = iy1 + ny + 2
	local nx = 16
	bu = self:add_monitor( {ix,iy1+ny, nx,ny*.5} )
		bu:set_texture_bind_2d( self:get_bind_by_name( "F1" ) )
		bu:set_ratio_max( nx/2 + .15 )
		ui.bu_mon = bu
	add_curve( 1, {ix,iy1, nx,ny}, "transfert 1" )
	add_curve( 2, {ix,iy2, nx,ny}, "transfert 2" )
	local s = 0.8
	iy1 = iy1 - s
	iy2 = iy2 + ny
	bu = self:add_button(	{	ix, 	iy1,	s,s }, "Alpha1",	self, "b_alpha1", 	true		)
		bu:set_text_rect_ratio( 3 )
	bu = self:add_button(	{	ix+2.5,	iy1,	s,s }, "hsv1",		self, "b_hsv1", 	false		)
		bu:set_text_rect_ratio( 2 )
	bu = self:add_button(	{	ix,		iy2,	s,s }, "Alpha2",	self, "b_alpha2",	true		)
		bu:set_text_rect_ratio( 3 )
	bu = self:add_button(	{	ix+2.5,	iy2,	s,s }, "hsv2",		self, "b_hsv2", 	false		)
		bu:set_text_rect_ratio( 2 )
end

function meu:img_init()
	local bind = self.ui.bu_dst:get_bind_2d()
	--aaa.debug.show( bind, "bind" )
	local sx,sy, nb = aaa.img.get_size_channel( bind )

	local asked = self.__img_asked
	if sx~=asked.sx or sy~=asked.sy or nb~=asked.ch_nb then
		local b = aaa.img.set_size_channel( bind, asked.sx,asked.sy, asked.ch_nb  )
		--aaa.show( bind, "bind" )
		self.ui.bu_mon:set_texture_bind_2d( bind )
		self.__cmire_bind = b and bind
	end

end
function meu:img_compute()
	self.__img_asked = { sx = 512, sy = 2, ch_nb = 4 }
	self:img_init()

	local bind = self.__cmire_bind
	if not bind then return end

	local nb_x = self.__img_asked.sx -1
	
	local col = {}
	local bdd1 = self.ref.curve[1]
	local bdd2 = self.ref.curve[2]
	local alpha
	if bdd1 and bdd2 then
		for x=0,nb_x do
			local ph = (x / nb_x);

			col[1] = aaa.bdd.get_curve_val( bdd1, 1, ph )
			col[2] = aaa.bdd.get_curve_val( bdd1, 2, ph )
			col[3] = aaa.bdd.get_curve_val( bdd1, 3, ph )
			if self.b_hsv1 then col[1], col[2], col[3] = aaa.color.hsv_to_rgb( col[1], col[2], col[3] ) end
			alpha = self.b_alpha1 and clamp_01( aaa.bdd.get_curve_val( bdd1, 4, ph ) ) or 1
			col[4] = alpha
			if self.b_hsv1 then col[1], col[2], col[3] = aaa.color.hsv_to_rgb( col[1], col[2], col[3] ) end

			aaa.img.set_color_xy( bind, x, 2, col )
			aaa.img.set_color_xy( bind, x, 3, col )

			col[1] = aaa.bdd.get_curve_val( bdd2, 1, ph )
			col[2] = aaa.bdd.get_curve_val( bdd2, 2, ph )
			col[3] = aaa.bdd.get_curve_val( bdd2, 3, ph )
			alpha = self.b_alpha2 and clamp_01( aaa.bdd.get_curve_val( bdd2, 4, ph ) ) or 1
			col[4] = alpha
			if self.b_hsv2 then col[1], col[2], col[3] = aaa.color.hsv_to_rgb( col[1], col[2], col[3] ) end

			aaa.img.set_color_xy( bind, x, 0, col )
			aaa.img.set_color_xy( bind, x, 1, col )
		end
		aaa.img.move_to_gpu( bind )
	end
end

function meu:init()
end

function meu:update()
	self:img_compute()
end

