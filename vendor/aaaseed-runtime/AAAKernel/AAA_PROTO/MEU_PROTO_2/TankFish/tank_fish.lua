function meu:define_meu_infos( )
	return { author = "Mâa",
				tags = { "3D" },
				help = {	"Deal with Fish Tank, done to generalized use in aquarium APPs."
						}
			}
end

function meu:define_ui()
	local tank = self:get_tank()
	TANK.cur = tank
	--self:add_button(	{	12, 1 },				"UI Race",	self, "b_ui_tank",	false		)

	local ix = 1
	local iy = 1
	local SY = 1
	local DY = SY * .2
	local SX = SY * 1.5

	local bu
	bu = self:add_trig_fn(	{	ix,		iy,				SX,	SY},		"DEF",			aaa.layers.edit_lua, tank:get_def_layers()	)
		bu:set_color_back_edit()
	bu = self:add_trig_fn(	{	ix+SX,	iy,				SX,	SY},		"RUN",			aaa.layers.edit_lua, tank:get_run_layers()	)
		bu:set_color_back_edit()
	bu = self:add_trig_method(	{ix+SX*2, iy,			4,	SY},		"Load Tank",	self, "load_tank"							):set_color_back("load")

	iy = 2
	ix = 1
	SX = 12/8

	for i=1,8 do
		bu = self:add_slider(	{ix+SX*(i-1),iy,	SX,SY},		"Morph"..i,		tank.morph_value_ui, i,		0,	0, 1	)
			bu:add_values_def( .25, .5, .75, 1, 2, 3, 4 )
	end
	iy = iy +SY + DY
	local ys = iy
	local bus = bus_cur
	--bus:print_down( "1" )
	if tank.b_loaded then
		--bus:set_tab_key( "Fish" )

		local top = 0.31
		local bot = -.47
		local ggrea = tank:get_ggrea()

		--ui which stay on the right
		local bup = bus:add_bup( {0.25, bot, .5, top} )
		local bup_size = 1/22
		local margin = .0
		bup:set_size( bup_size, bup_size, margin, margin ):set_col_nb( 1 ):start()
		tank:define_ui_updraw( bus )

		bus:set_tab_key( "Fish" )
		bup:set_lbrt( { -.75, bot, .5, top } )	-- -.75 is a pure hack but Maa have to move on
		ggrea:define_ui( bus )

		local line_nb = 25.000001
		if app.get_tank_grea_max_nb then
			line_nb = app:get_tank_grea_max_nb()
		end
		local grea_nb = tank:get_grea_nb()
		if grea_nb > (line_nb * 2) then
			line_nb = math.ceil( grea_nb/2 )
		end

		bup_size = ( top - bot )/line_nb
		local margin = bup_size*.2
		bup:set_lbrt( { -.5, bot, .5, top } )
		bup:start()
		bup:set_size( bup_size, bup_size, margin, margin ):set_col_nb( 6 ):start()
		ggrea:define_ui_greas( bus )

		--bus:print_down( "2" )

--		margin = size*.2
--		bup:set_lbrt( { .1, bot, .5, top ):set_size( size, size, margin, margin ):set_col_nb( 8 ):start(} )
		--bus:move_next_col()
		--bus:move_next_col()

		bus:set_tab_key( "Global" )
		bup:set_lbrt( { -.5, -.5, .5, top } )
		bus:set_xy(	1, 1 )
		tank:define_ui_global( bus )

		bus:set_tab_key( "Curve" )
		bup:set_lbrt( { -.5, -.5, .5, top } )
		bus:set_xy(	1, 1 )
		tank:define_ui_curve( bus )

	end

	bus:set_tab_key( "Races" )
	ix, iy = 1, ys
	bu = self:add_slider(	{ix,iy,	8,SY}, "Race.Size_factor", RACE, "size_factor", 1, 0, 2	)

	bus:set_tab_key( "Main" )
	--bus_cur:print_down( "4" )
