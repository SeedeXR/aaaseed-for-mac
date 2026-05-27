function meu:define_meu_infos( )
	return	{ author = "Mâa",
				help = "Generate texture by interpolating between two color curves\n"..
						"very useful to define color ramps or edit color curve with precision",
				tags = { "2d", "core", "coregraphic", "texture", "procedural", "unfinished", "VJ" }
			}
end

function meu:get_preset_nb() 	return 36, 3	end

function meu:pick_tr( bu, id )
	local val = bu:get_value()
	self:print( "pick_tr "..val.." "..id )
end

local function __draw_elt( bu )
	local att = bu.__att
	if att then
		gol.color( att[1], att[2], att[3] )
--		aaa.draw_rect( -.3, -.3, .3, .3 )
		gol.draw_triangles_2d(		0,0,	.5,.5,		-.5, .5 )
		local f = bu:get_y()
		gol.color( att[1]*f, att[2]*f, att[3]*f )
--		aaa.draw_rect( -.3, -.3, .3, .3 )
		gol.draw_triangles_2d(		0,0,	-.5,-.5,	.5, -.5 )
	end
end

function meu:__create_elt( i, bus, rect )
	local bu = SLIDER_MULTI_CURVE.create_elt( self, i, bus, rect )
	--self:print( "__create_elt "..bu.." with "..meu )
	bu.draw = __draw_elt
	--bu:set_text( "Zob" )
	return bu
end

function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
	local par

	local ix = 1
	local iy = 1
	local sx_sel = 0

--	self:set_tab_key( "Main" )

	bu = self:add_slider_two(	{ix + sx_sel,iy,	16-sx_sel,1}, "Flash",		self,"flash_min","flash_max", 	0,.25,	0,1 )
		bu:set_central( true )
		ui.bu_flash = bu
	iy = iy + 1

	local sy_all = 10
	local sy_mon = 1.8
	local ny = (sy_all - 2) * .5
	local iy1 = iy + 1
	local iy2 = iy1 + ny + sy_mon
	self.__curve_pos_y = { iy1, iy2 }
	self.__size = { iy1, iy2 }
	local nx = 16

	bu = self:add_monitor( {ix+sx_sel,iy1+ny, nx,sy_mon}, "Dst" )
		bu:set_texture_fill( true )
		--bu:set_ratio_max( nx/2 + .15 )
		bu:set_dplane( -42 )
		ui.bu_mon = bu

	ui.bu_curve = {}
	ui.bu_edit = {}
	local function add_curves( id )
		local iy = self.__curve_pos_y[id]
		local function get_iy( b ) return iy + (b and ny or -.8 ) end

		local function add_curve( id, sub_id, name )
			local bu = self:add_slider_multi_curve( {ix + sx_sel,iy, nx,ny}, name, 0 )
				bu:set_mini_click_double( false )
				bu.create_elt = self.__create_elt
				return bu
		end

		local SY = 0.8
		ui.bu_curve[id] = add_curve( id, 1, "curve_"..id )
		bu = self:add_button(		{ix,get_iy(id==2),	SY,SY}, "hsv"..id,		self, "b_hsv"..id, 			false )
			bu:set_text_rect_ratio( 2 )
		--bu = self:add_button(		{ix, get_iy(id==1),	sb,sb }, "A"..id,		self, "b_alpha"..id, 		true  )
		--	bu:set_text_rect_ratio( 2 )
		local y = get_iy(id==2)
		bu = self:add_rgbf(			{ix+2,y,			6,SY},	"Edit_"..id, false )
		--bu = self:add_rgb(		{15,iy,				2,nx},	"Edit_"..id, false )
			bu:set_dplane( 42 )
			ui.bu_edit[id] = bu

		local ox, SX = 8, 2*SY
		bu = self:add_trig_method(	{ix+ox, y,			SX,SY},	"inc_"..id,		ui.bu_curve[id], "add_elt_ui",	-1 )
			bu:set_text( "-" )
		bu = self:add_trig_method(	{ix+ox+SX, y,		SX,SY}, "dec_"..id,		ui.bu_curve[id], "add_elt_ui",	1 )
			bu:set_text( "+" )

		bu = self:add_trig_method(	{ix+ox+2.5*SX, y,	SX,SY}, "Push_"..id,	self, "push_curve", id, 3-id )
			bu:set_text( "Push" )
		bu = self:add_trig_method(	{ix+ox+3.5*SX, y,	SX,SY}, "Pop_"..id,		self, "pop_curve",  id, 3-id )
			bu:set_text( "Pop" )
	end

	add_curves( 1 )
	add_curves( 2 )
--	self:set_tab_key( "TEX" )

	ui.bu_dst = self:add_texture_button( {3,15, 4,1}, "button_dst", 2 )
		:set_text_selector( true )
		:set_value_load_save( true )
		:set_preset_use( false )
