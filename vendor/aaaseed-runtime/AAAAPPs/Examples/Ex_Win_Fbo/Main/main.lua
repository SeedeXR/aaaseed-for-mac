CLASS.DECLARE( "SER" )

function SER:create( name, x, y )
	local self = SER:create_instance( name )
	self:set_xy( x, y )
	self.x_last = 0
	self.y_last = 0
	return self
end
function SER:set_xy( x, y )
	self.x = x
	self.y = y
end
function SER:update()
	local x,y = self.x, self.y
	self.x_last = self.x
	self.y_last = self.y
	local dt = aaa.time.dt
	--if math.random() < dt then
		x = x + (math.random()-.5) * dt
		y = y + (math.random()-.5) * dt
	--end
	self.x = x
	self.y = y
end
function SER:draw()
	gol.draw_line_strip_2d(	self.x_last, self.y_last,	self.x, self.y )
	aaa.draw_cube( self.x, self.y, 0, .08 )
end


APP.DECLARE( "EX_WIN_FBO", APP )

function EX_WIN_FBO:init_app( ... )
	if not oo.getsuper(EX_WIN_FBO).init_app( self, ... ) then return end

	--	change init depending on machine
	if aaa.pc.is_maa() then
	else
	end

	self.scope_3d	= SCOPE_3D:create( "mouse" )
	self.scope_3d:set_distance_min( .1 )

	--	find the layers we use
	--	we get the module
	local module = aaa.module.get_cur()
	local ref = self.ref
	ref.tuto = {}
	for i=1,2 do
		local r = {}
		--	get layers by name
		r.obj	=	aaa.obj.get_down(	module,	"ex_tuto_"..string.char( i + 64 + 32 )	)
		--	find active param
		r.active = param.get_ref( r.obj, "active" )
		--	find fbo by class (only one in layers)
		r.fbo	=	aaa.obj.get_down_by_class(	r.obj,	"fbo"	)
		--	find fbo active param
		r.fbo_active = param.get_ref( r.fbo, "active" )

		ref.tuto[i] = { ref = r }
	end

	self.ser = SER:create( "toto", 0, 0 )

	self:pages_define()

	return true
end

local	function add_slider( text )
	local bu = bus_cur:add_slider( text )
	return bu
end

function EX_WIN_FBO:base_define()
	local bus = BUS:begin_window( "BUS_Base" )

		local bu
		--	add button to control scope
			bu	=	bus:add_but_target_lua( "Scope",	nil,	self, "b_draw_scope",		false	)
			bu	=	add_slider( "Grey" )
				--	1/	done storing the bu so we can get the value
				bu:set_value( 1 )
				self.ui.bu_scope_grey = bu
			bu	=	add_slider( "Alpha" )
				--	2/	done using lua field in table (self here)	:	target_lua
				bu.scope_alpha = 1
				bu:set_target_lua( self, "scope_alpha" )

		--	add button to control A
		--	done using lua field in table (self here)	:	target_lua
			bus:move_next()
			bu	=	bus:add_but_target_lua( "A",		nil,	self, "b_draw_a",		true	)
			bu	=	bus:add_but_target_lua( "A Fbo",	nil,	self, "b_draw_fbo_a",	true	)
		--	add button to control B
		--	done with attaching to param				:	target_param
			bus:move_next()
			local ref = self.ref.tuto[2].ref
			bu	=	bus_cur:add_but_target_param( "B", 		nil,	ref.active,		true )
			bu	=	bus_cur:add_but_target_param( "B Fbo",	nil,	ref.fbo_active,	true )

			bus:move_next()
			bu	=	bus:add_but_target_lua( "Add Blob",	nil,	self, "b_add_blob",		false	)
	bus:end_window()
	--bus:set_method( "draw_after",	self,	"draw_ui_back"	)

	return bus
end

function EX_WIN_FBO:base_define_bis()
	local bus = BUS:begin_window( "BUS_Base_Bis" )

		local bu
		bu = add_slider( "T1" ):set_meter( true )
		bu = add_slider( "T2" ):set_meter( true )
		bu = add_slider( "T3" ):set_meter( true )

	bus:end_window()

	return bus
end
function EX_WIN_FBO:add_blobs()
	local blobs = ga:get_blobs()
	if self.b_add_blob and blobs then
		self:print( "can now add blobs" )
		local new_blobs = {}
		local d = .5	--	instead of 0 to see better
		--	4 and 2.25 comme from 16/9 format with 8 unit horizontal
		new_blobs[1] =	{	x=0,	y=0,		id=1	}
		new_blobs[2] =	{	x=1	,	y=1,		id=2	}
		new_blobs[3] =	{	x=4-d,	y=2.25-d,	id=3	}
		blobs:register_blobs_external( new_blobs )
	end
