
function meu:define_meu_infos( )
	return { 	author = "Jean Pirsch",
				version = 0,
				tags = { "procedural", "2d","3d", "Draw" },
				help = "Implementation of Bullet Physics",
			}
end

local body_types = { ["RIGID"] = 1, ["SOFT"] = 2 }

function meu:define_ui_world(ref, bu, ix, iy)

	self:add_camera()

	self:set_tab_key( "World" )

	bu = self:add_trig(	{ix, iy }, "Restart", ref.restart_trig )
	iy = iy + 1.4

	bu = self:add_selector(	{ix,iy,	5,1}, "Dispatcher" )
		bu:set_nb_min_0( 2, 1 )
		bu:set_item_text( 1, "Default", "MultiThread" )
		bu:set_target_param( ref.dispatcher )
	iy = iy + 1.4
	bu = self:add_selector(	{ix,iy,	8,1}, "Broadphase" )
		bu:set_nb_min_0( 3, 1 )
		bu:set_item_text( 1, "Dbvt", "32BitAxisSweep3", "AxisSweep3" )
		bu:set_target_param( ref.broadphase )
	iy = iy + 1.4
	bu = self:add_selector(	{ix,iy,	4,1}, "Solver" )
		bu:set_nb_min_0( 2, 1 )
		bu:set_item_text( 1, "Default", "Multibody" )
		bu:set_target_param( ref.solver )
	iy = iy + 1.4
	bu = self:add_selector(	{ix,iy,	5,1}, "Collision" )
		bu:set_nb_min_0( 2, 1 )
		bu:set_item_text( 1, "Default", "Soft-Rigid" )
		bu:set_target_param( ref.collision )
	iy = iy + 1.4
	bu = self:add_selector(	{ix,iy,	8,1}, "Dynamics world" )
		bu:set_nb_min_0( 4, 1 )
		bu:set_item_text( 1, "rigid", "soft", "multibody", "multibody - soft" )
		bu:set_target_param( ref.dynamics_world )

	iy = iy + 1

	ix,iy = 9,3
		bu = self:add_slider(	{ix,iy,	4,1}, "Scale", ref.scale, nil, .01, 0., 1 )
		iy = iy + 1
		self.bu_obj_nb = self:add_text_info(	{ix,iy,	4,1}, "Objects_nb" )
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Objects_nb_alloc", ref.objects_nb_alloc, nil, 1., 0., 2000 )
		iy = iy + 1

		bu = self:add_slider(	{ix,iy,	4,1}, "Gravity_x", ref.gravity_x, nil, 0., 0., 100 ):set_color_back("x")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Gravity_y", ref.gravity_y, nil, 0., 0., 100 ):set_color_back("y")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Gravity_z", ref.gravity_z, nil, 0., 0., 100 ):set_color_back("z")
	ix, iy = ix + 4, iy - 1
		bu = self:add_trig_method(	{	ix, iy},	"Set gravity", self, "set_gravity" )

		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Wind_x", ref.wind_x, nil, 1., 0., 100 ):set_color_back("x")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Wind_y", ref.wind_y, nil, 1., 0., 100 ):set_color_back("y")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Wind_z", ref.wind_z, nil, 1., 0., 100 ):set_color_back("z")
