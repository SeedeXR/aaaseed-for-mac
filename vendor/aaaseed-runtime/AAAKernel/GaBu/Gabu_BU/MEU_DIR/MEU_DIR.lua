if CLASS.DECLARE( "MEU_DIR_BASE", MEU	) then	--todo mean it is a recursive MEU
	MEU_DIR_BASE:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
									"is the base class for all the recursive MEU" )
	MEU_DIR_BASE.Y_DIR_TOP   	= .435
	MEU_DIR_BASE.Y_DIR_BOTTOM	= -.3
	MEU_DIR_BASE.X_DIR_LEFT 	= -.275
	MEU_DIR_BASE.X_DIR_RIGHT	= -MEU_DIR_BASE.X_DIR_LEFT
end

if CLASS.DECLARE( "MEU_DIR",		MEU_DIR_BASE	) then
	MEU_DIR:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
									"A folder holding MEUs, act as a first level of encapsulation.",
									"used often to regroup Fbo, Video, Mires..."
								)
end
function MEU_DIR:define_meu_infos()
	return	{ author = "Mâa",
				tags = { "core" }
			}
end

--todo merge this with dir and add selection interface
if CLASS.DECLARE( "MEU_GRIDSEL",	MEU_DIR_BASE	) then
	MEU_GRIDSEL:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
										"Same as MEU DIR but with a 4x4 grid selector UI to select MUs"
								)
end
function MEU_GRIDSEL:define_meu_infos()
	return	{ author = "Mâa",
				tags = { "core", "VJ", "Unfinished" }
			}
end

function MEU_DIR_BASE:free()
	local mus = self:get_mus_down()
	if mus then
		mus:free()
	end
	
	oo.getsuper(MEU_DIR_BASE).free(self)
end

function MEU_DIR_BASE:get_preset_nb()	return 6	end

function MEU_DIR_BASE:__define_ui_generic( b_empty, b_restore, b_ab )
	local ui = self.ui
	local mus = self:get_mus_down()
	local bu
	--aaa.box_good( "mus is "..mus.." with bus "..bus_cur )

	mus:add_define( bus_cur )
	bus_cur:set_method( "draw_before" ,self ,"draw_before_bus"	)
	bus_cur:set_method( "draw_after"  ,self ,"draw_after_bus"	)

	bus_cur:set_bu_pos_load_save( true )
	-- if mus dir stuff change bus will follow
	bus_cur.get_dir = function() return mus:get_dir_inst() end
	-- was: bus_cur:set_dir( mus:get_dir_inst() )


	bus_cur:set_constraint( "none" ) 

	local s = 1 / 20
	local function add_but( x,y, text )
		local bu = bus_cur:add_button( text, {x,y, s,s} )
		bu:set_pos_load_save(false)
		bu:set_value_load_save(true)
		return bu
	end

	if b_ab then
		bu = add_but( -.5 + s * .5, self.Y_DIR_TOP - s * .5, "A" )
			bu:set_text_xy( 0,0 )
			ui.bu_on_a = bu

		bu = add_but(  .5 - s * .5, self.Y_DIR_TOP - s * .5, "B" )
			bu:set_text_xy( -2,0 )
			ui.bu_on_b = bu
	end

	if b_restore then
		bu = bus_cur:add_button( "Restore", {-0.05, -.4 + s * .5 - .005, s,s} )
			:set_text_color( "inactive" )
			bu:set_active( false )
			bu:set_pos_load_save(false)
			bu:set_value_load_save(true)
			ui.bu_restore = bu
	end
	
	if b_empty then
		local SY = .8
		local SX = SY * 3
		bu = self:add_trig_method(	{8,0,	 SX,SY}, "Empty", self, "empty_trash" )
		bu:set_text_xy(-.4)
	end
end

