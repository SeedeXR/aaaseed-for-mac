local folder = aaa.pc.is_maa() and "AAATree0K/" or "AAATree2K/"
local root = app.media_dir_rel.."AAATree/"..folder
local reader_nb_max = 15

if not aaa.lua.global.get( "arbre" ) then
	aaa.lua.global.declare_table( "arbre" )
end

function arbre:add_shading_selector( meu, rect )
	local bu = meu:add_selector(	rect, "Mode" )
		bu:set_nb_min_0( 5, 2 )
		bu:set_target_lua( meu, "s_mode" )
		bu:set_item_text( 1, "U", "U+K", "Khronos", "K+V", "V", "In", 1, 4, 9, 16 )
	return bu
end

function meu:get_preset_nb()	return 24	end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local SY

	local ix,iy = 1,1
--	self:add_button(		{ix, iy		},		"Draw",			self, "b_draw",				false )
--	iy = iy + 1
	bu = self:add_button(	{ix, iy		},		"Run",			self, "b_run",				false )
		bu:set_text_rect_ratio( 2 )
		bu:set_preset_use( false )
	bu = self:add_button(	{ix+2.5,iy	},		"Bio Active",	self, "b_bio_active",		false )

	iy = iy + 1.2
	bu = self:add_slider(	{ix,iy,		4,1},	"Len1",			self, "len_1",	90, 0, 90	)
		bu:set_preset_use( false )
	bu = self:add_slider(	{ix+4,iy,	4,1},	"Len2",			self, "len_2",	30, 0, 60	)
		bu:set_preset_use( false )
	self.len_1 = 100.
	self.len_2 = 45.

	iy = iy + 1
	bu = self:add_slider(	{ix,iy,		4,1},	"t1",			nil, nil,		0, 0, 90	)
		ui.bu_t1 = bu
	bu = self:add_slider(	{ix+4,iy,	4,1},	"t2",			nil, nil,		0, 0, 90	)
		ui.bu_t2 = bu


	iy = 4.8
	SY = .8
	bu =arbre:add_shading_selector( self, {ix,iy, 8,SY*2}, "Mode" )
		bu:set_preset_use( false )
		bu:set_value(1)
		self.kro = bu

	iy = iy + 2*SY
	bu = self:add_button(	{ix,iy		},		"Run Transition",	self, "b_run_transition",	true	 )
		bu:set_text_rect_ratio( 3 )
		bu:set_preset_use( false )

	bu = self:add_slider(	{ix+3,iy,	5,SY},	"Transition",		self, "transition",	0, 0,1 )
		bu:set_preset_use( false )

	ix,iy = 9,2.5
	bu = self:add_slider(	{ix,iy,		4,SY},	"Coverage_th",		self, "cov_th",	.1, 0,.2 )
		bu:set_preset_use( false )

	iy = iy + SY
	bu = self:add_button(	{ix,iy		},		"Simul",			self, "b_simul",			false )
		bu:set_text_rect_ratio( 2 )
		bu:set_preset_use( false )

	iy = iy + 1
	bu = self:add_slider(	{ix,iy,		4,1},	"Coverage",			self, "simul_cov" )
	bu = self:add_slider(	{ix,iy+1,	4,1},	"Fade",				self, "simul_fade" )
--	iy = iy + 1
--	bu = self:add_slider(	{ix+4,iy,	4,1},	"Depth",			self, "simul_depth" )

	bu = self:add_slider_two({ix+4,iy,	4,4},	"Position" )
		bu:set_text( "Pos" )
		bu:set_target_lua( self, "simul_pos_x", .5, 1  )
		bu:set_target_lua( self, "simul_pos_y", 0., 2  )
		--ui.bu_pos = bu

	ix = 1
	iy = 7.8
	SY = .5

	local seq_nb = 12
	bu = self:add_selector(	{ix,iy,	8,SY},	"Seqs" )
		bu:set_nb_min_0( seq_nb, 1 )
		bu:set_target_lua( self, "s_seqs" )
		bu:set_method_on_value_change( self, "change_readers", bu )

	iy = iy+SY+.2
	local bu_seqs = {}
	for i=1,reader_nb_max do
		bu = self:add_selector(	{ix,iy,	8,SY},	"Seq_"..i )
			bu:set_nb_min_0( seq_nb, 1 )
			bu:set_text_xy( 0, -1. )
			bu:set_method_on_value_change( self, "change_reader", i, bu )
			bu_seqs[i] = bu
		--bu = self:add_text_info(	{9,iy,	8,1}, seqs[i][5] )
		--	bu:set_text_line_length(1)
		iy = iy + SY
	end
	self.ui.bu_seqs = bu_seqs

		--bu:set_item_text( 1, "Carre", "Alcyone", "Zobi" )
	self:add_camera()

	ix,iy = 9,9
		bu = self:add_selector(	{ix,iy,	8,.6},	"Nb_playing" )
		bu:set_nb( reader_nb_max, 1 )
		bu:set_target_lua( self, "nb_to_play" )
		--bu:set_method_on_value_change( self, "change_readers", bu )
		bu:set_item_text_from_nb()
	iy = iy + .8
	SY = .8
	bu = self:add_slider(	{ix,iy,	8,SY},	"fps",				self, "fps",		1, 0., 60	)
		bu:add_values_def( 15, 30, 45 )

	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY},	"speed",			self, "speed",		1, 1., 64	)

	iy = iy + SY
	bu = self:add_slider(	{ix,iy+SY,	8,SY*2},	"Frame Offset",		self, "foffset",	1, -1, 1.	):set_meter( false )

