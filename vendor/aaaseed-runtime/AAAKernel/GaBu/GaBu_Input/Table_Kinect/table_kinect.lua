if aaa.lua.global.declare( "kinect_table") then
	kinect_table = {}
	kinect_table.ref = {}

	local ref = kinect_table.ref
	ref.tex_video		= aaa.obj.get( "table_texvideo_kinect_1" )
		ref.img_index 	= param.get_ref( ref.tex_video,	"image_index" )
	ref.kinect			= aaa.obj.get_branch_by_class( ref.tex_video, "kinect_ui"  )
		ref.motor			= param.get_ref( ref.kinect,	"motor_active" )
		ref.motor_pos		= param.get_ref( ref.kinect,	"motor_position" )

	if ref.motor then
		local name = aaa.net.machine
		local host = aaa.net.host_id
		aaa.print( "table kinect adjust to machine "..name.." "..host )
		param.set( ref.motor, 0. )
		if aaa.pc.is_maa() then
		else
		end
	end

	kinect_table.img_index_last = -42
end

function kinect_table.deal_with_save()
	local index = param.get( kinect_table.ref.img_index )
	if kinect_table.img_index_last ~= index then
		kinect_table.img_index_last = index
	end
	aaa.img.save( 0, "O:/Out/kinect"..index )
end

--kinect_table.deal_with_save()

