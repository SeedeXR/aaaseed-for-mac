if CLASS.DECLARE( "BU_WWW", BU ) then
	BU_WWW:set_class_status_doc(	CLASS.STATUS.GABU,
									"a BU to display/browse a web page" )
end

function BU_WWW:create( rect )
	local self = BU_WWW:create_low()
	self:set_border_line(true)
	self:set_kind_image()
	self.b_www = true
	self:disable_mobile()
	self:set_function_on_click_double( BU_WWW.trig_load, self )
	return self
end

function BU_WWW:create_low()
	local self = BU_WWW:create_instance_no_name()

	table.insert( BU_WWW.inst, self )

	self.ref = {}
	local ref = self.ref

	--	create a tex_video object from the one in the layer
	ref.web		= app:clone_obj( BU_WWW.ref.bdd_web )

	self:set_img( IMGS.get_img_new() )
	ref.bind_dst		= param.get_ref( ref.web, "bind_dst" )
	param.set( ref.bind_dst, self:get_texture_bind_2d() )

	ref.load_trig				= param.get_ref( ref.web, "load_trig" )
	ref.back_trig				= param.get_ref( ref.web, "history_backward_trig" )
	ref.forw_trig				= param.get_ref( ref.web, "history_forward_trig" )
	ref.http					= param.get_ref( ref.web, "http_address" )
	ref.mouse_x					= param.get_ref( ref.web, "mouse_x" )
	ref.mouse_y					= param.get_ref( ref.web, "mouse_y" )
	ref.mouse_move_trig			= param.get_ref( ref.web, "mouse_move_trig" )
	ref.mouse_left_down_trig	= param.get_ref( ref.web, "mouse_left_down_trig" )
	ref.mouse_left_up_trig		= param.get_ref( ref.web, "mouse_left_up_trig" )
	ref.size_x					= param.get_ref( ref.web, "render_size_x" )
	ref.size_y					= param.get_ref( ref.web, "render_size_y" )
	ref.wheel_trig				= param.get_ref( ref.web, "mouse_wheel_trig" )
	ref.wheel_scroll			= param.get_ref( ref.web, "mouse_wheel_scroll" )
	ref.transparent				= param.get_ref( ref.web, "background_transparent" )

	self:set_transparent( false )

	return self
end

function BU_WWW:set_pixel_size( sx, sy )
	--self:print_inverse( "------------set_pixel_size( "..sx..", "..sy.." )" )
	local ref = self.ref
	param.set( ref.size_x, sx )
	param.set( ref.size_y, sy )
end

function BU_WWW:trig_load()			param.set( self.ref.load_trig, 1 )		end
function BU_WWW:go_back()			param.set( self.ref.back_trig, 1 )		end
function BU_WWW:go_forward()		param.set( self.ref.forw_trig, 1 )		end

function BU_WWW:set_transparent(b)	param.set( self.ref.transparent, b )	end

function BU_WWW:set_www_page( addr, b_dont_load )
	if addr ~= nil then
		self:print( "set www page to "..addr )
		param.set( self.ref.http, addr )
		if not b_dont_load then
			self:trig_load()
		end
	end
end

function BU_WWW:update()
	if BU_WWW.focus == self then
		if aaa.mouse.is_wheel_up() then
			self:do_mouse_wheel( 100 )
		elseif aaa.mouse.is_wheel_down() then
			self:do_mouse_wheel( -100 )
		end
	end
	--gol.get_error( self.."update() before" )
	--self:print( "update()" )
	--self:trig_load()
	aaa.obj.update( self.ref.web )
	--gol.get_error( self.."update() after" )

	oo.getsuper(BU_WWW).update(self)
end

function BU_WWW:do_mouse_wheel( scroll )
	local ref = self.ref
	param.set( ref.wheel_scroll, scroll )
	param.set( ref.wheel_trig, true )
end

function BU_WWW:do_mouse_move( x, y )
	oo.getsuper(BU_WWW).do_mouse_move( self, x, y )
	x,y = x + .5, .5 - y
	--self:print( "mouse move at "..x..", "..y )
	local ref = self.ref
	param.set( ref.mouse_x, x )
	param.set( ref.mouse_y, y )
	param.set( ref.mouse_move_trig, true )
	aaa.obj.set_focus_ui( self.ref.web )
end

function BU_WWW:do_click_down( x, y )
	oo.getsuper(BU_WWW).do_click_down( self, x, y )
	self:do_mouse_move( x, y )
	--self:print( "click down at "..x..", "..y )
	param.set( self.ref.mouse_left_down_trig, 1 )
	BU_WWW.focus = self
	--[[
	if x < -.4 then
		self:go_back()
	elseif x > .4 then
		self:go_forward()
	end
	--]]
	--return true
end

function BU_WWW:do_click_up( x, y )
	oo.getsuper(BU_WWW).do_click_up( self, x, y )
	--self:print( "click up" )
	param.set( self.ref.mouse_left_up_trig, true )
	--return true
end

function BU_WWW.reinit()
	BU_WWW.inst = {}
end

function BU_WWW.c_init()
	if BU_WWW.inst then return end

	--aaa.print( "BU_WWW.c_init()" )
	BU_WWW.ref = {}
	--BU_WWW.ref.bdd_web = aaa.obj.get_by_name_symbo( "gabu_www" )
	--local layers = aaa.obj.get( "GaBu/GaBu_BU/BU_www/fx.layers_param" )
	BU_WWW.ref.bdd_web = aaa.obj.get_by_class_and_name_symbo( "bdd_web", "gabu_www" )
	BU_WWW.reinit()
end

function BU_WWW.free()
	for _, bu in pairs(BU_WWW.inst) do
		local obj = bu.ref.web
		if aaa.obj.is_focus( obj ) then
			--todo this dhould move to AAASeed
			--to avoid focus on a non existing obj and so a crash
			aaa.obj.set_focus_ui( aaa.ref.pref )
		end
		app:delete_obj( bu.ref.web )
	end
	aaa.print( "BU_WWW.free()" )

	BU_WWW.reinit()
end

BU_WWW.c_init()
