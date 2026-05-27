
function meu:define_meu_infos( )
	return { 	author = "Mâa",
				tags = { "2d", "imageprocessing", "texture" },
				help =	{	"MEU just encapsulating a c_bdd_img_cv_mutitouch, need manipulationd in flatland.",
						}
			}
end
function meu:define_ui()		
	local ref = self.ref
	local ui = self.ui

	self:add_camera()

	local bu
	if ref.coverage_anal then
		bu = self:add_text_info(	{1,1,	8,1}, "Cover" )
		--bu:set_text( "No analyse" )
		ui.bu_cover = bu
	end

end

function meu:init()
	local ref = self.ref
	local layer_anal = self:get_layer( 3 )
	if layer_anal then
		ref.bdd_anal = aaa.layer.get_bdd( layer_anal )
		ref.coverage_anal = param.get_ref( ref.bdd_anal, "out_coverage" )
	end
	--self.b_filled = false
end

function meu:update_ui()
	local ui = self.ui
	if self.coverage_anal then
		ui.bu_cover:set_text( "Out Coverage : "..self.coverage_anal )
	end
end

function meu:update()
	local ref = self.ref
	if ref.coverage_anal then
		self.coverage_anal = param.get( ref.coverage_anal )
		--todo replace this by a more generic way
		-- local meu_2054 = self:get_meu_by_name( "2054_1" )
		-- if meu_2054 then
		-- 	meu_2054.coverage_anal = self.coverage_anal
		-- end

	end
end

meu.draw_icon = MEU.draw_icon_opencv
