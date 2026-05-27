

--todo for testing
--local	VER_PM			= 5 + nil

function meu:define_ui()
	local ref = self.ref
	local bu
	local par


	self:add_camera()

	local iy = 1
	local ix = 1
	local SY = 1.2
	local DY = .2
	local bu

	bu = self:add_button(		{ix,iy,		8,SY},	"Draw Field",	self:get_layer_ref_table(1).use_bdd )
		:set_min_max( 1, 2 )
	iy = iy + SY + DY
	bu = self:add_button(		{ix,iy,		8,SY},	"Draw",			self:get_layer_ref_table(2).use_bdd )
		bu:set_min_max( 3, 2 )
	iy = iy + SY + DY
	

	bu = self:add_trig(			{ix,iy,		4,SY}, "Restart",		ref.restart_trig )
	bu = self:add_trig_method(	{ix+4,iy,	4,SY}, "Focus", 		self, "set_focus_boid" )
	iy = iy + SY + DY

	self:add_bu_texture_target_unit( 	{ix,iy,		8,6},  	"Field", 1 )
	iy = iy + 6

	ix,iy = 9,2+DY
	bu = self:add_button(		{ix,iy,		3,SY},	"Field Active", ref.field_active )
		:set_text( "Field" )
	bu = self:add_slider(		{ix+3,iy,	5,SY},	"Influence",	ref.field_influence, nil,	0, -1,1 )
	iy = iy + SY + DY

	local t = { "repulse", "flocking", "steering" }
	for i,name in ipairs(t) do
		bu = self:add_button(	{ix,iy,		3,SY},	name,				ref.bdd, name.."_active",		false )
		bu = self:add_slider(	{ix+3,iy,	5,SY},	name.."_Influence", ref.bdd, name.."_influence",	0, 0,1 )
			:set_text( "Influence" )
		iy = iy + SY
		bu = self:add_slider(	{ix+2,iy,	6,SY},	name.."_Distance", ref.bdd, name.."_distance",	0, 0,.25 )
			:set_text( "Distance" )
		iy = iy + SY + DY
	end
end

function meu:init()
	local ref = self.ref

	local lay2	=	self:get_layer( 2 )
	aaa.obj.update( lay2 )	-- update to create color object
						--todo make a get_obj_always to avoid the updating
	local col	=	aaa.obj.get_down_by_class( lay2, "color" )
	ref.alpha	=	param.get_ref( col, "global_alpha" )
	ref.bdd				=	self:get_layer_bdd( 2 )
		ref.restart_trig	=	param.get_ref( ref.bdd, "restart_trig" )
		ref.field_active	=	param.get_ref( ref.bdd, "field_active" )
		ref.field_influence	=	param.get_ref( ref.bdd, "field_influence" )

	ref.def_image = aaa.obj.get_down_by_class( lay2, "def_image" )
		ref.def_image_src = param.get_ref( ref.def_image, "Image_src" )
end

function meu:set_focus_boid()
	aaa.obj.set_focus_ui( self.ref.bdd )
end

---[[
function poid.hook_do_contact( a, b )
	--aaa.print( a.." - "..b )
	return 1.
end
--]]
--poid.hook_do_contact = nil

function meu:draw()
	local ref = self.ref
	local bind = self:get_texture_bind_2d(1)
	param.set( ref.def_image_src, bind )
	local ref = self.ref
	param.set( ref.alpha, self:get_alpha() )
	MEU.draw( self )
end