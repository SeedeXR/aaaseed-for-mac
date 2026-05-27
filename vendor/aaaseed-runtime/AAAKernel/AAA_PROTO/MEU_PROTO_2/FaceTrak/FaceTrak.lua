function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "2d", "3d", "geometry", "texture", "output", "Point", "procedural" },
			help = 	{
					"Handle face tracking (from the dlib Library) used in bdd_img_face_tracker",
					"used in the App_Dior"
					}
		}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	ui.cam = self:add_camera()
	bu = self:add_text_info(	{9,2,	8,1}, "info" )
		ui.info = bu
	self:add_bu_texture_target_unit()
	self:add_monitor( {9,3} )
		bu:set_texture_bind_2d( self:get_bind_by_name( "F1" ) )
		bu:set_texture_flip_u( true )
		bu:set_texture_flip_v( true )

	local ix,iy = 1,5
	bu = self:add_button(	{ix, 		iy}, 	"points",	ref.bdd_face_track, "draw_point",	false )
		bu:set_text_rect_ratio( 2.5 )
	bu = self:add_button(	{ix+3, 		iy}, 	"2d",		ref.bdd_face_track, "draw_face_2d", false )
		bu:set_text_rect_ratio( 2.5 )
	bu = self:add_button(	{ix+6, 		iy}, 	"3d",		ref.bdd_face_track, "draw_face_3d", false )
		bu:set_text_rect_ratio( 2.5 )
	bu = self:add_button(	{ix+6, 		iy+1},	"Out",		ref.bdd_face_track,	"img_out",		false )
		bu:set_text_rect_ratio( 2 )

	iy = iy+1
	bu = self:add_button(	{ix,		iy}, 	"Validate half", FACE_INFO,	"b_validate_half",	true )
	iy = iy+1
	bu = self:add_button(	{ix,		iy}, 	"Map",			self,		"b_draw_map",		true )
	iy = iy+1
	bu = self:add_button(	{ix, 		iy}, 	"Grille",		self,		"b_draw_grille",	true )
		bu:set_text_rect_ratio( 2.5 )
	bu = self:add_button(	{ix+2.7,	iy},	"Point",		self,		"b_draw_point",		true )
		bu:set_text_rect_ratio( 2.5 )
	iy = iy+1
	bu = self:add_button(	{ix+2.7,	iy }, 	"line",			self,		"b_draw_line",		true )
		bu:set_text_rect_ratio( 2.5 )
	bu = self:add_button(	{ix+5.4,	iy }, 	"Bary",			self,		"b_draw_bary",		true )
		bu:set_text_rect_ratio( 2.5 )


	iy = iy + 3
	bu = self:add_button(	{ix, 		iy}, 	"All",			FACE_INFO,	"b_85", 			false )
	bu = self:add_button(	{ix+4,		iy}, 	"Dior",			FACE_INFO,	"b_dior", 			false )
	bu = self:add_button(	{ix, 		iy+1},	"eye brow",		FACE_INFO,	"b_eye_brow", 		false )
	bu = self:add_button(	{ix+4, 		iy+1},	"old",			FACE_INFO,	"b_old", 			true )
	bu = self:add_button(	{ix, 		iy+2},	"nose scale",	FACE_INFO,	"b_nose_scale",		false )

	ix = 9
	iy = 9
	bu = self:add_slider(	{ix,iy,		8,1},	"Delay",		self,		"pre_delay",		1 )
	bu = self:add_slider(	{ix,iy+1,	8,1},	"Pre" )

end

local FACE_NB = 1
function meu:init()
	local ref = self.ref
	local layers = self:get_layers()
	ref.bdd_face_track = aaa.obj.get_down_by_class( layers, "bdd_img_face_tracker" )
	ref.bdd_face_active = param.get_ref( ref.bdd_face_track, "active" )
	ref.image_src 		= param.get_ref( ref.bdd_face_track, "image_src" )
	ref.sx 				= param.get_ref( ref.bdd_face_track, "image_src_size_x" )
	ref.sy 				= param.get_ref( ref.bdd_face_track, "image_src_size_y" )
	ref.crop_left 		= param.get_ref( ref.bdd_face_track, "crop_left" 		)
	ref.crop_right 		= param.get_ref( ref.bdd_face_track, "crop_right" 	)
	ref.crop_bottom 	= param.get_ref( ref.bdd_face_track, "crop_bottom"	)
	ref.crop_top 		= param.get_ref( ref.bdd_face_track, "crop_top" 		)
	ref.img_src_sx 		= param.get_ref( ref.bdd_face_track, "image_src_size_x" 	)
	ref.img_src_sy 		= param.get_ref( ref.bdd_face_track, "image_src_size_y" 	)
	ref.detect_sx 		= param.get_ref( ref.bdd_face_track, "detect_size_x" 	)
	ref.detect_sy 		= param.get_ref( ref.bdd_face_track, "detect_size_y" 	)

	local bdd = aaa.obj.get_down_by_class_no_error( layers, "bdd_img_analyse" )
	if bdd then
		ref.max_grey		= param.get_ref( bdd, "max_grey" )
		ref.max_coverage	= param.get_ref( bdd, "max_coverage" )
	end

	--ref.model = aaa.layer.get_model( self:get_layer( 1 ) )
	ref.model = aaa.obj.get_down_no_error( layers, "model_face_tracker" )
	if ref.model then
		ref.sx			=	param.get_ref( ref.model, "size_u" )
		ref.sy			=	param.get_ref( ref.model, "size_v" )
		ref.sf			=	param.get_ref( ref.model, "size_factor" )
	end

	--local layer = self:get_layer( 2 )
	--ref.mapping = aaa.layer.get_mapping( layer )
	ref.mapping = aaa.obj.get_down_no_error( layers, "mapping_face_tracker" )
	if ref.mapping then
		ref.tex_left 	=	param.get_ref( ref.mapping, "tex_left" 		)
		ref.tex_right 	=	param.get_ref( ref.mapping, "tex_right" 	)
		ref.tex_bottom 	=	param.get_ref( ref.mapping, "tex_bottom"	)
		ref.tex_top 	=	param.get_ref( ref.mapping, "tex_top" 		)
		local layer = aaa.obj.get_root( ref.mapping  )
		ref.tex_bank_2d, ref.tex_bind_2d = aaa.layer.get_bank_bind_2d_ref( layer, 0 )
	end
	FACE_INFO.tex_bind = self:get_bind_by_name( "F4" )

	self.faces = {}
	--todo set max faces
	for id=1,FACE_NB do
		local face = FACE_INFO:create( "Face Trak "..id )
		face:set_target( ref.bdd_face_track, id )
		self.faces[id] = face
	end
