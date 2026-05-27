function meu:define_meu_infos( )
	return { 	author = "Jean Pirsch",
				tags = {  "utility" , "depreciated", "unfinished" },
				help =	{	"first try at integrating an update system from a MEU, Depreciated for now (2024 Sep)."
						}
			}
end

function meu:define_ui()
	local ix, iy = 1.5, 1.5
	local SX, SY = 2, 1
	local ui = self.ui
	local bu

	bu = self:add_selector(	{ix,iy,	9,1}, "Target type" )
		bu:set_nb( 4, 2 )
		bu:set_item_text( 1, "File", "Dir", "App", "Exe", "Env", "Media", "Dlls" )
		bu:set_target_lua( self, "id_type" )

	-- (A) start : Different ways of selecting target
	iy = iy + 1.5
	self:begin_bu_group( "app" )
		bu = self:add_selector(	{ix,iy,	8,1.8}, "Target App" )
			bu:set_nb( 3, 2 )
			bu:set_item_text( 1, "APP_MonAqua", "APP_DPool", "APP_garden_min", "APP_kinect", "APP_lv_colors" )
			bu:set_target_lua( self, "id_loc" )
	self:end_bu_group()

	self:begin_bu_group( "file" )
		bu = self:add_trig_method(	{ix,	iy}, 	"Open target", 	self, "explorer_open" )
	self:end_bu_group()
	bu = self:add_trig_method(	{	ix + 10, iy,		SX, SY},		"Clear Targets", 		self, "clear_targets" )

	self:begin_bu_group( "exe" )
	bu = self:add_selector(	{ix,iy,	8,1.8}, "Target Exe" )
		bu:set_nb( 3, 2 )
		bu:set_item_text( 1, "NewMetal", "NewRelease", "NewDebug", "WoodMetal", "WoodRelease", "WoodDebug" )
		bu:set_target_lua( self, "id_exe" )
	self:end_bu_group()
	-- (A) end

	ix, iy = 1.5, iy + 3
	bu = self:add_selector(	{ix,iy,	6,1}, "Target_ip_type" )
		bu:set_nb( 3, 1 )
		bu:set_item_text( 1, "Single", "Range", "List" )
		bu:set_target_lua( self, "id_ip_type" )

	-- (B) start : Different ways of selecting target ip
	ix = ix + 6
	self:begin_bu_group( "single" )
		bu = self:add_slider(	{ix,iy,	2,1}, "IP_Target", self, "ip_target", 1, 1, 255 )
			bu:set_value_type_integer( true )
	self:end_bu_group()

	self:begin_bu_group( "range" )
		bu = self:add_slider_two(	{ix,iy,	4,1},	"IP_Target_range",	self, "ip_start",	"ip_end", 0, 254, 1, 255 )
			bu:set_value_type_integer( true )
			bu:set_meter( false )
	self:end_bu_group()

	self:begin_bu_group( "list" )
		bu = self:add_selector(	{ix,iy,	8,1}, "IP_Target_list" )
			bu:set_nb( 4, 1 )
			bu:set_item_text( 1, "Bertas", "Nucs", "Bertas_poc", "Nucs_poc" )
			bu:set_target_lua( self, "ip_target_list" )
	self:end_bu_group()
	-- (B) end

	ix, iy = 1.5, iy + 2
	self:add_trig_method(	{ix, iy,	 SX, SY}, "Set_target", self, "set_target" )
	ix = ix + SX
	self:add_trig_method(	{ix, iy,	 SX, SY}, "Pack_target", self, "pack_target" )
	ix = ix + SX
	self:add_trig_method(	{ix, iy,	 SX, SY}, "Do_maj", self, "do_maj" )
	ix = ix + SX
	self:add_trig_method(	{ix, iy,	 SX, SY}, "Test", self, "test" )
	--ix, iy = 1.5, iy + 2
	--self:add_trig_method(	{ix, iy,	 SX, SY}, "test_set_ip", self, "test_set_ip" )


	local ix, iy = 12, 1.5
	bu = self:add_selector(	{ix,iy,	4,1}, "Interface net id" )
		bu:set_nb( 4, 1 )
		bu:set_item_text( 1, "0", "1", "2", "3" )
		bu:set_target_lua( self, "id_inter" )

