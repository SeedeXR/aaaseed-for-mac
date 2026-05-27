--todo GP should include a MEU to be more generic even be a MU
if CLASS.DECLARE( "GP" ) then
	GP:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
								"GP is needed by and APP_GP, in conjunction with a MUS it handle a bunch of MUs",
								"use GP.cur to access the current GP" )
end

function GP:deactivate()
	ga:set_method( "draw_before", nil )
	ga:set_ui_group_active( false )
	ga:set_method( "draw_after", nil )
	MEU.cam = nil
end
function GP:free()
	self:free_menu()
	local mus = self:get_mus_down()
	if mus then
		self:remove_mus_down()
		mus:free()
	end
	table.apply_method( self.__busses, "free" )
	self.__busses = nil
	self.__meu_ctx:free_protos()
	GP.cur = nil
end


function GP:get_mus_down()			return self:get_down(1)	end
function GP:add_mus_down( mus )
	if not mus then
		self:print_error( "calling add_mus_down( mus ) with a nil mus, this should not happen" )
		return
	end
	if self:get_down(1) then
		self:print_error( "calling add_mus_down( mus ) when the GP already have a mus_down, this should not happen" )
		return
	end
	-- if mus:get_bu_up() then
	-- 	self:print_error( "calling add_mus_down( bus ) when the bus already attached to a BU, this should not happen" )
	-- 	return
	-- end
	self:add_down(mus)
end
function GP:remove_mus_down()
	local mus = self:get_mus_down()
	if not mus then return end	--	this way if we remove too much we don't crash

	self:remove_down(mus)
	return mus
end
--
--	NEW
--
function GP:create( name, dir, b_monitor_pass )
	aaa.print_inverse( "GP:create( "..name.." )" )
	local self = GP:create_instance( name )
	self.ref = {}
	self.__busses = {}		--just used to regroup the bus used and access by name
							-- infact the table is used for save_bu_pos() and free() only (2023 Sept)
	self.__monitors_by_name = {}
	self.__ui_slot = { nb=0 }
	self.__render_index = 0
	GP.cur = self

	--self:activate()
	GABU.add_free_fn( GP.deactivate, self  )

	self:set_render( true )

	self.__meu_ctx = MEU_CTX:create( name )

	self.__meu_ctx:create_protos_kernel()
	--self.b_fbo_bug = false	--	ok now ? (Maa August 2013)

	local mus =	MUS:create( name, true, dir, 1/32 )	--maa was 1/40 but p2 is fun
	self:add_mus_down( mus )
	--mus:__set_up( self )	--done in previous call

	self:update_ui_rects()
	return self
end
function GP:update_before()
	--self:print( "mus flag "..( ga:is_ui_group_active() and self.__b_draw_mus ) )
--	aaa.show( ga:is_ui_group_active() .." ".. self.__b_draw_mus, "mus flag" )
--	aaa.show( BU:__get_text_type(), "TEXT type " )
	local busses = self.__busses
	if not busses then
		self:print_error( "this GP have no busses: skipping update_before()" )
		return
	end

	busses.mus:set_active( ga:is_ui_group_active() and self.__b_draw_mus )

	local mus = self:get_mus_down()
	if mus then	-- needed because of free order 
		local bus = mus.__bus_mus
    	bus.__x_offset_exp = ga.ortho_x_offset or 0	-- order of init ?
    	bus.__y_offset_exp = ga.ortho_y_offset or 0
	end

	self:update_ui_rects()

	--aaa.print( "Will call update_protos_all" )
	self.__meu_ctx:update_protos_all()
	--self.skel:update()
	self:update_ui_slot()

	--	SKELETON
	--if self.b_skel_draw then
	--	self.skel:draw( false )
	--end
end

--
--	MU/MEU access
--
GP.doc.get_mu_by_name = "( name_or_array ) return the first element with the requested name.\n"..
							"The search start at the highest level of the hierarchy, then on branches.\n"..
							"If the argument is an array of string, names are use one by one until a MU is found.\n"..
							"no_error version of these methods don't print or trigger any error, use it for test."
GP.doc.get_mu_by_name_no_error	= GP.doc.get_mu_by_name
GP.doc.get_meu_by_name =  "( name_or_array ) return the first element with the requested name.\n"..
							"The search start at the highest level of the hierarchy, then on branches.\n"..
							"If the argument is an array of string, names are use one by one until a MEU is found.\n"..
							"no_error version of these methods don't print or trigger any error, use it for test."
GP.doc.get_meu_by_name_no_error	= GP.doc.get_meu_by_name
function GP:get_mu_by_name_no_error( name_or_array )
	local mus = self:get_mus_down()
	if mus then	-- needed because of free order
		return mus:get_mu_by_name_no_error( name_or_array )
	end
end
function GP:get_mu_by_name( name_or_array )
	local mus = self:get_mus_down()
	if mus then	-- needed because of free order
		return mus:get_mu_by_name( name_or_array )
	end
end
function GP:get_meu_by_name_no_error( name_or_array )
	local mus = self:get_mus_down()
	if mus then	-- needed because of free order
		return mus:get_meu_by_name_no_error( name_or_array )
	end
end
function GP:get_meu_by_name( name_or_array )
	local mus = self:get_mus_down()
	if mus then	-- needed because of free order
		return mus:get_meu_by_name( name_or_array )
	end
end

GP.doc.get_meu_by_name_cached = "( name ) return the first meu with the requested name as get_meu_by_name_no_error().\n"..
									"The search start at the highest level of the hierarchy, then on branches.\n"..
									"The search is cached, so subsequent call are faster\n"..
									"a second value (boolean) is return when the call trigger the caching.\n"..
									"cached meu are checked to have been freed, in this case the searching is done again."
