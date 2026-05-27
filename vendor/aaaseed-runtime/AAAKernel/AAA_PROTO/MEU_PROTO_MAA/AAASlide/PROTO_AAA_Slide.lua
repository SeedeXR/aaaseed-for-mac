function meu:define_meu_infos()
	return { author = "Mâa", date = "2025",
			   tags = { "core", "Tutorial", "Texture", "unfinished" },	--todo need more comments in the code
			   help = "basic example of how to load images from folder"
			 }
end

function meu:define_ui()
	local ref   = self.ref
	local ui    = self.ui
	local ix,iy = 1,1
	local SY,DY = 1,.2
	local bu

	self:add_camera()

	
	-- self:add_selector(			{ix,iy,			8,SY},		"Transition")
	-- 	:set_item_text( 2, "Fade", "Y Full", "fake Rot", "Y"  )
	-- 	:set_target_lua( self, "s_fx" )
	bu = self:add_button(	{ix,iy,	2,SY}, "What" )
 		bu:set_menu{ 	 "Direct", "Fade", "",
						"X 2", "Hori", "X",
						"Y 2", "Vert", "Y"
					}:set_nb_min_0( 3, 3 )
		bu:set_text_selector(true)
			:set_target_lua( self, "s_fx" )
		
	self:add_slider(			{ix+2,iy,			2,SY},		"Delay",	self,"delay",  .2, 0,1	)				
	iy = iy + 1 + DY

	local SXS = 8/5
	bu = self:add_trig_method(  {ix,		iy,		SXS,SY},	"First",	self,   "set_slide",  1 )
	bu = self:add_trig_method(  {ix+SXS,	iy,		SXS,SY},	"Previous",	self,   "inc_slide", -1 )
		:set_text("<-")
	bu = self:add_text_info(  	{ix+SXS*2,	iy,		SXS,SY},	"Slide Id"	):set_text_align_x( "center" )
		ui.info_slide_id = bu
	bu = self:add_trig_method(  {ix+SXS*3,	iy,		SXS,SY},	"Next",		self,   "inc_slide",  1 )
		:set_text("->")
	bu = self:add_trig_method(  {ix+SXS*4,	iy,		SXS,SY}, 	"Last",		self,   "set_slide", -1 )

	bu = self:add_text_info(  	{ix+4,		iy-SY,	4,SY},		"Slide Nb"	)
		ui.info_slide_nb = bu
	iy = iy + SY + DY
	
	local SXM = 8
	local SYM = SXM * 9/16
	bu = self:add_monitor(		{ix,iy,				SXM,SYM},	"Slide prev" )
		ui.bu_monitor_prev = bu
	bu = self:add_monitor(		{ix+8,iy,			SXM,SYM},	"slide next" )
		ui.bu_monitor_next = bu
	iy = iy + SYM
	bu = self:add_monitor(		{ix+4,iy,			SXM,SYM},	"slide current" )
		ui.bu_monitor = bu
		iy = iy + SYM / 2

	iy = iy + SYM / 2 + DY

	bu = self:add_trig_method(	{ix,iy, 		4,SY}, 		"folder", 	self, "get_folder" )
	iy = iy + SY
	bu = self:add_text(			{ix,iy, 		16,SY}, 	"folder path" )
		--bu:set_target_lua( self, "fpath" ) --todo function don't exist yet 
		ui.bu_folder_path = bu
	bu = self:add_trig_method(  {1,15,   		7,SY},  	"Load All Slides",		self,   "load_slides" )
		:set_color_back("load")
end


function meu:get_folder()
	local str = aaa.file.do_dialog_folder( "Slide folder"  )
	self:print( "Folder is \""..str.."\"" )
	if str then
		local ui = self.ui
		ui.bu_folder_path:set_text( str )
	end
end

function meu:init_after()
	self:init_slides()
end

function meu:init_slides()
	local slides = {}
	local folder_path = aaa.dir.get_dir_start().."/AAADoc/Slides"
	MEDIA.set_dir( folder_path )

	for i=1,999 do
		local fname = string.format( "AAASeed_Introduction - %03u.jpg", i  )
		--self:print( fname )

 		local bind = IMGS.assign_bind( fname )
 		if bind then
 			self:add_image_to_table( slides, fname, bind )
		else
			break
 		end
	end
	self.slides = slides
	self.slide_nb = #slides
	self.slider_folder = folder_path

	self.slide_id = 1
	self:inc_slide( 0 )
end


function meu:load_slide( id, b_force )
	local slide = self.slides[id]
	if slide then
		if not slide.b_load or b_force then
			local fname = self.slider_folder.."/"..slide.fname
			self:print( "asynchronously reading "..fname )
			aaa.img.read_no_error( slide.bind, fname, false, true )
			slide.b_load = true
		end
	end
end
function meu:load_slides()
	local nb = self.slide_nb
	if nb == 0 then
		self:init_slides()
	end
	for i=1,self.slide_nb do
		self:load_slide( i, true )
	end
end
function meu:set_slide( id )
	if id then
		if id < 0 then
			id = self.slide_nb + id + 1
		end
		self:load_slide( id )
		self:load_slide( id+1 )
		self:load_slide( id-1 )
		self.slide_id = id
	end
