local folder = aaa.pc.is_maa() and "BioEmergences/" or "BioEmergences/"
local root = app.media_dir_rel..folder
local root_bis = "C:/AAA/Media/"..folder
local reader_nb_max = 2

function meu:get_preset_nb() 	return 24, 2	end

function meu:init_seqs()
	self.seqs =
	{
		{	root,	"080618b",		77,		"ch00"	},		--1
		{	root,	"080618b",		77,		"ch01"	},
		{	root,	"071226a",		231,	"ch00"	},		--3
		{	root,	"071226a",		231,	"ch01"	},
		{	root,	"080528aF",		144,	"ch00"	},		--5
		{	root,	"080528aF",		144,	"ch01"	},
		{	root,	"070418a",		761,	"ch00"	},		--7
		{	root,	"070418a",		761,	"ch01"	},
		{	root,	"141001aR",		386,	"ch00"	},		--9
		{	root,	"141001aR",		386,	"ch01"	},
		{	root,	"160823aR",		402,	"ch00"	},		--11
		{	root,	"160823aR",		402,	"ch01"	},
		{	root,	"161031aF",		612,	"ch00"	},		--13
		{	root,	"161031aF",		612,	"ch01"	},
		{	root,	"161109a",		521,	"ch00"	},		--15
		{	root,	"161109a",		521,	"ch01"	},
		{	root,	"170109aF",		669,	"ch00"	},		--17
		{	root,	"170109aF",		669,	"ch01"	},
		{	root,	"170201aM",		444,	"ch00"	},		--19
		{	root,	"170201aM",		444,	"ch01"	},
		{	aaa.pc.is_maa() and root_bis or root,
					"161214aF",		707,	"ch00"	},		--21
	}

-- 1- dir,  2-name, 3-frame_nb, 4-offet_to_midnight, 5-day_nb, 6-name_short
	for i=1,#self.seqs do
		--self:print_inverse( "init_seqs( "..i.." )" )
		local seq = self.seqs[i]
		seq.dir				= seq[1]
		seq.name			= seq[2]
		seq.frame_nb		= seq[3]
		seq.post			= seq[4]
	end
end

local function __make_info_name( re, dir, frame )
	local seq = re.seq
	--re:print( "dir is : "..dir )
	local name = string.format( "%s/%s/%s_t%03d_%s.vtk", dir, dir, dir, frame-1, seq.post )
	--re:print( "name is : "..name )
	return name
end

function meu:set_reader_with_seq( reader_id, seq_id )
	local re = self.readers[reader_id]
	--self:print_inverse( "reader "..reader_id.." : seq_id is "..seq_id )
	local seq = self.seqs[seq_id]
	--table.print( seq,  "seq", 2 )
	re:init_dir( seq )
--	re:init_day_offset_frame_by_day( seq.day_nb, seq.frame_offset, seq.day_frame_nb )
end
function meu:set_readers_all_with_seq( nb, seq_id )
	--self.nb_reader = nb
	for i = 1,nb do
		self:set_reader_with_seq( i, seq_id )
	end
end
function meu:update_readers( bu )
	local val = bu:get_value() + 1
	--self:print( "seq is "..val )
	self:set_readers_all_with_seq( reader_nb_max, val )
end
function meu:change_reader( reader_id, bu )
	if self:is_preset_recalling() then	--avoid ColorCurve being altered at load 
		return
	end
	--self:print_inverse( "change_reader( "..reader_id..", "..bu.." )" )

	self:set_reader_with_seq( reader_id, bu:get_value() )
	if self.b_pal_link then
		local m = self:get_meu_by_name( "ColorCurve_1" )
		if m then
			--self:print_inverse( "change( "..(bu:get_value()+1).." )" )
			m:set_preset( bu:get_value()+1 )
		end
	end
end
function meu:change_readers( bu )
	local val = bu:get_value()
	for i, v in IPAIRS(self.ui.bu_seqs) do
		v:set_value( val )
	end