function GP:get_meu_by_name_cached( name )
	local cached = self.__meu_cached
	if not cached then
		cached = {}
		self.__meu_cached = cached
	end

	--self:print( "GP:get_meu_by_name_cached(): name is "..name )
	name = string.lower( name )
	local m = cached[name]
	if m then
		if m.__b_freed then
			self:print_debug( "GP:get_meu_by_name_cached() we had a MEU cached but it was freed : "..m )
		else
			return m, false
		end
	end

	m = self:get_meu_by_name_no_error( name )
	--todo do we really needed to check for this alternative name ?
	local name_bis
	if not m then
		name_bis = name.."_1"
		m = self:get_meu_by_name_no_error( name_bis )
	end
	if not m then
		self:print_error( "GP:get_meu_by_name_cached() Can't find "..name.." or "..name_bis )
		return
	end

	cached[name] = m
	return m, true
end


--TODO decide what we do for same name in dir
function GP:__register_mu( mu )
end
function GP:__unregister_mu( mu, b_definitive )
	if b_definitive then
		-- if self:is_mu_ui( mu ) then
		-- 	self:remove_mu_ui( mu )
		-- end

		if not mu:get_meu().__get_meu_referred then	--hack avoid for meu_ref
			local meu = mu:get_meu_used()
			if meu and (not meu.__b_freed) then
				self:remove_ui_slot( meu:get_ui_slot() )
			end
		end
	end
end

--
--	SAVE
--
function GP:save_as()
	aaa.print_method()
	local dirname = aaa.file.do_dialog_save( "GP Save", "" )
	if dirname then
		aaa.print( "will try to save "..dirname )
		self:get_mus_down():save_to_dir( dirname.."/", true )
	end
end

--
--	DRAW
--
function GP:draw_grid( alpha )
	gol.color_cyan( alpha )
	aaa.draw_lines_vert_nb(	31,	-4,.25,	-4,4 )
	aaa.draw_lines_vert_nb(	9,	-4,1,  	-4,4 )
	aaa.draw_lines_hori_nb(	16,	-2,.25,	-8,8 )
	aaa.draw_lines_hori_nb(	5,	-2,1,	-8,8 )
end

function GP:draw_grid_and_rect()
	--self:print_inverse( "GP:draw_grid_and_rect()" )
	--if true then return end
	self:draw_start()

	--	DRAW GRID
	local alpha = self.__grid_alpha 
	if alpha > 0 then
		self:draw_grid( alpha )
	end
end

function GP:__draw_gp_before()
	--if true then return end
	--if not bus_cur:is_active() then return end

	--self:set_viewport()

	self:draw_grid_and_rect()
end

function GP:register_mu_to_draw_link( mu )
	if not mu then return end

--	aaa.print_fn()
	mu.__t_link = { presence = 1 }

	local t = self:get_table_always( "__mu_link_to_draw" )
	t[mu] = mu
	
	local bu_meu = self:find_bu_meu_by_mu( mu )
	if bu_meu then
		bu_meu:active_link()
		--self:print( "ACTIVE LINK CALLED" )
	end
end

function GP:get_mu_or_up_drawn( mu, render_index )
	render_index = render_index or self:get_render_index()
	while mu.__draw_ui_render_index ~= render_index do	-- this mu was not drawn this frame
		--self:print( "mu is "..mu )
		mu = mu:get_meu_dir()
		if not mu then
			return
		end
	end
	return mu
end

function GP:__add_gp_connection( con, b_changed )
	--todo when in loops like with MuBegin/End we need to avoid storing one connexion each time
	--so we have check if connexion is stored and if it had changed target (in or out) or not   
	con.b_changed = b_changed
	table.insert( self.__trax_connexion, con )
end

function GP:__draw_gp_connexion()
	local cons = self.__trax_connexion
	if not cons then
		return
	end
	
	local render_index = self:get_render_index()
	local square_size = BU.__draw_text_line_width * .01
	local ph = aaa.time.t*4 % 1
	gol.set_line_width( BU.__draw_text_line_width )

	local function draw_square( xa,ya, xb,yb, ph )
		local x = interpolate( xa,xb, ph )
		local y = interpolate( ya,yb, ph )
		--self:print( ph )
		--self:print( x.." "..y )
		aaa.draw_rect_axe_z( x,y,0, square_size )
		--aaa.draw_disk_axe_z( x,y,0, size, 6 )
	end

	
	local function draw_link( xa,ya, xb,yb, b_out )
		if b_out then
			xa,ya,xb,yb = xb,yb,xa,ya
		end
		local dx = xb - xa
		local dy = yb - ya
		local len = math.sqrt( dx*dx + dy*dy )
		local nbf = len * 4
		local nb, frac = math.modf( nbf )
		aaa.draw_line( xb,yb, xa,ya )
		local x,y = xa,ya
		local xe,ye
		local f
		for i=1,nb do
			f = i / nbf
			xe = interpolate( xa,xb, f )
			ye = interpolate( ya,yb, f )
			draw_square( x,y,xe,ye, ph )
			x,y = xe,ye
		end
		if ph < frac then
			f = (nb+1) / nbf
			xe = interpolate( xa,xb, f )
			ye = interpolate( ya,yb, f )
			draw_square( x,y,xe,ye, ph )
		end
	end

	local function get_xy( meu )
		local mu = meu:get_mu()
		if mu then
			-- we get the mu or the containing dir drawn
			mu = self:get_mu_or_up_drawn( mu, render_index )
			if mu then
				-- self:print( " --> "..mu )
				return mu:convert_xy_local_to_top( -.35,0 )
			end
		end
	end

	local function draw_xy_meu( xa,ya, meu, b_out )
		local xb,yb = get_xy( meu )
		if xb then
			draw_link( xa,ya, xb,yb, b_out )
		else
			self:print( "draw_link "..meu.." with mu nil when processing bu_trax" )	
		end
	end

	--table.print( cons, "trax_connexion" )
	for i,con in IPAIRS(cons) do
		--table.print( con, "con", 1 )
		local bu_trax,meu_trax = con.bu_trax,con.meu_trax
		local bu,id,meu = meu_trax:__get_connection_bu_id_meu( con )
		local xa,ya, xb,yb
		local b_change
		local b_trax_out = con.b_trax_out
		if b_trax_out then	b_change = bu.__b_change_out
		else				b_change = bu.__b_change_in
		end
		
		local function set_color(bu)
			if not bu:__set_color_highlight() then 
				if b_change then gol.color_red(.5) else gol.color_green( .5 ) end
			end
		end
		if render_index == bu.__draw_ui_render_index then
			if b_trax_out then	xa,ya = bu:convert_xy_local_to_top( -.44,.25 )
			else				xa,ya = bu:convert_xy_local_to_top( .44,-.25 )
			end
			
			if render_index == bu_trax.__draw_ui_render_index then
				xb,yb = bu_trax:convert_xy_local_to_top( b_trax_out and .42 or -.42, .5 )
		
				if b_change then
					gol.color_red()
				else
					local _,b = bu_trax:__get_blink_phase_bool()
					if b then
						gol.color_black()
					else
						gol.color_green()
					end
				end
				aaa.draw_disk_axe_z( xb,yb, .1, square_size*1.5 )
				set_color(bu)
				draw_link( xa,ya, xb,yb, b_trax_out )
			else
				set_color(bu)
				draw_xy_meu( xa,ya, meu_trax, b_trax_out )
			end
		else
			xa,ya = get_xy( meu )
			if xa then
				if render_index == bu_trax.__draw_ui_render_index then	
					xb,yb = bu_trax:convert_xy_local_to_top( b_trax_out and .42 or -.42, .5 )
					gol.color_green()
					aaa.draw_disk_axe_z( xb,yb, .1, square_size*1.5 )
					set_color(bu)
					draw_link( xa,ya, xb,yb, b_trax_out )
				else
					set_color(bu)
					draw_xy_meu( xa,ya, meu_trax, b_trax_out )
				end
			end
		end
	end
