--todo add threshold
--todo add a "same for map and depth or reverse (generalize too)

function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "Proprietary", "2d", "Art", "Draw", "Experimental", "Procedural", "vj", "unfinished" },
			help = "Generate image like Mondrian painting"
			 }
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local ix = 1
	local iy = 1
	local SY = 1
	local DY = .5

	self:add_camera()
	self:add_rendering()

    bu = self:add_selector(			{ ix,iy,    8,SY}, "what" )
        bu:set_item_text( 1 , "Mondrian", "Mondrian Gpt", "Geodesique", "Face", "Metaball" )
        bu:set_target_lua( self, "s_what" )
		bu:set_text_draw( false ) 
	iy = iy + SY + DY

	self.size = {}
	self:add_sliders_uv_min_max(	{ix,iy,     8,SY},	"Size",			self.size, false, 1,	0,8,	{2,3,4,6} )
	iy = iy + SY + DY

	self:add_slider(				{ix,iy,	    4,SY},	"Separation",	self, "separation",		.02,	0,1 )
        :set_min_max_strict( true )
	self:add_slider(				{ix+4,iy,	4,SY},	"Jitter",	    self, "jitter",		    0,	    0,1 )
	iy = iy + SY + DY
	self:add_slider(				{ix,iy,	    8,SY},	"Gamma",		self, "gamma",			1,		0,8 )
	iy = iy + SY
	self:add_slider_two(			{ix,iy,	    8,SY},	"Range",		self, "min","max",		.1,.2,	0,1 )
	iy = iy + SY + DY

	self:add_button( {ix,iy,	SY,SY},		"Frequency_active",	self, "b_freq",		true	):set_text_visible(false)
	self:add_slider( {ix+SY,iy, 8-SY,SY},	"Frequency",		self, "freq",			.0,		0,128 )
	iy = iy + SY + DY
end

function meu:init()
	self.ph = 0
end

function meu:build_table( min,max, gamma )
	local t = {}
	if (min < 0 and max < -min*2) or max < 0 then
		self:print( "with this min and max algorithm will probably go to infinite loop: canceling build_table()" )
	else
		local val = 0
		while val < 1-min*.5 do
			table.insert( t, val )
			val = val + min + math.pow( math.random(), gamma ) * (max-min)
		end
		table.insert( t,1 )
	end
	return t
end

local 	colors = {
	{1,1,1,1}, 
	{1,1,1,1},
	{1,0,0,1},
	{1,1,0,1},
	{0,0,1,1},
	{.5,.5,.5,1},
	{0,0,0,1}
}

