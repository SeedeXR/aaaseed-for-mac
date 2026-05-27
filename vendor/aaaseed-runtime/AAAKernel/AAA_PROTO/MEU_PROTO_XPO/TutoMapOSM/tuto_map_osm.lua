------------------------------------------------------------------------
--	MEU TutoMapOSM — OpenStreetMap dashboard with city tour
--	Center: slippy map   Left/Right: data panels at 45°
------------------------------------------------------------------------

local TILE_URL    = "https://tile.openstreetmap.org/%d/%d/%d.png"
local USER_AGENT  = "AAASeed/1.0"
local MAX_ZOOM, MIN_ZOOM = 18, 2
local CACHE_ROOT  = nil

------------------------------------------------------------------------
--	Ile-de-France cities (pre-sorted by population)
------------------------------------------------------------------------

local IDF_CITIES = {
	{ name="Paris",                  lat=48.8589, lon=2.347,   pop=2103778, surf=10536, dept="75" },
	{ name="Saint-Denis",            lat=48.9378, lon=2.3657,  pop=149077,  surf=1577,  dept="93" },
	{ name="Boulogne-Billancourt",   lat=48.8375, lon=2.2429,  pop=119019,  surf=615,   dept="92" },
	{ name="Montreuil",              lat=48.8637, lon=2.4491,  pop=111934,  surf=891,   dept="93" },
	{ name="Nanterre",               lat=48.8974, lon=2.2018,  pop=97783,   surf=1222,  dept="92" },
	{ name="Vitry-sur-Seine",        lat=48.7893, lon=2.3951,  pop=93963,   surf=1166,  dept="94" },
	{ name="Creteil",                lat=48.7845, lon=2.4523,  pop=93397,   surf=1142,  dept="94" },
	{ name="Asnieres-sur-Seine",     lat=48.9181, lon=2.2935,  pop=93941,   surf=482,   dept="92" },
	{ name="Colombes",               lat=48.9218, lon=2.2469,  pop=91053,   surf=778,   dept="92" },
	{ name="Aubervilliers",          lat=48.9128, lon=2.3886,  pop=88365,   surf=577,   dept="93" },
	{ name="Aulnay-sous-Bois",       lat=48.9457, lon=2.4918,  pop=87599,   surf=1615,  dept="93" },
	{ name="Courbevoie",             lat=48.8976, lon=2.2574,  pop=82902,   surf=416,   dept="92" },
	{ name="Rueil-Malmaison",        lat=48.8717, lon=2.1806,  pop=82874,   surf=1453,  dept="92" },
	{ name="Champigny-sur-Marne",    lat=48.8173, lon=2.5206,  pop=78072,   surf=1130,  dept="94" },
	{ name="Saint-Maur-des-Fosses",  lat=48.7999, lon=2.4921,  pop=76572,   surf=1126,  dept="94" },
	{ name="Drancy",                 lat=48.925,  lon=2.4446,  pop=72390,   surf=776,   dept="93" },
	{ name="Noisy-le-Grand",         lat=48.8327, lon=2.556,   pop=72978,   surf=1313,  dept="93" },
	{ name="Levallois-Perret",       lat=48.8946, lon=2.2874,  pop=68092,   surf=242,   dept="92" },
	{ name="Issy-les-Moulineaux",    lat=48.824,  lon=2.2628,  pop=67669,   surf=424,   dept="92" },
	{ name="Ivry-sur-Seine",         lat=48.8125, lon=2.3872,  pop=65064,   surf=611,   dept="94" },
}

------------------------------------------------------------------------
--	tile math
------------------------------------------------------------------------

local function lon_to_tile_x( lon, z )   return math.floor( (lon+180)/360 * 2^z ) end
local function lat_to_tile_y( lat, z )
	local r = math.rad(lat)
	return math.floor( (1 - math.log(math.tan(r)+1/math.cos(r))/math.pi)/2 * 2^z )
