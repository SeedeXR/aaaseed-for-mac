
if CLASS.DECLARE( "BU_MEU", BU, {
		__b_uif_outside		= false,
	} ) then
end

BU_MEU:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
								"a specialized BU where the MEU can expose his UI.\n"..
								"These are seen by a user as the Uix Window, internally we talk about ui_slot" )

local OX = 0	--	-6.9
local DX = 0	--	1.7
local OY = -1.5
local DY = -1.2
local SX = 1.5
function BU_MEU:get_meu_attached()	return self.__meu_attached	end
function BU_MEU:get_mu_attached()	return self.__mu_attached	end

--there is still a little ambiguity on is BU_MEU related to MEU or MU (ref MEU make it more complex)
BU_MEU.doc.detach = "() detach the currently attached MEU/MU from this BU_MEU. The MEU stays in MEU_CTX, only the BU_MEU link is cut."
function BU_MEU:detach()
	local meu = self.__meu_attached
	if meu then
		meu.__slot_id = nil
		--self:remove_bus_down( meu.__bus_fix )
		self.__mu_attached = nil
		--meu.__b_gp_ui_displayed = false
		local bus = meu:get_bus()
		bus.__x_offset_exp = 0
		bus.__y_offset_exp = 0
		bus.__x_offset_target = 0
		bus.__y_offset_target = 0
		bus:set_active( false )
		self:remove_bus_down( bus )
	end
end

BU_MEU.doc.attach_meu_mu = "( meu, mu, slot_id ) called by GP to link a bu_meu and a mu/meu. When meu is nil, equivalent to :detach()."
function BU_MEU:attach_meu_mu( meu, mu, slot_id )

	--todo what happen with encapsulation/hierarchy
	if type(meu)=="string" then
		meu = app:get_meu_by_name_no_error( meu )
	end

	if meu then
		if mu then	--it has happen in some files (Nvember 2025)
			if type(mu)=="string" then
				mu = app:get_mu_by_name_no_error( mu )
			end
		else	--it has happen in some files (Nvember 2025)
			--self:box_debug( "in attach_meu_mu() no mu: so getting it from meu." )
			mu = meu:get_mu()
			if not mu then
				self:box_error( "in attach_meu_mu() we have no mu" )
			end
		end

		--todo was there for a try but BU can only have one bus down for now (2025 Jan)
		--local bus_fix
		--meu.__b_gp_ui_displayed = true
		local bus = meu:get_bus()
		--bus_fix = meu.__bus_fix
		self:add_bus_down( bus )
		--self:add_bus_down( bus_fix )
		self.__meu_attached = meu	--used for Shift Copy/Paste
		self.__mu_attached = mu --used to draw background and edit from BU_MEU the MU slider
		meu.__slot_id = slot_id

		if bus then
			bus:set_active( true )
			--if bus_fix then
			--	self:print( "bus_fix" )
			--	bus_fix:set_active( true )
			--end
			bus.__x_offset_target = 0
			bus.__y_offset_target = 0
		end

		self:active_link()
	else
		self:detach()
	end
end

BU_MEU.doc.save_pos_value_more = "( file ) used to save mu/meu attached"
function BU_MEU:save_pos_value_more( file )
	oo.getsuper(BU_MEU).save_pos_value_more( self, file )
	local meu = self.__meu_attached
	if meu then
		--aaa.debug.print_traceback()
		--self:box_good( self.." saving in save_pos_value_more")
		file:write( "BUS.do_bu_method( \"", self:get_name(), "\", \"attach_meu_mu\"" )
		file:write( ", \"", meu:get_name(), "\"" )
		local mu = self:get_mu_attached()
		if mu then	file:write( ", \"", mu:get_name(), "\"" )
		else		file:write( ", nil" )
		end
		file:write( ", ", meu:get_ui_slot(), " )\n" )
	end
end

function BU_MEU:get_uif_bui()
	local mu = self:get_mu_attached()
	if mu then
		return mu:get_slider()
	end
end

function BU_MEU:get_uif_zones()
	local zones = {}	--oo.getsuper(BU_MEU).get_uif_zones(self)
	local mu = self:get_mu_attached()
	local dist = 2
	if mu then 
		zones = mu:add_uif_zones_mu( zones, true )
		dist = 4.2
	else
		dist = 3
	end

	-- y=1.1 lifts Name/Values/Dump/Doc one row up to make room for the row of
	-- Destroy/Rename/Instantiate that now sits below them
	self:add_uif_zones_base( zones, 4.2, nil, 1.1 )
	return zones
end

function BU_MEU:do_uif_command( uif )
	--aaa.print_fn()

	local meu = self:get_meu_attached()
	--self:print( "try uif with meu attached "..meu )
	
	local b_used = false 
	if meu then
		b_used = meu:do_uif_command( uif )
		if not b_used then
			local mu = self:get_mu_attached()
			self:print( "try uif with mu attached "..mu )
			b_used = mu:do_uif_command( uif )
		end
	end

	return GABU_OBJ.do_uif_command_with_super( self, b_used, uif, BU_MEU )
end

function BU_MEU:get_uif_name()
	local meu = self:get_meu_attached()
	if meu then
		return meu:get_name()
	end
	return oo.getsuper(BU_MEU).get_uif_name(self)
end


