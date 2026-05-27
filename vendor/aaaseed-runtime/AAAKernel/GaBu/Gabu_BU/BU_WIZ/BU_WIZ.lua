--
--BU_WIZ
--
if CLASS.DECLARE( "BU_WIZ", BU, {
--	ref = {},
--	__edit = nil,
	} ) then
	BU_WIZ:set_class_status_doc(	CLASS.STATUS.GABU,
									"this is a dummy class for \"BU Vizard\" all derived class should become BU or BUTTON (2023 July Mâa)" )
	-- local ref = BU_WIZ.ref
	-- local o = aaa.get_caller()
	-- o = aaa.obj.get_root( o )
	-- ref.layers = o
	-- aaa.obj.update_then_draw( ref.layers )	--make sure it is updated so we have a color object
end

--[[
--nou useful
function BU_WIZ:update()
	oo.getsuper(BU_WIZ).update(self)
end
--]]


--
--	WIZ ELEMENT
--
local function draw_text_on_off( self )
	if self.verbose >= 3 then aaa.print_method() end

	--if self:is_text_draw() then
	--	aaa.print_fn()
		local text = self:get_text() 
		if self:get_value_as_bool()  then
			gol.color_green( self:get_alpha_bu_to_draw() )
		else
			gol.color_red( self:get_alpha_bu_to_draw() )
		end
		local SY = .4
		local SX = .5
		self:set_text_nice() -- force text nice
		self:draw_text_lrbt( text, -SX,SX, -SY,SY )
	--end
end

local function init_instance_wiz_button( self )
	self.draw = draw_text_on_off
	--self:set_text_draw_always( false )
	self:set_text_draw( false )
	return self
end

--todo there is another one in BU_TEXT
--todo check what to do with set_wiz
function BU:__init_new_wiz( post )
	self:set_text_color( "info" )
	self:set_mobile(false)
	--self:set_ui_active( false )
	self:set_ui_top_size( true )
--	self:set_back_alpha( 0 )

	return self
end
function BU.__create_wiz( post, rect )
	local class = _G[ "BU_"..post ]
	if rect then
		rect[3] = rect[3] or .25
		rect[4] = rect[4] or .25
	end
	local self = class:create_instance( "wiz_"..post, rect )
	self:__init_new_wiz( post )
	return self
end
function BU.__create_wiz_text( post, rect, b_save_value )
	local self = BU.__create_wiz( post, rect )
	self:set_gdebug( false )
	self:set_text_nice()
	self:set_text_draw_always( true )
	self.__text_len = 1
	self:set_text_color( "info" )
	self:set_value_load_save( b_save_value or false )
	return self
end
function BU.__create_wiz_button( post, rect )
	local self = BU.__create_wiz( post, rect )
	self:set_gdebug( false )
	self:set_text_nice()
	self:set_ui_active( true )
	
	init_instance_wiz_button(self)

	return self
end

--
--	ALIVE
--
if CLASS.DECLARE( "BU_ALIVE", BU_WIZ ) then
	BU_ALIVE:set_class_status_doc( CLASS.STATUS.GABU, "show if current app is rendering (F3 key to toggle)" )
end
function BU_ALIVE:create( rect )
	local self = BU.__create_wiz( "ALIVE", rect )
	self:set_back_alpha( 0 )
	self.__scale_min = .1
	self.phase = 0
	return self 
end
function BU_ALIVE:draw()
	local b_alive = app:get_gp():is_render()

	local ph = self.phase
	local sp = 1
	if b_alive then
		ph = ph + aaa.time.dt_real * sp
		self.phase = ph
	end
	local s1 = math.sin( ph       * math.pi2 )
	local s2 = math.sin( (ph+1/3) * math.pi2 )
	local s3 = math.sin( (ph+2/3) * math.pi2 )


	if b_alive then
		local alpha = 1.
		local s = .1
		local nb = 3
		local d = 1

		gol.set_line_width( 4 )
		gol.color_red( alpha )
		aaa.draw_circle_arc_axe_z( 0, 0, 0, d, -ph, s, nb )
		gol.color_green( alpha )
		aaa.draw_circle_arc_axe_z( 0, 0, 0, d, -ph + .33333, s, nb )
		gol.color_blue( alpha )
		aaa.draw_circle_arc_axe_z( 0, 0, 0, d, -ph + .66666, s, nb )
	end

