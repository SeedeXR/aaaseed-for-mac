local doc = MEU.doc
--	CAMERA
--
doc.get_camera_used = { "() return a reference to the c_seedcam set at the beginning of layers render (layes.draw_begin()).",
						"  so this fine to use it after draw_begin, if not you get the previous layers render camera used." }
function MEU:get_camera_used()
	local layers = self:get_layers()	--todo what happened when it is not the case
	--todoaqua refine for module
	return layers and aaa.layers.get_camera( layers )
end
doc.get_camera = { "() return a reference to the layers's c_seedcam object corresponding to index.",
						"  index is in the range [0,16]." }
function MEU:get_camera( index )
	local layers = self:get_layers()	--todo what happened when it is not the case
	--todoaqua refine for module
	return layers and aaa.layers.get_camera( layers, index )
end
function MEU:get_camera_sel()
	local bu = self.ui.bu_cam_sel
	if bu then	--protection in some initialisation scenarios
		local sel = bu:get_value()
		--todoaqua refine for module
		return self:get_camera( sel )
	end
end

function MEU:__update_camera_sel()
	aaa.print_fn()
	local ui = self.ui
	--self:print( ui.bu_cam_use:get_value() )
	local cam = self:get_camera_sel()
	--self:print( "__update_camera_sel -> "..sel.." "..cam )
	local r = param.get_ref( cam, "draw_axe" )
	param.set( r, ui.bu_cam_show:get_value() )
	param.set_save( r, false )

	local gp = app:get_gp()
	if gp.__b_rendering then
		self:print( "Is this should happen ?" )
	else
		if ui.bu_cam_use:get_value() > 0 then
			self:print( "Called become_ui camera" )
			aaa.obj.become_ui( cam )	
		end
	end
end
function MEU:__focus_camera()
	--aaa.print_method()
	local cam = self:get_camera_sel()
	--self:print( "__focus_camera -> "..sel.." "..cam )
	aaa.obj.set_focus_ui( cam )
end
function MEU:__set_ui_camera()
	--aaa.print_method()
	local cam = self:get_camera_sel()
	--self:print( "__set_ui_camera -> "..sel.." "..cam )
	if cam then	-- protect in some initialisation scenarios
		aaa.obj.become_ui( cam )
	end
end
MEU.doc.__camera_do_click_down = "(x,y) used to copy camera"
function MEU:__camera_do_click_down( x,y )
	local bu = self
	self = bu.__meu
	SELECTOR.do_click_down( bu, x,y )
	--aaa.print_fn()
	aaa.print( "CTRL "..aaa.keyboard.is_ctrl() )
	if aaa.keyboard.is_ctrl() then
		local src = MEU.__cam_last_picked
		if src then
			local sel = bu:get_value()
			--aaa.print( "sel "..sel )
			local dst = self:get_camera( sel )
			aaa.obj.set_param_from( dst, src )
		end
	elseif aaa.keyboard.is_shift() then
		local sel = bu:get_value()
		--aaa.print( "sel "..sel )
		local dst = self:get_camera( sel )
		param.flip( dst, "ui_lock" )
		--todoundo add the undo redo code
	end
end
doc.__camera_do_click_up = "( x,y ) used to copy camera"--	tab.add_param_size = ""
function MEU:__camera_do_click_up( x,y )
	local bu = self
	self = bu.__meu
	SELECTOR.do_click_up( bu, x,y )
	local sel = bu:get_value()
	--aaa.print( "Store cam "..sel.." ref for copy " )
	MEU.__cam_last_picked = self:get_camera( sel )
end

function MEU:__update_ui_cam()
	local ui = self.ui
	local val = ui.bu_cam_use:get_value()
	local bu_cam = ui.bu_cam_sel
	--self:print( "cam is "..b )
	local a = val>0
	bu_cam:set_color_back( a and "bui" or "none" )
	a = bu_cam:interpolate_alpha_bu( a )
	ui.bu_cam_show:set_alpha_bu( a )
end

local function draw_camera_items( self )
	local x,y
	local dx = self.__nb_u_over
	local dy = self.__nb_v_over
	local alpha = self:get_alpha_bu_to_draw() * .5
	gol.color_red( alpha )

	local meu = self.__meu
	for i = 1,self:get_nb() do
		local cam = meu:get_camera( i-1 )
		if param.get_bool( cam, "ui_lock" ) then
			x,y = self:get_item_lb(i)
			aaa.draw_rect( x,y, x+dx,y+dy )
		end
	end
	gol.color_white( alpha )
	local i_sel = self:get_value() + 1
	x,y = self:get_item_lb(i_sel)
	aaa.draw_rect( x,y, x+dx,y+dy )
end

local c = -.25
local D = .5
local d = .1
local pts_r_active = { -.42,c,	.42,c,	c+D,c,c+D-d,c+d, c+D,c,c+D-d,c-d }
local pts_r = { c,c,	c+D,c,	c+D,c,c+D-d,c+d, c+D,c,c+D-d,c-d }
local pts_g_active = { c,-.42,	c,.42,	c,c+D,c-d,c+D-d, c,c+D,c+d,c+D-d }
local pts_g = { c,c,	c,c+D,	c,c+D,c-d,c+D-d, c,c+D,c+d,c+D-d }
function BU:draw_icon_axe()
	local b = self:get_value_as_bool() 
	local alpha = self:get_alpha_bu_to_draw()
	if b then
        gol.color_red(alpha)
		--local tri_r = { c+D,c, c+D-d,c+d, c+D-d,c-d }
		--gol.draw_triangles_2d(	tri_r	)
		gol.draw_lines_2d( pts_r_active	)
		gol.color_green(alpha)
		gol.draw_lines_2d( pts_g_active	)
		gol.color_blue(alpha)
	else
		gol.color_white(alpha)
		gol.draw_lines_2d(	pts_r	)
		gol.draw_lines_2d(	pts_g	)
	end
	aaa.draw_rect( c-d,c-d, c+d,c+d )
