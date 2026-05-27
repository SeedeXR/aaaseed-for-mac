
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local ix,iy

	local sha = self:get_shading()

	ui.bu_col = self:add_rgbf(	{1,8.2 } )

	self:add_blending( {1,11} )

	self:add_camera()

	ix = 9
	iy = 3
	if aaa.lua.global.get( "arbre" ) then
		bu = arbre:add_shading_selector( self, {ix,iy, 8,2}, "Mode" )
			bu:set_value(2)
	end

	iy = iy + 2
	bu = self:add_selector(	{ix,iy,	8,2}, "See" )
		bu:set_nb_min_0( 8, 2 )
		bu:set_target_lua( self, "s_see" )
		--bu:set_method_on_value_change( self, "change_readers", bu )
		bu:set_item_text_from_nb()
		bu:set_item_text( 16, "In" )


	iy = iy + 2
		bu = self:add_trig(	{	ix,	iy },					"ReLoad",			sha.ref.reload_all,	nil,	false )
		ui.bu_reload = bu

	iy = iy + 1
		bu = self:add_slider(	{ix,iy,	8,1},		"Transition",		self,	"transition",		0, 0,1 )

end

function meu:init()
	self.time = 0
--	self:print( "je passe ds l'init" )
	self:add_shading()
end

function meu:update_ui()
	local ui = self.ui
	local sha = self:get_shading()
	ui.bu_reload:set_text_color( sha:is_valid() and "valid" or "invalid" )
end
function meu:update()
	local sha = self:get_shading()
	local binds = app.__tree_binds
	if binds then
		sha:set_frag_int( 1, #binds )
	end
	sha:set_frag_int( 2, self.s_mode )
	sha:set_frag_int( 3, self.s_see )
	sha:set_frag_float( 1, self.transition )

	--self:print( self.ui.bu_col:get_rgba() )
end

function meu:draw()
---[[
	local binds = app.__tree_binds
	if binds then
		for i=1,15 do
			if binds[i] then
				gol.set_tex_unit_2d_bind( i, binds[i] )
				gol.set_tex_unit_cur( 0 )
			end
		end
		gol.set_tex_unit_cur( 0 )
	end
--]]
	MEU.draw(self)
---[[
--	if binds then
		for i=1,15 do
			gol.set_tex_unit_0d( i )
		end
		gol.set_tex_unit_cur( 0 )
--	end
--]]
end
