function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "2d", "3D", "Core", "CoreGraphic", "Camera", "device", "texture", "input", "unfinished", "depreciated" },
			help = "real the image fluxs from a Kinect camera\n"..
					"Depreciated ise MEU Kinect instead"
		}
end

function meu:define_ui_proto( b_ui )
	if b_ui then
		--aaa.box_good( self..":define_ui_proto()" )
		self:add_button( {1, 1 }, "allow",	self, "b_allow", true )
		self:add_button( {1, 2 }, "On CPU",	self, "b_on_cpu", true )
	else
		self.b_allow	= true
		self.b_on_cpu	= true
	end
end

local function pgr( ref, name, obj, pname )
	ref[name]	=	param.get_ref(	obj, pname and pname or name )
end
function meu:init()
	local ref = self.ref
	ref.video	= self:get_obj_down( "tex_video" )
	local o = ref.video
		pgr( ref, "on_cpu",					o, "image_on_cpu_wanted"	)
		pgr( ref, "move_to_texture",		o	)
		pgr( ref, "sx",						o, 	"size_x"	)
		pgr( ref, "sy",						o, 	"size_y"	)
		pgr( ref, "capture_open",			o	)
		pgr( ref, "capture_opened",			o	)
		pgr( ref, "capture_running",		o	)
		pgr( ref, "capture_device_name",	o	)
		pgr( ref, "capture_crossbar_name",	o	)
	ref.kinect  = aaa.obj.get_down_by_class( ref.video, "kinect_ui" )
	-- body
end
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	bu = self:add_trig_fn(	{1, 2}, "Focus Video",	aaa.obj.set_focus_ui, ref.video	):set_color_back("focus")
	bu = self:add_trig_fn(	{2, 3}, "Focus Kinect",	aaa.obj.set_focus_ui, ref.kinect	):set_color_back("focus")

	bu = self:add_text_info(	{1,1,	nil,nil}, "name" )
		--bu:set_value_load_save( false )
		self.ui.bu_name = bu

	self.ui.bu_info = self:add_text_info(	{12,2,	nil,nil},	"State" )

	local ix = 9
	local iy = 2

	iy = iy + 3.5

	iy = iy + 4
	self:add_button( {			ix, iy }, "On Cpu", 	self, "b_on_cpu", false	)

	--iy = iy + 4.5
	--self:add_param_obj_name(	{ix,iy},	"sound",	r, "sound_active",	0,1 )
	--self:add_param_obj_name(	{ix,iy+1},	"volume",	r, "volume",		0,1 )

	--local bind = param.get( r, "bind_dst" )

	self:add_monitor( {1,10.5} )
end

function meu:update_ui()
	local	pg	= param.get
	local 	ref,ui	= self.ref,self.ui
	local	sx,sy = pg( ref.sx ), pg( ref.sy )

	ui.bu_info:set_text( sx.." by "..sy )
	ui.bu_name:set_text( pg( ref.capture_device_name ) )
end



function meu:update()
	if self.verbose >= 2 then aaa.print_method() end

	local ref = self.ref

	--self:print( "update() "..self:get_proto().." "..self:get_proto().b_allow.." "..self:get_alpha() )
--[[
	local b
	b = (self:get_alpha()>0.) and self.b_play and self:get_proto().b_allow
	if param.get_bool( ref.video_playing ) ~= b then
		param.set( ref.video_play, b )
	end
--]]

	local b = self.b_on_cpu and self:get_proto().b_on_cpu
	param.set( ref.on_cpu,			b 		)
	param.set( ref.move_to_texture, not b	)		--	false is slower when on cpu is on
end

