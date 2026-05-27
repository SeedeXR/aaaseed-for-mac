
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local SY = .8
	local DY = .1
	local ix,iy = 1,1

	self:add_camera()
	self:add_monitor()
	self:add_rendering()
--	self:set_tab_key( "Main" )

	-- bu = self:add_selector( {ix,iy,	8,6}, "Font" )
	-- 	bu:set_nb_min_0( 8, 8 )
	-- 	bu:set_item_text_from_nb()
	-- 	bu:set_target_param( ref.font )
	-- iy = iy + 6 + DY

	-- bu = self:add_text( {1,8, 16,SY}, "text" )
	-- 	bu:set_text( param.get( self.ref.text ) )
	-- 	--todo this don't function
	-- 	--bu:set_target_param( ref.text )
	-- 	ui.bu_text = bu
	self:add_transfo( {1,1, 8,2.4}, 4 )
end

function meu:init()
	-- local ref = self.ref
	-- ref.bdd		=	self:get_layer_bdd( 1 )
end