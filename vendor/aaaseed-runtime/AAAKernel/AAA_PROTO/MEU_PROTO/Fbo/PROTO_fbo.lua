--aaa.print_inverse( "----------------- RUN the fbo script -----------------" )

if CLASS.DECLARE( "MEU_FBO", MEU ) then
	MEU_FBO:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
									"This is a digital canvas:",
									"Fbo regroup textures(attachments) to draw to" )
end

function MEU_FBO:define_meu_infos( )
	return	{ name_long = "Frame Buffer Object",
				tags = { "Texture", "Core", "CoreGraphic", "2d", "Draw", "texture", "VJ" },
			}
end

local s_verbose = 0

-- function MEU_FBO.create_instance( class, name )
--     local self = oo.getsuper(MEU_FBO).create_instance( class, name ) -- create a new instance of the class using GABU_OBJ (the super)
--     self.__fbo_free = {}
-- 	return self
-- end

--todo sizes, channel nb and type have to be stored in preset
--todo no save when no attachment to save in ui 
local TEX_COPY_ID_BEGIN = 32
function MEU_FBO:get_texture_bind_2d( id )
	id = id or 1
	if id >= TEX_COPY_ID_BEGIN then	--this is the copy texture interface
		return MEU.get_texture_bind_2d( self, id )
	end
	--self:print( "fbo is "..self:get_fbo() )
--UI
	return self:get_fbo():get_attachment_bind( id )
end

--todo add factor for each texture and button to invert texture
--todo or better a min max for each
--todo later use u of "line" Texture
function MEU_FBO:define_ui()
	local ref = self.ref
	local bu
	local par
	local ui = self.ui

	local ix,iy = 1,1
	local SX = 1
	local SY = .95
	local DY = .2

	local fbo = self.ref.fbo

	bu = self:add_param(		{ix,iy,			3,SY},		"Active",	fbo.ref.active )
		--bu:set_text( "Active" )
		ui.bu_fbo_active = bu
	self:add_trig_method(		{ix+5,iy,		3,SY},		"Focus",	fbo, "set_focus" )
		--bu:set_text( "Focus" )
-- debug info
	bu = self:add_text_info(	{ix+3,iy+SY/2,	2,SY/2},	"Ite_nb"	)
		:set_text()
		:set_text_align_x( "center" )
		ui.bu_ite_nb = bu
	iy = iy + SY

	
--size  / format
	self:begin_bu_group( "fbo" )

	local dx = .05
	self:add_pixel_size(		{ix+dx,iy, 		4-dx*3/2,SY*3},	fbo )
	self:add_pixel_format(		{ix+4+dx/2,iy,	4-dx*3/2,SY*3},	fbo )

	iy = iy + 3*SY + DY

	local SYA = .8
	SX = 4
	local function add_selector_attachment( x,y,	sx, name, b_depth )
		local bu = self:add_selector(	{x,y,	sx,SYA}, name ):set_selection_multiple( true )
		local nb =  self.__attachment_nb_ui
		bu:set_nb( b_depth and (nb+1) or nb )
		bu:set_item_text_from_nb()
		if b_depth then
			bu:set_item_text( nb+1, "Depth" )
		end
		return bu
	end
	local sxa = 8/5*4
--	local DYA = .3
	iy = iy + DY
	bu = add_selector_attachment( ix,iy,	sxa,			"Attachment active" )
		bu:set_sel_multiple_item( 1, true )
		--bu:set_target_lua( self.__b_t_tex_active )
		ui.bu_sel_tex_active = bu
	bu = self:add_param(		{ix+sxa,iy,	 8-sxa,SYA},	"Depth",	fbo.ref.depth.active	)
	iy = iy + SYA + DY
	--to use it, sync with param : do set_multiple and target
	-- bu = add_selector_attachment( ix, iy,						"Attachment mipmap" )
	-- 	--bu:set_sel_multiple_item( 1, true )
	-- 	ui.bu_sel_mipmap = bu

--	iy = iy + SY
	bu = self:add_selector(		{ix,iy,		8,SYA},			"See" )
		bu:set_nb( self.__attachment_nb_ui+1, 1 )
		bu:set_item_text_from_nb()
		bu:set_item_text( 5, "Depth" )
		--bu:set_text_visible( false )
		ui.bu_see = bu
	iy = iy + SYA

	self:end_bu_group()	--todo deal with fact that monitor is appart because if __bu_active_master
						--todo have a push/pop too

	bu = self:add_monitor( {ix,iy,	8,5} )
		:add_buttons_below( self )
		ui.bu_mon = bu
		
	iy = iy + 6

	self:begin_bu_group( "fbo" )

	bu = add_selector_attachment(	ix,iy,	8,			"Attachment On Cpu", true )
	ui.bu_sel_tex_cpu = bu
	iy = iy + SYA + DY
	--self:add_param( {ix+4, iy}, "stencil", ref.fbo_stencil_use )
	bu = add_selector_attachment(	ix,iy,	8,			"Attachment to save", true )
		--bu:set_target_lua( self.__b_t_tex_save )
		ui.bu_sel_tex_save = bu

	iy = iy + SY
	bu = self:add_text_info(	{ix,iy,		2,SY},		"save_info"	 )
		ui.bu_info_save = bu
		bu:set_text( "" )
	bu = self:add_trig_method(	{ix+2,iy,	2.5,SY},	"Save trig",	self, "trig_save"	):set_color_back("save")
		bu:set_fx_on_click( true )
	bu = self:add_button(		{ix+4.5,iy,	2.5,SY},	"Save",			self, "b_save_ui",	false	)
		bu:set_preset_use( false )

	self:end_bu_group()