--[[
--BU_MEU.draw_back = nil
function BU_MEU:draw_back()
	local meu = self:get_meu_attached()
	if meu then
		local b_rendered = meu:is_rendered_in_this_frame()
		if true then
			if not b_rendered then
				self:draw_back_inactive()
				return
			end
		else
			local used =  b_rendered and 1 or 0
			used = interpolate( self.__meu_draw_inactive, used, aaa.time.dt_real )
			self.__meu_draw_inactive = used
			if used < .99 then
				self:draw_back_inactive( used )
				return
			end
		end			
	end
	BU.draw_back( self )
end
--]]

function BU_MEU:draw_fore()
	local meu = self:get_meu_attached()
	if meu then
		local b_draw
		if meu.__b_define_ui_needed then
			b_draw = true
		end
		if meu.__b_show_error then
			--self:print( "b_show_error" )
			b_draw = true
			meu.__b_show_error = false
		end 
		if b_draw then
			BU_PB:draw()
		end
		--for the fun 
		gol.push_matrix()
			gol.translate( .248, .4755 )
			local s = .8
			gol.scale( s/4, s/20 )
			meu:set_color_then_draw_icon()
		gol.pop_matrix()
	end

	oo.getsuper(BU_MEU).draw_fore( self )
end
function BU_MEU:gol_color_back()
	--aaa.print_fn()
	local meu = self:get_meu_attached()
	if meu then
		local b_rendered = meu:is_rendered_in_this_frame()
		-- if meu:is_name("fbo_f1") then
		-- 	aaa.print_fn()
		-- end
		if not b_rendered then
			local alpha = .8 + math.sin(aaa.time.t_real*math.pi2) * .1
			self:gol_rgba_back( .5,0,0, alpha )
			return 
		end		
	end
	return BU.gol_color_back( self )
end

function BU_MEU:do_key(key)
	self:print_do_key( "BU_MEU", key )

 	local b_key_used = false

 	return b_key_used or oo.getsuper(BU_MEU).do_key( self, key )
end

function BU_MEU:do_key_special(key)
	self:print_do_key_special( "BU_MEU", key )

 	local b_key_used = false

 	return b_key_used or oo.getsuper(BU_MEU).do_key_special( self, key )
end


function BU_MEU:create( name, rect )
	local self = BU_MEU:create_instance( name, rect )

	self:set_text_xy( .006,.962 )
	self:set_text_factor( .04 )
	self:set_ui_top_size( true )
	self:set_pos_load_save( true )
	self:set_border( true )
	self:set_mobile( false )
	self:set_border_line( true )
	self:set_color_back( { .0,.1,.2, .8 } )
	self:set_window()

	self:set_method_on_click_double( self, "do_click_double" )

	self.__meu_draw_inactive = 0

	return self
end

function BU_MEU:do_click_double( x,y )
	local slider = self:get_uif_bui() 
	if slider and not aaa.keyboard.is_alt() then
		slider:edit_dialog_param()
	else
		oo.getsuper(BU_MEU).do_click_double( self, x,y )
	end
end

function BU_MEU:do_click_down( x,y )
	--aaa.print_fn( "do_click_down" )
	local meu = self:get_meu_attached()
	if meu then
		meu:set_focus( false )
	end
	self:active_link( x,y )
	oo.getsuper(BU_MEU).do_click_down( self, x,y )
end

function BU_MEU:__draw_all()
 	if BU_MEU.__b_draw then
 		oo.getsuper(BU_MEU).__draw_all( self )
 	end
end

function BU_MEU:active_link( x,y )
	--aaa.debug.print_traceback()
	
	local mu = self:get_mu_attached()
	aaa.print_fn()
	if mu then
		local t = self.__t_link
		local b_new = not t
		if b_new then
			t = {}
			self.__t_link = t
		end
		t.presence = 1
		if x then
			t.x = x
			t.y = y
		end
		if b_new then	-- avoid infinite loop between bu_meu and mu
			mu:active_link()
		end
	end
end

function BU_MEU:set_bu_cur()
	oo.getsuper(BU_MEU).set_bu_cur(self)
	self:active_link()
end 

function BU_MEU:draw_mu_links()

	local t = self.__t_link
	if not t then
		return
	end

	local presence = t.presence
	presence = presence - aaa.time.dt_real * .5
	if presence < 0 then
		self.__t_link = nil
		return
	end
	t.presence = presence
	--self:print( "BU_MEU:draw_links() presence is  "..presence )

	local mu = self:get_mu_attached()
	if mu then
		mu = GP.cur:get_mu_or_up_drawn( mu )
		if mu then
			local x,y = t.x or 0, t.y or .42
			local xc,yc = self:convert_xy_local_to_top( x,y )

			--table.print( self.__bus_mus.__down )

			local xb,yb = mu:convert_xy_local_to_top( 0,0 )
			--	xb,yb = self:convert_xy_top_to_local( xb,yb )

			local v = { xc-xb, yc-yb }
			V2.normalize(v)
			V2.scale( v, .1	 )
			local dx,dy = v[1],v[2]
			V2.rotate_90(v)
			self:gol_color_link( presence )
			gol.draw_triangle_fan_2d(	xb,yb, xc-dx+v[1],yc-dy+v[2],
										xc,yc, xc-dx-v[1],yc-dy-v[2] )
		end
	end
end