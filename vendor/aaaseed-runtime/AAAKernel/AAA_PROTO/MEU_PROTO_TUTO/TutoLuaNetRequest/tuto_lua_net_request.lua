function meu:define_meu_infos()
	return { author = "Benjamin", date = "2025",
			   tags = { "Tutorial", "Network" },
			   help = "Basic example of how to execute HTTP requests and examine responses"
			 }
end

function meu:init()
	self.request_id = 0
	self.requests = {}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = 1
	local DY = .4

	self:add_camera()
	self:add_rendering()
	
	local SX = 8 / 4
	self:add_trig_method( 	{ix,		iy,	SX,SY}, "None",			self,"begin_example"	):set_color_back("load")
	self:add_trig_method( 	{ix+SX,		iy,	SX,SY}, "Paris",		self,"begin_example",	"Paris"		)
	self:add_trig_method( 	{ix+SX*2,	iy,	SX,SY}, "Brussels",		self,"begin_example",	"Brussels"	)
	self:add_trig_method( 	{ix+SX*3,	iy,	SX,SY}, "Vancouver",	self,"begin_example",	"Vancouver"	)
	iy = iy + SY + DY

	bu = self:add_text(		{ix,iy,     16,SY}, "URL"       	)
		ui.url = bu
	iy = iy + SY
	self:add_text(  		{ix,iy,     16,SY}, "Payload"   	)
	iy = iy + SY + DY
	self:add_text(  		{ix,iy,     6,SY},  "Username"  	)
	self:add_text(      	{ix+6,iy,   6,SY},  "Password"  	)
	iy = iy + SY
	self:add_text_info( 	{ix,iy,     3,SY}, "Set Auth:"		)
	self:add_trig_method( 	{ix+3,iy,	3,SY}, "BASIC",			self,"set_basic_auth"	)
	self:add_trig_method(	{ix+6,iy,	3,SY}, "DIGEST",		self,"set_digest_auth"	)
	self:add_trig_method(	{ix+9,iy,	3,SY}, "NTLM",			self,"set_ntlm_auth"	)
	iy = iy + SY + DY

	self:add_text(			{ix,iy,     16,SY}, "Bearer Token"	)
	iy = iy + SY
	self:add_text_info(		{ix,iy,     3,SY}, "Set OAuth:"		)
	self:add_trig_method(	{ix+3,iy,   3,SY}, "OAUTH",			self,"set_oauth"		)
	iy = iy + SY
	self:add_trig_method(	{ix,iy,		4,SY}, "CLEAR AUTH",	aaa.net.requests,"clear_auth"	)
	self:add_trig_method(	{ix+4,iy,	4,SY}, "ENABLE AUTH",	aaa.net.requests,"enable_auth"	)
	self:add_trig_method(	{ix+8,iy,	4,SY}, "DISABLE AUTH",	aaa.net.requests,"disable_auth"	)
	iy = iy + SY + DY

	local sx = 16/6

	for i,str in ipairs{ "Get", "Post", "Put", "Patch", "Download", "Delete" } do
		self:add_trig_method(	{ix+sx*(i-1),iy,	sx,SY},	str,	self, string.lower(str).."_request"		)
	end
	iy = iy + SY

	self:add_text_info(		{ix,iy,			16,SY},	"Response"		)
	iy = iy + SY + DY

	self:add_transfo(		{ix+8,iy-DY,		8,SY*2+DY}	)
end

-- we store mainly to debug, it is not needed
function meu:store_request( id, what, url, payload, header )
	local t = { what=what, url=url, payload=payload, header=header }
	self:print( "net request "..what.." with url: "..url )
	if payload then
		self:print( "  with Payload: "..payload )
	end
	if header then
		self:print( "  with Header: "..header )
	end	
	t.request_id = id
	self:print( "Request ID: "..id )
	self.requests[id] = t
	return t
end

--//{"accept", "application/json"}, {"Content-Type", "application/json"}
-- https://open-meteo.com/ for documentation
function meu:begin_example( name )
	local t
	if name then
		local latitude,longitude
		if name == "Paris" then
			latitude,longitude = 48.8534,2.3488
		elseif name == "Brussels" then
			latitude,longitude = 50.8505,4.3488
		elseif name == "Vancouver" then
			latitude,longitude = 49.2497,-123.1193
		end
		if latitude then
			t = "https://api.open-meteo.com/v1/forecast?latitude="..latitude.."&longitude="..longitude.."&hourly=temperature_2m"
			t = self:get_request( t )
			t.name = name
			self.example_running = t
		end
	else
		self.example_response = nil
	end
	return t
end


