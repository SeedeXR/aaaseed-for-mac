if false then
	if not FX.cur then
		return
	end

	local fx = FX.cur

	function fx:add_color( color, name, post  )
		local p = param.get_ref( color, "global_"..name )
		local bu = bus_cur:add_slider()
		bu:set_target_param( p )
		bu:set_min_max( 0, 1 )
		bu:set_text( post.."_"..name )
	end

	function fx:add_ui_window()
		FX.add_ui_window( self, false )
		self.meu:add_rgb( 0, 0, nil, nil, nil, self.ref.color )

		bus_cur:move_next()
		local ref = self.ref
		ref.layer_b		=	aaa.layers.get_layer( ref.obj, 2 )
		ref.color_b		=	aaa.layer.get_color( ref.layer_b )
		self:add_color( ref.color_b, "Alpha",	"B"  )
		self:add_color( ref.color_b, "Red",		"B"  )
		self:add_color( ref.color_b, "Green",	"B"  )
		self:add_color( ref.color_b, "Blue",	"B"  )
	end

	function fx:init()
		fx.camera_to_use = "back"
	end
else
	if not FX.cur then
		aaa.layers.skip_rest()
		return
	end

	local fx = FX.cur

	function fx:add_color( ref, name, pre  )
		local p = param.get_ref( ref, "global_"..name )
		if pre then name = pre.."_"..name end
		local bu = bus_cur:add_slider( name )
		bu:set_target_param( p )
		bu:set_min_max( 0, 1 )
		return bu
	end
	function fx:add_color_abgr( ref, pre  )
		local bu = fx:add_color( ref, "Alpha", pre  )
		bu:set_meter( true )
		self:add_color( ref, "Red", pre  )
		self:add_color( ref, "Green", pre  )
		self:add_color( ref, "Blue", pre  )
	end
	function fx:add_ui_window()
		FX.add_ui_window( self, false )

		local ref = self.ref
		self:add_color_abgr( ref.color, nil )

		bus_cur:move_next()
		ref.layer_b		=	aaa.layers.get_layer( ref.obj, 2 )
		ref.color_b		=	aaa.layer.get_color( ref.layer_b )
		self:add_color_abgr( ref.color_b, "B"  )

		bus_cur:move_next()
		ref.layer_c		=	aaa.layers.get_layer( ref.obj, 3 )
		ref.color_c		=	aaa.layer.get_color( ref.layer_c )
		self:add_color_abgr( ref.color_c, "C"  )
	end
	function fx:init()
		fx.camera_to_use = "back"
	end
end