end

-- key is "in" or "out" (relative to the MEU_TRAX)
function GP:__add_gp_trax( meu, bu, key, b_changed )
	--aaa.print_fn()
	local all = self.__bu_trax
	local t_bu = all[bu]
	local t_key
	local t_meu
	if t_bu then
		t_key = t_bu[key]
		if not t_key then
			t_key = {}
			t_bu[key] = t_key
		end
		t_meu = t_key[meu]
		if not t_meu then
			t_meu = {}
			t_key[meu] = t_meu
		end
	else
		t_bu = {}
		all[bu] = t_bu
		t_key = {}
		t_bu[key] = t_key
		t_meu = {}
		t_key[meu] = t_meu
	end

	t_meu.b_changed = b_changed
end

function GP:__draw_gp_after()

	local lw_pushed = gol.get_line_width()

	-- draw the links between BU_MEU and its corresponding MU
	for i=1,self:get_ui_slot_nb() do
		local bu_meu = self:get_ui_slot_bu_meu(i)
		bu_meu:draw_mu_links()
	end

	if MEU_TRAX.b_link_active then
		--self:__draw_gp_trax()
		self:__draw_gp_connexion()
	end

	local render_index = self:get_render_index()
	-- this is a table because of multitouch
	-- each elt is a mu we have to draw links to/with
	local to_draw = self.__mu_link_to_draw
	-- if table.count_item( to_draw ) > 0 then
	-- 	table.print( to_draw, "to_draw", 1 )
	-- end
	--todo not totally made for multitouch in particular __draw_link_render_index
	for mu,_ in PAIRS(to_draw) do
		--we keep this table t for each mu
		local t = mu.__t_link
		if not t then	--if gone we are done for this one
			to_draw[mu] = nil
		else
			-- the links fade
			local presence = t.presence - aaa.time.dt_real * .5
			--self:print( mu.." "..presence )
			if presence <= 0 then
				to_draw[mu] = nil
				mu.__t_link = nil
			else 
				t.presence = presence
	
				local meu_used = mu:get_meu_used()
				if meu_used then
					local proto = meu_used:get_proto()
					if proto then
						-- we got the proto (so the type from our original mu)
						-- we get the mu or the containing dir drawn  	
						mu = self:get_mu_or_up_drawn( mu, render_index )
						if mu then
							-- ok we have a source
							local x,y = mu:convert_xy_local_to_top( 0,0 )

							-- inst will contain all the instance of the proto in a new table	
							local insts = proto:get_instances_array()
							-- we now add the ref referencing this same type too
							local proto_ref = MEU_CTX.cur:get_proto_from_type( "Ref" )
							--table.print( proto_ref, "proto_ref" )
							for _, meu in IPAIRS( proto_ref:get_instances_array() ) do
								if meu.__b_freed then
									self:print_debug( "skipping freed meu "..meu )
								else
									local meu_used = meu:get_meu_used()
									if meu_used.__b_freed then
										self:print_debug( "skipping freed meu_used "..meu_used )
									else
										if meu_used:get_proto() == proto then
											table.insert( insts, meu )
										end
									end
								end
							end
				
							-- now we have a table of destinations as MEU
							--table.print( insts, "insts for draw_link" )
							--self:print( "draw_links"..insts )
							-- so now we draw links with other MU
							mu:gol_color_link( presence )
							for _, meu in PAIRS( insts ) do
								local mu_b = meu:get_mu()
								if mu_b then
									-- we get the mu or the containing dir drawn
									mu_b = self:get_mu_or_up_drawn( mu_b, render_index )
									if mu_b and mu_b ~= mu then
										if mu_b.__draw_link_render_index ~= render_index then -- avoid redraw (pb in mutitouch))
											-- self:print( " --> "..mu_b )
											local xb,yb = mu_b:convert_xy_local_to_top( 0,0 )
											aaa.draw_line( xb,yb, x,y )
											if meu:is_proto() then
												aaa.draw_circle_axe_z( xb,yb,0, .25, 32 )
											end
											mu_b.__draw_link_render_index = render_index	-- mark to avoid redrawing it
										end
									end
								else
									self:print( "draw_link "..meu.." with mu nil probably a proto and there is no Dir_Protos" )	
								end
							end
						end --if mu
					end -- if proto
				end -- if meu_used
			end -- if presence
		end
	end	-- for mu,_ in PAIRS(to_draw) door

	gol.set_line_width( lw_pushed )
end