end

function meu:init_seqs()
	if true then
		self.seqs =
		{
			{	root,	"TREE_2011_02_17h30_seq1_2day_MidnightOffset_00777/",	5732,	777	,	2,	"2011_02_17h30_seq1_2day"	},
			{	root,	"TREE_2012_01_18h00_seq3_3day_MidnightOffset_00670/",	7490,	670	,	3,	"2012_01_18h00_seq3_3day"	},
			{	root,	"TREE_2012_02_00h00_Seq1_1day_MidnightOffset_00001/",	2743,	1	,	1,	"2012_02_00h00_Seq1_1day"	},
			{	root,	"TREE_2012_03_17h00_Seq1_2day_MidnightOffset_00800/",	5579,	800	,	2,	"2012_03_17h00_Seq1_2day"	},
			{	root,	"TREE_2012_03_16h00_Seq2_1day_MidnightOffset_00950/",	2867,	950	,	1,	"2012_03_16h00_Seq2_1day"	},
			{	root,	"TREE_2012_05_00h00_seq1_1day_MidnightOffset_00001/",	2796,	1	,	1,	"2012_05_00h00_seq1_1day"	},
			{	root,	"TREE_2012_05_16h45_seq1_3day_MidnightOffset_00849/",	8380,	849	,	3,	"2012_05_16h45_seq1_3day"	},
			{	root,	"TREE_2011_10_13h00_seq1_5day_MidnightOffset_01202/",	13524,	1202,	5,	"2011_10_13h00_seq1_5day"	},
			{	root,	"TREE_2011_10_16h20_seq2_3day_MidnightOffset_00912/",	8532,	912	,	3,	"2011_10_16h20_seq2_3day"	},
			{	root,	"TREE_2011_10_00h00_seq2_1day_MidnightOffset_00001/",	2732,	1	,	1,	"2011_10_00h00_seq2_1day"	},
			{	root,	"TREE_2011_12_17h45_seq1_2day_MidnightOffset_00718/",	5506,	718	,	2,	"2011_12_17h45_seq1_2day"	},
			{	root,	"TREE_2011_12_17h45_seq2_1day_MidnightOffset_00623/",	2680,	623	,	1,	"2011_12_17h45_seq2_1day"	},
		}
	else
		self.seqs =
		{
			{	root,	"2011-10-15/", 		2731, 1	},
			{	root,	"2012-02-03-KO6L/",	2743, 1	},
			{	root,	"2012-05-15-KO6L/",	2796, 1	},
		}
	end

	-- 1- dir,  2-name, 3-frame_nb, 4-offet_to_midnight, 5-day_nb, 6-name_short
	for i=1,#self.seqs do
	--	self:print_inverse( "init_seqs( "..i.." )" )
		local seq = self.seqs[i]
		seq.dir				= seq[1]
		seq.name			= seq[2]
		seq.frame_nb		= seq[3]
		seq.frame_offset	= seq[4]
		seq.day_nb			= seq[5]
		seq.name_short		= seq[6]
		seq.day_frame_nb	= seq.frame_nb / seq.day_nb
		--self:print_inverse( "  day_nb "..seq.day_nb )
	end

end

function meu:set_reader_with_seq( reader_id, seq_id )
	local re = self.readers[reader_id]
	--self:print_inverse( "reader "..reader_id.." : seq_id is "..seq_id )
	local seq = self.seqs[seq_id]
	--table.print( seq,  "seq", 2 )
	re:init_dir( seq )
	re:init_day_offset_frame_by_day( seq.day_nb, seq.frame_offset, seq.day_frame_nb )
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
	--self:print_inverse( "change_reader( "..reader_id..", "..bu.." )" )
	self:set_reader_with_seq( reader_id, bu:get_value() + 1 )
end
function meu:change_readers( bu )
	local val = bu:get_value()
	for i, v in IPAIRS(self.ui.bu_seqs) do
		v:set_value( val )
	end
end

