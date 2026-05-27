function meu:define_meu_infos( )
	return { author = "Mâa", date="2024",
			tags = { "Core", "Experimental", "Procedural", "Tutorial", "Utility" },
			help="Used to batch stuff"
			 }
end

-- Description for MEY : https://docs.google.com/document/d/1xqjZczL0Q5o4BdirNRX17vNrZFU139mwSa3UZ0_ohwU/edit?usp=sharing

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
--	local par

	local ix = 1
	local iy = 1

	local SY = 1
	local DY = .2
	local SX = (8)/3
	local SXB = 6

	bu = self:add_trig_method(	{ix,iy,		SXB, SY},	"Rename Dir", 		self, "rename_dir" )
	bu = self:add_text(	{ix+SXB,iy,	5,SY}, 	"Src" ):set_value_load_save(true)
		ui.bu_dir_src = bu
	bu = self:add_text(	{ix+SXB+5,iy,	5,SY}, 	"Dst" ):set_value_load_save(true)
		ui.bu_dir_dst = bu
	iy = iy + SY + DY

	bu = self:add_trig_method(	{ix,iy,		SXB, SY},	".bu_pos to .bus", 	self, "rename_bu_pos" )
	iy = iy + SY + DY

	bu = self:add_trig_method(	{ix,iy,		SXB, SY},	".*_MEU.bus to MUS.bus", 	self, "rename_MEU_bus" )
	iy = iy + SY + DY

	bu = self:add_trig_method(	{ix,iy,		SXB, SY},	"Rename", 	self, "rename_file" )
	iy = iy + SY + DY
end

function meu:init()
	local ref = self.ref
end


function meu:update_ui()
	-- local ui = self.ui
	-- local ref = self.ref
	-- ui.bu_dir_name:set_text( "Input Dir : "..self.dir_input )
	-- ui.bu_dir_compressor:set_text( "Compressor : "..self.path_encoder )
	-- ui.bu_dir_out:set_text( "Out Dir : ".. self:get_dir_out() )
end

function meu:process_dir( str, dir_in, b_sub, method, ... )
	if not dir_in then
		self:box_error( "No directory selected" )
		return
	end
	if dir_in == "" then
		self:box_error( "Dir have empty name" )
		return
	end
	--self:print_inverse( str.." dir_in "..dir_in )

	if b_sub then
		local dirs = aaa.dir.get_dirs( dir_in )
		if dirs then
			for _, dir_name in pairs_sorted( dirs ) do
				local dir_new = dir_in.."/"..dir_name
				self:process_dir( str, dir_new, true, method, ... )
				self[method]( self, dir_new, ... )
			end
		end
	-- else
	-- 	self:process_by_type( str, dir, method )
	end

	--self:print_inverse( "____ "..str.." DONE ____" )
end

function meu:print_one( dir, src_name, dst_name )
	local len = string.len( src_name )
	if string.sub( dir, -len ) == src_name then
		local dst = string.sub( dir, 1, -len-1 ) ..dst_name
		self:print( dir.." To ".. dst )
	end
end

function meu:rename_one( dir, src_name, dst_name )
	local len = string.len( src_name )
	if string.sub( dir, -len ) == src_name then
		local dst = string.sub( dir, 1, -len-1 ) ..dst_name
		self:print( dir.." To ".. dst )
		aaa.dir.rename( dir, dst )
	end
end

function meu:rename_dir()
	local ui = self.ui
	local dir = aaa.file.do_dialog_folder( "Pick Root directory to rename " )
	if dir then
		self:print( "Dir selected is "..dir )
		local src_name = ui.bu_dir_src:get_text()
		local dst_name = ui.bu_dir_dst:get_text()
		local str = "rename "..src_name.." to "..dst_name
		self:process_dir( str, dir, true, "print_one", src_name, dst_name )
		if aaa.box_ask( str, "should we rename these") == 1 then
			self:process_dir( str, dir, true, "rename_one", src_name, dst_name )
		end	
	end
end