end


function meu:get_tank()
	return self.tank
end

function meu:load_tank()
	local tank = self:get_tank()

	tank:free()

	MEU.draw( self )
	--todoaqua cleaner
	MEDIA.set_dir( app.media_dir_rel.."Aqua/ACD/" )
	tank:init()
	--self:box_debug( "tank_inited : "..self.loaded )

	if tank.b_loaded then
		--self:box_look( "before" )
		self:redefine_bus( true )
		--self:box_look( "after" )

		self.tank_ui_bus = tank:define_ui()
		--hackmona
		self.tank_ui_bus:set_active( false )
	end
end

function meu:save_preset( ... )
	if self.tank and self.tank.b_loaded then
		MEU.save_preset( self, ... )
	end
end

function meu:update_ui_tank()
	local tank = self:get_tank()
	if tank.b_loaded and self.tank_ui_bus then
--hackmona
--		self.tank_ui_bus:set_active( self.b_ui_tank )
		self.tank_ui_bus:set_active( false )
	end
end
function meu:restart()
	local tank = self:get_tank()
	if tank then
		tank:restart()
	end
end
function meu:update_ui()
	self:update_ui_tank()
end

function meu:update()
	--monahack useful but need to be done in a compatible way with other app
	if app.is_tablet and app:is_tablet() then return end

	local tank = self:get_tank()
	if not tank.b_loaded then
		meu:load_tank()
	end
	--self:print( "tank_fish meu:update()" )
	self:update_ui_tank()
	tank:update()
	if self.__b_need_force_setting then
		if tank then
			tank:restart()
			--self:print( app:_slave() )
			tank:set_draw_fish( app:is_fish_draw() )
			tank:set_boid_draw( app:is_fish_boid() and (not app:is_so()) and (not app:is_pc_mas()) )
			self.__b_need_force_setting = false
		end
	end
end

function meu:draw()
	--monahack useful but need to be done in a compatible way with other app
	if app.is_tablet and app:is_tablet() then return end

	--self:print( "tank_fish meu:draw()" )
	self.tank:draw()
end

function meu:init()
	--monahack useful but need to be done in a compatible way with other app
	if app.is_tablet and app:is_tablet() then return end

	if not self.tank then

		--self:box_debug( "init again " )
		if app.create_tank then
			self.tank = app:create_tank( meu )
		else
			--todo this is dangerous with several tanks
			CELT.__shader_vert_def = 151
			CELT.__shader_frag_def = 150
			self.tank = TANK:create( "Generic", nil, self )
		end

		-- self:box_debug( "dir have to be but absolute\n"..aaa.obj.get_dir(self:get_obj_main()) )
		-- self:box_debug( "MEU dir Method 1\n"..dirname )
		local dirname = self:get_dir_absolute()
		--self:box_debug( "MEU dir Method 2\n"..dirname )
		--self:box_debug( "MEU dir\n"..self:get_dir_new() )
		--local dirname = aaa.obj.get_dir(obj)
		--self:box_debug( "module dir name is\n"..dirname )
		self.tank:set_module_fish_dir_name( dirname ) -- was "APP/APP_Garden_Min/AAA_MEU/TankFish_1/"
	end
	MEU.draw( self )
end

function meu:switch_off()
	local mu = self:get_mu()
	--self:print(mu:get_value())
	if mu:get_value()>0 then
		self:print( "switch_off()" )
		local tank = self:get_tank()
		if tank then
			tank:restart()
		end
		mu:set_value(0)
	end
end

function meu:switch_on()
	local mu = self:get_mu()
	if mu:get_value()~=1 then
		self:print( "switch_on()" )
		mu:set_value(1)
		--hackmona we should have a default
		self:set_ui_slot( 5 )
		self.__b_need_force_setting = true
	end
end

function meu:switch_off_on( b )
	if b then	self:switch_on()
	else		self:switch_off()
	end
end