function GP:draw_fore()
	self:draw_start()

	self:get_mus_down():__draw_on_top(true)
end

function GP:draw_start()
	gol.set_depth( false )
	gol.set_texture_dim( 0 )
	gol.set_line_width( 1. )
end

function GP:set_mu_selected( mu )
	self.mu_selected = mu
end

function GP:is_render()			return self.__b_render					end
function GP:set_render( b )		self.__b_render = b						end
function GP:flip_render()		self:set_render( not self.__b_render )	end

GP.doc.get_render_index = "() the index returned is incremented at each render"
function GP:get_render_index()	return self.__render_index				end

function GP:render()
	local mus = self:get_mus_down()
	if not mus then
		self:print_error( "this GP have no mus: skipping render()" )
		return
	end

	--self:print( "GP:render()" )
	--	if true then return end
	local ri = self.__render_index + 1
	self.__render_index = ri
	BU.__render_index_cur = ri

	--todo chage whole structure to reuse if we want to avoid GC
	self.__bu_trax = {}
	self.__trax_connexion = {}
	
	if GA.b_verbose_render then self:print( "\t\trender() "..self:get_render_index().." begin" ) end

		self.__b_rendering = true
			
		mus:begin_render()
		if not self:is_render() then
			--self:print( "No render" )
			return
		end

		if GA.b_spy then aaa.spy.push_range( "GP", 8 ) end
			mus:do_render( 8 )
		if GA.b_spy then aaa.spy.pop_range() end

		gol.reset()
		self.__b_rendering = false

	if GA.b_verbose_render then self:print( "\t\trender() "..self:get_render_index().." end" ) end
end

function GP:hide_mu_unused()
	aaa.show( "hide_mu_unused()", self ) 
	self:get_mus_down():hide_mu_unused()
end

--todo remove
function GP:init_ui_rects()
	self:set_rect_free{	l=0,		r=2,		b=0,		t=2		}
end

function GP:set_rect_free( r )		self.__rect_free = r 		end
function GP:get_rect_free()			return self.__rect_free		end

function GP:update_ui_rects()
	self:init_ui_rects()
end
--
--	Monitor
--
function GP:__recompute_register_monitor_with_button()
	local t = self:get_table_always( "__monitor_with_button_by_meu" )
	local out = {}
	for meu, bu_mon in PAIRS(t) do
		local name = string.lower( meu:get_inst_key() )
		out[name] = bu_mon
	end
	self.__monitor_with_button_by_name = out

end
function GP:__register_monitor_with_button( meu, bu_mon )
	local t = self:get_table_always( "__monitor_with_button_by_meu" )
	t[meu] = bu_mon
	self:__recompute_register_monitor_with_button()
end
function GP:__get_monitor_with_button( name )
	local t = self:get_table_always( "__monitor_with_button_by_name" )
	return t[string.lower(name)]
end

function GP:get_monitor( name )
--	table.print( self.__monitors_by_name, "Gp __monitors_by_name", 1 )
--	self:print( "Search "..name.." Found "..self.__monitors_by_name[name] )
	return self.__monitors_by_name[name]
end
function GP:get_monitor_by_bind( bind )
	for name,mon in PAIRS(self.__monitors_by_name) do
		if mon:get_texture_bind_2d() == bind then
			return mon
		end
	end
end

--todonownow
-- function GP.__is_monitor_active( bu )
-- 	if BU.is_active( bu ) then
-- 		return not bu:is_mini() or not GP.cur.bu_bank:is_full_page()
-- 	else
-- 		return false
-- 	end
-- end
function GP:add_monitor( bus, mu, name, x,y, f, bind )
	if type(bind)=="string" then
		bind = TEXS:get_bind_by_name(bind)
	end

--	if not mu then
--		self:box_good( "Can't add_monitor() named : "..name.." because no mu passed" )
--		return
--	end
--	self:box_good( mu.." add_monitor() with name "..name )

	--todonownow
	if mu then 
		local meu = mu and mu:get_meu() or nil
		if meu then
			name = name or mu:get_inst_key()
			bind = bind or meu:get_texture_bind_2d()
		end
	end


	if not name then
		name = "Bind "..bind
	end

	local sx,sy = aaa.img.get_size( bind )
	if not sx or not sy then
		sx = 2
		sy = sx * 9/16
	end
	if sx > sy * 2 then
		sy = 4 * sy/sx
		sx = 4
	else
		sy = 2 *sy/sx
		sx = 2
	end
	sx,sy = sx*f,sy*f

	local bu_mon = bus:add_monitor( name, {x,y, sx,sy},	bind, false	)
		bu_mon:set_grid_xy( 1/32, 1/32 )
		bu_mon:set_inertia( false )
		bu_mon:set_sxy_min( .125, .125 )
		bu_mon:set_ui_change_bind( true )
		name = name or ("NoName bind "..bind)	--2021 Maa happen in mdh_tous_dif
		bu_mon.__b_gp_mon = true
		self.__monitors_by_name[name] = bu_mon

	--todonownow
	--bu_mon.is_active = GP.__is_monitor_active

	return bu_mon
end

function GP:get_mu_video( i )
	local letter = string.char( i+64 )
	return self:get_mu_by_name_no_error( "Video_"..letter ), letter
end
function GP:get_mu_fbo( i )
	local name = "Fbo_F"..i
	return self:get_mu_by_name_no_error( name ), name
end

function GP:define_monitoring( b_monitor_pass, monitor_m_nb )
--	local scale_pushed = BU.__scale_min
	BU.__scale_min = .25

	local bus = BUS:create( self:get_name().."_Monitoring" )
	bus:set_fname( "AAA_Monitors" )

	bus:init_begin_add_to_ga()
	bus:set_bu_pos_load_save(true)

		local bu
		local param_ref

		local sx = 4
		local ry = 9/16		--default
		local sy = sx * ( ry or 1. )