end
function meu:define_ui_object(ref, bu, ix, iy)
	local object = self.object
	local bdd = self.ref.bdd
	self:set_tab_key( "Object" )
	self:add_text_info(	{ix,iy-1,	8,.8}, "Shape" )
	ix = ix + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "size_x", object, "sx", 1., 0., 100 ):set_color_back("x")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "size_y", object, "sy", 1., 0., 100 ):set_color_back("y")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "size_z", object, "sz", 1., 0., 100 ):set_color_back("z")
	ix, iy = ix + 4, 4
		bu = self:add_slider(	{ix,iy,	4,1}, "pos_x",  object, "px", 1., 0., 100 ):set_color_back("x")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "pos_y",  object, "py", 1., 0., 100 ):set_color_back("y")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "pos_z",  object, "pz", 1., 0., 100 ):set_color_back("z")
	ix, iy = ix + 4, 4
		bu = self:add_slider(	{ix,iy,	4,1}, "dir_x",  object, "dx", 1., 0., 1   ):set_color_back("x")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "dir_y",  object, "dy", 1., 0., 1   ):set_color_back("y")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "dir_z",  object, "dz", 1., 0., 1   ):set_color_back("z")
	ix, iy = ix + 4, 4
	bu = self:add_trig_method(	{	ix, iy},	"Create shape", self, "create_shape" )
	iy = iy + 1
	bu = self:add_trig_method(	{	ix, iy},	"Create transform", self, "create_transform" )
	iy = iy + 1
	bu = self:add_trig_method(	{	ix, iy},	"Add one", self, "add_one_object" )
	ix, iy = 1, iy + 1.4
	bu = self:add_selector(	{ix,iy,	8,1}, "Primitive" )
		bu:set_nb( 4, 1 )
		bu:set_item_text( 1, "Sphere", "Box", "Cylinder", "Mesh" )
		bu:set_target_lua( object, "prim" )
	iy = iy + 1

	self:add_text_info(	{ix,iy,	8,.8}, "Body" )
	iy = iy + 1.4
		bu = self:add_selector(	{ix,iy,	8,1}, "Body type" )
		bu:set_nb( 5, 1 )
		bu:set_item_text( 1, "Rigid", "Soft", "Multibody", "Rope", "Cloth" )
		bu:set_target_lua( object, "body" )
	iy = iy + 1.4
	ix = ix + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Mass",     object, "mass",     1., 0., 100 )
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Friction", object, "friction", .01, 0., 1 )
	ix, iy = ix + 4, iy - 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Pressure", object, "pressure", 1., 0., 2000 )
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Volume",   object, "volume",   1., 0., 2000 )
	ix, iy = ix + 4, iy - 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Nodes_x",  object, "nodes_x",  1., 0., 2000 ):set_color_back("x")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Nodes_y",  object, "nodes_y",  1., 0., 2000 ):set_color_back("y")
	ix, iy = 1, iy + 1
	--		self:add_button( {	ix, iy }, "Aerodynamics", 		ref, "aerodynamics", false )
end

function meu:define_ui_constraint(ref, bu, ix, iy)
		self:set_tab_key( "Constraint" )

	bu = self:add_selector(	{ix,iy,	8,1}, "Type" )
		bu:set_nb_min_0( 4, 1 )
		bu:set_item_text( 1, "6Dof", "Hinge", "Point", "Encastrement?" )
		bu:set_target_lua( self, "constraint_type")
	ix,iy = 9,3

end

function meu:define_ui_add(ref, bu, ix, iy)
	local add = self.add
			self:set_tab_key( "Add" )

	ix = ix + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Add sx", add, "sx", 1., 0., 100 ):set_color_back("x")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Add sy", add, "sy", 1., 0., 100 ):set_color_back("y")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Add sz", add, "sz", 1., 0., 100 ):set_color_back("z")
	ix, iy = ix + 4, 4
		bu = self:add_slider(	{ix,iy,	4,1}, "Add px", add, "px", 0., -100., 100  ):set_color_back("x")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Add py", add, "py", 30., -100., 100 ):set_color_back("y")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Add pz", add, "pz", 0., -100., 100  ):set_color_back("z")
	ix, iy = ix + 4, 4
		bu = self:add_slider(	{ix,iy,	4,1}, "Add dx", add, "dx", 0., -1., 1 ):set_color_back("x")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Add dy", add, "dy", 1., -1., 1 ):set_color_back("y")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Add dz", add, "dz", 0., -1., 1 ):set_color_back("z")

	ix, iy = 1, iy + 2
		bu = self:add_slider(	{ix,iy,	4,1}, "Number", add, "number", 100, 0, 1000 )
		iy = iy + 1
		bu = self:add_trig_method(	{	ix, iy},	"Add objects", self, "add_objects" )
end

function meu:define_ui_spawn(ref, bu, ix, iy)
	local spawn = self.spawn
			self:set_tab_key( "Spawn" )
	ix = ix + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Spawn sx", spawn, "sx", 1., 0., 100 ):set_color_back("x")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Spawn sy", spawn, "sy", 1., 0., 100 ):set_color_back("y")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Spawn sz", spawn, "sz", 1., 0., 100 ):set_color_back("z")
	ix, iy = ix + 4, 4
		bu = self:add_slider(	{ix,iy,	4,1}, "Spawn px", spawn, "px", 0., -100., 100 ):set_color_back("x")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Spawn py", spawn, "py", 30., -100., 100 ):set_color_back("y")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Spawn pz", spawn, "pz", 0., -100., 100 ):set_color_back("z")
	ix, iy = ix + 4, 4
		bu = self:add_slider(	{ix,iy,	4,1}, "Spawn dx", spawn, "dx", 0., -1., 1 ):set_color_back("x")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Spawn dy", spawn, "dy", 1., -1., 1 ):set_color_back("y")
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Spawn dz", spawn, "dz", 0., -1., 1 ):set_color_back("z")

	ix, iy = 1, iy + 2
		bu = self:add_slider(	{ix,iy,	4,1}, "Dispersion", spawn, "dispersion", 1, 0., 100 )
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Flux /s",    spawn, "flux", 1, 0., 100 )
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Spawn Time", spawn, "time", 1, 0., 100 )
		iy = iy + 1
		bu = self:add_slider(	{ix,iy,	4,1}, "Speed",      spawn, "speed", 1, 0., 100 )
		iy = iy + 1
		bu = self:add_trig_method(	{ix,iy},	"Add spawn", self, "add_spawn" )