end



function meu:explorer_open()
	--self:print( "Folder ASK" )
	local type = self.type
	local filter = "Reconnus\0*.bmp;*.gif;*.jpg;*.sgi;*.rgb;*.tga;*.tif;*.tiff;*.png;*.yuv;*.422;*.exr;*.dds\0".."Bmp\0*.bmp\0".."Gif\0*.gif\0".."Jpeg\0*.jpg\0".."Tga\0*.tga\0".."Tiff\0*.tif;*.tiff\0".."Png\0*.png\0".."Exr\0*.exr\0".."DDS\0*.dds\0".."Tous\0*.*\0".."\0"
	local str

	if type == "file" then
		str = aaa.file.do_dialog_open( "Pick a file" )
	elseif type == "dir" or type == "media" or type == "dlls" then
		str = aaa.file.do_dialog_folder( "Pick a directory" )
	end
	if str then
		self.target_path = str
		self:print( "Target path selected : \""..str.."\"" )
	end
end

-- 	i.e : "aaa.updater:set_target( \""..ip.."\", \""..type.."\", \""..location.."\" ) "
function meu:prepare_net_str( fn_name, ... )
	local count = select("#", ...)
	local net_str = fn_name
	if count == 0 then
		net_str = net_str.."()"
	end
	for i=1,count do
		local arg_i = select( i, ... )
		--self:print( "arg["..i.."] : "..arg_i )
		if i == 1 and i <= count-1 then
			net_str = net_str.."( \""..arg_i
		elseif i == 1 and i == count then
			net_str = net_str.."( \""..arg_i.."\" )"
		elseif i <= count-1 then
			net_str = net_str.."\", \""..arg_i
		elseif i == count then
			net_str = net_str.."\", \""..arg_i.."\" )"
		end
	end
	self:print( "net_str : "..net_str )
	return net_str
end

function meu:link_1_ip_change( ip_network, ip )
	self:print( "Setting ip to : "..ip_network..ip )

	--aaa.net.restart_with_ip_port { link_index=1, ip=ip_network..ip, port=18042 }

	param.set( self.ref.link_1_ip, ip_network..ip )
	aaa.net.restart( 1 ) -- link_index = 1
end

function meu:test_set_ip()
	local ip_network = "192.168.1."
	if self.ip_type == "single" then
		self:link_1_ip_change( ip_network, self.ip_target )
		self:print( "We'll update ip to "..ip_network..self.ip_target )
	elseif self.ip_type == "range" then
		self:print( "We just test setting a single ip you're in the bad mode for this (ip_range)" )
	elseif self.ip_type == "list" then
		self:print( "We just test setting a single ip you're in the bad mode for this (ip_list)" )
	end
end


function meu:acquired_last()
	if self._b_updating then
		return false
	else
		self._b_updating = true
		return true
	end
end

