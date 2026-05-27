function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "2d", "texture", "ImageProcessing", "Depreciated", "unfinished" },
			help = "Compute Distance field with OpenCV, JumpFlood is faster and more complete"
			 }
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = .8
	local DY = .5

	--self:add_camera()


	-- --todo do it giving it an update and/or draw fns 
	-- self.ui.bu_info_src = self:add_text_info( {ix,iy,		16,SY}, "Info_src" )
	-- self.ui.bu_info_dst = self:add_text_info( {ix,iy+SY*2,	16,SY}, "Info_dst" )
	-- iy = iy + SY * 4 + DY

	ui.bu_info_src = self:add_text_info( {ix,iy,		8,SY}, "Src" )
	ui.bu_info_dst = self:add_text_info( {ix+8,iy,		8,SY}, "Dst" )
	iy = iy + SY
	bu = self:add_monitor( {ix,iy,		8,6}, "Monitor_Src", param.get(ref.img_src) )
		ui.bu_mon_src = bu 
	bu = self:add_monitor( {ix+8,iy,	8,6}, "Monitor_Dst", param.get(ref.img_dst) )
		ui.bu_mon_dst = bu
	iy = iy + SY * 6 + DY
end

function meu:init()
	local ref = self.ref
	local layer = self:get_layer( 1 )
	ref.bdd = aaa.layer.get_bdd( layer )
	ref.img_src 		= param.get_ref( ref.bdd, "image_src" )
	ref.img_src_sx 		= param.get_ref( ref.bdd, "image_src_size_x" )
	ref.img_src_sy 		= param.get_ref( ref.bdd, "image_src_size_y" )
	ref.img_src_format	= param.get_ref( ref.bdd, "image_src_format" )
	ref.img_dst			= param.get_ref( ref.bdd, "image_dst_base" )
	ref.img_dst_sx 		= param.get_ref( ref.bdd, "image_dst_size_x" )
	ref.img_dst_sy 		= param.get_ref( ref.bdd, "image_dst_size_y" )
	ref.img_dst_format	= param.get_ref( ref.bdd, "image_dst_format" )
end

function meu:update_ui()
	local ref = self.ref
	local ui = self.ui
	local bind = param.get(ref.img_src)
	local str = bind.." "..param.get(ref.img_src_sx).."x"..param.get(ref.img_src_sx)
				.." "..param.get(ref.img_src_format)
	ui.bu_info_src:set_text( str )
	ui.bu_mon_src:set_texture_bind_2d( bind )
	bind = param.get(ref.img_dst)
	str = "->"..bind.." "..param.get(ref.img_dst_sx).."x"..param.get(ref.img_dst_sx)
	  			.." "..param.get(ref.img_dst_format)
	self.ui.bu_info_dst:set_text( str )
	ui.bu_mon_dst:set_texture_bind_2d( bind )
end

--function meu:update()
-- 	local ref = self.ref
--end

meu.draw_icon = MEU.draw_icon_opencv