end


function meu:define_ui_test( ref, bu, ix,iy )
	local test = self.test
	local lock = self.lock
		self:set_tab_key( "Test" )

	bu = self:add_trig_method(	{ix,iy},	"set pos", self, "set_pos" )
	ix = ix + 4
	bu = self:add_slider(	{ix,iy,	4,1},	"Set px", test, "px", 0., -100., 100 ):set_color_back("x")
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	4,1},	"Set py", test, "py", 0., -100., 100 ):set_color_back("y")
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	4,1},	"Set pz", test, "pz", 0., -100., 100 ):set_color_back("z")

	ix, iy = 1, iy - 1
	bu = self:add_trig_method(	{ix,iy},	"get pos", self, "get_pos" )

	iy = iy + 2
	bu = self:add_trig_method(	{ix,iy},	"set size", self, "set_size" )
	ix = ix + 4
	bu = self:add_slider(	{ix,iy,	4,1}, "Set sx", test, "sx", 1., 0., 100 ):set_color_back("x")
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	4,1}, "Set sy", test, "sy", 1., 0., 100 ):set_color_back("y")
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	4,1}, "Set sz", test, "sz", 1., 0., 100 ):set_color_back("z")

	ix, iy = 1, iy - 1
	bu = self:add_trig_method(	{ix, iy}, "get size", self, "get_size" )

	ix, iy = 9, 2
	bu = self:add_trig_method(	{ix, iy}, "clear objects", self, "clear_objects")
	iy = iy + 1
	bu = self:add_trig_method(	{ix, iy}, "create mini world", self, "create_mini_world")
	iy = iy + 1
	bu = self:add_trig_method(	{ix, iy}, "lock move rotate", self, "lock_move_rotate_all")
	iy = iy + 1
	bu = self:add_trig_method(	{ix, iy}, "set pos all", self, "set_pos_all")
	ix = ix + 4
	bu = self:add_button( {ix, iy }, "tx", lock, "tx" )
	iy = iy + 1
	bu = self:add_button( {ix, iy }, "ty", lock, "ty" )
	iy = iy + 1
	bu = self:add_button( {ix, iy }, "tz", lock, "tz" )
	iy = iy + 1
	bu = self:add_button( {ix, iy }, "rx", lock, "rx" )
	iy = iy + 1
	bu = self:add_button( {ix, iy }, "ry", lock, "ry" )
	iy = iy + 1
	bu = self:add_button( {ix, iy }, "rz", lock, "rz" )

end

function meu:define_ui()
	local ref = self.ref
	local bu
	local ix = 1
	local iy = 4

	self:define_ui_world(ref, bu, ix, iy)
	self:define_ui_object(ref, bu, ix, iy)
	--self:define_ui_constraint(ref, bu, ix, iy)
	self:define_ui_add(ref, bu, ix, iy)
	--self:define_ui_spawn(ref, bu, ix, iy)
	self:define_ui_test(ref, bu, ix, iy)
end

function meu:add_object_bid( bdd, mass, id_trans, id_shape, id_rigid_body, prim )
	local bid = aaa.bdd.add_object_bid( bdd, mass, id_trans, id_shape, id_rigid_body, prim )
	if bid then
		self.bids[bid] = bid
	end
	return bid
end
function meu:delete_obj_bid( bdd, bid )
	local bid = aaa.bdd.delete_obj_bid( bdd, bid )
	if bid then
		self.bids[bid] = nil
	end
	return bid
end