end
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local ix, iy
	local SY = 1
	local DY = .4

	ix, iy = 1,1
	bu = self:add_text_info(	{ix,iy,	8,SY}, "info_img3" )
		ui.info_img3 = bu
	local ny = 3 * SY
	iy = iy + SY
	bu = self:add_selector(		{ix,iy,	8,ny},	"Tex3d"	)
		bu:set_nb_min_0( 8, 4 )
		bu:set_target_lua( self, "s_bind_3d" )
		bu:set_item_text( 32, "No" )
	iy = iy + ny + DY

	bu = self:add_text_info(	{ix,iy,	8,SY}, "info_name" )
		ui.info_name = bu
	iy = iy+SY
	ny = SY*.8
	local function add_seq_sel( id )
		bu = self:add_selector(	{ix,iy,	8,ny},	"seq_"..id	)
			bu:set_nb( 21, 1 )
			bu:set_method_on_value_change( self, "change_reader", id, bu )
			for i=2,20,2 do
				bu:set_item_text( i, i )
			end
		iy = iy+SY
	end
	add_seq_sel( 1 )
	add_seq_sel( 2 )

	bu = self:add_button(	{ix+2,iy, 4, ny},	"pal_link",		self, "b_pal_link"	)
		bu:set_preset_use( false )
	iy = iy + ny + DY

	--iy = iy-SY*.2
	bu = self:add_text_info({ix,iy,		4,SY}, "info_frame" )
		bu:set_text_align_x( "right" )
		ui.info_frame = bu


	bu = self:add_text_info({ix+4,iy,	4,SY}, "info_frame_nb" )
		ui.info_frame_nb = bu

	iy = iy+SY
	bu = self:add_button(	{ix,iy,		SY,SY},	"play",			self, "b_play", 	false	)
		bu:set_text_visible( false )
		bu:set_preset_use( false )
	bu = self:add_slider(	{ix+SY,iy,	4-SY,SY},	"fps",			self, "fps",		15, 0., 60	)
		bu:add_values_def( 15, 25, 30, 50 )
		bu:set_preset_use( false )
	bu = self:add_slider(	{ix+4,iy,	4,SY},	"speed",		self, "speed",		1, -16, 16	)
		bu:set_meter( false )
		bu:set_preset_use( false )
	iy = iy+SY

	bu = self:add_slider(	{ix,iy,		8,SY},	"phase",		self, "phase"			)
		bu:set_preset_use( false )
		ui.bu_phase = bu
	iy = iy + SY + DY

	bu = self:add_slider(	{ix,iy,		8,SY},	"ZFactor",		self, "z_factor", 1, 1, 4	)
	iy = iy + SY + DY

	self:add_button(		{ix,iy,		2,SY},	"Crop", 			self,	"b_crop",					false )
	self:add_slider_two(	{ix+2,iy,	2,SY},	"U", 				self,	"u_min", "u_max"			)
	self:add_slider_two(	{ix+4,iy,	2,SY},	"V", 				self,	"v_min", "v_max"			)
	self:add_slider_two(	{ix+6,iy,	2,SY},	"W", 				self,	"w_min", "w_max"			)

--------------------------------------
	ix, iy = 9,2.5
	bu = self:add_camera()
		bu:set_preset_use( false )

	bu = self:add_selector(	{ix+2,iy,	4,SY},	"Render"	)
		bu:set_nb_min_0( 2, 1 )
		bu:set_target_lua( self, "s_render" )
		bu:set_item_text( 1, "Plane", "Voxel" )