end
local function lon_to_tile_xf( lon, z )  return (lon+180)/360 * 2^z end
local function lat_to_tile_yf( lat, z )
	local r = math.rad(lat)
	return (1 - math.log(math.tan(r)+1/math.cos(r))/math.pi)/2 * 2^z
end
local function tile_key( z, x, y )       return z.."/"..x.."/"..y end

------------------------------------------------------------------------
--	MEU
------------------------------------------------------------------------

function meu:define_meu_infos()
	return { author = "Louis Montagne", date = "2026",
		tags = { "Tutorial", "Experimental", "Unfinished" },
		help = "OpenStreetMap dashboard — city tour Ile-de-France"
	}
end

function meu:init()
	self.city_index   = 1
	self.lat          = IDF_CITIES[1].lat
	self.lon          = IDF_CITIES[1].lon
	self.zoom         = 14
	self.grid_n       = 2

	--	animation
	self.anim_lat     = self.lat
	self.anim_lon     = self.lon
	self.anim_speed   = 2.0		-- lerp speed
	self.auto_tour    = false
	self.tour_timer   = 0
	self.tour_pause   = 5		-- seconds per city

	--	tiles
	self.tiles         = {}
	self.download_queue = {}
	self.bind_pool     = {}		-- pre-allocated bind slots
	self.bind_next     = 0
	self.bind_max      = 200

	--	cache
	local cwd = aaa.dir and aaa.dir.get_def and aaa.dir.get_def() or "."
	if cwd and cwd:find("AAAKernel") then cwd = cwd:gsub("/AAAKernel.*$", "") end
	CACHE_ROOT = cwd .. "/Pref/map_tiles"
	os.execute( 'mkdir -p "' .. CACHE_ROOT .. '"' )

	self:download_all_tiles_for_view()
end

function meu:define_ui()
	local ix, iy = 1, 1
	local SY, DY = 1, 0.25

	--	camera & rendering go to the right column automatically
	self:add_camera()
	self:add_rendering()

	--	left column: navigation controls
	self:add_trig_method( {ix,   iy, 2, SY}, "Prev",  self, "prev_city" )
	self:add_trig_method( {ix+2, iy, 2, SY}, "Next",  self, "next_city" )
	self.ui.bu_tour = self:add_trig_method( {ix+4, iy, 4, SY}, "Tour OFF", self, "toggle_tour" )
		self.ui.bu_tour:set_color_back("x")
	iy = iy + SY + DY

	self:add_trig_method( {ix,   iy, 2, SY}, "Zoom+", self, "zoom_in"  ):set_color_back("z")
	self:add_trig_method( {ix+2, iy, 2, SY}, "Zoom-", self, "zoom_out" ):set_color_back("z")
	self:add_slider( {ix+4, iy, 4, SY}, "Speed", self, "tour_pause", 5, 1, 15 )
	iy = iy + SY + DY

	self.ui.bu_city = self:add_text_info( {ix, iy, 8, SY}, "Paris" )
	iy = iy + SY + DY

	--	transfo below city name, right column
	self:add_transfo( {9, iy, 8, SY*2+DY} )
end

------------------------------------------------------------------------
--	city navigation
------------------------------------------------------------------------

