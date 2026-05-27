--UTIL
aaa.show_file_begin( "aaa_net" )

--todo deal with several adapter on a pc
if not aaa.net.ref then
	aaa.net.ref = {}
	local ref = aaa.net.ref
	ref.obj = aaa.obj.get_from_top_by_class( "net" )
	aaa.net.ip = {}
	for i=1,4 do
		aaa.net.ip[i] = param.get( ref.obj, "host_ip_"..(i-1) )
	end
	aaa.net.host_id  = param.get( ref.obj, "host_id"  )
	aaa.net.langroup = param.get( ref.obj, "langroup" )
	aaa.net.machine  = param.get( ref.obj, "machine"  )
	aaa.net.link_index_max = 16	-- 0 to 16
	aaa.print_inverse( "starting machine "..aaa.net.machine.." on lan "..aaa.net.langroup.." with host_id "..aaa.net.host_id )
	local links = {}
	local p = param.get_ref( ref.obj, "link_base_0" )
	links[0] = param.get_obj_attached(p)
	for i = 1,aaa.net.link_index_max do
		p = param.get_ref( ref.obj, "link_"..i )
		links[i] = param.get_obj_attached(p)
	end
	ref.links = links
end

function aaa.net.set_active( b_active )	param.set( aaa.net.ref.obj, "active", b_active )	end
function aaa.net.is_active()			return param.get_bool( aaa.net.ref.obj, "active" )	end
function aaa.net.is_remote()			return param.get_bool( aaa.net.ref.obj, "remote" )	end

function aaa.net.get_host_id()
	return param.get( aaa.net.ref.obj, "host_id" )
end
function aaa.net.update()
	aaa.obj.update( aaa.net.ref.obj )
end
function aaa.net.is_ip_str( ip_str )
	local ref = aaa.net.ref
	for i=1,4 do
		if ip_str == param.get( ref.obj, "host_ip_"..i-1 ) then
			return true
		end
	end
	return false
end
function aaa.net.get_link_ref( link_index )
	if inside( link_index, 0, aaa.net.link_index_max ) then
		return aaa.net.ref.links[link_index]
	else
		aaa.print_error( "net_set_ip(), link index "..tostring(link_index).." is invalid." )
	end
end
function aaa.net.restart_link( link_index )
	local ref = aaa.net.get_link_ref( link_index )
	if ref then
		--stop
		param.set( ref, "enable", true )
		param.set( ref, "stop_trig", true )
		aaa.obj.update(	ref )
		--start
		param.set( ref, "start_trig", true )
		aaa.obj.update(	ref )
	end
end

-- fields are
-- link_index	start at 0
-- ip			in the format "192:168:0:1" for example or "any" or "local 
-- port
-- port_dst
function aaa.net.restart_with_ip_port( tab )
	local link_index = tab.link_index 
	local ref = aaa.net.get_link_ref( link_index )
	if ref then
		local str = "set net link "..link_index.." to"

		local ip = tab.ip
		if ip then
			param.set( ref, "dst_ip",			ip			)
			str = str.." ip-"..ip
		end

		local port = tab.port
		if port	then
			param.set( ref, "port_nb", 			port		)
			str = str.." port-"..port
		end

		local port_dst = tab.port_dst
		if port_dst	then
			param.set( ref, "port_nb_udp_dst", 	port_dst	)
			str = str.." port_udp_dst-"..port_dst
		end

		aaa.print_inverse( str )
		aaa.net.restart_link( link_index )
	end
end
function aaa.net.set_local( link_index )	aaa.net.restart_with_ip_port { link_index=link_index, ip="local" }	end
function aaa.net.set_any( link_index )		aaa.net.restart_with_ip_port { link_index=link_index, ip="any"   }	end

function aaa.net.start_with( tab )
	for i = 1, #tab do
		local l = tab[i]
		if l then
			aaa.net.restart_with_ip_port( l )
		end
	end
end
function aaa.net.stop_link( link_index )
	local ref = aaa.net.get_link_ref( link_index )
	if ref then
		local str = "stop net link "..link_index
		aaa.print_inverse( str )
		--stop
		param.set( ref, "enable", true )
		param.set(	ref, "stop_trig", true )
		aaa.obj.update(	ref )
		param.set( ref, "enable", false )
	end
end
function aaa.net.stop_link_all()
	for link_index = 0, aaa.net.link_index_max do
		aaa.net.stop_link( link_index )
	end
end
function aaa.net.lua_send_fn_global( link_index, channel, fn_name )
	aaa.net.lua_send( link_index, channel, "aaa.lua.global.call_fn( \""..fn_name.."()\" )" )
end

--	OSC
--
function aaa.net.osc_dump_tag_arg( index, tags, args )
	local function concat_tag( str, tab )
		for i=1,#tab do str = str.." "..tab[i] end
		return str
	end
	local function concat_arg( str, tab )
		for i=1,#tab do str = str.." "..tab[i].."("..type(tab[i])..")" end
		return str
	end
	--table.print( tags, "tags" )
	--table.print( args, "args" )
	local str = concat_tag(  "\t"..index.." OSC : ", tags  )
	if args then
		str = concat_arg(  str.." --->", args  )
	end
	aaa.print( str )
end

function aaa.net.osc_test( start )
	local loop = 1
	repeat
		local tab = aaa.net.osc_take_by_start( start )
		if tab and #tab>0 then
			if loop == 1 then
				local count = ( aaa.net.__osc_test_count or 0) + 1
				aaa.print( "osc_test "..count )
				aaa.net.__osc_test_count = count
			end
			for i=1,#tab do
				aaa.net.osc_dump_tag_arg( loop, tab[i].tags, tab[i].args )
			end
		else
			break
		end
		loop = loop + 1
	until false
end

function aaa.net.osc_apply_method( start, obj, method )
	repeat
		local tab = aaa.net.osc_take_by_start( start )
		if tab and #tab>0 then
			for i=1,#tab do
				obj[method]( obj, tab[i].tags, tab[i].args )
			end
		else
			break
		end
	until false
end

aaa.net.osc_test( "/AAABase/" )
--aaa.net.osc_test( "/mrmr/" )

--aaa.net.osc_send( 0, "/AAABase/AAALed", 1 ) --math.sin( aaa.time.t ) )
--aaa.net.osc_send( 0, "/1" ) --math.sin( aaa.time.t ) )

aaa.show_file_end( "aaa_net" )