function meu:init()
	self:init_seqs()
	self.nb_reader_max = reader_nb_max
	local pool = IMG_READER_POOL_2D:create( "AAATree", 512, math.max( self.nb_reader_max*4, 32 ) )	-- 2 is a minimum
	self.readers = {}
	for i = 1,self.nb_reader_max do
		self.readers[i] = IMG_READER:create( "AAATree_"..i, pool )
	end
	self:set_readers_all_with_seq( self.nb_reader_max, 1 )

	local ref = self.ref
	ref.anal		= self:get_layer_bdd( 2 )
	ref.max_u		= param.get_ref( ref.anal, "max_u" )
	ref.max_v		= param.get_ref( ref.anal, "max_v" )
	ref.grey		= param.get_ref( ref.anal, "max_grey" )
	ref.coverage	= param.get_ref( ref.anal, "max_coverage" )
end

--[[
--local nb_img = 2731
local nb_img = 119
--nb_img = 2731
local dir = app.media_dir_rel.."AAATree/Test4K/"
function meu:read_img( bind, index )
	index = math.floor( math.fmod( index, nb_img ) ) + 1
	local name = string.format( "%stest_%04d.tga", dir, index )
	--self:print( name )
	--aaa.img.read( bind, name, false, true )
	aaa.img.read( bind, name )
end

function meu:set_config( id )
	if id <= 2 then
		self:set_readers_all_with_seq( self.nb_asked, id+1 )
	elseif id == 3 then
		self:set_readers_all_with_seqs( {1,2,3} )
	end
end
--]]

function meu:update()
	local meu_3d = self.meu_3d
	if not meu_3d then
		self.meu_3d = self:get_meu_by_name( "Shade3d_1" )
		meu_3d = self.meu_3d
		meu_3d:set_ui_slot( 4 )	--todo simplify
		self:set_ui_slot( 5 )
		self:get_mu_by_name( "ColorCurve_1" ):set_ui_slot( 3 )
	end

	local meu_tree = self.meu_tree
	if not meu_tree then
		self.meu_tree = self:get_meu_by_name( "ShaderTreeA_1" )
		meu_tree = self.meu_tree
		self.mu_tree = self.meu_tree:get_mu()
	end
	local mu_tree = self.mu_tree

	local function l_set_preset( v )
		self.ui.bu_preset:set_value( v )
		self:recall_preset( v )
	end

--SCENARIO
	local ref = self.ref
	local ui = self.ui
	if self.b_run then
		if self.b_run_transition then
			self:set_bu_value( "transition", math.sin( aaa.time.t * .01 * math.pi2 ) * .5 + .501 )
		end

		local grey
		local coverage
		local rot_x = 0
		local rot_y
		local fade
		local dt = aaa.time.dt
		if self.b_simul then
			grey		= self.simul_pos_y
			rot_y		= -(self.simul_pos_x - .5) * 1.
			coverage	= self.simul_cov
			--fade 		= self.simul_fade
			dt			= dt * 10.
		else
			grey		= param.get( ref.grey 		)
			rot_y		= 1 - param.get( ref.max_u		)
			rot_x		= param.get( ref.max_v		)
			rot_x 		= ( clamp( (rot_x - .04) / (.20 - .04), 0, 1 ) - .5 ) * .125;
			coverage	= param.get( ref.coverage 	) == 1 and 0 or 1
			fade 		= .5	--do
		end
		rot_y = rot_y * .5
		grey = clamp( (grey - .15) / (.75 - .15), 0, 1 );
		aaa.show( grey, "grey" )
		--[[
		--if math.fmod( aaa.time.t, .3 ) < .1 then
		if inside( grey, .0, .1 ) then
			--NOBODY
			--l_set_preset( math.random(8)-1 )
			self.kro:set_value(0)
		else	--if inside( grey, .1, .98 ) then
			--ACTIVITY
			--l_set_preset( math.random(8)-1+12 )
			--	U+K, bande + khronos
			self.kro:set_value(1)
		end
		--]]
		local t1 = self.t1 or 0
		local t2 = self.t2 or 0
		if self.b_bio_active then
			if not self.b_bio then
				if coverage > self.cov_th or t1 < 4. then
					t1 = t1 + dt
				else
					t1 = t1 - dt
				end
				t1 = clamp( t1, 0, self.len_1 )
				if t1 >= self.len_1 then
					t2 = 0
					self.b_bio = true
					if meu_3d then
						meu_3d:set_preset( math.random(21) )
					end
				end
				--fade = clamp( t1, 0, 1 )
				fade = clamp( 1.-t1*.3, 0, 1 )
			else
				if coverage > self.cov_th then
					t2 = t2 + dt
				else
					t2 = t2 - dt
				end
				t2 = clamp( t2, 0, self.len_2 )
				--self:print( t2 )
				if t2 == self.len_2 or t2 == 0 then
					t1 = 0
					t2 = 0
					--		if math.fmod( aaa.time.t, 180. ) < .2 then
					l_set_preset( math.random(20)-1 )
					--end
					self.b_bio = false
				end
				fade = clamp( t2*.3, 0, 1 )
			end
			self.t1 = t1
			ui.bu_t1:set_value( t1 )
			self.t2 = t2
			ui.bu_t2:set_value( t2 )

			--ROTATION BioEmergence
			rot_y = interpolate( self.rot_y or 0, rot_y , .05 )
			self.rot_y = rot_y
			param.set( meu_3d:get_camera(1), "rot_y", rot_y )
			rot_x = interpolate( self.rot_x or 0, rot_x , .05 )
			self.rot_x = rot_x
			param.set( meu_3d:get_camera(1), "rot_x", rot_x )

			--image_change

			grey = interpolate( self.grey or 0, grey , .04 )
			meu_3d:set_bu_value( "phase", grey )
			self.grey = grey

			--meu_3d:set_bu_value( "color_alpha", self.simul_pos_y )
			meu_3d:get_bu_by_key( "color_color" ):set_rgba( 1, 1, 1, fade )
			if mu_tree then
				mu_tree:set_value( 1. - fade )
			end
		end
	else
		if not self.__b_onsite_forced and app:is_onsite() then
			-- can't be done in init or define_ui
			self.__b_onsite_forced = true
			self:set_bu_value( "run", true )
		end
	end

	local day = (self.day or 0) + aaa.time.dt / 192. * self.speed * self.fps/15.--math.random(3);
	day = math.fmod( day, 120. )		-- 120 = 2*3*4*5  we wrap to avoid lost of precision on decimal
	--t = math.random( nb_img )
	self.day = day

	--t = math.random( nb_img )
	local nb = self.nb_to_play
	local day_factor = self.foffset / nb
	for i=1,nb do
		local re = self.readers[i]

		local l_day = day + (i-1) * day_factor
		--todo decalage
		--frame = frame + i/nb -- use to avoid asking frames at the same time
		local frame = re:set_frame_using_day( l_day )
	end