function meu:do_bu_pos_one( dir, b_erase )
	local ext = ".bu_pos"
	local len = string.len(ext) 
	local files = aaa.dir.get_files( dir, "*"..ext )
	if files then
		for _, name in pairs_sorted( files ) do
			local src = dir.."/"..name
			name = string.sub( name, 1, -len-1 )
			local dst = dir.."/"..name..".bus"
			if b_erase then
				aaa.file.rename( src, dst )
			else
				self:print( "from "..src )
				self:print( "  to "..dst )
			end
		end
	end
end

function meu:rename_bu_pos()
	local ui = self.ui
	local dir = aaa.file.do_dialog_folder( "Pick Root directory to rename_bu_pos " )
	if dir then
		self:print( "Dir selected is "..dir )
		local str = "rename_bu_pos"
		self:process_dir( str, dir, true, "do_bu_pos_one", false  )
		if aaa.box_ask( str, "should we rename these") == 1 then
			self:process_dir( str, dir, true, "do_bu_pos_one", true  )
		end	
	end
end

function meu:do_MEU_bus_one( dir, b_erase )
	local str = "AAA_MEU"
	local len = string.len(str) 
	if string.sub( dir, -len) ==  str then
		--self:print( "found dir "..dir )
		local files = aaa.dir.get_files( dir, "*_MEU.bus" )
		if files then
			for _, name in pairs_sorted( files ) do
				local src = dir.."/"..name
				local dst = dir.."/MUS.bus"
				if b_erase then
					aaa.file.rename( src, dst )
				else
					self:print( "from "..src )
					self:print( "  to "..dst )
				end
			end
		end
	end
	-- local ext = ".bu_pos"
	-- local len = string.len(ext) 
	-- local files = aaa.dir.get_files( dir, "*"..ext )
	-- if files then
	-- 	for _, name in pairs_sorted( files ) do
	-- 		local src = dir.."/"..name
	-- 		name = string.sub( name, 1, -len-1 )
	-- 		local dst = dir.."/"..name..".bus"
	-- 		if b_erase then
	-- 			aaa.file.rename( src, dst )
	-- 		else
	-- 			self:print( "from "..src )
	-- 			self:print( "  to "..dst )
	-- 		end
	-- 	end
	-- end
end

function meu:rename_MEU_bus()
	local ui = self.ui
	local dir = aaa.file.do_dialog_folder( "Pick Root directory to rename_bu_pos " )
	if dir then
		self:print( "Dir selected is "..dir )
		local str = "rename_bu_pos"
		self:process_dir( str, dir, true, "do_MEU_bus_one", false  )
		if aaa.box_ask( str, "should we rename these") == 1 then
			self:process_dir( str, dir, true, "do_MEU_bus_one", true  )
		end	
	end
end

local names = { "AAA_BuGa.bus",
				"AAA_Ga.bus", 
				"AAA_Monitors.bus", 
				"AAA_regular.bus", 
				"AAA_top.bus", 
				"AAA_top_top.bus", 
				"AAA_Uis.bus",  
}
function meu:rename_file_one( dir, b_erase )
	local str = "AAA_MEU"
	local len = string.len(str) 
	if string.sub( dir, -len) ==  str then
		local dir_up = string.sub( dir, 1, -len-1)
		--self:print( "found dir "..dir.." dir up  is "..dir_up )
		-- local files = aaa.dir.get_files( dir, "MUS.bus" )
		-- if files then
		for _, name in pairs_sorted( names ) do
			local src = dir.."/"..name
			if aaa.file.is_exist( src ) then
				local dst = dir_up..name 
		 		if b_erase then
		 			aaa.file.rename( src, dst )
		 		else
		 			self:print( "from "..src )
					self:print( "  to "..dst )
		 		end
			end
		end
	end
end

function meu:rename_file()
	local ui = self.ui
	local dir = aaa.file.do_dialog_folder( "Pick Root directory to rename_file " )
	if dir then
		self:print( "Dir selected is "..dir )
		local str = "rename_file"
		self:process_dir( str, dir, true, "rename_file_one", false  )
		if aaa.box_ask( str, "should we rename these") == 1 then
			self:process_dir( str, dir, true, "rename_file_one", true  )
		end	
	end
end