--hack kind of a hack to for MU in
-- function MEU_DIR_BASE:set_focus( b_flatland_force )
-- 	--todo write it cleaner
-- 	-- local mus = self:get_mus_down()
-- 	-- local mu_nb = #(mus.__mu_down)
-- 	-- for i = mu_nb, 1, -1 do
-- 	-- 	local mu = mus.__mu_down[i]
-- 	-- 	local x,y = mu:get_xy()
-- 	-- 	x,y = mu:__constraint_inside_at_edge( x,y, 1,1 )
-- 	-- 	mu:set_xy( x,y )
-- 	-- end
	
-- 	oo.getsuper(MEU_DIR_BASE)( self, b_flatland_force )
-- end

function MEU_DIR_BASE:define_ui()
	local b_empty, b_restore, b_ab = false, true, false

	local b_trash = string.lower( self:get_inst_key() ) == "trash"
	local b_proto = string.lower( self:get_inst_key() ) == "protos"

	if b_proto or b_trash then
		b_restore = false
	else
		b_ab = true
	end

	if b_trash then
		b_empty = true
	end

	self:__define_ui_generic( b_empty, b_restore, b_ab )
end

function MEU_GRIDSEL:define_ui()
	local S = 16 / 5 * 4
	local bu = self:add_selector(	{1,1,	S,S}, "grid_sel")
	bu:set_nb(4, 4)

	--bu:set_target_param( par )
	bu:set_text_draw(false)
	bu:set_draw_grid_line(true)
	bu:set_move_to_front(false)
	--bu:set_color_back( { 0, .125, .125, 1 } )
	--bu:set_color_sel( { 0, .25, 0, 1 } )
	bu:set_pos_load_save(false)
	self.ui.bu_grid_sel = bu

	bu = self:add_button( {14,1, 1,1}, "Sel", self, "b_sel", true )
	bu:set_pos_load_save(false)

	self:__define_ui_generic( false, true, false )
end
--todo deal with this
function MEU_DIR_BASE:empty_trash()
	self:get_mus_down():free_mu_all()
end

local L = -.35
local S = .11
local YB = -.32
local YT = .15

function MEU_DIR_BASE:draw_icon_common()
	gol.set_line_width( BU.__draw_text_line_width )

	--	BOX
	aaa.draw_rect_line(L - S, YB, L + S, YT)
	--	HANDLE
	local X = L - S
	local DY = .12
	local DX = .02
	gol.draw_line_strip_2d(X, YT, X + DX, YT + DY, X + 4 * DX, YT + DY, X + 5 * DX, YT)
end

function MEU_DIR_BASE:draw_icon()
	self:draw_icon_common()
	--	BOX
	aaa.draw_rect(L - S, YB, L + S, YT)
	gol.color_white()
end

function MEU_GRIDSEL:draw_icon()
	self:draw_icon_common()
	local y = (YT + YB) * .5
	aaa.draw_rect(L, YB, L + S, y)
	gol.color_white()
end

function MEU_DIR_BASE:init()
	local name = self:get_name()
	local dir = MUS.get_cur():get_dir_inst() .. name .. "/"
	--aaa.box_good( "dir is "..dir )
	local mus = MUS:create( name, false, dir )
	mus:set_mu_sxy_def( MUS.__mu_def_sx*.5, MUS.__mu_def_sy*.5 )
	self:add_mus_down( mus )
	mus:__set_up( self )		--not done in previous call as it is done in GP

	self.__rect_sel	= {}
end

function MEU_DIR_BASE:init_mu(mu)
	if not self:is_proto() then
		mu:set_drop_receiver(self)
	end
end

function MEU_DIR_BASE:init_rect_sel_def()
	local rect = self.__rect_sel
	rect.l = -.5
	rect.r =  .5
	rect.b = -.5
	rect.t =  .5
	return rect
end
function MEU_DIR_BASE:update_rect_sel()
	local ui = self.ui
	local bu_on_a = ui.bu_on_a
	local rect = self:init_rect_sel_def()
	if bu_on_a then
		rect.l = bu_on_a:get_value_as_bool() and -.5 or self.X_DIR_LEFT
		rect.r = ui.bu_on_b:get_value_as_bool() and .5 or self.X_DIR_RIGHT
		rect.b = self.Y_DIR_BOTTOM
		rect.t = self.Y_DIR_TOP
	end
