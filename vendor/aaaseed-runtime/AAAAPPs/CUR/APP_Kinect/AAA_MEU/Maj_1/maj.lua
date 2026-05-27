
--[[

--]]

function meu:set_target()
	if self.watch then
		local type = self.type
		local ip = self.ip
		local location = self.location
		self:print( "Calling set_target on link 0 with ip : "..ip..", type : "..type..", location : "..location )
		if type == nil then
			self:print( "No target type selected" )
			return
		elseif type == "file" then
			location = string.gsub( location, "/", "__" )
		--	self.watch.net_send( 0, "aaa.updater:set_target( \""..ip.."\", \""..type.."\", \""..location.."\" )" )
		elseif type == "dir" or type == "app" then
			location = string.gsub( location, "/", "__" )..".7z"
		--elseif type == "app" then
		--	location = string.gsub( location, "/", "__" )..".7z"
		--	self.watch.net_send( 0, "aaa.updater:set_target( \""..ip.."\", \""..type.."\", \""..location.."\" )" )
		--	--self:print( "App target not implemented yet" )
		--elseif type == "exe" then
		--	self:print( "Exe target not implemented yet" )
		elseif type == "env" then
			location = location..".7z"
			--	self:print( "Env target not implemented yet" )
		end
		self.watch.net_send( 0, "aaa.updater:set_target( \""..ip.."\", \""..type.."\", \""..location.."\" )" )
	else
		self:print( "error watchdog is not init ?" )
	end
end

function meu:do_maj()
	self:print( "Calling do_maj on link 0" )
	if self.watch then
		self.watch.net_send( 0, "aaa.updater:do_maj()" )
	else
		self:print( "error watchdog is not init ?" )
	end
end

function meu.pong( updater )
	print( "we got it : "..updater )
end

function meu:hello_updater()
	-- app_kinect -> _gp -> _mus_down -> bus -> _down -> 18 ( Maj_1 )

--	self:print( "MUS : "..MUS )
	--table.print( app.__gp.__mus_down.bus.__down, "gp->mus_down->bus->_down" )
		--local mus_list = app.__gp.__mus_down.bus.__down
		--for i, mu in IPAIRS( mus_list ) do
		--	--print( "i : "..i )
		--	--print( "mu : "..mu )
		--	if mu:get_name() == "Maj_1" then
		--		table.print( mu.__meu )
		--	end
		--end

	--table.print( getmetatable( self ), "meta" )
	--table.print( self )
	--self:print( "get_name() : "..self:get_name() )
	--self:print( "MU : "..self.__mu )
	local ip = self.ip
	--table.print( MEU )
	--table.print( app_kinect )
	--table.print( app_kinect.__gp )
	--table.print( app_kinect.__gp.__mus_down )
	--table.print( app:get_meu_by_name( "Maj_1" ) )
	local name = self:get_name()
	self:print( "Calling updater.new("..ip..","..name..") on link 0" )
	--print( tostring(self) )
	self.watch.net_send( 0, "aaa.updater:hi( \""..ip.."\", \""..name.."\" )" )
end
function meu:check()
	self.watch.net_send( 0, "aaa.updater:check_target()" )
end
function meu:define_ui()
	local ix, iy = 1.5, 1.5
	local SX, SY = 2, 1
	local ui = self.ui
	local bu

	bu = self:add_selector(	{ix,iy,	8,1}, "Target_type" )
		bu:set_nb( 5, 1 )
		bu:set_item_text( 1, "File", "Dir", "App", "Exe", "Env" )
		bu:set_target_lua( self, "id_type" )
	ix = ix + 8
	bu = self:add_selector(	{ix,iy,	6,3}, "Target_pcs" )
		bu:set_nb( 2, 4 )
		bu:set_item_text( 1, "Left", "Right", "Front Left", "Front Right", "Middle Left", "Middle Right", "Back Left", "Back Right" )
		bu:set_target_lua( self, "id_pcs_group" )
	ix, iy = 1.5, iy + 2
	bu = self:add_selector(	{ix,iy,	4,1}, "Target Location" )
		bu:set_nb( 4, 1 )
		bu:set_item_text( 1, "APP_DPool", "APP_garden_min", "APP_kinect", "APP_lv_colors" )
	iy = iy + 4
	self:add_trig_method(	{ix, iy,	 SX, SY}, "Set_target", self, "set_target" )
	ix = ix + SX
	self:add_trig_method(	{ix, iy,	 SX, SY}, "Do_maj", self, "do_maj" )
	ix = ix + SX
	self:add_trig_method(	{ix, iy,	 SX, SY}, "Check", self, "check" )
	ix = ix + SX
	self:add_trig_method(	{ix, iy,	 SX, SY}, "Hello", self, "hello_updater" )

end

function meu:init()
	-- init list of pc potential targets for maj
	local pcs_capture = {}
	local group, j = 1, 1
	for i=1,24 do
		pcs_capture[i] = { ip="192.168.1.2"..group..j, name="kinect_"..i }
		--table.print( pcs_capture[i] )
		j = j + 1
		if i % 8 == 0 then
			group = group + 1
			j = 1
		end
	end
	local bertas = {}
	for i=1,3 do
		bertas[i] = { ip="192.168.1.2"..i.."0", name="berta_"..i }
	end
	self.watch = aaa.lua.global.get( "watchdog" )
	self.dir_maj = "AAAmaje"
	--self.filename = "git.txt"
	self.ip = "192.168.1.71"
	self.types = { "file", "dir", "app", "exe", "env" }
	--self.locations = { "", "", "" }
	--todi this wrong now 2024 June
	self.location = "AAAGaBuZo" -- ".APP_CUR/APP_Kinect" -- /AAA_MEU/Maj_1" -- maj.lua"
end

function meu:update_ui()
	local type = self.type
	--if type == "app" then
	--	b_app
	--end
	--self:active_group( ui.plan_group, self.b_plan )
	--ui.bu_plan:set_text_visible(not self.b_plan)
end

function meu:update()
	self.type = self.types[ self.id_type ]
end

