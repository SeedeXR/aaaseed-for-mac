function meu:define_meu_infos( )
	return { 	author = "Mâa",
				version = 0,
				tags = { "2d", "3d", "Vj", "coregraphic", "imageprocessing", "text", "unfinished" },
				help =	{	"Basic Meu to display text and load font (double click on a selector item).",
							"Font can be otf, ttf or Json (AAASeed use of SDF font)."
						}
			}
end

function meu:draw_icon()	self:draw_icon_text( "Abc", .76 )	end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local SY = 1
	local DY = .2
	local ix,iy = 1,1
	
	self:add_rendering()

	local SYS = 5
	bu = self:add_selector( {ix,iy,	8,SYS}, "Font" )
		bu:set_nb_min_0( 8, 8 )
		bu:set_item_text_from_nb()
		bu:set_target_param( ref.font )
		bu:set_method_on_click_double( self, "load_font" )
		--todo try to optimize later using this
		--bu:set_method_on_value_change( self, "change_font" )
		ui.bu_font_selector = bu
	iy = iy + SYS + DY

	bu = self:add_text_info( {ix,iy,	16,SY},	"Font Name" )
		ui.font_name = bu
	iy = iy + SY + DY

	bu = self:add_text( {1,iy, 16,SY}, "text" )
		bu:set_text( param.get( self.ref.text ) )
		--bu:set_text_nice()
	--todo this don't function
	--bu:set_target_param( ref.text )
	ui.bu_text = bu 
	iy = iy + SY + DY

	local iy_mon = iy

	self:add_camera()
	ix = 9
	iy = 2 + DY
	self:add_transfo( {ix,iy, 8,2.4}, 1 )
	iy = iy + 2.4 + DY
	self:add_rgbf( {ix,iy, 8,SY} )

	self:set_tab_key( "TEK")
	ix,iy = 9,12
	self:add_trig_fn(	{ix,iy, 3,SY}, "Focus", aaa.obj.set_focus_ui, ref.bdd )
	iy = iy + SY + DY
	self:add_slider( {ix,iy, 8,SY}, "Sdf Factor", ref.sha,"fu_float_01",	16,	1,256	 )

	self:add_button(	{13,9, 3,SY*2}, "VeryBiggerStuff\nYes" )
	self:add_button(	{13,11, 3,SY}, "AB" )
	self:add_trig_method(	{13,12, 3,SY}, "A", self, "test" )
end

function meu:test()
	aaa.mess.show( "Ta Mere" )
end
function meu:init()
	local ref = self.ref

	ref.bdd		=	self:get_layer_bdd( 1 )
	ref.text	=	param.get_ref( ref.bdd, "text" )
	ref.font	=	param.get_ref( ref.bdd, "font" )
	ref.font_name = param.get_ref( ref.bdd, "font_name" )

	local app = aaa.obj.get_from_top_by_class( "app" )
	local param_font_bind = param.get_ref( app, "font->" )
	ref.font_bind = param.get_obj_attached( param_font_bind )

	ref.sha = self:get_shading_ref_from_layer( 1 )

end

function meu:set_text( str )
	param.set( self.ref.text, str )
end

function meu:load_font( id )
	local ref = self.ref
	local ui = self.ui
	local bind = ui.bu_font_selector:get_value()
	local str = "load font at index "..bind
	local filter = "Reconnus\0*.ttf;*.otf;*.json\0Ttf\0*.ttf\0Otf\0*.otf\0Sdf\0*.json\0Tous\0*.*\0\0"
	local filenames = aaa.file.do_dialog_open( str, filter, true )
	table.print( filenames, "filenames" )
	for i,fname in IPAIRS(filenames) do
		self:print( i.." load "..fname.." at "..bind )
		-- 2025 October added by Mâa because the bind method do not function
		aaa.font.read( bind, fname )
		--param.set( ref.font_bind, "bind_"..aaa.format.int_to_char2(bind), fname )
		bind = bind + 1
	end
end
function meu:get_font_name()
	local ref = self.ref
	local ui = self.ui
	local bind = ui.bu_font_selector:get_value()
	return param.get( ref.font_bind, "bind_"..aaa.format.int_to_char2(bind) )
end
function meu:update_ui()
	local ui = self.ui
	local font_name = self:get_font_name()
	ui.font_name:set_text( font_name )
end

function meu:update()
	local ref = self.ref
	local ui = self.ui
	local str = ui.bu_text:get_text()
	param.set( ref.text, str )

	local font_name = self:get_font_name()
	--self:print( "Toto ".. string.sub( font_name, -5).." "..(string.sub( font_name, -5)==".json") )
	param.set( ref.layer[1].use_shading, (string.sub( font_name, -5)==".json") and 2 or 1 )
end