function meu:create_mini_world( )
	local bdd = self.ref.bdd
	local mass = 0
	local prim = 2
	local id_rigid_body = 0
	local id_ground_shape = aaa.bdd.create_box_shape( bdd, 100, 10, 100 )

	local id_border_shape1 = aaa.bdd.create_box_shape( bdd, 120, 30, 10  )
	local id_border_shape2 = aaa.bdd.create_box_shape( bdd, 10,  30, 100 )

	local id_ground_trans = aaa.bdd.create_transform( bdd, 0, -5, 0 )

	local id_border_trans1 = aaa.bdd.create_transform( bdd, 55,   5,  0   )
	local id_border_trans2 = aaa.bdd.create_transform( bdd, -55,  5,  0   )
	local id_border_trans3 = aaa.bdd.create_transform( bdd, 0,    5,  55  )
	local id_border_trans4 = aaa.bdd.create_transform( bdd, 0,    5, -55  )


	id_rigid_body = aaa.bdd.create_rigid_body( bdd, mass, id_border_trans1, id_border_shape2, prim )
	self:add_object_bid( bdd, mass, id_border_trans1, id_border_shape2, id_rigid_body, prim )
	id_rigid_body = aaa.bdd.create_rigid_body( bdd, mass, id_border_trans2, id_border_shape2, prim )
	self:add_object_bid( bdd, mass, id_border_trans2, id_border_shape2, id_rigid_body, prim )
	id_rigid_body = aaa.bdd.create_rigid_body( bdd, mass, id_border_trans3, id_border_shape1, prim )
	self:add_object_bid( bdd, mass, id_border_trans3, id_border_shape1, id_rigid_body, prim )
	id_rigid_body = aaa.bdd.create_rigid_body( bdd, mass, id_border_trans4, id_border_shape1, prim )
	self:add_object_bid( bdd, mass, id_border_trans4, id_border_shape1, id_rigid_body, prim )


	id_rigid_body = aaa.bdd.create_rigid_body( bdd, mass, id_ground_trans,  id_ground_shape, prim )
	self:add_object_bid( bdd, mass, id_ground_trans, id_ground_shape, id_rigid_body, prim )

	mass = 10
	local id_cube_shape = aaa.bdd.create_box_shape( bdd, 10, 10, 10 )
	local id_cube_trans = nil
	for i = 1, 3 do
		id_cube_trans = aaa.bdd.create_transform( bdd, i*5, i*5, 0 )
		id_rigid_body = aaa.bdd.create_rigid_body( bdd, mass, id_cube_trans, id_cube_shape, prim )
		self:add_object_bid( bdd, mass, id_cube_trans, id_cube_shape, id_rigid_body, prim )
	end
	local id_sphere_shape = aaa.bdd.create_sphere_shape( bdd, 5 )
	local id_sphere_trans = nil
	prim = 1
	for i = 1, 30 do
		id_sphere_trans = aaa.bdd.create_transform( bdd, 0, i*5, i*5-75 )
		id_rigid_body = aaa.bdd.create_rigid_body( bdd, mass, id_sphere_trans, id_sphere_shape, prim )
		self:add_object_bid( bdd, mass, id_sphere_trans, id_sphere_shape, id_rigid_body, prim )
	end
end

function meu:set_gravity()
	local x, y, z = param.get( self.ref.gravity_x ), param.get( self.ref.gravity_y ), param.get( self.ref.gravity_z )
	aaa.bdd.set_gravity( self.ref.bdd, x, y, z )
end

function meu:set_pos_all()
	for i = 0, self.objects_nb - 1 do
		aaa.bdd.set_pos_bid( self.ref.bdd, i, i*2, 10, 0 )
	end
end

function meu:lock_move_rotate_all()
	local lock = self.lock
	self:print( "tx : "..lock.tx.." ty : "..lock.ty.." tz : "..lock.tz.." rx : "..lock.rx.." ry : "..lock.ry.." rz : "..lock.rz )
	for i = 0, self.objects_nb - 1 do
		aaa.bdd.set_pos_bid( self.ref.bdd, i, 0, 10, 0 )
	end
	aaa.bdd.lock_move_rotate_all( self.ref.bdd, lock.tx, lock.ty, lock.tz, lock.rx, lock.ry, lock.rz )
end

function meu:create_shape( )
	local bdd = self.ref.bdd
	local object = self.object
	local id_shape = nil
	if object.prim == 1 then
		id_shape = aaa.bdd.create_sphere_shape( bdd, object.sx )
	elseif object.prim == 2 then
		id_shape = aaa.bdd.create_box_shape( bdd, object.sx, object.sy, object.sz )
	elseif object.prim == 3 then
		id_shape = aaa.bdd.create_cylinder_shape( bdd, object.sx, object.sy, object.sz )
	end
	if not id_shape then
		self:print( "error in create shape" )
		return
	end
	self.object.id_shape = id_shape
	self:print( "id shape : "..self.object.id_shape )
	return id_shape
