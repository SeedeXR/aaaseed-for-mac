function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:init_loc()

	self:add_camera()

	local ix = 1
	local iy = 1
	local SY = 1
	self:add_button(	{ix, iy,	4,SY },	"White",	self, "b_white",	false )

	iy = iy +1
	self:add_button(	{ix, iy,	4,SY },	"Move",	ref.boid_bdd, "move",	true )
	self:add_trig(		{ix+4, iy,	4,SY },	"restart",	self, "b_restart",	false )

	ix = 9
	iy = 2
	self:add_slider(	{ix,iy,		8,SY},	"nb boid",				self, "boid_nb", 1, 0, 4000 )

--	self:add_slider(	{ix,iy+1,	8,1},	"cone deformation",		ref.seine, "param_02", 1, -4, 4 )
end

function meu:init_loc()
	local ref = self.ref
	ref.attrib_layer	= self:get_obj_down( "attrib" )
	ref.boid_layer		= self:get_obj_down( "boid" )
 	ref.boid_bdd		= self:get_obj_down( "boid_bdd" )
	self.boids_nb = 3
	self.bdd_index = param.get( ref.boid_layer, "bdd" )
	ref.boids = {}
	ref.boid_param_begin = param.get_index( ref.boid_bdd, "dimension" )
	ref.boid_param_nb    = param.get_index( ref.boid_bdd, "scale_factor" ) - self.ref.boid_param_begin + 1
	ref.color = aaa.layer.create_color( ref.boid_layer, "boid" )
	for i = 1, self.boids_nb do
		local boid = BOID:create( "boid_multi_"..i, self:clone_obj(ref.boid_bdd) )
		self.ref.boids[i] = boid
		boid:set_param( "universe", 2 )
		boid:set_param( "id", 		i )
	end
end

local rgb_id =
{
	{ 1, 0, 0 },
	{ 0, 1, 0 },
	{ 0, 0, 1 },
	{ 1, 1, 0 },
	{ 0, 1, 1 },
	{ 1, 0, 1 },
}
function meu:make_color( id )
	if self.b_white then
		return 1, 1, 1
	else
		id = math.fmod( id-1, 6 ) + 1
		local c = rgb_id[id]
		return c[1], c[2], c[3]
	end
end
function meu:draw()
	local ref = self.ref
	self:draw_layers_begin()
---[[
		aaa.obj.update_then_draw( ref.attrib_layer	)
		local b_reset = self.b_restart
		for i = 1, 	self.boids_nb do
			local r, g, b = self:make_color( i )
			--self:print( i.."\t"..r.." "..g.." "..b)
			local boid = self.ref.boids[i]
			if b_reset then
				boid:trig_restart()
			end
			aaa.obj.set_param_from( boid:get_obj(), ref.boid_bdd, ref.boid_param_begin, ref.boid_param_nb )
			self.ref.color:set_rgb( r, g, b )
			boid:set_repulse_net_color( r, g, b )
			boid:set_nb( self.boid_nb )
			aaa.layer.set_bdd_external( ref.boid_layer, boid:get_obj() )
			param.set( ref.boid_layer, "use_bdd", "update_and_draw" )
			aaa.obj.update_then_draw( ref.boid_layer )
		end
		param.set( ref.boid_layer, "use_bdd", "none" )
		param.set( ref.boid_layer, "bdd", self.bdd_index )
		aaa.obj.update_then_draw( ref.boid_layer )

		--aaa.layer.set_bdd_external( ref.boid_layer )
--]]
	self:draw_layers_end()
end