--erase
	ix,iy = 9,1
	SY = 1
	local SYC = 1.6
	bu = self:add_camera(		{nil,nil,	nil,SYC}, 16 )
	iy = iy + SYC + DY
	local iy_post_cam = iy

	self:set_tab_key_def()

	self:begin_bu_group( "fbo" )
	
		bu = self:add_button(	{ix,iy,		8,SY},		"Clear"				)
			bu:set_multiple( { "No", "Clear", "Once by frame" },	"clear_sel", 2  )
			bu:set_target_lua( self, "__s_clear" )
			ui.bu_clear = bu
		iy = iy + SY

		
		local dx = 0
		
		local sxc = 8 - dx
		local clear = fbo:get_bdd_clear_screen()
		bu = self:add_param(	{ix,iy,		2,SY},		"Erase", 			clear.ref.color	):set_text( "Color" )	--erase was a bad name
			ui.bu_clear_color = bu
		bu = self:add_param(	{ix+2,iy,	4,SY},		"trail",			clear.ref.trail_time,	0,12, 	1	)
			ui.bu_trail = bu
		bu = self:add_param(	{ix+8-2,iy,	2,SY},		"erase_depth",		clear.ref.depth,   false	):set_text( "Depth" )
			ui.bu_clear_depth = bu
	--	self:add_param(			{ix+6,iy,	2,SY},		"erase_stencil",	clear.ref.stencil, false	):set_text( "Stencil" )
		iy = iy + SY + DY

		ix = ix + dx
		local SYC = 1.6
		if true then
			local text = { "Custom", "Black", "Grey 25", "Grey 50", "Grey 75", "White",	"Red", "Green", "Blue", "Cyan", "Yellow", "Magenta" }
			-- local text = {	"White",	"Red",
			-- 	"Grey 75",	"Green",
			-- 	"Grey 50",	"Blue",
			-- 	"Grey 25",	"Cyan",
			-- 	"Black",	"Yellow",
			-- 	"Custom",	"Magenta" }
			bu = self:add_button(	{ix,iy,		2,SY},		"Erase Color"		)
				bu:set_menu( text, "Erase Color" ):set_nb_min_0( 6, 2 )
				bu:set_text_selector(true)
			ui.sel_clear_color = bu
		else
			bu = self:add_selector(	{ix,iy,		sxc,SYC},	"Erase Color"		)
				bu:set_nb_min_0( 6, 2 )
				bu:set_item_text( 1, "Custom", "0", "25", "50", "75", "100",	"R", "G", "B", "Cy", "Ye", "Ma" )
		end
		bu:set_target_param( clear.ref.color_index )
			
		bu = self:add_param(	{ix+2,iy,		6,SY}, 		"Erase Custom_Alpha",	param.get_ref(clear.ref.obj,"color_alpha")	)
			:set_text("Alpha"):set_meter(true)
			ui.bu_alpha = bu
		iy = iy + SY
		self.ui.bu_rgb =
			self:add_rgbf(		{ix,iy,		sxc,1.5},	"Erase Custom",			clear.ref.obj, 		"color_"	):set_text("Custom")
		iy = iy + 1.5


-- Copy tab
		self:set_tab_key( "Copy" )

		ix,iy = 9,iy_post_cam

		bu = self:add_slider(		{ix,iy,		5,SY},	"Fbo Nb", 		self, "__fbo_nb_asked",	1,		1,16		)
			:set_value_type_integer(true):set_min_max_strict(true)
		
		iy = iy + SY + DY*3

		bu = self:add_selector(		{ix,iy,		8,SY},	"FlipFlop"		)
			bu:set_nb( 3 )
			bu:set_item_text( 1, "No", "Copy", "Swap" )
			bu:set_target( self, "__s_fbo_swap", 1 )
		iy = iy + SY + DY

		SX = 1.5
		bu = self:add_button(		{ix,iy,		SX,SY},	"Copy 1",		self, "__b_copy_1",			false	)
		self:add_bu_texture(		{ix,iy, 	8,6}, 	"Dst 1",		TEX_COPY_ID_BEGIN, SX	)
		iy = iy + 6 + DY

		bu = self:add_text_info(	{ix,iy,		8,SY},	"Draw Count"	)
			:set_text()
			:set_text_align_x( "left" )
			ui.bu_draw_count = bu
		bu = self:add_button(		{ix+5,iy,	3,SY},	"Do Restart", 	self, "b_do_restart",		false	)
		iy = iy + SY

		--self:set_tab_key()
		
		bu = self:add_text_info(	{ix,iy,		8,SY},	"Fbo id"		)
			:set_text()
			:set_text_align_x( "right" )
		ui.bu_fbo_id = bu

	self:end_bu_group()

	self:__add_debug_fbo( 9,4 )
end

function MEU_FBO:__build_ref_fbo()
	local fbo = oo.getsuper(MEU_FBO).__build_ref_fbo( self )
	if not fbo then
		self:print_error( "MEU Fbo require an actual fbo, this one have none." )
	end
	return fbo
end

function MEU_FBO:init()
	self.__attachment_nb_ui = 4
	self.__save_img_count = 0
	self.__save_snap_id = 0

	self.__icon_index = 3	-- so it start with Fbo Displayed
	self.__icon_phase = 0

	local fbo = self:get_fbo()
	--	need to do this to see it in monitor but can't just force it anywhere
	--fbo:set_attachment_depth_as_texture( true ) 
	local clear = fbo:get_bdd_clear_screen()
	self.__clear = clear
	local ref_clear = clear.ref
	--param.set( ref_clear.stencil, false )	-- we could force it for speed when we are sure we don't use it
	param.set( ref_clear.active, true )	-- we force it, if false the fbo will not do anything
	param.set( fbo.ref.texs[1].define_format, false )

