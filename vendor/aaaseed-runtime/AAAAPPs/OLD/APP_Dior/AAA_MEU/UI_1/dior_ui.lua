function meu:define_seq_ui(bus)
	self.bu_seq = self:add_bu_sequence( bus, ga:get_seqs() )
end

function meu:set_vol(vol)
	aaa.audio.set_volume(vol)
end

function meu:define_ui()
	local bu
	local par

	self.__b_need_seqs_define_ui = true

	local ix, iy = 9, 1
	local dx = 2.5
	bu = self:add_button( 	{ix + dx, iy}, "Simul", app, "b_simul", false)
	bu:set_text_rect_ratio(3)
	bu = self:add_slider(	{ix,iy,	dx,1}, "A")

	iy = iy + 1

	local function add_slider_out(ix, iy, name)
		local bu = self:add_slider(	{ix,iy,	2,2}, name)
		bu:set_ui_active(false)
		return bu
	end
	bu = add_slider_out(ix, iy, "Pre")

	bu = self:add_slider(	{ix + 4,iy,	4,1}, "Time Up", app, "time_delay_up", 2, 1, 10)
	bu = self:add_slider(	{ix + 4,iy + 1,	4,1}, "Time Down", app, "time_delay_down", 4, 1, 10)

	iy = 10
	bu = self:add_button(	{ix, iy}, "Force process", self, "b_process_force", false)
	bu:set_text_rect_ratio(8)

	--	bu = self:add_trig_method(	{	ix,	iy+1},		"Test", self, "do_a_copy" )
	--		bu:set_text_rect_ratio( 8 )

	--[[
	iy = 8
	bu = self:add_selector(	{ix,iy,	8,1},	"Langue" )	--notext not save in preset
		bu:set_nb_min_0( 4, 1 )
		bu:set_item_text( 1, "Francais", "Anglais", "Espagnol", "Signes" )

	iy = iy + 2
	bu = self:add_trig_method(	{ix, iy,			4, 1}, "Normal",		self, "set_vol", 1	)
	bu = self:add_trig_method(	{ix+4, iy,		4, 1}, "Low",		self, "set_vol", .5	)
	iy = iy + 1
 	bu = self:add_slider(	{ix,iy,	8,1}, "Volume", 	self, "volume_ui",	1, 0, 1 )
	bu = self:add_slider(	{ix,iy+1,	8,1}, "Music",  	self, "vol_mus_ui",	1, 0, 1 )
	bu = self:add_slider(	{ix,iy+2,	8,1}, "Voix",  		self, "vol_voi_ui",	1, 0, 1 )
--]]
end

function meu:set_process( b_on, id_texcorrect )
	--aaa.print_fn()
	--	local val = b_on and 1 or 0
	--	app.mu_f2:set_value( val )

	local b_render_skip = not b_on
	if b_render_skip then
		app.mu_f6:set_render_skip_next(true)
		app.mu_grid:set_render_skip_next(true)

		app.mu_f7:set_render_skip_next(true)
		--app.mu_facetrak:set_value( val )
		local mus = app.mu_texcorrect
		for i = 1, #mus do
			local mu = app.mu_texcorrect[i]
			if i == id_texcorrect then
				-- next line will have an effect only next frame but this is ok because it don't change dynamicly
				mu:set_value(1)
				app.mu_texcorrect_cur = mu
			else
				-- next line will have an effect only next frame but this is ok because it don't change dynamicly
				mu:set_value(0)
			end
			mu:set_render_skip_next(true)
		end
		app.mu_blur:set_render_skip_next(true)
	else
		if not app:is_blur_used() then
			app.mu_blur:set_render_skip_next(true)
		end
	end
end

function meu:init()
	local video_table = {}
	video_table["Video_A"] = 1
	video_table["MaaPort"] = 2
	video_table["Video_B"] = 3
	video_table["Video_C"] = 4
	self.video_table = video_table
end

function meu:update()
	local video = MEU.get_inst_last_by_type("Video")
	if not video then
		return
	end

	local video_name = video:get_name)
	--aaa.print( video_name )

	local index = video:get_image_index()
	--aaa.print( index )
	--self:print( "value is                                                      "..self.__frame_used.." "..index )
	local b_new = self.b_process_force or (self.__frame_used ~= index)
	if b_new then
		self.__frame_used = index
	end
	self:set_process(b_new, self.video_table[video_name])
	--self:set_process( true, self.video_table[video_name] )

	local pre
	if app.b_simul then
		pre = self:get_bu_value("a")
	else
		local fi = app:get_face_info(1, 1)
		pre = fi and fi:get_face_nb() or 0
	end
	pre = app:update_presence(pre)
	self:set_bu_value("pre", pre)

	--	this way the seqs can be initialized after the GP
	local seqs = self:check_need_define_seq_ui(ga:get_seqs())

	app:update_sound(self.volume_ui, self.vol_mus_ui, self.vol_voi_ui)
	app:sync_ui_customer()

	--self:print( pre )
	if seqs:is_run() then
		local seq_id = seqs:get_seq_cur_id()
		if seq_id == 1 then
			if pre == 1 then
				seqs:inc_seq()
			end
		else
			if seq_id ~= 7 then
				if pre == 0 then
					seqs:restart(1)
				end
			end
		end
	end
end
