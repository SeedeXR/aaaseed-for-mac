local pr = {y = 10}
function pr:print(...)
	for i=1,select("#", ...) do
		self.y = self.y - 1
		aaa.draw_str_maa_xy( select(i, ...), 1, self.y)
	end
end

local function doit()
	aaa.lua.global.declare('n')
	n = n or 0
	n = n + 1
	pr:print("n : " ..n.." t : "..aaa.time.t)
end

local function cube_size_osc()
	local ref = aaa.obj.get_by_name("tp1_model")
	param.set(ref, "size_factor", 1 + 0.5 * math.sin(aaa.time.t))
	--ou
	--param.set("Module/Jean/TpLua1/fx_a.model", "size_factor", math.sin(aaa.time.t))
	--ou
	--param.set("tp1_model", "size_factor", math.sin(aaa.time.t))
end

local function cube_detour_pts()

end

local function base_grid(size, step, def)
	gol.begin_points()
	size = size / 2
	for i = -size, size, step do
		for j = -size, size, step do
			gol.vertex(i * def, 0, j * def)
		end
	end
	gol.do_end()
end

local function grid_at_pt(size, step, def, pt)
	gol.begin_points()
	size = size / 2
	for i = -size, size, step do
		for j = -size, size, step do
			gol.vertex(pt[1] + i * def, pt[2], j * def + pt[3])
		end
	end
	gol.do_end()
end

local function egg_box(size, step, def, pt)
	gol.begin_points()
	size = size / 2
	for i = -size, size, step do
		for j = -size, size, step do
			gol.vertex(i * def + pt[1], math.sin(i) * math.sin(j) * def + pt[2], j * def + pt[3])
		end
	end
	gol.do_end()
end

local function my_curve()
	local curve = aaa.obj.get_by_name("tp1_bdd_curve")
	local nb = aaa.bdd.get_control_point_nb( curve, 1 )
	pr:print("nb_pts_curve :"..nb)
	aaa.bdd.clear_control_points( curve, 1 )
	aaa.bdd.push_control_point_back( curve, 1, 1, 1, 1 )
	aaa.bdd.push_control_point_back( curve, 1, -1, 1, 1 )
	aaa.bdd.push_control_point_back( curve, 1, 2, 1, 1 )
--	nb = aaa.bdd.get_control_point_nb( 	BDD, node_id )
--	aaa.bdd.push_control_point_back(	BDD, node_id, x, y, z )
--	aaa.bdd.pop_control_point_front(	BDD, node_id )
--	aaa.bdd.clear_control_points(		BDD, node_id )
end

local function sheets()
	local bdd_boid = aaa.obj.get_by_name("tp1_bdd_boid")
	local nb_points = aaa.bdd.get_point_nb(bdd_boid)
	local boid_pts = aaa.bdd.get_points(bdd_boid)
	local debug = "on"


	gol.color( 0.25,0.75,1, 1 )
	pr:print("nb_points : "..nb_points)
	for i = 1, nb_points do-- in pairs(boid_pts) do
		local b = boid_pts[i]
		b.z  = select( 3, aaa.camera.world_to_cam( b[1], b[2], b[3] ) )
		b.z1 = select( 3, aaa.camera.world_to_cam( b[1], b[2], b[3] ) )
		b.z2 = select( 3, aaa.camera.world_to_cam( b[1], b[2], b[3] ) )
		b.z3 = select( 3, aaa.camera.world_to_cam( b[1], b[2], b[3] ) )
	end

	local function z_sort( z1, z2 )
		return z1.z < z2.z
	end

	table.sort( boid_pts, z_sort )

	local curve = aaa.obj.get_by_name("tp1_bdd_curve")
	local nb = aaa.bdd.get_control_point_nb( curve, 1 )
	pr:print("nb_pts_curve :"..nb)
	aaa.bdd.clear_control_points( curve, 1)

	for i = 1, nb_points do
		gol.color( 0.25 + i / 32, 0.75, 1, 0.25 )
		local b = boid_pts[i]
		local x, y, z = b[1], b[2], b[3]
--		if (i % 5 == 0) then
			aaa.bdd.push_control_point_back( curve, 1, x, y, z )
--		end
		gol.draw_quads_3d(x, y, z, x - 1, y, z, x - 1, y - 1, z, x, y - 1, z)
		if (i % 30 == 0 and debug == "on") then
			pr:print( "id point :"..i,
						"	x :"..b[1],
						"	y :"..b[2],
						"	z :"..b[3] )
		else
	--		grid_at_pt(10, 0.2, 0.2, boid_pts[i])
	--		egg_box(10, 0.2, 0.2, boid_pts[i])
		end
	end
end

aaa.obj.update_then_draw("tp1_cam")
gol.color( 0.25,0.75,1, 1 )
doit()
cube_size_osc()
gol.set_depth( 1 )
local layers = aaa.layers.get_cur()
aaa.layers.draw_begin( layers )
aaa.layers.draw_layer_all( layers )
--base_grid(10, 0.1, 0.1)
--egg_box(20, 0.2, 0.1)
sheets()
--my_curve()

aaa.layers.draw_end( layers )
aaa.layers.skip_rest()
--xc, yc, zc, wc = aaa.camera.world_to_cam(		x, y, z )
--local layers = aaa.layers.get_cur()
--aaa.layers.draw_begin( layers )        --        should be match always by a draw_after
-- aaa.layers.draw_layer_all( layers )
--aaa.layers.draw_end( layers )