end

function meu:create_transform( )
	local bdd = self.ref.bdd
	local object = self.object
	local id_transform = aaa.bdd.create_transform( bdd, object.px, object.py, object.pz )
	if not id_transform then
		self:print( "error in create transform" )
		return
	end
	self.object.id_transform = id_transform
	self:print( "id transform : "..id_transform )
	return id_transform
end
local shape_name = {"sphere", "box"}
function meu:add_one_object( id_transform, id_shape )
	local bdd = self.ref.bdd
	local object = self.object
	local id_rigid_body = nil
	local id_soft_body = nil
	local bid = nil
	id_transform = id_transform or object.id_transform
	id_shape = id_shape or object.id_shape
	local prim = object.prim
	local mass = object.mass

	self:print( "bdd : "..bdd.." mass : "..mass.." trans : "..id_transform.." shape : "..id_shape.." prim : "..shape_name[prim] )
	if object.body == body_types["RIGID"] then
		id_rigid_body = aaa.bdd.create_rigid_body( bdd, mass, id_transform, id_shape, prim ) -- No local inertia
	end
	self:print( "object body type : "..object.body )
	if not id_rigid_body then
		self:print( "no valable id for rigid_body, stop add_objects" )
		return
	end
	bid = self:add_object_bid( bdd, mass, id_transform, id_shape, id_rigid_body, prim )
	if not bid then
		self:print( "no valable bid for object, stop add_object_bid" )
	end
	self:print( " bid : "..bid )
end

function meu:add_objects()
	local object = self.object
	local add = self.add
	local bdd = self.ref.bdd
	if add.number + self.objects_nb > param.get( self.ref.objects_nb_alloc ) then
		self:print( "can't add objects :\n    number of allocated objects < number of objects to add and present objects number" )
		return
	end
	--self:print( "obj prim : ", object.prim )
	local id_shape = self:create_shape( )
	local id_transform = nil

	if not id_shape then
		self:print( "no valable id for shape, stop add_objects" )
		return
	end
	local scale = param.get(self.ref.scale)

	self:print( "New ADD : "..add.number.." objects\n" )
	self:print( "px : " .. add.px .. "py : " .. add.py .. "pz : " .. add.pz )
	self:print( "sx : " .. add.sx .. "sy : " .. add.sy .. "sz : " .. add.sz )
	local rnd = math.random_centered
	for i = 0, add.number do
		id_transform = aaa.bdd.create_transform( bdd, add.px + rnd(add.sx), add.py + rnd(add.sy), add.pz + rnd(add.sz) )
		if not id_transform then
			self:print( "no valable id for transform, stop add_objects" )
			return
		end
		self:print( "object i : "..i )
		self:add_one_object( id_transform, id_shape )
	end
end

function meu:add_spawn()
	local object = self.object
	local spawn = self.spawn
	local bdd = self.ref.bdd
	if spawn.flux * spawn.time + self.objects_nb > param.get(self.ref.objects_nb_alloc) then
		self:print("can't add objects spawn :\n    number of allocated objects < objects flux * time of spawn")
		return
	end
	--self:print( "obj prim : ", object.prim )
	local id_shape = create_shape(bdd, object)
	local id_transform = nil
	local id_rigid_body = nil
	local id_soft_body = nil

	if not id_shape then
		self:print("no valable id for shape, stop add_objects")
		return
	end
	local scale = param.get(self.ref.scale)
		self:print( "New SPAWN - px : " .. spawn.px .. "py : " .. spawn.py .. "pz : " .. spawn.pz)
		print("todo dispersion factor influence")
		local rnd = math.random_centered
		id_transform = aaa.bdd.create_start_transform( bdd, spawn.px + rnd(spawn.sx), spawn.py + rnd(spawn.sy), spawn.pz + rnd(spawn.sz) )
		if not id_transform then
			self:print("no valable id for transform, stop add_objects")
			return
		end
		local sp = spawn.speed
		id_rigid_body = aaa.bdd.create_rigid_body( bdd, object.mass, id_transform, id_shape, spawn.dx * sp, spawn.dy * sp, spawn.dz * sp ) -- speed * dir as local inertia
		if not id_rigid_body then
			self:print("no valable id for rigid_body, stop add_objects")
			return
		end