--		BUI.set_meter( false )
--		BUI:set_meter( true )
	--
	--	VIZ PRE DIF
	--
		--	see fbos
		self:add_monitor( bus, self.mu_pre, "PRE",	-sx*.75,	2.25-sy*.25,	1 )
		self:add_monitor( bus, self.mu_dif, "DIF",	-sx*.25,	2.25-sy*.25,	1 )
		--	space to have a different name for load save
		-- remove (2025 Feb)
		--self:add_monitor( bus, self.mu_viz, "Viz ",	0.,			0.,				1 )
		self:add_monitor( bus, self.mu_pre, "Pre ",	2.,			2,				.2 )
		self:add_monitor( bus, self.mu_dif, "Dif ",	3.5,		2,				.2 )
		self:add_monitor( bus, nil,			"Free",	0,			0,				.2,		1 )


		sy = .75
		for i=1,26 do
			local mu, letter = self:get_mu_video( i )
			if mu then
				self:add_monitor( bus, mu, letter, -4.5+i, sy*.25, .5 )
			end
		end

		for i=1,24 do
			local mu, name = self:get_mu_fbo( i )
			if mu then
				--self:box_debug( "Monitor name "..name )
				--todo move in Meu monitor
				local meu_type, inst_name = MEU_CTX.cur:split_meu_type_inst( name )
				bu = self:add_monitor( bus, mu, inst_name, -4.5+i*.5, sy*.25, .5 )
				bu:set_text_xy_fxy_mini( 0,0, .08,.7 )
			end
		end

		sx,sy = .25,.25	--sx*9/16

		--aaa.box_debug( "monitor_m_nb is "..monitor_m_nb  )
		monitor_m_nb = monitor_m_nb or 16
		--aaa.box_debug( "after monitor_m_nb is "..monitor_m_nb  )
		for i=1,monitor_m_nb do
			--bu = self:add_monitor( bus, nil, "M"..i, TEXS:get_bind_by_name("in"..i), 0, 1., .5 )
			bu = self:add_monitor( bus, nil, "M"..i, 0, 1., .5,		"m"..i )
			local ix = (i-1)%8
			--bu:set_pos_mini( {(ix-.5)*sx, 2.75-((i<=8) and 0 or -1)*sy, sx,sy}, true )
		end

		if b_monitor_pass then
			local x = -2
			local dx = .25

			local group = TEXS:get_tex_named():get_group_from_name( "pass" )
			--table.print( __groups_by_name, "pass group", 3 )
			--self:box_debug( "stop" )
			for i, e in IPAIRS( group.texs ) do
				--local name = e.name_short
				--self:box_debug( "add_monitor : name is "..name )
				self:add_monitor( bus, nil,	e.name_short or e.name, x+dx*(i-1),		2,	.8, e.bind_flat )
				--self:add_monitor( bus, nil,	e.name, x+dx*(i-1),		2,	.8, e.bind_flat )
			end
		end

	bus:init_end()

--	BU.__scale_min = scale_pushed
	return bus
end

function GP:define_mus()	--	nb_vid, nb_vid_f, nb_fbo )
	local mus = self:get_mus_down()
	local bus = mus:begin_define()

		--	FBO DIF PRE VIZ F1 ...
		
		local function add_fbo( name )
			local bind = TEXS:get_bind_by_name( name )
			self:print( "add_fbo( \""..name.." \") bind is "..bind )

			local meu
			local tab = self.__meu_ctx:get_instances( "Fbo", name )
			if tab then
				if #tab == 1 then
					meu = tab[1]
				else
					self:print_error( "several fbo "..name )
					return
				end
			end
			local mu
			if meu then
				mu = meu:get_mu()
			else
				mu = mus:create_mu( "Fbo", name )
				if mu then
					meu = mu:get_meu()
				end
			end
			if meu then
				meu:get_fbo():set_attachment_bind( 1, bind )
			end
			return mu, meu
		end

		self.mu_dif = add_fbo( "DIF" )
		self.mu_pre = add_fbo( "PRE" )
		--todo remove or reactivate Viz
		-- exist but do nothing any more (2023 October)
		-- removed (2025 Feb)
		--self.mu_viz = add_fbo( "VIZ" )

	mus:end_define()

	ga:add_bus( bus )
	bus:set_method( "draw_after", self, "draw_fore" )

--[[
	self:box_debug( "start dealing with dir protos" )
	local mu_protos = self:get_mu_by_name_no_error( "dir_protos" )
	if mu_protos then
		--aaa.box_good( self.." found mu_protos "..mu_protos )
		local mus_protos = mu_protos:get_meu():get_mus_down()
		--aaa.box_good( self.." found mus_protos "..mus_protos )
		mus_protos:set_mu_save( false )
		mus_protos:set_render( false )

		-- init the protos
		table.apply_fn(	self.__meu_ctx:get_protos_table(),
			function(meu)
				if meu:is_proto_and_isolated() then
					mus_protos:create_mu_from_meu( meu )
				end
			end )
		mus_protos.__bus_mus:load_bu_pos()
	end
--]]

	return bus
end

function GP:get_bus_mus()
	return self.__busses.mus
end

--
--	UI SLOT
--
function GP:get_ui_slot_nb()			return self.__ui_slot.nb		end
function GP:get_ui_slot_def()			return self.__ui_slot.def		end
function GP:get_ui_slot_def_dir()		return self.__ui_slot.def_dir	end
function GP:set_ui_slot_def( id )		self.__ui_slot.def = id			end
function GP:set_ui_slot_def_dir( id )	self.__ui_slot.def_dir = id		end

GP.doc.set_ui_slot_config = "( { nb=N, def=D, def_dir=DD } ) central entry to set the ui_slot configuration. Initializes slot tables if not done yet and updates nb, def, def_dir."
function GP:set_ui_slot_config( t )
	--todo when UI_CONFIG lands, this will also diff old vs new nb and create/destroy BU_MEU accordingly
	self.__ui_slot.nb = t.nb
	for i=1,t.nb do
		self.__ui_slot[i] = self.__ui_slot[i] or {}
	end
	self:set_ui_slot_def( t.def )
	self:set_ui_slot_def_dir( t.def_dir )
end

function GP:get_ui_slot_bu_meu( i )
	return self.__ui_slot[i].bu_meu 	--todo protect