function meu:send_to_watch( fn_name, ... )
	if not self.watch then	self:print( "error watchdog is not init ?" ) end
	if self.ref.link_1_ip == nil then
		self:print( "Problem : link_1 ip ref is nil, we can't send commands to watchdog" )
		return
	elseif self.ip_target == nil then
		self:print( "Problem : self.ip_target is nil, we can't send commands to watchdog" )
		return
	end
	local net_str = self:prepare_net_str( fn_name, ... )

	local ip_network = "192.168.32."
	if self.ip_type == "single" then
		self:link_1_ip_change( ip_network, self.ip_target )
		self:print( "We'll send a command to watchdog at ip : "..ip_network..self.ip_target )
		self.watch.net_send( 1, net_str )
	elseif self.ip_type == "range" then
		for ip=self.ip_start, self.ip_end do
			self:print( "We'll send a command to watchdog at ip : "..ip_network..ip )
			self:link_1_ip_change( ip_network, ip )
			self.watch.net_send( 1, net_str )
			self:print( "Just sent the command : "..net_str )
			local max_wait = 5
			while not self:acquired_last() and max_wait > 0 do
				self:print( "Waiting for "..ip.." to finish processing command" )
				max_wait = max_wait - 1
				aaa.sleep(1000)
			end
		end
	elseif self.ip_type == "list" then
		if not self.ip_list then
			self:print( "Ip list isn't defined")
		elseif #self.ip_list == 0 then
			self:print( "Ip list is empty ?" )
		end
		table.print( self.ip_list, "self.ip_list", 2 )
		for t in PAIRS(self.ip_list) do
			local ip = self.ip_list[t]["ip"]
			self:print( "We'll send a command to watchdog at ip : "..ip_network..ip )
			self:link_1_ip_change( ip_network, ip )
			self.watch.net_send( 1, net_str )
			self:print( "Just sent the command : "..net_str )
			local max_wait = 5
			while not self:acquired_last() and max_wait > 0 do
				self:print( "Waiting for "..ip.." to finish processing command" )
				max_wait = max_wait - 1
				aaa.sleep(1000)
			end
		end
	else
		self:print( "Ip type not selected ?" )
	end
end

function meu:clear_targets()
	self:print( "Clear targets" )
	self.target_path = nil
	self.target_file_to_send = nil
end

function meu:build_target_names()
	local type = self.type
	if type == nil then self:print( "No target type selected" ) return end
	local t_name, t_path, t_file_to_send

	local location = self.target_name

	if type == "file" and self.target_path then
		t_name = string.sub( self.target_path, 18 )
		t_path = self.target_path
		t_file_to_send = "C:/AAAMaje/"..string.gsub( t_name, "/", "__" )
		self:print( " t_file_to_send "..t_file_to_send)
	elseif type == "dir" and self.target_path then
		t_name = string.sub( self.target_path, 18 )
		t_path = self.target_path
		t_file_to_send = "C:/AAAMaje/"..string.gsub( t_name, "/", "__" )..".7z"
	elseif type == "media" and self.target_path then
		t_name = string.sub( self.target_path, 8 )
		t_path = self.target_path
		t_file_to_send = "C:/AAAMaje/"..string.gsub( t_name, "/", "__" )..".7z"
	elseif type == "dlls" then
		t_name = string.sub( self.target_path, 8 )
		t_path = self.target_path
		t_file_to_send = "C:/AAAMaje/"..string.gsub( t_name, "/", "__" )..".7z"
	elseif type == "app" then
		t_name = self.locations[self.id_loc]
		t_path = "C:/AAA/AAAGaBuZo/"..t_name
		t_file_to_send = "C:/AAAMaje/"..string.gsub( t_name, "/", "__" )..".7z"
	elseif type == "exe" then
		self.exe_name = self.exe_names[self.id_exe]
		t_name = "AAASeed_"..self.exe_name.."_v142_x64.exe"
		t_path = "C:/AAA/AAAGaBuZo/"..t_name
		t_file_to_send = "C:/AAAMaje/"..t_name
	elseif type == "env" then
		t_name = "AAAGaBuZo"
		t_path = "C:/AAA/"
		t_file_to_send = "C:/AAAMaje/"..t_name..".7z"
	end
	self.target_name = t_name
	self.target_path = t_path
	self.target_file_to_send = t_file_to_send
	self:print( "Target name : "..t_name )
	self:print( "Target path : "..t_path )
	self:print( "Target file to send : "..t_file_to_send )
	if not t_name or not t_path or not t_file_to_send then return false end
	return true
end

function meu:set_target()
	if not self:build_target_names() then
		self:print( "Failed to build names for target ( filename, path, filepath to send )")
		return
	end
	local ip = self.master_ip
	local type = self.type

	self:send_to_watch( "aaa.updater:set_target", ip, type, string.sub( self.target_file_to_send, 12 ) )
end