---[[
	local alpha = .5
	local s = .7
	local D = .15
	local nb = 36

	--gol.set_default()
	gol.set_cull_back()
	--gol.disable_lighting()
	--gol.set_texture_dim(0)
	gol.set_blend_max()


	gol.rotate_z( ph )
		
		if b_alive then	gol.color_red( alpha )
		else			gol.color( 0.6, alpha )
		end
		aaa.draw_disk_axe_z( D, 0,				0, s * (s1*.3+.7), nb )

		if b_alive then	gol.color_green( alpha )
		else			gol.color( 0.8, alpha )
		end
		aaa.draw_disk_axe_z( -D*.5, D*0.866,	0, s * (s2*.3+.7), nb )

		if b_alive then	gol.color_blue( alpha )
		else			gol.color( 0.4, alpha )
		end
		aaa.draw_disk_axe_z( -D*.5, -D*0.866,	0, s * (s3*.3+.7), nb )

	gol.rotate_z( -ph )

	gol.set_blend_add()
--]]
end
function BU_ALIVE:do_click_down( x, y )
	app:get_gp():flip_render()
	oo.getsuper(BU_ALIVE).do_click_down(self, x, y)
end 

--
-- 	EYE
--
if CLASS.DECLARE( "BU_EYE", BU_WIZ ) then
	BU_EYE:set_class_status_doc( CLASS.STATUS.GABU, "the eye(s) follow the mouse cursor" )
end
function BU_EYE:create( rect, b_one_eye )
	local self = BU.__create_wiz( "EYE", rect )
	self.b_one_eye = b_one_eye
	self.eye_color = { 1,1,1, 1 }
	self.__scale_eye_y_base = 2	--maa I compensate quicky to have a smaller frame
	self.__scale_eye_y = self.__scale_eye_y_base
	return self
end
-- red eye when touched
function BU_EYE:do_click_down( x, y )
	self:print( "Aie" )
	self.eye_color = { 1,.1,.1, 1 }
	oo.getsuper(BU_EYE).do_click_down(self, x, y)
end
function BU_EYE:do_click_up( x, y )
	self.eye_color = { 1,1,1,1 }
	oo.getsuper(BU_EYE).do_click_up(self, x, y)
end
-- deal with time
function BU_EYE:update()
	local dt = aaa.time.dt_real
	self.eye_color_use = self.eye_color
	if self:get_contact_nb() == 0 then
		-- if self.__scale_eye_y >= 0 and math.random() < dt*1. then
		-- 	self.__scale_eye_y = 0		
		-- --elseif aaa.math.get_turbulence( 0,0,aaa.time.t_real, 1, 2 ) > -.2 then
		-- else
		math.randomseed( aaa.time.t_real )
		local t_closed = self.time_closed
		if t_closed then
			t_closed = t_closed + dt
			if t_closed > .1 then
				t_closed = nil
			end
			self.time_closed = t_closed
		end
		if t_closed then
			self.eye_color_use = {	0,0,0, 1 }
		else
			self.eye_color_use = self.eye_color
			if math.random() > 1-dt*.5 then
				self.time_closed = 0
			end
		end
	end
--	self.__scale_eye_y = 2
	oo.getsuper(BU_EYE).update(self)
end

function BU_EYE:draw()
	--self:print( "draw() ")
	--gol.reset()
	--gol.set_default()

	gol.set_cull_back()
