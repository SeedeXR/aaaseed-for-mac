
function meu:init()
	local ref = self.ref

	ref.bdd_field_gene = self:get_layer_bdd( 2 )

	ref.b_draw			= param.get_ref( ref.bdd_field_gene, "draw" )
	ref.b_draw_particle = param.get_ref( ref.bdd_field_gene, "draw_particle" )
	ref.scale			= param.get_ref( ref.bdd_field_gene, "scale" )
	ref.max_threshold	= param.get_ref( ref.bdd_field_gene, "max_threshold" )
	ref.min_threshold	= param.get_ref( ref.bdd_field_gene, "min_threshold")
	ref.decay_factor	= param.get_ref( ref.bdd_field_gene, "decay_factor")
	ref.precision		= param.get_ref( ref.bdd_field_gene, "precision")
	ref.draw_count		= param.get_ref( ref.bdd_field_gene, "draw_count")
	ref.draw_steps		= param.get_ref( ref.bdd_field_gene, "draw_steps")
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()
	self:add_rendering()

	local ix = 1
	local iy = 3
	local SY = 1
	local DY = .2

	bu = self:add_button( {			ix, 	iy }, 				"Draw", 			ref.bdd_field_gene, "draw",				false	)
	iy = iy + SY
	bu = self:add_button( {			ix, 	iy }, 				"Draw Particles", 	ref.bdd_field_gene, "draw_particle",	false	)
	iy = iy + SY
	bu = self:add_trig_fn(	{		ix+5, 	iy,			3, SY},	"Focus",			aaa.obj.set_focus_ui, self.ref.bdd_field_gene			)
	iy = iy + SY
	bu = self:add_trig_method(	{	ix+5, 	iy,			3, SY},	"Recompute", 		self, "recompute_field" 						)
	iy = iy + SY
	bu = self:add_trig_method(	{	ix+5, 	iy,			3, SY},	"Add_tapestry", 	self, "add_tapestry" 							)
	iy = iy + SY
	bu = self:add_trig_method(	{	ix+5, 	iy,			3, SY},	"Save_Mesh", 		self, "save_mesh" 								)
	iy = iy + SY

	iy = 5
	-- ui.bu_layer_index = self:add_slider(	{ix+4,iy,	4,SY},	"Index",			self, "mesh_index",		0, 0, 16 )
	bu = self:add_slider(	{ix,iy,	4,SY},	"Scale",			ref.bdd_field_gene, "scale",				1, 0, 100 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	4,SY},	"Max threshold",	ref.bdd_field_gene, "max_threshold",		1, 0, 20 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	4,SY},	"Min Threshold",	ref.bdd_field_gene, "min_threshold",		0, 0, 20 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	4,SY},	"Decay Factor",		ref.bdd_field_gene, "decay_factor",			1, 0, 5)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	4,SY},	"Precision",		ref.bdd_field_gene, "precision",			0.20, 0.01, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	4,SY},	"Draw count",		ref.bdd_field_gene, "draw_count",			1., 0.1, 2. )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	4,SY},	"Draw steps",		ref.bdd_field_gene, "draw_steps",			1., 1, 20 )
	iy = iy + SY

	-- bu = self:add_slider(	{ix,iy,	4,SY},	"Scale",	self, "scale",			1,		0, 10 )
	-- iy = iy + SY
	-- bu = self:add_slider(	{ix,iy,	4,SY},	"Size",		self, "size",			.01,	0, 1 )

end

function meu:recompute_field()
	param.set( self.ref.bdd_field_gene, "compute_trig", true )
end

function meu:save_mesh()
	param.set( self.ref.bdd_field_gene, "save_mesh", true )
end

function meu:add_tapestry()
	local meu = self:get_meu_by_name_no_error( "Tapestry_1" )
	self:print(meu)
	if meu then
		local boxes = meu:get_bounding_boxes()
		self:print("boxes: "..boxes)
		if boxes then
			for k, box in pairs(boxes) do
				self:print("box:")
				self:print('    x: '..box.center.x)
				self:print('    y: '..box.center.y)
				self:print('    z: '..box.center.z)
				self:print('    r: '..box.radius)
				aaa.bdd.add_sphere( self.ref.bdd_field_gene, box.center.x, box.center.y, box.center.z, box.radius )
			end
		end
	end

	local meu = self:get_meu_by_name_no_error( "Clam_Tapestry" )
	self:print(meu)
	if meu then
		local boxes = meu:get_bounding_boxes()
		self:print("boxes: "..boxes)
		if boxes then
			for k, box in pairs(boxes) do
				self:print("box:")
				self:print('    x: '..box.center.x)
				self:print('    y: '..box.center.y)
				self:print('    z: '..box.center.z)
				self:print('    r: '..box.radius)
				aaa.bdd.add_sphere( self.ref.bdd_field_gene, box.center.x, box.center.y, box.center.z, box.radius )
			end
		end
	end

end

function meu:draw()
	self:draw_layers_begin()
	-- TODO: remove layer 1.
	self:draw_layer( 2 )
	self:draw_layers_end()
end