function meu:pack_target( )
	local type = self.type
	local str_err
	local f_name = self.target_name
	local f_path = self.target_path
	local file_to_send = self.target_file_to_send

	self:print( "File to pack : "..f_path )
	self:print( "Will be available on network as : "..file_to_send )
	if type == "dir" or type == "app" or type=="media" then
		table.print( file_to_send, "file_to_send", 2)
		self:print( "Archiving "..type.." : "..f_path..", to "..file_to_send )
		str_err = aaa.os.execute_shell( "\"C:/Program Files/7-Zip/7z.exe\" a \""..file_to_send.."\" \""..f_path.."\"" )
	elseif type == "env" then
		f_path = f_path.."/"..f_name
		self:print( "Archiving "..type.." : "..f_path..", to "..file_to_send )
		str_err = aaa.os.execute_shell( "\"C:/Program Files/7-Zip/7z.exe\" a \""..file_to_send.."\" \""..f_path.."\"" )
	elseif type == "file" or type == "exe" then
		self:print( "Copying file : "..f_path..", to "..file_to_send )
		str_err = aaa.file.copy( f_path, file_to_send )
	end

	if str_err then
		self:print( str_err )
		return false
	end

	return true
end

function meu:do_maj()
	self:print( "Calling do_maj on link 1" )
	self:send_to_watch( "aaa.updater:do_maj" )
end

function meu.pong( updater )
	print( "We received a pong from : "..updater )
end

function meu:hello_updater()
	local ip = self.ip
	local name = self:get_name()
	self:print( "Calling updater.new("..ip..","..name..") on link 1" )
	--print( tostring(self) )
	self.watch.net_send( 1, "aaa.updater:hi( \""..ip.."\", \""..name.."\" )" )
end

function meu:test()
	--self:print( "Network test, link 1, ip : "..param.get( self.ref.link_1_ip ) )
	--self.watch.net_send( 1, "aaa.updater:print( \"THIS IS A DIRECT WATCH.NET_SEND(...) TEST\")" )
	self:send_to_watch( "aaa.updater:print", "\\n\\n THIS IS A NETWORK TEST \\n\\n" )

	--self.watch.net_send( 1, "aaa.updater:test_log( \"C:/AAA/AAAGaBuZo\", nil, \"LogFile\" )" )
	--self:send_to_watch( "aaa.updater:test_log", "C:/AAA/AAAGaBuZo", nil, "LogFile" )
	--self:print( self.target_name )
	--self:send_to_watch( "aaa.updater:print_args", "with", "some", "args" )
	--self:print( "We test a function. Now it is UPDATER:check_target()" )
	--self.watch.net_send( 1, "aaa.updater.check_target()" )
end

function meu:init_targets_ip_lv_colors()
	--local pcs_capture = {}
	--local group, j = 1, 1
	--for i=1,24 do
	--	pcs_capture[i] = { ip="192.168.1.2"..group..j, name="kinect_"..i }
	--	--table.print( pcs_capture[i] )
	--	j = j + 1
	--	if i % 8 == 0 then
	--		group = group + 1
	--		j = 1
	--	end
	--end
	--local bertas = {}
	--for i=1,3 do
	--	bertas[i] = { ip="192.168.1.2"..i.."0", name="berta_"..i }
	--end
end