end
function GP:get_ui_slot_meu( i )
	--aaa.print_fn()
	return self:get_ui_slot_bu_meu(i):get_meu_attached()
end

function GP:find_bu_meu_by_mu( mu )
	local slots = self.__ui_slot
	for i=1,slots.nb do
		local bu_meu = slots[i].bu_meu
 		if bu_meu:get_mu_attached() == mu then
 			return bu_meu
 		end
 	end
end
-- function GP:find_ui_slot_by_meu( meu )
-- 	local slots = self.__ui_slot
-- 	for i=1,slots.nb do
-- 		if slots[i].meu == meu then
-- 			return i
-- 		end
-- 	end
-- 	return nil
-- end
GP.doc.get_meu_ui_slot_def = "( meu )return the default ui_slot for this MEU"
function GP:get_meu_ui_slot_def( meu )
	return  meu:is_derived_from_class(MEU_DIR_BASE) and self:get_ui_slot_def_dir() or self:get_ui_slot_def()
end
GP.doc.set_meu_ui_slot = "( meu, mu, i ) i==nil means default. return the slot id used or nil"
function GP:set_meu_ui_slot( meu, mu, i )
--	aaa.print_fn()
--	aaa.print_method()
--	self:print_error( "i is "..i )

	if not self.__ui_slot then return false end

	if not meu then
		if not mu then
			self:print_error( "No meu or mu: can not do anything here" )
			aaa.debug.print_traceback()
			return false
		end
		meu = mu:get_meu_used()
	end

	local nb = self:get_ui_slot_nb()
	local i_old = meu:get_ui_slot()	--was self:find_ui_slot_by_meu( meu )
	if i_old then
		self:print_debug( meu.." already in Ui"..i_old )
	end
	if not i and i_old then return false end

	--aaa.print( i.." / "..nb )
	if i then
		if i==0 then
			self:print_error( "i can not be 0 means create a new one" )
			i =  self:get_meu_ui_slot_def( meu )
		end
		i = ((i-1) % nb) + 1
	else
		i = self:get_meu_ui_slot_def( meu )
	end
--	aaa.print_debug( i )

--	already there and at the same place or we want it in 4
--todo change the mu eventually
	if i_old and i==i_old then return false end

--	remove from older
	if i_old then
		self:remove_ui_slot( i_old )
	end

	--	remove this one
	self:remove_ui_slot( i )
	--self:print( mu..", "..i.." )" )

	local bu_meu = self:get_ui_slot_bu_meu(i)
	if not bu_meu then		
		self:box_debug( "No bu_meu for slot "..i )
		return
	end

	bu_meu:attach_meu_mu( meu, mu, i )

	return i	
end

function GP:remove_ui_slot( i )
	if not i then return end

--	aaa.print_fn()
--	aaa.debug.print_traceback()

	local bu_meu = self:get_ui_slot_bu_meu(i)
	if bu_meu then
		bu_meu:detach()
	else
		self:box_debug( "No bu_meu for slot "..i )	
	end

end
function GP:remove_meu_from_ui_slot( meu )
--	self:remove_ui_slot( self:find_ui_slot_by_meu( meu ) )
	self:remove_ui_slot( meu:get_ui_slot() )
end

function GP:update_ui_slot()
	local mu_ui = self:get_mu_ui()
	for i=1,self:get_ui_slot_nb() do
		local meu = self:get_ui_slot_meu(i)
		if meu then
			--todo refine this condition is too large
			--if meu.__b_gp_ui_displayed then
				meu:__do_update_ui( mu_ui )
			--end
		end
	end
end

-- UI
--
if false then
	function GP:set_mu_ui( mu )
		self:__change_mu_ui( mu )
		return mu:set_ui_slot()
	end
	function GP:is_mu_ui( mu )	return mu == self.__mu_ui	end
	function GP:get_mu_ui()		return self.__mu_ui end

	function GP:__change_mu_ui( mu_next )
		local cur = self:get_mu_ui()
		--self:print( mu.." "..mu_ui  )
		if cur == mu_next then
			return false
		end

		if cur then
			local bu_title = cur:get_meu_used():get_bu_title()
			-- could be nil when bu have no ui field
			if bu_title then
				bu_title:set_text_color( 1,1,1, 1 )
			end
		end

		self.__mu_ui = mu_next
		if mu_next then
			local meu_next = mu_next:get_meu_used()
			meu_next:set_focus_on_obj()	--todo mu:set_focus ?
			meu_next:get_bu_title():set_text_color( 1,0,1, 1 )
		end
	end

	function GP:remove_mu_ui( mu )
		self:__change_mu_ui( mu )
	end
	function GP:get_meu_ui()
		local mu = self.__mu_ui
		if mu then
			return mu:get_meu_used()
		end
	end
else
	function GP:get_mu_ui()
		local bu = BU:get_bu_cur()
		if bu then
			if bu:is_class(MU) then
				return bu
			end
		end
	end
end




function GP:define_back_ui()
	--	add windows for ui

	local bus = BUS:create( "Back Ui" )
	bus:set_fname( "AAA_Uis" )
--auv was	bus:init_begin()
	bus:init_begin_add_to_ga()
		bus:set_bu_pos_load_save( true )
		--	UIx rect
		local nb = self:get_ui_slot_nb()

		local S = 2. --todo this number seems to have no influence
		for i=1,nb do
			local bu_meu = bus:add_bu( BU_MEU:create( "Ui"..i,	{(i-1)/nb*3*S-3,-1.2, S,S} ) )
				bu_meu:set_size_orig_for_mini( 1/4, 1/4 )
				bu_meu:set_grid_xy( 1/8, 1/8 )
				bu_meu:set_sxy_min( 1/8, 1/8 )
				bu_meu:set_grid_angle( 1./32. )
				bu_meu:set_inertia( false )
				bu_meu:set_text_xy_fxy_mini( 0,.45,	.08,.6 )
				bu_meu:set_text_nice()

			self.__ui_slot[i].bu_meu = bu_meu
		end

	bus:init_end()

	return bus
end

