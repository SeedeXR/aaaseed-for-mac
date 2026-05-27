--todo deal with color
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_bu_texture( {1,1}, "Movie Writer Image" )
	bu = self:add_monitor( {1,11}, "Src" )
		bu:set_render_mode_alpha( 1 )

	self.ui.bu_name = bu

	local ix = 9
	local iy = 1
	self.ui.bu_name = self:add_text_info(	{ix,iy,	8,.8},	"name" )
	self.ui.bu_info = self:add_text_info(	{ix,iy+1,	8,.8},	"State" )

	iy = 3.25

	self:add_button( 			{ix,iy },		"V Flip",			ref.flip_v,	nil,	false	)

	bu = self:add_trig_method(	{1, 6,	 6, 1}, "Movie PathName",	self, "video_path" )
	bu = self:add_trig_method(	{9, 7,	 6, 1}, "Record",			self, "save_video" )
	bu = self:add_trig_method(	{9, 8,	 6, 1}, "Stop",				self, "close_video" )

	local bu = self:add_text(	{9,10,	6,1}, "tes.avi" )
	bu:set_color_back( { 0, 1, 1, .5 } )
	--bu:set_editable( true, "PC" )

--	bu = self:add_trig_method(	{	10, 2},		"Add keyboards",	self, "add_keyboard" )
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

function meu:init()
	local ref = self.ref

	ref.bdd_cv_writer =aaa.layer.get_bdd( self:get_layer(1) )
		ref.bind		=	param.get_ref( ref.bdd_cv_writer, "image_src" )
		ref.run			=	param.get_ref( ref.bdd_cv_writer, "run" )
		ref.filename	=	param.get_ref( ref.bdd_cv_writer, "filename" )
		ref.bgr			=	param.get_ref( ref.bdd_cv_writer, "flip_bgr" )
		ref.flip_v		=	param.get_ref( ref.bdd_cv_writer, "flip_vertical" )
		ref.frame_count	=	param.get_ref( ref.bdd_cv_writer, "frame_count" )

	self.bind_src	= 0
	self.filename	= "MovieTest.avi"

	self.path_video	= "../../AAAMovie"
	self.b_saving	= false
end

--todo optimize on change only
function meu:update_ui()
	local ui = self.ui
	local ref = self.ref
	local b_run = param.get_bool( ref.run )
	if b_run then
		ui.bu_name:set_text( self.full_path )
		ui.bu_info:set_text( "Record : "..param.get( ref.frame_count ) )
		ui.bu_info:set_text_color( "record" )
	else
		ui.bu_info:set_text( "Stopped" )
		ui.bu_info:set_text_color( "info" )
	end
end

function meu:update()
	local	ps	= param.set
	local	ref	= self.ref

	--self:print( "Here")
	local bind

--	self:print( "Filename is "..filename )

	GA:set_keyboard_physical_target_virtual( self.keyboard2 )
	bind = self:get_texture_bind_2d( 2 )	--todo 2 ???
	--self:print( "Bind is ".. bind )
	ps( ref.bind, bind )
end

function meu:start_video()
	param.set( self.ref.run, 1 )
	self.b_saving = true
end

function meu:stop_video()
	param.set( self.ref.run, 0 )
	self.b_saving = false
end

function meu:video_path( path)
	if( path ) then
		self.path_video = path
	else
		local title = "Movie Path"
		local chemin = aaa.file.do_dialog_folder( title )

		if chemin then
			self:print( "Path selected : "..chemin )
			self.path_video = chemin
		end
	end
	self:print( "Path selected : "..self.path_video )
end

function meu:set_filename( filename )
	local path = self.path_video.."/"

	self.filename = filename
	--	self.print( self.path_video )
	self:print( "Path is "..path )
	self:print( "Filename is "..filename )
	self.full_path = path..filename
	self:print( "Full path is "..self.full_path )

	param.set( self.ref.filename, self.full_path )
end

function meu:save_video( )
	if self.b_saving then return end

--	self:set_filename( self.bu_text_videoname:get_text() )
	self:set_filename( "test" )
	self:start_video()
end

function meu:close_video()
-- 	local t = aaa.time
	self:stop_video()
end


meu.draw_icon = MEU.draw_icon_base