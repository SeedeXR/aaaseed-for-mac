
--todo deal with color
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	local ix,iy = 1,1
	local SX = 8
	local SY = .9
	local DY = .2	

	bu = self:add_trig_method(	{ix,iy,		4,SY }, "Pick Folder", self, "pick_dir" ):set_color_back("load")
	ui.bu_dir = self:add_text(	{ix+4,iy,	12,SY }, "DIR" ):set_preset_use(true):set_value_load_save(true)
	iy = iy + SY + DY

	bu = self:add_trig_method(	{ix,iy,		4,SY }, "Dump", self, "dump_dir" )
	bu = self:add_trig_method(	{ix+4,iy,	4,SY }, "Erase Img", self, "erase_imgs" ):set_color_back("load")
	bu = self:add_trig_method(	{ix+8,iy,	4,SY }, "Erase Txt", self, "erase_txts" ):set_color_back("load")
	iy = iy + SY + DY

	bu = self:add_trig_method(	{ix,iy,		4,SY }, "Dif", self, "dump_dif" )
	iy = iy + SY + DY

	bu = self:add_button(	{ix,iy,		4,SY }, "Run", 	self, "b_run", 	false )
	iy = iy + SY + DY
end

function meu:init()
	local ref = self.ref
	self.bind = 2
end

function meu:pick_dir()
	local dir_name = aaa.file.do_dialog_folder( "Folder to watch" )
	self:print( "dir_name is "..dir_name )
	if dir_name then
		self.ui.bu_dir:set_text( dir_name )
	end
end

function meu:get_dir_to_use()
	return self.ui.bu_dir:get_text()
end
function meu:get_files( pat )
	local dir_name = self:get_dir_to_use()
	local files = aaa.dir.get_files( dir_name, pat )
	return files
end
function meu:get_imgs()
	return self:get_files( "*.png" )
end

function meu:dump_dir()
	local files = self:get_imgs()
	for i, file in IPAIRS(files) do
		self:print( file )
	end
end
function meu:erase_imgs()
	local dir_name = self:get_dir_to_use()
	local files = self:get_imgs()
	for i, fname in IPAIRS(files) do
		aaa.file.remove( dir_name.."/"..fname )
	end
end
function meu:erase_txts()
	local dir_name = self:get_dir_to_use()
	local files = self:get_files( "*.txt" )
	for i, fname in IPAIRS(files) do
		aaa.file.remove( dir_name.."/"..fname )
	end
end

function meu:dump_dif_pre()
	local files = self:get_imgs()
	if not files then return end

	local cur = self.files
	local found = nil
	if cur then
		for i, name in IPAIRS(files) do
			for _,name_pre in IPAIRS(cur) do
				if name == name_pre then
					found = name
					break
				end
			end
			if not found then
				found = name
				break
			else
				found = nil
			end
		end

	end
	if found then
		self:print( "New file found ".. found )
	end
	self.files = files
	return found
end
function meu:convert_to_table_by_name( tab )
	local tab_new = {}
	for i,name in IPAIRS(tab) do
		tab_new[name] = i
	end
	return tab_new
end
function meu:dump_dif()
	local files = self:get_imgs()
	if not files then return end

	files = self:convert_to_table_by_name( files )
	local cur = self.files
	local found = nil
	if cur then
		for name,i in PAIRS(files) do
			found = cur[name]
			if not found then
				--self:print( name.." -> "..i )
				found = name
				break
			else
				found = nil
			end
		end
		if found then
			self:print( "New file found ".. found )
		end
	end

	self.files = files
	return found
end

function meu:change_img( fpath )
	self.bind = 3 - self.bind
	aaa.img.read( self.bind, fpath, false, true, true )
	local m = self:get_meu_by_name_cached( "pip_sdfade" )
	m:set_texture( self.bind )
end

function meu:update()
	if self.b_run then
		local fname = self:dump_dif()
		if fname then
			local fpath = self:get_dir_to_use().."/"..fname
			self:change_img( fpath )
		end
	end
end