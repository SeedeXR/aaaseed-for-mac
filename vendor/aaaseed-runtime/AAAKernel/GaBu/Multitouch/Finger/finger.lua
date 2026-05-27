--[[
if GABU.finger then
	local ref = GABU.finger.ref
	ref.multiple_index = ref.multiple_index or param.get_ref( aaa.get_multiple_cur(), "index" )
	local index = param.get( ref.multiple_index )
	--aaa.print( "finger index "..index )
	local blob = blobs.blob[index+1]

	if blob then
		--aaa.print( (index+1).."-> blob "..blob.id..", x = "..blob.x..", y = "..blob.y )
		param.set( ref.tra_x, blob.x )
		param.set( ref.tra_y, blob.y )
		param.set( ref.text_id, blob.id )
	end
end
--]]