end

-- todo do we have things to free ?
-- function MEU_FBO:free()
-- 	oo.getsuper(MEU_FBO).free( self )
-- end

function MEU_FBO:disable_clear()
	self.__s_clear = 1
end
function MEU_FBO:force_swap( bind )
	self.__s_fbo_swap = 3
	self:set_fbo_tex_copy( true, bind )
	--self.__b_copy_1 = true
end

function MEU_FBO:__assign_texture_from_name()
	local name = self:get_inst_key()
	local bind = self:get_bind_by_name( name )
	if bind then
		self:get_fbo():set_attachment_bind( 1, bind )
	else
		local str = "no bind for MEU_FBO instance"
		self:print_debug( str.." "..name )
		self:show_error( str, nil, 10 )
	end
end

function MEU_FBO:init_mu( mu )
	if not self:is_proto() then
		mu:set_drop_receiver( self )
		self:__assign_texture_from_name()
	end
end

function MEU_FBO:change_name( name )
	oo.getsuper(MEU_FBO).change_name( self, name ) 
	self:__assign_texture_from_name()
end

-- if we drop a mu fbo will set the size from it
-- this mainly a test concept
--to add something to set size from bind
function MEU_FBO:do_drop( mu )
	local dmeu = mu:get_meu_used()
	if not dmeu:is_proto() then
		local mtype = dmeu:get_meu_type_lower()
		self:print( "do_drop ----- "..mu.." of type "..mtype )
		if mtype == "fbo" then
			local sx,sy = dmeu:get_pixel_size()
			--self:print( sx.." "..sy )
			self:set_pixel_size( sx, sy )
		end
	end
end

function MEU_FBO:__make_dir( dirname )
	if not aaa.dir.is_exist( dirname ) then
		if aaa.dir.create( dirname ) then
			self:print( "Created Dir : "..dirname )
		else
			self:print_error( "Error creating dir for Snapshots : "..dirname )
			return false
		end
	end
	return true
end

-- SAVE
--
function MEU_FBO:set_save_dir( folder )
	self.__save_dir_asked = folder
end

function MEU_FBO:__prepare_save_names( b_trig )
	self.__save_dir = self.__save_dir_asked or aaa.dir.get_dir_snapshot()
	--self:box_debug( "__save_dir is\n"..self.__save_dir )
	if self:__make_dir( self.__save_dir ) then
		local ymd = aaa.time.get_str_ymd_underscore()
		local hms = aaa.time.get_str_hms()
		local base = self:get_inst_key().."_"..ymd.."_"..hms
		if b_trig then	-- call everytime we trig one
			self.__save_fname = base
			if hms~=self.__hms_last then
				self.__save_snap_id = 1
				self.__hms_last = hms
			end
			return true
		else -- call at start of save
			self.__save_dir = self.__save_dir.."/"..base
			if self:__make_dir( self.__save_dir ) then
				self.__save_fname = self:get_inst_key()
				self.__save_img_count = 0
				self.__save_snap_id = 1
				return true
			end
		end
	end
	return false
end

function MEU_FBO:is_verbose_save()
	return false
--	return self:get_inst_key()=="GBuffer"
end

--make sure we have one item to same
function MEU_FBO:__begin_img_save( b_trig )
	if not self.__save_filename then
		if not self:__prepare_save_names(b_trig) then
			self:box_dev( "in __begin_img_save() __prepare_save_names() failed\nNo __save_filename" )
			return
		end
	end

	if self:is_verbose_save() then
		self:print( "__begin_img_save( "..b_trig.." )" )
	end

	--we build the save table
	local t = {}
	local b_empty = true
	for i,v in IPAIRS(self.ui.bu_sel_tex_save:get_values()) do
		t[i] = v
		if v then b_empty = false end
	end
	if b_empty then t[1] = true end
	self.__save_by_tex = t
	self.__b_save_trig = b_trig

	if self:is_verbose_save() then
		table.print( t, "__save_by_tex" )
		self:print( "__begin_img_save( "..b_trig.." ) Done" )
	end
end

function MEU_FBO:__end_img_save()
	self.__save_by_tex = nil
	self.__save_filename = nil
	if self:is_verbose_save() then
		self:print( "__end_img_save()" )
	end
end
function MEU_FBO:trig_save()	self.__b_save_one_asked = true		end

function MEU_FBO:save_albedo( filename )
	self.save_named_fbo( filename, nil )
end

function MEU_FBO:save_named_fbo( filename, compression )
	self.__save_filename = filename
	self.__save_compression = compression
	self:trig_save()
end

function MEU_FBO:save_fbo_attachment( tex_id, filename, format )
	local ref = self.ref
	local b_depth = tex_id==(self.__attachment_nb_ui+1) 
	local bind = ref.fbo:get_attachment_bind( b_depth and "depth" or tex_id )
	local bind_name = self:get_name_from_bind( bind )
	local save_dir
	if not filename then
		filename = self.__save_filename
		save_dir = self.__save_dir
		filename = filename or string.format( "%s_%s_%05d", self.__save_fname, bind_name, self.__save_snap_id )
	else
	end
	self:print( "try to save fbo attachment "..tex_id.." as "..filename )
	if not format then
		-- try to find format in filename, default to "def"
		local fname = filename
		if string.find( filename, "%." ) then
			fname = string.gmatch( fname, "(.+)%.([^/]+)" )
			local name,ext = fname()
			if name then
				filename = name
				format = ext
			end
		end
	end

	if save_dir then
		filename = string.format( "%s/%s", save_dir, filename )
	end

	local old_compression = nil
	if self.__save_compression ~= nil then
		old_compression = param.get( aaa.ref.pref, "image_save_compression" )
		param.set( aaa.ref.pref, "image_save_compression", self.__save_compression )
	end

	local result = false
	local b_saved = format and aaa.img.save( bind, filename, format ) or aaa.img.save( bind, filename )
	if b_saved then
		self.__save_img_count = self.__save_img_count + 1
		result = true
	else
		self:print_error( "failed to save fbo tex "..tex_id.." as "..filename )
	end

	if self.__save_compression ~= nil then
		param.set( aaa.ref.pref, "image_save_compression", old_compression )
		self.__save_compression = nil
	end	

	return result
