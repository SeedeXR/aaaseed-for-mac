function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
	local par

	local layer		=	self:get_layer(1)--identifying 3D object we are working with 
	local model		=	aaa.obj.get_down( layer, "model" )--identifying 3D object we are working with 
    -- (NOTE: the name "model" needs to be filled in by hand in the "ref_name" param in Flatland)


	local ix = 1
	local iy = 1
	local ix,iy = 1,1
    local SY,DY = 1,.2
	
	self:add_camera()

	-- self:add_bu_texture_target_unit( {ix,iy, 8,6} )

	self:add_rendering()


	self:add_slider({ix, iy, 4, SY}, "SizeU", param.get_ref( model, "size_u" ), nil, 1, 0, 8):set_color_back("u") --MEU nuits = 16x16
	self:add_button({ix+4, iy, SY, SY}, "Diff", self, "b_diff", false):set_text_visible(false) --MEU nuits = 16x16
    self:add_slider({ix+4+SY, iy, 4-SY, SY}, "SizeV", param.get_ref( model, "size_v" ), nil, 1, 0, 8):set_color_back("v") --MEU nuits = 16x16
	self:add_slider({ix, iy+1, 4-SY, SY}, "Size Axe", param.get_ref( model, "size_axe" ), nil, 0.25, 0, 8):set_color_back("w") --MEU nuits = 16x16
	-- AUTRES METHODES pour sliders
	-- self:add_slider({ix, iy, 4, SY}, "SizeU", ref.size_u, nil, 1, 0, 8):set_color_back("u") --MEU nuits = 16x16
	-- self:add_button({ix+4, iy, SY, SY}, "Diff", self, "b_diff", false):set_text_visible(false) --MEU nuits = 16x16
    -- self:add_slider({ix+4+SY, iy, 4-SY, SY}, "sizev", self, "size_v", 1, 0, 8):set_color_back("v") --MEU nuits = 16x16
	
	

	-- self:add_nb_uv( {1, 5}, self:get_layer_bdd(1) ) -- customize donut mode4l structure
	-- slider qui update Flatland et vice versa = affecte au slider un param
end



function meu:init()

	---aaa.flatland.set_color_blind( .2, .5, .3 )


	local ref	= self.ref -- table persistente dans ce MEU
-- -- -- --	self.time = 0
-- 	self.pos		= { x=0, y=0, z=0 }
-- 	self.rot		= { x=5, y=0, z=0 }
-- 	self.tra		= { u=5, v=0, w=0 }
-- -- -- 	self.tra_end	= { u=2, v=0, w=0 }

	local bdd			=	self:get_layer_bdd(1)
-- self:print( "bddxx is "..bdd )
-- self:print( "bdd "..bdd.." is class "..aaa.get_obj_class(bdd) ) -- reference 16802746

-- 	local layer			=	self:get_layer(1)
	
-- self:print( "layer is "..layer.." is name "..aaa.get_obj_name(layer) )




-- 	local model			=	aaa.get_obj_down( layer, "model" )

-- 	ref.size_u			=	param.get_ref( model, "size_u" )
-- 	ref.size_v			=	param.get_ref( model, "size_v" )
-- 	ref.size_axe			=	param.get_ref( model, "size_axe" )



-- self:print( "Size axe is "..layer.." is Size Axe "..param.get( model, "size_axe" ) )



-- 	local transfo		=	aaa.get_obj_down_by_class_no_error( layer, "transfo_trs" )
-- self:print( "transfo is "..transfo )
-- 	local tra = {}
-- 	tra.x, tra.y, tra.z	=	param.get_ref( transfo, "translate_x" ), param.get_ref( transfo, "translate_y" ), param.get_ref( transfo, "translate_z" )
-- 	ref.tra = tra
-- self:print( "translate x is "..param.get(tra.x) .." "..param.get(tra.y) )



-- -- 	local rot = {}
-- -- 	rot.x, rot.y, rot.z	=	param.get_ref( transfo, "rotate_x" ), param.get_ref( transfo, "rotate_y" ), param.get_ref( transfo, "rotate_z" )
-- -- 	ref.rot = rot
	-- local koloore	= param.get( 	aaa.ref.flatland, "Back_Color_High_red"	)
	-- self:print( koloore )

end

-- function meu:draw()
-- end


-- Ci- dessous: tu dois faire un nil ou commenter le print dans meuL:update qui te dérange.
-- Si tu commente et sauve le script, meu:update_ui() continuera de marcher, elle doit etre dé-définie:
-- voila le nil que tu dois faire sur la fonction: meu.update_ui = nil
-- les MEUs se rechargent en tapant sur F12, c'est la seule maniere de prendre en compte un élément commenté (pour le désactiver) a l'intérieur de la fonction Update. 
function meu:update_ui()

	-- local bu = self:get_bu_by_key("SizeV")
	-- bu:set_alpha_bu(self.b_diff and 1 or .25)
	-- local bu = self:get_bu_by_key("Size Axe")
	-- bu:set_alpha_bu(self.b_diff and 1 or .25)
end
-- -- -- meu.update_ui = nil -- dé-définit la fonction

-- function meu:update()
-- 	local ref = self.ref

-- 	if self.b_diff then 
-- 		param.set( ref.size_v, self.size_v ) 
-- 		self:print( "b-diff YES" )
-- 	else
-- 		-- param.set( ref.size_v, param.get(ref.size_v) )
-- 		self:print( "b-diff NO" )
-- 	end
-- end
