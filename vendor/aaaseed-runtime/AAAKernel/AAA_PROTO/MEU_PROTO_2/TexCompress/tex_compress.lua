
-- Description for MEY : https://docs.google.com/document/d/1xqjZczL0Q5o4BdirNRX17vNrZFU139mwSa3UZ0_ohwU/edit?usp=sharing

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
--	local par

	local ix = 1
	local iy = 1

	local SY = .8
	local DY = .2
	local SX = (8)/3

	bu = self:add_trig_method(	{	ix, iy,		SX, SY},	"Compressor Dir", 	self, "compressor_dir" )
	bu = self:add_text_info(	{ix+3,iy+SY/4,	16,SY/2}, 	"Compressor : " )
		ui.bu_dir_compressor = bu
	iy = iy + SY + DY

	ui.bu_dir_name = self:add_text_info({ix,iy,	16,SY/2},	"Dir : " )
	iy = iy + SY*.5
		bu = self:add_trig_method(	{	ix,		iy,	SX,SY},	"Set Input Dir", 	self, "select_dir_input" )
	bu = self:add_trig_method(	{	ix+SX,		iy,	SX,SY},	"Open Input Dir", 	self, "open_dir_input" )
	iy = iy + SY
	local SX = 2
	self:add_button(			{	ix,		iy,		SX,SY},	"TGA", 				self, "b_tga",		true	)
	self:add_button(			{	ix+SX,	iy,		SX,SY},	"PNG", 				self, "b_png",		false	)
	self:add_button(			{	ix+SX*2,iy,		SX,SY},	"JPG", 				self, "b_jpg",		false	)
	iy = iy + SY + DY



	ui.bu_dir_out = self:add_text_info(	{ix,iy,	16,SY/2},	"Out Dir : " )
	iy = iy + SY*.5
	self:add_button(			{	ix,	iy,			SY,SY},	"Same Folder", 		self, "b_same_folder",		true	)
	bu = self:add_trig_method(	{	ix + 5, iy,		SX,SY},	"Set Out Dir", 		self, "select_dir_out" )
	bu = self:add_trig_method(	{	ix+ 5 +SX, iy,	SX,SY},	"Open Out Dir", 	self, "open_dir_out" )
	iy = iy + SY + DY

	bu = self:add_trig_method(	{	ix,	iy,			SX,SY},	"Crop", 			self, "crop" )
		:set_color_back("save")
	self:add_slider(			{	ix+SX+DY,iy,	6,SY},	"Margin", 			self, "margin",		0,0,16	)
		:set_value_type_integer( true )
	iy = iy + SY + DY


	bu = self:add_trig_method(	{	ix,		iy,		SX,SY},	"Compress", 		self, "compress" )
		:set_color_back("save")
	iy = iy + SY
	self:add_button(			{	ix,	iy,			SY, SY},	"Overwrite", 		self, "b_overwrite",	true	)
	iy = iy + SY
	self:add_button(			{	ix,	iy,			SY, SY},	"Flip V", 			self, "b_flip_v",		true	)
	self:add_button(			{	ix + 4,	iy,		SY, SY},	"Max Compression", 	self, "b_bc_max",		true	)
	bu = self:add_selector(		{	ix + 9,iy,		4, SY},		"Format" )
		bu:set_nb( 2 )
		bu:set_item_text( 1, "Bc4", "Bc7" )
		bu:set_item_data( 1, "bc4", "bc7")
		ui.bu_out_format = bu
	--self:add_button(			{	ix + 9,	iy },				"BC4", 				self, "b_bc_4",			false	)
	iy = iy + SY
	self:add_button(			{	ix,	iy,			SY, SY},	"Generate MipMap", 	self, "b_mipmap_generate",		true	)
	self:add_slider(			{	ix+5,iy,		4, SY},		"MipMap Nb",		self, "mipmap_level",		0, 0, 30 ):set_value_type_integer(true)
	iy = iy + SY
	self:add_button(			{	ix,	iy,			SY, SY},	"GPU", 				self, "b_gpu",			true	)
	iy = iy + SY
	ui.bu_info = self:add_text_info(	{ix,iy,	16,SY * .5},	"Info : " )
end

function meu:init()
	local ref = self.ref
	self.dir_input = ""
	self.path_encoder = "../texconv.exe"
	self.dir_out = ""

	ref.layer_anal = self:get_layer_bdd(1)
	local bdd = self:get_layer_bdd(1)
	ref.bdd_anal = bdd
	ref.bind_img_src = param.get_ref( bdd, "image_src" )
	ref.min_x = param.get_ref( bdd, "out_min_x" )
	ref.max_x = param.get_ref( bdd, "out_max_x" )
	ref.min_y = param.get_ref( bdd, "out_min_y" )
	ref.max_y = param.get_ref( bdd, "out_max_y" )
end

