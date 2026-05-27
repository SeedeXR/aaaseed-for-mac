function meu:define_meu_infos()
	return { author = "Maa", date = "2025",
			   tags = { "Tutorial", "Network", "unfinished" },
			   help = "cudtom implementation of chatGpt using askem serveur"
			 }
end

function meu:init()
	self.text_str = "BONJOUR"
	self.response = nil
	self.request_id = 0
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

	self:add_trig_method(  		{ix,iy,		3,SY}, 	"Prompt",  			self,"do_prompt"	)
	bu = self:add_text_info(  	{ix+3,iy,	2,SY}, 	"Prompt_info"		):set_text("")
		ui.bu_prompt_info = bu
	iy = iy + SY
	self:add_text(  			{ix,iy,     16,SY}, "Prompt text"   	):set_text( "" )
	iy = iy + SY * 3 + DY

	self:add_trig_method(  		{ix,iy,  	3,SY},	"PrePrompt", 		self,"do_preprompt"	)
	bu = self:add_text_info(  	{ix+3,iy,	2,SY},	"PrePrompt_info"	):set_text("")
		ui.bu_preprompt_info = bu
	self:add_trig_method(  		{ix+8,iy,   4,SY},	"Delete",  			self,"delete_preprompt"	)
	iy = iy + SY
	self:add_text(  			{ix,iy,     16,SY},	"PrePrompt text"   	):set_text( "" )
	iy = iy + SY * 3 + DY

	self:add_trig_method( 		{ix,iy,		4,SY},	"Example 1",		self,"begin_example",	1	)
	self:add_trig_method( 		{ix+4,iy,	4,SY},	"Example 2",		self,"begin_example",	2	)
	iy = iy + SY + DY
	
end

local askem_url =  "http://askem.eu:8051/"
local askem_url_message			= askem_url.."send-message"
local askem_url_set_preprompt	= askem_url.."set-preprompt"
local askem_url_clear_preprompt = askem_url.."clear-preprompt"
local askem_chatgpt_header = "accept: application/json, Content-Type: application/json"

function meu:do_prompt()
	local text = self:get_bu_value( "prompt text" )
	text = text:gsub( "\n", "" )
	local t =
	{
		url = askem_url_message,
		payload = "{ \"message\": \""..text.."\" }",
		header = askem_chatgpt_header
	}
	local id = aaa.net.requests.post( t.url, t.payload, t.header )
	t.request_id = id
	self.ui.bu_prompt_info:set_text( id )
	self.request_running = t
	self:print( "request "..id.." started with "..t.url )
end

function meu:do_preprompt()
	local text = self:get_bu_value( "preprompt text" )
	text = text:gsub( "\n", "" )
	local t =
	{
		url = askem_url_set_preprompt,
		payload = "{ \"preprompt\": \""..text.."\" }",
		header = askem_chatgpt_header
	}
	local id = aaa.net.requests.post( t.url, t.payload, t.header )
	t.request_id = id
	self.ui.bu_preprompt_info:set_text( id )
	self.request_running = t
	self:print( "request "..id.." started with "..t.url )
end

function meu:delete_preprompt()
	local t = {	url = askem_url_clear_preprompt }
	local id = aaa.net.requests.delete( t.url )
	t.request_id = id
	self.request_running = t
	self:print( "request "..id.." started with "..t.url )
end

--//{"accept", "application/json"}, {"Content-Type", "application/json"}
function meu:begin_example( id )
	local t
	if id == 1 then
		t = { url = "http://askem.eu:8051/send-message",
			--payload = "{ \"message\": \"parle moi de AAASeed en anglais, en formattant ta reponse avec un passage a la ligne tout les 30 caracteres au maximum\" }",
			--payload = "{ \"message\": \"formate ta reponse en inserant un passage a la ligne (carriage return) tout les 20 caracteres au maximum, en GLSL parle moi de clamp\" }",
			--payload = "{ \"message\": \"donne moi un bout de code lua pour afficher un cercle\" }",
			--payload = "{ \"message\": \"les lignes de tes reponse sont trop longues pour mon affichage\" }",
			payload = "{ \"message\": \"voila je veux toutes les reponses ainsi: 40 caracteres max !\" }",
			header = "accept: application/json, Content-Type: application/json"
		}
		local id = aaa.net.requests.post( t.url, t.payload, t.header )
		t.request_id = id
		self.request_running = t
	end
	if self.request_running then
		self:print( "request "..self.request_id.." started with "..t.url )
	end
end

function meu:draw()	
	if self.response then
		self:draw_layers_begin()
			self:draw_layer( 1 )
		--	aaa.draw_str( self.response 
			local str = self.response
			local str,nb = string.wrap( str, 51 )
			local factor = math.max( 16 / math.max( (nb+1), 16), 1 ) 
			--local factor =  11 / math.min( (nb+1), 11 ) 
			BU:draw_text_nice( str, -8,3.5,0, .6 ,.6 * factor )
		self:draw_layers_end()
	end
end



function meu:update()
	local er = self.request_running
	if er then
		local id = er.request_id
		if id then
			--self:print( "Example dealing with request "..id  )
			local result = aaa.net.requests.get_request_result_if_done(id)
			if result ~= false then
				self:print( "Example completed for request "..id )
				local response = aaa.net.requests.json_to_table(result.text)
				table.print( response, self.."Example Response as table", 3 )
				self:print( "response is "..response.response )
				self.response = response.response
				er.request_id = nil
			end
		end
	end

	-- local request_list = aaa.net.requests.get_request_list()
	-- for k, v in pairs(request_list) do
	-- 	if (v == 1) then
	-- 		local result = aaa.net.requests.get_request_result_if_done(k)
	-- 		if result ~= false then
	-- 			self:print("Request complete!")
	-- 			self:print(result.status_code)
	-- 			self:print(result.text)
	-- 			self:print(result.url)
	-- 			self:print(result.elapsed)
	-- 			-- Parse the text from the response (which is a JSON object) into a Lua table for ease of use
	-- 			self.response = aaa.net.requests.json_to_table(result.text)
	-- 			self:set_bu_value("Response", result.text)
	-- 			table.print( self.response, self.."Response as table", 4 )
	-- 		else
	-- 			self:print( "Error with request endpoint or payload! ")
	-- 		end
	-- 		-- self.text_str = response.data.content
	-- 	end
	-- end
end


-- function meu:net_request()
-- 	local link = "https://animechan.io/api/v1/quotes/random"
-- 	self.request_id = aaa.net.requests.get(link)
-- 	self:print("Request ID: "..self.request_id)
-- end


