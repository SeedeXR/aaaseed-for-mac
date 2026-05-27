
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local SY = 1
	local DY = .2
	local ix,iy = 1,1

	self:add_camera()
	self:add_monitor()
	self:add_rendering()

	self.trs = {}

	bu = self:add_trig_method( {ix,iy,		4,SY}, "Maa",	self, "do_request_maa"	)
	bu = self:add_trig_method( {ix+4,iy,	4,SY}, "Louis", self, "do_request_louis" )
	iy = iy + SY + DY
	bu = self:add_trig_method( {ix+4,iy,	4,SY}, "Async", self, "do_request_async" )

	-- bu = self:add_selector( {ix,iy,	8,6}, "Font" )
	-- 	bu:set_nb_min_0( 8, 8 )
	-- 	bu:set_item_text_from_nb()
	-- 	bu:set_target_param( ref.font )
	-- iy = iy + 6 + DY

	-- bu = self:add_text( {1,8, 16,SY}, "text" )
	-- 	bu:set_text( param.get( self.ref.text ) )
	-- 	--todo this don't function
	-- 	--bu:set_target_param( ref.text )
	-- 	ui.bu_text = bu 
end


function meu:do_request_maa()
    aaa.print_fn()

	--local src = "https://demo.data.arnia-bfc.fr/d4c/api/records/1.0/search/dataset=evolution_des_voitures&facet=proprietaire"
	local src = "http://www.lagraine.com/"
	local src = "http://www.aaaseed.org/"

	-- load the http module
	local http = socket.http

	-- connect to server "www.cs.princeton.edu" and retrieves this manual
	-- file from "~diego/professional/luasocket/http.html" and print it to stdout
	if true then

			http.request{ 
			url = src,
		--	url = "http://www.cs.princeton.edu/~diego/professional/luasocket/http.html",
			sink = ltn12.sink.file(io.stdout)
		}	
	end
	-- connect to server "www.example.com" and tries to retrieve
	-- "/private/index.html". Fails because authentication is needed.
--	b, c, h = http.request("http://www.example.com/private/index.html")
	-- b returns some useless page telling about the denied access, 
	-- h returns authentication information
	-- and c returns with value 401 (Authentication Required)

	-- tries to connect to server "wrong.host" to retrieve "/"
	-- and fails because the host does not exist.
--	r, e = http.request("http://wrong.host/")
	-- r is nil, and e returns with value "host not found"
end

function meu:do_request_louis()
    aaa.print_fn()

    local src = "http://random.imagecdn.app/v1/image?width=400&height=400"

    local http = socket.http

    -- Variable to store the fetched image URL
    local imageURL = nil

    -- First request to get the image URL (assuming the API returns the URL directly in the response body)
    local response = {}
    http.request{
        url = src,
        sink = ltn12.sink.table(response)
    }

    imageURL = table.concat(response)

    -- Modify the URL scheme from https to http if necessary
    if imageURL:sub(1, 5) == "https" then
        imageURL = "http" .. imageURL:sub(6)
    end

    -- Variable to store the image data
    local imageData = {}

    -- Second request to fetch the actual image and store it in the imageData variable
    http.request{
        url = imageURL,
        sink = ltn12.sink.table(imageData)
    }

	self:print( "post request ")
    -- Convert the table of chunks into a single string containing the image data
    imageData = table.concat(imageData)

    -- Now, imageData contains the binary data of the image
    -- Here you would typically save the data to a file or use it as needed for your application
    -- For example, to save the image:
	local filename = aaa.dir.make_media_path("AAASnapshots/downloaded_image.jpg");
    local file = io.open( filename, "wb" )
    file:write(imageData)
    file:close()

	self:print( "post file ")
	local b = aaa.img.read( 0, filename, false, true )
	self:print( "post read ")
   -- return imageData -- Return the image data if needed
end

--local http = require("socket.http")
--local ltn12 = require("ltn12")
--local coroutine = require("coroutine")
function meu:callback_image( imageData )
	self:print( "callback image write" )
	local filename = aaa.dir.make_media_path("AAASnapshots/downloaded_image.jpg");
    local file = io.open( filename, "wb" )
    file:write(imageData)
    file:close()	
	self:print( "callback image write Done" )
	local b = aaa.img.read( 0, filename, false, true )
end

function meu:fetchImageAsync( url, callback )
    coroutine.wrap(
		function()
			local http = socket.http
			local response = {}
			http.request{
				url = url,
				sink = ltn12.sink.table(response)
			}
			local imageURL = table.concat(response)
			if imageURL:sub(1, 5) == "https" then
				imageURL = "http" .. imageURL:sub(6)
			end

			local imageData = {}
			http.request{
				url = imageURL,
				sink = ltn12.sink.table(imageData)
			}
			imageData = table.concat(imageData)

			-- Call the provided callback function with the image data
			callback(imageData)
		end
	)()
end

function meu:do_request_async()
    aaa.print_fn()
	aaa.print( "Before call to fetchImageAsync" )
	self:fetchImageAsync(
		"http://random.imagecdn.app/v1/image?width=400&height=400",
		 function(imageData)
			self:print( "callback image write" )
			local filename = aaa.dir.make_media_path("AAASnapshots/downloaded_image.jpg");
			local file = io.open( filename, "wb" )
			file:write(imageData)
			file:close()	
			self:print( "callback image write Done" )
			local b = aaa.img.read( 0, filename, false, true )
			-- Process the fetched image data, e.g., save to a file
			aaa.print("Image fetched and processed asynchronously")
			-- Example: Save the image data to a file
			-- local file = io.open("async_downloaded_image.jpg", "wb")
			-- file:write(imageData)
			-- file:close()
		end
	)
	aaa.print( "Aftercall to fetchImageAsync" )
end