--	gol.set_front_fill()
--	gol.set_front_line()
--	aaa.draw_rect( -1, -1, 0,0 )

	local size	= .45
	local x1	= -.5 + size * .5
	local x2	= .5 - size * .5
	local max_move_pupil = 0.175

	-- if one eye, center the eye and make it bigger
	if self.b_one_eye then
		size	= size * 2
		x1		= 0
		max_move_pupil = max_move_pupil * 2
	end

	-- Get mouse position
	local mouse_x,mouse_y = aaa.mouse.get_xy_render()

	-- Convert mouse position to the main coordinate
	mouse_x,mouse_y = EVENT.transform_move_coor( mouse_x,mouse_y )

	-- Set the new mouse position to the local bu eye
	mouse_x,mouse_y = self:convert_xy_bus_up_to_local( mouse_x,mouse_y )
	mouse_y = mouse_y / self.__scale_eye_y_base
	-- debug, draw a yellow circle to the mouse position
	--[[
	gol.color_yellow()
	aaa.draw_disk_axe_z( mouse_pos_x, mouse_pos_y, 0, 0.1, 24)
	self:print(mouse_pos_x.." "..mouse_pos_y)
	--]]

	-- get the vector
	local v_left	= { mouse_x - x1, mouse_y - 0 }
	local v_right	= { mouse_x - x2, mouse_y - 0 }

	-- normalize vector
	local norme_left	= V2.norm( v_left )
	local norme_right	= V2.norm( v_right )

	--right eye only if two eyes
	gol.push_matrix()
		gol.scale( 1, self.__scale_eye_y, 1 )	--maa say this is weird

		local function draw_pupil( x, y, vector )
			-- pupil's position
			local val_x = x + vector[1] * max_move_pupil
			local val_y = y + vector[2] * max_move_pupil

			local tmp = (size - size * 0.35) * .5

			local lim_x_left = x - tmp
			local lim_x_right = x + tmp

			local lim_y = tmp

			gol.color_black( 1 )
			-- if mouse is inside the eye
			if mouse_x < lim_x_right and lim_x_left < mouse_x and mouse_y < lim_y and -lim_y < mouse_y then
				aaa.draw_disk_axe_z( mouse_x,mouse_y, 0, size * 0.35, 24)
			else
				aaa.draw_disk_axe_z( val_x, val_y, 0, size * 0.35, 24)
			end
		end

		local function draw_eye( x, vect, norme )
			gol.color( self.eye_color_use )
			aaa.draw_disk_axe_z( x, 0, 0, size, 24 )
			if norme ~= 0 then
				vect[1] = vect[1] / norme
				vect[2] = vect[2] / norme
			end
			draw_pupil( x, 0, vect )
		end

		draw_eye( x1, v_left, norme_left )
		--right eye only if two eye
		if not self.b_one_eye then
			draw_eye( x2, v_right, norme_right )
		end
	gol.pop_matrix()
end

--
-- 	PB
--
if CLASS.DECLARE( "BU_PB", BU_WIZ ) then
	BU_PB:set_class_status_doc( CLASS.STATUS.GABU,
								"display a changing red cercle, used when MEU's define_ui() fail" )	
end

function BU_PB:create( rect )
	--aaa.debug.print_traceback()
	--self:box_debug( "Houla" )
	local self = BU.__create_wiz( "PB", rect )
	self:set_ui_active( false )
	return self
end
function BU_PB:draw()
	--self:print( "draw() ")
	--gol.reset()
	--gol.set_default()

--	gol.set_cull_back()
--	gol.set_front_fill()
--	gol.set_front_line()
--	aaa.draw_rect( -1, -1, 0,0 )
	local t = aaa.time.t_real
	gol.color_red( .25 )
--	local s = math.sin( t * 3.14159 * .25 ) *.5 + .5
	local s = 1 - math.fmod( t*.5, 1. )
	--s = math.pow( s, .5 )
	--s = interpolate( .0, 1, s )
	aaa.draw_disk_axe_z( 0,0, s,s, 44 )
end

--
-- 	MEMORY
--
if CLASS.DECLARE( "BU_MEMORY", BU_WIZ ) then
	BU_MEMORY:set_class_status_doc( CLASS.STATUS.GABUZOMEU, "Show the memory used by the C executable" )	
end
function BU_MEMORY:create( rect )
	local self = BU.__create_wiz( "MEMORY", rect )
	self.ref = {}
	self.__memory_max 		= 2500
	self.__memory_max_over	= 1. / self.__memory_max
	self.__memory_green 	= 1800
	self.__memory_red 		= 2100
	self.__memory_over		= 1. / (self.__memory_red - self.__memory_green)
	local ref = self.ref
	ref.mem			= param.get_ref( aaa.ref.pref, "working_set_size_MB" )
	ref.mem_peak	= param.get_ref( aaa.ref.pref, "working_set_size_peak_MB" )
	--self:set_border( true )
	return self
end

function BU_MEMORY:set_color( val )
	if val < self.__memory_green then
		gol.color_green();
	elseif val > self.__memory_red then
		gol.color_red();
	else
		val = (val - self.__memory_green) * self.__memory_over
		gol.color( val, 1-val ,0, 1 );
	end
end
function BU_MEMORY:draw()
	gol.set_cull_back()
--	gol.set_front_fill()
--	gol.set_back_point()
--	aaa.draw_rect( -1, -1, -,0 )
	gol.color_cyan()
	aaa.draw_rect_line_size()

	local ref = self.ref
	local val = param.get( ref.mem )
	self:set_color( val )
	local val_clamped = clamp_01(val * self.__memory_max_over )
	aaa.draw_rect( -.5, -.5, -.5 + val_clamped, .5 )

	local peak = param.get( ref.mem_peak )
	self:set_color( peak )
	peak = -.5 + clamp_01( peak * self.__memory_max_over )
	aaa.draw_line( peak, -.5, peak, .5 )

	gol.color_yellow( )
	if val > 1000 then
		val = math.floor( val/1000 ) .. " " .. aaa.format.int_to_char3(val % 1000)
	end
	BU:draw_text_nice_shadow( val .."MB", -.5,-.5,0, .3,1, "left" )