function meu:goto_city( idx )
	self.city_index = ((idx - 1) % #IDF_CITIES) + 1
	local c = IDF_CITIES[self.city_index]
	self.lat = c.lat
	self.lon = c.lon
	if self.ui.bu_city then
		self.ui.bu_city:set_text( c.name .. "  (" .. c.dept .. ")" )
	end
	self:download_all_tiles_for_view()
end

function meu:next_city()  self:goto_city( self.city_index + 1 ) end
function meu:prev_city()  self:goto_city( self.city_index - 1 ) end
function meu:toggle_tour()
	self.auto_tour = not self.auto_tour
	self.tour_timer = 0
	if self.ui.bu_tour then
		if self.auto_tour then
			self.ui.bu_tour:set_color_back("load")		-- green = running
			self.ui.bu_tour:set_text("Tour ON")
		else
			self.ui.bu_tour:set_color_back("x")			-- red = stopped
			self.ui.bu_tour:set_text("Tour OFF")
		end
	end
end

function meu:zoom_in()
	if self.zoom < MAX_ZOOM then self.zoom = self.zoom + 1; self:request_visible_tiles() end
end
function meu:zoom_out()
	if self.zoom > MIN_ZOOM then self.zoom = self.zoom - 1; self:request_visible_tiles() end
end

------------------------------------------------------------------------
--	tile management
------------------------------------------------------------------------

function meu:alloc_bind()
	--	recycle from pool or allocate new
	local idx = (self.bind_next % self.bind_max) + 1
	self.bind_next = self.bind_next + 1
	if not self.bind_pool[idx] then
		local b = IMGS.get_bind_free()
		if b then
			IMGS.inc_bind_free( 1 )
			self.bind_pool[idx] = b
		else
			return nil
		end
	end
	return self.bind_pool[idx]
end

function meu:request_visible_tiles()
	if not CACHE_ROOT then return end
	local z, n = self.zoom, self.grid_n
	local cx = lon_to_tile_x( self.lat and self.lon or 2.35, z )
	local cy = lat_to_tile_y( self.lat or 48.86, z )
	local mx = 2^z - 1
	for dy = -n, n do for dx = -n, n do
		local tx, ty = cx+dx, cy+dy
		if tx >= 0 and tx <= mx and ty >= 0 and ty <= mx then
			self:ensure_tile( z, tx, ty )
		end
	end end
end

function meu:preload_next_city()
	local next_idx = (self.city_index % #IDF_CITIES) + 1
	local c = IDF_CITIES[next_idx]
	if not c or not CACHE_ROOT then return end

	local z, n = self.zoom, self.grid_n
	local cx = lon_to_tile_x( c.lon, z )
	local cy = lat_to_tile_y( c.lat, z )
	local mx = 2^z - 1

	--	download missing tiles to disk (background shell)
	local cmds = {}
	for dy = -n, n do for dx = -n, n do
		local tx, ty = cx+dx, cy+dy
		if tx >= 0 and tx <= mx and ty >= 0 and ty <= mx then
			local fname = z.."_"..tx.."_"..ty..".png"
			local path  = CACHE_ROOT .. "/" .. fname
			if not aaa.file.is_exist( path ) then
				table.insert( cmds, string.format(
					'curl -s -L -A "%s" -o "%s" "%s"',
					USER_AGENT, path,
					string.format( TILE_URL, z, tx, ty )
				))
			end
		end
	end end

	if #cmds > 0 then
		--	run in background so it doesn't freeze the current frame
		os.execute( "(" .. table.concat( cmds, " && " ) .. ") &" )
	end
end

function meu:download_all_tiles_for_view()
	if not CACHE_ROOT then return end
	local z, n = self.zoom, self.grid_n
	local cx = lon_to_tile_x( self.lon, z )
	local cy = lat_to_tile_y( self.lat, z )
	local mx = 2^z - 1

	--	build a single shell command that downloads all missing tiles
	local cmds = {}
	for dy = -n, n do for dx = -n, n do
		local tx, ty = cx+dx, cy+dy
		if tx >= 0 and tx <= mx and ty >= 0 and ty <= mx then
			local key   = tile_key( z, tx, ty )
			local fname = z.."_"..tx.."_"..ty..".png"
			local path  = CACHE_ROOT .. "/" .. fname
			if not self.tiles[key] and not aaa.file.is_exist( path ) then
				table.insert( cmds, string.format(
					'curl -s -L -A "%s" -o "%s" "%s"',
					USER_AGENT, path,
					string.format( TILE_URL, z, tx, ty )
				))
			end
		end
	end end

	--	execute all downloads (sequential but in one shell call)
	if #cmds > 0 then
		os.execute( table.concat( cmds, " && " ) )
	end

	--	now load all tiles into GPU
	for dy = -n, n do for dx = -n, n do
		local tx, ty = cx+dx, cy+dy
		if tx >= 0 and tx <= mx and ty >= 0 and ty <= mx then
			self:ensure_tile( z, tx, ty )
		end
	end end
end

function meu:ensure_tile( z, tx, ty )
	local key = tile_key( z, tx, ty )
	if self.tiles[key] then return end
	local fname = z.."_"..tx.."_"..ty..".png"
	local path  = CACHE_ROOT .. "/" .. fname
	if aaa.file.is_exist( path ) then
		local bind = self:alloc_bind()
		aaa.img.read( bind, path, false, false, false, false )
		aaa.img.move_to_gpu( bind )
		self.tiles[key] = { bind = bind, state = "ready" }
		return
	end
	self.tiles[key] = { state = "queued" }
	table.insert( self.download_queue, { key=key, z=z, tx=tx, ty=ty, path=path } )
end

------------------------------------------------------------------------
--	update
------------------------------------------------------------------------

function meu:update()
	local dt = aaa.time.dt or 0.016

	--	download one tile per frame
	local q = self.download_queue
	if q and #q > 0 then
		local item = table.remove( q, 1 )
		local url = string.format( TILE_URL, item.z, item.tx, item.ty )
		os.execute( string.format('curl -s -L -A "%s" -o "%s" "%s"', USER_AGENT, item.path, url) )
		if aaa.file.is_exist( item.path ) then
			local bind = self:alloc_bind()
			aaa.img.read( bind, item.path, false, false, false, false )
			aaa.img.move_to_gpu( bind )
			local t = self.tiles[item.key]
			if t then t.bind = bind; t.state = "ready" end
		end
	end

	--	smooth animation
	self.anim_lat = self.anim_lat + (self.lat - self.anim_lat) * math.min(1, self.anim_speed * dt)
	self.anim_lon = self.anim_lon + (self.lon - self.anim_lon) * math.min(1, self.anim_speed * dt)

	--	auto tour
	if self.auto_tour then
		self.tour_timer = self.tour_timer + dt

		--	at half-time, preload next city tiles (download only, no GPU load yet)
		if not self.preloaded_next and self.tour_timer >= self.tour_pause * 0.4 then
			self:preload_next_city()
			self.preloaded_next = true
		end

		if self.tour_timer >= self.tour_pause then
			self.tour_timer = 0
			self.preloaded_next = false
			self:next_city()
		end
	end
end

------------------------------------------------------------------------
--	draw helpers
------------------------------------------------------------------------

local function format_pop( n )
	if n >= 1000000 then return string.format("%.1fM", n/1000000)
	elseif n >= 1000 then return string.format("%.0fk", n/1000)
	else return tostring(n) end
end

local function draw_bar_chart( data, max_val, x0, y0, w, h, alpha, highlight_idx )
	local n = #data
	if n == 0 then return end
	local bar_w = w / n * 0.8
	local gap   = w / n * 0.2
	for i, d in ipairs( data ) do
		local bx = x0 + (i-1) * (bar_w + gap)
		local ratio = d.val / max_val
		local bh = ratio * h

		if i == highlight_idx then
			gol.color( 0.9, 0.3, 0.2, alpha )
		else
			gol.color( 0.3, 0.5, 0.8, alpha * 0.8 )
		end
		aaa.draw_rect( bx, y0, bx + bar_w, y0 + bh )

		--	label
		gol.color( 1, 1, 1, alpha * 0.7 )
		local short = d.label:sub(1, 8)
		BU:draw_text_nice( short, bx, y0 - 0.15, 0, 0.045, 0.045 )
	end
end

local function draw_big_number( text, x, y, alpha, r, g, b )
	gol.color( r or 1, g or 1, b or 1, alpha )
	BU:draw_text_nice( text, x, y, 0, 0.22, 0.22 )
end

local function draw_label( text, x, y, alpha )
	gol.color( 0.7, 0.7, 0.8, alpha )
	BU:draw_text_nice( text, x, y, 0, 0.08, 0.08 )
end

------------------------------------------------------------------------
--	draw
------------------------------------------------------------------------

function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 )

		local alpha = self:get_alpha()
		local city  = IDF_CITIES[self.city_index]

		local map_half  = self.grid_n + 0.5
		local panel_w   = 3.6
		local z_fold    = 1.5				-- how far forward the outer edges come

		--	===== CENTER MAP =====
		self:draw_map( alpha )

		local angle = 0.38

		--	===== LEFT PANEL  \ _ /  =====
		gol.push_matrix()
			gol.translate( -map_half - panel_w/2, 0, -z_fold * 0.5 )
			gol.rotate_y( -angle )
			gol.scale( -1, 1, 1 )		-- un-mirror text after rotation
			self:draw_panel_left( alpha, city )
		gol.pop_matrix()

		--	===== RIGHT PANEL (flat, overlapping map slightly) =====
		gol.push_matrix()
			gol.translate( map_half + panel_w/2 - 1.2, 0, 0 )
			self:draw_panel_right( alpha, city )
		gol.pop_matrix()

	self:draw_layers_end()
end

------------------------------------------------------------------------
--	left panel: population
------------------------------------------------------------------------

function meu:draw_panel_left( alpha, city )
	--	background
	gol.color( 0.08, 0.08, 0.12, alpha * 0.85 )
	aaa.draw_rect( -1.8, -2.5, 1.8, 2.5 )

	--	city name
	gol.color( 1, 0.9, 0.3, alpha )
	BU:draw_text_nice( city.name, -1.5, 1.8, 0, 0.18, 0.20 )

	--	department
	draw_label( "Dept " .. city.dept .. " — Ile-de-France", -1.5, 1.5, alpha )

	--	population big number
	draw_label( "POPULATION", -1.5, 1.1, alpha )
	draw_big_number( format_pop( city.pop ), -1.5, 0.7, alpha, 0.4, 0.8, 1.0 )

	--	surface
	draw_label( "SURFACE", -1.5, 0.2, alpha )
	local surf_str = string.format( "%.1f ha", city.surf / 100 )
	draw_big_number( surf_str, -1.5, -0.2, alpha, 0.5, 0.9, 0.5 )

	--	density
	local density = city.pop / (city.surf / 10000)	-- hab/km²
	draw_label( "DENSITE", -1.5, -0.7, alpha )
	draw_big_number( string.format("%.0f hab/km2", density), -1.5, -1.1, alpha, 1.0, 0.6, 0.3 )

	--	bar chart: top 8 cities by pop
	local chart_data = {}
	local max_pop = 0
	local hi = 1
	for i = 1, math.min(8, #IDF_CITIES) do
		local c = IDF_CITIES[i]
		table.insert( chart_data, { label = c.name, val = c.pop } )
		if c.pop > max_pop then max_pop = c.pop end
		if c.name == city.name then hi = i end
	end
	draw_label( "TOP VILLES IDF", -1.5, -1.5, alpha )
	draw_bar_chart( chart_data, max_pop, -1.5, -2.3, 3.0, 0.7, alpha, hi )
end

------------------------------------------------------------------------
--	right panel: ranking & density comparison
------------------------------------------------------------------------

function meu:draw_panel_right( alpha, city )
	--	background
	gol.color( 0.08, 0.08, 0.12, alpha * 0.85 )
	aaa.draw_rect( -1.8, -2.5, 1.8, 2.5 )

	--	ranking
	gol.color( 1, 0.9, 0.3, alpha )
	BU:draw_text_nice( "CLASSEMENT IDF", -1.5, 1.8, 0, 0.14, 0.14 )

	draw_label( "RANG POPULATION", -1.5, 1.4, alpha )
	draw_big_number( "#" .. self.city_index, -1.5, 1.0, alpha, 0.4, 0.8, 1.0 )
	draw_label( "sur " .. #IDF_CITIES .. " villes", 0.5, 1.05, alpha )

	--	position in tour
	if self.auto_tour then
		gol.color( 0.3, 1, 0.3, alpha )
		BU:draw_text_nice( "TOUR EN COURS", -1.5, 0.5, 0, 0.10, 0.10 )
		--	progress bar
		local progress = self.tour_timer / self.tour_pause
		gol.color( 0.2, 0.2, 0.3, alpha )
		aaa.draw_rect( -1.5, 0.2, 1.5, 0.35 )
		gol.color( 0.3, 0.8, 0.3, alpha )
		aaa.draw_rect( -1.5, 0.2, -1.5 + 3.0 * progress, 0.35 )
	else
		draw_label( "Appuyer TOUR pour demarrer", -1.5, 0.5, alpha )
	end

	--	density bar chart for nearby cities
	local chart_data = {}
	local max_dens = 0
	local hi = 1
	local start = math.max(1, self.city_index - 3)
	local stop  = math.min(#IDF_CITIES, start + 7)
	for i = start, stop do
		local c = IDF_CITIES[i]
		local d = c.pop / (c.surf / 10000)
		table.insert( chart_data, { label = c.name, val = d } )
		if d > max_dens then max_dens = d end
		if c.name == city.name then hi = i - start + 1 end
	end
	draw_label( "DENSITE (hab/km2)", -1.5, -0.1, alpha )
	draw_bar_chart( chart_data, max_dens, -1.5, -0.9, 3.0, 0.7, alpha, hi )

	--	coordinates
	draw_label( string.format("lat: %.4f  lon: %.4f  z:%d", city.lat, city.lon, self.zoom), -1.5, -1.5, alpha )

	--	OSM attribution
	gol.color( 1, 1, 1, alpha * 0.3 )
	BU:draw_text_nice( "OpenStreetMap contributors", -1.5, -2.3, 0, 0.05, 0.05 )
end

------------------------------------------------------------------------
--	center: map
------------------------------------------------------------------------

function meu:draw_map( alpha )
	local z = self.zoom
	local n = self.grid_n
	local cxf = lon_to_tile_xf( self.anim_lon, z )
	local cyf = lat_to_tile_yf( self.anim_lat, z )
	local cx  = math.floor( cxf )
	local cy  = math.floor( cyf )
	local ox  = cxf - cx
	local oy  = cyf - cy
	local ts  = 1.0
	local mx  = 2^z - 1

	for dy = -n, n do for dx = -n, n do
		local tx, ty = cx+dx, cy+dy
		local sx = (dx - ox) * ts
		local sy = -(dy - oy) * ts

		if tx >= 0 and tx <= mx and ty >= 0 and ty <= mx then
			local key = tile_key( z, tx, ty )
			local t   = self.tiles[key]
			if t and t.state == "ready" and t.bind then
				gol.color_white( alpha )
				gol.push_matrix()
				gol.translate( sx, sy, 0 )
				gol.set_texture_dim( 2 )
				gol.bind_texture( t.bind )
				gol.set_quad_uv()
				aaa.draw_rect_uv( -ts/2, -ts/2, ts/2, ts/2 )
				gol.pop_matrix()
			else
				gol.color( 0.15, 0.15, 0.2, alpha * 0.5 )
				aaa.draw_rect( sx-ts/2, sy-ts/2, sx+ts/2, sy+ts/2 )
			end
		end
	end end

	--	crosshair
	gol.color( 1, 0.3, 0.3, alpha * 0.6 )
	gol.set_line_width( 2 )
	gol.draw_lines_2d( -0.12, 0, 0.12, 0 )
	gol.draw_lines_2d( 0, -0.12, 0, 0.12 )
	gol.set_line_width( 1 )

	--	city label under map
	local city = IDF_CITIES[self.city_index]
	gol.color_white( alpha )
	BU:draw_text_nice( city.name, -0.5, -n*ts - 0.2, 0, 0.08, 0.08 )
end