end

function meu:set_xyz( bid, x,y,z )
	--self:print( "bisdis "..bid )
	local bdd = self.ref.bdd
	aaa.bdd.set_pos_bid( bdd, bid, x,y,z )
end
function meu:get_xyz( bid )
	--self:print( "bisdis "..bid )
	local bdd = self.ref.bdd
	return aaa.bdd.get_pos_bid( bdd, bid )
	--self:print( x.." "..y.." "..z.." ")
	--return x,y,z
end
function meu:set_sxyz( bid, x,y,z )
	--self:print( "bisdis "..bid )
	local bdd = self.ref.bdd
	aaa.bdd.set_size_bid( bdd, bid, x,y,z )
end
function meu:get_sxyz( bid )
	--self:print( "bisdis "..bid )
	local bdd = self.ref.bdd
	return aaa.bdd.get_size_bid( bdd, bid )
	--self:print( x.." "..y.." "..z.." ")
	--return x,y,z
end
function meu:set_pos()
	local test = self.test
	local bdd = self.ref.bdd
	aaa.bdd.set_pos( bdd, self.bodies[0] , test.px, test.py, test.pz)
end

function meu:get_pos()
	local bdd = self.ref.bdd
	local px, py, pz = aaa.bdd.get_pos( bdd, self.bodies[0] )
	self:print("px : "..px.." py : "..py.." pz : "..pz )
end

function meu:set_size()
	local test = self.test
	local bdd = self.ref.bdd
	aaa.bdd.set_size( bdd, self.bodies[0] , test.sx, test.sy, test.sz)
end

function meu:get_size()
	local bdd = self.ref.bdd
	local sx, sy, sz = aaa.bdd.get_size( bdd, self.bodies[0] )
	self:print("sx : "..sx.." sy : "..sy.." sz : "..sz )
end
function meu:set_damping( bid, lin, ang )
	aaa.bdd.set_damping( self.ref.bdd, bid, lin, ang )
end
function meu:set_dynamic( bid, b )
	aaa.bdd.set_dynamic( self.ref.bdd, bid, b )
end

function meu:clear_objects()
	local bdd = self.ref.bdd
	self:print( "obj nb : "..self.objects_nb )
	for bid,v in PAIRS( self.bids ) do
		self:print( "deleting bid "..bid )
		self:delete_obj_bid( bdd, bid )
	end
	self.bids = {}
end

function meu:init()
	local ref = self.ref

	ref.bdd					=	self:get_layer_bdd( 1 )
	ref.restart_trig		=	param.get_ref( ref.bdd, "restart_trig" )

	ref.dispatcher 			=	param.get_ref( ref.bdd, "dispatcher" )
	ref.broadphase 			= 	param.get_ref( ref.bdd, "broadphase" )
	ref.solver 				= 	param.get_ref( ref.bdd, "solver" )
	ref.collision 			= 	param.get_ref( ref.bdd, "collision" )
	ref.dynamics_world 		= 	param.get_ref( ref.bdd, "dynamics_world" )

	ref.gravity_x			=	param.get_ref( ref.bdd, "gravity_x" )
	ref.gravity_y			=	param.get_ref( ref.bdd, "gravity_y" )
	ref.gravity_z			=	param.get_ref( ref.bdd, "gravity_z" )
	ref.wind_x				=	param.get_ref( ref.bdd, "wind_x" )
	ref.wind_y				=	param.get_ref( ref.bdd, "wind_y" )
	ref.wind_z				=	param.get_ref( ref.bdd, "wind_z" )
	ref.scale				=	param.get_ref( ref.bdd, "scale" )

	ref.objects_nb_alloc	=	param.get_ref( ref.bdd, "objects_nb_alloc" )
	ref.objects_nb			=	param.get_ref( ref.bdd, "objects_nb" )

	self.bids = {}

	-- World
	-- object
	local object = {}
	object.sx = 1
	object.sy = 1
	object.sz = 1
	object.px = 0
	object.py = 0
	object.pz = 0
	object.dx = 0
	object.dy = 1
	object.dz = 0
	object.prim = 0
	object.id_shape = nil
	object.id_transform = nil
	object.body = 0
	object.mass = 10
	object.friction = 0
	object.pressure = 0
	object.volume = 0
	object.nodes_x = 0
	object.nodes_y = 0
	self.object = object
	-- Constraint
	-- Add
	local add = {}
	add.sx = 10
	add.sy = 10
	add.sz = 10
	add.px = 0
	add.py = 30
	add.pz = 0
	add.dx = 0
	add.dy = 1
	add.dz = 0
	add.number = 100
	self.add = add
	-- Spawn
	local spawn = {}
	spawn.sx = 1
	spawn.sy = 1
	spawn.sz = 1
	spawn.px = 0
	spawn.py = 10
	spawn.pz = 0
	spawn.dx = 0
	spawn.dy = 1
	spawn.dz = 0
	spawn.flux = 0
	spawn.time = 0
	spawn.speed = 0
	spawn.dispersion = 0
	self.spawn = spawn
	-- Test
	local test = {}
	test.px = 0
	test.py = 0
	test.pz = 0
	test.sx = 0
	test.sy = 0
	test.sz = 0
	self.test = test

	local lock = {}
	lock.tx = 1
	lock.ty = 1
	lock.tz = 1
	lock.rx = 1
	lock.ry = 1
	lock.rz = 1
	self.lock = lock

	-- PICKING
	--[[

	self.layers = meu:get_layers( )
	self.camera_ref	= aaa.layers.get_camera( self.layers )
--	self.camera_ref	= aaa.camera.get_cur()
	local xc, yc, zc, wc = aaa.camera.world_to_cam( 0, 1, -3 )
	self.ray = {0, 1, -3, xc, yc, zc}

	self._b_is_picking = false
	self.cam_pos = {0,0,0}
	self.click_pos = {0,0,0}
	self.to = {0,0,0}
	--]]

