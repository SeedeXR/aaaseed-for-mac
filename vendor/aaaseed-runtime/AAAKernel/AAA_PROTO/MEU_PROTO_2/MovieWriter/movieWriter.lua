function meu:define_meu_infos( )
	return { author = "Mâa", date="2024",
			tags = { "Core", "CoreGraphic", "Output", "Utility" },
			help = "encode a texture flux trought time and save it"
			}
end

--todo deal with color
function	meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix,iy = 1,1
	local SY,DY = 1,.2
	--todo seems wrong
	self:add_bu_texture_target_unit( {ix,iy,	8,6}, "Source", nil, false )
	iy = iy + 6

--	bu = self:add_monitor( {1,11}, "Src" )
--		bu:set_render_mode_alpha( 1 )

	local bdd = ref.bdd_cv_writer

	ix,iy = 9,1
	bu = self:add_trig_method(				{ix,iy,	 	4,SY},	"Get Folder",	self, "set_video_path" )
	bu = self:add_trig_method(				{ix+4,iy,	4,SY},	"Open",			self, "open_video_path" )
	iy = iy + SY
	self.ui.bu_fpath = self:add_text(		{ix,iy,		8,SY},	"Path" ):set_text(  aaa.dir.get_dir_snapshot() )
	iy = iy + SY
	self.ui.bu_fname = self:add_text(		{ix,iy,		8,SY},	"name" ):set_text( "MovieTest" )
	iy = iy + SY + DY

	bu = self:add_button(           		{ix,iy,     3,SY},	"Codec",		nil,	nil,		 0				)
		bu:set_menu{ "MJPEG" , "H264" }:set_nb_min_0( 1, 2 )
   		bu:set_target_lua( self, "codec" )
   		bu:set_text_selector( true )
	self:add_slider(						{ix+3,iy,	5,SY},	"Quality",		bdd,	"quality",	70,		0,100	)
	iy = iy + SY

--	self:add_button(						{ix,iy,		4,SY},	"Hori Flip",	bdd,	"flip_horizontal",	false	)
	self:add_button(						{ix+2,iy,	4,SY},	"Vert Flip",	bdd,	"flip_vertical",	false	)
	iy = iy + SY + DY

	bu = self:add_button(					{ix,iy,		4,SY},	"Record",		self,	"b_record",			false	)
	
	iy = iy + SY + DY

	--[[bu = self:add_trig_method(				{ix,iy,		4,SY}, "Record",	self, "save_video" ):set_color_back("save")
	bu = self:add_trig_method(				{ix+4,iy,	4,SY}, "Stop",		self, "close_video" )
	iy = iy + SY]]
	self.ui.bu_info = self:add_text_info(	{ix,iy,		8,SY}, "info" )
	iy = iy + SY

	--bu:set_editable( true, "PC" )

--	bu = self:add_trig_method(	{10, 2}, "Add keyboards",	self, "add_keyboard" )
--		bu:set_text_rect_ratio( 6 )

	self.bu_text_videoname = bu
end

function meu:add_keyboard()
	local function __add_keyboard( name )
		local keyboard =  KEYBOARD:create( name )
		return keyboard
	end
	if not self.keyboard1 then
		self.keyboard1 = __add_keyboard( "NumPad" )
	end
	if not self.keyboard2 then
		self.keyboard2 = __add_keyboard( "PC" )
		self.keyboard2:set_button_close_active( true )
	end
end

function	meu:init()
	local ref = self.ref

	ref.bdd_cv_writer = aaa.layer.get_bdd( self:get_layer(1) )
		ref.bind		=	param.get_ref( ref.bdd_cv_writer, "image_src"	)
		ref.run			=	param.get_ref( ref.bdd_cv_writer, "run"			)
		ref.filename	=	param.get_ref( ref.bdd_cv_writer, "filename"	)
		ref.bgr			=	param.get_ref( ref.bdd_cv_writer, "flip_bgr"	)
		ref.frame_count	=	param.get_ref( ref.bdd_cv_writer, "frame_count"	)
		ref.codec		=	param.get_ref( ref.bdd_cv_writer, "codec"		)

	self.bind_src	= 0
	--self.b_saving	= false
end

--todo optimize on change only
function	meu:update_ui()
	local ui = self.ui
	local ref = self.ref
	local b_run = param.get_bool( ref.run )
	local frame_number = param.get( ref.frame_count )
	if b_run then
		ui.bu_info:set_text( "Recording : "..frame_number )
	else
		ui.bu_info:set_text( "Stopped :"..frame_number )
	end
	ui.bu_info:set_text_color_problem_info( not b_run )
end

function	meu:update()
	local	ps	= param.set
	local	ref	= self.ref

	--self:print( "Here")
	local bind

--	ps( ref.moviewriter_bind, bind )

--	self:print( "Filename is "..filename )

	GA:set_keyboard_physical_target_virtual( self.keyboard2 )

	bind = self:get_texture_bind_2d( 2 )
	--self:print( "Bind is ".. bind )
	ps( ref.bind, bind )
	self:save_function()
end

--[[function meu:start_video()
	param.set( self.ref.run, 1 )
	self.b_saving = true
end

function meu:stop_video()
	param.set( self.ref.run, 0 )
	self.b_saving = false
end]]

function meu:open_video_path()
	aaa.os.open_dir( self.path_video or self.ui.bu_fpath:get_text() )
end

function meu:set_video_path( path )
	if path  then
		self.path_video = path
	else
		local title = "Movie Path"
		local folder = aaa.file.do_dialog_folder( title )

		if folder then
			self:print( "Path selected : "..folder )
			self.ui.bu_fpath:set_text( folder )
		end
	end
	self:print( "Path selected : "..self.fpath )
end


function meu:get_filename()
	local ui = self.ui
	local path = ui.bu_fpath:get_text()
	local filename = ui.bu_fname:get_text()
	local hms = aaa.time.get_str_hms()

	path = path.."/"..filename .. hms
	--self:print( "Path is "..path ) 
	return path
end


--[[function	meu:save_video( )
	if self.b_saving then return end

--	self:set_filename( self.bu_text_videoname:get_text() )
	param.set( self.ref.filename, self:get_filename() )
	self:start_video()
end

function	meu:close_video()
-- 	local t = aaa.time
	self:stop_video()
end]]

function meu:save_function()
	local ref = self.ref
	if self.b_record then
		if self.codec == 1 then 
			param.set( ref.codec, 5 )
		else
			param.set( ref.codec, 4 )
		end
		param.set( ref.filename, self:get_filename() )
		param.set( ref.run, 1 )
	else 
		param.set( ref.run, 0 )
	end
end

meu.draw_icon = MEU.draw_icon_base