function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ui = self.ui

	self:add_camera()

	local function set_color_out( bu )
		bu:set_color_back( { 1, .5, 0, .5 } )
	end
	local DXO = 2
	local function add_output( ix, iy, name )
		local bu = self:add_slider(	{ix,iy,	DXO,4},	name,		nil, nil,	0, 0, 1 )
		set_color_out( bu )
		bu:set_meter( true )
		return bu
	end

	local bdd = self:get_layer_bdd( 1 )
	--self:print( "monthey "..bdd	 	)
	ref.nb_u	= param.get_ref( bdd, "nb_u" 	)
	ref.nb_v	= param.get_ref( bdd, "nb_v" 	)
	ref.nb_axe	= param.get_ref( bdd, "nb_axe" 	)

	local iy = 2
	local ix = 1
	local DY = .8

	iy = iy + 1
	ref.deformer	= self:get_obj_down( "def_blob" )
	ref.blob	= param.get_ref( ref.deformer, "strength"  )  --todo do good ortho in C
	bu = self:add_slider(	{ix,iy,	8,1},	"blob",			self, "blob_inf",		0, 0, 1 )

	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	8,1},	"size",			self, "size_inf",		0, 0, 1 )

	iy = iy + 1
	ref.deformer	= self:get_obj_down( "def" )
	ref.strenght	= param.get_ref( ref.deformer, "strenght"  )
	bu = self:add_slider(	{ix,iy,	8,1},	"strenght",		self, "strenght_inf",	0, 0, 1 )

	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	8,1},	"level",		self, "level",			1, 1, 10 )
		bu:set_value_type_integer( true )

	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	8,1},	"nb_axe",		bdd, "nb_axe",			1, 1, 16 )
		bu:set_value_type_integer( true )

end

function meu:init()
	aaa.lua.global.declare_table( "LeMur" )
end

function meu:update()
	local ref = self.ref
	local input = LeMur and LeMur.reldec or 1
	param.set(	ref.size_axe,	self.size_inf		* 4. * input )
	param.set(	ref.strenght,	self.strenght_inf	* 2. * input )
	param.set(	ref.blob,		-self.blob_inf 		* 2. * input )

	local l = self.level
	param.set(	ref.nb_u, l*4 + 1 )
	param.set(	ref.nb_v, l*3 + 1 )
end