end

function meu:init_for_bb()
	self:clear_objects()

	local bdd = self.ref.bdd

	local mass = 0
	local prim = 2
	local id_rigid_body = 0

	self:set_bu_value( "gravity_x", 0  )
	self:set_bu_value( "gravity_y", 0  )
	self:set_bu_value( "gravity_z", 10 )
	self:set_bu_value( "scale", 1 )

	local id_ground_shape = aaa.bdd.create_box_shape( bdd, 8, 8, 1 )
	local id_ground_trans = aaa.bdd.create_transform( bdd, 0, 0, -.5 )
	id_rigid_body = aaa.bdd.create_rigid_body( bdd, mass, id_ground_trans,  id_ground_shape, prim )
	self:add_object_bid( bdd, mass, id_ground_trans, id_ground_shape, id_rigid_body, prim )

end

function meu:add_bb( x, y, size )
	local bdd = self.ref.bdd
	local prim = 1
	local mass = 5
	local id_shape = aaa.bdd.create_sphere_shape( bdd, size )
	local id_transform = aaa.bdd.create_transform( bdd, x, y, size * .5 )
	local id_rigid_body = aaa.bdd.create_rigid_body( bdd, mass, id_transform, id_shape, prim )
	local bid = self:add_object_bid( bdd, mass, id_transform, id_shape, id_rigid_body, prim )
	if not bid then
		aaa.box_debug( "bid is nil in meu:add_bb()" )
	end
	return bid
end

function meu:force_update()
	aaa.obj.update_then_draw(self.ref.bdd)
end
function meu:update()
	local nb = param.get( self.ref.objects_nb )
	self.bu_obj_nb:set_text( nb )
	self.objects_nb = nb
--[[
	local mouse_x,mouse_y = aaa.mouse.get_xy_render()
	local bu = ga:find_bu_by_pos( mouse_x, mouse_y )
	--if mouse_y > min_y and mouse_y < max_y and mouse_x > min_x and mouse_x < max_x then
	local blobs = ga:get_blobs()

    self:update_cam()

    -- Set _b_is_picking to true if screen is touched
	if blobs:get_touch_nb() > 0 then
		self:print("click")
		print("mouse x : " .. mouse_x, "mouse y : " .. mouse_y, "bu : " .. bu )
		--table.print(blobs)
		--table.print(blobs.touch)
		--table.print(blobs.__transfo)
		self.click_pos = {mouse_x, mouse_y, 0.}
		self._b_is_picking = true
	end
	if blobs:get_untouch_nb() > 0 then
		self:print("unclick")
		print("mouse x : " .. mouse_x, "mouse y : " .. mouse_y, "bu : " .. bu )
		self._b_is_picking = false
	end
    -- If touching / clicking : cast ray to pick object
	if self._b_is_picking == true then
		self.click_pos = {mouse_x, mouse_y, 0.}
        local click_pos = self.click_pos
        local cam_pos = self.cam_pos
		--table.print(cam_pos)
		--table.print(click_pos)

		aaa.bdd.pick( self.ref.bdd, cam_pos[1], cam_pos[2], cam_pos[3], click_pos[1], click_pos[2], -1 )

		--table.print(self.to)
		--table.print(mproj)
	--	table.print(mview)
	end
	--]]
	--[[		local m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16,
		      ma, mb, mc, md, me, mf, mg, mh, mi, mj, mk, ml, mm, mn, mo, mp = aaa.bdd.get_matrixes( self.ref.bdd )
		local mproj = {m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16}
		local mview = {ma, mb, mc, md, me, mf, mg, mh, mi, mj, mk, ml, mm, mn, mo, mp}
		self.to = dotv3m4defeignasse(click_pos, mview) --]]