function GP:define_base_ui()
	--KINECT and SKEL
	--self.skel = SKEL:create( aaa.module.get_cur() )
	--self.skel:set_top_to_track()

	BU.__scale_min = .125

	local bus = BUS:create( self:get_name().."_base_ui"  )
	bus:set_fname( "AAA_regular" )

	bus:init_begin_add_to_ga()
	bus:set_bu_pos_load_save(true)

		--	ON/OFF button
		local s = .125
		local hs = s * .5

		local cx = 3
		local x2 = 4 - s*2
		local t = 2

		-- bu	= bus:add_trig( "Hide Unused", {cx-s*6,r_dif.t - hs+s, s*3,s} )
		-- 	bu:set_method_on_click( self, "hide_mu_unused")
		-- 	bu:set_pos_load_save( false )
		-- 	bu:set_value_load_save( false )
		local sxb = s*8/3
		local bu
		local function add( name, x,y, sx,sy, field_name, value_def )
			local bu = bus:add_button( name, {x,y, sx,sy} )
				:set_value_type_bool(true)
				:set_target( MU, field_name )
				:set_value( value_def )
				:set_pos_load_save( true )
				:set_value_load_save( true )
				:set_ui_top_size( true )
			return bu
		end
		local Y = t - hs+s
		add( "Hidden", cx-sxb,Y, sxb,s, "b_draw_hidden", false )
		self.ui.bu_unused =
		add( "Unused", cx,	  Y, sxb,s, "b_draw_unused", false )
		add( "Used",   cx+sxb,Y, sxb,s, "b_draw_used",   true  )

		--	UIx button
		local SX
		local SY = .1
		local Y = -SY*.5
		--todouif do it when no UIF 
		-- local nb = self.__ui_slot.nb
		-- SX = .9 / nb
		-- for i=1,nb do
		-- 	bu = bus:add_trig( "UI"..i, {1.+(i-0)/nb-.5*SX,Y, SX,SY}, .8 )
		-- 		bu:set_method_on_click( self, "send_to_ui_slot", i )
		-- 		bu:set_pos_load_save( false )
		-- end
		SX = SY*2
		local D = SY/16
		bu = bus:add_trig( "Debug", {2-.5*SX-D,Y, SX,SY}, 1 )
			bu:set_confirmation( true )
			bu:set_color_back_lua()
			--bu:set_pos_load_save( false )
			bu:set_function_on_click( aaa.debug.debug )
			bu:set_mobile( true )
			bu:set_ui_top_size( true )
			bu:set_pos_load_save( true )

		--	save all
		SX = .4
		bu = bus:add_trig( "Save All MEUs", {2-.5*SX-D,-.25+SY/2, SX,SY} )
			:set_color_back("save")
			bu:set_method_on_click( MEU, "save_all" )
			bu:set_mobile( true )
			bu:set_ui_top_size( true )
			bu:set_pos_load_save( true )
			--bu:set_pos_load_save( false )
			bu:set_fx_on_click( true )
			self.ui.bu_save_all = bu
		--	bu:set_confirmation( true, "Do Not", "Save" )

	--generate too massive problem for now	
		-- bu = bus:add_trig( "Save All Presets", {1-SX*.5,Y+SY, SX,SY} ):set_color_back("save")
		-- 	bu:set_method_on_click( MEU, "save_all_presets" )
		-- 	bu:set_pos_load_save( false )

--todo do it finally
--		bu = bus:add_trig( "Save inst", {1-SX*.5,Y+SY, SX,SY} )
--			bu:set_method_on_click( MEU, "save_as_all_inst" )

--		bu = bus:add_trig( "Save As",   {1-SX*.5,Y+2*SY, SX,SY} )
--			bu:set_method_on_click( self, "save_as" )

	bus:init_end()

	return bus
end

--todo make sure it load save
function GP:define_ui_gp()
	local bus = BUS:create( "GP" )
	bus:set_bu_pos_load_save( true )
	bus:set_fname( "AAA_Gp" )
	bus:init_begin()

		local SX  = .08
		local SY  = SX
		local DY  = SY/2
		local DDY = DY * .05
		local y_start = .4

		local y = .4

		local function process_bu( bu, tab, field_name, value )
			bu:set_pos_load_save(   false )
				:set_target( tab, field_name, value )
				y = y - SY
			return bu
		end	
		local function add_but( x, sx, name, tab, field_name, value, b_load_save )
			return process_bu( bus:add_button( name, {x,y, sx,SY}) , tab, field_name, value )
				:set_text_rect_ratio( 5 )
				:set_value_load_save( b_load_save~=nil and b_load_save or false )
		end
		local function add_slider( x, sx, name, tab, field_name, value, b_load_save )
			return process_bu( bus:add_slider( name, {x,y, sx,SY}) , tab, field_name, value )
				:set_value_load_save( b_load_save~=nil and b_load_save or false )
		end

		local x = -.4
		local DX = SX*.7

		local	bu

		bu	= add_but(	x,		SX,		"Monitor Draw",	BU_MONITOR, "__b_draw", 			true )
		bu	= add_but(	x+DX,	SX,		"Mini inside",	BU_MONITOR, "__b_draw_inside_mini", true )
		y = y - DY
		bu	= add_but(	x,		SX,		"MUS Draw",		self, "__b_draw_mus",	true )
		x = x + DX
		bu	= add_but(	x,		SX,		"MU Draw",		MU, "b_mu_draw",		true )
		x = x + DX