end

--
--  BU_SHOW (DEBUG WINDOW)
--
if CLASS.DECLARE( "BU_SHOW", BU_WIZ ) then
	BU_SHOW:set_class_status_doc( CLASS.STATUS.GABU, "Window used to show values, used for debug" )	
end
function BU_SHOW:create( rect )
	local self = BU.__create_wiz( "SHOW", rect )
	self.__alive_time = 10
	self.__tab_show = {}
--	self.draw_border_line = BU_SHOW.draw_border_line
--	self:set_color_back( { 0, 1, 1, .25 } )
--	self:set_ui_active( false )
	--hack deal with several
	self:set_mobile( false )
	aaa.debug.__bu_show = self
	return self
end

function BU_SHOW:add_element( elt_type, value, str, caller, alive_time )
	-- if elt_type == "error" then
	-- 	aaa.print_fn()
	-- end

	if str == nil then
		str = ""
	end

	local id
	if caller == nil then
		id = str
	else
		id = caller.."/"..str
	end

	str = id.." : "..value
	--aaa.print_fn()

	if elt_type == "error" then
		alive_time = alive_time or .1
		id = str
	elseif elt_type == "warning" then
		alive_time = alive_time or 5.
		id = str
	end

	local t = self.__tab_show
	local b_add = true
	local elt = t[id] 
	if elt ~= nil then
		b_add = false
		if elt.str ~= str then
			elt.str = str
			elt.alive_time = alive_time and alive_time or self.__alive_time
		end
	end

	if b_add then
		elt = {}
		elt.elt_type = elt_type
		elt.str = str
		elt.alive_time = alive_time and alive_time or self.__alive_time
		t[id] = elt
	end
end

function BU_SHOW:update()
	local t = self.__tab_show

	for key,elt in PAIRS(t) do
		local alive = elt.alive_time - aaa.time.dt_real
		if alive < 0 and (elt.alive_time ~= 0) then -- we want it at least once 
			t[key] = nil
		else
			elt.alive_time = alive
		end
	end

	oo.getsuper(BU_SHOW).update(self)
end

--BU_SHOW.draw_border_line = nil
-- function BU_SHOW:draw_border_line()
-- 	gol.color( .5,1,.5, 1 )
-- 	--struct detected with it
-- 	--todo do it
-- 	--gol.set_line_width( border_line_size )
-- 	gol.set_line_width( 2 )
-- 	gol.draw_lines_2d(	-.5, -.5,	.5, -.5,
-- 						-.5, .5,	-.5, -.5,
-- 						.5, .5,		.5, -.5
-- 					)
-- 	oo.getsuper(BU_SHOW).draw_fore( self )
-- end

function BU_SHOW:draw()
	self:draw_back()

	local l = -.49
	local r = .49
	local b = -.45
	local t

	local i = 1
	local b_mark
	for _, elt in pairs_sorted(self.__tab_show) do
		--self:print( " "..elt.str )
		local elt_type = elt.elt_type
		if elt_type == "error" then
			gol.color_red( .6 + wrap_01(aaa.time.t_real * 4.) * .4 )
			b_mark = true
		elseif elt_type == "warning" then
			gol.color_orange( .6 + wrap_01(aaa.time.t_real * 4.) * .4 )
			b_mark = true
		else
			gol.color_cyan()
		end
		t = -.45+i*.17
		self:draw_text_lrbt( elt.str, l,r, b,t )
		b = t
		i = i+1
	end

	if b_mark then
		gol.color_red( .2 + wrap_01(aaa.time.t_real * 4.) * .8 )
		self:draw_border_line( 3 )
	end
end

--
--	CAM
--
if CLASS.DECLARE( "BU_CAM", BUTTON ) then
	BU_CAM:set_class_status_doc( CLASS.STATUS.GABU, "Show and toggle if camera can be edited (Alt F4 toggle Too)" )

--	BU_CAM.init_instance = init_instance_test
end