function meu:get_dir_out()
	return self.b_same_folder and self.dir_input or self.dir_out
end

function meu:update_ui()
	local ui = self.ui
	local ref = self.ref
	ui.bu_dir_name:set_text( "Input Dir : "..self.dir_input )
	ui.bu_dir_compressor:set_text( "Compressor : "..self.path_encoder )
	ui.bu_dir_out:set_text( "Out Dir : ".. self:get_dir_out() )
end

function meu:open_dir_input()	aaa.os.open_dir( self.dir_input )		end
function meu:open_out_dir()		aaa.os.open_dir( self:get_dir_out() )	end

function meu:compressor_dir()
	local dir = aaa.file.do_dialog_folder( "Pick Compressor directory" )
	if dir then
		self:print( "Dir selected "..dir )
		self.path_encoder = dir.."/texconv.exe"
	end
end

function meu:select_dir_input()
	local dir  = aaa.file.do_dialog_folder( "Pick Input directory" )
	if dir then
		self:print( "Dir selected "..dir )
		self.dir_input = dir
	end
end

function meu:select_out_dir()
	local out_dir = aaa.file.do_dialog_folder( "Pick Output directory" )
	if out_dir then
		self:print( "Out Dir selected "..out_dir )
		self.dir_out = out_dir
	end
end

--todo put in APP
function meu:pick_format_from_name( name )
	if string.find( name, "ALBEDO" ) then
		self:print( "Albedo Texture found" )
		return nil
	elseif string.find( name, "METAL") then
		self:print( "Metallic Texture found" )
		return "bc4"
	elseif string.find( name, "ROUGHNESS") then
		self:print( "Roughness Texture found" )
		return "bc4"
	elseif string.find( name, "NORMAL") then
		self:print( "Normal Texture found" )
		return "bc7"
	end
end