end


--[[
function meu:draw()
	self:draw_layers_begin()
		self:draw_layer(1)
		--self:draw_layer(2)
	self:draw_layers_end()

	gol.color3(1, 0, 0)
	gol.set_line_width ( 5 )
	gol.draw_lines_3d( self.ray )
	local x, y, z = self.to
	--aaa.draw_cube( x, y, z )
end

local function rotX( a )
    local c = math.cos( a )
    local s = math.sin( a )
    return { 1, 0, 0, 0, c, -s, 0, s, c }
end

local function rotY( a )
    local c = math.cos( a )
    local s = math.sin( a )
    return { c, 0, s, 0, 1, 0, -s, 0, c }
end

local function rotZ( a )
	local c = math.cos( a )
	local s = math.sin( a )
	return { c, s, 0, -s, c, 0, 0, 0, 1.0 }
end

local function dot( vec, mat )
	local retx = vec[1] * mat[1] + vec[2] * mat[4] + vec[3] * mat[7]
	local rety = vec[1] * mat[2] + vec[2] * mat[5] + vec[3] * mat[8]
	local retz = vec[1] * mat[3] + vec[2] * mat[6] + vec[3] * mat[9]
	return { retx, rety, retz }
end
local function dotv3m4defeignasse( vec, mat )
	local retx = vec[1] * mat[1] + vec[2] * mat[5] + vec[3] * mat[9]
	local rety = vec[1] * mat[2] + vec[2] * mat[6] + vec[3] * mat[10]
	local retz = vec[1] * mat[3] + vec[2] * mat[7] + vec[3] * mat[11]
	return { retx, rety, retz }
end

-- USED FOR PICKING
function meu:update_cam()
    local ref = self.ref
    local cref = self.camera_ref
	local cam_pos = self.cam_pos
	local clp = self.click_pos

	if not cref then
		self.layers = meu:get_layers( )
		self.camera_ref	= aaa.layers.get_camera( self.layers )
		cref = self.camera_ref
	end
	if cref and self._b_is_picking then
        -- Get Translation
		local tx_ref = param.get_ref( cref , "tra_x" )
		local ty_ref = param.get_ref( cref , "tra_y" )
		local tz_ref = param.get_ref( cref , "tra_z" )
		local tx = param.get( tx_ref )
		local ty = param.get( ty_ref )
		local tz = param.get( tz_ref )
        -- Get Rotation
		local rx_ref = param.get_ref( cref , "rot_x" )
		local ry_ref = param.get_ref( cref , "rot_y" )
		local rz_ref = param.get_ref( cref , "rot_z" )
		local rx = param.get( rx_ref )
		local ry = param.get( ry_ref )
		local rz = param.get( rz_ref )
        -- Translate
		cam_pos = { tx+.01, ty, tz }
        -- Rotate
		cam_pos = dot( cam_pos, rotX( rx*math.pi*2 ) )
		cam_pos = dot( cam_pos, rotY( ry*math.pi*2 ) )
		cam_pos = dot( cam_pos, rotZ( rz*math.pi*2 ) )

		self.cam_pos = cam_pos
		self.ray = {self.click_pos[1], self.click_pos[2], 1, -cam_pos[1], -cam_pos[2], -cam_pos[3]}
	end
end

--]]


	local tab = {}
	tab.bullet = "Meu for bullet"
	tab.to_the_left = ""
	tab.define_ui = ""
	tab.init = ""
	tab.draw = ""
	tab.update = ""
	tab.update_cam = ""
-- Localz
	tab.rotX = ""
	tab.rotY = ""
	tab.rotZ = ""
	tab.dot = ""
	tab.dotv3m4defeignasse = ""