function meu:update()
	local t = self.example_running
	if t then
		local id = t.request_id
		--self:print( "example id is "..id )
		if id then	-- we pass here only once
			self:print( "Example dealing with request "..id  )
			local result = aaa.net.requests.get_request_result_if_done(id)
			if result ~= false then
				self:print( "Example completed for request "..id )
				local response = aaa.net.requests.json_to_table(result.text)
				--table.print( response, self.."Example Response as table", 1 )
				--local t = self.requests[id]
				t.response = response
				table.print( t, "example table", 2 )
				self.example_response = t
				t.request_id = nil  -- avoid next time
			end
		end
	end

	local request_list = aaa.net.requests.get_request_list()
	for id, v in pairs(request_list) do
		--self:print( id.." "..v )
		if (v == 1) then
			local result = aaa.net.requests.get_request_result_if_done(id)
			if result ~= false then
				self:print( "Request "..id.." complete !")
				self:print( result.status_code )
				self:print( result.text )
				self:print( result.url )
				self:print( result.elapsed )
				-- Parse the text from the response (which is a JSON object) into a Lua table for ease of use
				self.response = aaa.net.requests.json_to_table(result.text)
				self:set_bu_value("Response", result.text)
				table.print( self.response, self.."Response as table", 1 )
				local t = self.requests[id]
				t.response = self.response
			else
				self:print( "Error with request endpoint or payload! ")
			end
			-- self.text_str = response.data.content
		end
	end
end



-- function meu:net_request()
-- 	local link = "https://animechan.io/api/v1/quotes/random"
-- 	self.request_id = aaa.net.requests.get(link)
-- 	self:print("Request ID: "..self.request_id)
-- end
function meu:__get_url( url )
	url = url or self:get_bu_value( "URL" )
	if url == nil or url == "" or url == "URL" then
		self:print_debug( "No URL provided" )
		return
	end
	return url
end

function meu:__get_payload( payload )
	payload = payload or self:get_bu_value( "Payload" )
	if payload == nil or payload == "Payload" then
		payload = ""
	end
	return payload
end

function meu:get_request( url )
	url = self:__get_url( url )
	if url then
		local id = aaa.net.requests.get( url )
		return self:store_request( id, "get",  url )
	end
end

function meu:post_request( url, payload )
	url = self:__get_url( url )
	if url then
		payload = self:__get_payload( payload )
		local id = aaa.net.requests.post( url, payload )
		return self:store_request( id, "post",  url, payload )
	end
end

function meu:put_request( url, payload )
	url = self:__get_url( url )
	if url then
		payload = self:__get_payload( payload )
		local id = aaa.net.requests.put( url, payload )
		return self:store_request( id, "put",  url, payload )
	end
end

function meu:patch_request( url, payload )
	url = self:__get_url( url )
	if url then
		payload = self:__get_payload( payload )
		local id = aaa.net.requests.patch( url, payload )
		return self:store_request( id, "patch",  url, payload )
	end
end

function meu:download_request( url, path )
	url = self:__get_url( url )
	if url then
		path = self:__get_payload( path )
		if path == "" then
			path = "./download"
		end
		local id = aaa.net.requests.download( url, path )
		return self:store_request( id, "download",  url, path )
	end
end

function meu:delete_request()
	url = self:__get_url( url )
	if url then
		local id = aaa.net.requests.delete( url )
		return self:store_request( id, "delete",  url )
	end
end



-- AUTHORIZATION
--
function meu:__get_username_password()
	local user = self:get_bu_value( "Username" )
	if user == nil or user == "" then
		self:print( "No username provided" )
		return
	end
	local pw = self:get_bu_value( "Password" )
	if pw == nil or pw == "" then
		self:print( "No password provided" )
		return
	end
	return user, pw
end

function meu:set_basic_auth()
	local user, pw = self:__get_username_password()
	if user then
		aaa.net.requests.set_basic_auth( user, pw )
	end
end

function meu:set_digest_auth()
	local user, pw = self:__get_username_password()
	if user then
		aaa.net.requests.set_digest_auth( user, pw )
	end
end

function meu:set_ntlm_auth()
	local user, pw = self:__get_username_password()
	if user then
		aaa.net.requests.set_ntlm_auth( user, pw )
	end
end

function meu:set_oauth()
	local token = self:get_bu_value( "Bearer Token" )
	if token == nil or token == "" or token == "Bearer Token" then
		self:print("No token provided")
		return
	end
	aaa.net.requests.set_oauth( token )
end