end

function meu:cpy_curve_to( src_id, dst_id )
	self:box_debug( "cpy_curve_to" )
	local ui = self.ui
	ui.bu_curve[dst_id]:set_values( ui.bu_curve[src_id]:get_values() )
end
function meu:push_curve( id )
	self:box_debug( "push_curve" )
	local ui = self.ui
	self.stack_curve:push( ui.bu_curve[id]:get_values() )
end
function meu:pop_curve( id )
	self:box_debug( "pop_curve" )
	local ui = self.ui
	ui.bu_curve[id]:set_values( self.stack_curve:pop() )
end
function meu:init()
	self.stack_curve = STACK:create( "curve" )
end

function meu:img_init()
	local bind = self.tex_target_bind
	local sx,sy, nb = aaa.img.get_size_channel( bind )

	local asked = self.__img_asked
	if sx~=asked.sx or sy~=asked.sy or nb~=asked.ch_nb then
		local b = aaa.img.set_size_channel( bind, asked.sx,asked.sy, asked.ch_nb  )
		--2022 Oct Mâa try to solve problem but not enough
		aaa.img.set_cpu_keep( bind, true )
		--aaa.show( bind, "bind" )
		self.ui.bu_mon:set_texture_bind_2d( bind )
		self.__cmire_bind = b and bind
	else
		self.__cmire_bind = bind
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

	local nb_x = self.__img_asked.sx

	local function make_col( id )
		local b_hsv = self["b_hsv"..id]
		--local b_alpha = self["b_alpha"..id]
		local cu_id = (id-1) * 4
		local col = {}
		--local pix_id = (2-id) * 2
		local fx = 1 / (nb_x-1)

		local bu_flash = ui.bu_flash
--		ui.bu_flash:print( "contact "..bu_flash:get_value( 1 ) )
		if bu_flash:is_contact() then
			local pix_id = 2-id
			self:print( "contact" )
			local min = bu_flash:get_value( 1 )
			local max = bu_flash:get_value( 2 )
			col = { 1,1,1, 1 }
			for x=0,nb_x-1 do
				local ph = x * fx
				col[4] = inside( ph, min, max ) and 1 or 0
				set_color( bind, x, pix_id, col )
				--set_color( bind, x, pix_id+1, col )
			end
		else
			local y = (2-id)
			for x=0,nb_x-1 do
				local ph = x * fx

				local alpha, inter, e1, e2 = tr[id]:get_curve_value( ph )

				local a1, a2 = e1 and e1.__att, e2 and e2.__att

				if not a1 then
					a1,inter = a2,1
				elseif not a2 then
					a2,inter = a1,0
				end

				if a1 and a2 then
					if b_hsv then
						local h1, s1, v1 = aaa.color.rgb_to_hsv( a1[1], a1[2], a1[3] )
						local h2, s2, v2 = aaa.color.rgb_to_hsv( a2[1], a2[2], a2[3] )
						h1 = interpolate( h1, h2, inter )
						s1 = interpolate( s1, s2, inter )
						v1 = interpolate( v1, v2, inter )
						col[1], col[2], col[3] = aaa.color.hsv_to_rgb( h1, s1, v1 )
					else
						col[1] = interpolate( a1[1], a2[1], inter )
						col[2] = interpolate( a1[2], a2[2], inter )
						col[3] = interpolate( a1[3], a2[3], inter )
					end
				end
				col[4] = alpha

				set_color( bind, x,y, col )
			end
		end
	end

	make_col( 1 )
	make_col( 2 )

	--make sure monitor display well
	gol.bind_texture( bind )
	gol.set_wrap_2d_edge()

	aaa.img.move_to_gpu( bind )
end

function meu:update_curve()
	local ui = self.ui
	local last = self.__last or {}
	for id=1,2 do
		local curve = ui.bu_curve[id]
		local edit = ui.bu_edit[id]
		local bu = curve:get_bu_last()
		if bu then
			if last[id] ~= bu then
				local att = bu.__att
				if att then
					edit:set_rgba( att[1], att[2], att[3] )
				end
				last[id] = bu
			else
				local r,g,b = edit:get_rgba()
				bu.__att = { r,g,b }
			end
		end
	end
	self.__last = last
end
function meu:update_tex_target_bind()
	local bind = self.ui.bu_dst:get_bind_2d()
	if bind ~= "none" then
		self.tex_target_bind = bind
		self:print( "bind asked "..bind )
	end
	--aaa.debug.show( self.tex_target_bind, "bind" )
end

function meu:update_ui()
	self:update_tex_target_bind()
	self.ui.bu_mon:set_texture_bind_2d( self.tex_target_bind )
end

function meu:update()
	self:update_tex_target_bind()

	self:update_curve()

	self:img_compute()
end