end


function MEU:add_camera( rect, nb, b_force_no )	
	nb = nb or 8
	local b_line_two = nb>=12

	local SXB = 1
	local sy_def = 1
	local SYB = 1
	if b_line_two then
		sy_def = sy_def*1.8
	end
	local ix,iy, sx,sy = self:unpack_rect_using_def( rect, {9,1, 8,sy_def} )

	SYB = math.min( sy, SYB )

	local fy_text = .8
	local y_text = fy_text/6

	local bu
	local par


	--	this one define if we use a current camera or define our own (already in AAA flaatland infact)
	bu = self:add_button(	{ix,iy,	SXB,SYB}, "cam_use" )
		--bu:set_nb( 1, 3 )
		bu:set_menu( { "No", "Once", "Own" } )
		--bu.__bu_menu:set_text( "Cam" )
		bu:set_text_selector( true )
		bu:set_text_inside( true )
		bu:set_text_fxy( 1, fy_text )
		bu:set_text_y( y_text )
		
		--todo deal with MEU module
		par = param.get_ref( self:get_layers(), "use_camera" )
		bu:set_target_param( par )
		bu:set_draw_by_value( 0, "false" )
	
		local sel = bu:get_selector()
		sel:set_min_max_strict( true )
		sel:set_min_max( 0,2 )
		--bu:set_text_draw( false )
		if b_force_no then
			bu:set_value( 0 )
		end
		bu:set_method_on_value_change( self, "__update_camera_sel" )
		bu:set_preset_use( false )	--todo 2025 July solve this and next

		self.ui.bu_cam_use = bu

	--	this one define if we show the current axis  (already in AAA flaatland infact)
	bu = self:add_button(	{ix + sx-SXB,iy, SXB,SYB}, "cam_axe_show" )
		bu:set_text( "Axe" )
		bu:set_text_inside( true )
		bu:set_text_fxy( 1, fy_text )
		bu:set_text_y( y_text )

--		bu:set_text_inside( "Axe" )
		bu:set_method_on_value_change( self, "__update_camera_sel" )
		bu:set_preset_use( false )	--todo 2025 July solve this and next\
		bu.draw_fore = BU.draw_icon_axe
		self.ui.bu_cam_show = bu

	--	we choose one cam among 8
	bu = self:add_selector(	{ix + SXB,iy,	sx-2*SXB,sy}, "cam_index" )
		--todo deal with MEU module
		par = param.get_ref( self:get_layers(), "camera_index_ui" )
		bu:set_target_param( par )

		--have to be after set_target_param
		if b_line_two then
			bu:set_nb_min_0( nb/2, 2 )
		else
			bu:set_nb_min_0( nb )
		end
		bu:set_item_text_from_nb_minus_1( 2 )
		bu:disable_mobile()

		bu:set_method_on_value_change(	self, "__update_camera_sel" )	
		bu:set_method_on_click(			self, "__set_ui_camera" )
		bu:set_method_on_click_double(	self, "__focus_camera" )
		self.ui.bu_cam_sel = bu

		bu.do_click_down 	= self.__camera_do_click_down
		bu.do_click_up		= self.__camera_do_click_up
		bu.draw_items		= draw_camera_items
		bu.__meu = self

		--bu:print( "max is "..bu:get_max() )
		self:register_update_ui( "__update_ui_cam" )
	return bu
end

function MEU:set_screen_viewport( value )
	local cam = self:get_camera_used()
--hackjan2015
-- do we need a special mecanism
	if cam then
		aaa.screen.set_camera( cam, value )	--todo better than ( MEU.__screen_offset or 0 ) )
	end
end
function MEU:update_screen_viewport_from_bu( bu )
	self:set_screen_viewport( bu:get_value() )
end


-- function MEU:get_screen_viewport_sxy()
-- 	local id = self.ui.bu_screen_id:get_value()
-- 	local l,b, r,t = aaa.screen.get_viewport_cano(id)
-- 	local sx,sy = r-l, b-t
-- 	if sx~=0 and sy~=0 then
-- 		return sx,sy
-- 	end
-- end
-- function MEU:get_screen_viewport_ratio_x()
-- 	local sx,sy = self:get_screen_viewport_sxy()
-- 	if sy then
-- 		return sx/sy
-- 	end
-- end
-- function MEU:get_screen_viewport_ratio_y()
-- 	local sx,sy = self:get_screen_viewport_sxy()
-- 	if sy then
-- 		return sy/sx
-- 	end
-- end
function MEU:add_screen_viewport( ix, iy )
	ix = ix or 10
	iy = iy or 2

	local bu = self:add_selector(	{ix,iy,	6,1}, "screen_dst" )
		bu:set_nb_min_0( 7 )
		bu:set_item_text( 1, "Full" )
		bu:set_item_text_from_nb_minus_1( 2 )
		bu:disable_mobile()
		bu:set_method_on_value_change( self, "update_screen_viewport_from_bu", bu )
		self.ui.bu_screen_id = bu
end