function meu:draw_temperature_graph( city, timestamps, temperatures )
	if not timestamps or not temperatures then return end
	if #timestamps ~= #temperatures then return end

	-- === Init & Config ===
	gol.set_line_width(5)

	local function to_minutes(dt)
		local y, m, d, h, min = dt:match("^(%d+)%-(%d+)%-(%d+)T(%d+):(%d+)$")
		return os.time({year=tonumber(y), month=tonumber(m), day=tonumber(d), hour=tonumber(h), min=tonumber(min)})
	end

	local time_min = to_minutes(timestamps[1])
	local time_max = to_minutes(timestamps[#timestamps])
	local temp_min, temp_max = 10, -math.huge

	for _, temp in ipairs(temperatures) do
		if temp < temp_min then temp_min = temp end
		if temp > temp_max then temp_max = temp end
	end

	local margin = 0.1
	local graph_left = -1 + margin
	local graph_right = 1 + 20 * margin
	local graph_bottom = -1 + margin
	local graph_top = 1 - margin

	-- === Legend ===
	local legend_y = graph_bottom - 0.35
	-- === Axes ===
	gol.color_white( 1 )
	gol.draw_lines_2d(graph_left, graph_bottom, graph_right, graph_bottom) -- X
	gol.draw_lines_2d(graph_left, graph_bottom, graph_left, graph_top)     -- Y

		temp_max = 40
	-- === Y Axis Graduations (every 10°C) ===
	local y_step = 10
	local f = (graph_top - graph_bottom) / (temp_max - temp_min)
	for v = math.ceil(temp_min / y_step) * y_step, temp_max, y_step do
		local y = graph_bottom + f * (v - temp_min)
		gol.draw_lines_2d(graph_left - 0.01, y, graph_left, y)
		BU:draw_text_nice(string.format("%.1f", v), graph_left - .3,y - 0.02,0, .1,.1)
	end

	-- === X Axis Graduations (every 12h) ===
	local x_step = 24 * 3600
	local f = (graph_right - graph_left) / (time_max - time_min)
	for t = time_min, time_max, x_step do
		local x = graph_left + f * (t - time_min)
		gol.draw_lines_2d(x, graph_bottom, x, graph_bottom - 0.01)
		BU:draw_text_nice(os.date("%H:%M", t), x - .045,graph_bottom - .1,0, .09,.09)
	end

	-- === Vertical Lines every 24h ===
	gol.set_line_width( 3 )
	local a = 1
	local day_step = 24 * 3600 / 2
	for t = time_min, time_max, day_step do
		gol.color_yellow( a )
		local x = graph_left + f * (t - time_min) 
		gol.draw_lines_2d(x, graph_bottom, x, graph_top)
		a = 1.3 - a
	end
	-- 24h line legend
	gol.color_yellow( 1 )
	gol.draw_lines_2d(graph_left + 2, legend_y, graph_left + 2.2, legend_y)
	BU:draw_text_nice( "24h interval", graph_left + 2.3, legend_y - .03,0, .15,.15 )

	-- === Horizontal Lines every 5°C ===
	gol.set_line_width( 3 )
	local temp_step = 5
	local a = 1
	for v = math.ceil( temp_min / temp_step) * temp_step, temp_max, temp_step do
		gol.color_cyan( a )
		local y = graph_bottom + (graph_top - graph_bottom) * ((v - temp_min) / (temp_max - temp_min))
		gol.draw_lines_2d( graph_left, y, graph_right, y )
		a = 1.5 - a 
	end

	-- === Temperature Curve ===
	gol.set_line_width(8)
	gol.color_red( 1 )
	local px, py
	for i = 1, #timestamps do
		local t = to_minutes(timestamps[i])
		local temp = temperatures[i]
		--local x = graph_left + (graph_right - graph_left) * ((t - time_min) / (time_max - time_min))
		local x = graph_left + (graph_right - graph_left) * ((t - time_min) / (time_max - time_min))
		local y = graph_bottom + (graph_top - graph_bottom) * ((temp - temp_min) / (temp_max - temp_min))
		if px then gol.draw_lines_2d( px,py, x,y ) end
		px,py = x,y
	end

		-- Curve legend
	gol.draw_lines_2d( graph_left, legend_y, graph_left + 0.2, legend_y )
	BU:draw_text_nice( city or "City", graph_left + .25,legend_y - .03,0, .15, .15 )

	-- === Labels ===
	gol.color_white( 1 )
	BU:draw_text_nice( "Time (HH:mm)", graph_right - 1.2,graph_bottom - .25,0, .12, .12 )

	local start_str = os.date("%Y-%m-%d", time_min)
	local end_str = os.date("%Y-%m-%d", time_max)
	local title = "Temperature from " .. start_str .. " to " .. end_str
	BU:draw_text_nice( title, 0,graph_top + .25,0, 0.09, 0.11 )

	gol.set_line_width(1)
end

function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 )
		--gol.color_white( self:get_alpha() )
		--aaa.draw_str( self.text_str )
		local t = self.example_response
		if t then
			--self:print( "we have a t" )
			local response = t.response
			if response and response.hourly then
				--table.print( response.hourly, "will draw from ", 2 )
				self:draw_temperature_graph( t.name, response.hourly.time, response.hourly.temperature_2m )
			end
		end
	self:draw_layers_end()
end