---[[
	if meu_tree then
		--self:print( "Got Meu "..m )
		meu_tree:set_bu_value( "mode", self.s_mode )
		meu_tree:set_bu_value( "transition", self.transition )
	end
--]]
end


function meu:draw()
	MEU.draw(self)

	--t = math.random( nb_img )
	local nb = self.nb_to_play
	local side = math.floor( math.sqrt(nb-1) + 1 )
	local S = 2 / side
	local SX = S * 1.77
	local SY = S

	local binds = {}
	local sf = 1
	for i=1,nb do
		local re = self.readers[i]

		local bind = re:get_bind()
		binds[i] = bind
		if bind then
			if self.b_draw then
				local x = (i-1) % side
				local y = ((i-1) - x) / side - side*.5
				x = x - side*.5
				x = x * SX
				y = y * SY
				aaa.draw_bind_rect( bind, x, y, x + SX*sf, y + SY*sf )
			end
		else
			--re:print( "No Bind")
		end

		if bind then
			aaa.show( aaa.format.int_to_char3(bind-512).." "..re:get_frame(), "v"..i )
		end
	end

	app.__tree_binds = binds
end

--[[
function meu:read_file_async( bind, index )
	index = math.floor( math.fmod( index, nb_img ) ) + 1
	--aaa.img.read( bind, name )
	local name = string.format( "%stest_%04d.tga", dir, index )
	aaa.img.read_file_async( bind, name )
end
	MEU.draw(self)

	local b_odd = not self.b_odd
	self.b_odd = b_odd

	local t = aaa.time.t * 50.;
	--t = math.random( nb_img )
	local nb = 4
	local side = math.floor( math.sqrt(nb-1) + 1 )
	local S = 2 / side
	local SX = S * 1.77
	local SY = S
	for i=1,nb do
		if GA.b_spy then aaa.spy.push_range( "Read", 2 ) end
		self:read_img( 1024+i+(b_odd and 0 or nb), t+i*nb_img/nb )
		if GA.b_spy then aaa.spy.pop_range() end
		local x = (i-1) % side - side*.5
		local y = ((i-1) - x) / side - side*.5
		x = x * SX
		y = y * SY
		aaa.draw_bind_rect( 1024+i+(b_odd and 0 or nb), x, y, x+SX, y+SY )
	end

	for i=1,0 do
		if GA.b_spy then aaa.spy.push_range( "Read_file", 2 ) end
		self:read_file_async(i%4+ 1, t+i*32 )
		if GA.b_spy then aaa.spy.pop_range() end
	end
--	to crash if not thread safe
--	aaa.img.read( 1044, app.media_dir_rel.."AAATree/Test2K/test_0024.tga" )

end
--]]