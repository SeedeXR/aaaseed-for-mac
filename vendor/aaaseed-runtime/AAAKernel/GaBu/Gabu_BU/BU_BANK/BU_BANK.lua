
if CLASS.DECLARE( "BU_BANK", BU_WINDOW, {} ) then
	BU_BANK:set_class_status_doc(	CLASS.STATUS.GABU,
									"Window to navigate image/texture BANK" )
end


BU_BANK.doc.create =	"( name, rect ) Constructor, create a Bu_BANK window, calls define_ui."
function BU_BANK:create( name, rect )
	local self = BU_BANK:create_instance( name, rect )

	local sx,sy = self:get_sxy()
	--self:box_debug( "A\n"..sx.."\n"..sy )

	local bus_down = self:define_ui()
	--bus_down:set_transfo( 8 )
	--x,sy = self:get_sxy()
	--self:box_debug( "B\n"..sx.."\n"..sy )

	bus_down:set_transfo( sx,sy, sx/2,sy/2 )
	self:transform_in_window( bus_down )
	

	self:__set_window_state( "mini" )
	self:set_text_factor( .025 )

	--	local bu = bus_cur:add_window( "BANK", bus,	rect )
	self:set_back_alpha( .8 )

	--sx,sy = self:get_sxy()
	--self:box_debug( "C\n"..sx.."\n"..sy )

	return self
end

function BU_BANK:__change( b_update_bu_bank )
	local mons = self.monitors
	local sel_bank = self.sel_bank
	local bank_super = self.sel_bank_super:get_value()
	local val = sel_bank:get_value() + bank_super * 64
	local nb = self.mon_nb_u * self.mon_nb_v
	local off = val * nb - 1

	for i=1,nb do
		--bu:print( mons[i]..":set_texture_bind_2d( "..( off + i ).." )" )
		mons[i]:set_texture_bind_2d( off + i )
	end

	if b_update_bu_bank then
		local bind_begin = bank_super * 64 
		for i = 1,16*4,4 do sel_bank:set_item_text( i, tostring(bind_begin+i-1), "", "", "" ) end
		if bank_super == 0 then
			TEXS:set_bank_names( sel_bank, true )
		end
	end
end

function BU_BANK:get_monitor_cur_id()
	return self.monitors_to_id[ BU:get_bu_cur() ]
end

BU_BANK.doc.define_ui =	"() Defines ui in window."
function BU_BANK:define_ui()
	local bus = BUS:create( "Bank_in" )

	local bu

	local nb_u,nb_v = 8,4
	self.mon_nb_u,self.mon_nb_v = nb_u,nb_v
	local nb = nb_u * nb_v
	self.mon_nb = nb

	local dx = .01

	local sx,sy = self:get_sxy()
	--self:box_debug( sx.."\n"..sy )
	local ssy = .25
	local ssx = sx / 8
	local mon_d = 1 / 16
	local su = (sx-mon_d) / nb_u - mon_d
	local sv = (sy-ssy-mon_d) / nb_v - mon_d

	bus:init_begin()
		local SX = (4-mon_d)*3/4
		bu = bus:add_selector(	"Bank",			{SX*.5, ssy*.5, SX, ssy} )
			bu:set_nb_min_0( 16, 4 )
			--for i = 1,16*4,4 do bu:set_item_text( i, tostring(i-1) ) end
			--TEXS:set_bank_names( bu )
			bu:set_value( 0 )
			bu:set_method_on_value_change( self, "__change", false )
			self.sel_bank = bu
		
		SX = 4-SX-mon_d
		bu = bus:add_selector(	"Bank Offset",	{4-SX*.5, ssy*.5, SX, ssy} )
			bu:set_nb_min_0( 8, 4 )
			for i=1,32 do
				bu:set_item_text( i, tostring((i-1) * 64) )
			end
			--bu:set_item_text_from_nb_minus_1()
			bu:set_value( 0 )
			bu:set_method_on_value_change( self, "__change", true )
			self.sel_bank_super = bu
			
		local mons = {}
		local monitors_to_id = {}

		--local S = 4

		local index = 0
		local d = mon_d
		for iv = 1,nb_v do
			for iu = 1,nb_u do
				local rect = {	d*.5+(iu-.5)*(su+d),	ssy+d*.5+(nb_v-iv+.5)*(sv+d), su,sv}
				bu = bus:add_monitor( tostring(index), rect, math.fmod(index, nb), true )
					bu:set_ui_active( true )
					bu:set_text_info_size_fxy( .8 )
					bu:set_info_draw( true )
					bu:set_render_mode_alpha( 1 )
					bu.__window_bank = self
					--bu:
				index = index + 1
				mons[index] = bu
				monitors_to_id[bu] = index
			end
		end

	bus:init_end()
	self.monitors = mons
	self.monitors_to_id = monitors_to_id

	self:__change( true )

	return bus
end

-- --done by BU now that we have do_copy/paste
function BU_BANK:do_key(key)
	self:print_do_key( "BU_BANK", key )

 	local b_key_used = false
	-- if not aaa.keyboard.is_alt() and aaa.keyboard.is_ctrl() then
 	-- 	if key == 127 then
	-- 		b_key_used = self:do_action( "delete" )
	-- 	end
	-- end

 	return b_key_used or oo.getsuper(BU_BANK).do_key( self, key )
end

function BU_BANK:__swap_image( ia, inc )
	local ib = ia + inc
	aaa.print_fn()
	local mons = self.monitors
	local mon_a = mons[ia]
	local mon_b = mons[ib]
	local bind_a = mon_a:get_texture_bind_2d()
	local bind_b = mon_b:get_texture_bind_2d()
	aaa.img.swap( bind_a, bind_b )
	mon_a:__set_bind_2d_low( bind_a )
	mon_b:__set_bind_2d_low( bind_b )
end

function BU_BANK:do_key_special( key )
	self:print_do_key_special( "BU_BANK", key )

 	local b_key_used = false
	if aaa.keyboard.is_ctrl() and not aaa.keyboard.is_alt() then
		if inside( key, 272,275 ) then
			b_key_used = true
			local inc
			local id_mon = self:get_monitor_cur_id()
			if id_mon then
				if 		key == 274 then	-- ARROW_LEFT
					if id_mon > 1 then
						inc = -1
					end	
				elseif	key == 275 then	-- ARROW_RIGHT
					if id_mon < self.mon_nb then
						inc = 1
					end
				else
					local nb_u = self.mon_nb_u
					if		key == 273 then	-- ARROW_DOWN
						if id_mon <= self.mon_nb - nb_u then
							inc = nb_u
						end
					elseif	key == 272 then	-- ARROW_UP
						if id_mon > self.mon_nb_u then
							inc = -nb_u
						end
					end
				end
		--			if key == 272 or key == 273 then		-- UP DOWN
	--				b_key_used = self:do_action( key == 272 and "move_up" or "move_down" )
	--			elseif key == 276 then
	--				b_key_used = self:do_action( "insert" )
			end
			if inc then --todola test upper limit
				self:__swap_image( id_mon, inc )
				self.monitors[id_mon + inc]:set_bu_cur()				
			end
		end
	end

 	return b_key_used or oo.getsuper(BU_BANK).do_key_special( self, key )
end