function meu:init_targets_ip_monaco()
	local bertas = {}
	for i=1,4 do
		bertas[i] = { ip_str="192.168.1.21"..i,
						ip="21"..i,
						name="BERTHA-KL"..i }
	end
	for i=1,4 do
		bertas[i+4] = { ip_str="192.168.1.22"..i,
						ip="22"..i,
						name="BERTHA-KR"..i }
	end
	bertas[9] = { ip_str="192.168.1.210",
					ip="210",
					name="BERTHA-KC" }
	bertas[10] = { ip_str="192.168.1.205",
					ip="205",
					name="BERTHA-SO" }

	local nucs = {}
	for i=1,4 do
		nucs[i] = { ip_str="192.168.1.23"..i,
					ip="23"..i,
					name="NUCL"..i }
	end
	for i=1,4 do
		nucs[i+4] = { ip_str="192.168.1.24"..i,
					ip="24"..i,
					name="NUCR"..i }
	end
	self.ip_lists = {}
	self.ip_lists["Bertas"] = bertas
	self.ip_lists["Nucs"] = nucs

	local bertas_poc = {}
	for i=3,4 do
		bertas_poc[i] = { ip_str="192.168.32.20"..i,
						ip="20"..i,
						name="Berta"..i }
	end
	bertas_poc[5] = { ip_str="192.168.32.211",
					ip="211",
					name="BertaSol" }
	bertas_poc[10] = { ip_str="192.168.32.231",
					ip="231",
					name="BertaCentre" }
	local nucs_poc = {}
	for i=3,5 do
		nucs_poc[i] = { ip_str="192.168.32.20"..i+4,
					ip="20"..i+4,
					name="NUCL"..i }
	end
	self.ip_lists["Bertas_poc"] = bertas_poc
	self.ip_lists["Nucs_poc"] = nucs_poc
end

function meu:init_ref_link1_ip()
	local ref = aaa.net.ref
	local link_1 = param.get_ref( ref.obj, "link_1" )
	local link_1_ref
	if link_1 then
		link_1_ref = param.get_obj_attached( link_1 )
		if link_1_ref then
			self.ref.link_1_ip = param.get_ref( link_1_ref, "dst_ip" )
			local link_1_ip = param.get( self.ref.link_1_ip )
			--self:print( "Got link_1 ip : "..link_1_ip )

			self.ref.link_1_stop = param.get_ref( link_1_ref, "stop_trig" )
			self.ref.link_1_start = param.get_ref( link_1_ref, "start_trig" )
		else
			self:print( "Could not get link_1 attached obj." )
		end
	else
		self:print( "Could not get link_1 param.")
	end
end

function meu:init()
	-- init list of pc potential targets for maj
	--self:init_targets_ip_lv_colors()
	self.ip_list = {}
	self.lists_names = { "Bertas", "Nucs", "Bertas_poc", "Nucs_poc" }
	self:init_targets_ip_monaco()

	self:init_ref_link1_ip()

	self.watch = aaa.lua.global.get( "watchdog" )
	self.dir_maj = "AAAmaje"
	--self.filename = "git.txt"
	self.master_ip = aaa.net.ip[1] -- "192.168.32.71"
	self.types = { "file", "dir", "app", "exe", "env", "media", "dlls" }
	self.exe_names = { "NewMetal", "NewRelease", "NewDebug", "WoodMetal", "WoodRelease", "WoodDebug" }
	self.ip_types = { "single", "range", "list" }
	local base_loc = "AAAKernel/APP_CUR/"	--todo get it from exe
	self.locations = { base_loc.."APP_MonAqua", base_loc.."APP_DPool", base_loc.."APP_Garden_Min",
						base_loc.."APP_Kinect", base_loc.."APP_LV_Colors" }
end

function meu:update_ui()
	local ui = self.ui
	local type = self.type
	local ip_type = self.ip_type

	self:set_bu_group_active( "app",	type == "app" and true or false )
	self:set_bu_group_active( "exe",	type == "exe" and true or false )
	self:set_bu_group_active( "file",	( type == "file" or type == "dir" or type == "media" or type == "dlls" ) and true or false )

	self:set_bu_group_active( "single",	ip_type == "single" and true or false )
	self:set_bu_group_active( "range",	ip_type == "range" and true or false )
	self:set_bu_group_active( "list",	ip_type == "list" and true or false )
end

function meu:update()
	local new_type = self.types[self.id_type]
	if new_type ~= self.type then
		self:clear_targets()
	end
	self.type = self.types[self.id_type]
	self.ip_type = self.ip_types[self.id_ip_type]
	self.ip_list = self.ip_lists[self.lists_names[self.ip_target_list]]
	self.master_ip = aaa.net.ip[ self.id_inter ]

end

