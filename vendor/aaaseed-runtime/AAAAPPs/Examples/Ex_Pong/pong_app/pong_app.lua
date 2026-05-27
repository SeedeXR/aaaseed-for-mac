
-- aaa.print( "entering pong_app.lua" )

-- Reminders:
-- aaa.time.t  -- temps en seconde
-- aaa.time.dt -- temps depuis la derniere frame.

-- 	ref = param.get_ref( aaa.ref.pref, "camera_format_custom_x")
--	value = param.get( ref )
--	param.set( ref, value )



-- PONG INTERFACE DEFINITION
-- no need to be called here, but to be included into a windows bound to PONG.lua
CLASS.DECLARE( "INTERFACE" )

function INTERFACE:create( i )
	local self = INTERFACE:create_instance( "interface_"..i )
	self:init( i )
	return self
end

function INTERFACE:init( i )
	self.id = i

	self.controler_box={}
	self.controler_box.centerx = -0.6
	self.controler_box.centery = - 1.5

	self.controler_box.size = 0.5
	self.controler_box.width = 0.25
	self.controler_box.slength = 1	-- slider length
	self.controler_box.sangle = 1/4 -- slider angle

	self:print( "init done!" )

	self.player={}
	for i=1,2 do
		self.player[i]={}
		self.player[i].is_on_midi = false
		self.player[i].is_on_ia = false
		self.player[i].is_on_mouse = false
		--self.player[i].is_linked = false
	end

		self.player[1].is_on_midi = true
		self.player[2].is_on_midi = true

		self.player[1].is_on_ia = false
		self.player[2].is_on_ia = true

		self.player[1].is_on_mouse = true
		self.player[2].is_on_mouse = true

		self.player[1].is_on_kinect = false
		self.player[2].is_on_kinect = false

		self.player[1].is_on_external = false
		self.player[2].is_on_external = false
end

local function create_interface()
	interface = INTERFACE:create( 1 )
end

-- interfaces functions
local function define_ui_midi_bu( bus, controler_box, n_controlers )
-- feed from: interface
-- called in: pong_pages_define
	local ncc = n_controlers

	local size = controler_box.size
	local cx = controler_box.centerx
	local cy = controler_box.centery

	local s_wid = controler_box.width 	* size  -- slider W
	local s_len = controler_box.slength	* size  -- sliders length
	local s_angle = controler_box.sangle		-- sliders orientation angle

	-- 16 Rotative and linear sliders on midi CC
	controler_box.bu_r={} -- rotative dial
	controler_box.bu_s={} -- linear slider

	for n=1,ncc	do

		local line_period = 8
		local line_size = 1.5 * size
		local r_size = 0.3 * size

		local deltax = 0.35 *size --(linesize/line_period)*2*size
		local deltay = 0.3 * size
		local gap_y = -0.1 * size

		local i = math.ceil(n/line_period)
		local j = math.fmod(n-1,line_period)+0

		-- R_Dial i
		bu = bus:add_dial( n, cx + (j) * deltax, cy - (i) * deltay, r_size, r_size, 0 )
		bu:set_midi( 1, n-1 )
		bu:set_text("cc "..n )

		controler_box.bu_r[n] = bu

		--V_slider i
		--bu = bus:add_slider( nil, n, cx + (j) * deltax, cy + gap_y - (i)*deltay - s_len, s_len, s_width, s_angle, n )
		--bu:set_midi( 1, n-1 )
		--controler_box.bu_s[n] = bu

	end
end

-- PAGES Define
if not aaa.lua.global.get( "m42" ) and aaa.lua.global.get( "pong" ) then pong.active = true end

local function pong_draw_foreground()
-- called in: pong_pages_define
	if pong.active then
	   pong:update_and_draw()	--	same than pong.draw( pong )
	else
	end
end

local function pong_pages_define()

	aaa.print_inverse( "pong_pages_define()" )

	local bus = BUS:create( "pong" )
	bus:init_begin_add_to_ga()
		local ratio_x = 1.77
		local taille_x = 3 * ratio_x;
		local taille_y = 3;

		--bus:inc_page()

		--if interface is define
		--define_ui_midi_bu (bus, interface.controler_box, 16 )

		-- BUS set
		bus:make_bus_loop()
		-- the scrolling bu is on the bottom
		bus:add_scroll( 1, 2 )
	bus:init_end()

	bus:set_function( "draw_after", pong_draw_foreground	)
end


-- Call Zo
if IS_BUSS_OPEN() then
	-- create_interface()
	aaa.lua.global.declare_table( "pong" )
	pong = PONG:create( 1 ) -- PONG INSTANCIATION
	--APP.CREATE_INST( PONG, "pong", 1 )
	pong_pages_define()
end