--	iy = iy + 1
	ui.bu_reload= {}
	bu = self:add_trig(		{ix,iy,		2,SY},		"Plane",		self.sha[1].ref.reload_all,	nil,	false )
		ui.bu_reload[1] = bu
	bu = self:add_trig(		{ix+6,iy,	2,SY},		"Voxel",		self.sha[2].ref.reload_all,	nil,	false )
		ui.bu_reload[2] = bu
	iy = iy + SY

	bu = self:add_selector(	{ix,iy,		8,SY},	"How"	)
		bu:set_nb( 13, 1 )
		bu:set_min_max( -3, 9 )
		bu:set_target_lua( self, "s_how" )
		bu:set_item_text( 1, 3, 2, 1, "uvw", "", "Nor", "Nor1", "Light", nil, "6","7","8", "Tim" )
	iy = iy + SY + DY
	bu = self:add_slider(	{ix,iy,		4,SY},		"definition",		self,	"f_definition",				1., .01, 16	)
		:set_min_max_strict(true)

	bu = self:add_button( 	{ix+4,iy,	SY,SY},		"alpha_crop",		self,	"b_alpha_crop",				false )
		bu:set_text_visible( false )
	bu = self:add_slider_two({ix+4+SY,iy,4-SY,SY},	"Alpha_MinMax",		self,	"alpha_min", "alpha_max"	)
	iy = iy + SY + DY

	local SYC = 1.5
	bu = self:add_button(	{ix,iy,		SY,SY},		"map",				self,	"b_pal_use",				false )
		bu:set_text_visible( false )
	bu = self:add_slider(	{ix+SY,iy,	4-SY,SY},	"Palette_v",		self,	"palette_v",				.25, .0, 1.	)
	bu = self:add_slider(	{ix+4,iy,	4,SY},		"FColor",			self,	"f_color",					1., .0, 4.	)
	iy = iy + SY
	bu = self:add_button( {	ix+2, iy,	2,SY },		"More_inverse",		self,	"b_more_inv",				false )
		bu:set_text( "Inv" )
	bu = self:add_slider(	{ix+4,iy,	4,SY},		"FMore",			self,	"f_more",					1., 0., 4.	)
	iy = iy + SY + DY
	bu = self:add_rgbf(		{ix,iy,		8,SYC}, 	"light", false )
		ui.bu_light = bu
	iy = iy + SYC + DY
	bu = self:add_slider(	{ix+4,iy,	4,SY},		"FAlpha",			self,	"f_alpha",					1., .0, 2.	)
--	bu = self:add_slider(	{ix,iy,		4,SY},		"FineAlpha",		self,	"fine_alpha",				1., .01, 1.	)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,SY},		"Alpha_Stop",		self,	"alpha_stop",				1., .0, 1.	)

--		bu:set_value_type_integer( true )
--	bu = self:add_slider(	{ix+4,iy,	4,1},		"Maa_nb",		self, "maa_nb",	128, 1, 2048	)
--		bu:set_value_type_integer( true )
	iy = iy + SY + DY

	self:add_rgbf(			{ix,iy,	8,SYC},	"color" )
	iy = iy + SYC + DY
end


function meu:init()
	local ref = self.ref

	self:init_seqs()
	self.nb_reader_max = reader_nb_max
	local pool = IMG_READER_POOL_3D:create( "BioEmergences", 32, 4 )	-- 2 is a minimum
	self.readers = {}
	for i = 1,self.nb_reader_max do
		local re = IMG_READER:create( "BioEmer_"..i, pool )
			re.make_info_name = __make_info_name
			self.readers[i] = re
	end
	self:set_reader_with_seq( 1, 3 )
	self:set_reader_with_seq( 2, 4 )

	self.frame = 0

	local layer = self:get_layer( 1 )
	ref.bind_3d = param.get_ref( layer, "bind_3d" )
	local model = aaa.obj.get_down_by_class( layer, "model" )
	ref.size_axe = param.get_ref( model, "size_axe" )

	local GRID_INDEX = 2
	ref.grid = self:get_layer_bdd( GRID_INDEX )
	ref.grid_nb = param.get_ref( ref.grid, "nb_axe" )

	ref.active = {}
	self.sha = {}
	for i=1,2 do
		local layer = self:get_layer_ref_table(i+1)
		ref.active[i] = param.get_ref( layer.obj, "active" )
		self.sha[i] = self:create_shading_from_layer( "vol3d_"..i, i+1  )
	end