function meu:process_by_type( str, dir, method )
	local l = { "TGA", "PNG", "JPG" }
	local fn = self[method]
	self:print( "method "..method.." fn is "..fn )
	for _,k in ipairs(l) do
		local b_str = "b_"..string.lower( k )
		if self[b_str] then
			self:print( " call for "..k )
			self:print_inverse( "____ "..str.." "..k.." in folder "..dir.." ____" )
			local t_file = aaa.dir.get_files( dir, "*"..k )
			if t_file then
				local ui = self.ui
				ui.bu_info:set_text( str.." "..k.. " in folder : "..dir )
				fn( self, str, dir, t_file )
				ui.bu_info:set_text( str.." "..#t_file.." files" )
				self:print( str.." All files "..k )
			end		
		end
	end
end

function meu:process( str, dir, b_sub, method )
	if not dir then
		self:box_error( "No directory selected" )
		return
	end
	if dir == "" then
		self:box_error( "Dir have empty name" )
		return
	end
	self:print_inverse( str.." dir "..dir )

	if b_sub then
		local dirs = aaa.dir.get_dirs( dir )
		if dirs then
			for _, dir_name in pairs_sorted( dirs ) do
				self:process( str, dir.."/"..dir_name, false, method )
			end
		end
	else
		self:process_by_type( str, dir, method )
	end

	self:print_inverse( "____ "..str.." DONE ____" )
end

--todo compress only one by frame so we keep the ui working
function meu:compress_files( str, dir, t_file )

	local format_out_asked = ui.bu_out_format:get_item_data()

	for i= 1, #t_file do
		local fname = t_file[i]
		local mess = str.." image "..i.."/ "..#t_file.." : "..fname
		self.ui.bu_info:set_text( mess )
		self:print_inverse( mess )
		local source_file = dir.."/"..fname
		local name_pure = aaa.file.get_name_pure( fname )
		local dest_file =  dir.."/"..name_pure..".dds"
		local args = ""

		local out_format = self:pick_format_from_name( name_pure ) or format_out_asked

		if out_format == "bc4" then
			args = "-f BC4_UNORM"
			self:print( "Texture as BC4")
		elseif out_format == "bc7" then
			args = "-f BC7_UNORM"
			if self.b_bc_max then
				args = args.." -bc x"
			end
			self:print( "Texture as BC7" )
		else
			self:box_error( out_format.." is an unkown format" )
		end


		if self.b_flip_v	then	args = args.." -vflip"	end
		if self.b_overwrite then	args = args.." -y"		end
--		if self.b_bc_max	then	args = args.." -bc x"	end
		if self.b_gpu		then	args = args.." -gpu 0"	end
		if self.b_mipmap_generate then
			args = args.." -m "..self.mipmap_level
		else
			args = args.." -m 1"
		end
		local dir_out = self:get_dir_out()

		args = args.." -timing -o "..""..dir_out.."".." "..""..source_file..""
	--	local str_err = aaa.os.execute_shell( "\"C:/Program Files/ARM/Mali Developer Tools/Mali Texture Compression Tool v4.3.0/bin/astcenc.exe\" -c "..source_file.." "..dest_file..".astc 2.0 "..performance )
	--	local str_err = aaa.os.execute_shell( "\"C:/Compressonator_4.0.4855/bin/CLI/CompressonatorCLI.exe\" "..source_file.." "..dest_file.." -fd BC7 -performance -logcsv -nomipmap -Quality 1.0000  -NumThreads 16" )
	--	local str_err = aaa.os.execute_shell( "\"C:/AAA/bc7enc.exe\" -l -u4 -y -g "..source_file.." "..dest_file )
--		local str_err = aaa.os.execute_shell( "\"C:/AAA/texconv.exe\" -f BC7_UNORM -vflip -bcmax -m 0 -timing -gpu 0 -o "..dir.." "..source_file )
--		local str_err = aaa.os.execute_shell( "\"C:/AAA/texconv.exe\"", args.." -timing -o "..dir.." "..source_file )
		local str_err = aaa.os.execute_shell( self.path_encoder, args )
		self:print( "Result from shell exe "..str_err )
		--		-c input output 2.0 -exhaustive
	end
end

function meu:compress()
	self:process(  "Compress", self.dir_input, false, "compress_files" )
end

function meu:crop_files( str, dir, t_file )
	local bind = self.bind
	local ref = self.ref
	if not bind then
		bind = IMGS.inc_bind_free( 2 )
		self.bind = bind
		--self:box_debug( "will work using bind "..bind )
	end
	param.set( ref.bind_img_src, bind )

	local size_before,size_after = 0,0
	local margin = self.margin
	for i= 1, #t_file do
		local fname = t_file[i]
		local mess = str.." image "..i.."/ "..#t_file.." : "..fname
		self.ui.bu_info:set_text( mess )
		self:print_inverse( mess )

		local source_file = dir.."/"..fname
		local name_pure = aaa.file.get_name_pure( fname )

		if aaa.img.read( bind, source_file ) then
			local src_sx,src_sy,ch_nb = aaa.img.get_size_channel( bind )
			local str_for = aaa.img.get_format_name( 	bind )
			self:print( src_sx.." x "..src_sy.." x "..ch_nb.."   "..str_for )

			--aaa.obj.update_then_draw( ref.bdd_anal )
			self:draw_layer( 1 )
			local min_x = param.get( ref.min_x )
			local max_x = param.get( ref.max_x )
			local min_y = param.get( ref.min_y )
			local max_y = param.get( ref.max_y )
			self:print( min_x..","..min_y.." "..max_x..","..max_y )

			if min_x>max_x or min_y>max_y then
				--todo check what happen when empty result
				self:print_error( "Empty image ???" )
			else
				size_before = size_before + src_sx*src_sy
				min_x = math.max( 0, min_x-margin )
				min_y = math.max( 0, min_y-margin )
				max_x = math.min( src_sx-1, max_x+margin )
				max_y = math.min( src_sy-1, max_y+margin )
				local sx = max_x - min_x + 1
				local sy = max_y - min_y + 1 
			
				size_after = size_after + sx*sy

				aaa.img.copy( bind,bind+1, min_x,min_y, sx,sy )

				local dir_out = dir.."/"

				local ext = "tga"
				local crop_fname = name_pure.."_crop"
				local str
				local fname_img = crop_fname.."."..ext
				if true then
					str = "return\n{\n\tversion = 0,\n\timg_filename = \""..fname_img.."\",\n" 
					str = str.."\tcrop = { left="..min_x..", right="..max_x..", bottom="..min_y..", top="..max_y..", size_x="..src_sx..", size_y="..src_sy.." },\n"
					str = str.."}\n"
				else
					local tab = { version=0, img_filename=fname_img, crop = { left=min_x, right=max_x, bottom=min_y, top=max_y, size_x=src_sx, size_y=src_sy } }
					str = self:serialize_to_str( "return ", tab )
				end
				self:print( "save aaa_img proxy to "..dir_out..crop_fname..".aaa_img" )
				aaa.file.save_text( dir_out..name_pure..".aaa_img", str )
				--todo out dir

				local b = aaa.img.save(	bind+1,	dir_out..crop_fname, "tga" )
			end

		else
			self:print_error( "Can't read image ???" )
		end
	end
	self:print( "cropping use "..(size_after/size_before*100).." percent of source" )
	self:print_inverse( "____ TEST DONE ____" )
end

function meu:crop()
	self:process(  "Crop", self.dir_input, false, "crop_files" )
end


function meu:update()

end

function meu:draw()
	local ref = self.ref
	-- if self.layer_anal then
	-- 	aaa.obj.update_then_draw( self.layer_anal )
	-- end
	self:draw_layers_begin()
		self:draw_layer( 1 )
	self:draw_layers_end()
end