function BU_CAM:create( rect )
	local self = BU.__create_wiz_button( "CAM", rect )
	--self = BU_CAM:create_instance( "CAM" )
	--table.print( self, "BU_CAM", 2 )
	--self:box_debug( "init_instance()" )25)
	self:set_target_param( aaa.ref.camera_edit )
	self:set_text_xy_fxy( .3,0, .7,1 )
	self:set_text_align_x( "left" )
	return self
end
function BU_CAM:get_text()
	--hack perhaps should be done somewhere else
	self.b_can = false

	local b = self:get_value_as_bool()
	if not b then
		return "LOCKED"
	end

	--was (2025 June) local cam = aaa.camera.get_ui_or_find()
	local cam = aaa.camera.get_ui_or_find()
	if not cam then
		local gp = GP.cur
		local i = gp:get_ui_slot_def()
		local meu = gp:get_ui_slot_meu(i)
		if meu then
			self:print_debug( "No ui cam, setting it using Ui"..i )
			meu:__set_ui_camera()
			cam = aaa.camera.get_ui_or_find()
		end		
	end
	if not cam then
		return "No Camera"
	end

	--we have a camera
	local str_debug = " cam is nil"
	if cam then
		str_debug = aaa.obj.get_filename(cam)
		if app then
			if str_debug then
				str_debug = app:make_relative_path( str_debug )
			else
				str_debug = "No filename"
			end
		else
			str_debug = "app is nil"
		end
	end	
	aaa.show( str_debug, "CAM" )

	if param.get_bool( cam, "ui_lock" ) then
		return "Cam Lock"
	end

	self.b_can = true
	return "Free"
end

function BU_CAM:update()
	-- when cam is locked the bu manifest itself
	--self:print( aaa.mouse.is_but_middle().." "..(not self:get_value_as_bool()).." "..(aaa.mouse.is_but_middle() and not self:get_value_as_bool() ) )
	if ( not self.b_can ) and aaa.mouse.is_but_middle() then
		self:begin_fx( 8, 1 )
	end
	return oo.getsuper(BU_CAM).update(self)
end
function BU_CAM:draw_fore()
	gol.set_line_width( BU.__draw_text_line_width * 1 ) 
	MEU:draw_icon_camera()
	oo.getsuper(BU_CAM).draw_fore(self)
end

--
--	SEND
--
if CLASS.DECLARE( "BU_SEND", BUTTON ) then
	BU_SEND:set_class_status_doc( CLASS.STATUS.GABU,
									"Show and toggle if Bu interaction are sent to other machines using the network (key u toggle too)" )

--	BU_SEND.init_instance = init_instance_test
end
function BU_SEND:create( rect )
	local self = BU.__create_wiz_button( "SEND", rect )
	self:set_target_lua( app, "__b_bu_send" )
	return self
end
function BU_SEND:get_text()
	return self:get_value_as_bool() and "SEND" or "NO SEND"
end

--todo extend that BU_TEXT is now a BUI with target ... to have a standard update mecanism
--	FPS
--
if CLASS.DECLARE( "BU_FPS", BU_TEXT ) then
	BU_FPS:set_class_status_doc( CLASS.STATUS.GABU, "Show Frame Per Second" )
end
function BU_FPS:create( rect )
	return BU.__create_wiz_text( "FPS", rect )
end
--function BU_FPS:get_text()	return "FPS "..aaa.time.get_str_fps()	end
function BU_FPS:draw()
	gol.color_yellow( )
	BU:draw_text_nice_shadow( "FPS"..aaa.time.get_str_fps(), -.48,-.5,0, .23,1, "left" )
end
--
--	WATCH
--
if CLASS.DECLARE( "BU_WATCH", BU_TEXT ) then
	BU_WATCH:set_class_status_doc( CLASS.STATUS.GABU, "Show Time to avoid his disparition" )
end

function BU_WATCH:create( rect )
	return BU.__create_wiz_text( "WATCH", rect )
end
function BU_WATCH:get_text()
	--self:print( "BU_WATCH:get_text()" )
	local ti = aaa.time
	local t = ti.t
	t = (t - math.floor( t ) )*4

	local function make_num( v )
		if v < 10 then
			return "0"..v
		else
			return v
		end
	end
	local s0 = " : "
	local s1 = "   "
	local str = inside(t,3,4) and s0 or s1
	str = str..make_num(ti.hour)
	str = str..( inside(t,0,1) and s0 or s1 )
	str = str..make_num(ti.minute)
	str = str..( inside(t,1,2) and s0 or s1 )
	str = str..make_num(ti.second)
	str = str..( inside(t,2,3) and s0 or s1 )
	--self:print( str )
	return str
