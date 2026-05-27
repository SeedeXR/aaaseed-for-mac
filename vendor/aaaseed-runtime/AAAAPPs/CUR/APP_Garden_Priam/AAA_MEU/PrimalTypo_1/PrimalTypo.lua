function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local SY = 1
	local DY = .2
	local ix,iy = 1,1

	self:add_camera()
	self:add_monitor()

	bu = self:add_selector( {ix,iy,	8,6}, "Font" )
		bu:set_nb_min_0( 8, 8 )
		bu:set_item_text_from_nb()
		bu:set_target_param( ref.font )
	iy = iy + 6 + DY

	bu = self:add_text( {1,8, 16,SY}, "text" )
		bu:set_text( param.get( self.ref.text ) )
		--todo this don't function
		--bu:set_target_param( ref.text )
		ui.bu_text = bu 


-----------LOAD FONT try by Priam - too complicted ://
	-- self:add_trig_method(	{ix,iy,		4,SY},	"LoadFont", 		self, "load_font"	):set_color_back( "load" )
end

function meu:init()
	local ref = self.ref

	ref.bdd		=	self:get_layer_bdd( 1 )
	ref.text	=	param.get_ref( ref.bdd, "text" )
	ref.font	=	param.get_ref( ref.bdd, "font" )


-------------CUSTOM




end


-----------LOAD FONT try by Priam - too complicted ://
-- function meu:load_font( id )
-- 	--aaa.print_fn()
-- 	----local bind = self:get_texture_bind_2d( id )
-- 	local str = "load textures at index "..bind
-- 	local filter = "Reconnus\0*.oft;*.ttf;*"
-- 	local filenames = aaa.file.do_dialog_open( str, filter, true )
-- 	table.print( filenames, "filenames" )
-- 	for i,fname in IPAIRS(filenames) do
-- 		self:print( i.." load "..fname.." at "..bind )
-- 		aaa.img.read( bind, fname )
-- 		bind = bind + 1
-- 	end
-- end


function meu:set_text( str )
	param.set( self.ref.text, str )
end

function meu:update()
	local str = self.ui.bu_text:get_text()
	param.set( self.ref.text, str )
end