function meu:randomize_mondrian()
	local 	colors_pro = {
		.5,
		.5,
		.1,
		.1,
		.1,
		.1,
		.2,
	}

	local seed = math.floor( self.ph )
	if seed ~= self.seed then
		math.randomseed( seed )
		--local min
		local gamma = 1
		local f = .5
        --local sep = self.separation * ( 1 + self.jitter * ( math.random()-.5 ) * 2 )
		--min = sep*2/self.size.u
		self.hori = self:build_table( self.min, self.min + math.random()*self.max, self.gamma )
		--min = sep*2/self.size.v
		self.vert = self:build_table( self.min, self.min + math.random()*self.max, self.gamma )

        self.separation_used = self.separation * ( 1 + self.jitter * ( math.random()-.5 ) * 2 )
		local t = {}
		local nb = #colors_pro
		for i=1,(#self.hori-1)*(#self.vert-1) do
			local ind,pro
			repeat
				ind = math.random(nb)
				pro = colors_pro[ind]
				--self:print( ind.." - "..pro )
			until math.random()<=pro
			--self:print( "----------> "..ind ) 
			t[i] = ind
		end
		self.color_index = t
		self.seed = seed
	end
end

function meu:draw_mondrian_maa()
	gol.set_texture_dim(0)
	--table.print( self.size, "fsd", 2 )
	local fu,fv = self.size.u, self.size.v
	local tu,tv = self.hori,self.vert
	local d = self.separation_used
	local t_ci = self.color_index
	local i = 0
	local nb_u = #self.hori-1
	local nb_v = #self.vert-1
	for iv=1,nb_v do
		local b,t = tv[iv],tv[iv+1]
		b = (b-.5) * fv
		t = (t-.5) * fv		
		for iu=1,nb_u do
			local ci = t_ci[i]
			--self:print( i..":"..ci )
			gol.color( colors[ci] )

			local l,r = tu[iu],tu[iu+1]
			l = (l-.5) * fu
			r = (r-.5) * fu

			local lo,bo,ro,to, cond
			if true then
				cond = iu~=1 and t_ci[i-1]==ci
				lo = cond and l-d or l+d
				cond = not cond and t_ci[i-nb_u]==ci
				--cond = t_ci[i-nb_u]==ci
				bo = cond and b-d or b+d
			else
				lo = l+d
				bo = b+d
			end
			cond = true --t_ci[i+1]==ci
			ro = cond and r or (r-d)
			cond = true --t_ci[i+nb_u]==ci
			to = cond and t or (t-d)
			aaa.draw_rect( lo,bo, ro,to )
			i = i + 1
		end
	end
end

function meu:update()
	if self.b_freq then
		self.ph = self.ph + aaa.time.dt * self.freq
	end
    --if self.separation <= 0 then self.separation = 0.01 end
	self:randomize_mondrian()
end

-- experiment using chatgpt
function meu:draw_mondrian( x,y, sx,sy, ph, min_distance )
    -- Seed the random number generator with the integer part of ph
    math.randomseed(math.floor(ph))
    
    -- Function to draw a single rectangle
    local function draw_rect(rx, ry, width, height, r, g, b)
        gol.color(r, g, b)
        gol.begin_triangles()
        -- First triangle
        gol.vertex3(rx, ry, 0)
        gol.vertex3(rx + width, ry, 0)
        gol.vertex3(rx, ry + height, 0)
        -- Second triangle
        gol.vertex3(rx + width, ry, 0)
        gol.vertex3(rx + width, ry + height, 0)
        gol.vertex3(rx, ry + height, 0)
        gol.do_end()
    end

    -- Background
    draw_rect(x - sx, y - sy, 2 * sx, 2 * sy, 1, 1, 1) -- White background

    -- Helper function to generate non-overlapping lines
    local function generate_lines(count, min_val, max_val, min_distance)
        local lines = {}
        local attempts = 0
        local max_attempts = 1000 -- Limit the number of attempts to avoid infinite loops
        while #lines < count and attempts < max_attempts do
            local pos = math.random(min_val * 100 + min_distance * 100, max_val * 100 - min_distance * 100) / 100
            local too_close = false
            for _, line in ipairs(lines) do
                if math.abs(pos - line) < min_distance then
                    too_close = true
                    break
                end
            end
            if not too_close then
                table.insert(lines, pos)
            end
            attempts = attempts + 1
        end
        table.sort(lines)
        return lines
    end

    -- Draw the black lines to form a grid
    local num_vertical_lines = math.random(2, 15)
    local num_horizontal_lines = math.random(2, 15)
    local vertical_lines = generate_lines(num_vertical_lines, -sx, sx, min_distance)
    local horizontal_lines = generate_lines(num_horizontal_lines, -sy, sy, min_distance)

    -- Merge some lines to create a more irregular structure
    local function merge_lines(lines)
        local new_lines = {}
        local i = 1
        while i <= #lines do
            if i < #lines and math.random() < 0.3 then
                local merged_line = (lines[i] + lines[i + 1]) / 2
                table.insert(new_lines, merged_line)
                i = i + 2
            else
                table.insert(new_lines, lines[i])
                i = i + 1
            end
        end
        return new_lines
    end

    vertical_lines = merge_lines(vertical_lines)
    horizontal_lines = merge_lines(horizontal_lines)

    -- Helper function to draw lines
    local function draw_lines(lines, vertical)
        for _, line in ipairs(lines) do
            if vertical then
                draw_rect(x + line, y - sy, 0.05, 2 * sy, 0, 0, 0)
            else
                draw_rect(x - sx, y + line, 2 * sx, 0.05, 0, 0, 0)
            end
        end
    end

    draw_lines(vertical_lines, true)
    draw_lines(horizontal_lines, false)

    -- Draw random colored rectangles within the grid
    local colors = {
        {1, 0, 0},    -- Red
        {1, 1, 0},    -- Yellow
        {0, 0, 1},    -- Blue
        {1, 1, 1},    -- White
    }

    -- Helper function to choose random color
    local function get_random_color()
        return unpack(colors[math.random(1, #colors)])
    end

    -- Fill the grid cells with colors
    for i = 1, #vertical_lines + 1 do
        for j = 1, #horizontal_lines + 1 do
            local x1 = (i == 1) and (x - sx) or (x + vertical_lines[i - 1] + 0.05)
            local x2 = (i > #vertical_lines) and (x + sx) or (x + vertical_lines[i])
            local y1 = (j == 1) and (y - sy) or (y + horizontal_lines[j - 1] + 0.05)
            local y2 = (j > #horizontal_lines) and (y + sy) or (y + horizontal_lines[j])

            -- Randomly decide whether to fill this cell
            if math.random() < 0.5 then
                draw_rect(x1, y1, x2 - x1, y2 - y1, get_random_color())
            end
        end
    end
end

-- experiment using chatgpt
function meu:draw_geodesic( subdivisions )
    local function normalize( v )
        local f = 1. / math.sqrt(v[1]^2 + v[2]^2 + v[3]^2)
        return { v[1]*f, v[2]*f, v[3]*f }
    end
	local size = self.size.u
    local function normalize_and_size( v)
        local f = size / math.sqrt(v[1]^2 + v[2]^2 + v[3]^2)
        return { v[1]*f, v[2]*f, v[3]*f }
    end

    local function mid_point(v1, v2)
        return normalize_and_size({
            (v1[1] + v2[1]) / 2,
            (v1[2] + v2[2]) / 2,
            (v1[3] + v2[3]) / 2
        })
    end

    local function subdivide(v1, v2, v3, depth)
        if depth == 0 then
            gol.vertex3(v1[1], v1[2], v1[3])
            gol.vertex3(v2[1], v2[2], v2[3])
            gol.vertex3(v3[1], v3[2], v3[3])
        else
            local v12 = mid_point(v1, v2)
            local v23 = mid_point(v2, v3)
            local v31 = mid_point(v3, v1)

            subdivide(v1, v12, v31, depth - 1)
            subdivide(v2, v23, v12, depth - 1)
            subdivide(v3, v31, v23, depth - 1)
            subdivide(v12, v23, v31, depth - 1)
        end
    end

    local t = (1 + math.sqrt(5)) / 2

    local vertices = {
        normalize_and_size({ -1,  t,  0 }),
        normalize_and_size({  1,  t,  0 }),
        normalize_and_size({ -1, -t,  0 }),
        normalize_and_size({  1, -t,  0 }),
        normalize_and_size({  0, -1,  t }),
        normalize_and_size({  0,  1,  t }),
        normalize_and_size({  0, -1, -t }),
        normalize_and_size({  0,  1, -t }),
        normalize_and_size({  t,  0, -1 }),
        normalize_and_size({  t,  0,  1 }),
        normalize_and_size({ -t,  0, -1 }),
        normalize_and_size({ -t,  0,  1 })
    }

    local faces = {
        {1, 12, 6}, {1, 6, 2}, {1, 2, 8}, {1, 8, 11}, {1, 11, 12},
        {2, 6, 10}, {6, 12, 5}, {12, 11, 3}, {11, 8, 7}, {8, 2, 9},
        {4, 10, 5}, {4, 5, 3}, {4, 3, 7}, {4, 7, 9}, {4, 9, 10},
        {5, 10, 6}, {3, 5, 12}, {7, 3, 11}, {9, 7, 8}, {10, 9, 2}
    }

    gol.begin_triangles()
    for _, face in ipairs(faces) do
		
        local v1 = vertices[face[1]]
        local v2 = vertices[face[2]]
        local v3 = vertices[face[3]]
        subdivide( v1, v2, v3, subdivisions or 1 )
    end
    gol.do_end()
end

-- experiment using chatgpt: really bad
function meu:draw_human_face(subdivisions)
    local vertices = {
        -- Define vertices for a simplified human face (x, y, z)
        {0.0,  0.6,  0.0},  -- Top of the head
        {0.3,  0.4,  0.1},  -- Right forehead
        {-0.3,  0.4,  0.1},  -- Left forehead
        {0.4,  0.0,  0.2},  -- Right cheek
        {-0.4,  0.0,  0.2},  -- Left cheek
        {0.2, -0.5,  0.1},  -- Right jaw
        {-0.2, -0.5,  0.1}, -- Left jaw
        {0.0, -0.6,  0.0},  -- Chin
        {0.1,  0.2,  0.2},  -- Right eye outer corner
        {-0.1,  0.2,  0.2},  -- Left eye outer corner
        {0.05,  0.15,  0.2}, -- Right eye inner corner
        {-0.05,  0.15,  0.2},-- Left eye inner corner
        {0.0,  0.0,  0.25}, -- Nose tip
        {0.1, -0.2,  0.15}, -- Right mouth corner
        {-0.1, -0.2,  0.15}, -- Left mouth corner
        {0.0, -0.25,  0.15}, -- Mouth center
        {0.2,  0.0, -0.2},  -- Right back of head
        {-0.2,  0.0, -0.2},  -- Left back of head
        {0.0,  0.6, -0.2},  -- Top back of head
        {0.0, -0.6, -0.2}   -- Bottom back of head
    }

    local faces = {
        -- Define faces using indices to vertices
        -- Front face
        {1, 2, 9}, {1, 9, 10}, {1, 10, 3}, {3, 10, 5}, {2, 4, 9}, 
        {4, 12, 9}, {3, 5, 13}, {5, 7, 13}, {4, 6, 13}, {6, 8, 13}, 
        {7, 8, 15}, {13, 14, 15}, {2, 4, 17}, {3, 5, 18},
        
        -- Side faces
        {1, 2, 19}, {1, 3, 19}, {2, 4, 17}, {3, 5, 18}, {4, 17, 20}, 
        {5, 18, 20}, {6, 7, 20}, {6, 17, 20}, {7, 18, 20}, {8, 6, 20}, 
        {8, 7, 20},
        
        -- Back face
        {17, 18, 19}, {17, 18, 20}, {19, 18, 17}, {19, 20, 18}, {19, 17, 20}
    }

    local function normalize(v)
        local len = math.sqrt(v[1]^2 + v[2]^2 + v[3]^2)
        return {v[1] / len, v[2] / len, v[3] / len}
    end

    local function mid_point(v1, v2)
        return {
            (v1[1] + v2[1]) / 2,
            (v1[2] + v2[2]) / 2,
            (v1[3] + v2[3]) / 2
        }
    end

    local function subdivide(vertices, faces, depth)
        if depth == 0 then
            return vertices, faces
        end

        local new_faces = {}
        local mid_cache = {}

        local function get_mid_point(v1_idx, v2_idx)
            local key = v1_idx < v2_idx and {v1_idx, v2_idx} or {v2_idx, v1_idx}
            local cached = mid_cache[key]
            if cached then
                return cached
            end
            local mid = mid_point(vertices[v1_idx], vertices[v2_idx])
            table.insert(vertices, mid)
            local mid_idx = #vertices
            mid_cache[key] = mid_idx
            return mid_idx
        end

        for _, face in ipairs(faces) do
            local v1, v2, v3 = face[1], face[2], face[3]
            local v12 = get_mid_point(v1, v2)
            local v23 = get_mid_point(v2, v3)
            local v31 = get_mid_point(v3, v1)

            table.insert(new_faces, {v1, v12, v31})
            table.insert(new_faces, {v2, v23, v12})
            table.insert(new_faces, {v3, v31, v23})
            table.insert(new_faces, {v12, v23, v31})
        end

        return subdivide(vertices, new_faces, depth - 1)
    end

    vertices, faces = subdivide(vertices, faces, subdivisions or 1)

    gol.begin_triangles()
    for _, face in ipairs(faces) do
        local v1 = vertices[face[1]]
        local v2 = vertices[face[2]]
        local v3 = vertices[face[3]]
        gol.vertex3(v1[1], v1[2], v1[3])
        gol.vertex3(v2[1], v2[2], v2[3])
        gol.vertex3(v3[1], v3[2], v3[3])
    end
    gol.do_end()
end


-- Metaball parameters
local metaballs = {
    {pos = {0.0, 0.0, 0.0}, radius = 1.0},
    {pos = {2.0, 0.0, 0.0}, radius = 1.0},
    {pos = {1.0, 1.0, 0.0}, radius = 1.0},
}

local function metaball_field( x,y,z, th )
    local value = 0
    for _, mb in ipairs(metaballs) do
        local dx = x - mb.pos[1]
        local dy = y - mb.pos[2]
        local dz = z - mb.pos[3]
        value = value + mb.radius2 / (dx*dx + dy*dy + dz*dz)
		if value > th then
			return true
		end
    end
    return false
end

function meu:draw_metaballs( metaballs )
    local grid_size = 0.1
    local threshold = 1.5
    
    gol.begin_points()
    for x = -3, 3, grid_size do
        for y = -3, 3, grid_size do
            for z = -3, 3, grid_size do	
				if metaball_field( x,y,z, threshold ) then
                    gol.vertex3(x, y, z)
                end
            end
        end
    end
    gol.do_end()
end

function meu:place_metaballs( nb, ph )
    -- Update metaball positions for animation
	local su, sv = self.size.u, self.size.v
	local sw = math.min( su, sv )
	for i=1,nb do
		local mb = metaballs[i]
		if not mb then
			mb = {}
			metaballs[i] = mb
		end
    	mb.pos = { su*(math.random()-.5), sv*(math.random()-.5), sw*(math.random()-.5)}
		local id = math.random(3) 
		mb.pos[id] = mb.pos[id] + (ph - math.floor(ph)) * (mb.pos[id]>0 and 1 or -1) * 2
		mb.radius = math.random()
		mb.radius2 = mb.radius * mb.radius  
	end
end

-- Utility function to rotate a vector by given angles
local function rotate_vector(x, y, z, angle_x, angle_y)
    local cos_x = math.cos(angle_x)
    local sin_x = math.sin(angle_x)
    local cos_y = math.cos(angle_y)
    local sin_y = math.sin(angle_y)

    local new_x = x * cos_y + z * sin_y
    local new_y = y * cos_x - (x * sin_y - z * cos_y) * sin_x
    local new_z = z * cos_y - x * sin_y

    return new_x, new_y, new_z
end

-- Function to normalize a vector
local function normalize(v)
    local over = 1. / math.sqrt(v[1]^2 + v[2]^2 + v[3]^2)
    return { v[1]*over, v[2]*over, v[3]*over }
end

-- Function to interpolate between a cube vertex and its corresponding sphere vertex
local function interpolate_vertex( v, t, size )
    local nv = normalize(v)
    local t1 = 1 - t
    local t2 = size * t
    return {
        v[1] * t1 + nv[1] * t2,
        v[2] * t1 + nv[2] * t2,
        v[3] * t1 + nv[3] * t2
    }
end


-- Function to generate a grid of points on a face and convert to triangles
local function generate_face_triangles( v1, v2, v3, v4, resolution )
    local points = {}
    local triangles = {}
    local over = 1. / resolution
    for i = 0, resolution do
        points[i+1] = {}
        local u = i * over
        local u_m = 1-u
        for j = 0, resolution  do
            local v = j * over
            local v_m = 1-v
            local a,b,c,d = u_m * v_m, u * v_m, u * v, u_m * v
            points[i+1][j+1] = {
                v1[1] * a + v2[1] * b + v3[1] * c + v4[1] * d,
                v1[2] * a + v2[2] * b + v3[2] * c + v4[2] * d,
                v1[3] * a + v2[3] * b + v3[3] * c + v4[3] * d
            }
        end
    end

    for i = 1, resolution do
        local p1 = points[i]  [1]
        local p2 = points[i+1][1]
        for j = 1, resolution do
            local p3 = points[i]  [j+1]
            local p4 = points[i+1][j+1]
            table.insert( triangles, p1 )
            table.insert( triangles, p2 )
            table.insert( triangles, p3 )
            table.insert( triangles, p2 )
            table.insert( triangles, p4 )
            table.insert( triangles, p3 )
            p1,p2 = p3,p4 
        end
    end
    return triangles
end

-- Function to draw the interpolated shape
local function draw_interpolated_shape( center, size, t, resolution )
    -- Define the vertices of the cube
    local cube_vertices = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},  -- Front face
        {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}       -- Back face
    }

    -- Translate and scale the vertices to the desired size and position
    local sh = size * 0.5
    for i, v in ipairs(cube_vertices) do
        cube_vertices[i] = {
            center[1] + v[1] * sh,
            center[2] + v[2] * sh,
            center[3] + v[3] * sh
        }
    end

    -- Define the faces of the cube
    local faces = {
        {4, 3, 2, 1},  -- Front face
        {5, 6, 7, 8},  -- Back face
        {1, 2, 6, 5},  -- Bottom face
        {2, 3, 7, 6},  -- Right face
        {3, 4, 8, 7},  -- Top face
        {4, 1, 5, 8}   -- Left face
    }

    -- Generate and interpolate points for each face
    local all_triangles = {}
    for _, face in ipairs(faces) do
        local v1 = cube_vertices[face[1]]
        local v2 = cube_vertices[face[2]]
        local v3 = cube_vertices[face[3]]
        local v4 = cube_vertices[face[4]]
        local face_triangles = generate_face_triangles( v1,v2,v3,v4, resolution )
        for _, p in ipairs(face_triangles) do
            local interpolated = interpolate_vertex( p, t, size )
            table.insert( all_triangles, interpolated )
        end
    end

    -- Draw the triangles
    local vertices = {}
    for _, p in ipairs(all_triangles) do
        table.insert( vertices, p[1] )
        table.insert( vertices, p[2] )
        table.insert( vertices, p[3] )
    end
    gol.draw_triangles_3d( vertices )
end

function meu:draw_tree()
    -- Example usage
    local center = {0, 0, 0}
    local size = 2
    local t = 4 * 0.5 * ( 1 + math.cos(aaa.time.t) )    -- Interpolation parameter (0 = cube, 1 = sphere)
    local resolution = 8  -- Resolution of the subdivision

    --t = 0
    draw_interpolated_shape( center, size/(t+1), t, resolution )
end

function meu:draw()	
	self:draw_layers_begin()

		self:draw_layer( 1 )	--draw all layers in layers
		--self:draw_mondrian_maa( 0,0, self.size.u, self.size.v, aaa.time.t, .1 )

        if false then
            self:draw_tree()
		else
            if inside( self.s_what, 1,2 ) then
                if self.s_what==1 then	self:draw_mondrian_maa()
                else					self:draw_mondrian( 0,0, self.size.u,self.size.v, aaa.time.t, .1 )
                end
            else
                local seed = math.floor( self.ph )
                if seed ~= self.seed then
                    math.randomseed( seed )
                end
                if self.s_what==3 then
                    self:draw_geodesic( 4 )
                elseif self.s_what==4 then
                    self:draw_human_face(0)
                elseif self.s_what==5 then
                    local m = self:place_metaballs( 12, aaa.time.t )
                    self:draw_metaballs( m )
                end
            end
        end

	self:draw_layers_end()
end