end
function MEU_DIR_BASE:update_ui()
	-- we do this in case mu get outside of rectangle (bug) so it is still possible to access it
	-- we do it only when UI is displayed and BU_MEU is touched to avoid doing it all the time
	-- tyhis compensate a bug but don't solve it
	local ui_slot = self:get_ui_slot()
	--self:print( "ui_slot is "..ui_slot ) 
	if ui_slot then
		local bu_meu = GP.cur:get_ui_slot_bu_meu( ui_slot )
		--self:print( "bu_meu is "..bu_meu )
		if bu_meu:is_contact() then
			--self:print( "contat so we check dir for mu outside" )
			local mus = self:get_mus_down()
			mus:__force_mu_in_rect( {l=-.5,b=-.5, r=.5,t=.5} )
		end
	end

	oo.getsuper(MEU_DIR_BASE).update_ui(self)
end

function MEU_GRIDSEL:update_ui()
	local b = self.b_sel
	self.ui.bu_grid_sel:set_visible(b)
	self.ui.bu_grid_sel:set_active(b)

	--self:print( "MEU_DIR_BASE:draw()" )
	oo.getsuper(MEU_GRIDSEL).update_ui(self)
end
function MEU_GRIDSEL:update_rect_sel()
	if not self.b_sel then
		MEU_DIR.update_rect_sel(self)
		return
	end
	local dst = self:init_rect_sel_def()
	local bu = self:get_bu_by_key("grid_sel")
	local src = bu:get_item_rect()
	dst.l, dst.b = bu:convert_xy_local_to_bus_up( src.l, src.b )
	dst.r, dst.t = bu:convert_xy_local_to_bus_up( src.r, src.t )
end
-- function MEU_DIR_BASE:set_alpha(alpha)
-- 	self.__alpha_meu = alpha
-- 	local mus = self:get_mus_down()
-- 	if mus then
-- 		mus:set_alpha(alpha)
-- 	end
-- end

--todo this a render infact
function MEU_DIR_BASE:draw()
	--self:print( "MEU_DIR_BASE:draw()" )
	oo.getsuper(MEU_DIR_BASE).draw(self)

	local mus = self:get_mus_down()
	mus:begin_render()
	--self:print( "MEU_DIR_BASE:draw() mus is "..mus )
	if not mus:build_mu_to_render_from_rect( {l=-.5, r=.5, b=-.5, t=.5} ) then
		return
	end

	self:update_rect_sel()

	mus:do_render_pass_rect( self.__rect_sel, self:get_alpha(), "dir", 9 )

	--self:print( "MEU_DIR_BASE:draw()" )
	--bu_restore wll be nil for trashed or proto but they should not be drawn
	if self.ui.bu_restore:get_value_as_bool() then
		local cur = MUS.get_cur()
		if cur then
			cur:restore_meu_last_rendered()
		end
	end
end

function MEU_GRIDSEL:draw()
	--self:print( self.b_sel )
	if not self.b_sel then
		MEU_DIR.draw(self)
		return
	end
	--self:print( "testing ---> "..self:get_bu_value( "grid_sel" ) )
	local bdd_before = aaa.bdd.get_cur()
	--self:print( "MEU_DIR_BASE:draw()" )
	oo.getsuper(MEU_GRIDSEL).draw(self)
	local bdd_after = aaa.bdd.get_cur()
	self.__bdd_drawn = (bdd_before == bdd_after) and nil or aaa.bdd.get_cur()
end

function MEU_GRIDSEL:get_bdd_drawn()
	return self.__bdd_drawn
end
function MEU_GRIDSEL:set_selector_value( id )
	self:set_bu_value( "grid_sel", id )
end

--todo 2023 November should be removed when dir are done with BU_RECT
function MEU_DIR_BASE:__draw_dir_rect( b, rect )
	if rect then
		--table.print( self, "GP" )
		local mus = self:get_mus_down()
		--table.print( mus, "mus" )
		local bus = mus and mus.__bus_mus
		local ox,oy
		if bus then	ox,oy = bus.__x_offset_exp,bus.__y_offset_exp
		else		ox,oy = 0,0
		end
		if b then
			BU:gol_green_back()
		else
			BU:gol_red_back()
		end
		aaa.draw_rect( rect.l+ox, rect.b+oy, rect.r+ox, rect.t+oy )
	end
