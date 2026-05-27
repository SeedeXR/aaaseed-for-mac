
function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "art", "2d", "core", "draw", "procedural", "vj" },
			help = 	{
						"Draw the Maa signature.",
					}
			}
end

function meu:define_ui()
	--local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 9,2
	local SY,DY = 1, .2

	ui.cam = self:add_camera()
	iy = iy + 1 + DY
	self:add_trzs(			{ix,iy,		8,3}  )

--	self:add_rendering()

	ix,iy = 1,2

	self:add_button(		{ix,iy, 	4,SY},	"grid",				self, "b_grid", true )
	self:add_slider( 		{ix+4,iy,	4,SY},	"grid_line_width",	self, "grid_line_width",	3,	0,16	)
	    :set_text( "Line" )
	iy = iy + SY + 1

	bu = self:add_selector(	{ix,iy,		8,1},	"Mode" )
		bu:set_nb( 3, 1 )
		--bu.do_mouse_move = back_color_do_click_down
		--bu:set_method_on_click(			app, "change_back_color",	bu )
		bu:set_item_text( 1, "Maa", "MaaEB", "RT Art" )
		bu:set_target_lua( self, "s_mode" )
	iy = iy + SY
	self:add_slider( 		{ix,iy,		4,SY},	"text_line_width",	self, "text_line_width",	3,	0,16	)
	    :set_text( "Line" )
	iy = iy + SY + DY

end

function meu:init()
	local ref = self.ref
	
	ref.line_size = param.get_ref( ref.rendering, "line_size" )
	param.set_save( ref.line_size, false )
end

function meu:draw()
	local ref = self.ref

	local grid_line_width = self.grid_line_width
	local text_line_width = self.text_line_width
	local alpha = self:get_alpha()
	self:draw_layers_begin()
		gol.set_default()
		--gol.reset()	-- changed because of fog
		if self.s_mode == 3 then
			if self.b_grid then
				MAAEB.draw_grid( grid_line_width, alpha )
			end
			gol.color_white( alpha )
			gol.push_matrix()
				gol.translate(-2,.13)
				gol.scale(6.45)
		
				param.set( ref.line_size, text_line_width )
				self:draw_layers()
			gol.pop_matrix()
		else
			MAAEB.set_alpha( alpha )
			--MAAEB.draw( 0, 0, 0, 4, true, 3.14 )
			if self.s_mode == 1 then
				MAAEB.draw( 0,0, 0,2, self.b_grid, 3.14, text_line_width, grid_line_width )
			elseif self.s_mode == 2 then
				MAAEB.draw( 0,0, 0,2, self.b_grid, nil, text_line_width, grid_line_width )
			end
		end
	self:draw_layers_end()
end

function meu:draw_icon()
	gol.push_matrix()
		gol.scale( 1, 4 )
		MAAEB.draw( -.35, 0, 0, .25, false, nil, 1 )
	gol.pop_matrix()
end