end
function meu:update_ui()
	local face = self.faces and self.faces[1]
	if face then
		self.ui.info:set_text( face.face_nb.." "..face.model_nb )
		self:set_bu_value( "pre", face.pre )
	end
end

function meu:get_face_info( id )	return self.faces and self.faces[id]	end
function meu:get_track_rect()		return self.tex_rect_tracked			end
function meu:get_max_grey()
	local ref = self.ref
	return param.get( ref.max_grey ), param.get( ref.max_coverage )
end
function meu:update()
	--todo perhaps dont fit in the pipe any more

	local bind = self:get_texture_bind_2d()
	local bank_2d, bind_2d = self:get_texture_bank_bind()
	--self:print( bind.." "..bank_2d.." "..bind_2d )

	local ref = self.ref
	local pset = param.set
	pset( ref.bank_2d, bank_2d )
	pset( ref.bind_2d, bind_2d )
	if ref.mapping then
		pset( ref.tex_bank_2d, bank_2d )
		pset( ref.tex_bind_2d, bind_2d )
	end
	pset( ref.image_src, bind )
end

function meu:draw()
	local ref = self.ref
	local pget = param.get
	local pset = param.set

	local tex_rect = {}
	local tex_bottom
	local tex_top
	if ref.mapping then
		tex_rect.l = 1 - pget( ref.tex_left )
		tex_rect.r = 1 - pget( ref.tex_right )
		tex_bottom	= pget( ref.tex_bottom )
		tex_top		= pget( ref.tex_top )
		tex_rect.b = 1 - tex_bottom
		tex_rect.t = 1 - tex_top
		self.tex_rect_tracked = tex_rect
	end

	local img_sx = pget( ref.img_src_sx )
	local img_sy = pget( ref.img_src_sy )

	local detect_sx = pget( ref.detect_sx )
	local detect_sy = pget( ref.detect_sy )

	if ref.mapping then
		pset( ref.crop_left,	1-tex_rect.l )
		pset( ref.crop_right,	tex_rect.r )
		pset( ref.crop_bottom,	1-tex_rect.b )
		pset( ref.crop_top,		tex_rect.t )
	end

	local id = (self:get_inst_key() == "1") and 1 or 2
	local pre_raw = app:get_presence_raw(id) > 0 or not app:is_init_done()
	pset( ref.bdd_face_active, pre_raw )

--	pre_raw = true
	if pre_raw then
		MEU.draw( self )
	end

	local sx,sy
	if ref.model then
		local sf = pget( ref.sf )
		sx = pget( ref.sx ) * sf
		sy = pget( ref.sy ) * sf
	end

	aaa.bdd.set_lua_cur( self.ref.bdd_face_track )
	for i=1,FACE_NB do
		local face = self.faces[i]
		if tex_bottom then
			face.__tex_bottom	= tex_bottom
			face.__tex_top		= tex_top
		end
		face.__detect_sx	= detect_sx
		face.__detect_sy	= detect_sy
		local pre = false
		if pre_raw then
			face:update( tex_rect, detect_sx/detect_sy  )

			if face:is_use() then
				pre = true
				if sx and sy then
					face:use_transfo( 0, 0, sx, sy )
					if self.b_draw_map then
						--local ov = face.ov
						--face.ov = 0
						face:draw_face_map( 2, 0, sx, sy )
						--face.ov = ov
					end
				end
				if self.b_draw_grille then
					gol.color_red()
					face:draw_face_rect_line()
				end
				gol.color_white()
				if self.b_draw_point	then	face:draw_face_points()		end
				if self.b_draw_line 	then	face:draw_lines()			end
				if self.b_draw_bary 	then	face:draw_lines_bary_all()	end
			end
		end
		if pre then
			pre = 5.
		else
			pre = -1
		end
		local f = self.pre_delay
		f = (f > 0) and 1/f or 10000.
		face.pre = clamp_01( (face.pre or 1) + pre * aaa.time.dt * f  )
	end
--[[ test
	local t = {}
	local v = 0
	math.randomseed( 42)
	for i=1,8000 do
		table.insert( t, i*.00025)
		v = v + math.random()  * .02 - .01
		table.insert( t, v )
		table.insert( t, 0 )
	end
	gol.draw_line_strip_3d( t )
--]]
end


