
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	ui.cam = self:add_camera()

	local ix,iy = 1,2
--	bu = self:add_button(	{ix, iy }, "Full", self, "b_full", true )

	bu = self:add_button(	{ix, iy+1 }, "Tri", self, "b_tri", true )
	bu = self:add_button(	{ix, iy+2 }, "Edge", self, "b_edge", false )
end

function meu:init()
	local ref = self.ref
--[[
	ref.bdd_face_track = self:get_layer_bdd( 3 )
	ref.sx = param.get_ref( ref.bdd_face_track, "image_src_size_x" )
	ref.sy = param.get_ref( ref.bdd_face_track, "image_src_size_y" )
	ref.crop_left 	=	param.get_ref( ref.bdd_face_track, "crop_left" 		)
	ref.crop_right 	=	param.get_ref( ref.bdd_face_track, "crop_right" 	)
	ref.crop_bottom =	param.get_ref( ref.bdd_face_track, "crop_bottom"	)
	ref.crop_top 	=	param.get_ref( ref.bdd_face_track, "crop_top" 		)
	ref.img_src_sx 	=	param.get_ref( ref.bdd_face_track, "image_src_size_x" 	)
	ref.img_src_sy 	=	param.get_ref( ref.bdd_face_track, "image_src_size_y" 	)

	ref.model = aaa.layer.get_model( self:get_layer( 1 ) )
		ref.sx				=	param.get_ref( ref.model, "size_u" )
		ref.sy				=	param.get_ref( ref.model, "size_v" )
		ref.sf				=	param.get_ref( ref.model, "size_factor" )

	ref.mapping = aaa.layer.get_mapping( self:get_layer( 2 ) )
		ref.tex_left 	=	param.get_ref( ref.mapping, "tex_left" 		)
		ref.tex_right 	=	param.get_ref( ref.mapping, "tex_right" 	)
		ref.tex_bottom 	=	param.get_ref( ref.mapping, "tex_bottom"	)
		ref.tex_top 	=	param.get_ref( ref.mapping, "tex_top" 		)

	self.faces = {}
	--todo set max faces
	for id=1,3 do
		local face = FACE_INFO:create( "Face Acc "..i )
		face:set_target( ref.bdd_face_track, id )
		self.faces[id] = face
	end
--]]
end

function meu:draw()
	local seq = ga:get_seq_cur()
	if not seq then return end
	local ease = seq:get_ease_in()

	MEU.draw( self )

	local fid = (self:get_inst_key() == "1") and 1 or 2
	local face_info = app:get_face_info( fid, 1 )
	if face_info:is_use() then
		local m = self:get_meu_by_name( "FaceUV_1" )
		--tar:draw_tri( face_info, self.b_tri, self.b_edge and 1 or (1-ease), {1,1,1}, 2. )
		m:draw_tri( face_info, {1,1,1, self.b_edge and 1 or (1-ease) }, 2. )
	end
end


