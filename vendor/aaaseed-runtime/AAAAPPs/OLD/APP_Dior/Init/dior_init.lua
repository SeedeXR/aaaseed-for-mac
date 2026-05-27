if APP.DECLARE( "DIOR", APP_GP ) then
end

function DIOR:get_face_trak( facetrak_id )
	local name = "FaceTrak_"..facetrak_id
	return self:get_meu_by_name( name )
end
function DIOR:get_face_info( facetrak_id, face_id )
	local m = self:get_face_trak( facetrak_id )
	--self:print_error( "m is "..m )
	if m and m.get_face_info then
		--m:print( "m:get_face_info() ->  "..m )
		return m:get_face_info( face_id )
	end
end
function DIOR:get_face_track_rect( facetrak_id )
	local m = self:get_face_trak( facetrak_id )
	if m then return m:get_track_rect() end
end

function DIOR:is_blur_used()
	--local exp = app:get_experience_code()
	local seq_id_name	= string.lower( ga:get_seq_cur():get_title() )
	return seq_id_name~="wait" and seq_id_name~="welcome"
end

function DIOR:define_seqs()
	--aaa.box_warning( "DIOR:define_seqs()" )

	--aaa.audio.set_volume( self:is_onsite() and 1 or .8 )

	local seqs = SEQS:recreate( "Dior", self )

 	local function add_seq( name, level_id, seq_id, method_name, comment, duration )
		local seq = seqs:add_seq( SEQ:create( name, level_id, seq_id, comment, duration ) )
		--if seq_id>1 then
		--	seq:add_playlist( playlist, seq_id-1, 3. )
		--end
		seq:set_method( "update", self, method_name, seq )
		seq:set_ease_in_time( 1. )
		--seq:set_ease_out_time( 1. )
		return seq
	end

	local t_mu_face_track	=	{	"Ref_Fbo_f2",	"facetrak_1",
--									"Ref_Fbo_f6",	"facetrak_2"
								}
	--local imgs
	local seq
---[[
--	BEFORE
	local img_dir = "Text"
	local exp = app:get_experience_code()
	if exp=="HL" then
		local gp = app:get_gp()
		local ta, tb, tc = 19+12/30, 15+14/30, 24+20/30
		table.apply_fn( { "HemLight_1", "HemColor_1", "pre" }, function(name) gp:get_mu_by_name(name):set_value(0) end )
		seq = add_seq( "wait",		1, 1, "update_wait",	"on attend du public" )	--		, 1. )	--eotodo this is for test
		seq = add_seq( "welcome",	1, 2, "update_welcome", "Yes we track", 		6.  + 1./30		)
		seq = add_seq( "Sorbet",	1, 3, "update_anims",	"Sorbet", 				24. + 4./30.	)
		seq = add_seq( "Nature_a",	1, 4, "update_anims",	"Nature_a",				19. + 12./30.	)
		seq = add_seq( "Nature_b",	1, 5, "update_anims",	"Nature_b",				15. + 12./30.	)
		seq = add_seq( "Nature_c",	1, 6, "update_anims",	"Nature_c",				21. + 2./30.	)
		seq = add_seq( "end",		1, 7, "update_end",		"SumUp and Packshot",	13. + 25./30.	)
			seq:set_ease_in_time( 5. )
	elseif exp=="DSA" then
		local dur_def = 20.
		local t_mu_color		=	{	"HemColor_1",	"pre" }
		local t_mu_light		=	{	"HemLight_1",	"pre" }

		seq = add_seq( "wait",		1, 1, "update_wait",	"on attend du public" )	--		, 1. )	--eotodo this is for test

		seq = add_seq( "welcome",	1, 2, "update_welcome", "Yes we track", 		11 )

		seq = add_seq( "Texture",	1, 3, "update_texture",	"smooth the skin", 		dur_def )
	--		seq:add_mu_used_by_name( t_mu_face_track )

		seq = add_seq( "Color",		1, 4, "update_color",	"unify the skin",		dur_def )
	--		seq:add_mu_used_by_name( t_mu_face_track )
			seq:add_mu_used_by_name( t_mu_color		)
	--		seq:set_method( "update", self, "update_gene", seq )

		seq = add_seq( "Light",		1, 5, "update_light",	"light the skin",		dur_def )
			seq:add_mu_used_by_name( t_mu_light		)

		seq = add_seq( "end",		1, 6, "update_end",		"SumUp and Packshot", 	17 + 1.6 * 3 )
			--seq:add_mu_used_by_name( t_mu_face_track )
			seq:set_ease_in_time( 5. )
	end

	seqs:begin( true )
end

function DIOR:get_zone( id )
	return self.zone[id]
end

function DIOR:init_zone()
	self.zone = {}
	for i=1,self.nb_face do
		local zone = {
						presence_raw = 0,
						presence = 0,
						tracking = 0,
						inter = 0,
					}
		self.zone[i] = zone
	end
end

function DIOR:do_touch_none()
	if not ga:is_ui_group_active() or ga:is_ui_group_active( "customer" ) then
		ga:flip_ui_group_active( "customer" )
	end
end

function DIOR:init_app( ... )
	self.nb_face = 1
	self:init_experiences()
	self:init_lang()
	if not oo.getsuper(DIOR).init_app( self, ... ) then return end

	ga:define_ui_customer( self )
	ga:set_ui_group_active( false,	"customer" )

--	bus:set_method( "draw_after",	self,	"draw_scope"	)
	self:update_experience()

	self:load_medias()

	self:init_zone()

	local mu = self:get_mu_by_name( "UI_1" )
	mu:set_ui_slot( 2 )
	--	used
	self.meu_video_gene	= self:get_meu_by_name(	"Video_C"	)
	self.mu_f2			= self:get_mu_by_name(	"fbo_F2"	)
	self.mu_grid		= self:get_mu_by_name(	"grid_1"	)
	self.mu_f6			= self:get_mu_by_name(	"fbo_F6"	)
	self.mu_f7			= self:get_mu_by_name(	"fbo_F7"	)
	self.mu_texcorrect = {}
	for i=1,4 do
		self.mu_texcorrect[i] = self:get_mu_by_name(	"DiorTexCorrect_"..i )
	end
	self.mu_facetrak	= self:get_mu_by_name(		"FaceTrak_1"	)
	self.mu_blur		= self:get_mu_by_name(		"DiorBlur_1"	)

	aaa.audio.set_volume( 1 )

	self:define_seqs()

	self:get_meu_by_name( "Video_B" ):set_capture_open_run( true )

	local sel = self:is_onsite() and 3 or 1
	--keep on only film or camera
	for i=1,8 do
		self:get_mu_by_name(		"Video_"..string.char(64+i)	):set_value( i==sel and 1 or 0 )
	end

	ga:add_ga_and_bu_windows( nil, { "SEQS", "SEQ", "PLAYLIST" } )

	local binds = {}
	binds.dash			= TEXS:get_bind_by_name( "F9" )
	binds.grad_vert		= TEXS:get_bind_by_name( "F11" )
	binds.grad_radial	= TEXS:get_bind_by_name( "F12" )
	binds.src_small		= TEXS:get_bind_by_name( "F6" )
	binds.src			= TEXS:get_bind_by_name( "F7" )
	binds.blur			= TEXS:get_bind_by_name( "F8" )
	binds.pre			= TEXS:get_bind_by_name( "Pre" )
	self.binds = binds

	return true
end