--		bu	= add_but(	x,		SX,		"MU Unused",false ):set_target(	MU, "b_draw_unused"		):set_value_type_bool( true )
		bu	= add_but(	x,		SX,		"Text",			MU, "__b_draw_text",	true )
		bu	= add_but(	x,		SX,		"Icon",			MU, "__b_draw_icon",	true )
		bu	= add_but(	x,		SX,		"Clip",			MU, "__b_clip",			true )
		x = x - DX*2
		y = y - DY
		bu	= add_but(	x,		SX,		"BU_MEU Draw",	BU_MEU, "__b_draw",				true )
		y = y - DY
		bu	= add_but(	x,		SX,		"Render Chain",	GP, "__b_draw_render_chain",	true )

		local o = SX * 8
		x = x + o
		y = y_start
		bu	= add_slider(	x,	SX*5,	"Grid Alpha",	GP,	"__grid_alpha",	.25, true )
			:add_values_def( .25,.5,.75 ):set_meter(true)
		y = y - DY/2
		--bu	= add_but(	x,	y-S*1,	S,	"Monitor Lock",	false ):set_target(	BU_MONITOR, "__b_lock", false )
		bu	= add_but(	x,		SX*5,	"MEU Timings",	MEU,"__b_timings",			true, true )

	bus:init_end()
	return bus
end

function GP:define_ui_top()
--	BU.__scale_min = .125

	local bus = BUS:create( self:get_name().."_top" )
	bus:set_fname( "AAA_top" )

	bus:init_begin_add_to_ga()
	bus:set_bu_pos_load_save(true)

		local bu
		local mar = .0
		--	MIDI
		bu = bus_cur:add_bu( BU:create_window_center( "MIDI", {-.5, -.125, 1,1}, BUI:define_ui_midi() ) )
			--	bu:set_border( true )

		bu = bus_cur:add_midi(	{-0.5+mar,0.5-mar, 1-2*mar,1-2*mar}, "MIDI 1-64", .02,.015, 2,4, true,
								1, 0,  1, 8,
								1, 16,  1, 24,
								1, 32,  1, 40,
								1, 48,  1, 56
								)					
		bu = bus_cur:add_midi(	{-0.5+mar,0.5-mar, 1-2*mar,1-2*mar}, "MIDI Ch", .02,.015, 2,4, false,
						1, 0,  1, 8,
						2, 0,  2, 8,
						3, 0,  3, 8,
						4, 0,  4, 8
						)
			--	bu:set_size_orig_for_mini( .1, .1 )

		--	bu for cam edit state
		local SY = .125
	
		local y
		bus:add_wiz_alive(		{SY, 		-SY,	SY*2} )
		local SX = .5
		y = -SY/2
		local x = SY*2
		local SX_TEXT = .8
		bus:add_wiz_cam(		{x+SX/2, 	y,		SX, SY} )
		bus:add_wiz_send(		{x+SX+.2,	y,		.4, SY} )
		bus:add_wiz_watch(		{1.6, 		y,		SX_TEXT, SY} )
		
		bus:add_wiz_maaeb(		{1.875, 2.125,		.25} )		

		bus:add_wiz_show(		{1, 1, 	1, .5} )
		bus:add_wiz_mess(		{1, .5,	2, .5} )

		y = -.25+SY/2
		
		bus:add_wiz_fps(		{x+SX_TEXT/2,	y,	SX_TEXT, SY} )
		bus:add_wiz_memory(		{1.05, 			y,	SY*2, SY} )
		bus:add_wiz_eye(		{1.4,			y,	SY*2, SY} )

		y = 2.5
		bus:add_wiz_blob(		{1,		y,	SX} )
		bus:add_wiz_power(		{1.5,	y,	SX*2, SY} )

		bus:add_bu_keyboard(	"GP",	{0, 0,		1, 1.9} )
	
	bus:init_end()

	return bus
end

function GP:save_bu_pos( dirname )
	--aaa.debug.print_traceback()
	--self:box_good( "GP:save_bu_pos( \""..dirname.."\" )" ) 
	--self:print( "GP:save_bu_pos( \""..dirname.."\" )" ) 
	--todo really see with GA what he saves
	--table.apply_method( self.__busses, "save_bu_pos", dirname )
end
function GP:save_top_level()
	local mus = self:get_mus_down()
	local dir = mus:get_dir_inst()
	self:save_bu_pos( dir )
	mus:save_top_level()
end


function GP:define_ui_classic( ui_slot_nb, b_monitor_pass, monitor_m_nb )	--, nb_vid, nb_vid_f, nb_fbo )
	--self:box_error( "GP:define_ui_classic()" )

	self.ui = {}

	--aaa.print_fn()
	ui_slot_nb = ui_slot_nb or 4
	self:set_ui_slot_config{ nb=ui_slot_nb, def=ui_slot_nb-1, def_dir=ui_slot_nb }

	local bus
	local busses = self.__busses
	bus = self:define_base_ui()
		busses.base_ui = bus
		ga:register_ui_group( bus )
		bus:set_method( "draw_before",	self, "__draw_gp_before" )
		
	bus = self:define_mus()	--menu are here
		busses.mus = bus
		ga:register_ui_group( bus )	-- nb_vid, nb_vid_f, nb_fbo ) )

	bus = self:define_back_ui()
		busses.back_ui = bus
		ga:register_ui_group( bus, "top" )

	bus = self:define_ui_top()
		busses.ui_top = bus
		ga:register_ui_group( bus, "top" )
		
	bus = self:define_monitoring( b_monitor_pass, monitor_m_nb )
		busses.monitoring = bus
		ga:register_ui_group( bus, "top" )
		bus:set_method( "draw_after",	self, "__draw_gp_after" )
	
	bus = ga:get_bus_wiz()
	bus:push()
		bus:add_bu( BU:create_window_center( "GP", {-.5,-.125, 1,1}, self:define_ui_gp() ) )
		ga.bu_doc			= bus:create_add_bu( BU_DOC, 		"Doc",	{0, 0,		4, 3} )		:set_text_nice()
		ga.bu_create_meu	= bus:create_add_bu( BU_CREATE_MEU, "Meu",	{0, 0,		4, 3} )		:set_text_nice()
		self.bu_bank		= bus:create_add_bu( BU_BANK,		"BANK", {0, 0,		4, 2.25} )	:set_text_nice()
--		self.bu_bank = bus:add_wiz_bank(	{0, 0,		4, 2.25} )
	bus:pop()

	ga:set_ui_group_active( true )


	self:define_menu()
end

function GP:get_xy_new()
	local bu = self.ui.bu_unused
	local x,y = bu:convert_xy_local_to_top( 0,-1 )
	return x,y
end