end
function meu:inc_slide( inc )
	local nb = self.slide_nb
	if nb > 0 then
		self:set_slide( wrap_int( self.slide_id + inc, 1, self.slide_nb ) )
	end
end


function meu:add_image_to_table( tab, file_name, bind )
	self:print( "added to table image " .. file_name .. " with bind: " .. bind )
	table.insert( tab, { fname = file_name,	bind = bind } )
end

function meu:update_ui()
	local ui = self.ui
	local nb = self.slide_nb
	if nb > 0 then
		local slide_id = self.slide_id
		local slides = self.slides
		local bind = slides[slide_id].bind
		ui.bu_monitor_prev:set_texture_bind_2d( slides[wrap_int( slide_id-1, 1,nb )].bind )
		ui.bu_monitor     :set_texture_bind_2d( slides[slide_id                    ].bind )
		ui.bu_monitor_next:set_texture_bind_2d( slides[wrap_int( slide_id+1, 1,nb )].bind )

		ui.info_slide_id:set_text( slide_id )
		ui.info_slide_nb:set_text( nb.." slides" )
	else
		ui.info_slide_id:set_text( "" )
		ui.info_slide_nb:set_text( "No Slide" )
	end
end

function meu:update()
	local slide_id = self.slide_id
	if self.slide_nb > 0 then
		local layer = self:get_layer( 1 )
		local bind = self.slides[ self.slide_id ].bind
		aaa.layer.set_bind_2d( layer, bind )
	end
end

function meu:draw_slides( sx,sy, ph, alpha )
	local nb = self.slide_nb
	local i_beg = math.floor(1+ph)
	local slide = self.slides[i_beg]
	if slide then
		local bind = slide.bind
		local rest = math.fmod( ph, 1. )
		gol.color_white( alpha )
		
		if self.s_fx <= 2 then
		--FADE
			gol.bind_texture( bind )
			aaa.draw_rect_uv( -sx*.5, -sy*.5, sx*.5, sy*.5 )
			if rest > 0. then
				i_beg = wrap_int( i_beg+1, 1,nb )
				gol.color_white( rest * alpha )
				bind = self.slides[i_beg].bind
				gol.bind_texture(bind)
				aaa.draw_rect_uv( -sx*.5, -sy*.5, sx*.5, sy*.5 )
			end

		elseif self.s_fx <= 5 then
		--X MOVE
			local x = -rest * sx
			--self:print( ph )
			gol.bind_texture( bind )
			local xb = -sx*.5
			if self.s_fx == 3 then xb = xb + x end
			if self.s_fx == 5 then gol.set_quad_uv( rest,0, 1,1 ) end
			aaa.draw_rect_uv( xb, -sy*.5, x+sx*.5, sy*.5 )	
			
			if rest > 0. then
				x = x + sx
				i_beg = wrap_int( i_beg+1, 1,nb )
				bind = self.slides[i_beg].bind
				gol.bind_texture(bind)
				xb = sx*.5
				if self.s_fx == 3 then xb = xb + x end
				if self.s_fx == 5 then gol.set_quad_uv( 0,0, rest,1 ) end
				aaa.draw_rect_uv(  x-sx*.5, -sy*.5, xb, sy*.5 )		
			end
		elseif self.s_fx <= 8 then
		--Y MOVE
			local y = rest * sy
			--self:print( ph )
			gol.bind_texture( bind )
			local yb = sy*.5
			if self.s_fx == 6 then yb = yb + y end
			if self.s_fx == 8 then gol.set_quad_uv( 0,0, 1,1-rest ) end
			aaa.draw_rect_uv( -sx*.5, y-sy*.5, sx*.5, yb )	
			
			if rest > 0. then
				y = y - sy
				i_beg = wrap_int( i_beg+1, 1,nb )
				bind = self.slides[i_beg].bind
				gol.bind_texture(bind)
				yb = -sy*.5
				if self.s_fx == 6 then yb = yb + y end
				if self.s_fx == 8 then gol.set_quad_uv( 0,1-rest, 1,1 ) end
				aaa.draw_rect_uv( -sx*.5, yb, sx*.5, y+sy*.5 )		
			end
		end
	else
		self:print_error( "ph is "..ph.." i_beg is "..i_beg )
	end

	-- gol.color_white()
	-- gol.set_texture_dim(0)
	-- aaa.draw_rect_line( -sx*.5, -sy*.5, sx*.5, sy*.5 )
end

function meu:draw()
	self:draw_layers_begin()

		self:draw_layer(1)
		local ph = self.ph or 0
		local dir = self.slide_id - (ph+1)
	
		--self:print( "dir is "..dir )
		if self.s_fx > 0 then
			if dir ~= 0 then
				local sign = dir < 0 and -1 or 1 
				ph = ph + aaa.time.dt * math.max( 1, math.abs(dir) ) * sign / self.delay
				ph = math.fmod( ph, self.slide_nb )
				if dir > 0 then
					if ph >= self.slide_id-1 then
						ph = self.slide_id - 1
					end
				else
					if ph < self.slide_id-1 then
						ph = self.slide_id - 1
					end
				end
			end
		else
			ph = self.slide_id - 1
		end
		self.ph = ph
		local sx = 8
		local sy = sx * 9/16
		self:draw_slides( sx,sy, ph, self:get_alpha() )

	self:draw_layers_end()
end


