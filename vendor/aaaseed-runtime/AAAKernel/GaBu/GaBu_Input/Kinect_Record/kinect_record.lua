if not kinect_record then
	kinect_record = {}
	kinect_record.ref = {}

	local ref = kinect_record.ref
	ref.tex_video			= aaa.obj.get( "kinect_record_texvideo" )
	ref.img_index 			= param.get_ref( ref.tex_video, "image_index" )
	ref.kinect				= aaa.obj.get_branch_by_class( ref.tex_video, "kinect_ui"  )
	ref.motor				= param.get_ref( ref.kinect, "motor_active" )
	ref.motor_pos			= param.get_ref( ref.kinect, "motor_position" )

	if ref.motor then
		local name = aaa.net.machine
		local host = aaa.net.host_id
		aaa.print( "kinect record adjust to machine "..name.." "..host )
		param.set( ref.motor, 0. )
		if aaa.pc.is_maa() then
		else
		end
	end

	kinect_record.img_index_last = -42
end

function kinect_record.deal_with_save()
	local index = param.get( kinect_record.ref.img_index )
	if kinect_record.img_index_last ~= index then
		kinect_record.img_index_last = index
	end
	aaa.img.save( 0, "O:/Out/kinect"..index )
end

--kinect_record.deal_with_save()