end

function MEU_DIR_BASE:draw_ab()
	self:update_rect_sel()
	gol.color_white(.6)
	local r = self.__rect_sel
	gol.draw_lines_2d(
		-.5,				r.b,	.5,					r.b,
		-.5,				r.t,	.5,					r.t,
		self.X_DIR_LEFT,	r.b,	self.X_DIR_LEFT,	r.t,
		self.X_DIR_RIGHT,	r.b,	self.X_DIR_RIGHT,	r.t
	)
end
function MEU_DIR_BASE:draw_before_bus()
	GP.cur:draw_start()

	local ui = self.ui
	local bu_a = ui.bu_on_a
	local rect = {b = self.Y_DIR_BOTTOM, t = self.Y_DIR_TOP}
	if bu_a then
		rect.l = -.5
		rect.r = self.X_DIR_LEFT
		self:__draw_dir_rect( bu_a:get_value_as_bool(), rect )
		rect.l = self.X_DIR_RIGHT
		rect.r = .5
		self:__draw_dir_rect( ui.bu_on_b:get_value_as_bool(), rect )
	end
	rect.l = self.X_DIR_LEFT
	rect.r = self.X_DIR_RIGHT
	self:__draw_dir_rect( true, rect )

	self:draw_ab()
end

--[[
function MEU_GRIDSEL:draw_grid()
	gol.color_white( .6 )
	local S = .25
	for i=-.5+S,.5-S,S do
		aaa.draw_line( i, -.5, i, .5 )
		aaa.draw_line( -.5, i, .5, i )
	end
	for v=0,3 do
		local x = -.5+v*S
		for u=0,3 do
			local y = -.5+u*S
			if u <= v and ((u+v)%2 == 0) then
				gol.color_red( .2 )
			else
				gol.color_green( .1 )
			end
			aaa.draw_rect( x, y, x+S, y+S )
		end
	end
end
--]]

function MEU_GRIDSEL:draw_before_bus()
	oo.getsuper(MEU_GRIDSEL).draw_before_bus(self)
end

function MEU_DIR_BASE:draw_after_bus_begin()
	GP.cur:draw_start()
end
function MEU_DIR_BASE:draw_after_bus_central()
end
function MEU_DIR_BASE:draw_after_bus_end()
	self:get_mus_down():__draw_on_top( self:is_rendered_in_this_frame() )
end

function MEU_DIR_BASE:draw_after_bus()
	--self:draw_after_bus_begin()
	--self:draw_after_bus_central()
	self:draw_after_bus_end()
end

--we do this because with position of the bu the value are saved so saviong it in preset have no use
function MEU_DIR_BASE:store_preset( i, dirname )
	if i~=0 then
		MEU.store_preset( self, i, dirname )
	end
end

function MEU_DIR_BASE:save_to_dir_direct( dirname, b_complete )
	MEU.save_to_dir_direct( self, dirname, b_complete )
	local mus = self:get_mus_down()
	--todo this and this fns are unclear
	if mus then
		mus:save_to_dir( dirname, b_complete )
		self:save_bu_pos( dirname .. MUS:get_dir_MEU() ) 
	end
end

function MEU_DIR_BASE:save_to_dir( dirname, b_complete )
	if self.verbose >= 1 then
		aaa.print_method()
	end
	self:save_to_dir_direct( dirname, b_complete )
end

function MEU_DIR_BASE:change_filename( src_dir, dst_dir, b_verbose )
	aaa.print_fn()
	oo.getsuper(MEU_DIR_BASE).change_filename( self, src_dir, dst_dir, b_verbose )
	--local obj = self:__get_obj_main()
	--self:__change_obj_and_branched_filename( obj, src_dir, dst_dir, b_verbose )

	local mus = self:get_mus_down()
	mus:update_mu_path( dst_dir )
end