end

function meu:update_ui()
	local ui = self.ui
	for i=1,2 do
		ui.bu_reload[i]:set_text_color( self.sha[i]:is_valid() and "valid" or "invalid" )
	end
end

function meu:update()
--	self:print( self.f_definition )
	local ref = self.ref
	local ui = self.ui

	local bind = self.s_bind_3d
	local name
	if bind ~= 31 then
		param.set( self.ref.bind_3d, bind )
		name = aaa.img3d.get_bind_filename(	bind )
	else
		local readers = self.readers

		local frame
		local bu_phase = ui.bu_phase
		local frame_nb = readers[1]:get_frame_nb()
		if self.b_play then
			frame = self.frame + aaa.time.dt * self.speed * self.fps
		else
			frame = bu_phase:get_value() * (frame_nb-1)
		end
		self.frame = frame

		local nb = 1
		local lframe
		for i=1,nb do
			local re = readers[i]
			re:set_frame( frame )
		end

		local re = readers[1]
		frame = re:get_frame()
		self.ui.info_frame:set_text( frame )
		self.ui.info_frame_nb:set_text( "/ "..frame_nb )
		name = re.seq.name
		if self.b_play then
			bu_phase:set_value( (frame -1) / (frame_nb-1) )
		end
	end
	self.ui.info_name:set_text( name )

---[[
	local bind = self.bind
	if bind then
		local px,py,pz = aaa.img3d.get_size( bind )
		local a_min, a_max
		if self.b_alpha_crop then
			a_min, a_max =	self.alpha_min,	self.alpha_max
		else
			a_min, a_max =	0,				1
		end
		if pz then
			local str = aaa.img3d.get_format_name( bind )
			self.ui.info_img3:set_text( px.." x "..py.." x "..pz.." x "..str )
			local s_axe = pz * self.z_factor / px
			param.set( ref.size_axe, s_axe )
			for i=1,2 do
				local sha = self.sha[i]

				local alpha = self.f_alpha
				if i==2 then
					alpha = alpha / self.f_definition
				end
				sha:set_frag_float_1_4( self.f_color, alpha, 1./s_axe, self.palette_v )
				-- CROP
				if self.b_crop then
					sha:set_frag_vec4( 1,	self.u_min,	self.v_min,	self.w_min	)
					sha:set_frag_vec4( 2,	self.u_max,	self.v_max,	self.w_max	)
				else
					sha:set_frag_vec4( 1,	0, 			0,			0			)
					sha:set_frag_vec4( 2,	1, 			1,			1			)
				end

				sha:set_frag_vec4( 3,	a_min, a_max,	self.alpha_stop		)

				sha:set_frag_int( 1, self.s_how )
				sha:set_frag_int( 3, self.b_pal_use and 1 or 0 )
			end
			local step_nb = pz * self.f_definition
			param.set( ref.grid_nb, step_nb )
			local sha2 = self.sha[2]
			sha2:set_frag_int( 2, step_nb )
			local r,g,b = ui.bu_light:get_rgba()
			sha2:set_frag_vec4( 8, r,g,b, self.b_more_inv and -self.f_more or self.f_more		)
		else
			self.ui.info_img3:set_text( "No 3d texture" )
		end

	end
--]]

	local sel = self.s_render + 1
	for i =1,2 do
		param.set( ref.active[i], sel==i )
	end

end


function meu:update_bind()
	local bind = self.s_bind_3d
	if bind ~= 31 then
		param.set( self.ref.bind_3d, bind )
	else
		local nb = 1
		for i=1,nb do
			local re = self.readers[i]
			bind = re:get_bind()
			if bind then
				param.set( self.ref.bind_3d, bind )
			else
				--re:print( "No Bind")
			end

			if bind then
				aaa.show( aaa.format.int_to_char3(bind-512).." "..re:get_frame(), "v3d "..i.." : " )
			end
		end
	end
	self.bind = bind
end

function meu:draw()
	self:update_bind()

	MEU.draw(self)
end