end

function EX_WIN_FBO:pages_define()
	local bu

	--	this one appear in F2 because of the ga:register_ui_group below
	local bus = BUS:create( "BUS_Tuto_UI" )
	bus:set_bu_pos_load_save( true )	--	so position of the BU in this BUS are saved (in a lua file of course...)
	bus:set_active( true )
	bus:init_begin_add_to_ga()

		bu = bus:add_window( "Base", self:base_define(),	{0,0, 2,2} )
		bu = bus:add_window( "Bis", self:base_define_bis(),	{0,0, 2,2} )

		local function add_mon( text, bind, rect )
			local bu
			bu = bus_cur:add_image( text, rect, bind )	--	here we could use bus also instead of bus_cur
			bu:set_border( true )
			bu:set_border_line( true )
			bu:set_mobile( false )
			bu:set_ui_top_size( true )
			--bu:set_grid_xy( 1/32, 1/32 )
			bu:set_ratio_max( 1000 )
			bu:set_text( text, .1 )
			bu:set_border_line_color( { 0,1,1, .5 } )
			bu:set_window()
			return bu
		end

		--	very useful help
		local S = .5
	--	bus:add_wiz_cam(	{0, 0,		S} )
	--	bus:add_wiz_fps(	{0, -S*1.2,	S} )
	--	bus:add_wiz_watch(	{0, S*1.2,	S} )
	--	bus:add_wiz_maaeb(	{0, 0,		S} )
	--	bus:add_wiz_alive(	{0, 0,		S} )
	--	bus:add_wiz_mess(	{0, 0,		4, 2} )

	bus:init_end()

	ga:register_ui_group( bus )
	ga:set_ui_group_active( false )
	bus:set_method(	"draw_after",	self,	"draw_scope"	)

	local bus = BUS:create( "BUS_Tuto" )
	bus:set_bu_pos_load_save( true )	--	so position of the BU in this BUS are saved (in a lua file of course...)
	bus:set_active( true )
	bus:init_begin_add_to_ga()

		bu = add_mon( "A",			32,	{-2,1,	2,1}	)
		bu = add_mon( "B",			33,	{-2,-1,	2,1}	)
		bu = add_mon( "Final",		34,	{2,0,	2,1}	)

	bus:init_end()

--	next line try bus instead if ga if you are curious
	ga:set_method(	"update_before",	self,	"add_blobs"		)
	--ga:set_method(	"draw_before",		self,	"draw_before"	)
	--ga:set_method(	"draw_after",		self,	"draw_after"	)

	return bus
end

function EX_WIN_FBO:draw_before()
	if not self then return end
	if not self.ref.tuto then return end
	if not self.ref.tuto[1] then return end
	if self.b_draw_a then
		local r = self.ref.tuto[1].ref
		param.set( r.fbo_active, self.b_draw_fbo_a )
		aaa.obj.update_then_draw( r.obj )
	end
end
function EX_WIN_FBO:draw_scope()
	if not self.b_draw_scope then return end
	--aaa.print_method()
	local g = self.ui.bu_scope_grey:get_value()
	gol.color( g, g, g, self.scope_alpha )
	self.scope_3d:set_map_u( .2, .8 )
	--aaa.obj.update_then_draw( "attrib_light_paint" )
	self.scope_3d:draw_xy_tex( 0,0,0, 0, .1 )
end
function EX_WIN_FBO:draw_after()
	if not self then return end
	if not self.ref.tuto then return end
	if not self.ref.tuto[2] then return end
	self.ser:update()
	self.ser:draw()
	aaa.obj.update_then_draw( self.ref.tuto[2].ref.obj )
end

function EX_WIN_FBO:update()
	APP.update( self )

	local blobs = ga:get_blobs()
	local nb = blobs and blobs:get_blob_nb() or 0
	--aaa.print( nb )
	--param.set( GABU.finger.ref.nb, nb )	for i=1,nb do
	if nb>0 then
		local blob = blobs.blob[1]
			--aaa.print( i.." "..blob.x.." "..blob.y )
		local x = blob.x
		local y = blob.y
		local id = blob.id

		local v = { x, y, 0 }
		self.scope_3d:add( v )
	else
		self.scope_3d:update()
	end
end

-- Call Zo
if IS_BUSS_OPEN() then
	APP.CREATE_INST( EX_WIN_FBO ):print( "Ready" )
end