end

function MEU_FBO:save_fbo_interactive( format, folder_path )
	local bind = self.ref.fbo:get_attachment_bind(1)

	if bind then
		local fname
		if folder_path then
			if folder_path[#folder_path] == "/" then fname = folder_path .. "image"
			else						   			 fname = folder_path .. "/image"
			end
		else
			fname = aaa.file.do_dialog_save( "Save attachment 1 Image", "image" )
		end

		if fname then
			-- overwriting a file, need to remove extension
			if string.find( fname, "%." ) then
				fname = string.gmatch( fname, "(.+)%.(.+)" )
				local name,ext = fname()
				fname = name
			end
			self:print( "Saving to "..fname.."."..format )

			local compression = nil
			if     format == "jpg" then
				compression = .1
			elseif format == "png" then
				compression = 1.
			end

			-- saving
			self:save_named_fbo( fname.."."..format, compression )
			-- param.set( aaa.ref.pref, "image_save_compression", old_compression )
		else
			self:print_error( "No file to save to" )
		end
	else
		self:print_error( "No bind for attachment 1" )
	end
end


function MEU_FBO:get_save_image_count()
	return self.__save_img_count
end

function MEU_FBO:update_ui()
	local fbo = self.ref.fbo
	local ui = self.ui
	local bu_see = ui.bu_see
	if bu_see then
		local id = bu_see:get_value()
		--self:print( id )
		ui.bu_mon:set_texture_bind_2d( fbo:get_attachment_bind( id==5 and "depth" or id ) )
	else
		self:show_error( "No bu_see: is a Mac version" )
	end

-- info, debug field
	ui.bu_draw_count:set_text( self.__fbo_draw_count_frame )
 
	local id = self.__fbos_id_cur_offset
	if id then
		ui.bu_fbo_id:set_text( id .. "  -> " .. self:get_fbos_id_from_offset(0) )
	end

	--self:print( )
	local nb = self.__ite_by_update
	if nb and nb > 0 then
		ui.bu_ite_nb:set_text( "Iteration "..self.__ite_by_update )
		ui.bu_ite_nb:set_visible(true)
	else
		nb = self.__fbo_nb
		if nb and nb > 1 then
			ui.bu_ite_nb:set_text( "fbo nb "..nb )
			ui.bu_ite_nb:set_visible(true)
		else
			ui.bu_ite_nb:set_visible(false)
		end
	end

-- save counter
	if self.__save_img_count > 0 then
		ui.bu_info_save:set_text( self.__save_img_count )
	end

	local bu = ui.bu_fbo_active
	if bu then
		--todo generalize
		bu:set_text_color( fbo:is_valid() and "white" or "error" )
	end

	local b_clear = self.__s_clear > 1
	ui.bu_clear:set_color_back( b_clear and "bu_on" or "bu_off" )

	local a = ui.bu_clear_color:interpolate_alpha_bu( b_clear )
	ui.bu_clear_depth:set_alpha_bu( a )
	
	local b_trail = b_clear and (not ui.bu_clear_color:get_value_as_bool() ) 
	ui.bu_trail:interpolate_alpha_bu( b_trail )
	local b_color_used = b_clear or b_trail
	ui.sel_clear_color:interpolate_alpha_bu( b_color_used )

	b_trail = b_trail and ui.bu_trail:get_value()~=0.
	ui.bu_alpha:interpolate_alpha_bu( b_clear and (not b_trail) )

	local clear = fbo:get_bdd_clear_screen()
	ui.bu_rgb:interpolate_alpha_bu( b_color_used and param.get(clear.ref.color_index) == 0 ) 
end

function MEU_FBO:set_attachment_active( attach_id, b_active )
	self.ui.bu_sel_tex_active:set_item_selected_multiple( attach_id, true )
end

function MEU_FBO:__update_fbo1( fbo )
	local ui = self.ui

	--self:print( "fbo nb is "..self.__fbo_nb )
	
	if not self:is_restoring() then
		if GA.b_spy then aaa.spy.push_range( "__update_fbo1()", 9 ) end
	-- active
		local active = ui.bu_sel_tex_active:get_values()
		--table.print( 	, "t_active", 2 )
		for id_attach = 1,self.__attachment_nb_ui do
			fbo:set_attachment_active( id_attach, active[id_attach] )
		end

	-- SAVE BEGIN
		local save_by_tex = self.__save_by_tex	--this mean we want something saved
--		if save_by_tex then table.print( save_by_tex, "save_by_tex", 2 ) end
		if save_by_tex then
			if self:is_verbose_save() then table.print( save_by_tex, "saving now" ) end
			local b_ok = true
			for id_attach = 1,self.__attachment_nb_ui do
				if save_by_tex[id_attach] then
					--if fbo:is_attachment_tex_on_cpu(id_attach) then
						if not self:save_fbo_attachment(id_attach) then
							b_ok = false
						end
					--else
					--	self:print_error( id_attach.." - no texture on cpu to save" )
					--end
				end
			end
			-- Depth
			local i = self.__attachment_nb_ui + 1
			if save_by_tex[i] then
				if fbo:is_attachment_tex_on_cpu("depth") then
					if not self:save_fbo_attachment(i) then
						b_ok = false
					end
				else
					self:print_error( i.." - no depth texture on cpu to save" )
				end
			end
			if b_ok then self.__save_snap_id = self.__save_snap_id+1 end
			if self.__b_save_trig then self:__end_img_save() end
		end
	-- SAVE END

	-- set save: we will save the next frame when on_cpu flags ar the right state
	--	and so texture avalaible
		if self.__b_save_ui_last ~= self.b_save_ui then
			self.__b_save_ui_last = self.b_save_ui
			if self.b_save_ui		then	self:__begin_img_save( false )
			else							self:__end_img_save()
			end
		end
		if self.__b_save_one_asked then
			self.__b_save_one_asked = false
			if self:is_verbose_save()	then	self:print( "save one asked" ) end
			if self.__save_by_tex 		then	self:print_error( "Already saving sequence" )
			else								self:__begin_img_save( true )
			end
		end

	-- assign tex on cpu, done after save because we want to start at the next frame
		
		save_by_tex = self.__save_by_tex
		local on_cpu_by_tex = ui.bu_sel_tex_cpu:get_values()
		self.b_on_cpu_by_tex = on_cpu_by_tex
		--table.print( on_cpu_by_tex )
		local attach_nb = self.__attachment_nb_ui
		for id_attach = 1,attach_nb+1 do
			--self:print( id_attach.." - "..on_cpu_by_tex[id_attach].." "  )
			--self:print( id_attach.." - "..( save_by_tex~=nil and save_by_tex[id_attach] or false ) )
			local b = on_cpu_by_tex[id_attach] or ( save_by_tex~=nil and save_by_tex[id_attach] or false )
			on_cpu_by_tex[id_attach] = b
			--self:print( id_attach.." - "..b )
			--todo refine and extend this
			--todo transfer at each iteraion is really bad in terms of speed
			local fbo_nb = self.__fbo_nb
			local att_id = id_attach==attach_nb+1 and "depth" or id_attach
			if fbo_nb and fbo_nb > 1 then
				--self:print( "hack with multiple fbo" )
				local fbos = self.__fbos
				for fbo_i = 1,fbo_nb do
					fbos[fbo_i]:set_attachment_tex_on_cpu( att_id, false )
				end
			else
				if b then
					fbo:set_attachment_tex_on_cpu( att_id, true, true )
				else
					fbo:set_attachment_tex_on_cpu( att_id, false )
				end
			end
			if self:is_verbose_save() and save_by_tex then
				self:print( att_id.." On Cpu "..on_cpu_by_tex[id_attach].." "..b.." -> "..fbo:is_attachment_tex_on_cpu(att_id) )
			end
		end

		if GA.b_spy then aaa.spy.pop_range() end
	end

-- CLEAR
	--todo should we clear when flipflop ? pronably in some case and only for the first fbo used
	--	local s_clear = self:get_bu_value( "clear" )
	local s_clear = self.__s_clear
	--self:print( "s_clear is "..s_clear )
	if		s_clear == 1 then	self.b_clear = false	-- no
	elseif	s_clear == 2 then	self.b_clear = true		-- clear
	else	-- once by frame
		local index = GP.cur:get_render_index()
		local b_clear = self.__fbo_render_index ~= index
		if b_clear then
			self.__fbo_render_index = index
		end
		self.b_clear = b_clear
	end

end

function MEU_FBO:delete_fbos()
	local fbos = self.__fbos
	for id_fbo = self.__fbo_nb,2, -1 do
		local fbo = fbos[id_fbo]
		self:delete_obj( fbo:get_obj() )
		fbos[id_fbo] = nil
	end
end

--todofbo store free bind and reuse at the instance or the NEU_FBO level
--todofbo solve the cration problem
function MEU_FBO:get_free_bind()
	local bind = IMGS.get_bind_free()
	IMGS.inc_bind_free()
	return bind
end

-- when MEU_FBO swap, cycle flip through fbos, this fns create the c_fbo use from copying the original c_fbo
function MEU_FBO:__build_fbos()
--	aaa.debug.print_traceback( "__build_fbo" )
--	table.print( self.__fbo_free, "fbo_free" )
	
	local fbo_nb = self.__fbo_nb_asked
	--aaa.print_fn()

	local fbos = self:get_table_always( "__fbos" )
	--todo remove the old one when we change nb, careful we reuse existing fbo if we can 

--first fbo is the original
	local fbo_src = fbos[1] or self:get_fbo()
	fbos[1] = fbo_src

	local obj_src = fbo_src:get_obj()
	
	--store bind from first fbo (the one in the ui)
	local bind_attach_1 = {}
	for attach_id = 1,8 do
		if fbo_src:get_attachment_active( attach_id ) then
			bind_attach_1[attach_id] = fbo_src:get_attachment_bind( attach_id ) 
		end
	end

	for fbo_id = 1,fbo_nb do
		--self:print( "build_fbo "..fbo_id  )
	--second (and third) are copy of the original but modified, and we reuse existing fbo and FBO
		local fbo = fbos[fbo_id]
		if fbo_id ~= 1 then
			if not fbo then
				--create from scratch
				local o = self:clone_obj( obj_src )
				fbo = FBO:create( fbo_src:get_name().."_"..fbo_id, o )
				fbos[fbo_id] = fbo
			else
				--just copy the params
				--self:print( "copy fbo" )
				aaa.obj.set_param_from( fbo:get_obj(), obj_src )
			end
		end

		local patch = self.__patch_attach
		if s_verbose > 0 then 
			table.print( patch, "patch for fbo" )
		end
		if patch then
			local ask = patch.attach_ask
			if ask then
				if s_verbose > 0 then
					table.print( ask, "ask", 2  )
				end
				for attach_id = 2,8 do	--todo deal with index 1
					local t = ask[attach_id]
					if t then
						if fbo_id==1 then	-- we doit for the one the next start from a copy
							if s_verbose > 0 then
								table.print( t, "t["..attach_id.."]", 2  )
							end
							if t.ch_nb or t.ch_typw  then
								fbo:set_attachment_channel_type( attach_id, t.ch_nb, t.ch_type, true )
								self:set_attachment_active( attach_id, true )
								-- we don't update the fbo so we set tghis manually
								fbo:set_attachment_active( attach_id, true )
							end
						end
						if t.b_by_fbo then
							--local binds = t.binds
							local bind = self.__fbo_free_bind[fbo_id]
							if s_verbose > 0 then
								self:print( "bind is "..bind )
							end
							fbo:set_attachment_active( attach_id, true )	-- this shopuld already be doine by the copy
							fbo:set_attachment_bind( attach_id, bind )
						end
					end
				end
			end
			--todofbo
			--local bind = self:get_fbo_tex_copy( fbo_id-1 )
			--self:print( "bind is "..bind )
			--fbo:set_attachment_bind( 1, bind )
		else
			if fbo_id ~= 1 then
				for attach_id = 1,8 do
					local bind = bind_attach_1[attach_id]
					if bind then
						bind = bind + fbo_id-1
						if s_verbose > 0 then
							self:print( attach_id.." bind is "..bind )
						end
						fbo:set_attachment_bind( attach_id, bind )
					end
				end
			end
		end

		fbo:update()
		self:print( fbo_id.."- bind is "..fbo:get_attachment_bind(1) )
		--table.print( fbo:__get_tex_table( 1 ), "tex_table", 2 )
	end

	self.__fbos_id_cur_offset = 0
	self.__fbo_nb = fbo_nb
	--fbos[2]:set_focus()	//only for debug 
	self.__b_fbos_build_needed = false

	if s_verbose > 0 then
		table.print( fbos, fbo_nb.." fbos", 2 )
	end
	return fbos
end

--UPDATE DRAW
function MEU_FBO:update()

	local fbo = self.ref.fbo
	--aaa.print_fn()
	self:__update_fbo1( fbo )
	fbo:update()

	if fbo:is_attach() then
		self.__b_fbos_build_needed = true
	end
	if self.__fbo_nb ~= self.__fbo_nb_asked then
		self.__b_fbos_build_needed = true
	end
	if self.__b_fbos_build_needed then
		self:__build_fbos()
	end

	--self:print( "MEU_FBO:update() "..self.b_do_restart )
	if self.b_do_restart or not self.__fbo_draw_count_frame then
		--self:print( "b_do_restart" )
		self.__fbo_draw_count_frame = 0
	end
--	self:print( "FBO UPDATE ite "..self.__ite_by_update.." reset to 0" )
	self.__ite_by_update = 0

--	if app.verbose > 0 then self:print( "FBO Update()" ) end
	--todo deal with register_meu_last_rendered_by_type
--	MEU.draw_registered( self )
	--todo encapsulate better	-- try to mark MEU so background will not be red
	--todo do better
	--self:set_render_index()
	self.__icon_phase = self.__icon_phase + aaa.time.dt * 4.
	self:show( self.__icon_phase ) 
	self.__icon_index = triangle( aaa.time.t*8-1,0,4 )

	--todo refine this quick hack
	MEU.__meu_fbo_cur = self
end

--todo extand with table
function MEU_FBO:set_do_restart_counter_on_update( b_do_restart )
	self.b_do_restart = b_do_restart
end
function MEU_FBO:__set_flipflop_texture( attach_id, t )
	local ask = self:get_table_always( "__patch_attach", "attach_ask" )
	ask[attach_id] = t
end
function MEU_FBO:__set_flipflop_fbo( fbo_nb, tex_unit_index_dst, fbo_attachment_id_src, attachment_table )
	self.__fbo_nb_asked = fbo_nb
	if not self.__fbo_free_bind then
		local binds = {}
		for fbo_id = 1,fbo_nb do
			binds[fbo_id] = MEU_FBO:get_free_bind()
		end
		self.__fbo_free_bind = binds
	end
	if tex_unit_index_dst and fbo_attachment_id_src then 
		self.__patch_attach = {	tex_unit_index_dst     = tex_unit_index_dst,
							fbo_attachment_id_src = fbo_attachment_id_src	}
		if attachment_table then
			self:set_do_restart_counter_on_update( true )
			self:__set_flipflop_texture( fbo_attachment_id_src, attachment_table )
			self:disable_clear()
			self:force_swap( self.__fbo_free_bind[1] ) --todofbo this is dangerous
			self:__build_fbos()
		end
	end
end


function MEU_FBO:get_fbo_tex_copy( id )
	-- ui give is the first
	local bind = self:get_texture_bind_2d( TEX_COPY_ID_BEGIN )
	if id == 1 then
		return bind
	elseif id <= 15 then
		return bind + id - 1 --  
	else
		self:print_error( "get_fbo_tex_copy( "..id.." ) Deal only with id <= 15 for now (2024 Feb)" )
	end
end

function MEU_FBO:set_fbo_tex_copy( b_active, bind_dst )
	self.__b_copy_1 = b_active
	self:set_texture_bind_2d( TEX_COPY_ID_BEGIN, bind_dst )
end

--todo get make sure offset positive and negative are coherent and coherent with the calls
function MEU_FBO:get_fbos_id_from_offset( offset )
	local id = self.__fbos_id_cur_offset
	if offset then
		id = id + offset
	end
	return ((id-1) % self.__fbo_nb) + 1
end
function MEU_FBO:__get_fbo_from_offset( offset_id )
	--self.__fbos = nil
	local fbos = self.__fbos
	if not fbos then
		aaa.debug.print_traceback( "No fbos" )
		--fbos = self:__build_fbos()
		return
	end
	local id = self:get_fbos_id_from_offset( offset_id )
	--table.print( fbos )
	local fbo = fbos[ id ]
	--fbo:print( id .." returned "..fbo:get_attachment_bind( 1 ) )
	return fbo
end

function MEU_FBO:__copy_attachment_to_tex( id, bind_dst, b_force_mipmap, b_verbose )
	local fbo = self:__get_fbo_from_offset( 0 )
	local bind_src,b_mipmap = fbo:get_attachment_bind(id), b_force_mipmap or fbo:is_attachment_mipmap_generate(id)
	if b_verbose then
		self:print( "copy_tex "..bind_src.." to "..bind_dst.." mipmap "..b_mipmap )
	end
	--aaa.img.copy_tex_to_tex( bind_src, bind_dst, false )	--todo deal with mipmap
	--aaa.img.copy_tex_to_tex( bind_src, bind_dst )	--todo deal with mipmap
	local sx = aaa.img.get_size( bind_src )
	if sx then
		aaa.img.copy_tex_to_tex( bind_src, bind_dst, b_mipmap )
		return bind_src
	end
end

function MEU_FBO:__get_bind_prev( id )
	if self.__s_fbo_swap > 1 then
		if self.__b_copy_1 then
			--aaa.print_fn()
			if self.__s_fbo_swap == 2 then	-- flipflop by copy
				--self:print(2)
				return self:get_fbo_tex_copy( id )
			elseif self.__s_fbo_swap == 3 then	-- flipflop by swap
				local fbo = self:__get_fbo_from_offset( -1 )
				return fbo:get_attachment_bind( id )
			end
		end
	end
end


function MEU_FBO:__map_prev_output( b_verbose )
	
	-- local fbo = self:__get_fbo_from_offset( 0 )
	-- if b_verbose then
	-- 	self:print( self.__fbos_id_cur_offset.."__map_prev_output() "..self.__fbo_draw_count_frame.." "..fbo )
	-- end
	-- fbo:draw()

	if true then
--		local ff = self.path
--		if ff then
--			ff = ff.patch
			local patch = self.__patch_attach
			if patch then
				if b_verbose then
					self:print( "__get_bind_prev "..patch.fbo_attachment_id_src )
				end
				local bind = self:__get_bind_prev( patch.fbo_attachment_id_src )
				if bind then
					if b_verbose then
						self:print( "prev bind attachment "..bind )
					end
					gol.set_tex_unit_2d_bind( patch.tex_unit_index_dst, bind )
				end
			end
--		end
	end
end

function MEU_FBO:__bind_fbo( b_switch )
	
	-- this the one controlled directly by the ui of the MEU
	local fbo_ui = self:get_fbo()
	-- now we need to find which one to use
	local fbo
	if b_switch then
		local fbo_id = self.__fbos_id_cur_offset
		--todo 2023 Sept was done to avoid error stop but the solution was to make the fbo updated 
		if not fbo_id then
		-- 	-- todo why we have to do that here and not in an init
		-- 	-- todo is 1 the value to pass
			aaa.debug.print_traceback( "No fbos" )
			--self:__build_fbos()
			--fbo_id = self.__fbos_id_cur_offset 
			return
		end
		self.__fbos_id_cur_offset = fbo_id + 1
		fbo = self:__get_fbo_from_offset( 0 )
		--aaa.print_fn()
		if not fbo then
			self:print_error( "no fbo "..self:get_fbos_id_from_offset(0) )
			return
		end
	else
		fbo = fbo_ui
	end
	fbo:draw()
	--MEU.draw_registered( self )
	if self.b_clear then
	--that the first/ui/control by the BU fbo which fix the color
		local bdd_clear = fbo_ui:get_bdd_clear_screen()
		bdd_clear:update_then_draw()
	end
	return fbo
end

function MEU_FBO:__do_fbo_before( b_verbose, b_last )
	-- if self.__s_fbo_swap == 1 then
	-- 	--self:print_error( "do_fbo_before() called when no copy or fliflop" )
	-- 	return
	-- end

	self.__fbo_draw_count_frame = self.__fbo_draw_count_frame + 1
	self.__ite_by_update = self.__ite_by_update + 1
	--self:print( "MEU_FBO:__do_fbo_before() ite "..self.__ite_by_update )

--	aaa.debug.print_traceback()
	--self:print( "inc fbo_draw_count_frame to "..self.__fbo_draw_count_frame )

	local fbo_nb = self.__fbo_nb
	if b_last then
		if fbo_nb > 1 then
			local on_cpu_by_tex = self.b_on_cpu_by_tex
			local fbo = self:__get_fbo_from_offset( 1 )
			self.__fbo_to_use_after = fbo
			local attach_nb = self.__attachment_nb_ui
			for id_attach = 1,attach_nb do
				if on_cpu_by_tex[id_attach] then
					local att_id = id_attach==attach_nb and "depth" or id_attach
					--self:print( "attachment "..att_id.." on cpu asked" )
					fbo:set_attachment_tex_on_cpu( att_id, true, true )
				end
			end
		end
	else
		local fbo = self.__fbo_to_use_after
		if fbo then
			for id_attach=1,self.__attachment_nb_ui do		
				fbo:set_attachment_tex_on_cpu( id_attach, false )
			end
			fbo:set_attachment_tex_on_cpu( "depth", false )
			self.__fbo_to_use_after = nil
		end
	end

	if self.__s_fbo_swap == 3 and fbo_nb > 1 then
		self:__bind_fbo( true )
	end

	if self.__b_copy_1 then
		if self.__s_fbo_swap > 1 then	-- flipflop done with copy or swap
			--no real operation this fast so no spying
			--if GA.b_spy then aaa.spy.push_range( "FBO-"..self.." fbo flipflop", 7 ) end
				self:__map_prev_output( b_verbose )
			--if GA.b_spy then aaa.spy.pop_range() end
		end
	end

end

function MEU_FBO:__do_fbo_after( b_verbose, b_last )
	if self.__b_copy_1 then
		if self.__s_fbo_swap == 2  then	-- flipflop done with copy
			if GA.b_spy then aaa.spy.push_range( "FBO-"..self.." copy tex()", 7 ) end
				self:__copy_attachment_to_tex( 1, self:get_fbo_tex_copy(1), false, b_verbose )
			if GA.b_spy then aaa.spy.pop_range() end
		end
	end

	-- if b_last then
	-- 	local fbo = self.__fbo_to_use_after
	-- 	if fbo then
	-- 		for id_attach=1,self.__attachment_nb_ui do		
	-- 			fbo:set_attachment_tex_on_cpu( id_attach, false )
	-- 		end
	-- 		fbo:set_attachment_tex_on_cpu( "depth", false )
	-- 		self.__fbo_to_use_after = nil
	-- 	end
	-- end
end

function MEU_FBO:draw_quad_fbo_pass( s, b_verbose, b_last, b_trick, b_render )
	self:__do_fbo_before( b_verbose, b_last )

	if b_trick then
		gol.set_draw_attachment( b_render, not b_render )
	end	

	local o = 0
	aaa.draw_rect_uv( o-s,o-s, o+s,o+s ) -- draw with shader
	
	if b_trick then
		gol.set_draw_attachment( true, true )
	end

	self:__do_fbo_after( b_verbose, b_last )
end


function MEU_FBO:__draw_fbo_low( b_camera, iteration_nb )
	--todo
	
	--	if app.verbose > 0 then self:print( "FBO Draw()" ) end
	
	--gol.set_default()	--todo refine, here to stop shader
	--if GA.b_spy then aaa.spy.push_range( "FBO-"..self.." draw()", 8 ) end

	if iteration_nb then
		if self.__s_fbo_swap == 2 then
			self:__bind_fbo( false )
		end
	else
		self:__bind_fbo( self.__s_fbo_swap == 3 and self.__fbo_nb > 1 )
	end

	if b_camera then
		local cam = self:get_camera_sel()
		aaa.obj.update_then_draw(cam)
	end
	
	self:__register_to_mus()
	
	--if GA.b_spy then aaa.spy.pop_range() end
end



function MEU_FBO:draw()
	self:__draw_fbo_low( true )
	-- experiment
	-- self:print( self:get_inst_key() )
	-- if self:get_inst_key() == "F1" then
	-- 	self:__copy_attachment_to_tex( 1, nil, true, true )
	-- end
end

function MEU_FBO:do_fbo( iteration_nb )
	--self:print( "MEU_FBO:do_fbo()" )
	self:update()
	self:__draw_fbo_low( false, iteration_nb )
end
-- function MEU_FBO:do_fbo_multi()
-- 	--self:print( "MEU_FBO:do_fbo()" )
-- 	self:update()
-- 	self:__draw_fbo_low( false )
-- end


function MEU_FBO:draw_icon()
	local fbo = self.ref.fbo
	if not self:is_proto() then
		local icon_index = self.__icon_index
		-- if self:is_rendered_in_this_frame() then
		-- else
		-- 	self.__icon_index = 3
		-- end
		--local s = 1
		--gol.scale( s )
		--local s = 1.5 + .5 * math.sin(aaa.time.t_real*6)
		gol.disable_blend()
			local lbrt = self:draw_icon_texture( self.ref.fbo:get_attachment_bind(1), 1 )
		gol.enable_blend()
		--gol.scale( 1/s )
		--gol.set_texture_dim(0)

		--gol.set_line_width( BU.__draw_text_line_width )
		--local _,y = math.modf( aaa.time.t )
		--aaa.show( y )
		--y = y - .5
		local ph
		if self:is_rendered_in_this_frame() then
			ph = self.__icon_phase
			--ph = aaa.time.t * 4.
			ph = triangle_01( ph )
			ph = -2 + ph * 3
		else
			ph = 1
		end
		gol.color_yellow( ph )
		--local y = lbrt[2] + (lbrt[4] - lbrt[2]) * ph
		--local y = -.5 + ph
		--aaa.draw_line( lbrt[1],y, lbrt[3],y )
		gol.set_texture_dim(0)
		local x = (lbrt[1] + lbrt[3]) / 2
		local y = (lbrt[2] + lbrt[4]) / 2
		local s = 1.5
		local sx,sy = .03*s,.15*s
		aaa.draw_rect( x-sx,y-sy, x+sx,y+sy )

		gol.color_yellow()
		gol.set_line_width(1)
		aaa.draw_str_xy(  string.sub( "Fbo", 1,self.__icon_index ), -.4,-.6,	.1,.4 )
	end
end
