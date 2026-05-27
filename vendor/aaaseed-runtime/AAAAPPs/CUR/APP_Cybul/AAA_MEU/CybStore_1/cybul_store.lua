
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	ix = ix + 1

	local i = 1;
	bu = self:add_slider(	{ix,iy,	8,SY}, "transparence", ref, "transparency", 0, 0, 1000 )
	-- iy = iy + SY
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "color G", sha:get_ref_frag_float( 2 ), nil, .5, 0, 1 )
	-- iy = iy + SY
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "color B", sha:get_ref_frag_float( 3 ), nil, .5, 0, 1 )
	-- iy = iy + SY

	-- bu = self:add_slider(	{ix,iy,	8,SY}, "start", sha:get_ref_frag_float( 4 ), nil, .5, 0, 100 )
	-- iy = iy + SY
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "density", sha:get_ref_frag_float( 5 ), nil, .5, 0, 100 )
	-- iy = iy + SY

	iy = 4
	SY = 2.5

	self:add_bu_texture_target_unit( {9,iy, nil,SY}, "Src", 1	)
	iy = iy + SY

end

function meu:init()
	local ref = self.ref

	self.layer_attr = 1
	self.layer_shading = 13
	self.layer_attr_grid = 7
	self:add_shading( self.layer_shading )
	ref.shading_zip = self:get_shading()
	self:add_shading( self.layer_shading + 1 )
	ref.shading_lam = self:get_shading()

	self.use_cache = true
end

function meu:draw()
	-- cybul_store is only used to store the store characteristics... Should probably be cleaned in the near future.
	self:draw_layers_begin()
	self:draw_layers_end()
end

function meu:set_gl_parameters( tex_type )
	local tex, table

	if tex_type == "hessian" then table = app.hessian; tex = app.tab3d.tex_toile end
	if tex_type == "zip_tex" then table = app.zip_tex; tex = app.tab3d.tex_zips  end

	if tex then
		gol.set_texture_dim( 2 )
		gol.bind_texture( tex.bind )
	end


	if table then
		if table.gl_hor == "repeat" and table.gl_ver == "repeat" then gol.set_wrap_2d_repeat() 			end
		if table.gl_hor == "repeat" and table.gl_ver == "mirror" then gol.set_wrap_2d_repeat_mirror() 	end
		if table.gl_hor == "mirror" and table.gl_ver == "repeat" then gol.set_wrap_2d_mirror_repeat() 	end
		if table.gl_hor == "mirror" and table.gl_ver == "mirror" then gol.set_wrap_2d_mirror() 			end
	end

end

function meu:set_shader( tex_type )
	local ref = self.ref
	local sha
	if tex_type == "lambrequin_lumineux" then
		sha = ref.shading_lam
	else
		sha = ref.shading_zip
	end

	local val = 0
	if tex_type == "zip_tex" 				then val = app.zips.transparent 			and 1.0 or 0.0 end
	if tex_type == "lambrequin_lumineux" 	then val = app.lambrequin_lumineux.lumineux and 1.0 or 0.0 end
	sha:set_frag_float_2( val )
end
