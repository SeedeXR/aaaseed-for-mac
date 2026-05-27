function meu:get_preset_nb() 	return 32, 2	end

function meu:pick_tr( bu, id )
	local val = bu:get_value()
	self:print( "pick_tr "..val.." "..id )
end
function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
	local par

	local ix = 1
	local iy = 1.4
	ui.bu_dst = self:add_texture_selector( {ix,iy, 8,1.6}, "dst" )

	local ny = 3

	local iy1 = 4
	local iy2 = iy1 + ny + 2
	self.__curve_pos_y = { iy1, iy2 }
	self.__size = { iy1, iy2 }
	local nx = 13.5
	local sx_sel = 1.5
	bu = self:add_monitor( { ix+sx_sel, iy1+ny, nx} )
		bu:set_texture_bind_2d( self:get_bind_by_name( "F1" ) )
		bu:set_ratio_max( nx/2 + .15 )
		ui.bu_mon = bu

	local sb = 0.8
	ui.bu_curve = {}
	local function add_curves( id )
		local iy = self.__curve_pos_y[id]
		local function get_iy( b ) return iy + (b and ny or -.8 ) end

		local function add_curve( id, sub_id, name )
			local bu = self:add_slider_multi_curve( {ix + sx_sel,iy, nx,ny} ,id..name,	2 )
				local x,y = bu:get_xy()
				local sx,sy = bu:get_sxy()
				local win_state_pushed = bu:get_window_state()
				bu:__set_window_state( "mini" )
				bu:set_xy(  x + sx/6.*(sub_id-2), y + (id==1 and .6 or -.6) * sy )
				bu:__set_window_state( win_state_pushed )
				return bu
		end

		local is = (id-1)*4
		ui.bu_curve[is+1] = add_curve( id, 1, "R/H" )
		ui.bu_curve[is+2] = add_curve( id, 2, "G/S" )
		ui.bu_curve[is+3] = add_curve( id, 3, "B/V" )
		ui.bu_curve[is+4] = add_curve( id, 4, "A"	)
		bu = self:add_selector(	{ix,iy,	sx_sel,ny},	"curve"..id	)
			--bu:set_text_draw( false )
			bu:set_nb( 2, 4 )
			bu:set_value_load_save( false )
			bu:set_item_text( 1., "1R", "1G", "1B", "1A", "2R", "2G", "2B", "2A" )
			bu:set_method_on_value_change( self, "pick_tr", bu, id )

		bu = self:add_button(	{	ix+2.5,	get_iy(id==2),	sb,sb }, "hsv"..id,		self, "b_hsv"..id, 	false		)
			bu:set_text_rect_ratio( 2 )
		bu = self:add_button(	{	ix, 	get_iy(id==1),	sb,sb }, "A"..id,		self, "b_alpha"..id, 	true		)
			bu:set_text_rect_ratio( 2 )
	end

	add_curves( 1 )
	add_curves( 2 )

	bu = self:add_slider_two(	{ix + sx_sel,13,	16-sx_sel,1}, "Flash", self,	"flash_min", "flash_max", 0, .25, 0, 1 )
		bu:set_central( true )
		ui.bu_flash = bu
--	bu = self:add_slider_two(	{ix + sx_sel,14,	16-sx_sel,1}, "test", self )

end

function meu:img_init()
	local bind = self.ui.bu_dst:get_bind_2d()
	aaa.debug.show( bind, "bind" )
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

	local ui = self.ui
	local tr = ui.bu_curve
	local set_color = aaa.img.set_color_xy

	local nb_x = self.__img_asked.sx -1

	local function make_col( id )
		local b_hsv = self["b_hsv"..id]
		local b_alpha = self["b_alpha"..id]
		local cu_id = (id-1) * 4
		local col = {}
		--local pix_id = (2-id) * 2
		local pix_id = 2-id
		local fx = 1 / nb_x

--		ui.bu_flash:print( "contact "..ui.bu_flash:get_value( 1 ) )
		if ui.bu_flash:is_contact() then
			self:print( "contact" )
			local min = ui.bu_flash:get_value( 1 )
			local max = ui.bu_flash:get_value( 2 )
			col = { 1,1,1,1 }
			for x=0,nb_x do
				local ph = x * fx
				col[4] = inside( ph, min, max ) and 1 or 0
				set_color( bind, x, pix_id, col )
				set_color( bind, x, pix_id+1, col )
			end
		else
			for x=0,nb_x do
				local ph = x * fx
				col[1] = tr[cu_id+1]:get_curve_value( ph )
				col[2] = tr[cu_id+2]:get_curve_value( ph )
				col[3] = tr[cu_id+3]:get_curve_value( ph )
				if b_hsv then col[1], col[2], col[3] = aaa.color.hsv_to_rgb( col[1]*2., col[2], col[3] ) end
				col[4] = b_alpha and tr[cu_id+4]:get_curve_value( ph ) or 1

				set_color( bind, x, pix_id, col )
				--set_color( bind, x, pix_id+1, col )
			end
		end
	end

	make_col( 1 )
	make_col( 2 )

	aaa.img.move_to_gpu( bind )
end

function meu:init()
end

function meu:update()
	local ui = self.ui
	self:img_compute()
end