end

--
--	BLOB
--
if CLASS.DECLARE( "BU_BLOB", BU_TEXT ) then
	BU_BLOB:set_class_status_doc( CLASS.STATUS.GABU, "Show Nb of blob" )
end
function BU_BLOB:create( rect )
	return BU.__create_wiz_text( "BLOB", rect )
end
function BU_BLOB:get_text()
	local blobs = ga:get_blobs()
	local blob_nb = blobs:get_blob_nb()
	local str
	local t = aaa.time.t_real
	local c = (t - math.floor(t)) > .5 and "-" or " "
	if blob_nb > 0 then
		str = c..blobs:get_blob_nb()..c
	else
		str = c.."  "..c
	end
	return str
end

--
--	POWER
--
if CLASS.DECLARE( "BU_POWER", BU_TEXT ) then
	BU_POWER:set_class_status_doc( CLASS.STATUS.GABU, "Show state of Power" )
end
function BU_POWER:create( rect )
	return BU.__create_wiz_text( "POWER", rect )
end
function BU_POWER:get_text()
	return aaa.power.get_str()
end

--
--	MAAEB
--
if CLASS.DECLARE( "BU_MAAEB", BU_WIZ ) then
	BU_MAAEB:set_class_status_doc( CLASS.STATUS.GABU, "Mâa visual signature" )
end
function BU_MAAEB:create( rect )
	return BU.__create_wiz( "MAAEB", rect )
end
function BU_MAAEB:draw()
	MAAEB.draw( 0,0, 0,1, true )
end

--
--	MESS
--
if CLASS.DECLARE( "BU_MESS", BU_WIZ ) then
	BU_MESS:set_class_status_doc( CLASS.STATUS.GABU, "Window displaying message in the background os window (console)" )
end

function BU_MESS:create( rect )
	local self = BU.__create_wiz( "MESS", rect )
	self:set_method_on_click_double( self, "do_click_double" )
	self.__scale_min = .05	--hack or we have problem when inited bug
	self.ref =	{	sx = param.get_ref( aaa.ref.flatland, "character_size_x" ),
					sy = param.get_ref( aaa.ref.flatland, "character_size_y" )
				}
	self:set_mobile( false )
	self:set_ui_top( false )
	--self:set_ui_active( false )
	return self
end
function BU_MESS:draw()
	if not self.ref.sy then
	end

	local sx = param.get( self.ref.sx )
	local sy = param.get( self.ref.sy )

	gol.set_texture_dim( 0 )
	local flat = aaa.flatland
	local r,g,b, b_inv
	local SY = 4	--todo solve those
	--todo do a convert size
	local le,bo =	self:convert_xy_local_to_top( -.5, -.5 )
	local ri,to =	self:convert_xy_local_to_top( .5, .5 )
	--todo have a way to edit these constants
	local scaler = 400 
	local nb_y = math.floor( (to-bo)*scaler/sy )	
	local nb_x =  (ri-le)*scaler*.7/sx
	--self:print( bo.." , "..to.." "..nb )
	local nb_y_over = 1 / nb_y
	local nb_x_over = 1 / nb_x
	local nb_mess = flat.get_mess_line_nb()
	local nb = math.min( nb_y, nb_mess )
	local offset = 1 - self.__scroller:get_value()
	offset = math.floor( offset * (nb_mess-nb) )
	for i=1,nb_y do
		r,g,b, b_inv = flat.get_mess_color( i+offset )
		gol.color( r,g,b )
		local str = flat.get_mess( i+offset )
		aaa.draw_str_maa_xy( str, -.5, -.5+(i-1-.1)*nb_y_over, nb_x_over, nb_y_over );
	end
end
function BU_MESS:do_click_double( x,y )
	local bu_up = self:get_bu_up()

--	self:print( "click_double" )
	local x,y	= bu_up:get_xy()
	local sx,sy = bu_up:get_sxy()
	local nx, ny
	nx = x - sx *.5
	ny = y - sy *.5
---[[
	if self.__rect_last then
		local r = self.__rect_last
		nx,ny, sx,sy = r.x,r.y, r.sx,r.sy
		self.__rect_last = nil
	else
		self.__rect_last = {x=x,y=y, sx=sx,sy=sy} 
		sy = 2.5
		nx = nx + sx *.5
		ny = ny + sy *.5
	end
--]]

	bu_up:set_xy( nx, ny )
	bu_up:set_sxy( sx, sy )
end
